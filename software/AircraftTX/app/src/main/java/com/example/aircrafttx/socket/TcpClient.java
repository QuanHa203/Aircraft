package com.example.aircrafttx.socket;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.SocketException;
import java.net.SocketTimeoutException;
import java.nio.channels.IllegalBlockingModeException;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.Callable;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.concurrent.LinkedBlockingQueue;

public class TcpClient {
    private final InetSocketAddress _inetSocketAddress;
    private final Socket _socket;

    private final BlockingQueue<byte[]> _bufferQueueSender = new LinkedBlockingQueue<>();
    private final ExecutorService _executorService;

    private InputStream _inputStream;
    private OutputStream _outputStream;

    private OnMessageReceive _onMessageReceive;

    public TcpClient(InetAddress inetAddress, int port) {
        _inetSocketAddress = new InetSocketAddress(inetAddress, port);
        _executorService = Executors.newFixedThreadPool(2);
        _socket = new Socket();
    }

    public void setTcpNoDelay(boolean on) throws SocketException {
        _socket.setTcpNoDelay(on);
    }

    public void connect() throws  IOException, SocketTimeoutException, IllegalBlockingModeException, IllegalArgumentException {
        Callable<Void> task = () -> {
            _socket.connect(_inetSocketAddress, 3000);

            _inputStream = _socket.getInputStream();
            _outputStream = _socket.getOutputStream();

            sendMessageThread();
            receiveMessageThread();
            return null;
        };

        Future<?> future = _executorService.submit(task);

        try {
            future.get();
        } catch (ExecutionException | InterruptedException e) {
            Throwable cause = e.getCause();

            if (cause instanceof SocketTimeoutException)
                throw (SocketTimeoutException) cause;
            else if (cause instanceof IllegalBlockingModeException)
                throw (IllegalBlockingModeException) cause;
            else if (cause instanceof IllegalArgumentException)
                throw (IllegalArgumentException) cause;
            else if (cause instanceof IOException)
                throw (IOException) cause;
            else
                throw new IOException("Unknown error during connect", cause);
        }
    }

    public void send(byte[] buffer) {
        try {
            _bufferQueueSender.put(buffer);
        }
        catch (InterruptedException e) {
            throw new RuntimeException(e);
        }
    }

    public void onMessageReceive(OnMessageReceive onMessageReceive) {
        _onMessageReceive = onMessageReceive;
    }

    public void closeConnection() {
        try {
            _socket.close();
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

    private void sendMessageThread() {
        _executorService.execute(() -> {
            byte[] buffer;

            //noinspection InfiniteLoopStatement
            while (true) {
                try {
                    buffer = _bufferQueueSender.take();
                    _outputStream.write(buffer);
                    _outputStream.flush();
                }
                catch (InterruptedException | IOException e) {
                    throw new RuntimeException(e);
                }
            }
        });
    }

    private void receiveMessageThread() {
        _executorService.execute(() -> {
            byte[] bufferReceive = new byte[128];

            //noinspection InfiniteLoopStatement
            while (true) {
                try {
                    if(_inputStream.available() > 0) {
                        //noinspection ResultOfMethodCallIgnored
                        _inputStream.read(bufferReceive, 0,  bufferReceive.length);
                        _onMessageReceive.onMessageReceive(bufferReceive);
                    }
                } catch (IOException e) {
                    throw new RuntimeException(e);
                }
            }
        });
    }
}
