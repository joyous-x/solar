# -*- coding: utf-8 -*-

import time
from sanic import Blueprint
from sanic.response import redirect, html, text, json
from jinja2 import Environment, PackageLoader, select_autoescape
from operator import itemgetter
from urllib.parse import urlparse
from ereading.setting import *
from ereading.common.errors import *
from ereading.common.logger import slogger as logger
from ereading.views import utils
from ereading.views.template import *
from ereading.models.searchrules import ConfigBookSearch
from ereading.models.books.request import RequestRawChapterList, RequestChapterContent, beforeExtractChapterList, extractChapterList, extractBookIntros

def ParseBUrlInfo(urlpath):
    host, scheme = utils.parseUrlhost(urlpath)
    if host not in ConfigBookSearch.rules:
        return None
    if host in ConfigBookSearch.rulesReplace:
        urlpath = urlpath.replace(ConfigBookSearch.rulesReplace[host]['old'], ConfigBookSearch.rulesReplace[host]['new'])
    cptype = ConfigBookSearch.rules[host].cpType
    cpretype = ConfigBookSearch.rules[host].cpPreType
    return {"cptype":cptype, "cpretype":cpretype, "host":host, "scheme":scheme, "url_new":urlpath}

async def ChapterContentHandler(bname, cpurl, cplisturl = ""):
    """
        提取并标准化chapter内容
    """
    respData = {}
    urlInfo = ParseBUrlInfo(cpurl)
    if None == urlInfo:
        return respData, ErrHostNotReserved
    scheme,host,cptype,cpurl = urlInfo["scheme"], urlInfo["host"], urlInfo["cptype"], urlInfo["url_new"]

    content_data = await RequestChapterContent(url=cpurl, host=host)
    if not content_data:
        return respData, ErrRequestHtml
    try:
        content = content_data.get('content', '获取失败')
        next_chapter = content_data.get('next_chapter', [])
        cptitle = content_data.get('title', '').replace(bname, '')
        cptitle = cptitle if cptitle else bname
        # 破坏广告链接
        content = str(content).strip('[]Jjs,').replace('http', 'hs').replace('.js', '').replace('();', '')
        respData = {
            'bname':bname, 'cphost':host, 'cpscheme':scheme, "cptype": cptype,
            'cpurl':cpurl, 'cplisturl':cplisturl, 
            'cptitle':cptitle, 'cpcontent':content, 'next_cpurl':next_chapter,
        }
    except Exception as e:
        logger.error('ChapterContentHandler cpurl=%s error:%s' % (cpurl, e))
        return respData, ErrParseHtml
    logger.debug('ChapterContentHandler succ cpurl=%s next_chapter=%s' % (cpurl, next_chapter))
    return respData, ErrOK

async def ChapterListHandler(bname, cplisturl):
    """
        提取并标准化chapter列表
    """
    respData = {}
    urlInfo = ParseBUrlInfo(cplisturl)
    if None == urlInfo:
        return respData, ErrHostNotReserved
    scheme,host,cptype,cplisturl = urlInfo["scheme"], urlInfo["host"], urlInfo["cptype"], urlInfo["url_new"]

    rawHtml = await RequestRawChapterList(cplisturl)
    if not rawHtml:
        return respData, ErrRequestHtml

    content = beforeExtractChapterList(host, cplisturl, rawHtml)
    if not content:
        return respData, ErrRequestHtml
    content = str(content).strip('[],, Jjs').replace(', ', '').replace('onerror', '').replace('js', '').replace('加入书架', '')
    cplist = extractChapterList(cplisturl, content)
    if len(cplist) < 1:
        return respData, ErrParseHtml

    # 获取到最后更新时间
    binfos = extractBookIntros(cplisturl, rawHtml)
    if 'ts_update' in binfos:
        update_ts = binfos['ts_update']
    else:
        update_ts = ''

    logger.debug('ChapterListHandler succ cplisturl=%s len(cplist)=%d last_chapter=%s' % (cplisturl, len(cplist), cplist[-1]))
    respData = {
        'bname':bname, 'cphost':host, 'cpscheme':scheme, "cptype": cptype,
        'cplisturl':cplisturl, 'cplist':cplist, 'ts_update': update_ts,
    }
    return respData, ErrOK

def MakeupCplistHtmlContent(bname, cplistInfos, ts_update=None):
    if None == cplistInfos or not isinstance(cplistInfos, list):
        return None
    content = ""
    # 开始描述
    contentInfo = '''
        <div id="maininfo">
            <div id="info">'
                <h1>{bname}</h1>
                <p>作&nbsp;&nbsp;&nbsp;&nbsp;者：TODO</p>
                <p>状&nbsp;&nbsp;&nbsp;&nbsp;态：连载中,<a href="chapter?url=http://www.23txt.com/#footer&amp;name=直达底部&amp;chapter_url=https://www.23txt.com/files/article/html/44/44359/&amp;novels_name=元尊" target="_blank">直达底部</a></p>
                <p>最后更新：{ts_update}</p>
                <p>最新章节：<a href="{cpLatestUrl}" target="_blank">{cpLatestTitle}</a></p>
            </div>
            <div id="intro"><p>TODO: 介绍</p></div>
		</div>'''
    cpLatest = cplistInfos[-1]
    contentInfo = contentInfo.format(bname=bname,
                       cpLatestUrl=cpLatest["cpurl"], 
                       ts_update=ts_update if ts_update else "",
                       cpLatestTitle="%s %s" % (cpLatest["cpid"] if cpLatest["cpid"] else "", cpLatest["cpname"]))
    content = content + contentInfo
    # 开始列表
    content = content + '<div class="listmain"><dl>'
    # 填充最新章节
    contentLatest = ""
    if len(cplistInfos) > 16:
        for i, cp in enumerate(cplistInfos[-8:]):
            tmp = '<dd><a href="%s">%s %s</a></dd>' % (cp["cpurl"], cp["cpid"] if cp["cpid"] else "", cp["cpname"])
            contentLatest = tmp + contentLatest
        content = content + '<dt>最新章节</dt>' + contentLatest
    # 填充章节列表
    contentList = ""
    for i, cp in enumerate(cplistInfos):
        contentList = contentList + '<dd><a href="%s">%s %s</a></dd>' % (cp["cpurl"], cp["cpid"] if cp["cpid"] else "", cp["cpname"])
    content = content + '<dt>章节列表</dt>' + contentList
    # 结束列表
    content =  content + '</dl></div>'
    return content