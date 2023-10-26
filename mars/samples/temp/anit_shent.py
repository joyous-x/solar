# coding=utf8
import random
import time
import requests
import os
import json
from urllib.parse import urlparse


class UserInfo:
    def __init__(self, token="shentujun"):
        self.token = token


class ProdInfo:
    def __init__(self, title=None, share_title=None, type=None, bid=None, baseImage=None, image=None, bid_image=None, user=None, materials=None):
        self.title = title
        self.share_title = share_title
        self.type = type
        self.bid = bid
        self.image = image
        self.baseImage = baseImage
        self.bid_image = bid_image
        self.user = user
        self.materials = materials


class AntiShent(object):
    static_chars = "PXhw7UT1B0a9kQDKZsjIASmOezxYG4CHo5Jyfg2b8FLpEvRr3WtVnlqMidu6cN"

    def __init__(self):
        pass

    @staticmethod
    def encode(text):
        encode_chars = ""
        for i in range(len(text)):
            num0 = AntiShent.static_chars.find(text[i])
            if num0 < 0:
                code = text[i]
            else:
                code = AntiShent.static_chars[(num0 + 3) % 62]
            num1 = random.randrange(0, 62)
            num2 = random.randrange(0, 62)
            encode_chars += AntiShent.static_chars[num1] + code + AntiShent.static_chars[num2]
        return encode_chars

    @staticmethod
    def decode(sign):
        assert (len(sign) % 3 == 0)
        result = ""
        for i in range(0, len(sign), 3):
            code = sign[i + 1]
            num0 = AntiShent.static_chars.find(code)
            if num0 < 0:
                result += code
            else:
                num1 = ((num0 + 62) - 3) % 62
                result += AntiShent.static_chars[num1]
        return result

    @staticmethod
    def get_time_diff(global_data):
        local_ts = int(time.time())  # 10位数时间戳
        server_ts = local_ts
        if global_data is not None:
            if global_data.timediff is not None and global_data.timediff != 0:
                return global_data.timediff
            if global_data.server_timestamp is not None and global_data.server_timestamp > 0:
                server_ts = global_data.server_timestamp
        return server_ts - local_ts

    @staticmethod
    def make_sign(user_info: UserInfo, global_data):
        """
            wx3e01652976009f55/11/app-service.js =>  getToken: function getToken() {
        """
        cur_ts = time.time()
        time_diff = AntiShent.get_time_diff(global_data)
        timestamp = int(cur_ts)  # 10位数时间戳
        new_time = timestamp + time_diff
        rand = random.randint(100000, 999999)
        sign = AntiShent.encode(user_info.token + "###" + str(new_time) + "###" + str(rand))
        new_ts = int(round(cur_ts * 1000))
        print("---> make_sign: {} {}".format(new_ts, sign))
        return new_ts, sign

    @staticmethod
    def parse_sign(sign):
        content = AntiShent.decode(sign)
        subs = content.split("###")
        assert (len(subs) == 3)
        token = subs[0]
        new_time = subs[1]
        rand = subs[2]
        print("===> parse_sign: token={} new_time={} rand={}".format(token, new_time, rand))
        return token


class WalkShent(object):
    user_agent = 'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/98.0.4758.102 Safari/537.36 MicroMessenger/6.8.0(0x16080000) NetType/WIFI MiniProgramEnv/Mac MacWechat/WMPF XWEB/30817'
    referer = 'https://servicewechat.com/wx3e01652976009f55/11/page-frame.html'
    content_type = 'application/json'
    cid_map = {
        "爆款推荐": 0,
        "最近上新": -1,
        "励志壁纸": 13,
        "情侣姓氏": 12,
        "收款码": 4,
        "朋友圈背景": 6,
        "亲子全家福": 5,
        "情侣": 33,
        "女友壁纸": 7,
        "搞笑": 57,
        "情感壁纸": 56,
        "字母涂鸦": 55,
        "情侣头像": 54,
        "女生头像": 53,
        "男生头像": 52,
        "女生壁纸": 49,
        "男生壁纸": 48,
        "情侣背景": 44,
        "爱国": 43,
        "励志": 41,
        "治愈头像": 36,
        "情感头像": 35,
        "生肖头像": 34,
        "情感": 32,
        "孤独": 31,
        "励志头像": 29,
        "治愈壁纸": 28,
        "祝福壁纸": 25,
        "字母缩写": 24,
        "爱过头像": 20,
        "英雄合照": 15,
        "亲子": 11,
        "姓氏头像": 10,
        "新年": 19,
    }

    def __init__(self, user_info: UserInfo, openid, appid, output_dir):
        self.user_info = user_info
        self.appid = appid
        self.openid = openid
        self.output_dir = output_dir
        if not os.path.exists(output_dir):
            os.makedirs(output_dir)

    def getProductList(self, cid: int, page: int, limit: int = 10):
        filename = f"list_{cid}_{page}_{limit}.json"
        filepath = os.path.join(self.output_dir, filename)
        if os.path.exists(filepath):
            print(f"\t getProductList already: {filename}")
            return filepath
        timestamp, sign = AntiShent.make_sign(self.user_info, None)
        assert (len(sign) % 3 == 0)
        assert (len(str(timestamp)) == 13)
        url = "https://stapi.youpengw.com/miniwechat/v1/Diypic/getProductList"
        headers = {
            'Content-Type': WalkShent.content_type,
            'Referer': WalkShent.referer,
            'User-Agent': WalkShent.user_agent,
            'sign': sign
        }
        param = {
            "page": page,
            "limit": limit,
            "openid": self.openid,
            "timestamp": timestamp,
            "action": "share" if page == 1 and cid == 0 else "",
            "cid": cid,
            "version": 1
        }
        response = requests.post(url, json=param, headers=headers, verify=False)
        if response.status_code == 200:
            with open(filepath, "w") as file:
                file.write(response.text)
                print(f"\t getProductList ok: {filename}")
            return filepath
        else:
            print(f"\t getProductList fail: cid:{cid} page:{page} limit:{limit}")
            return None

    def getProInfo(self, prod_id: str):
        filename = f"info_{prod_id}.json"
        filepath = os.path.join(self.output_dir, filename)
        if os.path.exists(filepath):
            print(f"\t getProInfo already: {filename}")
            return filepath
        timestamp, sign = AntiShent.make_sign(self.user_info, None)
        assert (len(sign) % 3 == 0)
        assert (len(str(timestamp)) == 13)
        url = "https://stapi.youpengw.com/miniwechat/v1/Diypic/getProInfo"
        headers = {
            'Content-Type': WalkShent.content_type,
            'Referer': WalkShent.referer,
            'User-Agent': WalkShent.user_agent,
            'sign': sign
        }
        param = {
            "id": prod_id,
            "openid": self.openid,
            "appid": self.appid,
            "timestamp": timestamp
        }
        response = requests.post(url, json=param, headers=headers, verify=False)
        if response.status_code == 200:
            with open(filepath, "w") as file:
                file.write(response.text)
                print(f"\t getProInfo ok: {filename}")
            return filepath
        else:
            print(f"\t getProInfo fail: {prod_id}")
            return None

    def get_image(self, url: str):
        """
        下载图片，如果已经存在，则不再下载
        """
        if url is None or len(url) < 4 or url.startswith('data:image/png;'):
            return
        parsed_url = urlparse(url)
        scheme = parsed_url.scheme
        netloc = parsed_url.netloc
        path = parsed_url.path[1:] if parsed_url.path.startswith('/') else parsed_url.path
        fullpath = os.path.join(self.output_dir, netloc, path)
        if os.path.exists(fullpath):
            print(f"\t get_image already: {url}")
            return
        dirpath, filename = os.path.split(fullpath)
        filename = filename.lower()
        assert(filename.endswith(".png") or filename.endswith(".jpg") or filename.endswith(".jpeg"))
        if not os.path.exists(dirpath):
            os.makedirs(dirpath)
        response = requests.get(url)
        if response.status_code == 200:
            with open(fullpath, 'wb') as f:
                f.write(response.content)
            print(f"\t get_image ok: {url}")
        else:
            print(f"\t get_image fail: {url}")

    @staticmethod
    def load_json_file(filepath):
        with open(filepath, 'r') as file:
            content = file.read()
        try:
            data = json.loads(content)
        except Exception as e:
            print(filepath, str(e))
        return data

    @staticmethod
    def parse_pro_list(filepath):
        data = WalkShent.load_json_file(filepath)
        if data is None:
            return None
        if data["code"] != 1:
            print(f"\t parse_pro_list error(code error): {filepath} {data['code']} {data['msg']}")
            return None
        if 'data' not in data or 'list' not in data['data']:
            print(f"\t parse_pro_list error(no data>list): {filepath} {data['code']} {data['msg']}")
            return None
        return data['data']['list']

    @staticmethod
    def parse_pro_info(filepath):
        data = WalkShent.load_json_file(filepath)
        if data is None:
            return None
        if data["code"] != 1:
            print(f"\t parse_pro_info error(code error): {filepath} {data['code']} {data['msg']}")
            return None
        if 'data' not in data or 'materials' not in data['data']:
            print(f"\t parse_pro_info error(no data>materials): {filepath} {data['code']} {data['msg']}")
            return None
        prod_dict = data['data']
        prod_info = ProdInfo()
        prod_info.title = prod_dict['title']
        prod_info.share_title = prod_dict['share_title']
        prod_info.type = prod_dict['type'] if 'type' in prod_dict else None
        prod_info.bid = prod_dict['bid'] if 'bid' in prod_dict else None
        prod_info.image = prod_dict['image'] if 'image' in prod_dict else None
        prod_info.baseImage = prod_dict['baseImage'] if 'baseImage' in prod_dict else None
        prod_info.bid_image = prod_dict['bid_image'] if 'bid_image' in prod_dict else None
        prod_info.user = prod_dict['user'] if 'user' in prod_dict else None
        prod_info.materials = prod_dict['materials'] if 'materials' in prod_dict else None
        return prod_info

    def do_work(self, cid, page, limit):
        list_filepath = self.getProductList(cid, page, limit)
        if list_filepath is None:
            return None
        prod_list = WalkShent.parse_pro_list(list_filepath)
        if prod_list is None:
            return None
        for prod_item in prod_list:
            prod_id = prod_item['id'] if 'id' in prod_item else None
            prod_title = prod_item['title'] if 'title' in prod_item else None
            prod_image = prod_item['image'] if 'image' in prod_item else None
            if prod_id is None:
                print(f"===> do_work error(invalid prod_id): cid={cid} page={page} {prod_title} {list_filepath}")
                continue
            self.get_image(prod_image)
            info_filepath = self.getProInfo(prod_id)
            if info_filepath is None:
                continue
            prod_detail = WalkShent.parse_pro_info(info_filepath)
            if prod_detail is None:
                continue
            self.get_image(prod_detail.image)
            self.get_image(prod_detail.bid_image)
            for m in prod_detail.materials:
                if 'config' in m and 'image' in m['config']:
                    self.get_image(m['config']['image'])
        return len(prod_list)

    def workflow(self, limit=10):
        for title, cid in WalkShent.cid_map.items():
            for page in range(1, 100000):
                print(f"===> workflow start: {title} cid={cid} page={page}")
                count = self.do_work(cid, page, limit)
                if count is not None and count != limit:
                    break
                time.sleep(3)
            time.sleep(30)


if __name__ == "__main__":
    output = os.path.join(os.path.abspath(os.getcwd()), 'shent')
    openid = ' '
    sign_t = ' '
    token = AntiShent.parse_sign(sign_t)
    shenT = WalkShent(UserInfo(token), openid, "wx3e01652976009f55", output)
    shenT.workflow()
    #shenT.getProInfo("2606", sign, timestamp)
    #shenT.getProductList(0, 1, sign, timestamp)
