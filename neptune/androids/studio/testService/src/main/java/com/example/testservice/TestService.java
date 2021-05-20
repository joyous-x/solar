package com.example.testservice;

import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.os.Binder;
import android.os.IBinder;
import android.util.Log;
import android.widget.Toast;

public class TestService extends Service {
    private String TAG = "TestService";

    public TestService() {
    }

    @Override
    public void onCreate() {
        super.onCreate();
        Log.e(TAG, "onCreate");
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        Log.e(TAG, "onStartCommand");
        return super.onStartCommand(intent,flags,startId);
    }

    @Override
    public void onStart(Intent intent, int startId) {
        super.onStart(intent, startId);
        Log.e(TAG, "onStart");
    }


    @Override
    public IBinder onBind(Intent intent) {
        Log.e(TAG, "onBind");
        return new BinderForTestService();
    }

    @Override
    public boolean onUnbind(Intent intent) {
        Log.e(TAG, "onUnbind");
        return super.onUnbind(intent);
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        Log.e(TAG, "onDestroy");
    }

    public class BinderForTestService extends Binder {
        public void greetFromTestService(Context ctx) {
            Toast.makeText(ctx, "i am greeting from testservice", Toast.LENGTH_SHORT).show();
            Log.e(TAG, "greetFromTestService");
        }
    }
}
