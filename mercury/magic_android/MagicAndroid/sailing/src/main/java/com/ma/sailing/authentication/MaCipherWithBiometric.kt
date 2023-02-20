package com.ma.sailing.authentication

import android.os.Build
import androidx.annotation.RequiresApi
import androidx.biometric.BiometricPrompt
import androidx.fragment.app.FragmentActivity
import java.nio.charset.Charset
import javax.crypto.Cipher


typealias OnCipherWithBiometricFinish = (rstCode: Int, data: MaCipherDataWrapper?, result: BiometricPrompt.AuthenticationResult?) -> Unit


data class MaCipherDataWrapper(val cipherData: ByteArray, val iv: ByteArray) {
    override fun equals(other: Any?): Boolean {
        if (this === other) return true
        if (javaClass != other?.javaClass) return false

        other as MaCipherDataWrapper

        if (!cipherData.contentEquals(other.cipherData)) return false
        if (!iv.contentEquals(other.iv)) return false

        return true
    }

    override fun hashCode(): Int {
        var result = cipherData.contentHashCode()
        result = 31 * result + iv.contentHashCode()
        return result
    }
}


class MaCipherWithBiometric {
    companion object {
        fun bytes2String(data: ByteArray, charsetName: String = "UTF-8") : String {
            return String(data, Charset.forName(charsetName))
        }
    }

    private var secretKeyName: String = ""
    private var authCallback: FunBiometricAuthCallback? = null
    private var cipher: Cipher? = null

    @RequiresApi(Build.VERSION_CODES.M)
    fun buildEncryptCipher(keyName: String, rawData: String, onFinish: OnCipherWithBiometricFinish?) : MaCipherWithBiometric {
        this.authCallback = BiometricAuthCallback@{ success, rstCode, authResult ->
            if (!success || authResult == null) {
                onFinish?.invoke(rstCode, null, authResult)
                return@BiometricAuthCallback
            }

            val cipher = authResult.cryptoObject?.cipher
            if (cipher == null) {
                onFinish?.invoke(rstCode, null, authResult)
                return@BiometricAuthCallback
            }

            val encryptedData = cipher.doFinal(rawData.toByteArray(Charset.forName("UTF-8")))
            val cipherDataWrapper = MaCipherDataWrapper(encryptedData, cipher.iv)
            onFinish?.invoke(rstCode, cipherDataWrapper, authResult)
        }

        this.secretKeyName = keyName
        this.cipher = MaCipherUtil().getInitializedCipherForEncryption(secretKeyName)

        return this
    }

    @RequiresApi(Build.VERSION_CODES.M)
    fun buildDecryptCipher(keyName: String, cipherData: MaCipherDataWrapper, onFinish: OnCipherWithBiometricFinish?) : MaCipherWithBiometric {
        this.authCallback = BiometricAuthCallback@{ success, rstCode, authResult ->
            if (!success || authResult == null) {
                onFinish?.invoke(rstCode, null, authResult)
                return@BiometricAuthCallback
            }

            val cipher = authResult.cryptoObject?.cipher
            if (cipher == null) {
                onFinish?.invoke(rstCode, null, authResult)
                return@BiometricAuthCallback
            }

            val decryptedData = cipher.doFinal(cipherData.cipherData)
            val cipherDataWrapper = MaCipherDataWrapper(decryptedData, cipher.iv)
            onFinish?.invoke(rstCode, cipherDataWrapper, authResult)
        }

        this.secretKeyName = keyName
        this.cipher = MaCipherUtil().getInitializedCipherForDecryption(secretKeyName, cipherData.iv)

        return this
    }

    @RequiresApi(Build.VERSION_CODES.M)
    fun showAuthentication(activity: FragmentActivity?,
                           title: String,
                           subTitle: String,
                           description: String,
                           cancelBtn: String) : Boolean {
        val cryptoObject = cipher?.let { BiometricPrompt.CryptoObject(it) }
        return MaBiometricUtil()
                .buildBiometricPromptInfo(title, subTitle, description, cancelBtn)
                .buildAuthenticationCallback(authCallback)
                .showAuthentication(activity, cryptoObject)
    }
}
