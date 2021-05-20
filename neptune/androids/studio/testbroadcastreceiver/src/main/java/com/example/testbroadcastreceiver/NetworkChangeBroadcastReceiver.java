package com.example.testbroadcastreceiver;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.util.Log;
import android.widget.Toast;

/**
 * Created by jiao on 2016/6/5.
 */
public class NetworkChangeBroadcastReceiver extends BroadcastReceiver {
    private String TAG = "NetworkChangeBroadcastReceiver";
    @Override
    public void onReceive(Context context, Intent intent) {
        Log.e(TAG, "onReceive");

        NetworkInfo info = getNetWorkAvailable(context);
        if (null == info) {
            Toast.makeText(context, "network disconnected!", Toast.LENGTH_SHORT).show();
        } else {
            String str = info.getExtraInfo();
            int nType = info.getType();
            if (nType == ConnectivityManager.TYPE_WIFI) {
                str = str + "**type=wifi";
            }
            Toast.makeText(context, "network " + str + "connected", Toast.LENGTH_SHORT).show();
        }
    }

    private NetworkInfo getNetWorkAvailable(Context ctx) {
        ConnectivityManager mgr = (ConnectivityManager) ctx.getSystemService(Context.CONNECTIVITY_SERVICE);
        NetworkInfo info = mgr.getActiveNetworkInfo();
        if (info != null && info.isAvailable()) {
            return info;
        }
        return null;
    }
}