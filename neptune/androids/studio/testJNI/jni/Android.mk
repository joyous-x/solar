##############################################################################
#
# A、jni 中，C/C++ 和 Java 之间的关联都是通过函数实现的，
# B、但是 C/C++ 可以通过 env 来新建或得到已有的 Java 类对象/成员，并调用其方法；
# C、但是 Java 中无法得到 C++ 的对象，只能通过把 C++ 对象绑定到 Java 对象，再在
#    C++ 中获取对应的 Java 对象，进而获取 C++ 对象地址，强转为对应类型然后使用。
#
# 在当前目录运行ndk中的ndk-build.cmd，编译入库为(区分大小写)：Android.mk
#
# 加载so文件时，不能带lib前缀和.so后缀，如：System.loadLibrary("TestJni");
#
# 在jni中使用android的log输出，需要添加：
#		a、mk中增加：LOCAL_LDLIBS += -llog
#		b、c/c++中增加 #include <android/log.h> (目录不固定可以在ndk中搜索log.h来确定)
#       c、具体使用方法参考：android/log.h 文件
#
# 编译X86平台库
# APP_ABI 		:= x86
# 使用android-9以上的平台库
# APP_PLATFORM 	:= android-9
#
##############################################################################

# C/C++代码所在目录，也就是我们的jni目录
LOCAL_PATH		:= $(call my-dir)

# 一个完整模块编译
include $(CLEAR_VARS)

# 编译的源文件
LOCAL_SRC_FILES	:= \
	com_example_testjni_JniFooA.c \
	nativefun.cpp \
	JniTypeUtils.cpp \
	JniFooB.cpp

# 编译生成的目标对象
LOCAL_MODULE 	:= libTestJni

LOCAL_C_INCLUDES:= $(JNI_H_INCLUDE)

# 日志使用
LOCAL_LDLIBS += -llog

# 链接时需要的外部库
LOCAL_SHARED_LIBRARIES := libutils

LOCAL_PRELINK_MODULE 	:= false
LOCAL_MODULE_TAGS 		:=optional

# 指明要编译成动态库
include $(BUILD_SHARED_LIBRARY)

# 为了使用std(其实，依然不能使用std::string，需要继续查阅资料)
APP_CFLAGS += -fexceptions  
APP_STL := gnustl_static  
