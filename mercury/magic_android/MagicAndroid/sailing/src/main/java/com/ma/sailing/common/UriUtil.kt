package com.ma.sailing.common

import android.content.ContentResolver
import android.content.ContentUris
import android.content.Context
import android.content.Intent
import android.content.pm.PackageManager
import android.net.Uri
import android.os.Build
import android.os.Environment
import android.os.storage.StorageManager
import android.provider.DocumentsContract
import android.provider.MediaStore
import android.text.TextUtils
import android.util.Log
import android.widget.Toast
import androidx.documentfile.provider.DocumentFile
import com.ma.sailing.R
import java.io.*
import java.util.*


object UriUtil {
    fun gotoUrl(context: Context, resId: Int) {
        gotoUrl(context, context.getString(resId))
    }

    fun gotoUrl(context: Context, url: String?) {
        try {
            if (url != null && url.isNotEmpty()) {
                // Default http:// if no protocol specified
                val newUrl = if (!url.contains("://")) {
                    "http://$url"
                } else {
                    url
                }
                context.startActivity(Intent(Intent.ACTION_VIEW, Uri.parse(newUrl)))
            }
        } catch (e: Exception) {
            Toast.makeText(context, R.string.no_url_handler, Toast.LENGTH_LONG).show()
        }
    }

    @JvmStatic
    fun uri2File(context: Context, uri: Uri): File? {
        val authority = uri.authority
        val scheme = uri.scheme
        val path = uri.path
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N && path != null) {
            val externals = arrayOf("/external", "/external_path")
            for (external in externals) {
                if (path.startsWith("$external/")) {
                    val file = File(
                        Environment.getExternalStorageDirectory().absolutePath
                            + path.replace(external, ""))
                    if (file.exists()) {
                        return file
                    }
                }
            }
        }
        return if (ContentResolver.SCHEME_FILE == scheme) {
            path?.let { File(it) }
        } // end 0
        else if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT
            && DocumentsContract.isDocumentUri(context, uri)) {
            if ("com.android.externalstorage.documents" == authority) {
                val docId = DocumentsContract.getDocumentId(uri)
                val split = docId.split(":".toRegex()).toTypedArray()
                val type = split[0]
                if ("primary".equals(type, ignoreCase = true)) {
                    return File(Environment.getExternalStorageDirectory().toString() + "/" + split[1])
                } else {
                    // Below logic is how External Storage provider build URI for documents
                    // http://stackoverflow.com/questions/28605278/android-5-sd-card-label
                    val mStorageManager = context.getSystemService(Context.STORAGE_SERVICE) as StorageManager
                    try {
                        val storageVolumeClazz = Class.forName("android.os.storage.StorageVolume")
                        val getVolumeList = mStorageManager.javaClass.getMethod("getVolumeList")
                        val getUuid = storageVolumeClazz.getMethod("getUuid")
                        val getState = storageVolumeClazz.getMethod("getState")
                        val getPath = storageVolumeClazz.getMethod("getPath")
                        val isPrimary = storageVolumeClazz.getMethod("isPrimary")
                        val isEmulated = storageVolumeClazz.getMethod("isEmulated")
                        val result = getVolumeList.invoke(mStorageManager)
                        val length = java.lang.reflect.Array.getLength(result)
                        for (i in 0 until length) {
                            val storageVolumeElement = java.lang.reflect.Array.get(result, i)
                            //String uuid = (String) getUuid.invoke(storageVolumeElement);
                            val mounted = Environment.MEDIA_MOUNTED == getState.invoke(storageVolumeElement) || Environment.MEDIA_MOUNTED_READ_ONLY == getState.invoke(storageVolumeElement)

                            //if the media is not mounted, we need not get the volume details
                            if (!mounted) continue

                            //Primary storage is already handled.
                            if (isPrimary.invoke(storageVolumeElement) as Boolean
                                && isEmulated.invoke(storageVolumeElement) as Boolean) {
                                continue
                            }
                            val uuid = getUuid.invoke(storageVolumeElement) as String
                            if (uuid != null && uuid == type) {
                                return File(getPath.invoke(storageVolumeElement).toString() + "/" + split[1])
                            }
                        }
                    } catch (ex: Exception) {
                        Log.d("UriUtils", "$uri parse failed. $ex -> 1_0")
                    }
                }
                null
            } // end 1_0
            else if ("com.android.providers.downloads.documents" == authority) {
                val id = DocumentsContract.getDocumentId(uri)
                if (!TextUtils.isEmpty(id)) {
                    try {
                        val contentUri = ContentUris.withAppendedId(
                            Uri.parse("content://downloads/public_downloads"),
                            java.lang.Long.valueOf(id)
                        )
                        return getFileFromUri(context, contentUri, "1_1")
                    } catch (e: NumberFormatException) {
                        if (id.startsWith("raw:")) {
                            return File(id.substring(4))
                        }
                    }
                }
                null
            } // end 1_1
            else if ("com.android.providers.media.documents" == authority) {
                val docId = DocumentsContract.getDocumentId(uri)
                val split = docId.split(":".toRegex()).toTypedArray()
                val type = split[0]
                val contentUri: Uri
                contentUri = if ("image" == type) {
                    MediaStore.Images.Media.EXTERNAL_CONTENT_URI
                } else if ("video" == type) {
                    MediaStore.Video.Media.EXTERNAL_CONTENT_URI
                } else if ("audio" == type) {
                    MediaStore.Audio.Media.EXTERNAL_CONTENT_URI
                } else {
                    return null
                }
                val selection = "_id=?"
                val selectionArgs = arrayOf(split[1])
                getFileFromUri(context, contentUri, selection, selectionArgs, "1_2")
            } // end 1_2
            else if (ContentResolver.SCHEME_CONTENT == scheme) {
                getFileFromUri(context, uri, "1_3")
            } // end 1_3
            else {
                null
            } // end 1_4
        } // end 1
        else if (ContentResolver.SCHEME_CONTENT == scheme) {
            getFileFromUri(context, uri, "2")
        } // end 2
        else {
            null
        } // end 3
    }

    @JvmStatic
    private fun getFileFromUri(context: Context, uri: Uri, code: String): File? {
        return getFileFromUri(context, uri, null, null, code)
    }

    @JvmStatic
    private fun getFileFromUri(context: Context,
                               uri: Uri,
                               selection: String?,
                               selectionArgs: Array<String>?,
                               code: String): File? {
        val cursor = context.contentResolver.query(uri, arrayOf("_data"), selection, selectionArgs, null) ?: return null
        return try {
            if (cursor.moveToFirst()) {
                val columnIndex = cursor.getColumnIndex("_data")
                if (columnIndex > -1) {
                    File(cursor.getString(columnIndex))
                } else {
                    null
                }
            } else {
                null
            }
        } catch (e: Exception) {
            null
        } finally {
            try {
                cursor.close()
            } catch (e: Exception) {
            }
        }
    }

    fun getFileData(context: Context, fileUri: Uri?): DocumentFile? {
        if (fileUri == null)
            return null
        return try {
            when {
                isFileScheme(fileUri) -> {
                    fileUri.path?.let {
                        File(it).let { file ->
                            return DocumentFile.fromFile(file)
                        }
                    }
                }
                isContentScheme(fileUri) -> {
                    DocumentFile.fromSingleUri(context, fileUri)
                }
                else -> {
                    Log.e("FileData", "Content scheme not known")
                    null
                }
            }
        } catch (e: Exception) {
            Log.e(TAG, "Unable to get document file", e)
            null
        }
    }

    @Throws(FileNotFoundException::class)
    fun getUriOutputStream(contentResolver: ContentResolver, fileUri: Uri?): OutputStream? {
        if (fileUri == null)
            return null
        return when {
            isFileScheme(fileUri) -> fileUri.path?.let { FileOutputStream(it) }
            isContentScheme(fileUri) -> {
                try {
                    contentResolver.openOutputStream(fileUri, "wt")
                } catch (e: FileNotFoundException) {
                    Log.e(TAG, "Unable to open stream in `wt` mode, retry in `rwt` mode.", e)
                    // https://issuetracker.google.com/issues/180526528
                    // Try with rwt to fix content provider issue
                    val outStream = contentResolver.openOutputStream(fileUri, "rwt")
                    Log.w(TAG, "`rwt` mode used.")
                    outStream
                }
            }
            else -> null
        }
    }

    @Throws(FileNotFoundException::class)
    fun getUriInputStream(contentResolver: ContentResolver, fileUri: Uri?): InputStream? {
        if (fileUri == null)
            return null
        return when {
            isFileScheme(fileUri) -> fileUri.path?.let { FileInputStream(it) }
            isContentScheme(fileUri) -> contentResolver.openInputStream(fileUri)
            else -> null
        }
    }

    private fun isFileScheme(fileUri: Uri): Boolean {
        val scheme = fileUri.scheme
        if (scheme == null || scheme.isEmpty() || scheme.lowercase(Locale.ENGLISH) == "file") {
            return true
        }
        return false
    }

    private fun isContentScheme(fileUri: Uri): Boolean {
        val scheme = fileUri.scheme
        if (scheme != null && scheme.lowercase(Locale.ENGLISH) == "content") {
            return true
        }
        return false
    }

    fun parse(stringUri: String?): Uri? {
        return if (stringUri?.isNotEmpty() == true) {
            Uri.parse(stringUri)
        } else
            null
    }

    fun decode(uri: String?): String {
        return Uri.decode(uri) ?: ""
    }

    private fun persistUriPermission(contentResolver: ContentResolver?,
                                     uri: Uri,
                                     release: Boolean,
                                     readOnly: Boolean) {
        try {
            // try to persist read and write permissions
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT) {
                contentResolver?.apply {
                    var readPermissionAllowed = false
                    var writePermissionAllowed = false
                    // Check current permissions allowed
                    persistedUriPermissions.find { uriPermission ->
                        uriPermission.uri == uri
                    }?.let { uriPermission ->
                        Log.d(TAG, "Check URI permission : $uriPermission")
                        if (uriPermission.isReadPermission) {
                            readPermissionAllowed = true
                        }
                        if (uriPermission.isWritePermission) {
                            writePermissionAllowed = true
                        }
                    }

                    // Release permission
                    if (release) {
                        if (writePermissionAllowed) {
                            Log.d(TAG, "Release write permission : $uri")
                            val removeFlags: Int = Intent.FLAG_GRANT_WRITE_URI_PERMISSION
                            releasePersistableUriPermission(uri, removeFlags)
                        }
                        if (readPermissionAllowed) {
                            Log.d(TAG, "Release read permission $uri")
                            val takeFlags: Int = Intent.FLAG_GRANT_READ_URI_PERMISSION
                            releasePersistableUriPermission(uri, takeFlags)
                        }
                    }

                    // Take missing permission
                    if (!readPermissionAllowed) {
                        Log.d(TAG, "Take read permission $uri")
                        val takeFlags: Int = Intent.FLAG_GRANT_READ_URI_PERMISSION
                        takePersistableUriPermission(uri, takeFlags)
                    }
                    if (readOnly) {
                        if (writePermissionAllowed) {
                            Log.d(TAG, "Release write permission $uri")
                            val removeFlags: Int = Intent.FLAG_GRANT_WRITE_URI_PERMISSION
                            releasePersistableUriPermission(uri, removeFlags)
                        }
                    } else {
                        if (!writePermissionAllowed) {
                            Log.d(TAG, "Take write permission $uri")
                            val takeFlags: Int = Intent.FLAG_GRANT_WRITE_URI_PERMISSION
                            takePersistableUriPermission(uri, takeFlags)
                        }
                    }
                }
            }
        } catch (e: Exception) {
            if (release)
                Log.e(TAG, "Unable to release persistable URI permission", e)
            else
                Log.e(TAG, "Unable to take persistable URI permission", e)
        }
    }

    fun takeUriPermission(contentResolver: ContentResolver?,
                          uri: Uri,
                          readOnly: Boolean = false) {
        persistUriPermission(contentResolver, uri, false, readOnly)
    }

    fun releaseUriPermission(contentResolver: ContentResolver?,
                             uri: Uri) {
        persistUriPermission(contentResolver, uri, release = true, readOnly = false)
    }

    fun getUriFromIntent(intent: Intent, key: String): Uri? {
        try {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN) {
                val clipData = intent.clipData
                if (clipData != null) {
                    if (clipData.description.label == key) {
                        if (clipData.itemCount == 1) {
                            val clipItem = clipData.getItemAt(0)
                            if (clipItem != null) {
                                return clipItem.uri
                            }
                        }
                    }
                }
            }
        } catch (e: Exception) {
            return intent.getParcelableExtra(key)
        }
        return null
    }

    private fun isExternalAppInstalled(context: Context, packageName: String, showError: Boolean = true): Boolean {
        try {
            context.applicationContext.packageManager.getPackageInfo(packageName, PackageManager.GET_ACTIVITIES)
            return true
        } catch (e: Exception) {
            if (showError)
                Log.e(TAG, "App not accessible", e)
        }
        return false
    }

    fun openExternalApp(context: Context, packageName: String) {
        var launchIntent: Intent? = null
        try {
            launchIntent = context.packageManager.getLaunchIntentForPackage(packageName)?.apply {
                addFlags(Intent.FLAG_ACTIVITY_NEW_TASK)
            }
        } catch (ignored: Exception) {
        }
        try {
            if (launchIntent == null) {
                context.startActivity(
                    Intent(Intent.ACTION_VIEW)
                        .addFlags(Intent.FLAG_ACTIVITY_NEW_TASK)
                        .setData(Uri.parse("https://play.google.com/store/apps/details?id=$packageName"))
                )
            } else {
                context.startActivity(launchIntent)
            }
        } catch (e: Exception) {
            Log.e(TAG, "App cannot be open", e)
        }
    }

    fun getBinaryDir(context: Context): File {
        return if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            context.applicationContext.noBackupFilesDir
        } else {
            context.applicationContext.filesDir
        }
    }

    private const val TAG = "UriUtil"
}
