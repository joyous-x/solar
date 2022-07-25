package com.joyous.floatwin;

import android.app.Service;
import android.content.Intent;
import android.os.Binder;
import android.os.IBinder;

/**
 * Created by kingsoft on 2015/9/9.
 */
public class FloatWin_Service extends Service {
    Boolean bExit = true;
    InteractWithService_Activity.FloatWinCallback floatWinCallback;

    @Override
    public IBinder onBind(Intent intent) {
        return new ServiceBinder();
    }

    public void onCreate() {
        super.onCreate();
        startWork();
        System.out.println("runnable running : start");
    }

    public void onDestroy() {
        bExit = false;
        System.out.println("runnable running : destory");
        super.onDestroy();
    }

    public int onStartCommand(Intent intent, int flags, int startId) {
        //> startWork();
        return super.onStartCommand(intent, flags, startId);
    }

    private void startWork() {
        new Thread() {
            public void run() {
                int i = 0;
                while(bExit) {
                    String strServiceShow = "runnable running : " + i++;
                    System.out.println(strServiceShow);

                    try {
                        //> 广播方式实现 activity 和 service 交互
                        sendBroadcastMsg(strServiceShow);

                        //> binder方式实现 activity 和 service 交互
                        floatWinCallback.callback(strServiceShow);
                        sleep(1000);
                    } catch(Exception e) {
                        e.printStackTrace();
                    }
                }
            }
        }.start();
    }

    private void sendBroadcastMsg(String msg){
        //> 广播消息
        Intent intent = new Intent(FloatWinUtils.strBroadcastReceiverAction);
        intent.putExtra(FloatWinUtils.strBroadcastReceiverKey, msg);
        sendBroadcast(intent);
    }



    public class ServiceBinder extends Binder {
        FloatWin_Service getService(){
            return FloatWin_Service.this;
        }
    }

    public void setCallback(InteractWithService_Activity.FloatWinCallback callback) {
        floatWinCallback = callback;
    }
}
