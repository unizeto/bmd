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
#include <bmd/libbmdts/libbmdts.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>

/*teporarily added here by WSZ (finally will by moved to header file)*/
#include<bmd/libbmdasn1_common/TimeStampReq.h>
#include<bmd/libbmdasn1_common/PKIStatusInfo.h>
/**/

#ifndef TRUE
#define TRUE 1
#endif

#ifdef WIN32
#pragma warning(disable:4100)
#endif

/**
@param timeout to timeout (w sekundach) na polaczenie i transfer danych. Wartosc <= 0 oznacza brak timeout'u.
*/
long bmd_timestamp_data(	GenBuf_t *input,
				long hash_algo,
				char *ts_server,
				long ts_port,
				long transport_type,
		      		char *proxy_addr,
				long proxy_port,
				char *proxy_username,
				char *proxy_password,
				long retry_count,
				long timeout,
		      	 	GenBuf_t **output)
{
long status			= 0;
GenBuf_t *ts_request		= NULL;
struct curl_slist *headers	= NULL;
CURL *curl_handle		= NULL;
GenBuf_t chunk;
char *url			= NULL;
char *proxy_url			= NULL;
char *proxy_userpass		= NULL;
long tmp_count			= 0;
long i				= 0;
char **tempSeparated		= NULL;
long tempSeparatedCount		= 0;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (input==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (input->buf==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (input->size<=0)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (hash_algo!=BMD_HASH_ALGO_SHA1)	{	BMD_FOK(BMD_ERR_UNIMPLEMENTED);	}
	if (ts_server==NULL)			{	BMD_FOK(BMD_ERR_PARAM3);	}
	if ( ts_port<0)				{	BMD_FOK(BMD_ERR_PARAM4);	}
	if ( ts_port>65536)			{	BMD_FOK(BMD_ERR_PARAM4);	}
	if (output==NULL)			{	BMD_FOK(BMD_ERR_PARAM9);	}
	if ((*output)!=NULL)			{	BMD_FOK(BMD_ERR_PARAM9);	}

	if (ts_port!=80)
	{
		for (i=0; i<(int)strlen(ts_server); i++)
		{
			if (ts_server[i]=='/')
			{
				/*zmieniona implementacja - strsep() Lunix only (by WSZ)
				ts_server_prefix = strsep(&ts_server, "/");
				asprintf(&url,"%s:%i/%s",ts_server_prefix,ts_port,ts_server);
				*/
				bmd_strsep(ts_server, '/', &tempSeparated, &tempSeparatedCount);
				asprintf(&url,"%s:%li/%s",tempSeparated[0], ts_port, (ts_server+i+1));
				bmd_strsep_destroy(&tempSeparated, tempSeparatedCount);

				break;
			}
		}

		if (i==(int)strlen(ts_server))
		{
			asprintf(&url,"%s:%li",ts_server,ts_port);
		}
	}
	else
	{
		asprintf(&url,"%s",ts_server);
	}
	PRINT_DEBUG("LIBBMDPKDEBUG Timestamp address: %s\n", url);

	if ( (proxy_addr!=NULL) && ( (proxy_port>0) && (proxy_port<65536) ) )
	{
		asprintf(&proxy_url,"%s%li",proxy_addr,proxy_port);
	}
	if ( (proxy_username!=NULL) && (proxy_password!=NULL) )
	{
		asprintf(&proxy_userpass,"%s:%s",proxy_username,proxy_password);
	}
	//status=CreateTsRequest(input,hash_algo,&ts_request);
	BMD_FOK(_bmd_create_ts_request(input,hash_algo,&ts_request));

	chunk.buf=NULL;
	chunk.size=0;

	//curl_global_init(CURL_GLOBAL_ALL);
	curl_handle = curl_easy_init();
	headers = curl_slist_append(headers, "Content-Type: application/timestamp-query");
	status=curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, ts_request->buf);
	if (status!=CURLE_OK)		{	BMD_FOK(BMD_TIMESTAMP_OPERATION_FAILED);	}

	status=curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDSIZE, ts_request->size);
	if (status!=CURLE_OK)		{	BMD_FOK(BMD_TIMESTAMP_OPERATION_FAILED);	}

	status=curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, headers);
	if (status!=CURLE_OK)		{	BMD_FOK(BMD_TIMESTAMP_OPERATION_FAILED);	}

	status=curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_to_gen_buf_callback);
	if (status!=CURLE_OK)		{	BMD_FOK(BMD_TIMESTAMP_OPERATION_FAILED);	}

	status=curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
	if (proxy_url)
	{
		status=curl_easy_setopt(curl_handle,CURLOPT_PROXY,proxy_url);
		if (status!=CURLE_OK)		{	BMD_FOK(BMD_TIMESTAMP_OPERATION_FAILED);	}
	}
	if (proxy_userpass)
	{
		status=curl_easy_setopt(curl_handle,CURLOPT_PROXYAUTH,CURLAUTH_ANY);
		if (status!=CURLE_OK)		{	BMD_FOK(BMD_TIMESTAMP_OPERATION_FAILED);	}

		status=curl_easy_setopt(curl_handle,CURLOPT_PROXYUSERPWD,proxy_userpass);
		if (status!=CURLE_OK)		{	BMD_FOK(BMD_TIMESTAMP_OPERATION_FAILED);	}

	}

	status=curl_easy_setopt(curl_handle, CURLOPT_URL,url);
	if (status!=CURLE_OK)		{	BMD_FOK(BMD_TIMESTAMP_OPERATION_FAILED);	}

	if (timeout > 0)
	{
		status=curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, timeout);
		if (status != CURLE_OK)
			{ BMD_FOK(BMD_TIMESTAMP_OPERATION_FAILED); }
	}


	do
	{
		status=curl_easy_perform(curl_handle);
		if (status!=CURLE_OK)
		{
			free_gen_buf(&ts_request);
			free(url);
			curl_slist_free_all(headers);
			curl_easy_cleanup(curl_handle);
			BMD_FOK(BMD_TIMESTAMP_RESOURCE_NOT_AVAILABLE);
		}

		status=__bmd_decode_timestampresp(&chunk,output);
		if (status!=BMD_OK)
		{
			free(chunk.buf);chunk.buf=NULL;
			free_gen_buf(output);
			chunk.size=0;
			tmp_count++; /* zwieksz licznik wykonanych juz prob */
			/* cross-platform implementation (by WSZ)*/
			#ifndef WIN32
			sleep(1);
			#else //ifdef WIN32
			Sleep(1000); //w milisekundach
			#endif //ifndef WIN32
		}
		else
		{
			break;
		}
	} while( tmp_count<retry_count );

	free_gen_buf(&ts_request);
	free(url);
	curl_slist_free_all(headers);
	curl_easy_cleanup(curl_handle);
	if (status!=0)				{	BMD_FOK(BMD_TIMESTAMP_RESOURCE_NOT_AVAILABLE);	}

	if (proxy_url)
	{
		free(proxy_url);proxy_url=NULL;
	}
	if (proxy_userpass)
	{
		free(proxy_userpass);proxy_userpass=NULL;
	}
	if (chunk.buf)
	{
	    free(chunk.buf);
	    chunk.buf=NULL;
	}

	return BMD_OK;
}


/**----------------------------------------------------------------------------------------*/
/**----------------------------------------------------------------------------------------*/
/** funkcje publiczne do naszego TSA (by WSZ)*/
/**----------------------------------------------------------------------------------------*/
/**----------------------------------------------------------------------------------------*/



/*
Funkcja tworzy strukture TSTInfo
*/
long Create_TSTInfo(long oid_tsapolicy_tab[], long oid_N, TimeStampReq_t *ts_req, long serial, Certificate_t *cert, TSTInfo_t **tst_info)
{
TSTInfo_t *tst=NULL;
MessageImprint_t *mess_print=NULL;
GeneralizedTime_t gen_time;
time_t time_czas;
struct tm *tm_czas=NULL;
BOOLEAN_t *False=NULL; //typedef na int
INTEGER_t *Nonce=NULL;
Name_t *Subject=NULL;
GeneralName_t* gen_name=NULL;
long ret_val=0;

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if (oid_tsapolicy_tab == NULL)					{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (ts_req == NULL)						{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (serial <= 0)							{	BMD_FOK(BMD_ERR_PARAM3);	}
	if (cert == NULL)							{	BMD_FOK(BMD_ERR_PARAM4);	}
	if (tst_info == NULL)						{	BMD_FOK(BMD_ERR_PARAM5);	}
	if (*tst_info != NULL)						{	BMD_FOK(BMD_ERR_PARAM5);	}


	tst=(TSTInfo_t*)calloc(1, sizeof(TSTInfo_t));

	//wersja
	if ( (asn_long2INTEGER(&(tst->version), (long)TSTInfo__version_v1 )) != 0 )
	{
		free(tst); tst=NULL;
		BMD_FOK(BMD_ERR_OP_FAILED);
	}

	//tsa policy
	ret_val=OBJECT_IDENTIFIER_set_arcs(&(tst->policy), oid_tsapolicy_tab, sizeof(oid_tsapolicy_tab[0]), oid_N);
	if (ret_val != 0)
	{
		asn_DEF_TSTInfo.free_struct(&asn_DEF_TSTInfo, tst, 1);
		free(tst);
		BMD_FOK(BMD_ERR_OP_FAILED);
	}

	//klon MessageImprint
	ret_val=asn_cloneContent(&asn_DEF_MessageImprint, (void *)&(ts_req->messageImprint), (void**)&mess_print);
	if (ret_val != 0)
	{
		asn_DEF_TSTInfo.free_struct(&asn_DEF_TSTInfo, tst, 1);
		free(tst);
		BMD_FOK(BMD_ERR_OP_FAILED);
	}
	tst->messageImprint=*mess_print;
	free(mess_print); //zwolnione opakowanie

	//serial
	if ( (asn_long2INTEGER(&(tst->serialNumber), (long)serial)) != 0 )
	{
		asn_DEF_TSTInfo.free_struct(&asn_DEF_TSTInfo, tst, 1);
		free(tst);
		BMD_FOK(BMD_ERR_OP_FAILED);
	}

	//gentTime
	time(&time_czas);
	tm_czas=localtime(&time_czas);
	if (tm_czas == NULL)
	{
		asn_DEF_TSTInfo.free_struct(&asn_DEF_TSTInfo, tst, 1);
		free(tst);
		BMD_FOK(BMD_ERR_OP_FAILED);
	}
	if ( (asn_time2GT(&gen_time, tm_czas, 0)) == NULL )
	{
		asn_DEF_TSTInfo.free_struct(&asn_DEF_TSTInfo, tst, 1);
		free(tst);
		BMD_FOK(BMD_ERR_OP_FAILED);
	}
	tst->genTime=gen_time;

	/*!  accuracy jest OPTIONAL i nie wrzucam*/

	//ordering
	//defaultowo ustawiam ordering na FALSE
	False=(BOOLEAN_t*)calloc(1, sizeof(BOOLEAN_t));
	*False=0;
	tst->ordering=False;

	/*nonce (OPTIONAL)- jesli jest w request, nonce o takiej samej wartosci musi byc w odpowiedzi*/
	if (ts_req->nonce != NULL)
	{
		ret_val=asn_cloneContent(&asn_DEF_INTEGER, (void *)(ts_req->nonce), (void**)&Nonce);
		if (ret_val != 0)
		{
			asn_DEF_TSTInfo.free_struct(&asn_DEF_TSTInfo, tst, 1);
			free(tst);
			BMD_FOK(BMD_ERR_OP_FAILED);
		}
		tst->nonce=Nonce;
	}

	//tsa (OPTIONAL)
	ret_val=asn_cloneContent(&asn_DEF_Name, (void *)&(cert->tbsCertificate.subject), (void**)&Subject);
	if (ret_val != 0)
	{
		asn_DEF_TSTInfo.free_struct(&asn_DEF_TSTInfo, tst, 1);
		free(tst);
		BMD_FOK(BMD_ERR_OP_FAILED);
	}
	gen_name=(GeneralName_t*)calloc(1, sizeof(GeneralName_t));
	if (gen_name == NULL)
	{
		asn_DEF_TSTInfo.free_struct(&asn_DEF_TSTInfo, tst, 1);
		free(tst);
		asn_DEF_Name.free_struct(&asn_DEF_Name, Subject, 1);
		free(Subject);
	}
	gen_name->present=GeneralName_PR_directoryName;
	gen_name->choice.directoryName=*Subject;
	free(Subject); //zwalniam opakowanie
	tst->tsa=gen_name;

	/*!  extension jest OPTIONAL i nie wrzucam*/

	*tst_info=tst;
	return BMD_OK;
}


/*------------------------------------------------------------------------------------*/

/*context to przygotowany wczesniej kontekst*/
long bmd_generate_timestamp(	GenBuf_t *req,
					bmd_crypt_ctx_t *context,
					GenBuf_t **resp,
					void* opt)
{
long ret_val=0;
TimeStampReq_t *ts_req=NULL;
TimeStampResp_t *ts_resp=NULL;
asn_dec_rval_t dec_result;
PKIStatusInfo_t pki_statusinfo;
GenBuf_t *buf_resp=NULL;
TSTInfo_t *tst_info=NULL;
long Serial=0;
Certificate_t *cert=NULL;
EncapsulatedContentInfo_t *encapsul=NULL;
ContentInfo_t *signed_buf=NULL;
bmd_crypt_ctx_t *hash_context=NULL;
GenBuf_t *hash_from_cert=NULL;
long sha1_oid[] = { OID_SHA_1_LONG };
long tsapolicy_oid[] = { TSA_POLICY_OID_LONG }; /*! FAKE OID !!!!*/
GenBuf_t *cert_buf=NULL;
FILE *sn_file=NULL;


	Attribute_t *signingCert=NULL;
	SignedAttributes_t *signed_attrs=NULL;
	bmd_signature_params_t *parametry=NULL;


	if (req == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (context == NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (resp == NULL)		{	BMD_FOK(BMD_ERR_PARAM3);	}
	if (*resp != NULL)		{	BMD_FOK(BMD_ERR_PARAM3);	}

	memset(&pki_statusinfo, 0, sizeof(PKIStatusInfo_t)); //koniecznie wyzerowac


	/**--------------*/
	//decoder sam zaalokuje
	dec_result=ber_decode(0, &asn_DEF_TimeStampReq, (void**)&ts_req, req->buf, req->size);
	if (dec_result.code != RC_OK)
	{
		PRINT_DEBUG("LIBBMDPKIERR Cannot decode timestamp task\n");
		ret_val=_Create_negativeResponse(PKIStatus_rejection, PKIFailureInfo_badDataFormat, resp);
		if (ret_val == BMD_OK)
		{
			PRINT_VDEBUG("LIBBMDPKIINF Denial response given\n");
			return BMD_OK;
		}
		else
		{
			PRINT_DEBUG("LIBBMDPKIERR Cannot generate denial response\n");
			return BMD_ERR_OP_FAILED;
		}
	}

	/*sprawdzanie wykorzystanej funkcji hashujacej
	na razie tylko obsługujemy sha1*/
	ret_val=cmp_OID_with_longTab(&(ts_req->messageImprint.hashAlgorithm.algorithm), sha1_oid, sizeof(sha1_oid)/sizeof(long));
	if (ret_val != BMD_OK)
	{
		PRINT_DEBUG("LIBBMDPKIERR  Wymagana funkcja skrotu nie jest obslugiwana\n");
		asn_DEF_TimeStampReq.free_struct(&asn_DEF_TimeStampReq, ts_req, 1);
		free(ts_req);
		if ( (_Create_negativeResponse(PKIStatus_rejection, PKIFailureInfo_badAlg, resp)) == BMD_OK )
		{
			PRINT_VDEBUG("LIBBMDPKIINF Denial response given\n");
			return BMD_OK;
		}
		else
		{
			PRINT_DEBUG("LIBBMDPKIERR Cannot generate denial response\n");
			return BMD_ERR_OP_FAILED;
		}

	}

	//podukt sha-1 ma zawsze 20 bajtow
	if (ts_req->messageImprint.hashedMessage.size != 20)
	{
		PRINT_DEBUG("LIBBMDPKIERR Invalid hashing method\n");
		asn_DEF_TimeStampReq.free_struct(&asn_DEF_TimeStampReq, ts_req, 1);
		free(ts_req);
		ret_val=_Create_negativeResponse(PKIStatus_rejection, PKIFailureInfo_badDataFormat, resp);
		if (ret_val  == BMD_OK )
		{
			PRINT_VDEBUG("LIBBMDPKIINF Denial response given\n");
			return BMD_OK;
		}
		else
		{
			PRINT_DEBUG("LIBBMDPKIERR Cannot generate denial response\n");
			return BMD_ERR_OP_FAILED;
		}
	}

	/*----------------*/
	//TSA policy
	if (ts_req->reqPolicy == NULL)
	{
		/*PRINT_VDEBUG("OK: Zadanie znacznika czasu nie okresla wymaganej polityki TSA\n");*/
	}
	//sprawdzam czy obslugujemy wymagana polityke TSA
	else
	{
		ret_val=cmp_OID_with_longTab(ts_req->reqPolicy, tsapolicy_oid, sizeof(tsapolicy_oid)/sizeof(long));
		if (ret_val != BMD_OK)
		{
			PRINT_DEBUG("LIBBMDPKIERR Required TSA politics not available\n");
			asn_DEF_TimeStampReq.free_struct(&asn_DEF_TimeStampReq, ts_req, 1);
			free(ts_req);
			ret_val=_Create_negativeResponse(PKIStatus_rejection, PKIFailureInfo_unacceptedPolicy, resp);
			if (ret_val  == BMD_OK )
			{
				PRINT_VDEBUG("LIBBMDPKIINF Denial response given\n");
				return BMD_OK;
			}
			else
			{
				PRINT_DEBUG("LIBBMDPKIERR Cannot generate denial response\n");
				return BMD_ERR_OP_FAILED;
			}
		}
		else
		{
			/*PRINT_VDEBUG("OK: Wymagana polityka TSA jest obslugiwana\n");*/
		}
	}

	/*----------*/
	//Certyfikat z kontekstu
	ret_val=Certificate_from_ctx(context, &cert, 0);
	if (ret_val  != BMD_OK )
	{
		PRINT_DEBUG("LIBBMDPKIERR Cannot get certificate\n");
		asn_DEF_TimeStampReq.free_struct(&asn_DEF_TimeStampReq, ts_req, 1);
		free(ts_req);
		ret_val=_Create_negativeResponse(PKIStatus_rejection, PKIFailureInfo_systemFailure, resp);
		if (ret_val  == BMD_OK )
		{
			PRINT_VDEBUG("LIBBMDPKIINF Denial response given\n");
			return BMD_OK;
		}
		else
		{
			PRINT_DEBUG("LIBBMDPKIERR Cannot generate denial response\n");
			return BMD_ERR_OP_FAILED;
		}
	}
	certPtr_from_ctx(context, &cert_buf, 0); //NIE SPRAWDZAM
	//jest cert

	//jesli nie jest wymagany certyfikat podpisujacy, to pomiąchac trzeba w bmd_sign_TSTInfo
	if ( *(ts_req->certReq) == 0)
	{
		PRINT_DEBUG("LIBBMDPKIERR Signing certificate is not required in response\n");
		asn_DEF_TimeStampReq.free_struct(&asn_DEF_TimeStampReq, ts_req, 1);
		free(ts_req);
		//jesli doszlo tutaj, to na pewno jest Certificate_t *cert != NULL
		asn_DEF_Certificate.free_struct(&asn_DEF_Certificate, cert, 1);
		free(cert);
		return BMD_ERR_UNIMPLEMENTED;
	}
	else
	{
		//atrybut podpisany SigningCertificate
		bmd_set_ctx_hash(&hash_context, BMD_HASH_ALGO_SHA1);
		bmd_hash_data(cert_buf, &hash_context, &hash_from_cert, NULL);
		bmd_ctx_destroy(&hash_context);
		//jesli doszlo tutaj, to na pewno jest Certificate_t *cert != NULL

		ret_val=SigAttr_Create_SigningCertificate(hash_from_cert, cert, &signingCert);
		free_gen_buf(&hash_from_cert);
		if (ret_val != BMD_OK)
		{
			asn_DEF_TimeStampReq.free_struct(&asn_DEF_TimeStampReq, ts_req, 1);
			free(ts_req);
			//jesli doszlo tutaj, to na pewno jest Certificate_t *cert != NULL
			asn_DEF_Certificate.free_struct(&asn_DEF_Certificate, cert, 1);
			free(cert);
			PRINT_DEBUG("LIBBMDPKIERR Error. Error=%i\n",BMD_ERR_OP_FAILED);
			return BMD_ERR_OP_FAILED;
		}
		SignedAtributes_init(&signed_attrs);  // NIE SPRAWDZAM !!!
		SignedAtributes_add(signingCert, signed_attrs);  // NIE SPRAWDZAM !!!
		SigningParams_Create(signed_attrs, NULL, &parametry);  // NIE SPRAWDZAM !!!
	}

	/*-----------*/
	//wczytywanie ts_serial (jeszcze teraz z pliku)
	if ( (_read_ts_serial("serial_num.ts", &Serial, &sn_file)) != BMD_OK )
	{
		PRINT_DEBUG("LIBBMDPKIERR Cannot get serial number of timestamp\n");
		asn_DEF_TimeStampReq.free_struct(&asn_DEF_TimeStampReq, ts_req, 1);
		free(ts_req);
		//jesli doszlo tutaj, to na pewno jest Certificate_t *cert != NULL
		asn_DEF_Certificate.free_struct(&asn_DEF_Certificate, cert, 1);
		free(cert);
		ret_val=_Create_negativeResponse(PKIStatus_rejection, PKIFailureInfo_systemFailure, resp);
		if (ret_val  == BMD_OK )
		{
			PRINT_VDEBUG("LIBBMDPKIINF Denial response given\n");
			SigningParams_Destroy(&parametry);
			return BMD_OK;
		}
		else
		{
			PRINT_DEBUG("LIBBMDPKIERR Cannot generate denial response\n");
			SigningParams_Destroy(&parametry);
			return BMD_ERR_OP_FAILED;
		}
	}

	//przygotowanie TSTInfo
	ret_val=Create_TSTInfo(tsapolicy_oid, sizeof(tsapolicy_oid)/sizeof(long), ts_req, Serial, cert, &tst_info);
	//zwalnianie ts_req, bo juz nie jest dalej potrzebny
	asn_DEF_TimeStampReq.free_struct(&asn_DEF_TimeStampReq, ts_req, 1);
	free(ts_req);
	//zwalnianie cert, bo juz nie jest dalej potrzebny
	asn_DEF_Certificate.free_struct(&asn_DEF_Certificate, cert, 1);
	free(cert);
	if (ret_val != BMD_OK)
	{
		PRINT_DEBUG("LIBBMDPKIERR  Cannot create TSTInfo structure\n");
		ret_val=_Create_negativeResponse(PKIStatus_rejection, PKIFailureInfo_systemFailure, resp);
		if (ret_val  == BMD_OK )
		{
			PRINT_VDEBUG("LIBBMDPKIINF Denial response given\n");
			SigningParams_Destroy(&parametry);
			return BMD_OK;
		}
		else
		{
			PRINT_DEBUG("LIBBMDPKIERR Cannot generate denial response\n");
			SigningParams_Destroy(&parametry);
			return BMD_ERR_OP_FAILED;
		}
	}

	//zapis seriala kolejnego znacznika czasu
	if ( (_write_ts_serial("serial_num.ts", Serial+1, &sn_file)) != BMD_OK )
	{
		PRINT_DEBUG("LIBBMDPKIERR Cannot prepare serial number of the next timestamp\n");
		asn_DEF_TSTInfo.free_struct(&asn_DEF_TSTInfo, tst_info, 1);
		free(tst_info);
		ret_val=_Create_negativeResponse(PKIStatus_rejection, PKIFailureInfo_systemFailure, resp);
		if (ret_val  == BMD_OK )
		{
			PRINT_VDEBUG("LIBBMDPKIINF Denial response given\n");
			SigningParams_Destroy(&parametry);
			return BMD_OK;
		}
		else
		{
			PRINT_DEBUG("LIBBMDPKIERR Cannot generate denial response\n");
			SigningParams_Destroy(&parametry);
			return BMD_ERR_OP_FAILED;
		}
	}

	//przygotowanie encapsulatedContentInfo
	if (_Create_encapContentInfo(tst_info, &encapsul) != BMD_OK)
	{
		PRINT_DEBUG("LIBBMDPKIERR  Cannot prepare encapsulated content info\n");
		asn_DEF_TSTInfo.free_struct(&asn_DEF_TSTInfo, tst_info, 1);
		free(tst_info);
		ret_val=_Create_negativeResponse(PKIStatus_rejection, PKIFailureInfo_systemFailure, resp);
		if (ret_val  == BMD_OK )
		{
			PRINT_VDEBUG("LIBBMDPKIINF Denial response given\n");
			SigningParams_Destroy(&parametry);
			return BMD_OK;
		}
		else
		{
			PRINT_DEBUG("LIBBMDPKIERR Cannot generate denial response\n");
			SigningParams_Destroy(&parametry);
			return BMD_ERR_OP_FAILED;
		}
	}

	//tst_info juz wykorzystane, wiec mozna zwolnic
	asn_DEF_TSTInfo.free_struct(&asn_DEF_TSTInfo, tst_info, 1);
	free(tst_info);

	//signed_buf jest typu ContentInfo_t;
	ret_val=bmd_sign_EncapsulatedContentInfo(encapsul, context, &signed_buf, (void*)parametry);
	SigningParams_Destroy(&parametry);
	if (ret_val != BMD_OK)
	{
		PRINT_DEBUG("LIBBMDPKIERR Cannot sign data\n");
		ret_val=_Create_negativeResponse(PKIStatus_rejection, PKIFailureInfo_systemFailure, resp);
		if (ret_val  == BMD_OK )
		{
			PRINT_VDEBUG("LIBBMDPKIINF Denial response given\n");
			return BMD_OK;
		}
		else
		{
			PRINT_DEBUG("LIBBMDPKIERR Cannot generate denial response\n");
			return BMD_ERR_OP_FAILED;
		}
	}

	//zwalniam tylko opakowanie od encapsul
	free(encapsul);

	//SKLADANIE ODPOWIEDZI ZE ZNACZNIKIEM CZASU
	if (_Create_PKIStatus(PKIStatus_granted, &(pki_statusinfo.status)) != BMD_OK )
	{
		PRINT_DEBUG("LIBBMDPKIERR Cannot generate response status\n");
		asn_DEF_ContentInfo.free_struct(&asn_DEF_ContentInfo, signed_buf, 1);
		free(signed_buf);
		ret_val=_Create_negativeResponse(PKIStatus_rejection, PKIFailureInfo_systemFailure, resp);
		if (ret_val  == BMD_OK )
		{
			PRINT_VDEBUG("LIBBMDPKIINF Denial response given\n");
			return BMD_OK;
		}
		else
		{
			PRINT_DEBUG("LIBBMDPKIERR Cannot generate denial response\n");
			return BMD_ERR_OP_FAILED;
		}
	}

	//przygotowana struktura odpowiedzi znacznika czasu
	ts_resp=(TimeStampResp_t*)calloc(1, sizeof(TimeStampResp_t));
	if (ts_resp == NULL)
	{
		asn_DEF_ContentInfo.free_struct(&asn_DEF_ContentInfo, signed_buf, 1);
		free(signed_buf);
		asn_DEF_PKIStatusInfo.free_struct(&asn_DEF_PKIStatusInfo, &pki_statusinfo, 1);
		ret_val=_Create_negativeResponse(PKIStatus_rejection, PKIFailureInfo_systemFailure, resp);
		if (ret_val  == BMD_OK )
		{
			PRINT_VDEBUG("LIBBMDPKIINF Denial response given\n");
			return BMD_OK;
		}
		else
		{
			PRINT_DEBUG("LIBBMDPKIERR Cannot generate denial response\n");
			return BMD_ERR_OP_FAILED;
		}
	}
	ts_resp->status=pki_statusinfo;
	ts_resp->timeStampToken=signed_buf;

	if (_encode_TimeStampResp(ts_resp, &buf_resp) != BMD_OK)
	{
		PRINT_DEBUG("LIBBMDPKIERR  Cannot encode response\n");
		asn_DEF_TimeStampResp.free_struct(&asn_DEF_TimeStampResp, ts_resp, 1);
		free(ts_resp);
		ret_val=_Create_negativeResponse(PKIStatus_rejection, PKIFailureInfo_systemFailure, resp);
		if (ret_val  == BMD_OK )
		{
			PRINT_VDEBUG("LIBBMDPKIINF Denial response given\n");
			return BMD_OK;
		}
		else
		{
			PRINT_DEBUG("LIBBMDPKIERR Cannot generate denial response\n");
			return BMD_ERR_OP_FAILED;
		}
	}

	asn_DEF_TimeStampResp.free_struct(&asn_DEF_TimeStampResp, ts_resp, 1);
	free(ts_resp);

	*resp=buf_resp;
	return BMD_OK;
}

