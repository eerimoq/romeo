package qvist.com.romeo;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.Intent;
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
import java.util.Set;

import qvist.com.romeo.util.Constants;


public class BluetoothDevicePickerActivity extends Activity {

    // Used when logging
    private static final String TAG = "BluetoothDevicePicker";

    // Intent request codes
    private static final int REQUEST_ENABLE_BLUETOOTH = 1;

    private ArrayAdapter<String> mDevicesListAdapter;
    ArrayList<String> mDevicesListMacAddresses;

    // The bluetooth adapter
    private BluetoothAdapter mBluetoothAdapter;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_bluetooth_device_picker);

        // Get the local Bluetooth adapter
        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();

        if (mBluetoothAdapter == null) {
            Toast.makeText(this, "Bluetooth is not available.", Toast.LENGTH_SHORT).show();
            return;
        }

        // Create ArrayAdapter
        ArrayList<String> devices = new ArrayList<String>();
        mDevicesListAdapter = new ArrayAdapter<String>(this, R.layout.devices_list_row, devices);

        mDevicesListMacAddresses = new ArrayList<String>();

        // Get the devices list reference
        ListView devicesListView = (ListView)findViewById(R.id.devices_list);
        devicesListView.setAdapter(mDevicesListAdapter);

        // Add device item clicked listener
        devicesListView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            public void onItemClick(AdapterView<?> parent,
                                    View view,
                                    int position,
                                    long id) {
                Log.i(TAG, "Devices list item " + position
                        + " pressed (" + mDevicesListAdapter.getItem(position) + ").");
                passMacAddressToParent(mDevicesListMacAddresses.get(position));
            }
        });
    }

    @Override
    public void onStart() {
        super.onStart();

        // Request disabled bluetooth service to be enabled
        if (!mBluetoothAdapter.isEnabled()) {
            Log.i(TAG, "Bluetooth disabled. Send request to enable it.");
            Intent intent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(intent, REQUEST_ENABLE_BLUETOOTH);
        } else {
            populateDevicesList();
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_bluetooth_device_picket, menu);
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

    private void populateDevicesList() {
        // Get a set of currently paired devices
        Set<BluetoothDevice> mPairedDevices = mBluetoothAdapter.getBondedDevices();

        mDevicesListMacAddresses.clear();
        mDevicesListAdapter.clear();

        for (BluetoothDevice device : mPairedDevices) {
            mDevicesListMacAddresses.add(device.getAddress());
            mDevicesListAdapter.add(device.getName());
        }
    }

    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        switch (requestCode) {
            case REQUEST_ENABLE_BLUETOOTH:
                // When the request to enable Bluetooth returns
                if (resultCode == Activity.RESULT_OK) {
                    // Bluetooth is now enabled, so set up a chat session
                    Log.i(TAG, "Bluetooth enabled.");
                    populateDevicesList();
                } else {
                    // User did not enable Bluetooth or an error occurred
                    Log.d(TAG, "Bluetooth disabled");
                    Toast.makeText(this, "Bluetooth disabled", Toast.LENGTH_SHORT).show();
                    setResult(Activity.RESULT_CANCELED, null);
                    finish();
                }
        }
    }

    private void passMacAddressToParent(String macAddress) {
        Intent intent = new Intent(this, ControllerActivity.class);
        intent.putExtra(Constants.INTENT_EXTRA_MAC_ADDRESS, macAddress);
        setResult(Activity.RESULT_OK, intent);
        finish();
    }
}
