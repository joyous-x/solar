# coding=utf-8
import io
import os
from PIL import Image
from flask_cors import CORS
from flask import Blueprint, jsonify, request, send_file
from pymars.image.super_resolution import SuperResolution


blueprint = Blueprint("ai_api", __name__)
CORS(blueprint)


class ApiUtils(object):
    def __init__(self):
        self.path = os.path.dirname(os.path.dirname(os.path.dirname(__file__)))


def init_plugin(flask_app):
    print("===> init_plugin {}".format(__name__))


__scale = SuperResolution()


@blueprint.route('/api/sr', methods=['POST','GET'])
def super_resolution():
    file_obj = request.files.get('file')
    image_new, msg = None, ""
    try:
        if file_obj is not None:
            image_data = file_obj.read()
            image_obj = Image.open(io.BytesIO(image_data))
            image_new = __scale.scale2x(image_obj)
            if image_new is None:
                msg = "scale error"
    except Exception as e:
        image_new, msg = None, e

    def tran_image2bytes(pil_img, format="PNG"):
        bytes_array = io.BytesIO()
        pil_img.save(bytes_array, format=format)
        return bytes_array.getvalue()

    if image_new is None:
        return jsonify({"scale": None, "msg": msg})
    return send_file(io.BytesIO(tran_image2bytes(image_new)),
                     mimetype='image/{}'.format(image_obj.format),
                     as_attachment=True,
                     attachment_filename='result.{}'.format(image_obj.format.lower()))



@blueprint.route('/api/blinking_eyes', methods=['POST','GET'])
def blinking_eyes():
    file_obj = request.files.get('file')
    if file_obj is not None:
        image_data = file_obj.read()
        image_obj = Image.open(io.BytesIO(image_data))
    else:
        image_obj = Image.open("/Users/jiao/Downloads/26882-6-8-ball-pool-photos.png")

    # TODO:
    #   blinking eyes
    #   save video to tmp

    return jsonify({"video" : "video_path"})


@blueprint.route('/api/cartooning', methods=['POST'])
def cartooning():
    file_obj = request.files.get('file')
    if file_obj is not None:
        image_data = file_obj.read()
        image_obj = Image.open(io.BytesIO(image_data))
    else:
        image_obj = Image.open("/Users/jiao/Downloads/26882-6-8-ball-pool-photos.png")

    def tran_image2bytes(pil_img, format="PNG"):
        bytes_array = io.BytesIO()
        pil_img.save(bytes_array, format=format)
        return bytes_array.getvalue()

    # TODO:
    #   blinking eyes
    #   save video to tmp

    return send_file(io.BytesIO(tran_image2bytes(image_obj)),
                     mimetype='image/{}'.format(image_obj.format),
                     as_attachment=True,
                     attachment_filename='result.{}'.format(image_obj.format.lower()))

