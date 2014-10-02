#ifndef _AC_UnizetoAttrSyntax_INCLUDED_
#define _AC_UnizetoAttrSyntax_INCLUDED_

#include <bmd/libbmdasn1_common/UnizetoAttrSyntax.h>
#include <bmd/libbmdaa2/wrappers/AC_wrappers.h>

#if defined(__cplusplus)
extern "C" {
#endif
	int AC_UnizetoAttrSyntax_create(GeneralNames_t *policyAuthority,
					OBJECT_IDENTIFIER_t *syntaxId,
					UnizetoAttrSyntax_t *UnizetoAttrSyntax);
	int AC_UnizetoAttrSyntax_create_alloc(GeneralNames_t *policyAuthority,
					OBJECT_IDENTIFIER_t *SyntaxId,
					UnizetoAttrSyntax_t **UnizetoAttrSyntax);
#if defined(__cplusplus)
}
#endif /*__cplusplus*/
#endif /*_AC_UnizetoAttrSyntax_INCLUDED_*/
