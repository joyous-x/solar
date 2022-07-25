package com.example.testservice;

import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.IBinder;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;

public class MainActivity extends AppCompatActivity {
    private boolean bConnectted = false;
    private ServiceConnection connection = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName name, IBinder service) {
            bConnectted = true;
            //> 不对service指定进程名时，下面的代码可以正常运行，因为用的都是Local对象
            //> 如果service指定进程名(android:process=":testService")，就需要用 AIDL 进行操作
            TestService.BinderForTestService binder = (TestService.BinderForTestService)service;
            binder.greetFromTestService(MainActivity.this);
        }

        //> 在服务崩溃或被杀死导致的连接中断时被调用，而如果我们自己解除绑定时则不会被调用
        @Override
        public void onServiceDisconnected(ComponentName name) {

        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Button btnStart = (Button)findViewById(R.id.btn_service_start);
        Button btnStop  = (Button)findViewById(R.id.btn_service_stop);
        Button btnBind  = (Button)findViewById(R.id.btn_service_bind);
        Button btnUnbind = (Button)findViewById(R.id.btn_service_unbind);


        btnStart.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent(MainActivity.this, TestService.class);
                startService(intent);
            }
        });

        btnStop.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent(MainActivity.this, TestService.class);
                stopService(intent);
            }
        });

        btnBind.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                bind();
            }
        });

        btnUnbind.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                unbind();
            }
        });

    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        unbind();
    }

    public void bind( ) {
        Intent intent = new Intent(this, TestService.class);
        bindService(intent, connection, Context.BIND_AUTO_CREATE);
    }

    public void unbind( ) {
        //> 如果没有bind成功时，调用unbind会抛出异常
        if (bConnectted) {
            unbindService(connection);
            bConnectted = false;
        }
    }
}
