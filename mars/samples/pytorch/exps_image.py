# coding=utf-8

import os
import cv2
import numpy as np


class ImageCompare(object):
    def __init__(self, **exp_image_paths):
        self.exp_image_paths = exp_image_paths

    def __del__(self):
        cv2.destroyAllWindows()

    def _make_image_pair(self, exp_image_paths):
        '''
        :param exp_image_paths:
        :return: filename - [(exp,path),(exp,path)]
        '''
        results = dict()
        for exp, p in exp_image_paths.items():
            files = os.listdir(p)
            for f in files:
                if f not in results:
                    results[f] = []
                results[f].append((exp, f))
        final = dict()
        for name, files in results.items():
            rst = []
            tmp = dict()
            for f in files:
                tmp[f[0]] = f[1]
            for exp in exp_image_paths.keys():
                rst.append((exp, tmp[exp] if exp in tmp else None))
            final[name] = rst
        return final

    def _show_image(self, name, image, delay=0, max_h=540.0):
        resized = image.shape[0] / max_h if image.shape[0] > max_h else 1.0
        cv2.namedWindow(name, cv2.WINDOW_NORMAL)
        cv2.resizeWindow(name, int(image.shape[1] / resized), int(image.shape[0] / resized));
        cv2.imshow(name, image)
        key = cv2.waitKey(delay=delay)
        cv2.destroyWindow(name)
        if key & 0xFF == ord('q'):
            return False
        return True

    def _pack_images(self, name: str, files: list()):
        images = []
        for k in files:
            exp, path = k
            if not path:
                continue
            img = cv2.imread(path)
            cv2.putText(img, exp, (100,100), cv2.FONT_HERSHEY_COMPLEX, 3, (0, 255, 255), 2)
            images.append(img)
        H = max([img.shape[0] for img in images])
        W = max([img.shape[1] for img in images])
        images_padded = []
        for img in images:
            h, w, c = img.shape
            img = np.pad(img, ((0, H-h), (0, int((W-w) / 10)), (0,0)),'constant',constant_values = (0,0))
            images_padded.append(img)
        img_new = np.hstack(images_padded)
        return img_new

    def run(self):
        imgs = self._make_image_pair(self.exp_image_paths)
        for name, files in imgs.items():
            paths = []
            for i in range(len(files)):
                exp, name = files[i]
                paths.append((exp, os.path.join(self.exp_image_paths[exp], name) if name else None))
            img_new = self._pack_images(name, paths)
            if not self._show_image(name, img_new):
                break


if __name__ == "__main__":
    exps = {
       # "raw": "D:\\Workspace\\cartoon-style\\White-box-Cartoonization\\test_code\\test_images",
        "exp0": "D:\\Workspace\\cartoon-style\\White-box-Cartoonization\\test_code\\cartoonized_images.0" ,
        "exp1": "D:\\Workspace\\cartoon-style\\White-box-Cartoonization\\test_code\\cartoonized_images.1",
        "exp2": "D:\\Workspace\\cartoon-style\\White-box-Cartoonization\\test_code\\cartoonized_images",
        "exp_torch": "D:\\Workspace\\cartoon-style\\AnimeStylized\\asset_out",
    }
    image_compare = ImageCompare(**exps)
    image_compare.run()