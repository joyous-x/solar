# coding=utf-8
import os
import torch
import torch.nn as nn
import onnxruntime
from PIL import Image
from torchvision import transforms

try:
    import netron
except Exception as e:
    print("warn: {}".format(e))


class ONNXModel(object):
    """ ONNXModel
    help to load onnx models:
    a. translate pth to onnx
    b. simplify the onnx: python -m onnxsim [model_name].onnx [model_name].onnx
        eg. python -m onnxsim --input-shape "input:1,3,156,374" --dynamic-input-shape  mobilenet_v2_c3f192.30.onnx mobilenet_v2_c3f192.30.s.onnx
    """

    @staticmethod
    def tran_pth2onnx(input_model: nn.Module, input_shape=(1, 3, 244, 244), export_path="", device=torch.device("cpu"), dynamic_axes=None):
        """
            model_input_shape: batch_size * channels * H * W
            dynamic_axes: batch_size、W、H,
                          default value is {"input": {0: "batch_size", 2: "W", 3: "H"}, "output": {0: "batch_size", 2: "2W", 3: "2H"}}
        """
        # set the model to inference mode
        input_model.eval()
        x = torch.randn(input_shape).to(device)
        torch.onnx.export(input_model, x, export_path,
                          opset_version=11,
                          do_constant_folding=True,
                          input_names=["input"],
                          output_names=["output"],
                          operator_export_type=torch.onnx.OperatorExportTypes.ONNX,
                          dynamic_axes=dynamic_axes)
        return os.path.isfile(export_path)

    def __init__(self, model_path):
        self.onnx_session = onnxruntime.InferenceSession(model_path)
        self.input_name = []
        for node in self.onnx_session.get_inputs():
            self.input_name.append(node.name)
        self.output_name = []
        for node in self.onnx_session.get_outputs():
            self.output_name.append(node.name)
        print("input_name:{} \t output_name:{}".format(self.input_name, self.output_name))

    def forward(self, image_numpy):
        # scores, boxes = self.onnx_session.run(None, input_feed={self.input_name: image_numpy})
        input_feed = {}
        for name in self.input_name:
            input_feed[name] = image_numpy
        return self.onnx_session.run(self.output_name, input_feed=input_feed)

    @staticmethod
    def tensor_to_numpy(tensor):
        return tensor.detach().cpu().numpy() if tensor.requires_grad else tensor.cpu().numpy()


if __name__ == "__main__":
    onnx_model_path = "./models/mobilenet_v2_c3f192.30.onnx"
    pth_model_path = "./models/model.30.pt"
    if not os.path.exists(onnx_model_path):
        pth_model = torch.load(pth_model_path, map_location="cuda" if torch.cuda.is_available() else "cpu")
        ONNXModel.tran_pth2onnx(pth_model, (1, 3, 156, 374), onnx_model_path, torch.device("cpu"))
        # onnx 可视化
        if netron is not None:
            netron.start(onnx_model_path)

    pil_image = Image.open("D:\\Workspace\\datasets\\font_recognize\\infer\\05_calibri_a1.png")
    if pil_image.mode in ["RGBA"]:
        pil_image = pil_image.convert('RGB')
    data_transforms = transforms.Compose([
        transforms.Resize(96),
        transforms.Pad((50, 0, 50, 0), fill=255),
        transforms.CenterCrop((96, 192)),
        transforms.ToTensor(),
        transforms.Normalize(mean=[0.5, 0.5, 0.5], std=[0.5, 0.5, 0.5])
    ])
    img_tensor = data_transforms(pil_image)
    test_model = ONNXModel(onnx_model_path)
    output = test_model.forward(ONNXModel.tensor_to_numpy(img_tensor.unsqueeze(0)))
    print(output.index(max(output)))
