/***************************************************************************
 *            libbmdX509.c
 *  BMD X509 certificate parsing library
 *  Wed Mar  1 14:44:02 2006
 *  Copyright  2006  Marcin Szulga
 *  Email:  mszulga@certum.pl
 ****************************************************************************/

#include <bmd/libbmdX509/libbmdX509.h>
#include <bmd/libbmdpki/libbmdpki.h>

#include <bmd/common/bmd-openssl.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>
#include <bmd/libbmderr/libbmderr.h>

/****************
 * ONE CALL API *
 ***************/
long X509CertGetAttr(	GenBuf_t *GenBuf,
			X509CertAttrType_t X509CertAttrType,
			char **AttributeString,
			long *AttributeLong)
{
	PRINT_INFO("LIBBMDX509INF Getting certificate attributes\n");
	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if(	GenBuf == NULL )			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(	GenBuf->buf == NULL ||
		GenBuf->size == 0)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(X509CertAttrType < 0)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(	AttributeString == NULL ||
		*AttributeString != NULL)		{	BMD_FOK(BMD_ERR_PARAM3);	}
	if(	AttributeLong == NULL ||
		*AttributeLong != 0)			{	BMD_FOK(BMD_ERR_PARAM4);	}

	switch(X509CertAttrType)
	{
		case X509_ATTR_VERSION:
			BMD_FOK(GetVersionFromX509Certificate(GenBuf, AttributeLong));
			break;
		case X509_ATTR_SN:
			BMD_FOK(GetSerialNumberFromX509Certificate(GenBuf, AttributeString));
			break;
		case X509_ATTR_SUBJECT_SN:
			BMD_FOK(GetSubjectSerialNumberFromX509Certificate(GenBuf, AttributeString));
			break;
		case X509_ATTR_ISSUER_SN:
			BMD_FOK(GetIssuerSerialNumberFromX509Certificate(GenBuf, AttributeString));
			break;
		case X509_ATTR_SUBJECT_DN:
			BMD_FOK(GetSubjectDNFromX509Certificate(GenBuf, AttributeString));
			break;
		case X509_ATTR_ISSUER_DN:
			BMD_FOK(GetIssuerDNFromX509Certificate(GenBuf, AttributeString));
			break;
		case X509_ATTR_SIGALG_OID:
			BMD_FOK(GetAlgorithmOIDFromX509Certificate(GenBuf, AttributeString));
			break;
		case X509_ATTR_VALIDITY_NB:
			BMD_FOK(GetValidityNBFromX509Certificate(GenBuf, AttributeString));
			break;
		case X509_ATTR_VALIDITY_NA:
			BMD_FOK(GetValidityNAFromX509Certificate(GenBuf, AttributeString));
			break;
		case X509_ATTR_SUBJECT_SURNAME:
			BMD_FOK(GetSubjectSurnameFromX509Certificate(GenBuf, AttributeString));
			break;
		case X509_ATTR_SUBJECT_GIVENNAME:
			BMD_FOK(GetSubjectGivenNameFromX509Certificate(GenBuf, AttributeString));
			break;
		case X509_ATTR_SUBJECT_COMMONNAME:
			BMD_FOK(GetSubjectCommonNameFromX509Certificate(GenBuf, AttributeString));
			break;
		case X509_ATTR_SUBJECT_ORGANIZATION:
			BMD_FOK(GetSubjectOrganizationFromX509Certificate(GenBuf, AttributeString));
			break;
		case X509_ATTR_ISSUER_ORGANIZATION:
			BMD_FOK(GetIssuerOrganizationFromX509Certificate(GenBuf, AttributeString));
			break;
		case X509_ATTR_SUBJECTPUBLICKEYINFO_ALG_OID:
			BMD_FOK(GetSubjectPublicKeyInfoAlgorithmX509Certificate(GenBuf, AttributeString));
			break;
		case X509_ATTR_COUNTRY:
			BMD_FOK(GetSubjectCountryFromX509Certificate(GenBuf, AttributeString));
			break;
		case X509_ATTR_ISSUER_COUNTRY:
			BMD_FOK(GetIssuerCountryFromX509Certificate(GenBuf, AttributeString));
			break;
		case X509_ATTR_LOCALITY:
			BMD_FOK(GetSubjectLocalityFromX509Certificate(GenBuf, AttributeString));
			break;
		case X509_ATTR_ISSUER_LOCALITY:
			BMD_FOK(GetIssuerLocalityFromX509Certificate(GenBuf, AttributeString));
			break;
		case X509_ATTR_ORGANIZATIONALUNIT:
			BMD_FOK(GetSubjectOrganizationalUnitFromX509Certificate(GenBuf, AttributeString));
			break;
		case X509_ATTR_ISSUER_ORGANIZATIONALUNIT:
			BMD_FOK(GetIssuerOrganizationalUnitFromX509Certificate(GenBuf, AttributeString));
			break;
		case X509_ATTR_ISSUER_COMMONNAME:
			BMD_FOK(GetIssuerCommonNameFromX509Certificate(GenBuf,AttributeString));
			break;
		case X509_ATTR_EXT_CRL_DISPOINTS:
			BMD_FOK(GetCRLDistPointsFromX509Certificate(GenBuf,AttributeString));
			break;
		default:
			PRINT_ERROR("LIBBMDX509ERR Unrecognized X509 cert attribute type!\n");
			BMD_FOK(ERR_X509_UNRECOGNIZED_ATTR_TYPE);
			break;
	}
	return BMD_OK;
}

/************
 * EASY API *
 ***********/
long GetIssuerEMailFromX509Certificate(	char *pfx_file,
							char *pfx_pass,
							char **e_mail)
{
bmd_crypt_ctx_t *ctx		= NULL;
long AttributeLong		= 0;
char *tmp				= NULL;
char pattern[]			= {"emailAddress"};
char *ptr				= NULL;
char *ptr2				= NULL;

	PRINT_INFO("STDSESSIONINF Getting e-mail address from certificate\n");
	/********************************/
	/*	walidacja parametrow	*/
	/********************************/
	if (pfx_file==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (pfx_pass==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	/************************************************/
	/*	wczytanie certyfikatu do kontekstu	*/
	/************************************************/
	BMD_FOK(bmd_set_ctx_file(pfx_file, pfx_pass, (long)strlen(pfx_pass), &ctx));

	/****************************************/
	/*	pobranie atrybutu z certyfikatu	*/
	/****************************************/
	BMD_FOK(X509CertGetAttr(ctx->file->cert, X509_ATTR_SUBJECT_DN, &tmp, &AttributeLong));

	if ((ptr=strstr(tmp,pattern))==NULL)
	{
		return -1;
	}

	if ((ptr2=strstr(ptr,"<"))==NULL)
	{
		return -1;
	}


	asprintf(e_mail,"%s",(ptr2+1));
	(*e_mail)[strlen(*e_mail)-1]='\0';

	/******************/
	/*	porzadki	*/
	/******************/
	free(tmp); tmp=NULL;

	return 0;
}

long GetVersionFromX509Certificate(	GenBuf_t *GenBuf,
						long *version)
{
Certificate_t *Certificate = NULL;

	PRINT_INFO("LIBBMDX509INF Getting version from certificate\n");
	/******************************/
	/*	walidacja paramterow	*/
	/******************************/
	if(GenBuf == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(GenBuf->buf == NULL ||
		GenBuf->size == 0)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(version == NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(*version != 0)			{	BMD_FOK(BMD_ERR_PARAM2);	}

	BMD_FOK(GetCertificateStructFromX509Certificate(GenBuf, &Certificate));
	BMD_FOK(GetVersionFromCertificate(Certificate, version));

	asn_DEF_Certificate.free_struct(&asn_DEF_Certificate, Certificate, 0);

	return BMD_OK;
}


long CheckCertificateSubjectInX509Certificate(GenBuf_t *GenBuf, long *category, CertExtConfig_t *twf_CertExtConfig)
{
Certificate_t *Certificate = NULL;

	PRINT_INFO("LIBBMDX509INF Checking certificate category\n");
	/******************************/
	/*	walidacja paramterow	*/
	/******************************/
	if(GenBuf == NULL)									{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(GenBuf->buf == NULL || GenBuf->size == 0)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(category == NULL)								{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(*category != 0)									{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(twf_CertExtConfig == NULL)						{	BMD_FOK(BMD_ERR_PARAM3);	}

	BMD_FOK(GetCertificateStructFromX509Certificate(GenBuf, &Certificate));
	BMD_FOK(CheckCertificateSubject(Certificate, category, twf_CertExtConfig));

	asn_DEF_Certificate.free_struct(&asn_DEF_Certificate, Certificate, 0);

	return BMD_OK;
}

/*Na potrzeby deklaracji zgodnosci. Sprawdzenie wystawcy certyfikatu.*/
long CheckCertificateIssuer(GenBuf_t *GenBuf, CertExtConfig_t *twf_CertExtConfig)
{
	long issuer_cn_exists			= 0;
	long issuer_sn_exists			= 0;
	long tmplong					= 0;
	long status						= 0;
	char *tmpchar					= NULL;

	if(GenBuf == NULL)									{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(GenBuf->buf == NULL || GenBuf->size <=0)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(twf_CertExtConfig == NULL)						{	BMD_FOK(BMD_ERR_PARAM2);	}

	/*Pobieram country issuera - sprawdzenie na potrzeby deklaracji zgodnosci*/
	if(twf_CertExtConfig->check_issuer_country == 1)
	{
		status=X509CertGetAttr(GenBuf,X509_ATTR_ISSUER_COUNTRY,&tmpchar,&tmplong);
		if (tmpchar == NULL || status!=BMD_OK)
		{
			return ERR_X509_ISSUER_NOT_CORRECT;
		}
		free(tmpchar); tmpchar=NULL; tmplong =0;
	}


	/*Pobieram organizacje issuera - sprawdzenie na potrzeby deklaracji zgodnosci*/
	if(twf_CertExtConfig->check_issuer_organisation == 1)
	{
		status=X509CertGetAttr(GenBuf,X509_ATTR_ISSUER_ORGANIZATION,&tmpchar,&tmplong);
		if (tmpchar == NULL || status!=BMD_OK)
		{
			return ERR_X509_ISSUER_NOT_CORRECT;
		}
		free(tmpchar); tmpchar=NULL; tmplong =0;
	}


	/*Pobieram nazwe powszechna issuera - sprawdzenie na potrzeby deklaracji zgodnosci*/
	if(twf_CertExtConfig->check_issuer_cn_and_sn == 1)
	{
		status=X509CertGetAttr(GenBuf,X509_ATTR_ISSUER_COMMONNAME,&tmpchar,&tmplong);
		if (tmpchar != NULL && status==BMD_OK)
		{
			issuer_cn_exists = 1;
		}
		free(tmpchar); tmpchar=NULL; tmplong =0;

		/*Pobieram numer seryjny issuera - sprawdzenie na potrzeby deklaracji zgodnosci*/
		status=X509CertGetAttr(GenBuf,X509_ATTR_ISSUER_SN,&tmpchar,&tmplong);
		if (tmpchar != NULL && status==BMD_OK)
		{
			issuer_sn_exists = 1;
		}
		free(tmpchar); tmpchar=NULL; tmplong =0;

		if(issuer_cn_exists == 0 && issuer_sn_exists == 0)
		{
			return ERR_X509_ISSUER_NOT_CORRECT;
		}
	}

	return BMD_OK;
}

long CheckCertificateExtensionsInX509Certificate(GenBuf_t *GenBuf, CertExtConfig_t *twf_CertExtConfig)
{
Certificate_t *Certificate = NULL;

	PRINT_INFO("LIBBMDX509INF Checking certificate extensions\n");
	/******************************/
	/*	walidacja paramterow	*/
	/******************************/
	if(GenBuf == NULL)									{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(GenBuf->buf == NULL || GenBuf->size == 0)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(twf_CertExtConfig == NULL)						{	BMD_FOK(BMD_ERR_PARAM2);	}

	BMD_FOK(GetCertificateStructFromX509Certificate(GenBuf, &Certificate));
	BMD_FOK(CheckCertificateExtensions(Certificate, twf_CertExtConfig));

	asn_DEF_Certificate.free_struct(&asn_DEF_Certificate, Certificate, 0);

	return BMD_OK;
}



long GetSerialNumberFromX509Certificate(GenBuf_t *GenBuf, char **SerialNumber)
{
long err = 0;
Certificate_t *Certificate = NULL;

	PRINT_INFO("LIBBMDX509INF Getting serial number from certificate\n");
	if(GenBuf == NULL)								{		BMD_FOK(BMD_ERR_PARAM1);		}
	if(GenBuf->buf == NULL || GenBuf->size == 0)	{		BMD_FOK(BMD_ERR_PARAM1);		}

	if(SerialNumber == NULL)						{		BMD_FOK(BMD_ERR_PARAM2);		}
	if(*SerialNumber != NULL)						{		BMD_FOK(BMD_ERR_PARAM2);		}

	err = GetCertificateStructFromX509Certificate(GenBuf, &Certificate);
	if(err < 0)
	{
		PRINT_ERROR("LIBBMDCRLERR Error in getting certificate struct from X509 certificate list. Error=%li\n", err);
		return err;
	}
	err = GetSerialNumberFromCertificate(Certificate, SerialNumber);
	if(err < 0)
	{
		PRINT_ERROR("LIBBMDCRLERR Error in getting serial number from Certificate. Error=%li\n", err);
		return err;
	}
	asn_DEF_Certificate.free_struct(&asn_DEF_Certificate, Certificate, 0);
	return err;
}

long GetSubjectSerialNumberFromX509Certificate(GenBuf_t *GenBuf, char **SerialNumber)
{
long err = 0;
Certificate_t *Certificate = NULL;

	PRINT_INFO("LIBBMDX509INF Getting serial number from certificate subject\n");
	if(GenBuf == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(GenBuf->buf == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(GenBuf->size <= 0)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(SerialNumber == NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(*SerialNumber != NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	err = GetCertificateStructFromX509Certificate(GenBuf, &Certificate);
	if(err < 0)
	{
		PRINT_ERROR("LIBBMDCRLERR Error in getting certificate struct from X509 certificate list. Error=%li\n", err);
		return err;
	}
	err = GetSubjectSerialNumberFromCertificate(Certificate, SerialNumber);
	if(err < 0)
	{
		PRINT_ERROR("LIBBMDCRLERR Error in getting serial number from certificate subject. Error=%li\n", err);
		return err;
	}
	asn_DEF_Certificate.free_struct(&asn_DEF_Certificate, Certificate, 0);
	return err;
}


long GetIssuerSerialNumberFromX509Certificate(GenBuf_t *GenBuf, char **SerialNumber)
{
long err = 0;
Certificate_t *Certificate = NULL;

	PRINT_INFO("LIBBMDX509INF Getting serial number from certificate issuer\n");
	if(GenBuf == NULL || GenBuf->buf == NULL || GenBuf->size == 0)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid first parameter value. Error=%i",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(SerialNumber == NULL || *SerialNumber != NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid second parameter value. Error=%i",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	err = GetCertificateStructFromX509Certificate(GenBuf, &Certificate);
	if(err < 0)
	{
		PRINT_ERROR("LIBBMDCRLERR Error in getting certificate struct from X509 certificate list. Error=%li\n", err);
		return err;
	}
	err = GetIssuerSerialNumberFromCertificate(Certificate, SerialNumber);
	if(err < 0)
	{
		PRINT_ERROR("LIBBMDCRLERR Error in getting serial number from Certificate Issuer. Error=%li\n", err);
		return err;
	}
	asn_DEF_Certificate.free_struct(&asn_DEF_Certificate, Certificate, 0);
	return err;
}


long GetSubjectDNFromX509Certificate(GenBuf_t *GenBuf, char **SubjectDN)
{
long err = 0;
Certificate_t *Certificate = NULL;

	PRINT_INFO("LIBBMDX509INF Getting subject dnf from certificate\n");
	if(GenBuf == NULL || GenBuf->buf == NULL || GenBuf->size == 0)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid first parameter value. Error=%i",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(SubjectDN == NULL || *SubjectDN != NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid second parameter value. Error=%i",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	err = GetCertificateStructFromX509Certificate(GenBuf, &Certificate);
	if(err < 0)
	{
		PRINT_ERROR("LIBBMDCRLERR Error in getting certificate struct from X509 certificate list. Error=%li\n", err);
		return err;
	}
	err = GetSubjectDNFromCertificate(Certificate, SubjectDN);
	if(err < 0)
	{
		PRINT_ERROR("LIBBMDCRLERR Error in getting subject DN from Certificate. Error=%li\n", err);
		return err;
	}
	asn_DEF_Certificate.free_struct(&asn_DEF_Certificate, Certificate, 0);
	return err;
}

long GetSubjectSurnameFromX509Certificate(GenBuf_t *GenBuf, char **SubjectSurname)
{
long err = 0;
Certificate_t *Certificate = NULL;

	PRINT_INFO("LIBBMDX509INF Getting subject surname from certificate\n");
	if(GenBuf == NULL || GenBuf->buf == NULL || GenBuf->size == 0)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid first parameter value. Error=%i",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(SubjectSurname == NULL || *SubjectSurname != NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid second parameter value. Error=%i",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	err = GetCertificateStructFromX509Certificate(GenBuf, &Certificate);
	if(err < 0)
	{
		PRINT_ERROR("LIBBMDCRLERR Error in getting certificate struct from X509 certificate list. Error=%li\n", err);
		return err;
	}
	err = GetSubjectSurnameFromCertificate(Certificate, SubjectSurname);
	if(err < 0)
	{
		PRINT_ERROR("LIBBMDCRLERR Error in getting subject Surname from Certificate. Error=%li\n", err);
		return err;
	}
	asn_DEF_Certificate.free_struct(&asn_DEF_Certificate, Certificate, 0);
	return err;
}

long GetSubjectGivenNameFromX509Certificate(GenBuf_t *GenBuf, char **SubjectGivenName)
{
long err = 0;
Certificate_t *Certificate = NULL;

	PRINT_INFO("LIBBMDX509INF Getting subject given name from certificate\n");
	if(GenBuf == NULL || GenBuf->buf == NULL || GenBuf->size == 0)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid first parameter value. Error=%i",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(SubjectGivenName == NULL || *SubjectGivenName != NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid second parameter value. Error=%i",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	err = GetCertificateStructFromX509Certificate(GenBuf, &Certificate);
	if(err < 0)
	{
		PRINT_ERROR("LIBBMDCRLERR Error in getting certificate struct from X509 certificate list. Error=%li\n", err);
		return err;
	}
	err = GetSubjectGivenNameFromCertificate(Certificate, SubjectGivenName);
	if(err < 0)
	{
		PRINT_ERROR("LIBBMDCRLERR Error in getting subject GivenName from Certificate. Error=%li\n", err);
		return err;
	}
	asn_DEF_Certificate.free_struct(&asn_DEF_Certificate, Certificate, 0);
	return err;
}

long GetSubjectCommonNameFromX509Certificate(GenBuf_t *GenBuf, char **SubjectCommonName)
{
long err = 0;
Certificate_t *Certificate = NULL;

	PRINT_INFO("LIBBMDX509INF Getting subject common name from certificate\n");
	if(GenBuf == NULL || GenBuf->buf == NULL || GenBuf->size == 0)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid first parameter value. Error=%i",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(SubjectCommonName == NULL || *SubjectCommonName != NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid second parameter value. Error=%i",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	err = GetCertificateStructFromX509Certificate(GenBuf, &Certificate);
	if(err < 0)
	{
		PRINT_ERROR("LIBBMDCRLERR Error in getting certificate struct from X509 certificate list. Error=%li\n", err);
		return err;
	}
	err = GetSubjectCommonNameFromCertificate(Certificate, SubjectCommonName);
	if(err < 0)
	{
		PRINT_ERROR("LIBBMDCRLERR Error in getting subject CommonName from Certificate. Error=%li\n", err);
		return err;
	}
	asn_DEF_Certificate.free_struct(&asn_DEF_Certificate, Certificate, 0);
	return err;
}

long GetSubjectOrganizationFromX509Certificate(GenBuf_t *GenBuf, char **SubjectOrganization)
{
long err = 0;
Certificate_t *Certificate = NULL;

	PRINT_INFO("LIBBMDX509INF Getting subject organization from certificate\n");
	if(GenBuf == NULL || GenBuf->buf == NULL || GenBuf->size == 0)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid first parameter value. Error=%i",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(SubjectOrganization == NULL || *SubjectOrganization != NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid second parameter value. Error=%i",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	err = GetCertificateStructFromX509Certificate(GenBuf, &Certificate);
	if(err < 0)
	{
		PRINT_ERROR("LIBBMDCRLERR Error in getting certificate struct from X509 certificate list. Error=%li\n", err);
		return err;
	}
	err = GetSubjectOrganizationFromCertificate(Certificate, SubjectOrganization);
	if(err < 0)
	{
		PRINT_ERROR("LIBBMDCRLERR Error in getting subject Organization from Certificate. Error=%li\n", err);
		return err;
	}
	asn_DEF_Certificate.free_struct(&asn_DEF_Certificate, Certificate, 0);
	return err;
}


long GetIssuerOrganizationFromX509Certificate(GenBuf_t *GenBuf, char **IssuerOrganization)
{
long err = 0;
Certificate_t *Certificate = NULL;

	PRINT_INFO("LIBBMDX509INF Getting issuer organization from certificate\n");
	if(GenBuf == NULL || GenBuf->buf == NULL || GenBuf->size == 0)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid first parameter value. Error=%i",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(IssuerOrganization == NULL || *IssuerOrganization != NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid second parameter value. Error=%i",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	err = GetCertificateStructFromX509Certificate(GenBuf, &Certificate);
	if(err < 0)
	{
		PRINT_ERROR("LIBBMDCRLERR Error in getting certificate struct from X509 certificate list. Error=%li\n", err);
		return err;
	}
	err = GetIssuerOrganizationFromCertificate(Certificate, IssuerOrganization);
	if(err < 0)
	{
		PRINT_ERROR("LIBBMDCRLERR Error in getting subject Organization from Certificate. Error=%li\n", err);
		return err;
	}
	asn_DEF_Certificate.free_struct(&asn_DEF_Certificate, Certificate, 0);
	return err;
}


/*added by WSZ*/
long GetSubjectPublicKeyInfoAlgorithmX509Certificate(GenBuf_t *GenBuf, char **alg_oid)
{
	long ret_val=0;
	Certificate_t *Certificate=NULL;
	OBJECT_IDENTIFIER_t *oid=NULL;
	long arcs_num=0;
	long oid_table[20];
	char *temp1=NULL, *temp2=NULL;
	long iter=0;

	if(GenBuf == NULL)
		{ return -1; }
	if(GenBuf->buf == NULL)
		{ return -2; }
	if(GenBuf->size == 0)
		{ return -3; }
	if(alg_oid == NULL)
		{ return -4; }
	if(*alg_oid != NULL)
		{ return -5; }

	ret_val = GetCertificateStructFromX509Certificate(GenBuf, &Certificate);
	if(ret_val < 0)
		{ BMD_FOK(-6); }

	ret_val=GetSubjectPublicKeyInfoAlgorithmFromCertificate(Certificate, &oid);
	asn_DEF_Certificate.free_struct(&asn_DEF_Certificate, Certificate, 1);
	free(Certificate);
	if(ret_val < 0)
		{ return -7; }

	//zamiana OID na char*
	arcs_num=OBJECT_IDENTIFIER_get_arcs(oid, oid_table, sizeof(oid_table[0]), 20);
	asn_DEF_OBJECT_IDENTIFIER.free_struct(&asn_DEF_OBJECT_IDENTIFIER, oid, 1);
	free(oid);
	if(arcs_num < 0)
		{ return -8; }

	for(iter=0; iter < arcs_num; iter++)
	{
		if(iter == 0)
		{
			asprintf(&temp1, "%li", oid_table[iter]);
		}
		else
		{
			asprintf(&temp2, "%s.%li", temp1, oid_table[iter]);
			free(temp1);
			temp1=temp2; temp2=NULL;
		}
	}

	*alg_oid=temp1;
	return 0;
}


long GetIssuerCommonNameFromX509Certificate(GenBuf_t *GenBuf, char **IssuerCommonName)
{
long err = 0;
Certificate_t *Certificate = NULL;

	PRINT_INFO("LIBBMDX509INF Getting subject organization from certificate\n");
	if(GenBuf == NULL || GenBuf->buf == NULL || GenBuf->size == 0)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid first parameter value. Error=%i",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(IssuerCommonName == NULL || *IssuerCommonName != NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid second parameter value. Error=%i",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	err = GetCertificateStructFromX509Certificate(GenBuf, &Certificate);
	if(err < 0)
	{
		PRINT_ERROR("LIBBMDCRLERR Error in getting certificate struct from X509 certificate list. Error=%li\n", err);
		return err;
	}
	err = GetIssuerCommonNameFromCertificate(Certificate, IssuerCommonName);
	if(err < 0)
	{
		PRINT_ERROR("LIBBMDCRLERR Error in getting issuer CommonName from Certificate. Error=%li\n", err);
		return err;
	}
	asn_DEF_Certificate.free_struct(&asn_DEF_Certificate, Certificate, 0);
	return err;
}

long GetIssuerDNFromX509Certificate(GenBuf_t *GenBuf, char **IssuerDN)
{
Certificate_t *Certificate = NULL;

	PRINT_INFO("LIBBMDX509INF Getting issuer DNF from certificate\n");
	/************************/
	/* walidacja parametrow */
	/************************/
	if (GenBuf == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (GenBuf->buf == NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (GenBuf->size == 0)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (IssuerDN == NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (*IssuerDN != NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	BMD_FOK(GetCertificateStructFromX509Certificate(GenBuf, &Certificate));
	BMD_FOK(GetIssuerDNFromCertificate(Certificate, IssuerDN));

	asn_DEF_Certificate.free_struct(&asn_DEF_Certificate, Certificate, 0);

	return BMD_OK;
}

long GetAlgorithmOIDFromX509Certificate(GenBuf_t *GenBuf, char **SignatureAlgorithmOid)
{
long err = 0;
Certificate_t *Certificate = NULL;

	PRINT_INFO("LIBBMDX509INF Getting algorith oid from certificate\n");
	if(GenBuf == NULL || GenBuf->buf == NULL || GenBuf->size == 0)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid first parameter value. Error=%i",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(SignatureAlgorithmOid == NULL || *SignatureAlgorithmOid != NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid second parameter value. Error=%i",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	err = GetCertificateStructFromX509Certificate(GenBuf, &Certificate);
	if(err < 0)
	{
		PRINT_ERROR("LIBBMDCRLERR Error in getting certificate struct from X509 certificate list. Error=%li\n", err);
		return err;
	}
	err = GetAlgorithmOIDFromCertificate(Certificate, SignatureAlgorithmOid);
	if(err < 0)
	{
		PRINT_ERROR("LIBBMDCRLERR Error in getting signature algorithm OID from Certificate. Error=%li\n", err);
		return err;
	}
	asn_DEF_Certificate.free_struct(&asn_DEF_Certificate, Certificate, 0);
	return err;
}

long GetValidityNBFromX509Certificate(GenBuf_t *GenBuf, char **ValidityNB)
{
long err = 0;
Certificate_t *Certificate = NULL;

	PRINT_INFO("LIBBMDX509INF Getting validity NB from certificate\n");
	if(GenBuf == NULL || GenBuf->buf == NULL || GenBuf->size == 0)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid first parameter value. Error=%i",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(ValidityNB == NULL || *ValidityNB != NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid second parameter value. Error=%i",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	err = GetCertificateStructFromX509Certificate(GenBuf, &Certificate);
	if(err < 0)
	{
		PRINT_ERROR("LIBBMDCRLERR Error in getting certificate struct from X509 certificate list. Error=%li\n", err);
		return err;
	}
	err = GetValidityNBFromCertificate(Certificate, ValidityNB);
	if(err < 0)
	{
		PRINT_ERROR("LIBBMDCRLERR Error in getting validity (notBefore) from Certificate. Error=%li\n", err);
		return err;
	}
	asn_DEF_Certificate.free_struct(&asn_DEF_Certificate, Certificate, 0);
	return err;
}

long GetValidityNBFromX509Certificate_time(GenBuf_t *GenBuf,time_t *time_NB)
{
	long err=0;
	Certificate_t *Certificate=NULL;

	PRINT_INFO("LIBBMDX509INF Getting validity NB from certificate\n");
	if(GenBuf == NULL || GenBuf->buf == NULL || GenBuf->size == 0)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid first parameter value. Error=%i",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}

	err = GetCertificateStructFromX509Certificate(GenBuf, &Certificate);
	if(err < 0)
	{
		PRINT_ERROR("LIBBMDCRLERR Error in getting certificate struct from X509 certificate list. Error=%li\n", err);
		return err;
	}

	if( Certificate->tbsCertificate.validity.notBefore.present == Time_PR_utcTime )
	{
		(*time_NB)=asn_UT2time(&(Certificate->tbsCertificate.validity.notBefore.choice.utcTime),NULL,1);
	}
	else
	{
		(*time_NB)=asn_GT2time(&(Certificate->tbsCertificate.validity.notBefore.choice.generalTime),NULL,1);
	}

	asn_DEF_Certificate.free_struct(&asn_DEF_Certificate,Certificate,0);Certificate=NULL;

	return err;
}

long GetValidityNAFromX509Certificate(GenBuf_t *GenBuf, char **ValidityNA)
{
long err = 0;
Certificate_t *Certificate = NULL;

	PRINT_INFO("LIBBMDX509INF Getting validity NB from certificate\n");
	if(GenBuf == NULL || GenBuf->buf == NULL || GenBuf->size == 0)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid first parameter value. Error=%i",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(ValidityNA == NULL || *ValidityNA != NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid second parameter value. Error=%i",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	err = GetCertificateStructFromX509Certificate(GenBuf, &Certificate);
	if(err < 0)
	{
		PRINT_ERROR("LIBBMDCRLERR Error in getting certificate struct from X509 certificate list. Error=%li\n", err);
		return err;
	}
	err = GetValidityNAFromCertificate(Certificate, ValidityNA);
	if(err < 0)
	{
		PRINT_ERROR("LIBBMDCRLERR Error in getting validity (notAfter) from Certificate. Error=%li\n", err);
		return err;
	}
	asn_DEF_Certificate.free_struct(&asn_DEF_Certificate, Certificate, 0);
	return err;
}

long GetValidityNAFromX509Certificate_time(GenBuf_t *GenBuf, time_t *time_NA)
{
	long err=0;
	Certificate_t *Certificate = NULL;

	PRINT_INFO("LIBBMDX509INF Getting validity NB from certificate\n");
	if(GenBuf == NULL || GenBuf->buf == NULL || GenBuf->size == 0)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid first parameter value. Error=%i",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}

	err = GetCertificateStructFromX509Certificate(GenBuf, &Certificate);
	if(err < 0)
	{
		PRINT_ERROR("LIBBMDCRLERR Error in getting certificate struct from X509 certificate list. Error=%li\n", err);
		return err;
	}

	if( Certificate->tbsCertificate.validity.notAfter.present == Time_PR_utcTime )
	{
		(*time_NA)=asn_UT2time(&(Certificate->tbsCertificate.validity.notAfter.choice.utcTime),NULL,1);
	}
	else
	{
		(*time_NA)=asn_GT2time(&(Certificate->tbsCertificate.validity.notAfter.choice.generalTime),NULL,1);
	}

	asn_DEF_Certificate.free_struct(&asn_DEF_Certificate,Certificate,0);Certificate=NULL;

	return err;
}

long GetCRLDistPointsFromX509Certificate(GenBuf_t *GenBuf, char **CRLDistPointsString)
{
long err = 0;
Certificate_t *Certificate = NULL;

	PRINT_INFO("LIBBMDX509INF Getting CRL distance polong from certificate\n");
	if(GenBuf == NULL || GenBuf->buf == NULL || GenBuf->size == 0)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid first parameter value. Error=%i",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(CRLDistPointsString == NULL || *CRLDistPointsString != NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid second parameter value. Error=%i",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}

	err = GetCertificateStructFromX509Certificate(GenBuf, &Certificate);
	if(err < 0)
	{
		PRINT_ERROR("LIBBMDCRLERR Error in getting certificate struct from X509 certificate list. Error=%li\n",err);
		return err;
	}
	err = GetExtCRLDistPointsFromCertificate(Certificate, CRLDistPointsString);
	if(err < 0)
	{
		PRINT_ERROR("LIBBMDCRLERR Error in getting CRL distribution points extension from Certificate. Error=%li\n", err);
		return err;
	}
	asn_DEF_Certificate.free_struct(&asn_DEF_Certificate, Certificate, 0);
	return err;
}

long GetSubjectCountryFromX509Certificate(GenBuf_t *GenBuf, char **SubjectCountry)
{
long err = 0;
Certificate_t *Certificate = NULL;

	PRINT_INFO("LIBBMDX509INF Getting subject country from certificate\n");
	if(GenBuf == NULL || GenBuf->buf == NULL || GenBuf->size == 0)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid first parameter value. Error=%i",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(SubjectCountry == NULL || *SubjectCountry != NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid second parameter value. Error=%i",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	err = GetCertificateStructFromX509Certificate(GenBuf, &Certificate);
	if(err < 0)
	{
		PRINT_ERROR("LIBBMDCRLERR Error in getting certificate struct from X509 certificate list. Error=%li\n", err);
		return err;
	}
	err = GetSubjectCountryFromCertificate(Certificate, SubjectCountry);
	if(err < 0)
	{
		PRINT_ERROR("LIBBMDCRLERR Error in getting subject DN from Certificate. Error=%li\n", err);
		return err;
	}
	asn_DEF_Certificate.free_struct(&asn_DEF_Certificate, Certificate, 0);
	return err;
}

long GetIssuerCountryFromX509Certificate(GenBuf_t *GenBuf, char **IssuerCountry)
{
long err = 0;
Certificate_t *Certificate = NULL;

	PRINT_INFO("LIBBMDX509INF Getting issuer country from certificate\n");
	if(GenBuf == NULL || GenBuf->buf == NULL || GenBuf->size == 0)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid first parameter value. Error=%i",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(IssuerCountry == NULL || *IssuerCountry != NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid second parameter value. Error=%i",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	err = GetCertificateStructFromX509Certificate(GenBuf, &Certificate);
	if(err < 0)
	{
		PRINT_ERROR("LIBBMDCRLERR Error in getting certificate struct from X509 certificate list. Error=%li\n", err);
		return err;
	}
	err = GetIssuerCountryFromCertificate(Certificate, IssuerCountry);
	if(err < 0)
	{
		PRINT_ERROR("LIBBMDCRLERR Error in getting issuer DN from Certificate. Error=%li\n", err);
		return err;
	}
	asn_DEF_Certificate.free_struct(&asn_DEF_Certificate, Certificate, 0);
	return err;
}

long GetSubjectLocalityFromX509Certificate(GenBuf_t *GenBuf, char **Locality)
{
long err = 0;
Certificate_t *Certificate = NULL;

	PRINT_INFO("LIBBMDX509INF Getting subject locality from certificate\n");
	if(GenBuf == NULL || GenBuf->buf == NULL || GenBuf->size == 0)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid first parameter value. Error=%i",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(Locality == NULL || *Locality != NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid second parameter value. Error=%i",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	err = GetCertificateStructFromX509Certificate(GenBuf, &Certificate);
	if(err < 0)
	{
		PRINT_ERROR("LIBBMDCRLERR Error in getting certificate struct from X509 certificate list. Error=%li\n", err);
		return err;
	}
	err = GetSubjectLocalityFromCertificate(Certificate, Locality);
	if(err < 0)
	{
		PRINT_ERROR("LIBBMDCRLERR Error in getting subject DN from Certificate. Error=%li\n", err);
		return err;
	}
	asn_DEF_Certificate.free_struct(&asn_DEF_Certificate, Certificate, 0);
	return err;
}

long GetIssuerLocalityFromX509Certificate(GenBuf_t *GenBuf, char **Locality)
{
long err = 0;
Certificate_t *Certificate = NULL;

	PRINT_INFO("LIBBMDX509INF Getting issuer locality from certificate\n");
	if(GenBuf == NULL || GenBuf->buf == NULL || GenBuf->size == 0)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid first parameter value. Error=%i",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(Locality == NULL || *Locality != NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid second parameter value. Error=%i",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	err = GetCertificateStructFromX509Certificate(GenBuf, &Certificate);
	if(err < 0)
	{
		PRINT_ERROR("LIBBMDCRLERR Error in getting certificate struct from X509 certificate list. Error=%li\n", err);
		return err;
	}
	err = GetIssuerLocalityFromCertificate(Certificate, Locality);
	if(err < 0)
	{
		PRINT_ERROR("LIBBMDCRLERR Error in getting issuer DN from Certificate. Error=%li\n", err);
		return err;
	}
	asn_DEF_Certificate.free_struct(&asn_DEF_Certificate, Certificate, 0);
	return err;
}

long GetSubjectOrganizationalUnitFromX509Certificate(GenBuf_t *GenBuf, char **OrganizationalUnit)
{
long err = 0;
Certificate_t *Certificate = NULL;

	PRINT_INFO("LIBBMDX509INF Getting subject organizational unit from certificate\n");
	if(GenBuf == NULL || GenBuf->buf == NULL || GenBuf->size == 0)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid first parameter value. Error=%i",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(OrganizationalUnit == NULL || *OrganizationalUnit!= NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid second parameter value. Error=%i",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	err = GetCertificateStructFromX509Certificate(GenBuf, &Certificate);
	if(err < 0)
	{
		PRINT_ERROR("LIBBMDCRLERR Error in getting certificate struct from X509 certificate list. Error=%li\n", err);
		return err;
	}
	err = GetSubjectOrganizationalUnitFromCertificate(Certificate, OrganizationalUnit);
	if(err < 0){
		PRINT_ERROR("LIBBMDCRLERR Error in getting subject DN from Certificate. Error=%li\n", err);
		return err;
	}
	asn_DEF_Certificate.free_struct(&asn_DEF_Certificate, Certificate, 0);
	return err;
}

long GetIssuerOrganizationalUnitFromX509Certificate(GenBuf_t *GenBuf, char **OrganizationalUnit)
{
long err = 0;
Certificate_t *Certificate = NULL;

	PRINT_INFO("LIBBMDX509INF Getting issuer organizational unit from certificate\n");
	if(GenBuf == NULL || GenBuf->buf == NULL || GenBuf->size == 0)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid first parameter value. Error=%i",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(OrganizationalUnit == NULL || *OrganizationalUnit!= NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid second parameter value. Error=%i",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	err = GetCertificateStructFromX509Certificate(GenBuf, &Certificate);
	if(err < 0)
	{
		PRINT_ERROR("LIBBMDCRLERR Error in getting certificate struct from X509 certificate list. Error=%li\n", err);
		return err;
	}
	err = GetIssuerOrganizationalUnitFromCertificate(Certificate, OrganizationalUnit);
	if(err < 0){
		PRINT_ERROR("LIBBMDCRLERR Error in getting issuer DN from Certificate. Error=%li\n", err);
		return err;
	}
	asn_DEF_Certificate.free_struct(&asn_DEF_Certificate, Certificate, 0);
	return err;
}

/************
 * FAST API *
 ***********/
long GetCertificateStructFromX509Certificate(	GenBuf_t *GenBuf,
								Certificate_t **Certificate)
{
long err			= 0;
asn_dec_rval_t rval;

	PRINT_INFO("LIBBMDX509INF Getting certificate structure from certificate\n");
	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if(GenBuf == NULL )			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(GenBuf->buf == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(GenBuf->size <= 0)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(Certificate == NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(*Certificate != NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	/*BMD_FOK(bmd_save_buf(GenBuf, "/tmp/cert_test.der"));*/

	rval = ber_decode(0, &asn_DEF_Certificate,  (void **)Certificate, GenBuf->buf, GenBuf->size);
	if(rval.code != RC_OK)
	{
		PRINT_ERROR("LIBBMDX509ERR Error in ber decoding. Error=%i\n",ERR_X509_DECODE_CERTIFICATE_DATA);
		asn_DEF_Certificate.free_struct(&asn_DEF_Certificate, *Certificate, 0);
		return ERR_X509_DECODE_CERTIFICATE_DATA;
	}
	return err;
}

long GetVersionFromCertificate(Certificate_t *Certificate, long *version)
{
long err = 0;

	PRINT_INFO("LIBBMDX509INF Getting version from certificate\n");
	if(Certificate == NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid first parameter value. Error=%i",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(version == NULL || *version != 0)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid second parameter value. Error=%i",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	if(Certificate->tbsCertificate.version == NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Version field not present. Error=%i",ERR_X509_VERSION_FIELD_NOT_PRESENT);
		return ERR_X509_VERSION_FIELD_NOT_PRESENT;
	}

	err = asn_INTEGER2long(Certificate->tbsCertificate.version, version);
	if(err<0)
	{
		PRINT_ERROR("LIBBMDX509ERR Error converting INTEGER_t to long. Error=%li\n", err);
		return ERR_X509_CONVERT_INTEGER_T_TO_LONG;
	}
	return err;
}


/*Na potrzeby deklaracji zgodnosci. Sprawdzenie wymaganych pol podpiotu i okreslenie kategorii certyfikatu*/
long CheckCertificateSubject(Certificate_t *Certificate, long *category, CertExtConfig_t *twf_CertExtConfig)
{
long err							= 0;
RDNSequence_t *RDNSequence		= NULL;
char *SubjectCountry			= NULL;
char *SubjectSurname			= NULL;
char *SubjectGivenName			= NULL;
char *SubjectSerialNumber		= NULL;
char *SubjectCommonName			= NULL;
char *SubjectPseudonym			= NULL;
char *SubjectOrganisation		= NULL;
char *SubjectState				= NULL;
char *SubjectLocality			= NULL;
char *SubjectPostalAddress		= NULL;

	PRINT_INFO("LIBBMDX509INF Getting subject dnf from certificate\n");
	if(Certificate == NULL)					{		BMD_FOK(BMD_ERR_PARAM1);	}

	if(category == NULL)					{		BMD_FOK(BMD_ERR_PARAM2);	}
	if(*category != 0)						{		BMD_FOK(BMD_ERR_PARAM2);	}

	if(twf_CertExtConfig == NULL)			{		BMD_FOK(BMD_ERR_PARAM3);	}

	err = Name2RDNSequence(&(Certificate->tbsCertificate.subject), &RDNSequence);
	if(err<0)	{		BMD_FOK(ERR_X509_CONVERT_NAME_T_TO_RDNSEQUENCE);	}


	err = GetAttrFromRDNSequence(RDNSequence, RDNSEQ_ATTR_COUNTRY, &SubjectCountry);
	if(err<0)
	{
		SubjectCountry = NULL;
	}

	err = GetAttrFromRDNSequence(RDNSequence, RDNSEQ_ATTR_SURNAME, &SubjectSurname);
	if(err<0)
	{
		SubjectSurname = NULL;
	}

	err = GetAttrFromRDNSequence(RDNSequence, RDNSEQ_ATTR_GIVENNAME, &SubjectGivenName);
	if(err<0)
	{
		SubjectGivenName = NULL;
	}

	err = GetAttrFromRDNSequence(RDNSequence, RDNSEQ_ATTR_SERIALNUMBER, &SubjectSerialNumber);
	if(err<0)
	{
		SubjectSerialNumber = NULL;
	}

	err = GetAttrFromRDNSequence(RDNSequence, RDNSEQ_ATTR_COMMONNAME, &SubjectCommonName);
	if(err<0)
	{
		SubjectCommonName = NULL;
	}

	err = GetAttrFromRDNSequence(RDNSequence, RDNSEQ_ATTR_PSEUDONYM, &SubjectPseudonym);
	if(err<0)
	{
		SubjectPseudonym = NULL;
	}

	/*Ustalenia na temat organizacji*/
	err = GetAttrFromRDNSequence(RDNSequence, RDNSEQ_ATTR_ORGANIZATION, &SubjectOrganisation);
	if(err<0)
	{
		SubjectOrganisation = NULL;
	}

	err = GetAttrFromRDNSequence(RDNSequence, RDNSEQ_ATTR_STATE, &SubjectState);
	if(err<0)
	{
		SubjectState = NULL;
	}

	err = GetAttrFromRDNSequence(RDNSequence, RDNSEQ_ATTR_LOCALITY, &SubjectLocality);
	if(err<0)
	{
		SubjectLocality = NULL;
	}

	err = GetAttrFromRDNSequence(RDNSequence, RDNSEQ_ATTR_POSTALADDRESS, &SubjectPostalAddress);
	if(err<0)
	{
		SubjectPostalAddress = NULL;
	}


	/*Uzycie pseudonimu wyklucza jednoczesne uzycie imienia lub nazwiska*/
	if(twf_CertExtConfig->check_subject_name_with_pseudonym == 1)
	{
		if(SubjectPseudonym != NULL && (SubjectSurname != NULL || SubjectGivenName != NULL))
		{
			BMD_FOK(ERR_X509_PSEUDONYM_WITH_NAME);
		}
	}

	/*Zgodnie z rozporzadzeniem jezeli w podmiocie uzyto nazwy organizacji to musi byc rowniez uzyta nazwa
	wojewodztwa, nazw miejscowosci oraz adres*/
	if(twf_CertExtConfig->check_subject_org_with_address == 1)
	{
		if(SubjectOrganisation != NULL && (SubjectState == NULL || SubjectLocality == NULL || SubjectPostalAddress == NULL))
		{
			BMD_FOK(ERR_X509_ORGANISATION_WITHOUT_ADDRESS);
		}
	}

	/*Ustalanie kategorii rozporzadzenia wzgledem certyfikatu*/
	if(SubjectCountry != NULL && SubjectSurname != NULL && SubjectGivenName != NULL && SubjectSerialNumber != NULL)
	{
		*category = 1;
		return BMD_OK;
	}

	if(SubjectCountry != NULL && SubjectCommonName != NULL && SubjectSerialNumber != NULL)
	{
		*category = 2;
		return BMD_OK;
	}

	if(SubjectCountry != NULL && SubjectPseudonym != NULL)
	{
		*category = 3;
		return BMD_OK;
	}

	*category = 0;

	asn_DEF_RDNSequence.free_struct(&asn_DEF_RDNSequence, RDNSequence, 0);
	return BMD_OK;
}


/*Na potrzeby deklaracji zgodnosci. Sprawdzenie rozszerzen certyfikatow.*/
long CheckCertificateExtensions(Certificate_t *Certificate, CertExtConfig_t *twf_CertExtConfig)
{
	long extension_count							= 0;
	long iter										= 0;
	long inner_iter									= 0;
	long ret_val									= 0;
	char* subjectkeyidentifier_oid					= "2.5.29.14";
	char* keyusage_oid								= "2.5.29.15"; //oid : extension keyusage
	char* extendedkeyusage_oid						= "2.5.29.37";
	char* anyextendedkeyusage_oid					= "2.5.29.37.0"; //Rozszerzone uzycie klucza - kazde uzycie
	char* certificatepolicies_oid					= "2.5.29.32"; /*Polityka certyfikacji*/
	char* subjectaltname_oid						= "2.5.29.17"; /*Alternatywna nazwa podmiotu*/
	char* basicconstraints_oid						= "2.5.29.19"; /*Podstawowe ograniczenia*/
	char* qcstatements_oid							= "1.3.6.1.5.5.7.1.3";

	ExtKeyUsageSyntax_t *ExtKeyUsageSyntax		= NULL;
	BasicConstraints_t *BasicConstraints		= NULL;
	asn_dec_rval_t dec_result;

	if(Certificate == NULL)								{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(twf_CertExtConfig == NULL)						{	BMD_FOK(BMD_ERR_PARAM2);	}


	if(Certificate->tbsCertificate.extensions == NULL)
	{
		if(twf_CertExtConfig->check_extensions_existence == 1)
		{
			BMD_FOK(ERR_X509_NO_EXTENSIONS);
		}
		else
		{
			return BMD_OK;
		}
	}

	extension_count=Certificate->tbsCertificate.extensions->list.count;

	/*Sprawdzam czy nie ma rozszerzen zakazanych*/
	if(twf_CertExtConfig->check_forbidden_extensions == 1)
	{
		for(iter=0; iter < extension_count; iter++)
		{
			ret_val=OBJECT_IDENTIFIER_cmp(&(Certificate->tbsCertificate.extensions->list.array[iter]->extnID), subjectkeyidentifier_oid);
			if(ret_val == 0)
			{
				BMD_FOK(ERR_X509_SUBJECT_KEY_IDENTIFIER_EXISTS);
			}
		}
	}


	/*Sprawdzam czy rozumiemy rozszerzenia krytyczne*/
	if(twf_CertExtConfig->check_critical_extensions == 1)
	{/*if(twf_CertExtConfig->check_critical_extensions == 1)*/
		for(iter=0; iter < extension_count; iter++)
		{/*for(iter=0; iter < extension_count; iter++)*/
			if(	Certificate->tbsCertificate.extensions->list.array[iter]->critical != NULL)
			{
				if( *(Certificate->tbsCertificate.extensions->list.array[iter]->critical) != 0)
				{/*Critical*/

					ret_val=OBJECT_IDENTIFIER_cmp(&(Certificate->tbsCertificate.extensions->list.array[iter]->extnID), keyusage_oid);
					if(ret_val == 0)
					{/*keyusage_oid*/
						/*Tym rozszerzeniem zajmuje sie dokladnie funkcja X509_is_non_repudiation*/
						continue;
					}/*\keyusage_oid*/


					/*Sprawdzam rozszerzone uzycie klucza*/
					ret_val=OBJECT_IDENTIFIER_cmp(&(Certificate->tbsCertificate.extensions->list.array[iter]->extnID), extendedkeyusage_oid);
					if(ret_val == 0)
					{/*Sprawdzam rozszerzone uzycie klucza*/
						dec_result=ber_decode(0, &asn_DEF_ExtKeyUsageSyntax, (void**)&ExtKeyUsageSyntax, Certificate->tbsCertificate.extensions->list.array[iter]->extnValue.buf, Certificate->tbsCertificate.extensions->list.array[iter]->extnValue.size);
						if(dec_result.code != RC_OK)
						{
							BMD_FOK(ERR_X509_DECODE_EXTENDED_KEY_USAGE);
						}

						/*Sprawdzam poszczegolne oid rozszerzonego uzycia klucza*/
						if(twf_CertExtConfig->check_extended_key_usage == 1)
						{/*if(twf_CertExtConfig->check_extended_key_usage == 1)*/
							for(inner_iter = 0; inner_iter < ExtKeyUsageSyntax->list.count; inner_iter++)
							{
								ret_val=OBJECT_IDENTIFIER_cmp( ExtKeyUsageSyntax->list.array[inner_iter],  anyextendedkeyusage_oid);
								if(ret_val == 0)
								{
									/*Takie uĹĽycie klucza jeszcze mozna zastosowac do podpisywania*/
									continue;
								}
								else
								{
									/*Innych rozszerzonych uzyc klucza nie przyjmujemy*/
									BMD_FOK(ERR_X509_NOT_SUPPORTED_EXTENDED_KEY_USAGE);
								}
							}
						}/*\if(twf_CertExtConfig->check_extended_key_usage == 1)*/

						continue;
					}/*\Sprawdzam rozszerzone uzycie klucza*/


					ret_val=OBJECT_IDENTIFIER_cmp(&(Certificate->tbsCertificate.extensions->list.array[iter]->extnID), certificatepolicies_oid);
					if(ret_val == 0)
					{/*certificatepolicies_oid*/
						/*To rozszerzenie rozumiemy lecz nic z nim nie robimy*/
						continue;
					}/*\certificatepolicies_oid*/


					ret_val=OBJECT_IDENTIFIER_cmp(&(Certificate->tbsCertificate.extensions->list.array[iter]->extnID), subjectaltname_oid);
					if(ret_val == 0)
					{/*subjectaltname_oid*/
						/*To rozszerzenie rozumiemy lecz nic z nim nie robimy*/
						continue;
					}/*\subjectaltname_oid*/

					ret_val=OBJECT_IDENTIFIER_cmp(&(Certificate->tbsCertificate.extensions->list.array[iter]->extnID), basicconstraints_oid);
					if(ret_val == 0)
					{/*basicconstraints_oid*/
						/*To rozszerzenie rozumiemy powinno byc false*/
						dec_result=ber_decode(0, &asn_DEF_BasicConstraints, (void**)&BasicConstraints, Certificate->tbsCertificate.extensions->list.array[iter]->extnValue.buf, Certificate->tbsCertificate.extensions->list.array[iter]->extnValue.size);
						if(dec_result.code != RC_OK)
						{
							BMD_FOK(ERR_X509_DECODE_BASIC_CONSTRAINTS);
						}
						if(BasicConstraints->cA != NULL)
						{
							if(*(BasicConstraints->cA) != 0)
							{
								BMD_FOK(ERR_X509_BASIC_CONSTRAINTS_TRUE);
							}
						}

						continue;
					}/*\basicconstraints_oid*/


					ret_val=OBJECT_IDENTIFIER_cmp(&(Certificate->tbsCertificate.extensions->list.array[iter]->extnID), qcstatements_oid);
					if(ret_val == 0)
					{/*qcstatements_oid*/
						/*To rozszerzenie rozumiemy lecz nic z nim nie robimy*/
						continue;
					}/*\qcstatements_oid*/

					if(twf_CertExtConfig->raport_non_supported_critical == 1)
					{
						BMD_FOK(ERR_X509_NOT_SUPPORTED_CRITICAL_EXTENSION);
					}
				}/*\Critical*/
			}

		}/*\for(iter=0; iter < extension_count; iter++)*/
	}/*\if(twf_CertExtConfig->check_critical_extensions == 1)*/


	/*Sprawdzam czy rozszerzenia ktore powinny byc krytyczne nie sa oznaczone jako niekrytyczne.*/
	if(twf_CertExtConfig->check_wrongly_non_critical == 1)
	{/*if(twf_CertExtConfig->check_wrongly_non_critical == 1)*/
		for(iter=0; iter < extension_count; iter++)
		{/*for(iter=0; iter < extension_count; iter++)*/
			if(Certificate->tbsCertificate.extensions->list.array[iter]->critical != NULL)
			{
				if( *(Certificate->tbsCertificate.extensions->list.array[iter]->critical) != 0)
				{/*Critical*/
					continue;
				}
			}

			ret_val=OBJECT_IDENTIFIER_cmp(&(Certificate->tbsCertificate.extensions->list.array[iter]->extnID), keyusage_oid);
			if(ret_val == 0)
			{/*keyusage_oid*/
				continue;
			}/*\keyusage_oid*/

			ret_val=OBJECT_IDENTIFIER_cmp(&(Certificate->tbsCertificate.extensions->list.array[iter]->extnID), extendedkeyusage_oid);
			if(ret_val == 0)
			{/*extendedkeyusage_oid*/
				continue;
			}/*\extendedkeyusage_oid*/

			ret_val=OBJECT_IDENTIFIER_cmp(&(Certificate->tbsCertificate.extensions->list.array[iter]->extnID), certificatepolicies_oid);
			if(ret_val == 0)
			{/*certificatepolicies_oid*/
				/*To rozszerzenie rozumiemy lecz nic z nim nie robimy*/
				continue;
			}/*\certificatepolicies_oid*/

			ret_val=OBJECT_IDENTIFIER_cmp(&(Certificate->tbsCertificate.extensions->list.array[iter]->extnID), basicconstraints_oid);
			if(ret_val == 0)
			{/*basicconstraints_oid*/
				continue;
			}/*basicconstraints_oid*/

		}/*\for(iter=0; iter < extension_count; iter++)*/
	}/*\if(twf_CertExtConfig->check_wrongly_non_critical == 1)*/


	return BMD_OK;
}


long GetSerialNumberFromCertificate(Certificate_t *Certificate, char **serialNumber)
{
long err = 0;

	PRINT_INFO("LIBBMDX509INF Getting version from certificate\n");
	if(Certificate == NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid first parameter value. Error=%i",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(serialNumber == NULL || *serialNumber != NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid second parameter value. Error=%i",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	err = INTEGER2string(&(Certificate->tbsCertificate.serialNumber), serialNumber);
	if(err<0)
	{
		PRINT_ERROR("LIBBMDX509ERR Error converting INTEGER_t to string. Error=%i\n",ERR_X509_CONVERT_INTEGER_T_TO_STRING);
		return ERR_X509_CONVERT_INTEGER_T_TO_STRING;
	}
	return err;
}


long GetSubjectSerialNumberFromCertificate(Certificate_t *Certificate, char **serialNumber)
{
long err = 0;
RDNSequence_t *RDNSequence = NULL;

	PRINT_INFO("LIBBMDX509INF Getting subject serial number from certificate\n");
	if(Certificate == NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid first parameter value. Error=%i",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(serialNumber == NULL || *serialNumber != NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid second parameter value. Error=%i",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	err = Name2RDNSequence(&(Certificate->tbsCertificate.subject), &RDNSequence);
	if(err<0)
	{
		PRINT_ERROR("LIBBMDX509ERR Error converting Name_t to RDNSequence_t. Error=%i\n",ERR_X509_CONVERT_NAME_T_TO_RDNSEQUENCE);
		return ERR_X509_CONVERT_NAME_T_TO_RDNSEQUENCE;
	}
	err = GetAttrFromRDNSequence(RDNSequence, RDNSEQ_ATTR_SERIALNUMBER, serialNumber);
	if(err<0)
	{
		PRINT_ERROR("LIBBMDX509ERR Error converting RDNSequence to serial number string. Error=%i\n",ERR_X509_GET_SERIALNUMBER_FROM_RDNSEQUENCE);
		return ERR_X509_GET_SERIALNUMBER_FROM_RDNSEQUENCE;
	}
	asn_DEF_RDNSequence.free_struct(&asn_DEF_RDNSequence, RDNSequence, 0);

	return err;
}

long GetIssuerSerialNumberFromCertificate(Certificate_t *Certificate, char **serialNumber)
{
long err = 0;
RDNSequence_t *RDNSequence = NULL;

	PRINT_INFO("LIBBMDX509INF Getting issuer serial number from certificate\n");
	if(Certificate == NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid first parameter value. Error=%i",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(serialNumber == NULL || *serialNumber != NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid second parameter value. Error=%i",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	err = Name2RDNSequence(&(Certificate->tbsCertificate.issuer), &RDNSequence);
	if(err<0)
	{
		PRINT_ERROR("LIBBMDX509ERR Error converting Name_t to RDNSequence_t. Error=%i\n",ERR_X509_CONVERT_NAME_T_TO_RDNSEQUENCE);
		return ERR_X509_CONVERT_NAME_T_TO_RDNSEQUENCE;
	}
	err = GetAttrFromRDNSequence(RDNSequence, RDNSEQ_ATTR_SERIALNUMBER, serialNumber);
	if(err<0)
	{
		PRINT_ERROR("LIBBMDX509ERR Error converting RDNSequence to serial number string. Error=%i\n",ERR_X509_GET_SERIALNUMBER_FROM_RDNSEQUENCE);
		return ERR_X509_GET_SERIALNUMBER_FROM_RDNSEQUENCE;
	}
	asn_DEF_RDNSequence.free_struct(&asn_DEF_RDNSequence, RDNSequence, 0);

	return err;
}


long GetSubjectDNFromCertificate(Certificate_t *Certificate, char **SubjectDN)
{
long err = 0;

	PRINT_INFO("LIBBMDX509INF Getting subject dnf from certificate\n");
	if(Certificate == NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid first parameter value. Error=%i",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(SubjectDN == NULL || *SubjectDN != NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid second parameter value. Error=%i",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}
	err = Name2sortedString(&(Certificate->tbsCertificate.subject), SubjectDN);
	if(err<0)
	{
		PRINT_ERROR("LIBBMDX509ERR Error converting Name_t to string. Error=%i\n",ERR_X509_CONVERT_NAME_T_TO_STRING);
		return ERR_X509_CONVERT_NAME_T_TO_STRING;
	}
	return err;
}

long GetSubjectSurnameFromCertificate(Certificate_t *Certificate, char **SubjectSurname)
{
long err = 0;
RDNSequence_t *RDNSequence = NULL;

	PRINT_INFO("LIBBMDX509INF Getting subject surname from certificate\n");
	if(Certificate == NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid first parameter value. Error=%i",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(SubjectSurname == NULL || *SubjectSurname != NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid second parameter value. Error=%i",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	err = Name2RDNSequence(&(Certificate->tbsCertificate.subject), &RDNSequence);
	if(err<0)
	{
		PRINT_ERROR("LIBBMDX509ERR Error converting Name_t to RDNSequence_t. Error=%i\n",ERR_X509_CONVERT_NAME_T_TO_RDNSEQUENCE);
		return ERR_X509_CONVERT_NAME_T_TO_RDNSEQUENCE;
	}
	err = GetAttrFromRDNSequence(RDNSequence, RDNSEQ_ATTR_SURNAME, SubjectSurname);
	if(err<0)
	{
		PRINT_ERROR("LIBBMDX509ERR Error converting RDNSequence to Surname string. Error=%i\n",ERR_X509_GET_SURNAME_FROM_RDNSEQUENCE);
		return ERR_X509_GET_SURNAME_FROM_RDNSEQUENCE;
	}
	asn_DEF_RDNSequence.free_struct(&asn_DEF_RDNSequence, RDNSequence, 0);
	return err;
}

long GetSubjectGivenNameFromCertificate(Certificate_t *Certificate, char **SubjectGivenName)
{
long err = 0;
RDNSequence_t *RDNSequence = NULL;

	PRINT_INFO("LIBBMDX509INF Getting subject given name from certificate\n");
	if(Certificate == NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid first parameter value. Error=%i",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(SubjectGivenName == NULL || *SubjectGivenName != NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid second parameter value. Error=%i",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	err = Name2RDNSequence(&(Certificate->tbsCertificate.subject), &RDNSequence);
	if(err<0)
	{
		PRINT_ERROR("LIBBMDX509ERR Error converting Name_t to RDNSequence_t. Error=%i\n",ERR_X509_CONVERT_NAME_T_TO_RDNSEQUENCE);
		return ERR_X509_CONVERT_NAME_T_TO_RDNSEQUENCE;
	}
	err = GetAttrFromRDNSequence(RDNSequence, RDNSEQ_ATTR_GIVENNAME, SubjectGivenName);
	if(err<0)
	{
		PRINT_ERROR("LIBBMDX509ERR Error converting RDNSequence to GivenName string. Error=%i\n",ERR_X509_GET_GIVENNAME_FROM_RDNSEQUENCE);
		return ERR_X509_GET_GIVENNAME_FROM_RDNSEQUENCE;
	}
	asn_DEF_RDNSequence.free_struct(&asn_DEF_RDNSequence, RDNSequence, 0);
	return err;
}

long GetSubjectCommonNameFromCertificate(Certificate_t *Certificate, char **SubjectCommonName)
{
long err = 0;
RDNSequence_t *RDNSequence = NULL;


	PRINT_INFO("LIBBMDX509INF Getting subject given name from certificate\n");
	if(Certificate == NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid first parameter value. Error=%i",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(SubjectCommonName == NULL || *SubjectCommonName != NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid second parameter value. Error=%i",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	err = Name2RDNSequence(&(Certificate->tbsCertificate.subject), &RDNSequence);
	if(err<0)
	{
		PRINT_ERROR("LIBBMDX509ERR Error converting Name_t to RDNSequence_t. Error=%i\n",ERR_X509_CONVERT_NAME_T_TO_RDNSEQUENCE);
		return ERR_X509_CONVERT_NAME_T_TO_RDNSEQUENCE;
	}
	err = GetAttrFromRDNSequence(RDNSequence, RDNSEQ_ATTR_COMMONNAME, SubjectCommonName);
	if(err<0)
	{
		PRINT_ERROR("LIBBMDX509ERR Error converting RDNSequence to CommonName string. Error=%i\n",ERR_X509_GET_COMMONNAME_FROM_RDNSEQUENCE);
		return ERR_X509_GET_COMMONNAME_FROM_RDNSEQUENCE;
	}
	asn_DEF_RDNSequence.free_struct(&asn_DEF_RDNSequence, RDNSequence, 0);
	return err;
}

long GetSubjectOrganizationFromCertificate(Certificate_t *Certificate, char **SubjectOrganization)
{
long err = 0;
RDNSequence_t *RDNSequence = NULL;

	PRINT_INFO("LIBBMDX509INF Getting subject organization from certificate\n");
	if(Certificate == NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid first parameter value. Error=%i",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(SubjectOrganization == NULL || *SubjectOrganization != NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid second parameter value. Error=%i",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	err = Name2RDNSequence(&(Certificate->tbsCertificate.subject), &RDNSequence);
	if(err<0)
	{
		PRINT_ERROR("LIBBMDX509ERR Error converting Name_t to RDNSequence_t. Error=%i\n",ERR_X509_CONVERT_NAME_T_TO_RDNSEQUENCE);
		return ERR_X509_CONVERT_NAME_T_TO_RDNSEQUENCE;
	}
	err = GetAttrFromRDNSequence(RDNSequence, RDNSEQ_ATTR_ORGANIZATION, SubjectOrganization);
	if(err<0)
	{
		PRINT_ERROR("LIBBMDX509ERR Error converting RDNSequence to Organization string. Error=%i\n",ERR_X509_GET_ORGANIZATION_FROM_RDNSEQUENCE);
		return ERR_X509_GET_ORGANIZATION_FROM_RDNSEQUENCE;
	}
	asn_DEF_RDNSequence.free_struct(&asn_DEF_RDNSequence, RDNSequence, 0);
	return err;
}

long GetIssuerOrganizationFromCertificate(Certificate_t *Certificate, char **IssuerOrganization)
{
long err = 0;
RDNSequence_t *RDNSequence = NULL;

	PRINT_INFO("LIBBMDX509INF Getting issuer organization from certificate\n");
	if(Certificate == NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid first parameter value. Error=%i",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(IssuerOrganization == NULL || *IssuerOrganization != NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid second parameter value. Error=%i",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	err = Name2RDNSequence(&(Certificate->tbsCertificate.issuer), &RDNSequence);
	if(err<0)
	{
		PRINT_ERROR("LIBBMDX509ERR Error converting Name_t to RDNSequence_t. Error=%i\n",ERR_X509_CONVERT_NAME_T_TO_RDNSEQUENCE);
		return ERR_X509_CONVERT_NAME_T_TO_RDNSEQUENCE;
	}
	err = GetAttrFromRDNSequence(RDNSequence, RDNSEQ_ATTR_ORGANIZATION, IssuerOrganization);
	if(err<0)
	{
		PRINT_ERROR("LIBBMDX509ERR Error converting RDNSequence to Organization string. Error=%i\n",ERR_X509_GET_ORGANIZATION_FROM_RDNSEQUENCE);
		return ERR_X509_GET_ORGANIZATION_FROM_RDNSEQUENCE;
	}
	asn_DEF_RDNSequence.free_struct(&asn_DEF_RDNSequence, RDNSequence, 0);
	return err;
}

long GetIssuerCommonNameFromCertificate(Certificate_t *Certificate, char **IssuerCommonName)
{
long err = 0;
RDNSequence_t *RDNSequence = NULL;

	PRINT_INFO("LIBBMDX509INF Getting issuer common name from certificate\n");
	if(Certificate == NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid first parameter value. Error=%i",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(IssuerCommonName == NULL || *IssuerCommonName != NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid first parameter value. Error=%i",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	err = Name2RDNSequence(&(Certificate->tbsCertificate.issuer), &RDNSequence);
	if(err<0)
	{
		PRINT_ERROR("LIBBMDX509ERR Error converting Name_t to RDNSequence_t. Error=%i",ERR_X509_CONVERT_NAME_T_TO_RDNSEQUENCE);
		return ERR_X509_CONVERT_NAME_T_TO_RDNSEQUENCE;
	}
	err = GetAttrFromRDNSequence(RDNSequence, RDNSEQ_ATTR_COMMONNAME, IssuerCommonName);
	if(err<0)
	{
		PRINT_ERROR("LIBBMDX509ERR Error converting RDNSequence to CommonName string. Error=%i\n",ERR_X509_GET_COMMONNAME_FROM_RDNSEQUENCE);
		return ERR_X509_GET_COMMONNAME_FROM_RDNSEQUENCE;
	}
	asn_DEF_RDNSequence.free_struct(&asn_DEF_RDNSequence, RDNSequence, 0);
	return err;
}

long GetIssuerDNFromCertificate(	Certificate_t *Certificate,
					char **IssuerDN)
{
	PRINT_INFO("LIBBMDX509INF Getting issuer DNF from certificate\n");
	/************************/
	/* walidacja parametrow */
	/************************/
	if (Certificate == NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (IssuerDN == NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (*IssuerDN != NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	BMD_FOK_CHG(Name2sortedString(&(Certificate->tbsCertificate.issuer), IssuerDN), ERR_X509_CONVERT_NAME_T_TO_STRING);

	return BMD_OK;
}

long GetSubjectCountryFromCertificate(Certificate_t *Certificate, char **Country)
{
long err = 0;
RDNSequence_t *RDNSequence = NULL;

	PRINT_INFO("LIBBMDX509INF Getting subject country from certificate\n");
	if(Certificate == NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid first parameter value. Error=%i",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(Country == NULL || *Country != NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid second parameter value. Error=%i",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	err = Name2RDNSequence(&(Certificate->tbsCertificate.subject), &RDNSequence);
	if(err<0)
	{
		PRINT_ERROR("LIBBMDX509ERR Error converting Name_t to RDNSequence_t. Error=%i\n",ERR_X509_CONVERT_NAME_T_TO_RDNSEQUENCE);
		return ERR_X509_CONVERT_NAME_T_TO_RDNSEQUENCE;
	}
	err = GetAttrFromRDNSequence(RDNSequence, RDNSEQ_ATTR_COUNTRY, Country);
	if(err<0)
	{
		PRINT_ERROR("LIBBMDX509ERR Error converting RDNSequence to CommonName string. Error=%i\n",ERR_X509_GET_COUNTRY_FROM_RDNSEQUENCE);
		return ERR_X509_GET_COUNTRY_FROM_RDNSEQUENCE;
	}
	asn_DEF_RDNSequence.free_struct(&asn_DEF_RDNSequence, RDNSequence, 0);
	return err;
}

long GetIssuerCountryFromCertificate(Certificate_t *Certificate, char **Country)
{
long err = 0;
RDNSequence_t *RDNSequence = NULL;

	PRINT_INFO("LIBBMDX509INF Getting issuer country from certificate\n");
	if(Certificate == NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid first parameter value. Error=%i",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(Country == NULL || *Country != NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid second parameter value. Error=%i",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	err = Name2RDNSequence(&(Certificate->tbsCertificate.issuer), &RDNSequence);
	if(err<0)
	{
		PRINT_ERROR("LIBBMDX509ERR Error converting Name_t to RDNSequence_t. Error=%i\n",ERR_X509_CONVERT_NAME_T_TO_RDNSEQUENCE);
		return ERR_X509_CONVERT_NAME_T_TO_RDNSEQUENCE;
	}
	err = GetAttrFromRDNSequence(RDNSequence, RDNSEQ_ATTR_COUNTRY, Country);
	if(err<0)
	{
		PRINT_ERROR("LIBBMDX509ERR Error converting RDNSequence to CommonName string. Error=%i\n",ERR_X509_GET_COUNTRY_FROM_RDNSEQUENCE);
		return ERR_X509_GET_COUNTRY_FROM_RDNSEQUENCE;
	}
	asn_DEF_RDNSequence.free_struct(&asn_DEF_RDNSequence, RDNSequence, 0);
	return err;
}

long GetSubjectLocalityFromCertificate(Certificate_t *Certificate, char **Locality)
{
long err = 0;
RDNSequence_t *RDNSequence = NULL;

	PRINT_INFO("LIBBMDX509INF Getting subject locality from certificate\n");
	if(Certificate == NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid first parameter value. Error=%i",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(Locality == NULL || *Locality != NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid second parameter value. Error=%i",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	err = Name2RDNSequence(&(Certificate->tbsCertificate.subject), &RDNSequence);
	if(err<0){
		PRINT_ERROR("LIBBMDX509ERR Error converting Name_t to RDNSequence_t.\n");
		return ERR_X509_CONVERT_NAME_T_TO_RDNSEQUENCE;
	}
	err = GetAttrFromRDNSequence(RDNSequence, RDNSEQ_ATTR_LOCALITY, Locality);
	if(err<0){
		PRINT_ERROR("LIBBMDX509ERR Error converting RDNSequence to CommonName string.\n");
		return ERR_X509_GET_LOCALITY_FROM_RDNSEQUENCE;
	}
	asn_DEF_RDNSequence.free_struct(&asn_DEF_RDNSequence, RDNSequence, 0);
	return err;
}

long GetIssuerLocalityFromCertificate(Certificate_t *Certificate, char **Locality)
{
long err = 0;
RDNSequence_t *RDNSequence = NULL;

	PRINT_INFO("LIBBMDX509INF Getting issuer locality from certificate\n");
	if(Certificate == NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid first parameter value. Error=%i",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(Locality == NULL || *Locality != NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid second parameter value. Error=%i",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	err = Name2RDNSequence(&(Certificate->tbsCertificate.issuer), &RDNSequence);
	if(err<0){
		PRINT_ERROR("LIBBMDX509ERR Error converting Name_t to RDNSequence_t.\n");
		return ERR_X509_CONVERT_NAME_T_TO_RDNSEQUENCE;
	}
	err = GetAttrFromRDNSequence(RDNSequence, RDNSEQ_ATTR_LOCALITY, Locality);
	if(err<0){
		PRINT_ERROR("LIBBMDX509ERR Error converting RDNSequence to CommonName string.\n");
		return ERR_X509_GET_LOCALITY_FROM_RDNSEQUENCE;
	}
	asn_DEF_RDNSequence.free_struct(&asn_DEF_RDNSequence, RDNSequence, 0);
	return err;
}

long GetSubjectOrganizationalUnitFromCertificate(Certificate_t *Certificate, char **OrganizationalUnit)
{
long err = 0;
RDNSequence_t *RDNSequence = NULL;

	PRINT_INFO("LIBBMDX509INF Getting subject organizational unit from certificate\n");
	if(Certificate == NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid first parameter value. Error=%i",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(OrganizationalUnit == NULL || *OrganizationalUnit != NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid second parameter value. Error=%i",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	err = Name2RDNSequence(&(Certificate->tbsCertificate.subject), &RDNSequence);
	if(err<0)
	{
		PRINT_ERROR("LIBBMDX509ERR Error converting Name_t to RDNSequence_t.\n");
		return ERR_X509_CONVERT_NAME_T_TO_RDNSEQUENCE;
	}
	err = GetAttrFromRDNSequence(RDNSequence, RDNSEQ_ATTR_ORGANIZATIONALUNIT, OrganizationalUnit);
	if(err<0)
	{
		PRINT_ERROR("LIBBMDX509ERR Error converting RDNSequence to CommonName string.\n");
		return ERR_X509_GET_ORGANIZATIONALUNIT_FROM_RDNSEQUENCE;
	}
	asn_DEF_RDNSequence.free_struct(&asn_DEF_RDNSequence, RDNSequence, 0);
	return err;
}


long GetIssuerOrganizationalUnitFromCertificate(Certificate_t *Certificate, char **OrganizationalUnit)
{
long err = 0;
RDNSequence_t *RDNSequence = NULL;

	PRINT_INFO("LIBBMDX509INF Getting subject organizational unit from certificate\n");
	if(Certificate == NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid first parameter value. Error=%i",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(OrganizationalUnit == NULL || *OrganizationalUnit != NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid second parameter value. Error=%i",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	err = Name2RDNSequence(&(Certificate->tbsCertificate.subject), &RDNSequence);
	if(err<0)
	{
		PRINT_ERROR("LIBBMDX509ERR Error converting Name_t to RDNSequence_t.\n");
		return ERR_X509_CONVERT_NAME_T_TO_RDNSEQUENCE;
	}
	err = GetAttrFromRDNSequence(RDNSequence, RDNSEQ_ATTR_ORGANIZATIONALUNIT, OrganizationalUnit);
	if(err<0)
	{
		PRINT_ERROR("LIBBMDX509ERR Error converting RDNSequence to CommonName string.\n");
		return ERR_X509_GET_ORGANIZATIONALUNIT_FROM_RDNSEQUENCE;
	}
	asn_DEF_RDNSequence.free_struct(&asn_DEF_RDNSequence, RDNSequence, 0);
	return err;
}

long GetValidityNAFromCertificate(Certificate_t *Certificate, char **CertValidityNA)
{
	PRINT_INFO("LIBBMDX509INF Getting subject validity NA from certificate\n");

	if (Certificate == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (CertValidityNA == NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (*CertValidityNA != 0)			{	BMD_FOK(BMD_ERR_PARAM2);	}

	BMD_FOK_CHG(Time2string(&(Certificate->tbsCertificate.validity.notAfter), CertValidityNA),
			ERR_X509_GETTING_VALIDITY_NA_TIME);

	return BMD_OK;
}

long GetValidityNBFromCertificate(Certificate_t *Certificate, char **CertValidityNB)
{
long err = 0;

	PRINT_INFO("LIBBMDX509INF Getting subject validity NB from certificate\n");
	if(Certificate == NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid first parameter value. Error=%i",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(CertValidityNB == NULL || *CertValidityNB != 0)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid second parameter value. Error=%i",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	err = Time2string(&(Certificate->tbsCertificate.validity.notBefore), CertValidityNB);
	if(err < 0)
	{
		PRINT_ERROR("LIBBMDX509ERR Error in getting validity time (notBefore) from Certificate.\n");
		return ERR_X509_GETTING_VALIDITY_NB_TIME;
	}
	return err;
}

long GetAlgorithmOIDFromCertificate(Certificate_t *Certificate, char **SignatureAlgorithmOid)
{
long err = 0;

	PRINT_INFO("LIBBMDX509INF Getting algorithm OID from certificate\n");
	if(Certificate == NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid first parameter value. Error=%i",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(SignatureAlgorithmOid == NULL || *SignatureAlgorithmOid != NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid second parameter value. Error=%i",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	err = OBJECT_IDENTIFIER2string(&(Certificate->tbsCertificate.signature.algorithm), SignatureAlgorithmOid);
	if(err<0)
	{
		PRINT_ERROR("LIBBMDX509ERR Error converting OBJECT_IDENTIFIER_t to string.\n");
		return err;
	}
	return err;
}

long GetExtCRLDistPointsFromCertificate(Certificate_t *Certificate, char **CRLDistPointsString)
{
long err = 0;
long AttributeLong = 0;
Extensions_t *Extensions = NULL;

	PRINT_INFO("LIBBMDX509INF Getting ExtCRLDistPoints from certificate\n");
	if(Certificate == NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid first parameter value. Error=%i",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(CRLDistPointsString == NULL || *CRLDistPointsString != NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid second parameter value. Error=%i",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	err = GetExtensionsFromCertificate(Certificate, &Extensions);
	if(err<0)
	{
		PRINT_ERROR("LIBBMDX509ERR Error in getting extensions field from certificate. Error=%li\n", err);
		return err;
	}
	err = GetCertificateExtension(Extensions, CERT_EXT_CRL_DP, CRLDistPointsString, &AttributeLong);
	if(err<0)
	{
		PRINT_ERROR("LIBBMDX509ERR Error in getting CRL distribution points from certificate extensions filed. Error=%li\n", err);
		return err;
	}
	return err;
}

long GetExtensionsFromCertificate(Certificate_t *Certificate, Extensions_t **Extensions)
{
long err = 0;

	PRINT_INFO("LIBBMDX509INF Getting extensions from certificate\n");
	if(Certificate == NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid first parameter value. Error=%i",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(Extensions == NULL || *Extensions != NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid second parameter value. Error=%i",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	if(Certificate->tbsCertificate.extensions == NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Certificate missing. Error=%i",ERR_X509_CERTIFICATE_EXT_NOT_PRESENT);
		return ERR_X509_CERTIFICATE_EXT_NOT_PRESENT;
	}

	*Extensions = Certificate->tbsCertificate.extensions;
	return err;
}

/*added by WSZ*/
long GetSubjectPublicKeyInfoAlgorithmFromCertificate(Certificate_t *certificate, OBJECT_IDENTIFIER_t **oid)
{
	long status;

	if(certificate == NULL)
		{ return -1; }
	if(oid == NULL)
		{ return -2; }
	if(*oid != NULL)
		{ return -3; }

	status=asn_cloneContent(&asn_DEF_OBJECT_IDENTIFIER,
		                    &(certificate->tbsCertificate.subjectPublicKeyInfo.algorithm.algorithm),
					        (void **)oid);
	if( status != 0 )
		return BMD_ERR_OP_FAILED;

	return BMD_OK;
}



long GetCertificateExtension(Extensions_t *Extensions, X509CertExtType_t X509CertExtType,
				char **ExtensionString, long *ExtensionLong)
{
long err = 0;

	PRINT_INFO("LIBBMDX509INF Getting certificate extension\n");
	if(Extensions == NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid first parameter value. Error=%i",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(X509CertExtType < 0)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid second parameter value. Error=%i",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}
	if(ExtensionString == NULL || *ExtensionString != NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid third parameter value. Error=%i",BMD_ERR_PARAM3);
		return BMD_ERR_PARAM3;
	}
	if(ExtensionLong == NULL || *ExtensionLong < 0)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid fourth parameter value. Error=%i",BMD_ERR_PARAM4);
		return BMD_ERR_PARAM4;
	}
	switch(X509CertExtType)
	{
		case CERT_EXT_CRL_DP:
			err = GetCRLDistPointsFromCRLExtension(Extensions, ExtensionString);
			if(err<0)
			{
				PRINT_ERROR("LIBBMDX509ERR Error in getting CRL distribution points from Extensions. Error=%li\n", err);
				return err;
			}
			break;
		default:
			PRINT_ERROR("LIBBMDX509ERR Unrecognized Certificate extension type!\n");
			return ERR_X509_UNRECOGNIZED_EXT_TYPE;
			break;
	}
	return err;
}

long GetCRLDistPointsFromCRLExtension(Extensions_t *Extensions, char **CRLDistPoints)
{
long err 			= 0;
long i 				= 0;
X509CertExtType_t X509CertExtType = CERT_EXT_CRL_DP;
long CRLDistPointsExtFound	= 0;

	if(Extensions == NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid first parameter value. Error=%i",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(CRLDistPoints == NULL || *CRLDistPoints != NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid second parameter value. Error=%i",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	for(i=0; i<Extensions->list.count; i++)
	{
		err = GetCertExtType(&(Extensions->list.array[i]->extnID), &X509CertExtType);
		if(err<0 && err!=ERR_X509_UNDEFINED_EXTENSION_TYPE)
		{ /* nierozpoznane typy rozszerzen ingorujemy */
			PRINT_ERROR("LIBBMDX509ERR Error in getting Certificate extension type. Error=%li\n", err);
			return ERR_X509_CHECK_EXT_TYPE;
		}
		if(X509CertExtType == CERT_EXT_CRL_DP)
		{
			err = ExtensionCRLDistPoints2string(&(Extensions->list.array[i]->extnValue), CRLDistPoints);
			if(err<0)
			{
				PRINT_ERROR("LIBBMDX509ERR Error in converting Certificate extension Error=%li\n", err);
				return err;
			}
			CRLDistPointsExtFound = 1;
			break;
		}
	}
	if(CRLDistPointsExtFound == 0)
	{
		return ERR_X509_EXT_CRL_DISTP_NOT_FOUND;
	}
	return err;
}

long GetCertExtType(OBJECT_IDENTIFIER_t *Oid, X509CertExtType_t *X509CertExtType)
{
long err = 0;
long *extension_oid		= NULL;
long extension_oid_size				= 0;
long OID_id_ce_cRLDistributionPoints[] = {OID_X509_ID_CE_CRLDISTRIBUTIONPOINTS};

	PRINT_INFO("LIBBMDX509INF Getting certificate extension type\n");
	if(Oid == NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid first parameter value. Error=%i",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(X509CertExtType == NULL || *X509CertExtType < 0)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid second parameter value. Error=%i",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	*X509CertExtType = CERT_EXT_UNDEFINED;
	extension_oid_size = OID_to_ulong_alloc2(Oid, &extension_oid);
	if(extension_oid == NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Memory error. Error=%i",BMD_ERR_MEMORY);
		return BMD_ERR_MEMORY;
	}

	if(!OID_cmp2(OID_id_ce_cRLDistributionPoints, sizeof(OID_id_ce_cRLDistributionPoints),extension_oid, extension_oid_size * sizeof(long)))
	{
		*X509CertExtType = CERT_EXT_CRL_DP;
	}
	if(extension_oid) {free(extension_oid); extension_oid=NULL;}
	/* else if - tu dopisywac wykrywanie kolejnych rozszerzeń CRLa*/
	if(*X509CertExtType == CERT_EXT_UNDEFINED)
	{
		return ERR_X509_UNDEFINED_EXTENSION_TYPE;
	}
	return err;
}

long ExtensionCRLDistPoints2string(OCTET_STRING_t *ExtensionValue, char **string)
{
long err = 0;
CRLDistributionPoints_t *CRLDistributionPoints = NULL;

	if(ExtensionValue == NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid first parameter value. Error=%i",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(string == NULL || *string != NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid second parameter value. Error=%i",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	err = ExtensionValue2CRLDistributionPoints(ExtensionValue, &CRLDistributionPoints);
	if(err<0)
	{
		PRINT_ERROR("LIBBMDX509ERR Error in converting AttributeValue to CRLDistributionPoints\n");
		return err;
	}
	err = CRLDistributionPoints2string(CRLDistributionPoints, string);
	if(err<0)
	{
		PRINT_ERROR("LIBBMDX509ERR Error in converting CRLDistributionPoints to string\n");
		return err;
	}
	asn_DEF_CRLDistributionPoints.free_struct(&asn_DEF_CRLDistributionPoints, CRLDistributionPoints, 0);
	return err;
}

/* Kod Marka */
Extension_t *find_extension_by_OID(	Extensions_t *ext_list,
						char *oid_table)
{
long i=0;

	if(!ext_list)
		return NULL;

	for(i=0;i<ext_list->list.count;i++)
	{
		if(!OBJECT_IDENTIFIER_cmp(&(ext_list->list.array[i]->extnID),oid_table))
		{
			return ext_list->list.array[i];
		}
	}
	return NULL;
}

long get_count_of_crl_dps(CRLDistributionPoints_t *crl_dps)
{
long count=0;
long i=0;
DistributionPoint_t *dp=NULL;
DistributionPointName_t *dp_name=NULL;
GeneralNames_t *dp_generalnames=NULL;

	for(i=0;i<crl_dps->list.count;i++)
	{
		dp=crl_dps->list.array[i];
		dp_name=dp->distributionPoint;
		switch(dp_name->present)
		{
			case DistributionPointName_PR_NOTHING:
				break;
			case DistributionPointName_PR_fullName:
				dp_generalnames=&(dp_name->choice.fullName);
				count+=dp_generalnames->list.count;
			case DistributionPointName_PR_nameRelativeToCRLIssuer:
				break;
		}
	}
	return count;
}

/** @todo funkcje to trzeba zrobic tak azeby pasowala pod ogolny model biblioteki */
long GetCrlDistributionPoints(GenBuf_t *cert_buf,char ***crl_dp)
{
Certificate_t *Certificate = NULL;	/* obiekt certyfikatu */
TBSCertificate_t *tbsCertificate=NULL;	/* X509 wewnatrze tego co wyzej */
Extensions_t *X509Extensions=NULL;
Extension_t *CRLExtension=NULL;

CRLDistributionPoints_t *crl_dps=NULL;
DistributionPoint_t *dp=NULL;
DistributionPointName_t *dp_name=NULL;
GeneralNames_t *dp_generalnames;
/*long status;*/
long i;
long j;
long k;
asn_dec_rval_t rval;
char crl_dp_oid[]={"2.5.29.31"};
long ilosc_crl_dp=0;
long crl_dp_index=0;

	PRINT_INFO("LIBBMDX509 Getting CRL distribution points\n");
	BMD_FOK(GetCertificateStructFromX509Certificate(cert_buf, &Certificate));

	tbsCertificate=&(Certificate->tbsCertificate);
	X509Extensions=tbsCertificate->extensions;

	CRLExtension=find_extension_by_OID(X509Extensions,crl_dp_oid);
	if(CRLExtension==NULL)
	{
		BMD_FOK(-1);
	}

	rval = ber_decode(0, &asn_DEF_CRLDistributionPoints,(void **)&crl_dps, CRLExtension->extnValue.buf, CRLExtension->extnValue.size);

	if(rval.code!=RC_OK)
	{
		BMD_FOK(-2);
	}

	ilosc_crl_dp=get_count_of_crl_dps(crl_dps);

	(*crl_dp)=(char **)malloc((ilosc_crl_dp+2)*sizeof(char *));
	for(i=0;i<ilosc_crl_dp+1;i++)
	{
		(*crl_dp)[i]=NULL;
	}

	for(j=0;j<crl_dps->list.count;j++)
	{
		dp=crl_dps->list.array[j];
		dp_name=dp->distributionPoint;
		switch(dp_name->present)
		{
			case DistributionPointName_PR_NOTHING:
				break;
			case DistributionPointName_PR_fullName:
				dp_generalnames=&(dp_name->choice.fullName);
				for(k=0;k<dp_generalnames->list.count;k++)
				{
					GeneralName2string(dp_generalnames->list.array[k],&((*crl_dp)[crl_dp_index]));
					crl_dp_index++;
				}
			case DistributionPointName_PR_nameRelativeToCRLIssuer:
				break;
		}
	}
	return 0;
}

/** @todo funkcje to trzeba zrobic tak azeby pasowala pod ogolny model biblioteki */
long GetPubKeyFromCert(GenBuf_t *cert_buf,GenBuf_t **pub_key)
{
long status;
long rozmiar;
Certificate_t *Certificate = NULL;
TBSCertificate_t *tbsCertificate=NULL;
SubjectPublicKeyInfo_t *sub_key_info=NULL;

	PRINT_INFO("LIBBMDX509 Getting public key from certificate\n");
	if(cert_buf==NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid first parameter value. Error=%i",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(pub_key==NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid second parameter value. Error=%i",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}
	if((*pub_key)!=NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid second parameter value. Error=%i",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	status = GetCertificateStructFromX509Certificate(cert_buf, &Certificate);
	if(status!=0)
	{
		PRINT_ERROR("LIBBMDX509ERR Format error. Error=%i",BMD_ERR_FORMAT);
		return BMD_ERR_FORMAT;
	}

	tbsCertificate=&(Certificate->tbsCertificate);
	sub_key_info=&(tbsCertificate->subjectPublicKeyInfo);

	rozmiar=sub_key_info->subjectPublicKey.size;
	(*pub_key)=(GenBuf_t *)malloc(sizeof(GenBuf_t));
	(*pub_key)->buf=(char *)malloc(rozmiar+2);
	memset((*pub_key)->buf,0,rozmiar+1);
	(*pub_key)->size=rozmiar;
	memmove((*pub_key)->buf,sub_key_info->subjectPublicKey.buf,rozmiar);

	asn_DEF_Certificate.free_struct(&asn_DEF_Certificate, Certificate, 0);Certificate=NULL;


	return 0;
}


/** @todo funkcje to trzeba zrobic tak azeby pasowala pod ogolny model biblioteki */
long GetPubKeyInfoFromCert(GenBuf_t *cert_buf,AlgorithmIdentifier_t **Algorithm)
{
long status;
long rozmiar;
Certificate_t *Certificate = NULL;
TBSCertificate_t *tbsCertificate=NULL;
SubjectPublicKeyInfo_t *sub_key_info=NULL;

	PRINT_INFO("LIBBMDX509 Getting public key info from certificate\n");
	if(cert_buf==NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid first parameter value. Error=%i",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(Algorithm==NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid second parameter value. Error=%i",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}
	if((*Algorithm)!=NULL)
	{
		PRINT_ERROR("LIBBMDX509ERR Invalid second parameter value. Error=%i",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}
	status = GetCertificateStructFromX509Certificate(cert_buf, &Certificate);
	if(status!=0)
	{
		PRINT_ERROR("LIBBMDX509ERR Format error. Error=%i",BMD_ERR_FORMAT);
		return BMD_ERR_FORMAT;
	}
	//Certificate->signatureAlgorithm

	tbsCertificate=&(Certificate->tbsCertificate);
	sub_key_info=&(tbsCertificate->subjectPublicKeyInfo);

	rozmiar=sub_key_info->subjectPublicKey.size;
	//(*pub_key)=(GenBuf_t *)malloc(sizeof(GenBuf_t));
	//(*pub_key)->buf=(char *)malloc(rozmiar+2);
	//memset((*pub_key)->buf,0,rozmiar+1);
	//(*pub_key)->size=rozmiar;
	//memmove((*pub_key)->buf,sub_key_info->subjectPublicKey.buf,rozmiar);
	//
	//asn_DEF_Certificate.free_struct(&asn_DEF_Certificate, Certificate, 0);Certificate=NULL;


	return 0;
}

void get_CommonName_from_subject(X509 *cert,char **cn)
{
X509_NAME *subject_name;
X509_NAME_ENTRY *subject_name_entry;
long i,count;
char CommonName_OID_Table[]={CommonName_OID};

	subject_name=X509_get_subject_name(cert);
	count=sk_X509_NAME_ENTRY_num(subject_name->entries);
	for(i=0;i<count;i++)
	{
		subject_name_entry=sk_X509_NAME_ENTRY_value(subject_name->entries,i);
		if(!memcmp(CommonName_OID_Table,subject_name_entry->object->data,3))
		{
			(*cn)=(char *)malloc(subject_name_entry->value->length+2);
			memset(*cn,0,subject_name_entry->value->length+1);
			memmove(*cn,subject_name_entry->value->data,subject_name_entry->value->length);
			break;
		}
	}
}

void get_CommonName_from_issuer(X509 *cert,char **cn_issuer)
{
X509_NAME *subject_name;
X509_NAME_ENTRY *subject_name_entry;
long i,count;

char CommonName_OID_Table[]={CommonName_OID};

	subject_name=X509_get_issuer_name(cert);
	count=sk_X509_NAME_ENTRY_num(subject_name->entries);
	for(i=0;i<count;i++)
	{
		subject_name_entry=sk_X509_NAME_ENTRY_value(subject_name->entries,i);
		if(!memcmp(CommonName_OID_Table,subject_name_entry->object->data,3))
		{
			(*cn_issuer)=(char *)malloc(subject_name_entry->value->length+2);
			memset(*cn_issuer,0,subject_name_entry->value->length+1);
			memmove(*cn_issuer,subject_name_entry->value->data,subject_name_entry->value->length);
			break;
		}
	}
}

void get_Organization_from_subject(X509 *cert,char **organization)
{
X509_NAME *subject_name;
X509_NAME_ENTRY *subject_name_entry;
long i,count;

char Organization_OID_Table[]={Organization_OID};

	subject_name=X509_get_subject_name(cert);
	count=sk_X509_NAME_ENTRY_num(subject_name->entries);

	for(i=0;i<count;i++)
	{
		subject_name_entry=sk_X509_NAME_ENTRY_value(subject_name->entries,i);
		if(!memcmp(Organization_OID_Table,subject_name_entry->object->data,3))
		{
			(*organization)=(char *)malloc(subject_name_entry->value->length+2);
			memset(*organization,0,subject_name_entry->value->length+1);
			memmove(*organization,subject_name_entry->value->data,subject_name_entry->value->length);
			break;
		}
	}
}

void get_Serial_from_certificate(X509 *cert,char **serial)
{
long i,count;
char single_octet[3];

	if(cert==NULL)
	{
		serial=NULL;
		return;
	}
	count=cert->cert_info->serialNumber->length;

	(*serial)=(char *)malloc(count*3+2);
	memset(*serial,0,count*3+1);
	memset(single_octet,0,3);
	for(i=0;i<count;i++)
	{
		sprintf(single_octet,"%X",cert->cert_info->serialNumber->data[i]);
		strcat(*serial,single_octet);
		strcat(*serial," ");
		memset(single_octet,0,3);

	}
}

void get_base_info_from_cert(X509 *cert,char **cn,char **organization,char **cn_issuer,char **serial,long *cert_type)
{
X509_NAME *subject_name;
X509_NAME_ENTRY *subject_name_entry;
long i,count;
char single_octet[3];

char Organization_OID_Table[]={Organization_OID};
char CommonName_OID_Table[]={CommonName_OID};

	/*ustalenie cn z pola subject*/
	if(cn!=NULL)
	{
		subject_name=X509_get_subject_name(cert);
		count=sk_X509_NAME_ENTRY_num(subject_name->entries);
		for(i=0;i<count;i++)
		{
			subject_name_entry=sk_X509_NAME_ENTRY_value(subject_name->entries,i);
			if(!memcmp(CommonName_OID_Table,subject_name_entry->object->data,3))
			{
				(*cn)=(char *)malloc(subject_name_entry->value->length+2);
				memset(*cn,0,subject_name_entry->value->length+1);
				memmove(*cn,subject_name_entry->value->data,subject_name_entry->value->length);
				break;
			}
		}
	}

	/*ustalenie organization z subject*/
	if(organization!=NULL)
	{
		subject_name=X509_get_subject_name(cert);
		count=sk_X509_NAME_ENTRY_num(subject_name->entries);
		for(i=0;i<count;i++)
		{
			subject_name_entry=sk_X509_NAME_ENTRY_value(subject_name->entries,i);
			if(!memcmp(Organization_OID_Table,subject_name_entry->object->data,3))
			{
				(*organization)=(char *)malloc(subject_name_entry->value->length+2);
				memset(*organization,0,subject_name_entry->value->length+1);
				memmove(*organization,subject_name_entry->value->data,subject_name_entry->value->length);
				break;
			}
		}
	}

	/*ustalenie cn z issuer*/
	if(cn_issuer!=NULL)
	{
		subject_name=X509_get_issuer_name(cert);
		count=sk_X509_NAME_ENTRY_num(subject_name->entries);
		for(i=0;i<count;i++)
		{
			subject_name_entry=sk_X509_NAME_ENTRY_value(subject_name->entries,i);
			if(!memcmp(CommonName_OID_Table,subject_name_entry->object->data,3))
			{
				(*cn_issuer)=(char *)malloc(subject_name_entry->value->length+2);
				memset(*cn_issuer,0,subject_name_entry->value->length+1);
				memmove(*cn_issuer,subject_name_entry->value->data,subject_name_entry->value->length);
				break;
			}
		}
	}

	/*ustalenie seriala z certyfikatu*/
	if(serial!=NULL)
	{
		count=cert->cert_info->serialNumber->length;
		(*serial)=(char *)malloc(count*3+2);
		memset(*serial,0,count*3+1);
		memset(single_octet,0,3);
		for(i=0;i<count;i++)
		{
			sprintf(single_octet,"%X",cert->cert_info->serialNumber->data[i]);
			strcat(*serial,single_octet);
			strcat(*serial," ");
			memset(single_octet,0,3);
		}
	}

	/*ustalenie type certyfikatu*/
	if(cert_type!=NULL)
	{
		/*PRINT_INFO("TUTAJ DOROBIC nowa funkcje is_CA_Cert");*/
		*cert_type=100;
	}
}

long X509Certificate_free_ptr(Certificate_t **Certificate)
{
long err=0;

	if(Certificate == NULL)				{		BMD_FOK(BMD_ERR_PARAM1);	}
	if(*Certificate == NULL)			{		BMD_FOK(BMD_ERR_PARAM1);	}

	asn_DEF_Certificate.free_struct(&asn_DEF_Certificate, *Certificate, 0);
	*Certificate = NULL;
	return err;
}

long X509_is_non_repudiation(GenBuf_t *certificate, CertExtConfig_t *twf_CertExtConfig)
{
	Certificate_t *cert=NULL;
	asn_dec_rval_t dec_result;
	long iter=0;
	char keyusage_oid[]={"2.5.29.15"}; //oid : extension keyusage
	long extension_count=0;
	long ret_val=0;
	BIT_STRING_t *bitstr=NULL;


	if(certificate == NULL)				{		BMD_FOK(BMD_ERR_PARAM1);	}
	if(certificate->buf == NULL)		{		BMD_FOK(BMD_ERR_PARAM1);	}
	if(certificate->size == 0)			{		BMD_FOK(BMD_ERR_PARAM1);	}
	if(twf_CertExtConfig == NULL)		{		BMD_FOK(BMD_ERR_PARAM2);	}


	dec_result=ber_decode(0, &asn_DEF_Certificate, (void**)&cert,certificate->buf, certificate->size);
	if(dec_result.code != RC_OK)
	{
		return BMD_CERTIFICATE_DECODE_ERROR;
	}

	if(cert->tbsCertificate.extensions == NULL)
	{
		asn_DEF_Certificate.free_struct(&asn_DEF_Certificate, cert, 1);
		free(cert);
		return ERR_X509_NO_EXTENSIONS;

	}

	extension_count=cert->tbsCertificate.extensions->list.count;
	for(iter=0; iter < extension_count; iter++)
	{
		ret_val=OBJECT_IDENTIFIER_cmp(&(cert->tbsCertificate.extensions->list.array[iter]->extnID), keyusage_oid);

		//arcs_num=OBJECT_IDENTIFIER_get_arcs(&(cert->tbsCertificate.extensions->list.array[iter]->extnID), arcs,sizeof(arcs[0]),20);
		//for(i=0; i<arcs_num; i++)
		//{
		//	printf(".%i", arcs[i]);
		//}
		//printf("\n");


		if(ret_val == 0)  //jesli znaleziony szukany oid
		{
			//printf("octet size: %i\n",cert->tbsCertificate.extensions->list.array[iter]->extnValue.size);
			dec_result=ber_decode(0, &asn_DEF_BIT_STRING, (void**)&bitstr, cert->tbsCertificate.extensions->list.array[iter]->extnValue.buf, cert->tbsCertificate.extensions->list.array[iter]->extnValue.size);
			if(dec_result.code != RC_OK)
			{
				asn_DEF_Certificate.free_struct(&asn_DEF_Certificate, cert, 1);
				free(cert);
				return ERR_X509_DECODE_KEY_USAGE;
			}
			/** elaborat by WSZ
			wydawalo mi sie , ze powinno byc nastepujaco:
			9 mozliwych ustawien, wiec:
			2 bajty [index 0..1], 7 najmlodszych bitow niewykorzystanych...
			bit non-Repudation to zerowy bit starszego bajta
			ale gowno prawda
			unizeto, sigillum i kir robi tak:
			Poniewaz bit odpowiadajacy non-repudiation to bit nr 1 (numerowanie bitow od 0),
			to w takiej sytuacji potrzebne sa tylko 2 bity, wiec wystarczy BIT_STRING dlugosci 1 bajta.
			Ustawione jest 6 nieuzywanych bitow.
			Zeby bylo zabawniej, bit nr 0 to ten pierwszy od lewej, a nr 1 to drugi od lewej,
			Wynika wiec, ze najstarsze bity BUFORA maja najnizsza numeracje... ale shit!!!
			Niech bedzie, ze sie nie znam...
			*/

			//jesli wystawca certyfikatu zmieni podejscie i bedzie wykorzystywal wiecej niz 1 bajt
			if(bitstr->size != 1)
			{
				asn_DEF_Certificate.free_struct(&asn_DEF_Certificate, cert, 1);
				free(cert);
				asn_DEF_BIT_STRING.free_struct(&asn_DEF_BIT_STRING, bitstr, 1);
				free(bitstr);
				return ERR_X509_WRONG_SIZE_OF_BYTES;
			}

			//printf("bitstr size:%i bitstr unused %i; buf: %i\n", bitstr->size, bitstr->bits_unused, bitstr->buf[0]);
			if(twf_CertExtConfig->non_repudation_bit_only == 1)
			{
				if( (bitstr->buf[0] & 0x40) != 0 && (bitstr->buf[0] & 0xBF) == 0 )
				{
					/*Usatwiono non repudation, a jednoczesnie nie ustawiono zadnego innego uzycia*/
					asn_DEF_Certificate.free_struct(&asn_DEF_Certificate, cert, 1);
					free(cert);
					asn_DEF_BIT_STRING.free_struct(&asn_DEF_BIT_STRING, bitstr, 1);
					free(bitstr);
					return BMD_OK;
				}
			}
			else
			{
				if( (bitstr->buf[0] & 0x40) != 0 )
				{
					/*Usatwiono non repudation*/
					asn_DEF_Certificate.free_struct(&asn_DEF_Certificate, cert, 1);
					free(cert);
					asn_DEF_BIT_STRING.free_struct(&asn_DEF_BIT_STRING, bitstr, 1);
					free(bitstr);
					return BMD_OK;
				}
			}

			asn_DEF_Certificate.free_struct(&asn_DEF_Certificate, cert, 1);
			free(cert);
			asn_DEF_BIT_STRING.free_struct(&asn_DEF_BIT_STRING, bitstr, 1);
			free(bitstr);
			return ERR_X509_NON_REPUDATION_BIT_NOT_SET;
		}

	}

	asn_DEF_Certificate.free_struct(&asn_DEF_Certificate, cert, 1);
	free(cert);

	/*Musimy znalezc rozszerzenie key usage ze wzgledu na wymagania zwiazane z rozporzadzeniem*/
	return ERR_X509_KEY_USAGE_ABSENT;
}
