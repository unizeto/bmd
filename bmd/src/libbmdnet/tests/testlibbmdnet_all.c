#include <bmd/libbmdnet/libbmdnet.h>
#include <bmd/libbmdthreads/libbmdthreads.h>
#include <bmd/libbmdutils/libbmdutils.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

#define NCLIENTS 10000
#define SIZE 40
#define NLOOPS 1
#define PORT 12344

#define THREADING BMD_THREAD_FORK 
#define METHOD BMD_NET_PLAINTCP_METHOD


void * server (bmdnet_handler_ptr handler)
{
	char buf_in[SIZE+2];
	long status=0;
	long nbytes=0;
	long res;
	
  		nbytes = 0;
		do {		
			res = bmdnet_recv_min(handler,buf_in,SIZE,SIZE+1);
			/* printf("receive attempt in server, status=%i\n",status);*/
			if (status<0) { 
				perror("bmdnet");
				bmdnet_close(&handler);
				printf("EEEEEEEEEEEEEEERRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR received %i bytes! status=%i\n",nbytes,status);
				exit(0);
				
			}
			nbytes += status;
	    printf("hehhe!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!11");
		} while (nbytes != SIZE);
		

	    
	    status = bmdnet_send(handler,buf_in,SIZE);
	    printf("hehhe!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!11");
	    printf ("%s send status=%i\n",__FILE__,status);
	    if (status < 0) {
		perror("sending");
		bmdnet_close(&handler);
		exit(0);
		
	    }

	printf("SSSSSSSSSSSWEEEEEEEEEEERRRRRRRRRRRRRRRVVVVVVVVVVVVVVVVEEEEEEEEEEEERRRRRRRRRRRRRRRRR  exiting %s\n",__FUNCTION__);
	bmdnet_close(&handler);
	return NULL;
}

void * client (bmdnet_handler_ptr socket)
{
	char buf2[SIZE+2];
	long s,r;
	long count;
	
	if (bmdnet_connect (socket,"localhost",PORT)) {
		printf ("connect() failed!\n");
		exit(0);
	}

	{
		long nbytes;
		
		s=bmdnet_send (socket, buf2,SIZE);
		if (s < 1) {
			printf ("Sent %i bytes!\n",s);
			exit(0);
		};
		
		nbytes=0;
		do {
			r = bmdnet_recv_min (socket, buf2, SIZE,SIZE);
		
			if (r<0) { 
				printf("received %u bytes!\n",r);exit(0);
			}
			nbytes += r;
		} while (nbytes != SIZE);
		
		if (r<0) {
			perror("client recv");
			exit(0);
		}
	}
	
	printf ("Out of client() on count=%lu\n",count);
	
	bmdnet_close(&socket);
	
	return 0;
}


void * server_main (bmdnet_handler_ptr socket)
{
	return (void*) bmdnet_server( socket, "127.0.0.1", PORT, 10, NULL, server );	
}

int main(int argc, char ** argv)
{
	bmdth_thread_t server_thread;
	bmdnet_handler_ptr socket; 	
	long i;
	
	bmdth_init(THREADING);
	bmdnet_init ();
	
	
	i = bmdnet_create(METHOD,1, &socket);

	bmdth_create_thread(&server_thread,BMDTH_ATTR_DEFAULT, 
		(void *(*) (void *)) server_main, socket);
	bmdnet_close(&socket);

	sleep(1);
	printf("Starting tests!\n");
	bmd_dtime_start("clients");
	for (i=0; i < NCLIENTS; i++) {	
		bmdnet_create(METHOD,1,socket);
		client(socket);
		/*sleep(10);*/
	};	

	bmdth_wait(server_thread);
	
	
	

	bmdnet_destroy();
	printf ("%i\n",i);
	return 0;
}
