#ifndef _JS_DELETE_INCLUDED_
#define _JS_DELETE_INCLUDED_

#include <bmd/libbmdsql/libbmdsql.h>
#include <bmd/common/bmd-common.h>
#include <bmd/libbmdpr/bmd_datagram/bmd_datagram.h>
#include <bmd/libbmdpr/bmd_datagram/bmd_datagram_set.h>
#include <bmd/libbmddb/libbmddb.h>

#define ERR_JS_DELETE -100

LIBBMDSQL_SCOPE long JS_bmdDatagramSet_to_delete(	void *hDB,
						bmdDatagramSet_t * bmdJSRequestSet,
						bmdDatagramSet_t ** bmdJSResponseSet,
					server_request_data_t *req);

LIBBMDSQL_SCOPE long JS_bmdDatagramSet_to_DeleteCascadeLinks(	void *hDB,
					bmdDatagramSet_t *bmdJSRequestSet,
					bmdDatagramSet_t **bmdJSResponseSet,
					server_request_data_t *req);

#ifndef WIN32
LIBBMDSQL_SCOPE long JS_bmdDatagramSet_to_send_avizo(void *hDB,
						bmdDatagramSet_t *bmdJSRequestSet,
						bmdDatagramSet_t **bmdJSResponseSet,
					server_request_data_t *req);
#else /*WIN32*/
LIBBMDSQL_SCOPE long JS_bmdDatagramSet_to_send_avizo(void *hDB,
						bmdDatagramSet_t *bmdJSRequestSet,
						bmdDatagramSet_t **bmdJSResponseSet,
						server_request_data_t *req,
						HANDLE *threadHandler);
#endif /*ifndef WIN32*/


LIBBMDSQL_SCOPE long DeleteDocFromDatabase(	void *hDB,
								bmdDatagram_t *bmdJSRequest,
								char *CryptoObjectNr,
								char *location_id,
								server_request_data_t *req);

LIBBMDSQL_SCOPE long DeleteDataSlot(	void *hDB,
				bmdDatagram_t *bmdJSRequest,
				bmdDatagram_t **bmdJSResponse,
					server_request_data_t *req);

#endif
