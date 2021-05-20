# -*- coding: utf-8 -*-
from importlib import import_module

class EngineConf(object):
    bing = "bing"
    baidu = "baidu"
    so360 = "so360"
EngineNames = [EngineConf.bing, EngineConf.baidu, EngineConf.so360]

async def Search(engine, keywords, model='ereading.models.searcher.', pages=1):
    models = import_module('{}{}{}'.format(model, 'factory.', engine)) 
    rstInfo = await models.start(keywords, pages=pages)
    return rstInfo

if __name__ == '__main__':
    import asyncio
    import sys
    import os
    sys.path.append(os.path.abspath(os.path.abspath(__file__) + "/../../../.."))

    res = asyncio.get_event_loop().run_until_complete(
        #Search(class_name='baidu', keywords='intitle:元尊 小说 阅读', model=''))
        #Search(engine='bing', keywords='元尊 小说 阅读 最新章节', model=''))
        Search(engine='so360', keywords='元尊 小说 阅读 最新章节', model=''))
    print(res)
