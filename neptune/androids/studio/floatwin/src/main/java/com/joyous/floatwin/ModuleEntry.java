package com.joyous.floatwin;

import android.content.Context;
import android.content.Intent;

public class ModuleEntry {
    public ModuleEntry(Context ctx) {
        ctx.startActivity(new Intent(ctx, MainActivity.class));
    }
}

