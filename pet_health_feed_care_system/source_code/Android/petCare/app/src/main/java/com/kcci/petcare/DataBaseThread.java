package com.kcci.petcare;

import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.URL;

public class DataBaseThread extends Thread {
    private static final String TAG = DataBaseThread.class.getSimpleName();
    private String serverURL;

    private HttpURLConnection httpURLConnection;

    private OutputStream outputStream;
    private InputStream inputStream;

    private Handler handlerDatabase = null;

    private String startDate, endDate;

    private boolean phpFlag = true;



    DataBaseThread(String url) {
        this.serverURL = url;
    }

    DataBaseThread(String url, Handler handler, String startDate,String endDate) {
        this.serverURL = url;
        this.handlerDatabase = handler;
        this.startDate = startDate;
        this.endDate = endDate;
    }

    DataBaseThread(String url, Handler handler) {
        this.serverURL = url;
        this.handlerDatabase = handler;
    }

    synchronized void setTime(String startDate,String endDate) {
        this.startDate = startDate;
        this.endDate = endDate;
        Log.d(TAG, "setTime: "+this.startDate+","+this.endDate);
    }

    @Override
    public void run() {
        super.run();

        URL url = null; // 주소가 저장된 변수를 이곳에 입력합니다.
        try {

            phpFlag = true;

            url = new URL(serverURL);
            httpURLConnection = (HttpURLConnection) url.openConnection();
            httpURLConnection.setReadTimeout(5000); //5초안에 응답이 오지 않으면 예외가 발생합니다.
            httpURLConnection.setConnectTimeout(5000); //5초안에 연결이 안되면 예외가 발생합니다.\
            httpURLConnection.setRequestMethod("POST"); //요청 방식을 POST로 합니다.
            httpURLConnection.connect();

            //sendToPhp(startDate,endDate);

            String postParameters = "start=" + startDate + "&end=" + endDate;
            OutputStream outputStream = httpURLConnection.getOutputStream();
            outputStream.write(postParameters.getBytes("UTF-8"));
            outputStream.flush();
            outputStream.close();

            Log.d(TAG, "run: "+"connect");
            InputStream inputStream;

            //while (phpFlag) {

                int responseStatusCode = httpURLConnection.getResponseCode();
                Log.d(TAG, "POST response code - " + responseStatusCode);

                if(responseStatusCode == HttpURLConnection.HTTP_OK) {
                    inputStream = httpURLConnection.getInputStream();
                    InputStreamReader inputStreamReader = new InputStreamReader(inputStream, "UTF-8");
                    BufferedReader bufferedReader = new BufferedReader(inputStreamReader);

                    //StringBuilder sb = new StringBuilder();
                    String line = null;

                    while((line = bufferedReader.readLine()) != null){
                        if (!line.equals("")) {
                            Log.d(TAG, "run: "+ line);
                            sendHistory(line);
                        }

                        //sb.append(line);
                    }

                    bufferedReader.close();
                    phpFlag = false;
                    inputStream.close();
                    httpURLConnection.disconnect();

                }
                else {
                    inputStream = httpURLConnection.getErrorStream();
                }
            //}

            //inputStream = httpURLConnection.getInputStream();



        } catch (MalformedURLException e) {
            Log.d(TAG, "exception run: "+ e.getMessage());
            throw new RuntimeException(e);
        } catch (IOException e) {
            Log.d(TAG, "exception run: "+ e.getMessage());
            throw new RuntimeException(e);
        }
    }

    synchronized void disconnectPhp(){
        if (httpURLConnection != null) {
            phpFlag = false;
        }
    }

    synchronized void sendToPhp(String startDate, String endDate) throws IOException {
        if (httpURLConnection != null) {
            Log.d(TAG, "sendToPhp: "+startDate+","+endDate);
            String postParameters = "start=" + startDate + "&end=" + endDate;
            OutputStream outputStream = httpURLConnection.getOutputStream();
            outputStream.write(postParameters.getBytes("UTF-8"));
            outputStream.flush();
            outputStream.close();
        }
    }



    synchronized void sendHistory(String text) {
        Message message = handlerDatabase.obtainMessage();
        Bundle bundle = new Bundle();
        bundle.putString("php",text);
        message.setData(bundle);
        handlerDatabase.sendMessage(message);
    }
}
