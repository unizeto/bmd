#ifndef _AC_GeneralizedTime_INCLUDED_
#define _AC_GeneralizedTime_INCLUDED_

#include <bmd/libbmdasn1_core/GeneralizedTime.h>
#include <bmd/libbmdaa2/wrappers/AC_wrappers.h>

#if defined(__cplusplus)
extern "C" {
#endif
	int AC_GeneralizedTime_create_from_tm(struct tm *time, GeneralizedTime_t *GeneralizedTime);
	int AC_GeneralizedTime_create_from_tm_alloc(struct tm *time, GeneralizedTime_t **GeneralizedTime);
	int AC_GeneralizedTime_create_from_time(time_t * time, GeneralizedTime_t *GeneralizedTime);
	int AC_GeneralizedTime_create_from_time_alloc(time_t *time, GeneralizedTime_t **GeneralizedTime);
#if defined(__cplusplus)
}
#endif /*__cplusplus*/
#endif
