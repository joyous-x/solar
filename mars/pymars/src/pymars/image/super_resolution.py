# coding=utf-8
import os
import math
import numpy as np
import json
import cv2
import torch
import torch.nn as nn
from PIL import Image
from contextlib import contextmanager


IMG_EXTENSIONS = ['.jpg', '.JPG', '.jpeg', '.JPEG', '.png', '.PNG', '.ppm', '.PPM', '.bmp', '.BMP', '.tif']




class SplitImage(object):
    """ split a image to patches or merge patches to one image
    image must be : c * h * w or h * w
    """
    def __init__(self, split=True, scale_factor=2):
        self.split = split
        self.scale_factor = scale_factor

    def __call__(self, image, *args, **kwargs):
        if self.split:
            return self._split(image, *args, **kwargs)
        return self._merge(image, *args, **kwargs)

    @staticmethod
    def _split(image: np.array, patch_size=128, overlap_size=3):
        image = np.pad(image, ((0,0),(overlap_size,overlap_size),(overlap_size,overlap_size)), mode="edge")
        C, H, W = image.shape
        patches = []
        for h in range(overlap_size, H-overlap_size, patch_size):
            for w in range(overlap_size, W-overlap_size, patch_size):
                cur_h, cur_w = min(h+patch_size+overlap_size, H), min(w+patch_size+overlap_size, W)
                patches.append(image[:, h-overlap_size:cur_h, w-overlap_size:cur_w].astype(np.float) / 255.0 )
        return patches

    @staticmethod
    def _merge(images: list(), width=512, height=512, overlap_size=3 * 2):
        if len(images) < 1 or images[0].ndim != 3:
            raise TypeError("input must be: list of [c * h * w], the length of list is batch_size")
        for cur_patch in images:
            if cur_patch.shape[0] != images[0].shape[0]:
                raise TypeError("input must be the same channels")
        images = [i.mul(255).add_(0.5).clamp_(0, 255).to('cpu', torch.uint8).numpy() for i in images]
        cur_h_start, cur_w_start, result = 0, 0, np.zeros((images[0].shape[0], height, width), dtype=np.uint8)
        for cur_patch in images:
            c, h, w = cur_patch.shape
            cur_h_end, cur_w_end = min(cur_h_start+h-2*overlap_size, height), min(cur_w_start+w-2*overlap_size, width)
            cur_h, cur_w = cur_h_end-cur_h_start, cur_w_end-cur_w_start
            cur_patch = cur_patch[:, overlap_size:cur_h+overlap_size, overlap_size:cur_w+overlap_size]
            result[:, cur_h_start:cur_h_end, cur_w_start:cur_w_end] = cur_patch
            cur_w_start = cur_w_end if cur_w_end < width else 0
            cur_h_start = cur_h_end if cur_w_start == 0 and cur_h_end < height else cur_h_start
        return result


class BaseModule(nn.Module):
    def __init__(self):
        self.act_fn = None
        super(BaseModule, self).__init__()

    def selu_init_params(self):
        for m in self.modules():
            if isinstance(m, nn.Conv2d) and m.weight.requires_grad:
                m.weight.data.normal_(0.0, 1.0 / math.sqrt(m.weight.numel()))
                if m.bias is not None:
                    m.bias.data.fill_(0)
            elif isinstance(m, nn.BatchNorm2d) and m.weight.requires_grad:
                m.weight.data.fill_(1)
                m.bias.data.zero_()

            elif isinstance(m, nn.Linear) and m.weight.requires_grad:
                m.weight.data.normal_(0, 1.0 / math.sqrt(m.weight.numel()))
                m.bias.data.zero_()

    def initialize_weights_xavier_uniform(self):
        for m in self.modules():
            if isinstance(m, nn.Conv2d) and m.weight.requires_grad:
                # nn.init.kaiming_normal_(m.weight, mode='fan_out', nonlinearity='leaky_relu')
                nn.init.xavier_uniform_(m.weight)
                if m.bias is not None:
                    m.bias.data.zero_()
            elif isinstance(m, nn.BatchNorm2d) and m.weight.requires_grad:
                m.weight.data.fill_(1)
                m.bias.data.zero_()

    def load_state_dict(self, state_dict, strict=True, self_state=False):
        own_state = self_state if self_state else self.state_dict()
        for name, param in state_dict.items():
            if name in own_state:
                try:
                    own_state[name].copy_(param.data)
                except Exception as e:
                    print("Parameter {} fails to load.".format(name))
                    print("-----------------------------------------")
                    print(e)
            else:
                print("Parameter {} is not in the model. ".format(name))

    @contextmanager
    def set_activation_inplace(self):
        if hasattr(self, 'act_fn') and hasattr(self.act_fn, 'inplace'):
            # save memory
            self.act_fn.inplace = True
            yield
            self.act_fn.inplace = False
        else:
            yield

    def total_parameters(self):
        total = sum([i.numel() for i in self.parameters()])
        trainable = sum([i.numel() for i in self.parameters() if i.requires_grad])
        print("Total parameters : {}. Trainable parameters : {}".format(total, trainable))
        return total

    def forward(self, *x):
        raise NotImplementedError


class UpConv_7(BaseModule):
    """
    Note：
        1. upconv_7 has checkboard artifact problem. It happens very seldom, though. (https://github.com/nagadomi/waifu2x/issues/92)
        2. I think the fundamental problem is that there is no detail in the large image...
            waifu2x's default model(upconv_7) uses 15x15 fixed size filter to upscale. Probably it is too small to handle large detail of large images.
            (https://github.com/nagadomi/waifu2x/issues/185)
        3. An alternative to reconstruct large image by parts(https://github.com/nagadomi/waifu2x/issues/238)
    """
    def __init__(self):
        super(UpConv_7, self).__init__()
        self.act_fn = nn.LeakyReLU(0.1, inplace=False)
        self.offset = 7  # because of 0 padding
        from torch.nn import ZeroPad2d
        self.pad = ZeroPad2d(self.offset)
        m = [nn.Conv2d(3, 16, 3, 1, 0),
             self.act_fn,
             nn.Conv2d(16, 32, 3, 1, 0),
             self.act_fn,
             nn.Conv2d(32, 64, 3, 1, 0),
             self.act_fn,
             nn.Conv2d(64, 128, 3, 1, 0),
             self.act_fn,
             nn.Conv2d(128, 128, 3, 1, 0),
             self.act_fn,
             nn.Conv2d(128, 256, 3, 1, 0),
             self.act_fn,
             # in_channels, out_channels, kernel_size, stride=1, padding=0, output_padding=
             nn.ConvTranspose2d(256, 3, kernel_size=4, stride=2, padding=3, bias=False)
             ]
        self.Sequential = nn.Sequential(*m)

    def load_pre_train_weights(self, json_file):
        with open(json_file) as f:
            weights = json.load(f)
        box = []
        for i in weights:
            box.append(i['weight'])
            box.append(i['bias'])
        own_state = self.state_dict()
        for index, (name, param) in enumerate(own_state.items()):
            own_state[name].copy_(torch.FloatTensor(box[index]))

    def forward(self, x):
        x = self.pad(x)
        return self.Sequential.forward(x)


class Vgg_7(UpConv_7):
    def __init__(self):
        super(Vgg_7, self).__init__()
        self.act_fn = nn.LeakyReLU(0.1, inplace=False)
        self.offset = 7
        m = [nn.Conv2d(3, 32, 3, 1, 0),
             self.act_fn,
             nn.Conv2d(32, 32, 3, 1, 0),
             self.act_fn,
             nn.Conv2d(32, 64, 3, 1, 0),
             self.act_fn,
             nn.Conv2d(64, 64, 3, 1, 0),
             self.act_fn,
             nn.Conv2d(64, 128, 3, 1, 0),
             self.act_fn,
             nn.Conv2d(128, 128, 3, 1, 0),
             self.act_fn,
             nn.Conv2d(128, 3, 3, 1, 0)
             ]
        self.Sequential = nn.Sequential(*m)


"""
waifu2x：
    ncnn ： https://github.com/nihui/waifu2x-ncnn-vulkan
    pytroch：https://github.com/nagadomi/nunif

    waifu2x uses 24x24 randomly selected 64 patches from 1 image for each epoch. Not all image areas are used in 1 epoch.

     Learning based upscaling method strongly depends on a training dataset. A training dataset is made by
     image domain(anime style art, photo, 3DCG,..., etc) and downscaling methods(box, lanczos, vector graphics, ..., etc).

Dandere2x：
    https://github.com/akai-katto/dandere2x

    https://github.com/nagadomi/waifu2x/issues/251
"""


class SuperResolution(object):
    def __init__(self, model_path="", model_type="UpConv_7"):
        self.model_path = model_path
        self.model_type = model_type
        if model_type == "UpConv_7":
            if model_path is None or model_path == "":
                model_path = os.path.join(os.path.dirname(__file__), "models/waifu2x/anime/noise0_scale2.0x_model.json")
                self.model_path = model_path
            upconv7 = UpConv_7()
            upconv7.load_pre_train_weights(model_path)
            self._scaler_upconv7 = upconv7

    def scale2x(self, image, force_mode=True):
        if image.mode != "RGB":
            if force_mode:
                image = image.covert("RGB")
            else:
                raise NotImplementedError("invalid for image mode: " + image.mode)
        w, h = image_pil.size
        image_patches = SplitImage(split=True)(np.array(image).transpose((2, 0, 1)), patch_size=128)
        with torch.no_grad():
            patches = [torch.unsqueeze(torch.from_numpy(patch), 0).float() for patch in image_patches]
            outs = [self._scaler_upconv7.forward(patch) for patch in patches]
            outs = [out.squeeze(0) for out in outs]
        result = SplitImage(split=False)(outs, width=w * 2, height=h * 2)
        return Image.fromarray(result.transpose((1, 2, 0)))  # RGB

    def scale_bilinear(self):
        pass

    def scale_bicubic(self):
        pass

    def scale_lanczos(self):
        pass


if __name__ == '__main__':
    image_pil = Image.open("./output/datasets/super_resolution/c.jpg")
    scaler = SuperResolution()
    scaler.scale2x(image_pil).show()
    print("resize success ~")