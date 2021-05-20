package com.example.testcontentprovider;

import android.app.Activity;
import android.content.ContentResolver;
import android.content.ContentUris;
import android.content.ContentValues;
import android.database.Cursor;
import android.database.CursorWrapper;
import android.net.Uri;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.SimpleCursorAdapter;
import android.widget.Toast;

import java.util.ArrayList;

public class MainActivity extends Activity {

    private ContentResolver resolver;
    private ListView listView;
    Button btnInsert;
    Button btnInit;
    Button btnDelete;
    Button btnUpdate;
    Button btnQuery;
    EditText editText;

    private static final String AUTHORITY = "com.joyous.cp";
    private static final Uri PERSON_ALL_URI = Uri.parse("content://" + AUTHORITY + "/persons");

    private Handler handler = new Handler() {
        public void handleMessage(Message msg) {
            requery();
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        resolver = getContentResolver();
        listView = (ListView) findViewById(R.id.listView);

        //为PERSON_ALL_URI注册变化通知
        getContentResolver().registerContentObserver(PERSON_ALL_URI, true, new PersonObserver(handler));

        initView();
    }

    private void requery() {
        //实际操作中可以查询集合信息后Adapter.notifyDataSetChanged();
        query(null);
    }

    public void init(View view) {
        ArrayList<Person> persons = new ArrayList<Person>();

        Person person1 = new Person("Ella", 22, "lively girl");
        Person person2 = new Person("Jenny", 22, "beautiful girl");
        Person person3 = new Person("Jessica", 23, "sexy girl");
        Person person4 = new Person("Kelly", 23, "hot baby");
        Person person5 = new Person("Jane", 25, "pretty woman");

        persons.add(person1);
        persons.add(person2);
        persons.add(person3);
        persons.add(person4);
        persons.add(person5);

        for (Person person : persons) {
            ContentValues values = new ContentValues();
            values.put("name", person.name);
            values.put("age", person.age);
            values.put("info", person.info);
            resolver.insert(PERSON_ALL_URI, values);
        }
    }

    public void delete(View view) {
        String text = editText.getText().toString();
        int ret = 0;
        int _id = -1;
        try {
            _id = Integer.parseInt(text, 10);
        } catch (NumberFormatException e) {
            e.printStackTrace();
        }

        if (0 > _id) {
            Toast.makeText(MainActivity.this, "please input number", Toast.LENGTH_SHORT).show();
        } else if (0 < _id) {
            Uri delUri = ContentUris.withAppendedId(PERSON_ALL_URI, _id);
            ret = resolver.delete(delUri, null, null);
            Toast.makeText(MainActivity.this, "delete _id = " + _id + ", number:" + ret, Toast.LENGTH_SHORT).show();
        } else {
            //删除所有记录
            ret = resolver.delete(PERSON_ALL_URI, null, null);
            Toast.makeText(MainActivity.this, "delete all, number:"+ret, Toast.LENGTH_SHORT).show();
        }
    }

    public void insert(View view) {
        Person person = new Person("Alina", 26, "attractive lady");
        ContentValues values = new ContentValues();
        values.put("name", person.name);
        values.put("age", person.age);
        values.put("info", person.info);
        resolver.insert(PERSON_ALL_URI, values);
    }

    private static int age = 0;
    public void update(View view) {
        Person person = new Person();
        person.name = "Jane";
        person.age = age;

        //将指定name的记录age字段更新为30
        ContentValues values = new ContentValues();
        values.put("age", person.age);
        resolver.update(PERSON_ALL_URI, values, "name = ?", new String[]{person.name});

        //将_id为1的age更新为30
        // Uri updateUri = ContentUris.withAppendedId(PERSON_ALL_URI, 1);
        // resolver.update(updateUri, values, null, null);
    }

    public void query(View view) {
        Cursor cursor = resolver.query(PERSON_ALL_URI, null, null, null, null);
        CursorWrapper cursorWrapper = new CursorWrapper(cursor) {
            public String getString(int columnIndex) {
                if (getColumnName(columnIndex).equals("info")) {
                    int age = getInt(getColumnIndex("age"));
                    int index = getInt(getColumnIndex("_id"));
                    return index + " ," + age + " years old, " + super.getString(columnIndex);
                }
                return super.getString(columnIndex);
            }
        };

        //Cursor须含有"_id"字段
        SimpleCursorAdapter adapter = new SimpleCursorAdapter(this, android.R.layout.simple_list_item_2,
                cursorWrapper, new String[]{"name", "info"}, new int[]{android.R.id.text1, android.R.id.text2});
        listView.setAdapter(adapter);

        startManagingCursor(cursorWrapper);
    }

    private void initView() {
        editText = (EditText)findViewById(R.id.edittext_testcp_delete);
        btnInit = (Button)findViewById(R.id.btn_testcp_init);
        btnInsert = (Button)findViewById(R.id.btn_testcp_insert);
        btnDelete = (Button)findViewById(R.id.btn_testcp_delete);
        btnUpdate = (Button)findViewById(R.id.btn_testcp_update);
        btnQuery = (Button)findViewById(R.id.btn_testcp_query);
        btnInit.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                init(v);
            }
        });
        btnInsert.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                insert(v);
            }
        });
        btnQuery.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                query(v);
            }
        });
        btnDelete.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                delete(v);
            }
        });
        btnUpdate.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                update(v);
            }
        });
    }

}
