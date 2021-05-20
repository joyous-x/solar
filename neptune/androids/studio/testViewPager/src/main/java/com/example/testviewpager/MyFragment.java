package com.example.testviewpager;

import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

public class MyFragment extends Fragment {
    @Override
    public View onCreateView(LayoutInflater inflater,  ViewGroup container, Bundle savedInstanceState) {
        View view=inflater.inflate(R.layout.my_fragment,container,false);
        ((TextView)view.findViewById(R.id.text)).setText(getArguments().getString("text"));
        Log.e("xxxxxx", "MyFragment::onCreateView " + this);
        return view;
    }

    @Override
    public void onActivityCreated(Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);
        Log.e("xxxxxx", "MyFragment::onActivityCreated " + this);
    }
}
