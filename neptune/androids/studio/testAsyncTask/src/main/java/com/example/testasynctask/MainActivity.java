package com.example.testasynctask;

import android.net.http.HttpResponseCache;
import android.os.AsyncTask;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.View;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.Button;
import android.widget.ProgressBar;
import android.widget.TextView;


import java.io.BufferedReader;
import java.io.ByteArrayOutputStream;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.URL;

public class MainActivity extends AppCompatActivity {
    private static final String TAG = "TestAsyncTask";
    private Button execute;
    private Button cancel;
    private ProgressBar progressBar;
    private TextView textView;
    private MyAsyncTask asyncTask;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        final Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        execute = (Button)findViewById(R.id.execute);
        cancel = (Button)findViewById(R.id.cancel);
        progressBar = (ProgressBar)findViewById(R.id.progress_bar);
        textView = (TextView)findViewById(R.id.text_view);

        execute.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                asyncTask = new MyAsyncTask();
                asyncTask.execute("http://www.google.com", "http://www.baidu.com");
                execute.setEnabled(false);
                cancel.setEnabled(true);
            }
        });
        cancel.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                boolean bRet = asyncTask.cancel(false);
                //> 用下边这种方式时，系统会直接使用中断指令中断工作线程，工作线程抛出异常，进而onCancelled不会被调用
                // asyncTask.cancel(true);
                if (bRet == false) {
                    execute.setEnabled(true);
                    cancel.setEnabled(false);
                    textView.setText("cancelled");
                    progressBar.setProgress(0);
                }
            }
        });
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    private class MyAsyncTask extends AsyncTask<String, Integer, String> {
        @Override
        protected void onPreExecute() {
            Log.e(TAG, "onPreExecute called");
            textView.setText("loading...");
        }

        @Override
        protected String doInBackground(String... params) {
            Log.e(TAG, "doInBackground called");
            try {

                URL url = new java.net.URL(params[0]);
                HttpURLConnection urlConn = (HttpURLConnection)url.openConnection();
                urlConn.setRequestMethod("GET");
                // 不要配置下面的信息，使用默认值即可
                // urlConn.setDoOutput(true);
                // urlConn.setDoInput(true);
                urlConn.connect();

                int dataLength = urlConn.getContentLength();
                int statusCode = urlConn.getResponseCode();
                if (200 == statusCode) {
                    InputStream inputStream = urlConn.getInputStream();
                    byte data[] = new byte[4096];
                    int count = 0;
                    int total = 0;
                    if (dataLength < 0) {
                        dataLength = 1 * 1024 * 1024; //> 当服务器返回的头部不包含长度信息时，赋给默认值
                    }
                    ByteArrayOutputStream output = new ByteArrayOutputStream();
                    while ((count = inputStream.read(data)) != -1) {
                        if (isCancelled()) break;
                        output.write(data, 0, count);
                        total += count;

                        int progress = (int)((total * 100 / (float)dataLength));
                        publishProgress(progress > 99 ? 99 : progress);
                    }
                    publishProgress(100);
                    return output.toString();
                }
//
//                HttpClient client = new DefaultHttpClient();
//                HttpGet get = new HttpGet(params[0]);
//                HttpResponse response = client.execute(get);
//                if (response.getStatusLine().getStatusCode == HttpStatus.SC_OK) {
//                    HttpEntity entity = response.getEntity();
//                    InputStream is = entity.getContent();
//                    long total = entity.getContentLength();
//                    ByteArrayOutputStream output = new ByteArrayOutputStream();
//                    byte[] buf = new byte[1024];
//                    int count = 0;
//                    int length = -1;
//                    while ((length = is.read(buf)) != -1) {
//                        output.write(buf, 0, length);
//                        count += length;
//                        publishProgress((int)((count/(float)total) * 100));
//                    }
//                }

            } catch (Exception e) {
                Log.e(TAG, e.getMessage());
            }
            return null;
        }

        @Override
        protected void onProgressUpdate(Integer... progress) {
            Log.e(TAG, "onProgressUpdate called");
            progressBar.setProgress(progress[0]);
            textView.setText("loading..."+progress[0]+"%");
        }

        @Override
        protected void onPostExecute(String result) {
            Log.e(TAG, "onPostExecute called");
            textView.setText(result);
            execute.setEnabled(true);
            execute.setEnabled(false);
        }

        @Override
        protected void onCancelled() {
            Log.e(TAG, "onCancelled called");
            textView.setText("cancelled");
            progressBar.setProgress(0);
            execute.setEnabled(true);
            cancel.setEnabled(false);
        }
    }
}
