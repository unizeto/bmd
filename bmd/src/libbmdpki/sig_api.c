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

#ifdef WIN32
#include <sys/locking.h>
#pragma warning(disable:4055)
#endif



#ifndef TRUE
#define TRUE 1
#endif

long bmd_sign_data(GenBuf_t *input,bmd_crypt_ctx_t *ctx,GenBuf_t **output,void *opt)
{
SignedData_t *sigData=NULL;
SignerInfo_t *signerInfo=NULL;
ContentInfo_t *content=NULL;
bmd_signature_params_t *options=NULL;
long opt_local=0;

	if(input==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(input->buf==NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(input->size<=0)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(ctx==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	if(ctx->file)
	{
		BMD_FOK(bmd_check_ctx(ctx,BMD_CTX_SOURCE_FILE,BMD_CTX_TYPE_SIG));
	}
	else
	{
		if(ctx->pkcs11)
		{
			BMD_FOK(bmd_check_ctx(ctx,BMD_CTX_SOURCE_PKCS11,BMD_CTX_TYPE_SIG));
		}
		else
		{
			if( ctx->ks )
			{
				BMD_FOK(bmd_check_ctx(ctx,BMD_CTX_SOURCE_KS,BMD_CTX_TYPE_ASYM));
			}
			else
			{
				BMD_FOK(BMD_ERR_PARAM2);
			}
		}
	}

	if(output==NULL)		{	BMD_FOK(BMD_ERR_PARAM3);	}
	if((*output)!=NULL)	{	BMD_FOK(BMD_ERR_PARAM3);	}

	if(opt!=NULL)
	{
		options=(bmd_signature_params_t *)opt;
	}
	else
	{
		opt_local=1;
		options=(bmd_signature_params_t *)malloc(sizeof(bmd_signature_params_t));
		memset(options,0,sizeof(bmd_signature_params_t));
		options->signed_attributes=NULL;
		options->unsigned_attributes=NULL;
		options->signature_flags=BMD_CMS_SIG_INTERNAL;
	}

	BMD_FOK(__bmd_create_signed_data(input,options->signature_flags,&sigData));
	BMD_FOK(__bmd_create_signer_info(&signerInfo,&sigData,options,input,ctx));
	if(ctx->file)
	{
		BMD_FOK(__bmd_add_signer_cert(ctx->file->cert,&sigData));
	}
	else
	{
		if( ctx->pkcs11 != NULL )
		{
			BMD_FOK(__bmd_add_signer_cert(ctx->pkcs11->cert,&sigData));
		}
		else
			if( ctx->ks != NULL )
			{
				BMD_FOK(__bmd_add_signer_cert(ctx->ks->sig_cert,&sigData));
			}
	}

	{
		long OID_sha_1[] = {OID_SHA_1_LONG};
		DigestAlgorithmIdentifier_t *digestAlg = NULL;
		digestAlg = mallocStructure(sizeof(DigestAlgorithmIdentifier_t));
		AlgorithmIdentifier_set_OID(digestAlg,OID_sha_1,sizeof(OID_sha_1)/sizeof(*OID_sha_1));
		BMD_FOK(SignedData_add_DigestAlgorithm(sigData, digestAlg));
	}

	// ustawienie wersji
	BMD_FOK(SignedData_count_Version(sigData));

	// utworz general CMS content
	content = (ContentInfo_t *)malloc(sizeof(ContentInfo_t));
	if ( content == NULL )
	{
		PRINT_ERROR("LIBBMDPKIERR Memory error. Error=%i\n",BMD_ERR_MEMORY);
		return BMD_ERR_MEMORY;
	}
	memset(content,0,sizeof(ContentInfo_t));

	BMD_FOK(ContentInfo_set_SignedData(content, sigData));

	BMD_FOK(der_encode_ContentInfo(content, output));

	if ( content )
	{
		asn_DEF_ContentInfo.free_struct(&asn_DEF_ContentInfo, content, 0);
	}

	if ( sigData )
	{
		asn_DEF_SignedData.free_struct(&asn_DEF_SignedData, sigData, 0);
	}

	if(opt_local)
	{
		free(options);options=NULL;
	}

	return BMD_OK;
}

/* funkcja ta wykonuje podpis zewnetrzny na pliku wskazywanym przez filename metoda podpisu duzych plikow */
long bmd_sign_file(const char *filename,bmd_crypt_ctx_t *sig_ctx,GenBuf_t **output,void *opt,progress_data_t *pd)
{
	return bmd_sign_file_wv(filename,sig_ctx,output,opt,pd, NULL, 0 , 0 );
}

/* funkcja ta wykonuje podpis zewnetrzny na pliku wskazywanym przez filename metoda podpisu duzych plikow
jednoczesnie sprawdzajac wyliczone skroty z plikow ze skrotami podanymi w parametrze (weryfikacja podmiany)
@retval BMD_OK
@retval BMD_ERR_PARAM1 - bledny parameter wywolania funkcji
@retval BMD_ERR_PARAM2 - bledny parameter wywolania funkcji
@retval BMD_ERR_PARAM3 - bledny parameter wywolania funkcji
@retval BMD_ERR_INTERRUPTED - uzytkownik przerwal
@retval BMD_ERR_MEMORY - brak pamieci do wczytania pliku (dla wyliczenia skrotu)
@retval BMD_ERR_HASH_VERIFY - jesli skrot z podanego pliku i wartosci kontrolnej sa rozne
@retval BMD_ERR_DISK - jesli na przyklad plik jest w tym czasie w czym innym otwarty
@retval BMD_ERR_KEY_CORRESP	- blad - klucz publiczny nie koresponduje z prywatnym
@retval BMD_ERR_SIGN_VERIFY	blad - podpis nie zostal poprawnie zweryfikowany
@param filename - sciezka do pliku (koniecznie zakodowana w UTF8)
@param vrf_keys_corr - czy veryfikowac zgodnosc kluczy publicznego z prywatnym  - 0 nie weryfikowac, kazda inna wartosc oznacza zadanie weryfikacji - weryfikacja odbywa sie przez sprawdzenie poprawnosci podpisu  - zakladamy ze podpis odbywa sie z wykorzystaniem biblioteki PKCS#11 - dla podpisu z pliku nie bedzie
@param vrf_sig_hash - czy weryfikowac popdpis matematycznie - 0 nie weryfikowac, kazda inna wartosc oznacza zadanie weryfikacji - weryfikacja odbywa sie przez sprawdzenie zgodnosci skrotow z podpisu i skrotu z danych wejsciowych - funkcja pozwala zweryfikowac dzialanie zewnetrznych bibliotek podpisujacych

*/
long bmd_sign_file_wv(const char *filename,bmd_crypt_ctx_t *sig_ctx,GenBuf_t **output,void *opt,progress_data_t *pd, GenBuf_t *verhash, long vrf_keys_corr, long vrf_sig_hash)
{
long status;
long fd;
GenBuf_t *portion=NULL ,*tmpgb=NULL;
bmd_crypt_ctx_t *hash_ctx=NULL;
struct stat file_info;
long nread;
long count=0;
bmd_signature_params_t *sig_params=NULL;

#ifdef WIN32
	wchar_t *wideFilename = NULL;
#endif


	if(filename==NULL)
	{
		PRINT_ERROR("LIBBMDPKIERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(sig_ctx==NULL)
	{
		PRINT_ERROR("LIBBMDPKIERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
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

	if(opt==NULL)
	{
		sig_params=(bmd_signature_params_t *)malloc(sizeof(bmd_signature_params_t));
		memset(sig_params,0,sizeof(bmd_signature_params_t));
		sig_params->signed_attributes=NULL;
		sig_params->unsigned_attributes=NULL;
		sig_params->signature_flags=BMD_CMS_SIG_LOB;
	}
	else
	{
		sig_params=(bmd_signature_params_t *)opt;
	}
	#ifdef WIN32
		UTF8StringToWindowsString((char*)filename, &wideFilename, NULL);
		fd=_wopen(wideFilename,O_RDONLY|O_BINARY|_O_RDWR|_O_EXCL);
		free(wideFilename); wideFilename=NULL;
	#else
		fd=open(filename,O_RDONLY|O_BINARY|O_RDWR|O_EXCL);
	#endif

	if(fd<0)
	{
		PRINT_ERROR("LIBBMDPKIERR Disk error. Error=%i\n",BMD_ERR_DISK);
		return BMD_ERR_DISK;
	}
	#ifdef WIN32
	if( _locking( fd, LK_NBLCK, _filelength( fd)) == -1 )
	{
		perror( "Locking failed\n" );
		close( fd );
		return BMD_ERR_DISK;
	}
	#endif
	status=fstat(fd,&file_info);

	BMD_FOK(bmd_set_ctx_hash(&hash_ctx,BMD_HASH_ALGO_SHA1));
	hash_ctx->hash->hash_params=BMD_HASH_MULTI_ROUND;

	portion=(GenBuf_t *)malloc(sizeof(GenBuf_t));
	if(portion==NULL)
	{
		close(fd);
		PRINT_ERROR("LIBBMDPKIERR Memory error. Error=%i\n",BMD_ERR_MEMORY);
		return BMD_ERR_MEMORY;
	}
	memset(portion,0,sizeof(GenBuf_t));
	portion->buf=(char *)calloc(1024*1024, sizeof(char));

	while( (nread=read(fd,portion->buf,1024*1024))>0 )
	{
		portion->size=nread;
		BMD_FOK(bmd_hash_data(portion,&hash_ctx,output,NULL));
		count=count+nread;
		if(pd)
		{
			if( (pd->func) && (pd->prog_data) )
			{
			    pd->total_cur_size=pd->total_cur_size+nread;

				status=((progress_function)pd->func)(pd->prog_data,file_info.st_size,count,pd->total_size,pd->total_cur_size,pd->prog_text);
				if(status!=BMD_OK)
				{
					PRINT_ERROR("LIBBMDPKIERR Error. Error=%i\n",BMD_ERR_OP_FAILED);
					free_gen_buf(&portion);
					free_gen_buf(output);
					close(fd);
					BMD_FOK(bmd_ctx_destroy(&hash_ctx));
					return status; // @retval BMD_ERR_INTERRUPTED - uzytkownik przerwal lub BMD_OK
				}
			}
		}
	}
	close(fd);
	BMD_FOK(bmd_hash_data(NULL,&hash_ctx,output,NULL));
	if ( verhash )
	{
		if ( verhash->size != (*output)->size )
		{
			free_gen_buf(&portion);
			free_gen_buf(output);
			BMD_FOK(bmd_ctx_destroy(&hash_ctx));
			return BMD_ERR_HASH_VERIFY;
		}
		tmpgb=(*output);
		status = memcmp(verhash->buf, tmpgb->buf, verhash->size);
		if ( status != 0)
		{
			free_gen_buf(&portion);
			free_gen_buf(output);
			BMD_FOK(bmd_ctx_destroy(&hash_ctx));
			return BMD_ERR_HASH_VERIFY;
		}
	}

	memset(portion->buf, 0, portion->size);
	free_gen_buf(&portion);

	BMD_FOK(set_gen_buf2((*output)->buf,(*output)->size,&portion));
	memset((*output)->buf, 0, (*output)->size); /*TWo Na potrzeby rozporządzenia*/
	free_gen_buf(output);

	BMD_FOK(bmd_sign_data(portion,sig_ctx,output,sig_params));

	memset(portion->buf, 0, portion->size); /*TWo Na potrzeby rozporządzenia*/
	free_gen_buf(&portion);

	BMD_FOK(bmd_ctx_destroy(&hash_ctx));

	/* weryfikaca podpisu i kluczy*/

	if (vrf_keys_corr)
	{
		if ( (output) && (*output) && (sig_ctx) && (sig_ctx->pkcs11) && (sig_ctx->pkcs11->cert)
		&& (bmd_check_keys_correspondence(*output, sig_ctx->pkcs11->cert ) != BMD_OK ))
		{	return BMD_ERR_KEY_CORRESP;
		}
	}

	if (vrf_sig_hash)
	{
		if ( (output) && (*output) && (verhash)
		&& (bmd_check_sign_with_hash(*output, verhash ) != BMD_OK ))
		{	return BMD_ERR_SIGN_VERIFY;
		}
	}

	return BMD_OK;
}

long bmd_verify_sig_integrity(GenBuf_t *input,GenBuf_t *opt_content)
{
long status;
GenBuf_t *hash=NULL,*cms_sig_value=NULL;
SignedData_t *signed_data=NULL;
RSA *klucz_RSA=NULL;

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

	status=__bmd_get_signed_data(input,&signed_data);
	if (status<BMD_OK)
	{
		PRINT_ERROR("LIBBMDPKIERR Error in getting signed data. Error=%li\n",status);
		return status;
	}

	status=__bmd_get_signing_pubkey(signed_data,&klucz_RSA);
	if (status<BMD_OK)
	{
		PRINT_ERROR("LIBBMDPKIERR Error in getting signing public key data. Error=%li\n",status);
		return status;
	}

	status=__bmd_compute_hash_for_sig_check(signed_data,opt_content,&hash,&cms_sig_value);
	if (status<BMD_OK)
	{
		PRINT_ERROR("LIBBMDPKIERR Error in hash from signature. Error=%li\n",status);
		return status;
	}

	status=RSA_verify(NID_sha1, (unsigned char*)hash->buf, hash->size, (unsigned char*)cms_sig_value->buf, \
	cms_sig_value->size,klucz_RSA);
	if (status<BMD_OK)
	{
		PRINT_ERROR("RSA_verify=%li\n",status);
	}
	PRINT_ERROR("RSA_verify=%li\n",status);

	RSA_free(klucz_RSA);klucz_RSA=NULL;
	free_gen_buf(&hash);free_gen_buf(&cms_sig_value);
	asn_DEF_SignedData.free_struct(&asn_DEF_SignedData,signed_data,0);signed_data=NULL;

	if(status==1)
		return 0;

	return -10;
}

long bmd_get_cert_and_content_from_signature(	GenBuf_t *input,
								GenBuf_t **output_cert,
								GenBuf_t **content)
{
ContentInfo_t *content_info=NULL;
SignedData_t *signed_data=NULL;
GenBuf_t tmp_buf;

	/* zdekoduj struktura jako ContentInfo_t */
	BMD_FOK(get_CMScontent_from_GenBuf(input,&content_info));
	BMD_FOK(ContentInfo_get_SignedData(content_info,&signed_data));
	BMD_FOK(get_first_certificate_from_SignedData(signed_data,output_cert));
	BMD_FOK(SignedData_get_Content(signed_data,&tmp_buf));

	set_gen_buf2(tmp_buf.buf,tmp_buf.size,content);

	asn_DEF_ContentInfo.free_struct(&asn_DEF_ContentInfo, content_info, 0);content_info=NULL;
	asn_DEF_SignedData.free_struct(&asn_DEF_SignedData,signed_data,0);signed_data=NULL;
	return 0;
}

/*by WSZ*/
/*po wywolaniu tej funkcji nalezy zwolnic recznie samo opakowanie encap  (free(encap) - nie dotykac zawartosci)*/
long bmd_sign_EncapsulatedContentInfo(EncapsulatedContentInfo_t *encap, bmd_crypt_ctx_t *ctx, ContentInfo_t **output,void *opt)
{
SignedData_t *sigData=NULL;
SignerInfo_t *signerInfo=NULL;
ContentInfo_t *content=NULL;
bmd_signature_params_t *options=NULL;
// long id[] = {TSTINFO_OID};
long opt_local=0;
GenBuf_t *buf_to_sign=NULL;

	if(ctx==NULL)
	{
		PRINT_ERROR("LIBBMDPKIERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	if(ctx->file)
	{
		BMD_FOK(bmd_check_ctx(ctx,BMD_CTX_SOURCE_FILE,BMD_CTX_TYPE_SIG));
	}
	else
	{
		if(ctx->pkcs11)
		{
			BMD_FOK(bmd_check_ctx(ctx,BMD_CTX_SOURCE_PKCS11,BMD_CTX_TYPE_SIG));
		}
		else
		{
			PRINT_ERROR("LIBBMDPKIERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
			return BMD_ERR_PARAM2;
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
		options=(bmd_signature_params_t *)opt;
	}
	else
	{
		opt_local=1;
		options=(bmd_signature_params_t *)malloc(sizeof(bmd_signature_params_t));
		memset(options,0,sizeof(bmd_signature_params_t));
		options->signed_attributes=NULL;
		options->unsigned_attributes=NULL;
		options->signature_flags=BMD_CMS_SIG_INTERNAL;
	}

	/*zastapione BMD_FOK(__bmd_create_signed_data(input,options->signature_flags,&sigData));*/
	sigData=(SignedData_t *)calloc(1, sizeof(SignedData_t));
	if(sigData == NULL)
	{
		PRINT_ERROR("LIBBMDPKIERR Memory error. Error=%i\n",BMD_ERR_MEMORY);
		return BMD_ERR_MEMORY;
	}
	sigData->encapContentInfo=*encap;

	set_gen_buf2((char *)(encap->eContent->buf), encap->eContent->size, &buf_to_sign); //NIE SPRAWDZAM

	/*podawane id nie jest wewnatrz wykorzystywane (wenstrz w funkcji SignerInfo_generate_Signature())*/
	BMD_FOK(__bmd_create_signer_info(&signerInfo,&sigData,options,buf_to_sign,ctx));
	free_gen_buf(&buf_to_sign);

	if(ctx->file)
	{
		BMD_FOK(__bmd_add_signer_cert(ctx->file->cert,&sigData));
	}
	else
	{
		BMD_FOK(__bmd_add_signer_cert(ctx->pkcs11->cert,&sigData));
	}

	{
		long OID_sha_1[] = {OID_SHA_1_LONG};
		DigestAlgorithmIdentifier_t *digestAlg = NULL;
		digestAlg = mallocStructure(sizeof(DigestAlgorithmIdentifier_t));
		AlgorithmIdentifier_set_OID(digestAlg,OID_sha_1,sizeof(OID_sha_1)/sizeof(*OID_sha_1));
		BMD_FOK(SignedData_add_DigestAlgorithm(sigData, digestAlg));
	}

	// ustawienie wersji
	BMD_FOK(SignedData_count_Version(sigData));

	// utworz general CMS content
	content = (ContentInfo_t *)calloc(1, sizeof(ContentInfo_t));
	if ( content == NULL )
	{
		PRINT_ERROR("LIBBMDPKIERR Memory error. Error=%i\n",BMD_ERR_MEMORY);
		return BMD_ERR_MEMORY;
	}

	BMD_FOK(ContentInfo_set_SignedData(content, sigData));
	*output=content;
	//BMD_FOK(der_encode_ContentInfo(content, output));

	//if ( content )
	//asn_DEF_ContentInfo.free_struct(&asn_DEF_ContentInfo, content, 0);

	if ( sigData )
	{
		asn_DEF_SignedData.free_struct(&asn_DEF_SignedData, sigData, 0);
	}

	if(opt_local)
	{
		free(options);options=NULL;
	}
	return BMD_OK;
}


/*po wywolaniu tej funkcji nalezy zwolnic recznie samo opakowanie encap  (free(encap) - nie dotykac zawartosci)
dla DVCSRequest*/
long bmd_sign_EncapsulatedContentInfo_DVCSReq(EncapsulatedContentInfo_t *encap, bmd_crypt_ctx_t *ctx, ContentInfo_t **output,void *opt)
{
SignedData_t *sigData=NULL;
SignerInfo_t *signerInfo=NULL;
ContentInfo_t *content=NULL;
bmd_signature_params_t *options=NULL;
// long id[] = {DVCS_REQUEST_OID};
long opt_local=0;
GenBuf_t *buf_to_sign=NULL;

	if(ctx==NULL)
	{
		PRINT_ERROR("LIBBMDPKIERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	if(ctx->file)
	{
		BMD_FOK(bmd_check_ctx(ctx,BMD_CTX_SOURCE_FILE,BMD_CTX_TYPE_SIG));
	}
	else
		if(ctx->pkcs11)
		{
			BMD_FOK(bmd_check_ctx(ctx,BMD_CTX_SOURCE_PKCS11,BMD_CTX_TYPE_SIG));
		}
		else
		{
			PRINT_ERROR("LIBBMDPKIERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
			return BMD_ERR_PARAM2;
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
		options=(bmd_signature_params_t *)opt;
	}
	else
	{
		opt_local=1;
		options=(bmd_signature_params_t *)malloc(sizeof(bmd_signature_params_t));
		memset(options,0,sizeof(bmd_signature_params_t));
		options->signed_attributes=NULL;
		options->unsigned_attributes=NULL;
		options->signature_flags=BMD_CMS_SIG_INTERNAL;
	}

    /*zastapione BMD_FOK(__bmd_create_signed_data(input,options->signature_flags,&sigData));*/
	sigData=(SignedData_t *)calloc(1, sizeof(SignedData_t));
	if(sigData == NULL)
	{
		PRINT_ERROR("LIBBMDPKIERR Memory error. Error=%i\n",BMD_ERR_MEMORY);
		return BMD_ERR_MEMORY;
	}
	sigData->encapContentInfo=*encap;

	set_gen_buf2((char *)(encap->eContent->buf), encap->eContent->size, &buf_to_sign); //NIE SPRAWDZAM

	/*podawane id nie jest wewnatrz wykorzystywane (wenstrz w funkcji SignerInfo_generate_Signature())*/
	BMD_FOK(__bmd_create_signer_info(&signerInfo,&sigData,options,buf_to_sign,ctx));
	free_gen_buf(&buf_to_sign);

	if(ctx->file)
	{
		BMD_FOK(__bmd_add_signer_cert(ctx->file->cert,&sigData));
	}
	else
	{
		BMD_FOK(__bmd_add_signer_cert(ctx->pkcs11->cert,&sigData));
	}

	{
		long OID_sha_1[] = {OID_SHA_1_LONG};
		DigestAlgorithmIdentifier_t *digestAlg = NULL;
		digestAlg = mallocStructure(sizeof(DigestAlgorithmIdentifier_t));
		AlgorithmIdentifier_set_OID(digestAlg,OID_sha_1,sizeof(OID_sha_1)/sizeof(*OID_sha_1));
		//!!!!!!!!!!!!! by WSZ NULL dla params
		digestAlg->parameters=(ANY_t*)calloc(1, sizeof(ANY_t));
		digestAlg->parameters->size=2;
		digestAlg->parameters->buf=(uint8_t*)calloc(2, sizeof(uint8_t));
		digestAlg->parameters->buf[1]=0x00;
		digestAlg->parameters->buf[0]=0x05;
		BMD_FOK(SignedData_add_DigestAlgorithm(sigData, digestAlg));
	}

	// ustawienie wersji
	BMD_FOK(SignedData_count_Version(sigData));

	// utworz general CMS content
	content = (ContentInfo_t *)calloc(1, sizeof(ContentInfo_t));
	if ( content == NULL )
	{
		PRINT_ERROR("LIBBMDPKIERR Memory error. Error=%i\n",BMD_ERR_MEMORY);
		return BMD_ERR_MEMORY;
	}

	//! by WSZ , jesli nie to , to serdecznie to wszystko pierdole!!!
	sigData->signerInfos.list.array[0]->digestAlgorithm.parameters=(ANY_t*)calloc(1, sizeof(ANY_t));
	sigData->signerInfos.list.array[0]->digestAlgorithm.parameters->size=2;
	sigData->signerInfos.list.array[0]->digestAlgorithm.parameters->buf=(uint8_t*)calloc(2, sizeof(uint8_t));
	sigData->signerInfos.list.array[0]->digestAlgorithm.parameters->buf[1]=0x00;
	sigData->signerInfos.list.array[0]->digestAlgorithm.parameters->buf[0]=0x05;
	//
	sigData->signerInfos.list.array[0]->signatureAlgorithm.parameters=(ANY_t*)calloc(1, sizeof(ANY_t));
	sigData->signerInfos.list.array[0]->signatureAlgorithm.parameters->size=2;
	sigData->signerInfos.list.array[0]->signatureAlgorithm.parameters->buf=(uint8_t*)calloc(2, sizeof(uint8_t));
	sigData->signerInfos.list.array[0]->signatureAlgorithm.parameters->buf[1]=0x00;
	sigData->signerInfos.list.array[0]->signatureAlgorithm.parameters->buf[0]=0x05;

	BMD_FOK(ContentInfo_set_SignedData(content, sigData));
	*output=content;
	//BMD_FOK(der_encode_ContentInfo(content, output));

	//if ( content )
	//asn_DEF_ContentInfo.free_struct(&asn_DEF_ContentInfo, content, 0);

	if ( sigData )
	{
		asn_DEF_SignedData.free_struct(&asn_DEF_SignedData, sigData, 0);
	}
	if(opt_local)
	{
		free(options);options=NULL;
	}

	return BMD_OK;
}

/**
funkcja bmd_check_keys_correspondence() pozwala sprawdzic, czy podpis sporzadzony zostal kluczem prywatnym
skojarzonym z posiadanym certyfikatem klucza publicznego

@param signature to bufor ze sporzadzonym podpisem
@param certificate to bufor z certyfikatem klucza publicznego

@retval 0 jeśli klucz prywatny wykorzystany do sporzadzenia podpisu signature jest skojarzony
		z kluczem publicznym, którego certyfikat zostal przekazany za certificate
@retval -1 jeśli klucz prywatny wykorzystany do sporzadzenia podpisu signature NIE jest skojarzony
		z kluczem publicznym, którego certyfikat zostal przekazany za certificate
Inna wartości ujemne w przypadku błedów:
@retval -11 jesli za signature przekazano NULL
@retval -12 jesli za certificate przekazano NULL
@retval -13 jesli nie mozna wydobyc klucza publicznego z certyfikatu
@retval -14, -15, -17 jeśli nie można wydobyć danych z podpisu
@retval -16, -19 jeśli wystąpił problem z alokacją pamięci
@retval -18, -20 jeśli wystąpiły problemy z kodowaniem danych
@retval -21 jeśli wystąpił problem podczas weryfikacji danych
@retval -22 jeśli w podpisie uzyto funkcji skrotu innej niz SHA-1

*/
long bmd_check_keys_correspondence(	GenBuf_t *signature,
						GenBuf_t *certificate)
{

long ret_val=0;
GenBuf_t *pub_key=NULL; //klucz publiczny wyciagniety z certyfikatu certificate
SignedData_t *signedData=NULL;
GenBuf_t *sigValue=NULL; //wartosc podpisu wyciagnieta ze struktury CMS podpisu
GenBuf_t *derSigAttrs=NULL;
char sha1_oid[]= {OID_HASH_FUNCTION_SHA1};
long status;


	if(signature == NULL)
		{ return -11; }
	if(certificate == NULL)
		{ return -12; }


	ret_val=GetPubKeyFromCert(certificate, &pub_key);
	if(ret_val != BMD_OK)
		{ return -13; }

	ret_val=__bmd_get_signed_data(signature, &signedData );
	if(ret_val != BMD_OK)
	{
		free_gen_buf(&pub_key);
		return -14;
	}

	if(signedData->signerInfos.list.count <= 0)
	{
		asn_DEF_SignedData.free_struct(&asn_DEF_SignedData, signedData, 1);
		free(signedData);
		free_gen_buf(&pub_key);
		return -15;
	}

	//zakladamy istnienie jednego elementu w zbiorze signerInfos
	ret_val=set_gen_buf2( (char *)(signedData->signerInfos.list.array[0]->signature.buf), signedData->signerInfos.list.array[0]->signature.size, &sigValue);
	if(ret_val != BMD_OK)
	{
		asn_DEF_SignedData.free_struct(&asn_DEF_SignedData, signedData, 1);
		free(signedData);
		free_gen_buf(&pub_key);
		return -16;
	}


	ret_val=OBJECT_IDENTIFIER_cmp( &(signedData->signerInfos.list.array[0]->digestAlgorithm.algorithm), sha1_oid);
	if(ret_val != BMD_OK) //uzyto funkcji skrotu innej niz sha-1
	{
		asn_DEF_SignedData.free_struct(&asn_DEF_SignedData, signedData, 1);
		free(signedData);
		free_gen_buf(&pub_key);
		free_gen_buf(&sigValue);
		return -22;
	}

	//cuda - brak atrybutow podpidsanych - nadgorliwosc gorsza od faszyzmu ;) (by WSZ)
	if(signedData->signerInfos.list.array[0]->signedAttrs == NULL)
	{
		asn_DEF_SignedData.free_struct(&asn_DEF_SignedData, signedData, 1);
		free(signedData);
		free_gen_buf(&pub_key);
		free_gen_buf(&sigValue);
		return -17;
	}

	status=asn1_encode(&asn_DEF_SignedAttributes, signedData->signerInfos.list.array[0]->signedAttrs, NULL, \
	&derSigAttrs);
	if( status != BMD_OK )
	{
		asn_DEF_SignedData.free_struct(&asn_DEF_SignedData, signedData, 1);
		free(signedData);
		free_gen_buf(&pub_key);
		free_gen_buf(&sigValue);
		return -18;
	}

	ret_val=_bmd_verify_RsaWithSHA1_signature(sigValue, derSigAttrs, pub_key);

	asn_DEF_SignedData.free_struct(&asn_DEF_SignedData, signedData, 1);
	free(signedData);
	free_gen_buf(&pub_key);
	free_gen_buf(&sigValue);
	free_gen_buf(&derSigAttrs);

	if(ret_val == BMD_OK) //klucze skojarzone
		{ return 0; }
	else
	{
		if(ret_val == BMD_ERR_PKI_VERIFY) //klucze nie sa skojarzone
			{ return -1; }
		else //jakis inny blad w _bmd_verify_RsaWithSHA1_signature
			{ return -21; }
	}

}

/*by WSZ*/


/** funkcja sprawdza czy umieszczony w podpisie skrot dokumentu zgadza sie z wyliczonym skrotem
@param signature - wskaznik na bufor z weryfikowanym podpisem w der
@param hash - wskaznik na skrot wyliczony z danych
@retval BMD_OK - skrot z podpisu zgadza sie ze wskazanym skrotem
@retval -1 - bledne parametry wywolania
@retval -2 - blad podczas wydobywania skrótu z podpisu
@retval BMD_ERR_SIGN_VERIFY
*/

long bmd_check_sign_with_hash(GenBuf_t *signature, GenBuf_t* hash)
{
	long status =0;
	GenBuf_t *hash_f_sig = NULL;

	if ( (signature == NULL) || (hash == NULL) )											// bledne parametry wywolania
	{	return -1;
	}

	status = get_SignedAttribute_MessageDigest( signature, &hash_f_sig);
	if (status != BMD_OK)																	// blad podczas
	{	free_gen_buf( &hash_f_sig);
		return -1;
	}

	if (hash->size != hash_f_sig->size)
	{	free_gen_buf( &hash_f_sig);
		return BMD_ERR_SIGN_VERIFY;
	}

	status = memcmp(hash->buf, hash_f_sig->buf, hash->size);
	if (status != 0)
	{	free_gen_buf( &hash_f_sig);
		return BMD_ERR_SIGN_VERIFY;
	}

	free_gen_buf( &hash_f_sig);
	return BMD_OK;
}

