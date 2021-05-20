# coding=utf-8

from selenium import webdriver
from selenium.webdriver.support.wait import WebDriverWait
import re
import sys
import requests
import logging

# https://www.pexels.com/zh-cn/search/%E5%A4%A9%E7%A9%BA/
# https://pixabay.com/zh/photos/search/%E5%A4%A9%E7%A9%BA/?pagi=4
#   john_mytest@yeah.net
logging.basicConfig(stream=sys.stdout, level=logging.DEBUG, format="%(asctime)s %(levelname)s %(filename)s[%(lineno)d] - %(message)s")

def has_docs(driver):
    element = driver.find_elements_by_class_name("search_results")
    if len(element) > 0:
        return True
    return False

def handle_imgurl(img_url, dir = "../img"):
    is_match = re.match("http(s?)://.*?\.jpg", img_url)
    if not is_match:
        return
    file_name = img_url.split('/')[-1]
    file_path = "%s/%s"%(dir, file_name)
    for i in range(3):
        try:
            r = requests.get(img_url, timeout=10)
            with open(file_path, "wb") as f:
                f.write(r.content)
            break
        except Exception as e:
            print("!!! error: " , img_url, e)

def get_obj_list(driver, thumbnail=False):
    url_list = []
    thumbnail_list = []
    elements = driver.find_elements_by_class_name("search_results")
    if len(elements) < 1:
        return
    items = elements[0].find_elements_by_class_name("item")
    for item in items:
        item_type = item.get_attribute("itemtype").strip("'")
        if item_type != "schema.org/ImageObject":
            continue
        # herf
        herfs = item.find_elements_by_tag_name("a")
        for herf in herfs:
            forward = herf.get_attribute("href")
            if None == forward or "search" in forward:
                continue
            url_list.append(forward.strip("'"))
        if thumbnail == False:
            continue
        # download 缩略图
        metas = item.find_elements_by_tag_name("meta")
        for meta in metas:
            meta_type = meta.get_attribute("itemprop")
            meta_type.strip("'")
            if "contentUrl" == meta_type:
                thumbnail_list.append(meta.get_attribute("content"))
    return url_list

def get_page(driver, page):
    try:
        driver.get(page)
    except Exception as e:
        logging.error(e)

def forward_detail_page(driver, pages):
    for page in pages:
        get_page(driver, page)

        # # 会出人机交互
        # btn_download_pre = driver.find_elements_by_class_name("download_menu")
        # if len(btn_download_pre) > 0:
        #     btn_download_pre[0].click()
        #     btn_download = driver.find_elements_by_class_name("dl_btn")
        #     if len(btn_download) > 0:
        #         btn_download[0].click()
        # # 稍微低的分辨率
        medias = driver.find_element_by_id("media_container")
        if None != medias:
            imgs = medias.find_elements_by_tag_name("img")
            if len(imgs) > 0:
                img_src = imgs[0].get_attribute("src")
                handle_imgurl(img_src.strip("'"))

        driver.back()

if __name__ == "__main__":
    chromeOptions = webdriver.ChromeOptions()
    chromeOptions.binary_location = "C:\Program Files\Google\Chrome\Application\chrome.exe"
    chromeOptions.add_argument("--proxy-server=socks5://%s:%s" % ("101.32.14.3", "9094"))
    driver = webdriver.Chrome(chrome_options=chromeOptions)
    try:
        for page in range(281, 381):
            page_url = "https://pixabay.com/zh/photos/search/%E5%A4%A9%E7%A9%BA/?pagi=" + str(page)
            for i in range(3):
                try:
                    get_page(driver, page_url)
                    WebDriverWait(driver, 5, 0.5).until(has_docs)
                    forward_detail_page(driver, get_obj_list(driver))
                    break
                except Exception as e:
                    print(page_url, e)
    finally:
        driver.close()
