#include <bmd/libbmdcms/libbmdcms.h>
#include <bmd/libbmdts/libbmdts.h>

#define TIMESTAMP_FILENAME "timestamp_response.tsr"
/* #define TIMESTAMP_FILENAME "test.txt.tsr" */

int main(void)
{
int err			= 0;
GenBuf_t *GenBuf	= NULL;
char *TimestampGenTime	= NULL;
char *TsaCertValidityNA = NULL;
long version		= 0;
char *PolicyOid		= NULL;
char *SerialNumber	= NULL;
char *TsaDn		= NULL;
char *TsaCertSn		= NULL;

	_GLOBAL_debug_level=3;
	err = read_GenBuf_from_file(TIMESTAMP_FILENAME, &GenBuf);
	if(err < 0){
		PRINT_DEBUG("ERR[%s:%i:%s] Error in reading timestamp file. Returned error code = %i\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	PRINT_DEBUG("INFO[%s:%i:%s] Timestamp file read. Returned error code = %i\n",
		__FILE__, __LINE__, __FUNCTION__, err);
		
	printf("*******************************************************************************\n");
	err = GetGenerationTimeFromTimestamp(GenBuf, &TimestampGenTime);
	if(err < 0){
		PRINT_DEBUG("ERR[%s:%i:%s] Error in reading timestamp generation time. Returned error code = %i\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	PRINT_DEBUG("INFO[%s:%i:%s] Timestamp generation time: %s\n",
		__FILE__, __LINE__, __FUNCTION__,
		TimestampGenTime);
	if(TimestampGenTime) {free(TimestampGenTime); TimestampGenTime=NULL;}
		
	err = GetVersionFromTimestamp(GenBuf, &version);
	if(err < 0){
		PRINT_DEBUG("ERR[%s:%i:%s] Error in reading version from timestamp. Returned error code = %i\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	PRINT_DEBUG("INFO[%s:%i:%s] Timestamp version: %ld\n",
		__FILE__, __LINE__, __FUNCTION__,
		version);
		
	err = GetPolicyOIDFromTimestamp(GenBuf, &PolicyOid);
	if(err < 0){
		PRINT_DEBUG("ERR[%s:%i:%s] Error in reading policy OID from timestamp. Returned error code = %i\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	PRINT_VDEBUG("INFO[%s:%i:%s] Timestamp policy OID: %s\n",
		__FILE__, __LINE__, __FUNCTION__,
		PolicyOid);
	if(PolicyOid) { free(PolicyOid); PolicyOid=NULL; }
		
	err = GetSerialNumberFromTimestamp(GenBuf, &SerialNumber);
	if(err < 0){
		PRINT_DEBUG("ERR[%s:%i:%s] Error in reading serial number from timestamp. Returned error code = %i\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	PRINT_VDEBUG("INFO[%s:%i:%s] Timestamp serial number: %s\n",
		__FILE__, __LINE__, __FUNCTION__,
		SerialNumber);
	if(SerialNumber) { free(SerialNumber); SerialNumber=NULL; }
		
	err = GetTsaDnFromTimestamp(GenBuf, &TsaDn);
	if(err < 0){
		PRINT_DEBUG("ERR[%s:%i:%s] Error in reading TSA DN from timestamp. Returned error code = %i\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	PRINT_VDEBUG("INFO[%s:%i:%s] Timestamp TSA DN: %s\n",
		__FILE__, __LINE__, __FUNCTION__,
		TsaDn);
	if(TsaDn) { free(TsaDn); TsaDn=NULL; }

	err = GetTsaCertificateSnFromTimestamp(GenBuf, &TsaCertSn);
	if(err < 0){
		PRINT_DEBUG("ERR[%s:%i:%s] Error in reading TSA SN from certificate included in timestamp CMS envelope. "
			"Returned error code = %i\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	PRINT_VDEBUG("INFO[%s:%i:%s] Timestamp TSA Certificate SN: %s\n",
		__FILE__, __LINE__, __FUNCTION__,
		TsaCertSn);

	err = GetTsaCertificateValidityNAFromTimestamp(GenBuf, &TsaCertValidityNA);
	if(err < 0){
		PRINT_DEBUG("ERR[%s:%i:%s] Error in reading TSA certificate validity (notAfter) "
			"from certificate included in timestamp CMS envelope. "
			"Returned error code = %i\n",
			__FILE__, __LINE__, __FUNCTION__, err);
		return err;
	}
	PRINT_VDEBUG("INFO[%s:%i:%s] Timestamp certificate validity notAfter: %s\n",
		__FILE__, __LINE__, __FUNCTION__, TsaCertValidityNA);
	if(TsaCertValidityNA) {free(TsaCertValidityNA); TsaCertValidityNA=NULL;}
	free_gen_buf(&GenBuf);
	return err;
}


