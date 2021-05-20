package com.example.jiao.testacdd;

import android.content.Context;
import android.content.IntentFilter;
import android.util.Log;

import com.example.plugincluster.ModuleMgr;

import org.acdd.android.compat.ACDDApp;
import org.acdd.framework.ACDD;

import java.lang.reflect.Method;

/**
 * Created by jiao on 2016/5/31.
 */
public class MainEntry extends ACDDApp {
    private Context ctx = null;

    @Override
    protected boolean isPurgeUpdate() {
        return false;
    }

    @Override
    public void onCreate() {
        super.onCreate();
        registLoadBroadcast();
        registHost();
        installPlugin();
        new Runnable() {

            @Override
            public void run() {
                initPlugin();
            }
        }.run();
    }

    @Override
    protected void attachBaseContext(final Context base) {
        super.attachBaseContext(base);
        loadSubDexes();
        ctx = base;
    }

    /**
     * 注册宿主的资源方法供不能反向引用的插件化模块调用
     */
    private void initHost() {

    }

    private void loadSubDexes() {

    }

    private void registLoadBroadcast() {
        PluginoLoadMgr receiver = new PluginoLoadMgr();
        IntentFilter filter = new IntentFilter(PluginLoadUtil.LOAD_BUNDLE_ACTION);
        ctx.registerReceiver(receiver, filter);
    }

    //> 比较耗时，一般在后台进程中加载插件
    private void installPlugin() {
        PluginoInstallMgr.installBundleSync(this);
    }

    private void initPlugin() {
        try {
            if (BuildConfig.DEBUG) {
                Log.e("plugino", "======startModule=====");
            }

            if (true != PluginInstallUtil.isPluginInstalled(PluginConst.PLUGIN_COMPONENT_NAME_PLUGINO)) {
                return ;
            }

            ClassLoader loader = ACDD.getInstance().getBundleClassLoader(PluginConst.PLUGIN_NAME_PLUGINO);
            Class<?> clazz = Class.forName(PluginConst.PLUGIN_APPLICATION_NAME, false, loader);
            Method initMethod = clazz.getMethod("init");
            initMethod.invoke(null);
        } catch (Throwable e) {
            throw new RuntimeException("plugino init failed！", e);
        }
    }

    private void registHost() {
        ModuleMgr.registModule(ModuleMgr.MODULE_HOST, HostModuleForPluginoA.getHostModule());
    }
}
