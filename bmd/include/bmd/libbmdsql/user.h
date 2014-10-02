#ifndef _JS_USER_INCLUDED_
#define _JS_USER_INCLUDED_

#include <stdio.h>
#include <bmd/libbmdsql/libbmdsql.h>
#include <bmd/libbmdutils/libbmdutils.h>
#include <bmd/libbmddb/libbmddb.h>
#include <bmd/libbmdpr/bmd_datagram/asn1clib/BMDDatagram.h>
#include <bmd/libbmdpr/bmd_datagram/bmd_datagram.h>
#include <bmd/libbmdpr/bmd_datagram/bmd_datagram_set.h>
#include <bmd/libbmdsql/insert.h>
#include <bmd/libbmdsql/update.h>
#include <bmd/libbmdsql/delete.h>
#include <bmd/libbmdsql/common.h>
#include <bmd/common/global_types.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>
#include <bmd/libbmdlog/bmd_logs.h>
#include <bmd/libbmderr/libbmderr.h>

LIBBMDSQL_SCOPE long CheckIfUserExist(	char *userId);

LIBBMDSQL_SCOPE long CheckIfUserExist2(	char *certIssuerSerial,
					char **userId);

LIBBMDSQL_SCOPE long CheckIfCertInfoFree(	char *certIssuer,
						char *certSerial);

LIBBMDSQL_SCOPE long CheckIfIdentityExists(void *hDB, char* identityId);

LIBBMDSQL_SCOPE long IsPossibleIdentitityRegistration(void* hDB);
						
LIBBMDSQL_SCOPE long JS_bmdDatagramSet_to_disableUser(	void *hDB,
							bmdDatagramSet_t *bmdJSRequestSet,
							bmdDatagramSet_t **bmdJSResponseSet,
					server_request_data_t *req);

LIBBMDSQL_SCOPE long JS_bmdDatagram_to_disableUser(	void *hDB,
						bmdDatagram_t *bmdJSRequest,
						bmdDatagram_t **bmdJSResponse,
					server_request_data_t *req);

LIBBMDSQL_SCOPE long JS_bmdDatagramSet_to_disableIdentity(	void *hDB,
					bmdDatagramSet_t *bmdJSRequestSet,
					bmdDatagramSet_t **bmdJSResponseSet,
					server_request_data_t *req);

LIBBMDSQL_SCOPE long JS_bmdDatagramSet_to_deleteUser(	void *hDB,
							bmdDatagramSet_t *bmdJSRequestSet,
							bmdDatagramSet_t **bmdJSResponseSet,
					server_request_data_t *req);

LIBBMDSQL_SCOPE long JS_bmdDatagram_to_deleteUser(	void *hDB,
						bmdDatagram_t *bmdJSRequest,
						bmdDatagram_t **bmdJSResponse,
					server_request_data_t *req);

LIBBMDSQL_SCOPE long JS_bmdDatagramSet_to_registerUser(	void *hDB,
							bmdDatagramSet_t *bmdJSRequestSet,
							bmdDatagramSet_t **bmdJSResponseSet,
					server_request_data_t *req);

LIBBMDSQL_SCOPE long JS_bmdDatagram_to_registerUser(	void *hDB,
							bmdDatagram_t *bmdJSRequest,
							bmdDatagram_t **bmdJSResponse,
					server_request_data_t *req);

LIBBMDSQL_SCOPE long JS_bmdDatagram_to_updateUser(	void *hDB,
						bmdDatagram_t *bmdJSRequest,
						bmdDatagram_t **bmdJSResponse,
					server_request_data_t *req);

LIBBMDSQL_SCOPE long JS_bmdDatagramSet_to_getUserList(	void *hDB,
							bmdDatagramSet_t *bmdJSRequestSet,
							bmdDatagramSet_t **bmdJSResponseSet,
					server_request_data_t *req);

LIBBMDSQL_SCOPE long JS_bmdDatagram_to_getUserList(	void *hDB,
							long number,
							char *userId,
							bmdDatagram_t **bmdJSResponse,
							server_request_data_t *req);
#endif
