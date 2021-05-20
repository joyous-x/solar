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

class BaiduSearch(BaseSearch):
    def __init__(self, mode=mode_type_w):
        super(BaiduSearch, self).__init__(mode=mode)
        if mode == mode_type_m:
            self.homeURL = 'https://m.baidu.com/s'
        else:
            self.homeURL = 'http://www.baidu.com/s'

    async def search(self, keywords, page=1):
        params = {'wd': keywords, 'ie': 'utf-8'}
        if page > 1:
            params['pn'] = (page-1) * 10
        html = await self.request(url=self.homeURL, params=params)
        if html:
            result = BeautifulSoup(html, 'html5lib').find_all(class_='result')
            extra_tasks = [self.parse(html=i) for i in result]
            tasks = [asyncio.ensure_future(i) for i in extra_tasks]
            done_list, pending_list = await asyncio.wait(tasks)
            res = [task.result() for task in done_list if task.result()]
            return res
        else:
            return []

    async def parse(self, html):
        try:
            click_url = html.select('h3.t a')[0].get('href', None)
            real_url = await self.head(url=click_url) if click_url else None
            if not real_url:
                return None
            real_url = str(real_url)
            real_host = urlparse(real_url).netloc
            reserved = True
            if real_host not in self.whiteRules:
                logger.info("url_not_reserved : %s " % (real_url))
                reserved = False
            title = html.select('h3.t a')[0].get_text()
            recommend = 1 if real_host in self.rulesLatest.keys() else 0
            return {'title': title, 
                    'recommend': recommend,
                    'reserved': real_host in self.whiteRules,
                    'timestamp': None,
                    'realhost': real_host,
                    'url': real_url.replace('index.html', '')}
        except Exception as e:
            logger.exception(e)
            return None

    async def getRealURL(self, url):
        response = await self.request(url=url)
        if not response:
            return None
        return response.url if response.url else None

@cache_handler(ttl=259200)
async def start(keywords, pages=1):
    result = []
    for i in range(pages):
        tmp = await BaiduSearch.run(keywords, i+1)
        result += tmp 
        logger.debug("baidu search keys=%s page=%d got items=%d" % (keywords, i+1, len(tmp)))
    return result
