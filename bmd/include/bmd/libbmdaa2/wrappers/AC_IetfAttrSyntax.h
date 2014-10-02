#ifndef _AC_IetfAttrSyntax_INCLUDED_
#define _AC_IetfAttrSyntax_INCLUDED_

#include <bmd/libbmdasn1_common/IetfAttrSyntax.h>
#include <bmd/libbmdaa2/wrappers/AC_wrappers.h>

typedef enum EnumIetfAttrSyntaxGroupType {
		IETF_GROUP_UNDEFINED,
		IETF_GROUP_OCTET_STRING,
		IETF_GROUP_OID,
		IETF_GROUP_UTF8 } EnumIetfAttrSyntaxGroupType_t;

#if defined(__cplusplus)
extern "C" {
#endif
	int AC_IetfAttrSyntax_add_Group(EnumIetfAttrSyntaxGroupType_t EnumIetfAttrSyntaxGroupType, 
					GenBuf_t *GroupValue, 
					IetfAttrSyntax_t *IetfAttrSyntax);
	int GroupValue2ValueChoice(GenBuf_t *GroupValue, 
					EnumIetfAttrSyntaxGroupType_t EnumIetfAttrSyntaxGroupType,
					IetfAttrSyntax__values__Member_t **IetfAttrSyntaxValueChoice);
#if defined(__cplusplus)
}
#endif /*__cplusplus*/
#endif /*_AC_IetfAttrSyntax_INCLUDED_*/
