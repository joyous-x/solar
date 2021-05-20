package com.joyous.studio.module_entry;

import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.os.Bundle;

import java.lang.reflect.Constructor;

/**
 * Created by kingsoft on 2015/9/11.
 */
public class ModulesEntry {
    private static final String module_entry__floatwin = "floatwin";
    private static final String module_entry__notifycations = "notifications";
    private static final String module_entry__animation = "animations";
    private static final String module_entry__testFastJson = "testFastJson";
    private static final String module_entry__testTaskMode = "testTaskMode";
    private static final String module_entry__testViewAnimator = "testViewAnimator";

    public static void invokeFloatWinEntry(Context ctx) {
        String strModuleEntry = getModuleEntry(ctx, module_entry__floatwin);
        invokeModuleEntry(ctx, strModuleEntry);
    }

    public static void invokeNotificationEntry(Context ctx) {
        String strModuleEntry = getModuleEntry(ctx, module_entry__notifycations);
        invokeModuleEntry(ctx, strModuleEntry);
    }

    public static void invokeAnimationEntry(Context ctx) {
        String strModuleEntry = getModuleEntry(ctx, module_entry__animation);
        invokeModuleEntry(ctx, strModuleEntry);
    }

    public static void invokeTestTaskMode(Context ctx) {
        String strModuleEntry = getModuleEntry(ctx, module_entry__testTaskMode);
        invokeModuleEntry(ctx, strModuleEntry);
    }

    public static void invokeTestFastJson(Context ctx) {
        String strModuleEntry = getModuleEntry(ctx, module_entry__testFastJson);
        invokeModuleEntry(ctx, strModuleEntry);
    }

    public static void invokeTestViewAnimator(Context ctx) {
        String strModuleEntry = getModuleEntry(ctx, module_entry__testViewAnimator);
        invokeModuleEntry(ctx, strModuleEntry);
    }

    private static String getModuleEntry(Context ctx, String strKey) {
        Bundle metaData = getAppMetaData(ctx);
        if (isBundleUnpractical(metaData)) {
            return null;
        }
        return metaData.getString(strKey);
    }

    public static boolean isBundleUnpractical(Bundle bundle) {
        return bundle == null || bundle.size() <= 0;
    }

    private static Bundle getAppMetaData(Context ctx) {
        Bundle metaData = null;

        try {
            ApplicationInfo appInfo = ctx.getPackageManager().getApplicationInfo(
                                            ctx.getPackageName(), PackageManager.GET_META_DATA);
            if (null != appInfo) {
                metaData = appInfo.metaData;
            }
        } catch (Exception e) {
            e.printStackTrace();
        }

        return metaData;
    }

    /**
     * 通过调用入口类的带Context参数的构造函数来执行，构造函数通常情况下会使用context.startActivity()来调出真正的主界面
     * ??? 为什么不从 entryClass.getConstructor 取得构造函数
     */
    private static void invokeModuleEntry(Context ctx, String strModuleEntry) {
        try {
            Class<?> entryClass = Class.forName(strModuleEntry);
            Constructor constructor = entryClass.getDeclaredConstructor(Context.class);
            if (null != constructor) {
                constructor.newInstance(ctx);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
