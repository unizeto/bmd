#ifndef _AC_ClassList_INCLUDED_
#define _AC_ClassList_INCLUDED_

#include <bmd/libbmdasn1_common/ClassList.h>
#include <bmd/libbmdaa2/wrappers/AC_wrappers.h>

#if defined(__cplusplus)
extern "C" {
#endif
	int AC_ClassList_create(int level, ClassList_t *ClassList);
	int AC_ClassList_create_alloc(int level, ClassList_t **ClassList);
#if defined(__cplusplus)
}
#endif /*__cplusplus*/
#endif /*_AC_ClassList_INCLUDED_*/
