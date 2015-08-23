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

public class MainActivity extends ActionBarActivity {

    private final int RESULT_BLUETOOTH_DEVICE_PICKER = 0;
    private final int RESULT_PIN_CODE = 1;

    // Used when logging
    private static final String TAG = "MainActivity";
    private String mPinCode = "1234";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Log.d(TAG, "onCreate");

        if (savedInstanceState == null) {
            // start the bluetooth device picker
            Intent intent = new Intent(this, BluetoothDevicePickerActivity.class);
            startActivityForResult(intent, RESULT_BLUETOOTH_DEVICE_PICKER);
        }
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

    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        switch (requestCode) {
            case RESULT_BLUETOOTH_DEVICE_PICKER:
                if (resultCode == Activity.RESULT_OK) {
                    String macAddress = data.getExtras().getString(Constants.INTENT_EXTRA_MAC_ADDRESS);
                    Log.d(TAG, macAddress);

                    // start the bluetooth device service
                    BluetoothService bluetoothService = new BluetoothService(macAddress);

                    if (!bluetoothService.connect()) {
                        Log.d(TAG, "failed to connect to bluetooth device " + macAddress);
                        return;
                    }

                    RomeoApplication app = (RomeoApplication)getApplication();
                    app.mBluetoothService = bluetoothService;

                    // start the pin code dialog
                    Intent intent = new Intent(this, PinCodeActivity.class);
                    startActivityForResult(intent, RESULT_PIN_CODE);
                }
                break;

            case RESULT_PIN_CODE:
                if (resultCode == Activity.RESULT_OK) {
                    String pinCode = data.getExtras().getString(Constants.INTENT_EXTRA_PIN_CODE);

                    if (pinCode.equals(mPinCode)) {

                    } else {
                        Log.i(TAG, "wrong pin code '" + pinCode + "'");
                    }
                }
                break;
        }
    }

    public void onManualButtonClick(View view) {
        Intent intent = new Intent(this, ControllerActivity.class);
        startActivity(intent);
    }

    public void onAutomaticButtonClick(View view) {
    }

    public void onGraphsButtonClick(View view) {
    }
}
