Ashmem 匿名共享内存子系统（Anonymous Shared Memory）
    主体是以驱动程序的形式实现在内核空间的，同时，在系统运行时库层和应用程序框架层提供了访问接口，
其中，在系统运行时库层提供了C/C++调用接口，而在应用程序框架层提供了Java调用接口。
    它有两个特点:
        一是能够辅助内存管理系统来有效地管理不再使用的内存块
        二是它通过Binder进程间通信机制来实现进程间的内存共享。


在Android应用程序框架层，提供了一个MemoryFile接口来封装了匿名共享内存文件的创建和使用，
它实现在frameworks/base/core/java/android/os/MemoryFile.java文件中


Binder进程间通信接口定义在src/shy/luo/ashmem/IMemoryService.java

Ashmem驱动程序实现在kernel/common/mm/ashmem.c文件中
JNI方法native_open实现在frameworks/base/core/jni/adroid_os_MemoryFile.cpp
ashmem_create_region来创建匿名共享内存，这个函数实现在system/core/libcutils/ashmem-dev.c文件中


Ashmem驱动程序是怎么样辅助内存管理系统来有效管理内存的
在 ashmem_init 里通过调用register_shrinker函数向内存管理系统注册一个内存回收算法函数
在Linux内核中，当系统内存紧张时，内存管理系统就会进行内存回收算法。
因此，当内存管理系统进行内存回收时，就会调用到这里的ashmem_shrink函数，让Ashmem驱动程序执行内存回收操作

Ashmem是通过Binder机制实现进程间共享的，共享的就是ashmem关联的文件句柄。句柄跨进程
共享的关键是，struct file结构，它表示一个打开文件结构。在Linux系统中，打开文件结
构struct file是可以在进程间共享的，它与文件描述符不一样。