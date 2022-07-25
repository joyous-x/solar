package com.example.testviewpager;

import android.content.Context;
import android.graphics.Canvas;
import android.support.v4.view.ViewPager;
import android.util.AttributeSet;
import android.util.Log;

/**
 * Created by jiao on 2017/2/14.
 */

public class MyViewPager extends ViewPager {
    public MyViewPager(Context context) {
        super(context);
    }

    public MyViewPager(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    @Override
    protected void onDraw(Canvas canvas) {
        Log.e("xxxxxx", "MyViewPager::onDraw");
        super.onDraw(canvas);
    }
}
