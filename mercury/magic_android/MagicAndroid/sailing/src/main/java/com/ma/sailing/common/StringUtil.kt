package com.ma.sailing.common

import java.util.regex.Pattern

object StringUtil {
    fun String.removeLineChars(): String {
        return this.replace("[\\r|\\n|\\t|\\u00A0]+".toRegex(), "")
    }

    fun String.removeSpaceChars(): String {
        return this.replace("[\\r|\\n|\\t|\\s|\\u00A0]+".toRegex(), "")
    }

    fun ByteArray.toHexString() = joinToString("") { "%02X".format(it) }

    fun isEmailAvailable(input: String) : Boolean {
        val regexEmail = """^\w+([-+.]\w+)*@\w+([-.]\w+)*\.\w+([-.]\w+)*$"""
        return input.isNotEmpty() && Pattern.matches(regexEmail, input)
    }
}