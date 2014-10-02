/* AC_OBJECT_IDENTIFIER.h author: Marcin Szulga ver. 2.0.1 */
#ifndef _AC_OBJECT_IDENTIFIER_INCLUDED_
#define _AC_OBJECT_IDENTIFIER_INCLUDED_

#include <bmd/libbmdasn1_core/OBJECT_IDENTIFIER.h>
#include <bmd/libbmdaa2/wrappers/AC_wrappers.h>

#if defined(__cplusplus)
extern "C" {
#endif
	int AC_OBJECT_IDENTIFIER_clone_pointers(OBJECT_IDENTIFIER_t *src, 
					OBJECT_IDENTIFIER_t *dest);
#if defined(__cplusplus)
}
#endif /*__cplusplus*/
#endif /*_AC_OBJECT_IDENTIFIER_INCLUDED_*/
