#! /bin/bash
sudo killall -s9 adb transmitter hostapd
adb start-server
# cd to the wifi-rbs folder
cd ~/CellPerf/cell-perf/wifi-rbs/jni
# build Android exes
ndk-build clean
ndk-build 
# build Laptop transmitter
make clean
make transmitter
# run hostapd on the laptop to create AP
sudo hostapd ~/CellPerf/cell-perf/hostapd-rbs.conf -B > hostapd.log
sudo ifconfig wlan0 192.168.5.1 netmask 255.255.255.0
# setup binaries on each phone
ip=2
# run on each device
for dev in $(adb devices | head -n -1 | tail -n +2 | cut -f 1) ; do
   adb_root="adb -s $dev shell su -c"
   adb -s $dev push ../libs/armeabi/wifi-rbs /sdcard/rbs
   $adb_root "mount -o rw,remount -t yaffs2 /dev/block/mmcblk0p9 /system"
   $adb_root "cat /sdcard/rbs > /system/bin/rbs"
   $adb_root "chmod 777 /system/bin/rbs"
   adb -s $dev push ~/CellPerf/cell-perf/wpa_supplicant.conf /sdcard/rbs.conf
   $adb_root "wpa_supplicant -iwlan0 -c/sdcard/rbs.conf -B"
   $adb_root "ifconfig wlan0 192.168.5.$ip netmask 255.255.255.0"
   $adb_root "rm /sdcard/wifi-rbs-*"
   $adb_root "rbs" &
   ip=`expr $ip '+' 1`
   echo "Next ip is $ip"
done;
# now run transmitter
sudo ./transmitter > transmitter.log &
