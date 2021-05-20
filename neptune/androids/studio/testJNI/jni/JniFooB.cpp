//
// Created by jiao on 2016/8/20.
//
#include "nativefun.h"
#include "JniTypeUtils.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

jstring jstrcat(JNIEnv* env, const char* a, const char* b) {
    jstring strRst;
    char* result = (char*)malloc((strlen(a) + strlen(b) + 1) * sizeof(char));
    memset(result, 0, sizeof(result));
    strcat(result, a);
    strcat(result, b);
    strRst = env->NewStringUTF(result);
    free(result);
    return strRst;
}

jobject getInstance(JNIEnv* env, jclass obj_class)
{
    jmethodID construction_id = env->GetMethodID(obj_class, "<init>", "()V");
    jobject obj = env->NewObject(obj_class, construction_id);
    return obj;
}

JNIEXPORT jint JNICALL AddInt(JNIEnv* env, jclass cls, jint a, jint b) {
    return a*100+b;
}

JNIEXPORT jstring JNICALL AddStr(JNIEnv* env, jclass cls, jstring a, jstring b) {
    if (0 == env) {
        return 0;
    }

    jstring_2_const_char_ptr strA(env, a);
    if (0 == strA.get()) {
        env->ThrowNew(env->FindClass("java/lang/Exception"), NULL);
        return 0;
    }
    jstring_2_const_char_ptr strB(env, b);
    if (0 == strB.get()) {
        env->ThrowNew(env->FindClass("java/lang/Exception"), NULL);
        return 0;
    }

    jstring strRst;
    strRst = jstrcat(env, strA.get(), strB.get());
/*
    std::string strRst;
    strRst = strA;
    strRst += strB;
    const_char_ptr_2_jstring ResultString(env, strRst.c_str());
    strRst = ResultString.detach();
*/
    return strRst;
}

JNIEXPORT jint JNICALL AddIntByPrivate(JNIEnv* env, jclass cls, jint a) {
    jclass nativeDataProducer = env->FindClass("com/example/testjni/JniNativeDataProducer");
    if (0 == nativeDataProducer) {
        return 0;
    }

    jmethodID mid_getPrivateInt = env->GetStaticMethodID(nativeDataProducer, "getPrivateInt", "()I");
    if (0 == mid_getPrivateInt) {
        return 0;
    }

    int rst = env->CallStaticIntMethod(nativeDataProducer, mid_getPrivateInt);
    rst = a * 100 + rst;

    return rst;
}

JNIEXPORT jstring JNICALL AddStrByPrivate(JNIEnv* env, jclass cls, jstring a) {
    jclass nativeDataProducer = env->FindClass("com/example/testjni/JniNativeDataProducer");
    if (0 == nativeDataProducer) {
        return 0;
    }

    jmethodID mid_getPrivateStr = env->GetMethodID(nativeDataProducer, "getPrivateString", "()Ljava/lang/String;");
    if (0 == mid_getPrivateStr) {
        return 0;
    }

    jobject producer = getInstance(env, nativeDataProducer);
    jstring str = (jstring)env->CallObjectMethod(producer, mid_getPrivateStr);

    const char* aTmp = env->GetStringUTFChars(a, 0);
    const char* strTmp = env->GetStringUTFChars(str, 0);
    jstring strRst = jstrcat(env, strTmp, aTmp);

    return strRst;
}