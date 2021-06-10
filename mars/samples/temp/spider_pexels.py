# coding=utf-8

from selenium.webdriver.support.wait import WebDriverWait
# import using_mylib
from antenna.base.selenium_base import *

class Spider_PexelsCom(SeleniumBase):
    def __init__(self, driver_type, win_size = (1920,1080)):
        SeleniumBase.__init__(self, driver_type, None)
        self.cur_scroll_pos = 0
        self.all_items = set()
        self._handle_done(save=False)

    def __del__(self):
        self._handle_done(save=True)
        SeleniumBase.__del__(self)

    def _handle_done(self, path="./.spider.done", save=False):
        lines = []
        if not os.path.exists(path):
            return
        with open(path, "w+" if save else "r") as f:
            if save:
                for i in self.all_items:
                    lines.append("%s\n" % i)
                f.writelines(lines)
            else:
                lines = f.readlines()
                for l in lines:
                    self.all_items.add(l.strip())

    def save_imgurl(self, img_url, dir="./img", max_try=3):
        is_match = re.match("http(s?)://.*?\.jp.?g(\??.*)", img_url)
        if not is_match:
            return
        pos = img_url.find("?")
        if pos > 0:
            url_tmp = img_url[:pos]
            file_name = url_tmp.split('/')[-1]
        else:
            file_name = img_url.split('/')[-1]
        self.save_net_file(img_url, file_name, dir)

    def is_need_handle(self, media_id):
        if media_id in self.all_items:
            return False
        self.all_items.add(media_id)
        return True

    def _handle_item(self, items, func_need_handle=None):
        new_items = []
        for item in items:
            articles = item.find_elements_by_class_name("photo-item")
            if len(articles) < 1:
                continue
            article = articles[0]
            media_id = article.get_attribute("data-photo-modal-medium-id")
            if None == media_id:
                continue
            if None != func_need_handle and not func_need_handle(media_id):
                continue
            new_items.append(media_id)
            photo_links = article.find_elements_by_class_name("js-photo-link")
            if len(photo_links) > 0:
                items = photo_links[0].find_elements_by_class_name("photo-item__img")
                if len(items) > 0:
                    img_src = items[0].get_attribute("data-big-src")
                    img_src = self.update_url(img_src.strip("'"))
                    self.save_imgurl(img_src, dir="./img")
                    print("--->", img_src)
        return new_items

    def get_object_list(self):
        grids = self.driver.find_elements_by_class_name("search__grid")
        if len(grids) < 1:
            return
        photos = grids[0].find_elements_by_class_name("photos")
        if len(photos) < 1:
            return
        photo_cols = photos[0].find_elements_by_class_name("photos__column")
        if len(photo_cols) < 1:
            return
        for column in photo_cols:
            items = column.find_elements_by_class_name("hide-featured-badge")
            new_photos = self._handle_item(items, self.is_need_handle)

    def wait_page_loaded(self):
        def is_page_loaded(driver):
            return True
        WebDriverWait(self.driver, 5, 0.5).until(is_page_loaded)

    def update_url(self, url):
        # "https://images.pexels.com/photos/5560026/pexels-photo-5560026.jpeg?auto=compress&cs=tinysrgb&dpr=1&w=500"
        parseResult = parse.urlparse(url)
        params = parse.parse_qs(parseResult.query)
        w = int(params['w'][0]) if 'w' in params else 0
        h = int(params['h'][0]) if 'h' in params else 0
        if w == 0:
            scale = 750 / h if h < 750 else 1
        elif h == 0:
            scale = 750 / w if w < 750 else 1
        else:
            min_v = min(w, h)
            scale = 750 / min_v if min_v < 750 else 1
        w = int(w * scale / 5) * 5
        h = int(h * scale / 5) * 5
        if w != 0:
            params['w'][0] = str(w)
        if h != 0:
            params['h'][0] = str(w)
        for k, v in params.items():
            if len(v) == 1:
                params[k] = v[0]
        url_datas = [parseResult.scheme, parseResult.netloc, parseResult.path, parseResult.params,
                     parse.urlencode(params), parseResult.fragment]
        return parse.urlunparse(url_datas)

if __name__ == "__main__":
    try:
        spider = Spider_PexelsCom("chrome")
        spider.get_page("https://www.pexels.com/zh-cn/search/sky/")
        spider.wait_page_loaded()
        while True:
            spider.get_object_list()
            spider._handle_done(save=True)
            spider.scroll_page_down(scroll_to=0)
    finally:
        pass
