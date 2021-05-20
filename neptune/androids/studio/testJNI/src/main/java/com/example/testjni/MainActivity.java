package com.example.testjni;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        //> 为了打包so，需要在gradle中增加 jniLibs.srcDirs = ['libs']
        //> 加载生存的so文件时，不用带lib前缀和.so后缀
        System.loadLibrary("TestJni");
        String str = JniFooA.sayHello("JniFooA");

        TextView textView = (TextView)findViewById(R.id.textView);
        textView.setText(str);

        int rstInt = JniFooB.addInt(1, 2);
        Log.e("MainActivity", "JniFooB.addInt : " + rstInt);

        String rstStr = JniFooB.addStr("1", "2");
        Log.e("MainActivity", "JniFooB.addStr : " + rstStr);

        int rstInt2 = JniFooB.addIntByPrivate(3);
        Log.e("MainActivity", "JniFooB.addIntByPrivate : " + rstInt2);

        String rstStr2 = JniFooB.addStrByPrivate("3");
        Log.e("MainActivity", "JniFooB.addStrByPrivate : " + rstStr2);
    }
}
