# -*- coding: utf-8 -*-
import scrapy
from fontTools.ttLib import TTFont
from io import BytesIO
import requests
from er_spider.items import ErSpiderItem
from er_spider.common.logger import logger


class QdAntiSpider(object):
    def __init__(self, ttfURL):
        self.cmap = QdAntiSpider.getTTF(ttfURL)

    @staticmethod
    def getTTF(ttfURL):
        response = requests.get(ttfURL)
        font = TTFont(BytesIO(response.content))
        cmap = font.getBestCmap()
        font.close()
        return cmap

    def decode(self, values):
        WORD_MAP = {'zero':'0','one':'1','two':'2','three':'3','four':'4','five':'5','six':'6','seven':'7','eight':'8','nine':'9','period':'.'}
        words=''
        try:
            for value in values.split(';'):
                value = value[2:]
                if len(value) < 1:
                    break
                key = self.cmap[int(value)]
                words += WORD_MAP[key]
        except Exception as e:
            logger.error("QdAntiSpider.decode error: raw=%s exception:" % (values, e))
        return words


def makeQDBookListStartUrls():
    """
    爬虫启动页面
    """
    start_urls = ['https://www.qidian.com/all?page=1', # 人气排序
        "https://www.qidian.com/all?orderId=11&page=1&style=1&pageSize=20&siteid=1&pubflag=0&hiddenField=0", # 总收藏
        "https://www.qidian.com/all?orderId=3&page=1&style=1&pageSize=20&siteid=1&pubflag=0&hiddenField=0", # 总字数
        "https://www.qidian.com/all?size=-1&sign=-1&tag=-1&chanId=-1&subCateId=-1&orderId=9&update=-1&page=1&month=-1&style=1&action=-1&vip=-1", # 周推荐票
        "https://www.qidian.com/all?size=-1&sign=-1&tag=-1&chanId=-1&subCateId=-1&orderId=10&update=-1&page=1&month=-1&style=1&action=-1&vip=-1", # 月推荐票
        "https://www.qidian.com/all?size=-1&sign=-1&tag=-1&chanId=-1&subCateId=-1&orderId=2&update=-1&page=1&month=-1&style=1&action=-1&vip=-1", # 总推荐票
    ]
    boyPrefix = "https://www.qidian.com/all?"
    mmPrefix  = "https://www.qidian.com/mm/all?"
    mm_start_urls = [i.replace(boyPrefix, mmPrefix) for i in start_urls]
    return start_urls + mm_start_urls


class QidianBlistSpider(scrapy.Spider):
    name = 'qidian_blist'
    allowed_domains = ['qidian.com']
    start_urls = makeQDBookListStartUrls()
    maxPagesPerStarturl = 10

    def decodeQiDianTTF(self, ttfURL, encodeStr):
        if len(ttfURL):
            qdAntiDecoder = QdAntiSpider(ttfURL)
            return qdAntiDecoder.decode(encodeStr)
        return ""

    def parse(self, response):
        if response.status != 200:
            return
        # 为了处理html实体(qd_anti_spider 字体反爬虫)
        selector = scrapy.Selector(text=response.text.replace('&#', '||'), type="html")
        items = selector.xpath('//div[@class="all-book-list"]')
        assert(len(items) == 1)
        lis = items.css('li')
        for li in lis:
            index = li.css('::attr(data-rid)').extract_first()
            novel_url = li.css('div.book-img-box>a::attr(href)').extract_first()
            novel_cover = li.css('div.book-img-box>a>img::attr(src)').extract_first()
            novel_name = li.css('div.book-mid-info>h4>a::text').extract_first()
            author = li.css('div.book-mid-info>p.author>a.name::text').extract_first()
            category = li.css('div.book-mid-info>p.author>a:nth-child(4)::text').extract_first()
            subcategory = li.css('div.book-mid-info>p.author>a.go-sub-type::text').extract_first()
            status = li.css('div.book-mid-info>p.author>span::text').extract_first()
            introduce = li.css('div.book-mid-info>p.intro::text').extract_first().strip()

            countEncode = li.css('div.book-mid-info>p.update>span>span::text').extract_first()
            countSuffix = li.css('div.book-mid-info>p.update>span::text').extract_first()
            ttfURL = li.css('div.book-mid-info>p.update>span>style').re_first(r'woff.*?url.*?\'(.+?)\'.*?truetype')
            if ttfURL and len(ttfURL):
                countDecode = self.decodeQiDianTTF(ttfURL, countEncode)
                novel_wordscnt = countDecode + countSuffix if len(countDecode) else ""

            eritem = ErSpiderItem()
            eritem["index"] = index
            eritem["bname"] = novel_name
            eritem["author"] = author
            eritem["category"] = category
            eritem["subcategory"] = subcategory
            eritem["status"] = status
            eritem["introduce"] = introduce
            eritem["bwordscnt"] = novel_wordscnt
            eritem["bhome_qidian"] = 'https:' + novel_url
            eritem["bcover_url"] = 'https:' + novel_cover
            yield eritem

        curPageIndex = selector.xpath('//li[@class="lbf-pagination-item"]/a[contains(@class, "lbf-pagination-current")]/text()').extract_first()
        nextPageUrl = selector.xpath('//li[@class="lbf-pagination-item"]/a[@data-page="%s"]/@href' % (int(curPageIndex)+1)).extract_first()
        if nextPageUrl == None:
            logger.debug("QiDianSpider end: curPageIndex=%s" % curPageIndex)
        elif int(curPageIndex) > self.maxPagesPerStarturl:
                logger.debug("QiDianSpider has got %d pages" % self.maxPagesPerStarturl)
        else:
            nextPageUrl = 'https:' + nextPageUrl
            logger.debug("QiDianSpider next page: %s" % nextPageUrl)
            yield scrapy.Request(nextPageUrl, callback=self.parse)
        # print(response.body.decode(response.encoding))