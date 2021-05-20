#include "aim/ADetector.h"
#include "aim/utils.h"
#include <cmath>
#include <numeric>
#include "include/common.h"

bool gDrawForDebug = true;

int fitWhiteLineByPoints(std::vector<cv::Point>& points, cv::Vec3f& line) {
    cv::Vec4f line_para;
    cv::fitLine(points, line_para, cv::DIST_L2, 0, 1e-2, 1e-2);

    // line_para: a*x+b*y+c = 0
    cv::Point point0;
    auto a = -line_para[0];
    auto b = line_para[1];
    auto x = line_para[3];
    auto y = line_para[2];
    auto c = -(a * x + b * y);
    if (std::abs(a) < 1e-5) {
        a = 0;
    }
    if (std::abs(b) < 1e-5) {
        b = 0;
    }
    if (std::abs(c) < 1e-5) {
        c = 0;
    }
    line = cv::Vec3f(a, b, c);
    return 0;
}

//
// fitWhiteLine:
//     return : line = (a,b,c), ax + by + c = 0
//
int fitWhiteLine(const cv::Mat& imgBin, cv::Vec3f& line) {
    cv::Mat imageThin(imgBin.size(), CV_32FC1);
    cv::distanceTransform(imgBin, imageThin, cv::DIST_L2, 3);


    double maxValue, minValue = 0;
    cv::minMaxLoc(imageThin, &minValue, &maxValue);
    int nonZerosNum = cv::countNonZero(imageThin);
    double nonZerosSum = cv::sum(imageThin)[0];
    double thresholdValue = nonZerosNum == 0 ? 0 : (nonZerosSum / nonZerosNum) * 1.05;
    thresholdValue = thresholdValue >= maxValue ? maxValue * .9 : thresholdValue;


    std::vector<cv::Point> points;
    for (int i = 0; i < imageThin.rows; i++)
    {
        const float* rowData = imageThin.ptr<float>(i);
        for (int j = 0; j < imageThin.cols; j++)
        {
            // imageThin.ptr<float>(i) is more effective than imageThin.at<float>(i, j)
            if (rowData[j] != 0 && rowData[j] >= maxValue) {
                points.push_back(cv::Point(i, j));
            }
        }
    }
    fitWhiteLineByPoints(points, line);
    return 0;
}

//
// crossPoint2Line:
//     return : line = (a,b,c), ax + by + c = 0
//
int crossPoint2Line(cv::Vec3f la, cv::Vec3f lb, cv::Point2f& cross) {
    float a = la[0], b = la[1], c = la[2];
    float A = lb[0], B = lb[1], C = lb[2];
    if ((a == 0 && b == 0 && c == 0) || (A == 0 && B == 0 && C == 0)) {
        return -1;
    }
    if ((a == 0 && A == 0) || (b == 0 && B == 0)) {
        return -2;
    }
    if ((b && B) && (std::abs(a / b - A / B) < 1e-2)) {
        return -3;
    }
    cross.x = (b * C - B * c) / (B * a - b * A);
    cross.y = (a * C - A * c) / (A * b - a * B);
    return 0;
}

//
// checkRoIMask : must got wball
//
int checkRoIMask(const cv::Mat& imgBin, double radiusBall, cv::Vec3f& lineHit, cv::Point2f& wball, cv::Mat& maxContour) {
    int maxContLen = radiusBall * 2, maxContIndex = -1, maxContHit = -1;
    auto contours = std::vector<std::vector<cv::Point>>();
    cv::findContours(imgBin, contours, cv::noArray(), cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
    for (auto i = 0; i < contours.size(); i++) {
        cv::RotatedRect rotatedRect = cv::minAreaRect(contours[i]);
        auto w = std::min(rotatedRect.size.height, rotatedRect.size.width);
        auto h = std::max(rotatedRect.size.height, rotatedRect.size.width);
        if (h > radiusBall * 2 * 0.8 && h < radiusBall * 2 * 1.2 && h < w * 1.3) {
            wball = cv::Point2f(rotatedRect.center.x, rotatedRect.center.y);
            auto wRadius = (w + h) / 4;
            continue;
        }

        if (w > 1 && w < 10 && h / w > 10 && w * h > maxContHit) {
            maxContHit = w * h;
            cv::Mat imgCurRoI = cv::Mat::zeros(imgBin.rows, imgBin.cols, CV_8UC1);
            cv::drawContours(imgCurRoI, contours, i, cv::Scalar(255), -1);
            fitWhiteLine(imgCurRoI, lineHit);
            continue;
        }

        auto contLen = cv::arcLength(contours[i], false);
        if (contLen > maxContLen) {
            maxContLen = contLen;
            maxContIndex = i;
        }
    }

    if (maxContIndex >= 0) {
        maxContour = cv::Mat::zeros(imgBin.rows, imgBin.cols, CV_8UC1);
        cv::drawContours(maxContour, contours, maxContIndex, cv::Scalar(255), -1);
    }
    if (wball.x == 0 && wball.y == 0) {
        return -1;
    }
    return 0;
}

int checkMaxContour(cv::Mat& imgBinMaxContour, double radiusBall, cv::Point2f& wcircle, std::vector<cv::Vec3f>& lines) {
    cv::Vec3f whiteCircle = cv::Vec3f(0, 0, 0);
    std::vector<cv::Vec3f> circles;
    cv::HoughCircles(imgBinMaxContour, circles, cv::HOUGH_GRADIENT, 1, radiusBall * 1.8, 50.0, 10.0, int(radiusBall * 0.6), int(radiusBall * 1.3));
    if (circles.size() != 1) {
        return -1;
    }
    wcircle = cv::Point2f(circles[0][0], circles[0][1]);
    cv::circle(imgBinMaxContour, wcircle, circles[0][2] * 1.1, cv::Scalar(0), -1);
    // check lines
    cv::Mat imgCurRoI = cv::Mat::zeros(imgBinMaxContour.rows, imgBinMaxContour.cols, CV_8UC1);
    auto contours = std::vector<std::vector<cv::Point>>();
    cv::findContours(imgBinMaxContour, contours, cv::noArray(), cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
    for (auto i = 0; i < contours.size(); i++) {
        cv::RotatedRect rotatedRect = cv::minAreaRect(contours[i]);
        auto w = std::min(rotatedRect.size.height, rotatedRect.size.width);
        auto h = std::max(rotatedRect.size.height, rotatedRect.size.width);
        if (w > 1 && h / w > 5 && w < 10) {
            imgCurRoI = 0;
            cv::drawContours(imgCurRoI, contours, i, cv::Scalar(255), -1);
            cv::Vec3f line;
            if (fitWhiteLine(imgCurRoI, line) == 0) {
                lines.push_back(line);
            }
        }
    }
    return 0;
}

int verifyChecked(cv::Point2f& wball, cv::Point2f& wcircle, cv::Vec3f& lineShot, cv::Vec3f& lineHit, cv::Vec3f& lineReflect, float& aShot) {
    std::vector<cv::Point2f> crossPoints(3);
    crossPoint2Line(lineShot, lineHit, crossPoints[0]);
    crossPoint2Line(lineShot, lineReflect, crossPoints[1]);
    crossPoint2Line(lineHit, lineReflect, crossPoints[2]);
    // wcircle, sometimes we can't correct the center of wcircle, because no lines
    float avgPointX = 0, avgPointY = 0;
    int avgPointCnt = 0;
    for (auto iter : crossPoints) {
        if (iter.x == 0 && iter.y == 0) {
            continue;
        }
        avgPointX += iter.x;
        avgPointY += iter.y;
        avgPointCnt += 1;
    }
    if (avgPointCnt > 1) {
        wcircle = cv::Point2f(avgPointX / avgPointCnt, avgPointY / avgPointCnt);
    }
    // lineShot
    if (std::abs(wcircle.x - wball.x) < 1e-2) {
        float lineX = -lineReflect[2] / lineReflect[0];
        if (lineReflect[1] == 0 && std::abs(lineX - wball.x) < 1e-2) {
            auto tmp = lineShot;
            lineShot = lineReflect;
            lineReflect = tmp;
        }
        aShot = wcircle.y > wball.y ? CV_PI / 2 : -CV_PI / 2;
    }
    else {
        float lineShotA = (wcircle.y - wball.y) / (wcircle.x - wball.x);
        float slA = lineShot[0], slB = lineShot[1], slC = lineShot[2];
        float rlA = lineReflect[0], rlB = lineReflect[1], rlC = lineReflect[2];
        bool rsV = (slA != 0 || slB != 0 || slC != 0);
        bool rlV = (rlA != 0 || rlB != 0 || rlC != 0);
        if (rsV && rlV == false) {
            lineShot = rsV ? lineShot : lineReflect;
        }
        else if (rsV && rlV == true) {
            float sl = std::abs(-slA / slB);
            float rl = std::abs(-rlA / rlB);
            float lsa = std::abs(lineShotA);
            if (std::abs(sl - lsa) > std::abs(rl - lsa)) {
                auto tmp = lineShot;
                lineShot = lineReflect;
                lineReflect = tmp;
            }
        }
        if (lineShot[0] != 0 || lineShot[1] != 0 || lineShot[2] != 0) {
            auto lineShotB = -lineShot[0] / lineShot[1];
            auto dx = wcircle.x > wball.x ? 1 : -1;
            auto dy = wcircle.y > wball.y ? std::abs(lineShotB) : -std::abs(lineShotB);
            aShot = lineShotA * lineShotB > 0 ? (float)std::atan2(dy, dx) : (float)std::atan2(-dy, -dx);
        }
    }

    return 0;
}

std::vector<cv::Vec4f> tranVec3f2Line(std::vector<cv::Vec3f>& lines) {
    std::vector<cv::Vec4f> lines4f;
    for (auto iter : lines) {
        auto a = iter[0], b = iter[1], c = iter[2];
        cv::Point2f point1, point2, point3;
        point1.y = 0;
        point1.x = -c / a;
        point2.y = -c / b;
        point2.x = 0;
        point3.y = 1000;
        point3.x = -(c + b * point3.y) / a;
        lines4f.push_back(cv::Vec4f(point1.x, point1.y, point3.x, point3.y));
    }
    return lines4f;
}

//
// if success, we must have got: wball, wcircle, lineHit or lineReflect
//
int ADetector::detectWhites(const cv::Mat& imgGray, float radiusBall, cv::Point2f& wball, cv::Point2f& wcircle, float& aShot, cv::Vec3f& lHit, cv::Vec3f& lReflect) {
    auto stampStart = cv::getTickCount();
    cv::Mat imgBin = imgGray.clone();
    cv::GaussianBlur(imgBin, imgBin, cv::Size(3, 3), 0);
    cv::threshold(imgBin, imgBin, 200, 256, cv::THRESH_BINARY);

    int nRet = 0;
    bool isMyRound = false;
    cv::Vec3f lineHit, lineShot, lineReflect;
    do {
        auto imgRoIMask = _detectWhiteRoI(imgBin, 19);
        if (imgRoIMask.empty()) {
            nRet = -1; break;
        }
        auto stampA = cv::getTickCount();

        cv::Mat maxContour;
        nRet = checkRoIMask(imgRoIMask, radiusBall, lineHit, wball, maxContour);
        if (nRet != 0) {
            isMyRound = (lineHit[0] != 0 || lineHit[1] != 0 || lineHit[2] != 0) && ((wball.x * wball.y > 0) || !maxContour.empty());
            nRet = -2; break;
        }
        auto stampB = cv::getTickCount();

        std::vector<cv::Vec3f> lines;
        nRet = checkMaxContour(maxContour, radiusBall, wcircle, lines);
        if (nRet != 0) {
            nRet = -3; break;
        }
        auto stampC = cv::getTickCount();

        lineShot = lines.size() > 0 ? lines[0] : cv::Vec3f();
        lineReflect = lines.size() > 1 ? lines[1] : cv::Vec3f();
        nRet = verifyChecked(wball, wcircle, lineShot, lineHit, lineReflect, aShot);
        if (nRet != 0) {
            nRet = -4; break;
        }
        lHit = lineHit;
        lReflect = lineReflect;
        isMyRound = true;

        LOGD("ADetector::detectWhites ok: _detectWhiteRoI(%.4fs) checkRoIMask(%.4fs) checkMaxContour(%.4fs)",
            (stampA - stampStart) / cv::getTickFrequency(), (stampB - stampA) / cv::getTickFrequency(), (stampC - stampB) / cv::getTickFrequency());
    } while (false);
    LOGD("ADetector::detectWhites %s(%d-%.4fs): wball=(%.2f, %.2f) wcircle=(%.2f, %.2f) aShot=%.2f lHit=(%.2f,%.2f,%.2f) lReflect=(%.2f,%.2f,%.2f)",
        nRet == 0 ? "ok" : "error", nRet, (cv::getTickCount() - stampStart) / cv::getTickFrequency(),
        wball.x, wball.y, wcircle.x, wcircle.y, aShot, lHit[0], lHit[1], lHit[2], lReflect[0], lReflect[1], lReflect[2]);
#ifndef PLAT_ANDROID
    if (gDrawForDebug) {
        std::vector<cv::Vec3f> linesTmp;
        linesTmp.push_back(lineHit);
        linesTmp.push_back(lineShot);
        //linesTmp.push_back(lineReflect);
        auto linesToDraw = tranVec3f2Line(linesTmp);
        showImg("fitLine", imgRawGlobal, nullptr, &linesToDraw);
    }
#endif
    return nRet;
}

cv::Mat ADetector::_detectWhiteRoI(const cv::Mat& imgBinRaw, int kernelSize/*=7*/) {
    cv::Mat imgMask, imgBin;
    // delite
    auto openKernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(1, 1));
    auto closeKernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(kernelSize, kernelSize));
    cv::morphologyEx(imgBinRaw, imgBin, cv::MORPH_OPEN, openKernel);
    cv::dilate(imgBin, imgBin, closeKernel);
    // findContours: max area
    int maxContLen = 0, maxContIndex = 0;
    auto contours = std::vector<std::vector<cv::Point>>();
    cv::findContours(imgBin, contours, cv::noArray(), cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
    for (int i = 0; i < contours.size(); i++) {
        auto contLen = cv::arcLength(contours[i], false);
        if (contLen > maxContLen) {
            maxContLen = contLen;
            maxContIndex = i;
        }
    }
    if (maxContLen > 0) {
        imgMask = cv::Mat::zeros(imgBinRaw.rows, imgBinRaw.cols, CV_8UC1);
        cv::drawContours(imgMask, contours, maxContIndex, cv::Scalar(255), -1);
        cv::bitwise_and(imgBinRaw, imgMask, imgMask);
    }
    return imgMask;
}
