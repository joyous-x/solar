# coding=utf-8
import os
import bs4
import urllib
import requests
from antenna.base.spider_base import *
from antenna.base.spider_simple import SimpleAaItem, SimpleAaRequester
from spider_proxies_adapter import *


class KonachanSpider(AaSpider):
    def __init__(self):
        super(KonachanSpider, self).__init__(name="KonachanSpider")
        self.regist_requester('default', KonachanRequester(method="get", types="bs4"))
        self.regist_requester('downloader', KonachanRequester(method="get", types="file", timeout=8))
        self.regist_parser('default', AlphacodersParser())
        self.regist_handler('default', KonachanHandler(self))

    def run(self, url_home: str):
        self.start(SimpleAaItem(url_home, types="thumbnail"))


class KonachanRequester(SimpleAaRequester):
    def __init__(self, method="get", types="content", timeout=5, headers=None):
        super(KonachanRequester, self).__init__(method=method, types=types, timeout=timeout, headers=headers)
        self.proxy = None
        self.proxies_default = {
            "http": "socks5:101.32.14.3:9094",
            "https": "socks5:101.32.14.3:9094"
        }

    def get_proxies(self, force_refresh=False):
        if force_refresh:
            proxyAdapter.delete(self.proxy)
            self.proxy = None
        for i in range(100):
            if None != self.proxy:
                break
            time.sleep(0.3)
            proxy = proxyAdapter.get()
            if proxyAdapter.validible(proxy):
                self.proxy = proxy
        if None == self.proxy:
            return self.proxies_default
        return {"http:": "http://{}".format(self.proxy), "https": "http://{}".format(self.proxy)}

    def request(self, item: AaItemBase, data=None, params=None, proxies: dict = None, max_try=3) -> Any:
        resp = None
        for i in range(max_try):
            proxies = self.get_proxies(i != 0)
            resp = super(KonachanRequester, self).request(item, data, params, proxies, max_try=1)
            if None is not resp:
                break
        return resp


class KonachanHandler(AaHandler):
    def __init__(self, spider: KonachanSpider):
        super(KonachanHandler, self).__init__()
        self.spider = spider

    def handle(self, datas: List[AaItemBase]):
        if not isinstance(datas, list):
            return None
        for item in datas:
            if item.type == "detail":
                self.spider.scheduler.push(item)
            elif item.type == "final":
                item.parser = None
                item.handler = None
                item.requester = "downloader"

                url = item.item
                pos = url.find("?")
                if pos > 0:
                    url_tmp = url[:pos]
                    file_name = url_tmp.split('/')[-1]
                else:
                    file_name = url.split('/')[-1]
                file_name = urllib.parse.unquote(file_name)
                item.set_meta("file_path", "./img/{}".format(file_name))

                self.spider.scheduler.push(item)
        return


class AlphacodersParser(AaParser):
    def __init__(self):
        super(AlphacodersParser, self).__init__()

    def parse(self, item: AaItemBase, soup: bs4.BeautifulSoup):
        if item.type == "thumbnail":
            return self._parse_page_thumbnail(item, soup)
        elif item.type == "detail":
            return self._parse_page_detail(item, soup)

    def _parse_page_thumbnail(self, item: AaItemBase, soup: bs4.BeautifulSoup):
        if None == soup:
            return None
        classlist = soup.find_all(id="post-list-posts")
        if None == classlist:
            return None
        assert (len(classlist) == 1)
        container = classlist[0]
        items = container.select("li > div > a.thumb")
        if items == None:
            return None
        rst = []
        for item in items:
            tmpItem = SimpleAaItem("https://konachan.net{}".format(item.attrs['href']), types="detail")
            rst.append(tmpItem)
        return rst

    def _parse_page_detail(self, item: AaItemBase, soup: bs4.BeautifulSoup):
        infos = soup.select("body #content > #post-view > .content > div > img.image")
        if infos == None:
            return None
        item = infos[0]
        alt = item.attrs["alt"]
        src = item.attrs["src"]
        if src == None:
            return None
        tmpItem = SimpleAaItem(src, types="final")
        tmpItem.set_meta("alt", alt)
        tmpItem.set_meta("src", src)
        return [tmpItem]

import asyncio

class AsyncTask(object):
    """
        thread
        yield
        async await
    """
    def __init__(self):
        pass

    async def testHiBaidu(self, flag):
        resp = requests.get("http://www.baidu.com")
        print(flag)
        return resp

    def testAsync(self):
        resp = await self.testHiBaidu("1")
        print(resp.content)
        loop = asyncio.get_event_loop()
        res = loop.run_until_complete(asyncio.wait([self.testHiBaidu("a"), self.testHiBaidu("b")]))
        loop.close()


if __name__ == "__main__":
    test = AsyncTask()
    test.testAsync()

    asyncio.run

    wpaper = KonachanSpider()
    for i in range(0, 0):
        mlog.debug("!!! ready for ----> {}".format(i))
        wpaper.run("https://konachan.net/post?page={}&tags=".format(i))
