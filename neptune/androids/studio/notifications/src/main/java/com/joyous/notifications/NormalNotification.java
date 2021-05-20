package com.joyous.notifications;

import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.provider.MediaStore;
import android.support.v4.app.NotificationCompat;

/**
 * Created by kingsoft on 2015/10/9.
 */
public class NormalNotification {
    private int notifyID = 11;
    NotificationManager notificationMgr;

    //> 通知
    public void notify(Context c) {
        Notification notification = createNotification(c);

        notificationMgr = (NotificationManager)c.getSystemService(c.NOTIFICATION_SERVICE);
        notificationMgr.notify(notifyID, notification);
    }

    //> 取消掉通知栏上的通知
    public void clear() {
        notificationMgr.cancel(notifyID);
    }

    private Notification createNotification(Context c) {
        NotificationCompat.Builder notificationBuilder = new NotificationCompat.Builder(c);

        notificationBuilder.setContentTitle("title") //> 设置通知栏标题
                .setContentText("content")
                .setTicker("test is coming")
                .setShowWhen(true).setWhen(System.currentTimeMillis())
                .setPriority(Notification.PRIORITY_DEFAULT)
                .setOngoing(false)
                .setSmallIcon(R.drawable.ic_launcher);

        //> notificationBuilder.setNumber(5).setAutoCancel(true);

        //> 功能：提醒标志符，向通知添加声音、闪灯和振动效果等设置达到通知提醒效果，可以组合多个属性
        //> Method1:
        //>     Notification notification = mBuilder.build();
        //>     notification.flags = Notification.FLAG_AUTO_CANCEL;
        //> Method2:
        notificationBuilder.setContentIntent(getDefaultIntent(c, Notification.FLAG_AUTO_CANCEL));

        //> 功能：设置震动来提醒，需要 VIBRATE permission
        //> notificationBuilder.setDefaults(Notification.DEFAULT_VIBRATE);
        //> notificationBuilder.setVibrate(new long[]{0, 300, 500, 700});
        //> 功能：设置铃声来提醒
        notificationBuilder.setDefaults(Notification.DEFAULT_SOUND);
        notificationBuilder.setSound(Uri.withAppendedPath(MediaStore.Audio.Media.INTERNAL_CONTENT_URI, "5"));

        return notificationBuilder.build();
    }

    private PendingIntent getDefaultIntent(Context c, int flags){
        //> flags: 可以使用以下各值
        //>     Notification.FLAG_SHOW_LIGHTS   //在使用三色灯提醒时候必须加该标志符
        //>     Notification.FLAG_NO_CLEAR      //只有全部清除时，Notification才会清除(QQ的通知无法清除，就是用的这个)
        //>     FLAG_ONGOING_EVENT              //在顶部常驻，可以调用notificationMgr.cancel(notifyID)方法去除
        //>     PendingIntent.FLAG_NO_CREATE
        PendingIntent pendingIntent= PendingIntent.getActivity(c, 0, new Intent(), flags);
        return pendingIntent;
    }
}
