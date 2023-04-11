package com.kcci.petcare;
import android.util.Log;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.InetSocketAddress;
import java.net.Socket;

public class ClientThread extends Thread {
    static String serverIp="222.111.174.191";
    static int serverPort=5000;
    static String clientId="11119999";
    static String clientPw="PASSWD";
    static Socket socket = null;
    static String arduinoId="[0]";
    ClientThread(String Ip,int Port, String Id, String Pw) {
         serverIp = Ip;
         serverPort = Port;
         clientId = Id;
         clientPw = Pw;

    }
    ClientThread(String cliId) {
        clientId = cliId;
    }
    @Override
    public void run() {
        try {
            socket = new Socket();
            displayText("[연결 요청]");
            Log.d("run()", "ip: " + serverIp + ",port: " +serverPort );
            socket.connect(new InetSocketAddress(serverIp, serverPort));
            displayText("[연결 성공]");
            Thread.sleep(100);
            sendData("[" + clientId + ":" + clientPw + "]");
            byte[] bytes = new byte[100];
            String message = "";
            InputStream is = socket.getInputStream();
            while(true) {
                int readByteCount = is.read(bytes);
                if(readByteCount <= 0)
                    break;
                message = new String(bytes, 0, readByteCount, "UTF-8");
                displayText("[데이터 받기 성공]: " + message);
                //sendMainActivity(message);
            }
            is.close();
        } catch (Exception e) {
            displayText("서버가 중지되었습니다");
        }

        if (socket != null && !socket.isClosed()) {
            try {
                socket.close();
            } catch (IOException e1) {
            }
        }
    }
    void stopClient() {
        if(socket != null && !socket.isClosed()) {
            displayText("클라이언트 중지");
            try {
                socket.close();
            } catch (IOException e) {
                throw new RuntimeException(e);
            }
        }
    }
    synchronized void sendData(String data) { // final data
        Thread sendThread = new Thread() {
            @Override
            public void run() {
                try {
                    byte[] bytes = (data+"\n").getBytes("UTF-8");
                    OutputStream os = socket.getOutputStream();
                    os.write(bytes);
                    os.flush();
                    displayText("데이터 보내기 성공");
                } catch (Exception e) {
                    displayText("서버를 확인하세요");
                }
            }
        };
        sendThread.start();
    }

    synchronized void displayText(String text) {
        Log.d("displayText",text);
    }
}
