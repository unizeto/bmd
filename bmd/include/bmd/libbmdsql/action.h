#ifndef _LIBBMDSQL_ACTION_H_INCLUDED_
#define _LIBBMDSQL_ACTION_H_INCLUDED_


LIBBMDSQL_SCOPE long JS_bmdDatagramSet_GetActionsHistory(void *hDB, bmdDatagramSet_t *bmdJSRequestSet, bmdDatagramSet_t **bmdJSResponseSet, server_request_data_t *req);

LIBBMDSQL_SCOPE long JS_bmdDatagram_GetActionsHistory(void *hDB, bmdDatagram_t *bmdJSRequest, bmdDatagramSet_t **bmdJSResponseSet, server_request_data_t *req);


#endif /*ifndef _LIBBMDSQL_ACTION_H_INCLUDED_*/
