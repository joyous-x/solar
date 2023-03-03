package com.ma.sailing.dialogs

import android.app.Activity
import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.view.Window
import com.ma.sailing.R
import com.ma.sailing.databinding.DlgCommonLoadingBinding

class CommonLoadingDialog(
    private val activity: Activity,
    private val desc: String? = null,
    private val fullscreen: Boolean = false
) : CommonBaseDialog(activity, R.style.MaDialog_Base), View.OnClickListener {

    init {
        supportRequestWindowFeature(Window.FEATURE_NO_TITLE)
        setCancelable(false)
        setCanceledOnTouchOutside(false)
    }

    private val viewBinding: DlgCommonLoadingBinding by lazy {
        DlgCommonLoadingBinding.inflate(LayoutInflater.from(activity))
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(viewBinding.root)

        if (fullscreen) {
            // 需在 setContentView 后调用
            window?.setLayout(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT)
        }

        viewBinding.ivText.text = desc ?: ""
    }

    override fun onClick(p0: View?) {
        // TODO:
    }

}

