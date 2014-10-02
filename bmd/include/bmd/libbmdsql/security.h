#ifndef _JS_SECURITY_INCLUDED_
#define _JS_SECURITY_INCLUDED_

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

LIBBMDSQL_SCOPE long CheckIfSecCategoryExist(	char *secCatName,
						char **secCatId);

LIBBMDSQL_SCOPE long CheckIfSecLevelExist(	char *secLevelName,
						char **secLevelId);

LIBBMDSQL_SCOPE long CheckIfSecLevelPriorityExist(	char *secLevelPriority,
							char **secLevelId);

LIBBMDSQL_SCOPE long JS_bmdDatagramSet_to_getSecurityList(	void *hDB,
								bmdDatagramSet_t *bmdJSRequestSet,
								bmdDatagramSet_t **bmdJSResponseSet,
								server_request_data_t *req);

LIBBMDSQL_SCOPE long JS_bmdDatagram_to_getSecurityList(	void *hDB,
							long number,
							long secCatsNumber,
							long secLevelsNumber,
							bmdDatagram_t **bmdJSResponse);


LIBBMDSQL_SCOPE long JS_bmdDatagramSet_to_registerSecurity(	void *hDB,
								bmdDatagramSet_t *bmdJSRequestSet,
								bmdDatagramSet_t **bmdJSResponseSet);

LIBBMDSQL_SCOPE long JS_bmdDatagram_to_registerSecurity(	void *hDB,
								bmdDatagram_t *bmdJSRequest,
								bmdDatagram_t **bmdJSResponse);


#endif
