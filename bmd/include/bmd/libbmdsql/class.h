#ifndef _JS_CLASS_INCLUDED_
#define _JS_CLASS_INCLUDED_

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

LIBBMDSQL_SCOPE long GetUserDefaultClass(	char *userId,
						char **userDefaultClassId);

LIBBMDSQL_SCOPE long CheckIfClassExist(	char *classId,
					char **className);

LIBBMDSQL_SCOPE long CheckIfClassHasChild(	char *classId);

LIBBMDSQL_SCOPE long CheckIfUserHasClass(	char *classId,
						char *userId);

LIBBMDSQL_SCOPE	long DetermineUserClassForRequest(const bmdDatagram_t *requestDatagram, const server_request_data_t *req, char** userClassId);

LIBBMDSQL_SCOPE long JS_bmdDatagramSet_to_deleteClass(	void *hDB,
							bmdDatagramSet_t *bmdJSRequestSet,
							bmdDatagramSet_t **bmdJSResponseSet);

LIBBMDSQL_SCOPE long JS_bmdDatagram_to_deleteClass(	void *hDB,
							bmdDatagram_t *bmdJSRequest,
							bmdDatagram_t **bmdJSResponse);

LIBBMDSQL_SCOPE long JS_bmdDatagramSet_to_getClassList(	void *hDB,
							bmdDatagramSet_t *bmdJSRequestSet,
							bmdDatagramSet_t **bmdJSResponseSet,
							server_request_data_t *req);

LIBBMDSQL_SCOPE long JS_bmdDatagram_to_getClassList(	void *hDB,
							long number,
							char *classId,
							bmdDatagram_t **bmdJSResponse);

LIBBMDSQL_SCOPE long JS_bmdDatagramSet_to_registerClass(	void *hDB,
								bmdDatagramSet_t *bmdJSRequestSet,
								bmdDatagramSet_t **bmdJSResponseSet);

LIBBMDSQL_SCOPE long JS_bmdDatagram_to_registerClass(	void *hDB,
							bmdDatagram_t *bmdJSRequest,
							bmdDatagram_t **bmdJSResponse);

LIBBMDSQL_SCOPE long JS_bmdDatagram_to_updateClass(	void *hDB,
							bmdDatagram_t *bmdJSRequest,
							bmdDatagram_t **bmdJSResponse);

#endif
