package com.example.testcontentprovider;

import android.content.ContentProvider;
import android.content.ContentUris;
import android.content.ContentValues;
import android.content.UriMatcher;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.net.Uri;

public class TestContentProvider extends ContentProvider {
    private static final int    PERSON_ALL = 1;
    private static final int    PERSON_ONE = 2;

    public static final String CONTENT_TYPE = "android.cursor.dir/com.joyous.cp";
    public static final String CONTENT_ITEM_TYPE = "android.cursor.item/com.joyous.cp";

    private static final String AUTHORITY = "com.joyous.cp";
    private static final Uri NOTIFY_URI = Uri.parse("content://" + AUTHORITY + "/persons");

    private static final UriMatcher matcher;
    static {
        matcher = new UriMatcher(UriMatcher.NO_MATCH);
        matcher.addURI(AUTHORITY, "persons", PERSON_ALL);
        matcher.addURI(AUTHORITY, "persons/#", PERSON_ONE);
    }

    private DBHelper dbHelper;
    private SQLiteDatabase db;

    public TestContentProvider() {
    }

    @Override
    public int delete(Uri uri, String selection, String[] selectionArgs) {
        db = dbHelper.getWritableDatabase();
        int match = matcher.match(uri);
        switch (match) {
            case PERSON_ALL:
                break;
            case PERSON_ONE:
                long _id = ContentUris.parseId(uri);
                selection = "_id = ?";
                selectionArgs = new String[]{String.valueOf(_id)};
                break;
            default:
                throw new IllegalArgumentException("Unknown URI: " + uri);
        }
        int count = db.delete("person", selection, selectionArgs);
        if (count > 0) {
            notifyDataChanged();
        }
        return count;
    }

    @Override
    public String getType(Uri uri) {
        //> 用于 intent 的三个内容中(action、category、type) 的 type 的对比。对应的 intent-filter 中示例配置如下：
        //        <intent-filter>
        //            <action android:name="android.intent.action.MAIN" />
        //            <category android:name="android.intent.category.DEFAULT" />
        //            <data android:mimeType="android.cursor.dir/com.joyous.cp"/>
        //        </intent-filter>
        //
        //> 例如：在此例的 AndroidManifest.xml 中为 MainActivity 添加此MIME类型过滤器，告诉系统MainActivity可以处理的信息类型
        int match = matcher.match(uri);
        switch (match) {
            case PERSON_ALL:
                return CONTENT_TYPE;
            case PERSON_ONE:
                return CONTENT_ITEM_TYPE;
            default:
                throw new IllegalArgumentException("Unknown URI: " + uri);
        }
    }

    @Override
    public Uri insert(Uri uri, ContentValues values) {
        int match = matcher.match(uri);
        if (match != PERSON_ALL) {
            throw  new IllegalArgumentException("Wrong URI: "+uri);
        }

        db = dbHelper.getWritableDatabase();
        if (values == null) {
            values = new ContentValues();
            values.put("name", "no name");
            values.put("age", "1");
            values.put("info", "no info.");
        }
        long rowId = db.insert("person", null, values);
        if (rowId > 0) {
            notifyDataChanged();
            return ContentUris.withAppendedId(uri, rowId);
        }
        return null;
    }

    @Override
    public boolean onCreate() {
        dbHelper = new DBHelper(getContext());
        return true;
    }

    @Override
    public Cursor query(Uri uri, String[] projection, String selection,
                        String[] selectionArgs, String sortOrder) {
        db = dbHelper.getReadableDatabase();
        int match = matcher.match(uri);
        switch (match) {
            case PERSON_ALL:
                break;
            case PERSON_ONE:
                long _id = ContentUris.parseId(uri);
                selection = "_id = ?";
                selectionArgs = new String[] {String.valueOf(_id)};
                break;
            default:
                throw new IllegalArgumentException("Unknown URI: "+uri);
        }
        return db.query("person", projection, selection, selectionArgs, null, null, sortOrder);

    }

    @Override
    public int update(Uri uri, ContentValues values, String selection,
                      String[] selectionArgs) {
        db = dbHelper.getWritableDatabase();
        int match = matcher.match(uri);
        switch (match) {
            case PERSON_ALL:
                break;
            case PERSON_ONE:
                long _id = ContentUris.parseId(uri);
                selection = "_id = ?";
                selectionArgs = new String[]{String.valueOf(_id)};
                break;
            default:
                throw new IllegalArgumentException("Unknown URI: " + uri);
        }
        int count = db.update("person", values, selection, selectionArgs);
        if (count > 0) {
            notifyDataChanged();
        }
        return count;
    }

    private void notifyDataChanged() {
        getContext().getContentResolver().notifyChange(NOTIFY_URI, null);
    }
}
