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



int main(int argc, char ** argv)
{
	bmdth_thread_t server_thread;
	bmdnet_handler_ptr socket; 	
	long port = 8080;
	long i;
	_GLOBAL_debug_level=2;

	bmdnet_init ();
	
	
	i = bmdnet_create(BMD_NET_CONNECT_PROXY_METHOD,1, &socket);
	bmd_assert (i==0,"LK");
	i = bmdnet_set_option (socket, 1, "192.168.1.1",0);
	bmd_assert (i==0,"LK");
	i =bmdnet_set_option (socket, 2, &port,0);
	bmd_assert (i==0,"LK");
	
	i = bmdnet_connect(socket, "voip.inter-c.pl",443);
	bmd_assert (i==0,"LK");

	#define string "GET / HTTP/1.1\r\n\r\n"
	i = bmdnet_send(socket, string, strlen(string));
	bmd_assert (i!=0,"LK");

	char buffer[100] ={0};
	do {
		i=bmdnet_recv(socket, buffer, 99,1);
		buffer[i] = '\0';
		printf("%s",buffer);
	} while (i>0);
	

	bmdnet_close(&socket);
	bmdnet_destroy();
	printf ("%i\n",i);
	return 0;
}
