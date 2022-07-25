package com.example.testjunit;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.os.PowerManager;
import android.test.ActivityTestCase;
import android.test.ActivityUnitTestCase;
import android.test.suitebuilder.annotation.MediumTest;
import android.test.suitebuilder.annotation.SmallTest;
import android.util.Log;
import android.view.KeyEvent;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

/**
 * Created by jiao on 2016/6/16.
 */
public class ActivityTest extends ActivityUnitTestCase<MainActivity> {
    private Intent mStartIntent;

    //> 这个自动生成的构造函数是个坑货
    // public ActivityTest(Class<MainActivity> activityClass) {
    //    super(activityClass);
    // }
    public ActivityTest() {
        super(MainActivity.class);
    }

    @Override
    protected void setUp() throws Exception {
        super.setUp();
        mStartIntent = new Intent(getInstrumentation().getTargetContext(), MainActivity.class);
    }

    @Override
    protected void tearDown() throws Exception {
        super.tearDown();
    }

    @SmallTest
    public void testPreconditions() {
        startActivity(mStartIntent, null, null);
        assertNotNull(getActivity());

        Button btn;
    }

    @SmallTest
    public void testTextView() {
        startActivity(mStartIntent, null, null);
        TextView tvHello = (TextView)getActivity().findViewById(R.id.tv_tj_hi);
        tvHello.setText("i am in testcase");
        assertEquals(true, tvHello.getText().equals("i am in testcase"));
    }

    @MediumTest
    public void testLifecycle() {
        MainActivity activity = startActivity(mStartIntent, null, null);

        getInstrumentation().callActivityOnStart(activity);
        getInstrumentation().callActivityOnResume(activity);
        getInstrumentation().callActivityOnPause(activity);
        getInstrumentation().callActivityOnStop(activity);
    }

    @SuppressLint("LongLogTag")
    @MediumTest
    public void testUIbySendKey() {
        MainActivity activity = startActivity(mStartIntent, null, null);
        KeyEvent eventOne = new KeyEvent(KeyEvent.ACTION_DOWN, KeyEvent.KEYCODE_POWER);

        int pid = android.os.Process.myPid();
        Log.e("testJunit_testUIbySendKey", "pid = " + pid);

        PowerManager pm = (PowerManager) activity.getApplicationContext().getSystemService(Context.POWER_SERVICE);
        boolean screen = pm.isScreenOn();
        // assertEquals(false, screen);

        // sendKeys是调用getInstrumentation().sendKeySync实现的
        // sendKeys(KeyEvent.KEYCODE_POWER);
        getInstrumentation().sendKeySync(eventOne);

        screen = pm.isScreenOn();
        assertEquals(true, screen);
    }
}

/*
    getInstrumentation().sendKeySync(eventOne); 提示需要权限，详解：
    http://stackoverflow.com/questions/5383401/android-inject-events-permission
 */