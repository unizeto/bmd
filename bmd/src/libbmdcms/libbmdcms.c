#include <bmd/libbmdcms/libbmdcms.h>
#include <bmd/libbmdpki/libbmdpki.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>
#include <bmd/libbmderr/libbmderr.h>

#ifdef WIN32
#pragma warning(disable:4100)
#endif

/*
 * Obliczaj hash ze struktury Name (czyli np. Issuer)
 * rodzaj hash'a ustawia sie poprzez flags
 */
long Name_count_hash(Name_t *name, char *hash, long flags)
{
	long err = 0;
	GenBuf_t *buf = NULL;

	if (( name == NULL ) || ( hash == NULL ))
		return ERR_WRONG_ARGUMENT;

	err=asn1_encode(&asn_DEF_Name,name, NULL, &buf);

	if ( err != 0 )
		return err;
        
        {
        bmd_crypt_ctx_t *hash_ctx=NULL;
        GenBuf_t *tmp=NULL;
        BMD_FOK(bmd_set_ctx_hash(&hash_ctx,BMD_HASH_ALGO_MD5));
        BMD_FOK(bmd_hash_data(buf,&hash_ctx,&tmp,NULL));
	BMD_FOK(bmd_ctx_destroy(&hash_ctx));
        memmove(hash,tmp->buf,tmp->size);
        free_gen_buf(&tmp);
        free_gen_buf(&buf);
        }
		
	return BMD_OK;
}





