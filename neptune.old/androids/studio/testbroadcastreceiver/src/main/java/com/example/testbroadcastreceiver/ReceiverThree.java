package com.example.testbroadcastreceiver;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

/**
 * Created by jiao on 2016/6/5.
 */
public class ReceiverThree extends BroadcastReceiver {
    private String TAG = "BroadcastReceiver --- ReceiverThree";

    @Override
    public void onReceive(Context context, Intent intent) {
        String msg = intent.getStringExtra("msg");
        Log.e(TAG, msg);
    }
}
