/***************************************************************************
 *            libbmdts.c
 *  BMD timestamp parsing library
 *  Wed Mar  1 14:44:02 2006
 *  Copyright  2006  Marcin Szulga
 *  Email:  mszulga@certum.pl
 ****************************************************************************/
/********
 * TODO *
 *******/
/* zamienic GMTime na localtime albo sprawdzic strefy czasowe */

#include <bmd/libbmdts/libbmdts.h>
#include <bmd/libbmdasn1_common/TimeStampReq.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>
#include <bmd/libbmderr/libbmderr.h>

/****************
 * ONE CALL API *
 ***************/
long TSGetAttr(GenBuf_t *GenBuf, TSAttrType_t TSAttrType, char **AttributeString, long *AttributeLong, long twl_no_of_certificate)
{
	long err;

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if (GenBuf == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (GenBuf->buf == NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (GenBuf->size <= 0)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (TSAttrType < 0)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (AttributeString == NULL)	{	BMD_FOK(BMD_ERR_PARAM3);	}
	if (*AttributeString != NULL)	{	BMD_FOK(BMD_ERR_PARAM3);	}
	if (AttributeLong == NULL)	{	BMD_FOK(BMD_ERR_PARAM4);	}
	if (*AttributeLong != 0)	{	BMD_FOK(BMD_ERR_PARAM4);	}

	switch(TSAttrType)
	{
		case TS_ATTR_VERSION:
			BMD_FOK(GetVersionFromTimestamp(GenBuf, AttributeLong));
			break;
		case TS_ATTR_GENTIME:
			BMD_FOK(GetGenerationTimeFromTimestamp(GenBuf, AttributeString));
			break;
		case TS_ATTR_POLICY_OID:
			BMD_FOK(GetPolicyOIDFromTimestamp(GenBuf, AttributeString));
			break;
		case TS_ATTR_SN:
			BMD_FOK(GetSerialNumberFromTimestamp(GenBuf, AttributeString));
			break;
		case TS_ATTR_TSA_DN:
			err = GetTsaDnFromTimestamp(GenBuf, AttributeString);
			if(err == BMD_TIMESTAMP_NO_TSA_FIELD)
			{
				return BMD_TIMESTAMP_NO_TSA_FIELD;
			}
			BMD_FOK(err);
			break;
		case TS_ATTR_TSA_CERT_SN:
			BMD_FOK(GetTsaCertificateSnFromTimestamp(GenBuf, AttributeString, twl_no_of_certificate));
			break;
		case TS_ATTR_TSA_CERT_VALID_NA:
			BMD_FOK(GetTsaCertificateValidityNAFromTimestamp(GenBuf, AttributeString, twl_no_of_certificate));
			break;
		default:
			BMD_FOK(LIBBMDTS_ERR_TS_UNRECOGNIZED_ATTR_TYPE);
			break;
	}

	return BMD_OK;
}


/************
 * EASY API *
 ***********/
long TimestampGenBuf(	GenBuf_t *PlainData,
			GenBuf_t **TimestampedData,
			TimestampSource_t TimestampSource,
			char *TsSrvAddr,
			long TsSrvPort,
			long TsSrvTimeout)
{
bmd_crypt_ctx_t *ctx	= NULL;
GenBuf_t *HashBuf	= NULL;

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if (PlainData == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (PlainData->buf == NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (PlainData->size == 0)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (TimestampedData == NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (*TimestampedData != NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (TsSrvAddr == NULL)		{	BMD_FOK(BMD_ERR_PARAM4);	}
	if (TsSrvPort < 0)		{	BMD_FOK(BMD_ERR_PARAM5);	}

	switch(TimestampSource)
	{
		case PLAIN_DATA:
		{
			/* wykorzystana nowa funkcja do timestampa (by Turtur)*/
			BMD_FOK(bmd_create_ctx(&ctx, BMD_CTX_SOURCE_NONE, BMD_CTX_TYPE_HASH));
			BMD_FOK(bmd_hash_data(PlainData, &ctx, &HashBuf,NULL));
			BMD_FOK(bmd_timestamp_data(HashBuf,BMD_HASH_ALGO_SHA1,TsSrvAddr,TsSrvPort,0,NULL,0,NULL,NULL,3, TsSrvTimeout, TimestampedData));
			BMD_FOK(bmd_ctx_destroy(&ctx));
			break;
		}
		case HASH_DATA:
		{
			BMD_FOK(bmd_timestamp_data(PlainData,BMD_HASH_ALGO_SHA1,TsSrvAddr,TsSrvPort,0,NULL,0,NULL,NULL,3, TsSrvTimeout, TimestampedData));
			break;
		}
		default:
		{
			BMD_FOK(BMD_ERR_PARAM3);
		}
	}

	/************/
	/* poradzki */
	/************/
	free_gen_buf(&HashBuf);

	return BMD_OK;
}

long GetGenerationTimeFromTimestamp(	GenBuf_t *GenBuf,
						char **TimestampGenTime)
{
TSTInfo_t *TSTInfo	= NULL;

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if(GenBuf == NULL)						{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(GenBuf->buf == NULL)						{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(GenBuf->size <= 0)						{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(TimestampGenTime == NULL)					{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (*TimestampGenTime != NULL)					{	BMD_FOK(BMD_ERR_PARAM2);	}

	BMD_FOK(GetTSTInfoFromTimestamp(GenBuf, &TSTInfo));
	BMD_FOK(GetTimeFromTSTInfo(TSTInfo, TimestampGenTime));

	asn_DEF_TSTInfo.free_struct(&asn_DEF_TSTInfo, TSTInfo, 0);
	return BMD_OK;
}

long GetGenerationTimeFromTimestamp_time(	GenBuf_t *GenBuf,time_t *TimestampGenTime)
{
	TSTInfo_t *TSTInfo	= NULL;

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if(GenBuf == NULL)						{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(GenBuf->buf == NULL)						{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(GenBuf->size <= 0)						{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(TimestampGenTime == NULL)					{	BMD_FOK(BMD_ERR_PARAM2);	}

	BMD_FOK(GetTSTInfoFromTimestamp(GenBuf, &TSTInfo));
	//BMD_FOK(GetTimeFromTSTInfo(TSTInfo, TimestampGenTime));
	(*TimestampGenTime)=asn_GT2time(&(TSTInfo->genTime),NULL,1);

	asn_DEF_TSTInfo.free_struct(&asn_DEF_TSTInfo, TSTInfo, 0);
	return BMD_OK;
}

long GetVersionFromTimestamp(	GenBuf_t *GenBuf,
					long *version)
{
TSTInfo_t *TSTInfo		= NULL;

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if(GenBuf == NULL)						{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(GenBuf->buf == NULL)						{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(GenBuf->size == 0)						{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(version == NULL)						{	BMD_FOK(BMD_ERR_PARAM2);	}
	if ( *version != 0)						{	BMD_FOK(BMD_ERR_PARAM2);	}

	BMD_FOK(GetTSTInfoFromTimestamp(GenBuf, &TSTInfo));
	BMD_FOK(GetVersionFromTSTInfo(TSTInfo, version));

	asn_DEF_TSTInfo.free_struct(&asn_DEF_TSTInfo, TSTInfo, 0);
	return BMD_OK;
}

long GetPolicyOIDFromTimestamp(GenBuf_t *GenBuf, char **PolicyOid)
{
TSTInfo_t *TSTInfo = NULL;

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if(GenBuf == NULL)						{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(GenBuf->buf == NULL)						{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(GenBuf->size <= 0)						{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(PolicyOid == NULL)						{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (*PolicyOid != NULL)						{	BMD_FOK(BMD_ERR_PARAM2);	}

	BMD_FOK(GetTSTInfoFromTimestamp(GenBuf, &TSTInfo));
	BMD_FOK(GetPolicyOIDFromTSTInfo(TSTInfo, PolicyOid));

	asn_DEF_TSTInfo.free_struct(&asn_DEF_TSTInfo, TSTInfo, 0);
	return BMD_OK;
}

long GetSerialNumberFromTimestamp(	GenBuf_t *GenBuf,
						char **SerialNumber)
{
TSTInfo_t *TSTInfo = NULL;

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if(GenBuf == NULL)						{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(GenBuf->buf == NULL)						{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(GenBuf->size <= 0)						{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(SerialNumber == NULL)					{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (*SerialNumber != NULL)					{	BMD_FOK(BMD_ERR_PARAM2);	}

	BMD_FOK(GetTSTInfoFromTimestamp(GenBuf, &TSTInfo));
	BMD_FOK(GetSerialNumberFromTSTInfo(TSTInfo, SerialNumber));

	asn_DEF_TSTInfo.free_struct(&asn_DEF_TSTInfo, TSTInfo, 0);
	return BMD_OK;
}

long GetTsaDnFromTimestamp(	GenBuf_t *GenBuf,
					char **TsaDn)
{
TSTInfo_t *TSTInfo		= NULL;

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if(GenBuf == NULL)							{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(GenBuf->buf == NULL)							{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(GenBuf->size == 0)							{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(TsaDn == NULL)							{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (*TsaDn != NULL)							{	BMD_FOK(BMD_ERR_PARAM2);	}

	BMD_FOK(GetTSTInfoFromTimestamp(GenBuf, &TSTInfo));
	if(TSTInfo->tsa == NULL)
	{
		asn_DEF_TSTInfo.free_struct(&asn_DEF_TSTInfo, TSTInfo, 0);
		return BMD_TIMESTAMP_NO_TSA_FIELD;
	}
	BMD_FOK(GeneralName2string(TSTInfo->tsa, TsaDn));

	asn_DEF_TSTInfo.free_struct(&asn_DEF_TSTInfo, TSTInfo, 0);
	return BMD_OK;
}

long GetTsaCertificateSnFromTimestamp(	GenBuf_t *GenBuf,
					char **TsaCertSn,
					long twl_no_of_certificate)
{
Certificate_t *MatchedCert		= NULL;

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if (GenBuf == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (GenBuf->buf == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (GenBuf->size <= 0)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (TsaCertSn == NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (*TsaCertSn != NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}

	BMD_FOK(GetTsaCertificateFromTimestamp(GenBuf, &MatchedCert, twl_no_of_certificate));
	BMD_FOK(GetSerialNumberFromCertificate(MatchedCert, TsaCertSn));

	asn_DEF_Certificate.free_struct(&asn_DEF_Certificate, MatchedCert, 0);

	return BMD_OK;
}

long GetTsaCertificateValidityNAFromTimestamp(GenBuf_t *GenBuf, char **TsaCertValidityNA, long twl_no_of_certificate)
{
Certificate_t *MatchedCert	= NULL;

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if (GenBuf == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (GenBuf->buf == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (GenBuf->size <= 0)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (TsaCertValidityNA == NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (*TsaCertValidityNA != 0)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	BMD_FOK(GetTsaCertificateFromTimestamp(GenBuf, &MatchedCert, twl_no_of_certificate));
	BMD_FOK(GetValidityNAFromCertificate(MatchedCert, TsaCertValidityNA));

	asn_DEF_Certificate.free_struct(&asn_DEF_Certificate, MatchedCert, 0);
	return BMD_OK;
}

/************
 * FAST API *
 ***********/
long GetTSTInfoFromTimestamp(	GenBuf_t *GenBuf,
					TSTInfo_t **TSTInfo)
{
ContentInfo_t *TimeStampToken	= NULL;	/* Token timestampa ukryty w CMS */
SignedData_t *SignedData	= NULL;

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if(GenBuf == NULL)						{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(GenBuf->buf == NULL)						{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(GenBuf->size <= 0)						{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(TSTInfo == NULL)						{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (*TSTInfo != NULL)						{	BMD_FOK(BMD_ERR_PARAM2);	}


	BMD_FOK(GetTimeStampToken_from_GenBuf(GenBuf, &TimeStampToken));
	BMD_FOK(CheckIfCmsIsSignature(TimeStampToken));
	BMD_FOK(get_SignedData_from_CMScontent(&(TimeStampToken->content), &SignedData));

	asn_DEF_ContentInfo.free_struct(&asn_DEF_ContentInfo, TimeStampToken, 0);

	BMD_FOK(CheckIfSignatureIsTimestamp(SignedData));
	BMD_FOK(GetTSTInfo_from_SignedData(SignedData->encapContentInfo.eContent, TSTInfo));

	asn_DEF_SignedData.free_struct(&asn_DEF_SignedData, SignedData, 0);
	return BMD_OK;
}

long GetSetOfCertificatesFromTimestamp(	GenBuf_t *GenBuf,
							Certificate_t ***CertificatesTable,
							long *NoOfCertificates)
{
ContentInfo_t *TimeStampToken	= NULL;	/* Token timestampa ukryty w CMS */
SignedData_t *SignedData	= NULL;

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if(GenBuf == NULL)						{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(GenBuf->buf == NULL)						{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(GenBuf->size <= 0)						{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(CertificatesTable == NULL)					{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (*CertificatesTable != NULL)					{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(NoOfCertificates == NULL)					{	BMD_FOK(BMD_ERR_PARAM3);	}
	if (*NoOfCertificates != 0)				{	BMD_FOK(BMD_ERR_PARAM3);	}


	BMD_FOK(GetTimeStampToken_from_GenBuf(GenBuf, &TimeStampToken));
	BMD_FOK(CheckIfCmsIsSignature(TimeStampToken));
	BMD_FOK(get_SignedData_from_CMScontent(&(TimeStampToken->content), &SignedData));

	asn_DEF_ContentInfo.free_struct(&asn_DEF_ContentInfo, TimeStampToken, 0);

	BMD_FOK(GetCertificatesFromSignedData(SignedData, CertificatesTable, NoOfCertificates));

	asn_DEF_SignedData.free_struct(&asn_DEF_SignedData, SignedData, 0);
	return BMD_OK;
}

long FreeSetOfCertificatesFromTimestamp(	Certificate_t ***CertificatesTable,
							long NoOfCertificates)
{

long i = 0;

	if (*CertificatesTable != NULL)
	{
		for(i=0; i<NoOfCertificates; i++)
		{
			asn_DEF_Certificate.free_struct(&asn_DEF_Certificate, (*CertificatesTable)[i], 0);
		}
		free(*CertificatesTable);
		*CertificatesTable = NULL;
	}
	return BMD_OK;
}

long GetTimeFromTSTInfo(	TSTInfo_t *TSTInfo,
				char **GenerationTime)
{
	if (TSTInfo == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (GenerationTime == NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (*GenerationTime != 0)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	BMD_FOK(GeneralizedTime2string(&(TSTInfo->genTime), GenerationTime));

	return BMD_OK;
}

long GetVersionFromTSTInfo(TSTInfo_t *TSTInfo, long *version)
{
	if (TSTInfo == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (version == NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (*version != 0)			{	BMD_FOK(BMD_ERR_PARAM2);	}

	BMD_FOK(asn_INTEGER2long(&(TSTInfo->version), version));

	return BMD_OK;
}

long GetPolicyOIDFromTSTInfo(TSTInfo_t *TSTInfo, char **policyOID)
{
	if (TSTInfo == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (policyOID == NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (*policyOID != NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}

	BMD_FOK(OBJECT_IDENTIFIER2string(&(TSTInfo->policy), policyOID));

	return BMD_OK;
}

long GetSerialNumberFromTSTInfo(TSTInfo_t *TSTInfo, char **serialNumber)
{
	if (TSTInfo == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (serialNumber == NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (*serialNumber != NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	BMD_FOK(INTEGER2string(&(TSTInfo->serialNumber), serialNumber));

	return BMD_OK;
}


/*****************
 * LOW LEVEL API *
 ****************/
long GetTimeStampResp_from_GenBuf(	GenBuf_t *GenBuf,
						TimeStampResp_t **TimeStampResp)
{
asn_dec_rval_t rval;

	if (GenBuf==NULL)				{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (GenBuf->buf==NULL)				{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (GenBuf->size <= 0)				{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (TimeStampResp==NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (*TimeStampResp != NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}

	rval = ber_decode(0, &asn_DEF_TimeStampResp, (void **)TimeStampResp, GenBuf->buf, GenBuf->size);
	if(rval.code != RC_OK)
	{
		asn_DEF_TimeStampResp.free_struct(&asn_DEF_TimeStampResp, *TimeStampResp, 0);
		BMD_FOK(LIBBMDTS_ERR_TS_DECODE_TSR_DATA);
	}

	return BMD_OK;
}

long GetTimeStampToken_from_GenBuf(	GenBuf_t *GenBuf,
						ContentInfo_t **ContentInfo)
{

	if (GenBuf==NULL)				{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (GenBuf->buf==NULL)				{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (GenBuf->size == 0)				{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (ContentInfo == NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (*ContentInfo != NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}

	BMD_FOK(get_CMScontent_from_GenBuf(GenBuf, ContentInfo));

	return BMD_OK;
}

long CheckIfCmsIsSignature(	ContentInfo_t *ContentInfo)
{
long *signature_oid			= NULL;
char *signature_oid_str			= NULL;
long signature_oid_size			= 0;
char OID_id_signedData[] 		= {OID_CMS_ID_SIGNED_DATA};

	if (ContentInfo == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}

	signature_oid_size = OID_to_ulong_alloc2(&(ContentInfo->contentType), &signature_oid);
	if(signature_oid == NULL) 		{	BMD_FOK(ERR_NO_MEMORY);	}

	ulong_to_str_of_OID2(	signature_oid, signature_oid_size, &signature_oid_str);

	if(strcmp(OID_id_signedData, signature_oid_str)!=0)
	{
		free(signature_oid); signature_oid=NULL;
		free(signature_oid_str); signature_oid_str = NULL;
		BMD_FOK(LIBBMDTS_ERR_TS_SIGNATURE_NOT_PRESENT_IN_CMS);
	}

	free(signature_oid); signature_oid=NULL;
	free(signature_oid_str); signature_oid_str = NULL;

	return BMD_OK;
}

long CheckIfSignatureIsTimestamp(	SignedData_t *SignedData)
{
long *TSTInfo_oid			= NULL;
long TSTInfo_oid_size			= 0;
long OID_id_ct_TSTInfo[] 		= {OID_CMS_ID_CT_TSTINFO};

	if (SignedData == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}

	TSTInfo_oid_size = OID_to_ulong_alloc2(&(SignedData->encapContentInfo.eContentType), &TSTInfo_oid);
	if(TSTInfo_oid == NULL) 		{	BMD_FOK(ERR_NO_MEMORY);	}

	if(OID_cmp2(OID_id_ct_TSTInfo, sizeof(OID_id_ct_TSTInfo),TSTInfo_oid, TSTInfo_oid_size * sizeof(long)))
	{
		free(TSTInfo_oid); TSTInfo_oid=NULL;
		BMD_FOK(LIBBMDTS_ERR_TS_TIMESTAMP_NOT_PRESENT_IN_SIGNATURE);
	}
	free(TSTInfo_oid); TSTInfo_oid=NULL;
	return BMD_OK;
}

long GetTSTInfo_from_SignedData(	OCTET_STRING_t *eContent,
						TSTInfo_t **TSTInfo)
{
asn_dec_rval_t rval;

	if (eContent == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (eContent->buf == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (eContent->size <= 0)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (TSTInfo == NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (*TSTInfo != NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}

	rval = ber_decode(0, &asn_DEF_TSTInfo, (void **)TSTInfo, eContent->buf, eContent->size);
	if(rval.code != RC_OK)
	{
		asn_DEF_TSTInfo.free_struct(&asn_DEF_TSTInfo, *TSTInfo, 0);
		BMD_FOK(LIBBMDTS_ERR_TS_DECODE_TSTINFO);
	}

	return BMD_OK;
}

long GetCertificatesFromSignedData(	SignedData_t *SignedData,
						Certificate_t ***CertificatesTable,
						long *NoOfCertificates)
{
long a		= 0;
long counter	= 0;

	if (SignedData == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (CertificatesTable == NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (*CertificatesTable != NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (NoOfCertificates == NULL)		{	BMD_FOK(BMD_ERR_PARAM3);	}
	if (*NoOfCertificates != 0)		{	BMD_FOK(BMD_ERR_PARAM3);	}

	/* brak certyfikatów dot znacznika */
	if(SignedData->certificates == NULL)
	{
		*NoOfCertificates = 0;
		BMD_FOK(LIBBMDTS_ERR_TS_NO_CERTIFICATES_PRESENT_IN_SIGNED_DATA);
	}
	/* zlicz ilosc certyfikatow X509, inne nas nie obchodza */
	for(a=0; a<SignedData->certificates->list.count; a++)
	{
		if (SignedData->certificates->list.array[a]->present == CertificateChoices_PR_certificate)
		{
			(*NoOfCertificates)++;
		}
	}
	/* brak certyfikatow X509 */
	if (*NoOfCertificates == 0) 		{	BMD_FOK(LIBBMDTS_ERR_TS_NO_x509_CERTIFICATES_PRESENT_IN_SIGNED_DATA);	}

	/* zaalokuj pamiec na certyfikaty */
	*CertificatesTable = (Certificate_t **) malloc (sizeof(Certificate_t *) * (*NoOfCertificates));
	if (*CertificatesTable == NULL)	{	BMD_FOK(NO_MEMORY);	}
	/* przekopiuj struktury certyfikatow do tablicy */
	for(a=0; a<SignedData->certificates->list.count; a++)
	{
		if (SignedData->certificates->list.array[a]->present == CertificateChoices_PR_certificate)
		{
			(*CertificatesTable)[counter] = NULL;
			BMD_FOK(asn_cloneContent(&asn_DEF_Certificate,(void *)&(SignedData->certificates->list.array[a]->choice.certificate),(void **)&(*CertificatesTable)[counter]));
			counter++;
		}
	}

	return BMD_OK;
}

long CompareTimestampTSADNWithTSACertificateDN(	char *TsaDn,
								Certificate_t **CertificatesTable,
								long NoOfCertificates,
								long *NoOfMatchedCert,
								Certificate_t **MatchedCertificate)
{
long i = 0;
char *CertificateSubjectDn = NULL;

	*NoOfMatchedCert = -1;
	for(i=0; i<NoOfCertificates; i++)
	{
		BMD_FOK(Name2string(&(CertificatesTable[i]->tbsCertificate.subject), &CertificateSubjectDn));

		if (!strcmp(TsaDn, CertificateSubjectDn))
		{
			free(CertificateSubjectDn); CertificateSubjectDn = NULL;

			*NoOfMatchedCert = i;
			BMD_FOK(asn_cloneContent(&asn_DEF_Certificate, (void *)CertificatesTable[i], (void **)MatchedCertificate));
			return 0;
		}
		free(CertificateSubjectDn); CertificateSubjectDn = NULL;
	}
	BMD_FOK(ERR_NO_CERTIFICATES_MATCHED_BY_THEIR_DN);

	return BMD_OK;
}

long GetTsaCertificateFromTimestamp(	GenBuf_t *GenBuf,
					Certificate_t **TsaCertificate,
					long twl_no_of_certificate)
{
long err					= 0;
TSTInfo_t *TSTInfo			= NULL;
Certificate_t **CertificatesTable	= NULL;
long NoOfCertificates			= 0;
long NoOfMatchedCert			= 0;
char *TsaDn				= NULL;

	if (GenBuf==NULL)				{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (GenBuf->buf==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (GenBuf->size == 0)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (TsaCertificate == NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (*TsaCertificate != NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	BMD_FOK(GetTSTInfoFromTimestamp(GenBuf, &TSTInfo));

	/*BMD_FOK(GeneralName2string(TSTInfo->tsa, &TsaDn));

	asn_DEF_TSTInfo.free_struct(&asn_DEF_TSTInfo, TSTInfo, 0);
	BMD_FOK(GetSetOfCertificatesFromTimestamp(GenBuf, &CertificatesTable, &NoOfCertificates));

	 UWAGA!!!
	Sposrod dostepnych w kopercie certyfikatow wybieram ten, ktory jako pierwszy bedzie mial
	taka sama nazwe wlasciciela DN jak nazwa DN obecna w znacziku czasu (TSTinfo). Nie sprawdzam
	czy za pomoca umiesczonego w nim klucza publicznego mozna zweryfikowac podpis na znaczniku!!!
	Jest to ewidentne TODO w przypadku gdy do timestampowania bedziemy uzywac znacznikow z niezaufanych
	dostawcow (TSA) - w przypadku timestampa Certum zakladam ze nikt nie bedzie kombinowal.
	
	err=CompareTimestampTSADNWithTSACertificateDN(TsaDn, CertificatesTable, NoOfCertificates, &NoOfMatchedCert, TsaCertificate);
	if(err < 0)
	{
		BMD_FOK(err);
	}
	if(err>0)
	{
		PRINT_ERROR("LIBBMDTSERR[%s:%i:%s] Error in comparing TSA DN from TSTInfo and all Certificates present in set. "
			"Received error code = %li\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}

	BMD_FOK(FreeSetOfCertificatesFromTimestamp(&CertificatesTable, NoOfCertificates));
	free(TsaDn); TsaDn=NULL;*/



	if(TSTInfo->tsa != NULL)
	{/*if(TSTInfo->tsa != NULL)*/
		BMD_FOK(GeneralName2string(TSTInfo->tsa, &TsaDn));
		asn_DEF_TSTInfo.free_struct(&asn_DEF_TSTInfo, TSTInfo, 0);
		BMD_FOK( GetSetOfCertificatesFromTimestamp(GenBuf, &CertificatesTable, &NoOfCertificates));
		/**** UWAGA!!!
		Sposrod dostepnych w kopercie certyfikatow wybieram ten, ktory jako pierwszy bedzie mial
		taka sama nazwe wlasciciela DN jak nazwa DN obecna w znacziku czasu (TSTinfo). Nie sprawdzam
		czy za pomoca umiesczonego w nim klucza publicznego mozna zweryfikowac podpis na znaczniku!!!
		Jest to ewidentne TODO w przypadku gdy do timestampowania bedziemy uzywac znacznikow z niezaufanych
		dostawcow (TSA) - w przypadku timestampa Certum zakladam ze nikt nie bedzie kombinowal.
		****/
		err = CompareTimestampTSADNWithTSACertificateDN(TsaDn, CertificatesTable,
					NoOfCertificates, &NoOfMatchedCert, TsaCertificate);
		if(err < 0)
		{
			BMD_FOK(err);
		}
		if(err>0)
		{
			PRINT_ERROR("LIBBMDTSERR[%s:%i:%s] Error in comparing TSA DN from TSTInfo and all Certificates present in set. "
				"Received error code = %li\n",
				__FILE__, __LINE__, __FUNCTION__, err);
			return err;
		}
		BMD_FOK(FreeSetOfCertificatesFromTimestamp(&CertificatesTable, NoOfCertificates));
		free(TsaDn); TsaDn=NULL;
	}/*\if(TSTInfo->tsa != NULL)*/
	else
	{/*else(TSTInfo->tsa != NULL)*/
		asn_DEF_TSTInfo.free_struct(&asn_DEF_TSTInfo, TSTInfo, 0);
		BMD_FOK(GetSetOfCertificatesFromTimestamp(GenBuf, &CertificatesTable, &NoOfCertificates));

		/*Na wypadek gdyby w znaczniku czasu nie bylo pola tsa ale bylo dolaczone kilka certyfikatow.*/
		if( (NoOfCertificates > 1) && ( twl_no_of_certificate <= 0 || twl_no_of_certificate > NoOfCertificates) )
		{
			BMD_FOK(BMD_TIMESTAMP_NO_TSA_FIELD);
		}

		if(NoOfCertificates > 1)
		{
			PRINT_WARNING("LIBBMDTSWARN Warning! No tsa field in the timestamp. Assuming %li certificate found is tsa certificate.\n",
			twl_no_of_certificate);
			err = asn_cloneContent(&asn_DEF_Certificate, (void *)CertificatesTable[twl_no_of_certificate - 1], (void **)TsaCertificate);
		}
		else
		{
			PRINT_WARNING("LIBBMDTSWARN Warning! No tsa field in the timestamp. Assuming first certificate found is tsa certificate.\n");
			err = asn_cloneContent(&asn_DEF_Certificate, (void *)CertificatesTable[0], (void **)TsaCertificate);
		}
		if(err<0)
		{
			PRINT_ERROR("LIBBMDTSERR[%s:%i:%s] Error in cloning certificate. "
				"Received error code = %li.\n",
				__FILE__, __LINE__, __FUNCTION__, err);
			return LIBBMDTS_ERR_TS_CLONING_CERTIFICATE;
		}
		BMD_FOK(FreeSetOfCertificatesFromTimestamp(&CertificatesTable, NoOfCertificates));
	}/*\else(TSTInfo->tsa != NULL)*/

	return BMD_OK;
}

/* TimeStampReq
typedef struct TimeStampReq {
	INTEGER_t	 version;
	MessageImprint_t	 messageImprint;
	TSAPolicyId_t	*reqPolicy;
	INTEGER_t	*nonce;
	BOOLEAN_t	*certReq;
	struct Extensions	*extensions;

	asn_struct_ctx_t _asn_ctx;
} TimeStampReq_t;
*/

long _bmd_create_ts_request(GenBuf_t *hash,long hash_algo,GenBuf_t **output)
{
	TimeStampReq_t *ts_req=NULL;
	long OID_sha[]={1,3,14,3,2,26};
	BOOLEAN_t *one=NULL;
	if (hash==NULL)				{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (hash->buf==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (hash->size<=0)				{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (hash_algo!=BMD_HASH_ALGO_SHA1)	{	BMD_FOK(BMD_ERR_UNIMPLEMENTED);	}
	if (output==NULL)				{	BMD_FOK(BMD_ERR_PARAM3);	}
	if ((*output)!=NULL)			{	BMD_FOK(BMD_ERR_PARAM3);	}
	if (hash_algo==BMD_HASH_ALGO_SHA1)
		if(hash->size!=BMD_HASH_SHA1_SIZE)
			return BMD_ERR_FORMAT;
	one=(BOOLEAN_t *)malloc(sizeof(BOOLEAN_t));
	if(one==NULL)				{	BMD_FOK(BMD_ERR_MEMORY);	}
	*one=1;
	ts_req=(TimeStampReq_t *)malloc(sizeof(TimeStampReq_t));
	if(ts_req==NULL)				{	BMD_FOK(BMD_ERR_MEMORY);	}

	memset(ts_req,0,sizeof(TimeStampReq_t));
	/* ustawienie wersji requesta */
	BMD_FOK(asn_long2INTEGER(&(ts_req->version),TimeStampReq__version_v1));
	/* ustawienie OID'u SHA1 */
	BMD_FOK(OBJECT_IDENTIFIER_set_arcs(&(ts_req->messageImprint.hashAlgorithm.algorithm),OID_sha,sizeof(long),
								   sizeof(OID_sha)/sizeof(long)));
	/* ustawienie na sztywno braku parametrow */
	ts_req->messageImprint.hashAlgorithm.parameters=(ANY_t *)malloc(sizeof(ANY_t));
	if ( ts_req->messageImprint.hashAlgorithm.parameters == NULL )
	{	BMD_FOK(BMD_ERR_MEMORY);	}

	memset(ts_req->messageImprint.hashAlgorithm.parameters,0,sizeof(ANY_t));
	ts_req->messageImprint.hashAlgorithm.parameters->buf=(uint8_t *)malloc(2 * sizeof(uint8_t));
	if ( ts_req->messageImprint.hashAlgorithm.parameters->buf == NULL )
	{	BMD_FOK(BMD_ERR_MEMORY);	}

	ts_req->messageImprint.hashAlgorithm.parameters->buf[0]=0x05;
	ts_req->messageImprint.hashAlgorithm.parameters->buf[1]=0x00;
	ts_req->messageImprint.hashAlgorithm.parameters->size=2;
	/* ustawienie wartosci HASH */
	BMD_FOK(OCTET_STRING_fromBuf( &(ts_req->messageImprint.hashedMessage),(char *)hash->buf, hash->size));
	ts_req->certReq=one;
	BMD_FOK(asn1_encode(&asn_DEF_TimeStampReq,ts_req, NULL, output));
	asn_DEF_TimeStampReq.free_struct(&asn_DEF_TimeStampReq,ts_req,0);ts_req=NULL;
	return BMD_OK;
}


/**-----uzupelnienie do powyzszych funkcji (by WSZ)   -------------------------------------------*/
/*zawartosc failure_info = -1 jesli nie ma pola FailureInfo w TimeStampResp*/
/*pobiera info bledu ze struktury TimeStampResp_t*/
long Get_TSResp_failInfo(TimeStampResp_t *ts_resp, long *failure_info)
{
	BIT_STRING_t *info=NULL;

	if (ts_resp == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (failure_info == NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	info=ts_resp->status.failInfo;
	if (info == NULL)
	{
		*failure_info=-1;
		return BMD_OK;
	}
	//4 bajty , 6 nie wykorzystanych
	if (info->size != 4)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (info->bits_unused != 6)		{	BMD_FOK(BMD_ERR_PARAM1);	}

	if(info->buf[3] != 0)
	{
		if(info->buf[3] == 64)
		{
			*failure_info=PKIFailureInfo_badAlg;
			return BMD_OK;
		}
		else					{	BMD_FOK(BMD_ERR_PARAM1);	}
	}
	else
	{
		if(info->buf[2] != 0)
		{
			switch(info->buf[2])
			{
				case 1:
				{
					*failure_info=PKIFailureInfo_badRequest;
					return BMD_OK;
				}
				case 8:
				{
					*failure_info=PKIFailureInfo_badDataFormat;
					return BMD_OK;
				}
				default:
					{	BMD_FOK(BMD_ERR_PARAM1);	}
			}
		}
		else
		{
			if(info->buf[1] != 0)
			{
				switch(info->buf[1])
				{
					case 16 :
					{
						*failure_info=PKIFailureInfo_timeNotAvailable;
						return BMD_OK;
					}
					case 32 :
					{
						*failure_info=PKIFailureInfo_unacceptedPolicy;
						return BMD_OK;
					}
					case 64 :
					{
						*failure_info=PKIFailureInfo_unacceptedExtension;
						return BMD_OK;
					}
					case 128:
					{
						*failure_info=PKIFailureInfo_addInfoNotAvailable;
						return BMD_OK;
					}
					default :
						{	BMD_FOK(BMD_ERR_PARAM1);	}
				}
			}
			else
			{
				if(info->buf[0] != 0)
				{
					if(info->buf[0] == 128)
					{
						*failure_info=PKIFailureInfo_systemFailure;
						return BMD_OK;
					}
					else
						{ return BMD_ERR_PARAM1; }
				}
				//zaden bit nie jest zapalony, a tak nie moze byc
				else
					{ return BMD_ERR_PARAM1; }
			}
		}
	}

	return BMD_OK; //Visual tego wymaga
}


/*jesli zawartosc failure_info == -1, to oznacza brak PKIFailureInfo w odpowiedzi znacznika czasu */
/*pobiera info bledu z odpowiedzi serwera znacznika czasu przechowaywanej w GenBuf*/
long Get_GenBuf_failInfo(	GenBuf_t *resp_buf,
					long *failure_info)
{
TimeStampResp_t *ts_resp=NULL;
asn_dec_rval_t dec_result;
long ret_val;

	if (resp_buf == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (resp_buf->buf == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (failure_info == NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	dec_result=ber_decode(0, &asn_DEF_TimeStampResp, (void**)&ts_resp, resp_buf->buf, resp_buf->size);
	if(dec_result.code != RC_OK)
	{
		asn_DEF_TimeStampResp.free_struct(&asn_DEF_TimeStampResp, ts_resp, 1);
		free(ts_resp);
		BMD_FOK(BMD_ERR_OP_FAILED);
	}

	ret_val=Get_TSResp_failInfo(ts_resp, failure_info);
	asn_DEF_TimeStampResp.free_struct(&asn_DEF_TimeStampResp, ts_resp, 1);
	free(ts_resp);
	if(ret_val != BMD_OK)
	{
		BMD_FOK(BMD_ERR_OP_FAILED);
	}

	return BMD_OK;
}

/*pobiera status odpowiedzi znacznika czasu przechowywanej w strukturze TimeStampResp_t */
long Get_TSResp_status(	TimeStampResp_t *ts_resp,
				long *ts_resp_status)
{
long tmp=0;
long ret_val=0;

	if (ts_resp == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (ts_resp_status == NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	ret_val=asn_INTEGER2long( &(ts_resp->status.status), &tmp);
	if(ret_val != 0)
	{
		BMD_FOK(BMD_ERR_OP_FAILED);
	}

	*ts_resp_status=tmp;
	return BMD_OK;
}

/*pobiera status odpowiedzi znacznika czasu przechowywanej w GenBuf*/
long Get_GenBuf_status(	GenBuf_t *resp_buf,
				long *ts_resp_status)
{
TimeStampResp_t *ts_resp=NULL;
asn_dec_rval_t dec_result;
long ret_val;

	if (resp_buf == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (resp_buf->buf == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (ts_resp_status == NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	dec_result=ber_decode(0, &asn_DEF_TimeStampResp, (void**)&ts_resp, resp_buf->buf, resp_buf->size);
	if(dec_result.code != RC_OK)
	{
		asn_DEF_TimeStampResp.free_struct(&asn_DEF_TimeStampResp, ts_resp, 1);
		free(ts_resp);
		return BMD_ERR_OP_FAILED;
	}

	ret_val=Get_TSResp_status(ts_resp, ts_resp_status);
	asn_DEF_TimeStampResp.free_struct(&asn_DEF_TimeStampResp, ts_resp, 1);
	free(ts_resp);
	if(ret_val != BMD_OK)
	{
		return BMD_ERR_OP_FAILED;
	}

	return BMD_OK;
}


/**
-----------
wydajne One Call API (by WSZ)
wewnatrz funkcji dekodowanie GenBufa wykonywane jest tylko raz;
w przypadku API by Szu, kazde odwolanie do skladowej wymagalo dekodowania
-----------
*/

//jesli zadanie zostalo odrzucone to BMD_OK a *features = NULL
//jesli ok, to BMD_OK i *features != NULL
//jesli cos calkiem padlo, to != BMD_OK i *features = NULL
long Parse_TimeStampResponse(	GenBuf_t* ts_resp,
					TSRespFeatures_t** features)
{
long ret_val=0;
TimeStampResp_t *resp=NULL;
SignedData_t  *signedData=NULL;
TSTInfo_t *tst=NULL;
long status=0;
asn_dec_rval_t dec_result;
TSRespFeatures_t *zawartosc=NULL;

	if (ts_resp == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (features == NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (*features != NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}


	dec_result=ber_decode(0, &asn_DEF_TimeStampResp, (void**)&resp, ts_resp->buf, ts_resp->size);
	if(dec_result.code != RC_OK)
	{

		asn_DEF_TimeStampResp.free_struct(&asn_DEF_TimeStampResp, resp, 1);
		free(resp);
		BMD_FOK(BMD_ERR_OP_FAILED);
	}

	ret_val=Get_TSResp_status(resp, &status);
	if(ret_val != BMD_OK )
	{
		asn_DEF_TimeStampResp.free_struct(&asn_DEF_TimeStampResp, resp, 1);
		free(resp);
		BMD_FOK(BMD_ERR_OP_FAILED);
	}

	//jesli zadanie zostalo odrzucone
	if(status > 1)
	{
		asn_DEF_TimeStampResp.free_struct(&asn_DEF_TimeStampResp, resp, 1);
		free(resp);
		return BMD_OK;
	}
	/*----------*/

	ret_val=CheckIfCmsIsSignature(resp->timeStampToken);
	if(ret_val != 0)
	{
		asn_DEF_TimeStampResp.free_struct(&asn_DEF_TimeStampResp, resp, 1);
		free(resp);
		BMD_FOK(BMD_ERR_OP_FAILED);
	}

	ret_val=ANY_to_type( &(resp->timeStampToken->content), &asn_DEF_SignedData, (void**)&signedData);
	if(ret_val != 0)
	{
		asn_DEF_TimeStampResp.free_struct(&asn_DEF_TimeStampResp, resp, 1);
		free(resp);
		BMD_FOK(BMD_ERR_OP_FAILED);
	}

	asn_DEF_TimeStampResp.free_struct(&asn_DEF_TimeStampResp, resp, 1);
	free(resp);


	ret_val=CheckIfSignatureIsTimestamp(signedData);
	if(ret_val != 0)
	{
		asn_DEF_SignedData.free_struct(&asn_DEF_SignedData, signedData, 1);
		free(signedData);
		BMD_FOK(BMD_ERR_OP_FAILED);
	}

	ret_val=GetTSTInfo_from_SignedData(signedData->encapContentInfo.eContent, &tst);
	if(ret_val != 0)
	{
		asn_DEF_SignedData.free_struct(&asn_DEF_SignedData, signedData, 1);
		free(signedData);
		BMD_FOK(BMD_ERR_OP_FAILED);
	}

	asn_DEF_SignedData.free_struct(&asn_DEF_SignedData, signedData, 1);
	free(signedData); //to juz niepotrzebne


	//tworzenie zawartosci
	zawartosc=(TSRespFeatures_t *)calloc(1, sizeof(TSRespFeatures_t));
	if(zawartosc == NULL)
	{
		asn_DEF_TSTInfo.free_struct(&asn_DEF_TSTInfo, tst, 1);
		free(tst);
		BMD_FOK(BMD_ERR_MEMORY);
	}

	//wersja
	ret_val=GetVersionFromTSTInfo(tst, &(zawartosc->version));
	if(ret_val != 0)
	{
		Destroy_TSRespFeatures(&zawartosc);
		asn_DEF_TSTInfo.free_struct(&asn_DEF_TSTInfo, tst, 1);
		free(tst);
		BMD_FOK(BMD_ERR_OP_FAILED);
	}

	//oid polityki
	ret_val=GetPolicyOIDFromTSTInfo(tst, &(zawartosc->TSA_policyId));
	if(ret_val != 0)
	{
		Destroy_TSRespFeatures(&zawartosc);
		asn_DEF_TSTInfo.free_struct(&asn_DEF_TSTInfo, tst, 1);
		free(tst);
		BMD_FOK(BMD_ERR_OP_FAILED);
	}

	//oid algorytmu hashujacego
	ret_val=OBJECT_IDENTIFIER2string( &(tst->messageImprint.hashAlgorithm.algorithm), &(zawartosc->hash_alg_oid) );
	if(ret_val != 0)
	{
		Destroy_TSRespFeatures(&zawartosc);
		asn_DEF_TSTInfo.free_struct(&asn_DEF_TSTInfo, tst, 1);
		free(tst);
		BMD_FOK(BMD_ERR_OP_FAILED);
	}

	//wartosc skrotu
	ret_val=asn_cloneContent(&asn_DEF_OCTET_STRING, &(tst->messageImprint.hashedMessage), (void**)&(zawartosc->hash_val));
	if(ret_val != 0)
	{
		Destroy_TSRespFeatures(&zawartosc);
		asn_DEF_TSTInfo.free_struct(&asn_DEF_TSTInfo, tst, 1);
		free(tst);
		BMD_FOK(BMD_ERR_OP_FAILED);
	}

	//serial znacznika czasu
	ret_val=asn_INTEGER2long( &(tst->serialNumber), &(zawartosc->serial));
	if(ret_val != 0)
	{
		Destroy_TSRespFeatures(&zawartosc);
		asn_DEF_TSTInfo.free_struct(&asn_DEF_TSTInfo, tst, 1);
		free(tst);
		BMD_FOK(BMD_ERR_OP_FAILED);
	}

	//czas wygenerowania znacznika
	(zawartosc->genTime)=asn_GT2time( &(tst->genTime), NULL, 0);
	if(zawartosc->genTime == -1)
	{
		Destroy_TSRespFeatures(&zawartosc);
		asn_DEF_TSTInfo.free_struct(&asn_DEF_TSTInfo, tst, 1);
		free(tst);
		BMD_FOK(BMD_ERR_OP_FAILED);
	}

	//nazwa TSA
	ret_val=GeneralName2string(tst->tsa, &(zawartosc->tsa) );
	if(ret_val != 0)
	{
		Destroy_TSRespFeatures(&zawartosc);
		asn_DEF_TSTInfo.free_struct(&asn_DEF_TSTInfo, tst, 1);
		free(tst);
		BMD_FOK(BMD_ERR_OP_FAILED);
	}
	//nonce
	/*if(tst->nonce != NULL)
	{
		ret_val=asn_INTEGER2long(tst->nonce, &(zawartosc->nonce) );
		if(ret_val != 0)
		{
			Destroy_TSRespFeatures(&zawartosc);
			asn_DEF_TSTInfo.free_struct(&asn_DEF_TSTInfo, tst, 1);
			free(tst);
			return BMD_ERR_OP_FAILED;
		}
	}
	else
	{
		zawartosc->nonce=-1; //!!!!!
	}*/
	asn_DEF_TSTInfo.free_struct(&asn_DEF_TSTInfo, tst, 1);
	free(tst);
	*features=zawartosc;

	return BMD_OK;
}

/**funkcyjka do czyszczenia zawartosci struktury otrzymanej w wyniku parsowania odpowiedzi znacznika czasu*/
long Destroy_TSRespFeatures(	TSRespFeatures_t **tsresp_features)
{
TSRespFeatures_t *res_ptr=NULL;

	if(tsresp_features == NULL)		{	return BMD_OK;	}

	res_ptr=*tsresp_features;
	if(res_ptr == NULL)			{	return BMD_OK;	}

	free(res_ptr->TSA_policyId);
	free(res_ptr->hash_alg_oid);
	free(res_ptr->tsa);
	asn_DEF_OCTET_STRING.free_struct(&asn_DEF_OCTET_STRING, res_ptr->hash_val, 1);
	free(res_ptr->hash_val);
	free(*tsresp_features);
	*tsresp_features=NULL;

	return BMD_OK;
}
