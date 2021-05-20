# -*- coding: utf-8 -*-

import datetime
from sanic import Blueprint
from sanic.response import redirect, html, text, json
from urllib.parse import parse_qs, unquote
from ereading.common.logger import slogger
from ereading.setting import *
from ereading.views import utils

def userLogin(request):
    uid, sid, reqData = utils.parseRequestJson(request)
    passwd = reqData.get('passwd', None)
    if not (uid and passwd):
        return json({'code': 1, 'msg': "invalid arguments"})
    # TODO:
    respdata = json({'code':0, 'msg':'ok', 'data':{'sid':'test-sid', 'uid':'test-uid', 'name':'test-name'}})
    return respdata

def userLogout(request):
    uid, sid, reqData = utils.parseRequestJson(request)
    if not (uid and sid):
        return json({'code': 1, 'msg': "invalid arguments"})
    return json({'code':0, 'msg':'ok'})

def userRegist(request):
    reqdata = parse_qs(str(request.body, encoding='utf-8'))
    uid = reqdata.get('user', None)
    passwd = reqdata.get('passwd', None)
    nicky  = reqdata.get('nicky', None)
    if not (uid and passwd):
        return json({'code': 1, 'msg': "invalid arguments"})
    # TODO
    return json({'code':0, 'msg':'ok'})

def add2Bookmark(request):
    # TODO : main.js
    return

def delFromBookmark(request):
    return

def add2Bookrack(request):
    return

def delFromBookrack(request):
    return