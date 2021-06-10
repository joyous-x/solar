# coding=utf-8
import multiprocessing


"""
start ：gunicorn -w 4 -b 0.0.0.0:8000 app:app
apache bench : ab -n 100 -c 10 -p env/for_test/ab_json_font.txt -T application/json "http://127.0.0.1:8000/font"
"""


bind = '0.0.0.0:8000'      #绑定ip和端口号
backlog = 512                #监听队列
#chdir = '/home/test/server/bin'  #gunicorn要切换到的目的工作目录
timeout = 30      #超时
worker_class = 'gevent' #使用gevent模式，还可以使用sync 模式，默认的是sync模式

# 指定进程数 以及 每个进程开启的线程数
workers = multiprocessing.cpu_count() * 2 + 1
threads = 2

# 日志级别，这个日志级别指的是错误日志的级别，而访问日志的级别无法设置
loglevel = 'info'
# 设置gunicorn访问日志格式，错误日志无法设置
access_log_format = '%(t)s %(p)s %(h)s "%(r)s" %(s)s %(L)s %(b)s %(f)s" "%(a)s"'
accesslog = "/tmp/gunicorn_access.log"
errorlog = "/tmp/gunicorn_error.log"