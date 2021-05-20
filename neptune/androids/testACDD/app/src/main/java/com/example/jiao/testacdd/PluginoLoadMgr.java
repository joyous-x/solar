package com.example.jiao.testacdd;

/**
 * Created by jiao on 2016/6/1.
 */
public class PluginoLoadMgr extends PluginLoadUtil{
    @Override
    String getCurProcessName() {
        return PluginConst.PROCESS_NAME;
    }
}
