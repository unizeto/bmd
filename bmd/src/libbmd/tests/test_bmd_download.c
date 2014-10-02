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

	system("clear");
	_GLOBAL_debug_level=0;

	FOK(bmd_init());

	FOK(bmd_info_create(&bi));

	FOK(bmd_info_login_set("127.0.0.1",6666,NULL, -1, NULL, NULL, BMD_PROXY_TYPE_NONE, NULL,&bi));
	FOK(bmd_info_set_credentials_pfx("marua.pfx","12345678",8,&bi));
	FOK(bmd_info_set_bmd_cert("bmd.cer",&bi));

	FOK(bmd_login(&bi));

	FOK(bmd_info_set_selected_role(&bi));

	/*pobieranie wszystkich plików*/
	for (i=0; i<=0; i++)
	{

		printf("pobieranie pliku...");
		FOK(bmd_lob_request_info_create(&li));
		long int size = 1000000000;
		/*id = 150000+i;*/
		id=8;
		FOK(bmd_lob_request_info_add_filename(L"Lesioo1234.out",&li, BMD_RECV_LOB,size));
		FOK(bmd_datagram_create(BMD_DATAGRAM_TYPE_GET_LO,&dtg));
		FOK(bmd_datagram_set_id(&id,&dtg));

		FOK(bmd_datagram_add_to_set(dtg,&req_dtg_set));
		FOK(bmd_send_request_lo(bi, req_dtg_set, 100, &resp_dtg_set, li));

		printf("response\n=========================\n");
		printf("0.\t%i\n",resp_dtg_set->bmdDatagramSetTable[i]->datagramStatus);

		FOK(bmd_datagramset_free(&req_dtg_set));
		FOK(bmd_datagramset_free(&resp_dtg_set));
		FOK(bmd_lob_request_info_destroy(&li));

		printf(" pobrany.\n");

	}

	FOK(bmd_logout(&bi));
	FOK(bmd_end());

	return 0;
}
