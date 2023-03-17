package com.ma.sailing.common.hash

import com.ma.sailing.common.BytesUtil
import java.io.File
import java.io.FileInputStream
import java.io.InputStream
import java.security.MessageDigest

object HashUtil {

    @Synchronized
    fun getStrMD5(data: String): String {
        return getByteMD5(data.toByteArray(Charsets.UTF_8))
    }

    @Synchronized
    fun getByteMD5(data: ByteArray): String {
        val msgDigest = MessageDigest.getInstance("MD5").apply {
            update(data)
        }
        return BytesUtil.bytes2HexStr(msgDigest.digest())
    }

    @Synchronized
    fun getStreamMD5(data: InputStream): String {
        val msgDigest = MessageDigest.getInstance("MD5")

        var bufferSize = 1024
        val buffer = ByteArray(bufferSize)

        try {
            var numRead: Int
            while (data.read(buffer, 0, bufferSize).also { numRead = it } != -1) {
                msgDigest.update(buffer, 0, numRead)
            }
        } catch (e: Exception) {
            return ""
        }
        return BytesUtil.bytes2HexStr(msgDigest.digest())
    }

    fun getFileMD5(file: File): String {
        val msgDigest = MessageDigest.getInstance("MD5")

        var bufferSize = 1024
        val buffer = ByteArray(bufferSize)

        try {
            val fin = FileInputStream(file)
            var numRead = 0
            while (fin.read(buffer).also { numRead = it } > 0) {
                msgDigest.update(buffer, 0, numRead)
            }
        } catch (e: java.lang.Exception) {
            return ""
        }
        return BytesUtil.bytes2HexStr(msgDigest.digest())
    }
}