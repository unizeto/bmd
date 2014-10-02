#ifndef _AC_DB_DATABASE_INCLUDED_
#define _AC_DB_DATABASE_INCLUDED_

#include <bmd/libbmdaa2/configuration/AA_config.h>

#include <bmd/libbmdaa2/db/common/AC_db_common.h>
#include <bmd/libbmdaa2/db/common/AC_db_clearance.h>
#include <bmd/libbmdaa2/db/common/AC_db_IetfAttrSyntax.h>
#include <bmd/libbmdaa2/db/common/AC_db_role.h>
#include <bmd/libbmdaa2/db/common/AC_db_UnizetoAttrSyntax.h>

/*#ifdef WITH_POSTGRES
#include <bmd/libbmddb/postgres/pq_libbmddb.h>
#endif
#ifdef WITH_ORACLE
#include <bmd/libbmddb/oracle/libbmddb_oracle.h>
#endif
*/
#include <bmd/libbmddb/libbmddb.h>
/*#define ERR_OCI_NO_DATA -606*/

#if defined(__cplusplus)
extern "C" {
#endif

	/***********************************************/
	/* odczyt atrybutow z bazy danych do struktury */
	/***********************************************/
	long ST_ReadAttributesFromDatabase(	const char *CertificateSerialNr,
						const char *CertificateIssuerDN,
						const ConfigData_t *ConfigData,
						certificate_attributes_t **UserAttributes);

	long ST_FindUserInDatabase(	const char *CertificateSerialNr,
						const char *CertificateIssuerDN,
						certificate_attributes_t **UserAttributes);

	long ST_FindUsersAttrInDatabase(	const char *CertificateSerialNr,
							const char *CertificateIssuerDN,
							const char *attr_kind,
							certificate_attributes_t **UserAttributes);

	/************************************************/
	/* Odczyt atrybutów z bazy danych		*/
	/************************************************/
	long AA_ReadAttributesFromDatabase(const char *CertificateSerialNr,
				const char *CertificateIssuerDN,
				const ConfigData_t *ConfigData,
				AttributesSequence_t **AttributesSequence);
	long AA_FindUserInDatabase(void *DatabaseConnectionHandle,
				const char *CertificateSerialNr,
				const char *CertificateIssuerDN,
				char **UserId);
	long AA_ReadAttributesOwnedByUser(void *DatabaseConnectionHandle, const char *UserId,
				AttributesSequence_t **AttributesSequence,
				const ConfigData_t *ConfigData);
	/***********************
	* USERS
	***********************/
	long AA_SelectUserId(void *DatabaseConnectionHandle,
				const char *CertificateSerialNr,
				const char *CertificateIssuerDN,
				char **UserId);
	long AA_QueryResult2UserId(void *DatabaseConnectionHandle, void *QueryResult, char **UserId);
	/***********************
	* GROUPS
	***********************/
	long AA_ReadGroupsFromDatabase(void *DatabaseConnectionHandle, const char *UserId,
				AttributesSequence_t *AttributesSequence,
				const ConfigData_t *ConfigData);
	long AA_SelectGroups(	void *DatabaseConnectionHandle,
					const char *UserId,
					GenBufList_t **GenBufList);
// 	long AA_GroupsRolesQueryResult2GenBufList(void *DatabaseConnectionHandle,
// 				void *QueryResult, GenBufList_t **GenBufList);
	/***********************
	* ROLES
	***********************/
	long AA_ReadRolesFromDatabase(void *DatabaseConnectionHandle, const char *UserId,
				AttributesSequence_t *AttributesSequence,
				const ConfigData_t *ConfigData);
	long AA_SelectRoles(	void *hDB,
					const char *UserId,
					GenBufList_t **GenBufList);
	/***********************
	* CLEARANCES
	***********************/
	long AA_ReadClearancesFromDatabase(void *DatabaseConnectionHandle, const char *UserId,
				AttributesSequence_t *AttributesSequence,
				const ConfigData_t *ConfigData);
	long AA_SelectClearances(void *DatabaseConnectionHandle, const char *UserId,
				void **QueryResult);
	long AA_ClearancesQueryResult2GenBufList(void *DatabaseConnectionHandle, void *QueryResult,
				GenBufList_t **GenBufList);
	/***********************
	* UNIZETOS
	***********************/
	long AA_ReadUnizetosFromDatabase(void *DatabaseConnectionHandle, const char *UserId,
				AttributesSequence_t *AttributesSequence,
				const ConfigData_t *ConfigData);
	long AA_FindUnizetoIdsForGivenUser(void *DatabaseConnectionHandle,
				const char *UserId, const ConfigData_t *ConfigData, void **QueryResult);
	long AA_UnizetoIdQuery2UnizetoIDChar(void *DatabaseConnectionHandle, void *UnizetoIdQueryResult,
				long *fetched_counter, char **UnizetoAttrSyntaxId);
	long AA_UnizetoIdQuery2UnizetoAttrSyntaxIn(void *DatabaseConnectionHandle,
				char *UnizetoAttrIdString, UnizetoAttrSyntaxIn_t **UnizetoAttrSyntaxIn);
	long AA_UnizetoAttr_set_SyntaxOID(void *DatabaseConnectionHandle, char *UnizetoAttrIdString,
				UnizetoAttrSyntaxIn_t *UnizetoAttrSyntaxIn);
	long AA_UnizetoAttr_set_Slots(void *DatabaseConnectionHandle, char *UnizetoAttrIdString,
				UnizetoAttrSyntaxIn_t *UnizetoAttrSyntaxIn);
	long AA_FindSlotsForGivenUnizeto(void *DatabaseConnectionHandle, const char *UnizetoAttrIdString,
				void **UnizetoIdQueryResult);
	long AA_GetSlotTypeAndIdFromResult(void *DatabaseConnectionHandle, void *UnizetoIdQueryResult,
				long *fetched_counter,
				char **SlotIdString, char **SlotTypeString);
	long AA_CreateEmptySlotIn(SlotIn_t **SlotIn);
	long AA_SlotIn_set_Type(const char *SlotTypeString, SlotIn_t *SlotIn);
	long AA_SlotIn_set_Values(void *DatabaseConnectionHandle, const char *SlotIdString,
				SlotIn_t *SlotIn);
	long AA_FindSlotValuesForGivenSlot(void *DatabaseConnectionHandle, const char *SlotIdString,
				void **UnizetoIdQueryResult);
	long AA_SlotValues2SlotInGenBufList(void *DatabaseConnectionHandle, void *ValuesQueryResult,
				SlotIn_t *SlotIn);
	long AA_SlotValues2GenBuf(void *DatabaseConnectionHandle, void *ValuesQueryResult,
				long *fetched_counter, GenBuf_t **GenBuf);



#if defined(__cplusplus)
}
#endif /*__cplusplus*/
#endif /*_AC_DB_DATABASE_INCLUDED_*/



