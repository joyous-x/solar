# -*- coding: utf-8 -*-
from sanic.response import html
from jinja2 import Environment, PackageLoader, select_autoescape
from ereading.setting import *

# jinjia2 config
env = Environment(
    loader=PackageLoader('views.template', '../pages/templates/books'),
    autoescape=select_autoescape(['html', 'htm', 'xml', 'tpl']))

def activeTemplate(tpl, **kwargs):
    template = env.get_template(tpl)
    newargs = kwargs
    newargs["appname"] = Config.appname
    return html(template.render(newargs))
