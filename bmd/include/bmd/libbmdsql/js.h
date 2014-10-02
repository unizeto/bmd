#ifndef _BMD_JS_H_INCLUDED_
#define _BMD_JS_H_INCLUDED_

#include <bmd/libbmdsql/libbmdsql.h>

/*#ifdef WITH_POSTGRES*/
#include <bmd/libbmddb/libbmddb.h>
/*#endif*/

/*#ifdef WITH_ORACLE
#include <bmd/libbmddb/oracle/libbmddb_oracle.h>
#endif
*/

#include <bmd/common/bmd-common.h>
#include <bmd/libbmdpr/bmd_datagram/asn1clib/BMDDatagram.h>

#include <bmd/libbmddb/libbmddb.h>
#include <bmd/libbmdsql/select.h>
#include <bmd/libbmdsql/get.h>
#include <bmd/libbmdsql/insert.h>
#include <bmd/libbmdsql/common.h>
#include <bmd/libbmdsql/history.h>

#ifndef WIN32
long JS_main(void *db_conn_info,bmdDatagramSet_t *bmdJSRequestSet, bmdDatagramSet_t **bmdJSResponseSet,
			 bmdnet_handler_ptr handler, JS_return_t *ret,insert_data_t *data,
			 server_request_data_t *req);
#else /*WIN32*/
long JS_main(void *db_conn_info,bmdDatagramSet_t *bmdJSRequestSet, bmdDatagramSet_t **bmdJSResponseSet,
			 bmdnet_handler_ptr handler, JS_return_t *ret,insert_data_t *data,
			 server_request_data_t *req, HANDLE *eventHandler);
#endif /*ifndef WIN32*/

long CheckIfRoleAllowsOperation(	char *userChosenRole, long action);

#endif /*_BMD_JS_H_INCLUDED_*/

