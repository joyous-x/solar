# coding=utf-8
import os
from fontTools import ttLib,ttx


class TtfHelper(object):
    @staticmethod
    def extract_ttc2ttf(ttc_path, ttf_dir, name_prefix=""):
        """
        extract ttf files from ttc and save to files.
        using windows font name and language prefer chinese.
        reference: [NameRecord](https://docs.microsoft.com/en-us/typography/opentype/spec/name)
            Platform IDs :
                3 = Windows, 1 = Macintosh
            Windows language IDs:
                Chinese	People’s Republic of China	0804
                Chinese	Hong Kong S.A.R.	0C04
                Chinese	Macao S.A.R.	1404
                Chinese	Singapore	1004
                Chinese	Taiwan	0404
            Name IDS:
                1. The name the user sees. Times New Roman
                2. The name of the style. Bold
        """
        ttx_type = ttx.guessFileType(ttc_path)
        if ttx_type.lower() != "ttc":
            raise TypeError("expect ttc got {}".format(ttx_type))
        ttc = ttLib.TTCollection(ttc_path)
        for ttf in ttc:
            font_family_name, font_style_name = "", ""
            font_name_dict = {}
            langID_expected = [2052, 3076, 1028, 1033, 2057]
            for n in ttf['name'].names:
                if n.platformID != 3 or (n.langID not in langID_expected):
                    continue
                if n.nameID != 1 and n.nameID != 2:
                    continue
                if n.langID not in font_name_dict:
                    font_name_dict[n.langID] = {}
                font_name_dict[n.langID][n.nameID] = str(n)
            for langID in langID_expected:
                if langID not in font_name_dict:
                    continue
                font_family_name = font_name_dict[langID][1] if 1 in font_name_dict[langID] else ""
                font_style_name = font_name_dict[langID][2] if 2 in font_name_dict[langID] else ""
                break
            if font_family_name:
                ttf.save("{}/{}{}{}.ttf".format(ttf_dir, name_prefix, font_family_name, "_{}".format(font_style_name) if len(font_style_name) > 0 else ""))
            else:
                raise LookupError("has no name")

    @staticmethod
    def check_cn(ttf_file):
        """
        检查 ttf 文件是否支持中文字符
        reference: https://cloud.tencent.com/developer/article/1576291
        """
        result = True
        try:
            font = ttLib.TTFont(ttf_file)
            unicode_map = font['cmap'].tables[0].ttFont.getBestCmap()
            glyf_map = font['glyf']
            for word in '一二龍三四茧':
                result &= ord(word) in unicode_map
                if result:
                    result &= (unicode_map[ord(word)] in glyf_map and len(glyf_map[unicode_map[ord(word)]].getCoordinates(0)[0]) > 0)
        except ttLib.TTLibError as e:
            result = None
            print("check_cn : {} get error: {}".format(ttf_file, e))
        return result


if __name__ == "__main__":
    fonts_dir = "D:\\Workspace\\datasets\\font_recognize\\fonts"
    for file in os.listdir(fonts_dir):
        try:
            TtfHelper.extract_ttc2ttf(os.path.join(fonts_dir, file), fonts_dir, name_prefix=file.split(".")[0]+"_")
        except Exception as e:
            print(e)
