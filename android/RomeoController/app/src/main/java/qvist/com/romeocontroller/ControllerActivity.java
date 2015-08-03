package qvist.com.romeocontroller;

import qvist.com.romeocontroller.util.Constants;
import qvist.com.romeocontroller.util.SystemUiHider;

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
import android.widget.SeekBar;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.UUID;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;


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

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_controller);
        getActionBar().hide();

        mVibrator = (Vibrator)this.getSystemService(Context.VIBRATOR_SERVICE);

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

        mAngularVelocitySeekbar.setEnabled(false);
        mSpeedSeekbar.setEnabled(false);

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
        private byte EMTP_MESSAGE_BEGIN = 0x10;
        private byte[] PING_MESSAGE = { EMTP_MESSAGE_BEGIN, 0x00, 0x00, 0x04};

        // Data received from the bluetooth device
        private BlockingQueue<Integer> mInputQueue = new LinkedBlockingQueue<Integer>();

        // Data to send to the bluetooth device
        private BlockingQueue<String> mOutputQueue = new LinkedBlockingQueue<String>();

        private ReaderThread mReaderThread;
        private PingerThread mPingerThread;
        private String mMovementMessage = null;
        private boolean mRunning = true;

        public ControllerThread(BluetoothDevice device) {
            mDevice = device;

            try {
                mSocket = device.createInsecureRfcommSocketToServiceRecord(
                        ROMEO_CONTROLLER_UUID_INSECURE);
            } catch (IOException e) {
                Log.e(TAG, "failed to create bluetooth socket", e);
            }

        }

        public void run() {
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

            // Start reader thread
            mReaderThread = new ReaderThread();
            mReaderThread.start();

            // Start pinger thread
            mPingerThread = new PingerThread();
            //mPingerThread.start();

            // Setup
            executeCommand("robot/stop");
            executeCommand("robot/mode/set manual");
            executeCommand("robot/parameters/watchdog/enabled 0");
            executeCommand("robot/start");

            // Notify the Activity of successful connection
            Message msg = mHandler.obtainMessage(Constants.MESSAGE_CONNECTED_TO_DEVICE);
            mHandler.sendMessage(msg);

            // Handle one command at a time
            while (mRunning) {
                try {
                    // Sleep robot tick period time
                    sleep(100);
                } catch (InterruptedException e) {
                    return;
                }

                String message = getMovement();

                if (message != null) {
                    executeCommand(message);
                }
            }

            //mPingerThread.kill();
            try {
                mSocket.close();
            } catch (IOException e) {
            }
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

        private void waitForPrompt() {
            int data;
            String message = new String();

            while (true) {
                try {
                    data = mInputQueue.take();

                    message += Character.toString((char)data);

                    // Prompt found?
                    if (message.endsWith("$ ")) {
                        Log.d(TAG, "Prompt found.");
                        break;
                    }
                } catch (InterruptedException e) {
                    Log.e(TAG, "Wait for prompt interrupted");
                    return;
                }
            }
        }

        private void executeCommand(String command) {
            command += "\r\n";
            Log.d(TAG, "Writing command: " + command);
            write(command.getBytes());
            waitForPrompt();
        }

        private void write(byte[] data) {
            try {
                Log.d(TAG, "Writing '" + bytesToHex(data) + "' to output socket.");
                mOutputStream.write(data);
            } catch (IOException e){
                Log.e(TAG, "Exception during write", e);
            }
        }

        private class PingerThread extends Thread {

            private boolean mRunning = true;

            public PingerThread() {
            }

            public void run() {
                Log.d(TAG, "Pinger thread started.");

                // send ping periodically
                while (mRunning) {
                    try {
                        sleep(1000);
                    } catch (InterruptedException e) {
                        return;
                    }

                    write(PING_MESSAGE);
                }
            }

            public void kill() {
                mRunning = false;
            }

        }

        private class ReaderThread extends Thread {

            public ReaderThread() {
            }

            public void run() {
                Log.d(TAG, "Reader thread started.");

                while (true) {
                    int data;

                    try {
                        data = mInputStream.read();

                        if (data == EMTP_MESSAGE_BEGIN) {
                            // Ignore EMTP message
                            int message_type = mInputStream.read();
                            int message_size = (mInputStream.read() << 8) + mInputStream.read();
                            Log.d(TAG, "Reading EMTP message type " + message_type + " of size " + message_size);
                            int data_size = message_size - 4;

                            // read all data
                            for(int i = 0; i < data_size; i++) {
                                mInputStream.read();
                            }
                        } else {
                            try {
                                mInputQueue.put(data);
                            } catch (InterruptedException e) {
                                Log.e(TAG, "Unable to put to input queue.");
                            }
                        }
                    } catch (IOException e) {
                        Log.e(TAG, "Reading exception. Breaking.", e);
                        break;
                    }
                }
            }
        }
    }
}
