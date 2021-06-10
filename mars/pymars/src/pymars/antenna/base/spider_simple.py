# coding=utf-8
import requests
import bs4
import os
from ..base.spider_base import *
from ..utils.logger import mlog
from ..utils.user_agent import randUserAgent

""" Usage:

class MySpider(AaSpider):
    def __init__(self):
        def func_proxies():
            proxies_default = { }
            return proxies_default
        super(MySpider, self).__init__(name="MySpider")
        self.regist_requester('default', SimpleAaRequester(method="get", types="bs4", timeout=5, func_proxies=func_proxies))
        self.regist_requester('downloader', SimpleAaRequester(method="get", types="file", timeout=8, func_proxies=func_proxies))
        self.regist_handler('default', SimpleAaHandler(self))
        self.regist_parser('default', MyParser())

    def run(self, url_home: str):
        self.start(SimpleAaItem(url_home, types="item_type_user_defined"))

class MyParser(AaParser):
    def __init__(self):
        super(MyParser, self).__init__()

    def parse(self, item: AaItemBase, soup: bs4.BeautifulSoup):
        rst_items = []
        if item.type == "item_type_user_defined":
            # TODO
        else:
            # TODO
        return rst_items
            
def start(root_url):
    spider = MySpider()
    for i in range(1, 10):
        spider.run("root_url/?kw=&areas=&page={}".format(i))

"""

class SimpleAaItem(AaItemBase):
    def __init__(self, item, iid=None, types=None, requester="default", parser="default", handler="default"):
        super(SimpleAaItem, self).__init__(iid, types, item, requester=requester, parser=parser, handler=handler)
        self.set_type(types)

    def request_data(self):
        return None

    def request_params(self):
        return None


class SimpleAaHandler(AaHandler):
    def __init__(self, spider: AaSpider):
        super(SimpleAaHandler, self).__init__()
        self.spider = spider

    def handle(self, datas: List[AaItemBase]):
        if not isinstance(datas, list):
            return None
        for item in datas:
            self.spider.scheduler.push(item)
        return None


class SimpleAaRequester(AaRequester):
    """ mini spider : SimpleAaRequester
        simple implemention for request and proxies
    """
    def __init__(self, method="get", types="content", timeout=5, headers=None, func_proxies=None):
        """ SimpleAaRequester
        func_proxies is a function which will return an dict : {"http":"", "https":""}
        if func_proxies is None, request will do directly. otherwise, use the proxies returned by func_proxies.
        """
        super(SimpleAaRequester, self).__init__()
        self.method = method
        self.types = types
        # proxies by http and socks
        def func_proxies_default():
            proxies = {
                # "http": "http://10.10.1.10:3128",
                # "https": "http://10.10.1.10:1080",
                # 'http': 'socks5://user:pass@host:port',
                # 'https': 'socks5://user:pass@host:port'
            }
            return proxies
        self.func_proxies = func_proxies if func_proxies else func_proxies_default
        # timeout 仅对连接过程有效，与响应体的下载无关
        self.timeout = timeout
        # headers
        if headers == None:
            headers = { 'Accept': '*/*', 'user-agent': '' }
        self.headers = headers

    def request(self, item: AaItemBase, data=None, params=None, proxies: dict = None, max_try=3) -> Any:
        resp = None
        headers = self.headers
        if None == item:
            return resp
        if None is proxies:
            proxies = self.func_proxies()
        if None != headers:
            headers["user-agent"] = randUserAgent()
        status_code = None
        for i in range(max_try):
            try:
                if self.types == "content":
                    status_code, resp = self.request_for_content(item, data=data, params=params, headers=headers, proxies=proxies)
                elif self.types == "bs4":
                    status_code, resp = self.request_for_bs4(item, data=data, params=params, headers=headers, proxies=proxies)
                elif self.types == "file":
                    status_code, resp = self.request_for_file(item, data=data, params=params, headers=headers, proxies=proxies)
            except Exception as e:
                mlog.error("SimpleAaRequester.request exception try({}): {}".format(i + 1, e))
        if status_code is not None:
            rst = "ok" if status_code == requests.codes.OK else "error({})".format(status_code)
            mlog.debug("SimpleAaRequester.request({} {}) {} id({}): {}".format(self.types, self.method, rst, item.iid, item.item))
        return resp

    def request_for_content(self, item: AaItemBase, data, params, headers, proxies) -> Any:
        if self.method != "get":
            return None
        resp = requests.get(item.item, timeout=self.timeout, headers=headers, proxies=proxies, allow_redirects=True, verify=True)
        if None == resp:
            return None
        return resp.status_code, (resp.content, resp.encoding)

    def request_for_bs4(self, item: AaItemBase, data, params, headers, proxies) -> bs4.BeautifulSoup:
        resp = self.request_for_content(item, data, params, headers, proxies)
        if None == resp or resp[0] != 200:
            return None
        status_code, content, encoding = resp[0], resp[1][0], resp[1][1]
        return status_code, bs4.BeautifulSoup(content, from_encoding=encoding, features="html.parser")

    def request_for_file(self, item: AaItemBase, data, params, headers, proxies) -> int:
        file_path = item.get_meta("file_path")
        if file_path is None:
            raise Exception("don't have meta: file_path")
        dir_path, _ = os.path.split(file_path)
        if not os.path.exists(dir_path):
            os.mkdir(dir_path)
        resp = requests.get(item.item, timeout=self.timeout, headers=headers, proxies=proxies)
        if resp.status_code == requests.codes.OK:
            with open(file_path, "wb") as f:
                f.write(resp.content)
        return resp.status_code, None
