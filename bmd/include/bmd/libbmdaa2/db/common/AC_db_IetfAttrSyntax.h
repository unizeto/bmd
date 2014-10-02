#ifndef _AC_DB_IETFATTRSYNTAX_INCLUDED_
#define _AC_DB_IETFATTRSYNTAX_INCLUDED_

#include <bmd/libbmdaa2/configuration/AA_config.h>
#include <bmd/libbmdaa2/wrappers/AC_wrappers.h>
#include <bmd/libbmdaa2/db/common/AC_db_common.h>
#include <bmd/libbmdasn1_common/AttributesSequence.h>

#if defined(__cplusplus)
extern "C" {
#endif
	long AC_MakeAttribute_IetfAttrSyntax(GenBufList_t *GroupsGenBufList, 
					AttributesSequence_t *AttributesSequence,
					const ConfigData_t *ConfigData);

	long AC_IetfAttrSyntax_build(const GenBufList_t *GenBufList,
					IetfAttrSyntax_t **IetfAttrSyntax,
					const ConfigData_t *ConfigData);

	long AC_CreateGroupPolicyAuthorityGeneralNames(const ConfigData_t *ConfigData,
					GeneralNames_t **GeneralNames);
		
#if defined(__cplusplus)
}
#endif /*__cplusplus*/
#endif /*_AC_DB_IETFATTRSYNTAX_INCLUDED_*/

