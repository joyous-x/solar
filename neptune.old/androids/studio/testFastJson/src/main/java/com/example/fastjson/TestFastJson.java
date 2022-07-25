package com.example.fastjson;
import android.util.Log;

import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONArray;
import com.alibaba.fastjson.JSONObject;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;


/**
 * Created by kingsoft on 2016/5/20.
 */
public class TestFastJson {
    private static String TAG = "xxx-TFJ";

    public void TestCases( ) {
        Object2JsonString();
        Object2JsonObject();
        Object2JsonArray();
        String2Object();
        String2JsonObject();
        String2JsonArray();
        String2JsonObject();
        String2JsonObjectByParse();
        String2JsonArrayByParse();
        usageJsonObject();
    }

    private void Object2JsonString() {
        String tag = TAG+" Object2JsonString";
        int[] bwh = {60,80,60};
        Student student = new Student("sari", "female", bwh);
        String str = JSON.toJSONString(student);
        Log.e(tag, student.toString());
        String str2 = JSON.toJSONString(student, true);
        Log.e(tag, "prettyformat : " + str2);
    }

    private void Object2JsonObject() {
        String tag = TAG+" Object2JsonObject";
        int[] bwh = {60,80,60};
        Student student = new Student("sari", "female", bwh);
        JSONObject object = (JSONObject) JSON.toJSON(student);
        Log.e(tag, object.toString());
        Log.e(tag, object.toJSONString());
    }

    private void Object2JsonArray() {
        String tag = TAG+" Object2JsonObject";
        int[] bwh = {60,80,60};
        Student studentA = new Student("sari", "female", bwh);
        Student studentB = new Student("zaka", "male", bwh);
        ArrayList<Student> array = new ArrayList<Student>();
        array.add(studentA);
        array.add(studentB);
        JSONArray jsonArray = (JSONArray) JSON.toJSON(array);
        Log.e(tag, jsonArray.toString());
        Log.e(tag, jsonArray.toJSONString());
    }

    private void String2JsonArray() {
        String tag = TAG+" String2JsonArray";
        JSONArray array = JSON.parseArray(JsonConst.strJsonArray);
        Log.e(tag, array.toString());
        Log.e(tag, array.toJSONString());
    }

    private void String2ObjectList() {
        String tag = TAG+" String2ObjectList";
        List<Student> array = JSON.parseArray(JsonConst.strJsonArray, Student.class);
        Log.e(tag, array.toString());
    }

    private void String2Object() {
        String tag = TAG+" String2Object";
        Student student = JSON.parseObject(JsonConst.str, Student.class);
        Log.e(tag, student.toString());
    }

    private void String2JsonObject() {
        String tag = TAG+" String2JsonObject";
        JSONObject object = JSON.parseObject(JsonConst.str);
        Log.e(tag, object.toString());
    }

    private void String2JsonObjectByParse() {
        String tag = TAG+" Str2JsonObjectByParse";
        JSONObject object = (JSONObject) JSON.parse(JsonConst.str);
        Log.e(tag, object.toString());
        Log.e(tag, object.toJSONString());
    }

    private void String2JsonArrayByParse() {
        String tag = TAG+" Str2JsonArrayByParse";
        JSONArray object = (JSONArray) JSON.parse(JsonConst.strJsonArray);
        Log.e(tag, object.toString());
        Log.e(tag, object.toJSONString());
    }

    private void usageJsonObject() {
        String tag = TAG+" usageJsonObject";
        int[] bwh = {60,80,60};
        Student student = new Student("sari", "female", bwh);
        JSONObject object = (JSONObject)JSON.toJSON(student);

        String name = object.getString("name");
        Log.e(tag, "name : "+name);

        object.put("height", "18");
        Log.e(tag, object.toString());

        Map<String, String> map = new HashMap<String, String>();
        map.put("class", "A");
        map.put("country", "china");
        object.putAll(map);
        Log.e(tag, object.toJSONString());

        object.remove("height");
        Log.e(TAG, object.toJSONString());
    }
}
