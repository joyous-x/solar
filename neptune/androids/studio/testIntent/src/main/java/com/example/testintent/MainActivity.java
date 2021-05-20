package com.example.testintent;

import android.Manifest;
import android.app.SearchManager;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.support.v4.app.ActivityCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;

public class MainActivity extends AppCompatActivity {
    private static final String TAG = "TestIntent";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Button btnWB = (Button)findViewById(R.id.btn_testIntent_wb);
        Button btnWS = (Button)findViewById(R.id.btn_testIntent_ws);
        Button btnCT = (Button)findViewById(R.id.btn_testIntent_ct);

        btnWB.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                invokeWebBrowser(v);
            }
        });
        btnWS.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                invokeWebSearch(v);
            }
        });
        btnCT.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                callTel(v);
            }
        });
    }

    private void invokeWebBrowser(View v) {
        Uri uri = Uri.parse("http://www.baidu.com");
        printUri(uri);
        Intent intent = new Intent(Intent.ACTION_VIEW);
        intent.setData(uri);
        startActivity(intent);
    }

    private void invokeWebSearch(View v) {
        Intent intent = new Intent(Intent.ACTION_WEB_SEARCH);
        intent.putExtra(SearchManager.QUERY, "android");
        startActivity(intent);
    }

    private void callTel(View v) {
        Uri uri = Uri.parse("tel:12345567");
        printUri(uri);
        Intent intent = new Intent(Intent.ACTION_CALL);
        intent.setData(uri);
        if (ActivityCompat.checkSelfPermission(this, Manifest.permission.CALL_PHONE) != PackageManager.PERMISSION_GRANTED) {
            // TODO: Consider calling
            //    ActivityCompat#requestPermissions
            // here to request the missing permissions, and then overriding
            //   public void onRequestPermissionsResult(int requestCode, String[] permissions,
            //                                          int[] grantResults)
            // to handle the case where the user grants the permission. See the documentation
            // for ActivityCompat#requestPermissions for more details.
            return;
        }
        startActivity(intent);
    }

    private void printUri(Uri uri) {
        if (null == uri) {
            return ;
        }

        String autority = uri.getAuthority();
        String host = uri.getHost();
        String path = uri.getPath();
        String port = String.valueOf(uri.getPort());
        String scheme = uri.getScheme();
        Log.e(TAG,  "scheme: " + scheme +
                    " autority: " + autority +
                    " host: " + host +
                    " path: " + path +
                    " port: " + port);
    }
}
