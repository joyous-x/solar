import argparse
import os
import logging
import json


logging.basicConfig(level=logging.DEBUG)


def message(msg):
    logging.debug(msg)


class XcAssetsModifier(object):
    @staticmethod
    def parse_args():
        parser = argparse.ArgumentParser(description="")
        parser.add_argument('-p', '--path', help="the target directory path")
        parser.add_argument('-a', '--action', help="rename/tinypng")
        return parser.parse_args()

    @staticmethod
    def default_rename(filename, appearance, scale, prefer=None):
        scale = scale.lower()
        new_name = prefer if prefer else "image"
        filename = filename.lower()
        if filename == "Contents.json" or not filename.endswith(".png"):
            return None, None
        if scale == "1x":
            return "%s%s.png" % (new_name, "_" + appearance if appearance else "")
        assert(scale == "2x" or scale == "3x")
        return "%s%s@%s.png" % (new_name, "_"+appearance if appearance else "", scale)

    @staticmethod
    def filter_suffix(path, suffix=".imageset"):
        rst = path.lower().endswith(suffix)
        if not rst:
            message("filter_suffix ===> %s is dropped" % path)
        return rst

    @staticmethod
    def wall_dirs(path, matcher=None):
        if not os.path.isdir(path):
            raise BaseException("%1 should be directory" % (path))
        return list(filter(matcher, os.listdir(path)))

    @staticmethod
    def modify_imageset_rename(path, dir_name, json_data, files_map):
        current_dir = os.path.join(path, dir_name)
        prefer_name = os.path.splitext(dir_name)[0]

        for idiom, appearances in files_map.items():
            for appearance, scales in appearances.items():
                for scale, (file, _) in scales.items():
                    if not os.path.isfile(os.path.join(current_dir, file)):
                        raise BaseException("\t ===> %s is not a file" % file)
                    files_map[idiom][appearance][scale] = (file, XcAssetsModifier.default_rename(file, appearance, scale, prefer=prefer_name))

        new_files = set()
        for idiom, appearances in files_map.items():
            for appearance, scales in appearances.items():
                for scale, (file, new_file) in scales.items():
                    if new_file in new_files:
                        raise BaseException("\t %s(%s) has already exists" % (file, new_file))
                    new_files.add(new_file)

        for index, item in enumerate(json_data["images"]):
            if "filename" not in item or item["filename"] == "":
                continue
            if "appearances" in item:
                appearances = [x["value"] if "value" in x else "" for x in item["appearances"]]
                appearance = "_".join(sorted(appearances))
            else:
                appearance = ""
            idiom = item["idiom"]
            scale = item["scale"]
            filename = item["filename"]
            filename_new = files_map[idiom][appearance][scale][1]
            if idiom not in files_map or scale not in files_map[idiom][appearance]:
                continue
            json_data["images"][index]["filename"] = filename_new

            raw_path = os.path.join(current_dir, filename)
            new_path = os.path.join(current_dir, filename_new)
            if raw_path != new_path:
                os.rename(raw_path, new_path)
                message("\t ===> renamed: %s(%s, %s) --> %s" % (filename, scale, appearance, filename_new))
            else:
                message("\t ===> renamed: %s(%s, %s) --: already" % (filename, scale, appearance))
        with open(os.path.join(current_dir, "Contents.json"), encoding="utf8", mode="w") as f:
            f.write(json.dumps(json_data, ensure_ascii=False, indent=4))

    @staticmethod
    def modify_imageset_md5(path, dir_name, json_data, files_map):
        """ dependence：npm i thank-tiny-png
        """
        for idiom, appearances in files_map.items():
            for appearance, scales in appearances.items():
                for scale, (file, _) in scales.items():
                    current_path = os.path.join(path, dir_name, file)
                    if not os.path.isfile(current_path):
                        raise BaseException("\t ===> %s is not a file" % file)
                    cmdline = "npx thank-tiny-png %s" % current_path
                    with os.popen(cmdline, 'r') as f:
                        message(f.read())

    @staticmethod
    def compress_png(path):
        cmdline = "npx thank-tiny-png %s" % path
        with os.popen(cmdline, 'r') as f:
            message(f.read())

    @staticmethod
    def modify_imageset(path, dir_name, action=None):
        current_dir = os.path.join(path, dir_name)
        files_map = dict()
        with open(os.path.join(current_dir, "Contents.json"), encoding="utf8") as f:
            json_data = json.load(f)
            for index, item in enumerate(json_data["images"]):
                if "filename" not in item or item["filename"] == "":
                    continue
                if "appearances" in item:
                    appearances = [x["value"] if "value" in x else "" for x in item["appearances"]]
                    appearance = "_".join(sorted(appearances))
                else:
                    appearance = ""
                idiom = item["idiom"]
                scale = item["scale"]
                filename = item["filename"]
                if idiom not in files_map:
                    files_map[idiom] = dict()
                if appearance not in files_map[idiom]:
                    files_map[idiom][appearance] = dict()
                files_map[idiom][appearance][scale] = (filename, None)
        action(path, dir_name, json_data, files_map)

    @staticmethod
    def modify_xcassets(path, matcher=None, action=None):
        for dir_name in XcAssetsModifier.wall_dirs(args.path, matcher=matcher):
            message("modify_xcassets : current is %s" % dir_name)
            XcAssetsModifier.modify_imageset(args.path, dir_name, action=action)


import cv2
import requests


class ImgUtil(object):
    def getImage(self, url, filename, saveDir):
        outputPath = os.path.join(saveDir, filename)
        with requests.get(url, timeout=30) as resp, open(outputPath, 'wb') as f_save:
            f_save.write(resp.content)
            f_save.flush()
            f_save.close()
        return outputPath

    def resize(self, imgPath, outPath):
        imgData = cv2.imread(imgPath, flags=cv2.IMREAD_UNCHANGED)
        print("image shape:", imgData.shape[:2])
        newImg = cv2.resize(imgData, (335, 725)) # , interpolation=cv2.INTER_LINEAR
        cv2.imwrite(outPath, newImg) # , [cv2.IMWRITE_PNG_COMPRESSION, 9]

    def get_images(self, urls, output_dir):
        for i, url_path in enumerate(urls):
            _, filename = os.path.split(url_path)
            _, ext = os.path.splitext(filename)
            if ext.lower() != ".png":
                print("not png: %s" % (filename))
                continue
            if not os.path.exists(output_dir):
                os.mkdir(output_dir)
            filepath = self.getImage(url_path, filename, output_dir)
            print("get png (%d): %s" % (i, filename))

    def resize_images(self, source_dir, output_suffix=".resize.png"):
        files = os.listdir(source_dir)
        files = [i for i in files if i.endswith(".png") and not i.endswith(output_suffix)]
        for file in files:
            image = os.path.join(source_dir, file)
            self.resize(image, image + output_suffix)
            print("resize image: %s" % (image))

    def compress_images(self, source_dir, suffix=".resize.png"):
        files = os.listdir(source_dir)
        files = [i for i in files if i.endswith(suffix)]
        for i, file in enumerate(files):
            image = os.path.join(source_dir, file)
            XcAssetsModifier.compress_png(image)
            print("------- end: %d" % i)


# short, long
urls = [
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/a78f058fc6bd6f4e7bce81aeaa635cb8.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/3b0e5c12f5d53d42b6b777b68f920c61.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/58147f65d97fffaec6011cc7d9282812.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/6d2adea6327147552c4b84aa986befc1.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/390c3cbbcebcc3cf5e2e64e642967f57.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/4a65a2bd8c6e9937c1e5effa9e88f8e1.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/6cffedba1814d620c4314c5c56ec38cc.jpg", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/25ba6689c3257dfa05fbd1e6072af2f1.jpg" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/cfe4100e69bb183bafd080b2cf1c2cbd.jpg", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/a953f6eebc76161b63c2989062b5e5b6.jpg" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/90e3c2554a6c6654a477c93c7bb08de4.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/deddf00f2e101dca263283d5f252c0de.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/a1ebeedc145e8ed0817e032646287bff.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/29efc65e004353801a0ad8aa41ad8e71.jpg" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/45089ae96aa4d29a9cc8fa83664699cb.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/2f7e66cb3d6c40caee206ff6b201ba08.jpg" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/004438a25773b533610dceef09a725c5.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/13d6bdc350cec660df3d8c247f63a283.jpg" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/712651a49f6451383b770838798b8c2c.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/a9d186e531cfa44d527f0be2f058c338.jpg" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/6ad802f4106863377e3dcbb7021f50b4.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/de259e9c1030a81cb9c61c4389ae44ac.jpg" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/ab3e316e97a65c2499c215ddf68c508e.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/33925abdc3914fec2524cadf47f7aef1.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/9ea71aab5c41b638b8fd6eabebfcccc0.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/d962f114fbfc5c1566262a62ea7d644c.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/a7b77a01c9f4fc2df26f4f35f9f93c00.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/644efaced7355a967d749842783359ac.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/ccdf7b318b92e9a4a09dcbb45adca0dd.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/22a7413e60baf07afd49405b99321f1e.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/b31308444eeceeae5fa23039673f59da.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/9f428221c0ad785a54c66a5b33daaba5.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/98972dfd1ab7711739be9805ef7b46d3.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/c00fb1752a20874ecbbea29cdd9071b1.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/5889cfbad43804c2d6b5392bb5cefe9b.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/08f45b6974b3b30ff95538bae47f870f.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/d6ec24b785c0f2bbe553c4aadf93bd06.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/d1cdafebc79653cbf015620fa475bace.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/e982cf3cdbfd56022296488588905557.jpg", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/77d0695edcf98fe99e8d4d535b7910c2.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/5892a07dbb16caa5d55ba3e5b3c973a8.jpg", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/202eff7bcc32ec6fa8e02be168cc4c7a.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/5c817b2a6d5f39bc9f8802c0e9f7e1c4.jpg", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/bdc69a12eeaf0dcc8964f4dfadd8b966.jpg" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/7736fd09ddfb6a38046ce456acd93e94.jpg", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/0f2da6977d1540d8e46b12f0326d7e34.jpg" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/5b5226a2ba470fd35c03d261c3257e8c.jpg", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/4d931613b8f5a45ec0c3f590e4a216c5.jpg" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/739b5b05f7be7b5f5a72b276c693fc48.jpg", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/f614405a0b25e2a13cc1d22e1ff36fd4.jpg" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/2d95b7196a79ee524e94b232b83c8503.jpg", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/631aa709ee5236e489d921459cf8ee23.jpg" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/8726f6e90951df27ff589b97ed1c2255.jpg", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/c86707139bb317874943ae0f877767ea.jpg" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/57b386076b644d22b5c789d57a6d2d37.jpg", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/e9efa48c128ad8995499e356f96c67af.jpg" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/e53fef1f0f58e80c9e6915bd0b42e1f9.jpg", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/98921bd178021e7056a19e5df09e317f.jpg" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/8a913db24844899a32f041719ac8e90c.jpg", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/5476d69c2a73e5f7d48b044f0ec52ce7.jpg" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/685d72030bc41ebdeef6a9f9259928d3.jpg", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/a2cbffcceb91047e406ec8f4ef6c1126.jpg" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/855b7457296dd26adb03ee95578ca57a.jpg", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/d682dfb910662436430bcadbfe1e9fcc.jpg" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/9d8404a02e571d4476985381bff0fdf4.jpg", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/355a25bcce8547a5d36d68be59ac7f2d.jpg" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/d20eed80c74566b728c5085b38a101f4.jpg", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/cf6299a47a9691d14497e1e228c433b3.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/5d9472d2cc18a249d6d57fdd5f6f3fe8.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/dd6f459446d1f4ebf364818ff15ad92e.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/dd407c2dbabd9eb7a5a7041c641fdfa2.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/05304d7c4020704f3000ae2a59334b3f.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/19507542073fcf669045de7d55a5c214.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/87181031a3d14dd7e79c7339227dff34.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/4b62137cab7e2ea0411d4ee15759f4a3.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/4661f7d306bba17fae4e23245845a130.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/652392ab7bf6e1f1722a77ed2b1ff750.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/d61fd4603c0460aa0f2ab717368eaf98.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/679c10a7ea689cc91bf66062f6ee0e5b.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/17a87a35a563917ae7b519873be1aa2b.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/a1e7ff430b0a29fd29dfff10c44f790e.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/59573244457fef9b2495e8ba6bd97a6b.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/7df3e6900d1ad00e7710264ae0a23626.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/7408d006a4b78826636ba921d696cefa.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/f99ffe5b5d3a4a088a0fd7beacc8f096.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/b5d267eb0ce8725a0b560836d11ef8c3.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/f3a5e7883ec2411e602b7e13d062b7f0.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/344fed57817728c882458f97da0ee529.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/62a2ed7eaeeeea65c49c8637a0aea197.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/2174063d340cba2f10b34efa41457548.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/06f1fee5fb21541386a98ef43d631efb.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/422211a4773502727ca12c048eece723.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/0ced02a39c7bb25cd58ec02c70b7ffda.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/9d173fec7511ae13c9f17ae75c4ac4e6.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/f7d09ed2173b13bbf06551b3a2d9c2d3.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/1d0004eda9ebc77be7d100a28bfcc9bd.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/81a834cbc858beff904f647ce4497fca.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/981fc2e22266c28536b2c72a7c34f214.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/9839847da6b57d91aadfb5271529f831.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/fc82e2e104832ec1471c47ab333d60a8.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/c41a8a1f23a3dbbec780b703abccc2f6.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/e4dca3ee8a6a4be7385da26c4e445025.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/993cf1da2c74113289bbf9959f17145b.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/05928737243c5510790daf6e25bd276a.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/0318def8ad837afad58ebfa5525c185f.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/4620101192d70b54c29ba6575d9dd397.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/a573d76090c23813d6363b13325c134b.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/4d34c3935296b32458c6aadbf9494d27.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/22de46d29076b6f252cd0b2cf88c74df.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/4f0dcc1a57fd4837735b9606eac8576c.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/c3ae227e7db3cfa732abd43ed8bc6f4b.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/435e42b3f0daeebd351c749c98e5ec80.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/d410bf687b6ed2acaa3ead885d94b9de.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/8825bceb2b2ba16bbbc500021d808217.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/4866b0f02e3800315bc96b6d738266d8.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/21435a001ad5924a1ad61fd50b6b6b94.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/8ad994602a2929925443f7232da415f8.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/5cb103dfacbcb1dc3fef452e265c4335.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/a0da45e71e51e8bd5bf487c0f321bf78.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/a615fd7cb10132b2d166f64b1ea7924e.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/2af54f4807ca2a7a4abbbdc46f21b289.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/1de3471fb5c35155f8f4566d3dffb068.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/107f93b6d88cd0848aae1ad9a9db553e.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/fe1ea3474fc2c63e0c32d83035bbdb20.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/a43f8cb655ea4a314079004a69bf53de.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/a93c376bbc063eb5c7c89515d2b66c37.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/80b4d3d1530579e9a0f17e0a0400118e.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/22f87a2f8f46ecc68a84c6ee8e52aafd.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/6775ad0ed5d35f16a5302a4867a9e86d.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/fa3977606ed5e7198d05cd4084706b96.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/9cbf368dde0489f7ff668024ac458d7e.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/8c875042538332f45d6db3106a17f7ef.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/abc7ae9df52cc9119e30c43aa19fb6bd.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/21380c750aeab959e0f9881e0ed0decc.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/7debe834b2a96730d46fb16873e54a99.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/30b97bbaddaf40394837848ee98e5ad3.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/d4d0839074291d0c4a52fbdc63a87747.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/18348dcc56d4eadceae5f6711bd5b188.jpg" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/fd07e310d9887a92d8add2473c4d103c.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/99e5f10266225ac18d2968600a243910.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/a97bd455d43b4ab3ba9a1f87cd5075d2.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/f8ebd55fcc2ab0447c85a3d168a73f25.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/cb1cc95b5eaa475b8989ad3b04bf51f0.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/05b9ac2aebef5cbe968c97d2bb23394d.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/a5a34f27e9ffb2d94cf5416bade791c3.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/7fea23df9c2e80f447ed1f0ca924597a.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/631c2a3fcf0e2f03745a132b8a71b992.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/8b22fc621727e2b58df5c19b4f8ee747.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/4ee5a4e759015b8014388fdcb62991cd.jpg", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/fc0f752b50b8b0d5d573c58ac1e12344.jpg" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/a7e109a479cf4af519b67a9d5180abe2.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/c97f6d2a9ca0305f7ceb60323acd8c37.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/af8c776d68c6bcce16eb4377b73b2296.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/61fdcf9645809a753907f427102121fe.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/34fc2f0e8e634d8c634c2d048e8d81dc.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/6ad211e7238ded0a481b5a970880c60c.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/9b2b0477f84f97010b543130a06addbb.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/a26c65f1176f11ac4ebb81e45de86903.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/c6cd08af4924e51eafef18db46728bef.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/f2b237303018a024e775c8b4665e4756.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/a1c8365cca7bd87a6b4170e31f7db8a7.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/fbcb202820b75aec4b1511229b3a30aa.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/0ba59d01e4ab1f512152161593cbb9af.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/686da454a5b7927a8a315aed4d224267.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/5af0b39e190e1d01070d0c783afa2173.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/a5311f4b49af926d449f275a0cd918f5.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/3711feaa10ee2034bd8224e9b67fc7f1.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/d7197fb3182cfbb715935b504b114ec3.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/92c345a68ac7bfce7e341e3d912bb3c4.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/e46aa0486599951d2a30dee37c74e542.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/55a26d87ad2534df4c37e93be38d6dc3.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/e6d15d1d00d654b88bd717152672c5a9.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/07e39ef173f4836d8158811437745d48.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/55957ee065837850f9ebe035aebfdb67.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/48f33dfb59162a9e5872c8192252d1df.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/d4f0505a7518c7c90e4fed52e754324d.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/bde524bf5aec97df7d025e249003a4e8.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/f1b0f2cf2922bac8deec2a3e6afbf123.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/6937de7455e41c78b2dac9e41cb2438d.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/efb122189f5076cbf7d3996cc6eca6a0.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/2c3d21bdc78752a26eaa3a017eabcbaa.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/345de344f1f485cdebb2b6c284310b91.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/8f143416aa75556366b148a70751f759.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/9cd449715ebe8525117760fce3bbc05a.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/d6f6c3e022b24c53690d4bd9ac113cb9.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/bacea23a99b7b0fb927c3e95486753db.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/33713b4a1d13fd37126af40043c889f3.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/145b89873cd3dd6ffdd282b878787b2f.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/be0b1628f025f33956d5457732a86ca3.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/1cbb64256259f16b4e8010130635ac4a.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/89e2df21c940781c2f0c73bbbd8b5afc.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/90c73ccb2b2d7fe12eb7649a7c4ad0b9.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/016efa8707b9c8265a57da6ecfcc61bf.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/a43cf792021ef46c7aa5b8d114b38658.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/0208fe0762cd4ce416259c3c419d5bda.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/cd1594fd25920d6bf4604a0e98a33768.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/26041f10c977e79b5eedb72ca1c8ad07.jpg", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/1f0f24be7563a1d33679ebef0a39e13f.jpg" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/9717e32447fe1c1076ac2d7f5b564a85.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/dcb8abb212eada574b13582fdb25a265.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/561e15b51c7fc2c4a53c0a426595946b.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/963f4c683ed06c2e4653e6d0e886cd05.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/46679fb0049ff4b48017c63f62d69048.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/9edd522347a96684e6fd3eebef39dd23.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/c73e8beda60d0599e4e8f34c5a6f63a2.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/6ff3a9075a3e0dab7db3289f798addc8.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/92b3eaf8ce65f0d7cd2fc6a4c71bfd90.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/b08081133f748cb20d288e53bce05e8b.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/3429ec677bf590e3cb39cbc8829ffd03.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/5d5b05095c2629254b186fb29324b534.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/a78f058fc6bd6f4e7bce81aeaa635cb8.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/3b0e5c12f5d53d42b6b777b68f920c61.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/58147f65d97fffaec6011cc7d9282812.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/6d2adea6327147552c4b84aa986befc1.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/390c3cbbcebcc3cf5e2e64e642967f57.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/4a65a2bd8c6e9937c1e5effa9e88f8e1.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/6cffedba1814d620c4314c5c56ec38cc.jpg", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/25ba6689c3257dfa05fbd1e6072af2f1.jpg" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/cfe4100e69bb183bafd080b2cf1c2cbd.jpg", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/a953f6eebc76161b63c2989062b5e5b6.jpg" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/90e3c2554a6c6654a477c93c7bb08de4.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/deddf00f2e101dca263283d5f252c0de.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/a1ebeedc145e8ed0817e032646287bff.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/29efc65e004353801a0ad8aa41ad8e71.jpg" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/45089ae96aa4d29a9cc8fa83664699cb.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/2f7e66cb3d6c40caee206ff6b201ba08.jpg" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/004438a25773b533610dceef09a725c5.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/13d6bdc350cec660df3d8c247f63a283.jpg" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/712651a49f6451383b770838798b8c2c.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/a9d186e531cfa44d527f0be2f058c338.jpg" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/6ad802f4106863377e3dcbb7021f50b4.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/de259e9c1030a81cb9c61c4389ae44ac.jpg" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/ab3e316e97a65c2499c215ddf68c508e.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/33925abdc3914fec2524cadf47f7aef1.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/9ea71aab5c41b638b8fd6eabebfcccc0.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/d962f114fbfc5c1566262a62ea7d644c.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/a7b77a01c9f4fc2df26f4f35f9f93c00.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/644efaced7355a967d749842783359ac.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/ccdf7b318b92e9a4a09dcbb45adca0dd.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/22a7413e60baf07afd49405b99321f1e.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/b31308444eeceeae5fa23039673f59da.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/9f428221c0ad785a54c66a5b33daaba5.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/98972dfd1ab7711739be9805ef7b46d3.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/c00fb1752a20874ecbbea29cdd9071b1.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/5889cfbad43804c2d6b5392bb5cefe9b.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/08f45b6974b3b30ff95538bae47f870f.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/d6ec24b785c0f2bbe553c4aadf93bd06.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/d1cdafebc79653cbf015620fa475bace.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/e982cf3cdbfd56022296488588905557.jpg", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/77d0695edcf98fe99e8d4d535b7910c2.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/5892a07dbb16caa5d55ba3e5b3c973a8.jpg", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/202eff7bcc32ec6fa8e02be168cc4c7a.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/5c817b2a6d5f39bc9f8802c0e9f7e1c4.jpg", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/bdc69a12eeaf0dcc8964f4dfadd8b966.jpg" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/7736fd09ddfb6a38046ce456acd93e94.jpg", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/0f2da6977d1540d8e46b12f0326d7e34.jpg" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/5b5226a2ba470fd35c03d261c3257e8c.jpg", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/4d931613b8f5a45ec0c3f590e4a216c5.jpg" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/739b5b05f7be7b5f5a72b276c693fc48.jpg", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/f614405a0b25e2a13cc1d22e1ff36fd4.jpg" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/2d95b7196a79ee524e94b232b83c8503.jpg", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/631aa709ee5236e489d921459cf8ee23.jpg" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/8726f6e90951df27ff589b97ed1c2255.jpg", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/c86707139bb317874943ae0f877767ea.jpg" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/57b386076b644d22b5c789d57a6d2d37.jpg", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/e9efa48c128ad8995499e356f96c67af.jpg" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/e53fef1f0f58e80c9e6915bd0b42e1f9.jpg", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/98921bd178021e7056a19e5df09e317f.jpg" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/8a913db24844899a32f041719ac8e90c.jpg", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/5476d69c2a73e5f7d48b044f0ec52ce7.jpg" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/685d72030bc41ebdeef6a9f9259928d3.jpg", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/a2cbffcceb91047e406ec8f4ef6c1126.jpg" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/855b7457296dd26adb03ee95578ca57a.jpg", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/d682dfb910662436430bcadbfe1e9fcc.jpg" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/9d8404a02e571d4476985381bff0fdf4.jpg", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/355a25bcce8547a5d36d68be59ac7f2d.jpg" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/d20eed80c74566b728c5085b38a101f4.jpg", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/cf6299a47a9691d14497e1e228c433b3.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/5d9472d2cc18a249d6d57fdd5f6f3fe8.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/dd6f459446d1f4ebf364818ff15ad92e.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/dd407c2dbabd9eb7a5a7041c641fdfa2.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/05304d7c4020704f3000ae2a59334b3f.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/19507542073fcf669045de7d55a5c214.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/87181031a3d14dd7e79c7339227dff34.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/4b62137cab7e2ea0411d4ee15759f4a3.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/4661f7d306bba17fae4e23245845a130.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/652392ab7bf6e1f1722a77ed2b1ff750.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/d61fd4603c0460aa0f2ab717368eaf98.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/679c10a7ea689cc91bf66062f6ee0e5b.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/17a87a35a563917ae7b519873be1aa2b.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/a1e7ff430b0a29fd29dfff10c44f790e.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/59573244457fef9b2495e8ba6bd97a6b.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/7df3e6900d1ad00e7710264ae0a23626.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/7408d006a4b78826636ba921d696cefa.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/f99ffe5b5d3a4a088a0fd7beacc8f096.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/b5d267eb0ce8725a0b560836d11ef8c3.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/f3a5e7883ec2411e602b7e13d062b7f0.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/344fed57817728c882458f97da0ee529.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/62a2ed7eaeeeea65c49c8637a0aea197.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/2174063d340cba2f10b34efa41457548.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/06f1fee5fb21541386a98ef43d631efb.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/422211a4773502727ca12c048eece723.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/0ced02a39c7bb25cd58ec02c70b7ffda.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/9d173fec7511ae13c9f17ae75c4ac4e6.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/f7d09ed2173b13bbf06551b3a2d9c2d3.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/1d0004eda9ebc77be7d100a28bfcc9bd.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/81a834cbc858beff904f647ce4497fca.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/981fc2e22266c28536b2c72a7c34f214.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/9839847da6b57d91aadfb5271529f831.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/fc82e2e104832ec1471c47ab333d60a8.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/c41a8a1f23a3dbbec780b703abccc2f6.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/e4dca3ee8a6a4be7385da26c4e445025.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/993cf1da2c74113289bbf9959f17145b.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/05928737243c5510790daf6e25bd276a.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/0318def8ad837afad58ebfa5525c185f.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/4620101192d70b54c29ba6575d9dd397.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/a573d76090c23813d6363b13325c134b.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/4d34c3935296b32458c6aadbf9494d27.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/22de46d29076b6f252cd0b2cf88c74df.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/4f0dcc1a57fd4837735b9606eac8576c.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/c3ae227e7db3cfa732abd43ed8bc6f4b.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/435e42b3f0daeebd351c749c98e5ec80.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/d410bf687b6ed2acaa3ead885d94b9de.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/8825bceb2b2ba16bbbc500021d808217.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/4866b0f02e3800315bc96b6d738266d8.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/21435a001ad5924a1ad61fd50b6b6b94.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/8ad994602a2929925443f7232da415f8.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/5cb103dfacbcb1dc3fef452e265c4335.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/a0da45e71e51e8bd5bf487c0f321bf78.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/a615fd7cb10132b2d166f64b1ea7924e.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/2af54f4807ca2a7a4abbbdc46f21b289.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/1de3471fb5c35155f8f4566d3dffb068.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/107f93b6d88cd0848aae1ad9a9db553e.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/fe1ea3474fc2c63e0c32d83035bbdb20.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/a43f8cb655ea4a314079004a69bf53de.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/a93c376bbc063eb5c7c89515d2b66c37.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/80b4d3d1530579e9a0f17e0a0400118e.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/22f87a2f8f46ecc68a84c6ee8e52aafd.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/6775ad0ed5d35f16a5302a4867a9e86d.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/fa3977606ed5e7198d05cd4084706b96.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/9cbf368dde0489f7ff668024ac458d7e.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/8c875042538332f45d6db3106a17f7ef.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/abc7ae9df52cc9119e30c43aa19fb6bd.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/21380c750aeab959e0f9881e0ed0decc.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/7debe834b2a96730d46fb16873e54a99.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/30b97bbaddaf40394837848ee98e5ad3.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/d4d0839074291d0c4a52fbdc63a87747.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/18348dcc56d4eadceae5f6711bd5b188.jpg" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/fd07e310d9887a92d8add2473c4d103c.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/99e5f10266225ac18d2968600a243910.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/a97bd455d43b4ab3ba9a1f87cd5075d2.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/f8ebd55fcc2ab0447c85a3d168a73f25.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/cb1cc95b5eaa475b8989ad3b04bf51f0.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/05b9ac2aebef5cbe968c97d2bb23394d.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/a5a34f27e9ffb2d94cf5416bade791c3.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/7fea23df9c2e80f447ed1f0ca924597a.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/631c2a3fcf0e2f03745a132b8a71b992.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/8b22fc621727e2b58df5c19b4f8ee747.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/4ee5a4e759015b8014388fdcb62991cd.jpg", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/fc0f752b50b8b0d5d573c58ac1e12344.jpg" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/a7e109a479cf4af519b67a9d5180abe2.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/c97f6d2a9ca0305f7ceb60323acd8c37.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/af8c776d68c6bcce16eb4377b73b2296.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/61fdcf9645809a753907f427102121fe.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/34fc2f0e8e634d8c634c2d048e8d81dc.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/6ad211e7238ded0a481b5a970880c60c.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/9b2b0477f84f97010b543130a06addbb.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/a26c65f1176f11ac4ebb81e45de86903.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/c6cd08af4924e51eafef18db46728bef.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/f2b237303018a024e775c8b4665e4756.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/a1c8365cca7bd87a6b4170e31f7db8a7.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/fbcb202820b75aec4b1511229b3a30aa.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/0ba59d01e4ab1f512152161593cbb9af.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/686da454a5b7927a8a315aed4d224267.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/5af0b39e190e1d01070d0c783afa2173.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/a5311f4b49af926d449f275a0cd918f5.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/3711feaa10ee2034bd8224e9b67fc7f1.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/d7197fb3182cfbb715935b504b114ec3.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/92c345a68ac7bfce7e341e3d912bb3c4.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/e46aa0486599951d2a30dee37c74e542.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/55a26d87ad2534df4c37e93be38d6dc3.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/e6d15d1d00d654b88bd717152672c5a9.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/07e39ef173f4836d8158811437745d48.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/55957ee065837850f9ebe035aebfdb67.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/48f33dfb59162a9e5872c8192252d1df.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/d4f0505a7518c7c90e4fed52e754324d.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/bde524bf5aec97df7d025e249003a4e8.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/f1b0f2cf2922bac8deec2a3e6afbf123.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/6937de7455e41c78b2dac9e41cb2438d.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/efb122189f5076cbf7d3996cc6eca6a0.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/2c3d21bdc78752a26eaa3a017eabcbaa.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/345de344f1f485cdebb2b6c284310b91.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/8f143416aa75556366b148a70751f759.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/9cd449715ebe8525117760fce3bbc05a.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/d6f6c3e022b24c53690d4bd9ac113cb9.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/bacea23a99b7b0fb927c3e95486753db.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/33713b4a1d13fd37126af40043c889f3.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/145b89873cd3dd6ffdd282b878787b2f.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/be0b1628f025f33956d5457732a86ca3.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/1cbb64256259f16b4e8010130635ac4a.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/89e2df21c940781c2f0c73bbbd8b5afc.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/90c73ccb2b2d7fe12eb7649a7c4ad0b9.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/016efa8707b9c8265a57da6ecfcc61bf.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/a43cf792021ef46c7aa5b8d114b38658.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/0208fe0762cd4ce416259c3c419d5bda.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/cd1594fd25920d6bf4604a0e98a33768.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/26041f10c977e79b5eedb72ca1c8ad07.jpg", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/1f0f24be7563a1d33679ebef0a39e13f.jpg" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/9717e32447fe1c1076ac2d7f5b564a85.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/dcb8abb212eada574b13582fdb25a265.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/561e15b51c7fc2c4a53c0a426595946b.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/963f4c683ed06c2e4653e6d0e886cd05.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/46679fb0049ff4b48017c63f62d69048.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/9edd522347a96684e6fd3eebef39dd23.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/c73e8beda60d0599e4e8f34c5a6f63a2.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/6ff3a9075a3e0dab7db3289f798addc8.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/92b3eaf8ce65f0d7cd2fc6a4c71bfd90.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/b08081133f748cb20d288e53bce05e8b.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/3429ec677bf590e3cb39cbc8829ffd03.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/5d5b05095c2629254b186fb29324b534.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/f2e7008dc47deb8772ef2f06561364a4.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/d6b2255871b025303902d78746894f49.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/e5a140685610c36b3242f9e562e9fc18.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/22b8618a2e3fbd8b2af5cca2c55902f6.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/3abc30f55f94936dc06033070e61632a.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/55ffd4dd1316d4046bf0f15584ba693f.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/e8f371e8bce718e385ce796d4953532e.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/47b8b2195e87c474c035cc66f33f11dd.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/76d9094c5055ff5f3b7b49ca440ab084.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/d72632000d66f35650467e133b7e5638.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/2b2d97f16c7d757737d63a99bef8b570.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/095209219980c5132cae900541f81439.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/512d2be2dea565b7113ff67fb6034abb.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/a7f1f1f9bded0946f7d117aedbd5ea23.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/0fd101b5f33e77eb7929f899fe3ea507.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/4589460cc0e1a0c36ae5756cec250d75.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/5d9ed4b7695bd1c24c1dab41e2a6fa1d.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/fef74ccfa6744f8a2d8def5aa9d2f3db.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/e8644fc55d9f7259a71758f960d3b278.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/fd2c51749c2cb195ab7fc7a4394fc6df.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/720619df420572d1db8e19c429d470c8.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/06266912b3f7d165be346a0a27e84c6f.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/7e94e4f3ce1aa6698de97cc5145b00e2.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/d3113df6c764f7731b162f6f2e8822ce.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/1b796d2ff8743f5d7ee87955b51867ac.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/756ddfe82abbeeda14801f698144a74a.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/e61a010821b7710093a96b709f832f2e.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/65d6a08187edb24d060bffa4b1ce8a8c.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/61602aaeee5734bf1e7e18c97fa6e7dc.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/650c9da82daf5c53fcbbdb5b88368a83.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/86034b6f0a7dfca107f714535215aae9.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/5f665b8e36f4e479d9da6e91b036808f.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/c1b4666c11559d9b2ac0ecdbf4f1ee25.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/eceb111a21b234d7fabfc8bfcb866518.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/4545ac7dae47a120a1b4ac278eddfa28.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/0812a3428fd16557066dc0e4a4fc0d79.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/6fee8e61d86dfeeeb7bda7f4407be060.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/eb30986e70678f22323eedfb191a79c0.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/0aab862fd87884e44fb2054045886ba0.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/dcd17229220a1e84ca737417ca3e63b7.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/ea2b0e604944a70f6ea50400226f121c.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/8bd630a795923b631f936a66ac8ebee6.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/5aabc4d785839284f1715953dbc73988.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/51aeb46c7f81a66eced026a2f7664c14.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/edb685e9ae10f27b00c0f845a776f66c.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/0df12f7e95df8eda446022ee091d1ca4.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/732d2e4e52a2658cca5c8ab54fe41478.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/890a206470aa4dd12616a41f75f18c53.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/79b4573dbf1671ed38b920425a574ade.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/d164bef0afa6bb4979b3864a668dddf9.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/1a02cf3bff10921f2e79634f0ddffa42.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/33cac3ed132dc7ea96cb740901fe775d.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/cfe3c9b725a51dc48abc2f4c5754e7d0.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/6286e1ddfaa48cf9a47fb1abb1e78c7d.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/fa9e7e43aa725458cf46a569fc3bfdff.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/9d0b767b6b70534498fd6d30ae492f79.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/243fa8986f8be59d6557bc8c5abec8f8.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/dd1f82340792882da29414b6de8d2682.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/11fce1bd8cce13cff315ea3258dc6784.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/7fa881592ea3c265d9a1d6f0debf40b4.png" ],
[ "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/136a7b55ddf486436e3ed8e1e4f3d3dd.png", "https://cdn-wallpaper-overseas.cubeengineapp.com/mobile_composite/preview_jpg/79cad5576f89d5206859364508bfa890.png" ]
]


def get_size(dir, suffix=".resize.png"):
    files = os.listdir(dir)
    files = [i for i in files if i.endswith(suffix)]
    results = {}
    for file in files:
        if suffix == ".resize.png":
            name, _ = os.path.splitext(file.rstrip(suffix))
        else:
            name, _ = os.path.splitext(file)
        results[name] = os.path.getsize(os.path.join(dir, file))
    return results


if __name__ == '__main__':
    args = XcAssetsModifier.parse_args()
    if args.action == "rename":
        XcAssetsModifier.modify_xcassets(args.path, matcher=XcAssetsModifier.filter_suffix, action=XcAssetsModifier.modify_imageset_rename)
    if args.action == "tinypng":
        XcAssetsModifier.modify_xcassets(args.path, matcher=XcAssetsModifier.filter_suffix, action=XcAssetsModifier.modify_imageset_md5)
    if args.action == "resize":
        data_dir = "/Users/jiao/Downloads/mega_theme_long"
        shorts = set([i[0] for i in urls])
        longs = set([i[1] for i in urls])
        #ImgUtil().get_images(shorts, "/Users/jiao/Downloads/mega_theme_short")
        #ImgUtil().get_images(longs, "/Users/jiao/Downloads/mega_theme_long")
        #ImgUtil().resize_images(data_dir)
        #ImgUtil().compress_images(data_dir)
        shortSize = get_size("/Users/jiao/Downloads/mega_theme_short", ".png")
        longSize = get_size("/Users/jiao/Downloads/mega_theme_long", ".resize.png")
        results = []
        for item in urls:
            short, long = item[0], item[1]
            _, shortFile = os.path.split(short)
            shortName, _ = os.path.splitext(shortFile)
            _, longFile = os.path.split(long)
            longName, _ = os.path.splitext(longFile)
            data = {}
            data["short"] = short
            data["long"] = long
            data["short_size"] = shortSize.get(shortName, 0)
            data["long_size"] = longSize.get(longName, 0)
            if data["short_size"] == 0:
                data["compare"] = "Nan"
            else:
                data["compare"] = "%d%%" % ((data["long_size"] - data["short_size"]) * 100.0 / data["short_size"])
            print("====>  %s , %.2f %.2f :===: short: %s  long: %s " % (data["compare"], data["short_size"]/1024.0, data["long_size"]/1024.0, shortFile, longFile))


