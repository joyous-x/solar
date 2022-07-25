binder内核实现：
声明：kernel\common\drivers\staging\android\binder.c
实现：kernel\common\drivers\staging\android\binder.h


device_initcall(binder_init);
在其中注册binder设备：ret = misc_register(&binder_miscdev);
杂项设备作为字符设备的封装，为字符设备提供的简单的编程接口。

binder_proc结构：
	threads树用来保存binder_proc进程内用于处理用户请求的线程，它的最大数量由max_threads来决定；
	node树成用来保存binder_proc进程内的Binder实体, 是binder_node->rb_node形成的红黑树的根；
	refs_by_desc树和refs_by_node树用来保存binder_proc进程内的Binder引用，
	即引用的其它进程的Binder实体，它分别用两种方式来组织红黑树，refs_by_desc是以句柄作来key值来组织，
	refs_by_node是以引用的实体节点的地址值作来key值来组织，它们都是表示同一样东西，
	只不过是为了内部查找方便而用两个红黑树来表示。


用户空间请求binder_ioctl动作时，用到了
struct binder_write_read {
	signed long	write_size;	/* bytes to write */
	signed long	write_consumed;	/* bytes consumed by driver */
	unsigned long	write_buffer;
	signed long	read_size;	/* bytes to read */
	signed long	read_consumed;	/* bytes consumed by driver */
	unsigned long	read_buffer;
};
其中，具体数据的存储write_bufffer和read_buffer所指向的是struct binder_transaction_data，在其中有 
union {
	size_t handle;
    void *ptr;
} target;
结构，
其中的handle就是指要使用的binder_node的desc, 通过这个值可以在binder_proc的refs_by_desc红黑树中找到要使用的binder实体，
ptr是指在binder_node的地址值, 通过这个值可以在binder_proc的refs_by_node红黑树中找到要使用的binder实体。
前一种，适用于用户客户端请求binder实体的操作，后一种适用于binder服务端处理完成后的响应时查找binder实体。
(客户端请求时通过handler找到对应的binder实体，找到服务进程；
服务端应答时，通过binder_thread的transaction_stack结构中保存的请求者的binder_thread信息，找到请求者)


binder_open，每次调用都会生成一个 binder_proc 结构
binder_ioctl，每个调用此函数的线程都会生成一个 binder_thread，并保存在以 proc->threads.rb_node 根节点的红黑树
binder_node，每个binder实体都会对应一个 binder_node。
	结构体中的rb_node和dead_node组成一个联合体。
	如果这个Binder实体还在正常使用，则使用rb_node来连入proc->nodes所表示的红黑树，这棵红黑树用来组织属于这个进程的所有Binder实体；
	如果这个Binder实体所属的进程已经销毁，而这个Binder实体又被其它进程所引用，则这个Binder实体通过dead_node进入到一个哈希表中去存放。
	proc成员变量就是表示这个Binder实例所属于进程了。
	refs成员变量把所有引用了该Binder实体的Binder引用连接起来构成一个链表。	
每个binder_proc有个todo队列，binder_proc中的每个binder_thread也有一个todo队列。