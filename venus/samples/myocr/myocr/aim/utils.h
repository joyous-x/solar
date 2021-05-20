//
// Created by jiao on 2020/9/29.
//

#ifndef AIMBOOSTER_UTILS_H
#define AIMBOOSTER_UTILS_H

#include <opencv2/opencv.hpp>
#include <vector>


cv::Mat rotateImage(cv::Mat& src_img, int degree);
int avgMajority(std::vector<float> datas, float& avgMajority, int maxDistance=2);
void showImg(std::string winName, cv::Mat& img, std::vector<cv::Vec3f>* circles=nullptr, std::vector<cv::Vec4f>* lines=nullptr, std::vector<std::vector<cv::Point>>* conts=nullptr);

float getPoint2LineDist(cv::Point pointP, cv::Point pointA, cv::Point pointB);
cv::Point2f getFootPoint2Line(cv::Point2f pointP, cv::Point2f pointA, cv::Point2f pointB);

#endif //AIMBOOSTER_UTILS_H
