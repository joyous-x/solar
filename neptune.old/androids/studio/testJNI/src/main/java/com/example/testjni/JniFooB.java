package com.example.testjni;

/**
 * Created by jiao on 2016/8/20.
 */
public class JniFooB {
    public static native int addInt(int a, int b);
    public static native String addStr(String a, String b);
    public static native int addIntByPrivate(int a);
    public static native String addStrByPrivate(String a);
}
