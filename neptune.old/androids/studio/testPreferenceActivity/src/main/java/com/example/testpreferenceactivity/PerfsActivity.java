package com.example.testpreferenceactivity;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.preference.EditTextPreference;
import android.preference.ListPreference;
import android.preference.Preference;
import android.preference.PreferenceActivity;
import android.preference.PreferenceFragment;
import android.preference.PreferenceScreen;
import android.os.Bundle;
import android.widget.Button;
import android.widget.Toast;

import java.util.List;

//> PreferenceActivity中很多方法确实过时了，推荐用PreferenceFragment类
public class PerfsActivity extends PreferenceActivity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (hasHeaders()) {
            Button button = new Button(this);
            button.setText("Exit");
            setListFooter(button);
        }
    }

    @Override
    protected boolean isValidFragment(String fragmentName) {
        return true;
    }

    @Override
    public void onBuildHeaders(List<Header> target) {
        loadHeadersFromResource(R.xml.preference_home, target);
    }

    public static class PerfsFragment extends PreferenceFragment implements Preference.OnPreferenceChangeListener {
        private EditTextPreference nickName;
        private ListPreference textSize;
        private Preference cleanHistory;
        private Context context;

        @Override
        public boolean onPreferenceChange(Preference preference, Object newValue) {
            if (preference == nickName) {
                nickName.setSummary(newValue.toString());
            } else if (preference == textSize) {
                setTextSizeSummary(newValue.toString());
            }
            return true;
        }

        @Override
        public void onCreate(Bundle savedInstanceState) {
            super.onCreate(savedInstanceState);

            addPreferencesFromResource(R.xml.settings);
            nickName = (EditTextPreference)findPreference("nickName");
            textSize = (ListPreference)findPreference("textSize");
            cleanHistory = findPreference("cleanHistory");

            nickName.setOnPreferenceChangeListener(this);
            textSize.setOnPreferenceChangeListener(this);

            context = getActivity();
            initSummary();
        }

        @Override
        public boolean onPreferenceTreeClick(PreferenceScreen preferenceScreen, Preference preference) {
            if (preference == cleanHistory) {
                new AlertDialog.Builder(context).setTitle("清除历史记录")
                        .setMessage("是否真的要清除历史记录？")
                        .setPositiveButton("是", new DialogInterface.OnClickListener() {
                            @Override
                            public void onClick(DialogInterface dialog, int which) {
                                Toast.makeText(context, "清除成功", Toast.LENGTH_SHORT).show();
                            }
                        }).setNegativeButton("否", new DialogInterface.OnClickListener() {
                            @Override
                            public void onClick(DialogInterface dialog, int which) {
                                dialog.dismiss();
                            }
                        }).create().show();
            }
            return true;
        }

        // 初始化summary
        private void initSummary() {
            nickName.setSummary(nickName.getText());
            setTextSizeSummary(textSize.getValue());
        }

        private void setTextSizeSummary(String textSizeValue) {
            if (textSizeValue.equals("0")) {
                textSize.setSummary("小");
            } else if (textSizeValue.equals("1")) {
                textSize.setSummary("中");
            } else if (textSizeValue.equals("2")) {
                textSize.setSummary("大");
            }
        }
    }
}