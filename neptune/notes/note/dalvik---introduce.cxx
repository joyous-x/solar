1、
    a、类文件：Dalvik虚拟机使用的是dex（Dalvik Executable），而Java虚拟机使用的是class格式。
       一个dex文件可以包含若干个类，而一个class文件只包括一个类。所以dex文件可以进行适当的压缩。
       一般来说，包含有相同类的未压缩dex文件稍小于一个已经压缩的jar文件。

    b、指令集：Dalvik虚拟机是基于寄存器的，而Java虚拟机是基于堆栈的。
       由于基于寄存器的指令对目标机器的寄存器进行了假设，因此更利于进行AOT（ahead-of-time）优化。
       所谓AOT，就是在程序运行前，就先编译成本地机器语言程序。AOT本质上是一种静态编译。
       而JIT，是在程序运行时进行编译，生成本地机器语言程序。


2、Dalvik虚拟机的内存大体上可以分为Java Object Heap、Bitmap Memory和Native Heap三种。
   Java Object Heap是用来分配Java对象的，也就是在代码中new出来的对象都是位于Java Object Heap上的。
   Android应用程序进程能够使用的最大内存指的是能够用来分配Java Object的堆。

   Bitmap Memory也称为External Memory，它是用来处理图像的。
   在HoneyComb之前，Bitmap Memory是在Native Heap中分配的，但是这部分内存同样计入Java Object Heap中，
也就是说，Bitmap占用的内存和Java Object占用的内存加起来不能超过Java Object Heap的最大值。所以，
调用BitmapFactory相关接口处理大图像时，会抛出一个OutOfMemoryError异常的原因。
   在HoneyComb以及更高的版本中，Bitmap Memory就直接是在Java Object Heap中分配了，这样就可以直接接受GC的管理。

   Native Heap就是在Native Code中使用malloc等分配出来的内存。


3、JIT
   JIT占用的是运行时间，为了解决时间问题，JIT可能只会选择那些热点代码进行编译或者优化。
根据2-8原则，一个程序80%的时间可能都是在重复执行20%的代码。因此，JIT就可以选择这20%经常执行的代码来进行编译和优化。
由于JIT在编译和优化代码的时候，对程序的运行情况进行了假设，因此，它所采取的激进优化措施又称为赌博，即Gambling。


4、虚拟机最终是通过 Java本地调用(JNI) 调用到操作系统接口


5、进程和线程管理
   虚拟机的进程和线程都是与目标机器本地操作系统的进程和线程一一对应的，这样可以使本地操作系统来调度进程和线程。