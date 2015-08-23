package qvist.com.romeo;

import android.app.Activity;
import android.content.Intent;
import android.support.v7.app.ActionBarActivity;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.EditText;

import qvist.com.romeo.util.Constants;


public class PinCodeActivity extends Activity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pin_code);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_pin_code, menu);
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

    public void onCancelButtonClick(View view) {
        setResult(Activity.RESULT_CANCELED);
        finish();
    }

    public void onOkButtonClick(View view) {
        EditText pinCode;
        Intent intent;

        // pass pin code to parent activity
        pinCode = (EditText)findViewById(R.id.pin_code);
        intent = new Intent(this, ControllerActivity.class);
        intent.putExtra(Constants.INTENT_EXTRA_PIN_CODE, pinCode.getText().toString());
        setResult(Activity.RESULT_OK, intent);setResult(Activity.RESULT_OK, intent);
        finish();
    }
}
