package com.ma.sailing.activities

import android.app.Activity
import android.graphics.Color
import android.os.Build
import android.os.Bundle
import android.view.View
import android.view.ViewGroup
import android.view.Window
import android.view.WindowManager
import androidx.appcompat.app.AppCompatActivity
import androidx.core.view.ViewCompat
import com.ma.sailing.R

open class MaActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        translucentStatusBar(this, true)
    }

    private fun translucentStatusBar(activity: Activity, hideStatusBarBackground: Boolean) {
        val window: Window = activity.window
        //添加Flag把状态栏设为可绘制模式
        window.addFlags(WindowManager.LayoutParams.FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS)
        if (hideStatusBarBackground) {
            //如果为全透明模式，取消设置Window半透明的Flag
            window.clearFlags(WindowManager.LayoutParams.FLAG_TRANSLUCENT_STATUS)
            //设置状态栏为透明
            window.statusBarColor = Color.TRANSPARENT
            //设置window的状态栏不可见
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
                window.decorView.systemUiVisibility = View.SYSTEM_UI_FLAG_LIGHT_STATUS_BAR or View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
            } else {
                window.decorView.systemUiVisibility = View.SYSTEM_UI_FLAG_LAYOUT_STABLE or View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
            }
        } else {
            //如果为半透明模式，添加设置Window半透明的Flag
            window.addFlags(WindowManager.LayoutParams.FLAG_TRANSLUCENT_STATUS)
            //设置系统状态栏处于可见状态
            window.decorView.systemUiVisibility = View.SYSTEM_UI_FLAG_VISIBLE
        }
        //view不根据系统窗口来调整自己的布局
        val mContentView: ViewGroup = window.findViewById(Window.ID_ANDROID_CONTENT) as ViewGroup
        mContentView.getChildAt(0)?.run {
            this.fitsSystemWindows = false
            ViewCompat.requestApplyInsets(this)
        }
    }

    fun dismiss() {
        finish()
        // overridePendingTransition(R.anim.fade_in, R.anim.fade_out)
    }
}
