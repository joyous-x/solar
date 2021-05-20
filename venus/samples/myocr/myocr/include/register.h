//
// Created by jiao on 2020/9/23.
//

#ifndef AIMBOOSTER_REGISTER_H
#define AIMBOOSTER_REGISTER_H

#include <string>
#include <opencv2/opencv.hpp>

typedef struct point_t {
    float x;
    float y;
    void reset() {
        x = 0;
        y = 0;
    }
} jni_point;


typedef struct core_balls_info_t {
    std::string sceneIdentity;
    int category;
    cv::Rect roi;
    jni_point aimBall;
    jni_point whiteBall;
    jni_point whiteCircle;
    bool bCircleAsideBound;
    float angleShot;
    float angleHit;
    float angleReflect;
    jni_point balls[24];

    void reset() {
        sceneIdentity = "";
        category = 0;
        aimBall.reset();
        whiteBall.reset();
        whiteCircle.reset();
        bCircleAsideBound = false;
        angleShot = 0.0f;
        angleHit = 0.0f;
        angleReflect = 0.0f;
        for (int i = 0; i < sizeof(balls)/ sizeof(balls[0]); i ++) {
            balls[i].reset();
        }
    }
} jni_core_balls_info;

#endif //AIMBOOSTER_REGISTER_H
