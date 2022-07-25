package com.joyous.floatwin;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.Gravity;
import android.view.MotionEvent;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.Toast;

public class MainActivity extends AppCompatActivity {
    private Button btClose;
    private Button btOpen;
    private Button btStartActivity;

    private LinearLayout viewFloatWin;
    private WindowManager winManager;
    private WindowManager.LayoutParams winLayout;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main_floatwin);

        btClose = (Button)this.findViewById(R.id.floatwin_close);
        btOpen = (Button)this.findViewById(R.id.floatwin_open);
        btStartActivity = (Button)this.findViewById(R.id.floatwin_interactwithservice);

        btClose.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                try {
                    Toast.makeText(getApplicationContext(), "close", Toast.LENGTH_SHORT).show();
                    closeFloatWin();
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        });

        btOpen.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                try {
                    Toast.makeText(getApplicationContext(), "open", Toast.LENGTH_SHORT).show();
                    showFloatWin();
                } catch (Exception e) {
                    e.printStackTrace();
                }

            }
        });

        btStartActivity.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent(MainActivity.this, InteractWithService_Activity.class);
                startActivity(intent);
            }
        });

        try {
            createWindowManager();
            createFloatWin();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public void onDestroy() {
        super.onDestroy();
    }

    private void createWindowManager() {
        winManager = (WindowManager)getApplication().getSystemService(this.WINDOW_SERVICE);

        // 窗体的布局样式
        winLayout = new WindowManager.LayoutParams();

        // 设置窗体显示类型——TYPE_SYSTEM_ALERT(系统提示)
        winLayout.type = WindowManager.LayoutParams.TYPE_SYSTEM_ALERT;

        // 设置浮动窗口不可聚焦（实现操作除浮动窗口外的其他可见窗口的操作）
        winLayout.flags = WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE;

        // 设置显示的模式, 背景透明
        // winLayout.format = PixelFormat.RGBA_8888;

        // 设置对齐的方法
        winLayout.gravity = Gravity.TOP | Gravity.LEFT;

        winLayout.x = 0;
        winLayout.y = 0;

        // 设置窗体宽度和高度
        winLayout.width = WindowManager.LayoutParams.WRAP_CONTENT;
        winLayout.height = WindowManager.LayoutParams.WRAP_CONTENT;
    }

    private void createFloatWin() {
        viewFloatWin = (LinearLayout)LayoutInflater.from(getApplication()).inflate(R.layout.floatwin, null);
        viewFloatWin.setOnTouchListener(new android.view.View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                float x = event.getRawX();
                float y = event.getRawY();

                switch (event.getAction()) {
                    case MotionEvent.ACTION_DOWN:
                        break;
                    case MotionEvent.ACTION_MOVE:
                        winLayout.x = (int)(x - 0);
                        winLayout.y = (int)(y - 0);
                        winManager.updateViewLayout(viewFloatWin, winLayout);
                        break;
                    case MotionEvent.ACTION_UP:
                        winLayout.x = (int)(x - 0);
                        winLayout.y = (int)(y - 0);
                        winManager.updateViewLayout(viewFloatWin, winLayout);
                        break;
                }

                return true;
            }
        });
    }

    private void showFloatWin() {
        winManager.addView(viewFloatWin, winLayout);
    }

    private void closeFloatWin() {
        winManager.removeView(viewFloatWin);
    }
}
