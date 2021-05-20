package com.example.pluginoA;

import android.content.Context;
import android.content.Intent;

import com.example.plugincluster.IPluginoAModuleForHost;

/**
 * Created by jiao on 2016/6/1.
 */
public class PluginoAModuleForHost {

    private static IPluginoAModuleForHost sPluginoA;

    public static synchronized IPluginoAModuleForHost getHostModule() {
        if (sPluginoA == null) {
            sPluginoA = new PluginoAModuleForHostImpl();
        }
        return sPluginoA;
    }

    private static class PluginoAModuleForHostImpl implements IPluginoAModuleForHost {

        @Override
        public void startPluginoMainActivity(Context ctx) {
//            Intent intent = new Intent(ctx, ModuleAMain.class);
//            ctx.startActivity(intent);

            Intent intPlugin = new Intent();
            intPlugin.setClassName(ctx, "com.example.pluginoA.ModuleAMain");
            ctx.startActivity(intPlugin);

        }
    }
}
