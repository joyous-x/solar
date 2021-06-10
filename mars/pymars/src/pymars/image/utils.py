# coding=utf-8

import os
import numpy as np
import cv2
import math


class ImageUtils(object):
    """ ImageUtils
    1. help for show images

    """

    @staticmethod
    def show_image(name, image_cv_obj, delay=0, max_h=540.0):
        resized = image_cv_obj.shape[0] / max_h if image_cv_obj.shape[0] > max_h else 1.0
        cv2.namedWindow(name, cv2.WINDOW_NORMAL)
        cv2.resizeWindow(name, int(image_cv_obj.shape[1] / resized), int(image_cv_obj.shape[0] / resized));
        cv2.imshow(name, image_cv_obj)
        key = cv2.waitKey(delay=delay)
        cv2.destroyWindow(name)
        if key & 0xFF == ord('q'):
            return False
        return True

    @staticmethod
    def calc_mean_std(img_h, img_w, image_dirs: list, max_files=1000):
        """ calc_mean_std
        help to calculate the mean and std
        """
        means, stdevs = [], []
        imgs = np.zeros([img_w, img_h, 3, 1])

        files = []
        for dir in image_dirs:
            files.extend([os.path.join(dir, f) for f in os.listdir(dir)])

        indexs = np.array(range(len(files)))
        np.random.shuffle(indexs)
        for index in indexs[:max_files]:
            img = cv2.imread(files[index])
            img = cv2.resize(img, (img_h, img_w))
            img = img[:, :, :, np.newaxis]
            imgs = np.concatenate((imgs, img), axis=3)

        imgs = imgs.astype(np.float32) / 255.
        for i in range(3):
            pixels = imgs[:, :, i, :].ravel()  # 拉成一行
            means.append(np.mean(pixels))
            stdevs.append(np.std(pixels))

        # cv2 读取的图像格式为BGR，PIL/Skimage读取到的都是RGB不用转
        means.reverse()  # BGR --> RGB
        stdevs.reverse()

        print("normMean = {}".format(means))
        print("normStd = {}".format(stdevs))
        print('transforms.Normalize(normMean = {}, normStd = {})'.format(means, stdevs))


class ImageQuality(object):
    """ ImageQuality: help for calculating the quality (eg. psnr、ssim, ...) of images
    """
    @staticmethod
    def psnr(image_predicted: np.array, image_truth:  np.array):
        if image_predicted.shape != image_predicted.shape:
            raise TypeError('Input images must have the same dimensions')
        m = float(image_truth.max())
        mse = np.mean((image_predicted.astype(np.float64) - image_truth.astype(np.float64)) ** 2)
        if mse == 0:
            return float('inf')
        return 10 * math.log10(m * m / mse)

    @staticmethod
    def __ssim(image_p: np.array, image_t:  np.array, data_range=255):
        image_p = image_p.astype(np.float64)
        image_t = image_t.astype(np.float64)

        c1 = (0.01 * data_range) ** 2
        c2 = (0.03 * data_range) ** 2
        c3 = c2 / 2

        kernel = cv2.getGaussianKernel(11, 1.5)
        window = np.outer(kernel, kernel.transpose())

        mu1 = cv2.filter2D(image_p, -1, window)[5:-5, 5:-5]  # valid
        mu2 = cv2.filter2D(image_t, -1, window)[5:-5, 5:-5]
        mu1_sq = mu1 ** 2
        mu2_sq = mu2 ** 2
        mu1_mu2 = mu1 * mu2
        sigma1_sq = cv2.filter2D(image_p ** 2, -1, window)[5:-5, 5:-5] - mu1_sq
        sigma2_sq = cv2.filter2D(image_t ** 2, -1, window)[5:-5, 5:-5] - mu2_sq
        sigma12 = cv2.filter2D(image_p * image_t, -1, window)[5:-5, 5:-5] - mu1_mu2

        ssim_map = ((2 * mu1_mu2 + c1) * (2 * sigma12 + c2)) / ((mu1_sq + mu2_sq + c1) *
                                                                (sigma1_sq + sigma2_sq + c2))
        return ssim_map.mean()

    @staticmethod
    def ssim(image_predicted: np.array, image_truth:  np.array, boarder=0, data_range=255):
        """ Structural SIMilarity
        image must be c * h * w or h * w
        """
        if image_predicted.shape != image_predicted.shape:
            raise TypeError('Input images must have the same dimensions')
        if image_predicted.ndim not in (2, 3):
            raise TypeError('Input images must have 2 or 3 dimensions')
        if image_predicted.ndim == 2:
            image_truth = np.unsqeeze(image_truth, axis=0)
            image_predicted = np.unsqeeze(image_predicted, axis=0)
        c, h, w = image_predicted.shape
        ssims = []
        for i in range(c):
            ssims.append(ImageQuality.__ssim(image_predicted[i, :, :], image_truth[i, :, :]))
        return np.array(ssims).mean()


if __name__ == "__main__":
    ImageUtils.calc_mean_std(256, 256, ["./dataset/raw/open", "./dataset/raw/close"], 1000)
