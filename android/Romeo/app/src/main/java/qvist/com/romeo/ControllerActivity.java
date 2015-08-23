package qvist.com.romeo;

import qvist.com.romeo.util.Constants;
import qvist.com.romeo.util.SystemUiHider;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.Vibrator;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnTouchListener;
import android.widget.Button;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.Toast;

import org.w3c.dom.Text;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.UUID;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.regex.Matcher;
import java.util.regex.Pattern;


/**
 * An example full-screen activity that shows and hides the system UI (i.e.
 * status bar and navigation/system bar) with user interaction.
 *
 * @see SystemUiHider
 */
public class ControllerActivity extends Activity {

    // Used when logging
    private static final String TAG = "ControllerActivity";
    private static final UUID ROMEO_CONTROLLER_UUID_INSECURE =
            UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");
    final protected static char[] hexArray = "0123456789ABCDEF".toCharArray();
    private static final int VIBRATE_TIME_MS = 5;

    private static ControllerThread mControllerThread;
    private BluetoothAdapter mBluetoothAdapter;
    private int mSpeed = 0;
    private int mAngularVelocity = 0;
    private SeekBar mAngularVelocitySeekbar;
    private SeekBar mSpeedSeekbar;
    private Vibrator mVibrator;
    private Button mSelectButton;
    private Button mStartButton;
    private TextView mStatusText;
    private boolean mStarted = false;

    private void initButtons() {
        mSelectButton = (Button)findViewById(R.id.select_button);
        mSelectButton.setOnTouchListener(new OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                switch ( event.getAction() ) {
                    case MotionEvent.ACTION_DOWN:
                        mVibrator.vibrate(VIBRATE_TIME_MS);
                        break;
                    case MotionEvent.ACTION_UP:
                        Log.d(TAG, "Select button clicked");
                        mVibrator.vibrate(VIBRATE_TIME_MS);
                        break;
                }

                return true;
            }
        });

        mSelectButton.setEnabled(false);

        mStartButton = (Button)findViewById(R.id.start_button);
        mStartButton.setOnTouchListener(new OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                switch ( event.getAction() ) {
                    case MotionEvent.ACTION_DOWN:
                        mVibrator.vibrate(VIBRATE_TIME_MS);
                        break;
                    case MotionEvent.ACTION_UP:
                        Log.d(TAG, "Start button clicked");
                        mVibrator.vibrate(VIBRATE_TIME_MS);
                        if (mStarted) {
                            mControllerThread.sendCommand("robot/stop");
                            mStarted = false;
                        } else {
                            mControllerThread.sendCommand("robot/start");
                            mStarted = true;
                        }
                        break;
                }

                return true;
            }
        });

        mStartButton.setEnabled(false);
    }

    private void initSeekbars() {
        mAngularVelocitySeekbar = (SeekBar)findViewById(R.id.angular_velocity_seekbar);
        mAngularVelocitySeekbar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                // 'progress' is 0 - 100. Move it to the range -100 - +100.
                mAngularVelocity = 2 * (progress - 50);
                mControllerThread.setMovement(mSpeed, mAngularVelocity);
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
                mVibrator.vibrate(VIBRATE_TIME_MS);
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                mAngularVelocity = 0;
                mControllerThread.setMovement(mSpeed, mAngularVelocity);
                mAngularVelocitySeekbar.setProgress(50);
                mVibrator.vibrate(VIBRATE_TIME_MS);
            }
        });
        mAngularVelocitySeekbar.setEnabled(false);

        mSpeedSeekbar = (SeekBar)findViewById(R.id.speed_seekbar);
        mSpeedSeekbar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                // 'progress' is 0 - 100. Move it to the range -100 - +100.
                mSpeed = 2 * (progress - 50);
                mControllerThread.setMovement(mSpeed, mAngularVelocity);
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
                mVibrator.vibrate(VIBRATE_TIME_MS);
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                mSpeed = 0;
                mControllerThread.setMovement(mSpeed, mAngularVelocity);
                mSpeedSeekbar.setProgress(50);
                mVibrator.vibrate(VIBRATE_TIME_MS);
            }
        });

        mSpeedSeekbar.setEnabled(false);
    }

    private void initStatusText() {
        mStatusText = (TextView)findViewById(R.id.status_text);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_controller);
        getActionBar().hide();

        mVibrator = (Vibrator)this.getSystemService(Context.VIBRATOR_SERVICE);

        initSeekbars();
        initButtons();
        initStatusText();

        // Get the bluetooth device mac address from the intent passed to startActivity()
        Intent intent = getIntent();
        String macAddress = intent.getStringExtra(Constants.INTENT_EXTRA_MAC_ADDRESS);

        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        BluetoothDevice device = mBluetoothAdapter.getRemoteDevice(macAddress);

        Log.d(TAG, "Connecting to bluetooth device " + device);

        // Start controller thread
        mControllerThread = new ControllerThread(device);
        mControllerThread.start();
    }

    @Override
    protected void onDestroy() {
        Log.d(TAG, "onDestroy");
        mControllerThread.kill();
        super.onDestroy();
    }

    private final Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case Constants.MESSAGE_CONNECTED_TO_DEVICE:
                    mAngularVelocitySeekbar.setEnabled(true);
                    mSpeedSeekbar.setEnabled(true);
                    mSelectButton.setEnabled(true);
                    mStartButton.setEnabled(true);
                    break;
                case Constants.MESSAGE_STATUS_TEXT_UPDATE:
                    mStatusText.setText((String)msg.obj);
                    break;
            }
        }
    };

    public static String bytesToHex(byte[] bytes) {
        char[] hexChars = new char[bytes.length * 2];
        for ( int j = 0; j < bytes.length; j++ ) {
            int v = bytes[j] & 0xFF;
            hexChars[j * 2] = hexArray[v >>> 4];
            hexChars[j * 2 + 1] = hexArray[v & 0x0F];
        }
        return new String(hexChars);
    }

    private class ControllerThread extends Thread {
        private final BluetoothDevice mDevice;
        private BluetoothSocket mSocket;
        private InputStream mInputStream = null;
        private OutputStream mOutputStream = null;
        private String mMovementMessage = null;
        private String mCommandMessage = null;
        private boolean mRunning = true;
        private final Pattern re_perimeter_signal_level =
                Pattern.compile("^perimeter signal level = (.*)$");
        private final Pattern re_battery_energy_level =
                Pattern.compile("^battery energy level = ([^%]+)%$");

        public ControllerThread(BluetoothDevice device) {
            mDevice = device;

            try {
                mSocket = device.createInsecureRfcommSocketToServiceRecord(
                        ROMEO_CONTROLLER_UUID_INSECURE);
            } catch (IOException e) {
                Log.e(TAG, "failed to create bluetooth socket", e);
            }

        }

        private void notifyActivity(int message_id, String message) {
            Message msg = mHandler.obtainMessage(message_id);
            msg.obj = message;
            mHandler.sendMessage(msg);
        }

        private void init() {
            // Connect to the bluetooth device
            try {
                mBluetoothAdapter.cancelDiscovery();

                // This is a blocking call and will only return on a
                // successful connection or an exception
                mSocket.connect();
                mInputStream = mSocket.getInputStream();
                mOutputStream = mSocket.getOutputStream();
            } catch (IOException e) {
                Log.e(TAG, "Failed to connect to bluetooth device.", e);
                return;
            }

            // Send the empty string to get a prompt
            executeCommand("");
            executeCommand("robot/stop");
            executeCommand("robot/mode/set manual");
            executeCommand("robot/parameters/watchdog/enabled 0");

            notifyActivity(Constants.MESSAGE_CONNECTED_TO_DEVICE, null);
        }

        @Override
        public void run() {
            int statusCount = -1;

            init();

            // Handle one command at a time
            while (mRunning) {
                try {
                    // Sleep robot tick period time
                    sleep(100);
                } catch (InterruptedException e) {
                    return;
                }

                statusCount++;

                if ((statusCount % 10) == 0) {
                    status();
                }

                String message = getMovement();

                if (message != null) {
                    executeCommand(message);
                }

                message = getCommand();

                if (message != null) {
                    executeCommand(message);
                }
            }

            try {
                mSocket.close();
            } catch (IOException e) {
            }
        }

        private void status() {
            String battery_energy_level = null;
            int perimeter_signal_level = 0;
            Matcher matcher;
            String statusText;

            String command_output = executeCommand("robot/status");

            for (String line : command_output.split("[\\r\\n]+")) {
                matcher = re_perimeter_signal_level.matcher(line);

                if (matcher.find()) {
                    perimeter_signal_level = Float.valueOf(matcher.group(1)).intValue();
                    continue;
                }

                matcher = re_battery_energy_level.matcher(line);

                if (matcher.find()) {
                    battery_energy_level = matcher.group(1);
                    continue;
                }
            }

            statusText = "p: " + perimeter_signal_level + ", b: " + battery_energy_level + "%";

            notifyActivity(Constants.MESSAGE_STATUS_TEXT_UPDATE, statusText);
        }

        public void kill() {
            mRunning = false;
        }

        public synchronized void setMovement(int speed, int angularVelocity) {
            String command = "robot/manual/movement/set " + speed + " " + angularVelocity;
            mMovementMessage = command;
        }

        public synchronized String getMovement() {
            String message = mMovementMessage;
            mMovementMessage = null;
            return message;
        }

        public synchronized void sendCommand(String command) {
            mCommandMessage = command;
        }

        public synchronized String getCommand() {
            String message = mCommandMessage;
            mCommandMessage = null;
            return message;
        }

        private String waitForPrompt() {
            int data;
            String message = new String();

            while (true) {
                try {
                    data = mInputStream.read();

                    message += Character.toString((char)data);

                    // Prompt found?
                    if (message.endsWith("$ ")) {
                        Log.d(TAG, "Prompt found.");
                        return message.substring(0, message.length() - 2);
                    }
                } catch (IOException e) {
                    Log.e(TAG, "Failed to read from bluetooth device");
                    return null;
                }
            }
        }

        private String executeCommand(String command) {
            command += "\r\n";
            Log.d(TAG, "Writing command: " + command);
            write(command.getBytes());
            return waitForPrompt();
        }

        private void write(byte[] data) {
            try {
                Log.d(TAG, "Writing '" + bytesToHex(data) + "' to output socket.");
                mOutputStream.write(data);
            } catch (IOException e){
                Log.e(TAG, "Exception during write", e);
            }
        }
    }
}
