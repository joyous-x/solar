一、
startService ：在新进程中启动自定义服务的原理分析：

首先，Activity中通过 startService(new Intent("service name")); 启动服务。类Activity继承了ContextWrapper类，
而startService函数就是在ContextWrapper类中实现的。最终又调用了ActivityManagerProxy类的startService来实现启动服务的操作。
	
总结起来就是，Android系统通过三次Binder进程间通信完成了在新进程中启动服务的过程，三次操作分别是：
    1. 从主进程调用到ActivityManagerService进程中，完成新进程的创建；
    2. 从新进程调用到ActivityManagerService进程中，获取要在新进程启动的服务的相关信息；
    3. 从ActivityManagerService进程又回到新进程中，最终将服务启动起来。 

	
ContextWrapper实现：frameworks/base/core/java/android/content/ContextWrapper.java	
ActivityManagerService实现：frameworks/base/services/java/com/android/server/am/ActivityManagerService.java


二、
bindService ：这是一种在应用程序进程内部启动Service的方法。它的实现也在ContextWrapper类中实现的。
Android应用程序绑定服务（bindService）的过程，大致为：
    1. MainActivity调用bindService函数通知ActivityManagerService，它要启动CounterService这个服务，ActivityManagerService于是在MainActivity所在的进程内部把CounterService启动起来，并且调用它的onCreate函数；
    2. ActivityManagerService把CounterService启动起来后，继续调用CounterService的onBind函数，要求CounterService返回一个Binder对象给它；
    3. ActivityManagerService从CounterService处得到这个Binder对象后，就把它传给MainActivity，即把这个Binder对象作为参数传递给MainActivity内部定义的ServiceConnection对象的onServiceConnected函数；
    4. MainActivity内部定义的ServiceConnection对象的onServiceConnected函数在得到这个Binder对象后，就通过它的getService成同函数获得CounterService接口。