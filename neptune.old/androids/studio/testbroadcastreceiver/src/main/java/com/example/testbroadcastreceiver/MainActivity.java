package com.example.testbroadcastreceiver;

import android.content.Intent;
import android.content.IntentFilter;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;

public class MainActivity extends AppCompatActivity {
    private ReceiverThree receiverThree = new ReceiverThree();
    private OrderedBroadcastThree orderedBroadcastThree = new OrderedBroadcastThree();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Button btnNormal = (Button)findViewById(R.id.btn_normal_broadcastreceiver);
        Button btnOrdered = (Button)findViewById(R.id.btn_ordered_broadcastreceiver);

        IntentFilter intentFilter = new IntentFilter(BroadcastConst.broadcast_normal);
        registerReceiver(receiverThree, intentFilter);

        IntentFilter intentFilterOrdered = new IntentFilter(BroadcastConst.broadcast_ordered);
        intentFilterOrdered.setPriority(997);
        registerReceiver(orderedBroadcastThree, intentFilterOrdered);

        btnNormal.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent(BroadcastConst.broadcast_normal);
                intent.putExtra("msg", "hello every receiver");
                sendBroadcast(intent);
            }
        });

        btnOrdered.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent(BroadcastConst.broadcast_ordered);
                intent.putExtra("msg", "hello every ordered receiver");
                sendOrderedBroadcast(intent, BroadcastConst.broadcast_ordered_permission);
            }
        });
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        unregisterReceiver(receiverThree);
        unregisterReceiver(orderedBroadcastThree);
    }
}
