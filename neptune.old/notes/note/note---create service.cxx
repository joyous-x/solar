常规：
在服务端：a、Service对象实现：继承Service接口   b、Binder对象实现：继承Binder接口，并实现 [IPC交互接口] 
在客户端：启动service，并注册 ServiceConnection 回调。在回调接口中获取到 IBinder 对象，通过强转将 IBinder 对象转化成[IPC交互接口]
 
AIDL (Android Interface Definition Language) 是一种IDL 语言，用于生成可以在Android设备上两个进程之间进行进程间通信(interprocess communication, IPC)的代码。 
AIDL IPC机制是面向接口的，像COM或Corba一样，但是更加轻量级。它是使用代理类在客户端和实现端传递数据。
AIDL是一种接口定义语言，用于约束两个进程间的通讯规则，供编译器生成代码，实现Android设备上的两个进程间通信(IPC)。 
定义一个aidl文件，具体步骤很简单的：
aidl的定义和interface的定义很类似的，使用interface关键字，但aidl中不能有修饰符(public,protected,private)，不然报错，然后将文件的后缀名.java改成.aidl，此时在gen文件夹下面就会多出一个与之对应的java文件，
这个是编译器干的事情。这个aidl接口中定义的一般都是Client和Service交互的接口。
下面来看一下StudentQuery.aidl文件示例：
	package cn.itcast.aidl;  
	//注意没有任何的访问权限修饰符  
	interface StudentQuery {  
		//通过number来访问学生的name  
		String queryStudent(int number);  
	}  

官方文档特别提醒我们何时使用AIDL是必要的：只有你允许客户端从不同的应用程序为了进程间的通信而去访问你的service，以及想在你的service处理多线程。
在服务端：a、Service对象实现：继承Service接口  b、Binder对象实现：需要实现Binder对象，但是继承的是 [aidl文件中声明的接口名.Stub] 类(里边实现了Binder)
在客户端：因为是跨应用访问的，所以使用隐式Intent开启服务，并注册 ServiceConnection 回调。在回调接口中获取到 IBinder 对象，并通过 [aidl文件中声明的接口名.Stub].asInterface() 函数将 IBinder 对象转化成和服务端交互的接口  
          服务端中aidl文件所在package连同aidl文件一起拷贝到客户端，编译器会自动在客户端的gen目录中同步生成 [aidl文件名.java] 的接口文件
 
 