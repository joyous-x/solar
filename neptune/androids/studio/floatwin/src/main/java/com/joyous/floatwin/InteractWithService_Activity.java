package com.joyous.floatwin;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.Looper;
import android.os.Message;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

public class InteractWithService_Activity extends Activity {
    Button buttonStartService;
    Button buttonStopService;
    Button buttonBindService;
    Button buttonUnbindService;
    EditText textServiceShow;
    Button buttonBackActivity;

    //> bindService 方式启动服务, 并跟服务交互
    ServiceConnection serviceCon;

    interface FloatWinCallback {
        void callback(String str);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_interact_with_service);
        setAllButton();
    }

    protected void onDestory() {
        super.onDestroy();
        stopHandler();
    }

    private void setAllButton() {
        buttonBindService = (Button)this.findViewById(R.id.floatwin_bindservice);
        buttonUnbindService = (Button)this.findViewById(R.id.floatwin_unbindservice);
        buttonStartService = (Button)this.findViewById(R.id.floatwin_startservice);
        buttonStopService = (Button)this.findViewById(R.id.floatwin_stopservice);
        buttonBackActivity = (Button)this.findViewById(R.id.floatwin_back);

        ButtonClickListener listener = new ButtonClickListener();
        buttonStartService.setOnClickListener(listener);
        buttonStopService.setOnClickListener(listener);
        buttonBindService.setOnClickListener(listener);
        buttonUnbindService.setOnClickListener(listener);
        buttonBackActivity.setOnClickListener(listener);

        textServiceShow = (EditText)this.findViewById(R.id.floatwin_editext);
    }

    private class ButtonClickListener implements View.OnClickListener{
        @Override
        public void onClick(View v) {
            //> R.id.xxx 资源文件不能用于 switch语句的 case R.id.xxx:

            String strDes = "xxx";
            Intent intent = new Intent(InteractWithService_Activity.this, FloatWin_Service.class);

            if (v.getId() == R.id.floatwin_bindservice) {
                strDes = "bind service";

                serviceCon = new ServiceConnection() {
                    @Override
                    public void onServiceConnected(ComponentName name, IBinder service) {
                        try {
                            //> 如果 service 的声明中有 android:process=":FloatWin_Service" 描述，则以下
                            //> 类型转换 (FloatWin_Service.ServiceBinder)service 会抛出异常，
                            //> 因为，跨进程的通信需要用Binder接口(即AIDL)
                            FloatWin_Service floatwinService = ((FloatWin_Service.ServiceBinder)service).getService();
                            floatwinService.setCallback(new FloatWinCallback() {
                                @Override
                                public void callback(String str) {
                                    //> 这里是服务调用过来的，不再ui线程，所以不能调用：textServiceShow.setText("Bind: " + str);
                                    //> 也不能在 HanderThread 的消息处理中调用，会提示：哪个activity创建的组件，就在哪个消息循环中调用
                                    postMsg("Bind: " + str);
                                }
                            });
                        } catch (Exception e) {
                            e.printStackTrace();
                        }
                    }

                    @Override
                    public void onServiceDisconnected(ComponentName name) {
                    }
                };

                bindService(new Intent(FloatWinUtils.strFloatwinServiceName), serviceCon, Context.BIND_AUTO_CREATE);
            }
            else if (v.getId() == R.id.floatwin_unbindservice) {
                strDes = "unbind service";
                unbindService(serviceCon);
            }
            else if (v.getId() == R.id.floatwin_startservice) {
                strDes = "start service";
                registBroadcastReceiverDynamic();
                startService(intent);
            }
            else if (v.getId() == R.id.floatwin_stopservice) {
                strDes = "stop service";
                stopService(intent);
                unregistBroadcastReceiverDynamic();
            }
            else if (v.getId() == R.id.floatwin_back) {
                finish();
            }

            Toast.makeText(getApplicationContext(), strDes, Toast.LENGTH_SHORT).show();
        }
    }

/**************************************************************************************************/
//> 广播方式跟服务交互

    ServiceBroadcastReceiver receiver;

    private class ServiceBroadcastReceiver extends BroadcastReceiver {
        @Override
        public void onReceive(Context context, Intent intent) {
            String strShow = intent.getStringExtra(FloatWinUtils.strBroadcastReceiverKey);
            textServiceShow.setText("Start: " + strShow);
        }
    }

    private void registBroadcastReceiverDynamic() {
        if (receiver != null) return;
        receiver = new ServiceBroadcastReceiver();

        //> 动态注册广播接收
        IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction(FloatWinUtils.strBroadcastReceiverAction);
        registerReceiver(receiver, intentFilter);
    }

    private void unregistBroadcastReceiverDynamic() {
        if (receiver == null) return;
        unregisterReceiver(receiver);
        receiver = null;
    }


/**************************************************************************************************/
//> 使用 HandlerThread 来处理从服务线程发送的 UI 请求

    Handler handler;
    private static final int msg_from_floatwin_service = 0xffff;

    public class HandlerCallback implements Handler.Callback {
        @Override
        public boolean handleMessage(Message msg) {
            switch (msg.what) {
                case msg_from_floatwin_service:
                    try {
                        String str = msg.getData().get("value").toString();
                        textServiceShow.setText(str);
                    } catch (Exception e) {
                        e.printStackTrace();
                    }

                    break;
                default:
                    return false;
            }
            return true;
        }
    }

    private void postMsg(String str) {
        if (handler == null) {
            Looper l = InteractWithService_Activity.this.getMainLooper();
            handler = new Handler(l, new HandlerCallback());
        }

        Message msg = new Message();
        msg.what = msg_from_floatwin_service;
        Bundle bundle = new Bundle();
        bundle.putString("value", str);
        msg.setData(bundle);

        handler.sendMessage(msg);
    }

    private void stopHandler() {
        handler = null;
    }
}
