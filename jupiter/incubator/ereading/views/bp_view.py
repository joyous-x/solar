# -*- coding: utf-8 -*-
from sanic import Blueprint
from sanic.response import redirect, html, text, json
from sanic.exceptions import NotFound, ServerError
from ereading.common.logger import slogger
from ereading.common.errors import *
from ereading.setting import *
from ereading.views.template import *
from ereading.views import bp_view_search

bp_view = Blueprint('blueprint_view', url_prefix='')


@bp_view.exception(NotFound)
def page404(request, exception):
    return activeTemplate('404.html')

@bp_view.route("/")
async def index(request):
    user = request['session'].get('uid', None) if 'session' in request else None
    search_ranking = [] # TODO: search_ranking()
    return activeTemplate('index.html', title='%s - 搜索' % Config.appname, user=user, search_ranking=search_ranking[:25])

# @bp_view.route("/donate")
# async def donate(request):
#     return activeTemplate('donate.html')
# @bp_view.route("/feedback")
# async def feedback(request):
#     return activeTemplate('feedback.html')

@bp_view.route("/register")
async def register(request):
    user = request['session'].get('uid', None) if 'session' in request else None
    if user:
        return redirect('/')
    return activeTemplate('register.html', title='%s - 注册' % Config.appname)

@bp_view.route("/error")
async def perror(request):
    user = request['session'].get('uid', None) if 'session' in request else None
    if user:
        return redirect('/')
    url = str(request.args.get('url', '')).strip()
    return activeTemplate('perror.html', title='%s - ERROR' % Config.appname, url=url)

@bp_view.route("/logout")
async def logout(request):
    user = request['session'].get('uid', None)
    if user:
        del response.cookies['user']
        del response.cookies['owl_sid']
        return response
    else:
        return json({'status': 0})

@bp_view.route("/search", methods=['GET'])
async def search(request):
    """
    url for test: 
        http://127.0.0.1:8001/search?keys=元尊&pages=2
    """
    return await bp_view_search.searchViewHandler(request)

@bp_view.route("/chapter", methods=['GET'])
async def chapterContent(request):
    """
    url for test: 
        http://127.0.0.1:8001/chapter?url=https://www.biqudao.com/bqge10365/6619010.html&cplist=&bname=元尊
    """
    return await bp_view_search.chapterContentViewHandler(request)


@bp_view.route("/cplist", methods=['GET'])
async def chapterList(request):
    """
    url for test: 
        http://127.0.0.1:8001/cplist?url=https://www.biqudao.com/bqge10365/&bname=元尊
    """
    return await bp_view_search.chapterListViewHandler(request)