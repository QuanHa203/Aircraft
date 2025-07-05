package com.example.aircrafttx;

import android.os.Bundle;
import android.util.Log;
import android.widget.Toast;

import androidx.activity.EdgeToEdge;
import androidx.appcompat.app.AppCompatActivity;

import com.example.aircrafttx.socket.TcpClient;
import com.example.aircrafttx.view.JoystickView;

import java.io.IOException;
import java.net.InetAddress;

public class MainActivity extends AppCompatActivity {
    private TcpClient tcpClient;
    private JoystickView joystickLeft;
    private JoystickView joystickRight;
    private byte frameId = 0;

    /// packet[[0]] = frameId;
    /// packet[[1]] = throttle;
    /// packet[[2]] = yaw;
    /// packet[[3]] = pitch;
    /// packet[[4]] = roll;
    /// packet[[5]] = flags;
    /// packet[[6]] = checksum;
    /// packet[[7]] = reserved
    private byte[] packet = new byte[8];

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

            packet[0] = frameId;
            packet[1] = throttle;
            packet[2] = yaw;

            packet[5] = 0x00;
            packet[6] = 0x00;
            packet[7] = 0x00;

            tcpClient.send(packet);
            frameId++;
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

            packet[0] = frameId;
            packet[3] = pitch;
            packet[4] = roll;
            packet[5] = 0x00;
            packet[6] = 0x00;
            packet[7] = 0x00;

            tcpClient.send(packet);
            frameId++;
        });
        connectToTcpServer();
    }

    private void connectToTcpServer() {
        String host = "192.168.1.6";
        int port = 2003;

        try {
            tcpClient = new TcpClient(InetAddress.getByName(host), port);
            tcpClient.setTcpNoDelay(true);
            tcpClient.connect();

            tcpClient.onMessageReceive((bufferReceive) -> {
                Toast.makeText(this, new String(bufferReceive), Toast.LENGTH_SHORT).show();
            });
        }
        catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

    private byte normalizedValue(float normalized) {
        return (byte) ((normalized + 1f) * 127.6f);     // 127.6f to round up 255
    }
}