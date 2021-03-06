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
        """ dependence???npm i thank-tiny-png
        https://github.com/zhanyuzhang/super-tinypng
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


if __name__ == '__main__':
    args = XcAssetsModifier.parse_args()
    if args.action == "rename":
        XcAssetsModifier.modify_xcassets(args.path, matcher=XcAssetsModifier.filter_suffix, action=XcAssetsModifier.modify_imageset_rename)
    if args.action == "tinypng":
        XcAssetsModifier.modify_xcassets(args.path, matcher=XcAssetsModifier.filter_suffix, action=XcAssetsModifier.modify_imageset_md5)
