源码：frameworks\native\cmds\servicemanager\service_manager.c

1、binger 分为三部分，binder client、binder server、servicemanager

2、servicemanager 实际上就是一个 binder 服务，只不过是最新创建的那个 binder 服务。
   servicemanager 组件是用来管理Server并且向Client提供查询Server远程接口的功能。
   
3、初始化时的基本操作和普通 binder 服务一样，但是多了一个步骤：
   binder_become_context_manager(bs)
   使自己成功 binder 守护者。   
   随后，就进入一个无穷循环，充当Server的角色，等待Client的请求