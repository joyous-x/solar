package com.example.testjni;

/**
 * Created by jiao on 2016/8/20.
 */
public class JniNativeDataProducer {
    public String getPrivateString() {
        return "private " + this;
    }
    public static int getPrivateInt() {
        return 777;
    }
}
