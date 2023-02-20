package com.ma.sailing.common

import android.os.Handler
import android.os.Looper
import java.util.concurrent.ScheduledThreadPoolExecutor
import java.util.concurrent.TimeUnit

object ExecutorUtil {
    private var poolExecutor: ScheduledThreadPoolExecutor
    private var uiThread: Thread
    private var uiHandler: Handler

    init {
        val cpuNum = Runtime.getRuntime().availableProcessors()
        val corePoolSize = cpuNum + 1
        poolExecutor = ScheduledThreadPoolExecutor(corePoolSize)

        val looper = Looper.getMainLooper()
        uiThread = looper.thread
        uiHandler = Handler(looper)
    }

    @JvmStatic
    fun getExecutor(): ScheduledThreadPoolExecutor {
        return poolExecutor
    }

    /**
     * 执行异步任务
     *
     * @param action 异步任务
     */
    @JvmStatic
    fun execute(action: () -> Unit) {
        poolExecutor.execute(Runnable(action))
    }

    /**
     * 延迟执行异步任务
     *
     * @param action 任务
     * @param delayMLS 延迟时间（单位：毫秒）
     */
    @JvmStatic
    fun delayExecute(delayMLS: Int, action: () -> Unit) {
        poolExecutor.schedule(
            Runnable(action),
            delayMLS.toLong(),
            TimeUnit.MILLISECONDS
        )
    }

    /**
     * 主线程中执行任务
     *
     * @param action 任务
     */
    @JvmStatic
    fun runOnUiThread(action: () -> Unit) {
        if (Thread.currentThread() !== uiThread) {
            uiHandler.post(action)
        } else {
            action()
        }
    }

    /**
     * 延迟指定时间后在主线程中执行任务
     *
     * @param action   任务
     * @param delayMLS 延迟时间（单位：毫秒）
     */
    @JvmStatic
    fun delayRunOnUiThread(delayMLS: Long, action: () -> Unit) {
        try {
            uiHandler.postDelayed(action, delayMLS)
        } catch (e: Exception) {
            e.printStackTrace()
        }
    }


    @JvmStatic
    fun delayRunOnUiThread(delayMLS: Long, runnable: Runnable) {
        try {
            uiHandler.postDelayed(runnable, delayMLS)
        } catch (e: Exception) {
            e.printStackTrace()
        }
    }

    fun removeUiRunnable(runnable: Runnable) {
        uiHandler.removeCallbacks(runnable)
    }
}