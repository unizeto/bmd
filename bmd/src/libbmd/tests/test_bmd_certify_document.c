#include <stdio.h>
#include <bmd/libbmdpr/bmd_datagram/bmd_datagram_set.h>
#include <bmd/libbmd/libbmd.h>
#include <bmd/libbmdts/libbmdts.h>
#include <stdlib.h>
#include <bmd/bmd_server/bmd_server.h>

#ifndef FOK
#define FOK(x) { int status=x; if(status)  {printf("Error %i in %s:%i\n",status,__FILE__, __LINE__); exit(status); }}
#endif

extern long _GLOBAL_debug_level;
int main(int argc, char ** argv)
{
	bmd_info_t *bi=NULL;
	bmdDatagram_t *dtg=NULL;
	bmdDatagramSet_t *req_dtg_set=NULL;
	bmdDatagramSet_t *resp_dtg_set=NULL;
	long int id=0;
	_GLOBAL_debug_level=0;

	assert(bmd_init() == BMD_OK);
	assert(bmd_info_create(&bi) == BMD_OK);
	//assert(bmd_info_create(&bi2) == BMD_OK);
	assert(bmd_info_login_set("127.0.0.1",6780,NULL, -1, NULL, NULL, BMD_PROXY_TYPE_NONE, NULL,&bi) == BMD_OK);
	/*FOK(bmd_info_set_credentials_pfx("marua.pfx","12345678",8,&bi));*/
	assert(bmd_info_set_credentials_pfx("wszczygiel032008.pfx","4321",4,&bi) == BMD_OK);
	assert(bmd_info_set_bmd_cert("bmd.cer",&bi) == BMD_OK);
	assert(bmd_login(&bi) == BMD_OK);

	assert(bmd_info_set_selected_role(&bi) == BMD_OK);

	assert(bmd_datagram_create(BMD_DATAGRAM_TYPE_CERTIFY_DOCUMENT, &dtg) == BMD_OK);

	id=1; // id cryptoobjectu, ktorego dotyczy dvcs
	assert(bmd_datagram_set_id(&id,&dtg) == BMD_OK);
	assert(bmd_datagram_add_to_set(dtg,&req_dtg_set) == BMD_OK);

	FOK(bmd_send_request(bi, req_dtg_set, 100, &resp_dtg_set, 0));
	printf("response status :%li\n", resp_dtg_set->bmdDatagramSetTable[0]->datagramStatus);

	bmd_datagramset_free(&req_dtg_set);
	bmd_datagramset_free(&resp_dtg_set);

	bmd_logout(&bi);
	bmd_end();

	return 0;
}
