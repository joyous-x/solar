import base64
import os
import re


# 定义正则表达式
pattern = r'new String\(Base64\.decode\("([a-zA-Z0-9+/=]+)"\.getBytes\(\), 0\)\)'


class AntiApkTools(object):
    def __init__(self):
        pass

    @staticmethod
    def base64_decode(dir_path, pattern = r'new String\(Base64\.decode\("([^"]+)"\.getBytes\(\), 0\)\)'):
        for root, dirs, files in os.walk(dir_path):
            for file in files:
                if file.endswith('.java'):
                    filepath = os.path.join(root, file)
                    with open(filepath, 'r') as f:
                        content = f.read()
                    content = re.sub(pattern, lambda match: '"' + base64.b64decode(match.group(1)).decode('utf-8') + '"', content)
                    with open(filepath, 'w') as f:
                        f.write(content)
                    print("---> %s \t done".format(filepath))


if __name__ == "__main__":
    AntiApkTools.base64_decode("/Users/jiao/Workspace/anti-app/laowang_vpn/jadx.exported/app/src")