#include <bmd/libbmdnet/libbmdnet.h>
#include <bmd/libbmdthreads/libbmdthreads.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <bmd/common/bmd-common.h>


static char buf[65536] = "GET / HTTP/1.1\nHost: secure.inter-c.pl\n\n", buf2[1024];

int main(int argc, char ** argv)
{
	long i=0, j=0,k=0,res=0;
	long port;
	long packet_size=1024;
	long nof_packets=1024;
	long nof_clients=10;
	long mode=0;
	long thread_mode=0;
	bmdnet_handler_ptr socket;
	bmdnet_init ();

	if (argc<3 || argc>8) { 
		printf("Usage:\n%s interface port [mode] [thread_mode] [packet size] [nof_packets] [nof_clients]\n", argv[0]); 
		printf("mode: 0 - plain tcp, 1 - openssl, 2 - http proxy\n");
		printf("thread mode: 0 - fork, 1 - thread\n");
		return(-1); }
	port=strtol( argv[2], 0, 10 );
	if (port==LONG_MAX || port==LONG_MIN) {
		printf("Wrong port number.\n");
		exit(-1);
	}

	if (argc>3) {
		mode=strtol( argv[3], 0, 10 );
		printf("\n%s -> %d\n", argv[3], mode );
		if ((mode!=0) && (mode!=1) && (mode!=2) ) {
			printf("Wrong mode, 0 for PLAIN TCP, 1 for OPENSSL.\n");
			exit(-1);
		}
	}
	
	if (argc>4) {
		thread_mode=strtol( argv[4], 0, 10 );
		if ((thread_mode!=0) && (thread_mode!=1)) {
			printf("Wrong thread mode, 0 for forks, 1 for pthreads.\n");
			exit(-1);
		}
	}


	if (argc>5) {
		packet_size=strtol( argv[5], 0, 10 );
		if (packet_size==LONG_MAX || packet_size==LONG_MIN) {
			printf("Wrong packet size.\n");
			exit(-1);
		}
	}
	if (argc>6) {
		nof_packets=strtol( argv[6], 0, 10 );
		if (nof_packets==LONG_MAX || nof_packets==LONG_MIN) {
			printf("Wrong nof_packets.\n");
			exit(-1);
		}
	}
	if (argc>7) {
		nof_clients=strtol( argv[7], 0, 10 );
		if (nof_clients==LONG_MAX || nof_clients==LONG_MIN) {
			printf("Wrong nof_clients.\n");
			exit(-1);
		}
	}


	if (thread_mode==0) bmdth_init(BMD_THREAD_FORK);
		else bmdth_init(BMD_THREAD_PTHREAD);

	for (j=0;j<nof_clients;j++) {
		printf("*** mode: %d", mode );
		switch( mode ) {
			case 0:
				printf("\nmethod: PLAIN TCP\n");
				res = bmdnet_create(BMD_NET_PLAINTCP_METHOD,1,&socket);
				break;
			case 1:
				printf("\nmethod: OPEN SSL\n");
				res = bmdnet_create(BMD_NET_OPENSSL_BIO_METHOD,1,&socket);
				break;
			case 2: 
				printf("\nmethod: HTTP PROXY\n");
				res = bmdnet_create(BMD_NET_HTTP_PROXY_METHOD,1,&socket);
				break;
		}
	
		if( fork()==0 ) {
			bmdnet_connect (socket,"www.wp.pl",80);
			for(k=0;k<nof_packets; k++) {
				i=bmdnet_send (socket, buf,packet_size);
				printf ("Sent %i bytes\n",i);
				memset(buf, 0, sizeof( buf ) );
				i=0;
				do {
				    res=bmdnet_recv (socket, buf, packet_size-i, 1);
				    if (res<0) { PRINT_DEBUG("Recive error\n"); break; }
				    i+=res;
			    	printf("...received i=%u bytes...\n",i);
				} while (packet_size<10);
			printf ("Received %lu\n",i);
			}
			bmdnet_close(&socket);
			break;
		}
	
		bmdnet_close(&socket);
	}
	bmdnet_destroy();
	
	return 0;
}
