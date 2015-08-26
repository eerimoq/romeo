package qvist.com.romeo;

import android.app.Activity;
import android.content.Intent;
import android.support.v7.app.ActionBarActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.Toast;

import java.util.ArrayList;

import qvist.com.romeo.util.Constants;

public class MainActivity extends ActionBarActivity {

    // Used when logging
    private static final String TAG = "MainActivity";

    private ArrayAdapter<String> mRobotsListAdapter;
    private ArrayList<String> mRobotsListData;
    private ArrayList<RobotCredentials> mRobots = new ArrayList<RobotCredentials>();

    private class RobotCredentials {

        public String mName;
        public String mBluetoothMacAddress;
        public String mPassword;

        RobotCredentials(String name, String bluetoothMacAddress, String password) {
            mName = name;
            mBluetoothMacAddress = bluetoothMacAddress;
            mPassword = password;
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Log.d(TAG, "onCreate");

        mRobots.add(new RobotCredentials("Evil", "20:15:07:02:07:36", "1234"));

        // Create ArrayAdapter
        ArrayList<String> robots = new ArrayList<String>();
        mRobotsListAdapter = new ArrayAdapter<String>(this, R.layout.robots_list_row, robots);

        for (RobotCredentials robot : mRobots) {
            mRobotsListAdapter.add(robot.mName);
        }

        // Get the devices list reference
        ListView robotsListView = (ListView)findViewById(R.id.robots_list);
        robotsListView.setAdapter(mRobotsListAdapter);

        // Add device item clicked listener
        robotsListView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            public void onItemClick(AdapterView<?> parent,
                                    View view,
                                    int position,
                                    long id) {
                // Connect to the robo
                RobotCredentials robot = mRobots.get(position);
                String macAddress = robot.mBluetoothMacAddress;
                Log.d(TAG, macAddress);

                // start the bluetooth device service
                if (bluetoothConnect(robot.mName, robot.mBluetoothMacAddress)) {
                    if (getBluetoothService().login("root", robot.mPassword)) {
                        Toast toast = Toast.makeText(getApplicationContext(), "Logged in", Toast.LENGTH_LONG);
                        toast.show();

                        // start the robot activity
                        Intent intent = new Intent(getBaseContext(), RobotActivity.class);
                        startActivity(intent);
                    } else {
                        Toast toast = Toast.makeText(getApplicationContext(), "Login failed", Toast.LENGTH_LONG);
                        toast.show();
                    }
                }
            }
        });
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    private boolean bluetoothConnect(String robotName, String macAddress) {
        boolean connected;
        String toastText;
        BluetoothService bluetoothService = new BluetoothService(macAddress);

        Toast toast = Toast.makeText(getApplicationContext(),
                "Connecting to " + robotName + " ...",
                Toast.LENGTH_LONG);
        toast.show();

        connected = bluetoothService.connect();

        if (!connected) {
            Log.d(TAG, "failed to connect to bluetooth device " + macAddress);
            toastText = "Connection failed";
        } else {
            Log.d(TAG, "Connected to " + macAddress);

            toastText = "Connected";

            RomeoApplication app = (RomeoApplication) getApplication();
            app.mBluetoothService = bluetoothService;
        }

        toast = Toast.makeText(getApplicationContext(), toastText, Toast.LENGTH_LONG);
        toast.show();

        return connected;
    }

    private BluetoothService getBluetoothService() {
        RomeoApplication app = (RomeoApplication) getApplication();
        return app.mBluetoothService;
    }
}
