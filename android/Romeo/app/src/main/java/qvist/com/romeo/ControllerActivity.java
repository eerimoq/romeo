package qvist.com.romeo;

import qvist.com.romeo.util.Constants;
import qvist.com.romeo.util.SystemUiHider;

import android.app.Activity;
import android.content.Context;
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

/**
 * An example full-screen activity that shows and hides the system UI (i.e.
 * status bar and navigation/system bar) with user interaction.
 *
 * @see SystemUiHider
 */
public class ControllerActivity extends Activity {

    // Used when logging
    private static final String TAG = "ControllerActivity";
    private static final int VIBRATE_TIME_MS = 5;

    private int mSpeed = 0;
    private int mAngularVelocity = 0;
    private SeekBar mAngularVelocitySeekbar;
    private SeekBar mSpeedSeekbar;
    private Vibrator mVibrator;
    private Button mSelectButton;
    private Button mStartButton;
    private TextView mStatusText;
    private boolean mStarted = false;
    private BluetoothService mBluetoothService;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_controller);
        getActionBar().hide();

        mVibrator = (Vibrator)this.getSystemService(Context.VIBRATOR_SERVICE);

        initSeekbars();
        initButtons();
        initStatusText();

        RomeoApplication app = (RomeoApplication)getApplication();
        mBluetoothService = app.mBluetoothService;
        mBluetoothService.setHandler(mHandler);

        mBluetoothService.sendCommand("");
        mBluetoothService.sendCommand("robot/stop");
        mBluetoothService.sendCommand("robot/mode/set manual");
        mBluetoothService.sendCommand("robot/parameters/watchdog/enabled 0");
    }

    private void initButtons() {
        mSelectButton = (Button)findViewById(R.id.select_button);
        mSelectButton.setOnTouchListener(new OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                switch (event.getAction()) {
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

        mStartButton = (Button) findViewById(R.id.start_button);
        mStartButton.setOnTouchListener(new OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                switch (event.getAction()) {
                    case MotionEvent.ACTION_DOWN:
                        mVibrator.vibrate(VIBRATE_TIME_MS);
                        break;

                    case MotionEvent.ACTION_UP:
                        Log.d(TAG, "Start button clicked");
                        mVibrator.vibrate(VIBRATE_TIME_MS);

                        if (mStarted) {
                            mBluetoothService.sendCommand("robot/stop");
                            mStarted = false;
                        } else {
                            mBluetoothService.sendCommand("robot/start");
                            mStarted = true;
                        }

                        mAngularVelocitySeekbar.setEnabled(mStarted);
                        mSpeedSeekbar.setEnabled(mStarted);
                        break;
                }

                return true;
            }
        });
    }

    private void initSeekbars() {
        mAngularVelocitySeekbar = (SeekBar)findViewById(R.id.angular_velocity_seekbar);
        mAngularVelocitySeekbar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                // 'progress' is 0 - 100. Move it to the range -100 - +100.
                mAngularVelocity = 2 * (progress - 50);
                mBluetoothService.setMovement(mSpeed, mAngularVelocity);
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
                mVibrator.vibrate(VIBRATE_TIME_MS);
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                mAngularVelocity = 0;
                mBluetoothService.setMovement(mSpeed, mAngularVelocity);
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
                mBluetoothService.setMovement(mSpeed, mAngularVelocity);
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
                mVibrator.vibrate(VIBRATE_TIME_MS);
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                mSpeed = 0;
                mBluetoothService.setMovement(mSpeed, mAngularVelocity);
                mSpeedSeekbar.setProgress(50);
                mVibrator.vibrate(VIBRATE_TIME_MS);
            }
        });

        mSpeedSeekbar.setEnabled(false);
    }

    private void initStatusText() {
        mStatusText = (TextView)findViewById(R.id.status_text);
    }

    private final Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case Constants.MESSAGE_CONNECTED_TO_DEVICE:
                    mSelectButton.setEnabled(true);
                    mStartButton.setEnabled(true);
                    break;
                case Constants.MESSAGE_STATUS_TEXT_UPDATE:
                    mStatusText.setText((String)msg.obj + ", s: " + mStarted);
                    break;
            }
        }
    };
}
