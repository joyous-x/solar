package com.example.testjunit;

import android.app.Activity;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;

public class MainActivity extends Activity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        int pid = android.os.Process.myPid();
        Log.e("testJunit_MainActivity", "pid = " + pid);
    }
}
