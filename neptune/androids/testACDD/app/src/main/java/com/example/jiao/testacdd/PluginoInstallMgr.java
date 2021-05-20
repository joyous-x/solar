package com.example.jiao.testacdd;

import android.content.Context;
import android.content.Intent;
import android.util.Log;

import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.util.ArrayList;

/**
 * Created by jiao on 2016/5/31.
 */
public class PluginoInstallMgr {
    private final static String TAG = "PluginoInstallMgr";
    private final static String ASSET_PLUGIN_PKG_NAME = "plugino.so";
    private final static String PLUGINO_SIGNATURE_MD5 = "";

    public static boolean installBundleSync(final Context context) {
        if (context == null) return false;

        //> 是否已安装判定
        //> TODO:

        try {
            long length = 0;
            File pluginFile = null;

            if (BuildConfig.DEBUG) {
                pluginFile = new File("/sdcard/" + ASSET_PLUGIN_PKG_NAME);
                length = pluginFile.length();
            }
            else {
                String downloadKey = "asset_v_screensaver";
                String dir = context.getFilesDir().getAbsolutePath();
                pluginFile = new File(dir, String.format("SingleDL-%s.tmp", downloadKey));
                length = pullFileFromAssetsDir(context, ASSET_PLUGIN_PKG_NAME, pluginFile);
            }

            if (length <= 0) {
                Log.e(TAG, "Missing plugin => " + pluginFile.getAbsolutePath());
                return false;
            }

            boolean bRet = installPlugin(context, pluginFile, false);
            Log.d(TAG, "Install " + (bRet ? "success" : "failed") + " => " + pluginFile.getAbsolutePath());
            return bRet;
        } catch (Exception e) {
            e.printStackTrace();
        }

        return false;
    }

    //> 安装插件到当前apk中
    private static boolean installPlugin(Context context, File file, boolean signatureCheck) {
        String pluginPackageName = PluginConst.PLUGIN_NAME_PLUGINO;
        String pluginComponentName = PluginConst.PLUGIN_COMPONENT_NAME_PLUGINO;

        if (signatureCheck) {
            String pkgMd5 = PkgSignatureTool.getPkgSignatureMd5(context, file);
            if (pkgMd5 != null && !PLUGINO_SIGNATURE_MD5.equalsIgnoreCase(pkgMd5)) {
                return false;
            }
        }

        try {
            if (!PluginInstallUtil.isPluginInstalled(pluginComponentName)) {
                PluginInstallUtil.installPlugin(file.getAbsolutePath(), pluginPackageName);
            } else {
                PluginInstallUtil.updatePlugin(file.getAbsolutePath(), pluginPackageName);
            }

            sendReloadBundleBroadcast(context, pluginPackageName);
            return true;
        } catch (Exception e) {
            e.printStackTrace();
        }
        return false;
    }

    public static long pullFileFromAssetsDir(Context context, String assertRelativePath, File pluginFile) {
        long length = 0;
        InputStream is = null;
        FileOutputStream fos = null;
        try {
            is = context.getAssets().open(assertRelativePath);
            fos = new FileOutputStream(pluginFile);

            byte[] buffer = new byte[1024];
            int byteCount;
            while ((byteCount = is.read(buffer)) != -1) {
                fos.write(buffer, 0, byteCount);
                length += byteCount;
            }
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            try {
                if (fos != null) {
                    fos.flush();
                    fos.close();
                }
            } catch (Exception e) {
                e.printStackTrace();
            }
            try {
                if (is != null) is.close();
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
        return length;
    }

    //> 通知需要加载插件的进程加载插件
    private static void sendReloadBundleBroadcast(Context ctx, String pluginName) {
        ArrayList<String> pkgNameList = new ArrayList<>(2);
        ArrayList<String> processList = new ArrayList<>(2);

        {
            pkgNameList.add(pluginName);
            processList.add("work");
        }

        Intent intent = new Intent(PluginLoadUtil.LOAD_BUNDLE_ACTION);
        intent = intent.putExtra("fromUpdate", false);
        intent = intent.putStringArrayListExtra("pkgNameList", pkgNameList);
        intent = intent.putStringArrayListExtra("processList", processList);
        ctx.sendBroadcast(intent);
    }
}
