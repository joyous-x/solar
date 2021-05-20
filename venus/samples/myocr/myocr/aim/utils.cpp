//
// Created by jiao on 2020/9/29.
//

#include <numeric>
#include "aim/utils.h"

cv::Mat rotateImage(cv::Mat& src_img, int degree) {
    if (degree == 90) {
        cv::Mat srcCopy = cv::Mat(src_img.rows, src_img.cols, src_img.depth());
        cv::transpose(src_img, srcCopy);
        cv::flip(srcCopy, srcCopy, 1);
        return srcCopy;
    } else if (degree == 180) {
        cv::Mat srcCopy = cv::Mat(src_img.rows, src_img.cols, src_img.depth());
        cv::flip(src_img, srcCopy, -1);
        return srcCopy;
    } else if (degree == 270) {
        cv::Mat srcCopy = cv::Mat(src_img.rows, src_img.cols, src_img.depth());
        cv::transpose(src_img, srcCopy);
        cv::flip(srcCopy, srcCopy, 0);
        return srcCopy;
    } else {
        return src_img;
    }
}

int avgMajority(std::vector<float> datas, float& avgMajority, int maxDistance/*=2*/) {
    auto neighbors = std::map<int, std::vector<int>>();
    for (int i = 0; i < datas.size(); i++) {
        neighbors[i] = std::vector<int>();
        for (int j = 0; j < datas.size(); j++) {
            if (std::abs(datas[i] - datas[j]) < maxDistance) {
                neighbors[i].push_back(datas[j]);
            }
        }
    }
    int maxNeighborCnt = 0;
    int maxNeighborKey = 0;
    for (auto item : neighbors) {
        if (item.second.size() > maxNeighborCnt) {
            maxNeighborCnt = item.second.size();
            maxNeighborKey = item.first;
        }
    }
    auto majorityNeighbor = neighbors[maxNeighborKey];
    double sum = std::accumulate(std::begin(majorityNeighbor), std::end(majorityNeighbor), 0.0);
    avgMajority = (float)sum / majorityNeighbor.size();
    return 0;
}

void showImg(std::string winName, cv::Mat& img, std::vector<cv::Vec3f>* circles, std::vector<cv::Vec4f>* lines, std::vector<std::vector<cv::Point>>* conts) {
#ifndef PLAT_ANDROID
    if (circles != nullptr) {
        for (auto cir : *circles) {
            cv::circle(img, cv::Point(cir[0], cir[1]), cir[2], cv::Scalar(0, 255, 0), 1);
        }
    }
    if (lines != nullptr) {
        for (auto line : *lines) {
            cv::line(img, cv::Point(line[0], line[1]), cv::Point(line[2], line[3]), cv::Scalar(0, 0, 255), 1);
        }
    }
    if (conts != nullptr) {
        cv::drawContours(img, *conts, -1, cv::Scalar(255, 0, 0), 2);
    }
    cv::namedWindow(winName, cv::WINDOW_NORMAL);
    cv::imshow(winName, img);
    cv::waitKey(0);
#endif
}

float getPoint2LineDist(cv::Point pointP, cv::Point pointA, cv::Point pointB)
{
    //求直线方程
    auto A = pointA.y - pointB.y;
    auto B = pointB.x - pointA.x;
    auto C = pointA.x*pointB.y - pointA.y*pointB.x;
    //代入点到直线距离公式
    float distance = 0;
    distance = ((float)abs(A*pointP.x + B*pointP.y + C)) / ((float)sqrtf(A*A + B*B));
    return distance;
}

cv::Point2f getFootPoint2Line(cv::Point2f pointP, cv::Point2f pointA, cv::Point2f pointB)
{
    cv::Point2f foot(0, 0);
    if (abs(pointA.x - pointB.x) < 1e-3) // 线与x轴垂直
    {
        foot.x = pointA.x;
        foot.y = pointP.y;
    }
    else if (abs(pointA.y - pointB.y) < 1e-3) // 线与y轴垂直
    {
        foot.x = pointP.x;
        foot.y = pointA.y;
    }
    else
    {
        int a1 = -(pointB.y - pointA.y);
        int b1 = pointB.x - pointA.x;
        int c1 = (pointB.y - pointA.y) * pointA.x - (pointB.x - pointA.x) * pointA.y;
        foot.x = (b1 * b1 * pointP.x - a1 * b1 * pointP.y - a1 * c1) / (a1 * a1 + b1 * b1);
        foot.y = (a1 * a1 * pointP.y - a1 * b1 * pointP.x - b1 * c1) / (a1 * a1 + b1 * b1);
    }
    return foot;
}
