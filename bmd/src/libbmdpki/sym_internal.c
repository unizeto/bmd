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
#include <bmd/common/bmd-openssl.h>

#ifndef TRUE
#define TRUE 1
#endif 

long _bmd_gen_key_iv(GenBuf_t **sym_key,GenBuf_t **iv)
{
long status;
	
	if( sym_key == NULL )
	{
		PRINT_DEBUG("LIBBMDPKIERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if( (*sym_key)!=NULL )
	{
		PRINT_DEBUG("LIBBMDPKIERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	
	if( iv == NULL)
	{
		PRINT_DEBUG("LIBBMDPKIERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}
	if( (*iv)!=NULL )
	{
		PRINT_DEBUG("LIBBMDPKIERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}
	
	(*sym_key)=(GenBuf_t *)malloc(sizeof(GenBuf_t));
	if( (*sym_key)==NULL)
	{
		PRINT_DEBUG("LIBBMDPKIERR Memory error. Error=%i\n",BMD_ERR_MEMORY);
		return BMD_ERR_MEMORY;
	}
	(*sym_key)->buf=(char *)malloc(EVP_MAX_KEY_LENGTH);
	if( (*sym_key)->buf == NULL )
	{
		PRINT_DEBUG("LIBBMDPKIERR Memory error. Error=%i\n",BMD_ERR_MEMORY);
		return BMD_ERR_MEMORY;
	}
	
	(*iv)=(GenBuf_t *)malloc(sizeof(GenBuf_t));
	if( (*iv) == NULL )
	{
		PRINT_DEBUG("LIBBMDPKIERR Memory error. Error=%i\n",BMD_ERR_MEMORY);
		return BMD_ERR_MEMORY;
	}
	(*iv)->buf=(char *)malloc(EVP_MAX_IV_LENGTH);
	if( (*iv)->buf == NULL )
	{
		PRINT_DEBUG("LIBBMDPKIERR Memory error. Error=%i\n",BMD_ERR_MEMORY);
		return BMD_ERR_MEMORY;
	}
	
	status=RAND_bytes((unsigned char*)((*sym_key)->buf), EVP_MAX_KEY_LENGTH);
	if(status!=1)
	{
		PRINT_DEBUG("LIBBMDPKIERR Randomize error. Error=%i\n",BMD_ERR_RAND);
		return BMD_ERR_RAND;
	}
	status=RAND_pseudo_bytes((unsigned char*)((*iv)->buf), EVP_MAX_IV_LENGTH);
	if(status!=1)
	{
		PRINT_DEBUG("LIBBMDPKIERR Randomize error. Error=%i\n",BMD_ERR_RAND);
		return BMD_ERR_RAND;
	}
		
	(*sym_key)->size=EVP_MAX_KEY_LENGTH;
	(*iv)->size=EVP_MAX_IV_LENGTH;
	
	return BMD_OK;
}
