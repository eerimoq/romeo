package qvist.com.romeo;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.widget.Toast;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.LinkedList;
import java.util.UUID;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import qvist.com.romeo.util.Constants;

/**
 * Created by erik on 2015-08-23.
 *
 * Singleton class for bluetooth communication.
 *
 */
public class BluetoothService {

    // Used when logging
    private static final String TAG = "BluetoothService";

    private static final UUID ROMEO_CONTROLLER_UUID_INSECURE =
            UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");

    private String mMacAddress;
    private ControllerThread mControllerThread;
    private BluetoothAdapter mBluetoothAdapter;
    private Handler mHandler = null;
    private BluetoothDevice mDevice;
    private BluetoothSocket mSocket;
    private InputStream mInputStream = null;
    private OutputStream mOutputStream = null;

    public BluetoothService(String macAddress) {
        mMacAddress = macAddress;
    }

    public boolean connect() {
        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        mDevice = mBluetoothAdapter.getRemoteDevice(mMacAddress);

        // Connect to the bluetooth device
        try {
            mBluetoothAdapter.cancelDiscovery();

            mSocket = mDevice.createInsecureRfcommSocketToServiceRecord(
                    ROMEO_CONTROLLER_UUID_INSECURE);

            // This is a blocking call and will only return on a
            // successful connection or an exception
            mSocket.connect();
            mInputStream = mSocket.getInputStream();
            mOutputStream = mSocket.getOutputStream();
        } catch (IOException e) {
            Log.e(TAG, "Failed to connect to bluetooth device.", e);
            return false;
        }

        return true;
    }

    public void disconnect() {
    }

    public boolean login(String username, String password) {
        boolean result;

        // Start controller thread
        mControllerThread = new ControllerThread(mDevice, username, password);
        result = mControllerThread.login();

        if (result) {
            mControllerThread.start();
        }

        return result;
    }

    public void logout() {
        mControllerThread.kill();
    }

    public void setHandler(Handler handler) {
        mHandler = handler;
    }

    public void sendCommand(String command) {
        mControllerThread.sendCommand(command);
    }

    public void setMovement(int speed, int angularVelocity) {
        mControllerThread.setMovement(speed, angularVelocity);
    }

    private class ControllerThread extends Thread {
        private BluetoothDevice mDevice;
        private String mUsername;
        private String mPassword;
        private String mMovementMessage = null;
        private LinkedList<String> mCommandMessages = new LinkedList<String>();
        private boolean mRunning = true;
        private final Pattern re_perimeter_signal_level =
                Pattern.compile("^perimeter signal level = (.*)$");
        private final Pattern re_battery_energy_level =
                Pattern.compile("^battery energy level = ([^%]+)%$");
        private final Pattern re_mode =
                Pattern.compile("^mode = (.*)$");

        public ControllerThread(BluetoothDevice device, String username, String password) {
            mDevice = device;
            mUsername = username;
            mPassword = password;
        }

        public boolean login() {
            boolean result = true;

            // login
            write("\n");
            write(mUsername + "\n");
            write(mPassword + "\n");

            try {
                String output = waitForPrompt();
                if (output == null) {
                    Log.d(TAG, "login failed");
                    result = false;
                } else {
                    Log.d(TAG, "logged in");
                }
            } catch (InterruptedException e) {
                result = false;
            } catch (IOException e) {
                result = false;
            }

            return result;
        }

        private void notifyActivity(int message_id, String message) {
            if (mHandler == null) {
                return;
            }

            Message msg = mHandler.obtainMessage(message_id);
            msg.obj = message;
            mHandler.sendMessage(msg);
        }

        @Override
        public void run() {
            int statusCount = -1;

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
                write("\n");
                write("logout\n");
                mSocket.close();
            } catch (IOException e) {
            }
        }

        private void status() {
            String battery_energy_level = null;
            int perimeter_signal_level = 0;
            String mode = null;
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

                matcher = re_mode.matcher(line);
                if (matcher.find()) {
                    mode = matcher.group(1);
                    continue;
                }
            }

            statusText = "m: " + mode + ", p: " + perimeter_signal_level + ", b: " + battery_energy_level + "%";

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
            mCommandMessages.add(command);
        }

        public synchronized String getCommand() {
            if (mCommandMessages.size() != 0){
                return mCommandMessages.removeFirst();
            }
            return null;
        }

        private String waitForPrompt() throws InterruptedException, IOException{
            int data;
            int timeout_counter = 0;
            String message = new String();

            while (true) {

                if (mInputStream.available() == 0) {
                    // 1 second timeout
                    if (timeout_counter == 20) {
                        return null;
                    }

                    timeout_counter++;
                    sleep(50);
                    continue;
                }

                timeout_counter = 0;
                data = mInputStream.read();
                message += Character.toString((char)data);

                // Prompt found?
                if (message.endsWith("$ ")) {
                    Log.d(TAG, "Prompt found.");
                    return message.substring(0, message.length() - 2);
                }
            }
        }

        private String executeCommand(String command) {
            String result = null;

            command += "\r\n";

            while (result == null) {
                Log.d(TAG, "Writing command: " + command);

                write(command.getBytes());

                try {
                    result = waitForPrompt();
                } catch (InterruptedException e) {
                    return null;
                } catch (IOException e) {
                    Log.e(TAG, "Failed to read from bluetooth device");
                    return null;
                }
            }

            return result;
        }

        private void write(String data) {
            write(data.getBytes());
        }

        private void write(byte[] data) {
            try {
                mOutputStream.write(data);
            } catch (IOException e){
                Log.e(TAG, "Exception during write", e);
            }
        }
    }
}
