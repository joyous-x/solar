package com.example.jiao.testacdd;

import org.acdd.framework.ACDD;
import org.acdd.runtime.DelegateComponent;
import org.osgi.framework.Bundle;
import org.osgi.framework.BundleException;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;

/**
 * Created by jiao on 2016/5/31.
 */
public class PluginInstallUtil {
    public static boolean isPluginInstalled(String componentName) {
        return DelegateComponent.locateComponent(componentName) != null;
    }

    public synchronized static boolean installPlugin(String filepath, String packageName) throws FileNotFoundException, BundleException  {
        FileInputStream ins = null;
        try {
            ins = new FileInputStream(new File(filepath));
            Bundle bundle = ACDD.getInstance().installBundle(packageName, ins);
            if (null == bundle) {
                throw new RuntimeException("installPlugin fail");
            }
        } finally {
            try {
                if (ins != null) ins.close();
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
        return true;
    }

    public synchronized static boolean updatePlugin(String filepath, String packageName) throws FileNotFoundException, BundleException {
        ACDD.getInstance().updateBundle(packageName, new FileInputStream(new File(filepath)));
        return true;
    }
}
