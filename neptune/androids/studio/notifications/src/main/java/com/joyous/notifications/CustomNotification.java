package com.joyous.notifications;

import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.support.v4.app.NavUtils;
import android.support.v4.app.NotificationCompat;
import android.view.View;
import android.widget.ImageButton;
import android.widget.RemoteViews;

/**
 * Created by kingsoft on 2015/10/9.
 */
public class CustomNotification {
    private int notifyID = 12;
    private int requestCode = 123;
    private boolean bPause = false;
    private int progress = 10;

    public final static String ACTION_BUTTON_CLICK = "com.joyous.notifications.intent.action.ButtonClick";
    public final static String INTENT_BUTTON_KEY = "ButtonID";
    public final static int INTENT_BUTTON_ID_PREV = 1;
    public final static int INTENT_BUTTON_ID_START = 2;
    public final static int INTENT_BUTTON_ID_NEXT = 3;

    Context context;
    NotificationManager notificationMgr;
    NotificationBroadcastReceiver notificationBroadcastReceiver;

    public void init(Context c) {
        context = c;
        notificationMgr = (NotificationManager)c.getSystemService(c.NOTIFICATION_SERVICE);
        registerNotificationBroadcastReceiver(c);
    }

    public void unInit() {
        unregisterNotificationBroadcastReceiver(context);
        context = null;
        notificationMgr = null;
    }

    //> 通知
    public void send() {
        Notification notification = createNotification(context);
        notificationMgr.notify(notifyID, notification);
    }

    //> 取消掉通知栏上的通知
    public void clear() {
        notificationMgr.cancel(notifyID);
    }

/**************************************************************************************************/

    private Notification createNotification(Context c) {
        RemoteViews remoteView = new RemoteViews(c.getPackageName(), R.layout.custom_notification);
        remoteView.setImageViewResource(R.id.custom_notification_icon, R.drawable.ic_launcher);
        remoteView.setTextViewText(R.id.custom_notification_title, "Title");
        remoteView.setTextViewText(R.id.custom_notification_content, "this is content xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

        remoteView.setProgressBar(R.id.custom_notification_progressbar, 100, progress, false);
        remoteView.setViewVisibility(R.id.custom_notification_progressbar, View.VISIBLE);
        remoteView.setTextViewText(R.id.custom_notification_process_status, "10%");

        //> 如果版本号低于3.0，那么不显示按钮(带按钮的布局相应点击事件在3.0以下版本没有用)
        if(android.os.Build.VERSION.SDK_INT <= 9){
            remoteView.setViewVisibility(R.id.custom_notification_button_layout, View.GONE);
        }else{
            remoteView.setViewVisibility(R.id.custom_notification_button_layout, View.VISIBLE);
            if(bPause){
                remoteView.setImageViewResource(R.id.custom_notification_btn_start, R.drawable.btn_pause);
            }else{
                remoteView.setImageViewResource(R.id.custom_notification_btn_start, R.drawable.btn_play);
            }
        }

        //> 设置自绘通知中的按钮监听
        setClickListener(c, remoteView);

        NotificationCompat.Builder notificationBuilder = new NotificationCompat.Builder(c);
        notificationBuilder.setContent(remoteView)
                .setContentIntent(getDefaultIntent(c, Notification.FLAG_ONGOING_EVENT))
                .setTicker("test is coming")
                .setDefaults(Notification.DEFAULT_SOUND)
                .setPriority(Notification.PRIORITY_DEFAULT)
                .setOngoing(true)
                .setPriority(Notification.PRIORITY_DEFAULT)
                .setWhen(System.currentTimeMillis());

        //> 不设置此处，则不会通知
        //> 作用：通知栏不展开时，显示在屏幕顶部的如wifi信号之类的小图标
        notificationBuilder.setSmallIcon(R.drawable.lion);

        return notificationBuilder.build();
    }

    public PendingIntent getDefaultIntent(Context c, int flags){
        PendingIntent pendingIntent = PendingIntent.getActivity(c, 0, new Intent(), flags);
        return pendingIntent;
    }

/**************************************************************************************************/

    //> 设置通知栏中button的onClick事件监听
    private void setClickListener(Context c, RemoteViews remoteView) {
        Intent buttonIntent = new Intent(ACTION_BUTTON_CLICK);
        buttonIntent.putExtra(INTENT_BUTTON_KEY, INTENT_BUTTON_ID_START);
        PendingIntent intentStart = PendingIntent.getBroadcast(c, requestCode, buttonIntent, PendingIntent.FLAG_UPDATE_CURRENT);
        remoteView.setOnClickPendingIntent(R.id.custom_notification_btn_start, intentStart);
    }

    private void registerNotificationBroadcastReceiver(Context c) {
        if (null != notificationBroadcastReceiver) unregisterNotificationBroadcastReceiver(c);
        notificationBroadcastReceiver = new NotificationBroadcastReceiver();
        IntentFilter intentFiler = new IntentFilter(ACTION_BUTTON_CLICK);
        c.registerReceiver(notificationBroadcastReceiver, intentFiler);
    }

    private void unregisterNotificationBroadcastReceiver(Context c) {
        c.unregisterReceiver(notificationBroadcastReceiver);
        notificationBroadcastReceiver = null;
    }

    public class NotificationBroadcastReceiver extends BroadcastReceiver {
        public void onReceive(Context c, Intent intent) {
            String action = intent.getAction();
            if (!action.equals(ACTION_BUTTON_CLICK)) return ;

            int buttonID = intent.getIntExtra(INTENT_BUTTON_KEY, 0);
            switch (buttonID) {
                case INTENT_BUTTON_ID_START:
                    bPause = !bPause;
                    send();
                    break;
                default:
            }

            return ;
        }
    }
}
