package Saturatr.Android;
import android.app.Activity;
import android.os.Bundle;
import android.widget.Toast;
public class StatusActivity extends Activity
{
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        
        Toast.makeText(this, "Running Tandem Saturatr", Toast.LENGTH_SHORT).show();

	String         dev="rmnet1";
	String   server_ip="18.251.6.223";
	String server_port="1025";
	String   direction="1";
        
        runClient(dev, server_ip, server_port, direction);
    }
     
    public native void runClient(String dev, String server_ip, String server_port, String direction); // "hook" into native code

    static {
        System.loadLibrary("saturatr");
    }
}
