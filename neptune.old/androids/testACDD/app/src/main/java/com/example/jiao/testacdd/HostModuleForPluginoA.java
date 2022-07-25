package com.example.jiao.testacdd;

import com.example.plugincluster.IHostModuleForPluginoA;

/**
 * Created by jiao on 2016/6/1.
 */
public class HostModuleForPluginoA {
    private static IHostModuleForPluginoA sHostModule;

    public static synchronized IHostModuleForPluginoA getHostModule() {
        if (sHostModule == null) {
            sHostModule = new HostModuleForPluginoAImpl();
        }
        return sHostModule;
    }

    private static class HostModuleForPluginoAImpl implements IHostModuleForPluginoA {

        @Override
        public String getPluginoTextViewContent() {
            return "i am text from host";
        }
    }
}