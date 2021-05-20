# coding=utf-8
import pymysql
import threading
import logging
import sys
import datetime
from redisinst import *

logging.basicConfig(
    level = logging.INFO, stream = sys.stdout,
    format = '%(asctime)s [%(levelname)s] %(filename)s[line:%(lineno)d]: %(message)s')

def DecoratorTimeSpend(func):
    def wrapper(*args):
        start = datetime.datetime.now()
        rst = func(*args)
        end = datetime.datetime.now()
        logging.info("===> %s ends: spend(decorator)=%f(seconds)" % (func.__name__, (end-start).total_seconds()))
        return rst
    return wrapper 

@DecoratorTimeSpend
def PoolDo(threads, totals, func, *args, **kwargs):
    logging.info("PoolDo ready for func=%s: threads=%d, totals=%d" % (func.__name__, threads, totals))
    def dofunc(index, *args, **kwargs):
        repeat = totals/threads if (0 == totals % threads) else (totals/threads + 1)
        if repeat * index > totals:
            repeat = totals - (repeat * (threads-1))
        logging.debug("PoolDo.dofunc index:%d, repeat %d" % (index, repeat))
        for i in xrange(repeat):
            func(index, *args, **kwargs)
    try:
        threadList = []
        for i in range(threads):
            t = threading.Thread(target=dofunc, name='PoolDo-'+str(i), args=(i+1,) + args, kwargs=kwargs)
            threadList.append(t)
        for t in threadList:
            t.start()
        for t in threadList:
            t.join()
    except Exception, e:
        logging.error("PoolDo exception: %s" % e)
    finally:
        pass
    
################### MYSQL ################### 

def ConnMysql(host, port, user, passwd, dbname):
    config = {
        'host': host,
        'port': port, 
        'user': user,
        'password': passwd,
        'db': dbname,
        'charset': 'utf8mb4',
        'cursorclass':pymysql.cursors.DictCursor,
    }
    #pymysql.connect(host=host, port=port, user=user, password=passwd, db=dbname, charset='utf8mb4')
    return pymysql.connect(**config)

def testMysql(index, connection):
    try:
        with connection.cursor() as cursor:
            sql = 'select * from my_test_table limit 2'
            cursor.execute(sql)
            results = cursor.fetchall()
            for i in xrange(len(results)):
                logging.debug("result %d --- %s" % (i, results[i]))
            connection.commit() # 没有设置默认自动提交，需要主动提交，以保存所执行的语句
    finally:
        connection.close()
#PoolDo(1, 10, testMysql, ConnMysql("127.0.0.1", 3306, "admin", "king", "mytestdb"))

################### REDIS.list ################### 

def redisListLpush(index, redis, key, size):
    try:
        for i in xrange(size):
            rst = redis.lpush(key, i)
            if rst != i+1:
                logging.error("redis.lpush %s error: index=%d" % (key, i))
                return
    finally:
        pass

def redisListRpop(index, redis, key, size):
    try:
        for i in xrange(size):
            rst = redis.rpop(key)
            if int(rst) != i:
                logging.error("redis.rpop %s error: expect=%d, rst=%d" % (key, i, int(rst)))
                return
    finally:
        redis.delete(key)

def redisListLrange(index, redis, key, size):
    try:
        rst = redis.lrange(key, 0, -1)
        if len(rst) != size:
            logging.error("redis.lpush %s error: size=%d, expect=%d" % (key, len(rst), size))
            return 
    finally:
        redis.delete(key)

def testRedisList():
    key  = "mytest_list_key"
    size = 10000
    PoolDo(1, 1, redisListLpush, RedisInst("10.12.32.140:6379::").conn(), key+"a", size)
    PoolDo(1, 1, redisListRpop, RedisInst("10.12.32.140:6379::").conn(), key+"a", size)
    PoolDo(1, 1, redisListLpush, RedisInst("10.12.32.140:6379::").conn(), key+"b", size)
    PoolDo(1, 1, redisListLrange, RedisInst("10.12.32.140:6379::").conn(), key+"b", size)
testRedisList()