#ifndef _JS_DATA_DICT_INCLUDED_
#define _JS_DATA_DICT_INCLUDED_


#include <bmd/libbmdsql/libbmdsql.h>
#include <bmd/libbmdutils/libbmdutils.h>
#include <bmd/libbmddb/libbmddb.h>
#include <bmd/libbmdpr/bmd_datagram/bmd_datagram.h>
#include <bmd/libbmdpr/bmd_datagram/bmd_datagram_set.h>

typedef struct dictionary_data
{
	long rows;
	long cols;
	char *SQLQuery;
	char ***data;
} dictionary_data_t;

LIBBMDSQL_SCOPE long dumpSingleDataDictionary(	void *hDB,	dictionary_data_t **dictionary);
LIBBMDSQL_SCOPE long dumpAllDataDictionaries(	char *db_conn_info);
LIBBMDSQL_SCOPE long updateAllDataDictionary();

#endif
