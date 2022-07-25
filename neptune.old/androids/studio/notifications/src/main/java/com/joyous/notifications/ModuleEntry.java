package com.joyous.notifications;

import android.content.Context;
import android.content.Intent;

public class ModuleEntry {
    public ModuleEntry(Context ctx) {
        ctx.startActivity(new Intent(ctx, NotificationActivity.class));
    }
}

