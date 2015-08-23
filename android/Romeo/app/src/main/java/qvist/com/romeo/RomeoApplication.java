package qvist.com.romeo;

import android.app.Application;

/**
 * Created by erik on 2015-08-23.
 */
public class RomeoApplication extends android.app.Application {

    public BluetoothService mBluetoothService = null;

    @Override
    public void onCreate()
    {
        super.onCreate();
    }
}
