#ifdef WIN32
#pragma warning(disable:4206)
#pragma warning(disable:4100)
#endif

#include <bmd/libbmddb/db2/db_libbmddb.h>

#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>

long db2_bmd_db_start_transaction(void *hDB,long transaction_isolation_level)
{
	PRINT_INFO("Function:db2_bmd_db_start_transaction \n");
	if( hDB == NULL )
		return BMD_ERR_PARAM1;

	SQLRETURN     rc;
	DB_bmd_db_conn_t *db_conn		= NULL;

	if( (*((long *)hDB)) != BMD_DBCONN_MAGIC )
	{	BMD_FOK(BMD_ERR_INVALID_PTR);	}

	db_conn=(DB_bmd_db_conn_t *)hDB;
	if( db_conn->hdbc == NULL )
	{
		PRINT_ERROR("LIBBMDDB error. Null DB pointer\n");
		BMD_FOK(BMD_ERR_OP_FAILED);
	}


	/* set AUTOCOMMIT off */
	rc = SQLSetConnectAttr( *(db_conn->hdbc), SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_OFF, SQL_NTS);
	if (rc != SQL_SUCCESS)
	{
		PRINT_ERROR(">--- Error while SQLSetConnectAttr (SQL_ATTR_AUTOCOMMIT):\n");
		extract_error("SQLSetConnectAttr", *(db_conn->hdbc), SQL_HANDLE_DBC);

		return BMD_DB_CONNECTION_ERROR;
	}

	db_conn->transaction_present = 1;

	return BMD_OK;
}

long db2_bmd_db_end_transaction(void *hDB)
{
	PRINT_INFO("Function:db2_bmd_db_end_transaction \n");
	if( hDB == NULL )
		return BMD_ERR_PARAM1;
	SQLRETURN     rc;
	DB_bmd_db_conn_t *db_conn		= NULL;
	if( (*((long *)hDB)) != BMD_DBCONN_MAGIC )
	{	BMD_FOK(BMD_ERR_INVALID_PTR);	}

	db_conn=(DB_bmd_db_conn_t *)hDB;
	if( db_conn->hdbc == NULL )
	{
		PRINT_ERROR("LIBBMDDB error. Null DB pointer\n");
		BMD_FOK(BMD_ERR_OP_FAILED);
	}


	rc = SQLEndTran(SQL_HANDLE_DBC, *(db_conn->hdbc), SQL_COMMIT);

	if (rc != SQL_SUCCESS)
	{
		PRINT_ERROR(">--- Error while SQLEndTran\n");
		extract_error("SQLEndTran", *(db_conn->hdbc), SQL_HANDLE_DBC);

		return BMD_DB_CONNECTION_ERROR;
	}

	/* set AUTOCOMMIT off */

	rc = SQLSetConnectAttr( *(db_conn->hdbc), SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, SQL_NTS);
	if (rc != SQL_SUCCESS)
	{
		PRINT_ERROR(">--- Error while SQLSetConnectAttr\n");
		extract_error("SQLSetConnectAttr", *(db_conn->hdbc), SQL_HANDLE_DBC);

		return BMD_DB_CONNECTION_ERROR;
	}
	db_conn->transaction_present = -1;
	return BMD_OK;
}

long db2_bmd_db_rollback_transaction(void *hDB,void *params)
{
	printf("db2_bmd_db_rollback_transaction\n");

	SQLRETURN     rc;
	DB_bmd_db_conn_t *db_conn=NULL;

	if( hDB == NULL )
		return BMD_ERR_PARAM1;
	if( (*((long *)hDB)) != BMD_DBCONN_MAGIC )
		return BMD_ERR_INVALID_PTR;

	db_conn=(DB_bmd_db_conn_t *)hDB;
	if( db_conn->hdbc == NULL )
	{
		PRINT_ERROR("LIBBMDDB error NULL db ptr\n");
		return BMD_ERR_OP_FAILED;
	}
	if( db_conn->transaction_present != 1 )
	{
		PRINT_ERROR("LIBBMDDB error transaction not started\n");
		return BMD_ERR_FORMAT;
	}

	rc = SQLEndTran(SQL_HANDLE_DBC, *(db_conn->hdbc), SQL_ROLLBACK);

	if (rc != SQL_SUCCESS)
	{
		PRINT_ERROR(">--- Error while SQL_ROLLBACK\n");
		extract_error("SQL_ROLLBACK", *(db_conn->hdbc), SQL_HANDLE_DBC);

		return BMD_DB_CONNECTION_ERROR;
	}
	
	db_conn->transaction_present=-1;
	
	return BMD_OK;
}
