#include <stdio.h>
#include <bmd/libbmdpr/bmd_datagram/bmd_datagram_set.h>
#include <bmd/libbmd/libbmd.h>
#include <bmd/libbmdconfig/libbmdconfig.h>
#include <bmd/libbmdts/libbmdts.h>
//#include <conf_sections.h>
#include <stdlib.h>
#include <signal.h>


#ifndef FOK
#define FOK(x) { int status=x; if(status)  {printf("Error %i in %s:%i\n",status,__FILE__, __LINE__); exit(status); }}
#endif


int j=1;
int k=1;
pid_t spid=0;

void signal_handler(int status)
{
/*int id=0;
int pid=0;*/

	printf("BMDSERVERINF Finishing programm work pid=%i status=%i\n",getpid(),status);
	printf("%i\n",spid);
	kill(spid,SIGUSR);
	j=0;

}

void signal_handler2(int status)
{
/*int id=0;
int pid=0;*/

	printf("BMDSERVERINF Finishing subprocess programm work pid=%i status=%i\n",getpid(),status);
	k=0;
}

extern long _GLOBAL_debug_level;
int main(int argc, char ** argv)
{
	system("clear");
	/*printf("działa\n");
	signal(SIGINT,signal_handler);
	signal(SIGUSR,signal_handler2);
	signal(SIGCHLD,SIG_IGN);
	pid_t pid=fork();
	if (pid==0)
	{
		signal(SIGINT,SIG_IGN);
		printf("jestem dzieckiem pid=%i\n\n",getpid());
		while(k)
		{}
	}
	else
	{
		spid=pid;
		printf("jestem ojcem pid=%i\n\n",getpid());
		while(j)
		{}
	}*/
	bmd_info_t *bi=NULL;
	bmdDatagram_t *dtg=NULL,*tmp_dtg=NULL;
	bmdDatagramSet_t *req_dtg_set=NULL;
	bmdDatagramSet_t *resp_dtg_set=NULL;
	lob_request_info_t *li=NULL;
	long int index=0;
	long int tmps=0;
	char *filename=NULL;
	GenBuf_t tmp_gb;

	BMD_attr_t **form=NULL;
	int form_count;
	GenBuf_t *mtd_buf=NULL;
	char *oid=NULL,*desc=NULL;


	long int i=0;
	long int id=0;

	_GLOBAL_debug_level=20;

	FOK(bmd_init());

	FOK(bmd_info_create(&bi));

	FOK(bmd_info_login_set(	"192.168.129.41",
				6783,
				NULL,
				-1,
				NULL,
				NULL,
				BMD_PROXY_TYPE_NONE,
				NULL,
				&bi));
	FOK(bmd_info_set_credentials_pfx("/home/Lesioo/Lesioo/trunk/src/libbmdsoap/server/gniazdownik.pfx","12345678",8,&bi));
	FOK(bmd_info_set_bmd_cert("bmd.cer",&bi));

	FOK(bmd_login(&bi));


	FOK(bmd_info_set_selected_role(&bi));


	FOK(bmd_logout(&bi));
	//FOK(bmd_info_destroy(&bi));

	FOK(bmd_end());

	return 0;
}
