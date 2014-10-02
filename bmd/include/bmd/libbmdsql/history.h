#ifndef _JS_HISTORY_INCLUDED_
#define _JS_HISTORY_INCLUDED_

#include <bmd/libbmdsql/libbmdsql.h>
#include <bmd/common/bmd-common.h>
#include <bmd/libbmddb/libbmddb.h>
#include <bmd/libbmdpr/bmd_datagram/asn1clib/BMDDatagram.h>
#include <bmd/libbmdpr/bmd_datagram/bmd_datagram.h>
#include <bmd/libbmdpr/bmd_datagram/bmd_datagram_set.h>
#include <bmd/libbmdsql/common.h>



LIBBMDSQL_SCOPE long JS_bmdDatagramSet_to_getMetaDataWithTime(	void *hDB,
								bmdDatagramSet_t *bmdJSRequestSet,
								bmdDatagramSet_t **bmdJSResponseSet,
					server_request_data_t *req);

LIBBMDSQL_SCOPE long JS_bmdDatagram_to_getMetaDataWithTime(	void *hDB,
								bmdDatagram_t *bmdJSRequest,
								bmdDatagramSet_t **bmdJSResponseSet,
					server_request_data_t *req);
					
LIBBMDSQL_SCOPE long JS_bmdDatagramSet_to_getVersionsHistory(void *hDB,
					bmdDatagramSet_t *bmdJSRequestSet,
					bmdDatagramSet_t **bmdJSResponseSet,
					server_request_data_t *req);
#endif
