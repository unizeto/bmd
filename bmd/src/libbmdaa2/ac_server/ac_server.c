#include <bmd/libbmdaa2/ac_server/ac_server.h>
#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdglobals/libbmdglobals.h>

#ifndef WIN32
#include <sys/file.h>
#endif

#ifdef WIN32
	#ifndef open
		#define open _open
	#endif
	#ifndef read
		#define read _read
	#endif
	#ifndef close
		#define close _close
	#endif
	#ifndef write
		#define write _write
	#endif
#pragma warning(disable:4100)
#endif

long ST_GenAttributeCert(	GenBuf_t *X509Certificate,
				certificate_attributes_t **UserAttributes,
				ConfigData_t *ConfigDataStruct)
{
char *Issuer						= NULL;
char *Serial						= NULL;
Certificate_t *Certificate				= NULL;
asn_dec_rval_t rval;

	PRINT_INFO("LIBBMDAA2INF Generating attribute certificate\n");

	/************************/
	/* walidacja parametrow */
	/************************/
	if (X509Certificate == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (X509Certificate->buf == NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (X509Certificate->size < 1)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (UserAttributes == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (ConfigDataStruct == NULL)		{	BMD_FOK(BMD_ERR_PARAM3);	}

	rval=ber_decode(NULL,&asn_DEF_Certificate,(void **)&Certificate, X509Certificate->buf,X509Certificate->size);
	if( rval.code != RC_OK )		{	BMD_FOK(BMD_ERR_OP_FAILED);	}

	BMD_FOK(GetSerialNumberFromCertificate(Certificate,&Serial));
	BMD_FOK(GetIssuerDNFromCertificate(Certificate,&Issuer));

	PRINT_INFO("LIBBMDAA2INF X509 Issuer: %s\n", Issuer);
	PRINT_INFO("LIBBMDAA2INF X509 Serial: %s\n", Serial);

	PRINT_INFO("LIBBMDAA2INF Reading attributes from database\n");
 	BMD_FOK(ST_ReadAttributesFromDatabase(Serial, Issuer, ConfigDataStruct, UserAttributes));

	/************/
	/* porzadki */
	/************/
	free0(Issuer);
	free0(Serial);

	return BMD_OK;
}



int increment_buf(GenBuf_t *buf,unsigned char **output,long *len);
int AA_SetSerial(const char *file,AttributeCertificate_t *AC);

/* wejscie - DER(X509), wyjscie - DER(AC) */
int AA_GenAttributeCert(	GenBuf_t *X509Certificate,
					GenBuf_t **AttrCertificate,
					ConfigData_t *ConfigDataStruct)
{
int status							= 0;
char *Issuer						= NULL;
char *Serial						= NULL;
AttributesSequence_t *AttributesSequence		= NULL;
AttributeCertificate_t *AttributeCertificate	= NULL;
Certificate_t *Certificate				= NULL;
asn_dec_rval_t rval;

	PRINT_INFO("LIBBMDAA2INF Generating attribute certificate\n");

	/************************/
	/* walidacja parametrow */
	/************************/
	if (X509Certificate == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (X509Certificate->buf == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (X509Certificate->size < 1)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (AttrCertificate == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (*AttrCertificate != NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (ConfigDataStruct == NULL)			{	BMD_FOK(BMD_ERR_PARAM3);	}

	/****************************************************************/
	/* zdekodowanie certyfikatu i wydobycie podstawowych informacji */
	/****************************************************************/
	rval=ber_decode(NULL,&asn_DEF_Certificate,(void **)&Certificate, X509Certificate->buf,X509Certificate->size);
	if( rval.code != RC_OK )	{	BMD_FOK(BMD_ERR_OP_FAILED);	}
	BMD_FOK(GetSerialNumberFromCertificate(Certificate,&Serial));
	BMD_FOK(GetIssuerDNFromCertificate(Certificate,&Issuer));

	PRINT_INFO("LIBBMDAA2INF X509 Issuer: %s\n", Issuer);
	PRINT_INFO("LIBBMDAA2INF X509 Serial: %s\n", Serial);

	/***********************************************************************************/
	/* odczytwanie atrybutów certyfikatu z bazy danych na podstawie seriala i wystawcy */
	/***********************************************************************************/
	PRINT_INFO("LIBBMDAA2INF Reading attributes from database\n");
 	BMD_FOK(AA_ReadAttributesFromDatabase(Serial, Issuer, ConfigDataStruct, &AttributesSequence));

	AttributeCertificate=(AttributeCertificate_t *)malloc(sizeof(AttributeCertificate_t));
	if( AttributeCertificate == NULL )	{	BMD_FOK(BMD_ERR_MEMORY);	}

	memset(AttributeCertificate,0,sizeof(AttributeCertificate_t));

	BMD_FOK(asn_long2INTEGER(&(AttributeCertificate->acinfo.version), ConfigDataStruct->ACConfiguration->ACVersionSection->ac_version));
	BMD_FOK(AA_Create_baseCertificateID(Certificate,ConfigDataStruct, &(AttributeCertificate->acinfo.holder.baseCertificateID)));
	BMD_FOK(AA_AttributeCertificate_set_Issuer(ConfigDataStruct, AttributeCertificate));
	BMD_FOK(AA_AttributeCertificate_set_signatureAlgorithmIdentifier(ConfigDataStruct, AttributeCertificate));
	BMD_FOK(AA_SetSerial(ConfigDataStruct->ACConfiguration->ACSerialNumberSection->serial_number_filename, AttributeCertificate));
	BMD_FOK(AA_AttributeCertificate_set_attrCertValidityPeriod(ConfigDataStruct, AttributeCertificate));
	BMD_FOK(AA_AttributeCertificate_set_attributes(ConfigDataStruct, AttributesSequence, AttributeCertificate));
	BMD_FOK(AA_AttributeCertificate_set_signatureValue(ConfigDataStruct, AttributeCertificate));

	/****************************/
	/* wyświetlenie certyfikatu */
	/****************************/
	if ((_GLOBAL_debug_level>=7) || (_GLOBAL_debug_level==-4))
	{
		BMD_FOK(asn1_encode(&asn_DEF_AttributeCertificate,AttributeCertificate, NULL, AttrCertificate));
		asn_DEF_Certificate.free_struct(&asn_DEF_Certificate,Certificate,0);Certificate=NULL;

		asn_fprint(stdout, &asn_DEF_AttributeCertificate, AttributeCertificate);

		asn_DEF_AttributeCertificate.free_struct(&asn_DEF_AttributeCertificate,AttributeCertificate,0);
		AttributeCertificate=NULL;
	}

	free0(Issuer);free0(Serial);
	free(AttributesSequence); AttributesSequence = NULL;

	return status;
}

/* zwieksza o jeden wielkosc bufora z uzyciem BN */
int increment_buf(	GenBuf_t *buf,
				unsigned char **output,
				long *len)
{
BIGNUM *serial_bn		= NULL;
BIGNUM *one_bn		= NULL;
BIGNUM *result_bn		= NULL;
int status			= 0;

	if( buf->size == 0 )
	{
		serial_bn=BN_new();
		if( serial_bn == NULL )		{	BMD_FOK(BMD_ERR_OP_FAILED);	}
		status=BN_zero(serial_bn);
		if( status != 1 )			{	BMD_FOK(BMD_ERR_OP_FAILED);	}
	}
	else
	{
		serial_bn=BN_bin2bn((unsigned char*)buf->buf, buf->size, NULL);
		if( serial_bn == NULL )		{	BMD_FOK(BMD_ERR_OP_FAILED);	}
	}
	one_bn=BN_new();
	if( one_bn == NULL )		{	BMD_FOK(BMD_ERR_OP_FAILED);	}

	status=BN_one(one_bn);
	if( status != 1 )     		{	BMD_FOK(BMD_ERR_OP_FAILED);	}

	result_bn=BN_new();
	if( result_bn == NULL )		{	BMD_FOK(BMD_ERR_OP_FAILED);	}

	status=BN_add(result_bn,serial_bn,one_bn);
	if( status != 1 )			{	BMD_FOK(BMD_ERR_OP_FAILED);	}

	BN_free(serial_bn);serial_bn=NULL;
	BN_free(one_bn);one_bn=NULL;
	*len=(long)BN_num_bytes(result_bn);
	if( (*len) <= 0 )			{	BMD_FOK(BMD_ERR_OP_FAILED);	}

	(*output)=(unsigned char *)malloc(*len);
	if( (*output) == NULL )		{	BMD_FOK(BMD_ERR_OP_FAILED);	}

	memset((*output),0,*len);
	status=BN_bn2bin(result_bn,(*output));
	if( status != (*len) )		{	BMD_FOK(BMD_ERR_OP_FAILED);	}

	(*len)=status;
	BN_free(result_bn);result_bn=NULL;

	return BMD_OK;
}

/* blokuje plik, wczytuje zawartosc, zwieksza o jeden,
   zwiekszony umieszcze w AC, zamyka plik, sciaga blokade*/
int AA_SetSerial(const char *file,AttributeCertificate_t *AC)
{
int fd			= 0;
int status			= 0;
long len			= 0;
GenBuf_t *buf		= NULL;
unsigned char *serial_buf=NULL;

	if( file == NULL )	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if( AC == NULL )		{	BMD_FOK(BMD_ERR_PARAM2);	}

	fd=open(file,O_RDWR);
	if( fd < 0 )		{	BMD_FOK(BMD_ERR_DISK);	}

#ifndef WIN32
	status=flock(fd,LOCK_EX);
	if( status != 0 )
	{
		close(fd);
		BMD_FOK(BMD_ERR_DISK);
	}
#endif
	status=bmd_load_binary_content(file,&buf);
	if( status != BMD_OK )
	{
#ifndef WIN32
		flock(fd,LOCK_UN);
		close(fd);
#endif
		return status;
	}
	status=increment_buf(buf,&serial_buf,&len);
	if( status != BMD_OK )
	{
		#ifndef WIN32
		flock(fd,LOCK_UN);
		close(fd);
		#endif
		return status;
	}
	status=write(fd,serial_buf,len);
	if( status != len )
	{
		#ifndef WIN32
		flock(fd,LOCK_UN);
		close(fd);
		#endif
		return BMD_ERR_OP_FAILED;
	}
#ifndef WIN32
	flock(fd,LOCK_UN);
#endif
	close(fd);
	AC->acinfo.serialNumber.buf=(unsigned char *)malloc(len);
	if( AC->acinfo.serialNumber.buf == NULL )	{	BMD_FOK(BMD_ERR_MEMORY);	}

	memmove(AC->acinfo.serialNumber.buf,serial_buf,len);
	AC->acinfo.serialNumber.size=len;
	free0(serial_buf);
	free_gen_buf(&buf);

	return BMD_OK;
}

int AA_AttributeCertificate_set_Issuer(	ConfigData_t *ConfigData,
							AttributeCertificate_t *AC)
{
	PRINT_INFO("LIBBMDAA2INF Setting issuer of attribute certificate\n");

	if(ConfigData == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(AC == NULL)				{	BMD_FOK(BMD_ERR_PARAM2);	}

	AC->acinfo.issuer.present=AttCertIssuer_PR_v2Form;
	BMD_FOK(AC_ACDistinguishedName2GeneralNames(ConfigData->ACConfiguration->ACIssuerSection->dn, &(AC->acinfo.issuer.choice.v2Form.issuerName)));
	return BMD_OK;
}


int AA_AttributeCertificate_set_signatureAlgorithmIdentifier(	ConfigData_t *ConfigData,
											AttributeCertificate_t *AC)
{
OBJECT_IDENTIFIER_t *SignatureAlgorithmOid_1	= NULL;
OBJECT_IDENTIFIER_t *SignatureAlgorithmOid_2	= NULL;
ANY_t *SignatureAlgorithmPrarameters_1		= NULL;
ANY_t *SignatureAlgorithmPrarameters_2		= NULL;
unsigned char DerNullCode[]				= {0x05, 0x00};

	PRINT_INFO("LIBBMDAA2INF Setting signature algorithm identifier of attribute certificate\n");

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if (ConfigData == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (AC == NULL)				{	BMD_FOK(BMD_ERR_PARAM2);	}

	switch(ConfigData->ACConfiguration->ACSignatureSection->choice_signature_format)
	{
		case SIGNATURE_SHAWITHRSA:
		default:
			BMD_FOK(String2OID_alloc(OID_SHA1_WITH_RSA_STR, &SignatureAlgorithmOid_1));
			BMD_FOK(String2OID_alloc(OID_SHA1_WITH_RSA_STR, &SignatureAlgorithmOid_2));
			SignatureAlgorithmPrarameters_1=(ANY_t *)ANY_new_fromBuf((const char *)DerNullCode,2);
			if( SignatureAlgorithmPrarameters_1 == NULL )
				BMD_FOK(BMD_ERR_OP_FAILED);
			SignatureAlgorithmPrarameters_2=(ANY_t *)ANY_new_fromBuf((const char *)DerNullCode,2);
			if( SignatureAlgorithmPrarameters_2 == NULL )
				BMD_FOK(BMD_ERR_OP_FAILED);
			break;
	}

	BMD_FOK(AC_AlgorithmIdentifier_set_AlgorithmOid(SignatureAlgorithmOid_1, &(AC->acinfo.signature)));
	BMD_FOK(AC_AlgorithmIdentifier_set_AlgorithmOid(SignatureAlgorithmOid_2, &(AC->signatureAlgorithm)));
	BMD_FOK(AC_AlgorithmIdentifier_set_AlgorithmParameters(SignatureAlgorithmPrarameters_1,&(AC->acinfo.signature)));
	BMD_FOK(AC_AlgorithmIdentifier_set_AlgorithmParameters(SignatureAlgorithmPrarameters_2,&(AC->signatureAlgorithm)));

	free(SignatureAlgorithmOid_1); SignatureAlgorithmOid_1 = NULL;
	free(SignatureAlgorithmOid_2); SignatureAlgorithmOid_2 = NULL;

	return BMD_OK;
}

int AA_AttributeCertificate_set_attrCertValidityPeriod(	ConfigData_t *ConfigData,
										AttributeCertificate_t *AC)
{
GeneralizedTime_t *NotBeforeTimeStruct	= NULL;
GeneralizedTime_t *NotAfterTimeStruct	= NULL;
struct tm NotBeforeTm;
struct tm NotAfterTm;

	PRINT_INFO("LIBBMDAA2INF Setting serial number of attribute certificate\n");

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if (ConfigData == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (AC == NULL)				{	BMD_FOK(BMD_ERR_PARAM2);	}

	if(GetActualTime(&NotBeforeTm) != 0)
		{ BMD_FOK(BMD_ERR_OP_FAILED); }
	if(GetActualTime(&NotAfterTm) != 0)
		{ BMD_FOK(BMD_ERR_OP_FAILED); }

	switch(ConfigData->ACConfiguration->ACAttCertValidityPeriodSection->choice_ac_lifetime)
	{
		case ACVALIDITY_PERIOD:
				NotAfterTm.tm_year += ConfigData->ACConfiguration->
							ACAttCertValidityPeriodSection->validity_period_years;
				NotAfterTm.tm_mon += ConfigData->ACConfiguration->
							ACAttCertValidityPeriodSection->validity_period_months;
				NotAfterTm.tm_mday += ConfigData->ACConfiguration->
							ACAttCertValidityPeriodSection->validity_period_days;
				NotAfterTm.tm_hour += ConfigData->ACConfiguration->
							ACAttCertValidityPeriodSection->validity_period_hours;
				NotAfterTm.tm_min += ConfigData->ACConfiguration->
							ACAttCertValidityPeriodSection->validity_period_minutes;
				NotAfterTm.tm_sec += ConfigData->ACConfiguration->
							ACAttCertValidityPeriodSection->validity_period_seconds;
				break;
		case ACVALIDITY_SESSION:
		case ACVALIDITY_UNDEFINED:
		default:
				break;
	}

	BMD_FOK(AC_GeneralizedTime_create_from_tm_alloc(&NotBeforeTm, &NotBeforeTimeStruct));
	BMD_FOK(AC_GeneralizedTime_create_from_tm_alloc(&NotAfterTm, &NotAfterTimeStruct));
	BMD_FOK(AC_AttCertValidityPeriod_create(NotBeforeTimeStruct, NotAfterTimeStruct, &(AC->acinfo.attrCertValidityPeriod)));

	free(NotBeforeTimeStruct); NotBeforeTimeStruct = NULL;
	free(NotAfterTimeStruct); NotAfterTimeStruct = NULL;

	return BMD_OK;
}


int AA_AttributeCertificate_set_attributes(	ConfigData_t *ConfigData,
								AttributesSequence_t *AttributesSequence,
								AttributeCertificate_t *AC)
{
	PRINT_INFO("LIBBMDAA2INF Setting attribute of attribute certificate\n");

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if (ConfigData == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (AttributesSequence == NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (AC == NULL)				{	BMD_FOK(BMD_ERR_PARAM3);	}

	AC->acinfo.attributes.list.array = AttributesSequence->list.array;
	AC->acinfo.attributes.list.count = AttributesSequence->list.count;
	AC->acinfo.attributes.list.size = AttributesSequence->list.size;
	AC->acinfo.attributes.list.free = AttributesSequence->list.free;

	return BMD_OK;
}

int AA_AttributeCertificate_set_signatureValue(	ConfigData_t *ConfigData,
								AttributeCertificate_t *AC)
{
	ANY_t *ACInfoDER			= NULL;

	PRINT_INFO("LIBBMDAA2INF Setting signature value of attribute certificate\n");

	/************************/
	/* walidacja parametrow */
	/************************/
	if (ConfigData == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (AC == NULL)				{	BMD_FOK(BMD_ERR_PARAM2);	}

	/***********************************************/
	/* tworzymy DER'a z Attribute Certificate Info */
	/***********************************************/
	ACInfoDER=ANY_new_fromType(&asn_DEF_AttributeCertificateInfo,&(AC->acinfo));
	if( ACInfoDER == NULL )			{	BMD_FOK(BMD_ERR_OP_FAILED);	}

	/********************/
	/* obliczamy podpis */
	/********************/
	memset(&(AC->signatureValue),0,sizeof(BIT_STRING_t));
	BMD_FOK(AC_SignatureValue_create_alloc(ACInfoDER,&(AC->signatureValue),ConfigData));

	asn_DEF_ANY.free_struct(&asn_DEF_ANY,ACInfoDER,0);ACInfoDER=NULL;

	return BMD_OK;
}


int AA_Create_baseCertificateID(	Certificate_t *Certificate,
						ConfigData_t *ConfigData,
						IssuerSerial_t **IssuerSerial)
{
	GeneralNames_t *IssuerGeneralNames			= NULL;
	char *HolderString					= NULL;
	Name_t *PKCIssuerName					= NULL;
	CertificateSerialNumber_t *PKCserialNumber	= NULL;

	PRINT_INFO("LIBBMDAA2INF Creating base certificate id\n");

	if (Certificate == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (ConfigData == NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (IssuerSerial == NULL)		{	BMD_FOK(BMD_ERR_PARAM3);	}
	if (*IssuerSerial != NULL)		{	BMD_FOK(BMD_ERR_PARAM3);	}

	HolderString = ConfigData->ACConfiguration->ACHolderSection->ac_holder_source;

	(*IssuerSerial)=(IssuerSerial_t *)malloc(sizeof(IssuerSerial_t));
	if( (*IssuerSerial) == NULL )
		BMD_FOK(BMD_ERR_MEMORY);
	memset(*IssuerSerial,0,sizeof(IssuerSerial_t));

	if(!strcmp(HolderString, HOLDER_PRESENT_IN_PKC_STR))
	{
		GeneralName_t *tmp_gn=NULL;

		tmp_gn=(GeneralName_t *)malloc(sizeof(GeneralName_t));
		if( tmp_gn == NULL )
			BMD_FOK(BMD_ERR_MEMORY);
		memset(tmp_gn,0,sizeof(GeneralName_t));
		tmp_gn->present=GeneralName_PR_directoryName;

		PKCIssuerName=&(tmp_gn->choice.directoryName);
		PKCserialNumber=&((*IssuerSerial)->serial);
		IssuerGeneralNames=&((*IssuerSerial)->issuer);

		BMD_FOK(asn_cloneContent(&asn_DEF_Name,(void *)&(Certificate->tbsCertificate.issuer),
			                     (void **)&PKCIssuerName));
		asn_set_add(&(IssuerGeneralNames->list),tmp_gn);
		BMD_FOK(asn_cloneContent(&asn_DEF_CertificateSerialNumber,
			    (void *)&(Certificate->tbsCertificate.serialNumber),(void **)&PKCserialNumber));
	}
	else
	{
		BMD_FOK(ERR_AASRV_UNKNOWN_AC_HOLDER_SOURCE);
	}

	return BMD_OK;
}

