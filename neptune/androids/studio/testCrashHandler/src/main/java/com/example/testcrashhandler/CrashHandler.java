package com.example.testcrashhandler;

import android.content.Context;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Environment;
import android.os.Looper;
import android.util.Log;
import android.widget.Toast;

import java.io.File;
import java.io.FileOutputStream;
import java.io.PrintWriter;
import java.io.StringWriter;
import java.io.Writer;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.HashMap;
import java.util.Map;
import java.lang.reflect.Field;

/**
 * Created by jiao on 2016/6/15.
 */
public class CrashHandler implements Thread.UncaughtExceptionHandler{
    private static final String TAG = "CrashHandler";
    private Context context;
    private static CrashHandler crashHandler;

    private Thread.UncaughtExceptionHandler defaultExceptionHandler;

    public static CrashHandler instance() {
        crashHandler = new CrashHandler();
        return crashHandler;
    }

    public void init(Context context) {
        this.context = context;
        defaultExceptionHandler = Thread.getDefaultUncaughtExceptionHandler();
        Thread.setDefaultUncaughtExceptionHandler(this);
    }

    @Override
    public void uncaughtException(Thread thread, Throwable ex) {
        boolean bHandled = myHandlerException(ex);
        if (false == bHandled && null != defaultExceptionHandler) {
            defaultExceptionHandler.uncaughtException(thread, ex);
        } else {
            try {
                Thread.sleep(3000);
            } catch (InterruptedException e) {
                Log.e(TAG, "uncaughtException: ", e);
            }
            android.os.Process.killProcess(android.os.Process.myPid());
            System.exit(1);
        }
    }

    private boolean myHandlerException(Throwable ex) {
        if (null == ex) return false;

        new Thread() {
            @Override
            public void run() {
                Looper.prepare();
                Toast.makeText(context, "anr error, exit current apk", Toast.LENGTH_SHORT).show();
                Looper.loop();
            }
        }.start();
//
//        for (int i = 0; i < 1000; i++) {
//            try {
//                Thread.sleep(1000);
//            } catch (InterruptedException e) {
//                e.printStackTrace();
//            }
//        }

        saveExceptionInfo(ex);
        return true;
    }

    private void saveExceptionInfo(Throwable ex) {
        Map<String, String> infos = new HashMap<String, String>();

        try {
            PackageManager pm = context.getPackageManager();
            PackageInfo pkgInfo = pm.getPackageInfo(context.getPackageName(), PackageManager.GET_ACTIVITIES);
            if (null != pkgInfo) {
                String versionName = pkgInfo.versionName == null ? "null" : pkgInfo.versionName;
                String versionCode = pkgInfo.versionCode+"";
                infos.put("versionName", versionName);
                infos.put("versionCode", versionCode);
            }
        } catch (PackageManager.NameNotFoundException e) {
            Log.e(TAG, e.toString());
        }

        Field[] fields = Build.class.getDeclaredFields();
        for (Field field : fields) {
            try {
                field.setAccessible(true);
                infos.put(field.getName(), field.get(null).toString());
            } catch (IllegalAccessException e) {
                e.printStackTrace();
            }
        }

        StringBuffer sb = new StringBuffer();
        for (Map.Entry<String, String> entry : infos.entrySet()) {
            String key = entry.getKey();
            String value = entry.getValue();
            sb.append(key+"="+value+"\n");
        }

        Writer writer = new StringWriter();
        PrintWriter printWriter = new PrintWriter(writer);
        ex.printStackTrace(printWriter);
        Throwable cause = ex.getCause();
        while (cause != null) {
            cause.printStackTrace(printWriter);
            cause = cause.getCause();
        }

        printWriter.close();
        String result = writer.toString();
        sb.append(result);

        try {
            DateFormat formatter = new SimpleDateFormat("yyyy-MM-dd-HH-mm-ss");
            long timestamp = System.currentTimeMillis();
            String time = formatter.format(new Date());
            String fileName = "crash-"+time+"-"+timestamp+".log";
            if (Environment.getExternalStorageState().equals(Environment.MEDIA_MOUNTED)) {
                String path = "/sdcard/crash/";
                File dir = new File(path);
                if (!dir.exists()) {
                    dir.mkdirs();
                }
                FileOutputStream fos = new FileOutputStream(path+fileName);
                fos.write(sb.toString().getBytes());
                fos.close();
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
