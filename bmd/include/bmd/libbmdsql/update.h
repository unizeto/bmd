#ifndef _JS_UPDATE_INCLUDED_
#define _JS_UPDATE_INCLUDED_

#include <stdio.h>
#include <bmd/libbmdsql/libbmdsql.h>
#include <bmd/libbmdutils/libbmdutils.h>
#include <bmd/libbmddb/libbmddb.h>
#include <bmd/libbmdpr/bmd_datagram/asn1clib/BMDDatagram.h>
#include <bmd/libbmdpr/bmd_datagram/bmd_datagram.h>
#include <bmd/libbmdpr/bmd_datagram/bmd_datagram_set.h>
#include <bmd/libbmdsql/common.h>
#include <bmd/libbmdsql/insert.h>


/* przeniesc do bmd_common.h */
#define ERR_JS_NO_USER_METADATA_PRESENT -100
#define ERR_JS_NO_ADDITIONAL_METADATA_PRESENT -101
#define ERR_JS_NO_SYS_METADATA_PRESENT -101
#define ERR_JS_NO_PKI_METADATA_PRESENT -102
#define ERR_JS_NO_LOB_IN_PROTOCOL_DATA -103



LIBBMDSQL_SCOPE long LockCryptoObjectsRow(void *db_connection_handle, char *CryptoObjectNr);

LIBBMDSQL_SCOPE long JS_bmdDatagramSet_to_updateNewVersion(	void *hDB,
					bmdDatagramSet_t *bmdJSRequestSet,
					bmdDatagramSet_t **bmdJSResponseSet,
					bmdnet_handler_ptr handler,
					insert_data_t *data,
					server_request_data_t *req);


/************************************************/
/*		UPDATE FUNCTIONS		*/
/************************************************/
LIBBMDSQL_SCOPE long JS_bmdDatagramSet_to_updateMetadataValues(	void *hDB,
								bmdDatagramSet_t *bmdJSRequestSet,
								bmdDatagramSet_t **bmdJSResponseSet,
								server_request_data_t *req);

LIBBMDSQL_SCOPE long JS_bmdDatagram_to_updateMetadataValues(	void *hDB,
								bmdDatagram_t *bmdJSRequest,
								bmdDatagram_t **bmdJSResponse,
								server_request_data_t *req);

LIBBMDSQL_SCOPE long JS_bmdDatagramSet_to_update(	void *hDB,
						bmdDatagramSet_t *bmdJSRequestSet,
						bmdDatagramSet_t **bmdJSResponseSet,
					server_request_data_t *req);

LIBBMDSQL_SCOPE long JS_bmdDatagram_to_update(void *hDB,
					bmdDatagram_t *bmdJSRequest,
					bmdDatagram_t **bmdJSResponse,
					server_request_data_t *req);

LIBBMDSQL_SCOPE long JS_bmdDatagramSet_to_updateSysMetadata(void *hDB,
					bmdDatagramSet_t *bmdJSRequestSet,
					bmdDatagramSet_t **bmdJSResponseSet,
					server_request_data_t *req);


LIBBMDSQL_SCOPE long JS_PrepareAndExecuteSQLQuery_Hide_AdditionalMetadata(	void *hDB,
										char *CryptoObjectNr,
										char *location_id,
										server_request_data_t *req);

LIBBMDSQL_SCOPE long JS_PrepareAndExecuteSQLQuery_Insert_AdditionalMetadataHistory(	void *hDB,
											bmdDatagram_t *bmdJSRequest,
											char *CryptoObjectNr,
											char *location_id,
											long addMetadataCount,
											char **addMetadata);


#ifndef WIN32
LIBBMDSQL_SCOPE long JS_bmdDatagramSet_to_CloseTransaction(	void *hDB,
								bmdDatagramSet_t *bmdJSRequestSet,
								bmdDatagramSet_t **bmdJSResponseSet,
								server_request_data_t *req);
#else /*WIN32*/
LIBBMDSQL_SCOPE long JS_bmdDatagramSet_to_CloseTransaction(	void *hDB,
								bmdDatagramSet_t *bmdJSRequestSet,
								bmdDatagramSet_t **bmdJSResponseSet,
								server_request_data_t *req,
								HANDLE *threadHandler);
#endif /*ifndef WIN32*/

LIBBMDSQL_SCOPE long JS_bmdDatagram_to_CloseTransaction(	void *hDB,
							bmdDatagram_t *bmdJSRequest,
							char ***id_list,
							long *id_count,
					server_request_data_t *req);


// LIBBMDSQL_SCOPE long JS_PrepareAndExecuteSQLQuery_Update_Time(	void *hDB,
// 								char *twf_CryptoObjectNr,
// 								char *twf_insert_user_dn,
// 								char *twf_insert_reason,
//
// 								char *twf_metadataOID,
//
// 								char *twf_first_time_metadataOID,
// 								char *twf_first_time_user_dn,
//
// 								char *twf_no_updated_string,
// 								char *twf_after_update_string,
// 								long twf_enable_time_history,
//
// 								long twf_enable_first_time_history,
// 								long twf_enable_time,
//
// 								long twf_force_first_time,
// 								long twf_equal_first_check,
// 					server_request_data_t *req);

LIBBMDSQL_SCOPE long JS_bmdDatagramSet_to_addSignature(	void *hDB,
										bmdDatagramSet_t *bmdJSRequestSet,
										bmdDatagramSet_t **bmdJSResponseSet,
										insert_data_t *data,
					server_request_data_t *req);

LIBBMDSQL_SCOPE long JS_bmdDatagram_to_addSignature(	void *hDB,
									bmdDatagram_t *bmdJSRequest,
									bmdDatagram_t **bmdJSResponse,
									insert_data_t *data,
					server_request_data_t *req);

#endif
