#ifndef _BMD_DB_FUNCTIONS_INCLUDED_
#define _BMD_DB_FUNCTIONS_INCLUDED_

#include <stdio.h>
#include <bmd/common/bmd-common.h>
#include <bmd/common/global_types.h>
#include <bmd/libbmdutils/libbmdutils.h>

/* obslugiwane bazy */
#define BMD_DB_POSTGRES	0x01
#define BMD_DB_ORACLE	0x02
#define BMD_DB_DB2		0x03
#define BMD_DB_UNKNOWN	0xFF

/* typy transakcji */
#define BMD_TRANSACTION_LEVEL_NONE			0x00
#define BMD_TRANSACTION_LEVEL_SERIALIZABLE	0x01


/**jezeli zdefiniujemy BLOB_IN_BYTEA to u�ywamy zapisu BLOBow w tabelach z polami BYTEA */
#define BLOB_IN_BYTEA

/* struktura do przechowywania zmiennych z calego wiersza rezultatu */
typedef struct db_row_strings {
    long size;			/* ile kolumn zawiera wiersz */
    char **colvals;		/* wartosci zmiennych w koljnych kolumnach */
} db_row_strings_t;

#if defined(__cplusplus)
extern "C" {
#endif

/* wskazniki na polaczenie i rozlaczanie z baza danych */
typedef long (*bmd_db_connect_ptr)(const char *,void **);
typedef long (*bmd_db_connect2_ptr)(const char *,char *,const char *,const char *,const char *,void **);
typedef long (*bmd_db_disconnect_ptr)(void **);
typedef long (*bmd_db_reconnect_ptr)(void*);

/* funkcja do przypisania funkcji definiujacej zachowanie przy utraconym polaczeniu */
typedef long (*bmd_db_restart_callback)();
typedef long (*bmd_db_set_lost_connection_callback_ptr)(void*, bmd_db_restart_callback);


/* wskazniki na funkcje operujace na transakcjach */
typedef long (*bmd_db_start_transaction_ptr)(void *,long);
typedef long (*bmd_db_end_transaction_ptr)(void *);
typedef long (*bmd_db_rollback_transaction_ptr)(void *,void *);

/* wskazniki na funkcje operujace na BLOB'ach */
typedef long (*bmd_db_export_blob_ptr)(void *,GenBuf_t *,char **);
typedef long (*bmd_db_import_blob_ptr)(void *,char *,GenBuf_t **);
typedef long (*bmd_db_export_blob_begin_ptr)(void *,char **,void **);
typedef long (*bmd_db_export_blob_continue_ptr)(void *,GenBuf_t *,void *,EnumDbWhichPiece_t);
typedef long (*bmd_db_export_blob_end_ptr)(void *,void **);
typedef long (*bmd_db_import_blob_begin_ptr)(void *,char *,long *,void **,GenBuf_t **);
typedef long (*bmd_db_import_blob_continue_ptr)(void *,GenBuf_t *,void *,EnumDbWhichPiece_t);
typedef long (*bmd_db_import_blob_end_ptr)(void *,void **,GenBuf_t **);
typedef long (*bmd_db_delete_blob_ptr)(void *, char *);

/* operowanie na wynikach zapytania SQL */
typedef long (*bmd_db_row_struct_free_ptr)(db_row_strings_t **);
typedef long (*bmd_db_result_get_value_ptr)(void *,void *,long,long,char **,EnumDbFetchMode_t,long *);
typedef long (*bmd_db_result_get_row_ptr)(void *,void *,long ,long,db_row_strings_t **,EnumDbFetchMode_t,long *);
typedef long (*bmd_db_result_free_ptr)(void **);

/* wykonywanie zapytan SQL */
typedef long (*bmd_db_execute_sql_ptr)(void *,const char *,long *,long *,void **);
typedef long (*bmd_db_execute_sql_bind_ptr)(void *,char *,long,const char * const *,long *,long *,void **);
typedef long (*bmd_db_add_bind_var_ptr)(const char *,const char *,void *,char **,char ***,long *);
typedef long (*bmd_db_add_bind_var_long_ptr)(long,const char *,void *,char **,char ***,long *);
typedef long (*bmd_db_link_bind_var_list_ptr)(char **,long,void *);
typedef long (*bmd_db_free_bind_var_list_ptr)(void *);
typedef long (*bmd_db_free_user_bind_var_list_ptr)(void *);

/* funkcje uzytkowe dla zapytan SQL */
typedef long (*bmd_db_generate_order_clause_ptr)(bmdDatagram_t *, server_request_data_t*, char **);
typedef long (*bmd_db_paginate_sql_query_ptr)(char *,char *,bmdDatagram_t *,long,long,char **,char**);

/* pozostale funkcje uzytkowe */
typedef long (*bmd_db_escape_string_ptr)(void *,const char *, char **);
typedef long (*bmd_db_connect_get_db_type_ptr)(void *);
typedef char* (*bmd_db_get_actualtime_sql_ptr)(void *);
typedef char* (*bmd_db_get_sequence_nextval_ptr)(void *,char *);
typedef char* (*bmd_db_get_sequence_currval_ptr)(void *,char *);
typedef long (*bmd_db_get_max_entry_ptr)(void *,char *,char *,char **);
typedef long (*bmd_db_date_to_timestamp_ptr)(const char *, char **);

typedef struct bmd_db_fun_table {
	#ifdef WIN32
	HMODULE lH;
	#else
	void *lH;
	#endif
	bmd_db_connect_ptr bmd_db_connect;
	bmd_db_connect2_ptr bmd_db_connect2;
	bmd_db_disconnect_ptr bmd_db_disconnect;
	bmd_db_reconnect_ptr bmd_db_reconnect;
	bmd_db_set_lost_connection_callback_ptr bmd_db_set_lost_connection_callback;
	bmd_db_start_transaction_ptr bmd_db_start_transaction;
	bmd_db_end_transaction_ptr bmd_db_end_transaction;
	bmd_db_rollback_transaction_ptr bmd_db_rollback_transaction;
	bmd_db_export_blob_ptr bmd_db_export_blob;
	bmd_db_import_blob_ptr bmd_db_import_blob;
	bmd_db_export_blob_begin_ptr bmd_db_export_blob_begin;
	bmd_db_export_blob_continue_ptr bmd_db_export_blob_continue;
	bmd_db_export_blob_end_ptr bmd_db_export_blob_end;
	bmd_db_import_blob_begin_ptr bmd_db_import_blob_begin;
	bmd_db_import_blob_continue_ptr bmd_db_import_blob_continue;
	bmd_db_import_blob_end_ptr bmd_db_import_blob_end;
	bmd_db_delete_blob_ptr bmd_db_delete_blob;
	bmd_db_row_struct_free_ptr bmd_db_row_struct_free;
	bmd_db_result_get_value_ptr bmd_db_result_get_value;
	bmd_db_result_get_row_ptr bmd_db_result_get_row;
	bmd_db_result_free_ptr bmd_db_result_free;
	bmd_db_execute_sql_ptr bmd_db_execute_sql;
	bmd_db_execute_sql_bind_ptr bmd_db_execute_sql_bind;
	bmd_db_add_bind_var_ptr bmd_db_add_bind_var;
	bmd_db_add_bind_var_long_ptr bmd_db_add_bind_var_long;
	bmd_db_link_bind_var_list_ptr bmd_db_link_bind_var_list;
	bmd_db_free_bind_var_list_ptr bmd_db_free_bind_var_list;
	bmd_db_free_user_bind_var_list_ptr bmd_db_free_user_bind_var_list;
	bmd_db_generate_order_clause_ptr bmd_db_generate_order_clause;
	bmd_db_paginate_sql_query_ptr bmd_db_paginate_sql_query;
	bmd_db_escape_string_ptr bmd_db_escape_string;
	bmd_db_connect_get_db_type_ptr bmd_db_connect_get_db_type;
	bmd_db_get_actualtime_sql_ptr bmd_db_get_actualtime_sql;
	bmd_db_get_sequence_nextval_ptr bmd_db_get_sequence_nextval;
	bmd_db_get_sequence_currval_ptr bmd_db_get_sequence_currval;
	bmd_db_get_max_entry_ptr bmd_db_get_max_entry;
	bmd_db_date_to_timestamp_ptr bmd_db_date_to_timestamp;
} bmd_db_fun_table_t;

#if defined(__cplusplus)
}
#endif
#endif
