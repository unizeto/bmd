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
#include <bmd/libbmdasn1_common/CommonUtils.h>

#ifndef TRUE
#define TRUE 1
#endif

long __bmd_encrypt_key_with_pubkey(long enc_algo,GenBuf_t *certificate,GenBuf_t *sym_key,GenBuf_t **key)
{
GenBuf_t *pub_key=NULL;
char *buf=NULL;
RSA *rsa_pub_key=NULL;
long status;

        if(certificate==NULL)
	{
		PRINT_DEBUG("LIBBMDPKIERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}
        if(certificate->buf==NULL)
	{
		PRINT_DEBUG("LIBBMDPKIERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}
        if(certificate->size<=0)
	{
		PRINT_DEBUG("LIBBMDPKIERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

        if(sym_key==NULL)
	{
		PRINT_DEBUG("LIBBMDPKIERR Invalid third parameter value. Error=%i\n",BMD_ERR_PARAM3);
		return BMD_ERR_PARAM3;
	}
        if(sym_key->buf==NULL)
	{
		PRINT_DEBUG("LIBBMDPKIERR Invalid third parameter value. Error=%i\n",BMD_ERR_PARAM3);
		return BMD_ERR_PARAM3;
	}
        if(sym_key->size<=0)
	{
		PRINT_DEBUG("LIBBMDPKIERR Invalid third parameter value. Error=%i\n",BMD_ERR_PARAM3);
		return BMD_ERR_PARAM3;
	}

        if(key==NULL)
	{
		PRINT_DEBUG("LIBBMDPKIERR Invalid third parameter value. Error=%i\n",BMD_ERR_PARAM3);
		return BMD_ERR_PARAM3;
	}
        if((*key)!=NULL)
	{
		PRINT_DEBUG("LIBBMDPKIERR Invalid third parameter value. Error=%i\n",BMD_ERR_PARAM3);
		return BMD_ERR_PARAM3;
	}

	GetPubKeyFromCert(certificate,&pub_key);
	buf=pub_key->buf;
	rsa_pub_key=d2i_RSAPublicKey(NULL,(const unsigned char **)&buf, pub_key->size);

	free_gen_buf(&pub_key);

	(*key) = (GenBuf_t *)malloc(sizeof(GenBuf_t));
	if ( (*key) == NULL )
	{
		PRINT_DEBUG("LIBBMDPKIERR Memory error. Error=%i\n",ERR_NO_MEMORY);
		return ERR_NO_MEMORY;
	}
	memset((*key), 0, sizeof(GenBuf_t));
	(*key)->size = 0;
	(*key)->buf=(char *)malloc(RSA_size(rsa_pub_key));

	if( enc_algo == BMD_CRYPT_ALGO_DES3 )
	{
		status=RSA_public_encrypt(EVP_CIPHER_key_length(EVP_des_ede3_cbc()), (unsigned char*)(sym_key->buf),
		(unsigned char*)((*key)->buf), rsa_pub_key,RSA_PKCS1_PADDING);
		(*key)->size=RSA_size(rsa_pub_key);
		RSA_free(rsa_pub_key);rsa_pub_key=NULL;
	}
	else
	{
		PRINT_DEBUG("LIBBMDPKIERR Unimplemented method. Error=%i\n",BMD_ERR_UNIMPLEMENTED);
		return BMD_ERR_UNIMPLEMENTED;
	}
	return 0;
}

long __bmd_add_recipient(GenBuf_t *cert,bmd_encryption_params_t *options,GenBuf_t *symmetric_key,EnvelopedData_t **envData)
{
RecipientInfo_t *recipient=NULL;
GenBuf_t *key=NULL;
Certificate_t *asncert=NULL;

	if( cert == NULL )
		return BMD_ERR_PARAM1;

	recipient = mallocStructure(sizeof(RecipientInfo_t));
	if ( recipient == NULL )
        {
		PRINT_DEBUG("LIBBMDPKIERR Memory error. Error=%i\n",ERR_NO_MEMORY);
		return ERR_NO_MEMORY;
	}

	__bmd_encrypt_key_with_pubkey(options->encryption_algo,cert,symmetric_key,&key);

	BMD_FOK(RecipientInfo_generate(recipient,cert,key));

	BMD_FOK(EnvelopedData_add_RecipientInfo(*envData, recipient));

	free_gen_buf(&key);

	ber_decode(0, &asn_DEF_Certificate, (void **)&asncert, cert->buf,cert->size);
	BMD_FOK(EnvelopedData_add_Certificate(*envData, asncert));

	asn_DEF_Certificate.free_struct(&asn_DEF_Certificate,asncert, 0);

	return BMD_OK;
}

long __bmd_get_enveloped_data(	GenBuf_t **input,
						long twf_offset,
						EnvelopedData_t **enveloped_data,
						long twf_input_free)
{
long status 				= 0;
long *content_oid				= NULL;
long content_oid_size			= 0;
char *content_oid_str			= NULL;
char enveloped_oid[]			= {OID_CMS_ID_ENVELOPED_DATA};
long original_size			= 0;
ContentInfo_t *content_info		= NULL;
char *original_buf			= NULL;

	if (*input==NULL)				{	BMD_FOK(BMD_ERR_PARAM1);	}
	if ((*input)->buf==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if ((*input)->size<=0)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if ((*input)->size <= twf_offset)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (twf_input_free < 0)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (twf_input_free > 1)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(enveloped_data==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if((*enveloped_data)!=NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	/*Musze dokonac przesunieca zeby ominac ewentualny prefix, na przykla identyfikator zadania */
	original_buf = (*input)->buf;
	original_size = (*input)->size;

	(*input)->buf = (*input)->buf + twf_offset;
	(*input)->size = (*input)->size - twf_offset;

	/* zdekoduj struktura jako ContentInfo_t */
	status = get_CMScontent_from_GenBuf(*input, &content_info);
	/*Przywracam oryginalny adres*/
	(*input)->buf = original_buf;
	(*input)->size = original_size;
	BMD_FOK(status);

	/*Czy zwolnic bufor wejsciowy*/
	if(twf_input_free == 1)
	{
		free_gen_buf(input);
	}

	/* sprawdz poprawnosc OID'u */
	content_oid_size=OID_to_ulong_alloc2(&(content_info->contentType),&content_oid);
	ulong_to_str_of_OID2( content_oid, content_oid_size, &content_oid_str);

	if(strcmp(enveloped_oid,content_oid_str)==0)
	{
		ber_decode(0,&asn_DEF_EnvelopedData,(void **)enveloped_data,content_info->content.buf, content_info->content.size);
	}
	else
	{
		BMD_FOK(BMD_ERR_FORMAT);
	}

	free(content_oid);content_oid=NULL;
	free(content_oid_str);content_oid_str=NULL;
	asn_DEF_ContentInfo.free_struct(&asn_DEF_ContentInfo,content_info,0);content_info=NULL;

	return BMD_OK;
}

long __bmd_check_and_get_encrypted_key(	EnvelopedData_t *enveloped_data,
							GenBuf_t **output)
{
RecipientInfo_t *recipient_info	= NULL;
KeyTransRecipientInfo_t *ktri		= NULL;
char RSA_oid[]				= {OID_RSAENCRYPTION};
long *key_encrypt_algo_oid		= NULL;
char *key_encrypt_algo_oid_str	= NULL;
long key_encrypt_algo_oid_size	= 0;

	if(enveloped_data==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(output==NULL)				{	BMD_FOK(BMD_ERR_PARAM2);	}
	if((*output)!=NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}

	recipient_info=enveloped_data->recipientInfos.list.array[0];
	if(recipient_info->present!=RecipientInfo_PR_ktri)
	{
		BMD_FOK(BMD_ERR_FORMAT);
	}

	/* sprawdzenie czy algorytmem szyfrujacym klucz jest RSA */
	ktri=&(recipient_info->choice.ktri);
	key_encrypt_algo_oid_size=OID_to_ulong_alloc2(&(ktri->keyEncryptionAlgorithm.algorithm),&key_encrypt_algo_oid);
	ulong_to_str_of_OID2(key_encrypt_algo_oid, key_encrypt_algo_oid_size, &key_encrypt_algo_oid_str);
	if(strcmp(RSA_oid,key_encrypt_algo_oid_str)!=0)
	{
		BMD_FOK(BMD_ERR_UNIMPLEMENTED);
	}

	BMD_FOK(set_gen_buf2( (char*)ktri->encryptedKey.buf,ktri->encryptedKey.size,output));

	free(key_encrypt_algo_oid);key_encrypt_algo_oid=NULL;

	return BMD_OK;
}

long __bmd_check_and_get_IV_and_enc_content(EnvelopedData_t *enveloped_data,GenBuf_t **IV,GenBuf_t **enc_content)
{
long DES3_oid[]={1,2,840,113549,3,7};
EncryptedContentInfo_t *encrypted_info=NULL;
long *sym_algo_oid=NULL;
OCTET_STRING_t *octet_iv=NULL;

	if(enveloped_data==NULL)
	{
		PRINT_DEBUG("LIBBMDPKIERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(IV==NULL)
	{
		PRINT_DEBUG("LIBBMDPKIERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}
	if((*IV)!=NULL)
	{
		PRINT_DEBUG("LIBBMDPKIERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	if(enc_content==NULL)
	{
		PRINT_DEBUG("LIBBMDPKIERR Invalid third parameter value. Error=%i\n",BMD_ERR_PARAM3);
		return BMD_ERR_PARAM3;
	}
	if((*enc_content)!=NULL)
	{
		PRINT_DEBUG("LIBBMDPKIERR Invalid third parameter value. Error=%i\n",BMD_ERR_PARAM3);
		return BMD_ERR_PARAM3;
	}

	encrypted_info=&(enveloped_data->encryptedContentInfo);

	/* sprawdzenie czy algorytmem szyfrujacym zawartosc jest 3DES */
	OID_to_ulong_alloc2(&(encrypted_info->contentEncryptionAlgorithm.algorithm),&sym_algo_oid);
	if(OID_cmp2(DES3_oid,sizeof(DES3_oid),sym_algo_oid,sizeof(DES3_oid))!=0)
	{
		PRINT_DEBUG("LIBBMDPKIERR Unimplemented method. Error=%i\n",BMD_ERR_UNIMPLEMENTED);
		return BMD_ERR_UNIMPLEMENTED;
	}

	ber_decode(0, &asn_DEF_OCTET_STRING,
			   (void **)&octet_iv,
			   encrypted_info->contentEncryptionAlgorithm.parameters->buf,
		       encrypted_info->contentEncryptionAlgorithm.parameters->size);

	BMD_FOK(set_gen_buf2((char *)(octet_iv->buf),octet_iv->size,IV));
	BMD_FOK(set_gen_buf2((char *)(encrypted_info->encryptedContent->buf), encrypted_info->encryptedContent->size, \
	enc_content));
	free(sym_algo_oid);sym_algo_oid=NULL;
	asn_DEF_OCTET_STRING.free_struct(&asn_DEF_OCTET_STRING, octet_iv, 0);

	return BMD_OK;
}

long file_decrypt_raw(	void *info,
				GenBuf_t *input,
				GenBuf_t **output)
{
long status;
file_crypto_info_t *tmp=NULL;
long size;

	if(info==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(input==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(input->buf==NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(input->size<=0)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if((*output)!=NULL)	{	BMD_FOK(BMD_ERR_PARAM3);	}


	tmp=(file_crypto_info_t *)info;

	(*output)=(GenBuf_t *)malloc(sizeof(GenBuf_t));
	size=RSA_size(tmp->publicKey);
	(*output)->buf=(char *)malloc(size+2);
	memset((*output)->buf,0,size+1);
	status=RSA_private_decrypt(input->size, (unsigned char*)(input->buf), (unsigned char*)((*output)->buf), tmp->privateKey,RSA_PKCS1_PADDING);
	if(status>0)
	{
		(*output)->size=status;
	}
	else
	{
		BMD_FOK(LIBBMDPKI_CMS_INTERNAL_PRIVATE_KEY_DECRYPT_ERR);
	}

	return BMD_OK;
}

long file_encrypt_raw(void *info,GenBuf_t *input,GenBuf_t **output)
{
	long status;
	file_crypto_info_t *tmp=NULL;
	long size;

	tmp=(file_crypto_info_t *)info;

	(*output)=(GenBuf_t *)malloc(sizeof(GenBuf_t));
	size=RSA_size(tmp->publicKey);
	(*output)->buf=(char *)malloc(size+2);
	memset((*output)->buf,0,size+1);
        status=RSA_public_encrypt(input->size, (unsigned char*)(input->buf), (unsigned char*)((*output)->buf), \
	tmp->publicKey,RSA_PKCS1_PADDING);
	if(status)
	{
		(*output)->size=status;
	}
	else
	{
		return -1;
	}
	return 0;
}
