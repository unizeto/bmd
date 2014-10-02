#ifndef _AC_DB_UNIZETOATTRSYNTAX_INCLUDED_
#define _AC_DB_UNIZETOATTRSYNTAX_INCLUDED_

#include <bmd/libbmdaa2/configuration/AA_config.h>
#include <bmd/libbmdaa2/wrappers/AC_wrappers.h>
#include <bmd/libbmdasn1_common/AttributesSequence.h>

typedef struct SlotIn {
	GenBuf_t *OidString;
	GenBufList_t *ValuesList;
} SlotIn_t;

typedef struct SlotListIn{
	long size;
	SlotIn_t **Slots;
} SlotListIn_t;

typedef struct UnizetoAttrSyntaxIn{
	GenBuf_t *SyntaxOid;
	SlotListIn_t *SlotList;
} UnizetoAttrSyntaxIn_t;

typedef struct UnizetoAttrSyntaxInList{
	long size;
	UnizetoAttrSyntaxIn_t **UnizetoAttrSyntaxIn;
} UnizetoAttrSyntaxInList_t;


#include <bmd/libbmdaa2/db/common/AC_db_common.h>

#if defined(__cplusplus)
extern "C" {
#endif
	long AC_MakeAttribute_UnizetoAttrSyntax(UnizetoAttrSyntaxInList_t *UnizetoAttrSyntaxInList,
				AttributesSequence_t *AttributesSequence,
				const ConfigData_t *ConfigData);
	long AC_UnizetoAttrSyntax_build(UnizetoAttrSyntaxIn_t *UnizetoAttrSyntaxIn,
				UnizetoAttrSyntax_t **UnizetoAttrSyntax, 
				const ConfigData_t *ConfigData);
	long AC_Slot_build(SlotIn_t *SlotIn, 
				Slot_t **Slot, 
				const ConfigData_t *ConfigData);

#if defined(__cplusplus)
}
#endif /*__cplusplus*/
#endif /*_AC_DB_UNIZETOATTRSYNTAX_INCLUDED_*/
