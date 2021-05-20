# -*- coding: utf-8 -*-
import aiohttp
import asyncio
import async_timeout
from aiocache.serializers import PickleSerializer
from bs4 import BeautifulSoup
from urllib.parse import urlparse, parse_qs
from .base import *
from ereading.models.decorator import cache_handler
from ereading.common.logger import slogger as logger 

class So360Search(BaseSearch):
    def __init__(self, mode=mode_type_w):
        super(So360Search, self).__init__(mode=mode)
        if mode == mode_type_m:
            self.homeHost = 'm.so.com'
            self.homeURL = 'https://m.so.com/s'
        else:
            self.homeHost = 'www.so.com'
            self.homeURL = 'https://www.so.com/s'

    async def search(self, keywords, page=1):
        params = {'ie': 'utf-8', 'src': 'noscript_home', 'shb': 1, 'q': keywords}
        if page > 1:
            params['pn'] = page
        headers = {
            'Referer': "http://%s/haosou.html?src=home" % self.homeHost
        }
        html = await self.request(url=self.homeURL, params=params)
        if html:
            result = BeautifulSoup(html, 'html5lib').find_all(class_='res-list')
            extra_tasks = [self.parse(html=i) for i in result]
            tasks = [asyncio.ensure_future(i) for i in extra_tasks]
            done_list, pending_list = await asyncio.wait(tasks)
            res = [task.result() for task in done_list if task.result()]
            return res
        else:
            return []

    async def parse(self, html):
        try:
            click_url = html.select('h3 a')[0].get('href', None)
            if ".so.com/link?m=" in click_url:
                click_url = html.select('h3 a')[0].get('data-url', None)
            elif ".so.com/link?url=" in click_url:
                click_url = parse_qs(urlparse(url).query).get('url', None)
                click_url = click_url[0] if click_url else None
            real_host = urlparse(click_url).netloc
            reserved = True
            if real_host not in self.whiteRules:
                logger.info("url_not_reserved : %s " % (click_url))
                reserved = False
            title = html.select('h3 a')[0].get_text()
            recommend = 1 if real_host in self.rulesLatest.keys() else 0
            return {'title': title, 
                    'recommend': recommend,
                    'reserved': reserved,
                    'timestamp': None,
                    'realhost': real_host,
                    'url': click_url.replace('index.html', '').replace('Index.html', '')}
        except Exception as e:
            logger.exception(e)
            return None

@cache_handler(ttl=259200)
async def start(keywords, pages=1):
    result = []
    for i in range(pages):
        tmp = await So360Search.run(keywords, i+1)
        result += tmp 
        logger.debug("bing search keys=%s page=%d got items=%d" % (keywords, i+1, len(tmp)))
    return result