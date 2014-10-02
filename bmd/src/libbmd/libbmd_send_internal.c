#define _FILE_OFFSET_BITS 64
#include <bmd/common/bmd-const.h>
#include <bmd/common/bmd-common.h>
#include <bmd/libbmd/libbmd.h>
#include <bmd/libbmdpr/bmd_datagram/asn1clib/BMDDatagram.h>
#include <bmd/libbmdnet/libbmdnet.h>
#include <bmd/common/bmd-sds.h>
#include <bmd/libbmdutils/libbmdutils.h>
#include <bmd/libbmdpr/bmd_datagram/bmd_datagram.h>
#include <bmd/libbmdpr/bmd_datagram/bmd_datagram_set.h>
#include <bmd/libbmdprotocol/libbmdprotocol.h>
#include <bmd/libbmdlob/libbmdlob.h>

#include <bmd/libbmd/libbmd_internal.h>

#include <bmd/libbmdprotocol/libbmdprotocol.h>
#include <bmd/libbmderr/libbmderr.h>
#ifdef WIN32
#pragma warning(disable:4055)
#pragma warning(disable:4100)
#endif
bmdnet_handler_ptr __bmd_create_handler(bmd_info_t *bmd_info)
{
    long status;
    bmdnet_handler_ptr session_handle = NULL;

    if(bmd_info==NULL)
		return NULL;

    status= bmdnet_create(&session_handle);
    if(status!=0)
		return NULL;

	if( bmd_info->proxy_info.proxy_url != NULL )
	{
		long auth_type=BMDNET_PROXY_AUTH_NONE;
		if( bmd_info->proxy_info.proxy_auth_type != NULL )
		{
			if( strcmp(bmd_info->proxy_info.proxy_auth_type,"ntlm") == 0 )
				auth_type=BMDNET_PROXY_AUTH_NTLM;
			else
			if( strcmp(bmd_info->proxy_info.proxy_auth_type,"basic") == 0 )
				auth_type=BMDNET_PROXY_AUTH_BASIC;
			else
			{
				PRINT_ERROR("unsupported auth type for proxy\n");
				return NULL;
			}
		}
		status=bmdnet_set_connection_params(bmd_info->proxy_info.proxy_url,
											bmd_info->proxy_info.proxy_port,
											bmd_info->proxy_info.proxy_type,
											auth_type,
											bmd_info->proxy_info.proxy_username,
											bmd_info->proxy_info.proxy_password,
											session_handle);
		if( status != BMD_OK )
		{
			PRINT_ERROR("Failed to set connection parameters\n");
			return NULL;
		}
	}
    return session_handle;
}

long __bmd_prepare_request(	bmd_info_t *bmd_info,
				bmdDatagram_t *request_datagram,
				bmd_crypt_ctx_t **lob_crypt_ctx)
{
long status			= 0;
GenBuf_t *tmp			= NULL;

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if (bmd_info==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (bmd_info->crypto_info.log_ctx==NULL){	BMD_FOK(BMD_ERR_PARAM1);	}
	if (request_datagram==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	/* sprawdzenie czy poprawne metadane sa ustawione w datagramie */
	BMD_FOK(check_metadata_in_datagram(bmd_info,request_datagram));

	/* wykasowanie z datagramu zbednych metadanych */
	BMD_FOK(strip_metadata_in_datagram(&request_datagram));

	/* w przypadku requesta normalnej wysylki szyfrujemy dane w protocolData */

	if( 	(request_datagram->datagramType==BMD_DATAGRAM_GET_ONLY_FILE) ||
	    	(request_datagram->datagramType==BMD_DATAGRAM_TYPE_GET_LO) ||
	    	(request_datagram->datagramType==BMD_DATAGRAM_TYPE_GET_PKI_LO) ||
	    	(request_datagram->datagramType==BMD_DATAGRAM_GET_CGI_LO) ||
			(request_datagram->datagramType==BMD_DATAGRAM_TYPE_GET_ARCHIVE_FILE_VERSION_LO) ||
		(request_datagram->datagramType==BMD_DATAGRAM_DIR_GET_DETAIL))
	{

		bmd_crypt_ctx_t *sym_ctx=NULL;
		BMD_FOK(bmd_set_ctx_sym(&sym_ctx,BMD_CRYPT_ALGO_DES3,NULL,NULL));
		BMD_FOK(bmd_create_der_bmdKeyIV(BMD_CRYPT_ALGO_RSA,BMD_CRYPT_ALGO_DES3,bmd_info->crypto_info.bmd_ctx,sym_ctx->sym->key,sym_ctx->sym->IV,NULL,&tmp));
		BMD_FOK(bmd2_datagram_set_symkey(tmp,&request_datagram));

		free_gen_buf(&tmp);
	}


	if( 	(request_datagram->datagramType==BMD_DATAGRAM_TYPE_INSERT) ||
		(request_datagram->datagramType==BMD_DATAGRAM_TYPE_INSERT_LO) ||
		(request_datagram->datagramType==BMD_DATAGRAM_TYPE_UPDATE_FILE_VERSION_LO) ||
		(request_datagram->datagramType==BMD_DATAGRAM_DIR_CREATE) ||
		(request_datagram->datagramType==BMD_DATAGRAM_LINK_CREATE) ||
		(request_datagram->datagramType==BMD_DATAGRAM_TYPE_CREATE_CASCADE_LINKS) )
	{

		/* dodanie kontrolek niewidocznych */
		status=add_unvisible_kontrolki(bmd_info,&request_datagram);
		if(status!=BMD_OK)
		{
			return BMD_ERR_OP_FAILED;
		}

		if (	(request_datagram->datagramType==BMD_DATAGRAM_TYPE_INSERT) ||
			(request_datagram->datagramType==BMD_DATAGRAM_DIR_CREATE) ||
			(request_datagram->datagramType==BMD_DATAGRAM_LINK_CREATE) ||
			(request_datagram->datagramType==BMD_DATAGRAM_TYPE_CREATE_CASCADE_LINKS) )
		{

			bmd_crypt_ctx_t *sym_ctx=NULL;
			GenBuf_t *enc=NULL;

			if (request_datagram->protocolData==NULL)
			{
				BMD_FOK(BMD_ERR_FORMAT);
			}

			/* NIE SZYFRUJEMY JUZ DO CMS, A KLUCZEM SYMETRYCZNYM */
			/*BMD_FOK(bmd_encrypt_data(request_datagram->protocolData,bmd_info->crypto_info.bmd_ctx,&ciphered_data,NULL));*/

			/* NOWE SZYFROWANIE ZA POMOCA KLUCZA SYMETRYCZNEGO */
			BMD_FOK(bmd_set_ctx_sym(&sym_ctx,BMD_CRYPT_ALGO_DES3,NULL,NULL));
			if (request_datagram->datagramType!=BMD_DATAGRAM_ADD_SIGNATURE)
			{

				BMD_FOK(bmd_symmetric_encrypt(request_datagram->protocolData,&sym_ctx,&enc,NULL));
				free_gen_buf(&(request_datagram->protocolData));

				set_gen_buf2(enc->buf,enc->size,&(request_datagram->protocolData));
				free_gen_buf(&enc);

			}

			BMD_FOK(bmd_create_der_bmdKeyIV(BMD_CRYPT_ALGO_RSA,BMD_CRYPT_ALGO_DES3,bmd_info->crypto_info.bmd_ctx,sym_ctx->sym->key,sym_ctx->sym->IV,NULL,&tmp));

			BMD_FOK(bmd2_datagram_set_symkey(tmp,&request_datagram));

			free_gen_buf(&tmp);

		}

	}

	/* w przypadku requesta wysylki lobow generujemy klucze symetryczne i wektory IV i umieszczamy je w datagramie */
	if(	request_datagram->datagramType==BMD_DATAGRAM_TYPE_INSERT_LO ||
		request_datagram->datagramType==BMD_DATAGRAM_TYPE_UPDATE_FILE_VERSION_LO)
	{
		/* stworzenie kontekstu do szyfrowania z zaznaczeniem ze szyfrowanie bedzie strumieniowe */
		BMD_FOK(bmd_set_ctx_sym(lob_crypt_ctx,BMD_CRYPT_ALGO_DES3,NULL,NULL));
		(*lob_crypt_ctx)->sym->sym_params=BMD_SYM_MULTI_ROUND;
		BMD_FOK(bmd_create_der_bmdKeyIV(BMD_CRYPT_ALGO_RSA,BMD_CRYPT_ALGO_DES3,bmd_info->crypto_info.bmd_ctx,(*lob_crypt_ctx)->sym->key,(*lob_crypt_ctx)->sym->IV,NULL,&tmp));
		BMD_FOK(bmd2_datagram_set_symkey(tmp,&request_datagram));
		free_gen_buf(&tmp);
	}

	/* dodanie wlasciciela wysylanych danych */
	if(bmd_info->cert_login_as)
	{

		BMD_FOK(get_owner_from_cert(bmd_info->cert_login_as,&(request_datagram->protocolDataOwner)));
		BMD_FOK(PR2_bmdDatagram_add_metadata(OID_SYS_METADATA_CERT_LOGIN_AS,bmd_info->cert_login_as->buf, bmd_info->cert_login_as->size, SYS_METADATA,request_datagram,0,0,0, 0));
	}
	else
	{

		if(bmd_info->crypto_info.log_ctx->file)
		{
			BMD_FOK(get_owner_from_cert(bmd_info->crypto_info.log_ctx->file->cert,&(request_datagram->protocolDataOwner)));
		}
		else
		{

			if(bmd_info->crypto_info.log_ctx->pkcs11)
			{
				BMD_FOK(get_owner_from_cert(bmd_info->crypto_info.log_ctx->pkcs11->cert,&(request_datagram->protocolDataOwner)));
			}
		}

	}

	/* dodanie losowego ID */
	request_datagram->randId=123456;

	/* ustawienie metadanej systemowej OID_RFC3281_ROLE w datagramie */
	/*
	if(bmd_info->selected_role)
	{
		BMD_FOK(PR2_bmdDatagram_add_metadata(OID_RFC3281_ROLE,bmd_info->selected_role->buf, bmd_info->selected_role->size, SYS_METADATA,request_datagram,0,0,0, 0));
	}
	*/

	/* ustawienie metadanej systemowej bedacej hashem z klucza symetrycznego */
	if(bmd_info->hcsb!=NULL)
	{
		char hcsb_OID[]={OID_SYS_METADATA_DTG_SYMKEY_HASH};
		BMD_FOK(PR2_bmdDatagram_add_metadata(hcsb_OID,bmd_info->hcsb->buf,bmd_info->hcsb->size,SYS_METADATA,request_datagram,0,0,0, 0));
	}

#if 0
	/* msilewicz - blok testowych metadanych przekraczajacych uprawnienia uzytkownika */
	{
		/* najpierw grupa niezgodna z uprawnieniami */
		long group_OID[]={OID_RFC3281_GROUP};
		long sc_OID[]={OID_SYS_METADATA_SEC_CAT};
		long sl_OID[]={OID_SYS_METADATA_BMD_SEC_LEVELS};
		char *fake="1.2.616.1.113527.4.3.2.3.111";
		char *fake1="1.2.616.1.113527.4.3.2.1.3";
		char *fake2="1.2.616.1.113527.4.3.2.4.2";
		PR2_bmdDatagram_add_metadata(group_OID,sizeof(group_OID),
											(char *)fake,strlen(fake),
										    SYS_METADATA,request_datagram,0,0,0);
		/* security category OK, ale... */
		PR2_bmdDatagram_add_metadata(sc_OID,sizeof(sc_OID),
											(char *)fake1,strlen(fake1),
										    SYS_METADATA,request_datagram,0,0,0);
		/* ... security level niezgodne z kategoriami */
		PR2_bmdDatagram_add_metadata(sl_OID,sizeof(sl_OID),
											(char *)fake2,strlen(fake2),
										    SYS_METADATA,request_datagram,0,0,0);
	}
#endif
	/*PR_bmdDatagram_print(request_datagram,0);*/
	return BMD_OK;
}

long __bmd_prepare_request_buffer(	bmdDatagramSet_t *request_datagram_set,
					bmd_info_t *bmd_info,
					long twf_start_index,
					long twf_transmission_count,

					GenBuf_t ***output,
					long *twf_no_of_buffers,

					GenBuf_t *tmp_ki_mod_n_client,
					long twf_free_datagrams_flag)
{
/*GenBuf_t *request_der_encoded		= NULL;*/
GenBuf_t **twl_output_buffers		= NULL;
long twl_no_of_buffers			= 0;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (request_datagram_set==NULL)					{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (bmd_info==NULL)						{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (output==NULL)						{	BMD_FOK(BMD_ERR_PARAM5);	}
	if ((*output)!=NULL)						{	BMD_FOK(BMD_ERR_PARAM5);	}
	if (tmp_ki_mod_n_client == NULL)				{	BMD_FOK(BMD_ERR_PARAM6);	}
	if (twf_free_datagrams_flag < 0)				{	BMD_FOK(BMD_ERR_PARAM7);	}
	if (twf_free_datagrams_flag > 1)				{	BMD_FOK(BMD_ERR_PARAM7);	}


	/* przygotowanie requesta */
	BMD_FOK(PR2_bmdDatagramSet_serialize(	request_datagram_set,
						tmp_ki_mod_n_client, /*&request_der_encoded,*/
						&twl_output_buffers,
						&twl_no_of_buffers,
						twf_free_datagrams_flag,
						twf_start_index,
						twf_transmission_count));
	free_gen_buf(&(bmd_info->k0));

	/* tmp_ki_mod_n_client to wartosc k2 przy pierwszym sesyjnym requescie */
	BMD_FOK(bmd2_create_session_request(	/*&request_der_encoded,*/
						twl_output_buffers,
						twl_no_of_buffers,
						bmd_info->crypto_info.sym_ctx,
						bmd_info->request_id,
						output,
						twf_no_of_buffers));

	free(twl_output_buffers); twl_output_buffers = NULL;
	/* *twf_no_of_buffers = twl_no_of_buffers;*/

	PRINT_VDEBUG("LIBBMDVDEBUG No more datagrams to process.\n");

	return BMD_OK;
}

long __bmd_request_send(	bmdnet_handler_ptr *session_handle,
				/*GenBuf_t *request_to_send,*/
				GenBuf_t **request_to_send, long twf_no_of_buffers,
				bmd_info_t *bmd_info,
				long twf_connect)
{
long i				= 0;
	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if(session_handle==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(request_to_send==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	/*if(request_to_send->buf==NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(request_to_send->size<=0)		{	BMD_FOK(BMD_ERR_PARAM2);	}*/
	if(twf_no_of_buffers < 0)		{	BMD_FOK(BMD_ERR_PARAM3);	}
	if(bmd_info==NULL)			{	BMD_FOK(BMD_ERR_PARAM4);	}
	if(twf_connect < 0 || twf_connect > 1)	{	BMD_FOK(BMD_ERR_PARAM5);	}

	/* wyslanie requesta */
	if(twf_connect == 1)
	{
		if( (*session_handle) != NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}

		*session_handle = __bmd_create_handler(bmd_info);
 		if ((*session_handle) == NULL)		{	BMD_FOK(BMD_ERR_OP_FAILED);	}

		BMD_FOK_CHG(bmdnet_connect(*session_handle,bmd_info->bmd_address,bmd_info->bmd_port), BMD_ERR_NET_CONNECT);
	}
	else
	{
		if( (*session_handle) == NULL)  {	BMD_FOK(BMD_ERR_PARAM1);	}
	}

	for(i = 0; i < twf_no_of_buffers; i++)
	{
		BMD_FOK_CHG(bmdnet_send(*session_handle,request_to_send[i]->buf, request_to_send[i]->size),
		BMD_ERR_NET_WRITE);
		free_gen_buf(&(request_to_send[i]));
	}

	return BMD_OK;
}

long __bmd_response_recv(	bmdnet_handler_ptr *session_handle,
					bmd_info_t *bmd_info,
					GenBuf_t **received_data)
{
/*long status;*/

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if(session_handle==NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if((*session_handle)==NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(bmd_info==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(received_data==NULL)		{	BMD_FOK(BMD_ERR_PARAM3);	}
	if((*received_data)!=NULL)	{	BMD_FOK(BMD_ERR_PARAM3);	}

	/******************************/
	/*	odebranie odpowiedzi	*/
	/******************************/
	/**received_data=(GenBuf_t *)malloc(sizeof(GenBuf_t));
	if((*received_data)==NULL)	{	BMD_FOK(BMD_ERR_MEMORY);	}

	(*received_data)->buf=NULL;*/
	BMD_FOK(bmd_get_from_socket(*session_handle, received_data));
	/*if (status<BMD_OK)
	{
		return status;
	}*/

/*
	(*received_data)->size=status;
*/
	bmd_info->conn_handler = NULL;

	return BMD_OK;
}

long __bmd_decode_response_buffer(	GenBuf_t **input,
					bmd_info_t *bmd_info,
					GenBuf_t *tmp_ki_mod_n_client,
					bmdDatagramSet_t **response_datagram_set,
					GenBuf_t **new_ki_mod_n,
					long *twf_bmdDatagramsTransmissionCount,
					long *twf_bmdDatagramsAllCount,
					long *twf_memory_guard,
					long deserialisation_max_memory)
{
GenBuf_t *tmp1					= NULL;
uint32_t tmp_request_id				= 0;
bmdDatagram_t *dtg				= NULL;
long status					= 0;

	PRINT_INFO("LIBBMDINF Decoding response.\n");
	/************************/
	/* walidacja parametrow */
	/************************/
	if (input==NULL)					{	BMD_FOK(BMD_ERR_PARAM1);	}
	if ((*input)==NULL)					{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (bmd_info==NULL)					{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (tmp_ki_mod_n_client==NULL)				{	BMD_FOK(BMD_ERR_PARAM3);	}
	if (tmp_ki_mod_n_client->buf==NULL)			{	BMD_FOK(BMD_ERR_PARAM3);	}
	if (tmp_ki_mod_n_client->size<=0)			{	BMD_FOK(BMD_ERR_PARAM3);	}
	if (response_datagram_set==NULL)			{	BMD_FOK(BMD_ERR_PARAM4);	}
	if ((*response_datagram_set)!=NULL)			{	BMD_FOK(BMD_ERR_PARAM4);	}
	if (new_ki_mod_n != NULL)
	{
		if(*new_ki_mod_n != NULL)			{	 BMD_FOK(BMD_ERR_PARAM5);	}
	}
	if (twf_bmdDatagramsTransmissionCount == NULL)		{	BMD_FOK(BMD_ERR_PARAM6);	}
	if (twf_bmdDatagramsAllCount == NULL)			{	BMD_FOK(BMD_ERR_PARAM7);	}
	if (deserialisation_max_memory<0)			{	BMD_FOK(BMD_ERR_PARAM8);	}

	BMD_FOK(strip_prefix(input,&tmp_request_id,&tmp1));
	status = tmp_request_id;
	PRINT_DEBUG("LIBBMDDEBUG Request id: %li.\n", status);

	if(bmd_info->request_id!=(long)tmp_request_id)
	{
		if(tmp_request_id==0) /* na pewno jest blad */
		{
			long err;
			GenBuf_t *dec=NULL;
			GenBuf_t *unwrapped=NULL;
			GenBuf_t *to_compare_ki_mod_n=NULL,*server_ki_mod_n=NULL,*tmp=NULL;
			BMD_FOK(bmd_symmetric_decrypt(tmp1, 0, &(bmd_info->crypto_info.sym_ctx),&dec,NULL));

			PRINT_INFO("LIBBMDINF Unwrapping and symetric decryption succeded\n");
			free_gen_buf(&tmp1);
			free_gen_buf(&unwrapped);
			status=PR2_bmdDatagram_deserialize(dec,&dtg);
			if(status!=BMD_OK)
			{
				PRINT_ERROR("LIBBMDERR Datagram deserializing failed\n");
				return BMD_ERR_USER_OP_FAILED; /* serwer przyslal nie poprawny DER datagramu */
			}
			status=dtg->datagramStatus;
			if(status==BMD_OK)
			{
				PRINT_ERROR("LIBBMDERR Datagram status invalid\n");
				return BMD_ERR_USER_OP_FAILED; /* tutaj datagram nie moze byc "0" bo musial zdarzyc sie blad */
			}

			/* aktualizacja ki_mod_n */
			PRINT_INFO("LIBBMDINF ki_mod_n actualization.\n");
			BMD_FOK(bmd2_datagram_get_ki_mod_n(dtg,&tmp));

			set_gen_buf2(tmp->buf,BMD_BN_BYTE_SIZE,&server_ki_mod_n);
			err=bmd_compute_ki_mod_n(tmp_ki_mod_n_client,bmd_info->i,bmd_info->n,&to_compare_ki_mod_n);
			err=bmd_compare_bn(to_compare_ki_mod_n,server_ki_mod_n);
			if(err!=BMD_OK)
			{
				PRINT_ERROR("LIBBMDERR Comparing i-s and k-s failed\n");
				return BMD_ERR_PROTOCOL;
			}
			free_gen_buf(&tmp1);
			err=bmd_compute_ki_mod_n(bmd_info->cur_ki_mod_n,bmd_info->i,bmd_info->n,&tmp1);
			if(err!=BMD_OK)
			{
				PRINT_ERROR("LIBBMDERR Computing ki-s and n-s failed\n");
				return BMD_ERR_USER_OP_FAILED;
			}
			free_gen_buf(&(bmd_info->cur_ki_mod_n));
			set_gen_buf2(tmp1->buf,tmp1->size,&((bmd_info->cur_ki_mod_n)));


			free_gen_buf(&tmp1);
			PR2_bmdDatagram_free(&dtg);
			free_gen_buf(&dec);
			free_gen_buf(&unwrapped);
			free_gen_buf(&to_compare_ki_mod_n);
			free_gen_buf(&server_ki_mod_n);
			free_gen_buf(&tmp);
			return status;
		}
		else
		{
			BMD_FOK(BMD_ERR_PROTOCOL);
		}
	}
	BMD_FOK(bmd2_decode_session_request(	&tmp1,
						&(bmd_info->cert_login_as),
						0,
						bmd_info->crypto_info.sym_ctx,
						tmp_ki_mod_n_client,
						bmd_info->i,
						bmd_info->n,
						new_ki_mod_n,
						response_datagram_set,
						twf_bmdDatagramsTransmissionCount,
						twf_bmdDatagramsAllCount,
						1,
						twf_memory_guard,
						deserialisation_max_memory));

	if(new_ki_mod_n != NULL) /*TODO*/
	{
		BMD_FOK(bmd_compute_ki_mod_n(bmd_info->cur_ki_mod_n,bmd_info->i,bmd_info->n,&tmp1));
		free_gen_buf(&(bmd_info->cur_ki_mod_n));
		set_gen_buf2(tmp1->buf,tmp1->size,&((bmd_info->cur_ki_mod_n)));
		free_gen_buf(&tmp1);
		free_gen_buf(&(bmd_info->server_ki_mod_n));
	}
	return BMD_OK;
}

long __bmd_genbuf_to_char(GenBuf_t *input, char **output)
{
    if(input==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
    if(input->buf==NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
    if(input->size<=0)		{	BMD_FOK(BMD_ERR_PARAM1);	}

    if(output==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
    if((*output)!=NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}

    (*output)=(char *)malloc(input->size+1);
    if((*output)==NULL)		{	BMD_FOK(BMD_ERR_MEMORY);	}

    memset((*output),0,input->size+1);
    memmove((*output),input->buf,input->size);

    return BMD_OK;
}

/******************************************************************************************************/
/*	funkcja odbiera strumieniowo loby ustawione w liscie w lob_info, konteksty tez sa w lob_info	*/
/******************************************************************************************************/
long __bmd_recv_lobs(	bmdnet_handler_ptr session_handle,
				lob_request_info_t *lob_info,
				bmdDatagramSet_t *dtg)
{
long i					= 0;
long status				= 0;
char *tmp				= NULL;
char *converted_dir		= NULL;
long fd				= 0;
long nwrite				= 0;
struct lob_transport *lob	= NULL;
GenBuf_t *gb_ptr			= NULL;
long count				= 0;

#ifdef WIN32
wchar_t *wideFilename=NULL;
#endif

	if( (	((dtg->bmdDatagramSetTable[0])->datagramType==BMD_DATAGRAM_TYPE_GET_LO_RESPONSE) ||
		((dtg->bmdDatagramSetTable[0])->datagramType==BMD_DATAGRAM_TYPE_GET_PKI_LO_RESPONSE) ||
		((dtg->bmdDatagramSetTable[0])->datagramType==BMD_DATAGRAM_GET_ONLY_FILE_RESPONSE) ||
		((dtg->bmdDatagramSetTable[0])->datagramType==BMD_DATAGRAM_GET_CGI_LO_RESPONSE) ||
		((dtg->bmdDatagramSetTable[0])->datagramType==BMD_DATAGRAM_TYPE_GET_ARCHIVE_FILE_VERSION_LO_RESPONSE)))
	{

		for(i=0; i<lob_info->no_of_files_to_recv; i++)
		{

			lob_info->recv_prog_data.current_object_count=i+1;
			lob_info->recv_prog_data.prog_text=(void *)lob_info->files_to_recv[i];
			status = dtg->bmdDatagramSetTable[i]->datagramStatus;
			if ( status >= BMD_OK)
			{

				/* stworz deskryptor pliku w ktorym beda zapisywane dane */
				#ifdef WIN32
				UTF8StringToWindowsString(lob_info->files_to_recv[i], &wideFilename, NULL);
				fd=_wopen(wideFilename, O_CREAT|O_WRONLY|O_TRUNC|_O_BINARY, 0600);
				free(wideFilename); wideFilename=NULL;
				if(fd == -1)
				{
					BMD_FOK(BMD_ERR_DISK);
				}
				#else
				if( (fd=open(lob_info->files_to_recv[i],O_CREAT|O_WRONLY|O_TRUNC,0600)) == -1)
				{
					PRINT_ERROR("Error opening file %s", lob_info->files_to_recv[i]);
					BMD_FOK(BMD_ERR_DISK);
				}
				#endif

				status=new_lob_transport( &lob, &session_handle, NULL, lob_info->lob_contexts[0], NULL, NULL, 0 );
				if(status!=BMD_OK)
				{
					close(fd);
					BMD_FOK(BMD_ERR_OP_FAILED);
				}

				/* zapisuj LOB'a */
				while ( (status=recv_lob_chunk( &lob, &gb_ptr, 100, LARGE_FILE_DB_CHUNK_SIZE+1024)) == 0)
				{
					if (status != BMD_OK)
					{
						close(fd);
						BMD_FOK(BMD_ERR_OP_FAILED);
					}

					if(gb_ptr)
					{
						if(gb_ptr->size>0)
						{
							nwrite=write(fd,gb_ptr->buf,gb_ptr->size);
							if(nwrite!=gb_ptr->size)
							{
								close(fd);
								BMD_FOK(BMD_ERR_OP_FAILED);
							}
							/***********/
							count=count+nwrite;
							if(lob_info->show_recv_progress==1)
							{

								lob_info->recv_prog_data.total_cur_size=lob_info->recv_prog_data.total_cur_size+nwrite;

								PRINT_INFO("lob_info->recv_files_sizes[i] %lu\ncount %lu\nlob_info->recv_prog_data.total_size %lu\nlob_info->recv_prog_data.total_cur_size %lu\n",
									(long)lob_info->recv_files_sizes[i],
									(long)count,
									(long)lob_info->recv_prog_data.total_size,
									(long)lob_info->recv_prog_data.total_cur_size);
								status=((progress_function)lob_info->recv_prog_data.func)(lob_info->recv_prog_data.prog_data,
				    									lob_info->recv_files_sizes[i],count,
													lob_info->recv_prog_data.total_size,
													lob_info->recv_prog_data.total_cur_size,
													lob_info->recv_prog_data.prog_text);
								if(status!=BMD_OK)
								{
									close(fd);
									BMD_FOK(BMD_ERR_INTERRUPTED);
								}
							}
						}
					}
					free_gen_buf( &gb_ptr );
				}

				//wyjscie z funkcji nalezy przerobic tak by najpierw po sobie czyscila i nie zostawiala smici - RO
				if (status < BMD_OK)	// jezeli nie udalo sie pobrac pliku w calosci to nie zapisujemy - RO
				{
					close(fd);
					BMD_FOK(BMD_ERR_OP_FAILED);
				}
				if(gb_ptr)
				{
					nwrite=write(fd,gb_ptr->buf,gb_ptr->size);
					if(nwrite!=gb_ptr->size)
					{
						close(fd);
						BMD_FOK(BMD_ERR_OP_FAILED);
					}
				}

				free_gen_buf(&gb_ptr);

				count=0;

				close(fd);
				free(converted_dir);converted_dir=NULL;
				free(tmp);tmp=NULL;

				BMD_FOK(free_lob_transport( &lob ));
				bmd_ctx_reinit(&(lob_info->lob_contexts[0]));

				count=count+nwrite;
				if(lob_info->show_recv_progress==1)
				{

					lob_info->recv_prog_data.total_cur_size=lob_info->recv_prog_data.total_cur_size+nwrite;

					PRINT_INFO("lob_info->recv_files_sizes[i] %lu\ncount %lu\nlob_info->recv_prog_data.total_size %lu\nlob_info->recv_prog_data.total_cur_size %lu\n",
						(long)lob_info->recv_files_sizes[i],
						(long)count,
						(long)lob_info->recv_prog_data.total_size,
						(long)lob_info->recv_prog_data.total_cur_size);
					status=((progress_function)lob_info->recv_prog_data.func)(lob_info->recv_prog_data.prog_data,
	    									lob_info->recv_files_sizes[i],count,
										lob_info->recv_prog_data.total_size,
										lob_info->recv_prog_data.total_cur_size,
										lob_info->recv_prog_data.prog_text);
					if(status!=BMD_OK)
					{
						BMD_FOK(BMD_ERR_INTERRUPTED);
					}
				}

			}
			else
			{
				continue;
			}
		}
	}

	return BMD_OK;
}

/**
*funkcja stanowi odmiane funcji __bmd_recv_lob() rozszerzona o dwa dodatkowe parametry:
*@param lobBuffer, to wskaznik na alokowany wewnatrz funkcji buffor z pobranym lobem
*@param maxLobSize to maksymalny rozmiar pobieranego loba (dla wartosci <= 0 bufor domyslnie przyjmuje ograniczenie 1024*1024 bajtow)
*funckja zostala zaimplementowana na potrzeby funkcji __bmd_prepare_response_to_buf()
*@retval 0 jesli sukces
*@retval wartosc mniejsza od 0 w przypadku bledu (rowniez jesli lob przekracza rozmiar maxLobSize)
*/
long __bmd_recv_lob_to_buf(	bmdnet_handler_ptr session_handle,
				lob_request_info_t *lob_info,
				bmdDatagramSet_t *datagramSet,
				GenBuf_t **buffer,
				long maxLobSize)
{

long status				= 0;
long downloadedBytesCount=0;
struct lob_transport *lob	= NULL;
GenBuf_t *gb_ptr			= NULL;
GenBuf_t *tempBuf=NULL;

	if(session_handle == NULL)
		{ return -1; }
	if(lob_info == NULL)
		{ return -2; }
	if(datagramSet == NULL)
		{ return -3; }
	if(buffer == NULL)
		{ return -4; }

	//modyfikacja zmiennej lokalnej funkcji
	if(maxLobSize <= 0)
		{ maxLobSize = 1024*1024; }


	//przyjmuje tylko datagramsety z jednym datagramem (mozna pobrac tylko jeden plik do bufora)
	if(datagramSet->bmdDatagramSetSize != 1)
	{
		return -5;
	}


	if( (	((datagramSet->bmdDatagramSetTable[0])->datagramType==BMD_DATAGRAM_TYPE_GET_LO_RESPONSE) ||
		((datagramSet->bmdDatagramSetTable[0])->datagramType==BMD_DATAGRAM_TYPE_GET_PKI_LO_RESPONSE) ||
		((datagramSet->bmdDatagramSetTable[0])->datagramType==BMD_DATAGRAM_GET_ONLY_FILE_RESPONSE) ||
		((datagramSet->bmdDatagramSetTable[0])->datagramType==BMD_DATAGRAM_GET_CGI_LO_RESPONSE) ||
		((datagramSet->bmdDatagramSetTable[0])->datagramType==BMD_DATAGRAM_TYPE_GET_ARCHIVE_FILE_VERSION_LO_RESPONSE)) )
	{


		tempBuf=(GenBuf_t*)calloc(1, sizeof(GenBuf_t));
		if(tempBuf == NULL)
			{ return -12; }
		tempBuf->buf=(char*)calloc(maxLobSize, sizeof(char));
		if(tempBuf->buf == NULL)
		{
			free(tempBuf); tempBuf=NULL;
			return -13;
		}
		//rozmiar nie jest ustawiany, bo zostanie po sciagnieciu nadany


		lob_info->recv_prog_data.current_object_count=1;
		lob_info->recv_prog_data.prog_text=(void *)lob_info->files_to_recv[0];
		if ( datagramSet->bmdDatagramSetTable[0]->datagramStatus >= BMD_OK)
		{

			/* stworz deskryptor pliku w ktorym beda zapisywane dane */
			status=new_lob_transport( &lob, &session_handle, NULL, lob_info->lob_contexts[0], NULL, NULL, 0 );
			if(status!=BMD_OK)
			{
				free_gen_buf(&tempBuf);
				return -6;
			}

			/* pobieraj LOB'a */
			while ( (status=recv_lob_chunk( &lob, &gb_ptr, 1, LARGE_FILE_DB_CHUNK_SIZE+1024)) == BMD_OK)
			{
				if(gb_ptr)
				{
					if(gb_ptr->size>0)
					{
						if(downloadedBytesCount+gb_ptr->size > maxLobSize)
						{
							free_gen_buf(&tempBuf);
							return -7;
						}
						memcpy((tempBuf->buf)+downloadedBytesCount, gb_ptr->buf, gb_ptr->size);
						tempBuf->size += gb_ptr->size;
						downloadedBytesCount += gb_ptr->size;

						if(lob_info->show_recv_progress==1)
						{

							lob_info->recv_prog_data.total_cur_size=lob_info->recv_prog_data.total_cur_size+gb_ptr->size;
							status=((progress_function)lob_info->recv_prog_data.func)(lob_info->recv_prog_data.prog_data,
			    									lob_info->recv_files_sizes[0],downloadedBytesCount,
												lob_info->recv_prog_data.total_size,
												lob_info->recv_prog_data.total_cur_size,
												lob_info->recv_prog_data.prog_text);
							if(status!=BMD_OK)
							{
								free_gen_buf(&tempBuf);
								return BMD_ERR_INTERRUPTED;
							}
						}
					}
				}
				free_gen_buf( &gb_ptr );
			}

			//wyjscie z funkcji nalezy przerobic tak by najpierw po sobie czyscila i nie zostawiala smici - RO
			if (status < BMD_OK)	// jezeli nie udalo sie pobrac pliku w calosci to nie zapisujemy - RO
			{
				free_gen_buf(&tempBuf);
				BMD_FOK(BMD_ERR_OP_FAILED);
			}
			if(gb_ptr)
			{
				if(gb_ptr->size>0)
				{
					if(downloadedBytesCount+gb_ptr->size > maxLobSize)
					{
						free_gen_buf(&tempBuf);
						return -9;
					}
					memcpy((tempBuf->buf)+downloadedBytesCount, gb_ptr->buf, gb_ptr->size);
					tempBuf->size += gb_ptr->size;
					downloadedBytesCount += gb_ptr->size;
				}
			}
			free_gen_buf( &gb_ptr );
			free_lob_transport( &lob );
			bmd_ctx_reinit(&(lob_info->lob_contexts[0]));

			if(lob_info->show_recv_progress==1)
			{

				lob_info->recv_prog_data.total_cur_size=downloadedBytesCount;
				status=((progress_function)lob_info->recv_prog_data.func)(lob_info->recv_prog_data.prog_data,
    									lob_info->recv_files_sizes[0],downloadedBytesCount,
									lob_info->recv_prog_data.total_size,
									lob_info->recv_prog_data.total_cur_size,
									lob_info->recv_prog_data.prog_text);
				if(status!=BMD_OK)
				{
					free_gen_buf(&tempBuf);
					return BMD_ERR_INTERRUPTED;
				}
			}
		}
		else
		{
			free_gen_buf(&tempBuf);
			return -14;
		}
	}
	else
	{
		return -11;
	}

	*buffer=tempBuf;
	tempBuf=NULL;
	return 0;
}





/******************************************************************************************************/
/*	funkcja odbiera strumieniowo loby ustawione w liscie w lob_info, konteksty tez sa w lob_info,	*/
/*	odebrane loby zapisywane są w osobnych plikach w określonym katalogu					*/
/******************************************************************************************************/
long __bmd_recv_lobs_in_parts(	bmdnet_handler_ptr session_handle,
					lob_request_info_t *lob_info,
					bmdDatagramSet_t *dtg,
					long lob_session_hash,
					long twf_timeout)

{
#ifndef WIN32
char buffer_array[256];
long i					= 0;
long j					= 0;
long status				= 0;
char *tmp				= NULL;
char *converted_name			= NULL;
long fd					= 0;
long nwrite				= 0;
long file2				= 0;
struct lob_transport *lob		= NULL;
GenBuf_t *gb_ptr			= NULL;
long count				= 0;
char *tmp_location_lock			= NULL;

	if( (	((dtg->bmdDatagramSetTable[0])->datagramType==BMD_DATAGRAM_TYPE_GET_LO_RESPONSE) ||
		((dtg->bmdDatagramSetTable[0])->datagramType==BMD_DATAGRAM_TYPE_GET_PKI_LO_RESPONSE) ||
		((dtg->bmdDatagramSetTable[0])->datagramType==BMD_DATAGRAM_GET_ONLY_FILE_RESPONSE) ||
		((dtg->bmdDatagramSetTable[0])->datagramType==BMD_DATAGRAM_GET_CGI_LO_RESPONSE) ||
		((dtg->bmdDatagramSetTable[0])->datagramType==BMD_DATAGRAM_TYPE_GET_ARCHIVE_FILE_VERSION_LO_RESPONSE)))
	{
		lob_info->recv_prog_data.current_object_count=i+1;
		lob_info->recv_prog_data.prog_text=(void *)lob_info->files_to_recv[0];

		/******************************************************************/
		/*	mozliwe jest pobieranie tylko jednego pliku tym sposobem	*/
		/******************************************************************/
		if ( dtg->bmdDatagramSetTable[0]->datagramStatus>=BMD_OK)
		{
			status=new_lob_transport( &lob, &session_handle, NULL, lob_info->lob_contexts[0], NULL, NULL, 0 );
			if(status!=BMD_OK)
			{
				close(fd);
				BMD_FOK(BMD_ERR_OP_FAILED);
			}

			while (1)
			{
				status=recv_lob_chunk( &lob, &gb_ptr, 100, LARGE_FILE_DB_CHUNK_SIZE+1024);
				PRINT_TEST("status=%li\tsize=%li\tsizelimit=%i\n", status, gb_ptr->size, \
				LARGE_FILE_DB_CHUNK_SIZE-16);
				if ((status<BMD_OK) || (gb_ptr->size<LARGE_FILE_DB_CHUNK_SIZE-16))
				{
					if(gb_ptr)
					{
						asprintf(&converted_name, "%s/soap_get_chunk_%li%li_final", \
						lob_info->files_to_recv[0], (long)getpid(), lob_session_hash);
						if(converted_name == NULL)
						{
							BMD_FOK(NO_MEMORY);
						}

						asprintf(&tmp_location_lock, "%s.lock", converted_name);
						if(tmp_location_lock == NULL)
						{
							BMD_FOK(NO_MEMORY);
						}

						/************************/
						/*	zakladam locka	*/
						/************************/
						j=0;
						while(1)
						{/*while(1)*/
							if ((file2=open(tmp_location_lock, O_WRONLY|O_CREAT,
							S_IRWXU|S_IRWXG|S_IRWXO))>0)
							{
								close(file2);
								break;
							}
							else
							{
								if (j>=twf_timeout)
								{
							PRINT_ERROR("LIBBMDERR Error in opening lock file %s - reason %s.\n",
							tmp_location_lock, strerror_r(errno, buffer_array, 256) );

									BMD_FOK(BMD_ERR_TIMEOUT);
								}
								j++;
								sleep(1);
							}
						}/*while(1)*/
						BMD_FOK(bmd_save_buf(gb_ptr,converted_name));
						unlink(tmp_location_lock);
					}
					close(fd);
					break;
				}
				else
				{
					if(gb_ptr)
					{
						asprintf(&converted_name, "%s/soap_get_chunk_%li%li_%li",
						lob_info->files_to_recv[0], (long)getpid(), lob_session_hash,++count);
						if(converted_name == NULL)
						{
							BMD_FOK(NO_MEMORY);
						}

						asprintf(&tmp_location_lock, "%s.lock", converted_name);
						if(tmp_location_lock == NULL)
						{
							BMD_FOK(NO_MEMORY);
						}

						/************************/
						/*	zakladam locka	*/
						/************************/
						j=0;
						while(1)
						{
							if ((file2=open(tmp_location_lock, O_WRONLY|O_CREAT,
							S_IRWXU|S_IRWXG|S_IRWXO))>0)
							{
								close(file2);
								break;
							}
							else
							{
								if (j>=twf_timeout)
								{
							PRINT_ERROR("LIBBMDERR Error in opening lock file %s - reason %s.\n",
							tmp_location_lock, strerror_r(errno, buffer_array, 256) );

									BMD_FOK(BMD_ERR_TIMEOUT);
								}
								j++;
								sleep(1);
							}
						}
						BMD_FOK(bmd_save_buf(gb_ptr,converted_name));
						unlink(tmp_location_lock);
					}
				}

				if(gb_ptr->size>0)
				{
					count=count+nwrite;
					if(lob_info->show_recv_progress==1)
					{
						lob_info->recv_prog_data.total_cur_size=lob_info->recv_prog_data.total_cur_size+nwrite;
						PRINT_INFO("lob_info->recv_files_sizes[0] %lu\ncount %lu\nlob_info->recv_prog_data.total_size %lu\nlob_info->recv_prog_data.total_cur_size %lu\n",
												(long)lob_info->recv_files_sizes[0],
												(long)count,
												(long)lob_info->recv_prog_data.total_size,
												(long)lob_info->recv_prog_data.total_cur_size);
						status=((progress_function)lob_info->recv_prog_data.func)(lob_info->recv_prog_data.prog_data,
												lob_info->recv_files_sizes[0],count,
												lob_info->recv_prog_data.total_size,
												lob_info->recv_prog_data.total_cur_size,
												lob_info->recv_prog_data.prog_text);
						if(status!=BMD_OK)
						{
							close(fd);
							BMD_FOK(BMD_ERR_INTERRUPTED);
						}
					}
				}
				free_gen_buf( &gb_ptr );
			}
			if (status <0)
			{
				close(fd);
				BMD_FOK(BMD_ERR_OP_FAILED);
			}

			free_gen_buf(&gb_ptr);
			count=0;
			close(fd);
			free(converted_name);converted_name=NULL;
			free(tmp);tmp=NULL;
			BMD_FOK(free_lob_transport( &lob ));
			bmd_ctx_reinit(&(lob_info->lob_contexts[0]));
			count=count+nwrite;
			if(lob_info->show_recv_progress==1)
			{

				lob_info->recv_prog_data.total_cur_size=lob_info->recv_prog_data.total_cur_size+nwrite;

				PRINT_INFO("lob_info->recv_files_sizes[] %lu\ncount %lu\nlob_info->recv_prog_data.total_size %lu\nlob_info->recv_prog_data.total_cur_size %lu\n",
									(long)lob_info->recv_files_sizes[0],
									(long)count,
									(long)lob_info->recv_prog_data.total_size,
									(long)lob_info->recv_prog_data.total_cur_size);
				status=((progress_function)lob_info->recv_prog_data.func)(lob_info->recv_prog_data.prog_data,
									lob_info->recv_files_sizes[0],count,
									lob_info->recv_prog_data.total_size,
									lob_info->recv_prog_data.total_cur_size,
									lob_info->recv_prog_data.prog_text);
				if(status!=BMD_OK)
				{
					BMD_FOK(BMD_ERR_INTERRUPTED);
				}
			}
		}
	}

	/*******************/
	/*	porzadki	*/
	/******************/
	free(tmp_location_lock); tmp_location_lock=NULL;

	return BMD_OK;
#else
return BMD_ERR_UNIMPLEMENTED;
#endif
}

long __bmd_prepare_response(	bmd_info_t *bmd_info,
					bmdDatagramSet_t **response_datagram_set,
					lob_request_info_t *lob_info,
					bmdnet_handler_ptr session_handle)
{
long i					= 0;
GenBuf_t *deciphered_data	= NULL;
GenBuf_t *tmp			= NULL;
bmd_crypt_ctx_t *sym_ctx	= NULL;
//long dtg_set_ok			= 0;
long status				= 0;
//long status_mem=0;
long symkeyAlreadyFound=0;

	PRINT_INFO("LIBBMDINF Preparing response\n");
	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if(bmd_info==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(response_datagram_set==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if((*response_datagram_set)==NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	/************************************/
	/*	alokacja listy kontekstow	*/
	/************************************/
	if(lob_info)
	{
		if(lob_info->lob_contexts==NULL)
		{
			lob_info->lob_contexts=(bmd_crypt_ctx_t **)malloc(((*response_datagram_set)->bmdDatagramSetSize+2)*sizeof(bmd_crypt_ctx_t *));
			memset(lob_info->lob_contexts,0,((*response_datagram_set)->bmdDatagramSetSize+1)*sizeof(bmd_crypt_ctx_t *));
		}
	}

	/******************************************************/
	/*	jeśli datagram odpowiedzi nie zawiera bledow	*/
	/******************************************************/

		for(i=0;i<(long)(*response_datagram_set)->bmdDatagramSetSize;i++)
		{
			if((*response_datagram_set)->bmdDatagramSetTable[i]->datagramStatus < 0)
			{
				continue;
			}
			/******************************************************/
			/*	jesli poslugujemy sie transmisja bez lobow	*/
			/******************************************************/
			if( 	(((*response_datagram_set)->bmdDatagramSetTable[i])->datagramType==BMD_DATAGRAM_TYPE_GET_RESPONSE) ||
				(((*response_datagram_set)->bmdDatagramSetTable[i])->datagramType==BMD_DATAGRAM_TYPE_GET_PKI_RESPONSE) ||
				(((*response_datagram_set)->bmdDatagramSetTable[i])->datagramType==BMD_DATAGRAM_GET_CGI_RESPONSE) ||
				(((*response_datagram_set)->bmdDatagramSetTable[i])->datagramType==BMD_DATAGRAM_GET_CGI_PKI_RESPONSE)
			)
			{
				/************************************************************/
				/*	z pierwszego datagramu pobieramy klucz symetryczny	*/
				/************************************************************/
				if(i==0)
				{
					BMD_FOK(bmd2_datagram_get_symkey((*response_datagram_set)->bmdDatagramSetTable[0],&tmp));
					BMD_FOK(bmd_decode_bmdKeyIV(tmp,NULL,bmd_info->crypto_info.log_ctx,&sym_ctx));
				}

				BMD_FOK(bmd_symmetric_decrypt((*response_datagram_set)->bmdDatagramSetTable[i]->\
				protocolData, 0, &sym_ctx, &deciphered_data, NULL));

				free_gen_buf( &( ( (*response_datagram_set)->bmdDatagramSetTable[i] )->protocolData ) );
				set_gen_buf2(deciphered_data->buf,deciphered_data->size, &( ( (*response_datagram_set)->bmdDatagramSetTable[i] )->protocolData ));
				free_gen_buf(&deciphered_data);
				bmd_ctx_reinit(&sym_ctx);
				free_gen_buf(&tmp);
			}
			/************************************************/
			/*	jesli poslugujemy sie transmisja lobowa	*/
			/************************************************/
			else
			{
				if( (((*response_datagram_set)->bmdDatagramSetTable[i])->datagramType==BMD_DATAGRAM_TYPE_GET_LO_RESPONSE) ||
				(((*response_datagram_set)->bmdDatagramSetTable[i])->datagramType==BMD_DATAGRAM_TYPE_GET_PKI_LO_RESPONSE) ||
				(((*response_datagram_set)->bmdDatagramSetTable[i])->datagramType==BMD_DATAGRAM_GET_ONLY_FILE_RESPONSE) ||
				(((*response_datagram_set)->bmdDatagramSetTable[i])->datagramType==BMD_DATAGRAM_GET_CGI_LO_RESPONSE) ||
				(((*response_datagram_set)->bmdDatagramSetTable[i])->datagramType==BMD_DATAGRAM_TYPE_GET_ARCHIVE_FILE_VERSION_LO_RESPONSE) ||
				(((*response_datagram_set)->bmdDatagramSetTable[i])->datagramType==BMD_DATAGRAM_GET_CGI_PKI_LO_RESPONSE))
				{
					/************************************************************/
					/*	z pierwszego datagramu pobieramy klucz symetryczny	*/
					/************************************************************/
					if(symkeyAlreadyFound == 0)
					{
						BMD_FOK(bmd2_datagram_get_symkey((*response_datagram_set)->bmdDatagramSetTable[i],&tmp));
						BMD_FOK(bmd_decode_bmdKeyIV(tmp,NULL,bmd_info->crypto_info.log_ctx,&(lob_info->lob_contexts[0])));
						symkeyAlreadyFound = 1;

						free_gen_buf(&tmp);
						if ( (lob_info->lob_contexts[0]!=NULL) && (lob_info->lob_contexts[0]->sym!=NULL))
						{
							lob_info->lob_contexts[0]->sym->sym_params=BMD_SYM_MULTI_ROUND;
						}
						else
						{
							return BMD_ERR_PKI_VERIFY;	// straznik wykryl blad utraconego kontekstu - powodowalo segfault'a klienta -  FS#312
						}
					}
				}
			}
		}

	/************************/
	/*	odbior LOB'ow	*/
	/************************/
	status=__bmd_recv_lobs(session_handle,lob_info,*response_datagram_set);
	if (status<BMD_OK)
	{
		PRINT_ERROR("LIBBMDERR Error. Error =%li\n",status);
	}

	return status;
}


/**
*funckja __bmd_prepare_response_to_buf() jest odmiana funckji __bmd_prepare_response() przyjmujaca dwa dodatkowe parametry:
*@param lobBuffer, to wskaznik na alokowany wewnatrz funkcji buffor z pobranym lobem
*@param maxLobSize to maksymalny rozmiar pobieranego loba (dla wartosci <= 0 bufor domyslnie przyjmuje ograniczenie 1024*1024 bajtow)
*funkcja zostala zaimplementowana na potrzeby bmd_send_request_lo2_to_buf()
*/
long __bmd_prepare_response_to_buf(	bmd_info_t *bmd_info,
					bmdDatagramSet_t **response_datagram_set,
					lob_request_info_t *lob_info,
					bmdnet_handler_ptr session_handle,
					GenBuf_t **lobBuffer,
					long maxLobSize)
{
long i					= 0;
GenBuf_t *deciphered_data	= NULL;
GenBuf_t *tmp			= NULL;
bmd_crypt_ctx_t *sym_ctx	= NULL;
//long dtg_set_ok			= 0;
long status				= 0;
//long status_mem=0;
long symkeyAlreadyFound=0;

	PRINT_INFO("LIBBMDINF Preparing response\n");
	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if(bmd_info==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(response_datagram_set==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if((*response_datagram_set)==NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	/************************************/
	/*	alokacja listy kontekstow	*/
	/************************************/
	if(lob_info)
	{
		if(lob_info->lob_contexts==NULL)
		{
			lob_info->lob_contexts=(bmd_crypt_ctx_t **)malloc(((*response_datagram_set)->bmdDatagramSetSize+2)*sizeof(bmd_crypt_ctx_t *));
			memset(lob_info->lob_contexts,0,((*response_datagram_set)->bmdDatagramSetSize+1)*sizeof(bmd_crypt_ctx_t *));
		}
	}

	/******************************************************/
	/*	jeśli datagram odpowiedzi nie zawiera bledow	*/
	/******************************************************/

		for(i=0;i<(long)(*response_datagram_set)->bmdDatagramSetSize;i++)
		{
			if((*response_datagram_set)->bmdDatagramSetTable[i]->datagramStatus < 0)
			{
				continue;
			}
			/******************************************************/
			/*	jesli poslugujemy sie transmisja bez lobow	*/
			/******************************************************/
			if( 	(((*response_datagram_set)->bmdDatagramSetTable[i])->datagramType==BMD_DATAGRAM_TYPE_GET_RESPONSE) ||
				(((*response_datagram_set)->bmdDatagramSetTable[i])->datagramType==BMD_DATAGRAM_TYPE_GET_PKI_RESPONSE) ||
				(((*response_datagram_set)->bmdDatagramSetTable[i])->datagramType==BMD_DATAGRAM_GET_CGI_RESPONSE) ||
				(((*response_datagram_set)->bmdDatagramSetTable[i])->datagramType==BMD_DATAGRAM_GET_CGI_PKI_RESPONSE)
			)
			{
				/************************************************************/
				/*	z pierwszego datagramu pobieramy klucz symetryczny	*/
				/************************************************************/
				if(i==0)
				{
					BMD_FOK(bmd2_datagram_get_symkey((*response_datagram_set)->bmdDatagramSetTable[0],&tmp));
					BMD_FOK(bmd_decode_bmdKeyIV(tmp,NULL,bmd_info->crypto_info.log_ctx,&sym_ctx));
				}

				BMD_FOK(bmd_symmetric_decrypt((*response_datagram_set)->bmdDatagramSetTable[i]->\
				protocolData, 0, &sym_ctx, &deciphered_data, NULL));

				free_gen_buf( &( ( (*response_datagram_set)->bmdDatagramSetTable[i] )->protocolData ) );
				set_gen_buf2(deciphered_data->buf,deciphered_data->size, &( ( (*response_datagram_set)->bmdDatagramSetTable[i] )->protocolData ));
				free_gen_buf(&deciphered_data);
				bmd_ctx_reinit(&sym_ctx);
				free_gen_buf(&tmp);
			}
			/************************************************/
			/*	jesli poslugujemy sie transmisja lobowa	*/
			/************************************************/
			else
			{
				if( (((*response_datagram_set)->bmdDatagramSetTable[i])->datagramType==BMD_DATAGRAM_TYPE_GET_LO_RESPONSE) ||
				(((*response_datagram_set)->bmdDatagramSetTable[i])->datagramType==BMD_DATAGRAM_TYPE_GET_PKI_LO_RESPONSE) ||
				(((*response_datagram_set)->bmdDatagramSetTable[i])->datagramType==BMD_DATAGRAM_GET_ONLY_FILE_RESPONSE) ||
				(((*response_datagram_set)->bmdDatagramSetTable[i])->datagramType==BMD_DATAGRAM_GET_CGI_LO_RESPONSE) ||
				(((*response_datagram_set)->bmdDatagramSetTable[i])->datagramType==BMD_DATAGRAM_TYPE_GET_ARCHIVE_FILE_VERSION_LO_RESPONSE) ||
				(((*response_datagram_set)->bmdDatagramSetTable[i])->datagramType==BMD_DATAGRAM_GET_CGI_PKI_LO_RESPONSE)
				)
				{
					/************************************************************/
					/*	z pierwszego datagramu pobieramy klucz symetryczny	*/
					/************************************************************/
					if(symkeyAlreadyFound == 0)
					{
						BMD_FOK(bmd2_datagram_get_symkey((*response_datagram_set)->bmdDatagramSetTable[i],&tmp));
						BMD_FOK(bmd_decode_bmdKeyIV(tmp,NULL,bmd_info->crypto_info.log_ctx,&(lob_info->lob_contexts[0])));
						symkeyAlreadyFound = 1;

						free_gen_buf(&tmp);
						if ( (lob_info->lob_contexts[0]!=NULL) && (lob_info->lob_contexts[0]->sym!=NULL))
						{
							lob_info->lob_contexts[0]->sym->sym_params=BMD_SYM_MULTI_ROUND;
						}
						else
						{
							return BMD_ERR_PKI_VERIFY;	// straznik wykryl blad utraconego kontekstu - powodowalo segfault'a klienta -  FS#312
						}
					}
				}
			}
		}

	/************************/
	/*	odbior LOB'a	*/
	/************************/
	status=__bmd_recv_lob_to_buf(session_handle,lob_info,*response_datagram_set, lobBuffer, maxLobSize);
	if (status<0)
	{
		PRINT_ERROR("LIBBMDERR Error. Error =%li\n",status);
	}

	return status;
}


long __bmd_verify_lob_input_data(bmdDatagramSet_t *request_datagram_set,lob_request_info_t *info)
{
	PRINT_INFO("LIBBMDINF Verifying lob's input data\n");
	if(request_datagram_set==NULL)
	{
		PRINT_ERROR("LIBBMDERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(info==NULL)
	{
		PRINT_ERROR("LIBBMDERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}
    /*
    if( (info->show_send_progress==BMD_SHOW_PROGRESS) && (info->send_progress_data==NULL) )
	return BMD_ERR_PARAM2;
    if( (info->show_recv_progress==BMD_SHOW_PROGRESS) && (info->recv_progress_data==NULL) )
	return BMD_ERR_PARAM2;
    */
	if(request_datagram_set->bmdDatagramSetTable==NULL)
	{
		PRINT_ERROR("LIBBMDERR Invalid format. Error=%i\n",BMD_ERR_FORMAT);
		return BMD_ERR_FORMAT;
	}

	if(request_datagram_set->bmdDatagramSetTable[0]->datagramType==BMD_DATAGRAM_TYPE_INSERT_LO ||
		request_datagram_set->bmdDatagramSetTable[0]->datagramType==BMD_DATAGRAM_TYPE_UPDATE_FILE_VERSION_LO)
	{
		if(info->files_to_send==NULL)
		{
			PRINT_ERROR("LIBBMDERR Invalid format. Error=%i\n",BMD_ERR_FORMAT);
			return BMD_ERR_FORMAT;
		}

		if(request_datagram_set->bmdDatagramSetSize!=(long)info->no_of_files_to_send)
		{
			PRINT_ERROR("LIBBMDERR Invalid format. Error=%i\n",BMD_ERR_FORMAT);
			PRINT_ERROR("LIBBMDERR %li %li\n",request_datagram_set->bmdDatagramSetSize, \
			info->no_of_files_to_send);
			return BMD_ERR_FORMAT;
		}
	}

	return BMD_OK;
}


/************************************************************************************************/
/*	odbieranie odpowiedz na zadania lobowe i zapisywanie kawalkow do odpowiedniego katalogu	*/
/************************************************************************************************/
long __bmd_prepare_response_in_parts(	bmd_info_t *bmd_info,
							bmdDatagramSet_t **response_datagram_set,
							lob_request_info_t *lob_info,
							bmdnet_handler_ptr session_handle,
							long lob_session_hash,
							long twf_timeout)
{
long i					= 0;
GenBuf_t *deciphered_data		= NULL;
GenBuf_t *tmp				= NULL;
bmd_crypt_ctx_t *sym_ctx		= NULL;

	PRINT_INFO("LIBBMDINF Preparing response\n");
	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if(bmd_info==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(response_datagram_set==NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if((*response_datagram_set)==NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	/************************************/
	/*	alokacja listy kontekstow	*/
	/************************************/
	if(lob_info)
	{
		if(lob_info->lob_contexts==NULL)
		{
			lob_info->lob_contexts=(bmd_crypt_ctx_t **)malloc(((*response_datagram_set)->bmdDatagramSetSize+2)*sizeof(bmd_crypt_ctx_t *));
			memset(lob_info->lob_contexts,0,((*response_datagram_set)->bmdDatagramSetSize+1)*sizeof(bmd_crypt_ctx_t *));
		}
	}

	/******************************************************/
	/*	jeśli datagram odpowiedzi nie zawiera bledow	*/
	/******************************************************/
	if(((*response_datagram_set)->bmdDatagramSetTable[0])->datagramStatus>=BMD_OK)
	{

		for(i=0;i<(long)(*response_datagram_set)->bmdDatagramSetSize;i++)
		{
			/******************************************************/
			/*	jesli poslugujemy sie transmisja bez lobow	*/
			/******************************************************/
			if( 	(((*response_datagram_set)->bmdDatagramSetTable[i])->datagramType==BMD_DATAGRAM_TYPE_GET_RESPONSE) ||
				(((*response_datagram_set)->bmdDatagramSetTable[i])->datagramType==BMD_DATAGRAM_TYPE_GET_PKI_RESPONSE) ||
				(((*response_datagram_set)->bmdDatagramSetTable[i])->datagramType==BMD_DATAGRAM_GET_CGI_RESPONSE) ||
				(((*response_datagram_set)->bmdDatagramSetTable[i])->datagramType==BMD_DATAGRAM_GET_CGI_PKI_RESPONSE)
			)
			{
				/************************************************************/
				/*	z pierwszego datagramu pobieramy klucz symetryczny	*/
				/************************************************************/
				if(i==0)
				{
					BMD_FOK(bmd2_datagram_get_symkey((*response_datagram_set)->bmdDatagramSetTable[0],&tmp));
					BMD_FOK(bmd_decode_bmdKeyIV(tmp,NULL,bmd_info->crypto_info.log_ctx,&sym_ctx));
				}

				BMD_FOK(bmd_symmetric_decrypt((*response_datagram_set)->bmdDatagramSetTable[i]->\
				protocolData, 0, &sym_ctx,&deciphered_data,NULL));

				free_gen_buf( &( ( (*response_datagram_set)->bmdDatagramSetTable[i] )->protocolData ) );
				set_gen_buf2(deciphered_data->buf,deciphered_data->size, &( ( (*response_datagram_set)->bmdDatagramSetTable[i] )->protocolData ));
				free_gen_buf(&deciphered_data);
				bmd_ctx_reinit(&sym_ctx);
				free_gen_buf(&tmp);
			}
			/************************************************/
			/*	jesli poslugujemy sie transmisja lobowa	*/
			/************************************************/
			else
			{
				if( (((*response_datagram_set)->bmdDatagramSetTable[i])->datagramType==BMD_DATAGRAM_TYPE_GET_LO_RESPONSE) ||
				(((*response_datagram_set)->bmdDatagramSetTable[i])->datagramType==BMD_DATAGRAM_TYPE_GET_PKI_LO_RESPONSE) ||
				(((*response_datagram_set)->bmdDatagramSetTable[i])->datagramType==BMD_DATAGRAM_GET_ONLY_FILE_RESPONSE) ||
				(((*response_datagram_set)->bmdDatagramSetTable[i])->datagramType==BMD_DATAGRAM_GET_CGI_LO_RESPONSE) ||
				(((*response_datagram_set)->bmdDatagramSetTable[i])->datagramType==BMD_DATAGRAM_TYPE_GET_ARCHIVE_FILE_VERSION_LO_RESPONSE) ||
				(((*response_datagram_set)->bmdDatagramSetTable[i])->datagramType==BMD_DATAGRAM_GET_CGI_PKI_LO_RESPONSE))
				{
					/************************************************************/
					/*	z pierwszego datagramu pobieramy klucz symetryczny	*/
					/************************************************************/
					if(i==0)
					{
						BMD_FOK(bmd2_datagram_get_symkey((*response_datagram_set)->bmdDatagramSetTable[0],&tmp));
						BMD_FOK(bmd_decode_bmdKeyIV(tmp,NULL,bmd_info->crypto_info.log_ctx,&(lob_info->lob_contexts[0])));

						free_gen_buf(&tmp);
						if ( (lob_info->lob_contexts[i]!=NULL) && (lob_info->lob_contexts[i]->sym!=NULL))
						{
							lob_info->lob_contexts[i]->sym->sym_params=BMD_SYM_MULTI_ROUND;
						}
						else
						{
							BMD_FOK(BMD_ERR_PKI_VERIFY);
							/* straznik wykryl blad utraconego kontekstu -
							powodowalo segfault'a klienta -  FS#312*/
						}
					}
				}
			}
		}
	}

	/************************/
	/*	odbior LOB'ow	*/
	/************************/
	BMD_FOK(__bmd_recv_lobs_in_parts(session_handle,lob_info,(*response_datagram_set),lob_session_hash, twf_timeout));

	return BMD_OK;

}

