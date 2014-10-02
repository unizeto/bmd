#ifndef _JS_INSERT_INCLUDED_
#define _JS_INSERT_INCLUDED_

#include <stdio.h>
#include <bmd/libbmdsql/libbmdsql.h>
#include <bmd/libbmdutils/libbmdutils.h>
#include <bmd/libbmddb/libbmddb.h>
#include <bmd/libbmdpr/bmd_datagram/asn1clib/BMDDatagram.h>
#include <bmd/libbmdpr/bmd_datagram/bmd_datagram.h>
#include <bmd/libbmdpr/bmd_datagram/bmd_datagram_set.h>
#include <bmd/libbmdsql/common.h>


/* przeniesc do bmd_common.h */
#define ERR_JS_NO_USER_METADATA_PRESENT -100
#define ERR_JS_NO_ADDITIONAL_METADATA_PRESENT -101
#define ERR_JS_NO_SYS_METADATA_PRESENT -101
#define ERR_JS_NO_PKI_METADATA_PRESENT -102
#define ERR_JS_NO_LOB_IN_PROTOCOL_DATA -103

/*typedef enum LobsInDatagram {LOB_NOT_IN_DATAGRAM, LOB_IN_DATAGRAM} LobsInDatagram_t;
*/


typedef enum bmdVisibility { VISIBLE = 1, INVISIBLE = 2, ALL = 4 } bmdVisibility_e;

typedef struct insert_data {
	bmd_crypt_ctx_t * serverPrivateKey;
} insert_data_t;

#ifndef WIN32
LIBBMDSQL_SCOPE long JS_bmdDatagramSet_to_insert(	void *hDB,
						bmdDatagramSet_t * bmdJSRequestSet,
						bmdDatagramSet_t ** bmdJSResponseSet,
						LobsInDatagram_t LobsInDatagram,
						bmdnet_handler_ptr handler,
						insert_data_t *data,
					server_request_data_t *req);
#else /*WIN32*/
LIBBMDSQL_SCOPE long JS_bmdDatagramSet_to_insert(	void *hDB,
						bmdDatagramSet_t * bmdJSRequestSet,
						bmdDatagramSet_t ** bmdJSResponseSet,
						LobsInDatagram_t LobsInDatagram,
						bmdnet_handler_ptr handler,
						insert_data_t *data,
						server_request_data_t *req,
						HANDLE *threadHandler);
#endif /*ifndef WIN32*/


LIBBMDSQL_SCOPE long JS_bmdDatagram_to_insert(	void *hDB,
						bmdDatagram_t * bmdJSRequest,
						bmdDatagram_t ** bmdJSResponse,
						LobsInDatagram_t LobsInDatagram,
						bmdnet_handler_ptr handler,insert_data_t *data,
					server_request_data_t *req);

LIBBMDSQL_SCOPE long InsertLOBandTimestampIntoDatabase(	void *hDB,
							bmdDatagram_t **bmdJSRequest,
							bmdDatagram_t *bmdJSResponse,
							LobsInDatagram_t LobsInDatagram,
							char **lobjId,
							char** correspondingId,
							bmdnet_handler_ptr handler,
							char **hash_as_hex_string,
							insert_data_t *data,
							u_int64_t *lob_size_for_db,
					server_request_data_t *req);

LIBBMDSQL_SCOPE long SetHashValueAndFileIdDatagram(	char *hash_as_hex_string,
						char **crypto_object_id,
						char *MaxId,
						LobsInDatagram_t LobsInDatagram,
						bmdDatagram_t *bmdJSResponse,
						insert_data_t *data,
					server_request_data_t *req);

LIBBMDSQL_SCOPE long JS_PrepareAndExecuteSQLQuery_Insert_CryptoObjects(	void *hDB,
										bmdDatagram_t *bmdJSRequest,
										char* NewCryptoObjectId,
										char* correspondingId,
										char *lobjId,
										u_int64_t lob_size,
										char* hashValueHexString,
					server_request_data_t *req);

LIBBMDSQL_SCOPE long JS_PrepareAndExecuteSQLQuery_Insert_AdditionalMetadata(void *hDB,
										bmdDatagram_t *bmdJSRequest,
										char *MaxId,
					server_request_data_t *req);

LIBBMDSQL_SCOPE long JS_Prepare_Insert_SysMetaData(	void *hDB,
							bmdDatagram_t *bmdJSRequest,
							char *MaxId,
							char* correspondingId,
							server_request_data_t *req,
							char** out_transaction_id,
							char** out_visible,
							char** out_present,
							char** out_pointing_id,
							char** out_pointing_location_id,
							char** out_file_type,
							char** out_corresponding_id,
							char** out_fk_group,
							char** out_fk_sec_category,
							char** out_fk_sec_level,
							char** out_for_grant,
							char** out_expiration_date,
							char** out_for_timestamping);

LIBBMDSQL_SCOPE long JS_PrepareAndExecuteSQLQuery_Insert_PkiMetaData(	void *hDB,
									bmdDatagram_t *bmdJSRequest,
									char *MaxId,
									insert_data_t *data,
									server_request_data_t *req,
									char** newHashHexString);

LIBBMDSQL_SCOPE long JS_bmdDatagramSet_to_Nop(	bmdDatagramSet_t * bmdJSRequestSet,
						bmdDatagramSet_t ** bmdJSResponseSet);


LIBBMDSQL_SCOPE long JS_bmdDatagram_to_Nop(	bmdDatagram_t * bmdJSRequest,
						bmdDatagram_t ** bmdJSResponse);


LIBBMDSQL_SCOPE long JS_bmdDatagramSet_to_CreateCascadeLinks(	void *hDB,
					bmdDatagramSet_t *bmdJSRequestSet,
					bmdDatagramSet_t **bmdJSResponseSet,
					server_request_data_t *req);

/*not for export*/
long GetOwnerFromRequestDatagram(void* hDB, bmdDatagram_t* requestDatagram, char* creatorIdentityId, char** ownerIdentityId, char** ownerGroupId);
long IgnoreDuplicateLinks(void* hDB, char* locationId, char* creatorId, char* ownerId, char* ownerGroupId, bmdStringArray_t** idsArray);
#endif
