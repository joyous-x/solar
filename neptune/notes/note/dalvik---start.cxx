1、dalvik虚拟机的启动流程图，见 startview---dalvik.jpg


2、在Android系统中，应用程序进程都是由Zygote进程孵化出来的，而Zygote进程是由Init进程启动的。
Zygote进程在启动时会创建一个Dalvik虚拟机实例，每当它孵化一个新的应用程序进程时，都会将这
个Dalvik虚拟机实例复制到新的应用程序进程里面去，从而使得每一个应用程序进程都有一个独立的Dalvik虚拟机实例。


3、Dalvik虚拟机启动过程，主要就是完成以下四件事：
    a、创建了一个Dalvik虚拟机实例；
    b、加载了Java核心类及其JNI方法；
    c、为主线程的设置了一个JNI环境；
    d、注册了Android核心类的JNI方法。


4、性能
   Zygote进程为了加快Android应用程序进程的启动过程，牺牲了自己的启动速度，
因为它需要加载大量的Java核心类，以及注册大量的Android核心类JNI方法。

frameworks/base/core/jni/AndroidRuntime.cpp