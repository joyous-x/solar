//
// Created by jiao on 2016/8/20.
//
#include <android/log.h>
#include <stdio.h>
#include "nativefun.h"

//> 详情参考：note---jni的映射实现.md
//> 后两个函数会调用java类
static JNINativeMethod g_func_testjni[] = {
    {("addInt"), ("(II)I"), (void*)(AddInt)},
    {("addStr"), ("(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;"), (void*)(AddStr)},
    {("addIntByPrivate"), ("(I)I"), (void*)(AddIntByPrivate)},
    {("addStrByPrivate"), ("(Ljava/lang/String;)Ljava/lang/String;"), (void*)(AddStrByPrivate)},
};

int RegisterNatives_util(JNIEnv* env, jclass clazz) {
	int nRC = 0;

	nRC = env->RegisterNatives(clazz, g_func_testjni, sizeof(g_func_testjni)/sizeof(g_func_testjni[0]));
	if (JNI_TRUE != nRC) return nRC;

	return JNI_TRUE;
}

void UnregisterNatives_util(JNIEnv* env, jclass clazz) {
    env->UnregisterNatives(clazz);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

//定义目标类名称
static const char* const className = "com/example/testjni/JniFooB";
jint JNI_OnLoad(JavaVM* vm, void* reserved){
    jint result = JNI_ERR;
    JNIEnv* env = NULL;

    __android_log_print(ANDROID_LOG_INFO, "nativefun", "jni : call JNI_OnLoad begin ! \n");

    if (vm->GetEnv((void**)&env, JNI_VERSION_1_6) != JNI_OK) {
        return result;
    }

    jclass clazz = env->FindClass(className);
    if (clazz == NULL) {
        return result;
    }

    if (0 != RegisterNatives_util(env, clazz)) {
        __android_log_print(ANDROID_LOG_INFO, "nativefun", "jni : RegisterNatives_util failed ! \n");
        return result;
    }

    __android_log_print(ANDROID_LOG_INFO, "nativefun", "jni : call JNI_OnUnload success ! \n");
    return JNI_VERSION_1_6;
}

void JNI_OnUnload(JavaVM* vm, void* reserved){
    __android_log_print(ANDROID_LOG_INFO, "nativefun", "jni : call JNI_OnUnload! \n");

    JNIEnv *env = 0;
    if (vm->GetEnv((void**)&env, JNI_VERSION_1_6) != JNI_OK) {
        return;
    }

    jclass clazz = env->FindClass(className);
    if (clazz == NULL) {
        return ;
    }
    UnregisterNatives_util(env, clazz);
}