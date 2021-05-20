# -*- coding: utf-8 -*-

# Define here the models for your scraped items
#
# See documentation in:
# https://doc.scrapy.org/en/latest/topics/items.html

import scrapy


class ErSpiderItem(scrapy.Item):
    # define the fields for your item here like:
    # name = scrapy.Field()
    index = scrapy.Field()
    category = scrapy.Field()
    subcategory = scrapy.Field()
    author = scrapy.Field()
    status = scrapy.Field()
    introduce = scrapy.Field()
    bname = scrapy.Field()
    bcover = scrapy.Field()
    bwordscnt = scrapy.Field()
    bhome_qidian = scrapy.Field()
    bcover_url = scrapy.Field()
    bcover_local = scrapy.Field()

