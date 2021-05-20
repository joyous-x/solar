# -*- coding: utf-8 -*-
import aiohttp
import asyncio
import async_timeout
from aiocache.serializers import PickleSerializer
from bs4 import BeautifulSoup
from urllib.parse import urlparse
from .base import *
from ereading.models.decorator import cache_handler
from ereading.common.logger import slogger as logger

class BingSearch(BaseSearch):
    def __init__(self, mode=mode_type_w):
        super(BingSearch, self).__init__(mode=mode)
        self.homeHost = 'www2.bing.com'
        self.homeURL  = 'https://%s/' % self.homeHost

    async def search(self, keywords, page=1):
        params = { 'q': keywords, 'sp': 1 }
        if page > 1:
            params['first'] = (page-1) * 10 + 1
        headers = { 'cookie':'_EDGE_V=1', "host": self.homeHost}
        html = await self.request(url=self.homeURL, params=params, headers=headers)
        if html:
            result = BeautifulSoup(html, 'html5lib').find_all(class_='b_algo')
            extra_tasks = [self.parse(html=i) for i in result]
            tasks = [asyncio.ensure_future(i) for i in extra_tasks]
            done_list, pending_list = await asyncio.wait(tasks)
            res = [task.result() for task in done_list if task.result()]
            return res
        else:
            return []

    async def parse(self, html):
        try:
            click_url = html.select('h2 a')[0].get('href', None)
            if not click_url:
                return None
            real_host = urlparse(click_url).netloc
            reserved = True
            if real_host not in self.whiteRules:
                logger.info("url_not_reserved : %s " % (click_url))
                reserved = False
            title = html.select('h2 a')[0].get_text()
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
        tmp = await BingSearch.run(keywords, i+1)
        result += tmp 
        logger.debug("bing search keys=%s page=%d got items=%d" % (keywords, i+1, len(tmp)))
    return result
