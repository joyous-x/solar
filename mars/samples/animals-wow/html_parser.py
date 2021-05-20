# coding=utf8
import re 
import os

# 以 动物叫声 进行google搜索得到
#    : https://www.google.com/search?q=%E5%8A%A8%E7%89%A9%E5%8F%AB%E5%A3%B0&oq=%E5%8A%A8%E7%89%A9%E5%8F%AB%E5%A3%B0&aqs=chrome..69i57.6441j0j7&sourceid=chrome&ie=UTF-8
# 以此脚本分析得到 img\name\wow 列表
# 以以下脚本得到具体内容：
#    : cat a.txt | awk '{print($1)}' | xargs -I {} wget {}
#    : cat a.txt | awk '{print($3)}' | xargs -I {} wget {}

def ParseHtml():
    with open("/home/jiao/nrs/xxxxxxx/a.html") as f:
        data = f.read()
        gs = re.findall('<div class=".*?" jscontroller=".*?" data-mid=".*?" .*?"><img src="(.*?)" alt=".*?">.*?</div></div><div class=".*?">(.*?)</div><audio .*? src="(.*?)"></audio></div>', data)
        for i in gs:
            print("https:%s %s https:%s" % (i[0], i[1], i[2]))
        print(len(gs))

def GenJson():
    data = dict()
    with open("./a.txt") as f:
        lines = f.readlines()
        for line in lines:
            line = line.strip()
            fields = line.split()
            if len(fields) < 2:
                continue
            enname = fields[0].split('/')[-1]
            enname = enname.split('.')[0]
            data[enname] = fields[1]
    for k,v in data.iteritems():
        print("{ name:'%s', alias:'', name_en:'%s', wow:'../resources/images/%s.png', img:'../resources/sounds/%s.mp3' }," % (v, k, k, k))

GenJson()
