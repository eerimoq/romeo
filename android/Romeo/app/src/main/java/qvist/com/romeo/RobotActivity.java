package qvist.com.romeo;

import android.app.Activity;
import android.content.Intent;
import android.support.v7.app.ActionBarActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;

import qvist.com.romeo.util.Constants;

public class RobotActivity extends ActionBarActivity {

    // Used when logging
    private static final String TAG = "RobotActivity";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_robot);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_robot, menu);
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

    public void onManualButtonClick(View view) {
        Intent intent = new Intent(this, ControllerActivity.class);
        startActivity(intent);
    }

    public void onAutomaticButtonClick(View view) {
        BluetoothService bluetoothService;

        RomeoApplication app = (RomeoApplication)getApplication();
        bluetoothService = app.mBluetoothService;
        bluetoothService.setHandler(null);

        bluetoothService.sendCommand("");
        bluetoothService.sendCommand("robot/stop");
        bluetoothService.sendCommand("robot/mode/set automatic");
        bluetoothService.sendCommand("robot/parameters/watchdog/enabled 0");
        bluetoothService.sendCommand("robot/start");
    }

    public void onGraphsButtonClick(View view) {
    }
}
