#ifndef _AC_DB_ROLE_INCLUDED_
#define _AC_DB_ROLE_INCLUDED_

#include <bmd/libbmdaa2/configuration/AA_config.h>
#include <bmd/libbmdaa2/db/common/AC_db_common.h>

#if defined(__cplusplus)
extern "C" {
#endif
	long AC_MakeAttribute_RoleSyntax(GenBufList_t *RolesGenBufList,
				AttributesSequence_t *AttributesSequence,
				const ConfigData_t *ConfigData);
				
	long AC_RoleSyntax_build(const GenBuf_t *Role,
				RoleSyntax_t **RoleSyntax,
				const ConfigData_t *ConfigData);
				
#if defined(__cplusplus)
}
#endif /*__cplusplus*/
#endif /*_AC_DB_ROLE_INCLUDED_*/


