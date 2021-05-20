//
// Created by jiao on 2020/10/30
//

#include "ADetector.h"
#include "utils.h"
#include "../include/common.h"
#include <cmath>
#include <opencv2/opencv.hpp>

int ADetector::_detectFrame(cv::Mat& mat) {
    if (mat.empty()) {
        return -1;
    }
    cv::Mat imgGray;
    cv::cvtColor(mat, imgGray, cv::COLOR_RGB2GRAY);
    cv::GaussianBlur(imgGray, imgGray, cv::Size(5, 5), 0);
    auto rectValid = _detectRectValid(imgGray);
    auto rectRoI = _detectRectRoI(imgGray, 5, 3);
    if ((rectRoI.x * rectRoI.y != 0) && (rectRoI.width * rectRoI.height != 0)) {
        if (abs(rectValid.x - mRectValid.x) < 10 && mRectValid.width > 0) {
            rectValid = mRectValid;
        }
        _detectFrameDefault(rectValid);
        if (abs(rectRoI.width - mRectRoI.width) > rectRoI.width / 100 || abs(rectRoI.height - mRectRoI.height) > rectRoI.height / 100) {
            mRectRoI.width = rectRoI.width;
            mRectRoI.height = rectRoI.height;
        }
        if (abs(rectRoI.x - mRectRoI.x) > rectRoI.x / 50) {
            mRectRoI.x = rectRoI.x;
        }
        if (abs(rectRoI.y - mRectRoI.y) > rectRoI.y / 50) {
            mRectRoI.y = rectRoI.y;
        }

        if (mRectRoI.width % 2 != 0) {
            mRectRoI.x -= 1;
            mRectRoI.width += 1;
        }
        if (mRectRoI.height % 2 != 0) {
            mRectRoI.y -= 1;
            mRectRoI.height += 1;
        }
    }
    int nRet = ((rectRoI.x * rectRoI.y != 0) && (rectRoI.width * rectRoI.height != 0)) ? 0 : -1;
    LOGD("ADetector::_detectFrame %s: bar=%d roi_new=(%d,%d,%d,%d) roi=(%d,%d,%d,%d) rectValid=(%d,%d,%d,%d)",
        nRet == 0 ? "ok" : "error", mScreenBarH, rectRoI.x, rectRoI.y, rectRoI.width, rectRoI.height,
        mRectRoI.x, mRectRoI.y, mRectRoI.width, mRectRoI.height, rectValid.x, rectValid.y, rectValid.width, rectValid.height);
    return nRet;
}

int ADetector::_detectFrameDefault(const cv::Rect& rectValid) {
    mRectValid = rectValid;
    if (mRectValid.empty() || mRectValid.width == 0 || mRectValid.height == 0) {
        mRectValid = cv::Rect(mScreenBarH, 0, mScreenW - mScreenBarH, mScreenH);
    }

    // desktop: rotation = 90 or 270
    int top = std::round(((float)mScreenH) * 0.237037f);
    int bottom = std::round(((float)mScreenH) * 0.912963f);
    mRectRoI = cv::Rect();
    mRectRoI.width = (bottom - top) * 2;
    mRectRoI.y = top;
    mRectRoI.height = bottom - top;
    mRectRoI.x = mRectValid.x + ((mRectValid.width - mRectRoI.width) / 2);

    // radius for ball
    mRadiusBall = std::round((float)mRectRoI.height * 0.03f);
    mRadiusCircle = (mRadiusBall * 12) / 11;

    // hole
    mRadiusHoleB = std::round((float)mRectRoI.height * 0.073973f);
    mRadiusHoleS = std::round((float)mRectRoI.height * 0.063f);

    // 空洞口: 洞口的几个坐标：四个顶点的洞口边界在半径为 mRadiusHoleB 的圆上
    mHoleMouthLx = mRectRoI.x + mRadiusHoleB;
    mHoleMouthRx = mRectRoI.x + mRectRoI.width - mRadiusHoleB;
    mHoleMouthTy = mRectRoI.y + mRadiusHoleB;
    mHoleMouthBy = mRectRoI.y + mRectRoI.height - mRadiusHoleB;
    mHoleMouthMxl = (mRectRoI.width / 2) + mRectRoI.x - mRadiusHoleS;
    mHoleMouthMxr = (mRectRoI.width / 2) + mRectRoI.x + mRadiusHoleS;

    return 0;
}

cv::Rect ADetector::_detectRectValid(const cv::Mat& imgRawGray) {
    cv::Mat imgBin = imgRawGray.clone();
    //
    double pixMin, pixMax, pixSample = int(imgBin.at<uint8_t >(10, 10));
    cv::minMaxLoc(imgBin, &pixMin, &pixMax);
    //-- binary
    cv::threshold(imgBin, imgBin, 8, 256, cv::THRESH_BINARY_INV);
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
    cv::morphologyEx(imgBin, imgBin, cv::MORPH_OPEN, kernel);
    // contours
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(imgBin, contours, cv::noArray(), cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    // check
    auto imgMax = std::max(imgBin.cols, imgBin.rows);
    auto imgMin = std::min(imgBin.cols, imgBin.rows);
    cv::Rect rectValid;
    for (auto i = 0; i < contours.size(); i++) {
        auto rectBound = cv::boundingRect(contours[i]);
        auto rectW = std::max(rectBound.width, rectBound.height);
        auto rectH = std::min(rectBound.width, rectBound.height);
        if (rectH < imgMax * 0.05 && rectW > imgMin * 0.8) {
            rectValid = rectBound;
            break;
        }
    }
    cv::Rect rectRst;
    if (imgBin.cols > imgBin.rows) {
        auto imgBlackW = std::min(rectValid.width, rectValid.height);
        rectRst = rectValid.x < imgMax / 2 ? cv::Rect(imgBlackW, 0, imgMax - imgBlackW, imgMin) : cv::Rect(0, 0, imgMax - imgBlackW, imgMin);
    }
    else {
        auto imgBlackW = std::min(rectValid.width, rectValid.height);
        rectRst = rectValid.y < imgMax / 2 ? cv::Rect(0, imgBlackW, imgMin, imgMax - imgBlackW) : cv::Rect(0, 0, imgMin, imgMax - imgBlackW);
    }
    LOGD("ADetector::_detectRectValid contours(%d): rst=(%d,%d,%d,%d), pix=(%d, %d), sample=%d", contours.size(), rectValid.x, rectValid.y, rectValid.height, rectValid.width, int(pixMin), int(pixMax), int(pixSample));
    return rectRst;
}

cv::Rect ADetector::_detectRectRoI(const cv::Mat& imgRawGray, int ksizeMedia, int tolerance/*=3*/) {
    cv::Mat matImg = imgRawGray.clone();
    cv::adaptiveThreshold(matImg, matImg, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 5, 2);
    cv::medianBlur(matImg, matImg, ksizeMedia);

    auto kernelSize = std::min(imgRawGray.rows, imgRawGray.cols) * .2;
    auto matLineHorizon = matImg.clone();
    matLineHorizon = 255 - matLineHorizon;
    cv::Mat horizonStructure = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(kernelSize, 1));
    cv::erode(matLineHorizon, matLineHorizon, horizonStructure, cv::Point(-1, -1));
    cv::dilate(matLineHorizon, matLineHorizon, horizonStructure, cv::Point(-1, -1));
    auto matLineVertical = matImg.clone();
    matLineVertical = 255 - matLineVertical;
    cv::Mat verticalStructure = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(1, kernelSize));
    cv::erode(matLineVertical, matLineVertical, verticalStructure, cv::Point(-1, -1));
    cv::dilate(matLineVertical, matLineVertical, verticalStructure, cv::Point(-1, -1));

    int rMin = 0, rMax = matLineHorizon.rows, threshold = kernelSize * 2;
    for (auto r = 0; r < matLineHorizon.rows; r++) {
        if (cv::countNonZero(matLineHorizon.row(r)) < threshold) {
            continue;
        }
        if (r < matLineHorizon.rows / 3 && r > rMin) {
            rMin = r;
            continue;
        }
        if (r > matLineHorizon.rows * 2 / 3 && r < rMax) {
            rMax = r;
            break;
        }
    }
    int cMin = 0, cMax = matLineVertical.cols;
    for (auto c = 0; c < matLineVertical.cols; c++) {
        if (cv::countNonZero(matLineVertical.col(c)) < threshold) {
            continue;
        }
        if (c < matLineVertical.cols / 3 && c > cMin) {
            cMin = c;
            continue;
        }
        if (c > matLineVertical.cols * 2 / 3 && c < cMax) {
            cMax = c;
            break;
        }
    }

    auto rectRoI = cv::Rect(cMin, rMin, cMax - cMin, rMax - rMin);
    if (rectRoI.width < rectRoI.height) {
        rectRoI = cv::Rect(rectRoI.y, imgRawGray.cols - rectRoI.x - rectRoI.width, rectRoI.height, rectRoI.width);
    }
    return rectRoI;
}
