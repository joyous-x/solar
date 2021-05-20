# -*- coding: utf-8 -*-
import aiohttp
import async_timeout
from ereading.common.logger import logger 
from ereading.setting import *
from ereading.models import helper
from ereading.models.searchrules import ConfigBookSearch

mode_type_w = "pc"
mode_type_m = "mobile"

class BaseSearch:
    def __init__(self, mode):
        self.mode = mode
        self.whiteRules = ConfigBookSearch.rules
        self.rulesLatest = ConfigBookSearch.rulesLatest

    async def request(self, url, params, headers=None, allow_redirects=True):
        return await helper.HtmlGetter(url, params, headers, allow_redirects)

    async def head(self, url, headers=None, allow_redirects=True):
        if not headers:
            headers = {'user-agent': helper.RandomUserAgent()}
        elif 'user-agent' not in headers:
            headers['user-agent'] = helper.RandomUserAgent()
        with async_timeout.timeout(Config.timeout):
            try:
                async with aiohttp.ClientSession() as client:
                    async with client.head(url, headers=headers, allow_redirects=True) as response:
                        return response.url if response.url else None
            except Exception as e:
                logger.exception(e)
            return None

    @classmethod
    async def run(cls, keywords, page=1):
        return await cls().search(keywords, page)

    async def parse(self, html):
        raise NotImplementedError

    async def search(self, keywords, page):
        raise NotImplementedError
