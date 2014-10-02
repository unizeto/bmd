#ifndef _AC_DB_FILE_INCLUDED_
#define _AC_DB_FILE_INCLUDED_

#include <bmd/libbmdaa2/configuration/AA_config.h>

#include <bmd/libbmdaa2/db/common/AC_db_common.h>
#include <bmd/libbmdaa2/db/common/AC_db_clearance.h>
#include <bmd/libbmdaa2/db/common/AC_db_IetfAttrSyntax.h>
#include <bmd/libbmdaa2/db/common/AC_db_role.h>
#include <bmd/libbmdaa2/db/common/AC_db_UnizetoAttrSyntax.h>

#include <bmd/libbmdaa2/wrappers/AC_wrappers.h>

#if defined(__cplusplus)
extern "C" {
#endif
	long AA_ReadAttributesFromFile(const char *filename,
			const char *CertificateSerialNr,
			const char *CertificateIssuerDN,
			const ConfigData_t *ConfigData,
			AttributesSequence_t **AttributesSequence);

	long AA_FindUserInAttributesDB(GenBuf_t *AttributesDB, 
			const char *CertificateSerialNr,
			const char *CertificateIssuerDN,
			GenBuf_t **AttributesDBUserLine);

	long AA_ReadAttributesFromLine(GenBuf_t *AttributesLine,
			AttributesSequence_t **AttributesSequence,
			const ConfigData_t *ConfigData);

	long AA_ReadGroupsFromLine(GenBuf_t *AttributesLine,
			AttributesSequence_t *AttributesSequence,
			const ConfigData_t *ConfigData);

	long AA_ReadRolesFromLine(GenBuf_t *AttributesLine,
			AttributesSequence_t *AttributesSequence,
			const ConfigData_t *ConfigData);

	long AA_ReadClearancesFromLine(GenBuf_t *AttributesLine,
			AttributesSequence_t *AttributesSequence,
			const ConfigData_t *ConfigData);

	long AA_ReadUnizetosFromLine(GenBuf_t *AttributesLine,
			AttributesSequence_t *AttributesSequence,
			const ConfigData_t *ConfigData);
	
	long Substrings2GenBufList(SubstringsGenBuf_t *SubstringsGenBuf, 
			GenBufList_t **GenBufList, 
			long step, long offset, long setsize);

	long GenBuf2UnizetoAttrSyntaxInput(GenBuf_t *GenBuf, 
			UnizetoAttrSyntaxIn_t **UnizetoAttrSyntaxIn);
	
	long CreateEmptyUnizetoAttrSyntaxIn(GenBufList_t *SlotsGenBufList, 
			UnizetoAttrSyntaxIn_t **UnizetoAttrSyntaxIn);
	long UnizetoAttrSyntaxIn_add_SlotsIn(GenBufList_t *SlotsGenBufList, 
			UnizetoAttrSyntaxIn_t *UnizetoAttrSyntaxIn);
	long SetOidInSlot(GenBuf_t *SlotOid, SlotIn_t *SlotIn);
	long SetValuesInSlot(GenBuf_t *SlotValuesString, SlotIn_t *SlotIn);
	long InsertDataIntoSlotIn(GenBuf_t *SlotOid, GenBuf_t *SlotValuesString, 
			SlotIn_t **SlotIn);
	long GenBufList2UnizetoAttrSyntaxInList(GenBufList_t *GenBufList, 
			UnizetoAttrSyntaxInList_t **UnizetoAttrSyntaxInList);



			
#if defined(__cplusplus)
}
#endif /*__cplusplus*/
#endif /*_AC_DB_FILE_INCLUDED_*/

