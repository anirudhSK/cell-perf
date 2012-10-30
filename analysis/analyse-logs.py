#! /usr/bin/python
# -analyse cellular network packet arrivals 
# -plot packet arrivals from 2 different phones on the same graph using wifi logs for clock sync
import sys
from clksync import *
if len(sys.argv) < 5 : 
   print "Usage : ", sys.argv[0]," phone1_wifi phone1_cellular phone2_wifi phone2_cellular "
   exit(5)

phone1_wifi=sys.argv[1]
phone1_cellular=sys.argv[2]
phone2_wifi=sys.argv[3]
phone2_cellular=sys.argv[4]
# apply clock sync function to the cellular logs
def apply_clk_sync(time,clock_sync_function) :
   # search for time
   start_time=clock_sync_function[2]
   end_time=clock_sync_function[3]  
   for bkt in start_time :
      if ( (time > start_time[bkt]) and (time < end_time[bkt]) ) :
         # bring phone2 back "down" to phone 1
         return int(time-float(clock_sync_function[0][bkt])/clock_sync_function[1][bkt])

# analyse wifi logs to retrieve the clock syncing function 
clock_sync_function=clk_sync(phone1_wifi,phone2_wifi)
# now process cell phone data 
phone1_cell=open(phone1_cellular,"r");
phone2_cell=open(phone2_cellular,"r");
phone1_ts=[]
phone2_ts=[]
for line in phone1_cell.readlines() :
  records=line.split()
  phone1_ts.append(int(records[6].strip(',').split('=')[1]))

for line in phone2_cell.readlines() :
  records=line.split()
  recv_time=(int(records[6].strip(',').split('=')[1]))
  phone2_ts.append(apply_clk_sync(recv_time,clock_sync_function))
phone1_logs=open("ph1.opt","w");
phone2_logs=open("ph2.opt","w");
for ts in phone1_ts : 
  phone1_logs.write(str(ts)+"\n")
for ts in phone2_ts : 
  phone2_logs.write(str(ts)+"\n")
