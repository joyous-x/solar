# -*- coding: utf-8 -*-

import re
import time
from collections import OrderedDict
from operator import itemgetter
from bs4 import BeautifulSoup
from urllib.parse import urlparse
from ereading.common import cnum2digit
from ereading.common.logger import slogger as logger
from ereading.models.searchrules import ConfigBookSearch
from ereading.models.helper import HtmlGetter, HtmlGetterByRequests
from ereading.models.decorator import cache_handler

@cache_handler(ttl=300)
async def RequestRawChapterList(url):
    html = await HtmlGetter(url=url)
    if not html:
        html = HtmlGetterByRequests(url=url)
        if not html:
            return None
    return html

async def RequestChapterContent(url, host):
    html = await HtmlGetter(url=url)
    if not html:
        html = HtmlGetterByRequests(url=url)
        if not html:
            return None
    soup = BeautifulSoup(html, 'html5lib')
    labels = ConfigBookSearch.rules[host].contentLabels
    if labels.get('id', None):
        content = soup.find_all(id=labels['id'])
    elif labels.get('class', None):
        content = soup.find_all(class_=labels['class'])
    else:
        content = soup.find_all(labels.get('tag'))
    if not content:
        return None
    # 提取出真正的章节标题
    title_reg = r'(第?\s*[一二两三四五六七八九十○零百千万亿0-9１２３４５６７８９０]{1,6}\s*[章回卷节折篇幕集]\s*.*?)[_,-]'
    title = soup.title.string
    extract_title = re.findall(title_reg, title, re.I)
    if extract_title:
        title = extract_title[0]
    else:
        title = soup.select('h1')[0].get_text()
    if not title:
        title = soup.title.string
    next_chapter = parsePreNextChapter(cpListUrl=url, cpHtmlContent=str(soup))
    content = [str(i) for i in content]
    return {'title': title, 'content': str(''.join(content)), 'next_chapter': next_chapter}

def parsePreNextChapter(cpListUrl, cpHtmlContent):
    next_chapter = OrderedDict()
    try:
        # 参考https://greasyfork.org/zh-CN/scripts/292-my-novel-reader
        next_reg = r'(<a\s+.*?>.*[第上前下后][一]?[0-9]{0,6}?[页张个篇章节步].*?</a>)'
        judge_reg = r'[第上前下后][一]?[0-9]{0,6}?[页张个篇章节步]'
        # 这里同样需要利用bs再次解析
        next_res = re.findall(next_reg, cpHtmlContent.replace('<<', '').replace('>>', ''), re.I)
        next_res_soup = BeautifulSoup('\n'.join(next_res), 'html5lib')
        for link in next_res_soup.find_all('a'):
            text = (link.text or '').replace(' ', '')
            if novels_list(text):
                is_next = re.search(judge_reg, text)
                if is_next:
                    # url = urljoin(cpListUrl, link.get('href')) or ''
                    next_chapter[text[:5]] = link.get('href')
        return next_chapter
    except Exception as e:
        return next_chapter

def extractChapterNameFromTitle(title):
    # cpindx_reg = r'^第?\s*([一二两三四五六七八九十○零百千万亿0-9１２３４５６７８９０]{1,6}\s*)[章回卷节折篇幕集]'
    cpname, cpid = title, ""
    firReStr = r'^第?\s*(.{1,7}?\s*)[章回卷节折篇幕集](\s*.*?)$'
    results = re.findall(firReStr, title, re.I)
    if len(results) != 1 or re.search(r'.*?[章回卷节折篇幕集]', results[0][0], re.I):
        logger.error("===> extractChapterName error: title=%s results=%s" % (title, results))
        if len(results) != 1:
            return cpid, cpname
    cpidRaw, cpnameRaw = results[0][0].strip(), results[0][1].strip()
    cpidRaw = cpidRaw.replace("第章回卷节折篇幕集 ?？", "")
    cpidInt = cnum2digit.TranStr2Int(cpidRaw)
    cpid, cpname = str(cpidInt) if None != cpidInt else cpidRaw, cpnameRaw.strip(" ?？")
    return cpid, cpname

def beforeExtractChapterList(host, cpListUrl, rawCpHtml):
    soup = BeautifulSoup(rawCpHtml, 'html5lib')
    labels = ConfigBookSearch.rules[host].cpLabels
    if labels.get('id', None):
        content = soup.find_all(id=labels['id'])
    elif labels.get('class', None):
        content = soup.find_all(class_=labels['class'])
    else:
        content = soup.find_all(labels.get('tag'))
    return str(content).replace('style', '') if content else None

def extractChapterList(cpListUrl, cpHtmlContent, orderReverse=False):
    # 参考https://greasyfork.org/zh-CN/scripts/292-my-novel-reader
    cplist_reg = r'(<a\s+.*?>.*第?\s*[一二两三四五六七八九十○零百千万亿0-9１２３４５６７８９０]{1,6}\s*[章回卷节折篇幕集].*?</a>)'
    # 这里不能保证获取的章节分得很清楚，但能保证这一串str是章节目录。可以利用bs安心提取a
    cplist_res = re.findall(cplist_reg, str(cpHtmlContent), re.I)
    cplist_res_soup = BeautifulSoup('\n'.join(cplist_res), 'html5lib')
    all_chapters = []
    all_chapters_set = dict()
    all_chapters_order_asc = True
    for i, link in enumerate(cplist_res_soup.find_all('a')):
        each_data = {}
        url, title = (link.get('href') or '').strip(), (link.text or '').strip()
        cpid, cpname = extractChapterNameFromTitle(title)

        if url in all_chapters_set:
            all_chapters[all_chapters_set[url]]['id'] = i
            continue
        else:
            all_chapters_set[url] = len(all_chapters)

        each_data['id'] = i
        each_data['cpid'] = cpid
        each_data['cpurl'] = url
        each_data['cpname'] = cpname
        each_data['index'] = int(urlparse(url).path.split('.')[0].split('/')[-1])
        all_chapters.append(each_data)

    # 不再使用 index 排序，因为有些页面不这样做，如：https://www.biqugex.com/book_67257/
    chapters_sorted = sorted(all_chapters, key=itemgetter('id'))
    for i, v in enumerate(chapters_sorted):
        if i < 1 or not v['cpid'] or len(v['cpid']) < 1 or not chapters_sorted[i-1]['cpid'] or len(chapters_sorted[i-1]['cpid']) < 1:
            continue
        if chapters_sorted[i-1]['cpid'] < v['cpid']:
            all_chapters_order_asc = True
        else:
            all_chapters_order_asc = False
        break
    if orderReverse & all_chapters_order_asc:
            chapters_sorted = sorted(all_chapters, reverse=True, key=itemgetter('id'))

    return chapters_sorted

def novels_list(text):
    rm_list = ['后一个', '天上掉下个']
    for i in rm_list:
        if i in text:
            return False
        else:
            continue
    return True

def extractBookIntros(cpListUrl, cpHtmlContent):
    result = {}
    lastUpdateTSReg = r'>[\s.]*[最后更新时间]+[\s：:]*([\s：:0-9\-]*)[\s.]*<' # r'>(\s*.*最后更新\s*[0-9：:\s]+?\s*<)'
    lastUpdateTSRes = re.findall(lastUpdateTSReg, str(cpHtmlContent), re.I)
    for tsStr in lastUpdateTSRes:
        tsStr = tsStr.strip().replace('-','').replace('：', ':')
        try:
            tsTime = time.strptime(tsStr,'%Y%m%d %H:%M:%S')
            result["ts_update"] = time.strftime('%Y-%m-%d %H:%M:%S', tsTime)
        except Exception as e:
            logger.error("===> extractBookIntros error: parse lastUpdateTS=%s" % (tsStr))
    return result