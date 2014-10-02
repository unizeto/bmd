#include <bmd/libbmd/libbmd.h>
#include <stdlib.h>
#include <string.h>
#include <bmd/common/bmd-sds.h>
#ifndef FOK
#define FOK(x) { int status=x; if(status)  {printf("Error %i in %s:%i\n",status,__FILE__, __LINE__); exit(status); }}
#endif


#include <bmd/libbmdpkcs12/libbmdpkcs12.h>

extern long _GLOBAL_debug_level;

long IDpliku;
int wyslij_plik( void);
int wyslij_plik_lo( void);
int pobierz_plik( void);
int pobierz_plik_lo( void);

int main(int argc, char ** argv)
{
	_GLOBAL_debug_level=0;

	FOK( wyslij_plik_lo() );
	FOK( pobierz_plik_lo() );

/*	FOK( wyslij_plik() );
	FOK( pobierz_plik() );*/
	return 0;
}

int wyslij_plik( void)
{
	bmd_info_t * bi = NULL;
	bmdDatagram_t *dtg=NULL;
	bmdDatagramSet_t *req_dtg_set=NULL;
	bmdDatagramSet_t *resp_dtg_set=NULL;

	FOK(bmd_init());

	FOK(bmd_info_create(&bi));
	FOK(bi==NULL);
	FOK(bmd_info_login_set("192.168.129.113",6666,NULL, -1, NULL, NULL, BMD_PROXY_TYPE_NONE,NULL, &bi));

	FOK(bmd_info_set_credentials_pfx("marua.pfx","12345678",8,&bi));
	FOK(bmd_info_set_bmd_cert("bmd.cer",&bi));

	FOK(bmd_login(&bi));

	FOK(bmd_info_set_selected_role(&bi));

	FOK(bmd_datagram_create(BMD_DATAGRAM_TYPE_INSERT,&dtg));

	FOK(bmd_datagram_add_file("test-RO.txt",&dtg));
	FOK(bmd_datagram_add_to_set(dtg,&req_dtg_set));

	FOK(bmd_send_request(bi,req_dtg_set, 100, &resp_dtg_set,0));

	FOK(bmd_datagramset_free(&req_dtg_set));
	FOK(bmd_datagramset_free(&resp_dtg_set));
	FOK(bmd_logout(&bi));
	FOK(bmd_info_destroy(&bi));	//- te działania są wykonywane przez bmd_logout()
	FOK(bmd_end());

	return BMD_OK;
}

int wyslij_plik_lo( void)
{
	bmd_info_t *bi=NULL;
	bmdDatagram_t *dtg=NULL;
	bmdDatagramSet_t *req_dtg_set=NULL;
	bmdDatagramSet_t *resp_dtg_set=NULL;
	lob_request_info_t *li=NULL;

	FOK(bmd_init());
	FOK(bmd_info_create(&bi));
	FOK(bmd_info_login_set("192.168.129.113",6666,NULL, -1, NULL, NULL, BMD_PROXY_TYPE_NONE, &bi));

	FOK(bmd_info_set_credentials_pfx("marua.pfx","12345678",8,&bi));
	FOK(bmd_info_set_bmd_cert("bmd.cer",&bi));
	FOK(bmd_login(&bi));
	FOK(bmd_info_set_selected_role(&bi));

	FOK(bmd_lob_request_info_create(&li));
	FOK(bmd_lob_request_info_add_filename(L"test-RO.txt",&li, BMD_SEND_LOB,0));
	FOK(bmd_datagram_create(BMD_DATAGRAM_TYPE_INSERT_LO,&dtg));
	FOK(bmd_datagram_set_filename(L"test-RO.txt",&dtg));

	FOK(bmd_datagram_add_to_set(dtg,&req_dtg_set));
	FOK(bmd_send_request_lo(bi,req_dtg_set, 100, &resp_dtg_set,0,li));

	bmdDatagram_t * temp1 ;
	GenBuf_t * temp2;
	temp1 = ((*resp_dtg_set).bmdDatagramSetTable[0]);
	temp2 = temp1->protocolDataFileId;
	PRINT_DEBUG("\nID pliku %s\n\n",temp2->buf) ;
	IDpliku = atol( temp2->buf );
	PRINT_DEBUG("\nID pliku %i\n\n",(int)IDpliku) ;

	FOK(bmd_datagramset_free(&req_dtg_set));
	FOK(bmd_datagramset_free(&resp_dtg_set));
	FOK(bmd_logout(&bi));
	FOK(bmd_info_destroy(&bi));
	FOK(bmd_lob_request_info_destroy(&li));
	FOK(bmd_end());

	return BMD_OK;
}

int pobierz_plik( void)
{
	bmd_info_t *bi=NULL;
	bmdDatagram_t *dtg=NULL;
	bmdDatagramSet_t *req_dtg_set=NULL;
	bmdDatagramSet_t *resp_dtg_set=NULL;
	long id;

	FOK(bmd_init());
	FOK(bmd_info_create(&bi));
	FOK(bmd_info_login_set("192.168.129.113",6666,NULL, -1, NULL, NULL, BMD_PROXY_TYPE_NONE, &bi));

	FOK(bmd_info_set_credentials_pfx("marua.pfx","12345678",8,&bi));
	FOK(bmd_info_set_bmd_cert("bmd.cer",&bi));
	FOK(bmd_login(&bi));
	FOK(bmd_info_set_selected_role(&bi));
	FOK(bmd_datagram_create(BMD_DATAGRAM_TYPE_GET,&dtg));
	id=IDpliku;
	FOK(bmd_datagram_set_id(&id,&dtg));
	FOK(bmd_datagram_add_to_set(dtg,&req_dtg_set));
	FOK(bmd_send_request(bi,req_dtg_set, 100, &resp_dtg_set,0));
	FOK(bmd_datagram_save_file("pobr.txt",dtg));
	FOK(bmd_datagramset_free(&req_dtg_set));
	FOK(bmd_datagramset_free(&resp_dtg_set));
	FOK(bmd_logout(&bi));
	FOK(bmd_info_destroy(&bi));
	FOK(bmd_end());

	return BMD_OK;
}

int pobierz_plik_lo()
{
	bmd_info_t *bi=NULL;
	bmdDatagram_t *dtg=NULL;
	bmdDatagramSet_t *req_dtg_set=NULL;
	bmdDatagramSet_t *resp_dtg_set=NULL;
	lob_request_info_t *li=NULL;
	unsigned long int size;
	long id;
	FOK(bmd_init());
	FOK(bmd_info_create(&bi));
	FOK(bmd_info_login_set("192.168.129.113",6666,NULL, -1, NULL, NULL, BMD_PROXY_TYPE_NONE, &bi));

	FOK(bmd_info_set_credentials_pfx("marua.pfx","12345678",8,&bi));
	FOK(bmd_info_set_bmd_cert("bmd.cer",&bi));

	FOK(bmd_login(&bi));
	FOK(bmd_info_set_selected_role(&bi));

	FOK(bmd_lob_request_info_create(&li));
	size=100000000;
	id=IDpliku;
	//id=81948;
	FOK(bmd_lob_request_info_add_filename(L"pobrane.txt",&li, BMD_RECV_LOB,size));
	FOK(bmd_datagram_create(BMD_DATAGRAM_TYPE_GET_LO,&dtg));
	FOK(bmd_datagram_set_id(&id,&dtg));

	FOK(bmd_datagram_add_to_set(dtg,&req_dtg_set));
	FOK(bmd_send_request_lo(bi,req_dtg_set, 100, &resp_dtg_set, li));

	FOK(bmd_datagramset_free(&req_dtg_set));
	FOK(bmd_datagramset_free(&resp_dtg_set));
	FOK(bmd_logout(&bi));
	FOK(bmd_info_destroy(&bi));
	FOK(bmd_lob_request_info_destroy(&li));
	FOK(bmd_end());

	return BMD_OK;
}
