#ifndef _AC_Attribute_INCLUDED_
#define _AC_Attribute_INCLUDED_

#include <bmd/libbmdasn1_common/Attribute.h>
#include <bmd/libbmdaa2/wrappers/AC_wrappers.h>

#if defined(__cplusplus)
extern "C" {
#endif
	int AC_CreateEmptyAttributeSet(Attribute_t **Attribute);
	int AC_Attribute_set_TypeOid(char *OidType, Attribute_t *Attribute);
#if defined(__cplusplus)
}
#endif /*__cplusplus*/
#endif /*_AC_Attribute_INCLUDED_*/
