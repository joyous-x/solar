在系统启动脚本system/core/rootdir/init.rc文件中，我们可以看到启动Zygote进程的脚本命令：
service zygote /system/bin/app_process -Xzygote /system/bin --zygote --start-system-server  
    socket zygote stream 666  
    onrestart write /sys/android_power/request_state wake  
    onrestart write /sys/power/state on  
    onrestart restart media  
    onrestart restart netd  
关键字service告诉init进程创建一个名为"zygote"的进程，这个zygote进程要执行的程序是/system/bin/app_process，后面是要传给app_process的参数。
系统启动脚本system/core/rootdir/init.rc是由init进程解释执行，init进程的源代码位于system/core/init/init.c，是由service_start函数来解释init.rc文件中的service命令


1、
Zygote执行程序system/bin/app_process的入口是main函数
实现：frameworks/base/cmds/app_process/app_main.cpp

2、
在zygote启动过程中，会先执行 runtime.start("com.android.internal.os.ZygoteInit", startSystemServer);  
runtime.start()最终调用的是 AndroidRuntime.start，作用是启动系统运行时库。
它主要做了三件事情：
	一是调用startVM启动虚拟机，
	二是调用startReg注册JNI方法，
	三是调用了com.android.internal.os.ZygoteInit类的main函数。
AndroidRuntime的实现：frameworks/base/core/jni/AndroidRuntime.cpp

3、
zygoteInit.main()
它主要作了三件事情: 
	一是调用registerZygoteSocket函数创建了一个socket接口，用来和ActivityManagerService通讯，
	二是调用startSystemServer函数来启动SystemServer组件，
	三是调用ZygoteInit.runSelectLoopMode函数进入一个无限循环在前面创建的socket接口上等待ActivityManagerService请求创建新的应用程序进程。
实现：frameworks/base/core/java/com/android/internal/os/ZygoteInit.java


4、
上个步骤中的第二个动作---调用startSystemServer函数来启动SystemServer组件：
	a、ZygoteInit.startSystemServer
	Zygote进程通过Zygote.forkSystemServer函数创建一个新的进程来启动SystemServer组件，返回值pid等0的地方就是新的进程要执行的路径，即新创建的进程会执行handleSystemServerProcess函数
		   
	b、ZygoteInit.handleSystemServerProcess
	由于由Zygote进程创建的子进程会继承Zygote进程在上个步骤的第一个动作中创建的Socket文件描述符，而这里的子进程又不会用到它，因此需要调用closeServerSocket来关闭它。
	这个函数接着调用RuntimeInit.zygoteInit函数来进一步执行启动SystemServer组件的操作

	c、RuntimeInit.zygoteInit
	这里会执行两个操作：
	    一、调用zygoteInitNative函数来执行一个Binder进程间通信机制的初始化工作，这个工作完成之后，这个进程中的Binder对象就可以进行进程间通信了
		二、调用上个步骤传进来的com.android.server.SystemServer类的main函数。

5、
Zygote进程就启动完成了，这里总结一下：
    1. 系统启动时init进程会创建Zygote进程，Zygote进程负责后续Android应用程序框架层的其它进程的创建和启动工作。
    2. Zygote进程会首先创建一个SystemServer进程，SystemServer进程负责启动系统的关键服务，如包管理服务PackageManagerService和应用程序组件管理服务ActivityManagerService。
    3. 当我们需要启动一个Android应用程序时，ActivityManagerService会通过Socket进程间通信机制，通知Zygote进程为这个应用程序创建一个新的进程。

		
SystemServer实现：在frameworks/base/services/java/com/android/server/SystemServer.java
ZygoteInit实现：在frameworks/base/core/java/com/android/internal/os/ZygoteInit.java
ZygoteInit.runSelectLoopMode实现：在frameworks/base/core/java/com/android/internal/os/ZygoteInit.java