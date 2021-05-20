# coding=utf-8
import requests


class ProxyAdapter(object):
    def __init__(self, host):
        self.host = host

    def validible(self, proxy):
        resp = None
        proxies = {
            "http" : "http://{}".format(proxy),
            "https": "http://{}".format(proxy),
        }
        try:
            resp = requests.get("https://www.baidu.com", timeout=5, proxies=proxies)
        except Exception as e:
            print("ProxyAdapter.validible error: {}".format(e))
        return resp is not None and resp.status_code == requests.codes.OK

    def get(self):
        proxy = None
        try:
            resp = requests.get("{}/get/".format(self.host), timeout=5)
            data = resp.json()
            proxy = data.get("proxy")
        except Exception as e:
            print("ProxyAdapter.get error: {}".format(e))
        return proxy

    def delete(self, proxy):
        requests.get("{}/delete/?proxy={}".format(self.host, proxy))

    def status(self):
        count = None
        try:
            resp = requests.get("{}/get_status/".format(self.host), timeout=5)
            data = resp.json()
            count = data("count")
        except Exception as e:
            print("ProxyAdapter.status error: {}".format(e))
        return count


proxyAdapter = ProxyAdapter("http://10.12.129.139:5010/")
