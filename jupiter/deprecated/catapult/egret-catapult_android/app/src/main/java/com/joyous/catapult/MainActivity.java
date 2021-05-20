package com.joyous.catapult;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;
import android.widget.Toast;

import com.joyous.common.admob.AdsRewardHandler;
import com.joyous.common.admob.RewardAds;

import org.egret.runtime.launcherInterface.INativePlayer;
import org.egret.egretnativeandroid.EgretNativeAndroid;

//Android项目发布设置详见doc目录下的README_ANDROID.md

public class MainActivity extends Activity implements AdsRewardHandler {
    private final String TAG = "MainActivity";
    private EgretNativeAndroid nativeAndroid;
    private RewardAds rewardedAd = new RewardAds();
    private boolean DEBUG = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        nativeAndroid = new EgretNativeAndroid(this);
        if (!nativeAndroid.checkGlEsVersion()) {
            Toast.makeText(this, "This device does not support OpenGL ES 2.0.",
                    Toast.LENGTH_LONG).show();
            return;
        }

        if (DEBUG) {
            nativeAndroid.config.showFPS = true;
            nativeAndroid.config.fpsLogTime = 30;
        } else {
            nativeAndroid.config.showFPS = false;
            nativeAndroid.config.fpsLogTime = 0;
        }
        nativeAndroid.config.disableNativeRender = false;
        nativeAndroid.config.clearCache = false;
        nativeAndroid.config.loadingTimeout = 0;

        setExternalInterfaces();
        if (!nativeAndroid.initialize("http://tool.egret-labs.org/Weiduan/game/index.html")) {
            Toast.makeText(this, "Initialize native failed.",
                    Toast.LENGTH_LONG).show();
            return;
        }

        setContentView(nativeAndroid.getRootFrameLayout());
        rewardedAd.init(this, this);
    }

    @Override
    protected void onPause() {
        super.onPause();
        nativeAndroid.pause();
    }

    @Override
    protected void onResume() {
        super.onResume();
        nativeAndroid.resume();
    }


    @Override
    protected void onDestroy() {
        super.onDestroy();
    }

    @Override
    public boolean onKeyDown(final int keyCode, final KeyEvent keyEvent) {
        Log.d(TAG, "onKeyDown: " + keyCode);
        if (keyCode == KeyEvent.KEYCODE_BACK) {
            nativeAndroid.callExternalInterface("onBackPressed", "" + keyCode);
            return false;
        }
        return super.onKeyDown(keyCode, keyEvent);
    }

    private void setExternalInterfaces() {
        nativeAndroid.setExternalInterface("sendToNative", new INativePlayer.INativeInterface() {
            @Override
            public void callback(String message) {
                String str = "Native get message: ";
                str += message;
                Toast.makeText(getApplicationContext(), str, Toast.LENGTH_SHORT).show();
                nativeAndroid.callExternalInterface("sendToJS", str);
            }
        });

        nativeAndroid.setExternalInterface("showNativeAd", new INativePlayer.INativeInterface() {
            @Override
            public void callback(String message) {
                rewardedAd.show();
            }
        });

        nativeAndroid.setExternalInterface("onBackPressedCallback", new INativePlayer.INativeInterface() {
            @Override
            public void callback(String message) {
                if (message.equals("true")) {
                    nativeAndroid.exitGame();
                    System.exit(0);
                }
            }
        });
    }

    public int handleReward(String rtype, int rcnt) {
        nativeAndroid.callExternalInterface("handleAdReward", rtype);
        Log.d(TAG, "handleAdReward: " + rtype + " cnt=" + rcnt);
        return 0;
    }

    public int handleShowFailed(int errorCode) {
        Toast.makeText(this, "Sorry, Show RewardedAd Failed. Try again later.", Toast.LENGTH_LONG).show();
        return 0;
    }

}
