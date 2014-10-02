#include <bmd/libbmdutils/libbmdutils.h>
#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdpki/libbmdpki.h>

#include <bmd/libbmdprotocol/libbmdprotocol.h>
#include <bmd/libbmdpr/bmd_datagram/bmd_datagram.h>
#include <bmd/libbmdpr/bmd_datagram/bmd_datagram_set.h>

#include <bmd/libbmdcms/libbmdcms.h>
#include <bmd/libbmdpr/bmd_resp/bmd_resp.h>
#include "../klient/vs2005/klient/VersionNo.h"

long bmd2_create_login_request(	bmd_crypt_ctx_t *sig_ctx,
						bmd_crypt_ctx_t *enc_ctx,
						GenBuf_t *cert_login_as,
                              	GenBuf_t **k,
						GenBuf_t **request)
{
bmdDatagram_t *dtg=NULL;
GenBuf_t *tmp=NULL;
GenBuf_t *tmp1=NULL;

	PRINT_INFO("LIBBMDPROTOCOLINF Creating login request\n");

	/************************/
	/* walidacja parametrow */
	/************************/
	if(sig_ctx==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(enc_ctx==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(request==NULL)		{	BMD_FOK(BMD_ERR_PARAM3);	}
	if((*request)!=NULL)		{	BMD_FOK(BMD_ERR_PARAM3);	}

	dtg=(bmdDatagram_t *)malloc(sizeof(bmdDatagram_t));
	if(dtg==NULL)		{	BMD_FOK(NO_MEMORY);	}

	PR_bmdDatagram_init(dtg);
	dtg->datagramType=BMD_DATAGRAM_TYPE_GET_FORM;

	BMD_FOK(bmd_generate_number(BMD_BN_BYTE_SIZE,k));
	BMD_FOK(bmd2_datagram_set_k(*k,&dtg));

	if(cert_login_as)
	{
		char cert_login_as_OID[]={OID_SYS_METADATA_CERT_LOGIN_AS};
		BMD_FOK(PR2_bmdDatagram_add_metadata(cert_login_as_OID,cert_login_as->buf, cert_login_as->size,SYS_METADATA,dtg,0,0,0, 0));
	}
	/*BMD_FOK(bmd_save_buf(cert_login_as, "/tmp/cert_not_encoded.der"));*/

	BMD_FOK(PR2_bmdDatagram_serialize(dtg,&tmp));
	PR2_bmdDatagram_free(&dtg);

	BMD_FOK(bmd_sign_data(tmp,sig_ctx,&tmp1,NULL));
	free_gen_buf(&tmp);

	BMD_FOK(bmd_encrypt_data(tmp1,enc_ctx,request,NULL));
	free_gen_buf(&tmp1);

	set_prefix_from_int(0, (*request)->size, request);



	return BMD_OK;
}

long bmd2_decode_login_request(	GenBuf_t **der_input,
						long twf_offset,
						bmd_crypt_ctx_t *dec_ctx,
						GenBuf_t **k,
						GenBuf_t **sig_cert,
						GenBuf_t **cert_login_as,
						long twf_der_input_free)
{
GenBuf_t *decrypted			= NULL;
GenBuf_t *tmp_content			= NULL;
bmdDatagram_t *dtg			= NULL;

	PRINT_INFO("LIBBMDPROTOCOLINF Decoding login request\n");

	/************************/
	/* walidacja parametrow */
	/************************/
	if(*der_input == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if((*der_input)->buf == NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if((*der_input)->size <= 0)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(dec_ctx==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(k==NULL)			{	BMD_FOK(BMD_ERR_PARAM3);	}
	if((*k)!=NULL)			{	BMD_FOK(BMD_ERR_PARAM3);	}
	if(sig_cert==NULL)		{	BMD_FOK(BMD_ERR_PARAM4);	}
	if((*sig_cert)!=NULL)		{	BMD_FOK(BMD_ERR_PARAM4);	}

	/*Skoro jeden to funkcja sama zwolni bufor*/
	BMD_FOK(bmd_decrypt_data(der_input, twf_offset, dec_ctx, &decrypted, NULL, twf_der_input_free));
	BMD_FOK(bmd_get_cert_and_content_from_signature(decrypted, sig_cert, &tmp_content));
	free_gen_buf(&decrypted);

	BMD_FOK(PR2_bmdDatagram_deserialize(tmp_content,&dtg));
	free_gen_buf(&tmp_content);


	if((int)(dtg->protocolVersion/100)!=BMD_PROTOCOL_VERSION/100)
	{
		PRINT_ERROR("LIBBMDPROTOCOLERR Client protocol version: %li   Server protocol version: %li\n",dtg->protocolVersion,(long)BMD_PROTOCOL_VERSION);
		BMD_FOK(VERSION_NUMBER_ERR);
	}

	if(dtg->datagramType!=BMD_DATAGRAM_TYPE_GET_FORM)
	{
		BMD_FOK(BMD_ERR_NOTAVAIL);
	}

	/*PR_bmdDatagram_print(dtg,0);*/
	BMD_FOK(bmd2_datagram_get_k(dtg,k));

	/* pobranie z datagramu metadanej przedstawiajacej certyfikat w imieniu ktorego nastepuje logowanie */
	BMD_FOK(bmd2_datagram_get_cert_login_as(dtg,cert_login_as));
	PR2_bmdDatagram_free(&dtg);

	return BMD_OK;
}
