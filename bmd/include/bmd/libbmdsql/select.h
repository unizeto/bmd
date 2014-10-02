#ifndef _JS_SELECT_INCLUDED_
#define _JS_SELECT_INCLUDED_

#include <bmd/libbmdsql/libbmdsql.h>
#include <bmd/common/bmd-common.h>
#include <bmd/libbmddb/libbmddb.h>
#include <bmd/libbmdpr/bmd_datagram/asn1clib/BMDDatagram.h>
#include <bmd/libbmdsql/common.h>
#include <bmd/libbmdpr/bmd_datagram/bmd_datagram.h>
#include <bmd/libbmdpr/bmd_datagram/bmd_datagram_set.h>


/* określa czy metadana jest pierwszą z analizowanych czy kolejną */
typedef enum { false, true } bool_t;

/** select_main.c */
LIBBMDSQL_SCOPE long JS_bmdDatagramSet_to_select(	void *hDB,
							bmdDatagramSet_t * bmdJSRequestSet,
							bmdDatagramSet_t ** bmdJSResponseSet,
							server_request_data_t *req);

LIBBMDSQL_SCOPE long GenerateSearchQuery(	void *hDB,
						bmdDatagram_t *bmdDatagram,
						EnumSearchResultMode_t SearchResultMode,
						long ignoreDependentGroups,
						char **SelectQuery,
						char **SelectLimits,
						char **SelectQueryColumns,
						server_request_data_t *req);

LIBBMDSQL_SCOPE long GenerateSelectQuery(	void *hDB,
					bmdDatagram_t *bmdDatagram,
					EnumSearchMode_t SearchMode,
					EnumSearchResultMode_t SearchResultMode,
					char **SelectQuery,
					server_request_data_t *req);

LIBBMDSQL_SCOPE long FillbmdDatagramSetFromSelectResult(	void *hDB,
								void *SelectResult,
								bmdDatagramSet_t *bmdJSResponseSet,
								server_request_data_t *req);



LIBBMDSQL_SCOPE long ConcatenateWithWhereUserId(	void *hDB,
						char *SQLAfterAddMtd,
						bmdDatagram_t *bmdDatagram,
						long ignoreDependentGroups,
						char **SQLPrefix,
						server_request_data_t *req);

/** select_sys.c */
// LIBBMDSQL_SCOPE long GenerateSystemWhereCondition(	void *hDB,
// 						MetaDataBuf_t **MetaDataBuf,
// 						long no_of_MetaData,
// 						char **SQLConditions,
// 						long SQL_operator_flag,
// 						EnumSearchMode_t SearchMode,
// 					server_request_data_t *req);

LIBBMDSQL_SCOPE long ConcatenateWithSysMetadata(	char *SQLPrefix,
						bmdDatagram_t * bmdDatagram,
						char **SQLAfterSysMtd,
					server_request_data_t *req);

// LIBBMDSQL_SCOPE long ConcatenateWithWhereSysMetadata(	void *hDB,
// 							char *SQLPrefix,
// 							bmdDatagram_t * bmdDatagram,
// 							EnumSearchMode_t SearchMode,
// 							char **SQLAfterSysMtd,
// 					server_request_data_t *req);


LIBBMDSQL_SCOPE long GetChildren(	char *GroupId,
						char ***ChildrenList,
						long *no_of_children);

LIBBMDSQL_SCOPE long GenerateDependentGroupsIdList(	char *GroupId,
									char ***DependentGroupsList,
									long *no_of_DependentGroups);

/** select_add.c */
LIBBMDSQL_SCOPE long GenerateAdditionalWhereCondition(	void *hDB,
							MetaDataBuf_t * MetaDataBuf,
							long which_additional_metadata,
							char **SQLConditions,
							long SQL_operator_flag,
							EnumSearchMode_t SearchMode,
					server_request_data_t *req);

LIBBMDSQL_SCOPE long ConcatenateWithAddMetadata(	char *SQLPrefix,
						bmdDatagram_t * bmdDatagram,
						char **SQLAfterAddMtd,
					server_request_data_t *req);

LIBBMDSQL_SCOPE long ConcatenateWithMetadataConditions(	void *hDB,
							char *SQLBegin,
							bmdDatagram_t *bmdDatagram,
							long *add_met_count,
							long *sys_met_count,
							long *act_met_count,
							long *pki_met_count,
							char **SQLFinish);

LIBBMDSQL_SCOPE long ConcatenateWithWhereAddMetadata(	void *hDB,
							char *SQLPrefix,
							bmdDatagram_t * bmdDatagram,
							EnumSearchMode_t SearchMode,
							char **SQLAfterAddMtd,
					server_request_data_t *req);

/** select_utils.c */
LIBBMDSQL_SCOPE long OID_to_column_name(char *OIDTableBuf,
				char **column_name);

LIBBMDSQL_SCOPE char *ulong_table_to_string(	long *OIDTableBuf,
					long OIDTableBufSize);

LIBBMDSQL_SCOPE long ulong_table_to_string2(	long *OIDTableBuf,
					long OIDTableBufSize,
					char **string);

LIBBMDSQL_SCOPE long ConcatenateStrings(	char **src,
					char *add);

LIBBMDSQL_SCOPE long OIDsufixToOperatorType(	char *OIDStringSufix,
					char **relation_operator);

LIBBMDSQL_SCOPE long SplitOidStringToPrefixAndSufix(char *OIDstring,
						char **OIDstringPrefix,
						char **OIDStringSufix);

/** select_unreceived.c */
LIBBMDSQL_SCOPE long JS_bmdDatagramSet_to_select_unreceived(	void *hDB,
					bmdDatagramSet_t *bmdJSRequestSet,
					bmdDatagramSet_t **bmdJSResponseSet,
					server_request_data_t *req);

LIBBMDSQL_SCOPE long JS_bmdDatagramSet_to_GetAccessObjects(	void *hDB,
					bmdDatagramSet_t *bmdJSRequestSet,
					bmdDatagramSet_t **bmdJSResponseSet,
					server_request_data_t *req);

LIBBMDSQL_SCOPE long DetermineSearchConditionArguments(	MetaDataBuf_t *metaData,
					long defaultrecRelation,
					char **recColumnName,
					char **recLogical,
					char **recRelation,
					char **recOpenBracets,
					char **recCloseBrackets,
					char **recStartPerc,
					char **recStopPerc,
					char **metadataCast,
					long *add_met_count,
					long *sys_met_count,
					long *act_met_count,
					long *pki_met_count);

LIBBMDSQL_SCOPE long BuildSQLConditionPartForGroupsGraph(long ignoreDependentGroups, server_request_data_t *req, char** conditionPart);

#endif
