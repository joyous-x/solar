package com.ma.sailing.common

import android.annotation.SuppressLint
import android.content.Context
import android.content.Intent
import android.content.pm.PackageManager
import android.net.Uri
import android.os.Build
import android.util.Log
import android.view.View
import androidx.activity.result.ActivityResultCallback
import androidx.activity.result.ActivityResultLauncher
import androidx.activity.result.contract.ActivityResultContracts
import androidx.fragment.app.Fragment
import androidx.fragment.app.FragmentActivity
import com.ma.sailing.dialogs.FileManagerDialogFragment

class SafHelper {
    companion object {
        private const val TAG = "SafHelper"

        @SuppressLint("InlinedApi")
        fun allowCreateFileBySAF(packageManager: PackageManager,
                                 fileType: String = "application/octet-stream"): Boolean {
            return when {
                // To check if a custom file manager can manage the ACTION_CREATE_DOCUMENT
                Build.VERSION.SDK_INT < Build.VERSION_CODES.KITKAT -> {
                    packageManager.queryIntentActivities(Intent(Intent.ACTION_CREATE_DOCUMENT).apply {
                        addCategory(Intent.CATEGORY_OPENABLE)
                        type = fileType
                    }, PackageManager.MATCH_DEFAULT_ONLY).isNotEmpty()
                }
                else -> true
            }
        }
    }

    private var activity: FragmentActivity? = null
    private var fragment: Fragment? = null

    private var getContentResultLauncher: ActivityResultLauncher<String>? = null
    private var openFileResultLauncher: ActivityResultLauncher<Array<String>>? = null
    private var createFileResultLauncher: ActivityResultLauncher<String>? = null

    constructor(context: FragmentActivity) {
        this.activity = context
        this.fragment = null
    }

    constructor(context: Fragment) {
        this.activity = context.activity
        this.fragment = context
    }

    fun buildOpenFile(onFileSelected: ((uri: Uri?) -> Unit)?) {
        val resultCallback = ActivityResultCallback<Uri?> { result ->
            result?.let { uri ->
                UriUtil.takeUriPermission(activity?.contentResolver, uri)
                onFileSelected?.invoke(uri)
            }
        }

        getContentResultLauncher = if (fragment != null) {
            fragment?.registerForActivityResult(
                GetContent(),
                resultCallback
            )
        } else {
            activity?.registerForActivityResult(
                GetContent(),
                resultCallback
            )
        }

        openFileResultLauncher = if (fragment != null) {
            fragment?.registerForActivityResult(
                OpenDocument(),
                resultCallback
            )
        } else {
            activity?.registerForActivityResult(
                OpenDocument(),
                resultCallback
            )
        }
    }

    fun buildCreateFile(fileType: String = "application/octet-stream", onFileCreated: (fileCreated: Uri?)->Unit) {
        val resultCallback = ActivityResultCallback<Uri?> { result ->
            onFileCreated.invoke(result)
        }

        createFileResultLauncher = if (fragment != null) {
            fragment?.registerForActivityResult(
                CreateDocument(fileType),
                resultCallback
            )
        } else {
            activity?.registerForActivityResult(
                CreateDocument(fileType),
                resultCallback
            )
        }
    }

    fun openFile(forContent: Boolean = false, fileType: String = "*/*") {
        try {
            if (forContent) {
                getContentResultLauncher?.launch(fileType)
            } else {
                openFileResultLauncher?.launch(arrayOf(fileType))
            }
        } catch (e: Exception) {
            Log.e(TAG, "Unable to open document", e)
            showFileManagerDialogFragment()
        }
    }

    fun createFile(title: String) {
        try {
            createFileResultLauncher?.launch(title)
        } catch (e: Exception) {
            Log.e(TAG, "Unable to create document", e)
            showFileManagerDialogFragment()
        }
    }

    /**
     * Show Browser dialog to select file picker app
     */
    private fun showFileManagerDialogFragment() {
        try {
            if (fragment != null) {
                fragment?.parentFragmentManager
            } else {
                activity?.supportFragmentManager
            }?.let { fragmentManager ->
                FileManagerDialogFragment().show(fragmentManager, "browserDialog")
            }
        } catch (e: Exception) {
            Log.e(TAG, "Can't open BrowserDialog", e)
        }
    }

    class OpenDocument : ActivityResultContracts.OpenDocument() {
        @SuppressLint("InlinedApi")
        override fun createIntent(context: Context, input: Array<String>): Intent {
            return super.createIntent(context, input).apply {
                addCategory(Intent.CATEGORY_OPENABLE)
                addFlags(Intent.FLAG_GRANT_PERSISTABLE_URI_PERMISSION)
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
                    addFlags(Intent.FLAG_GRANT_PREFIX_URI_PERMISSION)
                }
                addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION)
                addFlags(Intent.FLAG_GRANT_WRITE_URI_PERMISSION)
            }
        }
    }

    class GetContent : ActivityResultContracts.GetContent() {
        @SuppressLint("InlinedApi")
        override fun createIntent(context: Context, input: String): Intent {
            return super.createIntent(context, input).apply {
                addCategory(Intent.CATEGORY_OPENABLE)
                addFlags(Intent.FLAG_GRANT_PERSISTABLE_URI_PERMISSION)
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
                    addFlags(Intent.FLAG_GRANT_PREFIX_URI_PERMISSION)
                }
                addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION)
                addFlags(Intent.FLAG_GRANT_WRITE_URI_PERMISSION)
            }
        }
    }

    class CreateDocument(private val typeString: String) : ActivityResultContracts.CreateDocument() {
        override fun createIntent(context: Context, input: String): Intent {
            return super.createIntent(context, input).apply {
                addCategory(Intent.CATEGORY_OPENABLE)
                type = typeString
            }
        }
    }

}


fun View.setClickListenerForOpenFile(safHelper: SafHelper?) {
    safHelper?.let { helper ->
        setOnClickListener {
            helper.openFile(false)
        }
        setOnLongClickListener {
            helper.openFile(true)
            true
        }
    } ?: kotlin.run {
        setOnClickListener(null)
        setOnLongClickListener(null)
    }
}
