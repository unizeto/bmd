#ifndef _DB_LIBBMDDB_INCLUDED_
#define _DB_LIBBMDDB_INCLUDED_


#define BMD_DBCONN_MAGIC	0x424D4400
#define BMD_DBRES_MAGIC		0x424D4401

#include <stdio.h>
#include <sqlcli1.h>
#include <bmd/common/bmd-common.h>
#include <bmd/common/global_types.h>
#include <bmd/libbmdutils/libbmdutils.h>
#include <bmd/libbmddb/bmd_db_functions.h>


typedef struct DB_bmd_db_conn {
	long magic;
	long db_type;
	SQLHENV  *henv;
	SQLHDBC  *hdbc;
	SQLHSTMT *hstmt;
	SQLRETURN     rc;
	long transaction_present;
	long transaction_isolation_level;
	long transaction_for_blob; /* czy transakacja rozpoczeta przez wysylanie/pobieranie BLOB'a */

	bmd_db_restart_callback lostConnectionCallback; /* funkcja definiujaca zachowanie w przypadku utraconego polaczenia */

	/* zmienne do bindowania */
	char **bind_vars;
	long bind_count;
} DB_bmd_db_conn_t;


#define LIBBMDDB_SCOPE



/* operowanie na wyniku zapytania */
LIBBMDDB_SCOPE long db2_bmd_db_row_struct_free(db_row_strings_t ** rowstruct);
LIBBMDDB_SCOPE long db2_bmd_db_result_get_value(void *hDB,void *hRes,long which_row,long which_column,char **value,
											EnumDbFetchMode_t fetch_mode,long *successfully_fetched);
LIBBMDDB_SCOPE long db2_bmd_db_result_get_row(void *hDB,void *hRes,long which_row,long max_columns,
										  db_row_strings_t **rowstruct,
										  EnumDbFetchMode_t fetch_mode,long *successfully_fetched);
LIBBMDDB_SCOPE long db2_bmd_db_result_free(void **hRes);

/* Funkcje do obslugi BLOB */
LIBBMDDB_SCOPE long db2_bmd_db_export_blob(void *hDB,GenBuf_t *dataGenBuf,char **blob_number);
LIBBMDDB_SCOPE long db2_bmd_db_import_blob(void *hDB,char *blob_number,GenBuf_t **dataGenBuf);
LIBBMDDB_SCOPE long db2_bmd_db_export_blob_begin(void *hDB,char **blob_number,void **blob_locator);
LIBBMDDB_SCOPE long db2_bmd_db_export_blob_continue(void *hDB,GenBuf_t *chunkGenBuf,void *blob_locator,
											    EnumDbWhichPiece_t which_piece);
LIBBMDDB_SCOPE long db2_bmd_db_export_blob_end(void *hDB,void **blob_locator);
LIBBMDDB_SCOPE long db2_bmd_db_import_blob_begin(void *hDB,char *blob_number,long *blob_size,void **blob_locator,
											 GenBuf_t **dataGenBuf);
LIBBMDDB_SCOPE long db2_bmd_db_import_blob_continue(void *hDB,GenBuf_t *chunkGenBuf,void *blob_locator,
												EnumDbWhichPiece_t which_piece);
LIBBMDDB_SCOPE long db2_bmd_db_import_blob_end(void *hDB,void **blob_locator,GenBuf_t **dataGenBuf);
LIBBMDDB_SCOPE long db2_bmd_db_delete_blob(void *hDB, char *blob_number);

/* Funkcje do laczenia i rozlaczenia z baza */
LIBBMDDB_SCOPE long db2_bmd_db_connect(const char *conn_info,void **hDB);
LIBBMDDB_SCOPE long db2_bmd_db_connect2(const char *adr,char *port,const char *db_name,const char *user,
					 const char *pass,void **hDB);
LIBBMDDB_SCOPE long db2_bmd_db_disconnect(void **hDB);
LIBBMDDB_SCOPE long db2_bmd_db_reconnect(void *hDB);
LIBBMDDB_SCOPE long db2_bmd_db_set_lost_connection_callback(void *hDB, bmd_db_restart_callback callbackFunction);

/* transakcje */
LIBBMDDB_SCOPE long db2_bmd_db_start_transaction(void *hDB,long transaction_isolation_level);
LIBBMDDB_SCOPE long db2_bmd_db_end_transaction(void *hDB);
LIBBMDDB_SCOPE long db2_bmd_db_rollback_transaction(void *hDB,void *params);

/* wykonywanie zapytania SQL */
LIBBMDDB_SCOPE long db2_bmd_db_execute_sql(void *hDB,const char *sql_query,long *rows,long *cols,void **hRes);
/* jeszcze nie zaimplementowane po nowemu */
LIBBMDDB_SCOPE long db2_bmd_db_execute_sql_bind(void *hDB,char *SQLQuery,
											long nParams,const char * const *params_values,
											long *ms_rows,long *ms_cols,void **hRes);
/* bindowanie zmiennych */
LIBBMDDB_SCOPE long db2_bmd_db_add_bind_var(const char *var,const char *var_type,void *hDB,char **bind_sql_str,
										char ***bv,long *bc);
LIBBMDDB_SCOPE long db2_bmd_db_add_bind_var_long(long var,const char *var_type,void *hDB,char **bind_sql_str,
											 char ***bv,long *bc);

/* escapowanie danych */
LIBBMDDB_SCOPE long db2_bmd_db_escape_string(void *hDB,const char *src, char **dest);

/* pobieranie typu bazy danych */
LIBBMDDB_SCOPE long db2_bmd_db_connect_get_db_type(void *hDB);

/* funkcje uzytkowe */
LIBBMDDB_SCOPE char *db2_bmd_db_get_actualtime_sql(void *hDB);
LIBBMDDB_SCOPE char *db2_bmd_db_get_sequence_nextval(void *hDB,char *SequenceName);
LIBBMDDB_SCOPE char *db2_bmd_db_get_sequence_currval(void *hDB,char *SequenceName);
LIBBMDDB_SCOPE long db2_bmd_db_get_max_entry(void *hDB,char *column_name,char *table_name,char **id);
LIBBMDDB_SCOPE long db2_bmd_db_date_to_timestamp(const char *Date, char **Timestamp);

/* generuje klauzule ORDER BY */
LIBBMDDB_SCOPE long db2_bmd_db_generate_order_clause(bmdDatagram_t *dtg, server_request_data_t* req, char **order_by);
/* generuje zapytanie takie by wyniki byly stronicowane */
LIBBMDDB_SCOPE long db2_bmd_db_paginate_sql_query(	char *sql_begin,
							char *order_by,
							bmdDatagram_t *dtg,
							long sql_offset,
							long sql_limit,
							char **sql_final,
							char **sql_limits);

long print_error(SQLRETURN ret, char * info);
void extract_error(char *fn, SQLHANDLE handle, SQLSMALLINT type);

#endif
