# coding=utf-8
import cv2
import math
import os
import sys
import logging

logging.basicConfig(
    level=logging.DEBUG,
    stream=sys.stdout,
    format="%(asctime)s %(levelname)s %(filename)s:L%(levelno)s %(message)s"
)

maxWidth = 256
maxThumbnailBytes = 50 * 1024

def getDocBytes(path):
    try:
        size = os.path.getsize(path)
        return size
    except Exception as err:
        logging.error(err)

def thumbnailPath(path):
    p, s = os.path.splitext(path)
    return "%s.thumbnail%s" % (p,s)


def scaleImg(imgPath, scaleFactor=0):
    img = cv2.imread(imgPath, cv2.IMREAD_UNCHANGED)
    imgWidth = img.shape[1]
    imgHeight = img.shape[0]

    # 去掉Alpha通道 (其实可以读取的时候，不读取就可以了)
    # if len(img.shape) > 2 and img.shape[2] == 4:
    #    img = cv2.cvtColor(img, cv2.COLOR_BGRA2BGR)

    if scaleFactor == 0:
        scaleFactor = maxWidth * 100/imgWidth if imgWidth > maxWidth else 100
    width = int(scaleFactor * imgWidth / 100)
    height = int(scaleFactor * imgHeight / 100)

    dim = (width, height)
    resized = cv2.resize(img, dim, interpolation = cv2.INTER_AREA)
    cv2.imshow("Resized image", resized)

    newImgPath = thumbnailPath(imgPath)
    imgBytes = getDocBytes(imgPath)
    qualityJPEG = int(maxThumbnailBytes * 100 / imgBytes) if imgBytes > maxThumbnailBytes else 95
    compressionPNG = int(9 - math.floor(maxThumbnailBytes / imgBytes)) if imgBytes > maxThumbnailBytes else 3
    print(qualityJPEG, compressionPNG)
    if False == cv2.imwrite(newImgPath, resized, [cv2.IMWRITE_PNG_COMPRESSION, compressionPNG, cv2.IMWRITE_JPEG_QUALITY, qualityJPEG]):
        raise Exception("Could not write image: %s" % (newImgPath))

    #cv2.waitKey(0)
    cv2.destroyAllWindows()

def makeThumbnails(dirPath):
    files = os.listdir(dirPath)
    for f in files:
        p, s = os.path.splitext(f)
        if len(s) > 0 and s.lower() in (".jpeg", ".jpg", ".png") and not p.endswith("thumbnail"):
            logging.debug("%s", f)
            scaleImg(os.path.join(dirPath, f))

#scaleImg('../../jupiter/shadow/resource/turtle-tortoise-swim-sea-turtle-preview.jpg') 
makeThumbnails('../../jupiter/shadow/resource')
