package com.example.pluginoA;

import android.app.Activity;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;

import com.example.plugincluster.IHostModuleForPluginoA;
import com.example.plugincluster.IPluginoAModuleForHost;
import com.example.plugincluster.ModuleMgr;
import com.example.pluginomodulea.R;

public class ModuleAMain extends Activity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_module_amain);

        Button btn = (Button)findViewById(R.id.plugin_btn);
        btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String text = null;
                IHostModuleForPluginoA module = (IHostModuleForPluginoA) ModuleMgr.getModule(ModuleMgr.MODULE_HOST);
                if (null == module) {
                    text = "can't get host module";
                } else {
                    text = module.getPluginoTextViewContent();
                }
                ((Button)v).setText(text);
            }
        });
    }
}
