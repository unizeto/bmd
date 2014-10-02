#ifndef _AC_DB_CLEARANCE_INCLUDED_
#define _AC_DB_CLEARANCE_INCLUDED_

#include <bmd/libbmdaa2/configuration/AA_config.h>

#include <bmd/libbmdaa2/wrappers/AC_Clearance.h>
#include <bmd/libbmdaa2/wrappers/AC_SecurityCategory.h>
#include <bmd/libbmdaa2/wrappers/AC_ClassList.h>
#include <bmd/libbmdasn1_common/AttributesSequence.h>

/* Funkcja alokuje pamiec pod wezly drzewa Clearance */
#if defined(__cplusplus)
extern "C" {
#endif
	long AC_MakeAttribute_Clearance(GenBufList_t *ClearancesGenBufList,
				AttributesSequence_t *AttributesSequence,
				const ConfigData_t *ConfigData);
				
	long AC_Clearance_build(const GenBuf_t *ClassListString,
				const GenBuf_t *SecurityCategoryTypeString, 
				const GenBuf_t *SecurityCategoryValueString, 
				Clearance_t **Clearance,
				const ConfigData_t *ConfigData);
		
		
#if defined(__cplusplus)
}
#endif /*__cplusplus*/
#endif /*_AC_DB_CLEARANCE_INCLUDED_*/

