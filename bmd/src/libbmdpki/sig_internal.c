#include <bmd/common/bmd-common.h>

#include <bmd/libbmdpki/libbmdpki.h>
#include <bmd/libbmdpkcs12/libbmdpkcs12.h>
//#include <bmd/libbmddialogs/libbmddialogs.h>
#include <bmd/libbmdutils/libbmdutils.h>
#include <bmd/libbmderr/libbmderr.h>

#include <bmd/libbmdts/libbmdts.h>
#include <bmd/libbmdpkcs11/libbmdpkcs11.h>
#include <bmd/common/bmd-crypto_ds.h>

#include <bmd/common/bmd-const.h>

#include <bmd/libbmdcms/libbmdcms.h>
#include <bmd/libbmdks/bmd_ks_cli.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>

#ifndef WIN32 /* potrzebne naglowki dla keyservice */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#ifndef TRUE
#define TRUE 1
#endif

long __bmd_create_signed_data(	GenBuf_t *input,
						long flags,
						SignedData_t **sigData)
{
long id[] = {OID_CMS_ID_DATA_LONG};
long size=0;

	size=sizeof(id)/sizeof(long);
	if(input==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(input->buf==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(input->size<=0)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(sigData==NULL)			{	BMD_FOK(BMD_ERR_PARAM3);	}
	if((*sigData)!=NULL)		{	BMD_FOK(BMD_ERR_PARAM3);	}

	(*sigData)=(SignedData_t *)malloc(sizeof(SignedData_t));
	if((*sigData)==NULL)		{	BMD_FOK(BMD_ERR_MEMORY);	}

	memset(*sigData,0,sizeof(SignedData_t));

	/* w przypadku podpisu wewnetrznego ustaw dane w strukturze */
	if( flags == BMD_CMS_SIG_INTERNAL )
	{
		BMD_FOK(SignedData_set_Content(*sigData,input));
	}

	BMD_FOK(SignedData_set_ContentType(*sigData, id, size));

	return BMD_OK;
}

long __bmd_create_signer_info(	SignerInfo_t **signerInfo,
						SignedData_t **sigData,
						bmd_signature_params_t *opt,
                             		GenBuf_t *input,
						bmd_crypt_ctx_t *ctx)
{
	if(signerInfo==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if((*signerInfo)!=NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(sigData==NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}

	(*signerInfo) = (SignerInfo_t *)malloc(sizeof(SignerInfo_t));
	if((*signerInfo)==NULL)		{	BMD_FOK(BMD_ERR_MEMORY);	}

	memset(*signerInfo,0,sizeof(SignerInfo_t));
	/* Generuj podpis */
	BMD_FOK(SignerInfo_generate_Signature(*signerInfo, ctx, opt->signature_flags, opt->signed_attributes, opt->unsigned_attributes,input));

	/* dodanie *signerInfo do *sigData */
	BMD_FOK(SignedData_add_SignerInfo(*sigData, *signerInfo));

	return BMD_OK;
}

long __bmd_add_signer_cert(GenBuf_t *cert,SignedData_t **sigData)
{
Certificate_t *asncert = NULL;

	if(cert==NULL)
	{
		PRINT_DEBUG("LIBBMDPKIERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(cert->buf==NULL)
        {
		PRINT_DEBUG("LIBBMDPKIERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(cert->size<=0)
        {
		PRINT_DEBUG("LIBBMDPKIERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}

	ber_decode(0, &asn_DEF_Certificate, (void **)&asncert,cert->buf,cert->size);

	BMD_FOK(SignedData_add_Certificate(*sigData, asncert));

	asn_DEF_Certificate.free_struct(&asn_DEF_Certificate,asncert, 0);asncert=NULL;

	return BMD_OK;
}

long __keyservice_connect(const char *adr,long port,SOCKET *s)
{
#ifdef WIN32
	SOCKET tmp_s;
#else
	long tmp_s;
#endif
	long status;
	struct sockaddr_in dest;

	if( adr == NULL )
		return BMD_ERR_PARAM1;
	if( ( port == 0 ) || ( port > 65536 ) )
		return BMD_ERR_PARAM2;
	if( s == NULL )
		return BMD_ERR_PARAM3;

	tmp_s=socket(AF_INET,SOCK_STREAM,0);
	if( tmp_s == INVALID_SOCKET )
		return BMD_ERR_OP_FAILED;
	memset(&dest,0,sizeof(struct sockaddr_in));
	dest.sin_family = AF_INET;
	dest.sin_addr.s_addr = inet_addr(adr);
	dest.sin_port = htons((u_short)port);

	#ifdef WIN32
	status=connect(tmp_s,(SOCKADDR *)&dest,sizeof(dest));
	#else
	status=connect(tmp_s,(const struct sockaddr *)&dest,sizeof(dest));
	#endif
	if( status == SOCKET_ERROR )
		return BMD_ERR_OP_FAILED;

	(*s)=tmp_s;

	return BMD_OK;
}

long __keyservice_communicate(SOCKET s,char *buf,long len,
							 char **recv_buf,long *recv_length)
{
	long status;

	if( buf == NULL )
		return BMD_ERR_PARAM2;

	status=svc_tcp_sendall(s,buf,len);
	if( status != BMD_OK )
		return BMD_ERR_NET_WRITE;
	status=svc_tcp_recvall(s,0,recv_buf,recv_length); /* domyslny timeout to bedzie 5 sekund*/
	if( status != BMD_OK )
		return BMD_ERR_NET_READ;
	return BMD_OK;
}

long ks_sign_raw(void *info,GenBuf_t *input,GenBuf_t **output)
{
	ks_crypto_info_t *ks=NULL;
	long status;
	SOCKET s;
	char *req=NULL,*resp=NULL;
	long req_len,resp_len;

	bmd_ks_request_t *ks_req=NULL;
	bmd_ks_response_t *ks_resp=NULL;

	if( info == NULL )
		return BMD_ERR_PARAM1;
	if( input == NULL )
		return BMD_ERR_PARAM2;
	if( output == NULL )
		return BMD_ERR_PARAM3;
	if( (*output) != NULL )
		return BMD_ERR_PARAM3;

	ks=(ks_crypto_info_t *)info;

	if( ( ks->oper_type != 0 ) && ( ks->oper_type != 2 ) ) /* funkcja robi podpis, a typ operacji jest inny */
	{

		return BMD_ERR_FORMAT;
	}

	status=svc_encode_request(SVC_REQUEST_SIGN,input->buf,input->size,&ks_req);
	if( status != SVC_OK )
	{

		return BMD_ERR_OP_FAILED;
	}

	status=svc_serialize_request(ks_req,&req,&req_len);
	if( status != SVC_OK )
	{

		return BMD_ERR_OP_FAILED;
	}

	status=__keyservice_connect(ks->ks_adr,ks->ks_port,&s);
	if( status != BMD_OK )
	{

		return BMD_ERR_NET_CONNECT;
	}

	status=__keyservice_communicate(s,req,req_len,&resp,&resp_len);
	if( status != BMD_OK )
	{

		return status;
	}

	status=svc_decode_response(resp,resp_len,&ks_resp);
	if( status != SVC_OK )
	{

		return BMD_ERR_FORMAT;
	}

	if(ks_resp->resp_type != SVC_RESPONSE_SIGN )
	{

		return BMD_ERR_FORMAT;
	}

	if(ks_resp->status != SVC_RESPONSE_OK )
	{

		return BMD_ERR_OP_FAILED;
	}
#ifdef WIN32
	closesocket(s);
#else
	close(s);
#endif
	set_gen_buf2(ks_resp->resp_data,*((long *)(ks_resp->resp_data_length)),output);
	free(ks_resp);ks_resp=NULL;free(ks_req);ks_req=NULL;
	free0(req);

	return BMD_OK;
}

/* @note: a ta funkcja jest tutaj z rozpedu :-) - mozna ja przeniesc do cms_internal.c,
          ale czy to robi komus roznice ?
 */
long ks_decrypt_raw(void *info,GenBuf_t *input,GenBuf_t **output)
{
	ks_crypto_info_t *ks=NULL;
	long status;
	SOCKET s;
	char *req=NULL,*resp=NULL;
	long req_len,resp_len;

	bmd_ks_request_t *ks_req=NULL;
	bmd_ks_response_t *ks_resp=NULL;

	if( info == NULL )
		return BMD_ERR_PARAM1;
	if( input == NULL )
		return BMD_ERR_PARAM2;
	if( output == NULL )
		return BMD_ERR_PARAM3;
	if( (*output) != NULL )
		return BMD_ERR_PARAM3;

	ks=(ks_crypto_info_t *)info;

	if( ( ks->oper_type != 1 ) && ( ks->oper_type != 2 ) )/* funkcja robi deszyfrowanie, a typ operacji jest inny */
	{

		return BMD_ERR_FORMAT;
	}

	status=svc_encode_request(SVC_REQUEST_DECRYPT,input->buf,input->size,&ks_req);
	if( status != SVC_OK )
		return BMD_ERR_OP_FAILED;

	status=svc_serialize_request(ks_req,&req,&req_len);
	if( status != SVC_OK )
		return BMD_ERR_OP_FAILED;

	status=__keyservice_connect(ks->ks_adr,ks->ks_port,&s);
	if( status != BMD_OK )
		return BMD_ERR_NET_CONNECT;

	status=__keyservice_communicate(s,req,req_len,&resp,&resp_len);
	if( status != BMD_OK )
		return status;

	status=svc_decode_response(resp,resp_len,&ks_resp);
	if( status != SVC_OK )
		return status;

	if(ks_resp->resp_type != SVC_RESPONSE_DECRYPT )
	{

		return BMD_ERR_FORMAT;
	}
	if(ks_resp->status != SVC_RESPONSE_OK )
		return BMD_ERR_OP_FAILED;
#ifdef WIN32
	closesocket(s);
#else
	close(s);
#endif
	set_gen_buf2(ks_resp->resp_data,*((long *)(ks_resp->resp_data_length)),output);
	free(ks_resp);ks_resp=NULL;free(ks_req);ks_req=NULL;

	return BMD_OK;
}

long ks_encrypt_raw(void *info,GenBuf_t *input,GenBuf_t **output)
{
	long status,size;
	ks_crypto_info_t *ks=NULL;
	GenBuf_t *pub_key=NULL;
	char *buf=NULL;
	RSA *pubKey=NULL;

	if( info == NULL )
		return BMD_ERR_PARAM1;
	if( input == NULL )
		return BMD_ERR_PARAM2;
	if( output == NULL )
		return BMD_ERR_PARAM3;
	if( (*output) != NULL )
		return BMD_ERR_PARAM3;

	ks=(ks_crypto_info_t *)info;

	GetPubKeyFromCert(ks->ed_cert,&pub_key);
	if( pub_key == NULL )
		return BMD_ERR_OP_FAILED;
	buf=pub_key->buf;
	pubKey=d2i_RSAPublicKey(NULL,(const unsigned char **)&buf,pub_key->size);
	if(pubKey == NULL )
		return BMD_ERR_OP_FAILED;
	free_gen_buf(&pub_key);

	(*output)=(GenBuf_t *)malloc(sizeof(GenBuf_t));
	size=RSA_size(pubKey);
	(*output)->buf=(char *)malloc(size+2);
	memset((*output)->buf,0,size+1);
	status=RSA_public_encrypt(input->size, (unsigned char *)input->buf, (unsigned char*)(*output)->buf, pubKey, \
	RSA_PKCS1_PADDING);
	RSA_free(pubKey);
	if( status > 0 )
	{
		(*output)->size=status;
	}
	else
	{
		return BMD_ERR_OP_FAILED;
	}

	return BMD_OK;
}

long ks_s_prepare_ctx(void *info)
{
	SOCKET s;
	bmd_ks_request_t *ks_req=NULL;
	char *req=NULL,*resp=NULL;
	long req_len,resp_len;
	long status;

	ks_crypto_info_t *ks=NULL;
	if( info == NULL )
		return BMD_ERR_PARAM1;

	ks=(ks_crypto_info_t *)info;

	status=__keyservice_connect(ks->ks_adr,ks->ks_port,&s);
	if( status != BMD_OK )
		return BMD_ERR_NET_CONNECT;

	status=svc_encode_request(SVC_REQUEST_GET_S_CERT,NULL,0,&ks_req);
	if( status != SVC_OK )
		return BMD_ERR_OP_FAILED;

	status=svc_serialize_request(ks_req,&req,&req_len);
	if( status != SVC_OK )
		return BMD_ERR_OP_FAILED;

	status=__keyservice_communicate(s,req,req_len,&resp,&resp_len);
	if( status != BMD_OK )
		return BMD_ERR_OP_FAILED;

	set_gen_buf2(resp,resp_len,&(((ks_crypto_info_t *)info)->sig_cert));
	free0(req);free0(resp);

	return BMD_OK;
}

long ks_ed_prepare_ctx(void *info)
{
	SOCKET s;
	bmd_ks_request_t *ks_req=NULL;
	char *req=NULL,*resp=NULL;
	long req_len,resp_len;
	long status;

	ks_crypto_info_t *ks=NULL;
	if( info == NULL )
		return BMD_ERR_PARAM1;

	ks=(ks_crypto_info_t *)info;

	status=__keyservice_connect(ks->ks_adr,ks->ks_port,&s);
	if( status != BMD_OK )
		return BMD_ERR_NET_CONNECT;

	status=svc_encode_request(SVC_REQUEST_GET_ED_CERT,NULL,0,&ks_req);
	if( status != SVC_OK )
		return BMD_ERR_OP_FAILED;

	status=svc_serialize_request(ks_req,&req,&req_len);
	if( status != SVC_OK )
		return BMD_ERR_OP_FAILED;

	status=__keyservice_communicate(s,req,req_len,&resp,&resp_len);
	if( status != BMD_OK )
		return BMD_ERR_OP_FAILED;

	if( resp_len >=9 ) /* w przypadku bledu KS zwraca "UNDEFINED" */
	{
		if(memcmp(resp,"UNDEFINED",9) == 0 )
		{
			return BMD_ERR_NOTAVAIL;
		}
	}
	else
		return BMD_ERR_FORMAT;

	set_gen_buf2(resp,resp_len,&(((ks_crypto_info_t *)info)->ed_cert));
	free0(req);free0(resp);

	return BMD_OK;
}

long file_sign_raw(void *info,GenBuf_t *input,GenBuf_t **output)
{
long status;
file_crypto_info_t *tmp=NULL;
long size;
unsigned int ui_size;

	if(info==NULL)
	{
		PRINT_DEBUG("LIBBMDPKIERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(input==NULL)
	{
		PRINT_DEBUG("LIBBMDPKIERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}
	if(input->buf==NULL)
	{
		PRINT_DEBUG("LIBBMDPKIERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}
	if(input->size<=0)
	{
		PRINT_DEBUG("LIBBMDPKIERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}
	if(output==NULL)
	{
		PRINT_DEBUG("LIBBMDPKIERR Invalid third parameter value. Error=%i\n",BMD_ERR_PARAM3);
		return BMD_ERR_PARAM3;
	}
	if((*output)!=NULL)
	{
		PRINT_DEBUG("LIBBMDPKIERR Invalid third parameter value. Error=%i\n",BMD_ERR_PARAM3);
		return BMD_ERR_PARAM3;
	}

	tmp=(file_crypto_info_t *)info;

	if(tmp->hash_alg==BMD_HASH_ALGO_SHA1)
	{
		(*output)=(GenBuf_t *)malloc(sizeof(GenBuf_t));
		if((*output)==NULL)
		{
			PRINT_DEBUG("LIBBMDPKIERR Memory error. Error=%i\n",BMD_ERR_MEMORY);
			return BMD_ERR_MEMORY;
		}
		size=RSA_size(tmp->privateKey);
		(*output)->buf=(char *)malloc(size+2);
		if((*output)->buf==NULL)
		{
			PRINT_DEBUG("LIBBMDPKIERR Memory error. Error=%i\n",BMD_ERR_MEMORY);
			return BMD_ERR_MEMORY;
		}
		memset((*output)->buf,0,size+1);
		ui_size = (*output)->size;
		status=RSA_sign(NID_sha1, (unsigned char*)(input->buf), input->size, (unsigned char*)((*output)->buf), \
		&ui_size, tmp->privateKey);
		if(status!=1)
		{
			PRINT_DEBUG("LIBBMDPKIERR Error. Error=%i\n",BMD_ERR_OP_FAILED);
			return BMD_ERR_OP_FAILED;
		}
		(*output)->size = ui_size;
	}
	else
	{
		PRINT_DEBUG("LIBBMDPKIERR Method unimplemented. Error=%i\n",BMD_ERR_UNIMPLEMENTED);
		return BMD_ERR_UNIMPLEMENTED;
	}
	return 0;
}

long __bmd_get_signed_data(GenBuf_t *input,SignedData_t **signed_data)
{
ContentInfo_t *content_info=NULL;

	if(input==NULL)
	{
		PRINT_DEBUG("LIBBMDPKIERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(input->buf==NULL)
	{
		PRINT_DEBUG("LIBBMDPKIERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(input->size<=0)
	{
		PRINT_DEBUG("LIBBMDPKIERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}

	if(signed_data==NULL)
	{
		PRINT_DEBUG("LIBBMDPKIERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}
	if((*signed_data)!=NULL)
	{
		PRINT_DEBUG("LIBBMDPKIERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	/* zdekoduj struktura jako ContentInfo_t */
	BMD_FOK(get_CMScontent_from_GenBuf(input,&content_info));

	/* pobierz struktura SignedData_t */
	BMD_FOK(ContentInfo_get_SignedData(content_info,signed_data));

	asn_DEF_ContentInfo.free_struct(&asn_DEF_ContentInfo, content_info, 0);

	return BMD_OK;
}

long __bmd_get_signing_pubkey(SignedData_t *signed_data,RSA **public_key)
{
GenBufList_t *cert_list=NULL;
GenBuf_t *pub_key=NULL;
char *tmp=NULL;

	if(signed_data==NULL)
	{
		PRINT_DEBUG("LIBBMDPKIERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}

	if(public_key==NULL)
	{
		PRINT_DEBUG("LIBBMDPKIERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}
	if(*public_key!=NULL)
	{
		PRINT_DEBUG("LIBBMDPKIERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	/* pobierz ilosc odbiorcow podpisanych danych wraz z certyfikatami w DER */
	BMD_FOK(GetX509CertListFromSignedData(signed_data,&cert_list));
	if(cert_list->size==0)
	{
		PRINT_DEBUG("LIBBMDPKIERR Method unimplemented. Error=%i\n",BMD_ERR_UNIMPLEMENTED);
		return BMD_ERR_UNIMPLEMENTED;
	}
	if(cert_list->size>1)
	{
		PRINT_DEBUG("LIBBMDPKIERR Method unimplemented. Error=%i\n",BMD_ERR_UNIMPLEMENTED);
		return BMD_ERR_UNIMPLEMENTED;
	}

	BMD_FOK(GetPubKeyFromCert(cert_list->gbufs[0],&pub_key));

	tmp=pub_key->buf;
	(*public_key)=d2i_RSAPublicKey(NULL,(const unsigned char **)&tmp,pub_key->size);
	free_gen_buf(&pub_key);
	free_gen_buf(&(cert_list->gbufs[0]));
	free(cert_list);cert_list=NULL;

	return BMD_OK;
}

long __bmd_compute_hash_for_sig_check(	SignedData_t *signed_data,
							GenBuf_t *opt_content,
							GenBuf_t **hash,
							GenBuf_t **sig_value)
{
GenBuf_t *tmp_buf			= NULL;
char SHA_oid[]			= {OID_HASH_FUNCTION_SHA1};
long *sig_oid			= NULL;
char *sig_oid_str			= NULL;
long sig_oid_size			= 0;
long err				= 0;
bmd_crypt_ctx_t *hash_ctx	= NULL;
GenBuf_t *attrbuf			= NULL;

	if(signed_data==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(hash==NULL)			{	BMD_FOK(BMD_ERR_PARAM3);	}
	if((*hash)!=NULL)			{	BMD_FOK(BMD_ERR_PARAM3);	}
	if(sig_value==NULL)		{	BMD_FOK(BMD_ERR_PARAM4);	}
	if((*sig_value)!=NULL)		{	BMD_FOK(BMD_ERR_PARAM4);	}

	sig_oid_size=OID_to_ulong_alloc2(&(signed_data->signerInfos.list.array[0]->digestAlgorithm.algorithm),&sig_oid);
	ulong_to_str_of_OID2(sig_oid, sig_oid_size, &sig_oid_str);
	if(strcmp(SHA_oid,sig_oid_str)!=0)
	{
		BMD_FOK(-1);
	}

	if(signed_data->signerInfos.list.array[0]->signedAttrs!=NULL)
	{
		/* zakoduj w DER atrybuty */
		err=asn1_encode(	&asn_DEF_SignedAttributes,signed_data->signerInfos.list.array[0]->signedAttrs, NULL,
					&attrbuf);
		if ( err != 0 )
		{
			PRINT_DEBUG("LIBBMDPKIERR Error. Error=%li\n",err);
			return err;
		}
		BMD_FOK(bmd_set_ctx_hash(&hash_ctx,BMD_HASH_ALGO_SHA1));
		BMD_FOK(bmd_hash_data(attrbuf,&hash_ctx,hash,NULL));
	}
	else
	{
		/* policz skrot z zewnetrznej zawartosci lub tylko z zawartosci */
		if(signed_data->encapContentInfo.eContent!=NULL)
		{
			set_gen_buf2((char *)signed_data->encapContentInfo.eContent->buf, signed_data->encapContentInfo.eContent->size,&tmp_buf);
			BMD_FOK(bmd_set_ctx_hash(&hash_ctx,BMD_HASH_ALGO_SHA1));
			BMD_FOK(bmd_hash_data(tmp_buf,&hash_ctx,hash,NULL));
			free_gen_buf(&tmp_buf);
		}
		else
		{
			BMD_FOK(bmd_set_ctx_hash(&hash_ctx,BMD_HASH_ALGO_SHA1));
			BMD_FOK(bmd_hash_data(opt_content,&hash_ctx,hash,NULL));
		}
	}
	set_gen_buf2((char *)signed_data->signerInfos.list.array[0]->signature.buf,signed_data->signerInfos.list.array[0]->signature.size,sig_value);

	free(sig_oid_str); sig_oid_str=NULL;
	free(sig_oid); sig_oid=NULL;

	return BMD_OK;
}
