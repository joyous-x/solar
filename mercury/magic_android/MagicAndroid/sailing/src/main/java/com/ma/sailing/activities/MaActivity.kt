package com.ma.sailing.activities

import android.app.Activity
import android.app.ActivityManager
import android.content.Context
import android.content.Intent
import android.graphics.Color
import android.os.Build
import android.os.Bundle
import android.os.Process
import android.view.*
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import androidx.core.view.ViewCompat
import com.ma.sailing.R
import kotlin.system.exitProcess

open class MaActivity : AppCompatActivity() {
    companion object {
        fun exitApp(context: Context, use: String = "m1") {
            when(use) {
                "m1" -> {
                    // 方法1: 在首个activity中使用，关闭app程序(不会关闭服务)；否则是退出当前 activity
                    exitProcess(0)
                }
                "m2" -> {
                    // 方法2: 只杀当前进程，所以会遗留某些后进程, 例如: Service,Notifications 等
                    Process.killProcess(Process.myPid())
                }
                else -> {
                    // 方法3: 此方法会结束本应用程序的一切活动，但，要添加权限声明：android.permission.KILL_BACKGROUND_PROCESSES
                    val manager = context.getSystemService(Context.ACTIVITY_SERVICE) as ActivityManager
                    manager.killBackgroundProcesses(context.packageName)
                }
            }
        }
    }

    protected var enableBackPressedTwice2Exit = false
    protected var enableBackPressed = true

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

    open fun dismiss(result: Intent? = null) {
        result?.let { setResult(Activity.RESULT_OK, it) }
        finish()
        // overridePendingTransition(R.anim.fade_in, R.anim.fade_out)
    }

    private var mFirstPressBackTs = 0L

    override fun onKeyDown(keyCode: Int, event: KeyEvent?): Boolean {
        if (keyCode == KeyEvent.KEYCODE_BACK) {
            if (!enableBackPressed) {
                return true
            }

            if (enableBackPressedTwice2Exit && event?.action == KeyEvent.ACTION_DOWN) {
                if (System.currentTimeMillis() - mFirstPressBackTs > 2000) {
                    mFirstPressBackTs = System.currentTimeMillis()
                    Toast.makeText(this, R.string.hint_press_again_to_exit, Toast.LENGTH_SHORT).show();
                } else {
                    exitApp(this)
                }
                return true
            }
        }

        return super.onKeyDown(keyCode, event)
    }
}
