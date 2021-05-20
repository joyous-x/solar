# coding=utf-8
import redis

"""
RedisInst
    host: for example, "127.0.0.1:6379:o3xTl(ptCV38mrKn:"
"""
class RedisInst(object):
    def __init__(self, host):
        self.inst = None
        self._ip, self._port, self._pass, self._db = self.split_host_conf(host)
        self._conn(self._ip, self._port, self._pass, self._db)
        super(RedisInst, self).__init__()

    def __del__(self):
        self.close()

    def _conn(self, _ip, _port, _pass, _db):
        if self.inst != None:
            raise("redis conn already exists")
        self.inst = redis.Redis(host=_ip, port=_port, password=_pass, db=_db)
        return self.inst

    def split_host_conf(self, host):
        tmp_list = host.split(":")
        if len(tmp_list) != 4:
            return None,None,None,None
        _ip = tmp_list[0]
        _port = int(tmp_list[1])
        if len(tmp_list[2]) > 0:
            _pass = tmp_list[2]
        else:
            _pass = None
        if len(tmp_list[3]) > 0:
            _db = int(tmp_list[3])
        else:
            _db = 0
        return _ip, _port, _pass, _db

    def conn(self):
        return self.inst

    def close(self): 
        self.inst = None
