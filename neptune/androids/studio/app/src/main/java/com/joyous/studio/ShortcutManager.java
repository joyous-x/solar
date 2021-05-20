package com.joyous.studio;

import android.content.ContentResolver;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.ProviderInfo;
import android.content.pm.ResolveInfo;
import android.database.Cursor;
import android.net.Uri;
import android.os.Parcelable;
import android.util.Log;


import java.util.List;

/**
 * Created by kingsoft on 2015/9/22.
 */
public class ShortcutManager {
    private static String TAG = "ShortcutManager";

    private static String[] arrayOfString;
    static
    {
        arrayOfString = new String[9];
        arrayOfString[0] = "com.sec.android.app.launcher";
        arrayOfString[1] = "org.adwfreak.launcher";
        arrayOfString[2] = "org.adw.launcher";
        arrayOfString[3] = "com.fede.launcher";
        arrayOfString[4] = "com.qihoo360.launcher";
        arrayOfString[5] = "net.qihoo.launcher";
        arrayOfString[6] = "com.lge.launcher";
        arrayOfString[7] = "com.huawei.android.launcher";
        arrayOfString[8] = "com.miui.home";
    }

    public boolean addShortcut(Context c, String strShortcutName, int nResID, Class<?> cTarget) {
        if (!isAllowed()) {
            return false;
        }

        if (isShortcutExist(c, strShortcutName)) {
            return false;
        }

        createShortcut(c, strShortcutName, nResID, cTarget);
        return true;
    }

    private boolean isAllowed() {
        //> TODO: 增加开关控制
        return true;
    }

    private boolean isShortcutExist(Context c, String strShortcutName) {
        String strShortCutUri = getShortcutUri(c);
        int    nShortcutCount = getShortcutCount(c, strShortcutName, strShortCutUri);
        return (nShortcutCount > 0);
    }

    private void createShortcut(Context c, String strName, int nResID, Class<?> cTarget) {
        Intent shortcut = new Intent("com.android.launcher.action.INSTALL_SHORTCUT");
        shortcut.putExtra("duplicate", true); //> 不可重复
        shortcut.putExtra(Intent.EXTRA_SHORTCUT_NAME, strName);

        Parcelable icon = Intent.ShortcutIconResource.fromContext(c, nResID);
        shortcut.putExtra(Intent.EXTRA_SHORTCUT_ICON_RESOURCE, icon);
        //> 使用以下语句不能创建快捷方式，详情请查看 EXTRA_SHORTCUT_ICON_RESOURCE 和 EXTRA_SHORTCUT_ICON 的区别
        //> shortcut.putExtra(Intent.EXTRA_SHORTCUT_ICON, icon);

        //> 注意：关联的Activity的 Action(必需) 和 Category(可选) 值
        Intent intent = new Intent(Intent.ACTION_MAIN);
        intent.addCategory(Intent.CATEGORY_LAUNCHER);
        intent.setClass(c, cTarget);
        //> 可以附加其它信息，以在被启动的activity中获取相关信息，如：
        //> intent.putExtra("fromShortCut", true);
        shortcut.putExtra(Intent.EXTRA_SHORTCUT_INTENT, intent);

        c.sendBroadcast(shortcut);
    }
/*
     content://com.example.project:200/folder/subfolder/etc
    \-scheme-/\-host:port(authority)-/\--------path--------/
    host:port 即 authority，用于唯一标识一个ContentProvider，外部调用者可以根据这个标识来找到它
*/
    private String getShortcutUri(Context c) {
        String strUri = "";
        String strAuthority = getAuthority(c);
        String strLaunchPkgName = getLauncherPkgName(c);

        if (strAuthority == null || strAuthority.length() < 0) {
            for (int i = 0; i < arrayOfString.length; i++) {
                if (strLaunchPkgName.contains(arrayOfString[i])) {
                    strAuthority = arrayOfString[i];
                    break;
                }
            }
        }

        if (strAuthority != null && strAuthority.trim().length() > 0) {
            strUri = "content://" + strAuthority + "/favorites?notify=true";
        } else {
            if (android.os.Build.VERSION.SDK_INT < 8) {
                strUri = "content://com.android.launcher.settings/favorites?notify=true";
            } else {
                strUri = "content://com.android.launcher2.settings/favorites?notify=true";
            }
        }

        return strUri;
    }

    private String getLauncherPkgName(Context c) {
        Intent intent = new Intent(Intent.ACTION_MAIN);
        intent.addCategory(Intent.CATEGORY_HOME);
        ResolveInfo resInfo = c.getPackageManager().resolveActivity(intent, 0);
        if (null == resInfo) return "";
        return resInfo.activityInfo.packageName;
    }

    private String getAuthority(Context c) {
        try {
            String strPkgName = getLauncherPkgName(c);
            List<PackageInfo> pkgs = c.getPackageManager().getInstalledPackages(PackageManager.GET_PROVIDERS);

            for (PackageInfo pkg : pkgs) {
                if (!strPkgName.equals(pkg.packageName)) {
                    continue;
                }

                ProviderInfo[] providers = pkg.providers;
                for (ProviderInfo provider : providers) {
                    boolean bRead  = provider.readPermission != null && provider.readPermission.contains("READ_SETTINGS");
                    boolean bWrite = provider.writePermission!= null && provider.writePermission.contains("WRITE_SETTINGS");
                    if (!(bRead || bWrite)) {
                        continue;
                    }

                    return provider.authority;
                }
            }
        } catch (Exception e) {
            Log.d(TAG, e.toString());
        }
        return null;
    }

    private int getShortcutCount(Context c, String strShortcutName, String strUri) {
        int nShortcutCount = 0;
        Cursor cursor = null;

        try {
            ContentResolver resolver = c.getContentResolver();
            cursor = resolver.query(Uri.parse(strUri), null, "title=?", new String[]{strShortcutName}, null);
            if (null != cursor) {
                nShortcutCount = cursor.getCount();
            }
        } catch (Exception e) {
            Log.d(TAG, e.toString());
        } finally {
            if (null != cursor) cursor.close();
        }

        return nShortcutCount;
    }
}
