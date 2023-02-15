package com.ma.sailing.dialogs

import android.app.Dialog
import android.os.Bundle
import android.widget.Button
import android.widget.TextView
import androidx.appcompat.app.AlertDialog
import androidx.fragment.app.DialogFragment
import com.ma.sailing.R
import com.ma.sailing.common.UriUtil

class FileManagerDialogFragment : DialogFragment() {

    override fun onCreateDialog(savedInstanceState: Bundle?): Dialog {
        activity?.let { activity ->
            val builder = AlertDialog.Builder(activity)
            // Get the layout inflater
            val root = activity.layoutInflater.inflate(R.layout.fragment_browser_install, null)
            builder.setView(root)
                .setNegativeButton(android.R.string.cancel) { _, _ -> }

            val textDescription = root.findViewById<TextView>(R.id.file_manager_install_description)
            textDescription.text = getString(R.string.file_manager_install_description)

            root.findViewById<Button>(R.id.file_manager_button).setOnClickListener {
                UriUtil.gotoUrl(requireContext(), R.string.file_manager_explanation_url)
                dismiss()
            }

            return builder.create()
        }
        return super.onCreateDialog(savedInstanceState)
    }
}
