#include <bmd/libbmddb/libbmddb.h>
#include <bmd/libbmddb/bmd_db_functions.h>
#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdglobals/libbmdglobals.h>

#define CHECK_LIB		if( ( _GLOBAL_libbmddb_initialized == 0 ) || ( _GLOBAL_f_tbl == NULL ) ) \
							return BMD_ERR_NOTINITED
#define CHECK_LIB_PTR	if( ( _GLOBAL_libbmddb_initialized == 0 ) || ( _GLOBAL_f_tbl == NULL ) ) \
							return NULL


#ifdef WIN32

	#define db_lib_open( _libraryname, _loadmode ) LoadLibraryA( _libraryname )
	#define GET_SYM( _hmodule, _funsymname ) GetProcAddress( (HMODULE)_hmodule, _funsymname )
	#define db_lib_close( _hmodule ) FreeLibrary( (HMODULE)_hmodule )
#else
	#define db_lib_open(_libraryname,_loadmode) dlopen( _libraryname,_loadmode)
	#define GET_SYM( _hmodule,_funsymname) dlsym( _hmodule, _funsymname )
	#define db_lib_close(_hmodule) dlclose(_hmodule)
#endif

long bmd_db_init(const char *db_library)
{
	if( _GLOBAL_libbmddb_initialized == 1 )
		return BMD_OK;
	if( _GLOBAL_f_tbl != NULL )
		return BMD_OK;
	if( db_library == NULL )
		return BMD_ERR_PARAM1;

	_GLOBAL_f_tbl=(bmd_db_fun_table_t *)malloc(sizeof(bmd_db_fun_table_t));
	if( _GLOBAL_f_tbl == NULL )
		return BMD_ERR_MEMORY;
	memset(_GLOBAL_f_tbl,0,sizeof(bmd_db_fun_table_t));

	/* tutaj wczytanie odpowiedniej biblioteki i wypełnienie tablicy funkcji */
	_GLOBAL_f_tbl->lH=db_lib_open(db_library,RTLD_LAZY);
    //printf ("Openning\n");
	if( _GLOBAL_f_tbl->lH == NULL )
	{
		printf ("===>Plik [libbmddb.c] Linia [42] Funkcja [bmd_db_init]:\nFailed to open %s library, dlerror: %s\n",db_library,dlerror());		
		//PRINT_DEBUG("Failed to open %s library, dlerror: %s\n",db_library, dlerror());
		BMD_FOK(BMD_ERR_OP_FAILED);
	}
	if( strstr(db_library,"libbmddb_postgres") != NULL )
	{

		_GLOBAL_f_tbl->bmd_db_add_bind_var=(bmd_db_add_bind_var_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"postgres_bmd_db_add_bind_var");
		_GLOBAL_f_tbl->bmd_db_add_bind_var_long=(bmd_db_add_bind_var_long_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"postgres_bmd_db_add_bind_var_long");
		_GLOBAL_f_tbl->bmd_db_connect=(bmd_db_connect_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"postgres_bmd_db_connect");
		_GLOBAL_f_tbl->bmd_db_connect2=(bmd_db_connect2_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"postgres_bmd_db_connect2");
		_GLOBAL_f_tbl->bmd_db_connect_get_db_type=(bmd_db_connect_get_db_type_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"postgres_bmd_db_connect_get_db_type");
		_GLOBAL_f_tbl->bmd_db_reconnect=(bmd_db_reconnect_ptr)GET_SYM(_GLOBAL_f_tbl->lH, "postgres_bmd_db_reconnect");
		_GLOBAL_f_tbl->bmd_db_set_lost_connection_callback=(bmd_db_set_lost_connection_callback_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"postgres_bmd_db_set_lost_connection_callback");
		_GLOBAL_f_tbl->bmd_db_date_to_timestamp=(bmd_db_date_to_timestamp_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"postgres_bmd_db_date_to_timestamp");
		_GLOBAL_f_tbl->bmd_db_delete_blob=(bmd_db_delete_blob_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"postgres_bmd_db_delete_blob");
		_GLOBAL_f_tbl->bmd_db_disconnect=(bmd_db_disconnect_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"postgres_bmd_db_disconnect");
		_GLOBAL_f_tbl->bmd_db_end_transaction=(bmd_db_end_transaction_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"postgres_bmd_db_end_transaction");
		_GLOBAL_f_tbl->bmd_db_escape_string=(bmd_db_escape_string_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"postgres_bmd_db_escape_string");
		_GLOBAL_f_tbl->bmd_db_execute_sql=(bmd_db_execute_sql_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"postgres_bmd_db_execute_sql");
		_GLOBAL_f_tbl->bmd_db_execute_sql_bind=(bmd_db_execute_sql_bind_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"postgres_bmd_db_execute_sql");
		_GLOBAL_f_tbl->bmd_db_export_blob=(bmd_db_export_blob_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"postgres_bmd_db_export_blob");
		_GLOBAL_f_tbl->bmd_db_export_blob_begin=(bmd_db_export_blob_begin_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"postgres_bmd_db_export_blob_begin");
		_GLOBAL_f_tbl->bmd_db_export_blob_continue=(bmd_db_export_blob_continue_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"postgres_bmd_db_export_blob_continue");
		_GLOBAL_f_tbl->bmd_db_export_blob_end=(bmd_db_export_blob_end_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"postgres_bmd_db_export_blob_end");
		_GLOBAL_f_tbl->bmd_db_generate_order_clause=(bmd_db_generate_order_clause_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"postgres_bmd_db_generate_order_clause");
		_GLOBAL_f_tbl->bmd_db_get_actualtime_sql=(bmd_db_get_actualtime_sql_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"postgres_bmd_db_get_actualtime_sql");
		_GLOBAL_f_tbl->bmd_db_get_max_entry=(bmd_db_get_max_entry_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"postgres_bmd_db_get_max_entry");
		_GLOBAL_f_tbl->bmd_db_get_sequence_currval=(bmd_db_get_sequence_currval_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"postgres_bmd_db_get_sequence_currval");
		_GLOBAL_f_tbl->bmd_db_get_sequence_nextval=(bmd_db_get_sequence_nextval_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"postgres_bmd_db_get_sequence_nextval");
		_GLOBAL_f_tbl->bmd_db_import_blob=(bmd_db_import_blob_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"postgres_bmd_db_import_blob");
		_GLOBAL_f_tbl->bmd_db_import_blob_begin=(bmd_db_import_blob_begin_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"postgres_bmd_db_import_blob_begin");
		_GLOBAL_f_tbl->bmd_db_import_blob_continue=(bmd_db_import_blob_continue_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"postgres_bmd_db_import_blob_continue");
		_GLOBAL_f_tbl->bmd_db_import_blob_end=(bmd_db_import_blob_end_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"postgres_bmd_db_import_blob_end");
		_GLOBAL_f_tbl->bmd_db_paginate_sql_query=(bmd_db_paginate_sql_query_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"postgres_bmd_db_paginate_sql_query");
		_GLOBAL_f_tbl->bmd_db_result_free=(bmd_db_result_free_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"postgres_bmd_db_result_free");
		_GLOBAL_f_tbl->bmd_db_result_get_row=(bmd_db_result_get_row_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"postgres_bmd_db_result_get_row");
		_GLOBAL_f_tbl->bmd_db_result_get_value=(bmd_db_result_get_value_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"postgres_bmd_db_result_get_value");
		_GLOBAL_f_tbl->bmd_db_rollback_transaction=(bmd_db_rollback_transaction_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"postgres_bmd_db_rollback_transaction");
		_GLOBAL_f_tbl->bmd_db_row_struct_free=(bmd_db_row_struct_free_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"postgres_bmd_db_row_struct_free");
		_GLOBAL_f_tbl->bmd_db_start_transaction=(bmd_db_start_transaction_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"postgres_bmd_db_start_transaction");
	}
	else if( strstr(db_library,"libbmddb_db2") != NULL )
		{
			_GLOBAL_f_tbl->bmd_db_add_bind_var=(bmd_db_add_bind_var_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"db2_bmd_db_add_bind_var");
			_GLOBAL_f_tbl->bmd_db_add_bind_var_long=(bmd_db_add_bind_var_long_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"db2_bmd_db_add_bind_var_long");
			_GLOBAL_f_tbl->bmd_db_connect=(bmd_db_connect_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"db2_bmd_db_connect");
			_GLOBAL_f_tbl->bmd_db_connect2=(bmd_db_connect2_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"db2_bmd_db_connect2");
			_GLOBAL_f_tbl->bmd_db_connect_get_db_type=(bmd_db_connect_get_db_type_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"db2_bmd_db_connect_get_db_type");
			_GLOBAL_f_tbl->bmd_db_reconnect=(bmd_db_reconnect_ptr)GET_SYM(_GLOBAL_f_tbl->lH, "db2_bmd_db_reconnect");
			_GLOBAL_f_tbl->bmd_db_set_lost_connection_callback=(bmd_db_set_lost_connection_callback_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"db2_bmd_db_set_lost_connection_callback");
			_GLOBAL_f_tbl->bmd_db_date_to_timestamp=(bmd_db_date_to_timestamp_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"db2_bmd_db_date_to_timestamp");
			_GLOBAL_f_tbl->bmd_db_delete_blob=(bmd_db_delete_blob_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"db2_bmd_db_delete_blob");
			_GLOBAL_f_tbl->bmd_db_disconnect=(bmd_db_disconnect_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"db2_bmd_db_disconnect");
			_GLOBAL_f_tbl->bmd_db_end_transaction=(bmd_db_end_transaction_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"db2_bmd_db_end_transaction");
			_GLOBAL_f_tbl->bmd_db_escape_string=(bmd_db_escape_string_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"db2_bmd_db_escape_string");
			_GLOBAL_f_tbl->bmd_db_execute_sql=(bmd_db_execute_sql_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"db2_bmd_db_execute_sql");
			_GLOBAL_f_tbl->bmd_db_execute_sql_bind=(bmd_db_execute_sql_bind_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"db2_bmd_db_execute_sql");
			_GLOBAL_f_tbl->bmd_db_export_blob=(bmd_db_export_blob_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"db2_bmd_db_export_blob");
			_GLOBAL_f_tbl->bmd_db_export_blob_begin=(bmd_db_export_blob_begin_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"db2_bmd_db_export_blob_begin");
			_GLOBAL_f_tbl->bmd_db_export_blob_continue=(bmd_db_export_blob_continue_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"db2_bmd_db_export_blob_continue");
			_GLOBAL_f_tbl->bmd_db_export_blob_end=(bmd_db_export_blob_end_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"db2_bmd_db_export_blob_end");
			_GLOBAL_f_tbl->bmd_db_generate_order_clause=(bmd_db_generate_order_clause_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"db2_bmd_db_generate_order_clause");
			_GLOBAL_f_tbl->bmd_db_get_actualtime_sql=(bmd_db_get_actualtime_sql_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"db2_bmd_db_get_actualtime_sql");
			_GLOBAL_f_tbl->bmd_db_get_max_entry=(bmd_db_get_max_entry_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"db2_bmd_db_get_max_entry");
			_GLOBAL_f_tbl->bmd_db_get_sequence_currval=(bmd_db_get_sequence_currval_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"db2_bmd_db_get_sequence_currval");
			_GLOBAL_f_tbl->bmd_db_get_sequence_nextval=(bmd_db_get_sequence_nextval_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"db2_bmd_db_get_sequence_nextval");
			_GLOBAL_f_tbl->bmd_db_import_blob=(bmd_db_import_blob_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"db2_bmd_db_import_blob");
			_GLOBAL_f_tbl->bmd_db_import_blob_begin=(bmd_db_import_blob_begin_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"db2_bmd_db_import_blob_begin");
			_GLOBAL_f_tbl->bmd_db_import_blob_continue=(bmd_db_import_blob_continue_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"db2_bmd_db_import_blob_continue");
			_GLOBAL_f_tbl->bmd_db_import_blob_end=(bmd_db_import_blob_end_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"db2_bmd_db_import_blob_end");
			_GLOBAL_f_tbl->bmd_db_paginate_sql_query=(bmd_db_paginate_sql_query_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"db2_bmd_db_paginate_sql_query");
			_GLOBAL_f_tbl->bmd_db_result_free=(bmd_db_result_free_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"db2_bmd_db_result_free");
			_GLOBAL_f_tbl->bmd_db_result_get_row=(bmd_db_result_get_row_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"db2_bmd_db_result_get_row");
			_GLOBAL_f_tbl->bmd_db_result_get_value=(bmd_db_result_get_value_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"db2_bmd_db_result_get_value");
			_GLOBAL_f_tbl->bmd_db_rollback_transaction=(bmd_db_rollback_transaction_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"db2_bmd_db_rollback_transaction");
			_GLOBAL_f_tbl->bmd_db_row_struct_free=(bmd_db_row_struct_free_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"db2_bmd_db_row_struct_free");
			_GLOBAL_f_tbl->bmd_db_start_transaction=(bmd_db_start_transaction_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"db2_bmd_db_start_transaction");
		}
	else
	if( strstr(db_library,"libbmddb_oracle") != NULL )
	{

		_GLOBAL_f_tbl->bmd_db_add_bind_var=(bmd_db_add_bind_var_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"oracle_bmd_db_add_bind_var");
		if( _GLOBAL_f_tbl->bmd_db_add_bind_var == NULL )
		{
			PRINT_DEBUG("NULL bmd_db_add_bind_var pointer");
			BMD_FOK(BMD_ERR_FORMAT);
		}
		_GLOBAL_f_tbl->bmd_db_add_bind_var_long=(bmd_db_add_bind_var_long_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"oracle_bmd_db_add_bind_var_long");
		if( _GLOBAL_f_tbl->bmd_db_add_bind_var_long == NULL )
		{
			PRINT_DEBUG("NULL bmd_db_add_bind_var_long");
			BMD_FOK(BMD_ERR_FORMAT);
		}
		_GLOBAL_f_tbl->bmd_db_link_bind_var_list=(bmd_db_link_bind_var_list_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"oracle_bmd_db_link_bind_var_list");
		if( _GLOBAL_f_tbl->bmd_db_link_bind_var_list == NULL )
		{
			PRINT_DEBUG("NULL bmd_db_link_bind_var_list");
			BMD_FOK(BMD_ERR_FORMAT);
		}
		_GLOBAL_f_tbl->bmd_db_free_bind_var_list=(bmd_db_free_bind_var_list_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"oracle_bmd_db_free_bind_var_list");
		if( _GLOBAL_f_tbl->bmd_db_free_bind_var_list == NULL )
		{
			PRINT_DEBUG("NULL bmd_db_free_bind_var_list");
			BMD_FOK(BMD_ERR_FORMAT);
		}
		_GLOBAL_f_tbl->bmd_db_free_user_bind_var_list=(bmd_db_free_user_bind_var_list_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"oracle_bmd_db_free_user_bind_var_list");
		if( _GLOBAL_f_tbl->bmd_db_free_user_bind_var_list == NULL )
		{
			PRINT_DEBUG("NULL bmd_db_free_user_bind_var_list");
			BMD_FOK(BMD_ERR_FORMAT);
		}
		_GLOBAL_f_tbl->bmd_db_connect=(bmd_db_connect_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"oracle_bmd_db_connect");
		_GLOBAL_f_tbl->bmd_db_connect2=(bmd_db_connect2_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"oracle_bmd_db_connect2");
		_GLOBAL_f_tbl->bmd_db_connect_get_db_type=(bmd_db_connect_get_db_type_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"oracle_bmd_db_connect_get_db_type");
		_GLOBAL_f_tbl->bmd_db_reconnect = NULL; // do implementacji
		_GLOBAL_f_tbl->bmd_db_set_lost_connection_callback = NULL; // do implementacji
		_GLOBAL_f_tbl->bmd_db_date_to_timestamp=(bmd_db_date_to_timestamp_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"oracle_bmd_db_date_to_timestamp");
		_GLOBAL_f_tbl->bmd_db_delete_blob=(bmd_db_delete_blob_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"oracle_bmd_db_delete_blob");
		_GLOBAL_f_tbl->bmd_db_disconnect=(bmd_db_disconnect_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"oracle_bmd_db_disconnect");
		_GLOBAL_f_tbl->bmd_db_end_transaction=(bmd_db_end_transaction_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"oracle_bmd_db_end_transaction");
		_GLOBAL_f_tbl->bmd_db_escape_string=(bmd_db_escape_string_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"oracle_bmd_db_escape_string");
		_GLOBAL_f_tbl->bmd_db_execute_sql=(bmd_db_execute_sql_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"oracle_bmd_db_execute_sql");
		_GLOBAL_f_tbl->bmd_db_execute_sql_bind=(bmd_db_execute_sql_bind_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"oracle_bmd_db_execute_sql");
		_GLOBAL_f_tbl->bmd_db_export_blob=(bmd_db_export_blob_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"oracle_bmd_db_export_blob");
		_GLOBAL_f_tbl->bmd_db_export_blob_begin=(bmd_db_export_blob_begin_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"oracle_bmd_db_export_blob_begin");
		_GLOBAL_f_tbl->bmd_db_export_blob_continue=(bmd_db_export_blob_continue_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"oracle_bmd_db_export_blob_continue");
		_GLOBAL_f_tbl->bmd_db_export_blob_end=(bmd_db_export_blob_end_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"oracle_bmd_db_export_blob_end");
		_GLOBAL_f_tbl->bmd_db_generate_order_clause=(bmd_db_generate_order_clause_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"oracle_bmd_db_generate_order_clause");
		_GLOBAL_f_tbl->bmd_db_get_actualtime_sql=(bmd_db_get_actualtime_sql_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"oracle_bmd_db_get_actualtime_sql");
		_GLOBAL_f_tbl->bmd_db_get_max_entry=(bmd_db_get_max_entry_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"oracle_bmd_db_get_max_entry");
		_GLOBAL_f_tbl->bmd_db_get_sequence_currval=(bmd_db_get_sequence_currval_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"oracle_bmd_db_get_sequence_currval");
		_GLOBAL_f_tbl->bmd_db_get_sequence_nextval=(bmd_db_get_sequence_nextval_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"oracle_bmd_db_get_sequence_nextval");
		_GLOBAL_f_tbl->bmd_db_import_blob=(bmd_db_import_blob_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"oracle_bmd_db_import_blob");
		_GLOBAL_f_tbl->bmd_db_import_blob_begin=(bmd_db_import_blob_begin_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"oracle_bmd_db_import_blob_begin");
		_GLOBAL_f_tbl->bmd_db_import_blob_continue=(bmd_db_import_blob_continue_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"oracle_bmd_db_import_blob_continue");
		_GLOBAL_f_tbl->bmd_db_import_blob_end=(bmd_db_import_blob_end_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"oracle_bmd_db_import_blob_end");
		_GLOBAL_f_tbl->bmd_db_paginate_sql_query=(bmd_db_paginate_sql_query_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"oracle_bmd_db_paginate_sql_query");
		_GLOBAL_f_tbl->bmd_db_result_free=(bmd_db_result_free_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"oracle_bmd_db_result_free");
		_GLOBAL_f_tbl->bmd_db_result_get_row=(bmd_db_result_get_row_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"oracle_bmd_db_result_get_row");
		_GLOBAL_f_tbl->bmd_db_result_get_value=(bmd_db_result_get_value_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"oracle_bmd_db_result_get_value");
		_GLOBAL_f_tbl->bmd_db_rollback_transaction=(bmd_db_rollback_transaction_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"oracle_bmd_db_rollback_transaction");
		_GLOBAL_f_tbl->bmd_db_row_struct_free=(bmd_db_row_struct_free_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"oracle_bmd_db_row_struct_free");
		_GLOBAL_f_tbl->bmd_db_start_transaction=(bmd_db_start_transaction_ptr)GET_SYM(_GLOBAL_f_tbl->lH,"oracle_bmd_db_start_transaction");
	}
	_GLOBAL_libbmddb_initialized=1;
	return BMD_OK;
}

long bmd_db_end(void)
{
	CHECK_LIB;

	db_lib_close(_GLOBAL_f_tbl->lH);
	free0(_GLOBAL_f_tbl);

	return BMD_OK;
}

long bmd_db_row_struct_free(db_row_strings_t ** rowstruct)
{
	CHECK_LIB;

	if( _GLOBAL_f_tbl->bmd_db_row_struct_free == NULL )
		return BMD_ERR_INVALID_PTR;
	return _GLOBAL_f_tbl->bmd_db_row_struct_free(rowstruct);
}

long bmd_db_result_get_value(void *hDB,void *hRes,long which_row,long which_column,char **value,
											EnumDbFetchMode_t fetch_mode,long *successfully_fetched)
{
	CHECK_LIB;

	if( _GLOBAL_f_tbl->bmd_db_result_get_value == NULL )
		return BMD_ERR_INVALID_PTR;
	return _GLOBAL_f_tbl->bmd_db_result_get_value(hDB,hRes,which_row,which_column,value,
										  fetch_mode,successfully_fetched);
}

long bmd_db_result_get_row(	void *hDB,
				void *hRes,
				long which_row,
				long max_columns,
				db_row_strings_t **rowstruct,
				EnumDbFetchMode_t fetch_mode,long *successfully_fetched)
{
	CHECK_LIB;

	if( _GLOBAL_f_tbl->bmd_db_result_get_row == NULL )
		return BMD_ERR_INVALID_PTR;
	return _GLOBAL_f_tbl->bmd_db_result_get_row(hDB,hRes,which_row,max_columns, rowstruct,fetch_mode,successfully_fetched);
}

long bmd_db_result_free(void **hRes)
{
	CHECK_LIB;

	if( _GLOBAL_f_tbl->bmd_db_result_free == NULL )
		return BMD_ERR_INVALID_PTR;

	return _GLOBAL_f_tbl->bmd_db_result_free(hRes);
}

/* Funkcje do obslugi BLOB */
long bmd_db_export_blob(void *hDB,GenBuf_t *dataGenBuf,char **blob_number)
{
	CHECK_LIB;

	if( _GLOBAL_f_tbl->bmd_db_export_blob == NULL )
		return BMD_ERR_INVALID_PTR;
	return _GLOBAL_f_tbl->bmd_db_export_blob(hDB,dataGenBuf,blob_number);
}

long bmd_db_import_blob(void *hDB,char *blob_number,GenBuf_t **dataGenBuf)
{
	CHECK_LIB;

	if( _GLOBAL_f_tbl->bmd_db_import_blob == NULL )
		return BMD_ERR_INVALID_PTR;

	return _GLOBAL_f_tbl->bmd_db_import_blob(hDB,blob_number,dataGenBuf);
}

long bmd_db_export_blob_begin(void *hDB,char **blob_number,void **blob_locator)
{
	CHECK_LIB;

	if( _GLOBAL_f_tbl->bmd_db_export_blob_begin == NULL )
		return BMD_ERR_INVALID_PTR;

	return _GLOBAL_f_tbl->bmd_db_export_blob_begin(hDB,blob_number,blob_locator);
}

long bmd_db_export_blob_continue(void *hDB,GenBuf_t *chunkGenBuf,void *blob_locator,
											    EnumDbWhichPiece_t which_piece)
{
	CHECK_LIB;

	if( _GLOBAL_f_tbl->bmd_db_export_blob_continue == NULL )
		return BMD_ERR_INVALID_PTR;

	return _GLOBAL_f_tbl->bmd_db_export_blob_continue(hDB,chunkGenBuf,blob_locator,which_piece);
}

long bmd_db_export_blob_end(void *hDB,void **blob_locator)
{
	CHECK_LIB;

	if( _GLOBAL_f_tbl->bmd_db_export_blob_end == NULL )
		return BMD_ERR_INVALID_PTR;

	return _GLOBAL_f_tbl->bmd_db_export_blob_end(hDB,blob_locator);
}

long bmd_db_import_blob_begin(void *hDB,char *blob_number,long *blob_size,void **blob_locator,
											 GenBuf_t **dataGenBuf)
{
	CHECK_LIB;

	if( _GLOBAL_f_tbl->bmd_db_import_blob_begin == NULL )
		return BMD_ERR_INVALID_PTR;

	return _GLOBAL_f_tbl->bmd_db_import_blob_begin(hDB,blob_number,blob_size,blob_locator,dataGenBuf);
}

long bmd_db_import_blob_continue(void *hDB,GenBuf_t *chunkGenBuf,void *blob_locator,
												EnumDbWhichPiece_t which_piece)
{
	CHECK_LIB;

	if( _GLOBAL_f_tbl->bmd_db_import_blob_continue == NULL )
		return BMD_ERR_INVALID_PTR;
	return _GLOBAL_f_tbl->bmd_db_import_blob_continue(hDB,chunkGenBuf,blob_locator,which_piece);
}

long bmd_db_import_blob_end(void *hDB,void **blob_locator,GenBuf_t **dataGenBuf)
{
	CHECK_LIB;

	if( _GLOBAL_f_tbl->bmd_db_import_blob_end == NULL )
		return BMD_ERR_INVALID_PTR;
	return _GLOBAL_f_tbl->bmd_db_import_blob_end(hDB,blob_locator,dataGenBuf);
}

long bmd_db_delete_blob(void *hDB, char *blob_number)
{
	CHECK_LIB;

	if( _GLOBAL_f_tbl->bmd_db_delete_blob == NULL )
		return BMD_ERR_INVALID_PTR;
	return _GLOBAL_f_tbl->bmd_db_delete_blob(hDB,blob_number);
}

/* Funkcje do laczenia i rozlaczenia z baza */
long bmd_db_connect(const char *conn_info,void **hDB)
{
	CHECK_LIB;

	if( _GLOBAL_f_tbl->bmd_db_connect == NULL )
	{
		BMD_FOK(BMD_ERR_INVALID_PTR);
	}
	return _GLOBAL_f_tbl->bmd_db_connect(conn_info,hDB);
}

long bmd_db_connect2(const char *adr,char *port,const char *db_name,const char *user,
					 const char *pass,void **hDB)
{
	CHECK_LIB;

	if( _GLOBAL_f_tbl->bmd_db_connect2 == NULL )
		return BMD_ERR_INVALID_PTR;
	return _GLOBAL_f_tbl->bmd_db_connect2(adr,port,db_name,user,pass,hDB);
}

long bmd_db_disconnect(void **hDB)
{
	CHECK_LIB;

	if( _GLOBAL_f_tbl->bmd_db_disconnect == NULL )
	{
		BMD_FOK(BMD_ERR_INVALID_PTR);
	}

	BMD_FOK(_GLOBAL_f_tbl->bmd_db_disconnect(hDB));

	return BMD_OK;
}

long bmd_db_set_lost_connection_callback(void *hDB, bmd_db_restart_callback callbackFunction)
{
	CHECK_LIB;

	if( _GLOBAL_f_tbl->bmd_db_set_lost_connection_callback == NULL )
	{
		BMD_FOK(BMD_ERR_INVALID_PTR);
	}

	BMD_FOK(_GLOBAL_f_tbl->bmd_db_set_lost_connection_callback(hDB, callbackFunction));

	return BMD_OK;
}

long bmd_db_reconnect(void *hDB)
{
	CHECK_LIB;

	if( _GLOBAL_f_tbl->bmd_db_reconnect == NULL )
	{
		BMD_FOK(BMD_ERR_INVALID_PTR);
	}

	BMD_FOK(_GLOBAL_f_tbl->bmd_db_reconnect(hDB));

	return BMD_OK;
}

/* transakcje */
long bmd_db_start_transaction(void *hDB,long transaction_isolation_level)
{
	CHECK_LIB;

	if( _GLOBAL_f_tbl->bmd_db_start_transaction == NULL )
		return BMD_ERR_INVALID_PTR;
	return _GLOBAL_f_tbl->bmd_db_start_transaction(hDB,transaction_isolation_level);
}

long bmd_db_end_transaction(void *hDB)
{
	CHECK_LIB;

	if( _GLOBAL_f_tbl->bmd_db_end_transaction == NULL )
		return BMD_ERR_INVALID_PTR;
	return _GLOBAL_f_tbl->bmd_db_end_transaction(hDB);
}

long bmd_db_rollback_transaction(void *hDB,void *params)
{
	CHECK_LIB;

	if( _GLOBAL_f_tbl->bmd_db_rollback_transaction == NULL )
		return BMD_ERR_INVALID_PTR;
	return _GLOBAL_f_tbl->bmd_db_rollback_transaction(hDB,params);
}

/* wykonywanie zapytania SQL */
long bmd_db_execute_sql(void *hDB,const char *sql_query,long *rows,long *cols,void **hRes)
{
	CHECK_LIB;

	if( _GLOBAL_f_tbl->bmd_db_execute_sql == NULL )
		return BMD_ERR_INVALID_PTR;
	return _GLOBAL_f_tbl->bmd_db_execute_sql(hDB,sql_query,rows,cols,hRes);
}

/* jeszcze nie zaimplementowane po nowemu */
long bmd_db_execute_sql_bind(void *hDB,char *SQLQuery,
											long nParams,const char * const *params_values,
											long *ms_rows,long *ms_cols,void **hRes)
{
	CHECK_LIB;

	if( _GLOBAL_f_tbl->bmd_db_execute_sql_bind == NULL )
		return BMD_ERR_INVALID_PTR;
	return _GLOBAL_f_tbl->bmd_db_execute_sql_bind(hDB,SQLQuery,nParams,params_values,ms_rows,ms_cols,hRes);
}

/* bindowanie zmiennych */
long bmd_db_add_bind_var(const char *var,const char *var_type,void *hDB,char **bind_sql_str,
										char ***bv,long *bc)
{
	CHECK_LIB;

	if( _GLOBAL_f_tbl->bmd_db_add_bind_var == NULL )
		return BMD_ERR_INVALID_PTR;
	return _GLOBAL_f_tbl->bmd_db_add_bind_var(var,var_type,hDB,bind_sql_str,bv,bc);
}

long bmd_db_add_bind_var_long(long var,const char *var_type,void *hDB,char **bind_sql_str,
											 char ***bv,long *bc)
{
	CHECK_LIB;

	if( _GLOBAL_f_tbl->bmd_db_add_bind_var_long == NULL )
		return BMD_ERR_INVALID_PTR;
	return _GLOBAL_f_tbl->bmd_db_add_bind_var_long(var,var_type,hDB,bind_sql_str,bv,bc);
}

long bmd_db_link_bind_var_list(char **var_list,long var_count,void *hDB)
{
	CHECK_LIB;
	if( _GLOBAL_f_tbl->bmd_db_link_bind_var_list == NULL )
		return BMD_ERR_INVALID_PTR;

	return _GLOBAL_f_tbl->bmd_db_link_bind_var_list(var_list,var_count,hDB);
}

long bmd_db_free_bind_var_list(void *hDB)
{
	CHECK_LIB;
	if( _GLOBAL_f_tbl->bmd_db_free_bind_var_list == NULL )
		return BMD_ERR_INVALID_PTR;

	return _GLOBAL_f_tbl->bmd_db_free_bind_var_list(hDB);
}

long bmd_db_free_user_bind_var_list(void *hDB)
{
	CHECK_LIB;

	if( _GLOBAL_f_tbl->bmd_db_free_user_bind_var_list == NULL )
		return BMD_ERR_INVALID_PTR;

	return _GLOBAL_f_tbl->bmd_db_free_user_bind_var_list(hDB);
}

/* escapowanie danych */
#if 1
long escapeString(	const char *input,
				char **output)
{
long how_many=0;
long move=0;
long i=0;
char *temp=NULL;

	PRINT_INFO("LIBBMDDBINF Escaping SQL query condition string\n");
	/******************************/
	/*	walidacja paramterow	*/
	/******************************/
	if (input==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (output==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (*output!=NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	/********************************/
	/*	alokacja pamięci	*/
	/********************************/
	(*output)=(char *)malloc(sizeof(char)*strlen(input)*3+2);
	if(*output==NULL)	{	BMD_FOK(NO_MEMORY);	}
	memset(*output,0,sizeof(char)*strlen(input)*3+1);

	/******************************************/
	/*	oznaczanie poszukiwanych znakow	*/
	/******************************************/
	for (i=0; i<(long)strlen(input); i++)
	{
		if ((input[i]=='_')
			|| (input[i]=='%')
			|| (input[i]=='/')
			|| (input[i]=='\\')
			|| (input[i]==')')
			|| (input[i]=='(')
			|| (input[i]=='<')
			|| (input[i]=='>')
			|| (input[i]=='?')
			|| (input[i]=='*')
			|| (input[i]==';'))
		{
			(*output)[i+move++]='/';
			(*output)[i+move]=input[i];
		}
		else
		{
			(*output)[i+move]=input[i];
		}
	}

	asprintf(&temp,"%s",*output);
	if (temp==NULL)	{	BMD_FOK(NO_MEMORY);	}

	/************/
	/* porzadki */
	/************/
	free(temp); temp=NULL;

	return how_many;
}
#endif


long bmd_db_escape_string(	void *hDB,
					const char *src, 
					esc_mode_t mode,
					char **dest )
{
char *tmp		= NULL;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (hDB==NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (src==NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (dest==NULL)	{	BMD_FOK(BMD_ERR_PARAM3);	}
	if (*dest!=NULL)	{	BMD_FOK(BMD_ERR_PARAM3);	}

	CHECK_LIB;

	if( _GLOBAL_f_tbl->bmd_db_escape_string == NULL )
		return BMD_ERR_INVALID_PTR;
	if (mode == ADDITIONAL_ESCAPING) {
		BMD_FOK(escapeString(src,&tmp));
		BMD_FOK(_GLOBAL_f_tbl->bmd_db_escape_string(hDB,tmp,dest));
		free0(tmp);
	}
	else {
		BMD_FOK(_GLOBAL_f_tbl->bmd_db_escape_string(hDB,src,dest));
	}
	
	return BMD_OK;
}

/* pobieranie typu bazy danych */
long bmd_db_connect_get_db_type(void *hDB)
{
	CHECK_LIB;

	if( _GLOBAL_f_tbl->bmd_db_connect_get_db_type == NULL )
		return BMD_ERR_INVALID_PTR;
	return _GLOBAL_f_tbl->bmd_db_connect_get_db_type(hDB);
}

/* funkcje uzytkowe */
char *bmd_db_get_actualtime_sql(void *hDB)
{
	CHECK_LIB_PTR;

	if( _GLOBAL_f_tbl->bmd_db_get_actualtime_sql == NULL )
		return NULL;
	return _GLOBAL_f_tbl->bmd_db_get_actualtime_sql(hDB);
}

char *bmd_db_get_sequence_nextval(void *hDB,char *SequenceName)
{
	CHECK_LIB_PTR;

	if( _GLOBAL_f_tbl->bmd_db_get_sequence_nextval == NULL )
		return NULL;
	return _GLOBAL_f_tbl->bmd_db_get_sequence_nextval(hDB,SequenceName);
}

char *bmd_db_get_sequence_currval(void *hDB,char *SequenceName)
{
	CHECK_LIB_PTR;

	if( _GLOBAL_f_tbl->bmd_db_get_sequence_currval == NULL )
		return NULL;
	return _GLOBAL_f_tbl->bmd_db_get_sequence_currval(hDB,SequenceName);
}

long bmd_db_get_max_entry(void *hDB,char *column_name,char *table_name,char **id)
{
	CHECK_LIB;

	if( _GLOBAL_f_tbl->bmd_db_get_max_entry == NULL )
		return BMD_ERR_INVALID_PTR;
	return _GLOBAL_f_tbl->bmd_db_get_max_entry(hDB,column_name,table_name,id);
}

long bmd_db_date_to_timestamp(const char *Date, char **Timestamp)
{
	CHECK_LIB;

	if( _GLOBAL_f_tbl->bmd_db_date_to_timestamp == NULL )
		return BMD_ERR_INVALID_PTR;
	return _GLOBAL_f_tbl->bmd_db_date_to_timestamp(Date,Timestamp);
}

/* generuje klauzule ORDER BY */
long bmd_db_generate_order_clause(bmdDatagram_t *dtg, server_request_data_t* req, char **order_by)
{
	CHECK_LIB;

	if( _GLOBAL_f_tbl->bmd_db_generate_order_clause == NULL )
		return BMD_ERR_INVALID_PTR;
	return _GLOBAL_f_tbl->bmd_db_generate_order_clause(dtg, req, order_by);
}

/* generuje zapytanie takie by wyniki byly stronicowane */
long bmd_db_paginate_sql_query(	char *sql_begin,
				char *order_by,
				bmdDatagram_t *dtg,
				long sql_offset,
				long sql_limit,
				char **sql_final,
				char **sql_limits)
{
	CHECK_LIB;

	if( _GLOBAL_f_tbl->bmd_db_paginate_sql_query == NULL )
		return BMD_ERR_INVALID_PTR;
	return _GLOBAL_f_tbl->bmd_db_paginate_sql_query(sql_begin,order_by,dtg,sql_offset,sql_limit,sql_final,sql_limits);
}
