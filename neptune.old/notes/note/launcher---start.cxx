Android系统的桌面(home)应用程序Launcher的启动以及包管理


Android系统在启动的过程中，会启动一个应用程序管理服务PackageManagerService 来做包的管理 以及 辅助实现双击桌面图标启动应用程序等功能。
Android系统的桌面(home)应用程序Launcher的启动流程如下：

1、SystemServer.main
   实现：frameworks/base/services/java/com/android/server/SystemServer.java
2、SystemServer.init1
   这个函数是一个JNI方法
   实现：frameworks/base/services/jni/com_android_server_SystemServer.cpp
3、libsystem_server.system_init
   函数system_init实现在libsystem_server库中
   实现：frameworks/base/cmds/system_server/library/system_init.cpp
4、
   随后，初始化SurfaceFlinger、MediaPlayerService、CameraService等服务，
   后续，通过系统全局唯一的AndroidRuntime实例的callStatic来调用SystemServer的init2函数
5、AndroidRuntime.callStatic
   实现：frameworks/base/core/jni/AndroidRuntime.cpp
6、SystemServer.init2
   在这里创建的ServerThread的run函数中，启动PackageManagerService、ActivityManagerServcie等服务
   实现：frameworks/base/services/java/com/android/server/SystemServer.java
7、PackageManagerService.main
   这里负责把已安装的应用程序解析后得到的package、provider、service、receiver和activity等信息保存在PackageManagerService服务中
   实现：frameworks/base/services/java/com/android/server/PackageManagerService.java
   此时，已安装的应用程序只是相当于在PackageManagerService服务中注册好了，如果想要在Android桌面上看到这些应用程序，还需要有一个Home应用程序，
   负责从PackageManagerService服务中把已注册的应用程序取出来，并以友好的方式在桌面上展现出来，例如以快捷图标的形式。
   在Android系统中，负责把系统中已经安装的应用程序在桌面中展现出来的Home应用程序就是Launcher。
8、ActivityManagerService.setSystemProcess
   实现：frameworks/base/services/java/com/android/server/am/ActivityManagerServcie.java
   这里会将ActivityManagerService实例添加到ServiceManager中托管，这样就可以通过ServiceManager.getService接口来访问这个全局唯一的ActivityManagerService实例了。
   后续会构造ActivityStack，通过CATEGORY_HOME类型的Intent去启动Launcher(一般只有Launcher应用程序注册了HOME类型的Activity)，
   在Launcher的onCreate函数中将已解析的Activity中Action类型为Intent.ACTION_MAIN，并且Category类型为Intent.CATEGORY_LAUNCHER的Activity信息保
   存在 ApplicationInfo 对象中。
   Launcher会根据ApplicationInfo展示桌面图标，同时，可以通过Launcher(其实就是一个Activity)响应click等事件，来完成启动某一应用等各种功能。

   
Android系统的Home应用程序Launcher是由ActivityManagerService启动的，而ActivityManagerService和PackageManagerService一样，都是在开机时由SystemServer组件启动的。
SystemServer组件首先是启动PackageManagerService，由它来负责安装系统的应用程序。
系统中的应用程序安装好了以后，SystemServer组件接下来就要通过ActivityManagerService来启动Home应用程序Launcher了，
Launcher在启动的时候便会通过PackageManagerServic把系统中已经安装好的应用程序以快捷图标的形式展示在桌面上，这样用户就可以使用这些应用程序了。
	