# -*- coding: utf-8 -*-
import time
from sanic import Blueprint
from sanic.response import redirect, html, text, json
from ereading.common.logger import slogger as logger
from ereading.common.errors import *
from ereading.setting import *
from ereading.views.template import *
from ereading.views import utils
from ereading.models.searchrules import ConfigBookSearch
from ereading.models.books.searcher import SearchBook
from ereading.models.books.chapter import ChapterContentHandler, ChapterListHandler, MakeupCplistHtmlContent, ParseBUrlInfo

async def searchViewHandler(request):
    keys = str(request.args.get('keys', '')).strip()
    engs = str(request.args.get('engs', '')).strip()
    pages = int(str(request.args.get('pages', '1')).strip())
    if not keys:
        return redirect('/')

    start = time.time()
    try:
        logger.info("search recorder: keys=%s" % keys) # TODO: redis统计搜索频度
    except Exception as e:
        logger.error('%s error: url=%s' % (request.path, url))

    result_sorted = await SearchBook(engs, keys, pages=pages)
    if result_sorted:
        user = request['session'].get('user', None) if 'session' in request else None
        return activeTemplate('result.html', user=user, keys=keys, timeSpend='%.2f' % (time.time() - start), count=len(result_sorted), result=result_sorted)
    else:
        return html("no results")

async def chapterContentViewHandler(request):
    bname = request.args.get('bname', None)
    cpurl = request.args.get('url', None)
    cplisturl = request.args.get('cplist', None)
    user = request['session'].get('uid', None) if 'session' in request else None

    urlInfo = ParseBUrlInfo(cpurl)
    if None == urlInfo:
        return redirect(cpurl)
    scheme,host,cptype,cpurl = urlInfo["scheme"], urlInfo["host"], urlInfo["cpretype"], urlInfo["url_new"]

    respData, ret = await ChapterContentHandler(bname, cpurl)
    logger.info('chapterContent handle path=%s cpurl=%s retcode=%s' % (request.path, cpurl, ret.code))

    if ret.code == ErrHostNotReserved.code:
        return redirect(cpurl)
    elif ret.code == ErrParseHtml.code:
        er_cplist_url = "/cplist?url={cplisturl}&bname={bname}".format(cplisturl=cplisturl, bname=bname)
        return redirect(er_cplist_url)
    elif ret.code != ErrOK.code:
        return activeTemplate('perror.html', url=cpurl, user=user)

    for k, v in respData['next_cpurl'].items():
        if cptype == 'join_host':
            tmp = "%s://%s%s"%(scheme,host,v)
        elif cptype == 'join_cplist':
            tmp = "%s%s"%(cplisturl,v)
        elif cptype == 'raw':
            tmp = v
        else:
            return redirect(cpurl)
        if v[-1] == '/':
            respData['next_cpurl'][k] = '/cplist?url={0}&bname={1}'.format(tmp, bname)
        else:
            respData['next_cpurl'][k] = '/chapter?url={0}&bname={1}&cplist={2}'.format(tmp, bname, cplisturl)

    bookracked = "1" # TODO
    cptitle = respData['cptitle']
    cpcontent = respData['cpcontent']
    next_cpurl = respData['next_cpurl']
    return activeTemplate('chapter.html', bname=bname, cpurl=cpurl, cplisturl=cplisturl, cphost=host, cpscheme=scheme,
                cptitle=cptitle, cpcontent=cpcontent, next_cpurl=next_cpurl,  bookmarked=1, bookracked=bookracked)

async def chapterListViewHandler(request):
    url = request.args.get('url', None)
    bname = request.args.get('bname', None)

    urlInfo = ParseBUrlInfo(url)
    if None == urlInfo:
        return redirect(url)
    scheme,host,cptype,url = urlInfo["scheme"], urlInfo["host"], urlInfo["cptype"], urlInfo["url_new"]

    #提取出章节目录后自行组织
    cplists, err = await ChapterListHandler(bname, url)
    if err.code != ErrOK.code:
        if scheme == "https": # 有些网站不规矩，搜索出的是https，但请求经常失败
            url = "/cplist?url=%s&bname=%s" % (url.replace("https:", "http:"), bname)
        logger.error('chapterListViewHandler: parse for chapter lists error: path=%s, redirect to %s' % (request.path, url))
        return redirect(url)
    content = MakeupCplistHtmlContent(bname, cplists['cplist'], cplists['ts_update'])
    if None == content:
        logger.error('chapterListViewHandler: makeup cplist html error: path=%s url=%s' % (request.path, url))
        return redirect(url)
    return activeTemplate('cplist.html', bname=bname, url=url, cphost=host, cpscheme=scheme, cptype=cptype, contents=[content])
