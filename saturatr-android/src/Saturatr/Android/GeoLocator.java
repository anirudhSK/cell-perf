package Saturatr.Android;
import  android.location.LocationManager;
import android.content.Context;
import android.location.LocationListener;
import android.location.Location;
import android.util.Log;
import android.os.Bundle;
public class GeoLocator {
	/* Wrapper around the Android Nwk. Loc API for now, switch to GPS if required later */
	private LocationManager  locationManager;
	private LocationListener locationListener;
	public 	GeoLocator(Context _context) {
		// Acquire a reference to the system Location Manager
		locationManager = (LocationManager) _context.getSystemService(Context.LOCATION_SERVICE);

		// Define a listener that responds to location updates
		LocationListener locationListener = new LocationListener() {
			public void onLocationChanged(Location location) {
				Log.d("Saturatr:GeoLocator","Current location is "+Double.toString(location.getLatitude())+","+Double.toString(location.getLongitude()));
			}
			public void onStatusChanged(String provider, int status, Bundle extras) {}
			public void onProviderEnabled(String provider) {}
			public void onProviderDisabled(String provider) {}
		};

		// Register the listener with the Location Manager to receive location updates
		locationManager.requestLocationUpdates(LocationManager.NETWORK_PROVIDER, 0, 0, locationListener);
	}
}
