package com.example.testbroadcastreceiver;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;

/**
 * Created by jiao on 2016/6/5.
 */
public class OrderedBroadcastThree extends BroadcastReceiver {
    private String TAG = "BroadcastReceiver --- OrderedBroadcastThree";

    @Override
    public void onReceive(Context context, Intent intent) {
        String msg = getResultExtras(true).getString("msg");
        if (null == msg) {
            msg = intent.getStringExtra("msg");
        }
//        //> 这种方式不能获取 setResultExtras 的数据
//        String msg = intent.getStringExtra("msg");
        Log.e(TAG, msg);

        Bundle bundle = new Bundle();
        bundle.putString("msg", msg + "@OrderedBroadcastThree");
        setResultExtras(bundle);
    }
}
