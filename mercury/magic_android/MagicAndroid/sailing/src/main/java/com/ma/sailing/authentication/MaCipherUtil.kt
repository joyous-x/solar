package com.ma.sailing.authentication

import android.os.Build
import android.security.keystore.KeyGenParameterSpec
import android.security.keystore.KeyProperties
import androidx.annotation.RequiresApi
import java.security.KeyStore
import java.security.spec.AlgorithmParameterSpec
import javax.crypto.Cipher
import javax.crypto.KeyGenerator
import javax.crypto.SecretKey
import javax.crypto.spec.GCMParameterSpec


@RequiresApi(Build.VERSION_CODES.M)
class MaCipherUtil(
    private val algorithm: String = KeyProperties.KEY_ALGORITHM_AES,
    private val blockMode: String = KeyProperties.BLOCK_MODE_GCM,
    private val encryptionPadding: String = KeyProperties.ENCRYPTION_PADDING_NONE
) {
    companion object {
        // 为了关联密钥和终端中注册的指纹，请使用由 AndroidKeyStore 供应器
        // 提供的 KeyStore 实例，来创建并注册新密钥或注册现有密钥。
        const val PROVIDER_ANDROID_KEY_STORE = "AndroidKeyStore"
        //
        const val KEY_SIZE = 256
    }

    private fun getSecretKeyFromKeyStore(keyAlias: String, keyPasswd: String? = null, createIfNotExist: Boolean = true) : SecretKey? {
        // ? 是否是：ANDROID_KEYSTORE
        val defaultType = KeyStore.getDefaultType()

        val keyStore = KeyStore.getInstance(PROVIDER_ANDROID_KEY_STORE).apply {
            // Before a keystore can be accessed, it must be loaded.
            this.load(null)
            //> 可以从指定文件中加载
            // FileInputStream("keyStoreName").use { fis -> keyStore.load(fis, keyPasswd?.toCharArray()) }
        }

        keyStore.getKey(keyAlias, keyPasswd?.toCharArray())?.let {
            return it as SecretKey
        }

        if (!createIfNotExist) {
            return null
        }

        val keyGenerator = KeyGenerator.getInstance(algorithm, PROVIDER_ANDROID_KEY_STORE).apply {
            val parameterPurposes = KeyProperties.PURPOSE_ENCRYPT or KeyProperties.PURPOSE_DECRYPT
            val keyGenParams = KeyGenParameterSpec.Builder(keyAlias, parameterPurposes).apply {
                setBlockModes(blockMode)
                setEncryptionPaddings(encryptionPadding)
                setKeySize(KEY_SIZE)
                setUserAuthenticationRequired(true)
            }.build()
            this.init(keyGenParams)
        }
        return keyGenerator.generateKey()
    }

    private fun createCipher() : Cipher {
        val transformation = "$algorithm/$blockMode/$encryptionPadding"
        return Cipher.getInstance(transformation)
    }

    fun getInitializedCipherForEncryption(keyAlias: String): Cipher {
        return createCipher().apply {
            val secretKey = getSecretKeyFromKeyStore(keyAlias)
            this.init(Cipher.ENCRYPT_MODE, secretKey)
        }
    }

    fun getInitializedCipherForDecryption(keyAlias: String, iv: ByteArray): Cipher {
        return createCipher().apply {
            val secretKey = getSecretKeyFromKeyStore(keyAlias)
            this.init(Cipher.DECRYPT_MODE, secretKey, getAlgorithmParameterSpec(iv))
        }
    }

    private fun getAlgorithmParameterSpec(iv: ByteArray?) : AlgorithmParameterSpec? {
        if (blockMode == KeyProperties.BLOCK_MODE_GCM) {
            return GCMParameterSpec(128, iv)
        }
        return null
    }
}