package com.example.testviewanimator;

import android.animation.ObjectAnimator;
import android.animation.ValueAnimator;
import android.app.Activity;
import android.renderscript.Sampler;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.Menu;
import android.view.View;
import android.view.animation.Animation;
import android.view.animation.AnimationSet;
import android.view.animation.ScaleAnimation;
import android.view.animation.TranslateAnimation;
import android.widget.Button;
import android.widget.ViewAnimator;

public class InteractiveBook extends Activity {
    Button prev;
    Button next;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_interactive_book);

        final View rollingBall = findViewById(R.id.rollingball);
        ObjectAnimator ballRoller = ObjectAnimator.ofFloat(rollingBall, "TranslationX", 0, 400);
        ballRoller.setDuration(2000);
        ballRoller.setRepeatMode(ObjectAnimator.REVERSE);
        ballRoller.setRepeatCount(ObjectAnimator.INFINITE);
        ballRoller.start();

        final View bouncingBall = findViewById(R.id.bouncingball);
        final ValueAnimator ballBouncer = ValueAnimator.ofInt(0, 40);
        ValueAnimator.setFrameDelay(50);
        ballBouncer.addUpdateListener(new ValueAnimator.AnimatorUpdateListener() {

            @Override
            public void onAnimationUpdate(ValueAnimator animation) {
                final int animatedValue = (Integer)ballBouncer.getAnimatedValue();
                bouncingBall.post(new Runnable() {
                    @Override
                    public void run() {
                        bouncingBall.setPadding(bouncingBall.getPaddingLeft(), 40-animatedValue, bouncingBall.getPaddingRight(), animatedValue);
                    }
                });
            }
        });
        ballBouncer.setDuration(2000);
        ballBouncer.setRepeatMode(ValueAnimator.REVERSE);
        ballBouncer.setRepeatCount(ValueAnimator.INFINITE);
        ballBouncer.start();

        final AnimationSet slideinToLeft = new AnimationSet(true);
        TranslateAnimation slide1 = new TranslateAnimation(
                Animation.RELATIVE_TO_PARENT, 1f,
                Animation.RELATIVE_TO_PARENT, 0,
                Animation.RELATIVE_TO_SELF, 0,
                Animation.RELATIVE_TO_SELF, 0);
        ScaleAnimation scale1 = new ScaleAnimation(10,1,10,1);
        slideinToLeft.addAnimation(slide1);
        slideinToLeft.addAnimation(scale1);
        slideinToLeft.setDuration(2000);

        final AnimationSet slideoutToLeft = new AnimationSet(true);
        TranslateAnimation slide2 = new TranslateAnimation(
                Animation.RELATIVE_TO_PARENT, 0f,
                Animation.RELATIVE_TO_PARENT, -1f,
                Animation.RELATIVE_TO_SELF, 0,
                Animation.RELATIVE_TO_SELF, 0);
        ScaleAnimation scale2 = new ScaleAnimation(1,10,1,10);
        slideinToLeft.addAnimation(slide2);
        slideinToLeft.addAnimation(scale2);
        slideinToLeft.setDuration(2000);

        final AnimationSet slideinToRight = new AnimationSet(true);
        TranslateAnimation slide3 = new TranslateAnimation(
                Animation.RELATIVE_TO_PARENT, -1f,
                Animation.RELATIVE_TO_PARENT, 0,
                Animation.RELATIVE_TO_SELF, 0,
                Animation.RELATIVE_TO_SELF, 0);
        ScaleAnimation scale3 = new ScaleAnimation(10,1,10,1);
        slideinToLeft.addAnimation(slide3);
        slideinToLeft.addAnimation(scale3);
        slideinToLeft.setDuration(2000);

        final AnimationSet slideoutToRight = new AnimationSet(true);
        TranslateAnimation slide4 = new TranslateAnimation(
                Animation.RELATIVE_TO_PARENT, 0f,
                Animation.RELATIVE_TO_PARENT, 1f,
                Animation.RELATIVE_TO_SELF, 0,
                Animation.RELATIVE_TO_SELF, 0);
        ScaleAnimation scale4 = new ScaleAnimation(1,10,1,10);
        slideinToLeft.addAnimation(slide2);
        slideinToLeft.addAnimation(scale2);
        slideinToLeft.setDuration(2000);


        final ViewAnimator pages = (ViewAnimator)findViewById(R.id.pages);
        prev = (Button)findViewById(R.id.prev);
        next = (Button)findViewById(R.id.next);
        prev.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
                pages.clearAnimation();
                pages.setInAnimation(slideinToLeft);
                pages.setOutAnimation(slideoutToRight);
                pages.showPrevious();
            }
        });
        next.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
                pages.clearAnimation();
                pages.setInAnimation(slideinToLeft);
                pages.setOutAnimation(slideoutToRight);
                pages.showNext();
            }
        });
    }
}
