# -*- coding: utf-8 -*-
import json as basejson
from sanic.response import json
from urllib.parse import urlparse, parse_qs, unquote
from ereading.common.logger import slogger
from ereading.setting import *
from ereading.common.errors import ErrCode, ErrServerBad

def makeDictResp(respData, errcode):
    assert isinstance(errcode, ErrCode)
    respData["msg"] = errcode.msg
    respData["code"] = errcode.code
    return respData

def newDictResp(errcode = ErrServerBad):
    respData = {'code':0, 'msg':'ok', 'data':{}}
    return makeDictResp(respData, errcode)

def parseRequestJson(request):
    # reqData = parse_qs(str(request.body, encoding='utf-8')) # application/x-www-form-urlencoded
    reqData = basejson.loads(str(request.body, encoding='utf-8'))
    uid = reqData.get('uid', None)
    sid = reqData.get('sid', None)
    if not uid:
        uid = request['session'].get(SessKey.uid) if 'session' in request else None
    if not sid:
        sid = request['session'].get(SessKey.sid) if 'session' in request else None
    return uid, sid, reqData

def parseUrlhost(urlstr):
    tmp = urlparse(urlstr)
    host = tmp.netloc
    scheme = tmp.scheme
    return host, scheme


