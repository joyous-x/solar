import random
import cchardet
import aiohttp
import async_timeout
import requests
from ereading.setting import *
from ereading.common.logger import logger 

def RandomUserAgent() -> str:
    return random.choice(Config.user_agents)

async def HtmlGetter(url, params=None, headers=None, allow_redirects=True, timeout=Config.timeout):
    if not headers:
        headers = {'user-agent': RandomUserAgent()}
    elif 'user-agent' not in headers:
        headers['user-agent'] = RandomUserAgent()
    with async_timeout.timeout(timeout):
        try:
            async with aiohttp.ClientSession() as client:
                async with client.get(url, params=params, headers=headers, allow_redirects=allow_redirects) as response:
                    assert response.status == 200
                    try:
                        text = await response.text()
                    except:
                        text = await response.read()
                    return text
        except Exception as e:
            logger.error("htmlGetter exception: %s" % str(e))
            return None

def HtmlGetterByRequests(url, headers=None, timeout=Config.timeout):
    if not headers:
        headers = {'user-agent': RandomUserAgent()}
    elif 'user-agent' not in headers:
        headers['user-agent'] = RandomUserAgent()
    try:
        response = requests.get(url=url, headers=headers, verify=False, timeout=timeout)
        response.raise_for_status()
        content = response.content
        charset = cchardet.detect(content)
        return content.decode(charset['encoding'])
    except Exception as e:
        logger.error("htmlGetterByRequests exception: %s" % str(e))
        return None
