package com.example.plugino;

import android.app.Application;
import android.util.Log;

import com.example.plugincluster.ModuleMgr;
import com.example.pluginoA.PluginoAModuleForHost;

import java.util.concurrent.atomic.AtomicBoolean;

/**
 * Created by jiao on 2016/6/1.
 */
public class PluginoApplication extends Application {
    private static String TAG = "PluginoApplication";
    public void onCreate() {
        super.onCreate();
        init();
        Log.e(TAG, "onCreate: ");
    }

    private static AtomicBoolean mIsInit = new AtomicBoolean(false);

    public static void init() {
        if (!mIsInit.compareAndSet(false, true)) {
            return;
        }
        registModule();
        initPlugino();
    }

    private static void registModule() {
        ModuleMgr.registModule(ModuleMgr.MODULE_PLUGINOA, PluginoAModuleForHost.getHostModule());
    }

    private static void initPlugino() {
        try {
        } catch (Throwable e) {
            e.printStackTrace();
            throw new RuntimeException("initPlugino init error", e);
        }
    }
}
