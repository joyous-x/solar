package com.example.testbroadcastreceiver;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;

public class OrderedReceiverTwo extends BroadcastReceiver {
    private String TAG = "BroadcastReceiver --- OrderedReceiverTwo";

    public OrderedReceiverTwo() {
    }

    @Override
    public void onReceive(Context context, Intent intent) {
        String msg = getResultExtras(true).getString("msg");
        if (null == msg) {
            msg = intent.getStringExtra("msg");
        }
        Log.e(TAG, msg);

        Bundle bundle = new Bundle();
        bundle.putString("msg", msg + "@OrderedReceiverTwo");
        setResultExtras(bundle);
        //> 可以中断广播的链式传递
        // abortBroadcast();
    }
}
