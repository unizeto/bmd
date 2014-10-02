#include <bmd/libbmdcms/libbmdcms.h>
#include <bmd/libbmdX509/libbmdX509.h>

#define CERTIFICATE_FILENAME "bbanan1024.cer"

extern long _GLOBAL_debug_level;

int main(void)
{
int err			= 0;
GenBuf_t *GenBuf	= NULL;
long version		= 0;
char *SerialNumber	= NULL;
char *AttributeString	= NULL;
long AttributeLong	= 0;
char *SubjectDN		= NULL;
char *IssuerDN		= NULL;
char *SignatureAlgorithmOID = NULL;
char *ValidityNB	= NULL;
char *ValidityNA	= NULL;
char *SubjectSurname	= NULL;
char *SubjectGivenName	= NULL;
char *SubjectCommonName	= NULL;
char *CRLDistPoints	= NULL;

	_GLOBAL_debug_level = 2;
	err = read_GenBuf_from_file(CERTIFICATE_FILENAME, &GenBuf);
	if(err < 0){
		printf("ERR[%s:%i:%s] Error in reading certificate file. Returned error code = %i\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	printf("INFO[%s:%i:%s] Certificate file read. Returned error code = %i\n",
		__FILE__, __LINE__, __FUNCTION__, err);

	printf("*******************************************************************************\n");
	err = X509CertGetAttr(GenBuf, X509_ATTR_VERSION, &AttributeString, &version);
	if(err < 0){
		printf("ERR[%s:%i:%s] Error in reading version from certificate. Returned error code = %i\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	printf("INFO[%s:%i:%s] Certificate version: %ld\n",
		__FILE__, __LINE__, __FUNCTION__,
		version);

	err = X509CertGetAttr(GenBuf, X509_ATTR_SN, &SerialNumber, &AttributeLong);
	if(err < 0){
		printf("ERR[%s:%i:%s] Error in reading serial number from certificate. Returned error code = %i\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	printf("INFO[%s:%i:%s] Certificate serial number: %s\n",
		__FILE__, __LINE__, __FUNCTION__,
		SerialNumber);
	if(SerialNumber) { free(SerialNumber); SerialNumber=NULL; }

	err = X509CertGetAttr(GenBuf, X509_ATTR_SUBJECT_DN, &SubjectDN, &AttributeLong);
	if(err < 0){
		printf("ERR[%s:%i:%s] Error in reading subject DN from certificate. Returned error code = %i\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	printf("INFO[%s:%i:%s] Subject DN: %s\n",
		__FILE__, __LINE__, __FUNCTION__,
		SubjectDN);
	if(SubjectDN) { free(SubjectDN); SubjectDN=NULL; }

	err = X509CertGetAttr(GenBuf, X509_ATTR_ISSUER_DN, &IssuerDN, &AttributeLong);
	if(err < 0){
		printf("ERR[%s:%i:%s] Error in reading issuer DN from certificate. Returned error code = %i\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	printf("INFO[%s:%i:%s] Issuer DN: %s\n",
		__FILE__, __LINE__, __FUNCTION__,
		IssuerDN);
	if(IssuerDN) { free(IssuerDN); IssuerDN=NULL; }

	err = X509CertGetAttr(GenBuf, X509_ATTR_SIGALG_OID, &SignatureAlgorithmOID, &AttributeLong);
	if(err < 0){
		printf("ERR[%s:%i:%s] Error in reading signature algorithm OID from certificate. Returned error code = %i\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	printf("INFO[%s:%i:%s] Signature algorithm OID: %s\n",
		__FILE__, __LINE__, __FUNCTION__,
		SignatureAlgorithmOID);
	if(SignatureAlgorithmOID) { free(SignatureAlgorithmOID); SignatureAlgorithmOID=NULL; }

	err = X509CertGetAttr(GenBuf, X509_ATTR_VALIDITY_NB, &ValidityNB, &AttributeLong);
	if(err < 0){
		printf("ERR[%s:%i:%s] Error in reading validity NotBefore from certificate. Returned error code = %i\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	printf("INFO[%s:%i:%s] Validity NotBefore: %s\n",
		__FILE__, __LINE__, __FUNCTION__,
		ValidityNB);
	if(ValidityNB) { free(ValidityNB); ValidityNB=NULL; }

	err = X509CertGetAttr(GenBuf, X509_ATTR_VALIDITY_NA, &ValidityNA, &AttributeLong);
	if(err < 0){
		printf("ERR[%s:%i:%s] Error in reading validity NotAfter from certificate. Returned error code = %i\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	printf("INFO[%s:%i:%s] Validity NotAfter: %s\n",
		__FILE__, __LINE__, __FUNCTION__,
		ValidityNA);
	if(ValidityNA) { free(ValidityNA); ValidityNA=NULL; }

/*	err = X509CertGetAttr(GenBuf, X509_ATTR_SUBJECT_SURNAME, &SubjectSurname, &AttributeLong);
	if(err < 0){
		printf("ERR[%s:%i:%s] Error in reading subject surname from certificate. Returned error code = %i\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	printf("INFO[%s:%i:%s] Subject surname: %s\n",
		__FILE__, __LINE__, __FUNCTION__,
		SubjectSurname);
	if(SubjectSurname) { free(SubjectSurname); SubjectSurname=NULL; }

	err = X509CertGetAttr(GenBuf, X509_ATTR_SUBJECT_GIVENNAME, &SubjectGivenName, &AttributeLong);
	if(err < 0){
		printf("ERR[%s:%i:%s] Error in reading subject givenname from certificate. Returned error code = %i\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	printf("INFO[%s:%i:%s] Subject givenname: %s\n",
		__FILE__, __LINE__, __FUNCTION__,
		SubjectGivenName);
	if(SubjectGivenName) { free(SubjectGivenName); SubjectGivenName=NULL; }

*/	err = X509CertGetAttr(GenBuf, X509_ATTR_SUBJECT_COMMONNAME, &SubjectCommonName, &AttributeLong);
	if(err < 0){
		printf("ERR[%s:%i:%s] Error in reading subject common from certificate. Returned error code = %i\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	printf("INFO[%s:%i:%s] Subject commonname: %s\n",
		__FILE__, __LINE__, __FUNCTION__,
		SubjectCommonName);
	if(SubjectCommonName) { free(SubjectCommonName); SubjectCommonName=NULL; }


	err = X509CertGetAttr(GenBuf, X509_ATTR_ISSUER_COMMONNAME, &AttributeString, &AttributeLong);
	if(err < 0){
		printf("ERR[%s:%i:%s] Error in reading issuer commonname from certificate. Returned error code = %i\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	printf("INFO[%s:%i:%s] Issuer Commonaname: %s\n",
		__FILE__, __LINE__, __FUNCTION__,
		AttributeString);
	if(AttributeString) { free(AttributeString); AttributeString=NULL; }


	err = X509CertGetAttr(GenBuf, X509_ATTR_SUBJECT_ORGANIZATION, &AttributeString, &AttributeLong);
	if(err < 0){
		printf("ERR[%s:%i:%s] Error in reading subject organization from certificate. Returned error code = %i\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	printf("INFO[%s:%i:%s] Subject organization: %s\n",
		__FILE__, __LINE__, __FUNCTION__,
		AttributeString);
	if(AttributeString) { free(AttributeString); AttributeString=NULL; }

	err = X509CertGetAttr(GenBuf, X509_ATTR_EXT_CRL_DISPOINTS, &CRLDistPoints, &AttributeLong);
	if(err < 0){
		printf("ERR[%s:%i:%s] Error in reading CRL distribution points from certificate. Returned error code = %i\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	printf("INFO[%s:%i:%s] CRL distribution points: %s\n",
		__FILE__, __LINE__, __FUNCTION__,
		CRLDistPoints);
	if(CRLDistPoints) { free(CRLDistPoints); CRLDistPoints=NULL; }


	free_gen_buf(&GenBuf);
	return err;
}


