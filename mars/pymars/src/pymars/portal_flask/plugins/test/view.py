# coding=utf-8
from flask import Blueprint, jsonify


blueprint = Blueprint("test_view", __name__)


@blueprint.route("/test/view/version", methods=["GET", "POST"])
def version():
    return jsonify({"version": "test_20210417"})


def init_plugin(flask_app):
    print("===> init_plugin {}".format(__name__))