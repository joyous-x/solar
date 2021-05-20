# -*- coding: utf-8 -*-

from sanic import Blueprint
from sanic.response import redirect, html, text, json
from ereading.common.errors import *
from ereading.views import utils
from ereading.models.books.searcher import SearchBook
from ereading.models.books.chapter import ChapterListHandler, ChapterContentHandler

async def bookSearch(request):
    respData = utils.newDictResp()
    uid, sid, reqData = utils.parseRequestJson(request)
    keys = reqData.get('keys', None)
    engs = reqData.get('engs', None)

    content = await SearchBook(engs, keys)
    if None == content:
        return json(utils.makeDictResp(respData, ErrRequestHtml))
    respData["data"] = content

    return json(utils.makeDictResp(respData, ErrOK))

async def bookChapterList(request):
    respData = utils.newDictResp()
    uid, sid, reqData = utils.parseRequestJson(request)
    cplisturl = reqData.get('url', None)
    bname = reqData.get('bname', None)

    content, ret = await ChapterListHandler(bname, cplisturl)
    if ret.code != ErrOK.code:
        return json(utils.makeDictResp(respData, ret))
    respData["data"] = content

    return json(utils.makeDictResp(respData, ErrOK))

async def bookChapterContent(request):
    respData = utils.newDictResp()
    uid, sid, reqData = utils.parseRequestJson(request)
    cpurl = reqData.get('url', None)
    bname = reqData.get('bname', None)

    content, ret = await ChapterContentHandler(bname, cpurl)
    if ret.code != ErrOK.code:
        return json(utils.makeDictResp(respData, ret))
    respData["data"] = content

    return json(utils.makeDictResp(respData, ErrOK))