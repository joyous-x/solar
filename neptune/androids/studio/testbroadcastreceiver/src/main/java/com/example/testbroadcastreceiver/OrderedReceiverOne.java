package com.example.testbroadcastreceiver;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;

public class OrderedReceiverOne extends BroadcastReceiver {
    private String TAG = "BroadcastReceiver --- OrderedReceiverOne";

    public OrderedReceiverOne() {
    }

    @Override
    public void onReceive(Context context, Intent intent) {
        String msg = getResultExtras(true).getString("msg");
        if (null == msg) {
            msg = intent.getStringExtra("msg");
        }
        Log.e(TAG, msg);

        Bundle bundle = new Bundle();
        bundle.putString("msg", msg + "@OrderedReceiverOne");
        setResultExtras(bundle);
    }
}
