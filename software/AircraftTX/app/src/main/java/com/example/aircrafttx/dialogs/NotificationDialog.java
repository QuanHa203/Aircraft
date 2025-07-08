package com.example.aircrafttx.dialogs;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.app.Dialog;
import android.view.View;
import android.view.Window;
import android.widget.Button;
import android.widget.TextView;

import com.example.aircrafttx.R;

public class NotificationDialog {
    private final Activity _activity;
    private Dialog notifyDialog;
    private final TextView textViewNotifyText;
    private final TextView textViewNotifyTitle;
    private final Button btnConfirm;
    private final Button btnCancel;


    public NotificationDialog(Activity activity) {
        _activity = activity;
        View view = _activity.getLayoutInflater().inflate(R.layout.notification_dialog, null);
        textViewNotifyText = view.findViewById(R.id.textViewNotifyText);
        textViewNotifyTitle = view.findViewById(R.id.textViewNotifyTitle);
        btnConfirm = view.findViewById(R.id.btnConfirm);
        btnCancel = view.findViewById(R.id.btnCancel);

        btnConfirm.setOnClickListener(v -> {
            hideNotify();
        });

        btnCancel.setOnClickListener(v -> {
            hideNotify();
        });

        _activity.runOnUiThread(() -> {
            notifyDialog = new Dialog(_activity);
            notifyDialog.requestWindowFeature(Window.FEATURE_NO_TITLE);
            notifyDialog.setContentView(view);
            notifyDialog.setCancelable(false);
        });
    }

    @SuppressLint("InflateParams")
    public NotificationDialog(Activity activity, String titleText, String notifyText) {
        this(activity);
        textViewNotifyText.setText(notifyText);
        textViewNotifyTitle.setText(titleText);
    }

    public void setTitleText(String title) {
        textViewNotifyTitle.setText(title);
    }

    public void setNotifyText(String notify) {
        textViewNotifyText.setText(notify);
    }

    public void setBtnConfirmListener(View.OnClickListener l) {
        btnConfirm.setOnClickListener(v -> {
            l.onClick(v);
            hideNotify();
        });
    }

    public void setBtnCancelListener(View.OnClickListener l) {
        btnCancel.setOnClickListener(v -> {
            l.onClick(v);
            hideNotify();
        });
    }

    public void setBtnConfirmText(String text) {
        btnConfirm.setText(text);
    }

    public void setBtnCancelText(String text) {
        btnCancel.setText(text);
    }

    public void showNotify() {
        _activity.runOnUiThread(() -> {
            notifyDialog.show();
        });
    }

    private void hideNotify() {
        _activity.runOnUiThread(() -> {
            notifyDialog.cancel();
        });
    }
}