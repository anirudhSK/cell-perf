#include <string>
#include <vector>
#include <poll.h>
#include <assert.h>
#include <sys/types.h>
#include <unistd.h>

#include "acker.hh"
#include "saturateservo.hh"

using namespace std;

int main( int argc, char *argv[] )
{
  if ( argc != 1 && argc != 4 ) {
    fprintf( stderr, "Usage: %s [IP DEV SERVER_IP]\n",
	     argv[ 0 ]);
    exit( 1 );
  }

  Socket data_plus_feedback_socket;
  bool server;

  uint32_t sender_id = getpid();

  Socket::Address remote_data_address( UNKNOWN ), remote_feedback_address( UNKNOWN );

  uint64_t ts=Socket::timestamp();
  if ( argc == 1 ) { /* server */
    server = true;
    data_plus_feedback_socket.bind( Socket::Address( "0.0.0.0", 9001 ) );
  } else { /* client */
    server = false;
    
    const char *ip = argv[ 1 ];
    const char *dev = argv[ 2 ];

    const char *server_ip = argv[ 3 ];

    sender_id = (ts/1e9);

    data_plus_feedback_socket.bind( Socket::Address( ip, 9002 ) );
    data_plus_feedback_socket.bind_to_device( dev );
    remote_data_address = Socket::Address( server_ip, 9001 );
    remote_feedback_address = Socket::Address( server_ip, 9001 );
  }

  FILE* log_file;
  char log_file_name[50];
  sprintf(log_file_name,"%s-%d-%d",server ? "server" : "client",(int)(ts/1e9),sender_id);
  log_file=fopen(log_file_name,"w");

  SaturateServo saturatr( "OUTGOING", log_file, data_plus_feedback_socket, data_plus_feedback_socket, remote_data_address, server, sender_id );
  Acker acker( "INCOMING", log_file, data_plus_feedback_socket, data_plus_feedback_socket, remote_feedback_address, server, sender_id );


  saturatr.LOWER_RTT = 0.75;
  saturatr.UPPER_RTT = 3.0;

  while ( 1 ) {
    fflush( NULL );

    /* possibly send packet */
    if (server)     saturatr.tick();
    else            acker.tick();
    /* fire off either the saturatr or the acker, never both,
       This is so that you can run this on only 1 phone */ 
    
    /* wait for incoming packet OR expiry of timer */
    struct pollfd poll_fds[ 1 ];
    poll_fds[ 0 ].fd = data_plus_feedback_socket.get_sock();
    poll_fds[ 0 ].events = POLLIN;

    struct timespec timeout;
    uint64_t next_transmission_delay = server ? saturatr.wait_time() : acker.wait_time() ;

    if ( next_transmission_delay == 0 ) {
      fprintf( stderr, "ZERO %ld %ld\n", saturatr.wait_time(), acker.wait_time() );
    }
  
    uint32_t timeout_in_ms= next_transmission_delay / (uint64_t) 1000000;
    poll( poll_fds, 1, timeout_in_ms);/* timeout in ms*/

    if ( poll_fds[ 0 ].revents & POLLIN ) {
      if (server) saturatr.recv() ;
      else        acker.recv();
    }
  }
}
