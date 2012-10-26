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
  if ( argc != 1 && argc != 6 ) {
    fprintf( stderr, "Usage: %s [FEEDBACK_IP FEEDBACK_DEV DATA_IP DATA_DEV SERVER_IP]\n",
	     argv[ 0 ]);
    exit( 1 );
  }

  Socket data_socket, feedback_socket;
  bool server;

  int sender_id = getpid();

  Socket::Address remote_data_address( UNKNOWN ), remote_feedback_address( UNKNOWN );

  uint64_t ts=Socket::timestamp();
  if ( argc == 1 ) { /* server */
    server = true;
    data_socket.bind( Socket::Address( "0.0.0.0", 9001 ) );
    feedback_socket.bind( Socket::Address( "0.0.0.0", 9002 ) );
  } else { /* client */
    server = false;
    
    const char *feedback_ip = argv[ 1 ];
    const char *feedback_dev = argv[ 2 ];

    const char *data_ip = argv[ 3 ];
    const char *data_dev = argv[ 4 ];

    const char *server_ip = argv[ 5 ];

    sender_id = ( (int)(ts/1e9) );

    data_socket.bind( Socket::Address( data_ip, 9003 ) );
    data_socket.bind_to_device( data_dev );
    remote_data_address = Socket::Address( server_ip, 9001 );

    feedback_socket.bind( Socket::Address( feedback_ip, 9004 ) );
    feedback_socket.bind_to_device( feedback_dev );
    remote_feedback_address = Socket::Address( server_ip, 9002 );
  }

  FILE* log_file;
  char log_file_name[50];
  sprintf(log_file_name,"%s-%d-%d",server ? "server" : "client",(int)(ts/1e9),sender_id);
  log_file=fopen(log_file_name,"w");

  SaturateServo saturatr( "OUTGOING", log_file, feedback_socket, data_socket, remote_data_address, server, sender_id );
  Acker acker( "INCOMING", log_file, data_socket, feedback_socket, remote_feedback_address, server, sender_id );

  saturatr.set_acker( &acker );
  acker.set_saturatr( &saturatr );

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
    struct pollfd poll_fds[ 2 ];
    poll_fds[ 0 ].fd = data_socket.get_sock();
    poll_fds[ 0 ].events = POLLIN;
    poll_fds[ 1 ].fd = feedback_socket.get_sock();
    poll_fds[ 1 ].events = POLLIN;

    struct timespec timeout;
    uint64_t next_transmission_delay = std::min( saturatr.wait_time(), acker.wait_time() );

    if ( next_transmission_delay == 0 ) {
      fprintf( stderr, "ZERO %ld %ld\n", saturatr.wait_time(), acker.wait_time() );
    }

    timeout.tv_sec = next_transmission_delay / 1000000000;
    timeout.tv_nsec = next_transmission_delay % 1000000000;
    poll( poll_fds, 2, timeout.tv_sec*1000+1000000*timeout.tv_nsec);/* timeout in ms*/

    if ( poll_fds[ 0 ].revents & POLLIN ) {
      acker.recv();
    }

    if ( poll_fds[ 1 ].revents & POLLIN ) {
      saturatr.recv();
    }
  }
}
