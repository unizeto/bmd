/*Autor: Włodzimierz Szczygieł ( Dział Programowania, Unizeto Technologies SA ) */

#include <bmd/libbmddvcs/libbmddvcs.h>
#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdts/libbmdts.h>


/**XADES !!*/
long bmdDVCS_load_Sig(GenBuf_t *signature, bmdDVCS_t **input_data)
{
/*asn_dec_rval_t dec_result;*/
bmdDVCS_t *in=NULL;
GenBuf_t *podpis=NULL;
long i=0;

	PRINT_INFO("LIBBMDDVCSINF Loading signature from genbuf\n");

	/********************************/
	/*	walidacja parametrów	*/
	/********************************/
	if(signature == NULL)
	{
		PRINT_ERROR("LIBBMDDVCSERR Invalid first parameter error. Error=%i\n",-1);
		return -1;
	}
	if(signature->buf == NULL)
	{
		PRINT_ERROR("LIBBMDDVCSERR Invalid first parameter error. Error=%i\n",-2);
		return -2;
	}
	if(input_data == NULL)
	{
		PRINT_ERROR("LIBBMDDVCSERR Invalid second parameter error. Error=%i\n",-3);
		return -3;
	}
	if(*input_data != NULL)
	{
		PRINT_ERROR("LIBBMDDVCSERR Invalid second parameter error. Error=%i\n",-4);
		return -4;
	}

	in=(bmdDVCS_t*)calloc(1, sizeof(bmdDVCS_t));
	if(in == NULL)
	{
		PRINT_ERROR("LIBBMDDVCSERR Memory error. Error=%i\n",-5);
		return -5;
	}

	podpis=(GenBuf_t*)calloc(1, sizeof(GenBuf_t));
	if(podpis == NULL)
	{
		free(in);
		PRINT_ERROR("LIBBMDDVCSERR Memory error. Error=%i\n",-6);
		return -6;
	}
	podpis->size=signature->size;
	podpis->buf=(char *)calloc(signature->size, sizeof(uint8_t));
	if(podpis->buf == NULL)
	{
		free(in);
		free(podpis);
		podpis=NULL;
		PRINT_ERROR("LIBBMDDVCSERR Memory error. Error=%i\n",-7);
		return -7;
	}

	for(i=0; i < podpis->size; i++)
	{
		podpis->buf[i]=signature->buf[i];
	}

	in->signedData=podpis;

	*input_data=in;
	return 0;
}

/** XADES !!*/
long bmdDVCS_load_fileSig(char *file_name, bmdDVCS_t **input_data)
{
GenBuf_t *podpis=NULL;
/*asn_dec_rval_t dec_result;*/
long ret_val=0;
bmdDVCS_t *in=NULL;

	PRINT_INFO("LIBBMDDVCSINF Loading signature from file\n");

	/********************************/
	/*	walidacja parametrów	*/
	/********************************/
	if(file_name == NULL)
	{
		PRINT_ERROR("LIBBMDDVCSERR Invalid first parameter error. Error=%i\n",-1);
		return -1;
	}
	if(input_data == NULL)
	{
		PRINT_ERROR("LIBBMDDVCSERR Invalid second parameter error. Error=%i\n",-2);
		return -2;
	}
	if(*input_data != NULL)
	{
		PRINT_ERROR("LIBBMDDVCSERR Invalid second parameter error. Error=%i\n",-3);
		return -3;
	}

	ret_val=bmd_load_binary_content(file_name, &podpis);
	if(ret_val != 0)
	{
		PRINT_ERROR("LIBBMDDVCSERR Invalid first parameter error. Error=%i\n",-4);
		return -4;
	}

	in=(bmdDVCS_t*)calloc(1, sizeof(bmdDVCS_t));
	if(in == NULL)
	{
		free_gen_buf(&podpis);
		PRINT_ERROR("LIBBMDDVCSERR Memory error. Error=%i\n",-5);
		return -5;
	}

	in->signedData=podpis;

	*input_data=in;
	return 0;
}


long bmdDVCS_set_server(bmdDVCS_t *input_data, char *url)
{
	PRINT_INFO("LIBBMDDVCSINF  Setting DVCS's host address\n");

	/********************************/
	/*	walidacja parametrów	*/
	/********************************/
	if(input_data == NULL)
	{
		PRINT_ERROR("LIBBMDDVCSERR Invalid first parameter error. Error=%i\n",-1);
		return -1;
	}
	if(url == NULL)
	{
		PRINT_ERROR("LIBBMDDVCSERR Invalid second parameter error. Error=%i\n",-2);
		return -2;
	}

	free(input_data->url);
	input_data->url=strdup(url);
	if(input_data->url == NULL)
	{
		PRINT_ERROR("LIBBMDDVCSERR Error. Error=%i\n",-3);
		return -3;
	}
	return 0;
}


long bmdDVCS_set_connectionTimeout(bmdDVCS_t *input_data, long timeout)
{
	if(input_data == NULL)
		{ return -1; }
	if(timeout <= 0)
		{ return -2; }

	input_data->connectionTimeout=timeout;
	return 0;
}


long bmdDVCS_validate_signature(bmdDVCS_t *input_data, bmd_crypt_ctx_t *context)
{
long result=0;
long ret_val=0;

	PRINT_INFO("LIBBMDDVCSINF  Validating signature by DVCS\n");

	/********************************/
	/*	walidacja parametrów	*/
	/********************************/
	if(input_data == NULL)
	{
		PRINT_ERROR("LIBBMDDVCSERR Invalid first parameter error. Error=%i\n",-11);
		return -11;
	}
	if(input_data->url == NULL)
	{
		PRINT_ERROR("LIBBMDDVCSERR Invalid first parameter error. Error=%i\n",-12);
		return -12;
	}
	if(context == NULL)
	{
		PRINT_ERROR("LIBBMDDVCSERR Invalid second parameter error. Error=%i\n",-13);
		return -13;
	}

	//funkcja przypisuje poswiadczenie (nawet jesli nie mozna orzec poprawnosci - poswiadczenie to cms zawieracjacy DVCSResponse)
	ret_val=call_DVCSservice_signed(SERVICE_TYPE_VSD, context, input_data, &result, 0);
	if(ret_val != 0)
	{
		PRINT_ERROR("LIBBMDDVCSERR Error. Error=%li\n",ret_val);
		return -14;
	}

	return result; //0, 1, badz -1
}


long bmdDVCS_validate_externalSignature(	bmdDVCS_t *input_data,
						GenBuf_t *document,
						long document_type,
						bmd_crypt_ctx_t *context)
{
long result	= 0;
long ret_val	= 0;

	PRINT_INFO("LIBBMDDVCSINF  Validating external signature by DVCS\n");
	/********************************/
	/*	walidacja parametrów	*/
	/********************************/
	if (input_data == NULL)		{	return -11;	}
	if (input_data->url == NULL)	{	return -12;	}
	if (input_data->data != NULL)	{	return -17;	}
	if (document == NULL)		{	return -13;	}
	if (document->buf == NULL && document->size > 0)	{	return -14;	}
	if(context == NULL)		{	return -15;	}
	if(document_type == BMDDVCS_EXTERNAL_DOCUMENT_HASH && input_data->hashAlgorithmOid == NULL)
		{ return -18; }

	//okreslenie czy document to podpisane dane, czy skrot z podpisanych danych
	if(document_type != BMDDVCS_EXTERNAL_DOCUMENT_CONTENT && document_type != BMDDVCS_EXTERNAL_DOCUMENT_HASH)
	{
		input_data->externalDocumentType = BMDDVCS_EXTERNAL_DOCUMENT_CONTENT;
	}
	else
	{
		input_data->externalDocumentType = document_type;
	}

	/*****************************************************************/
	/* funkcja przypisuje poswiadczenie (nawet jesli nie mozna orzec */
	/* poprawnosci - poswiadczenie to cms zawieracjacy DVCSResponse) */
	/*****************************************************************/
	input_data->data=document; //chwilowe zbindowanie wskaznika
	ret_val=call_DVCSservice_externalSig(document, context, input_data, &result);
	input_data->data=NULL;
	if(ret_val != 0)
	{
		PRINT_ERROR("LIBBMDDVCSERR Error. Error=%li\n",ret_val);
		return -16;
	}

	return result; //0, 1, badz -1
}


long bmdDVCS_load_fileCert(	char *pfx_path,
					char *password,
					bmdDVCS_t **input_data)
{
bmd_crypt_ctx_t *context	= NULL;
long ret_val				= 0;
bmdDVCS_t* in			= NULL;

	PRINT_INFO("LIBBMDDVCS Loading certificate from file to DVCS strukture\n");

	/********************************/
	/*	walidacja paramterów	*/
	/********************************/
	if(pfx_path == NULL)
	{
		PRINT_ERROR("LIBBMDPRERR Invalid first parameter value. Error=%i\n",-1);
		return -1;
	}
	if(password == NULL)
	{
		PRINT_ERROR("LIBBMDPRERR Invalid second parameter value. Error=%i\n",-2);
		return -2;
	}
	if(input_data == NULL)
	{
		PRINT_ERROR("LIBBMDPRERR Invalid third parameter value. Error=%i\n",-3);
		return -3;
	}
	if(*input_data != NULL)
	{
		PRINT_ERROR("LIBBMDPRERR Invalid third parameter value. Error=%i\n",-4);
		return -4;
	}

	ret_val=bmd_set_ctx_file(pfx_path, password, (long)strlen(password), &context);
	if(ret_val != 0)
	{
		PRINT_ERROR("LIBBMDPRERR Error in setting certificate from file. Error=%i\n",-5);
		return -5;
	}

	in=(bmdDVCS_t*)calloc(1, sizeof(bmdDVCS_t));
	if( in == NULL )
	{
		bmd_ctx_destroy(&context);
		PRINT_ERROR("LIBBMDPRERR Memory error. Error=%i\n",-6);
		return -6;
	}

	ret_val=Certificate_from_ctx(context, &(in->certificate), 0);
	if(ret_val != 0)
	{
		bmd_ctx_destroy(&context);
		free(in);
		PRINT_ERROR("LIBBMDPRERR Error in getting certificate from context. Error=%i\n",-7);
		return -7;
	}
	bmd_ctx_destroy(&context);

	*input_data=in;
	return 0;
}


long bmdDVCS_load_memCert(GenBuf_t *cert, bmdDVCS_t **input_data)
{
bmdDVCS_t* in=NULL;
asn_dec_rval_t dec_result;

	PRINT_INFO("LIBBMDDVCS Loading certificate from memory to DVCS strukture\n");
	/********************************/
	/*	walidacja paramterów	*/
	/********************************/
	if(cert == NULL)
	{
		PRINT_ERROR("LIBBMDPRERR Invalid first parameter value. Error=%i\n",-1);
		return -1;
	}
	if(cert->buf == NULL)
	{
		PRINT_ERROR("LIBBMDPRERR Invalid first parameter value. Error=%i\n",-2);
		return -2;
	}
	if(input_data == NULL)
	{
		PRINT_ERROR("LIBBMDPRERR Invalid second parameter value. Error=%i\n",-3);
		return -3;
	}
	if(*input_data != NULL)
	{
		PRINT_ERROR("LIBBMDPRERR Invalid second parameter value. Error=%i\n",-4);
		return -4;
	}

	in=(bmdDVCS_t*)calloc(1, sizeof(bmdDVCS_t));
	if (in == NULL)
	{
		PRINT_ERROR("LIBBMDPRERR Memory error. Error=%i\n",-5);
		return -5;
	}

	/*
	dec_result=ber_decode(0, &asn_DEF_Certificate, (void**)cert,ctx->file->cert->buf, ctx->file->cert->size);
	*/

	dec_result=ber_decode(0, &asn_DEF_Certificate, (void**)&((in->certificate)), cert->buf, cert->size);

	if(dec_result.code != RC_OK)
	{
		free(in);
		PRINT_ERROR("LIBBMDPRERR Errorin decoding certificate. Error=%i\n",-6);
		return -6;
	}

	*input_data=in;
	return 0;
}


/*cert num to numer certyfikatu dla kilku czytnikow - numeracja od 0*/
long bmdDVCS_load_Cert(char *pkcs11_library_path, bmdDVCS_t **input_data, long cert_num)
{
	bmd_crypt_ctx_t *context=NULL;
	long ret_val=0;
	bmdDVCS_t* in=NULL;

	if(pkcs11_library_path == NULL)
		{ return -1; }
	if(input_data == NULL)
		{ return -2; }
	if(*input_data != NULL)
		{ return -3; }

	ret_val=bmd_set_ctx_pkcs11(&context, pkcs11_library_path, BMD_CTX_TYPE_SIG, BMD_PKCS11_AUTO_LOAD_CERT, BMD_CTX_PKCS11_OPTION_DEFAULT);
	if(ret_val != 0)
	{
		return -4;
	}

	in=(bmdDVCS_t*)calloc(1, sizeof(bmdDVCS_t));
	if( in == NULL )
	{
		bmd_ctx_destroy(&context);
		return -5;
	}

	ret_val=Certificate_from_ctx(context, &(in->certificate), cert_num);
	if(ret_val != 0)
	{
		bmd_ctx_destroy(&context);
		free(in);
		return -6;
	}

	bmd_ctx_destroy(&context);

	*input_data=in;
	return 0;
}

long bmdDVCS_verify_cert(bmdDVCS_t *input_data, bmd_crypt_ctx_t *context)
{
long ret_val=0;
long result=0;

	PRINT_INFO("LIBBMDDVCSINF Verifying certificate with dvcs\n");
	if(input_data == NULL)
	{
		PRINT_ERROR("LIBBMDDVCSERR Invalid first parameter value. Error=%i\n",-11);
		return -11;
	}
	if(input_data->url == NULL)
	{
		PRINT_ERROR("LIBBMDDVCSERR Invalid first parameter value. Error=%i\n",-12);
		return -12;
	}
	if(context == NULL)
	{
		PRINT_ERROR("LIBBMDDVCSERR Invalid second parameter value. Error=%i\n",-13);
		return -13;
	}

	//funkcja przypisuje poswiadczenie (nawet jesli nie mozna orzec poprawnosci - poswiadczenie to cms zawieracjacy DVCSResponse)
	ret_val=call_DVCSservice_signed(SERVICE_TYPE_VPKC, context, input_data, &result, 0);
	if(ret_val != 0)
	{
		PRINT_ERROR("LIBBMDDVCSERR Error in calling DVCS service. Error=%li\n",ret_val);
		return -14;
	}

	return result; // 0, 1 badz -1
}

long bmdDVCS_load_data(GenBuf_t *data, bmdDVCS_t **input_data)
{
	bmdDVCS_t *in = NULL;

	if(data == NULL)
		{ return -1; }
	if(data->buf == NULL || data->size < 0)
		{ return -2; }
	if(input_data == NULL)
		{ return -3; }
	if(*input_data != NULL)
		{ return -4; }

	in=(bmdDVCS_t*)calloc(1, sizeof(bmdDVCS_t));
	if(in == NULL)
		{ return -5; }
	if( set_gen_buf2(data->buf, data->size, &(in->data)) != 0 )
	{
		free(in);
		return -6;
	}

	*input_data=in;
	return 0;
}

long bmdDVCS_load_fileData(char *file_name, bmdDVCS_t **input_data)
{
	long ret_val=0;
	bmdDVCS_t *in = NULL;

	if(file_name == NULL)
		{ return -1; }
	if(strlen(file_name) <= 0)
		{ return -2; }
	if(input_data == NULL)
		{ return -3; }
	if(*input_data != NULL)
		{ return -4; }

	in=(bmdDVCS_t*)calloc(1, sizeof(bmdDVCS_t));
	if(in == NULL)
		{ return -5; }

	ret_val=bmd_load_binary_content(file_name, &(in->data));
	if(ret_val != BMD_OK)
	{
		free(in);
		return -6;
	}

	*input_data=in;
	return 0;
}


long bmdDVCS_certify_possession_of_data(bmdDVCS_t *input_data, bmd_crypt_ctx_t *context)
{
	long ret_val = 0;
	long result = 0;

	if(input_data == NULL)
		{ return -11; }
	if(input_data->url == NULL)
		{ return -12; }
	if(input_data->data == NULL)
		{ return -13; }
	if(context == NULL)
		{ return -14; }

	ret_val=call_DVCSservice_signed(SERVICE_TYPE_CPD, context, input_data, &result, 0);
	if(ret_val != 0)
	{
		return -15;
	}

	return result; // 0 badz -1
}

long bmdDVCS_certify_existence_of_data(bmdDVCS_t *input_data, bmd_crypt_ctx_t *context, long countHashFlag)
{
	long ret_val = 0;
	long result = 0;

	if(input_data == NULL)
		{ return -11; }
	if(input_data->url == NULL)
		{ return -12; }
	if(input_data->data == NULL)
		{ return -13; }
	if(context == NULL)
		{ return -14; }
	if(countHashFlag == BMDDVCS_HASH_AT_INPUT && input_data->data->size != 20)
		{ return -16; }

	ret_val=call_DVCSservice_signed(SERVICE_TYPE_CCPD, context, input_data, &result, countHashFlag);
	if(ret_val != 0)
	{
		return -15;
	}

	return result; // 0 badz -1
}


long bmdDVCS_get_dvcsCert(bmdDVCS_t *input_data, GenBuf_t **dvcsCert)
{
	if(input_data == NULL)
		{ return -1; }
	if(dvcsCert == NULL)
		{ return -2; }
	if(*dvcsCert != NULL)
		{ return -3; }
	if(input_data->dvcsCert == NULL)
		{ return -4; }

	if( (set_gen_buf2(input_data->dvcsCert->buf, input_data->dvcsCert->size, dvcsCert)) != 0)
		{ return -5; }
	else
		{ return 0; }
}


long bmdDVCS_get_errString(bmdDVCS_t *input_data, char** errString)
{
	if(input_data == NULL)
		{ return -1; }
	if(errString == NULL)
		{ return -2; }
	if( (*errString) != NULL)
		{ return -3; }
	if( input_data->errorString == NULL )
		{ return -4; }

	*errString=strdup( input_data->errorString );
	return 0;
}

/** XADES!!*/
long bmdDVCS_destroy(bmdDVCS_t **input_data)
{
	if(input_data != NULL)
	{
		if(*input_data != NULL)
		{
			free( (*input_data)->url);
			free_gen_buf(&((*input_data)->data));
			Destroy_Certificate(&((*input_data)->certificate));
			//!uwaga , co z NULL w ponizszej funkcji
			free_gen_buf(&((*input_data)->signedData));
			free( (*input_data)->hashAlgorithmOid );
			bmdDVCS_clear_proof( *input_data );
			_destroy_tsAttachments( *input_data );
			free_gen_buf(&((*input_data)->dvcsCert));
			/*
			asn_DEF_ContentInfo.free_struct(&asn_DEF_ContentInfo, (*input_data)->signedData, 1);
			free((*input_data)->signedData);
			*/
			free( (*input_data)->errorString );
			free(*input_data);
			*input_data=NULL;
		}
	}
	return 0;
}

long bmdDVCS_set_proof_Time(bmdDVCS_t* input_data, GenBuf_t* binaryTimestampToken)
{
long retVal=0;
asn_dec_rval_t dec_result;
ContentInfo_t* tsContentInfo = NULL;
SignedData_t* signedData = NULL;
TSTInfo_t* tstInfo = NULL;

	PRINT_INFO("LIBBMDDVCSINF Setting time from timestamp of signature\n");
	if(input_data == NULL)
		{ return -1; }
	if(binaryTimestampToken == NULL)
		{ return -2; }
	if(binaryTimestampToken->buf == NULL || binaryTimestampToken->size <= 0)
		{ return -3; }


	if(input_data->tsTime != NULL)
	{
		PRINT_INFO("LIBBMDDVCSINF Setting time from timestamp of signature (replacing data)\n");
		asn_DEF_DVCSTime.free_struct(&asn_DEF_DVCSTime, input_data->tsTime, 1);
		free(input_data->tsTime); input_data->tsTime=NULL;
	}
	
	dec_result=ber_decode(0, &asn_DEF_ContentInfo, (void**)&tsContentInfo, binaryTimestampToken->buf, binaryTimestampToken->size);
	if(dec_result.code != RC_OK)
	{
		return -11;
	}
	
	retVal=CheckIfCmsIsSignature(tsContentInfo);
	if(retVal != 0)
	{
		asn_DEF_ContentInfo.free_struct(&asn_DEF_ContentInfo, tsContentInfo, 1);
		free(tsContentInfo); tsContentInfo=NULL;
		return -12;
	}

	retVal=ANY_to_type( &(tsContentInfo->content), &asn_DEF_SignedData, (void**)&signedData);
	asn_DEF_ContentInfo.free_struct(&asn_DEF_ContentInfo, tsContentInfo, 1);
	free(tsContentInfo); tsContentInfo=NULL;
	if(retVal != 0)
	{
		return -13;
	}

	retVal=CheckIfSignatureIsTimestamp(signedData);
	if(retVal != 0)
	{
		asn_DEF_SignedData.free_struct(&asn_DEF_SignedData, signedData, 1);
		free(signedData); signedData=NULL;
		return -14;
	}

	retVal=GetTSTInfo_from_SignedData(signedData->encapContentInfo.eContent, &tstInfo);
	asn_DEF_SignedData.free_struct(&asn_DEF_SignedData, signedData, 1);
	free(signedData); signedData=NULL;
	if(retVal != 0)
	{
		return -15;
	}


	input_data->tsTime=(DVCSTime_t*)calloc(1, sizeof(DVCSTime_t));
	if(input_data->tsTime == NULL)
	{
		asn_DEF_TSTInfo.free_struct(&asn_DEF_TSTInfo, tstInfo, 1);
		free(tstInfo); tstInfo=NULL;
		return -16;
	}

	input_data->tsTime->present=DVCSTime_PR_genTime;
	memcpy(&(input_data->tsTime->choice.genTime), &(tstInfo->genTime), sizeof(GeneralizedTime_t));
	memset(&(tstInfo->genTime), 0, sizeof(GeneralizedTime_t));
	asn_DEF_TSTInfo.free_struct(&asn_DEF_TSTInfo, tstInfo, 1);
	free(tstInfo); tstInfo=NULL;

	return 0;
}

long bmdDVCS_set_proof_TimestampToken(bmdDVCS_t* input_data, GenBuf_t* binaryTimestampToken)
{
long retVal=0;
asn_dec_rval_t dec_result;
ContentInfo_t* tsContentInfo = NULL;


	PRINT_INFO("LIBBMDDVCSINF Setting timestamp of signature\n");
	if(input_data == NULL)
		{ return -1; }
	if(binaryTimestampToken == NULL)
		{ return -2; }
	if(binaryTimestampToken->buf == NULL || binaryTimestampToken->size <= 0)
		{ return -3; }


	if(input_data->tsTime != NULL)
	{
		PRINT_INFO("LIBBMDDVCSINF Setting timestamp of signature (replacing data)\n");
		asn_DEF_DVCSTime.free_struct(&asn_DEF_DVCSTime, input_data->tsTime, 1);
		free(input_data->tsTime); input_data->tsTime=NULL;
	}

	dec_result=ber_decode(0, &asn_DEF_ContentInfo, (void**)&tsContentInfo, binaryTimestampToken->buf, binaryTimestampToken->size);
	if(dec_result.code != RC_OK)
	{
		return -11;
	}
	
	retVal=CheckIfCmsIsSignature(tsContentInfo);
	if(retVal != 0)
	{
		asn_DEF_ContentInfo.free_struct(&asn_DEF_ContentInfo, tsContentInfo, 1);
		free(tsContentInfo); tsContentInfo=NULL;
		return -12;
	}

	input_data->tsTime=(DVCSTime_t*)calloc(1, sizeof(DVCSTime_t));
	if(input_data->tsTime == NULL)
	{
		asn_DEF_ContentInfo.free_struct(&asn_DEF_ContentInfo, tsContentInfo, 1);
		free(tsContentInfo); tsContentInfo=NULL;
		return -13;
	}

	input_data->tsTime->present=DVCSTime_PR_timeStampToken;
	memcpy(&(input_data->tsTime->choice.timeStampToken), tsContentInfo, sizeof(ContentInfo_t));
	tsContentInfo=NULL;

	return 0;
}


long bmdDVCS_add_timestamp_attachment(bmdDVCS_t* input_data, GenBuf_t* timestampBuf)
{
long retVal			= 0;

	PRINT_INFO("LIBBMDDVCSINF Adding timestamp attachment\n");
	
	if(input_data == NULL)
		{ return -1; }
	if(timestampBuf == NULL)
		{ return -2; }
	if(timestampBuf->buf == NULL || timestampBuf->size <= 0)
		{ return -3; }
	
	retVal = _add_tsAttachment(input_data, timestampBuf);
	if(retVal < 0)
	{
		PRINT_ERROR("LIBBMDDVCS Error in adding timestamp attachment function (status = %li) \n", retVal);
		return -11;
	}
	
	return 0;
}


long bmdDVCS_clear_timestamp_attachments(bmdDVCS_t* input_data)
{
	PRINT_INFO("LIBBMDDVCSINF Clearing timestamp attachments\n");

	return _destroy_tsAttachments(input_data);
}


long bmdDVCS_clear_proof(bmdDVCS_t* input_data)
{
	PRINT_INFO("LIBBMDDVCSINF Clearing time/timestamp in structure\n");
	if(input_data != NULL)
	{
		if(input_data->tsTime != NULL)
		{
			asn_DEF_DVCSTime.free_struct(&asn_DEF_DVCSTime, input_data->tsTime, 1);
			free(input_data->tsTime); input_data->tsTime=NULL;
		}
	}

	return 0;
}

long bmdDVCS_set_hash_algorithm(bmdDVCS_t* input_data, char* hashAlgorithmOid)
{
	if(input_data == NULL)
		{ return -1; }
	if(hashAlgorithmOid == NULL)
		{ return -2; }
	if(IsOidFormat(hashAlgorithmOid) < 1)
		{ return -3; }
		
	if(input_data->hashAlgorithmOid != NULL)
		{ free(input_data->hashAlgorithmOid); }
	
	input_data->hashAlgorithmOid=strdup(hashAlgorithmOid);
	if(input_data->hashAlgorithmOid == NULL)
		{ return -11; }
	
	return 0;
}
