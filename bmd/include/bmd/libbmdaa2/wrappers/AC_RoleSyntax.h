#ifndef _AC_RoleSyntax_INCLUDED_
#define _AC_RoleSyntax_INCLUDED_

#include <bmd/libbmdasn1_common/RoleSyntax.h>
#include <bmd/libbmdaa2/wrappers/AC_wrappers.h>

#if defined(__cplusplus)
extern "C" {
#endif
	int AC_RoleSyntax_create(GeneralNames_t *roleAuthority,
				GeneralName_t *roleName,
				RoleSyntax_t *RoleSyntax);
	int AC_RoleSyntax_create_alloc(GeneralNames_t *roleAuthority,
				GeneralName_t *roleName,
				RoleSyntax_t **RoleSyntax);
#if defined(__cplusplus)
}
#endif /*__cplusplus*/
#endif /*_AC_RoleSyntax_INCLUDED_*/
