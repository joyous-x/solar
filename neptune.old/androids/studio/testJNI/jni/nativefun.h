//
// Created by jiao on 2016/8/20.
//
#ifndef __native_fun_h__
#define __native_fun_h__

#include <jni.h>

#ifdef __cplusplus
//extern "C" {
#endif

/**
* Class     :
* Method    :
* Signature :
*/
JNIEXPORT jint JNICALL AddInt(JNIEnv*, jclass, jint, jint);

/**
* Class     :
* Method    :
* Signature :
*/
JNIEXPORT jstring JNICALL AddStr(JNIEnv*, jclass, jstring, jstring);

/**
* Class     :
* Method    :
* Signature :
*/
JNIEXPORT jint JNICALL AddIntByPrivate(JNIEnv*, jclass, jint);

/**
* Class     :
* Method    :
* Signature :
*/
JNIEXPORT jstring JNICALL AddStrByPrivate(JNIEnv*, jclass, jstring);
#ifdef __cplusplus
//}
#endif

#endif //> __native_fun_h__
