#include <stdio.h>
#include <bmd/libbmdpr/bmd_datagram/bmd_datagram_set.h>
#include <bmd/libbmd/libbmd.h>
#include <bmd/libbmdconfig/libbmdconfig.h>
#include <bmd/libbmdts/libbmdts.h>
//#include <conf_sections.h>
#include <stdlib.h>

#ifndef FOK
#define FOK(x) { int status=x; if(status)  {printf("Error %i in %s:%i\n",status,__FILE__, __LINE__); exit(status); }}
#endif

extern long _GLOBAL_debug_level;
int main(int argc, char ** argv)
{
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

char *host=NULL;
char *pfx=NULL;
char * pfx_pass=NULL;
char *cer=NULL;
int port=0;


	_GLOBAL_debug_level=0;
	if (argc==15)
	{
		for (i=1; i<argc; i++)
		{
			if (strcmp(argv[i],"-d")==0)
			{
				if (argc>i+1) _GLOBAL_debug_level=atoi(argv[i+1]);
			}
			if (strcmp(argv[i],"-h")==0)
			{
				if (argc>i+1) asprintf(&host,"%s",argv[i+1]);
			}
			if (strcmp(argv[i],"-pr")==0)
			{
				if (argc>i+1) port=atoi(argv[i+1]);
			}
			if (strcmp(argv[i],"-pfx")==0)
			{
				if (argc>i+1) asprintf(&pfx,"%s",argv[i+1]);
			}
			if (strcmp(argv[i],"-pfx_p")==0)
			{
				if (argc>i+1) asprintf(&pfx_pass,"%s",argv[i+1]);
			}
			if (strcmp(argv[i],"-c")==0)
			{
				if (argc>i+1) asprintf(&cer,"%s",argv[i+1]);
			}
			if (strcmp(argv[i],"-p")==0)
			{
				if (argc>i+1) id=atoi(argv[i+1]);
			}
		}
	}
	else
	{
		printf("%s\n",argv[0]);
		printf("\nniepoprawne wywołanie\n\nuzyj ponizszych parametrow\n");
		printf("-------------------------------------------------------\n");
		printf("\t-d liczba\tpoziom logowania\n");
		printf("\t-h adres\thost BMD\n");
		printf("\t-pr liczba\tport hosta BMD\n");
		printf("\t-pfx plik .pfx\tcertyfikat logowania\n");
		printf("\t-pfx_p hasło\thasło do certyfikatu logowania\n");
		printf("\t-c plik .cer\tklucz publiczny serwera\n");
		printf("\t-p liczba\tid usuwanego pliku\n");
		printf("-------------------------------------------------------\n");
		return -1;
	}

	FOK(bmd_init());
	FOK(bmd_info_create(&bi));
	FOK(bmd_info_login_set(host,port,NULL, -1, NULL, NULL, BMD_PROXY_TYPE_NONE, NULL,&bi));
	FOK(bmd_info_set_credentials_pfx(pfx,pfx_pass,strlen(pfx_pass),&bi));
	FOK(bmd_info_set_bmd_cert(cer,&bi));
	FOK(bmd_login(&bi));
	FOK(bmd_info_set_selected_role(&bi));
	printf("deleting file nr %li... ",id);
	FOK(bmd_datagram_create(BMD_DATAGRAM_TYPE_DELETE,&dtg));
	FOK(bmd_datagram_set_id(&id,&dtg));
	FOK(bmd_datagram_add_to_set(dtg,&req_dtg_set));
	FOK(bmd_send_request(bi, req_dtg_set, 100, &resp_dtg_set, 0));
	printf("done with status = %li \n",resp_dtg_set->bmdDatagramSetTable[0]->datagramStatus);
	FOK(bmd_datagramset_free(&req_dtg_set));
	FOK(bmd_datagramset_free(&resp_dtg_set));
	FOK(bmd_logout(&bi));
	FOK(bmd_end());
	return 0;
}
