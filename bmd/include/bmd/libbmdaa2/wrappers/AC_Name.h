#ifndef _AC_Name_INCLUDED_
#define _AC_Name_INCLUDED_

#include <bmd/libbmdasn1_common/Name.h>
#include <bmd/libbmdaa2/wrappers/AC_wrappers.h>

#if defined(__cplusplus)
extern "C" {
#endif
	int AC_NameSetAddValue(DNValueType_t DNValueType, const char *DNValue, Name_t *Name);
	int AC_Name_clone_pointers(Name_t *src, Name_t *dest);
#if defined(__cplusplus)
}
#endif /*__cplusplus*/
#endif /*_AC_Name_INCLUDED_*/


