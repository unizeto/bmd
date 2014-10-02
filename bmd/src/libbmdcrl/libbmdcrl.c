/***************************************************************************
 *            libbmdcrl.c
 *  BMD CRL parsing library 
 *  Wed Mar  1 14:44:02 2006
 *  Copyright  2006  Marcin Szulga
 *  Email:  mszulga@certum.pl
 ****************************************************************************/

#include <bmd/libbmdcrl/libbmdcrl.h>
#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>
/****************
 * ONE CALL API *
 ***************/
long CRLGetAttr(GenBuf_t *GenBuf, CRLAttrType_t CRLAttrType, char **AttributeString, long *AttributeLong)
{
long err = 0;
	
	PRINT_VDEBUG("LIBBMDCRLINF Getting CRL attributes\n");
	if(GenBuf == NULL || GenBuf->buf == NULL || GenBuf->size == 0)
	{
		PRINT_DEBUG("LIBBMDCRLERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(CRLAttrType < 0)
	{
		PRINT_DEBUG("LIBBMDCRLERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}
	if(AttributeString == NULL || *AttributeString != NULL)
	{
		PRINT_DEBUG("LIBBMDCRLERR Invalid third parameter value. Error=%i\n",BMD_ERR_PARAM3);
		return BMD_ERR_PARAM3;
	}	
	if(AttributeLong == NULL || *AttributeLong != 0)
	{
		PRINT_DEBUG("LIBBMDCRLERR Invalid fourth parameter value. Error=%i\n",BMD_ERR_PARAM4);
		return BMD_ERR_PARAM4;
	}
		
	switch(CRLAttrType)
	{
		case CRL_ATTR_VERSION:
			err = GetVersionFromCRL(GenBuf, AttributeLong);
			if(err<0) return err;
			break;
		case CRL_ATTR_SN:
			err = GetSerialNumberFromCRL(GenBuf, AttributeString);
			if(err<0) return err;
			break;
		case CRL_ATTR_DELTA:
			err = GetDeltaStatusFromCRL(GenBuf, AttributeString);
			if(err<0) return err;
			break;
		case CRL_ATTR_SIGALG_OID:
			err = GetSignatureAlgorithmOIDFromCRL(GenBuf, AttributeString);
			if(err<0) return err;
			break;
		case CRL_ATTR_ISSUER_DN:
			err = GetIssuerDNFromCRL(GenBuf, AttributeString);
			if(err<0) return err;
			break;
		case CRL_ATTR_THIS_UPDATE:
			err = GetThisUpdateFromCRL(GenBuf, AttributeString);
			if(err<0) return err;
			break;
		case CRL_ATTR_NEXT_UPDATE:
			err = GetNextUpdateFromCRL(GenBuf, AttributeString);
			if(err<0) return err;
			break;
		case CRL_ATTR_NUMBER_OF_REVOKED_CERTS:
			err = GetNumberOfRevokedCertsFromCRL(GenBuf, AttributeLong);
			if(err<0) return err;
			break;
		default:
			PRINT_DEBUG("LIBBMDCRLERR Unrecognized CRL attribute type!\n");
			return ERR_CRL_UNRECOGNIZED_ATTR_TYPE;
			break;	
	}
	return err;
}

/************
 * EASY API *
 ***********/
long GetVersionFromCRL(GenBuf_t *GenBuf, long *version)
{
long err = 0;
CertificateList_t *CertificateList = NULL;


	PRINT_VDEBUG("LIBBMDCRLINF Getting version from CRL\n");
	if(GenBuf == NULL || GenBuf->buf == NULL || GenBuf->size == 0)
	{
		PRINT_DEBUG("LIBBMDCRLERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(version == NULL || *version != 0)
		return BMD_ERR_PARAM2;

	err = GetCertificateListFromCRL(GenBuf, &CertificateList);
	if(err < 0)
	{
		PRINT_DEBUG("LIBBMDCRLERR Error in getting CertificateList from CRL list. Error=%li\n", err);
		return err;
	}
	err = GetVersionFromCertificateList(CertificateList, version);
	if(err < 0 && err != ERR_CRL_VERSION_NOT_PRESENT)
	{
		PRINT_DEBUG("LIBBMDCRLERR Error in getting version from CertificateList. Error=%li\n", err);
		return err;
	}
	if(err == ERR_CRL_VERSION_NOT_PRESENT)
	{
		PRINT_DEBUG("LIBBMDCRLINF Warning: CRL version field not present.\n");
		return err;
	}
	asn_DEF_CertificateList.free_struct(&asn_DEF_CertificateList, CertificateList, 0);
	return err;
}

long GetSerialNumberFromCRL(GenBuf_t *GenBuf, char **SerialNumber)
{
long err = 0;
CertificateList_t *CertificateList = NULL;

	PRINT_VDEBUG("LIBBMDCRLINF Getting serial number from CRL\n");
	if(GenBuf == NULL || GenBuf->buf == NULL || GenBuf->size == 0)
	{
		PRINT_DEBUG("LIBBMDCRLERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(SerialNumber == NULL || *SerialNumber != NULL)
	{
		PRINT_DEBUG("LIBBMDCRLERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	err = GetCertificateListFromCRL(GenBuf, &CertificateList);
	if(err < 0)
	{
		PRINT_DEBUG("LIBBMDCRLERR Error in getting CertificateList from CRL list. Error=%li\n", err);
		return err;
	}
	err = GetSerialNumberFromCertificateList(CertificateList, SerialNumber);
	if(err < 0)
	{
		PRINT_DEBUG("LIBBMDCRLERR Error in getting serial number from CertificateList extensions. Error=%li\n", err);
		return err;
	}
	asn_DEF_CertificateList.free_struct(&asn_DEF_CertificateList, CertificateList, 0);
	return err;
}

long GetDeltaStatusFromCRL(GenBuf_t *GenBuf, char **isDelta)
{
long err = 0;
CertificateList_t *CertificateList = NULL;

	PRINT_VDEBUG("LIBBMDCRLINF Getting data status from CRL\n");
	if(GenBuf == NULL || GenBuf->buf == NULL || GenBuf->size == 0)
	{
		PRINT_DEBUG("LIBBMDCRLERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(isDelta == NULL || *isDelta != NULL)
	{
		PRINT_DEBUG("LIBBMDCRLERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	err = GetCertificateListFromCRL(GenBuf, &CertificateList);
	if(err < 0)
	{
		PRINT_DEBUG("LIBBMDCRLERR Error in getting CertificateList from CRL list. Error=%li\n", err);
		return err;
	}
	err = GetIsDeltaInfoFromCertificateList(CertificateList, isDelta);
	if(err < 0)
	{
		PRINT_DEBUG("LIBBMDCRLERR Error in getting CRL type (full,delta) from CertificateList extensions. Error=%li\n", err);
		return err;
	}
	asn_DEF_CertificateList.free_struct(&asn_DEF_CertificateList, CertificateList, 0);
	return err;
}

long GetSignatureAlgorithmOIDFromCRL(GenBuf_t *GenBuf, char **SignatureAlgorithmOid)
{
long err = 0;
CertificateList_t *CertificateList = NULL;

	PRINT_VDEBUG("LIBBMDCRLINF Getting signature algorithm oid from CRL\n");
	if(GenBuf == NULL || GenBuf->buf == NULL || GenBuf->size == 0)
	{
		PRINT_DEBUG("LIBBMDCRLERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(SignatureAlgorithmOid == NULL || *SignatureAlgorithmOid != NULL)
	{
		PRINT_DEBUG("LIBBMDCRLERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	err = GetCertificateListFromCRL(GenBuf, &CertificateList);
	if(err < 0)
	{
		PRINT_DEBUG("LIBBMDCRLERR Error in getting CertificateList from CRL list. Error=%li\n", err);
		return err;
	}
	err = GetSignatureAlgorithmOIDFromCertificateList(CertificateList, SignatureAlgorithmOid);
	if(err < 0)
	{
		PRINT_DEBUG("LIBBMDCRLERR Error in getting CRL signature algorithm OID from CertificateList extensions. Error=%li\n", err);
		return err;
	}
	asn_DEF_CertificateList.free_struct(&asn_DEF_CertificateList, CertificateList, 0);
	return err;
}

long GetIssuerDNFromCRL(GenBuf_t *GenBuf, char **IssuerDN)
{
long err = 0;
CertificateList_t *CertificateList = NULL;

	PRINT_VDEBUG("LIBBMDCRLINF Getting issuer DN from CRL\n");
	if(GenBuf == NULL || GenBuf->buf == NULL || GenBuf->size == 0)
	{
		PRINT_DEBUG("LIBBMDCRLERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(IssuerDN == NULL || *IssuerDN != NULL)
	{
		PRINT_DEBUG("LIBBMDCRLERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	err = GetCertificateListFromCRL(GenBuf, &CertificateList);
	if(err < 0)
	{
		PRINT_DEBUG("LIBBMDCRLERR Error in getting CertificateList from CRL list. Error=%li\n", err);
		return err;
	}
	err = GetIssuerDNFromCertificateList(CertificateList, IssuerDN);
	if(err < 0)
	{
		PRINT_DEBUG("LIBBMDCRLERR Error in getting CRL issuer DN from CertificateList. Error=%li\n", err);
		return err;
	}
	asn_DEF_CertificateList.free_struct(&asn_DEF_CertificateList, CertificateList, 0);
	return err;
}

long GetThisUpdateFromCRL(GenBuf_t *GenBuf, char **ThisUpdate)
{
long err = 0;
CertificateList_t *CertificateList = NULL;

	PRINT_VDEBUG("LIBBMDCRLINF Getting this update from CRL\n");
	if(GenBuf == NULL || GenBuf->buf == NULL || GenBuf->size == 0)
	{
		PRINT_DEBUG("LIBBMDCRLERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(ThisUpdate == NULL || *ThisUpdate != NULL)
	{
		PRINT_DEBUG("LIBBMDCRLERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	err = GetCertificateListFromCRL(GenBuf, &CertificateList);
	if(err < 0)
	{
		PRINT_DEBUG("LIBBMDCRLERR Error in getting CertificateList from CRL list. Error=%li\n", err);
		return err;
	}
	err = GetThisUpdateFromCertificateList(CertificateList, ThisUpdate);
	if(err < 0)
	{
		PRINT_DEBUG("LIBBMDCRLERR Error in getting CRL this update from CertificateList. Error=%li\n", err);
		return err;
	}
	asn_DEF_CertificateList.free_struct(&asn_DEF_CertificateList, CertificateList, 0);
	return err;
}

long GetNextUpdateFromCRL(GenBuf_t *GenBuf, char **NextUpdate)
{
long err = 0;
CertificateList_t *CertificateList = NULL;

	PRINT_VDEBUG("LIBBMDCRLINF Getting next update from CRL\n");
	if(GenBuf == NULL || GenBuf->buf == NULL || GenBuf->size == 0)
	{
		PRINT_DEBUG("LIBBMDCRLERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(NextUpdate == NULL || *NextUpdate != NULL)
	{
		PRINT_DEBUG("LIBBMDCRLERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	err = GetCertificateListFromCRL(GenBuf, &CertificateList);
	if(err < 0)
	{
		PRINT_DEBUG("LIBBMDCRLERR Error in getting CertificateList from CRL list. Error=%li\n", err);
		return err;
	}
	err = GetNextUpdateFromCertificateList(CertificateList, NextUpdate);
	if(err < 0)
	{
		PRINT_DEBUG("LIBBMDCRLERR Error in getting CRL next update from CertificateList. Error=%li\n", err);
		return err;
	}
	asn_DEF_CertificateList.free_struct(&asn_DEF_CertificateList, CertificateList, 0);
	return err;
}

long GetNumberOfRevokedCertsFromCRL(GenBuf_t *GenBuf, long *NoOfRevokedCerts)
{
long err = 0;
CertificateList_t *CertificateList = NULL;

	PRINT_VDEBUG("LIBBMDCRLINF Getting number of revoked certificates from CRL\n");
	if(GenBuf == NULL || GenBuf->buf == NULL || GenBuf->size == 0)
	{
		PRINT_DEBUG("LIBBMDCRLERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(NoOfRevokedCerts == NULL || *NoOfRevokedCerts < 0)
	{
		PRINT_DEBUG("LIBBMDCRLERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	err = GetCertificateListFromCRL(GenBuf, &CertificateList);
	if(err < 0)
	{
		PRINT_DEBUG("LIBBMDCRLERR Error in getting CertificateList from CRL list. Error=%li\n", err);
		return err;
	}
	err = GetNumberOfRevokedCertsFromCertificateList(CertificateList, NoOfRevokedCerts);
	if(err < 0)
	{
		PRINT_DEBUG("LIBBMDCRLERR Error in getting number of revoked certs from CertificateList. Error=%li\n", err);
		return err;
	}
	asn_DEF_CertificateList.free_struct(&asn_DEF_CertificateList, CertificateList, 0);
	return err;
}

/************
 * FAST API *
 ***********/
long GetCertificateListFromCRL(GenBuf_t *GenBuf, CertificateList_t **CertificateList)
{
long err = 0;
asn_dec_rval_t rval;
		
	PRINT_VDEBUG("LIBBMDCRLINF Getting certificate list from CRL\n");
	if(GenBuf == NULL || GenBuf->buf == NULL || GenBuf->size == 0)
	{
		PRINT_DEBUG("LIBBMDCRLERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(CertificateList == NULL || *CertificateList != NULL)
	{
		PRINT_DEBUG("LIBBMDCRLERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}
			
	rval = ber_decode(0, &asn_DEF_CertificateList,  (void **)CertificateList, GenBuf->buf, GenBuf->size);
	if(rval.code != RC_OK)
	{
		PRINT_DEBUG("LIBBMDCRLERR Error in ber decoding. Error=%i\n",ERR_CRL_DECODE_CRL_DATA);
		asn_DEF_CertificateList.free_struct(&asn_DEF_CertificateList, *CertificateList, 0);
		return ERR_CRL_DECODE_CRL_DATA;
	}
	return err;
}

long GetVersionFromCertificateList(CertificateList_t *CertificateList, long *version)
{
long err = 0;
		
	PRINT_VDEBUG("LIBBMDCRLINF Getting version from certificate list\n");
	if(CertificateList == NULL)
	{
		PRINT_DEBUG("LIBBMDCRLERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(version == NULL || *version != 0)
	{
		PRINT_DEBUG("LIBBMDCRLERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}
		
	if(CertificateList->tbsCertList.version == NULL)
	{
		PRINT_DEBUG("LIBBMDCRLERR CRL version not present. Error=%i\n",ERR_CRL_VERSION_NOT_PRESENT);
		return ERR_CRL_VERSION_NOT_PRESENT;
	}
		
	err = asn_INTEGER2long(CertificateList->tbsCertList.version, version);
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDCRLERR Error converting INTEGER_t to long.\n");
		return ERR_CRL_CONVERT_INTEGER_T_TO_LONG;
	}
	return err;
}

long GetSerialNumberFromCertificateList(CertificateList_t *CertificateList, char **SerialNumber)
{
long err = 0;
long AttributeLong = 0;
Extensions_t *Extensions = NULL;
	
	PRINT_VDEBUG("LIBBMDCRLINF Getting serial number from certificate list\n");
	if(CertificateList == NULL)
	{
		PRINT_DEBUG("LIBBMDCRLERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(SerialNumber == NULL || *SerialNumber != NULL)
	{
		PRINT_DEBUG("LIBBMDCRLERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	/* pamiec nie jest kopiowana pod Extensions!!!				*/
	/* Po zwolnieniu CertificateList wskaznik Extensions pokazuje na nic	*/
	err = GetExtensionsFromCertificateList(CertificateList, &Extensions);
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDCRLERR Error getting extensions field from certificate list. Error=%li\n", err);
		return err;
	}
	err = GetCRLExtension(Extensions, CRL_EXT_SN, SerialNumber, &AttributeLong);
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDCRLERR Error getting serial number from certificate list extensions filed. Error=%li\n", err);
		return err;
	}
	return err;
}

long GetIsDeltaInfoFromCertificateList(CertificateList_t *CertificateList, char **isDelta)
{
long err = 0;
long AttributeLong = 0;
Extensions_t *Extensions = NULL;

	PRINT_VDEBUG("LIBBMDCRLINF Getting presence of delta info from certificate list\n");
	if(CertificateList == NULL)
	{
		PRINT_DEBUG("LIBBMDCRLERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(isDelta == NULL || *isDelta != NULL)
	{
		PRINT_DEBUG("LIBBMDCRLERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}
	
	/* pamiec nie jest kopiowana pod Extensions!!!				*/
	/* Po zwolnieniu CertificateList wskaznik Extensions pokazuje na nic	*/
	err = GetExtensionsFromCertificateList(CertificateList, &Extensions);
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDCRLERR Error getting extensions field from certificate list. Error=%li\n", err);
		return err;
	}
	err = GetCRLExtension(Extensions, CRL_EXT_DELTA, isDelta, &AttributeLong);
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDCRLERR Error getting delta info from certificate list extensions field. Error=%li\n", err);
		return err;
	}
	return err;
}

long GetSignatureAlgorithmOIDFromCertificateList(CertificateList_t *CertificateList, char **SignatureAlgorithmOid)
{
long err = 0;

	PRINT_VDEBUG("LIBBMDCRLINF Getting signature algorithm oid from certificate list\n");
	if(CertificateList == NULL)
	{
		PRINT_DEBUG("LIBBMDCRLERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(SignatureAlgorithmOid == NULL || *SignatureAlgorithmOid != NULL)
	{
		PRINT_DEBUG("LIBBMDCRLERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}
	
	err = OBJECT_IDENTIFIER2string(&(CertificateList->tbsCertList.signature.algorithm), SignatureAlgorithmOid);
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDCRLERR Error converting OBJECT_IDENTIFIER_t to string.\n");
		return err;
	}
	return err;
}

long GetIssuerDNFromCertificateList(CertificateList_t *CertificateList, char **IssuerDN)
{
long err = 0;

	PRINT_VDEBUG("LIBBMDCRLINF Getting issuer DNF from certificate list\n");
	if(CertificateList == NULL)
	{
		PRINT_DEBUG("LIBBMDCRLERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(IssuerDN == NULL || *IssuerDN != NULL)
	{
		PRINT_DEBUG("LIBBMDCRLERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}
	
	err = Name2string(&(CertificateList->tbsCertList.issuer), IssuerDN);
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDCRLERR Error converting Name_t to string.\n");
		return err;
	}
	return err;
}

long GetThisUpdateFromCertificateList(CertificateList_t *CertificateList, char **ThisUpdate)
{
long err = 0;

	PRINT_VDEBUG("LIBBMDCRLINF Getting this update from certificate list\n");
	if(CertificateList == NULL)
	{
		PRINT_DEBUG("LIBBMDCRLERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(ThisUpdate == NULL || *ThisUpdate != NULL)
	{
		PRINT_DEBUG("LIBBMDCRLERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	err = Time2string(&(CertificateList->tbsCertList.thisUpdate), ThisUpdate);
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDCRLERR Error converting Time_t to string.\n");
		return err;
	}
	return err;
}

long GetNextUpdateFromCertificateList(CertificateList_t *CertificateList, char **NextUpdate)
{
long err = 0;

	PRINT_VDEBUG("LIBBMDCRLINF Getting next update from certificate list\n");
	if(CertificateList == NULL)
	{
		PRINT_DEBUG("LIBBMDCRLERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(NextUpdate == NULL || *NextUpdate != NULL)
	{
		PRINT_DEBUG("LIBBMDCRLERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}
		
	if(CertificateList->tbsCertList.nextUpdate == NULL) 
	{
		PRINT_DEBUG("LIBBMDCRLERR Next update not present. Error=%i\n",ERR_CRL_NEXT_UPDATE_NOT_PRESENT);
		return ERR_CRL_NEXT_UPDATE_NOT_PRESENT;
	}
	err = Time2string(CertificateList->tbsCertList.nextUpdate, NextUpdate);
	if(err<0)
	{
		PRINT_DEBUG("LIBBMDCRLERR Error converting Time_t to string.\n");
		return err;
	}
	return err;
}

long GetNumberOfRevokedCertsFromCertificateList(CertificateList_t *CertificateList, long *NoOfRevokedCerts)
{
long err = 0;

	PRINT_VDEBUG("LIBBMDCRLINF Getting number of revoked certificates from certificate list\n");
	if(CertificateList == NULL)
	{
		PRINT_DEBUG("LIBBMDCRLERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(NoOfRevokedCerts == NULL || *NoOfRevokedCerts < 0)
	{
		PRINT_DEBUG("LIBBMDCRLERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}

	if(CertificateList->tbsCertList.revokedCertificates == NULL) { *NoOfRevokedCerts = 0; return err; }
	*NoOfRevokedCerts = (long) CertificateList->tbsCertList.revokedCertificates->list.count;
	return err;
}

/*****************
 * LOW LEVEL API *
 ****************/
long GetExtensionsFromCertificateList(CertificateList_t *CertificateList, Extensions_t **Extensions)
{
long err = 0;
		
	if(CertificateList == NULL)
	{
		PRINT_DEBUG("LIBBMDCRLERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(Extensions == NULL || *Extensions != NULL)
	{
		PRINT_DEBUG("LIBBMDCRLERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}
		
	if(CertificateList->tbsCertList.crlExtensions == NULL)
	{
		PRINT_DEBUG("LIBBMDCRLERR CRL extenisions not present. Error=%i\n",ERR_CRL_EXTENSIONS_NOT_PRESENT);
		return ERR_CRL_EXTENSIONS_NOT_PRESENT;
	}
	
	*Extensions = CertificateList->tbsCertList.crlExtensions;
	return err;
}

long GetCRLExtension(Extensions_t *Extensions, CRLExtType_t CRLExtType, char **ExtensionString, long *ExtensionLong)
{
long err = 0;
	
	if(Extensions == NULL)
	{
		PRINT_DEBUG("LIBBMDCRLERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(CRLExtType < 0)
	{
		PRINT_DEBUG("LIBBMDCRLERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}
	if(ExtensionString == NULL || *ExtensionString != NULL)
	{
		PRINT_DEBUG("LIBBMDCRLERR Invalid third parameter value. Error=%i\n",BMD_ERR_PARAM3);
		return BMD_ERR_PARAM3;
	}
	if(ExtensionLong == NULL || *ExtensionLong < 0)
	{
		PRINT_DEBUG("LIBBMDCRLERR Invalid fourth parameter value. Error=%i\n",BMD_ERR_PARAM4);
		return BMD_ERR_PARAM4;
	}
	
	switch(CRLExtType)
	{
		case CRL_EXT_SN:
			err = GetSerialNumberFromCRLExtension(Extensions, ExtensionString);
			if(err<0)
			{
				PRINT_DEBUG("LIBBMDCRLERR Error in getting serial number from CRL extension. Error=%li\n",err);
				return err;
			}
			break;
		case CRL_EXT_DELTA:
			err = GetIsDeltaInfoFromCRLExtension(Extensions, ExtensionString);
			if(err<0) 
			{
				PRINT_DEBUG("LIBBMDCRLERR Error in checking if delta info is present in CRL extension. Error=%li\n",err);
				return err;
			}
			break;
		default:
			PRINT_DEBUG("LIBBMDCRLERR Unrecognized CRL extension type!\n");
			return ERR_CRL_UNRECOGNIZED_EXT_TYPE;
			break;	
	}
	return err;
}

long GetSerialNumberFromCRLExtension(Extensions_t *Extensions, char **SerialNumber)
{
long err 		= 0;
long i 			= 0;
CRLExtType_t CRLExtType = CRL_EXT_UNDEFINED;
long SerialExtFound 	= 0;

	if(Extensions == NULL)
	{
		PRINT_DEBUG("LIBBMDCRLERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(SerialNumber == NULL || *SerialNumber != NULL)
		return BMD_ERR_PARAM2;

	for(i=0; i<Extensions->list.count; i++){
		err = GetCrlExtType(&(Extensions->list.array[i]->extnID), &CRLExtType);
		if(err<0 && err!=ERR_CRL_UNDEFINED_EXTENSION_TYPE){ /* nierozpoznane typy rozszerzen ingorujemy */
			PRINT_DEBUG("LIBBMDCRLERR Error in getting CRL extension type. Error=%li\n", err);
			return ERR_CRL_CHECK_EXT_TYPE;
		}
		if(CRLExtType == CRL_EXT_SN){
			err = ExtensionSerial2string(&(Extensions->list.array[i]->extnValue), SerialNumber);
			if(err<0){
				PRINT_DEBUG("LIBBMDCRLERR Error in converting CRL extension serial to string. Error=%li\n", err);
				return err;
			}
			SerialExtFound = 1;
			break;
		}
	}
	if(SerialExtFound == 0){
		return ERR_CRL_SERIAL_NOT_FOUND;
	}
	return err;
}

long GetIsDeltaInfoFromCRLExtension(Extensions_t *Extensions, char **isDelta)
{
long err 		= 0;
long i 			= 0;
CRLExtType_t CRLExtType = CRL_EXT_UNDEFINED;
long DeltaExtFound 	= 0;

	if(Extensions == NULL)
		return BMD_ERR_PARAM1;
	if(isDelta == NULL || *isDelta != NULL)
		return BMD_ERR_PARAM2;
	
	for(i=0; i<Extensions->list.count; i++){
		err = GetCrlExtType(&(Extensions->list.array[i]->extnID), &CRLExtType);
		if(err<0 && err!=ERR_CRL_UNDEFINED_EXTENSION_TYPE){ /* nierozpoznane typy rozszerzen ingorujemy */
			PRINT_DEBUG("LIBBMDCRLERR Error in getting CRL extension type. Error=%li\n", err);
			return ERR_CRL_CHECK_EXT_TYPE;
		}
		if(err==ERR_CRL_UNDEFINED_EXTENSION_TYPE) err=0;
		if(CRLExtType == CRL_EXT_DELTA){
			err = ExtensionSerial2string(&(Extensions->list.array[i]->extnValue), isDelta);
			if(err<0){
				PRINT_DEBUG("LIBBMDCRLERR Error in converting CRL extension isDelta to string. Error=%li\n", err);
				return err;
			}
			DeltaExtFound = 1;
			break;
		}
	}
	if(DeltaExtFound == 0){
		asprintf(isDelta, "0");
		if(*isDelta == NULL) return NO_MEMORY;
	}
	return err;
}

long GetCrlExtType(OBJECT_IDENTIFIER_t *Oid, CRLExtType_t *CRLExtType)
{
long err = 0;
long *extension_oid		= NULL;
long extension_oid_size				= 0;
long OID_id_ce_crlNumber[] 		= {OID_CRL_ID_CE_CRLNUMBER};
long OID_id_ce_deltaCRLIndicator[] 	= {OID_CRL_ID_CE_DELTACRLINDICATOR};
		
	if(Oid == NULL)
		return BMD_ERR_PARAM1;
	if(CRLExtType == NULL || *CRLExtType < 0)
		return BMD_ERR_PARAM2;	
		
	*CRLExtType = CRL_EXT_UNDEFINED;
	extension_oid_size = OID_to_ulong_alloc2(Oid, &extension_oid);
	if(extension_oid == NULL) return ERR_NO_MEMORY;
		
	if(!OID_cmp2(OID_id_ce_crlNumber, sizeof(OID_id_ce_crlNumber),
			extension_oid, extension_oid_size * sizeof(long))){
		*CRLExtType = CRL_EXT_SN;
	}
	else if(!OID_cmp2(OID_id_ce_deltaCRLIndicator, sizeof(OID_id_ce_deltaCRLIndicator),
			extension_oid, extension_oid_size * sizeof(long))){
		*CRLExtType = CRL_EXT_DELTA;
	}
	if(extension_oid) {free(extension_oid); extension_oid=NULL;}
	/* else if - tu dopisywac wykrywanie kolejnych rozszerzeń CRLa*/
	if(*CRLExtType == CRL_EXT_UNDEFINED)
		return ERR_CRL_UNDEFINED_EXTENSION_TYPE;
	return err;
}

long ExtensionSerial2string(OCTET_STRING_t *octet_string, char **string)
{
long err = 0;
asn_dec_rval_t rval;
INTEGER_t *integer = 0;
		
	if(octet_string == NULL || octet_string->buf == NULL || octet_string->size == 0)
		return BMD_ERR_PARAM1;
	if(string == NULL || *string != NULL)
		return BMD_ERR_PARAM2;

	rval = ber_decode(0, &asn_DEF_INTEGER,  (void **)&integer,
		octet_string->buf,
		octet_string->size);
	if(rval.code != RC_OK){
		asn_DEF_INTEGER.free_struct(&asn_DEF_INTEGER, integer, 0);
		return ERR_CRL_DECODE_CRL_SERIAL;
	}
	err = INTEGER2string(integer, string);
	if(err<0){
		PRINT_DEBUG("LIBBMDCRLERR converting CRL serial integer to string\n");
		return err;
	}
	asn_DEF_INTEGER.free_struct(&asn_DEF_INTEGER, integer, 0);
	return err;
}

/** @todo poprawic ta funkcje - jest zrobiona analogicznie do ostatniej z libbmdX509 */
long GetSigValueFromCRL(GenBuf_t *crl_buf,GenBuf_t **sig_value,GenBuf_t **DER_output)
{
	long status;
	long rozmiar;
	CertificateList_t *CertificateList = NULL;
	
	status=GetCertificateListFromCRL(crl_buf, &CertificateList);
	
	if(status==0)
	{
		asn1_encode(&asn_DEF_TBSCertList,&(CertificateList->tbsCertList), NULL, DER_output);
		rozmiar=CertificateList->signature.size;
		(*sig_value)=(GenBuf_t *)malloc(sizeof(GenBuf_t));
		(*sig_value)->buf=(char *)malloc(rozmiar+2);
		memset((*sig_value)->buf,0,rozmiar+1);
		(*sig_value)->size=rozmiar;
		memmove((*sig_value)->buf,CertificateList->signature.buf,rozmiar);
	}
	else
		return -1;
	return 0;
}

