package com.ma.sailing.activities

import android.content.Context
import android.content.Intent
import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.webkit.WebResourceError
import android.webkit.WebResourceRequest
import android.webkit.WebView
import android.webkit.WebViewClient
import com.ma.sailing.databinding.ActivityMaWebBinding

class MaWebActivity : MaActivity() {
    private val binding: ActivityMaWebBinding by lazy(LazyThreadSafetyMode.NONE) {
        ActivityMaWebBinding.inflate(layoutInflater)
    }

    companion object {
        private const val WEB_VIEW_TITLE = "title"
        private const val WEB_VIEW_URL = "url"

        // Calling startActivity() from outside of an Activity context
        // requires the FLAG_ACTIVITY_NEW_TASK flag
        fun startWebActivity(context: Context, url: String, title: String, newTask: Boolean = false) {
            val intent  = Intent(context, MaWebActivity::class. java)
            intent.putExtra(WEB_VIEW_TITLE, title)
            intent.putExtra(WEB_VIEW_URL, url)
            if (newTask) {
                intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK)
            }
            context.startActivity(intent)
        }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(binding.root)

        initView(intent)
    }

    private fun initView(intent: Intent) {
        val title = intent.getStringExtra(WEB_VIEW_TITLE)
        val url = intent.getStringExtra(WEB_VIEW_URL);
        binding.webviewBack.setOnClickListener(View.OnClickListener {
            finish()
        })
        binding.webviewTitle.text = title
        binding.webviewContent.loadUrl(url ?: "")
        binding.webviewContent.webViewClient = object : WebViewClient() {
            override fun shouldOverrideUrlLoading(view: WebView, url: String?): Boolean {
                view.loadUrl(url ?: "")
                return true
            }

            override fun shouldOverrideUrlLoading(view: WebView, request: WebResourceRequest): Boolean {
                view.loadUrl(request.getUrl().toString())
                return true
            }

            override fun onPageFinished(view: WebView?, url: String?) {
                binding.webviewLoading.visibility = View.GONE;
            }

            override fun onReceivedError(view: WebView?, request: WebResourceRequest?, error: WebResourceError?) {
            }
        }
    }

}