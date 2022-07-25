package com.example.jiao.testacdd;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

import com.example.plugincluster.IPluginoAModuleForHost;
import com.example.plugincluster.ModuleMgr;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Button btn = (Button)findViewById(R.id.host_btn);
        btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                IPluginoAModuleForHost plugin = (IPluginoAModuleForHost)ModuleMgr.getModule(ModuleMgr.MODULE_PLUGINOA);
                if (plugin == null) {
                    Toast.makeText(MainActivity.this, "plugin : " + PluginConst.PLUGIN_NAME_PLUGINO + "is not installed", Toast.LENGTH_LONG).show();
                }
                else {
                    plugin.startPluginoMainActivity(MainActivity.this);
                }
            }
        });
    }
}
