#! /bin/bash
# remove vestiges of old runs
sudo killall -s9 adb transmitter hostapd dhcpd saturatr

# start adb to recognize all phones 
adb start-server
# TODO: Set phone to 3G or 4G

# build exes
# --saturatr
cd ~/CellPerf/cell-perf/saturatr-android
ndk-build clean
ndk-build 
# --rbs
cd ~/CellPerf/cell-perf/wifi-rbs
ndk-build clean
ndk-build 

# build Laptop exes
# --transmitter
cd ~/CellPerf/cell-perf/wifi-rbs/jni
make clean
make transmitter
# --saturatr
cd ~/CellPerf/cell-perf/saturatr-android/jni
make clean
make saturatr

# clean vestiges on the phones  
for dev in $(adb devices | head -n -1 | tail -n +2 | cut -f 1) ; do
 echo "Device is $dev"
 adb_root="adb -s $dev shell su -c"
 # remove old sync logs
 $adb_root "rm /sdcard/wifi-rbs-*"

 # remove old client server logs
 $adb_root "rm /data/client-*"
 $adb_root "rm /data/server-*"

 # kill any old supplicants
 $adb_root "/data/killall.sh wpa_supplicant-iwlan0-B-c/sdcard/rbs.conf"
done

echo "sleep 5 seconds before starting hostapd"
sleep 5
# run hostapd on the laptop to create AP
sudo ifconfig wlan0 192.168.5.1 netmask 255.255.255.0 up
xterm -e "sudo hostapd ~/CellPerf/cell-perf/hostapd-rbs.conf" &

echo "Press enter after entering password on xterm within 10 seconds"
read -t 10 
ip=2
#run on each device
for dev in $(adb devices | head -n -1 | tail -n +2 | cut -f 1) ; do
 echo "Device is $dev"
 adb_root="adb -s $dev shell su -c"
 $adb_root "echo \"hi\" > /data/rbs"
 $adb_root "echo \"hi\" > /data/saturatr"

 $adb_root "chmod 777 /data/rbs"
 $adb_root "chmod 777 /data/saturatr"

 adb -s $dev push ~/CellPerf/cell-perf/wifi-rbs/libs/armeabi/wifi-rbs /data/rbs
 adb -s $dev push ~/CellPerf/cell-perf/saturatr-android/libs/armeabi/saturatr /data/saturatr
 adb -s $dev push ~/CellPerf/cell-perf/wpa_supplicant.conf /sdcard/rbs.conf

 # run supplicant on phone 
 $adb_root "wpa_supplicant -iwlan0 -B -c /sdcard/rbs.conf"

 # set static ip on phone
 $adb_root "ifconfig wlan0 192.168.5.$ip netmask 255.255.255.0"

 ip=`expr $ip '+' 1`
 echo "Next ip is $ip"

done

# wait a while
echo "sleep 5 seconds"
sleep 5
ping -c5 192.168.5.2
ping -c5 192.168.5.3

# now run transmitter on laptop
sudo ~/CellPerf/cell-perf/wifi-rbs/jni/transmitter > transmitter.log 
