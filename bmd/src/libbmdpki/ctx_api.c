#include <bmd/common/bmd-common.h>

#include <bmd/libbmdpki/libbmdpki.h>
#include <bmd/libbmdpkcs12/libbmdpkcs12.h>
#include <bmd/libbmdutils/libbmdutils.h>
#include <bmd/libbmderr/libbmderr.h>

#include <bmd/libbmdts/libbmdts.h>
#include <bmd/libbmdpkcs11/libbmdpkcs11.h>
#include <bmd/common/bmd-crypto_ds.h>

#include <bmd/common/bmd-const.h>

#include <bmd/libbmdcms/libbmdcms.h>
#include <bmd/libbmdbase64/libbmdbase64.h>

#ifndef TRUE
#define TRUE 1
#endif


long bmd_create_ctx(	bmd_crypt_ctx_t **ctx,
				long ctx_source,
				long ctx_type)
{
	PRINT_INFO("LIBBMDPKIINF Creating ctx \n");

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if( ctx == NULL )				{	BMD_FOK(BMD_ERR_PARAM1);	}
	if( ( *ctx ) != NULL )			{	BMD_FOK(BMD_ERR_PARAM1);	}

	BMD_FOK(__bmd_check_ctx_source_and_type(ctx_source,ctx_type));

	(*ctx)=(bmd_crypt_ctx_t *)malloc(sizeof(bmd_crypt_ctx_t));
	(*ctx)->ctx_source=ctx_source;
	(*ctx)->ctx_type=ctx_type;

	switch((*ctx)->ctx_source)
	{
		case BMD_CTX_SOURCE_FILE:
			(*ctx)->pkcs11=NULL;
			(*ctx)->sym=NULL;
			(*ctx)->hash=NULL;
                        (*ctx)->file=NULL;
			(*ctx)->ks=NULL;
			BMD_FOK(_bmd_file_crypto_info_init( & ( (*ctx)->file ) ) );
			if( (*ctx)->ctx_type == BMD_CTX_TYPE_SIG )
			{
				(*ctx)->file->hash_alg=-1;
			}
                        break;
		case BMD_CTX_SOURCE_PKCS11:
			(*ctx)->file=NULL;
			(*ctx)->sym=NULL;
			(*ctx)->hash=NULL;
                        (*ctx)->pkcs11=NULL;
			(*ctx)->ks=NULL;
			BMD_FOK(_bmd_pkcs11_crypto_info_init( & ( (*ctx)->pkcs11 ) ) );
			if( ( (*ctx)->ctx_type != BMD_CTX_TYPE_SIG ) && ( (*ctx)->ctx_type != BMD_CTX_TYPE_ASYM ) )
			{
				(*ctx)->pkcs11->hash_alg=-1;
			}
            break;
		case BMD_CTX_SOURCE_KS:
			(*ctx)->pkcs11=NULL;
			(*ctx)->sym=NULL;
			(*ctx)->hash=NULL;
            (*ctx)->file=NULL;
			(*ctx)->ks=NULL;
			BMD_FOK(_bmd_ks_crypto_info_init(&((*ctx)->ks)));
			break;
		case BMD_CTX_SOURCE_WIN:
			BMD_FOK(BMD_ERR_UNIMPLEMENTED);
	}

	if( ( (*ctx)->ctx_source==BMD_CTX_SOURCE_NONE) && ( (*ctx)->ctx_type==BMD_CTX_TYPE_SYM ) )
	{
		(*ctx)->file=NULL;
		(*ctx)->pkcs11=NULL;
		(*ctx)->hash=NULL;
                (*ctx)->sym=NULL;
		(*ctx)->ks=NULL;
		BMD_FOK(_bmd_sym_crypto_info_init( & ( (*ctx)->sym ) ) );
	}

	if( ( (*ctx)->ctx_source==BMD_CTX_SOURCE_NONE) && ( (*ctx)->ctx_type==BMD_CTX_TYPE_HASH ) )
	{
		(*ctx)->file=NULL;
		(*ctx)->pkcs11=NULL;
		(*ctx)->hash=NULL;
		(*ctx)->sym=NULL;
		(*ctx)->ks=NULL;
		BMD_FOK(_bmd_hash_crypto_info_init( & ( (*ctx)->hash ) ) );
	}

	return BMD_OK;
}

long bmd_ctx_destroy(bmd_crypt_ctx_t **ctx)
{
long i=0;

	PRINT_INFO("LIBBMDPKIINF Destroying ctx\n");

	if(ctx==NULL)		{	return BMD_ERR_PARAM1;	}
	if((*ctx)==NULL)		{	return BMD_ERR_PARAM1;	}

	if((*ctx)->pkcs11)
	{
		bmd_pkcs11_finalize((*ctx)->pkcs11);
		bmd_pkcs11_clear_pin( (*ctx)->pkcs11 );
		if((*ctx)->pkcs11->all_certs)
		{
			while((*ctx)->pkcs11->all_certs[i])
			{
				free_gen_buf( &( (*ctx)->pkcs11->all_certs[i]->cka_id ) );
				free_gen_buf( &( (*ctx)->pkcs11->all_certs[i]->gb_cert ) );
				free((*ctx)->pkcs11->all_certs[i]);(*ctx)->pkcs11->all_certs[i]=NULL;
				i++;
			}
			free((*ctx)->pkcs11->all_certs);(*ctx)->pkcs11->all_certs=NULL;
			free_gen_buf(&((*ctx)->pkcs11->cert));

		}
		free((*ctx)->pkcs11);(*ctx)->pkcs11=NULL;
	}

	if((*ctx)->file)
	{
		free_gen_buf(&( (*ctx)->file->cert) );
		RSA_free((*ctx)->file->privateKey);
		RSA_free((*ctx)->file->publicKey);
		free((*ctx)->file);(*ctx)->file=NULL;
	}
	if((*ctx)->sym)
	{
		free_gen_buf(&((*ctx)->sym->key));free_gen_buf(&((*ctx)->sym->IV));
		free((*ctx)->sym);(*ctx)->sym=NULL;
	}
	if((*ctx)->hash)
	{
		/* niszczenie kontekstu EVP_MD_CTX odbywa sie w funkcji bmd_hash_data*/
		free((*ctx)->hash);(*ctx)->hash=NULL;
	}
	if((*ctx)->ks)
	{
		free((*ctx)->ks->ks_adr);
		free_gen_buf(&((*ctx)->ks->output));
		free_gen_buf(&((*ctx)->ks->sig_cert));
		free_gen_buf(&((*ctx)->ks->ed_cert));
	}

	free(*ctx);*ctx=NULL;
	return BMD_OK;
}

/** @todo dorobic wczytywanie plikow PEM oraz mozliwosc ladowania oddzielnie,
					tylko klucza prywatnego lub certyfikatu i klucza publicznego */
long bmd_set_ctx_file(char *filename,char *pass,long passlen,bmd_crypt_ctx_t **ctx)
{
	PRINT_INFO("LIBBMDPKIINF Setting ctx file\n");

	/************************/
	/* walidacja parametrow */
	/************************/
	if( filename == NULL )					{	BMD_FOK(BMD_ERR_PARAM1);	}
	if( ( passlen != 0 ) && ( pass == NULL ) )	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if( (passlen == 0 ) && ( pass != NULL ) )		{	BMD_FOK(BMD_ERR_PARAM3);	}
	if( ctx == NULL )						{	BMD_FOK(BMD_ERR_PARAM4);	}
	if( (*ctx) != NULL )					{	BMD_FOK(BMD_ERR_PARAM4);	}

	BMD_FOK(bmd_create_ctx(ctx,BMD_CTX_SOURCE_FILE,BMD_CTX_TYPE_ASYM));
	BMD_FOK(bmd_parse_p12_file(filename,pass,passlen,&((*ctx)->file->publicKey),&((*ctx)->file->privateKey),&((*ctx)->file->cert)));

	return BMD_OK;
}


long bmd_set_ctx_fileInMem(	GenBuf_t *file_buf,
					char *pass,
					long passlen,
					bmd_crypt_ctx_t **ctx)
{
	PRINT_INFO("LIBBMDPKIINF Setting ctx file from mem\n");

	if( file_buf == NULL )			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if( file_buf->buf == NULL )		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if( ( passlen != 0 ) && ( pass == NULL ) )	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if( (passlen == 0 ) && ( pass != NULL ) )		{	BMD_FOK(BMD_ERR_PARAM3);	}
	if( ctx == NULL )				{	BMD_FOK(BMD_ERR_PARAM4);	}
	if( (*ctx) != NULL )			{	BMD_FOK(BMD_ERR_PARAM4);	}

	BMD_FOK(bmd_create_ctx(ctx,BMD_CTX_SOURCE_FILE,BMD_CTX_TYPE_ASYM));
	BMD_FOK(bmd_parse_p12_fileInMem(file_buf,pass,passlen,&((*ctx)->file->publicKey),&((*ctx)->file->privateKey),&((*ctx)->file->cert)));

	return BMD_OK;
}


/* ustawia kontekst z PKCS#11 - laduje biblioteke, inicjalizuje,
   pobiera wszystkie certyfikaty z kart jesli opcja auto_load jest ustawiona (BMD_PKCS11_AUTO_LOAD_CERT)
*/
long bmd_set_ctx_pkcs11	(bmd_crypt_ctx_t **ctx,
				char *pkcs11_library,
				long ctx_type,
				long auto_load,
				BmdCtxPkcs11Option_e ctx_option)
{
	PRINT_INFO("LIBBMDPKIINF Setting ctx pkcs11\n");

	if(ctx==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if((*ctx)!=NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(pkcs11_library==NULL){	BMD_FOK(BMD_ERR_PARAM2);	}
	if( (auto_load!=BMD_PKCS11_AUTO_LOAD_CERT) && (auto_load!=0) )
	{
		BMD_FOK(BMD_ERR_PARAM4);
	}

	BMD_FOK(bmd_create_ctx(ctx,BMD_CTX_SOURCE_PKCS11,ctx_type));
	BMD_FOK(bmd_pkcs11_init(pkcs11_library,(*ctx)->pkcs11));

	if(auto_load==BMD_PKCS11_AUTO_LOAD_CERT)
	{
		BMD_FOK(bmd_pkcs11_get_all_certs((*ctx)->pkcs11,&((*ctx)->pkcs11->all_certs)));
	}

	// sprawdzenie poprawnosci podanej opji
	// BMD_CTX_PKCS11_OPTION_DEFAULT i BMD_CTX_PKCS11_OPTION_DEDBS wzajemnie sie wykluczaja
	if(ctx_option != BMD_CTX_PKCS11_OPTION_DEFAULT && ctx_option != BMD_CTX_PKCS11_OPTION_DEDBS)
	{
		BMD_FOK(BMD_ERR_PARAM5);
	}
	
	if(ctx_option & BMD_CTX_PKCS11_OPTION_DEFAULT)
	{
		(*ctx)->pkcs11->dontEstimateDecryptBufferSize = 0;
	}
	if(ctx_option & BMD_CTX_PKCS11_OPTION_DEDBS)
	{
		(*ctx)->pkcs11->dontEstimateDecryptBufferSize = 1;
	}
	

	return BMD_OK;
}


long bmd_set_ctx_pkcs11_privkey(	bmd_crypt_ctx_t **ctx,
						pkcs11_cert_t *sel_cert,
						char *pin,
						long pin_length)
{
	char *pinTemp=NULL;
	PRINT_INFO("LIBBMDPKIINF Setting ctx pkcs11 private key\n");

	if(ctx==NULL)				{	BMD_FOK(BMD_ERR_PARAM1);	}
	if((*ctx)==NULL)				{	BMD_FOK(BMD_ERR_PARAM1);	}
	if((*ctx)->pkcs11==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if((*ctx)->pkcs11->pLibrary==NULL)	{	BMD_FOK(BMD_ERR_NOTINITED);	}
	if(sel_cert==NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(sel_cert->cka_id==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(sel_cert->gb_cert==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(pin==NULL)				{	BMD_FOK(BMD_ERR_PARAM3);	}

	/*
	bmd_pkcs11_get_privkey() czysci i zwalnia pin, dlatego stworze kopie, ktora zostanie zniszczona a 
	oryginal pinu nie bedzie tkniety (by WSZ)
	*/
	pinTemp=(char*)calloc(pin_length, sizeof(char*));
	if(pinTemp == NULL)
	{
		return BMD_ERR_MEMORY;
	}
	memcpy(pinTemp, pin, pin_length);
	BMD_FOK(bmd_pkcs11_get_privkey(sel_cert,&pinTemp,pin_length,(*ctx)->pkcs11));

	return BMD_OK;
}

long bmd_set_ctx_sym(	bmd_crypt_ctx_t **ctx,
				long sym_algo,
				GenBuf_t *key,
				GenBuf_t *IV)
{
	PRINT_INFO("LIBBMDPKIINF Setting ctx symmetric key\n");

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if(ctx==NULL)						{	BMD_FOK(BMD_ERR_PARAM1);	}
	if((*ctx)!=NULL)						{	BMD_FOK(BMD_ERR_PARAM1);	}
	if( 	(sym_algo!=BMD_CRYPT_ALGO_DES3) &&
		(sym_algo!=BMD_CRYPT_ALGO_AES) &&
		(sym_algo!=BMD_CRYPT_ALGO_SYM_DEFAULT) )	{	BMD_FOK(BMD_ERR_UNIMPLEMENTED);	}

	BMD_FOK(bmd_create_ctx(ctx,BMD_CTX_SOURCE_NONE,BMD_CTX_TYPE_SYM));

	if((key==NULL) || (IV==NULL))
	{
		BMD_FOK(_bmd_gen_key_iv(&((*ctx)->sym->key),&((*ctx)->sym->IV)));	/* generuj klucz oraz IV */
	}
	else
	{
		BMD_FOK(set_gen_buf2(key->buf,key->size,&((*ctx)->sym->key)));
		BMD_FOK(set_gen_buf2(IV->buf,IV->size,&((*ctx)->sym->IV)));
	}

	(*ctx)->sym->algo_type=sym_algo;

	return BMD_OK;
}

long bmd_set_ctx_hash(	bmd_crypt_ctx_t **ctx,
				long hash_algo)
{
	PRINT_INFO("LIBBMDPKIINF Setting ctx hash\n");
	if(ctx==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if((*ctx)!=NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if( (hash_algo!=BMD_HASH_ALGO_SHA1) && (hash_algo!=BMD_HASH_ALGO_MD5) && (hash_algo!=BMD_HASH_ALGO_DEFAULT) )
	{
		BMD_FOK(BMD_ERR_UNIMPLEMENTED);
	}

	BMD_FOK(bmd_create_ctx(ctx,BMD_CTX_SOURCE_NONE,BMD_CTX_TYPE_HASH));
	(*ctx)->hash->hash_alg=hash_algo;

	return BMD_OK;
}

/** @note - szyfrowanie kluczem publicznym i tak mozna na robic poza karta/urzadzeniem stad zrodlo tutaj BMD_CTX_SOURCE_FILE */
long bmd_set_ctx_cert(	bmd_crypt_ctx_t **ctx,
				GenBuf_t *cert)
{
	PRINT_INFO("LIBBMDPKIINF Setting ctx certificate\n");
	if(ctx==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if((*ctx)!=NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(cert==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	BMD_FOK(bmd_create_ctx(ctx,BMD_CTX_SOURCE_FILE,BMD_CTX_TYPE_ASYM));
	BMD_FOK(set_gen_buf2(cert->buf,cert->size,&((*ctx)->file->cert)));
	BMD_FOK_CHG(__bmd_cert_to_pubkey(cert,&((*ctx)->file->publicKey)),
			LIBBMDPKI_CTX_API_PUBLIC_KEY_FROM_CERT_FILE_ERR);

	return BMD_OK;
}

/* 0 - podpis, 1 - deszyfrowanie, 2 - oba */
long bmd_set_ctx_ks(bmd_crypt_ctx_t **ctx,const char *adr,const long port,long ks_type)
{
	long status;

	if( ctx == NULL )
		return BMD_ERR_PARAM1;
	if( (*ctx) != NULL )
		return BMD_ERR_PARAM1;
	if( adr == NULL )
		return BMD_ERR_PARAM2;
	if( ( port == 0 ) || ( port > 65536 ) )
		return BMD_ERR_PARAM3;
	if( ( ks_type != 0 ) && ( ks_type != 1 ) && ( ks_type != 2 ) )
		return BMD_ERR_PARAM4;

	status=bmd_create_ctx(ctx,BMD_CTX_SOURCE_KS,BMD_CTX_TYPE_ASYM);
	if( status != BMD_OK )
		return status;

	asprintf(&((*ctx)->ks->ks_adr),"%s",adr);
	(*ctx)->ks->ks_port=port;
	if( ks_type == 2 )
	{
		(*ctx)->ks->oper_type=2;
		(*ctx)->ks->sign=ks_sign_raw;
		(*ctx)->ks->decrypt=ks_decrypt_raw;
		(*ctx)->ks->encrypt=ks_encrypt_raw;
		(*ctx)->ks->s_prepare=ks_s_prepare_ctx;
		(*ctx)->ks->ed_prepare=ks_ed_prepare_ctx;
		status=(*ctx)->ks->s_prepare((*ctx)->ks);
		if( status != BMD_OK )
		{
			PRINT_DEBUG("LIBBMDPKIERR Error == %li while fetching sig cert from keyservice\n",status);
			return status;
		}
		status=(*ctx)->ks->ed_prepare((*ctx)->ks);
		if( status != BMD_OK )
		{
			PRINT_DEBUG("LIBBMDPKIERR Error == %li while fetching enc/dec cert from keyservice\n",status);
			return status;
		}
	}
	else
	{
		if( ks_type == 0 )
		{
			(*ctx)->ks->oper_type=0;
			(*ctx)->ks->sign=ks_sign_raw;
			(*ctx)->ks->s_prepare=ks_s_prepare_ctx;
			status=(*ctx)->ks->s_prepare((*ctx)->ks);
			if( status != BMD_OK )
			{
				PRINT_DEBUG("LIBBMDPKIERR Error == %li while fetching sig cert from keyservice\n",status);
				return status;
			}
		}
		else
		{
			(*ctx)->ks->oper_type=1;
			(*ctx)->ks->decrypt=ks_decrypt_raw;
			(*ctx)->ks->encrypt=ks_encrypt_raw;
			(*ctx)->ks->ed_prepare=ks_ed_prepare_ctx;
			status=(*ctx)->ks->ed_prepare((*ctx)->ks);
			if( status != BMD_OK )
			{
				PRINT_DEBUG("LIBBMDPKIERR Error == %li while fetching enc/dec cert from keyservice\n", \
				status);
				return status;
			}
		}
	}
	return BMD_OK;
}

/** @todo dorobic bardziej szczegolowa kontrole bledow zwlaszcza w zakresie BMD_CTX_SOURCE_NONE */
long bmd_check_ctx(bmd_crypt_ctx_t *ctx,long ctx_source,long ctx_type)
{
	PRINT_INFO("LIBBMDPKIINF Checking ctx\n");
	if(ctx==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}

	BMD_FOK(__bmd_check_ctx_source_and_type(ctx_source,ctx_type));

	switch(ctx_source)
	{
		case BMD_CTX_SOURCE_FILE:
			if( ctx->file == NULL )			{	BMD_FOK(BMD_ERR_PARAM1);	}
			if( ctx->file->cert==NULL )		{	BMD_FOK(BMD_ERR_PARAM1);	}
			if( ctx->file->cert->buf == NULL )	{	BMD_FOK(BMD_ERR_PARAM1);	}
			if( ctx->file->cert->size <= 0 )	{	BMD_FOK(BMD_ERR_PARAM1);	}
			if( ctx->file->publicKey == NULL)
			{
				/* jesli szyfrujemy lub mamy ogolny kontekst to jest to blad */
				if( (ctx_type == BMD_CTX_TYPE_ASYM_E ) || (ctx_type == BMD_CTX_TYPE_ASYM) )
				{
					BMD_FOK(BMD_ERR_PARAM1);
				}
			}
			if( ctx->file->privateKey == NULL )
			{
				/* jesli deszyfrujemy, ogolny kontekst lub podpis to jest to blad */
				if( (ctx_type == BMD_CTX_TYPE_ASYM_D ) || (ctx_type == BMD_CTX_TYPE_ASYM) || (ctx_type == BMD_CTX_TYPE_SIG) )
				{
					BMD_FOK(BMD_ERR_PARAM1);
				}
			}
			if( ctx_type == BMD_CTX_TYPE_SIG )
			{
				if(ctx->file->hash_alg==-1)		{	BMD_FOK(BMD_ERR_PARAM1);	}
			}
                        break;
		case BMD_CTX_SOURCE_PKCS11:
			if( ctx->pkcs11 == NULL )			{	BMD_FOK(BMD_ERR_PARAM1);	}
			if( ctx->pkcs11->pFunctionList == NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
			if( ctx->pkcs11->hPrivateKey <= 0 )		{	BMD_FOK(BMD_ERR_PARAM1);	}
			if( ctx->pkcs11->hSession <= 0 )		{	BMD_FOK(BMD_ERR_PARAM1);	}
			if( ctx->pkcs11->pLibrary <= 0 )		{	BMD_FOK(BMD_ERR_PARAM1);	}
			if( ctx_type == BMD_CTX_TYPE_SIG )
			{
				if(ctx->pkcs11->hash_alg==-1)		{	BMD_FOK(BMD_ERR_PARAM1);	}
			}
			if( ctx->pkcs11->cert == NULL )		{	BMD_FOK(BMD_ERR_PARAM1);	}
			if( ctx->pkcs11->cert->buf == NULL )	{	BMD_FOK(BMD_ERR_PARAM1);	}
			if( ctx->pkcs11->cert->size <= 0 )		{	BMD_FOK(BMD_ERR_PARAM1);	}
			break;
		case BMD_CTX_SOURCE_WIN:
			PRINT_ERROR("LIBBMDPKIERR Algorithm unimplemented. Error=%i\n",BMD_ERR_UNIMPLEMENTED);
			return BMD_ERR_UNIMPLEMENTED;
                        break;
		case BMD_CTX_SOURCE_NONE:
			if( ctx_type == BMD_CTX_TYPE_HASH )
			{
				if(ctx->hash == NULL )			{	BMD_FOK(BMD_ERR_PARAM1);	}

				if( (ctx->hash->hash_alg!=BMD_HASH_ALGO_SHA1) && (ctx->hash->hash_alg!=BMD_HASH_ALGO_MD5) )
				{
					BMD_FOK(BMD_ERR_UNIMPLEMENTED);
				}

				/* sprawdzenie czy kontekst nie jest w fazie koncowej - liczenie skrotu sie zakonczylo */
				/* get_lob_hash i nastepnie get_lob_timestamp nie moga dzialac gdy to jest aktywne */
				/*if( (ctx->hash->hash_stage==BMD_HASH_STAGE_COMPLETE) )
					return BMD_ERR_COMPLETE;*/
			}

			if( ctx_type == BMD_CTX_TYPE_SYM )
			{
				if(ctx->sym == NULL )		{	BMD_FOK(BMD_ERR_PARAM1);	}
				if(ctx->sym->key == NULL )	{	BMD_FOK(BMD_ERR_PARAM1);	}
				if(ctx->sym->IV == NULL )	{	BMD_FOK(BMD_ERR_PARAM1);	}

				if(ctx->sym->algo_type!=BMD_CRYPT_ALGO_DES3)
				{
					BMD_FOK(BMD_ERR_UNIMPLEMENTED);
				}

				/* sprawdzenie czy kontekst nie jest w fazie koncowej - liczenie skrotu sie zakonczylo */
				/*if( (ctx->sym->sym_stage==BMD_SYM_STAGE_COMPLETE) )
					return BMD_ERR_COMPLETE;
				*/
			}
                        break;
	};

	return BMD_OK;
}

/* generowanie kontekstu w oparciu o haslo */
long bmd_gen_sym_ctx_with_pass(long crypt_algo,long hash_algo,char *pass,long pass_len,
							   bmd_crypt_ctx_t **ctx)
{
long status;
char tmp_key[EVP_MAX_KEY_LENGTH];
char tmp_IV[EVP_MAX_IV_LENGTH];

	PRINT_INFO("LIBBMDPKIINF Generating symmetric ctx with pass\n");
	if(crypt_algo!=BMD_CRYPT_ALGO_DES3)		{	BMD_FOK(BMD_ERR_UNIMPLEMENTED);	}
	if(hash_algo!=BMD_HASH_ALGO_SHA1)		{	BMD_FOK(BMD_ERR_UNIMPLEMENTED);	}
	if(pass==NULL)					{	BMD_FOK(BMD_ERR_PARAM3);	}
	if(ctx==NULL)					{	BMD_FOK(BMD_ERR_PARAM5);	}
	if((*ctx)!=NULL)					{	BMD_FOK(BMD_ERR_PARAM5);	}

	status=EVP_BytesToKey(EVP_des_ede3_cbc(),EVP_sha1(),NULL, (unsigned char*)pass,pass_len,500,(unsigned char*)tmp_key, (unsigned char*)tmp_IV);
	if( status < 0 )
	{
		PRINT_ERROR("LIBBMDPKIERR EVP_BytesToKey %li\n",status);
		return BMD_ERR_OP_FAILED;
	}
	BMD_FOK(bmd_create_ctx(ctx,BMD_CTX_SOURCE_NONE,BMD_CTX_TYPE_SYM));
	set_gen_buf2(tmp_key,status,&((*ctx)->sym->key));
	set_gen_buf2(tmp_IV,EVP_MAX_IV_LENGTH,&((*ctx)->sym->IV));
	memset(tmp_key,0,EVP_MAX_KEY_LENGTH);
	memset(tmp_IV,0,EVP_MAX_IV_LENGTH);

	return BMD_OK;
}

long bmd_ctx_reinit(bmd_crypt_ctx_t **ctx)
{
	PRINT_INFO("LIBBMDPKIINF Reinitializing ctx\n");
	if(ctx==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if((*ctx)->sym==NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}

	memset((*ctx)->sym->last_block,0,EVP_MAX_KEY_LENGTH);
	(*ctx)->sym->last_block_length=0;
	(*ctx)->sym->sym_stage=BMD_SYM_STAGE_FIRST;
	(*ctx)->sym->padding_present=0;
	return BMD_OK;
}


/**
funkcja bmd_set_ctx_from_base64content() na podstawie bufora z zakodowana w base64 zawartoscia pliku .pfx (.p12)
ustawia context zgodny z biblioteka libbmdpki (kontekst jest dynamicznie alokowany)
Funkcja wymaga, aby wczesniej wywolane bylo bmd_init()

@param encoded_content to bufor z zakodowana w base64 zawartoscia pliku .pfx (.p12)
@param pass to haslo do pfx
@param passlen to dlugosc hasla do pfx
@param set_ctx to ustawiany w funkcji kontekst zgodny z biblioteka libbmdpki

@retval 0 jesli operacja sie powiodla
@retval -1 jesli za encoded_content przekazano NULL
@retval -2 jesli bufor encoded_content jest pusty
@retval -3 jesli za pass przekazano NULL
@retval -4 jesli podano ujemna dlugosc hasla
@retval -5 jeśli za set_ctx przekazano NULL
@retval -6 jesli wartosc wyluskana z set_ctx nie jest wyNULLowana
@retval -7 jesli nie mozna zdekodowac encoded_content (spowodowane rowniez brakiem pamieci)
@retval -8 jesli nie mozna ustawic kontekstu


*/
long bmd_set_ctx_from_base64content(char *encoded_content, char *pass, long passlen, bmd_crypt_ctx_t **set_ctx)
{
	GenBuf_t decoded64;
	int err				= 0;
	bmd_crypt_ctx_t *new_ctx	= NULL;
	size_t tmp_size			= 0;

	if(encoded_content == NULL)
		{ return -1; }

	if( (strlen(encoded_content)) <= 0 )
		{ return -2; }

	if(pass == NULL)
		{ return -3; }
	if(passlen < 0)
		{ return -4; }

	if(set_ctx == NULL)
		{ return -5; }
	if((*set_ctx) != NULL)
		{ return -6; }

	decoded64.buf = (char*)spc_base64_decode((unsigned char *)encoded_content, &tmp_size, 0, &err);

	if(decoded64.buf == NULL)	{ return -7; }
	decoded64.size=(int)tmp_size;

	if( (bmd_set_ctx_fileInMem(&decoded64, pass, passlen, &new_ctx)) != 0 )
	{
		free(decoded64.buf);
		return -8;
	}
	free(decoded64.buf);

	*set_ctx=new_ctx;
	return 0;
}


