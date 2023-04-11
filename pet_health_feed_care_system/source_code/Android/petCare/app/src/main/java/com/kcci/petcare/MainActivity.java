package com.kcci.petcare;

import androidx.activity.result.ActivityResult;
import androidx.activity.result.ActivityResultCallback;
import androidx.activity.result.ActivityResultLauncher;
import androidx.activity.result.contract.ActivityResultContracts;
import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.Menu;
import android.view.MenuItem;
import android.webkit.WebView;
import android.widget.Button;
import android.widget.TextView;

import com.google.android.material.bottomnavigation.BottomNavigationView;

public class MainActivity extends AppCompatActivity {

    private static final String TAG = MainActivity.class.getSimpleName();


    private TextView textView;

    private String clientID;
    private String clientPW;
    private String serverIP;
    private String iotServerName = "[4]";
    private int serverPORT;

    static Handler clientHandler;
    static ClientThread clientThread;
    private ActivityResultLauncher<Intent> resultLauncher;

    private CameraFragment cameraFragment;
    private HistoryFragment historyFragment;
    private BottomNavigationView bottomNavigationView;
    private PetPulseFragment petPulseFragment;
    private TimerFragment timerFragment;






    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        cameraFragment = new CameraFragment();
        historyFragment = new HistoryFragment();
        petPulseFragment = new PetPulseFragment();
        timerFragment = new TimerFragment();

        getSupportFragmentManager().beginTransaction().replace(R.id.container, cameraFragment).commit();
        bottomNavigationView = findViewById(R.id.bottom_navigation);
        bottomNavigationView.setOnNavigationItemSelectedListener(new BottomNavigationView.OnNavigationItemSelectedListener() {
            @Override
            public boolean onNavigationItemSelected(@NonNull MenuItem item) {
                switch (item.getItemId()) {
                    case R.id.tab1:
                        getSupportFragmentManager().beginTransaction().replace(R.id.container,cameraFragment).commit();
                        return true;

                    case R.id.tab2:
                        getSupportFragmentManager().beginTransaction().replace(R.id.container,historyFragment).commit();
                        return true;
                    case R.id.tab3:
                        getSupportFragmentManager().beginTransaction().replace(R.id.container,petPulseFragment).commit();
                        return true;
                    case R.id.tab4:
                        getSupportFragmentManager().beginTransaction().replace(R.id.container,timerFragment).commit();
                        return true;

                }

                return false;
            }
        });

        resultLauncher = registerForActivityResult(new ActivityResultContracts.StartActivityForResult(), new ActivityResultCallback<ActivityResult>() {
            @Override
            public void onActivityResult(ActivityResult result) {
                if (result.getResultCode() == RESULT_OK) {
                    Intent data = result.getData();
                    clientID = data.getStringExtra("clientId");
                    clientPW = data.getStringExtra("clientPw");
                    serverIP = data.getStringExtra("serverIp");
                    serverPORT = data.getIntExtra("serverPort", 5000);
                    if (clientThread == null) {
                        clientThread = new ClientThread(serverIP, serverPORT, clientID, clientPW);
                        clientThread.start();
                    } else {

                    }


                }
            }
        });

        clientHandler = new ClientHandler();

    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.setting_menu, menu);
        return super.onCreateOptionsMenu(menu);
    }

    @Override
    public boolean onOptionsItemSelected(@NonNull MenuItem item) {
        switch (item.getItemId()) {
            case R.id.login:
                Intent intent = new Intent(MainActivity.this,LoginActivity.class);
//                intent.putExtra("serverIp",ClientThread.serverIp);
//                intent.putExtra("serverPort",ClientThread.serverPort);
//                intent.putExtra("clientId",ClientThread.clientId);
//                intent.putExtra("clientPw",ClientThread.clientPw);
                resultLauncher.launch(intent);
                break;
            case R.id.db_delete:

                break;
            case R.id.setting:
                break;
        }


        return super.onOptionsItemSelected(item);
    }

    class ClientHandler extends Handler {
        @Override
        public void handleMessage(@NonNull Message msg) {
            super.handleMessage(msg);
            Bundle bundle = msg.getData();
            String data = bundle.getString("msg");
        }
    }
}