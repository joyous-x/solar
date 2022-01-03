package com.joyous.common.admob;

import android.app.Activity;
import android.content.Context;
import android.util.Log;

import androidx.annotation.NonNull;

import com.google.android.gms.ads.AdRequest;
import com.google.android.gms.ads.MobileAds;
import com.google.android.gms.ads.initialization.InitializationStatus;
import com.google.android.gms.ads.initialization.OnInitializationCompleteListener;
import com.google.android.gms.ads.rewarded.RewardItem;
import com.google.android.gms.ads.rewarded.RewardedAd;
import com.google.android.gms.ads.rewarded.RewardedAdCallback;
import com.google.android.gms.ads.rewarded.RewardedAdLoadCallback;


public class RewardAds extends RewardedAdCallback {
    private static final String TAG = "RewardAds";
    private RewardedAd rewardedAd;
    private Context context;
    private AdsRewardHandler rewardAdHandler;

    // 设置在 AndroidManifest.xml 中
    private static final String googleAdAppID = "ca-app-pub-5271832886118855~1361674556";
    // for test
    // private static final String googleAdAppID = "ca-app-pub-3940256099942544~3347511713";

    private static final String googleAdUnitID = "ca-app-pub-5271832886118855/1793947067";
    // for test
    // private static final String googleAdUnitID = "ca-app-pub-3940256099942544/5224354917";

    public void init(Activity activity, AdsRewardHandler rewardAdHandler) {
        MobileAds.initialize(activity, new OnInitializationCompleteListener() {
            @Override
            public void onInitializationComplete(InitializationStatus initializationStatus) {
                Log.d(TAG, "onInitializationComplete");
            }
        });
        this.context = activity;
        this.rewardAdHandler = rewardAdHandler;
        this.rewardedAd = createAndLoadRewardedAd(this.context);
    }

    public boolean show() {
        if (this.rewardedAd.isLoaded()) {
            this.rewardedAd.show((Activity) this.context, this);
            return true;
        } else {
            if (this.rewardAdHandler != null) {
                this.rewardAdHandler.handleShowFailed(-1);
            }
        }
        return false;
    }


    public RewardedAd createAndLoadRewardedAd(Context context) {
        RewardedAd rewardedAd = new RewardedAd(context,this.googleAdUnitID);
        RewardedAdLoadCallback adLoadCallback = new RewardedAdLoadCallback() {
            @Override
            public void onRewardedAdLoaded() {
                Log.d(TAG, "onRewardedAdLoaded");
            }

            @Override
            public void onRewardedAdFailedToLoad(int errorCode) {
                // Ad failed to load.
                String msg = "";
                if (errorCode == AdRequest.ERROR_CODE_INTERNAL_ERROR) {
                    msg = "ERROR_CODE_INTERNAL_ERROR"; // 内部出现问题；例如，收到广告服务器的无效响应。
                } else if (errorCode == AdRequest.ERROR_CODE_INVALID_REQUEST) {
                    msg = "ERROR_CODE_INVALID_REQUEST"; // 广告请求无效；例如，广告单元 ID 不正确。
                } else if (errorCode == AdRequest.ERROR_CODE_NETWORK_ERROR) {
                    msg = "ERROR_CODE_NETWORK_ERROR"; // 由于网络连接问题，广告请求失败
                } else if (errorCode == AdRequest.ERROR_CODE_NO_FILL) {
                    msg = "ERROR_CODE_NO_FILL"; // 广告请求成功，但由于缺少广告资源，未返回广告。
                } else {
                    msg = "" + errorCode;
                }
                Log.d(TAG, "onRewardedAdFailedToLoad: " + msg);
            }
        };
        rewardedAd.loadAd(new AdRequest.Builder().build(), adLoadCallback);
        return rewardedAd;
    }

    @Override
    public void onRewardedAdClosed() {
        this.rewardedAd = createAndLoadRewardedAd(this.context);
    }

    @Override
    public void onRewardedAdOpened() {
        // Ad opened.
        Log.d(TAG, "onRewardedAdOpened");
    }


    @Override
    public void onUserEarnedReward(@NonNull RewardItem reward) {
        // User earned reward.
        String rtype = reward.getType();
        int rcnt = reward.getAmount();
        if (this.rewardAdHandler != null) {
            this.rewardAdHandler.handleReward(rtype, rcnt);
        }
    }

    @Override
    public void onRewardedAdFailedToShow(int errorCode) {
        // Ad failed to display
        Log.d(TAG, "onRewardedAdFailedToShow: " + errorCode);
        if (this.rewardAdHandler != null) {
            this.rewardAdHandler.handleShowFailed(errorCode);
        }
    }
}
