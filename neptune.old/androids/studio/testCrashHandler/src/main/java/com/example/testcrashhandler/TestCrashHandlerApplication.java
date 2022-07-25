package com.example.testcrashhandler;

import android.app.Application;

/**
 * Created by jiao on 2016/6/15.
 */
public class TestCrashHandlerApplication extends Application {
    @Override
    public void onCreate() {
        super.onCreate();
        CrashHandler.instance().init(getApplicationContext());
    }
}
