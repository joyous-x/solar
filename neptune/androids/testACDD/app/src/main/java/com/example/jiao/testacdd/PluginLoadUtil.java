package com.example.jiao.testacdd;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;

import java.util.List;

/**
 * Created by jiao on 2016/6/1.
 */
public abstract class PluginLoadUtil extends BroadcastReceiver {
    public static final String LOAD_BUNDLE_ACTION = "com.example.jiao.plugino.load";
    private static boolean bHasInstalledPlugin = false;
    private String curProcessName = "";

    abstract String getCurProcessName();

    @Override
    public void onReceive(Context context, Intent intent) {
        if (intent == null) return;

        boolean fromUpdate = intent.getBooleanExtra("fromUpdate", true);
        if (fromUpdate) {
            doLoadPlugin(intent);
            return ;
        }

        if (bHasInstalledPlugin) {
            return;
        } else {
            bHasInstalledPlugin = true;
        }

        doLoadPlugin(intent);
    }

    private void doLoadPlugin(Intent intent) {
        List<String> pkgNameList = intent.getStringArrayListExtra("pkgNameList");
        List<String> processList = intent.getStringArrayListExtra("processList");
        if (pkgNameList == null || processList == null) return;

        final String currentProcess = getCurProcessName();
        for (int i = 0; i < processList.size(); i++) {
            String pkgName = pkgNameList.get(i);
            String process = processList.get(i);

            if (!currentProcess.equals(process)) continue;

            switch (pkgName) {
                case PluginConst.PLUGIN_NAME_PLUGINO:
                    boolean isInstall = PluginInstallUtil.isPluginInstalled(PluginConst.PLUGIN_COMPONENT_NAME_PLUGINO);
                    if (true == isInstall) break;
                    //> TODO
                    //> 新版ACDD没有此动作
                    // Framework.tryLoadBundleInstance(pkgName);
                    break;
            }
        }
    }
}
