#include"socket.hh"
#include<sys/time.h>
/* Receive broadcasts on wlan0 for RBS sync */
int main()  {
 /* Get current timeStamp */
 struct timeval timeStamp;
 gettimeofday(&timeStamp,NULL);

 /* Create log file */
 char fileName[100];
#ifdef PC
 sprintf(fileName,"../../wifi-rbs-%ld.txt", (long)timeStamp.tv_sec);
#else
 sprintf(fileName,"/mnt/sdcard/wifi-rbs-%ld.txt", (long)timeStamp.tv_sec);
#endif

 /* Open file for logging */
 FILE * log_handle;
 log_handle=fopen(fileName,"w");

 /* Create wifi socket */
 Socket wifi_socket;
 Socket::Address wifi_addr( "0.0.0.0", 9000 );
 wifi_socket.bind(wifi_addr);
 wifi_socket.bind_to_device("wlan0");

 /* Allow it to receive broadcast packets*/
 wifi_socket.enable_broadcast();

 /* now receive and log every packet */
 while(1) {
  fflush(log_handle);
  Socket::Packet sync_pkt=wifi_socket.recv();
  long double ts=sync_pkt.timestamp; /* timestamp in secs */
  fprintf(log_handle,"Received packet seq num %llu @ %lf \n",*(uint64_t *)sync_pkt.payload.data(),ts);  
 } 
 
 return 0;
}
