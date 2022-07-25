package com.example.testcontentprovider;

import android.content.Context;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;

/**
 * Created by jiao on 2016/6/12.
 */
public class DBHelper extends SQLiteOpenHelper {
    private static String DBName = "test_db";
    private static int DBVersion = 1;

    public DBHelper(Context ctx) {
        super(ctx, DBName, null, DBVersion);
    }

    @Override
    public void onCreate(SQLiteDatabase db) {
        String sql = "CREATE TABLE IF NOT EXISTS person (_id INTEGER PRIMARY KEY AUTOINCREMENT, name VARCHAR, age INTEGER, info TEXT)";
        db.execSQL(sql);
    }

    @Override
    public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {
        db.execSQL("DROP TABLE IF EXISTS person");
        onCreate(db);
    }
}
