package com.joyous.animations;

import android.content.DialogInterface;
import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.animation.AlphaAnimation;
import android.view.animation.Animation;
import android.view.animation.AnimationSet;
import android.view.animation.AnimationUtils;
import android.view.animation.RotateAnimation;
import android.view.animation.ScaleAnimation;
import android.view.animation.TranslateAnimation;
import android.widget.Button;
import android.widget.ImageView;

public class AnimationsActivity extends AppCompatActivity{
    Button btnScale;
    Button btnTranslate;
    Button btnAlpha;
    Button btnRotate;
    ImageView image;
    Button btnFrame;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_animations);

        btnAlpha = (Button)this.findViewById(R.id.btn_animation_alpha);
        btnRotate = (Button)this.findViewById(R.id.btn_animation_rotate);
        btnScale = (Button)this.findViewById(R.id.btn_animation_scale);
        btnTranslate = (Button)this.findViewById(R.id.btn_animation_translate);

        btnTranslate.setOnClickListener(new AnimationListener());
        btnScale.setOnClickListener(new AnimationListener());
        btnRotate.setOnClickListener(new AnimationListener());
        btnAlpha.setOnClickListener(new AnimationListener());

        image = (ImageView)this.findViewById(R.id.imageview_animation_ctx);

        btnFrame = (Button)this.findViewById(R.id.btn_frame_animation);
        btnFrame.setOnClickListener(
                new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        Intent intent = new Intent(AnimationsActivity.this, FrameAnimationActivity.class);
                        startActivity(intent);
                    }
                }
        );
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_animations, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    class AnimationListener implements View.OnClickListener {
        public void onClick(View v) {
            int id = v.getId();

            if (R.id.btn_animation_alpha == id) {
                alpha(); //> 代码生成动画
                animationFromXML(R.anim.alpha); //> 从xml中加载动画
            } else if (R.id.btn_animation_rotate == id) {
                rotate();
                animationFromXML(R.anim.rotate);
            } else if (R.id.btn_animation_translate == id) {
                translate();
                animationFromXML(R.anim.translate);
            } else if (R.id.btn_animation_scale == id) {
                scale();
                animationFromXML(R.anim.scale);
                animationFromXML(R.anim.muti_animation); //> 从xml中加载叠加动画
            }

            return ;
        }

        private void alpha() {
            AnimationSet animationSet = new AnimationSet(true);
            AlphaAnimation alphaAnimation = new AlphaAnimation(1, 0);
            alphaAnimation.setDuration(1000); //> 动画执行时间
            animationSet.addAnimation(alphaAnimation);
            image.startAnimation(animationSet);
        }

        private void scale() {
            AnimationSet animationSet = new AnimationSet(true);

            ScaleAnimation scaleAnimation = new ScaleAnimation(
                    0, 0.1f, 0, 0.1f, Animation.RELATIVE_TO_SELF, 0.5f, Animation.RELATIVE_TO_SELF, 0.5f);
            scaleAnimation.setDuration(1000);
            animationSet.addAnimation(scaleAnimation);
            image.startAnimation(animationSet);
        }

        private void rotate() {
            AnimationSet animationSet = new AnimationSet(true);

            RotateAnimation animation = new RotateAnimation(0, 360, Animation.RELATIVE_TO_SELF, 0.5f, Animation.RELATIVE_TO_SELF, 0.5f);
            animation.setDuration(1000);
            animationSet.addAnimation(animation);
            image.startAnimation(animationSet);
        }

        private void translate() {
            AnimationSet animationSet = new AnimationSet(true);

            TranslateAnimation animation = new TranslateAnimation(
                    Animation.RELATIVE_TO_SELF, 0f, Animation.RELATIVE_TO_SELF, 0.5f,
                    Animation.RELATIVE_TO_SELF, 0f, Animation.RELATIVE_TO_SELF, 0.5f);
            animation.setDuration(1000);
            animationSet.addAnimation(animation);
            image.startAnimation(animationSet);
        }

        private void animationFromXML(int animID) {
            Animation animation = AnimationUtils.loadAnimation(AnimationsActivity.this, animID);
            image.startAnimation(animation);
        }
    }


}
