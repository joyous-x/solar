package com.ma.sailing.common;


import androidx.annotation.Nullable;

import java.util.UUID;

public class UuidUtil {
    public static @Nullable String toHexString(@Nullable UUID uuid) {
        if (uuid == null) { return null; }
        try {
            byte[] buf = BytesUtil.INSTANCE.uuidTo16Bytes(uuid);

            int len = buf.length;
            if (len == 0) {
                return "";
            }

            StringBuilder sb = new StringBuilder();

            short bt;
            char high, low;
            for (byte b : buf) {
                bt = (short) (b & 0xFF);
                high = (char) (bt >>> 4);
                low = (char) (bt & 0x0F);
                sb.append(byteToChar(high));
                sb.append(byteToChar(low));
            }

            return sb.toString();
        } catch (Exception e) {
            return null;
        }
    }

    public static @Nullable UUID fromHexString(@Nullable String hexString) {
        if (hexString == null)
            return null;

        if (hexString.length() != 32)
            return null;

        char[] charArray = hexString.toLowerCase().toCharArray();
        char[] leastSignificantChars = new char[16];
        char[] mostSignificantChars = new char[16];

        for (int i = 31; i >= 0; i = i-2) {
            if (i >= 16) {
                mostSignificantChars[32-i] = charArray[i];
                mostSignificantChars[31-i] = charArray[i-1];
            } else {
                leastSignificantChars[16-i] = charArray[i];
                leastSignificantChars[15-i] = charArray[i-1];
            }
        }
        StringBuilder standardUUIDString = new StringBuilder();
        standardUUIDString.append(leastSignificantChars);
        standardUUIDString.append(mostSignificantChars);
        standardUUIDString.insert(8, '-');
        standardUUIDString.insert(13, '-');
        standardUUIDString.insert(18, '-');
        standardUUIDString.insert(23, '-');
        try {
            return UUID.fromString(standardUUIDString.toString());
        } catch (Exception e) {
            return null;
        }
    }

    // Use short to represent unsigned byte
    private static char byteToChar(char bt) {
        if (bt >= 10) {
            return (char)('A' + bt - 10);
        }
        else {
            return (char)('0' + bt);
        }
    }
}
