package com.example.testcontentprovider;

import android.database.ContentObserver;
import android.os.Handler;
import android.os.Message;

/**
 * Created by jiao on 2016/6/12.
 */
public class PersonObserver extends ContentObserver {
    private Handler handler;

    public PersonObserver(Handler handler) {
        super(handler);
        this.handler = handler;
    }

    public void onChange(boolean selfChange) {
        super.onChange(selfChange);
        Message msg = new Message();
        handler.sendMessage(msg);
    }
}
