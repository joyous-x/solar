package com.joyous.common.admob;

public interface AdsRewardHandler {
    int handleReward(String rtype, int rcnt);
    int handleShowFailed(int errorCode);
}