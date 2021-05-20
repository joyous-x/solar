# coding=utf-8
import requests
import bs4
import os
from antenna.base.spider_base import *
from antenna.utils.logger import mlog
from antenna.utils.user_agent import randUserAgent


class SimpleAaItem(AaItemBase):
    def __init__(self, item, iid=None, types=None, requester="default", parser="default", handler="default"):
        super(SimpleAaItem, self).__init__(iid, types, item, requester=requester, parser=parser, handler=handler)
        self.set_type(types)

    def request_data(self):
        return None

    def request_params(self):
        return None


class SimpleAaRequester(AaRequester):
    """ mini spider : SimpleAaRequester
        simple implemention for request and proxies
    """
    def __init__(self, method="get", types="content", timeout=5, headers=None, proxies: dict = None):
        """ if proxies is None, request will try find proxies by get_proxies
        """
        super(SimpleAaRequester, self).__init__()
        self.method = method
        self.types = types
        # proxies by http and socks
        if None is proxies:
            proxies = {
                # "http": "http://10.10.1.10:3128",
                # "https": "http://10.10.1.10:1080",
                # 'http': 'socks5://user:pass@host:port',
                # 'https': 'socks5://user:pass@host:port'
            }
        self.proxies = proxies
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
            proxies = self.proxies
        if None != headers:
            headers["user-agent"] = randUserAgent()
        for i in range(max_try):
            try:
                if self.types == "content":
                    resp = self.request_for_content(item, data=data, params=params, headers=headers, proxies=proxies)
                elif self.types == "bs4":
                    resp = self.request_for_bs4(item, data=data, params=params, headers=headers, proxies=proxies)
                elif self.types == "file":
                    resp = self.request_for_file(item, data=data, params=params, headers=headers, proxies=proxies)
            except Exception as e:
                mlog.error("SimpleAaRequester.request exception try({}): {}".format(i + 1, e))
        if None is not resp:
            mlog.debug("SimpleAaRequester.request({} {}) ok id({}): {}".format(self.types, self.method, item.iid, item.item))
        return resp

    def request_for_content(self, item: AaItemBase, data, params, headers, proxies) -> Any:
        if self.method != "get":
            return None
        resp = requests.get(item.item, timeout=self.timeout, headers=headers, proxies=proxies, allow_redirects=True, verify=True)
        if None == resp:
            return None
        return (resp.status_code, resp.content, resp.encoding)

    def request_for_bs4(self, item: AaItemBase, data, params, headers, proxies) -> bs4.BeautifulSoup:
        resp = self.request_for_content(item, data, params, headers, proxies)
        if None == resp or resp[0] != 200:
            return None
        status, content, encoding = resp
        return bs4.BeautifulSoup(content, from_encoding=encoding, features="html.parser")

    def request_for_file(self, item: AaItemBase, data, params, headers, proxies) -> int:
        file_path = item.get_meta("file_path")
        dir_path, _ = os.path.split(file_path)
        if not os.path.exists(dir_path):
            os.mkdir(dir_path)
        resp = requests.get(item.item, timeout=self.timeout, headers=headers, proxies=proxies)
        if resp.status_code == requests.codes.OK:
            with open(file_path, "wb") as f:
                f.write(resp.content)
        return resp.status_code
