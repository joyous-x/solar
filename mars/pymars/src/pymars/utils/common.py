# coding=utf-8


def message(*args, **kwargs):
    print(' '.join(map(str, args)), **kwargs)  # noqa E999


class DictObject(dict):
    """ convert dict to object
    we can treat dict's keys as attributes
    """
    def __getattr__(self, key):
        return self.get(key)

    def __setattr__(self, key, value):
        self[key] = value


def DecoratorTimeSpend(func):
    """ DecoratorTimeSpend
    """
    def wrapper(*args):
        start = datetime.datetime.now()
        rst = func(*args)
        end = datetime.datetime.now()
        message("===> %s ends: spend(decorator)=%f(seconds)" % (func.__name__, (end-start).total_seconds()))
        return rst
    return wrapper 


@DecoratorTimeSpend
def PoolDo(threads, totals, func, *args, **kwargs):
    message("PoolDo ready for func=%s: threads=%d, totals=%d" % (func.__name__, threads, totals))
    def dofunc(index, *args, **kwargs):
        repeat = totals/threads if (0 == totals % threads) else (totals/threads + 1)
        if repeat * index > totals:
            repeat = totals - (repeat * (threads-1))
        message("PoolDo.dofunc index:%d, repeat %d" % (index, repeat))
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
        message("PoolDo exception: %s" % e)
    finally:
        pass
    