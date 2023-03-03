package com.ma.sailing.dialogs

import android.content.Context
import android.content.DialogInterface
import androidx.appcompat.app.AppCompatDialog

open class CommonBaseDialog(context: Context, themeResId: Int) :
    AppCompatDialog(context, themeResId),
    DialogInterface.OnDismissListener,
    DialogInterface.OnShowListener {

    private var cancelable = false

    override fun setCancelable(cancelable: Boolean) {
        super.setCancelable(cancelable)
        if (this.cancelable != cancelable) {
            this.cancelable = cancelable
            onSetCancelable(cancelable)
        }
    }

    override fun show() {
        try {
            super.show()
        } catch (e: Exception) {
            e.printStackTrace()
        }
    }

    override fun dismiss() {
        try {
            super.dismiss()
        } catch (e: Exception) {
            e.printStackTrace()
        }
    }

    protected open fun onSetCancelable(cancelable: Boolean) {
        // TODO: override to process something when cancelable is set
    }

    override fun onDismiss(dialog: DialogInterface?) {
        // TODO:
    }

    override fun onShow(dialog: DialogInterface?) {
        // TODO:
    }
}