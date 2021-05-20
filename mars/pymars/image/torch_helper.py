# coding=utf-8
import os
import random
import math
import argparse
import torch
import torch.nn as nn
import torch.nn.functional as F
import torch.optim as optim
from torch.autograd import Variable
from torch.utils.data import Dataset
from torchvision import transforms, datasets, models
from tensorboardX import SummaryWriter
from PIL import Image, ImageDraw
import numpy as np
from sklearn import metrics
import matplotlib.pyplot as plt


''' Notes
1. torchvision.models : 内置模型

2. 为什么使用相同的网络结构，跑出来的效果完全不同，用的学习率，迭代次数，batch size 都是一样？
    固定随机数种子是非常重要的, 因為训练的初始化参数一般都是随机的。但是如果你使用的是PyTorch等框架，还要看一下框架的种子是否固定了。
    还有，如果你用了cuda，别忘了cuda的随机数种子。这里还需要用到torch.backends.cudnn.deterministic. 
    torch.backends.cudnn.deterministic是啥？顾名思义，将这个 flag 置为True的话，每次返回的卷积算法将是确定的，即默认算法。
    如果配合上设置 Torch 的随机种子为固定值的话，应该可以保证每次运行网络的时候相同输入的输出是固定的:
        def manual_seed(random_seed):
            ...

3. 视觉注意力机制在分类网络中的应用——SENet、CBAM、SKNet : https://blog.csdn.net/dedell/article/details/106768052
    综述：图像处理中的注意力机制 : https://zhuanlan.zhihu.com/p/257137546
    Mobilenet_v3 自带了注意力机制
'''


def manual_seed(random_seed):
    np.random.seed(random_seed)
    torch.manual_seed(random_seed)
    torch.cuda.manual_seed(random_seed)
    torch.cuda.manual_seed_all(random_seed)
    if random_seed == 0:
        torch.backends.cudnn.deterministic = True
        torch.backends.cudnn.benchmark = False


class DcOptions(object):
    def __init__(self):
        self._opt = None
        self._arg_parser = argparse.ArgumentParser()
        self.initialize()

    def initialize(self):
        self._arg_parser.add_argument('--random_seed', type=int, default=3, help='random seed')
        self._arg_parser.add_argument('--mode', type=str, default='train', help='mode: train、test、infer')
        self._arg_parser.add_argument('--data_dir', type=str, default='.', help='path to dataset')
        self._arg_parser.add_argument('--batch_size', type=int, default=16, help='input batch size')
        self._arg_parser.add_argument('--gpu_ids', type=str, default='-1',
                                      help='gpu ids: e.g. 0  0,1,2, 0,2. use -1 for CPU')
        self._arg_parser.add_argument('--name', type=str, default='experiment_1',
                                      help='name of the experiment. It decides where to store samples and models')
        self._arg_parser.add_argument('--model', type=str, default='mobilenet_v2', help='model to run[cnn_model]')
        self._arg_parser.add_argument('--model_path', type=str, default='', help='model path to be loaded')
        self._arg_parser.add_argument('--output', type=str, default='./output', help='root directory of outputs')
        self._arg_parser.add_argument('--log_dir', type=str, default='',
                                      help='directory for logs, default: [output]/log/[name]')
        self._arg_parser.add_argument('--checkpoint_dir', type=str, default='',
                                      help='logs are saved here, default: [output]/checkpoint/[name]')

    def parse(self):
        self._opt = self._arg_parser.parse_args()
        self._get_set_gpus()
        self._opt.device = torch.device('cuda') if len(self._opt.gpu_ids) > 0 else torch.device('cpu')
        if len(self._opt.log_dir) < 1:
            self._opt.log_dir = os.path.join(self._opt.output, "log", self._opt.name)
        if len(self._opt.checkpoint_dir) < 1:
            self._opt.checkpoint_dir = os.path.join(self._opt.output, "checkpoint", self._opt.name)

        args = vars(self._opt)
        self._print(args)

        return self._opt

    def _get_set_gpus(self):
        # get gpu ids
        str_ids = self._opt.gpu_ids.split(',')
        self._opt.gpu_ids = []
        for str_id in str_ids:
            id = int(str_id)
            if id >= 0:
                self._opt.gpu_ids.append(id)
        # set gpu ids
        if len(self._opt.gpu_ids) > 0:
            torch.cuda.set_device(self._opt.gpu_ids[0])

    def _print(self, args):
        print('------------ Options -------------')
        for k, v in sorted(args.items()):
            print('%s: %s' % (str(k), str(v)))
        print('-------------- End ----------------')


class DcDataLoader(object):
    def __init__(self):
        pass

    def __getitem__(self, item):
        pass

    def __len__(self):
        pass


class AucRoc(object):
    def __init__(self):
        self.TP, self.TN, self.FN, self.FP = 0,0,0,0

    @staticmethod
    def auc(predicted, labels):
        fpr, tpr, th = metrics.roc_curve(labels, predicted, pos_label=1)
        return metrics.auc(fpr, tpr)


class SELayer(nn.Module):
    def __init__(self, channel, reduction=16):
        super(SELayer, self).__init__()
        self.avg_pool = nn.AdaptiveAvgPool2d(1)
        self.fc = nn.Sequential(
            nn.Linear(channel, channel // reduction, bias=False),
            nn.ReLU(inplace=True),
            nn.Linear(channel // reduction, channel, bias=False),
            nn.Sigmoid()
        )

    def forward(self, x):
        b, c, _, _ = x.size()
        y = self.avg_pool(x).view(b, c)
        y = self.fc(y).view(b, c, 1, 1)
        return x * y.expand_as(x)


class VisualizingFiltersFeatures(object):
    """ visualizing-filters-and-feature-maps-in-convolutional-neural-networks-using-pytorch
    reference: https://debuggercafe.com/visualizing-filters-and-feature-maps-in-convolutional-neural-networks-using-pytorch/
    """
    @staticmethod
    def check_conv2d(layer, name="", model_weights: list = None, conv_layers: list = None):
        """ 遍历模型的每一层
        可以通过以下方式遍历
            - for child in layer:
            - for child in layer.children():
            - for i in range(len(layer)): layer[i]
            - for name, child_module in layer.named_children():
        """
        if type(layer) == nn.Conv2d:
            model_weights.append(layer.weight)
            if conv_layers is not None:
                conv_layers.append(layer)
        elif type(layer) == nn.Sequential or isinstance(layer, nn.Sequential):
            print("layer {} has {} children".format(name, len(layer)))
            for child_name, child_module in layer.named_children():
                child_name = "{}.{}".format(name, child_name)
                VisualizingFiltersFeatures.check_conv2d(child_module, child_name, model_weights, conv_layers)
        elif len(list(layer.children())) > 0:
            for child_name, child_module in layer.named_children():
                child_name = "{}.{}".format(name, child_name)
                VisualizingFiltersFeatures.check_conv2d(child_module, child_name, model_weights, conv_layers)
        else:
            print("layer {} has no children".format(name))

    @staticmethod
    def parse_layers(net: torch.nn.Module, visualize_filter_in_layers=(0,0)):
        conv_layers, model_weights = [], []
        # get all the conv layers and their respective weights to the list
        VisualizingFiltersFeatures.check_conv2d(net, "", model_weights, conv_layers)
        print("Total convolutional layers: {}".format(len(conv_layers)))
        # take a look at the conv layers and the respective weights
        for weight, conv in zip(model_weights, conv_layers):
            print(f"CONV: {conv} =====> SHAPE: {weight.shape}")
        if visualize_filter_in_layers and isinstance(visualize_filter_in_layers, tuple):
            index_start, index_end = visualize_filter_in_layers
            plt.figure(figsize=(20, 17))
            for j in range(len(model_weights)):
                if j < index_start or j >= index_end:
                    continue
                size = math.ceil(math.sqrt(len(model_weights[j])))
                for i, cur_filter in enumerate(model_weights[j]):
                    plt.subplot(size, size, i + 1)
                    plt.imshow(cur_filter[0, :, :].detach(), cmap='gray')
                    plt.axis('off')
                plt.show()
        return conv_layers, model_weights

    @staticmethod
    def visualizing(conv_layers, model_weights, net_input: torch.Tensor = None, net_input_name="", output_path=""):
        if net_input is None:
            return
        # 1. pass the image through all the layers
        results = [conv_layers[0](net_input)]
        for i in range(1, len(conv_layers)):
            # 2. pass the result from the last layer to the next layer
            results.append(conv_layers[i](results[-1]))
        # 3. make a copy of the `results`
        outputs = results

        # visualize 64 features from each layer
        # (although there are more feature maps in the upper layers)
        for num_layer in range(len(outputs)):
            plt.figure(figsize=(30, 30))
            layer_viz = outputs[num_layer][0, :, :, :]
            layer_viz = layer_viz.data
            print(layer_viz.size())
            for i, filter in enumerate(layer_viz):
                if i == 81:  # we will visualize only 8x8 blocks from each layer
                    break
                plt.subplot(9, 9, i + 1)
                plt.imshow(filter, cmap='gray')
                plt.axis("off")
            if output_path:
                plt.savefig(f"{output_path}/{net_input_name}_layer_{num_layer}.png")
            else:
                plt.show()
            plt.close()

    @staticmethod
    def parse(net: torch.nn.Module, net_input: torch.Tensor = None):
        conv_layers, model_weights = VisualizingFiltersFeatures.parse_layers(net, visualize_filter_in_layers=(0,1))
        VisualizingFiltersFeatures.visualizing(conv_layers, model_weights, net_input, net_input_name=random.randint(1,100), output_path="")


class TransformRandomScale(object):
    def __init__(self, scale_min=1.0, scale_max=1.1, interpolation=transforms.InterpolationMode.BILINEAR):
        self.scale_min = scale_min
        self.scale_max = scale_max
        self.interpolation = interpolation

    def __call__(self, pil_img):
        width, height = pil_img.size[0], pil_img.size[1]
        size = int(random.uniform(self.scale_min, self.scale_max) * min(width, height))
        return transforms.Resize(size, self.interpolation)(pil_img)


class TransformGaussianNoise(object):
    def __init__(self, mean=0.0, variance=1.0, amplitude=1.0):
        self.mean = mean
        self.variance = variance
        self.amplitude = amplitude

    def __call__(self, img):
        img = np.array(img)
        h, w, c = img.shape
        N = self.amplitude * np.random.normal(loc=self.mean, scale=self.variance, size=(h, w, 1))
        N = np.repeat(N, c, axis=2)
        img = N + img
        img[img > 255] = 255 # 避免有值超过255而反转
        img = Image.fromarray(img.astype('uint8')).convert('RGB')
        return img


class TransformRandomPad(object):
    """ TransformRandomPad
    Pad the given pillow image to the expected size (h, w) with the given "fill" value.
    Default, "fill" is 0.
    """
    def __init__(self, expect_h, expect_w, fill=0, padding_mode="constant"):
        self.expect_w = expect_w
        self.expect_h = expect_h
        self.fill = fill
        self.padding_mode = padding_mode

    def __call__(self, img_pil):
        if self.padding_mode != "constant":
            raise NotImplementedError("invalid padding_mode")
        w, h, image_mode = img_pil.size[0], img_pil.size[1], img_pil.mode
        diff_w, diff_h = self.expect_w - w, self.expect_h - h
        if diff_w < 0 and diff_h < 0:
            raise AttributeError("too small to expect")
        diff_h = 0 if diff_h < 0 else diff_h
        diff_w = 0 if diff_w < 0 else diff_w
        img_padded = transforms.Pad((diff_w, diff_h, diff_w, diff_h), fill=255, padding_mode='constant')(img_pil)
        img_padded = transforms.RandomCrop((self.expect_h, self.expect_w), pad_if_needed=True, fill=self.fill)(img_padded)
        return img_padded


class TransformRemovePadding(object):
    def __init__(self, pad_value = 255, tolerance=0):
        self.pad_value = pad_value
        self.tolerance = tolerance

    def __call__(self, pil_img):
        # h * w * c
        img_mode = pil_img.mode
        img_data = np.array(pil_img)
        x_s, x_e, y_s, y_e = TransformRemovePadding.remove_white_padding(img_data[:, :, 0])
        img_data = img_data[x_s:x_e + 1, y_s:y_e + 1, :]
        return Image.fromarray(img_data.astype('uint8')).convert(img_mode)

    @staticmethod
    def remove_white_padding(np_array_2d, tolerance=0):
        """
        remove white paddings in numpy array
        """
        x_s, x_e, y_s, y_e = None, None, None, None
        h, w = np_array_2d.shape
        for i in range(h):
            if x_s is not None and x_e is not None:
                break
            sum_s, sum_e, sum_max = sum(np_array_2d[i, :]), sum(np_array_2d[h - i - 1, :]), (255 - tolerance) * w
            if x_s is None and sum_s < sum_max:
                x_s = i
            if x_e is None and sum_e < sum_max:
                x_e = h - i - 1
        for i in range(w):
            if y_s is not None and y_e is not None:
                break
            sum_s, sum_e, sum_max = sum(np_array_2d[:, i]), sum(np_array_2d[:, w - i - 1]), (255 - tolerance) * h
            if y_s is None and sum_s < sum_max:
                y_s = i
            if y_e is None and sum_e < sum_max:
                y_e = w - i - 1
        x_s = x_s if x_s is not None else 0
        x_e = x_e if x_e is not None else h - 1
        y_s = y_s if y_s is not None else 0
        y_e = y_e if y_e is not None else w - 1
        return x_s, x_e, y_s, y_e


if __name__ == '__main__':
    pass
