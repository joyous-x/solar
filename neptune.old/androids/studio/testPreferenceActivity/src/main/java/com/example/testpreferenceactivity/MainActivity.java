package com.example.testpreferenceactivity;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.graphics.Color;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

public class MainActivity extends Activity {
    private static final String TAG = "TestPreferenceActivity";
    private TextView textView;

    private static final int SETTINGS_ID = 0;
    private static final int EXIT_ID = 1;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        textView = (TextView)findViewById(R.id.textview_pa);
        showSettings();

        Button btnSetting = (Button)findViewById(R.id.btn_pa_setting);
        Button btnExit = (Button)findViewById(R.id.btn_pa_exit);
        btnExit.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                finish();
            }
        });
        btnSetting.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent(MainActivity.this, PerfsActivity.class);
                startActivityForResult(intent, 1);
            }
        });

    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        super.onCreateOptionsMenu(menu);
        menu.add(0, SETTINGS_ID, 0, "Settings");
        menu.add(0, EXIT_ID, 0, "Quit");
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        if (item.getItemId() == SETTINGS_ID) {
            Intent intent = new Intent(MainActivity.this, PerfsActivity.class);
            //> 如果requestCode >= 0 则返回结果时会回调 onActivityResult()方法
            startActivityForResult(intent, 1);
        } else {
            finish();
        }
        return true;
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        showSettings();
    }

    private void showSettings() {
        //> 固定格式的名字：[packagename]_preferences
        String prefsName = getPackageName() + "_preferences";
        SharedPreferences prefs = getSharedPreferences(prefsName, Context.MODE_PRIVATE);

        String nickName = prefs.getString("nickName", "机器人");
        textView.setText("欢迎你: " + nickName);

        boolean nightMode = prefs.getBoolean("nightMode", false);
        textView.setBackgroundColor(nightMode ? Color.BLACK : Color.WHITE);

        String textSize = prefs.getString("textSize", "0");
        if (textSize.equals("0")) {
            textView.setTextSize(18f);
        } else if (textSize.equals("1")) {
            textView.setTextSize(22f);
        } else if (textSize.equals("2")) {
            textView.setTextSize(36f);
        }
    }
}
