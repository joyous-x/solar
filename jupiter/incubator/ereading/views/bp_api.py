# -*- coding: utf-8 -*-

import time
import json
from sanic import Blueprint
from sanic.response import redirect, html, text
from ereading.setting import *
from ereading.views.template import *
from ereading.common.logger import slogger
from ereading.views import bp_api_user, bp_api_book

bp_api = Blueprint('blueprint_api', url_prefix='v1')

@bp_api.route("/user/login", methods=['POST'])
async def userLoginRouter(request):
    resp = bp_api_user.userLogin(request)
    respData = json.loads(str(resp.body, encoding='utf-8'))
    if 'data' in respData:
        request['session'][SessKey.uid] = respData['data'][SessKey.uid]
        request['session'][SessKey.sid] = respData['data'][SessKey.sid]
    return resp

@bp_api.route("/user/logout", methods=['POST'])
async def userLogoutRouter(request):
    resp = bp_api_user.userLogout(request)
    for i in CookieKey.keys:
        del resp.cookies[i]
    if 'session' in request:
        for i in SessKey.keys:
            if request['session'].get(i):
                del request["session"][i]
    return resp

@bp_api.route("/user/regist", methods=['POST'])
async def userRegistRouter(request):
    return bp_api_user.userRegist(request)

@bp_api.route("/bookmark/add", methods=['POST'])
async def add2BookmarkRouter(request):
    return bp_api_user.add2Bookmark(request)

@bp_api.route("/bookmark/del", methods=['POST'])
async def delFromBookmarkRouter(request):
    return bp_api_user.delFromBookmark(request)

@bp_api.route("/bookrack/add", methods=['POST'])
async def add2BookrackRouter(request):
    return bp_api_user.add2Bookrack(request)

@bp_api.route("/bookrack/del", methods=['POST'])
async def delFromBookrackRouter(request):
    return bp_api_user.delFromBookrack(request)

@bp_api.route("/book/search", methods=['POST'])
async def search(request):
    """
    url for test: 
        curl -XPOST http://127.0.0.1:8001/v1/book/search -d '{"keys":"元尊", "engs":""}'
    """
    return await bp_api_book.bookSearch(request)

@bp_api.route("/book/cplist", methods=['POST'])
async def queryBookChapters(request):
    """
    url for test: 
        curl -XPOST http://127.0.0.1:8001/v1/book/cplist -d '{"url":"https://www.biqudao.com/bqge10365/", "bname":"元尊"}'
    """
    return await bp_api_book.bookChapterList(request)

@bp_api.route("/book/chapter", methods=['POST'])
async def queryBookChapters(request):
    """
    url for test: 
        curl -XPOST http://127.0.0.1:8001/v1/book/chapter -d '{"url":"https://www.23txt.com/files/article/html/44/44359/22331006.html", "bname":"元尊"}'
    """
    return await bp_api_book.bookChapterContent(request)