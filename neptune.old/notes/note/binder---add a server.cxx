media服务启动流程
源码：frameworks\av\media\mediaserver\main_mediaserver.cpp

1、sp<ProcessState> proc(ProcessState::self());
ProcessState::self() ：ProcessState单实例的引用方式。
ProcessState 的构造函数中：
    1、mDriverFD(open_driver())
    2、mVMStart = mmap(0, BINDER_VM_SIZE, PROT_READ, MAP_PRIVATE | MAP_NORESERVE, mDriverFD, 0);
	 
2、sp<IServiceManager> sm = defaultServiceManager();
sm对象在下文中没有使用，看似是无用语句，其实是通过 defaultServiceManager() 调用，
在添加服务实例前构造好 ServiceManager 对象
声明：frameworks\native\include\binder\IServiceManager.h
实现：frameworks\native\libs\binder\IServiceManager.cpp

3、CameraService::instantiate(); 创建服务实例，并加入到 servicemanager。
一般都是通过继承模版public BinderService<CameraService>, 来实现。BinderService<>中实现了，BinderService的几种常见的静态方法。
但是，MediaPlayerService 是自己实现了静态方法 instantiate()。

4、ProcessState::self()->startThreadPool(); 启动消息处理的looper
PoolThread的位置：
声明：frameworks\native\include\binder\ProcessState.h
实现：frameworks\native\libs\binder\ProcessState.cpp

5、IPCThreadState::self()->joinThreadPool();
当前线程加入线程池


4、5两个步骤很奇怪，因为最终的执行语句都是：IPCThreadState::self()->joinThreadPool(true); 只不过在不同线程而已。

Thread的源码：
声明：system\core\include\utils\threads.h
实现：system\core\libutils\Threads.cpp


IPCThreadState* IPCThreadState::self()
线程状态，保存在TLS中，就是说，每条线程对应一份IPCThreadState。
在IPCThreadState::self()中，构造TLS内容的方法，比较有意思：
  1、检查是否有gTlsKey，如果没有，则创建TLS的key：gTlsKey
  2、如果有gTlsKey，就从TLS中取出 IPCThreadState, 如果为空, 
     则创建 IPCThreadState 对象; 如果不为空, 则返回取出的值。
  (IPCThreadState* st = (IPCThreadState*)pthread_getspecific(k);)
  3、在 IPCThreadState 的构造函数中，设置TLS内容。再次访问时可以不重复创建。
声明：rameworks\native\include\binder\IPCThreadState.h
实现：frameworks\native\libs\binder\IPCThreadState.cpp 
