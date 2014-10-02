#ifndef _JS_RAPORT_INCLUDED_
#define _JS_RAPORT_INCLUDED_

#include <bmd/libbmdsql/libbmdsql.h>
#include <bmd/common/bmd-common.h>
/*#include <bmd/libbmddb/postgres/pq_libbmddb.h>*/
#include <bmd/libbmdpr/bmd_datagram/asn1clib/BMDDatagram.h>
#include <bmd/libbmdsql/common.h>
#include <bmd/libbmdpr/bmd_datagram/bmd_datagram.h>
#include <bmd/libbmdpr/bmd_datagram/bmd_datagram_set.h>


LIBBMDSQL_SCOPE long JS_bmdDatagramSet_to_count(	void *hDB,
							bmdDatagramSet_t *bmdJSRequestSet,
							bmdDatagramSet_t **bmdJSResponseSet,
							server_request_data_t *req);

LIBBMDSQL_SCOPE long JS_bmdDatagram_to_count(	void *hDB,
						bmdDatagram_t *bmdJSRequest,
						bmdDatagram_t **bmdJSResponse,
						server_request_data_t *req);

#endif
