/* RBS transmitter that runs on the laptop */
#include"socket.hh"
#include<string>
int main() {
  /* Create WiFi transmitter socket */
  Socket wifi_tx_sock;
  Socket::Address wifi_addr("192.168.5.1",9001);
  wifi_tx_sock.bind(wifi_addr);
  wifi_tx_sock.bind_to_device("wlan0");

 /* Allow it to transmit broadcast packets*/
  wifi_tx_sock.enable_broadcast();

 /* Keep transmitting broadcast packets */
 Socket::Address bcast("192.168.5.255",9000);
 uint64_t packets_sent=0;
 uint64_t last_time=0;
 while(1) {
  uint64_t the_time = Socket::timestamp();
  if ( the_time > last_time + 10e6 ) { /* 10 ms */
   packets_sent++;
   char *seq_encoded = (char *)&packets_sent;
   wifi_tx_sock.send( Socket::Packet(bcast, std::string( seq_encoded, 51 ) ) );
   last_time=the_time;
  }
  else {
   usleep(last_time + 10e6 - the_time);
  }
 }
}
