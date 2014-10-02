#ifndef _JS_GET_INCLUDED_
#define _JS_GET_INCLUDED_

#include <bmd/libbmdsql/libbmdsql.h>
#include <bmd/common/bmd-common.h>
#include <bmd/libbmddb/libbmddb.h>
#include <bmd/libbmdpr/bmd_datagram/asn1clib/BMDDatagram.h>
#include <bmd/libbmdpr/bmd_datagram/bmd_datagram.h>
#include <bmd/libbmdpr/bmd_datagram/bmd_datagram_set.h>
#include <bmd/libbmdsql/common.h>



#define ERR_GET_ID_FROM_HASH -111



LIBBMDSQL_SCOPE long JS_bmdDatagramSet_to_get(	void *hDB,
						bmdDatagramSet_t *bmdJSRequestSet,
						EnumWithProtocolData_t WithProtocolData,
						EnumWithPkiMetadata_t WithPkiMetadata,
						EnumWithAddMetadata_t WithAddMetadata,
						EnumWithSysMetadata_t WithSysMetadata,
						EnumWithActionMetadata_t WithActionMetadata,
						LobsInDatagram_t LobsInDatagram,
						GetInCGIMode_t CGIMode,
						bmdDatagramSet_t **bmdJSResponseSet,
						JS_return_t *js_ret,
						server_request_data_t *req);

LIBBMDSQL_SCOPE long JS_bmdDatagram_to_get(	void *hDB,
						bmdDatagram_t *bmdJSRequest,
						EnumWithProtocolData_t WithProtocolData,
						EnumWithPkiMetadata_t WithPkiMetadata,
						EnumWithAddMetadata_t WithAddMetadata,
						EnumWithSysMetadata_t WithSysMetadata,
						EnumWithActionMetadata_t WithActionMetadata,
						LobsInDatagram_t LobsInDatagram,
						GetInCGIMode_t CGIMode,
						bmdDatagram_t **bmdJSResponse,
						JS_return_t *js_ret,
						long i,
						server_request_data_t *req);
#endif
