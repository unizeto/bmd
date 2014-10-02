#ifndef _AC_AttCertValidityPeriod_INCLUDED_
#define _AC_AttCertValidityPeriod_INCLUDED_

#include <bmd/libbmdasn1_common/AttCertValidityPeriod.h>
#include <bmd/libbmdaa2/wrappers/AC_wrappers.h>

#if defined(__cplusplus)
extern "C" {
#endif
	int AC_AttCertValidityPeriod_create(GeneralizedTime_t *notBeforeTime,
					GeneralizedTime_t *notAfterTime,
					AttCertValidityPeriod_t *AttCertValidityPeriod);
#if defined(__cplusplus)
}
#endif /*__cplusplus*/
#endif /*_AC_AttCertValidityPeriod_INCLUDED_*/
