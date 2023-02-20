package com.ma.sailing.common

import java.util.regex.Pattern

object StringUtil {

    fun isEmailAvailable(input: String) : Boolean {
        val regexEmail = """^\w+([-+.]\w+)*@\w+([-.]\w+)*\.\w+([-.]\w+)*$"""
        return input.isNotEmpty() && Pattern.matches(regexEmail, input)
    }
}