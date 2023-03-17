package com.ma.sailing.common

import java.util.*

object BytesUtil {

    fun uuidTo16Bytes(uuid: UUID): ByteArray {
        val buf = ByteArray(16)
        for (i in 0 until 8) {
            buf[i] = (uuid.mostSignificantBits.ushr(8 * i) and 0xFF).toByte()
        }
        for (i in 8 until 16) {
            buf[i] = (uuid.leastSignificantBits.ushr(8 * i) and 0xFF).toByte()
        }
        return buf
    }

    fun bytes2HexStr(bytes: ByteArray): String {
        val resultBuff = StringBuffer(bytes.size * 2)
        for (e in bytes) {
            val cH = e.toInt() and 0xF0 shr 4
            val cL = e.toInt() and 0x0F
            resultBuff.append(cH)
            resultBuff.append(cL)
        }
        return resultBuff.toString()
    }

}