#ifndef _JS_TIMESTAMP_INCLUDED_
#define _JS_TIMESTAMP_INCLUDED_

#include <stdio.h>
#include <bmd/libbmdsql/libbmdsql.h>
#include <bmd/libbmdutils/libbmdutils.h>
#include <bmd/libbmddb/libbmddb.h>
#include <bmd/libbmdpr/bmd_datagram/asn1clib/BMDDatagram.h>
#include <bmd/libbmdpr/bmd_datagram/bmd_datagram.h>
#include <bmd/libbmdpr/bmd_datagram/bmd_datagram_set.h>
#include <bmd/common/global_types.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>
#include <bmd/libbmdlog/bmd_logs.h>
#include <bmd/libbmderr/libbmderr.h>

LIBBMDSQL_SCOPE long LockPrfTimestampsRow(	void *db_connection_handle,
						char *TimestampId);

LIBBMDSQL_SCOPE long JS_bmdDatagramSet_to_conservateTimestamps(	void *hDB,
								bmdDatagramSet_t *bmdJSRequestSet,
								bmdDatagramSet_t **bmdJSResponseSet,
								server_request_data_t *req);

LIBBMDSQL_SCOPE long TimestampAndReplacePades(const void *hDB, const char* cryptoObjectId, const char* padesRaster, const char* symkeyRaster, char **newHashHexString);

LIBBMDSQL_SCOPE long SynchronousPadesTimestamping(const void *hDB, const char* cryptoObjectId, char **newHashHexString);


LIBBMDSQL_SCOPE long JS_bmdDatagramSet_to_deleteReplacedLobs(bmdDatagramSet_t *requestSet,
											bmdDatagramSet_t **responseSet,
											server_request_data_t *req);

#endif
