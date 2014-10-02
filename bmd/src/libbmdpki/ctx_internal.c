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

long _bmd_pkcs11_crypto_info_init(pkcs11_crypto_info_t **out)
{
	if( out == NULL )
	{
		PRINT_DEBUG("LIBBMDPKIERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if( (*out) != NULL )
	{
		PRINT_DEBUG("LIBBMDPKIERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}

	(*out)=(pkcs11_crypto_info_t *)malloc(sizeof(pkcs11_crypto_info_t));
	if( (*out) == NULL )
	{
		PRINT_DEBUG("LIBBMDPKIERR Memory error. Error=%i\n",BMD_ERR_MEMORY);
		return BMD_ERR_MEMORY;
	}

	memset(*out,0,sizeof(pkcs11_crypto_info_t));
	
	/** @info domyslnie ustawiamy funkcje skrotu na SHA-1 - jak ktos bedzie chcial zmienic musi wywolac odpowiednia funkcje */
	(*out)->hash_alg=BMD_HASH_ALGO_SHA1;

	return 0;
}

long _bmd_file_crypto_info_init(file_crypto_info_t **out)
{
	if( out == NULL )
	{
		PRINT_DEBUG("LIBBMDPKIERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if( (*out) != NULL )
	{
		PRINT_DEBUG("LIBBMDPKIERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}

	(*out)=(file_crypto_info_t *)malloc(sizeof(file_crypto_info_t));
	if( (*out) == NULL )
	{
		PRINT_DEBUG("LIBBMDPKIERR Memory error. Error=%i\n",BMD_ERR_MEMORY);
		return BMD_ERR_MEMORY;
	}

	memset(*out,0,sizeof(file_crypto_info_t));

	/** @info domyslnie ustawiamy funkcje skrotu na SHA-1 - jak ktos bedzie chcial zmienic musi wywolac odpowiednia funkcje */
	(*out)->hash_alg=BMD_HASH_ALGO_SHA1;

	return 0;
}

long _bmd_sym_crypto_info_init(symmetric_crypto_info_t **out)
{
	if( out == NULL )
	{
		PRINT_DEBUG("LIBBMDPKIERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if( (*out) != NULL )
	{
		PRINT_DEBUG("LIBBMDPKIERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}

	(*out)=(symmetric_crypto_info_t *)malloc(sizeof(symmetric_crypto_info_t));
	if( (*out) == NULL )
	{
		PRINT_DEBUG("LIBBMDPKIERR Memory error. Error=%i\n",BMD_ERR_MEMORY);
		return BMD_ERR_MEMORY;
	}

	(*out)->key=NULL;
	(*out)->IV=NULL;
	
	/** @info domyslnie ustawiamy funkcje szyfrujaca na DES3 - jak ktos bedzie chcial zmienic musi wywolac odpowiednia funkcje */
	(*out)->algo_type=BMD_CRYPT_ALGO_DES3;

	memset((*out)->last_block,0,EVP_MAX_KEY_LENGTH);
	(*out)->last_block_length=0;
	(*out)->sym_params=BMD_SYM_SINGLE_ROUND;    /* domyslnie liczymy wszystko w jednym przebiegu */
	(*out)->sym_stage=BMD_SYM_STAGE_FIRST;
	return BMD_OK;
}

long _bmd_hash_crypto_info_init(hash_crypto_info_t **out)
{
	if( out == NULL )
	{
		PRINT_DEBUG("LIBBMDPKIERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if( (*out) != NULL )
	{
		PRINT_DEBUG("LIBBMDPKIERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}

	(*out)=(hash_crypto_info_t *)malloc(sizeof(hash_crypto_info_t));
	if( (*out) == NULL )
	{
		PRINT_DEBUG("LIBBMDPKIERR Memory error. Error=%i\n",BMD_ERR_MEMORY);
		return BMD_ERR_MEMORY;
	}

	/** @info domyslnie ustawiamy funkcje skrotu na SHA-1 - jak ktos bedzie chcial zmienic musi wywolac odpowiednia funkcje */
	(*out)->hash_alg=BMD_HASH_ALGO_SHA1;

	(*out)->hash_params=BMD_HASH_SINGLE_ROUND; /* domyslnie liczymy wszystko w jednym przebiegu */
	(*out)->hash_stage=BMD_HASH_STAGE_FIRST;
	
	/* inicjalizacja kontekstow */
	(*out)->md=NULL;
	(*out)->md_ctx=NULL;
	
	(*out)->hash_size=0;
	
	return BMD_OK;
}

long _bmd_ks_crypto_info_init(ks_crypto_info_t **ks_info)
{
	if( ks_info == NULL )
		return BMD_ERR_PARAM1;
	if( (*ks_info) != NULL )
		return BMD_ERR_PARAM1;

	(*ks_info)=(ks_crypto_info_t *)malloc(sizeof(ks_crypto_info_t));
	if( (*ks_info) == NULL )
		return BMD_ERR_MEMORY;
	memset(*ks_info,0,sizeof(ks_crypto_info_t));

	(*ks_info)->ks_resp_status=-1;
	(*ks_info)->oper_type=-1;

	return BMD_OK;
}

long __bmd_check_ctx_source_and_type(long ctx_source,long ctx_type)
{
	if( ctx_source == BMD_CTX_SOURCE_WIN )
	{
		PRINT_DEBUG("LIBBMDPKIERR Unimplemented method. Error=%i\n",BMD_ERR_UNIMPLEMENTED);
		return BMD_ERR_UNIMPLEMENTED;
	}

	if( (ctx_source == BMD_CTX_SOURCE_NONE) && ( ctx_type != BMD_CTX_TYPE_SYM) && (ctx_type != BMD_CTX_TYPE_HASH) )
	{
		PRINT_DEBUG("LIBBMDPKIERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	if( ( ctx_source != BMD_CTX_SOURCE_FILE ) && ( ctx_source != BMD_CTX_SOURCE_PKCS11 ) &&
		 ( ctx_source != BMD_CTX_SOURCE_NONE ) && (ctx_source != BMD_CTX_SOURCE_KS) )
	{
		PRINT_DEBUG("LIBBMDPKIERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	if( ( ctx_type != BMD_CTX_TYPE_ASYM )   && ( ctx_type != BMD_CTX_TYPE_SYM )   && 
	    ( ctx_type != BMD_CTX_TYPE_HASH )   && ( ctx_type != BMD_CTX_TYPE_SIG )   && 
            ( ctx_type != BMD_CTX_TYPE_ASYM_E ) && ( ctx_type != BMD_CTX_TYPE_ASYM_D) )
	{
		PRINT_DEBUG("LIBBMDPKIERR Invalid third parameter value. Error=%i\n",BMD_ERR_PARAM3);
		return BMD_ERR_PARAM3;
	}

	if( (ctx_source == BMD_CTX_SOURCE_KS) && (ctx_type != BMD_CTX_TYPE_ASYM) )
	{
		PRINT_DEBUG("LIBBMDPKIERR Invalid third parameter value. Error=%i\n",BMD_ERR_PARAM3);
		return BMD_ERR_PARAM3;
	}

	return BMD_OK;
}

long __bmd_cert_to_pubkey(GenBuf_t *cert,RSA **publicKey)
{
X509 *tmp_cert=NULL;
unsigned char *p=NULL;
EVP_PKEY *tmp_key=NULL;

	p=(unsigned char*)cert->buf;
	if(d2i_X509(&tmp_cert, (CONST_CAST unsigned char **)&p, cert->size)==NULL)
	{
		PRINT_DEBUG("LIBBMDPKIERR Format error. Error=%i\n",BMD_ERR_FORMAT);
		return BMD_ERR_FORMAT;
	}
	
	tmp_key=X509_get_pubkey(tmp_cert);
	if(tmp_key==NULL)
	{
		PRINT_DEBUG("LIBBMDPKIERR Error. Error=%i\n",BMD_ERR_OP_FAILED);
		return BMD_ERR_OP_FAILED;
	}

	(*publicKey)=EVP_PKEY_get1_RSA(tmp_key);
	if((*publicKey)==NULL)
	{
		PRINT_DEBUG("LIBBMDPKIERR Error. Error=%i\n",BMD_ERR_OP_FAILED);
		return BMD_ERR_OP_FAILED;
	}
	
	X509_free(tmp_cert);tmp_cert=NULL;
	return BMD_OK;
}
