#!/usr/bin/python3
import aiocache
import os
import sys

from sanic import Sanic
from sanic.response import html, redirect, json
from sanic_session import RedisSessionInterface, InMemorySessionInterface
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from ereading.common.logger import slogger as logger
from ereading.models.dbs import redis 
from ereading.views.bp_api import bp_api
from ereading.views.bp_view import bp_view
from ereading.views.bp_static import bp_static
from ereading.setting import *

app = Sanic(__name__, log_config=None)
app.blueprint(bp_api)
app.blueprint(bp_view)
app.blueprint(bp_static)

@app.listener('before_server_start')
def init_cache(app, loop):
    logger.info("before_server_start: Starting aiocache")
    # reference: https://sanic-session.readthedocs.io/en/latest/api.html
    if Config.debug:
        app.session_interface = InMemorySessionInterface(session_name="session", cookie_name="sinfo", expiry=7*24*3600)
    else:
        app.session_interface = RedisSessionInterface(redis.redisInst.get_redis_pool, session_name="session", cookie_name="sinfo", expiry=7*24*3600)

@app.middleware('request')
async def before_request(request):
    host = request.headers.get('host', None)
    user_agent = request.headers.get('user-agent', None)
    if user_agent:
        await app.session_interface.open(request)
        user_ip = request.headers.get('X-Forwarded-For')
    else:
        return html("<h3>网站正在维护...</h3>")
    logger.debug("before_request.access path=%s" % request.url)

@app.middleware('response')
async def save_session(request, response):
    try:
        if request.path == '/v1/user/login' and request['session'].get(SessKey.sid, None):
            await app.session_interface.save(request, response)
            import datetime
            response.cookies['sexpires'] = str(datetime.datetime.now() + datetime.timedelta(days=7))
        elif request.path == '/v1/user/logout':
            await app.session_interface.save(request, response)
        elif request.path == '/v1/user/regist':
            response.cookies['regindex'] = str(request['session'][SessKey.index][0])
        else:
            logger.debug('save_session ending: path={}'.format(request.path))
    except KeyError as e:
        logger.error(e)

if __name__ == "__main__":
    app.run(host="0.0.0.0", workers=1, port=8001, debug=Config.debug)
