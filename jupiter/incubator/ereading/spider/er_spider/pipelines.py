# -*- coding: utf-8 -*-

# Define your item pipelines here
#
# Don't forget to add your pipeline to the ITEM_PIPELINES setting
# See: https://doc.scrapy.org/en/latest/topics/item-pipeline.html

import scrapy
from er_spider.common.logger import logger
from scrapy.exceptions import DropItem
from scrapy.pipelines.images import ImagesPipeline
import re 

class ErSpiderPipeline(object):
    def __init__(self):
        pass
    def __del__(self):
        pass
    def process_item(self, item, spider):
        return item

class ErImgsPipeline(ImagesPipeline):
    default_headers = {
        'accept': 'image/webp,image/*,*/*;q=0.8',
        'accept-encoding': 'gzip, deflate, sdch, br',
        'accept-language': 'zh-CN,zh;q=0.8,en;q=0.6',
        'cookie': 'bid=yQdC/AzTaCw',
        'referer': 'https://www.douban.com/photos/photo/2370443040/',
        'user-agent': 'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_11_4) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/52.0.2743.116 Safari/537.36',
    }

    def file_path(self, request, response=None, info=None):
        image_guid = request.url.split('/')[-1]
        name = request.meta['name']
        cate = request.meta['type']
        # 过滤windows字符串
        name = re.sub(r'[？\\*|“<>:/]', '', name)
        cate = re.sub(r'[？\\*|“<>:/]', '', cate)
        # 重构文件名
        filename = u'{0}/{1}.jpeg'.format(cate, name)
        return filename

    def get_media_requests(self, item, info):
        yield scrapy.Request(item['bcover_url'], headers=self.default_headers, meta={'name': item['bname'], 'type': 'cover'})

    def item_completed(self, results, item, info):
        image_paths = [x['path'] for ok, x in results if ok]      # ok判断是否下载成功
        if not image_paths:
            raise DropItem("Item contains no images")
        item['bcover_local'] = image_paths[0]
        return item
