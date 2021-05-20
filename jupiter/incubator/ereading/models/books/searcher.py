# -*- coding: utf-8 -*-
from operator import itemgetter
from ereading.models.searcher import entry as sentry

async def SearchBook(engine, keys, pages=1):
    """
        提取并标准化搜索引擎的搜索结果
    """
    result = None
    keyOpt = None
    keys = keys.strip()
    if len(engine) == 0:
        # bing 经常出问题, 所以使用 baidu, TODO fix
        engine = sentry.EngineConf.baidu 
    for item in sentry.EngineNames:
        if item != engine:
            continue
        if item == "bing":
            keyOpt = "{name} 小说 阅读 最新章节".format(name=keys)
        if item == "so360": # 360 so
            keyOpt = "{name} 小说 最新章节".format(name=keys)
        if item == "baidu":
            keyOpt = 'intitle:{name} 小说 阅读'.format(name=keys)
    if not keyOpt:
        return None
    result = await sentry.Search(engine=engine, keywords=keyOpt, pages=pages)
    if not result:
        return None 
    result_sorted = sorted(result, reverse=True, key=itemgetter('recommend', 'reserved', 'timestamp'))
    return result_sorted