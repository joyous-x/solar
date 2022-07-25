package com.example.testtaskmode;

import android.content.Context;
import android.content.Intent;

/**
 * Created by kingsoft on 2015/10/10.
 */
public class ModuleEntry {
    public ModuleEntry(Context ctx) {
        ctx.startActivity(new Intent(ctx, TestTaskMode.class));
    }
}
