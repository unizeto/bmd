
/*Autor: Włodzimierz Szczygieł ( Dział Programowania, Unizeto Technologies SA )*/

#include <bmd/libbmddvcs/libbmddvcs.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>
#include <bmd/libbmderr/libbmderr.h>



/* resp_buf zawiera DER z ContentInfo typu SignedData ; w EncapsulatedContentInfo jest DVCSResponse
	na wyjsciu resp , czyli wyodrebniony DVCSResponse
*/

long decode_DVCSResponse(	GenBuf_t *resp_buf,
					DVCSResponse_t **resp)
{
ContentInfo_t *content_info	= NULL;
SignedData_t *signed_data	= NULL;
DVCSResponse_t *dvcs_resp	= NULL;
asn_dec_rval_t dec_result;

char oid_DVCSresp_tab[]		= { DVCS_RESPONSE_OID };
char oid_SignedData_tab[]	= { OID_CMS_ID_SIGNED_DATA };
long ret_val=0;

	if(resp_buf == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(resp_buf->buf == NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(resp == NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(*resp != NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}

	//dekodowanie bufora do ContentInfo
	dec_result=ber_decode(0, &asn_DEF_ContentInfo, (void**)&content_info, resp_buf->buf, resp_buf->size);
	if(dec_result.code != RC_OK)	{	BMD_FOK(-5);	}

	//sprawdzanie,czy faktycznie SignedData
	ret_val=cmp_OID_with_charTab( &(content_info->contentType), oid_SignedData_tab);
	if(ret_val != 0)
	{
		asn_DEF_ContentInfo.free_struct(&asn_DEF_ContentInfo, content_info, 1);
		free(content_info);
		BMD_FOK(-6);
	}
	//dekodowanie zawartosci ContentInfo do SignedData
	dec_result=ber_decode(0, &asn_DEF_SignedData, (void**)&signed_data, content_info->content.buf, content_info->content.size);
	if(dec_result.code != RC_OK)
	{
		asn_DEF_ContentInfo.free_struct(&asn_DEF_ContentInfo, content_info, 1);
		free(content_info);
		BMD_FOK(-7);
	}
	//ContentInfo juz nie potrzebny
	asn_DEF_ContentInfo.free_struct(&asn_DEF_ContentInfo, content_info, 1);
	free(content_info);
	//sprawdzanie, czy w SignedData znajduje sie DVCSResponse

	ret_val=cmp_OID_with_charTab( &(signed_data->encapContentInfo.eContentType), oid_DVCSresp_tab); //libbmdpki
	if(ret_val != 0)
	{
		asn_DEF_SignedData.free_struct(&asn_DEF_SignedData, signed_data, 1);
		free(signed_data);
		BMD_FOK(-8);
	}
	//decodowanie zawartosci SignedData do DVCSResponse
	dec_result=ber_decode(0, &asn_DEF_DVCSResponse, (void**)&dvcs_resp, signed_data->encapContentInfo.eContent->buf, signed_data->encapContentInfo.eContent->size);
	//SignedData nie jest juz potrzebne
	asn_DEF_SignedData.free_struct(&asn_DEF_SignedData, signed_data, 1);
	free(signed_data);

	if(dec_result.code != RC_OK)	{	BMD_FOK(-9);	}

	*resp=dvcs_resp;

	return 0;
}

// rodzaje uslug zdefiniowane w ServiceType.h w libbmdasn1_common
long _create_DVCSRequestInformation(bmdDVCS_t *input, ServiceType_e serviceType, DVCSRequestInformation_t **reqInformation)
{
	DVCSRequestInformation_t *req_info=NULL;
	long ret_val=0;

	if(input == NULL)
		{ return -1; }
	//jesli typ uslugi spoza oczekiwanego zbioru
	if( serviceType < ServiceType_cpd  ||  serviceType > ServiceType_ccpd)
		{ return -2; }
	if(reqInformation == NULL)
		{ return -3; }
	if(*reqInformation != NULL)
		{ return -4; }

	req_info=(DVCSRequestInformation_t*)calloc(1, sizeof(DVCSRequestInformation_t));
	if(req_info == NULL)
		{ return -5; }

	//!version (default 1) - serwer DVCS wymaga, aby nie wypelniac tego pola
	/*!req_info->version=(INTEGER_t*)calloc(1, sizeof(INTEGER_t));
	if(req_info->version == NULL)
	{
		free(req_info);
		return -5;
	}
	ret_val=asn_long2INTEGER(req_info->version, (long)1);
	if(ret_val != 0)
	{
		asn_DEF_DVCSRequestInformation.free_struct(&asn_DEF_DVCSRequestInformation, req_info, 1);
		free(req_info);
		return -6;
	}*/

	//rodzaj wymaganej uslugi
	ret_val=asn_long2INTEGER(&(req_info->service), (long)serviceType);
	if(ret_val != 0)
	{
		asn_DEF_DVCSRequestInformation.free_struct(&asn_DEF_DVCSRequestInformation, req_info, 1);
		free(req_info);
		return -7;
	}

	//nonce OPTIONAL (nie wykorzystujemy)
	//requestTime OPTIONAL (wykorzystane do przekazania znacznika czasu podpisu lub czasu znakowania podpisu)
	if(input->tsTime != NULL)
	{
		ret_val=asn_cloneContent(&asn_DEF_DVCSTime, input->tsTime, (void**)&(req_info->requestTime));
		if(ret_val != 0)
		{
			asn_DEF_DVCSRequestInformation.free_struct(&asn_DEF_DVCSRequestInformation, req_info, 1);
			free(req_info);
			return -8;
		}
	}
	//!requester OPTIONAL ??
	//requestPolicy OPTIONAL (nie narzucamy polityki)
	//dvcs OPTIONAL (nie wykorzystujemy listy DVCS'ow wspolpracujacych)
	//dataLocations OPTIONAL (nie wskazujemy kopi danych z pola 'data' zadania)
	//extensions OPTIONAL (nie wykorzystujemy rozszerzen)

	*reqInformation=req_info;
	return 0;
}

long _create_Data(Data_t **data)
{
	if(data == NULL)
		{ return -1; }
	if(*data != NULL)
		{ return -2; }

	*data=(Data_t*)calloc(1, sizeof(Data_t));
	if(*data == NULL)
		{ return -3; }
	return 0;
}

//dodawanie danych w przypadku uslugi VSD - DER z ContentInfo (zawiera SignedData) wrzucany do Data.message (OCTET_STRING)
//XADES !!
long _add_to_Data_VSD(GenBuf_t *signedData, Data_t *data)
{
	/*asn_enc_rval_t enc_result;*/
	long i=0;

	if(signedData==NULL)
		{ return -1; }
	if(signedData->buf == NULL)
		{ return -2; }
	if(data == NULL)
		{ return -3; }

	data->present=Data_PR_message;

	data->choice.message.size=signedData->size;
	data->choice.message.buf=(uint8_t*)calloc(signedData->size, sizeof(uint8_t));
	for(i=0;i <signedData->size; i++)
	{
		data->choice.message.buf[i]=signedData->buf[i];
	}

	return 0;
}


long _add_to_Data_VSD_External(GenBuf_t *signedData, GenBuf_t *dataContent, long dataContentType, char* hashAlgorithmOid, Data_t *data)
{
	long retVal=0;
	DigestInfo_t *digestInfo=NULL;
	long sha1_oid[] = { OID_SHA_1_LONG};
	GenBuf_t *hash = NULL;
	bmd_crypt_ctx_t *hash_ctx=NULL;
	

	if(signedData == NULL)
		{ return -1; }
	if(signedData->buf == NULL)
		{ return -2; }
	if(dataContent == NULL)
		{ return -3; }
	if(dataContent->buf == NULL)
		{ return -4; }
	if(data == NULL)
		{ return -5; }
	// gdy przekazany podpisany dokument, parametr hashAlgorithmOid jest ignorowany
	if(dataContentType == BMDDVCS_EXTERNAL_DOCUMENT_HASH && hashAlgorithmOid == NULL)
		{ return -6; }

	data->present=Data_PR_messageInfo;

	//ustawienie podpisu zewnetrznego

	retVal=OCTET_STRING_fromBuf( &(data->choice.messageInfo.message), signedData->buf, signedData->size);
	if(retVal != 0)
		{ return -10; }


	//ustawienie skrotu z podpisanych danych

	digestInfo=(DigestInfo_t*)calloc(1, sizeof(DigestInfo_t));
	if(digestInfo == NULL)
	{
		asn_DEF_MessageInfo.free_struct(&asn_DEF_MessageInfo, &(data->choice.messageInfo), 1);
		return -11;
	}

	//ustawienie oidu funkcji skrotu (jesli przekazany byl skrot, a nie caly dokument)
	if(dataContentType == BMDDVCS_EXTERNAL_DOCUMENT_HASH)
	{
		retVal=String2OID(	hashAlgorithmOid, &(digestInfo->digestAlgorithm.algorithm));
		if(retVal < 0)
		{
			asn_DEF_MessageInfo.free_struct(&asn_DEF_MessageInfo, &(data->choice.messageInfo), 1);
			free(digestInfo);
			return -20;
		}
	}
	else
	{
		retVal=OBJECT_IDENTIFIER_set_arcs( &(digestInfo->digestAlgorithm.algorithm), sha1_oid, sizeof(long), sizeof(sha1_oid)/sizeof(long));
		if(retVal != 0)
		{
			asn_DEF_MessageInfo.free_struct(&asn_DEF_MessageInfo, &(data->choice.messageInfo), 1);
			free(digestInfo);
			return -12;
		}
	}

	//ustawienie wartosci skrotu
	if(dataContentType == BMDDVCS_EXTERNAL_DOCUMENT_HASH) //gotowy skrot
	{
		
		retVal=OCTET_STRING_fromBuf( &(digestInfo->digest), dataContent->buf, dataContent->size);
		if(retVal != 0)
		{
			asn_DEF_MessageInfo.free_struct(&asn_DEF_MessageInfo, &(data->choice.messageInfo), 1);
			asn_DEF_DigestInfo.free_struct(&asn_DEF_DigestInfo, digestInfo, 1);
			free(digestInfo);
			return -13;
		}

	}
	else //wyliczanie skrotu
	{
		retVal=bmd_set_ctx_hash(&hash_ctx, BMD_HASH_ALGO_SHA1);
		if(retVal != BMD_OK)
		{
			asn_DEF_MessageInfo.free_struct(&asn_DEF_MessageInfo, &(data->choice.messageInfo), 1);
			asn_DEF_DigestInfo.free_struct(&asn_DEF_DigestInfo, digestInfo, 1);
			free(digestInfo);
			return -14;
		}

		retVal=bmd_hash_data(dataContent, &hash_ctx, &hash, NULL);
		bmd_ctx_destroy(&hash_ctx);
		if(retVal != BMD_OK)
		{
			asn_DEF_MessageInfo.free_struct(&asn_DEF_MessageInfo, &(data->choice.messageInfo), 1);
			asn_DEF_DigestInfo.free_struct(&asn_DEF_DigestInfo, digestInfo, 1);
			free(digestInfo);
			return -15;
		}
		
		retVal=OCTET_STRING_fromBuf( &(digestInfo->digest), hash->buf, hash->size);
		free_gen_buf(&hash);
		if(retVal != 0)
		{
			asn_DEF_MessageInfo.free_struct(&asn_DEF_MessageInfo, &(data->choice.messageInfo), 1);
			asn_DEF_DigestInfo.free_struct(&asn_DEF_DigestInfo, digestInfo, 1);
			free(digestInfo);
			return -16;
		}
		
	}
	
	//wstawienie digestInfo do zbioru externalData
	if( (asn_set_add((void*)&(data->choice.messageInfo.externalData.list), (void*)digestInfo)) != 0 )
	{
		asn_DEF_MessageInfo.free_struct(&asn_DEF_MessageInfo, &(data->choice.messageInfo), 1);
		asn_DEF_DigestInfo.free_struct(&asn_DEF_DigestInfo, digestInfo, 1);
		free(digestInfo);
		return -17;
	}
	

	return 0;
}

//dodawanie danych w  przypadku uslugi- wrzucany DER z dowolnej zawartosci (DER w GenBuf)
long _add_to_Data_CPD(GenBuf_t *der, Data_t *data)
{
	long ret_val=0;

	if(der == NULL)
		{ return -1; }
	if(der->buf == NULL)
		{ return -2; }
	if(data == NULL)
		{ return -3; }

	data->present=Data_PR_message;

	ret_val=OCTET_STRING_fromBuf(&(data->choice.message), (char *)der->buf, der->size);
	if(ret_val != 0)
		{ return -4; }

	return 0;
}

/*liczony jest strot z przekazywanych danych funkcja SHA-1, jesli countHashFlag=CCPD_COUNT_HASH */
long _add_to_Data__CCPD(	GenBuf_t *data_for_hash,
					Data_t *data,
					long countHashFlag)
{
bmd_crypt_ctx_t *hash_context		= NULL;
GenBuf_t *hash				= NULL;
char oid_sha1_tab[]			= { OID_HASH_FUNCTION_SHA1 };
long ret_val				= 0;

	if(data_for_hash == NULL)		{	BMD_FOK(-1);	}
	if(data_for_hash->buf == NULL)	{	BMD_FOK(-2);	}
	if(data == NULL)				{	BMD_FOK(-3);	}

	//w przypadku skrotu na wejsciu, dopuszczalna jest tylko funkcja SHA-1, a wiec 160 bitow (20 bajtow)
	if(countHashFlag == BMDDVCS_HASH_AT_INPUT && data_for_hash->size != 20)
	{
		{	BMD_FOK(-6);	}
	}


	data->present=Data_PR_messageImprint;

	//oid SHA-1
	ret_val=OBJECT_IDENTIFIER_set_arcs(&(data->choice.messageImprint.digestAlgorithm.algorithm),
	oid_sha1_tab, sizeof(oid_sha1_tab[0]), sizeof(oid_sha1_tab)/sizeof(oid_sha1_tab[0]));
	if(ret_val != 0)		{	BMD_FOK(-4);	}

	// serdecznie pierdole
	data->choice.messageImprint.digestAlgorithm.parameters=(ANY_t*)calloc(1, sizeof(ANY_t));
	data->choice.messageImprint.digestAlgorithm.parameters->size=2;
	data->choice.messageImprint.digestAlgorithm.parameters->buf=(uint8_t*)calloc(2, sizeof(uint8_t));
	data->choice.messageImprint.digestAlgorithm.parameters->buf[1]=0x00;
	data->choice.messageImprint.digestAlgorithm.parameters->buf[0]=0x05;

	if(countHashFlag == BMDDVCS_COUNT_HASH)
	{
		//liczenie skrotu z danych za pomoca SHA-1
		bmd_set_ctx_hash(&hash_context,BMD_HASH_ALGO_SHA1);
		bmd_hash_data(data_for_hash, &hash_context, &hash, NULL);
		bmd_ctx_destroy(&hash_context);
	}
	else
	{
		hash=data_for_hash;
	}

	ret_val=OCTET_STRING_fromBuf(&(data->choice.messageImprint.digest), (char *)hash->buf, hash->size);
	if(countHashFlag == BMDDVCS_COUNT_HASH)
	{
		free_gen_buf(&hash);
	}
	else
	{
		hash = NULL;
	}

	if(ret_val != 0)		{	BMD_FOK(-5);	}

	return 0;
}


// tworzenie CertEtcToken - certificate jest klonowany

long _create_CertEtcToken(CertEtcToken_PR flag, Certificate_t *certificate, CertEtcToken_t **cert_token)
{
	CertEtcToken_t *certTok=NULL;
	Certificate_t *cert=NULL;

	bmd_crypt_ctx_t *hash_context=NULL;
	GenBuf_t *der=NULL;
	GenBuf_t *hash=NULL;
	long ret_val=0;
	OCTET_STRING_t *octet_str=NULL;
	long status;
	CertificateSerialNumber_t *sn=NULL;
	Name_t *issuer=NULL;
	IssuerSerial_t *issuer_serial=NULL;
	GeneralNames_t *general_list=NULL;
	GeneralName_t *general_name=NULL;


	if(certificate == NULL)
		{ return -1; }
	if(cert_token == NULL)
		{ return -2; }
	if(*cert_token != NULL)
		{ return -3; }

	//wartosci niewymienione w nawiasie nie sa dopuszczalne w DVCSRequest
	if(flag != CertEtcToken_PR_certificate && flag != CertEtcToken_PR_esscertid && flag != CertEtcToken_PR_extension && flag != CertEtcToken_PR_oscpcertid)
		{ return -4; }

	//niezaimplementowane
	if(flag == CertEtcToken_PR_extension || flag == CertEtcToken_PR_oscpcertid)
		{ return -5; }

	certTok=(CertEtcToken_t*)calloc(1, sizeof(CertEtcToken_t));
	if(certTok == NULL)
		{ return -6; }

	//caly certyfikat wrzucany
	if(flag == CertEtcToken_PR_certificate)
	{
		certTok->present=CertEtcToken_PR_certificate;
		ret_val=asn_cloneContent(&asn_DEF_Certificate, (void *)certificate, (void **)&cert);
		if(ret_val != 0)
		{
			free(certTok);
			return -7;
		}
		certTok->choice.certificate=*cert;
		//niszczenie samego opakowania - zawartosc zostala przypisana
		free(cert); cert=NULL;
	}
	//hash z certyfikatu wrzucany (ESSCertId)
	else
	{
		if(flag == CertEtcToken_PR_esscertid)
		{
			certTok->present=CertEtcToken_PR_esscertid;

			status=asn1_encode(&asn_DEF_Certificate,certificate, NULL, &der);
			if( status != BMD_OK )
				return -8;

			//certyfikat zakodoany
			//liczenie hashu
			//liczenie skroutu z danych za pomoca SHA-1
			bmd_set_ctx_hash(&hash_context,BMD_HASH_ALGO_SHA1);
			bmd_hash_data(der, &hash_context, &hash, NULL);
			free_gen_buf(&der);
			bmd_ctx_destroy(&hash_context);

			//hash wyliczony
			/*Hash_t jest typedefem na OCTET_STRING_t*/
			octet_str=OCTET_STRING_new_fromBuf(&asn_DEF_OCTET_STRING, (const char*)hash->buf, hash->size);
			free_gen_buf(&hash);
			if(hash == NULL)
			{
				free(certTok);
				return -12;
			}
			//
			certTok->choice.esscertid.certHash=*octet_str;
			//niszczenie samego opakowania - zawartosc zostala przypisana
			free(octet_str); octet_str=NULL;

			/*!DONE issuerSerial*/

			//klonowanie seriala certyfikatu z Certificate_t
			ret_val=asn_cloneContent( &asn_DEF_CertificateSerialNumber, &(cert->tbsCertificate.serialNumber), (void **)&(sn));
			if(ret_val != 0)
			{
				asn_DEF_CertEtcToken.free_struct(&asn_DEF_CertEtcToken, certTok ,1);
				free(certTok);
				return -13;
			}

			ret_val=asn_cloneContent( &asn_DEF_Name, &(cert->tbsCertificate.issuer), (void **)&(issuer));
			if(ret_val != 0)
			{
				asn_DEF_CertEtcToken.free_struct(&asn_DEF_CertEtcToken, certTok ,1);
				free(certTok);
				asn_DEF_CertificateSerialNumber.free_struct(&asn_DEF_CertificateSerialNumber, sn, 1);
				free(sn); sn=NULL;
				return -14;
			}

			general_name=(GeneralName_t*)calloc(1, sizeof(GeneralName_t));
			if(general_name == NULL)
			{
				asn_DEF_CertEtcToken.free_struct(&asn_DEF_CertEtcToken, certTok ,1);
				free(certTok);
				asn_DEF_CertificateSerialNumber.free_struct(&asn_DEF_CertificateSerialNumber, sn, 1);
				free(sn); sn=NULL;
				asn_DEF_Name.free_struct(&asn_DEF_Name, issuer, 1);
				free(issuer); issuer=NULL;
				return -15;
			}

			general_name->present=GeneralName_PR_directoryName;
			general_name->choice.directoryName=*issuer;
			free(issuer); issuer=NULL; //opakowanie mozna zwolnic (bebechy przypisane wewnatrz general_name)

			general_list=(GeneralNames_t*)calloc(1, sizeof(GeneralNames_t));
			if(general_list == NULL)
			{
				asn_DEF_CertEtcToken.free_struct(&asn_DEF_CertEtcToken, certTok ,1);
				free(certTok);
				asn_DEF_CertificateSerialNumber.free_struct(&asn_DEF_CertificateSerialNumber, sn, 1);
				free(sn); sn=NULL;
				//opakowanie issuer zostalo zwolnione a bebechy odpowiednio przypisane do general_name
				asn_DEF_GeneralName.free_struct(&asn_DEF_GeneralName, general_name, 1);
				free(general_name); general_name=NULL;
				return -16;
			}

			//binduje wskaznik, wiec juz z general_name nic nie trzeba juz robic
			if( (asn_set_add((void*)&(general_list->list), (void*)general_name)) != 0 )
			{
				asn_DEF_CertEtcToken.free_struct(&asn_DEF_CertEtcToken, certTok ,1);
				free(certTok);
				asn_DEF_CertificateSerialNumber.free_struct(&asn_DEF_CertificateSerialNumber, sn, 1);
				free(sn); sn=NULL;
				asn_DEF_GeneralName.free_struct(&asn_DEF_GeneralName, general_name, 1);
				free(general_name); general_name=NULL;
				free(general_list); general_list=NULL;
				return -17;
			}

			issuer_serial=(IssuerSerial_t*)calloc(1, sizeof(IssuerSerial_t));
			if(issuer_serial == NULL)
			{
				asn_DEF_CertEtcToken.free_struct(&asn_DEF_CertEtcToken, certTok ,1);
				free(certTok);
				asn_DEF_CertificateSerialNumber.free_struct(&asn_DEF_CertificateSerialNumber, sn, 1);
				free(sn); sn=NULL;
				asn_DEF_GeneralNames.free_struct(&asn_DEF_GeneralNames, general_list, 1);
				free(general_list); general_list=NULL;
				return -18;
			}

			issuer_serial->issuer=*general_list;
			free(general_list); general_list=NULL; //opakowanie mozna zwolnic, bebechy odpowiendio przypisane

			issuer_serial->serial=*sn;
			free(sn); sn=NULL; //opakowanie mozna zwolnic, bebechy odpowiendio przypisane

			//binduje wskazniki
			certTok->choice.esscertid.issuerSerial=issuer_serial;

			/*!DONE issuerSerial*/
		}
	}

	*cert_token=certTok;
	return 0;
}

//tworzenie TargetEtcChain
long _create_TargetEtcChain(Certificate_t *certificate, long flag, TargetEtcChain_t **target_chain)
{
	TargetEtcChain_t *tgChain=NULL;
	CertEtcToken_t *certTok=NULL;
	long ret_val=0;

	if(certificate == NULL)
		{ return -1; }
	if(flag != CERT_CONTENT && flag != CERT_HASH)
		{ return -2; }
	if(target_chain == NULL)
		{ return -3; }
	if(*target_chain != NULL)
		{ return -4; }

	tgChain=(TargetEtcChain_t*)calloc(1, sizeof(TargetEtcChain_t));
	if(tgChain == NULL)
		{ return -5; }

	ret_val= _create_CertEtcToken(flag, certificate, &certTok);
	if(ret_val != 0)
	{
		free(tgChain);
		return -6;
	}
	tgChain->target=*certTok;
	//niszczenie samego opakowania - zawartosc zostala przypisana
	free(certTok); certTok=NULL;

	//chain jest OPTIONAL (nie podajemy sciezek certyfikatow)
	//pathProcInput jest OPTIONAL (nie wskazujemy polityk)

	*target_chain=tgChain;
	return 0;
}

long _add_to_Data_VPKC(Certificate_t *certificate, long flag, Data_t *data)
{
	long ret_val=0;
	TargetEtcChain_t *target_chain=NULL;

	if(certificate == NULL)
		{ return -1; }
	if(flag != CERT_CONTENT && flag != CERT_HASH)
		{ return -2; }
	if(data == NULL)
		{ return -3; }

	data->present=Data_PR_certs1;

	ret_val=_create_TargetEtcChain(certificate, flag, &target_chain);
	if(ret_val != 0)
	{
		return -4;
	}

	ret_val=asn_set_add(&(data->choice.certs1.list), (void*)target_chain);
	if(ret_val != 0 )
	{
		asn_DEF_TargetEtcChain.free_struct(&asn_DEF_TargetEtcChain, target_chain, 1);
		free(target_chain);
		return -5;
	}

	return 0;
}

void _destroy_DVCSRequest(DVCSRequest_t **req)
{
	if(req==NULL)
		{ return; }
	if(*req != NULL)
	{
		asn_DEF_DVCSRequest.free_struct(&asn_DEF_DVCSRequest, *req, 1);
		free(*req); *req=NULL;
	}
}


long _create_DVCSRequest(bmdDVCS_t *input, long flag, DVCSRequest_t **request, long countHashFlag)
{
	DVCSRequest_t *dvcsReq=NULL;
	DVCSRequestInformation_t *reqInformation=NULL;
	Data_t *data=NULL;
	long ret_val=0;

	if(input == NULL)
		{ return -1; }
	if(request == NULL)
		{ return -2; }
	if(*request != NULL)
		{ return -3; }


	dvcsReq=(DVCSRequest_t*)calloc(1, sizeof(DVCSRequest_t));
	if(dvcsReq == NULL)
		{ return -4; }

	ret_val=_create_DVCSRequestInformation(input, flag, &reqInformation);
	if(ret_val != 0)
	{
		free(dvcsReq);
		return -5;
	}

	dvcsReq->requestInformation=*reqInformation;
	//niszczenie samego opakowania - zawartosc zostala przypisana
	free(reqInformation); reqInformation=NULL;

	//tworzenie data
	ret_val=_create_Data(&data);
	if(ret_val != 0)
	{
		_destroy_DVCSRequest(&dvcsReq);
		return -6;
	}

	switch(flag)
	{
		case SERVICE_TYPE_CPD:
		{
			if(input->data==NULL)
			{
				free(data);
				_destroy_DVCSRequest(&dvcsReq);
				return -7;
			}
			if(input->data->buf==NULL)
			{
				free(data);
				_destroy_DVCSRequest(&dvcsReq);
				return -8;
			}
			//printf("CPD\n");
			ret_val=_add_to_Data_CPD(input->data, data);
			if(ret_val != 0)
			{
				free(data);
				_destroy_DVCSRequest(&dvcsReq);
				return -9;
			}
			break;
		}
		case SERVICE_TYPE_CCPD:
		{
			if(input->data==NULL)
			{
				free(data);
				_destroy_DVCSRequest(&dvcsReq);
				return -10;
			}
			if(input->data->buf==NULL)
			{
				free(data);
				_destroy_DVCSRequest(&dvcsReq);
				return -11;
			}
			//printf("CCPD\n");
			ret_val=_add_to_Data__CCPD(input->data, data, countHashFlag);
			if(ret_val != 0)
			{
				free(data);
				_destroy_DVCSRequest(&dvcsReq);
				return -12;
			}
			break;
		}
		case SERVICE_TYPE_VSD:
		{
			if(input->signedData==NULL)
			{
				free(data);
				_destroy_DVCSRequest(&dvcsReq);
				return -13;
			}
			//printf("VSD\n");
			if(input->externalDocumentType == 0)
			{
				ret_val=_add_to_Data_VSD(input->signedData, data);
			}
			else
			{
				ret_val=_add_to_Data_VSD_External(input->signedData, input->data, input->externalDocumentType, input->hashAlgorithmOid, data);
			}
			if(ret_val != 0)
			{
				free(data);
				_destroy_DVCSRequest(&dvcsReq);
				return -14;
			}
			break;
		}
		case SERVICE_TYPE_VPKC:
		{
			if(input->certificate==NULL)
			{
				free(data);
				_destroy_DVCSRequest(&dvcsReq);
				return -15;
			}
			//!default CERT_CONTENT
			//printf("VPKC\n");
			ret_val=_add_to_Data_VPKC(input->certificate, CERT_CONTENT, data);
			if(ret_val != 0)
			{
				free(data);
				_destroy_DVCSRequest(&dvcsReq);
				return -16;
			}
			break;
		}
		default:
		{
			free(data);
			_destroy_DVCSRequest(&dvcsReq);
			return -17;
		}
	}
	dvcsReq->data=*data;
	//niszczenie samego opakowania - zawartosc zostala przypisana
	free(data); data=NULL;

	//transactionIdentifier jest OPTIONAL (w chwili obecnej nie implementuje)

	*request=dvcsReq;
	return 0;
}


/*
funkcja koduje do DER strukture DVCSRequest i wynik kodowania umieszcza w OCTET_STRING
*/
long _encode_DVCSRequest_to_eContent(DVCSRequest_t *req, OCTET_STRING_t **econtent)
{
	OCTET_STRING_t *octet=NULL;
	GenBuf_t *tmp_buf=NULL;
	long status;

	if(req == NULL)
		{ return -1; }
	if(econtent == NULL)
		{ return -2; }
	if(*econtent != NULL)
		{ return -3; }


	octet=(OCTET_STRING_t*)calloc(1, sizeof(OCTET_STRING_t));
	if(octet == NULL)
		{ return -4; }

	status=asn1_encode(&asn_DEF_DVCSRequest,req, NULL, &tmp_buf);
	if( status != BMD_OK )
		return -5;

	status=OCTET_STRING_fromBuf(octet,(const char *)tmp_buf->buf,tmp_buf->size);
	free_gen_buf(&tmp_buf);
	if( status != 0 )
		return -6;

	*econtent=octet;
	return 0;
}

/*
funkcja na podstawie struktury DVCSRequest tworzy strukture EncapsulatedContentInfo
*/
long _Create_encapContentInfo_DVCSReq(DVCSRequest_t *req, EncapsulatedContentInfo_t** encap)
{
	OBJECT_IDENTIFIER_t *oid_dvcsReq=NULL;
	EncapsulatedContentInfo_t *nowy_encap=NULL;
	OCTET_STRING_t *econtent=NULL;
	/** @bug serwer DVCS nie rozpoznaje OID DVCSRequest w EncapsulatedData, jedynie data (ogólny typ) */
	/*long oid_tab[]= { DVCS_REQUEST_OID };*/
	long oid_tab[]= { 1, 2, 840, 113549, 1, 7, 1 }; /*CMS data*/
	long ret_val=0;

	if(req == NULL)
		{ return -1; }
	if(encap == NULL)
		{ return -2; }
	if(*encap != NULL)
		{ return -3; }


	if( (_encode_DVCSRequest_to_eContent(req, &econtent)) != 0 )
	{
		return -4;
	}


	oid_dvcsReq=(OBJECT_IDENTIFIER_t*)calloc(1, sizeof(OBJECT_IDENTIFIER_t));
	if(oid_dvcsReq == NULL)
	{
		free(econtent->buf);
		free(econtent);
		return -5;
	}
	ret_val=OBJECT_IDENTIFIER_set_arcs(oid_dvcsReq, oid_tab, sizeof(oid_tab[0]), sizeof(oid_tab)/sizeof(long));
	if(ret_val != 0)
	{
		free(econtent->buf);
		free(econtent);
		free(oid_dvcsReq);
		return -6;
	}

	nowy_encap=(EncapsulatedContentInfo_t*)calloc(1, sizeof(EncapsulatedContentInfo_t));
	if(nowy_encap == NULL)
	{
		free(econtent->buf);
		free(econtent);
		asn_DEF_OBJECT_IDENTIFIER.free_struct(&asn_DEF_OBJECT_IDENTIFIER, oid_dvcsReq, 1);
		free(oid_dvcsReq);
		return -7;
	}
	nowy_encap->eContent=econtent;
	nowy_encap->eContentType=*oid_dvcsReq;
	free(oid_dvcsReq); //opakowanie zwolnione

	*encap=nowy_encap;
	return 0;
}

/*3 kwietnia -------------------------*/

long _encode_DVCSRequest_to_genbuf(DVCSRequest_t *req, GenBuf_t **bufor)
{
	GenBuf_t *der=NULL;
	long status;

	if(req == NULL)
		{ return -1; }
	if(bufor == NULL)
		{ return -2; }
	if(*bufor != NULL)
		{ return -3; }

	status=asn1_encode(&asn_DEF_DVCSRequest,req, NULL, &der);
	if( status != BMD_OK )
		return -4;

	*bufor=der;
	return 0;
}

long _create_Encapsulated_DVCSRequest(bmdDVCS_t *input, long flag, EncapsulatedContentInfo_t **encap, long countHashFlag)
{
	long ret_val=0;
	DVCSRequest_t *request=NULL;
	EncapsulatedContentInfo_t*  encapsulated_contentinfo= NULL;
	/*GenBuf_t *req_der=NULL; !!!test_service*/


	if(input == NULL)
		{ return -1; }
	if(encap == NULL)
		{ return -2; }
	if(*encap != NULL)
		{ return -3; }

	ret_val=_create_DVCSRequest(input, flag, &request, countHashFlag);
	if(ret_val != 0)
	{
		return -4;
	}

	/* test_service
	_encode_DVCSRequest_to_genbuf(request, &req_der);
	bmd_save_buf(req_der, "req.der");
	free_gen_buf(&req_der);
	*/

	ret_val=_Create_encapContentInfo_DVCSReq(request, &encapsulated_contentinfo);
	if(ret_val != 0)
	{
		asn_DEF_DVCSRequest.free_struct(&asn_DEF_DVCSRequest, request, 1);
		free(request);
		return -5;
	}

	_destroy_DVCSRequest(&request);

	*encap=encapsulated_contentinfo;
	return 0;
}

long _get_respStatus(DVCSResponse_t *response, long *status)
{
long resp_status=0;
long ret_val=0;

	if(response == NULL)
	{
		PRINT_ERROR("LIBBMDPRERR Invalid first parameter value. Error=%i\n",-1);
		return -1;
	}

	/********************** poprawka **********************************/
	if(response->present != DVCSResponse_PR_dvCertInfo)
	{
		PRINT_ERROR("Error\n");
		return -2;
	}
	/******************************************************************/
	if(response->choice.dvCertInfo.dvStatus == NULL) //jesli brak to sukces
	{
		*status=0;
		return 0;
	}

	ret_val=asn_INTEGER2long(&(response->choice.dvCertInfo.dvStatus->status), &resp_status);
	if(ret_val != 0)
	{
		PRINT_ERROR("Error\n");
		return -3;
	}

	/*!!! tak mozna dobrac sie o info, co nie tak z certyfikatem
	printf("<%s>\n", response->choice.dvCertInfo.certs2->list.array[0]->chain->list.array[0]->choice.pkistatus.statusString->list.array[0]->buf);
	*/

	*status=resp_status;
	return 0;
}

/*funkcja z odpowiedzi DVCS wydobywa poswiadczenie*/
long _get_dvcsCert(DVCSResponse_t *resp, GenBuf_t **dvcsCert)
{
	GenBuf_t *der=NULL;
	long status;

	if(resp == NULL)
		{ return -1; }
	if(dvcsCert == NULL)
		{ return -2; }
	if(*dvcsCert != NULL)
		{ return -3; }
	if(resp->present != DVCSResponse_PR_dvCertInfo)
		{ return -4; }

	status=asn1_encode(&asn_DEF_DVCSCertInfo,&(resp->choice.dvCertInfo), NULL, &der);
	if( status != BMD_OK )
		return -5;

	*dvcsCert=der;
	return 0;
}

//potrzebne do wersji z tworzeniem ContentInfo - signedData
long _prepare_DVCSReq_to_send(bmdDVCS_t *input , long service_type ,bmd_crypt_ctx_t *context, GenBuf_t **req_buf, long countHashFlag)
{
long ret_val=0;
EncapsulatedContentInfo_t *encap=NULL;
ContentInfo_t *content_info=NULL;
GenBuf_t *der=NULL;
long status;

	if(input == NULL)
	{
		PRINT_ERROR("LIBBMDDVCSERR Invalid first parameter value. Error=%i\n",-1);
		return -1;
	}
	if(context == NULL)
	{
		PRINT_ERROR("LIBBMDDVCSERR Invalid second parameter value. Error=%i\n",-2);
		return -2;
	}
	if(req_buf == NULL)
	{
		PRINT_ERROR("LIBBMDDVCSERR Invalid third parameter value. Error=%i\n",-3);
		return -3;
	}
	if(*req_buf != NULL)
	{
		PRINT_ERROR("LIBBMDDVCSERR Invalid third parameter value. Error=%i\n",-4);
		return -4;
	}

	ret_val=_create_Encapsulated_DVCSRequest(input, service_type, &encap, countHashFlag);
	if(ret_val != 0)
	{
		PRINT_ERROR("LIBBMDDVCSERR Error in creating encapsulated request. Error=%i\n",-5);
		return -5;
	}

	ret_val=bmd_sign_EncapsulatedContentInfo_DVCSReq(encap, context, &content_info, NULL);
	if(ret_val != 0)
	{
		asn_DEF_EncapsulatedContentInfo.free_struct(&asn_DEF_EncapsulatedContentInfo, encap, 1);
		free(encap);
		PRINT_ERROR("LIBBMDDVCSERR Error in signing encapsulated request. Error=%i\n",-6);
		return -6;
	}
	free(encap); encap=NULL; //tylko opakowanie

	//kodowanie do DER ContentInfo
	status=asn1_encode(&asn_DEF_ContentInfo,content_info, NULL, &der);
	if( status != BMD_OK )
	{
		asn_DEF_ContentInfo.free_struct(&asn_DEF_ContentInfo, content_info, 1);
		free(content_info);
		PRINT_ERROR("LIBBMDDVCSERR Error in encoding encapsulated request. Error=%i\n",-7);
		return -7;
	}
	asn_DEF_ContentInfo.free_struct(&asn_DEF_ContentInfo, content_info, 1);
	free(content_info);

	*req_buf=der;

	return 0;
}

void _destroy_DVCSResponse(DVCSResponse_t **resp)
{
	if(resp != NULL)
	{
		if(*resp != NULL)
		{
			asn_DEF_DVCSResponse.free_struct(&asn_DEF_DVCSResponse, *resp, 1);
			free(*resp);
			*resp=NULL;
		}
	}
}

//wersji z tworzeniem ContentInfo - signedData
/*
countHashFlag brane jset tylko pod uwage dla SERVICE_TYPE_CCPD
 countHashFlag rowne 0 oznacza, ze dane wejsciowe to juz gotowy skrot SHA-1 (BMDDVCS_HASH_AT_INPUT)
 countHashFlag > 0 oznacza, ze funkcja ma policzyc skrot SHA-1 z wejsciowych danych (BMDDVCS_COUNT_HASH)
*/
long call_DVCSservice_signed(long service_type, bmd_crypt_ctx_t *context, bmdDVCS_t *input, long *result, long countHashFlag)
{
struct curl_slist *headers=NULL;
CURL *curl_handle=NULL;
GenBuf_t *request=NULL;
GenBuf_t chunk;
long ret_val=0;
DVCSResponse_t *response=NULL;
long resp_stat=0;
long iter=0;
long num_err_strings=0;
char *err_string=NULL; //blad podczas przetwarzania zadania DVCS
char *err_tmp=NULL;
long httpCode=0;

	PRINT_INFO("LIBBMDDVCS Calling dvcs service\n");
	/********************************/
	/*	walidacja parametrów	*/
	/********************************/
	if(context == NULL)
	{
		PRINT_ERROR("LIBBMDDVCSERR Invalid second parameter value. Error=%i\n",-1);
		return -1;
	}
	if(input == NULL)
	{
		PRINT_ERROR("LIBBMDDVCSERR Invalid third parameter value. Error=%i\n",-2);
		return -2;
	}
	if(result == NULL)
	{
		PRINT_ERROR("LIBBMDDVCSERR Invalid fourth parameter value. Error=%i\n",-3);
		return -3;
	}


	//bmd_crypt_ctx_t *context=NULL;
	chunk.buf=NULL;
	chunk.size=0;

	free_gen_buf(&(input->dvcsCert));
	free(input->errorString); input->errorString=NULL;

	ret_val=_prepare_DVCSReq_to_send(input, service_type, context, &request, countHashFlag);
	if(ret_val != 0)
	{
		PRINT_ERROR("LIBBMDDVCSERR Error in preparing DVCS request to send. Error=%i\n",-4);
		return -4;
	}
	//! zapisywanie do pliku wygenerowanego zadania
	/*bmd_save_buf(request, "dvcs.req");*/

	//
	curl_handle = curl_easy_init();

	headers = curl_slist_append(headers, "Content-Type: application/dvcs");

	/*!*///ret_val=curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, cont_temp->content.buf);

	ret_val=curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, request->buf);
	if(ret_val!=CURLE_OK)
	{
		PRINT_ERROR("LIBBMDDVCSERR Error in setting curl option. Error=%i\n",-5);
		return -5;
	}
	/*!*///ret_val=curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDSIZE, cont_temp->content.size);
	ret_val=curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDSIZE, request->size);
	if(ret_val!=CURLE_OK)
	{
		PRINT_ERROR("LIBBMDDVCSERR Error in setting curl option. Error=%i\n",-6);
		return -6;
	}
	ret_val=curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, headers);
	if(ret_val!=CURLE_OK)
	{
		PRINT_ERROR("LIBBMDDVCSERR Error in setting curl option. Error=%i\n",-7);
		return -7;
	}
	ret_val=curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_to_gen_buf_callback);
	if(ret_val!=CURLE_OK)
	{
		PRINT_ERROR("LIBBMDDVCSERR Error in setting curl option. Error=%i\n",-8);
		return -8;
	}
	ret_val=curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
	if(ret_val!=CURLE_OK)
	{
		PRINT_ERROR("LIBBMDDVCSERR Error in setting curl option. Error=%i\n",-9);
		return -9;
	}

	ret_val=curl_easy_setopt(curl_handle, CURLOPT_URL,input->url);
	if(ret_val!=CURLE_OK)
	{
		PRINT_ERROR("LIBBMDDVCSERR Error in setting curl option. Error=%i\n",-10);
		return -10;
	}

	if(input->connectionTimeout > 0)
	{
		ret_val=curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, input->connectionTimeout);
		if(ret_val!=CURLE_OK)
		{
			PRINT_ERROR("LIBBMDDVCSERR Error in setting curl timeout. Error=%i\n",-11);
			return -11;
		}
	}

	ret_val=curl_easy_perform(curl_handle);
	if(ret_val!=CURLE_OK)
	{
		PRINT_ERROR("LIBBMDDVCSERR Error in performing curl operation. Error=%i\n",-12);
		return -12;
	}

	free_gen_buf(&request);

	curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &httpCode);
	asprintf( &(input->errorString), "HTTP RESPONSE CODE: %li", httpCode);

	//! zapisywanie odpowiedzi servera DVCS do odpowiedniego pliku
	/*
	switch(service_type)
	{
		case SERVICE_TYPE_CPD:
			{ bmd_save_buf(&chunk, "dvcs_cpd.resp"); break; }
		case SERVICE_TYPE_CCPD:
			{ bmd_save_buf(&chunk, "dvcs__ccpd.resp"); break;}
		case SERVICE_TYPE_VSD:
			{ bmd_save_buf(&chunk, "dvcs_vsd.resp"); break; }
		case SERVICE_TYPE_VPKC:
			{ bmd_save_buf(&chunk, "dvcs_vpkc.resp"); break; }
		default: { printf ("nierozpoznana usluga\n"); }
	}
	*/


	curl_slist_free_all(headers);
	curl_easy_cleanup(curl_handle);

	ret_val=decode_DVCSResponse(&chunk, &response);
	if(ret_val != 0)
	{
		PRINT_ERROR("LIBBMDDVCSERR Error in decoding DVCS response. Error=%i\n",-13);
		return -13;
	}

	//przypisywanie poswiadczenia
	ret_val=set_gen_buf2(chunk.buf, chunk.size, &(input->dvcsCert));
	if(ret_val != BMD_OK)
	{
		PRINT_ERROR("LIBBMDDVCSERR Error in assigning DVCS certificate. Error=%i\n",-14);
		return -14;
	}
	chunk.buf=NULL;
	chunk.size=0;


	ret_val=_get_respStatus(response, &resp_stat);
	if(ret_val != 0)
	{
		if(response->present == DVCSResponse_PR_dvErrorNote)
		{
			if(response->choice.dvErrorNote.transactionStatus.statusString != NULL)
			{
				num_err_strings=response->choice.dvErrorNote.transactionStatus.statusString->list.count;
				for(iter=0; iter<num_err_strings; iter++)
				{
					if(iter == 0)
					{
						asprintf(&err_tmp, "%s\nDVCS ERROR NOTICE: ", input->errorString); //kod odpowiedzi HTTP
						free(input->errorString);
						input->errorString=NULL;
					}
					asprintf(&err_string, "%s%s\n", err_tmp,  response->choice.dvErrorNote.transactionStatus.statusString->list.array[iter]->buf);
					free(err_tmp);
					err_tmp=err_string;
				}
				err_tmp=NULL;
			}
			if(err_string != NULL)
			{
				free(input->errorString);
				input->errorString=err_string;
			}
		}
		PRINT_ERROR("LIBBMDDVCSERR Error in getting response status. Error=%i\n",-15);
		_destroy_DVCSResponse(&response);
		return -15;
	}

	_destroy_DVCSResponse(&response);

	if(resp_stat > 1)
	{
		*result=-1;
	}
	else //ok
	{
		*result=resp_stat; //0 lub 1 odpowiednio dla kwalifikowanego lub zwyklego
	}
	return 0;
}

/*
long test_service(long service_type, bmd_crypt_ctx_t *context, bmdDVCS_t *input, long *result, DVCSResponse_t **server_resp)
{
	struct curl_slist *headers=NULL;
	CURL *curl_handle=NULL;
	GenBuf_t *request=NULL;
	GenBuf_t chunk;
	long ret_val=0;
	DVCSResponse_t *response=NULL;
	long resp_stat=0;

	if(context == NULL)
		{ return -1; }
	if(input == NULL)
		{ return -2; }
	if(result == NULL)
		{ return -3; }
	if(server_resp == NULL)
		{ return -4; }
	if(*server_resp != NULL)
		{ return -5; }

	//bmd_crypt_ctx_t *context=NULL;
	chunk.buf=NULL;
	chunk.size=0;


	if( (bmd_load_binary_content("req.sig.vpkc.txt",&request)) != 0)
	{
		printf("brak pliku z przygotowanym zadaniem dvcs\n");
		return -6;
	}


	//!!!!
	//ret_val=_prepare_DVCSReq_to_send(input, service_type, context, &request);
	//if(ret_val != 0)
	//	{ return -7; }
	//! zapisywanie do pliku wygenerowanego zadania
	//
	//bmd_save_buf(request, "dvcs.req");

	//
	curl_handle = curl_easy_init();

	headers = curl_slist_append(headers, "Content-Type: application/dvcs");

	//!//ret_val=curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, cont_temp->content.buf);
	ret_val=curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, request->buf);
	if(ret_val != CURLE_OK)
	{
		return -8;
	}
	//!//ret_val=curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDSIZE, cont_temp->content.size);
	ret_val=curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDSIZE, request->size);
	if(ret_val != CURLE_OK)
	{
		return -9;
	}
	ret_val=curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, headers);
	if(ret_val != CURLE_OK)
	{
		return -10;
	}
	ret_val=curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_to_gen_buf_callback);
	if(ret_val != CURLE_OK)
	{
		return -11;
	}
	ret_val=curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
	if(ret_val != CURLE_OK)
	{
		return -12;
	}
	ret_val=curl_easy_setopt(curl_handle, CURLOPT_URL,input->url);
	if(ret_val != CURLE_OK)
	{
		return -13;
	}

	ret_val=curl_easy_perform(curl_handle);
	if(ret_val != CURLE_OK)
	{
		return -14;
	}

	free_gen_buf(&request);

	//! zapisywanie odpowiedzi servera DVCS do odpowiedniego pliku

	switch(service_type)
	{
		case SERVICE_TYPE_CPD:
			{ bmd_save_buf(&chunk, "dvcs_cpd.resp"); break; }
		case SERVICE_TYPE_CCPD:
			{ bmd_save_buf(&chunk, "dvcs__ccpd.resp"); break;}
		case SERVICE_TYPE_VSD:
			{ bmd_save_buf(&chunk, "dvcs_vsd.resp"); break; }
		case SERVICE_TYPE_VPKC:
			{ bmd_save_buf(&chunk, "dvcs_vpkc.resp"); break; }
		default: { printf ("nierozpoznana usluga\n"); }
	}


	curl_slist_free_all(headers);
	curl_easy_cleanup(curl_handle);
	//

	ret_val=decode_DVCSResponse(&chunk, &response);
	if(ret_val != 0)
	{
		_destroy_DVCSResponse(&response);
		return -15;
	}

	//przypisywanie odpowiedzi
	*server_resp=response;

	ret_val=_get_respStatus(response, &resp_stat);
	if(ret_val != 0)
	{
		return -16;
	}

	free(chunk.buf);

	if(resp_stat > 1)
	{
		*result=-1;
		return 0;
	}
	else //ok
	{
		*result=resp_stat;
		return 0;
	}
}
*/



/*
//wersja z puszczaniem DER bezposrednio z DVCSRequest ...
long call_DVCSservice(char *dvcs_host, long dvcs_port, long service_type, bmdDVCS_t *input)
{
	struct curl_slist *headers=NULL;
	CURL *curl_handle=NULL;
	GenBuf_t *request=NULL;
	GenBuf_t chunk;
	long ret_val=0;
	char *url=NULL;
	DVCSResponse_t *response=NULL;
	long resp_stat=0;
	DVCSRequest_t *DVCSReq=NULL;

	chunk.buf=NULL;
	chunk.size=0;

	ret_val=_create_DVCSRequest(input, service_type, &DVCSReq);
	if(ret_val != BMD_OK)
	{
		return -201;
	}
	ret_val=_encode_DVCSRequest_to_genbuf(DVCSReq, &request);
	if(ret_val != BMD_OK)
	{
		_destroy_DVCSRequest(&DVCSReq);
		return -202;
	}
	_destroy_DVCSRequest(&DVCSReq);

	asprintf(&url,"%s:%i/dvcsserver", dvcs_host, dvcs_port); //!!!

	//
	curl_handle = curl_easy_init();

	headers = curl_slist_append(headers, "Content-Type: application/dvcs");

	ret_val=curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, request->buf);
	if(ret_val != CURLE_OK)
	{
		free(url);
		return -203;
	}
	ret_val=curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDSIZE, request->size);
	if(ret_val != CURLE_OK)
	{
		free(url);
		return -204;
	}
	ret_val=curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, headers);
	if(ret_val != CURLE_OK)
	{
		free(url);
		return -205;
	}
	ret_val=curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_to_gen_buf_callback);
	if(ret_val != CURLE_OK)
	{
		free(url);
		return -206;
	}
	ret_val=curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
	if(ret_val != CURLE_OK)
	{
		free(url);
		return -207;
	}
	ret_val=curl_easy_setopt(curl_handle, CURLOPT_URL,url);
	if(ret_val != CURLE_OK)
	{
		free(url);
		return -208;
	}

	printf("przed wykonaniem\n");
	//free(url); url=NULL;
	ret_val=curl_easy_perform(curl_handle);
	if(ret_val != CURLE_OK)
	{
		return -209;
	}

	bmd_save_buf(&chunk, "dvcs1.resp"); //!zapisywanie do bufora

	curl_slist_free_all(headers);
	curl_easy_cleanup(curl_handle);
	//



	ret_val=decode_DVCSResponse(&chunk, &response);
	if(ret_val != BMD_OK)
	{
		return -300;
	}

	ret_val=_get_respStatus(response, &resp_stat);
	if(ret_val != BMD_OK)
	{
		return -400;
	}

	free(chunk.buf);

	if(resp_stat > 1)
	{
		return -1;
	}
	else //ok
	{
		return 0;
	}
}
*/
