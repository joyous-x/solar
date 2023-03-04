package com.ma.sailing.common

class ColorUtil {
    companion object {
        private val instance = ColorUtil()

        @JvmStatic
        fun getInstance(): ColorUtil {
            return instance
        }
    }

    fun getFormattedColorString(color: Int, showAlpha: Boolean): String {
        return if (showAlpha) String.format("#%08X", color) else String.format(
            "#%06X", 16777215 and color
        )
    }
}