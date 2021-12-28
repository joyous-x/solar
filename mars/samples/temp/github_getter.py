# coding = utf-8
import os
import subprocess
import requests
import logging
import time
import hashlib
import random
from urllib import parse
from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.support.wait import WebDriverWait
from selenium.webdriver.support import expected_conditions
from selenium.webdriver.common.action_chains import ActionChains

"""
webdriver:
    https://sites.google.com/chromium.org/driver/
"""


class SeleniumBase(object):
    def __init__(self, driver_type, proxy=None, window=(1920, 3000)):
        """
            proxy : (typee, ip, port), eg: (socks5, 10.32.14.3, 9004)
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


# shell, haskell, scala,windows batch file,php,perl,lisp,ruby,rust,c-sharp, javascript, python
def git_url_getter(keyword, first_page_fmt="https://github.com/search?q=%s", max_pages=25):
    selenium = SeleniumBase("chrome")

    urls = {}
    next_page = first_page_fmt % keyword
    while len(urls) < max_pages:
        try:
            if isinstance(next_page, str):
                selenium.get_page(next_page)
                next_page = selenium.driver.find_element(By.CLASS_NAME, "next_page")
            else:
                next_page = selenium.driver.find_element(By.CLASS_NAME, "next_page")
                ActionChains(selenium.driver).move_to_element(next_page).click(next_page).perform()
                WebDriverWait(selenium.driver, 30, 0.5).until(
                    expected_conditions.presence_of_element_located((By.CLASS_NAME, "repo-list-item")))
        except Exception as e:
            print("get page error: " + str(e))
            break
        page_url = selenium.driver.current_url
        print("\t===> current_page : %s" % (page_url))

        element_id = 0
        while True:
            elements = selenium.driver.find_elements(By.CLASS_NAME, "repo-list-item")
            if elements is None:
                print("! current page don't find repos : %s" % (selenium.driver.current_url))
                break
            if element_id >= len(elements):
                break
            # forward
            try:
                print("\t forward : %d %s" % (element_id, page_url))
                time.sleep(0.5 + random.random())
                item = elements[element_id].find_element(By.CLASS_NAME, "v-align-middle")
                ActionChains(selenium.driver).move_to_element(item).click(item).perform()
            except Exception as e:
                element_id = element_id + 1
                print("get forward error: " + str(e))
                continue
            # save
            try:
                print("\t save : %d %s" % (element_id, page_url))
                element = WebDriverWait(selenium.driver, 30, 0.5).until(
                    expected_conditions.presence_of_element_located((By.TAG_NAME, "clipboard-copy")))
                git_url = element.get_attribute("value")
                if page_url not in urls:
                    urls[page_url] = []
                urls[page_url].append(git_url)
            except Exception as e:
                print("get content error: " + str(e))
            # back
            print("\t back : %d %s" % (element_id, page_url))
            time.sleep(0.5 + random.random())
            selenium.driver.back()
            try:
                WebDriverWait(selenium.driver, 30, 0.5).until(
                    expected_conditions.presence_of_element_located((By.CLASS_NAME, "repo-list")))
            except Exception as e:
                print("get back error: " + str(e))
            element_id = element_id + 1
    with open("./github_url_" + keyword, "a", encoding="utf-8") as f:  #
        lines = []
        for k, vs in urls.items():
            lines = lines + ["%s \t %s \n" % (k, i) for i in vs]
        f.writelines(lines)
    print("ending")


def git_url_clone(lines, catalog, dst_dir=".", max_count = 50):
    lines = lines[: max_count * 10]
    for i in range(len(lines)):
        line = lines[i].strip()
        page, url = line.split("\t")
        if not url.endswith(".git"):
            print("line (%s) is illegal" % (line))
            continue
        #url = url.replace("https://github.com/", "https://hub.fastgit.org/")
        index = url.rindex("/")
        name = url[index+1:] if index >= 0 else url
        name, ext = os.path.splitext(name)
        hasher = hashlib.md5()
        hasher.update(url.encode("utf-8"))
        cmd = "git clone %s \"%s\"" % (url, os.path.join(dst_dir, "%s_%s_%s" % (catalog, name, hasher.hexdigest())))
        rst = subprocess.Popen(cmd, stdout=subprocess.PIPE, shell=True) #stderr = subprocess.PIPE, ,
        out, err = rst.communicate()
        rst.wait()
        print(out.decode())
        print("%s has cloned：%d \n" % (url, i))


if __name__ == "__main__":
    getter = False
    if getter:
        git_url_getter("batch-files", first_page_fmt="https://github.com/search?q=%s", max_pages=50)
    else:
        with open("./gits_url/github_url_batch-files", "r") as f:
            lines = f.readlines()
            git_url_clone(lines, "bat", "d:/Workspace/tmp/filetype/gits/")
