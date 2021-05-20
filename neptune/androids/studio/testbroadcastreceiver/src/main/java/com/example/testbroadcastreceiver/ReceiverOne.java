package com.example.testbroadcastreceiver;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

public class ReceiverOne extends BroadcastReceiver {
    private String TAG = "BroadcastReceiver --- ReceiverOne";
    public ReceiverOne() {
    }

    @Override
    public void onReceive(Context context, Intent intent) {
        String msg = intent.getStringExtra("msg");
        Log.e(TAG, msg);
        abortBroadcast();
    }
}
