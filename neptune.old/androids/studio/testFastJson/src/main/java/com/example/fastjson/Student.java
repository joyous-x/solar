package com.example.fastjson;

/**
 * Created by kingsoft on 2016/5/21.
 */
public class Student {
    private String name = null;
    private String sexuality = null;
    private int[] bwh = {0,0,0};

    public static String Female = "female";
    public static String Male = "male";

    public Student() {
    }
    public Student(String n, String s, int[] b) {
        name = n;
        sexuality = s;
        bwh = b;
    }

    public String getName() {
        return name;
    }

    public String getSexuality() {
        return sexuality;
    }

    public int[] getBwh() {
        return bwh;
    }

    public void setName(String n) {
        name = n;
    }

    public void setSexuality(String s) {
        sexuality = s;
    }

    public void setBwh(int[] b) {
        bwh = b;
    }

    @Override
    public String toString() {
        return "name="+name+" sexuality="+sexuality+" bwh="+bwh[0]+","+bwh[1]+","+bwh[2];
        // return "name="+name+" sexuality="+sexuality+" bwh="+bwh.toString();
    }
}
