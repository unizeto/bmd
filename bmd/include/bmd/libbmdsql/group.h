#ifndef _JS_GROUP_INCLUDED_
#define _JS_GROUP_INCLUDED_

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
#include <bmd/libbmdsql/role.h>
#include <bmd/common/global_types.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>
#include <bmd/libbmdlog/bmd_logs.h>
#include <bmd/libbmderr/libbmderr.h>

LIBBMDSQL_SCOPE long CheckIfGroupExist(	char *groupId,
					char **groupName);

LIBBMDSQL_SCOPE long CheckIfGroupExistByName(	char *groupName);

LIBBMDSQL_SCOPE long CheckIfGroupHasChild(	char *groupId);

LIBBMDSQL_SCOPE long CheckIfUserHasGroup(	char *groupId,
						char *userId);

LIBBMDSQL_SCOPE long JS_bmdDatagramSet_to_deleteGroup(	void *hDB,
							bmdDatagramSet_t *bmdJSRequestSet,
							bmdDatagramSet_t **bmdJSResponseSet,
							server_request_data_t *req);

LIBBMDSQL_SCOPE long JS_bmdDatagram_to_deleteGroup(	void *hDB,
							bmdDatagram_t *bmdJSRequest,
							bmdDatagram_t **bmdJSResponse);

LIBBMDSQL_SCOPE long JS_bmdDatagramSet_to_getGroupList(	void *hDB,
							bmdDatagramSet_t *bmdJSRequestSet,
							bmdDatagramSet_t **bmdJSResponseSet,
							server_request_data_t *req);

LIBBMDSQL_SCOPE long JS_bmdDatagram_to_getGroupList(	void *hDB,
							long number,
							char *groupId,
							bmdDatagram_t **bmdJSResponse);

LIBBMDSQL_SCOPE long JS_bmdDatagramSet_to_registerGroup(	void *hDB,
								bmdDatagramSet_t *bmdJSRequestSet,
								bmdDatagramSet_t **bmdJSResponseSet,
								server_request_data_t *req);

LIBBMDSQL_SCOPE long JS_bmdDatagram_to_registerGroup(	void *hDB,
							bmdDatagram_t *bmdJSRequest,
							bmdDatagram_t **bmdJSResponse);

LIBBMDSQL_SCOPE long JS_bmdDatagram_to_updateGroup(	void *hDB,
							bmdDatagram_t *bmdJSRequest,
							bmdDatagram_t **bmdJSResponse);

#endif
