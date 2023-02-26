package com.ma.sailing.common

import android.content.Context
import android.view.View
import android.view.inputmethod.InputMethodManager
import java.lang.NullPointerException

/**
 * Input Method Manager Util
 */
class ImmUtil {
    companion object {
        val instance: ImmUtil = ImmUtil()
    }

    fun showKeyBoard(context: Context, view: View) {
        view.requestFocus()
        (context.getSystemService(Context.INPUT_METHOD_SERVICE) as InputMethodManager).apply {
            showSoftInput(view, InputMethodManager.SHOW_FORCED)
        }
    }

    fun hideKeyBoard(context: Context, view: View) {
        try {
            val imm = context.getSystemService(Context.INPUT_METHOD_SERVICE) as InputMethodManager
            imm.hideSoftInputFromWindow(view.windowToken, 0)
        } catch (e: NullPointerException) {
        }
    }
}