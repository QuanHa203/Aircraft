package com.example.aircrafttx.activities;

import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.net.ConnectivityManager;
import android.net.LinkAddress;
import android.net.LinkProperties;
import android.net.Network;
import android.net.NetworkCapabilities;
import android.os.Bundle;
import android.os.IBinder;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

import androidx.activity.EdgeToEdge;
import androidx.appcompat.app.AppCompatActivity;

import com.example.aircrafttx.R;
import com.example.aircrafttx.dialogs.NotificationDialog;
import com.example.aircrafttx.services.TcpAircraftService;

import java.net.Inet4Address;
import java.net.InetAddress;
import java.nio.charset.StandardCharsets;

public class LoginActivity extends AppCompatActivity {
    private EditText editTextLoginUserName;
    private EditText editTextLoginPassword;
    private Button btnLogin;

    private boolean isTcpAircraftServiceConnectionBound = false;
    private final ServiceConnection tcpAircraftServiceConnection = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName name, IBinder service) {
            NotificationDialog notifyDialog = new NotificationDialog(LoginActivity.this, "Thông báo", "Tài khoản và mật khẩu không chính xác");
            notifyDialog.setBtnCancelListener(v -> LoginActivity.this.finish());

            TcpAircraftService.LocalBinder binder = (TcpAircraftService.LocalBinder)service;
            tcpAircraftService = binder.getService();

            tcpAircraftService.addOnReceiveListener(bufferReceive -> {
                // UserName and password correct
                if (bufferReceive[0] == 0x01) {
                    Intent intent = new Intent(LoginActivity.this, MainActivity.class);
                    startActivity(intent);
                    LoginActivity.this.finish();
                }
                // UserName and password incorrect
                else if (bufferReceive[0] == 0x00){
                    notifyDialog.showNotify();
                }
            });
        }

        @Override
        public void onServiceDisconnected(ComponentName name) {
            isTcpAircraftServiceConnectionBound = false;
        }
    };
    private TcpAircraftService tcpAircraftService;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        EdgeToEdge.enable(this);
        setContentView(R.layout.activity_login);

        editTextLoginUserName = findViewById(R.id.editTextLoginUserName);
        editTextLoginPassword = findViewById(R.id.editTextLoginPassword);
        btnLogin = findViewById(R.id.btnLogin);

        btnLogin.setOnClickListener(view -> {
            String userName = editTextLoginUserName.getText().toString().trim();
            String password = editTextLoginPassword.getText().toString().trim();

            if(userName.isEmpty()) {
                Toast.makeText(this, "User Name không được bỏ trống", Toast.LENGTH_SHORT).show();
                return;
            }

            if(password.isEmpty()) {
                Toast.makeText(this, "Password không được bỏ trống", Toast.LENGTH_SHORT).show();
                return;
            }

            byte[] buffer = (userName + '\n' + password).getBytes();
            if (tcpAircraftService != null)
                tcpAircraftService.send(buffer);

        });
    }

    private boolean checkWiFiIpAddress() {
        NotificationDialog notifyDialog = new NotificationDialog(this, "Lỗi", "Vui lòng kết nối WiFi: ESP32_TX và tắt dữ liệu di động");
        notifyDialog.setBtnCancelListener(v -> this.finish());
        notifyDialog.setBtnConfirmListener(v -> this.finish());

        ConnectivityManager connectivityManager = (ConnectivityManager)this.getSystemService(CONNECTIVITY_SERVICE);
        if (connectivityManager == null)
            throw new RuntimeException("ConnectivityManager is null");

        Network activeNetwork = connectivityManager.getActiveNetwork();
        if (activeNetwork == null){
            notifyDialog.showNotify();
            return false;
        }

        NetworkCapabilities capabilities = connectivityManager.getNetworkCapabilities(activeNetwork);
        if (capabilities == null || !capabilities.hasTransport(NetworkCapabilities.TRANSPORT_WIFI)) {
            notifyDialog.showNotify();
            return false;
        }

        LinkProperties linkProperties = connectivityManager.getLinkProperties(activeNetwork);
        if (linkProperties == null) {
            notifyDialog.showNotify();
            return false;
        }

        for (LinkAddress linkAddress : linkProperties.getLinkAddresses()) {
            InetAddress address = linkAddress.getAddress();
            if (address instanceof Inet4Address && !address.isLoopbackAddress()) {
                String myIp = address.getHostAddress();
                if (myIp == null)
                    continue;

                if (myIp.startsWith("192.168.4."))
                    return true;
            }
        }

        notifyDialog.showNotify();
        return false;
    }

    @Override
    protected void onStart() {
        super.onStart();

        if (checkWiFiIpAddress()) {
            Intent intentDeviceControlService = new Intent(LoginActivity.this, TcpAircraftService.class);
            startService(intentDeviceControlService);
            bindService(intentDeviceControlService, tcpAircraftServiceConnection, Context.BIND_AUTO_CREATE);
        }
    }

    @Override
    protected void onStop() {
        super.onStop();
        if (isTcpAircraftServiceConnectionBound) {
            unbindService(tcpAircraftServiceConnection);
            isTcpAircraftServiceConnectionBound = false;
        }
    }
}