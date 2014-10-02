#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>
#include <bmd/libbmddb/db2/db_libbmddb.h>

#ifdef WIN32
#pragma warning(disable:4206)
#pragma warning(disable:4100)
#endif



long db2_bmd_db_execute_sql(	void *hDB,
					const char *sql_query,
					long *rows,
					long *cols,
					void **hRes)
{
//PRINT_INFO("Function: db2_bmd_db_execute_sql\n");
	PRINT_INFO("SQL query: %s\n", sql_query);

DB_bmd_db_conn_t *db_conn		= NULL;
SQLRETURN     rc;


//int   rowsFetchedNb 			= 0;
SQLSMALLINT   nResultCols;

	if (hDB==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (sql_query==NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	//if (hRes==NULL)		{	BMD_FOK(BMD_ERR_PARAM5);	}
	//if ((*hRes)!=NULL)	{	BMD_FOK(BMD_ERR_PARAM5);	}

	SQLCHAR *stmt = (SQLCHAR *)sql_query;

	
	if( (*((long *)hDB)) != BMD_DBCONN_MAGIC )
	{	BMD_FOK(BMD_ERR_INVALID_PTR);	}

	db_conn=(DB_bmd_db_conn_t *)hDB;
	if( db_conn->hdbc == NULL )
	{
		PRINT_ERROR("LIBBMDDB error. Null DB pointer\n");
		BMD_FOK(BMD_ERR_OP_FAILED);
	}


	rc = SQLAllocHandle(SQL_HANDLE_STMT, *(db_conn->hdbc), db_conn->hstmt);
	if (rc != SQL_SUCCESS)
	{
		PRINT_ERROR("Driver reported the following diagnostics\n");
		extract_error("SQLAllocHandle", *(db_conn->hstmt), SQL_HANDLE_STMT);
		return BMD_DB_EXECUTION_ERROR;
	}

	SQLINTEGER autocommit;
	rc = SQLGetConnectAttr( *(db_conn->hdbc), SQL_AUTOCOMMIT,
	                       &autocommit, 0, NULL ) ;
	if (rc != SQL_SUCCESS)
	{
		PRINT_ERROR("Driver reported the following diagnostics\n");
		extract_error("SQLGetConnectAttr", *(db_conn->hdbc), SQL_HANDLE_DBC);
		return BMD_DB_EXECUTION_ERROR;
	}

	rc = SQLExecDirect( *(db_conn->hstmt), stmt, SQL_NTS );

	if(print_error(rc, "SQLExecDirect") == -1)
	{
		SQLFreeHandle(SQL_HANDLE_DBC, *(db_conn->hstmt));
		PRINT_ERROR("SQLExecDirect returned with error while sqlQuery: %s\n", stmt);
		extract_error("SQLExecDirect", *(db_conn->hstmt), SQL_HANDLE_STMT);
		return BMD_DB_EXECUTION_ERROR;
	}



	/* identify the output columns */
	rc = SQLNumResultCols( *(db_conn->hstmt), &nResultCols);
	print_error(rc, "SQLNumResultCols");

	if( cols != NULL )
		(*cols)=(long)nResultCols;


	return BMD_OK;
}

long db2_bmd_db_execute_sql_bind(	void *hDB,
					char *SQLQuery,
					long nParams,
					const char * const *params_values,
					long *ms_rows,
					long *ms_cols,
					void **hRes)
{

	return -1;
}

