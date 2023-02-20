package com.ma.sailing.common

import android.content.Context
import android.widget.Toast

object ToastUtil {
    fun show(context: Context, resId: Int) {
        ExecutorUtil.runOnUiThread {
            Toast.makeText(context, resId, Toast.LENGTH_SHORT).show()
        }
    }

    fun show(context: Context, text: String) {
        ExecutorUtil.runOnUiThread {
            Toast.makeText(context, text, Toast.LENGTH_SHORT).show()
        }
    }
}