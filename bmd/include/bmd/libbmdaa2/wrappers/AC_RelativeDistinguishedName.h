#ifndef _AC_RelativeDistinguishedName_INCLUDED_
#define _AC_RelativeDistinguishedName_INCLUDED_

#include <bmd/libbmdasn1_common/RelativeDistinguishedName.h>
#include <bmd/libbmdaa2/wrappers/AC_wrappers.h>

#if defined(__cplusplus)
extern "C" {
#endif
	int AC_CreateRelativeDistinguishedNameFromString(DNValueType_t DNValueType, 
				const char *DNValue, 
				RelativeDistinguishedName_t **RelativeDistinguishedName);	
#if defined(__cplusplus)
}
#endif /*__cplusplus*/
#endif /*_AC_RelativeDistinguishedName_INCLUDED_*/

