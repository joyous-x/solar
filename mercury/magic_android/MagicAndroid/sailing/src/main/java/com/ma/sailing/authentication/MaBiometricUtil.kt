package com.ma.sailing.authentication

import android.content.Context
import android.util.Log
import androidx.biometric.BiometricManager
import androidx.biometric.BiometricPrompt
import androidx.core.content.ContextCompat
import androidx.fragment.app.Fragment
import androidx.fragment.app.FragmentActivity
import java.util.concurrent.Executor

typealias FunBiometricAuthCallback = (success: Boolean, rstCode: Int, result: BiometricPrompt.AuthenticationResult?) -> Unit

class MaBiometricUtil {
    companion object {
        val TAG: String = MaBiometricUtil::class.java.simpleName

        fun available(context: Context) : Boolean {
            return state(context) == BiometricManager.BIOMETRIC_SUCCESS
        }

        fun state(context: Context, authenticators: Int = BiometricManager.Authenticators.DEVICE_CREDENTIAL) : Int {
            var biometricMgr : BiometricManager = BiometricManager.from(context)
            return biometricMgr.canAuthenticate(authenticators).apply {
                MaBiometricUtil().printDebug(this)
            }
        }
    }

    fun printDebug(biometricState: Int) {
        when (biometricState) {
            BiometricManager.BIOMETRIC_SUCCESS -> {
                Log.d(TAG, "应用可以进行生物识别技术进行身份验证")
            }
            BiometricManager.BIOMETRIC_ERROR_NO_HARDWARE -> {
                Log.d(TAG, "该设备上没有搭载可用的生物特征功能")
            }
            BiometricManager.BIOMETRIC_ERROR_HW_UNAVAILABLE -> {
                Log.d(TAG, "生物识别功能当前不可用")
            }
            BiometricManager.BIOMETRIC_ERROR_NONE_ENROLLED -> {
                Log.d(TAG, "用户没有录入生物识别数据")
            }
            BiometricManager.BIOMETRIC_ERROR_UNSUPPORTED -> {
                // 当设备的Android版本等于或小于Q
                // 并且allowedAuthenticators设定为BIOMETRIC_STRONG or DEVICE_CREDENTIAL时，回调此方法
                // 需要通过其他API判断设备是否支持生物识别，因此当设备的Android版本等于或小于Q时
                // allowedAuthenticators建议设置为BIOMETRIC_STRONG、BIOMETRIC_STRONG或 BIOMETRIC_WEAK or DEVICE_CREDENTIAL
            }
            else -> {
                Log.d(TAG, "Unknown")
            }
        }
    }

    // 设置 DEVICE_CREDENTIAL 表示：允许利用设备 PIN 码、图案或密码等非生物认证进行身份验证
    private val allowedAuthenticators = BiometricManager.Authenticators.BIOMETRIC_STRONG or BiometricManager.Authenticators.DEVICE_CREDENTIAL
    private var authenticationCallback: BiometricPrompt.AuthenticationCallback? = null
    private var biometricPromptInfo: BiometricPrompt.PromptInfo? = null

    fun buildAuthenticationCallback(onResult: FunBiometricAuthCallback?) : MaBiometricUtil {
        authenticationCallback = object : BiometricPrompt.AuthenticationCallback() {
            override fun onAuthenticationError(errCode: Int, errString: CharSequence) {
                super.onAuthenticationError(errCode, errString)
                onResult?.let {
                    it(false, errCode, null)
                }
            }

            override fun onAuthenticationFailed() {
                super.onAuthenticationFailed()
                onResult?.let {
                    it(false, 0, null)
                }
            }

            override fun onAuthenticationSucceeded(result: BiometricPrompt.AuthenticationResult) {
                super.onAuthenticationSucceeded(result)
                onResult?.let {
                    it(true, 0, result)
                }
                // 验证通过，可以通过result.authenticationType获取使用的生物识别类型。
            }
        }
        return this
    }

    fun buildBiometricPromptInfo(title: String, subTitle: String, description: String, cancelBtn: String) : MaBiometricUtil {
        biometricPromptInfo = BiometricPrompt.PromptInfo.Builder().run {
            setTitle(title)
            setSubtitle(subTitle)
            setDescription(description)
            setAllowedAuthenticators(allowedAuthenticators)
            setConfirmationRequired(false)
            if (allowedAuthenticators and BiometricManager.Authenticators.DEVICE_CREDENTIAL == 0) {
                // 注意：
                //  如果 allowedAuthenticators 配置了 DEVICE_CREDENTIAL 则不能 setNegativeButtonText，
                //      否则会抛出异常 IllegalArgumentException("Negative text must not be set if device credential authentication is allowed.")
                //  如果 allowedAuthenticators 没配置 DEVICE_CREDENTIAL 则需要 setNegativeButtonText，
                //      否则会抛出异常 IllegalArgumentException("Negative text must be set and non-empty.")
                setNegativeButtonText(cancelBtn)
            }
            build()
        }
        return this
    }

    fun showAuthentication(activity: FragmentActivity?, crypto: BiometricPrompt.CryptoObject? = null, fragment: Fragment? = null, executor: Executor? = null) : Boolean {
        if (authenticationCallback == null || biometricPromptInfo == null) {
            return false
        }

        val callback = authenticationCallback!!
        val promptInfo = biometricPromptInfo!!

        activity?.let { curActivity ->
            val curExecutor = executor ?: ContextCompat.getMainExecutor(curActivity)
            BiometricPrompt(curActivity, curExecutor, callback).apply {
                if (null == crypto) {
                    this.authenticate(promptInfo)
                } else {
                    this.authenticate(promptInfo, crypto)
                }
            }
            return true
        }

        fragment?.let { curFragment ->
            executor?.let { curExecutor ->
                BiometricPrompt(curFragment, curExecutor, callback).apply {
                    if (null == crypto) {
                        this.authenticate(promptInfo)
                    } else {
                        this.authenticate(promptInfo, crypto)
                    }
                }
                return true
            }
        }

        return false
    }
}