package com.joyous.studio;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;

import com.joyous.studio.module_entry.WelcomeActivity;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        startActivity(new Intent(MainActivity.this, WelcomeActivity.class));

        ShortcutManager shortcut = new ShortcutManager();
        shortcut.addShortcut(this, "sudio(2)", R.mipmap.ic_launcher, MainActivity.class);

        doAction();

        finish();
    }

    private void doAction() {

    }

}
