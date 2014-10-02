#ifndef _JS_ROLE_INCLUDED_
#define _JS_ROLE_INCLUDED_

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

LIBBMDSQL_SCOPE long CheckIfRoleExist(	char *roleName,
					char **roleId);

LIBBMDSQL_SCOPE long CheckIfRoleExistByName(	char *roleName);

LIBBMDSQL_SCOPE long CheckIfUserHasRole(	char *roleName,
						char *userId);

LIBBMDSQL_SCOPE long JS_bmdDatagramSet_to_changeChosenRole(	void *hDB,
								bmdDatagramSet_t *bmdJSRequestSet,
								bmdDatagramSet_t **bmdJSResponseSet,
								server_request_data_t *req);

LIBBMDSQL_SCOPE long JS_bmdDatagram_to_changeChosenRole(	void *hDB,
								bmdDatagram_t *bmdJSRequest,
								bmdDatagram_t **bmdJSResponse,
								server_request_data_t *req);

LIBBMDSQL_SCOPE long JS_bmdDatagramSet_to_getRoleList(	void *hDB,
							bmdDatagramSet_t *bmdJSRequestSet,
							bmdDatagramSet_t **bmdJSResponseSet,
							server_request_data_t *req);

LIBBMDSQL_SCOPE long JS_bmdDatagram_to_getRoleList(	void *hDB,
							long number,
							char *roleId,
							bmdDatagram_t **bmdJSResponse,
							server_request_data_t *req);

LIBBMDSQL_SCOPE long JS_bmdDatagramSet_to_registerRole(	void *hDB,
							bmdDatagramSet_t *bmdJSRequestSet,
							bmdDatagramSet_t **bmdJSResponseSet,
							server_request_data_t *req);

LIBBMDSQL_SCOPE long JS_bmdDatagram_to_registerRole(	void *hDB,
							bmdDatagram_t *bmdJSRequest,
							bmdDatagram_t **bmdJSResponse,
							server_request_data_t *req);

LIBBMDSQL_SCOPE long JS_bmdDatagram_to_updateRole(	void *hDB,
							bmdDatagram_t *bmdJSRequest,
							bmdDatagram_t **bmdJSResponse,
							server_request_data_t *req);

LIBBMDSQL_SCOPE long JS_bmdDatagramSet_to_deleteRole(	void *hDB,
							bmdDatagramSet_t *bmdJSRequestSet,
							bmdDatagramSet_t **bmdJSResponseSet,
							server_request_data_t *req);

LIBBMDSQL_SCOPE long JS_bmdDatagram_to_deleteRole(	void *hDB,
							bmdDatagram_t *bmdJSRequest,
							bmdDatagram_t **bmdJSResponse,
							server_request_data_t *req);

#endif
