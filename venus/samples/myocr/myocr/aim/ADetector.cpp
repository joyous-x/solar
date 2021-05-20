//
// Created by jiao on 2020/9/27.
//

#include "ADetector.h"
#include <cmath>
#include "include/common.h"
#include "include/register.h"
#include "aim/utils.h"

ADetector::ADetector() {

}

ADetector::~ADetector() {

}

void ADetector::setScreenInfo(int w, int h, int barH, int rotation) {
    mScreenH = h;
    mScreenW = w;
    mScreenBarH = barH;
    mScreenRotation = rotation;
}

cv::Rect ADetector::getRectRoI() {
    return mRectRoI;
}

cv::Rect ADetector::getRectValid() {
    return mRectValid;
}

int ADetector::getRadiusHoleB() {
    return mRadiusHoleB;
}

int ADetector::getRadiusHoleS() {
    return mRadiusHoleS;
}

int ADetector::getRadiusBall() {
    return mRadiusBall;
}

int ADetector::getRadiusCircle() {
    return mRadiusCircle;
}

int ADetector::getScreenH() {
    return mScreenH;
}

int ADetector::getScreenW() {
    return mScreenW;
}


int ADetector::pushScene(std::string key, cv::Mat* img) {
    if (!mSceneKey.empty()) {
        LOGW("ADetector::pushScene error, mSceneKey=%s key=%s", mSceneKey.c_str(), key.c_str());
        return -1;
    }
    mSceneKey = key;
    mSceneMat = img;
    LOGD("ADetector::pushScene ok, mSceneKey=%s key=%s", mSceneKey.c_str(), key.c_str());
    return 0;
}

cv::Mat* ADetector::getScene(std::string key) {
    if (mSceneKey != key) {
        LOGW("ADetector::getScene error, mSceneKey=%s key=%s", mSceneKey.c_str(), key.c_str());
        return nullptr;
    }
    return mSceneMat;
}

cv::Mat* ADetector::popScene(std::string key) {
    if (!key.empty() && mSceneKey != key) {
        LOGW("ADetector::popScene error, mSceneKey=%s key=%s", mSceneKey.c_str(), key.c_str());
        return nullptr;
    }
    auto ret = mSceneMat;
    mSceneKey.clear();
    mSceneMat = nullptr;
    LOGD("ADetector::popScene ok, mSceneKey=%s key=%s", mSceneKey.c_str(), key.c_str());
    return ret;
}

int ADetector::detectFrameInfo(std::string key) {
    if (!mSceneKey.empty() && mSceneKey != key && mSceneMat != nullptr) {
        LOGW("ADetector::detectFrameInfo error, mSceneKey=%s key=%s mSceneMat=%d", mSceneKey.c_str(), key.c_str(), mSceneMat != nullptr);
        return -1;
    }
    return _detectFrame(*mSceneMat);
}

int ADetector::resetFrameInfo(std::string key) {
    if (!key.empty() && mSceneKey != key) {
        LOGW("ADetector::resetFrameInfo error, mSceneKey=%s key=%s", mSceneKey.c_str(), key.c_str());
        return -1;
    }
    mRectRoI = cv::Rect(0, 0, 0, 0);
    mRectValid = cv::Rect(0, 0, 0, 0);
    mRadiusHoleB = 0;
    mRadiusHoleS = 0;
    mRadiusBall = 0;
    mRadiusCircle = 0;
    mHoleMouthLx = 0;
    mHoleMouthRx = 0;
    mHoleMouthTy = 0;
    mHoleMouthBy = 0;
    mHoleMouthMxr = 0;
    mHoleMouthMxl = 0;
    return 0;
}

bool ADetector::_isDetectorValid(const cv::Mat& imgRaw) {
    bool valid = false;
    do {
        if (mScreenRotation != 90 && mScreenRotation != 270) {
            break;
        }
        if (imgRaw.empty() || mRectRoI.x < mRectValid.x) {
            break;
        }
        if (mRectRoI.width == 0 || mRectRoI.height == 0) {
            break;
        }
        valid = true;
    } while (false);
    if (!valid) {
        LOGE("ADetector::_isDetectorValid error: empty=%d rotation=%d rectValid.x=%d rectRoI=(%d, %d, %d, %d)",
            imgRaw.empty(), mScreenRotation, mRectValid.x, mRectRoI.x, mRectRoI.y, mRectRoI.width, mRectRoI.height);
    }
    return valid;
}

int ADetector::detectBallsInfo(cv::Mat& imgRaw, jni_core_balls_info& ballsInfo) {
    auto curStamp = cv::getTickCount();
    auto startStamp = curStamp;
    if (false == _isDetectorValid(imgRaw)) {
        return -1;
    }
    if (imgRaw.cols < imgRaw.rows) {
        imgRaw = rotateImage(imgRaw, abs(360 - mScreenRotation) % 360);
    }
    // pre process
    imgRawGlobal = imgRaw(mRectRoI).clone();
    //-- gray
    cv::Mat imgRoIGray = imgRaw(mRectRoI);
    cv::cvtColor(imgRoIGray, imgRoIGray, cv::COLOR_RGB2GRAY);
    // detect white infos
    float aShot = ANGLE_INVALID, aHit = ANGLE_INVALID, aReflect = ANGLE_INVALID;
    cv::Point2f wball, wcircle;
    cv::Vec3f lHit, lReflect;
    int nRet = detectWhites(imgRoIGray, mRadiusBall, wball, wcircle, aShot, lHit, lReflect);
    if (nRet != 0) {
        return -2;
    }
    // balls
    float rball = 0, rcircle = 0;
    std::vector<cv::Vec3f> balls;
    nRet = detectBalls(imgRoIGray, balls, wball, rball, aShot);
    if (nRet < 0) {
        return -3;
    }
    rball = mRadiusBall, rcircle = mRadiusCircle;
    // rectify
    cv::Point2f aimBall = cv::Point2f(0, 0);
    if (0 > _checkAimBall(balls, wball, wcircle, rcircle, rball, aimBall)) {
        LOGW("detectBallsInfo._checkAimBall error: balls_size=%d wcircle=(%.2f,%.2f) rcircle=%.2f rball=%.2f", balls.size(), wcircle.x, wcircle.y, rcircle, rball);
    }
    if (0 == _checkAllAngles(wball, wcircle, aimBall, lHit, lReflect, aShot, aHit, aReflect)) {
        LOGD("detectBallsInfo ok(total:%.4fs): wball=(%.2f,%.2f) wcircle=(%.2f,%.2f) aimBall=(%.2f,%.2f) rcircle=%.2f rball=%.2f angleShot=%.2f angleHit=%.2f angleReflect=%.2f",
            (cv::getTickCount() - startStamp) / cv::getTickFrequency(),
            wball.x, wball.y, wcircle.x, wcircle.y, aimBall.x, aimBall.y, rcircle, rball, aShot, aHit, aReflect);
    }
#ifndef PLAT_ANDROID
    {
        auto imgRawBg = imgRaw(mRectRoI);
        balls.push_back(cv::Vec3f(wball.x, wball.y, rball));
        balls.push_back(cv::Vec3f(wcircle.x, wcircle.y, rcircle));
        std::vector<cv::Vec4f> lines;
        //lines.push_back(cv::Vec4i(wball.x, wball.y, wcircle.x, wcircle.y));
        lines.push_back(cv::Vec4f(wcircle.x, wcircle.y, wcircle.x + 200 * cos(aHit), wcircle.y + 200 * sin(aHit)));
        lines.push_back(cv::Vec4f(wcircle.x, wcircle.y, wcircle.x + 200 * cos(aReflect), wcircle.y + 200 * sin(aReflect)));
        showImg("xxx", imgRawBg, &balls, &lines);
    }
#endif
    ballsInfo.reset();
    ballsInfo.sceneIdentity = mSceneKey;
    ballsInfo.angleShot = aShot;
    ballsInfo.angleHit = aHit;
    ballsInfo.angleReflect = aReflect;
    ballsInfo.bCircleAsideBound = false;
    if (wball.x != 0 || wball.y != 0) {
        ballsInfo.whiteBall.x = wball.x + mRectRoI.x - mRectValid.x;
        ballsInfo.whiteBall.y = wball.y + mRectRoI.y - mRectValid.y;
    }
    if (wcircle.x != 0 || wcircle.y != 0) {
        ballsInfo.whiteCircle.x = wcircle.x + mRectRoI.x - mRectValid.x;
        ballsInfo.whiteCircle.y = wcircle.y + mRectRoI.y - mRectValid.y;
    }
    if (aimBall.x != 0 || aimBall.y != 0) {
        ballsInfo.aimBall.x = aimBall.x + mRectRoI.x - mRectValid.x;
        ballsInfo.aimBall.y = aimBall.y + mRectRoI.y - mRectValid.y;
    }
    for (int i = 0; i < balls.size(); i++) {
        if (sizeof(ballsInfo.balls) / sizeof(ballsInfo.balls[0]) <= i) {
            break;
        }
        ballsInfo.balls[i].x = balls[i][0] + mRectRoI.x - mRectValid.x;
        ballsInfo.balls[i].y = balls[i][1] + mRectRoI.y - mRectValid.y;
    }
    return 0;
}

int ADetector::detectBalls(cv::Mat& imgRoIGray, std::vector<cv::Vec3f>& balls, cv::Point2f& wball, float& rball, float angleShot) {
    int nRet = 0;
    int deviation = 3, radiusMin = mRadiusBall * .7, radiusMax = mRadiusBall * 1.3;
    bool useCache = cv::norm(cv::Point(mWhiteBallLast.x, mWhiteBallLast.y) - cv::Point(wball.x, wball.y)) <= deviation;

    auto curStamp = cv::getTickCount();
    if (!useCache || mBallsLast.size() < 1) {
        balls.clear();
        cv::GaussianBlur(imgRoIGray, imgRoIGray, cv::Size(5, 5), 0);
        nRet = _detectBalls(imgRoIGray, balls, wball, rball, angleShot, radiusMin, radiusMax);
        if (balls.size() < mMaxBallCnt && balls.size() > 1) {
            mBallsLast.clear();
            mBallsLast = balls;
        }
        mWhiteBallLast = (0 == nRet ? wball : cv::Point2f(0, 0));
    }
    else {
        balls = mBallsLast;
        wball = mWhiteBallLast;
    }
    LOGI("detectBallsInfo.detectBalls %s rst=%d(%.4fs): balls_size=%d wball=(%.2f,%.2f) rball=%.2f",
        useCache ? "cached" : "", nRet, (cv::getTickCount() - curStamp) / cv::getTickFrequency(), balls.size(), wball.x, wball.y, rball);
    return nRet;
}


int ADetector::_detectBalls(cv::Mat& imgBin, std::vector<cv::Vec3f>& balls, cv::Point2f& wball, float& rball, float angleShot, int radiusMin/* = 15 */, int radiusMax/* = 28*/) {
    cv::Mat imgRoI = imgBin;
    // circles
    std::vector<cv::Vec3f> circles;
    cv::HoughCircles(imgRoI, circles, cv::HOUGH_GRADIENT, 1, radiusMin * 2, 50, 15, radiusMin, radiusMax);
    // majority radius
    std::vector<float> radiuses;
    std::for_each(std::begin(circles), std::end(circles), [&](const cv::Vec3f& d) {
        radiuses.push_back(d[2]);
    });
    avgMajority(radiuses, rball, 3);
    // white ball
    cv::Vec3f whiteBall = cv::Vec3f(0, 0, 0);
    for (auto ball : circles) {
        if (cv::norm(cv::Point2f(ball[0], ball[1]) - cv::Point2f(wball.x, wball.y)) < int(rball * 0.8)) {
            whiteBall = ball;
            break;
        }
    }
    if (whiteBall[2] == 0) {
        return -1;
    }
    wball = cv::Point2f(whiteBall[0], whiteBall[1]);
    // white circle
    auto whiteCircle = cv::Vec3f(0, 0, 0);
    auto minWhiteDistance = 100000.0f;
    for (auto ball : circles) {
        cv::Point2f e(ball[0], ball[1]);
        cv::Point2f s(wball.x, wball.y);
        if (e.x == s.x && e.y == s.y) {
            continue;
        }
        float angle = (float)std::atan2(e.y - s.y, e.x - s.x) * 180.0 / CV_PI;
        if (abs(angle - angleShot) > 1) {
            continue;
        }
        auto distance = cv::norm(e - s);
        if (distance > minWhiteDistance) {
            continue;
        }
        minWhiteDistance = distance;
        whiteCircle = ball;
    }
    if (whiteCircle[2] > 0) {
        // 暂时没用
        cv::Vec3f wcircle;
        wcircle = whiteCircle;
    }
    // balls
    balls.clear();
    for (auto ball : circles) {
        if (abs(ball[0] - whiteCircle[0]) < 1e-2 && abs(ball[1] - whiteCircle[1]) < 1e-2) {
            continue;
        }
        if (abs(ball[2] - rball) > rball / 5) {
            continue;
        }
        balls.push_back(ball);
    }
    return 0;
}

int ADetector::_checkAimBall(const std::vector<cv::Vec3f>& balls, cv::Point2f wball, cv::Point2f wcircle, float rcircle, float rball, cv::Point2f& aimBall) {
    auto tmpHitCircle = cv::Vec3f(0, 0, 0);
    auto maxHitDistance = (rball + rcircle) * 1.1;
    for (auto ball : balls) {
        cv::Point2f e(ball[0], ball[1]);
        cv::Point2f s(wcircle);
        if (std::abs(e.x - wball.x) < 1e-2 && std::abs(e.y - wball.y) < 1e-2) {
            continue;
        }
        double distance = cv::norm(e - s);
        if (distance < maxHitDistance && distance > rball) {
            maxHitDistance = distance;
            tmpHitCircle = ball;
        }
        if (maxHitDistance == distance) {
            LOGD("ADetector::_checkAimBall item(%s): ball=(%.2f, %.2f) dist-maxdist-r=(%.2f,%.2f,%.2f)", maxHitDistance == distance ? "ok" : "error", e.x, e.y, distance, maxHitDistance, rball);
        }
    }
    if (tmpHitCircle[2] != 0) {
        aimBall = cv::Point2f(tmpHitCircle[0], tmpHitCircle[1]);
    }
    return tmpHitCircle[2] != 0 ? 0 : -1;
}

int angleBy2Vec(float angleA, float angleB) {
    auto aAa = angleA > 0 ? angleA : angleA + CV_2PI;
    auto aBb = angleB > 0 ? angleB : angleB + CV_2PI;
    auto sub = std::abs(aBb - aAa);
    if (sub > CV_PI) {
        sub = CV_2PI - sub;
    }
    return sub;
}

int ADetector::_checkAllAngles(cv::Point2f& ball, cv::Point2f& circle, cv::Point2f& hit, cv::Vec3f lHit, cv::Vec3f lReflect, float& aShot, float& aHit, float& aReflect) {
    float devation = 1;
    float aHitTmp = (hit.x == 0 && hit.y == 0) ? ANGLE_INVALID : (float)std::atan2(hit.y - circle.y, hit.x - circle.x);
    if (ANGLE_INVALID == aShot) {
        aShot = (float)std::atan2(circle.y - ball.y, circle.x - ball.x);
    }
    LOGD("ADetector::_checkAllAngles ready: aShot=%.2f aHit=%.2f aReflect=%.2f aHitTmp=%.2f", aShot, aHit, aReflect, aHitTmp);

    auto lrA = lReflect[0], lrB = lReflect[1], lrC = lReflect[2];
    if (lrA != 0 || lrB != 0 || lrC != 0) {
        auto aReflectA = (float)std::atan2(-lrA, lrB);
        auto aReflectB = (float)std::atan2(lrA, -lrB);
        if (angleBy2Vec(aReflectA, aShot) < CV_PI / 2) {
            aReflect = aReflectA;
        }
        else if (angleBy2Vec(aReflectB, aShot) < CV_PI / 2) {
            aReflect = aReflectB;
        }
    }

    auto lhA = lHit[0], lhB = lHit[1], lhC = lHit[2];
    if (lhA != 0 || lhB != 0 || lhC != 0) {
        auto aA = (float)std::atan2(-lhA, lhB);
        auto aB = (float)std::atan2(lhA, -lhB);
        if (angleBy2Vec(aA, aShot) < CV_PI / 2) {
            aHit = aA;
        }
        else if (angleBy2Vec(aB, aShot) < CV_PI / 2) {
            aHit = aB;
        }
    }
    if (aHit == ANGLE_INVALID) {
        aHit = _calcLineAnlge(circle, hit, lHit);
    }

    if (aHit != ANGLE_INVALID) {
        aReflect = _calcHitAnlges(aShot, aHit);
    }
    else if (aReflect != ANGLE_INVALID) {
        aHit = _calcHitAnlges(aShot, aReflect);
    }
    else if (aHitTmp != ANGLE_INVALID) {
        aHit = aHitTmp;
        aReflect = _calcHitAnlges(aShot, aHit);
    }
    else {
        aHit = ANGLE_INVALID;
        aReflect = ANGLE_INVALID;
    }
    auto tmpS = aShot * 180 / CV_PI;
    auto tmpH = aHit * 180 / CV_PI;
    auto tmpR = aReflect * 180 / CV_PI;
    if (abs(aHit - aShot) < 0.01) {
        aReflect = ANGLE_INVALID;
    }

    return 0;
}

float ADetector::_calcLineAnlge(cv::Point2f& src, cv::Point2f& dst, cv::Vec3f line) {
    float angle = ANGLE_INVALID;
    if (line[0] == 0 && line[1] == 0 && line[2] == 0) {
        return angle;
    }
    if ((src.x == 0 && src.y == 0) || (dst.x == 0 && dst.y == 0)) {
        return angle;
    }
    if (std::abs(dst.x - src.x) < 1e-2) {
        angle = dst.y > src.y ? CV_PI / 2 : -CV_PI / 2;
    }
    else {
        float lineShotA = (dst.y - src.y) / (dst.x - src.x);
        auto dx = dst.x > src.x ? std::abs(line[1]) : -std::abs(line[1]);
        auto dy = dst.y > src.y ? std::abs(line[0]) : -std::abs(line[0]);
        angle = -line[1] * line[0] * lineShotA >= 0 ? (float)std::atan2(dy, dx) : (float)std::atan2(-dy, -dx);
    }
    return angle;
}

float ADetector::_calcHitAnlges(float angleShot, float angleHit) {
    float angleReflect = 0;
    float subValue = angleShot - angleHit;
    if (subValue > 0) {
        if (subValue > CV_PI) {
            subValue = CV_2PI - subValue;
            angleReflect = angleShot - (CV_PI / 2 - subValue);
        }
        else {
            angleReflect = angleShot + CV_PI / 2 - subValue;
        }
    }
    else {
        if (abs(subValue) > CV_PI) {
            subValue = CV_2PI - abs(subValue);
            angleReflect = angleShot + (CV_PI / 2 - abs(subValue));
        }
        else {
            angleReflect = angleShot - (CV_PI / 2 - abs(subValue));
        }
    }
    return abs(angleReflect) <= CV_PI ? angleReflect : (angleReflect > 0 ? angleReflect - CV_2PI : angleReflect + CV_2PI);
}
