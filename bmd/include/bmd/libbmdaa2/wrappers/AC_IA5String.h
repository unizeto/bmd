/* AC_IA5String.h author: Marcin Szulga ver. 2.0.2 */
#ifndef _AC_IA5String_INCLUDED_
#define _AC_IA5String_INCLUDED_

#include <bmd/libbmdasn1_core/IA5String.h>
#include <bmd/libbmdaa2/wrappers/AC_wrappers.h>

#if defined(__cplusplus)
extern "C" {
#endif
	int AC_IA5String_clone_pointers(IA5String_t *src, IA5String_t *dest);
#if defined(__cplusplus)
}
#endif /*__cplusplus*/
#endif /*_AC_IA5String_INCLUDED_*/
