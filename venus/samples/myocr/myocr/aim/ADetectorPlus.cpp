//
// Created by jiao on 2020/10/1.
//

#include "aim/ADetector.h"
#include "aim/utils.h"
#include <cmath>
#include <include/common.h>

int ADetector::_imgBinary(cv::Mat& imgMat, cv::Mat& imgOut, int ksizeMedia, int ksizeBlur) {
    cv::cvtColor(imgMat, imgOut, cv::COLOR_RGBA2GRAY);
    cv::GaussianBlur(imgOut, imgOut, cv::Size(ksizeBlur, ksizeBlur), 0);
    cv::adaptiveThreshold(imgOut, imgOut, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 5, 2);
    cv::medianBlur(imgOut, imgOut, ksizeMedia);
    return 0;
}

int ADetector::detectShotLineGray(const cv::Mat& imgMat, const cv::Rect& rectRoI, const cv::Vec3f& whiteBall, float& whiteLineAngle, int minLineLength/* = 50 */) {
    int imgW = imgMat.cols;
    int imgH = imgMat.rows;
    cv::Mat imgRoI = imgMat(rectRoI);

    cv::cvtColor(imgRoI, imgRoI, cv::COLOR_RGBA2GRAY);
    cv::GaussianBlur(imgRoI, imgRoI, cv::Size(3, 3), 0);
    cv::adaptiveThreshold(imgRoI, imgRoI, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 5, 2);
    cv::medianBlur(imgRoI, imgRoI, 5);
    cv::bitwise_not(imgRoI, imgRoI);

    std::vector<cv::Vec4i> lines;
    cv::HoughLinesP(imgRoI, lines, 1, CV_PI / 180, minLineLength, minLineLength, 1);

    cv::Point ballCenter(whiteBall[0], whiteBall[1]);
    std::vector<cv::Vec3f> linesCrossWhite;
    for (auto line : lines) {
        cv::Point a(line[0], line[1]);
        cv::Point b(line[2], line[3]);
        cv::Point s(0, 0);
        cv::Point e(0, 0);
        if (cv::norm(a - ballCenter) < whiteBall[2]) {
            s = a;
            e = b;
        }
        else if (cv::norm(b - ballCenter) < whiteBall[2]) {
            s = b;
            e = a;
        }
        else {
            continue;
        }

        float angle = (float)std::atan2(e.y - s.y, e.x - s.x) * 180.0 / CV_PI;
        linesCrossWhite.push_back(cv::Vec3f(s.x, s.y, angle));
    }

    std::vector<float> angles;
    std::for_each(std::begin(linesCrossWhite), std::end(linesCrossWhite), [&](const cv::Vec3f& d) {
        angles.push_back(d[2]);
    });
    return avgMajority(angles, whiteLineAngle, 1);
}

int ADetector::detectWhiteBall(const cv::Mat& imgBin, int radiusBall, cv::Vec3f& whiteBall, cv::Vec3f& whiteCircle) {
    cv::Mat imgRoI = imgBin.clone();
    // morphology
    auto kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(7, 7));
    cv::morphologyEx(imgRoI, imgRoI, cv::MORPH_CLOSE, kernel);
    cv::morphologyEx(imgRoI, imgRoI, cv::MORPH_OPEN, kernel);
    auto kernelZ = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    cv::erode(imgRoI, imgRoI, kernelZ, cv::Point(-1, -1), 1);
    // circles
    whiteBall = cv::Vec3f(0, 0, 0);
    std::vector <cv::Vec3f> circles;
    cv::HoughCircles(imgRoI, circles, cv::HOUGH_GRADIENT, 1, radiusBall * 1.6, 50, 10,
        radiusBall * 0.6, radiusBall * 1.2);
    //cv::HoughCircles(imgRoI, circles, cv::HOUGH_GRADIENT_ALT, 1.5, radiusBall * 1.6, 300, 0.4, radiusBall * 0.6, radiusBall * 1.2);
    for (auto circle : circles) {
        if (circle[2] > whiteBall[2]) {
            whiteBall = circle;
        }
    }
    return 0;
}

int ADetector::isMyRound(cv::Mat& imgRaw) {
    auto rectRoundRoI = cv::Rect(mRectValid.x + 10, 0, (mRectRoI.x - mRectValid.x) / 2, mRectValid.height);
    if (imgRaw.cols < imgRaw.rows) {
        rectRoundRoI = cv::Rect(0, mRectValid.x + 10, mRectValid.height, (mRectRoI.x - mRectValid.x) / 2);
    }

    // isMyRound
    if (!_isMyRound(imgRaw, rectRoundRoI, 3, 0.5)) {
        LOGE("isMyRound error: w=%d h=%d, rectValid.x=%d rectRoI.x=%d", imgRaw.cols, imgRaw.rows, mRectValid.x, mRectRoI.x);
        return -1;
    }

    return 0;
}

bool ADetector::_isMyRound(cv::Mat& imgMat, const cv::Rect& rectRoI, int errorTheta, float scaleFactor) {
    auto startStamp = cv::getTickCount();
    // preprocess
    cv::Mat imgRoI = imgMat(rectRoI);
    if (scaleFactor != 1) {
        cv::resize(imgRoI, imgRoI, cv::Size(), scaleFactor, scaleFactor, cv::INTER_LINEAR);
    }
    cv::cvtColor(imgRoI, imgRoI, cv::COLOR_RGBA2GRAY);
    // arguments
    auto roiH = std::max(imgRoI.cols, imgRoI.rows);
    auto roiW = std::min(imgRoI.cols, imgRoI.rows);
    auto kernelSize = int(7 * scaleFactor) > 3 ? int(7 * scaleFactor) : 3;
    kernelSize = kernelSize % 2 == 1 ? kernelSize : kernelSize + 1;
    cv::GaussianBlur(imgRoI, imgRoI, cv::Size(kernelSize, kernelSize), 0);
    //cv::adaptiveThreshold(imgRoI, imgRoI, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 3, 2);
    cv::Canny(imgRoI, imgRoI, 20, 50, 3);
    // dilate
    auto kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(kernelSize, kernelSize));
    cv::dilate(imgRoI, imgRoI, kernel, cv::Point(-1, -1), 1);
    // contours
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(imgRoI, contours, cv::noArray(), cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    // check
    auto bHasAsideBound = false;
    for (auto cont : contours) {
        auto rectMinArea = cv::minAreaRect(cont);
        auto theta = int(abs(rectMinArea.angle)) % 90;
        auto rectSize = rectMinArea.size;
        if ((theta < errorTheta) || (theta > 45 and errorTheta > 90 - theta)) {
            auto height = std::max(rectSize.height, rectSize.width);
            auto width = std::min(rectSize.height, rectSize.width);
            if (height > roiH / 4 && height < roiH * 3 / 4 && width > roiW / 4) {
                LOGD("ADetector::isMyRound check %s ok: cont=(%.2f,%.2f,%.2f), roi=(%d,%d)", mSceneKey.c_str(), rectSize.width, rectSize.height, rectMinArea.angle, roiW, roiH);
                bHasAsideBound = true;
                break;
            }
        }
        LOGD("ADetector::isMyRound check %s fail: cont=(%.2f,%.2f,%.2f), roi=(%d,%d)", mSceneKey.c_str(), rectSize.width, rectSize.height, rectMinArea.angle, roiW, roiH);
    }
    LOGD("ADetector::isMyRound %s rst=%d: roi=(%d,%d,%d,%d) raw=(%d,%d) %.4fs", mSceneKey.c_str(), bHasAsideBound, rectRoI.x, rectRoI.y, rectRoI.width, rectRoI.height, imgMat.cols, imgMat.rows, (cv::getTickCount() - startStamp) / cv::getTickFrequency());
    return bHasAsideBound;
}

int ADetector::_detectWhiteLines(const cv::Mat& imgBin, cv::Point2f wball, cv::Point2f& wcircle, float rball, float& aShot, float& aHit, float& aReflect) {
    std::vector<cv::Vec4f> lines;
    float minLen = rball * 2;
    cv::HoughLinesP(imgBin, lines, 1, CV_PI / 180, minLen * 0.36, minLen, 1);

    float angleShot = 0, angleHit = 0, angleReflect = 0;
    int nRet = _checkLineInCircle(lines, wball, rball * 2, angleShot);
    if (nRet == 0) {
        aShot = angleShot;
    }

    std::vector<cv::Point2f> linesCross;
    for (auto iter = lines.begin(); iter != lines.end(); iter++) {
        auto line = *iter;
        cv::Point2f a(line[0], line[1]);
        cv::Point2f b(line[2], line[3]);
        if (cv::norm(a - wcircle) < 3) {
            linesCross.push_back(a);
        }
        else if (cv::norm(b - wcircle) < 3) {
            linesCross.push_back(b);
        }
    }
    float sx = wcircle.x, sy = wcircle.y;
    for (auto item : linesCross) {
        sx += item.x;
        sy += item.y;
    }
    //wcircle.x = sx / (linesCross.size() + 1);
    //wcircle.y = sy / (linesCross.size() + 1);


    nRet = _checkLineInCircle(lines, wcircle, rball * 1.2, angleReflect, &angleShot);
    if (nRet == 0) {
        aReflect = angleReflect;
    }

    nRet = _checkLineInCircle(lines, wcircle, rball * 3, angleHit, &angleShot);
    if (nRet == 0) {
        aHit = angleHit;
    }

    return 0;
}

int ADetector::_checkLineInCircle(std::vector<cv::Vec4f>& lines, cv::Point2f pc, float pr, float& angle, const float* angleFilter) {
    std::vector<cv::Vec3f> linesCross;
    for (auto iter = lines.begin(); iter != lines.end(); ) {
        auto line = *iter;
        cv::Point2f a(line[0], line[1]);
        cv::Point2f b(line[2], line[3]);
        cv::Point s(0, 0);
        cv::Point e(0, 0);
        if (cv::norm(a - pc) < pr) {
            s = a;
            e = b;
        }
        else if (cv::norm(b - pc) < pr) {
            s = b;
            e = a;
        }
        else {
            iter++;
            continue;
        }
        float angle = (float)std::atan2(e.y - s.y, e.x - s.x) * 180.0 / CV_PI;
        if (angleFilter != nullptr && abs(abs(*angleFilter - angle) - 180) < 2) {
            iter = lines.erase(iter);
            continue;
        }
        linesCross.push_back(cv::Vec3f(s.x, s.y, angle));
        iter = lines.erase(iter);
    }
    if (linesCross.size() < 1) {
        return -1;
    }
    std::for_each(std::begin(linesCross), std::end(linesCross), [&](const cv::Vec3f& d) {
        angle += d[2];
    });
    angle = angle / linesCross.size();
    return 0;
}

int ADetector::_detectWhiteBall(const cv::Mat& imgBin, float radiusBall, cv::Vec3f& whiteBall, cv::Vec3f& whiteCircle, float threshold) {
    const cv::Mat& imgRoI = imgBin;

    std::vector<cv::Vec3f> circles;
    cv::HoughCircles(imgRoI, circles, cv::HOUGH_GRADIENT, 1, radiusBall * 1.8, 50.0, 10.0, int(radiusBall * 0.6), int(radiusBall * 1.3));
    // cv::HoughCircles(imgRoI, circles, cv::HOUGH_GRADIENT_ALT, 1.5, radiusBall * 1.6, 300, 0.4, radiusBall * 0.6, radiusBall * 1.2);
    if (circles.size() < 2) {
        return -1;
    }

    // wcircle && wball
    cv::Vec3f bC;
    cv::Vec3f sC;
    for (auto cir : circles) {
        if (cir[2] > bC[2]) {
            sC = bC;
            bC = cir;
            continue;
        }
        if (cir[2] > sC[2]) {
            sC = cir;
            continue;
        }
    }
    std::vector<cv::Vec3f> cirs;
    cirs.push_back(bC);
    cirs.push_back(sC);
    for (auto cir : cirs) {
        if (cir[2] <= 0) {
            continue;
        }
        auto xa = int(cir[0] + (cir[2] / 2));
        auto ya = int(cir[1] + (cir[2] / 2));
        auto xb = int(cir[0] - (cir[2] / 2));
        auto yb = int(cir[1] - (cir[2] / 2));
        auto sum = int(imgRoI.at<uchar>(ya, xa)) + int(imgRoI.at<uchar>(ya, xb)) + int(imgRoI.at<uchar>(yb, xa)) + int(imgRoI.at<uchar>(yb, xb));
        if (sum >= 128 * 4) {
            whiteBall = cir;
        }
        else if (sum < 128 * 4) {
            whiteCircle = cir;
        }
        LOGD("ADetector::_detectWhiteBall circle - (%.2f,%.2f,%.2f), sum=%d (%d,%d,%d,%d)", cir[0], cir[1], cir[2], sum, int(imgRoI.at<uchar>(ya, xa)), int(imgRoI.at<uchar>(ya, xb)), int(imgRoI.at<uchar>(yb, xa)), int(imgRoI.at<uchar>(yb, xb)));
    }
    return 0;
}

/// deprecated
/// ///////////////////////////////////////////////////////////////////////////
///
int ADetector::detectWhiteInfo(const cv::Mat& imgGray, float radiusBall, cv::Point2f& wball, cv::Point2f& wcircle, float& shotAngle) {
    auto stampA = cv::getTickCount();

    cv::Mat imgBin;
    cv::threshold(imgGray, imgBin, 200, 256, cv::THRESH_BINARY);

    int scaleFactor = 1;
    auto imgRoIMask = _detectWhiteRoI(imgBin, 7);
    if (imgRoIMask.empty()) {
        return -1;
    }

    auto stampB = cv::getTickCount();
    cv::Vec3f whiteBall = cv::Vec3f(0, 0, 0);
    cv::Vec3f whiteCircle = cv::Vec3f(0, 0, 0);
    if (0 > _detectWhiteBall(imgRoIMask, radiusBall / scaleFactor, whiteBall, whiteCircle, (float)2 / scaleFactor)) {
        return -2;
    }

    if (whiteBall[2] < 1) {
        return -3;
    }

    auto stampC = cv::getTickCount();
    if (0 > _detectShotLine(imgRoIMask, whiteBall, shotAngle, int(radiusBall * 2.5 / scaleFactor))) {
        return -4;
    }

    wball = cv::Point2f(whiteBall[0] * scaleFactor, whiteBall[1] * scaleFactor);
    wcircle = cv::Point2f(whiteCircle[0] * scaleFactor, whiteCircle[1] * scaleFactor);
    auto stampD = cv::getTickCount();
    LOGD("ADetector::detectWhiteInfo total=%.4fs cut_roi=%.4fs  circles=%.4fs shotLine=%.4fs",
        (stampD - stampA) / cv::getTickFrequency(), (stampB - stampA) / cv::getTickFrequency(),
        (stampC - stampB) / cv::getTickFrequency(), (stampD - stampC) / cv::getTickFrequency());
    return 0;
}

int ADetector::_detectShotLine(const cv::Mat& imgBin, const cv::Vec3f& whiteBall, float& shotAngle, int minLength /*=50*/) {
    //> canny �� threshold �����Զ� HoughLinesP ����Ч��
    // cv::Canny(imgRoI, imgRoI, 1, 60, 3);
    // or
    // cv::threshold(imgRoI, imgRoI, 200, 256, cv::THRESH_BINARY);
    const cv::Mat& imgRoI = imgBin;

    std::vector<cv::Vec4i> lines;
    cv::HoughLinesP(imgRoI, lines, 2, CV_PI / 180, minLength * 0.86, minLength, 2);

    cv::Point ballCenter(whiteBall[0], whiteBall[1]);
    std::vector<cv::Vec3f> linesCrossWhite;
    for (auto line : lines) {
        cv::Point a(line[0], line[1]);
        cv::Point b(line[2], line[3]);
        cv::Point s(0, 0);
        cv::Point e(0, 0);
        if (cv::norm(a - ballCenter) < 2 * whiteBall[2]) {
            s = a;
            e = b;
        }
        else if (cv::norm(b - ballCenter) < 2 * whiteBall[2]) {
            s = b;
            e = a;
        }
        else {
            continue;
        }

        float angle = (float)std::atan2(e.y - s.y, e.x - s.x) * 180.0 / CV_PI;
        linesCrossWhite.push_back(cv::Vec3f(s.x, s.y, angle));
    }
    if (linesCrossWhite.size() < 1) {
        return -1;
    }

    std::vector<float> angles;
    std::for_each(std::begin(linesCrossWhite), std::end(linesCrossWhite), [&](const cv::Vec3f& d) {
        angles.push_back(d[2]);
    });
    return avgMajority(angles, shotAngle, 1);
}

cv::Rect ADetector::_detectRectRoI(const cv::Mat& matRaw, int ksizeBlur, int ksizeMedia, int tolerance/*=3*/) {
    cv::Mat matImg = matRaw.clone();
    cv::cvtColor(matImg, matImg, cv::COLOR_RGBA2GRAY);
    cv::GaussianBlur(matImg, matImg, cv::Size(ksizeBlur, ksizeBlur), 0);
    cv::adaptiveThreshold(matImg, matImg, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 5, 2);
    cv::medianBlur(matImg, matImg, ksizeMedia);

    cv::Canny(matImg, matImg, 10, 150, 5);

    std::vector<cv::Vec4i> lines;
    auto minLineLength = std::min(matRaw.cols, matRaw.rows) / 3;
    cv::HoughLinesP(matImg, lines, 1, CV_PI / 180, minLineLength * 0.3, minLineLength, 3);

    int imgH = matImg.rows;
    int imgW = matImg.cols;
    int lX = 0, rX = 2 << 20, tY = 0, bY = 2 << 20;
    for (auto& line : lines) {
        cv::Point a(line[0], line[1]);
        cv::Point b(line[2], line[3]);
        if (abs(a.x - b.x) <= tolerance) {
            int xn = round((a.x + b.x) / 2);
            if (xn < imgW / 3 && lX < xn) {
                lX = xn;
            }
            else if (xn > imgW / 2 && rX > xn) {
                rX = xn;
            }
        }
        else if (abs(a.y - b.y) <= tolerance) {
            int yn = round((a.y + b.y) / 2);
            if (yn < imgH / 3 && tY < yn) {
                tY = yn;
            }
            else if (yn > imgH * 2 / 3 && bY > yn) {
                bY = yn;
            }
        }
    }
    if (lX == 0 || tY == 0 || bY == 2 << 20 || rX == 2 << 20) {
        return cv::Rect();
    }
    auto rectRoI = cv::Rect(std::min(lX, rX), std::min(bY, tY), std::abs(lX - rX), std::abs(tY - bY));
    return rectRoI.width > rectRoI.height ? rectRoI : cv::Rect(rectRoI.y, imgW - rectRoI.x - rectRoI.width, rectRoI.height, rectRoI.width);
}