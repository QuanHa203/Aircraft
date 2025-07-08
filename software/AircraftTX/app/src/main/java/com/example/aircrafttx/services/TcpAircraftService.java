package com.example.aircrafttx.services;

import android.app.Service;
import android.content.Intent;
import android.os.Binder;
import android.os.IBinder;
import android.widget.Toast;

import androidx.annotation.Nullable;

import com.example.aircrafttx.socket.OnMessageReceive;
import com.example.aircrafttx.socket.TcpClient;

import java.io.IOException;
import java.net.InetAddress;
import java.net.SocketException;
import java.net.SocketTimeoutException;
import java.net.UnknownHostException;

public class TcpAircraftService extends Service {
    public class LocalBinder extends Binder {
        public TcpAircraftService getService() {
            return TcpAircraftService.this;
        }
    }


    private final IBinder _binder = new LocalBinder();
    private final String host = "192.168.4.1";
    private final int port = 2003;
    private TcpClient tcpClient;

    @Override
    public void onCreate() {
        super.onCreate();
        connectToTcpServer();
    }

    @Nullable
    @Override
    public IBinder onBind(Intent intent) {
        return _binder;
    }

    public void send(byte[] buffer) {
        tcpClient.send(buffer);
    }

    public void closeConnection() {
        tcpClient.closeConnection();
    }

    public void addOnReceiveListener(OnMessageReceive onMessageReceive) {
        tcpClient.onMessageReceive(onMessageReceive);
    }

    private void connectToTcpServer() {
        try {
            tcpClient = new TcpClient(InetAddress.getByName(host), port);
            tcpClient.setTcpNoDelay(true);
            tcpClient.connect();

            tcpClient.onMessageReceive((bufferReceive) -> {
                Toast.makeText(this, new String(bufferReceive), Toast.LENGTH_SHORT).show();
            });
        }
        catch (UnknownHostException e) {
            Toast.makeText(this, e.getMessage(), Toast.LENGTH_SHORT).show();
        }
        catch (SocketTimeoutException e) {
            Toast.makeText(this, e.getMessage(), Toast.LENGTH_SHORT).show();
        }
        catch (SocketException e) {
            Toast.makeText(this, e.getMessage(), Toast.LENGTH_SHORT).show();
        }
        catch (IOException e) {
            Toast.makeText(this, e.getMessage(), Toast.LENGTH_SHORT).show();
        }
    }
}
