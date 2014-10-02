#include <stdio.h>
#include <bmd/libbmdpr/bmd_datagram/bmd_datagram_set.h>
#include <bmd/libbmd/libbmd.h>
#include <bmd/libbmdconfig/libbmdconfig.h>
#include <bmd/libbmdts/libbmdts.h>
//#include <conf_sections.h>
#include <stdlib.h>
#include <bmd/bmd_server/bmd_server.h>

#ifndef FOK
#define FOK(x) { int status=x; if(status)  {printf("Error %i in %s:%i\n",status,__FILE__, __LINE__); exit(status); }}
#endif

extern long _GLOBAL_debug_level;
int main(int argc, char ** argv)
{
#if 0
	/****************************************/
	/*					*/
	/****************************************/
	bmd_signature_params_t *sig_params=NULL;
	GenBuf_t *plik=NULL;
	GenBuf_t *plik2=NULL;
	GenBuf_t *podpis=NULL;
	bmd_crypt_ctx_t *ctx=NULL;
	int status=0;

	FOK(bmd_init());
	FOK(bmd_load_binary_content("/home/Lesioo/Lesioo/trunk/ChangeLog",&plik));
	FOK(bmd_load_binary_content("/home/Lesioo/Lesioo/trunk/juppi4.txt",&plik2));
	printf("plik: %i %s\n",plik->size,plik->buf);


	FOK(SigningParams_Create(NULL, NULL, &sig_params));
	sig_params->signature_flags=BMD_CMS_SIG_EXTERNAL;

	FOK(bmd_set_ctx_file("marua.pfx","12345678",8,&ctx));
	FOK(bmd_sign_data(plik, ctx, &podpis, (void*)sig_params));

	printf("podpis: %i %s\n",podpis->size,podpis->buf);


	status=bmd_verify_sig_integrity(podpis,plik);
	if (status<BMD_OK)
	{
		printf("podpis zweryfikowany - nie poprawny\n");
	}
	else
	{
		printf("podpis zweryfikowany - poprawny\n");
	}



	status=bmd_verify_sig_integrity(podpis,plik2);
	if (status<BMD_OK)
	{
		printf("podpis zweryfikowany - nie poprawny\n");
	}
	else
	{
		printf("podpis zweryfikowany - poprawny\n");
	}






	FOK(bmd_end());




	return 0;


#endif






	bmd_info_t *bi=NULL;
	bmd_info_t *bi2=NULL;
	bmdDatagram_t *dtg=NULL;
	bmdDatagramSet_t *req_dtg_set=NULL;
	bmdDatagramSet_t *resp_dtg_set=NULL;
	int i=0;
	int id=0;
	_GLOBAL_debug_level=0;
	FOK(bmd_init());
	FOK(bmd_info_create(&bi));
	FOK(bmd_info_create(&bi2));
	FOK(bmd_info_login_set("127.0.0.1",6666,NULL, -1, NULL, NULL, BMD_PROXY_TYPE_NONE, NULL,&bi));
	/*FOK(bmd_info_set_credentials_pfx("marua.pfx","12345678",8,&bi));*/
	/*FOK(bmd_info_set_credentials_pfx("cert.pfx","4321",4,&bi2));*/
	FOK(bmd_info_set_credentials_pfx("marua.pfx","12345678",8,&bi));
	FOK(bmd_info_set_bmd_cert("bmd.cer",&bi));
	FOK(bmd_login(&bi));
	FOK(bmd_info_set_selected_role(&bi));
	for (i=0; i<1; i++)
	{
		FOK(bmd_datagram_create(BMD_DATAGRAM_TYPE_VALIDATE_SIGNATURE,&dtg));
		//FOK(bmd_datagram_create(BMD_DATAGRAM_TYPE_VALIDATE_CERTIFICATE,&dtg));

		/*dtg->protocolData=(GenBuf_t *)malloc(sizeof(GenBuf_t)+2);
		memset(dtg->protocolData,0,sizeof(GenBuf_t)+1);

		dtg->protocolData->buf=(char *)malloc(bi2->crypto_info.log_ctx->file->cert->size+2);
		memset(dtg->protocolData->buf,0,bi2->crypto_info.log_ctx->file->cert->size+1);

		memcpy(dtg->protocolData->buf,bi2->crypto_info.log_ctx->file->cert->buf,bi2->crypto_info.log_ctx->file->cert->size);
		dtg->protocolData->size=bi2->crypto_info.log_ctx->file->cert->size;*/

		id=26447;
		FOK(bmd_datagram_set_id(&id,&dtg));

		//printf("%i\n",dtg->protocolData->size);
		//printf("%s\n",dtg->protocolData->buf);

		/*printf("%i\n",dtg->protocolDataFileId->size);
		printf("%s\n\n",dtg->protocolDataFileId->buf);
		*/
		FOK(bmd_datagram_add_to_set(dtg,&req_dtg_set));

	}
	FOK(bmd_send_request(bi, req_dtg_set, 100, &resp_dtg_set, 0, deserialisation_max_memory));

	printf("\n\n\nresponse:\n-------------\n\n");

	for (i=0; i<resp_dtg_set->bmdDatagramSetSize; i++)
	{
		printf("%i.    %i      %s     %s\n",i+1,resp_dtg_set->bmdDatagramSetTable[i]->datagramStatus, req_dtg_set->bmdDatagramSetTable[i]->protocolDataFileId->buf,GetErrorMsg(resp_dtg_set->bmdDatagramSetTable[i]->datagramStatus));
	}


	FOK(bmd_datagramset_free(&req_dtg_set));
	FOK(bmd_datagramset_free(&resp_dtg_set));



	FOK(bmd_logout(&bi, deserialisation_max_memory));
	FOK(bmd_end());

	return 0;
}
