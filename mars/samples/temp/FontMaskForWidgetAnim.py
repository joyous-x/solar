# coding=utf-8
import os

from fontTools import ttLib
from fontTools.pens.ttGlyphPen import TTGlyphPen
from fontTools.ttLib.tables._g_l_y_f import Glyph

import xml.etree.ElementTree as ET

class FontHandler(object):
    def __init__(self):
        pass

    @staticmethod
    def split_svg_by_color(svg_file):
        svg_dir = os.path.dirname(svg_file)

        tree = ET.parse(svg_file)
        root = tree.getroot()

        # 获取所有的路径元素
        paths = root.findall(".//{http://www.w3.org/2000/svg}path")

        # 将路径元素按颜色分组
        color_groups = {}
        for path in paths:
            # 获取路径元素的颜色属性
            color = path.get("fill")
            if color not in color_groups:
                color_groups[color] = []
            color_groups[color].append(path)

        # 为每个颜色创建一个新的 SVG 文件
        for i, (color, paths) in enumerate(color_groups.items()):
            # 创建一个新的 SVG 根元素
            new_root = ET.Element("svg")
            new_root.set("xmlns", "http://www.w3.org/2000/svg")
            new_root.set("version", "1.1")

            # 将路径元素添加到新的 SVG 文件中
            for path in paths:
                new_root.append(path)

            # 保存新的 SVG 文件
            new_file_name = f"{svg_dir}/{i + 1}_{color}.svg"
            ET.ElementTree(new_root).write(new_file_name)

    @staticmethod
    def read_font(path):
        """
            码点值(unicode)、名字值(glyphName)

            font.getGlyphOrder() # 返回一个字形名称列表，以其在文件中的顺序排序
            font.getGlyphNames() # 返回一个字形名称列表，以字母顺序排序
            font.getBestCmap() # 返回一个字形ID为键、字形名称为值的字典
            font.getReverseGlyphMap() # 返回一个字形名称为键、字形ID为值的字典
            font.getGlyphName(10000) # 输入字形ID返回字形名称
            font.getGlyphID("uni70E0") # 输入字形名称返回字形ID
            font.getGlyphSet() # 返回一个_TTGlyphSet对象，包含字形轮廓数据

            font.keys()
                表名中包含‘glyf‘，所以simsun.ttf是一个使用TrueType轮廓的字体文件。
                表名中没有'glyf'而存在'CFF '，后者是存储Postscript信息的表格。
            对于TrueType Collection文件则可以使用如下方法读取，返回一个TTFont实例的列表
                from fontTools.ttLib.ttCollection import TTCollection
                collection = TTCollection("Resources/simsun.ttc")
                print(list(collection))

                https://zhuanlan.zhihu.com/p/491175407
                https://www.cnblogs.com/yc0806/p/15842529.html
        """
        f = ttLib.TTFont(path)
        print("keys:\t", f.keys())
        for i in f["name"].names:
            print(i)
        print("GlyphNames:\t", f.getGlyphNames())

    @staticmethod
    def update_glyph(raw_path, rst_path, data_chars=["B", "W", "W", "W", "W", "W"]):
        """
        编辑、替换字体文件的字形
        """
        labels = ["zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine"]

        target_names = []
        for i in range(0, 6):
            target_names.extend(["%s_%s" % (labels[i], l) for l in labels])

        font_src = ttLib.TTFont(raw_path)
        font_dst = ttLib.TTFont(rst_path)

        glyph_src = font_src.getGlyphSet()
        glyph_dst = font_dst["glyf"]

        for i in range(0, len(target_names)):
            src_key = data_chars[i % len(data_chars)]
            sg = glyph_src[src_key]
            pen = TTGlyphPen(None)
            sg.draw(pen)

            dst_key = target_names[i]
            glyph_dst[dst_key] = Glyph()
            glyph_dst[dst_key] = pen.glyph()
            # glyph_dst[dst_key].program = sg._glyph.program
            font_dst["hmtx"].metrics[dst_key] = font_src["hmtx"].metrics[src_key]
        font_dst.save("%s.updated.ttf" % rst_path)


if __name__ == "__main__":
    dirPath = "/Users/jiao/Downloads/font_mask/"
    FontHandler.read_font(dirPath + "src_bw.ttf")
    FontHandler.update_glyph(dirPath + "src_bw.ttf", dirPath + "oia_glyph_locker_6.ttf")
    #FontHandler.split_svg_by_color("/Users/jiao/Downloads/ColorfulWidget/my/1.svg")