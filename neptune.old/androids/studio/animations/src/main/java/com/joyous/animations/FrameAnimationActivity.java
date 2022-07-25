package com.joyous.animations;

import android.content.DialogInterface;
import android.graphics.drawable.AnimationDrawable;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.view.animation.LayoutAnimationController;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.Toast;

public class FrameAnimationActivity extends AppCompatActivity {
    Button btnFrameAnim = null;
    Button btnLayoutAnim = null;
    ImageView image = null;
    LinearLayout layout;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_frame_animation);

        btnFrameAnim = (Button)this.findViewById(R.id.btn_frame_animation);
        btnFrameAnim.setOnClickListener(new BtnFrameListener());
        image = (ImageView)this.findViewById(R.id.imageview_frame_animation);

        btnLayoutAnim = (Button)this.findViewById(R.id.btn_animation_ctl);
        btnLayoutAnim.setOnClickListener(new BtnLayoutListener());

        layout = (LinearLayout)this.findViewById(R.id.linear_layout_frame_2);

        Toast.makeText(this, "Button 所在的 layout 使用了 xml 的方式来使用 LayoutAnimationController"
                       , Toast.LENGTH_LONG).show();
    }

    class BtnFrameListener implements View.OnClickListener{
        public void onClick(View v) {
            image.setBackgroundResource(R.anim.frame_anim);
            AnimationDrawable animationDrawable = (AnimationDrawable)image.getBackground();
            animationDrawable.start();
        }
    }

    class BtnLayoutListener implements View.OnClickListener{
        public void onClick(View v) {
            //> Button 所在的 layout 使用了 xml 的方式来使用 LayoutAnimationController
            //> ImageView 所在的 layout 使用了代码的方式来使用 LayoutAnimationController
            //>
            //> LayoutAnimationController 是一次性动画
            Animation alpha = AnimationUtils.loadAnimation(FrameAnimationActivity.this, R.anim.alpha);
            LayoutAnimationController ctl = new LayoutAnimationController(alpha);
            layout.setLayoutAnimation(ctl);
            image.setBackgroundResource(R.drawable.lions);

            Toast.makeText(FrameAnimationActivity.this, "ImageView 所在的 layout 使用了代码的方式来使用 LayoutAnimationController"
                    , Toast.LENGTH_LONG).show();
        }
    }
}
