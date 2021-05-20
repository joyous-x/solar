package com.example.jiao.testacdd;

import android.content.Context;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.Signature;
import android.os.Build;
import android.util.Base64;

import java.io.ByteArrayInputStream;
import java.io.File;
import java.io.InputStream;
import java.security.MessageDigest;
import java.security.cert.CertificateFactory;
import java.security.cert.X509Certificate;

/**
 * Created by jiao on 2016/5/31.
 */
public class PkgSignatureTool {
    public static String getPkgSignatureMd5(Context ctx, File file) {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.ICE_CREAM_SANDWICH) {
            // In android 4.0 or less, the apk does not include signatures. It is an android system bug;
            return null;
        }

        if (file == null || !file.exists()) {
            return null;
        }

        if (null == ctx) {
            return null;
        }

        final PackageManager pkgMgr = ctx.getPackageManager();
        PackageInfo pkgSignatureInfo = null;

        try {
            pkgSignatureInfo = pkgMgr.getPackageArchiveInfo(file.getAbsolutePath(), PackageManager.GET_SIGNATURES);
        } catch (Exception e) {
            e.printStackTrace();
        }

        String[] pkgSignatureMD5 = getPkgSignatureMD5(pkgSignatureInfo.signatures);
        if (pkgSignatureMD5 == null || pkgSignatureMD5.length == 0) {
            return null;
        }

        return pkgSignatureMD5[0];
    }

    private static String[] getPkgSignatureMD5(Signature[] signature) {
        if (signature.length == 0)
            return null;

        if (signature[0] == null)
            return null;

        byte[] cert = signature[0].toByteArray();
        if (cert.length <= 0)
            return null;

        InputStream input = new ByteArrayInputStream(cert);

        CertificateFactory cf;
        X509Certificate c;
        String SignatureMD5[] = new String[2];
        try {
            cf = CertificateFactory.getInstance("X509");

            c = (X509Certificate) cf.generateCertificate(input);
            byte[] encoded = c.getEncoded();

            String base64 = Base64.encodeToString(encoded, Base64.NO_WRAP);
            SignatureMD5[0] = getByteArrayMD5(base64.getBytes());
            SignatureMD5[1] = c.getIssuerDN().toString();

        } catch (Exception e) {
            e.printStackTrace();
            return null;
        }

        return SignatureMD5;
    }

    private static String getByteArrayMD5(byte[] data) {
        if (null == data) return null;
        try {
            MessageDigest md = MessageDigest.getInstance("MD5");
            md.update(data);
            return encodeHex(md.digest());
        } catch (Exception e) {
            return null;
        }
    }

    private static String encodeHex(byte[] data) {
        if (data == null) return null;

        final String HEXES = "0123456789abcdef";
        int len = data.length;
        StringBuilder hex = new StringBuilder(len * 2);

        for (byte aData : data) {
            hex.append(HEXES.charAt((aData & 0xF0) >>> 4));
            hex.append(HEXES.charAt((aData & 0x0F)));
        }

        return hex.toString();
    }
}
