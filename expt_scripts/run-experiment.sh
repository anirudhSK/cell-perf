#! /bin/bash
if [ $# -ne 1 ] ; then
  echo "Enter server ip "
  exit 
fi
server_ip=$1
sudo killall -s9 adb saturatr screen
adb start-server
port=9005
rm server-*
for dev in $(adb devices | head -n -1 | tail -n +2 | cut -f 1) ; do
 #0. saturatr on laptop, one for each phone :
 port=`expr $port '+' 1`
 screen -d -m ~/CellPerf/cell-perf/saturatr-android/jni/saturatr $port 
 adb_root="adb -s $dev shell su -c"
 screen -d -m $adb_root "cd /data ; ./rbs"  # rbs on each phone 
 screen -d -m $adb_root "cd /data ; ./saturatr rmnet1 $server_ip $port" 
done 
