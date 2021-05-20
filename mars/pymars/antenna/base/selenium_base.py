# coding=utf-8

import re
import os
import sys
import time
import requests
import logging
from urllib import parse
from selenium import webdriver
from selenium.webdriver.support.wait import WebDriverWait

class SeleniumBase(object):
    def __init__(self, driver_type, proxy=None, window=(1920, 3000)):
        """
            proxy : (type, ip, port), eg: (socks5, 101.32.14.3, 9094)
        """
        if driver_type == "chrome":
            chrome_options = self.default_chrome_options(proxy=proxy, window=window)
            # 创建一个driver,进行后面的请求页面等操作，executable_path指定本机中chromedriver.exe的位置
            self.driver = webdriver.Chrome(chrome_options=chrome_options,
                                           executable_path="D:/Workspace/chrome_driver/chromedriver.exe")
        else:
            self.driver = None

    def __del__(self):
        self.driver.close()

    def print(self, msg):
        print(msg)

    def default_chrome_options(self, proxy=None, headless=False, bin_location=None, window=(1920, 3000)):
        chrome_options = webdriver.ChromeOptions()
        # 一定要注意, add_argument 的 = 两边不能有空格
        if None != proxy:
            chrome_options.add_argument("--proxy-server=%s://%s:%s" % (proxy[0], proxy[1], proxy[2]))
        if None != window:
            # self.driver.set_window_size(win_size[0], win_size[1])
            chrome_options.add_argument('window-size=%dx%d' % (window[0], window[1]))  # 指定浏览器分辨率
        if headless:
            chrome_options.add_argument('--headless')  # 浏览器不提供可视化页面. linux下如果系统不支持可视化不加这条会启动失败
        if bin_location:
            # 手动指定本机电脑使用的浏览器位置, eg. "C:\Program Files\Google\Chrome\Application\chrome.exe"
            chrome_options.binary_location = bin_location
        chrome_options.add_argument('--no-sandbox')  # 解决DevToolsActivePort文件不存在的报错
        chrome_options.add_argument('--disable-gpu')  # 谷歌文档提到需要加上这个属性来规避bug
        chrome_options.add_argument('--hide-scrollbars')  # 隐藏滚动条, 应对一些特殊页面
        chrome_options.add_argument('blink-settings=imagesEnabled=true')  # 不加载图片, 提升速度
        return chrome_options

    def get_page(self, page_url, load_timeout_sec=None):
        try:
            if load_timeout_sec != None:
                self.driver.set_page_load_timeout(load_timeout_sec)
            self.driver.get(page_url)
        except Exception as e:
            logging.error(e)

    def scroll_page_down(self, scroll_to=0, interval=1):
        """
            args: scroll_to, 0 - bottom, int - postion
        """
        scripts = "window.scrollTo(0, document.body.scrollHeight);var lenOfPage=document.body.scrollHeight;return lenOfPage;"
        len_of_page, last_postion = 0, 0
        while scroll_to == 0 or len_of_page < scroll_to:
            len_of_page = self.driver.execute_script(scripts)
            time.sleep(interval)
            len_of_page = self.driver.execute_script("var lenOfPage=document.body.scrollHeight;return lenOfPage;")
            if last_postion == len_of_page:
                is_bottom = True
                break
            last_postion = len_of_page
        return len_of_page

    def save_net_file(self, file_url, file_name, file_dir="./res", max_try=3):
        file_path = "%s/%s" % (file_dir, file_name)
        for i in range(max_try):
            try:
                r = requests.get(file_url, timeout=10)
                with open(file_path, "wb") as f:
                    f.write(r.content)
                break
            except Exception as e:
                self.print("!!! error: %s %s" % (file_url, e))
