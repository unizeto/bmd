#include <bmd/libbmdnet/libbmdnet.h>
#include <bmd/libbmdthreads/libbmdthreads.h>
#include <bmd/common/bmd-common.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>



void * server (bmdnet_handler_ptr handler) {
	char buf_in[65536];
	long i;
	PRINT_DEBUG("New server.\n");
	while(1) {
		i=bmdnet_recv(handler,buf_in,sizeof(buf_in),1);
		if (i<0) { PRINT_DEBUG("recive error"); break; };
		PRINT_DEBUG("Recived %lu bytes.\n", (long )i);
		i=bmdnet_send(handler,buf_in,i);
		if (i<0) { PRINT_DEBUG("send error"); break; };
		PRINT_DEBUG("Sent %lu bytes.\n",(long )i);
	}
	bmdnet_close(&handler);
	return NULL;
}

int main(int argc, char ** argv)
{
	long port;
	long mode=0;
	long thread_mode=0;
	long res;

	if (argc<3 || argc>5) { 
		printf("Usage:\n%s interface port [mode] [thread_mode]\n", argv[0]); 
		printf("mode: 0 - plain tcp, 1 - openssl\n");
		printf("thread mode: 0 - fork, 1 - thread\n");
		return(-1); 
	}

	port=strtol( argv[2], 0, 10 );
	if (port==LONG_MAX || port==LONG_MIN) {
		printf("Wrong port number.\n");
		exit(-1);
	}

	if (argc>3) {
		mode=strtol( argv[3], 0, 10 );
		if (mode!=0 && mode!=1) {
			printf("Wrong mode, 0 for PLAIN TCP, 1 for OPENSSL.\n");
			exit(-1);
		}
	}

	if (argc>4) {
		mode=strtol( argv[4], 0, 10 );
		if (mode!=0 && mode!=1) {
			printf("Wrong thread mode, 0 for forks, 1 for pthreads.\n");
			exit(-1);
		}
	}


	if (thread_mode==0) bmdth_init(BMD_THREAD_FORK);
		else bmdth_init(BMD_THREAD_PTHREAD);
	bmdnet_init ();
	
	bmdnet_handler_ptr socket;
	if (mode==0)	res = bmdnet_create(BMD_NET_PLAINTCP_METHOD,1, &socket );
		else res = bmdnet_create(BMD_NET_OPENSSL_BIO_METHOD,1, &socket );

	bmdnet_server( socket, argv[1], port, 10, NULL, server );

	bmdnet_close(&socket);
	bmdnet_destroy();
	
	return 0;
}
