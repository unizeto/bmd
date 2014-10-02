#ifndef _AC_SecurityCategory_INCLUDED_
#define _AC_SecurityCategory_INCLUDED_

#include <bmd/libbmdasn1_common/SecurityCategory.h>
#include <bmd/libbmdaa2/wrappers/AC_wrappers.h>

#if defined(__cplusplus)
extern "C" {
#endif
	int AC_SecurityCategory_create(const OBJECT_IDENTIFIER_t *SecCatType,
					ANY_t *SecCatValue,
					SecurityCategory_t *SecurityCategory);
	int AC_SecurityCategory_create_alloc(const OBJECT_IDENTIFIER_t *SecCatType, 
					ANY_t *SecCatValue, 
					SecurityCategory_t **SecurityCategory);	
#if defined(__cplusplus)
}
#endif /*__cplusplus*/
#endif /*_AC_SecurityCategory_INCLUDED_*/
