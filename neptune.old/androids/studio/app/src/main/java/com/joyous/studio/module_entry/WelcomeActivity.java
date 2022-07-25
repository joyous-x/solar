package com.joyous.studio.module_entry;

import android.app.Activity;
import android.content.SharedPreferences;
import android.os.CountDownTimer;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.TextView;

import com.joyous.studio.R;

public class WelcomeActivity extends AppCompatActivity {
    private int mCntDown = 2;
    CountDownTimerHelper timer;

    private void startPage() {
        timer.cancel();
        //> ModulesEntry.invokeFloatWinEntry(WelcomeActivity.this);
        //> ModulesEntry.invokeNotificationEntry(WelcomeActivity.this);
        ModulesEntry.invokeAnimationEntry(WelcomeActivity.this);
        //> ModulesEntry.invokeTestTaskMode(WelcomeActivity.this);
        //> ModulesEntry.invokeTestFastJson(WelcomeActivity.this);
        //> ModulesEntry.invokeTestViewAnimator(WelcomeActivity.this);
        finish();
    }

    private Handler handler = new Handler() {
        public void handleMessage(Message msg) {
            TextView textView = (TextView) findViewById(R.id.text_countdown);

            switch (msg.what) {
                case 1110: {
                    if (0 < mCntDown) {
                        textView.setText("跳过" + mCntDown);
                    } else if (0 == mCntDown){
                        textView.setText("跳过0");
                        startPage();
                    }
                    mCntDown--;
                    break;
                }
                case 1111: {
                    //> 不在此处做跳转动作是因为：
                    //>    倒数时，从 最后一次onTick 到 finish 会花费2倍间隔的时间
                    finish(); ;
                    break;
                }
                default:
                    break;
            }
            super.handleMessage(msg);
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_welcome);
        showSomething();

        this.findViewById(R.id.imagebtn_startpage).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                startPage();
            }
        });

        this.findViewById(R.id.countdown_background).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                startPage();
            }
        });

        timer = new CountDownTimerHelper(handler, 111, (mCntDown+2)*1000, 1000);
        timer.start();
    }


    private void showSomething() {
        String text = "---这是第(%d)次使用---";
        TextView textView = (TextView)this.findViewById(R.id.imagebtn_startpage_text);
        textView.setText(String.format(text, updateData()));
    }

    private int updateData() {
        SharedPreferences sharedPre = getSharedPreferences("welcomedata", Activity.MODE_PRIVATE);
        int n = sharedPre.getInt("key", 1);

        SharedPreferences.Editor editor = sharedPre.edit();
        editor.putInt("key", n + 1);
        editor.commit();

        return n;
    }


    public class CountDownTimerHelper extends CountDownTimer {
        private Handler mhandler = null;
        private int     mObtainId = 0;

        public CountDownTimerHelper(Handler handler, int nObtainId, long millisInFuture, long interval) {
            super(millisInFuture, interval);
            mhandler  = handler;
            mObtainId = nObtainId;
        }

        @Override
        public void onFinish() {
            Message message = mhandler.obtainMessage(mObtainId);
            message.what = 1111;
            mhandler.sendMessageDelayed(message, 0);
        }

        @Override
        public void onTick(long millisUntilFinished) {
            Message message = mhandler.obtainMessage(mObtainId);
            message.what = 1110;
            mhandler.sendMessageDelayed(message, 0);
        }
    }

}
