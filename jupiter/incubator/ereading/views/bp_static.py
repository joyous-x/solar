# -*- coding: utf-8 -*-

from sanic import Blueprint
from sanic.response import redirect, html, text, json
from ereading.common.logger import slogger
from ereading.setting import *
from ereading.views.template import *

bp_static = Blueprint('blueprint_static', url_prefix='')
bp_static.static('/static', Config.workspace + '/pages/static') 
