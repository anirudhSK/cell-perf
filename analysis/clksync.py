#! /usr/bin/python
import sys
def clk_sync(file1_name,file2_name) :
  file1=open(file1_name,"r");
  file2=open(file2_name,"r");
  clock1={};
  clock2={};
  for line in file1.readlines() : 
     records=line.split();
     seq_num=records[4];
     clock1[int(seq_num)]=(records[6]);
  for line in file2.readlines() : 
     records=line.split();
     seq_num=records[4];
     clock2[int(seq_num)]=(records[6]);
  # do the averaging in buckets of 100 consecutive packets 
  bkt_size=100
  sum_diff={}
  count={}
  start_time={}
  end_time={}
  for pkt in clock1 :
       if pkt in clock2 : 
        if((pkt/bkt_size) in count) :
         count[(pkt/bkt_size)]+=1;
         sum_diff[(pkt/bkt_size)]+=int(clock2[pkt])-int(clock1[pkt])
         end_time[(pkt/bkt_size)]=int(clock2[pkt])
        else :
         count[(pkt/bkt_size)]=1;
         sum_diff[(pkt/bkt_size)]=int(clock2[pkt])-int(clock1[pkt]);
         start_time[(pkt/bkt_size)]=int(clock2[pkt])      
  return (sum_diff,count,start_time,end_time)
