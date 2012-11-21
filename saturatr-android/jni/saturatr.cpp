#include <string>
#include <vector>
#include <poll.h>
#include <assert.h>
#include <sys/types.h>
#include <unistd.h>

#include "acker.hh"
#include "saturateservo.hh"

using namespace std;
#ifdef ANDROID_SAT
#include <jni.h>
int mainFunction( int argc, const char *argv[]) ;
JNIEXPORT void JNICALL Java_Saturatr_Android_runClient( JNIEnv* env, jobject thiz, jstring dev, jstring server_ip, jstring server_port, jstring direction)
{
   const char** argv=(const char**)malloc(sizeof(char*)*10); // ANIRUDH: Mod here
   argv[0]="saturatr";
   argv[1] = (env)->GetStringUTFChars(dev, 0);
   argv[2] = (env)->GetStringUTFChars(server_ip, 0);
   argv[3] = (env)->GetStringUTFChars(server_port, 0);
   argv[4] = (env)->GetStringUTFChars(direction, 0);
   mainFunction(5,argv);
}
#endif

#ifdef ANDROID_SAT
int mainFunction( int argc, const char *argv[]) 
#elif PC_SAT
int main( int argc, char *argv[] )
#endif
{
  if ( argc != 3 && argc != 5 ) {
    fprintf( stderr, " Usage: %s [DEV SERVER_IP SERVER_PORT DIRECTION:1 for downlink ] \n Usage: %s [SERVER_PORT DIRECTION:1 for downlink]\n",
	     argv[ 0 ],
             argv[ 0 ]);
    exit( 1 );
  }

  Socket data_plus_feedback_socket;
  bool server;
  bool _downlink;

  uint32_t sender_id = getpid();

  Socket::Address remote_data_address( UNKNOWN ), remote_feedback_address( UNKNOWN );

  uint64_t ts=Socket::timestamp();
  if ( argc == 3 ) { /* server */
    server = true;
    uint32_t server_port=atoi(argv[ 1 ]);
    data_plus_feedback_socket.bind( Socket::Address( "0.0.0.0", server_port ) );
    _downlink = (atoi(argv[ 2 ])==1) ? true : false ;
  } else { /* client */
    server = false;
    
    const char *dev = argv[ 1 ];

    const char *server_ip = argv[ 2 ];

    uint32_t server_port=atoi(argv[ 3 ]);

    _downlink = (atoi(argv[ 4 ])==1) ? true : false ;

    sender_id = (ts/1e9);

    data_plus_feedback_socket.bind( Socket::Address( "0.0.0.0", 9002 ) );
    data_plus_feedback_socket.bind_to_device( dev );
    remote_data_address = Socket::Address( server_ip, server_port );
    remote_feedback_address = Socket::Address( server_ip, server_port );
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
    if (_downlink) {
     if (server)     saturatr.tick();
     else            acker.tick();
    }
    else {
     if (server)     acker.tick();
     else            saturatr.tick();
    }
    /* fire off either the saturatr or the acker, never both,
       This is so that you can run this on only 1 phone */ 
    
    /* wait for incoming packet OR expiry of timer */
    struct pollfd poll_fds[ 1 ];
    poll_fds[ 0 ].fd = data_plus_feedback_socket.get_sock();
    poll_fds[ 0 ].events = POLLIN;

    uint64_t next_transmission_delay = server ? saturatr.wait_time() : acker.wait_time() ;

    if ( next_transmission_delay == 0 ) {
      fprintf( stderr, "ZERO %ld %ld\n", saturatr.wait_time(), acker.wait_time() );
    }
  
    uint32_t timeout_in_ms= next_transmission_delay / (uint64_t) 1000000;
    poll( poll_fds, 1, timeout_in_ms);/* timeout in ms*/

    if ( poll_fds[ 0 ].revents & POLLIN ) {
     if(_downlink) {
      if (server) saturatr.recv() ;
      else        acker.recv();
     }
     else {
      if (server) acker.recv() ;
      else        saturatr.recv();
     }
    }
  }
}
