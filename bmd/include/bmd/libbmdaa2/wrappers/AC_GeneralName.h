#ifndef _AC_GeneralName_INCLUDED_
#define _AC_GeneralName_INCLUDED_

#include <bmd/libbmdasn1_common/GeneralName.h>
#include <bmd/libbmdaa2/wrappers/AC_wrappers.h>

#if defined(__cplusplus)
extern "C" {
#endif
	int AC_GeneralName_create(void *value, GeneralName_PR name_type, GeneralName_t *GeneralName);
	int AC_GeneralName_create_alloc(void *value, GeneralName_PR name_type, GeneralName_t **GeneralName);
	int AC_GeneralName_clone_pointers(GeneralName_t *src, GeneralName_t *dest);
#if defined(__cplusplus)
}
#endif /*__cplusplus*/
#endif /*_AC_GeneralName_INCLUDED_*/
