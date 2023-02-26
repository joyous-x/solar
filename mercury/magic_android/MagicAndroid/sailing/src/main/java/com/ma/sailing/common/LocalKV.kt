package com.ma.sailing.common

import android.content.Context
import com.tencent.mmkv.MMKV

class LocalKV(private val mSectionName: String) {
    private val defaultMKS: MMKV = MMKV.defaultMMKV(MMKV.MULTI_PROCESS_MODE, null)

    private fun getKeyName(key: String): String {
        return mSectionName + "_" + key
    }

    // 外部通过 LocalKVEditor 调用对应的方法
    fun putString(key: String, value: String?) {
        defaultMKS.putString(getKeyName(key), value)
    }

    fun getString(key: String, defValue: String?): String? {
        return defaultMKS.getString(getKeyName(key), defValue)
    }

    // 外部通过 LocalKVEditor 调用对应的方法
    fun putBoolean(key: String, value: Boolean) {
        defaultMKS.encode(getKeyName(key), value)
    }

    fun getBoolean(key: String, defValue: Boolean): Boolean {
        return defaultMKS.decodeBool(getKeyName(key), defValue)
    }

    // 外部通过 LocalKVEditor 调用对应的方法
    fun putInt(key: String, value: Int) {
        defaultMKS.encode(getKeyName(key), value)
    }

    fun getInt(key: String, defValue: Int): Int {
        return defaultMKS.decodeInt(getKeyName(key), defValue)
    }

    // 外部通过 LocalKVEditor 调用对应的方法
    fun putLong(key: String, value: Long) {
        defaultMKS.encode(getKeyName(key), value)
    }

    fun getLong(key: String, defValue: Long): Long {
        return defaultMKS.decodeLong(getKeyName(key), defValue)
    }

    // 外部通过 LocalKVEditor 调用对应的方法
    fun putFloat(key: String, value: Float) {
        defaultMKS.encode(getKeyName(key), value)
    }

    fun getFloat(key: String, defValue: Float): Float {
        return defaultMKS.decodeFloat(getKeyName(key), defValue)
    }

    // 外部通过 LocalKVEditor 调用对应的方法
    fun remove(key: String) {
        defaultMKS.remove(getKeyName(key))
    }

    fun contains(key: String): Boolean {
        return defaultMKS.contains(key)
    }

    fun allKeys(): Array<String>? {
        return defaultMKS.allKeys()
    }

    companion object {
        private lateinit var sDefaultKv: LocalKV

        @JvmStatic
        fun init(context: Context) {
            MMKV.initialize(context)
            sDefaultKv = LocalKV("local_kv")
        }

        fun putString(key: String, value: String) {
            sDefaultKv.putString(key, value)
        }

        fun getString(key: String, default: String?): String? {
            return sDefaultKv.getString(key, default)
        }

        fun putBoolean(key: String, value: Boolean) {
            sDefaultKv.putBoolean(key, value)
        }

        fun getBoolean(key: String, default: Boolean): Boolean {
            return sDefaultKv.getBoolean(key, default)
        }

        fun putLong(key: String, value: Long) {
            sDefaultKv.putLong(key, value)
        }

        fun getLong(key: String, default: Long): Long {
            return sDefaultKv.getLong(key, default)
        }
    }
}