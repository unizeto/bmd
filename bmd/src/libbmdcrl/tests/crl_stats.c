#include <bmd/libbmdcms/libbmdcms.h>
#include <bmd/libbmdcrl/libbmdcrl.h>

#define CRL_FILENAME "OstatniCRL.crl"

int main(void)
{
int err			= 0;
GenBuf_t *GenBuf	= NULL;
long version		= 0;
char *SerialNumber	= NULL;
char *isDelta		= NULL;
char *SignatureAlgorithm= NULL;
char *IssuerDN		= NULL;
char *ThisUpdate	= NULL;
char *NextUpdate	= NULL;
long NoOfRevokedCerts	= 0;

	err = read_GenBuf_from_file(CRL_FILENAME, &GenBuf);
	if(err < 0){
		printf("ERR[%s:%i:%s] Error in reading CRL file. Returned error code = %i\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	printf("INFO[%s:%i:%s] CRL file read. Returned error code = %i\n",
		__FILE__, __LINE__, __FUNCTION__, err);
		
	printf("*******************************************************************************\n");
	err = GetVersionFromCRL(GenBuf, &version);
	if(err < 0){
		printf("ERR[%s:%i:%s] Error in reading version from CRL. Returned error code = %i\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	printf("INFO[%s:%i:%s] CRL version: %ld\n",
		__FILE__, __LINE__, __FUNCTION__, 
		version);
	err = GetSerialNumberFromCRL(GenBuf, &SerialNumber);
	if(err < 0){
		printf("ERR[%s:%i:%s] Error in reading serial from CRL. Returned error code = %i\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	printf("INFO[%s:%i:%s] CRL serial number: %s\n",
		__FILE__, __LINE__, __FUNCTION__, 
		SerialNumber);

	err = GetDeltaStatusFromCRL(GenBuf, &isDelta);
	if(err < 0){
		printf("ERR[%s:%i:%s] Error in reading delta info from CRL. Returned error code = %i\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	printf("INFO[%s:%i:%s] CRL delta info number: %s\n",
		__FILE__, __LINE__, __FUNCTION__, 
		isDelta);

	err = GetSignatureAlgorithmOIDFromCRL(GenBuf, &SignatureAlgorithm);
	if(err < 0){
		printf("ERR[%s:%i:%s] Error in reading signature algorithm OID from CRL. Returned error code = %i\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	printf("INFO[%s:%i:%s] CRL signature algorithm OID: %s\n",
		__FILE__, __LINE__, __FUNCTION__, 
		SignatureAlgorithm);

	err = GetIssuerDNFromCRL(GenBuf, &IssuerDN);
	if(err < 0){
		printf("ERR[%s:%i:%s] Error in reading issuer DN from CRL. Returned error code = %i\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	printf("INFO[%s:%i:%s] Issuer DN: %s\n",
		__FILE__, __LINE__, __FUNCTION__, 
		IssuerDN);

	err = GetThisUpdateFromCRL(GenBuf, &ThisUpdate);
	if(err < 0){
		printf("ERR[%s:%i:%s] Error in reading this update from CRL. Returned error code = %i\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	printf("INFO[%s:%i:%s] This update: %s\n",
		__FILE__, __LINE__, __FUNCTION__, 
		ThisUpdate);

	err = GetNextUpdateFromCRL(GenBuf, &NextUpdate);
	if(err < 0){
		printf("ERR[%s:%i:%s] Error in reading next update from CRL. Returned error code = %i\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	printf("INFO[%s:%i:%s] Next update: %s\n",
		__FILE__, __LINE__, __FUNCTION__, 
		NextUpdate);

	err = GetNumberOfRevokedCertsFromCRL(GenBuf, &NoOfRevokedCerts);
	if(err < 0){
		printf("ERR[%s:%i:%s] Error in reading number of revoked certs from CRL. Returned error code = %i\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	printf("INFO[%s:%i:%s] No of revoked certs: %li\n",
		__FILE__, __LINE__, __FUNCTION__, 
		NoOfRevokedCerts);

	if(isDelta)		{free(isDelta);			isDelta=NULL;			}
	if(SerialNumber)	{free(SerialNumber);		SerialNumber=NULL;		}
	if(SignatureAlgorithm)	{free(SignatureAlgorithm);	SignatureAlgorithm=NULL;	}
	if(IssuerDN)		{free(IssuerDN);		IssuerDN=NULL;			}
	if(ThisUpdate)		{free(ThisUpdate);		ThisUpdate=NULL;			}
	if(NextUpdate)		{free(NextUpdate);		NextUpdate=NULL;			}

	free_gen_buf(&GenBuf);
	return err;
}


