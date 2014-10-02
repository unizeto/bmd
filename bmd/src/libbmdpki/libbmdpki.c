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

#include <bmd/libbmdasn1_common/BMDkeyIV.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>

#ifndef TRUE
#define TRUE 1
#endif

long bmd_generate_number(long byte_size,GenBuf_t **k)
{
long status;

	if(k==NULL)
	{
		PRINT_ERROR("LIBBMDPKIERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if((*k)!=NULL)
	{
		PRINT_ERROR("LIBBMDPKIERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}

	(*k)=(GenBuf_t *)malloc(sizeof(GenBuf_t));
	if((*k)==NULL)
	{
		PRINT_ERROR("LIBBMDPKIERR Memory error. Error=%i\n",BMD_ERR_MEMORY);
		return BMD_ERR_MEMORY;
	}
	(*k)->buf=(char *)malloc(byte_size);
	if((*k)->buf==NULL)
	{
		PRINT_ERROR("LIBBMDPKIERR Memory error. Error=%i\n",BMD_ERR_MEMORY);
		return BMD_ERR_MEMORY;
	}

	(*k)->size=byte_size;
	status=RAND_bytes((unsigned char*)((*k)->buf), byte_size);
	if(status!=1)
	{
		PRINT_ERROR("LIBBMDPKIERR Error. Error=%i\n",BMD_ERR_RAND);
		return BMD_ERR_RAND;
	}

	return 0;
}

long bmd_compute_ki_mod_n(	GenBuf_t *k,
				GenBuf_t *i,
				GenBuf_t *n,
				GenBuf_t **result)
{
BIGNUM *bn_k=NULL;
BIGNUM *bn_i=NULL;
BIGNUM *bn_n=NULL;
BIGNUM *bn_r=NULL;
BN_CTX *ctx=NULL;
long length;
long status;

	if(k==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(k->buf==NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(k->size<=0)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(i==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(i->buf==NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(i->size<0)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(n==NULL)		{	BMD_FOK(BMD_ERR_PARAM3);	}
	if(n->buf==NULL)	{	BMD_FOK(BMD_ERR_PARAM3);	}
	if(n->size<=0)		{	BMD_FOK(BMD_ERR_PARAM3);	}
	if(result==NULL)	{	BMD_FOK(BMD_ERR_PARAM4);	}
	if((*result)!=NULL)	{	BMD_FOK(BMD_ERR_PARAM4);	}

	bn_k=BN_bin2bn((unsigned char*)k->buf,k->size,NULL);
	if(bn_k==NULL)		{	BMD_FOK(BMD_ERR_OP_FAILED);	}

	bn_i=BN_bin2bn((unsigned char*)i->buf,i->size,NULL);
	if(bn_i==NULL)		{	BMD_FOK(BMD_ERR_OP_FAILED);	}

	bn_n=BN_bin2bn((unsigned char*)n->buf,n->size,NULL);
	if(bn_n==NULL)		{	BMD_FOK(BMD_ERR_OP_FAILED);	}

	bn_r=BN_new();
	if(bn_r==NULL)		{	BMD_FOK(BMD_ERR_OP_FAILED);	}

	ctx=BN_CTX_new();
	if(ctx==NULL)		{	BMD_FOK(BMD_ERR_OP_FAILED);	}

	status=BN_mod_exp(bn_r,bn_k,bn_i,bn_n,ctx);
	if(status!=1)		{	BMD_FOK(BMD_ERR_OP_FAILED);	}

	/* wynikowa wartosc wazna jest by byla na BMD_BN_BYTE_SIZE ustawiona - potrzebne w prefixowaniu */
	length=BN_num_bytes(bn_r);
	if(length!=BMD_BN_BYTE_SIZE)
	{
		length=BMD_BN_BYTE_SIZE;
	}

	(*result)=(GenBuf_t *)malloc(sizeof(GenBuf_t));
	if((*result)==NULL)	{	BMD_FOK(NO_MEMORY);	}

	(*result)->buf=(char *)malloc(length+2);
	if((*result)->buf==NULL){	BMD_FOK(NO_MEMORY);	}

	memset((*result)->buf,0,length+1);
	(*result)->size=length;

	status=BN_bn2bin(bn_r, (unsigned char*)(*result)->buf);
	if(status<=0)		{	BMD_FOK(BMD_ERR_OP_FAILED);	}

	BN_clear_free(bn_k);BN_clear_free(bn_i);BN_clear_free(bn_n);BN_clear_free(bn_r);
	BN_CTX_free(ctx);

	return BMD_OK;
}

long bmd_compare_bn(	GenBuf_t *a,
				GenBuf_t *b)
{
long status		= 0;
char *twl_temp_a	= NULL;
char *twl_temp_b	= NULL;
BIGNUM *bn_a		= NULL;
BIGNUM *bn_b		= NULL;

	bn_a=BN_bin2bn((unsigned char*)(a->buf), a->size,NULL);
	if(bn_a==NULL)		{	BMD_FOK(BMD_ERR_OP_FAILED);	}

	bn_b=BN_bin2bn((unsigned char*)(b->buf), b->size,NULL);
	if(bn_b==NULL)		{	BMD_FOK(BMD_ERR_OP_FAILED);	}

	__pki_bmd_genbuf_hex_dump(a, &twl_temp_a, 1, 1);
	__pki_bmd_genbuf_hex_dump(b, &twl_temp_b, 1, 1);

	PRINT_VDEBUG("LIBBMDPKIVDEBUG a: %s b: %s\n", twl_temp_a, twl_temp_b);
	status=BN_cmp(bn_a,bn_b);   /* zwraca 0 jesli sa rowne */
	if(status!=0)		{	BMD_FOK(BMD_ERR_OP_FAILED);	}

	BN_clear_free(bn_a);BN_clear_free(bn_b);

	return status;
}

long bmd_create_der_bmdKeyIV(	long keyEncAlgo,
					long symAlgo,
					bmd_crypt_ctx_t *enc_ctx,
					GenBuf_t *sym_key,
					GenBuf_t *sym_IV,
					GenBuf_t *enc_IV,
					GenBuf_t **output)
{
BMDkeyIV_t *tmp					= NULL;
GenBuf_t *enc_buf					= NULL;
OCTET_STRING_t *tmp_octet			= NULL;
long status						= 0;
long OID_id_DES_EDE3_CBC[] 	= {1,2,840,113549,3,7};
long OID_RSA_encryption[]  	= {1,2,840,113549,1,1,1};

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if(enc_ctx==NULL)				{	BMD_FOK(BMD_ERR_PARAM3);	}
	if(sym_key==NULL)				{	BMD_FOK(BMD_ERR_PARAM4);	}
	if(sym_key->buf==NULL)			{	BMD_FOK(BMD_ERR_PARAM4);	}
	if(sym_key->size<=0)			{	BMD_FOK(BMD_ERR_PARAM4);	}
	if(sym_IV==NULL)				{	BMD_FOK(BMD_ERR_PARAM5);	}
	if(sym_IV->buf==NULL)			{	BMD_FOK(BMD_ERR_PARAM5);	}
	if(sym_IV->size<=0)			{	BMD_FOK(BMD_ERR_PARAM5);	}
	if(output==NULL)				{	BMD_FOK(BMD_ERR_PARAM7);	}
	if((*output)!=NULL)			{	BMD_FOK(BMD_ERR_PARAM7);	}

	tmp=(BMDkeyIV_t *)malloc(sizeof(BMDkeyIV_t));
	memset(tmp,0,sizeof(BMDkeyIV_t));

	if( (keyEncAlgo!=BMD_CRYPT_ALGO_DES3) && (keyEncAlgo!=BMD_CRYPT_ALGO_RSA) )
	{
		BMD_FOK(BMD_ERR_UNIMPLEMENTED);
	}
	else
	{
		if(keyEncAlgo==BMD_CRYPT_ALGO_DES3)
		{
			status=OBJECT_IDENTIFIER_set_arcs(&(tmp->keyEncAlgoOID.algorithm),OID_id_DES_EDE3_CBC,sizeof(OID_id_DES_EDE3_CBC[0]),6);
			if(status!=0)		{	BMD_FOK(BMD_ERR_OP_FAILED);	}

			tmp_octet=(OCTET_STRING_t *)malloc(sizeof(OCTET_STRING_t));
			memset(tmp_octet,0,sizeof(OCTET_STRING_t));
			status=OCTET_STRING_fromBuf(tmp_octet,(char *)enc_IV->buf,enc_IV->size);
			if(status!=0)		{	BMD_FOK(BMD_ERR_OP_FAILED);	}

			tmp->keyEncAlgoOID.parameters=(ANY_t *)malloc(sizeof(ANY_t));
			memset(tmp->keyEncAlgoOID.parameters,0,sizeof(ANY_t));
			status=ANY_fromType(tmp->keyEncAlgoOID.parameters,&asn_DEF_OCTET_STRING,tmp_octet);
			if(status!=0)		{	BMD_FOK(BMD_ERR_OP_FAILED);	}

			asn_DEF_OCTET_STRING.free_struct(&asn_DEF_OCTET_STRING,tmp_octet,0);tmp_octet=NULL;
		}
		else
		{
			if(keyEncAlgo==BMD_CRYPT_ALGO_RSA)
			{
				OBJECT_IDENTIFIER_set_arcs(&(tmp->keyEncAlgoOID.algorithm),OID_RSA_encryption,sizeof(OID_RSA_encryption[0]),7);
			}
		}
	}
	/* szyfrujemy kluczem publicznym stad operacja : file_encrypt_raw */
	if(keyEncAlgo==BMD_CRYPT_ALGO_RSA)
	{
		file_encrypt_raw(enc_ctx->file,sym_key,&enc_buf);
	}
	else
	{
		bmd_symmetric_encrypt(sym_key,&enc_ctx,&enc_buf,NULL);
	}
	OCTET_STRING_fromBuf(&(tmp->encryptedKey),(char *)enc_buf->buf,enc_buf->size);
	free_gen_buf(&enc_buf);

	if(symAlgo!=BMD_CRYPT_ALGO_DES3)	{	BMD_FOK(BMD_ERR_UNIMPLEMENTED);	}
	else
	{
		OBJECT_IDENTIFIER_set_arcs(&(tmp->symAlgoOID.algorithm),OID_id_DES_EDE3_CBC,sizeof(OID_id_DES_EDE3_CBC[0]),6);
		tmp_octet=(OCTET_STRING_t *)malloc(sizeof(OCTET_STRING_t));
		memset(tmp_octet,0,sizeof(OCTET_STRING_t));
		status=OCTET_STRING_fromBuf(tmp_octet,(char *)sym_IV->buf,sym_IV->size);
		if(status!=0)			{	BMD_FOK(BMD_ERR_OP_FAILED);	}

		tmp->symAlgoOID.parameters=(ANY_t *)malloc(sizeof(ANY_t));
		memset(tmp->symAlgoOID.parameters,0,sizeof(ANY_t));
		status=ANY_fromType(tmp->symAlgoOID.parameters,&asn_DEF_OCTET_STRING,tmp_octet);
		if(status!=0)			{	BMD_FOK(BMD_ERR_OP_FAILED);	}

		asn_DEF_OCTET_STRING.free_struct(&asn_DEF_OCTET_STRING,tmp_octet,0);tmp_octet=NULL;
	}

	BMD_FOK(asn1_encode(&asn_DEF_BMDkeyIV,tmp, NULL, output));
	asn_DEF_BMDkeyIV.free_struct(&asn_DEF_BMDkeyIV,tmp,0);tmp=NULL;

	return BMD_OK;
}

long bmd_decode_bmdKeyIV(	GenBuf_t *input,
				GenBuf_t *dec_sym_key,
				bmd_crypt_ctx_t *dec_ctx,
				bmd_crypt_ctx_t **sym_ctx)
{
BMDkeyIV_t *tmp					= NULL;
bmd_crypt_ctx_t *tmp_sym_ctx			= NULL;
GenBuf_t *klucz_zaszyfrowany			= NULL;
GenBuf_t *klucz					= NULL;
OCTET_STRING_t *IV_from_der			= NULL;
GenBuf_t *IV					= NULL;
long count						= 0;
long OID_id_DES_EDE3_CBC[] 	= {1,2,840,113549,3,7};
long OID_RSA_encryption[]  	= {1,2,840,113549,1,1,1};
long encryption_oid[10];
long sym_oid[10];
asn_dec_rval_t rc_code;

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if (input==NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (dec_ctx==NULL)	{	BMD_FOK(BMD_ERR_PARAM3);	}
	if (sym_ctx==NULL)	{	BMD_FOK(BMD_ERR_PARAM4);	}
	if (*sym_ctx != NULL)	{	BMD_FOK(BMD_ERR_PARAM4);	}

	memset(encryption_oid,0,sizeof(encryption_oid));

	rc_code=ber_decode(NULL,&asn_DEF_BMDkeyIV,(void **)&tmp,input->buf,input->size);
	if(rc_code.code!=RC_OK)				{	BMD_FOK(BMD_ERR_FORMAT);	}

	/* pobierz identyfikator algorytmu ktory szyfrowal klucz i nastepnie zdeszyfruj ten klucz */
	count=OBJECT_IDENTIFIER_get_arcs(&(tmp->keyEncAlgoOID.algorithm),encryption_oid,sizeof(encryption_oid[0]),10);
	if(count==-1)					{	BMD_FOK(BMD_ERR_OP_FAILED);	}

	if(memcmp(OID_RSA_encryption,encryption_oid,sizeof(OID_RSA_encryption))==0) /* RSA */
	{

		set_gen_buf2((char*)(tmp->encryptedKey.buf),tmp->encryptedKey.size,&klucz_zaszyfrowany);
		if(dec_ctx->file)
		{
			BMD_FOK(file_decrypt_raw(dec_ctx->file,klucz_zaszyfrowany,&klucz));
		}
		if(dec_ctx->pkcs11)
		{
			BMD_FOK(bmd_pkcs11_decrypt(dec_ctx->pkcs11,klucz_zaszyfrowany,&klucz));
		}
		if(dec_ctx->ks)
		{
			long status;
			PRINT_VDEBUG("LIBBMDVDEBUG Key service context.\n");
			status=ks_decrypt_raw(dec_ctx->ks, klucz_zaszyfrowany, &klucz);
                        if(status!=BMD_OK)
                        {
                                PRINT_ERROR("LIBBMDPKIERR Error. Error=%i\n",BMD_ERR_OP_FAILED);
                                return BMD_ERR_OP_FAILED;
                        }


		}

		free_gen_buf(&klucz_zaszyfrowany);
	}
	else
	{
		if(memcmp(OID_id_DES_EDE3_CBC,encryption_oid,sizeof(OID_id_DES_EDE3_CBC))==0)
		{
			set_gen_buf2((char*)(tmp->encryptedKey.buf),tmp->encryptedKey.size,&klucz_zaszyfrowany);

			rc_code=ber_decode(NULL,&asn_DEF_OCTET_STRING,(void **)&IV_from_der,tmp->keyEncAlgoOID.parameters->buf,tmp->keyEncAlgoOID.parameters->size);
			if(rc_code.code!=RC_OK)		{	BMD_FOK(BMD_ERR_OP_FAILED);	}

			set_gen_buf2((char*)(IV_from_der->buf),IV_from_der->size,&IV);

			BMD_FOK(bmd_set_ctx_sym(&tmp_sym_ctx,BMD_CRYPT_ALGO_DES3,dec_sym_key,IV));
			BMD_FOK(bmd_symmetric_decrypt(klucz_zaszyfrowany, 0, &tmp_sym_ctx,&klucz,NULL));

			bmd_ctx_destroy(&tmp_sym_ctx);
			free_gen_buf(&klucz_zaszyfrowany);
			asn_DEF_OCTET_STRING.free_struct(&asn_DEF_OCTET_STRING,IV_from_der,0);IV_from_der=NULL;
			free_gen_buf(&IV);
		}
		else
		{
			BMD_FOK(BMD_ERR_UNIMPLEMENTED);
		}
	}


	/* pobierz wartosc wektora IV, uprzednio pobierajac OID */
	memset(sym_oid,0,sizeof(sym_oid));

	count=OBJECT_IDENTIFIER_get_arcs(&(tmp->symAlgoOID.algorithm),sym_oid,sizeof(sym_oid[0]),10);
	if(count==-1)					{	BMD_FOK(BMD_ERR_OP_FAILED);	}

	if(memcmp(OID_id_DES_EDE3_CBC,sym_oid,sizeof(OID_id_DES_EDE3_CBC))==0)
	{
		ber_decode(NULL,&asn_DEF_OCTET_STRING,(void **)&IV_from_der,tmp->symAlgoOID.parameters->buf,tmp->symAlgoOID.parameters->size);
		set_gen_buf2((char*)(IV_from_der->buf),IV_from_der->size,&IV);

		BMD_FOK(bmd_set_ctx_sym(sym_ctx,BMD_CRYPT_ALGO_DES3,klucz,IV));

		asn_DEF_OCTET_STRING.free_struct(&asn_DEF_OCTET_STRING,IV_from_der,0);IV_from_der=NULL;
		free_gen_buf(&IV);
	}
	else
	{
		BMD_FOK(BMD_ERR_UNIMPLEMENTED);
	}

	asn_DEF_BMDkeyIV.free_struct(&asn_DEF_BMDkeyIV,tmp,0);tmp=NULL;
	free_gen_buf(&klucz);klucz=NULL;

	return BMD_OK;
}

