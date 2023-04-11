package com.kcci.petcare;

import android.app.DatePickerDialog;
import android.graphics.Color;
import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.fragment.app.Fragment;

import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.DatePicker;
import android.widget.TextView;

import com.github.mikephil.charting.charts.LineChart;
import com.github.mikephil.charting.components.Description;
import com.github.mikephil.charting.components.Legend;
import com.github.mikephil.charting.components.YAxis;
import com.github.mikephil.charting.data.Entry;
import com.github.mikephil.charting.data.LineData;
import com.github.mikephil.charting.data.LineDataSet;
import com.github.mikephil.charting.interfaces.datasets.ILineDataSet;

import java.util.ArrayList;
import java.util.Calendar;

/**
 * A simple {@link Fragment} subclass.
 * Use the {@link PetPulseFragment#newInstance} factory method to
 * create an instance of this fragment.
 */
public class PetPulseFragment extends Fragment {

    // TODO: Rename parameter arguments, choose names that match
    // the fragment initialization parameters, e.g. ARG_ITEM_NUMBER
    private static final String ARG_PARAM1 = "param1";
    private static final String ARG_PARAM2 = "param2";

    // TODO: Rename and change types of parameters
    private String mParam1;
    private String mParam2;

    private static String IP_ADDRESS = "10.10.141.32";
    private static String TAG = "phptest";

    private int yyst, ddst ,mmst;
    private int yyed, dded, mmed;

    private String startDateStr, endDateStr;

    private ArrayList<String> arrayList;

    private TextView textView, startDate, endDate;

    private Button btnStartDate, btnEndDate, btnGetData;

    private LineChart chart;

    private DataBaseThread dataBaseThread;
    private Handler databaseHandler;

    private Handler uiHandler;

    private Calendar calendar = Calendar.getInstance();

    private DatePickerDialog datePickerDialog;




    public PetPulseFragment() {
        // Required empty public constructor
    }

    /**
     * Use this factory method to create a new instance of
     * this fragment using the provided parameters.
     *
     * @param param1 Parameter 1.
     * @param param2 Parameter 2.
     * @return A new instance of fragment PetPulseFragment.
     */
    // TODO: Rename and change types and number of parameters
    public static PetPulseFragment newInstance(String param1, String param2) {
        PetPulseFragment fragment = new PetPulseFragment();
        Bundle args = new Bundle();
        args.putString(ARG_PARAM1, param1);
        args.putString(ARG_PARAM2, param2);
        fragment.setArguments(args);
        return fragment;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (getArguments() != null) {
            mParam1 = getArguments().getString(ARG_PARAM1);
            mParam2 = getArguments().getString(ARG_PARAM2);
        }
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_pet_pulse, container, false);
        // Inflate the layout for this fragment

        textView = view.findViewById(R.id.history_textview2);
        //textView.setMovementMethod(new ScrollingMovementMethod());
        startDate = view.findViewById(R.id.startdate2);
        endDate = view.findViewById(R.id.enddate2);

        btnGetData = view.findViewById(R.id.btnCheck2);
        btnStartDate = view.findViewById(R.id.btnStartDate2);
        btnEndDate = view.findViewById(R.id.btnEndDate2);
        chart = view.findViewById(R.id.LineChart2);
        init_graph();

        arrayList = new ArrayList<>();




        databaseHandler = new DatabaseHandler();
        uiHandler = new Handler();

        String url = "http://" + IP_ADDRESS + "/data_pet.php";


        btnGetData.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                //new Handler()
                dataBaseThread = new DataBaseThread(url,databaseHandler,startDateStr,endDateStr);
                dataBaseThread.setTime(startDateStr, endDateStr);
                dataBaseThread.start();
            }
        });

        btnStartDate.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Calendar mcurrentTime = Calendar.getInstance();
                int year = mcurrentTime.get(Calendar.YEAR);                //한국시간 : +9
                int month = mcurrentTime.get(Calendar.MONTH);
                int day = mcurrentTime.get(Calendar.MONTH);

                DatePickerDialog mDatePicker = new DatePickerDialog(getContext(), new DatePickerDialog.OnDateSetListener() {
                    @Override
                    public void onDateSet(DatePicker datePicker, int year, int month, int day) {
                        startDate.setText(" " + year + "년 " + (month+1) + "월" + day +"일");
                        yyst = year;
                        mmst = month+1;
                        ddst = day;
                        startDateStr = String.format("%4d-%02d-%02d",yyst,mmst,ddst);
                        //startDateStr = "" + String.valueOf(yyst) +"-" + String.valueOf(mmst)  +"-" +String.valueOf(ddst) ;
                    }
                },year,month,day);
                mDatePicker.setTitle("Select Time");
                mDatePicker.show();
            }
        });


        btnEndDate.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Calendar mcurrentTime = Calendar.getInstance();
                int year = mcurrentTime.get(Calendar.YEAR);                //한국시간 : +9
                int month = mcurrentTime.get(Calendar.MONTH);
                int day = mcurrentTime.get(Calendar.MONTH);

                DatePickerDialog mDatePicker = new DatePickerDialog(getContext(), new DatePickerDialog.OnDateSetListener() {
                    @Override
                    public void onDateSet(DatePicker datePicker, int year, int month, int day) {
                        endDate.setText(" " + year + "년 " + (month+1) + "월" + day +"일");
                        yyed = year;
                        mmed = month+1;
                        dded = day;
                        endDateStr = String.format("%4d-%02d-%02d",yyed,mmed,dded);
                        //endDateStr = "" + String.valueOf(yyed) +"-" + String.valueOf(mmed)  +"-" +String.valueOf(dded) ;

                    }
                },year,month,day);
//                TimePickerDialog mTimePicker;
//                mTimePicker = new TimePickerDialog(getContext(), new TimePickerDialog.OnTimeSetListener() {
//                    @Override
//                    public void onTimeSet(TimePicker timePicker, int selectedHour, int selectedMinute) {
//                        String state = "AM";
//                        // 선택한 시간이 12를 넘을경우 "PM"으로 변경 및 -12시간하여 출력 (ex : PM 6시 30분)
//                        if (selectedHour > 12) {
//                            selectedHour -= 12;
//                            state = "PM";
//                        }
//                        // EditText에 출력할 형식 지정
//                        startDate.setText(state + " " + selectedHour + "시 " + selectedMinute + "분");
//                    }
//                }, hour, minute, false); // true의 경우 24시간 형식의 TimePicker 출현
                mDatePicker.setTitle("Select Time");
                mDatePicker.show();
            }
        });

        return view;
    }

    private void init_graph(){
        chart.setDrawGridBackground(true);
        chart.setBackgroundColor(Color.BLACK);
        chart.setGridBackgroundColor(Color.BLACK);

// description text
        chart.getDescription().setEnabled(true);
        Description des = chart.getDescription();
        des.setEnabled(true);
        des.setText("Real-Time DATA");
        des.setTextSize(15f);
        des.setTextColor(Color.WHITE);

// touch gestures (false-비활성화)
        chart.setTouchEnabled(false);

// scaling and dragging (false-비활성화)
        chart.setDragEnabled(false);
        chart.setScaleEnabled(false);

//auto scale
        chart.setAutoScaleMinMaxEnabled(true);

// if disabled, scaling can be done on x- and y-axis separately
        chart.setPinchZoom(false);

//X축
        chart.getXAxis().setDrawGridLines(true);
        chart.getXAxis().setDrawAxisLine(false);

        chart.getXAxis().setEnabled(true);
        chart.getXAxis().setDrawGridLines(false);

//Legend
        Legend l = chart.getLegend();
        l.setEnabled(true);
        l.setFormSize(10f); // set the size of the legend forms/shapes
        l.setTextSize(12f);
        l.setTextColor(Color.WHITE);

//Y축
        YAxis leftAxis = chart.getAxisLeft();
        leftAxis.setEnabled(true);
        leftAxis.setTextColor(Color.WHITE);
        leftAxis.setDrawGridLines(true);
        leftAxis.setGridColor(Color.WHITE);

        YAxis rightAxis = chart.getAxisRight();
        rightAxis.setEnabled(false);


// don't forget to refresh the drawing
        chart.invalidate();
    }
    class DatabaseHandler extends Handler {
        @Override
        public void handleMessage(@NonNull Message msg) {
            super.handleMessage(msg);
            Bundle bundle = msg.getData();
            String data = bundle.getString("php");
            String[] splitData = data.split("PULSE:");
//            for (int i = 0; i < splitData.length; i++) {
//                Log.d(TAG, "handleMessage: "+ splitData[i]);
//            }
            float foodNum = Float.parseFloat(splitData[1]);
            updateGraph(foodNum);
            arrayList.add(data);
        }
    }


    private void updateGraph(float num) {
        new Thread(new Runnable() {
            @Override
            public void run() {
                databaseHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        Log.d(TAG, "run: " + num);
                        addEntry(num);
                    }
                });
            }
        }).start();
    }


    private void addEntry(float num) {

        LineData data = chart.getData();

        if (data == null) {
            data = new LineData();
            chart.setData(data);
        }

        ILineDataSet set = data.getDataSetByIndex(0);
        // set.addEntry(...); // can be called as well

        if (set == null) {
            set = createSet();
            data.addDataSet(set);
        }

        data.addEntry(new Entry((float)set.getEntryCount(), (float)num), 0);
        data.notifyDataChanged();

        // let the chart know it's data has changed
        chart.notifyDataSetChanged();

        chart.setVisibleXRangeMaximum(150);
        // this automatically refreshes the chart (calls invalidate())
        chart.moveViewTo(data.getEntryCount(), 50f, YAxis.AxisDependency.LEFT);

    }

    private LineDataSet createSet() {

        LineDataSet set = new LineDataSet(null, "Real-time Line Data");
        set.setLineWidth(1f);
        set.setDrawValues(false);
        set.setValueTextColor(Color.WHITE);
        set.setColor(Color.WHITE);
        set.setMode(LineDataSet.Mode.LINEAR);
        set.setDrawCircles(false);
        set.setHighLightColor(Color.rgb(190, 190, 190));

        return set;
    }
}