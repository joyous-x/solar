package com.example.plugincluster;

import android.util.SparseArray;

import java.util.Objects;

/**
 * Created by jiao on 2016/6/1.
 */
public class ModuleMgr {
    public static int MODULE_PLUGINOA = 1;
    public static int MODULE_HOST = 2;
    private static SparseArray<Object> modules = new SparseArray<>();

    public static void registModule(int type, Object object) {
        modules.put(type, object);
    }

    public static Object getModule(int  type) {
        return modules.get(type);
    }
}
