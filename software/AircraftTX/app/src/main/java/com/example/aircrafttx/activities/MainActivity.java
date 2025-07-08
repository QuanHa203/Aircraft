package com.example.aircrafttx.activities;

import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.IBinder;
import android.widget.Toast;

import androidx.activity.EdgeToEdge;
import androidx.appcompat.app.AppCompatActivity;

import com.example.aircrafttx.R;
import com.example.aircrafttx.services.TcpAircraftService;
import com.example.aircrafttx.socket.TcpClient;
import com.example.aircrafttx.view.JoystickView;

import java.io.IOException;
import java.net.InetAddress;
import java.net.SocketException;
import java.net.SocketTimeoutException;
import java.net.UnknownHostException;

public class MainActivity extends AppCompatActivity {
    private JoystickView joystickLeft;
    private JoystickView joystickRight;

    /// packet[[0]] = frameId;
    /// packet[[1]] = throttle;
    /// packet[[2]] = yaw;
    /// packet[[3]] = pitch;
    /// packet[[4]] = roll;
    /// packet[[5]] = flags;
    /// packet[[6]] = checksum;
    /// packet[[7]] = reserved
    private byte[] packet = new byte[8];

    private TcpAircraftService tcpAircraftService;
    private boolean isTcpAircraftServiceConnectionBound = false;
    private final ServiceConnection tcpAircraftServiceConnection = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName name, IBinder service) {
            TcpAircraftService.LocalBinder binder = (TcpAircraftService.LocalBinder)service;
            tcpAircraftService = binder.getService();

            tcpAircraftService.addOnReceiveListener(bufferReceive -> {

            });
        }

        @Override
        public void onServiceDisconnected(ComponentName name) {
            isTcpAircraftServiceConnectionBound = false;
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        EdgeToEdge.enable(this);
        setContentView(R.layout.activity_main);

        joystickLeft = findViewById(R.id.joystickLeft);
        joystickLeft.setEnableXAxis(false);
        joystickLeft.setEnableYAxis(true);
        joystickLeft.setAutoReturnYToCenter(false);

        joystickLeft.setJoystickListener((x, y) -> {
            float throttleNormalized = -y; // -1 to 1
            float yawNormalized = x; // -1 to 1
            byte throttle = normalizedValue(throttleNormalized);
            byte yaw = normalizedValue(yawNormalized);

            packet[0] = 0x00;
            packet[1] = throttle;
            packet[2] = yaw;

            packet[5] = 0x00;
            packet[6] = 0x00;
            packet[7] = 0x00;

            tcpAircraftService.send(packet);
        });

        joystickRight = findViewById(R.id.joystickRight);
        joystickRight.setEnableXAxis(true);
        joystickRight.setEnableYAxis(true);
        joystickRight.setAutoReturnXToCenter(true);
        joystickRight.setJoystickListener((x, y) -> {
            float pitchNormalized = -y;
            float rollNormalized = x;

            byte pitch = normalizedValue(pitchNormalized);
            byte roll  = normalizedValue(rollNormalized);

            packet[0] = 0x00;
            packet[3] = pitch;
            packet[4] = roll;
            packet[5] = 0x00;
            packet[6] = 0x00;
            packet[7] = 0x00;

            tcpAircraftService.send(packet);
        });

    }

    private byte normalizedValue(float normalized) {
        return (byte) ((normalized + 1f) * 127.6f);     // 127.6f to round up 255
    }

    @Override
    protected void onStart() {
        super.onStart();
        Intent intentDeviceControlService = new Intent(MainActivity.this, TcpAircraftService.class);
        startService(intentDeviceControlService);
        bindService(intentDeviceControlService, tcpAircraftServiceConnection, Context.BIND_AUTO_CREATE);
    }

    @Override
    protected void onStop() {
        super.onStop();
        tcpAircraftService.closeConnection();
        if (isTcpAircraftServiceConnectionBound) {
            unbindService(tcpAircraftServiceConnection);
            isTcpAircraftServiceConnectionBound = false;
        }
        this.finish();
    }
}