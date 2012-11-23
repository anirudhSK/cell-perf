package Saturatr.Android;
import java.lang.Runtime;
import java.lang.Process;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import android.util.Log;
public class ServerLocator  {
        /* Pick the closest EC2 instance using 10 pings.*/
	private String[] serverList;
	
	public ServerLocator(String[] _serverList) {
		serverList=_serverList;
	}

	public double GetClosestServer() {
		int i=0;
		double[] latency = new double[serverList.length];
		for (i=0; i < serverList.length; i++) 
		{
			latency[i]=Ping(serverList[i]);
		}
		double minLatency=latency[0];
		String bestServer=serverList[0];
		for (i=0; i < latency.length; i++)
		{
			if (latency[i] < minLatency) 
			{
				minLatency=latency[i];
				bestServer=serverList[i];
			}
		}
		Log.d("Saturatr","min latency is "+Double.toString(minLatency)+ " ms ");
		return minLatency;
			
	}
	
	private double Ping(String serverName) {
		try{
                	Process process;            
	                process = Runtime.getRuntime().exec("ping -c 10 " + serverName +" ");
        	        BufferedReader in = new BufferedReader(new InputStreamReader(process.getInputStream()));
			String line=in.readLine();
			double averageRtt=0.0;
			while (line != null )  {
				if (line.startsWith("rtt")) {
					/* This is the RTT line */
					averageRtt=Float.parseFloat(line.split(" ")[3].split("/")[1]);
				}
				line=in.readLine();
				
			}
			Log.d("Saturatr","ping output: average rtt is "+Double.toString(averageRtt)+" ms ");
			/* parse and get average */
			return averageRtt;
    		}
		catch (Exception e) {
        		e.printStackTrace();
		}
			return -1.0;
	
	}
}
