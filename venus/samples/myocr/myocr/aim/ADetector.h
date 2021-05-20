//
// Created by jiao on 2020/9/27.
//

#ifndef AIMBOOSTER_ADETECTOR_H
#define AIMBOOSTER_ADETECTOR_H

#include <opencv2/opencv.hpp>
#include <iostream>
#include "include/register.h"

const int ANGLE_INVALID = 1 << 10;


class ADetector {
public:
    ADetector();
    ~ADetector();

    static ADetector* instance() {
        static ADetector instance;
        return &instance;
    }
    void setScreenInfo(int w, int h, int barH, int rotation);
    // if key is empty, will be reset forcely
    int resetFrameInfo(std::string key);

    int pushScene(std::string key, cv::Mat* img);
    // if key is empty, will be pop forcely
    cv::Mat* popScene(std::string key);
    cv::Mat* getScene(std::string key);

    cv::Rect getRectRoI();
    cv::Rect getRectValid();
    int getRadiusHoleB();
    int getRadiusHoleS();
    int getRadiusBall();
    int getRadiusCircle();
    int getScreenH();
    int getScreenW();

public:
    int detectFrameInfo(std::string key);
    int detectBallsInfo(cv::Mat& imgMat, jni_core_balls_info& ballsInfo);
    int detectBalls(cv::Mat& imgRoIGray, std::vector<cv::Vec3f>& balls, cv::Point2f& wball, float& rball, float angleShot);

private:
    bool _isDetectorValid(const cv::Mat& imgRaw);
    int _checkAimBall(const std::vector<cv::Vec3f>& balls, cv::Point2f wball, cv::Point2f wcircle, float rcircle, float rball, cv::Point2f& aimBall);
    int _checkAllAngles(cv::Point2f& ball, cv::Point2f& circle, cv::Point2f& hit, cv::Vec3f lHit, cv::Vec3f lReflect, float& aShot, float& aHit, float& aReflect);
    int _detectBalls(cv::Mat& imgBin, std::vector<cv::Vec3f>& balls, cv::Point2f& wball, float& rball, float angleShot, int radiusMin = 15, int radiusMax = 28);

private: //> check frame
    int _detectFrame(cv::Mat& mat);
    int _detectFrameDefault(const cv::Rect& rectValid);
    cv::Rect _detectRectValid(const cv::Mat& imgGray);
    cv::Rect _detectRectRoI(const cv::Mat& matRaw, int ksizeMedia, int tolerance = 3);

private: //> check with white
    int detectWhites(const cv::Mat& imgGray, float radiusBall, cv::Point2f& wball, cv::Point2f& wcircle, float& aShot, cv::Vec3f& lHit, cv::Vec3f& lReflect);
    cv::Mat _detectWhiteRoI(const cv::Mat& imgBin, int kernelSize = 7);
    float _calcHitAnlges(float angleShot, float angleHit);
    float _calcLineAnlge(cv::Point2f& src, cv::Point2f& dst, cv::Vec3f line);

private: //> ADetectorPlus
    int isMyRound(cv::Mat& imgRaw);
    bool _isMyRound(cv::Mat& imgMat, const cv::Rect& rectRoI, int errorTheta = 3, float scaleFactor = 1);
    int _imgBinary(cv::Mat& imgMat, cv::Mat& imgOut, int ksizeMedia = 5, int ksizeBlur = 3);
    int detectShotLineGray(const cv::Mat& imgMat, const cv::Rect& rectRoI, const cv::Vec3f& whiteBall, float& whiteLineAngle, int minLineLength = 50);
    int detectWhiteBall(const cv::Mat& imgBin, int radiusBall, cv::Vec3f& whiteBall, cv::Vec3f& whiteCircle);
    //>
    cv::Rect _detectRectRoI(const cv::Mat& matRaw, int ksizeBlur, int ksizeMedia, int tolerance/*=3*/);
    //>
    int _detectWhiteLines(const cv::Mat& imgBin, cv::Point2f wball, cv::Point2f& wcircle, float rball, float& aShot, float& aHit, float& aReflect);
    int _checkLineInCircle(std::vector<cv::Vec4f>& lines, cv::Point2f pc, float pr, float& angle, const float* angleFilter = nullptr);
    int detectWhiteInfo(const cv::Mat& imgGray, float radiusBall, cv::Point2f& wball, cv::Point2f& wcircle, float& shotAngle);
    int _detectWhiteBall(const cv::Mat& imgBin, float radiusBall, cv::Vec3f& whiteBall, cv::Vec3f& whiteCircle, float threshold);
    int _detectShotLine(const cv::Mat& imgBin, const cv::Vec3f& whiteBall, float& shotAngle, int minLength = 50);

private:
    int mScreenW{ 0 };
    int mScreenH{ 0 };
    int mScreenBarH{ 0 };
    int mScreenRotation{ 0 };
    cv::Rect mRectRoI;
    cv::Rect mRectValid;
    int mRadiusHoleB{ 0 };
    int mRadiusHoleS{ 0 };
    int mRadiusBall{ 0 };
    int mRadiusCircle{ 0 };
    int mHoleMouthLx{ 0 };
    int mHoleMouthRx{ 0 };
    int mHoleMouthTy{ 0 };
    int mHoleMouthBy{ 0 };
    int mHoleMouthMxr{ 0 };
    int mHoleMouthMxl{ 0 };

    cv::Mat imgRawGlobal;

    // cache
    cv::Point2f mWhiteBallLast;
    std::vector<cv::Vec3f> mBallsLast;
    cv::Point2f mWhiteCircleLast;

    // easily
    int mMaxBallCnt{ 17 };
    std::string mSceneKey{ "" };
    cv::Mat* mSceneMat{ nullptr };
};

#endif //AIMBOOSTER_ADETECTOR_H
