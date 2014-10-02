#ifndef _JS_COMMON_INCLUDED_
#define _JS_COMMON_INCLUDED_

#include <bmd/libbmdsql/libbmdsql.h>
/*#include <bmd_pr_common.h>*/
#include <bmd/libbmdutils/libbmdutils.h>
#include <bmd/libbmddb/libbmddb.h>
#include <bmd/libbmdpr/bmd_datagram/asn1clib/BMDDatagram.h>
#include <bmd/libbmdpr/bmd_datagram/bmd_datagram.h>
#include <bmd/common/global_types.h>

#define IGNORE_DOC_VISIBILITY 4
#define IGNORE_DOC_VISIBILITY_STRING "4"

typedef enum GetInCGIMode {NO_CGI_MODE, CGI_MODE} GetInCGIMode_t;
typedef enum EnumSearchMode { SEARCH_WITH_AND, SEARCH_WITH_OR } EnumSearchMode_t;
typedef enum EnumSearchResultMode { SEARCH_RESULT_LIST, SEARCH_RESULT_COUNT } EnumSearchResultMode_t;
typedef enum EnumActionRegisterObjectType { UNDEFINED_OBJECT_TYPE, CRYPTO_OBJECT_OBJECT_TYPE, GROUP_OBJECT_TYPE, ROLE_OBJECT_TYPE, SECURITY_OBJECT_TYPE, USER_OBJECT_TYPE, CLASS_OBJECT_TYPE, IDENTITY_OBJECT_TYPE, SIGNATURE_OBJECT_TYPE } EnumActionRegisterObjectType_t;

typedef struct PrAddMetadata {
    long *MetadataOid;
    long MetadataOidSize;
    EnumMetaData_t KindOfMetadata;
} PrAddMetadata_t;

typedef enum bmdSearchType {
	CONSIDER_GROUPS_GRAPH=1,
	CONSIDER_CURRENT_GROUP=2,
	CONSIDER_ONLY_OWNER=4,
	CONSIDER_ONLY_CREATOR=8,
	CONSIDER_CREATOR_OR_OWNER=16,
	CONSIDER_CREATOR_OR_OWNER_OR_GROUPS=32,
	CONSIDER_CREATOR_OR_OWNER_OR_CURRENT_GROUP=64,
	SKIP_AUTHORIZATION=128,
	CONSIDER_ONLY_DIRECTORY=256
} bmdSearchType_e;


typedef enum bmdFileVersion {
	CURRENT_FILE_VERSION=1,
	ARCHIVE_FILE_VERSION=2
} bmdFileVersion_e;


LIBBMDSQL_SCOPE long VerifyPermissionsForCryptoObject(	void *hDB,
				server_request_data_t *req,
				char* verifyingFileId,
				bmdSearchType_e verificationType,
				char** fileName,
				char** fileType,
				char** pointingId,
				char** correspondingId,
				char** forGrant,
				long *additionalMetadataCount,
				char ***additionalMetadataValues);

LIBBMDSQL_SCOPE long VerifyPermissionsForCryptoObjectByMetadata(	void *hDB,
				server_request_data_t *req,
				bmdDatagram_t *requestWithMetadata,
				bmdSearchType_e verificationType,
				char** fileName,
				char** fileType,
				char** pointingId,
				char** correspondingId,
				char** forGrant,
				long *additionalMetadataCount,
				char ***additionalMetadataValues);
				
LIBBMDSQL_SCOPE long VerifyPermissionsForArchiveCryptoObject(	void *hDB,
				server_request_data_t *req,
				char* currentVersionFileId,
				char* archiveVersionFileId,
				bmdSearchType_e verificationType,
				char** fileName,
				char** fileType,
				char** pointingId,
				char** correspondingId,
				char** forGrant,
				long *additionalMetadataCount,
				char ***additionalMetadataValues);

LIBBMDSQL_SCOPE long bmdQuoteSqlConditionValue(char* conditionValue, char** quotedConditionValue);

typedef enum SqlTimestampValidationType {
	GENERAL_VALIDATION=0,
	LATER_THAN_NOW
} SqlTimestampValidationType_e;

LIBBMDSQL_SCOPE long bmdParseSqlTimestamp(char* sqlTimestamp, char** year, char** month, char** day,
					char** hour, char** minute, char** second);

LIBBMDSQL_SCOPE long bmdValidateSqlTimestamp(char* year, char* month, char* day,
					char* hour, char* minute, char* second,
					SqlTimestampValidationType_e validationType);

LIBBMDSQL_SCOPE long FillFromCryptoObjects(	void *hDB,
						char *CryptoObjectNr,
						char *location_id,
						EnumWithSysMetadata_t WithSysMetadata,
						bmdDatagram_t *bmdJSResponse,
						server_request_data_t *req);

LIBBMDSQL_SCOPE long FillFromActionMetaData(	void *hDB,
						char *CryptoObjectNr,
						char *location_id,
						bmdDatagram_t *bmdJSResponse);

LIBBMDSQL_SCOPE long FillFromAdditionalMetaData(	void *hDB,
							char *CryptoObjectNr,
							char *location_id,
							bmdDatagram_t *bmdJSResponse,
							server_request_data_t *req);

LIBBMDSQL_SCOPE long FillFromAdditionalMetaDataHistory(	void *hDB,
							char *MetadataId,
							char *location_id,
							bmdDatagram_t *bmdJSPrevResponse,
							bmdDatagram_t *bmdJSResponse,
							server_request_data_t *req);

LIBBMDSQL_SCOPE long FillFromProtocolData(	void *hDB,
						char *CryptoObjectNr,
						char *location_id,
						LobsInDatagram_t LobsInDatagram,
						bmdFileVersion_e fileVersion,
						bmdDatagram_t *bmdJSResponse,
						JS_return_t *js_ret);

LIBBMDSQL_SCOPE long CopySendAdviceParameters(bmdDatagram_t *srcDatagram, bmdDatagram_t* destDatagram);

LIBBMDSQL_SCOPE long InsertStringToGenBufAlloc(	char *string,
						GenBuf_t **GenBuf);

/* Funkcja zwalnia tablice ktora uwywana jest w budowaniu warunku WHERE w zapytaniu SELECT */
LIBBMDSQL_SCOPE long AllocCharArray(	long NoOfSlots,
					char ***array);

LIBBMDSQL_SCOPE long FreeCharArray(	long NoOfSlots,
					char **array);

LIBBMDSQL_SCOPE long cb_PrDatagramAddMetadata(	char *ResultString,
						long ResultStringLength,
			     			void *input,
						void *output);

LIBBMDSQL_SCOPE long GetIdFromHash(	void *hDB,
					char *hash_value,
					char **FileId);

LIBBMDSQL_SCOPE long GetHashFromId(	void *hDB,
					char *FileId,
					char *location_id,
					char **hash_value);

LIBBMDSQL_SCOPE long GetEmailFromId(	void *hDB,
					char *FileId,
					char **email);

/*OBSOLETE
LIBBMDSQL_SCOPE long CheckIfCryptoObjectExist(	void *hDB,
						bmdDatagram_t *bmdJSRequest,
						long ignoreDependentGroups,
						char **id,
						char **file_type,
						char **location_id,
						char **file_name,
						char **corresponding_id,
						long *addMetadataCount,
						char ***addMetadata,
						server_request_data_t *req);
*/

LIBBMDSQL_SCOPE long owner2id(	void *hDB,
				char *file_owner,
				char **owner_id);

LIBBMDSQL_SCOPE long id2identify(	void *hDB,
					char *id,
					char **identify);

/*** OBSOLETE
LIBBMDSQL_SCOPE long register_owner(	void *hDB,
					char *file_owner,
					char **owner_id,
					char *groupId);
OBSOLETE ***/

/* Wrzucanie dodatkowych metadanych */
LIBBMDSQL_SCOPE long CharReplace(char *Oid, char **EscapedOid, char old_char, char new_char);

LIBBMDSQL_SCOPE long CreateAdditionalMetadataTable(void *connection_handle, char *db_type);
LIBBMDSQL_SCOPE long create_am_table(void *db_conn_info);

LIBBMDSQL_SCOPE long ExecuteSQLQueryWithAnswerKnownDBConnection(	void *db_conn_info,
									char *SQLQuery,
									char **ans);

LIBBMDSQL_SCOPE long ExecuteSQLQueryWithAnswersKnownDBConnection(	void *db_conn_info,
									char *SQLQuery,
									long *ans_count,
									char ***ans);

LIBBMDSQL_SCOPE long ExecuteSQLQueryWithRowAnswersKnownDBConnection(	void *db_conn_info,
									char *SQLQuery,
									long *ans_count, char ***ans);

LIBBMDSQL_SCOPE long FillFromAllMetadata(	void *hDB,
						char *CryptoObjectNr,
						char *location_id,
						EnumWithPkiMetadata_t WithPkiMetadata,
						EnumWithAddMetadata_t WithAddMetadata,
						EnumWithAddMetadata_t WithSysMetadata,
						EnumWithActionMetadata_t WithActionMetadata,
						bmdDatagram_t *bmdJSResponse,
						server_request_data_t *req);

LIBBMDSQL_SCOPE long checkMandatoryMetadataExistenceInDb(	void *hDB);

LIBBMDSQL_SCOPE long notifyObjectActionPerformed(	bmdDatagramSet_t *dtgSet,
							char *destFilename);

LIBBMDSQL_SCOPE long VerifyAdditionalMetadataTypeAndValue(bmdDatagram_t *requestDatagram, server_request_data_t *req, char** addMetadataValues, long addMetadataValuesCount);
LIBBMDSQL_SCOPE long VerifySystemMetadataTypeAndValue(bmdDatagram_t *requestDatagram);

typedef enum ActivityResult
{
	ACTIVITY_FAILED = -1,
	ACTIVITY_SUCCEED = 0
	
} ActivityResult_e;

typedef enum TransactionStatus
{
	TRANSACTION_ALREADY_STARTED = 0,
	TRANSACTION_NOT_STARTED=1
	
} TransactionStatus_e;


LIBBMDSQL_SCOPE long noticeFirstTimeActivity(void *hDB,
											TransactionStatus_e transactionStatus,
											ActivityResult_e activityResult,
											const char* cryptoObjectId,
											const char* positiveNoticeString,
											const char* negativeNoticeString,
											char* noticeMetadataOid);

#endif
