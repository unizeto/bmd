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

#ifndef TRUE
#define TRUE 1
#endif
#ifdef WIN32
#pragma warning(disable:4100)
#endif
long bmd_encrypt_data(GenBuf_t *input,bmd_crypt_ctx_t *ctx,GenBuf_t **output,void *opt)
{
EnvelopedData_t *envData=NULL;
bmd_crypt_ctx_t *symmetric_ctx=NULL;
bmd_encryption_params_t *options=NULL;
GenBuf_t *tmp_buf=NULL;
ContentInfo_t *content=NULL;

	if(input==NULL)
	{
		PRINT_ERROR("LIBBMDPKIERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(input->buf==NULL)
	{
		PRINT_ERROR("LIBBMDPKIERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(input->size<=0)
	{
		PRINT_ERROR("LIBBMDPKIERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}

	if(ctx==NULL)
	{
		PRINT_ERROR("LIBBMDPKIERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	if(ctx->file)
	{
		BMD_FOK(bmd_check_ctx(ctx,BMD_CTX_SOURCE_FILE,BMD_CTX_TYPE_ASYM_E));
	}
	else
	{
		if(ctx->pkcs11)
		{
			BMD_FOK(bmd_check_ctx(ctx,BMD_CTX_SOURCE_PKCS11,BMD_CTX_TYPE_ASYM_E));
		}
		else
		{
			if(ctx->ks)
			{
				BMD_FOK(bmd_check_ctx(ctx,BMD_CTX_SOURCE_KS,BMD_CTX_TYPE_ASYM));
			}
			else
			{
				PRINT_DEBUG("LIBBMDPKIERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
				return BMD_ERR_PARAM2;
			}
		}
	}
	if(output==NULL)
	{
		PRINT_ERROR("LIBBMDPKIERR Invalid third parameter value. Error=%i\n",BMD_ERR_PARAM3);
		return BMD_ERR_PARAM3;
	}
	if((*output)!=NULL)
        {
		PRINT_ERROR("LIBBMDPKIERR Invalid third parameter value. Error=%i\n",BMD_ERR_PARAM3);
		return BMD_ERR_PARAM3;
	}

	if(opt!=NULL)
	{
		options=(bmd_encryption_params_t *)opt;
	}
        else
        {
		options=(bmd_encryption_params_t *)malloc(sizeof(bmd_encryption_params_t));
		memset(options,0,sizeof(bmd_encryption_params_t));
		options->encryption_type=BMD_CMS_ENV_INTERNAL;
		options->encryption_algo=BMD_CRYPT_ALGO_DES3;
		options->opt_enc_content=NULL;
        }
        envData = (EnvelopedData_t *)malloc(sizeof(EnvelopedData_t));
        if(envData==NULL)
	{
		PRINT_ERROR("LIBBMDPKIERR Memory error. Error=%i\n",BMD_ERR_MEMORY);
		return BMD_ERR_MEMORY;
	}
        memset(envData,0,sizeof(EnvelopedData_t));

	/* stworzenie danych zaszyfrowanych */
        BMD_FOK(bmd_set_ctx_sym(&symmetric_ctx,options->encryption_algo,NULL,NULL));
        BMD_FOK(bmd_symmetric_encrypt(input,&symmetric_ctx,&tmp_buf,NULL));

	if( options->encryption_type == BMD_CMS_ENV_EXTERNAL )
	{
		set_gen_buf2(tmp_buf->buf,tmp_buf->size,&(options->opt_enc_content));
	}

	BMD_FOK(EnvelopedData_set_EncryptedData(envData, symmetric_ctx->sym->IV, tmp_buf, options));

	free_gen_buf(&tmp_buf);

	if(ctx->file)
	{
		BMD_FOK(__bmd_add_recipient(ctx->file->cert,options,symmetric_ctx->sym->key,&envData));
	}
	else
	{
		if(ctx->pkcs11)
		{
			BMD_FOK(__bmd_add_recipient(ctx->pkcs11->cert,options,symmetric_ctx->sym->key,&envData));
		}
		else
			if(ctx->ks)
			{
				BMD_FOK(__bmd_add_recipient(ctx->ks->ed_cert,options,symmetric_ctx->sym->key,&envData));
			}
	}
	BMD_FOK(EnvelopedData_count_Version(envData));

	// utworz general CMS content
	content = (ContentInfo_t *)malloc(sizeof(ContentInfo_t));
	if ( content == NULL )
	{
		PRINT_ERROR("LIBBMDPKIERR Memory error. Error=%i\n",BMD_ERR_MEMORY);
		return BMD_ERR_MEMORY;
	}
        memset(content,0,sizeof(ContentInfo_t));

	BMD_FOK(ContentInfo_set_EnvelopedData(content, envData));

	BMD_FOK(der_encode_ContentInfo(content, output));

	if ( content )
	{
		asn_DEF_ContentInfo.free_struct(&asn_DEF_ContentInfo, content, 0);
	}
	if ( envData )
	{
		asn_DEF_EnvelopedData.free_struct(&asn_DEF_EnvelopedData, envData, 0);
	}

	free(options);options=NULL;
	BMD_FOK(bmd_ctx_destroy(&symmetric_ctx));
	return BMD_OK;
}

long bmd_decrypt_data(	GenBuf_t **input,
			long twf_offset,
			bmd_crypt_ctx_t *ctx,
			GenBuf_t **output,
			void *opt,
			long twf_input_free)
{
EnvelopedData_t *enveloped_data	= NULL;
GenBuf_t *iv				= NULL;
GenBuf_t *tmp_buf				= NULL;
GenBuf_t *secret_key			= NULL;
bmd_crypt_ctx_t *sym_ctx		= NULL;
GenBuf_t *enc_content			= NULL;

	if(*input==NULL)				{	BMD_FOK(BMD_ERR_PARAM1);	}
	if((*input)->buf==NULL)				{	BMD_FOK(BMD_ERR_PARAM1);	}
	if((*input)->size<=0)				{	BMD_FOK(BMD_ERR_PARAM1);	}
	if((*input)->size <= twf_offset)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(twf_input_free < 0 || twf_input_free > 1)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(ctx==NULL)					{	BMD_FOK(BMD_ERR_PARAM3);	}
	if(output==NULL)				{	BMD_FOK(BMD_ERR_PARAM4);	}
	if((*output)!=NULL)				{	BMD_FOK(BMD_ERR_PARAM4);	}


	if(ctx->pkcs11!=NULL)
	{
		BMD_FOK(bmd_check_ctx(ctx,BMD_CTX_SOURCE_PKCS11,BMD_CTX_TYPE_ASYM_D));
	}
	else
	{
		if(ctx->file!=NULL)
		{
			BMD_FOK(bmd_check_ctx(ctx,BMD_CTX_SOURCE_FILE,BMD_CTX_TYPE_ASYM_D));
		}
		else
		{
			if( ctx->ks != NULL )
			{
				BMD_FOK(bmd_check_ctx(ctx,BMD_CTX_SOURCE_KS,BMD_CTX_TYPE_ASYM));
			}
			else
			{
				BMD_FOK(BMD_ERR_PARAM2);
			}
		}
	}

	BMD_FOK_CHG(__bmd_get_enveloped_data(input, twf_offset, &enveloped_data, twf_input_free), BMD_ERR_FORMAT);
	BMD_FOK_CHG(__bmd_check_and_get_encrypted_key(enveloped_data,&tmp_buf), BMD_ERR_FORMAT);
	BMD_FOK_CHG(__bmd_check_and_get_IV_and_enc_content(enveloped_data,&iv,&enc_content), BMD_ERR_FORMAT);

	/* zdeszyfrowanie zaszyfrowanego klucza symetrycznego */
	if(ctx->pkcs11)
	{
		BMD_FOK(bmd_pkcs11_decrypt(ctx->pkcs11,tmp_buf,&secret_key));

	}
	else if(ctx->file)
	{
		BMD_FOK(file_decrypt_raw(ctx->file,tmp_buf,&secret_key));

	}
	else	if(ctx->ks)
	{
		BMD_FOK(ctx->ks->decrypt((void *)ctx->ks,tmp_buf,&secret_key));

	}

	asn_DEF_EnvelopedData.free_struct(&asn_DEF_EnvelopedData,enveloped_data,0);enveloped_data=NULL;

	/* zdeszyfrowanie zawartosci */
	BMD_FOK(bmd_set_ctx_sym(&sym_ctx,BMD_CRYPT_ALGO_DES3,secret_key,iv));
	BMD_FOK(bmd_symmetric_decrypt(enc_content, 0, &sym_ctx,output,NULL));

	free_gen_buf(&secret_key);
	free_gen_buf(&iv);
	free_gen_buf(&tmp_buf);
	free_gen_buf(&enc_content);
	bmd_ctx_destroy(&sym_ctx);

	return BMD_OK;
}
