# coding=utf-8
import bs4
from antenna.utils.logger import mlog

"""
    source: https://art.alphacoders.com/
        important: https://api.alphacoders.com/content/get-download-link
    source: https://www.zdqx.com/
"""


class AlphacodersSpider(MsSpiderBase):
    def __init__(self):
        super(AlphacodersSpider, self).__init__(name="AlphacodersSpider")

    def start(self, url_home: str):
        self.start(AlphacoderItem(url_home, method="get", action="thumbnail"))


class AlphacodersMsDataHandler(MsItemHandlerBase):
    def __init__(self, spider: AlphacodersSpider):
        super(AlphacodersMsDataHandler, self).__init__()
        self.spider = spider

    def url_file2local(self, url, file_path, max_try=3):
        for i in range(max_try):
            try:
                r = requests.get(url)
                with open(file_path, "wb") as f:
                    f.write(r.content)
                    break
            except Exception as e:
                print("url_file2local error: ", url, e)

    def handle(self, item: Any):
        if item[0] == "page":
            self.spider.scheduler.push(
                AlphacoderItem("https://mobile.alphacoders.com{}".format(item[1]), method="get", action="detail"))
        else:
            url = item[1]["url"]
            pos = url.find("?")
            if pos > 0:
                url_tmp = url[:pos]
                file_name = url_tmp.split('/')[-1]
            else:
                file_name = url.split('/')[-1]
            self.url_file2local(url, "./img/{}".format(file_name))
        return


class AlphacodersPageParser(MsPageParserBase):
    def __init__(self):
        super(AlphacodersPageParser, self).__init__()

    def parse(self, soup: bs4.BeautifulSoup, meta: dict()):
        action = meta["action"] if meta is not None else ""
        if action == "thumbnail":
            return self._parse_page_thumbnail(soup)
        elif action == "detail":
            return self._parse_page_detail(soup)

    def _parse_page_thumbnail(self, soup: bs4.BeautifulSoup):
        classlist = soup.find_all(class_="container-masonry")
        if classlist == None:
            return None
        assert (len(classlist) == 1)
        container = classlist[0]
        items = container.select("div.item-element a")
        if items == None:
            return None
        rst = []
        for item in items:
            rst.append(("page", item.attrs['href']))
        return rst

    def _parse_page_detail(self, soup: bs4.BeautifulSoup):
        infos = soup.select("div.center div.info-container div.center a.download-button")
        if infos == None:
            return None
        data_id = infos[0].attrs["data-id"]
        imgs = soup.select("div.center a picture source")
        if imgs == None:
            return None
        img_info = imgs[0].parent.parent
        data_url = img_info.attrs["href"]
        return [("detail", {"id": data_id, "url": data_url})]


class AlphacoderItem(SimpleMsItem):
    def __init__(self, url, method="get", action=""):
        super(AlphacoderItem, self).__init__(url, method=method)
        self.set_meta("action", action)
        self.set_meta("method", method)

    def do(self, max_try=1) -> bs4.BeautifulSoup:
        response = None
        for i in range(max_try):
            try:
                if self.set_meta("method") == "get":
                    response = requests.get(self.item)
            except Exception as e:
                mlog.error("SimpleMsItem.do exception try({}): {}".format(i + 1, e))
        if response == None or response.status_code != 200:
            return None
        soup = bs4.BeautifulSoup(response.content, from_encoding=response.encoding, features="html")
        return soup


if __name__ == "__main__":
    wpaper = AlphacodersSpider()
    wpaper.start("https://mobile.alphacoders.com/by-category/3?page=1")
