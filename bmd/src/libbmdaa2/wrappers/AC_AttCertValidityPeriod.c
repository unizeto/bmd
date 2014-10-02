#include <bmd/libbmdaa2/wrappers/AC_AttCertValidityPeriod.h>
#include <bmd/libbmderr/libbmderr.h>

int AC_AttCertValidityPeriod_create(GeneralizedTime_t *notBeforeTime,
				GeneralizedTime_t *notAfterTime,
				AttCertValidityPeriod_t *AttCertValidityPeriod)
{
int err = 0;
	if(notBeforeTime == NULL)
		return BMD_ERR_PARAM1;
	if(notAfterTime == NULL)
		return BMD_ERR_PARAM2;
	if(AttCertValidityPeriod == NULL)
		return BMD_ERR_PARAM3;
	
	memset(AttCertValidityPeriod,0,sizeof(AttCertValidityPeriod_t));
	AttCertValidityPeriod->notBeforeTime.buf = notBeforeTime->buf;
	AttCertValidityPeriod->notBeforeTime.size = notBeforeTime->size;
		
	AttCertValidityPeriod->notAfterTime.buf = notAfterTime->buf;
	AttCertValidityPeriod->notAfterTime.size = notAfterTime->size;
	return err;
}
