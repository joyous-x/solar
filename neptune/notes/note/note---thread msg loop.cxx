一、
线程消息循环处理：
  1、
	应用程序启动时，主线程会准备好一个主消息循环。
  2、
	Android系统提供的HandlerThread类，它具有创建具有消息循环功能的子线程的作用。
    HandlerThread类实现在frameworks/base/core/java/android/os/HandlerThread.java文件中，
  3、
	由于子线程不能直接操作应用程序的UI，因此，需要通过往应用程序的主线程中发送消息来通知应用程序主线程更新界面上的下载进度。
	Android系统为开发人员提供了一个异步任务类（AsyncTask）来实现上面所说的功能，即它会在一个子线程中执行计算任务，同时通过主线程的消息循环来获得更新应用程序界面的机会。
		A. MyTask类继承于AsyncTask类，因此它也是一个异步任务类；
		B. MyTask类的doInBackground函数是在后台的子线程中运行的，这时候它不可以操作应用程序的界面；
		C. MyTask类的onProgressUpdate和onPostExecute两个函数是应用程序的主线程中执行，它们可以操作应用程序的界面。
	AsyncTask类定义在frameworks/base/core/java/android/os/AsyncTask.java文件中
	 
二、
键盘消息处理流程：
	SystemServer ---> WindowManagerService ---> InputManager
	WindowManagerService.main
	实现：frameworks/base/services/java/com/android/server/WindowManagerService.java文件
	InputManager，Java层实现：frameworks/base/services/java/com/android/server/InputManager.java
	InputManager，C++层实现中，分别创建了一个InputReader对象和一个InputDispatcher对象，前者负责读取系统中的键盘消息，后者负责把键盘消息分发出去；
 
