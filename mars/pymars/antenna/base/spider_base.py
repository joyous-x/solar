# coding=utf-8
import time
import random
from typing import Any, List
from antenna.utils.logger import mlog

"""
框架：

方法：
    api 接口逆向、selenium+phantomJS 模拟

FORBIDDEN:
    隐私、反爬、robots.txt、不正当竞争、付费内容、反爬措施不共享
"""


class AaBase(object):
    """ Antenna base :
    global information about the spider
    """
    pass


class AaItemBase(AaBase):
    """ mini spider : item(url, etc.)
    call do() will get the content of a page which should be handled by MsParser
    """

    def __init__(self, iid: str, types: str, item: Any, requester: str = None, parser: str = None, handler: str = None, meta: dict = None):
        if None == iid:
            iid = "{}-{}".format(int(round(time.time() * 1000)), int(random.random() * 10000))
        self.iid = iid
        self.item = item
        self.type = types
        self.meta = meta if meta is not None else dict()
        self.requester = requester if requester is not None else "default"
        self.parser = parser if parser is not None else "default"
        self.handler = handler if handler is not None else "default"

    def set_metas(self, metas: dict()):
        for k, v in metas:
            self.meta[k] = v

    def set_meta(self, key, val):
        self.meta[key] = val

    def get_meta(self, key=None):
        if key is None:
            return self.meta
        if key in self.meta:
            return self.meta[key]
        return None

    def set_type(self, types: str):
        self.type = types

    def request_data(self):
        raise NotImplementedError("AaItemBase.request_data not implemented!")

    def request_params(self):
        raise NotImplementedError("AaItemBase.request_params not implemented!")


class MsSchedulerBase(AaBase):
    """ mini spider : scheduler item(url, etc.) """

    def __init__(self):
        self.filters = []
        self.items = []

    def add_filter(self, fn_filter):
        if fn_filter not in self.filters:
            self.filters.append(fn_filter)

    def push(self, item: AaItemBase) -> bool:
        rst = True
        for fn in self.filters:
            ret = fn(item)
            if ret is False:
                rst = False
                break
        if rst:
            self.items.append(item)
        return rst

    def pop(self) -> AaItemBase:
        rst = None
        if len(self.items) > 0:
            rst = self.items[0]
            self.items = self.items[1:]
        return rst


class AaRequester(AaBase):
    """ mini spider : page requester
    the return of function request() is the formatted data which should be handler by AaParser
    """

    def __init__(self, name="default"):
        self.name = name

    def request(self, item: AaItemBase, data=None, params=None, proxies: dict = None, max_try=1) -> Any:
        raise NotImplementedError("AaRequester.request not implemented!")


class AaParser(AaBase):
    """ mini spider : page parser
    the return of function parse() is the formatted data which should be handler by AaHandler
    """

    def __init__(self, name="default"):
        self.name = name

    def parse(self, item: AaItemBase, requester_response: Any) -> List[AaItemBase]:
        raise NotImplementedError("AaParser.parse not implemented!")


class AaHandler(AaBase):
    """ mini spider : data handler
    as the result of handler, new item may be produced and scheduled, item may be downloaded, .etc
    """

    def __init__(self, name="default"):
        self.name = name

    def handle(self, parser_response: List[AaItemBase]):
        raise NotImplementedError("AaHandler.handle not implemented!")


class AaSpider(AaBase):
    """ mini spider : base """

    def __init__(self, name: str = "my_spider"):
        self.name = name
        self.scheduler = MsSchedulerBase()
        self.handlers = dict()
        self.parsers = dict()
        self.requesters = dict()
        self.regist_handler("default", AaHandler())
        self.regist_parser("default", AaParser())
        self.regist_requester("default", AaRequester())

    def regist_handler(self, name: str, handler: AaHandler):
        self.handlers[name] = handler

    def regist_parser(self, name: str, parser: AaParser):
        self.parsers[name] = parser

    def regist_requester(self, name: str, requester: AaRequester):
        self.requesters[name] = requester

    def start(self, root_item: AaItemBase):
        ''' TODO：增加多线程支持
        '''
        mlog.info("---> AaSpider ready: {}".format(root_item.iid))
        self.scheduler.push(root_item)
        while True:
            item = self.scheduler.pop()
            if None == item:
                break

            if None == item.requester or item.requester not in self.requesters:
                mlog.warn("AaSpider: item(id={}) need a available requester({})".format(item.iid, item.requester))
                continue
            response = self.requesters[item.requester].request(item, data=item.request_data(), params=item.request_params())

            if None == item.parser:
                mlog.warn("AaSpider: item(id={}) parser is None, final".format(item.iid))
                continue
            elif item.parser not in self.parsers:
                mlog.warn("AaSpider: item(id={}) need a available parser({})".format(item.iid, item.parser))
                continue
            new_items = self.parsers[item.parser].parse(item, response)

            if None == item.handler:
                mlog.warn("AaSpider: item(id={}) handler is None, final".format(item.iid))
                continue
            elif item.handler not in self.handlers:
                mlog.warn("AaSpider: item(id={}) need a available handler({})".format(item.iid, item.handler))
                continue
            self.handlers[item.handler].handle(new_items)
        mlog.info("---> AaSpider end: {}".format(root_item.iid))


if __name__ == "__main__":
    raise NotImplementedError("Ms is not implemented!")
