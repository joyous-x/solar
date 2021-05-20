# coding=utf-8
import requests
import json
import random
import time
from antenna.utils.logger import mlog as logger

class MoguProxyPool(object):
    """ ProxyPool: a simple pool of proxies from http://piping.mogumiao.com/proxy/api """
    def __init__(self, proxyMoguHost):
        self.proxiesPool = dict()
        self.proxyMoguHost = proxyMoguHost

    def getProxyIPs(self, proxyHost):
        proxies = []
        try:
            resp = requests.get(proxyHost)
            if resp.status_code != 200:
                logger.error("getProxy error(%s) : %s" % (resp.status_code, proxyHost))
            else:
                respData = json.loads(resp.text)
                if respData == None:
                    logger.error("getProxy error(%s) : %s" % ("invalid json", proxyHost))
                elif respData["code"] != "0":
                    code, errorMsg = int(respData["code"]), ""
                    if code == 3001:
                        errorMsg = "appkey提取频繁，请按照所购买订单规定的频率进行合理提取。如仍未解决，建议检查相关后台进程"
                    if code == 3002 or code == 3005:
                        errorMsg = "请检查订单的有效时间"
                    if code == 3006:
                        errorMsg = "请检查订单的剩余数量"
                    if code == 3004:
                        errorMsg = "appkey有误"
                    logger.error("getProxy error(%s) : %s" % (errorMsg, proxyHost))
                else:
                    for item in respData["msg"]:
                        ip, port = item["ip"],item["port"]
                        proxies.append("%s:%s" % (ip, port))
        except Exception as e:
            logger.error("getProxy error(%s)" % (e))
        return proxies

    def getValidProxyIPFromPool(self):
        if len(self.proxiesPool) < 8:
            ips = self.getProxyIPs(self.proxyMoguHost)
            for i in ips:
                logger.debug("addToProxyPool -> %s" % i)
                self.proxiesPool[i] = 0
            if len(self.proxiesPool) == 0:
                raise Exception("Invalid proxiesPool!")
            time.sleep(8)
        randomKey = random.choice(list(self.proxiesPool.keys()))
        self.proxiesPool[randomKey] = self.proxiesPool[randomKey] + 1
        return randomKey

    def popFromProxyIPFromPool(self, key):
        logger.debug("popFromProxyPool -> %s" % key)
        self.proxiesPool.pop(key)

