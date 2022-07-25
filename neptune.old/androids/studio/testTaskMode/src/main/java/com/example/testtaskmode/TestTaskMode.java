package com.example.testtaskmode;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;

import com.example.testtaskmode.R;


public class TestTaskMode extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        setTitle("Main Activity");

//        new TestFastJson().TestCases();
//
//        Button btn = (Button)findViewById(R.id.btn_A);
//        btn.setOnClickListener(new View.OnClickListener() {
//            @Override
//            public void onClick(View v) {
//                Intent intent = new Intent(TestTaskMode.this, ActivityB.class);
//                startActivity(intent);
//            }
//        });
//
//        Button btnAnimation = (Button)findViewById(R.id.btn_animation);
//        btnAnimation.setOnClickListener(new View.OnClickListener() {
//            @Override
//            public void onClick(View v) {
//                Intent intent = new Intent(TestTaskMode.this, InteractiveBook.class);
//                startActivity(intent);
//            }
//        });
    }
}
