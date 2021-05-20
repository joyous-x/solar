Android应用程序框架层创建应用程序的进程


在Android应用程序框架层中，是由ActivityManagerService组件负责为Android应用程序创建新的进程的，它是运行在一个在系统启动的过程中创建的独立进程之中。
Android应用程序框架层创建的应用程序进程具有两个特点，一是进程的入口函数是ActivityThread.main，二是进程天然支持Binder进程间通信机制；这两个特点都是在进程的初始化过程中实现的。

ActivityManagerService启动新的进程是从其成员函数startProcessLocked开始的。
函数startProcessLocked中调用了Process.start 函数开始为应用程序创建新的进程，注意，它传入一个第一个参数为"android.app.ActivityThread"，
这就是进程初始化时要加载的Java类了，把这个类加载到进程之后，就会把它里面的静态成员函数main作为进程的入口点。

详细情况如下：
---> Process.startViaZygote
---> zygoteSendArgsAndGetPid
---> sZygoteWriter写数据 [函数中sZygoteWriter是一个由openZygoteSocketIfNeeded函数打开的Socket写入流]
---> 写入的数据在ZygoteInit.runSelectLoopMode函数侦听 ---> peers.get(index)得到ZygoteConnection对象[表示一个Socket连接]，接着调用ZygoteConnection.runOnce函数处理
---> Zygote.forkAndSpecialize 创建子进程 ---> 子进程[pid==0]中调用ZygoteConnection.handleChildProc --->  RuntimeInit.zygoteInit 
---> RuntimeInit.zygoteInitNative 执行Binder驱动程序初始化 ---> RuntimeInit.invokeStaticMain执行真正的加载动作[传入的className为"android.app.ActivityThread"]
---> ClassLoader.loadClass加载className对象，然后获取它的静态成员函数main 
---> 没有直接调用静态成员函数main，而是通过抛出异常ZygoteInit.MethodAndArgsCaller，让ZygoteInit.main函数在捕获这个异常时再调用它
     为什么呢？注释说，是为了清理堆栈，这样会让android.app.ActivityThread类的main函数觉得自己是进程的入口函数，而事实上，在此之前已经做了大量的工作。
---> 异常处理中调用 android.app.ActivityThread类的main函数就被执行 ---》 Looper.loop() 这样，我们以后就可以在这个进程中启动Activity或者Service了。



Process.start实现：frameworks/base/core/java/android/os/Process.java
ActivityThread.main实现：frameworks/base/core/java/android/app/ActivityThread.java
ZygoteInit.runSelectLoopMode实现：frameworks/base/core/java/com/android/internal/os/ZygoteInit.java		
ZygoteConnection.handleChildProc实现：frameworks/base/core/java/com/android/internal/os/ZygoteConnection.java	
ActivityManagerService.startProcessLocked实现：frameworks/base/services/java/com/android/server/am/ActivityManagerService.java

RuntimeInit.zygoteInit实现：frameworks/base/core/java/com/android/internal/os/RuntimeInit.java
RuntimeInit.zygoteInitNative是一个Native函数, 实现在 frameworks/base/core/jni/AndroidRuntime.cpp 文件中，其实现中的 gCurRuntime 对象实际就是
一个AppRuntime类对象。AppRuntime类，定义在frameworks/base/cmds/app_process/app_main.cpp。
