在Java代码中使用的Log.x(TAG,“message”)系列方法，在c/c++代码中也一样可以，不过首先你要include相关头文件。但是使用不同的编译环境，对应的头文件略有不同。

#### 一、完整源码编译环境
只要include <utils/Log.h>头文件，就可以使用对应的LOGI、LOGD等方法了，同时请定义LOG_TAG，LOG_NDEBUG等宏值，示例代码如下：
``` 
#define LOG_TAG "HelloJni"
    
#define LOG_NDEBUG 0  
#define LOG_NIDEBUG 0  
#define LOG_NDDEBUG 0  
      
#include <string.h>  
#include <jni.h>  
#include <utils/Log.h>  
    
jstring Java_com_HelloJni_stringFromJNI(JNIEnv* env,jobject thiz){  
    LOGI("Call stringFromJNI!\n");  
    return (*env)->NewStringUTF(env, "Hello from JNI !");  
}
```

与日志相关的.h头文件，在以下源码路径：
```
myeclair\frameworks\base\include\utils\Log.h
myeclair\system\core\include\cutils\log.h
```

#### 二、在NDK环境下编译
需要#include <android/log.h>，示例代码如下：
```
#define LOG_TAG "HelloJni"  
  
#include <string.h>  
#include <jni.h>  
#include <utils/Log.h>

jstring Java_com_HelloJni_stringFromJNI(JNIEnv* env,jobject thiz){  
    __android_log_print(ANDROID_LOG_INFO,LOG_TAG,"Call stringFromJNI!\n");  
    return (*env)->NewStringUTF(env, "Hello from JNI !");  
}
```

注意，其中输出日志的方法是： __android_log_print(....) ， 并不是LOG.x(...)系列方法。不过这种方式在完整源码环境下也是可用的，因此，可以用一下的头文件来统两种环境下的差异：
```
#ifndef __JNILOGGER_H_  
#define __JNILOGGER_H_  
  
#include <android/log.h>  
  
#ifdef _cplusplus  
extern "C" {  
#endif  
  
#ifndef LOG_TAG  
#define LOG_TAG    "MY_LOG_TAG"  
#endif  
  
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)  
#define LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)  
#define LOGW(...)  __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)  
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)  
#define LOGF(...)  __android_log_print(ANDROID_LOG_FATAL,LOG_TAG,__VA_ARGS__) 

#ifdef __cplusplus  
}  
#endif  
  
#endif /* __JNILOGGER_H_ */
```

#### 编译时的区别
编译时需要在 Android.mk 文件中加入对类库的应用，两种环境下分别是：
```
```

1 ifeq ($(HOST_OS),windows)  
2 #NDK环境下  
3     LOCAL_LDLIBS := -llog  
4 else  
5 #完整源码环境下  
6     LOCAL_SHARED_LIBRARIES := libutils  
7 endif