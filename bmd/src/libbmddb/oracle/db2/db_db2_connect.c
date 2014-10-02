#ifdef WIN32
#pragma warning(disable:4206)
#pragma warning(disable:4100)
#endif

#include <bmd/libbmddb/db2/db_libbmddb.h>

#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>

void extract_error(char *fn, SQLHANDLE handle,SQLSMALLINT type);

long ConnectionData2ConnectionString(	const char *hostaddr,
					const char *port,
					const char *dbname,
					const char *user,
					const char *password,
					char **connection_string)
{
char *tmp = NULL;

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if(hostaddr == NULL)				{	BMD_FOK(BMD_DB_SERVER_ADDRESS_NOT_SPECIFIED);	}
	if(port == NULL)				{	BMD_FOK(BMD_DB_SERVER_PORT_NOT_SPECIFIED);	}
	if(dbname == NULL)				{	BMD_FOK(BMD_DB_NAME_NOT_SPECIFIED);		}
	if(user == NULL)				{	BMD_FOK(BMD_DB_USER_NAME_NOT_SPECIFIED);	}
	if(password == NULL)				{	BMD_FOK(BMD_DB_USER_PASSWORD_NOT_SPECIFIED);	}


	asprintf(connection_string, "hostaddr=%s port=%s dbname=%s user=%s", hostaddr, port, dbname, user);

	if(password)
	{
		asprintf(&tmp, "%s password=%s", *connection_string, password);
		free(*connection_string); *connection_string=NULL;

		asprintf(connection_string, "%s", tmp);
		free(tmp); tmp=NULL;
	}
	return -1;
}

/**
	Funkcja nawiazuje polaczenie z baza, umieszczajac
	dane w strukturze DB_bmd_db_conn_t, przekazujac
	ja dalej jako void, funkcje ustawia magiczny wskaznik
	na wartosc BMD_DBCONN_MAGIC.
 */
long db2_bmd_db_connect(const char *conn_info,void **hDB)
{
	PRINT_INFO("\n");



/* Local variables */
	SQLRETURN     rc;

	DB_bmd_db_conn_t *db_conn=NULL;

	if( conn_info == NULL )
		return BMD_ERR_PARAM1;
	if( hDB == NULL )
		return BMD_ERR_PARAM2;
	if( (*hDB) != NULL )
		return BMD_ERR_PARAM2;

	db_conn=(DB_bmd_db_conn_t *)malloc(sizeof(DB_bmd_db_conn_t));
	if( db_conn == NULL )
		return BMD_ERR_MEMORY;
	memset(db_conn,0,sizeof(DB_bmd_db_conn_t));
	db_conn->magic=BMD_DBCONN_MAGIC;
	db_conn->db_type=BMD_DB_DB2;


	rc = SQLSetEnvAttr(*(db_conn->henv), SQL_ATTR_ODBC_VERSION,
		        (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);
	rc = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, db_conn->henv);



	if (rc != SQL_SUCCESS)
    {
		PRINT_ERROR(">--- Error while connecting to database\n");
        return (SQL_ERROR);
    }
    else {
		db2_bmd_db_disconnect(hDB);
		return BMD_OK;
    }


	(*hDB)=db_conn;

	return -1;
}


long db2_bmd_db_connect2(const char *adr,char *port,const char *db_name,const char *user,
					 const char *pass,void **hDB)
{
	PRINT_INFO("\n");
	
	//	return BMD_ERR_PARAM1;
	if( db_name == NULL )
		return BMD_ERR_PARAM3;
	if( hDB == NULL )
		return BMD_ERR_PARAM6;
	if( (*hDB) != NULL )
		return BMD_ERR_PARAM6;


	SQLRETURN     rc;
	DB_bmd_db_conn_t *db_conn=NULL;

	db_conn=(DB_bmd_db_conn_t *)malloc(sizeof(DB_bmd_db_conn_t));
	if( db_conn == NULL )
		return BMD_ERR_MEMORY;

	memset(db_conn,0,sizeof(DB_bmd_db_conn_t));

	db_conn->magic=BMD_DBCONN_MAGIC;
	db_conn->db_type=BMD_DB_DB2;
	
	/*transakcja rozpoczêta 1*/
	/*transakcja nierozpoczêta -1*/
	db_conn->transaction_present = -1;

	db_conn->henv = (SQLHENV*)malloc(sizeof(SQLHENV));
	if( db_conn->henv == NULL )
			return BMD_ERR_MEMORY;
	db_conn->hdbc = (SQLHDBC*)malloc(sizeof(SQLHDBC));
		if( db_conn->hdbc == NULL )
			return BMD_ERR_MEMORY;
	db_conn->hstmt = (SQLHSTMT*)malloc(sizeof(SQLHSTMT));
		if( db_conn->hstmt == NULL )
			return BMD_ERR_MEMORY;


	rc = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, db_conn->henv);
	if (rc != SQL_SUCCESS)
	{

		PRINT_ERROR(">--- Error while SQLAllocHandle(SQL_HANDLE_ENV): %d\n", rc);
		extract_error("SQLAllocHandle", *(db_conn->henv), SQL_HANDLE_ENV);
		return BMD_DB_CONNECTION_ERROR;
	}

	rc = SQLSetEnvAttr(*(db_conn->henv), SQL_ATTR_ODBC_VERSION,(SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);
	if (rc != SQL_SUCCESS)
	{
		PRINT_ERROR(">--- Error while SQLSetEnvAttr (SQL_ATTR_ODBC_VERSION)\n");
		extract_error("SQLSetEnvAttr",*(db_conn->henv), SQL_HANDLE_ENV);
	    SQLFreeHandle(SQL_HANDLE_ENV, *(db_conn->henv));
	    return BMD_DB_CONNECTION_ERROR;
	}


	rc = SQLAllocHandle(SQL_HANDLE_DBC, *(db_conn->henv), db_conn->hdbc);
	if (rc != SQL_SUCCESS)
	{
		PRINT_ERROR(">--- Error while SQLAllocHandle(SQL_HANDLE_DBC)\n");
    	extract_error("SQLAllocHandle", *(db_conn->hdbc), SQL_HANDLE_DBC);

    	SQLFreeHandle(SQL_HANDLE_DBC, *(db_conn->hdbc));
		SQLFreeHandle(SQL_HANDLE_ENV, *(db_conn->henv));
		return BMD_DB_CONNECTION_ERROR;
	}



	/* set AUTOCOMMIT on */	
	rc = SQLSetConnectAttr( *(db_conn->hdbc), SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, SQL_NTS);
	if (rc != SQL_SUCCESS)
	{
		PRINT_ERROR(">--- Error while SQLSetConnectAttr (SQL_ATTR_AUTOCOMMIT)\n");
		extract_error("SQLAllocHandle", *(db_conn->hdbc), SQL_HANDLE_DBC);

		SQLFreeHandle(SQL_HANDLE_DBC, *(db_conn->hdbc));
		SQLFreeHandle(SQL_HANDLE_ENV, *(db_conn->henv));
		SQLFreeHandle(SQL_HANDLE_DBC, *(db_conn->hstmt));
		return BMD_DB_CONNECTION_ERROR;
	}
	
	rc = SQLConnect(*(db_conn->hdbc),(SQLCHAR *)(db_name), SQL_NTS,(SQLCHAR *)user, SQL_NTS, (SQLCHAR *)pass, SQL_NTS);

	if (rc != SQL_SUCCESS)
    {
    	PRINT_ERROR(">--- Error while connecting to database\n");
		extract_error("SQLConnect", *(db_conn->hdbc), SQL_HANDLE_DBC);
        SQLFreeHandle(SQL_HANDLE_DBC, *(db_conn->hdbc));
        SQLFreeHandle(SQL_HANDLE_ENV, *(db_conn->henv));
        return (BMD_DB_CONNECTION_ERROR);
    }

	(*hDB)=db_conn;
	return BMD_OK;
}

long db2_bmd_db_disconnect(void **hDB)
{
	PRINT_INFO("Function: db2_bmd_db_disconnect\n");
	DB_bmd_db_conn_t *db_conn=NULL;
	SQLRETURN     rc;
	long ret = BMD_OK;

	if( hDB == NULL )
		return BMD_OK;
	if( (*hDB) == NULL )
		return BMD_OK;
	if( (*((long *)hDB)) == BMD_DBCONN_MAGIC )
		return BMD_ERR_INVALID_PTR;
	if( (*((long *)(*hDB)) != BMD_DBCONN_MAGIC ) )
		return BMD_ERR_INVALID_PTR;

	db_conn=(DB_bmd_db_conn_t *)(*hDB);
	
	/* w db2 musimy skoñczyæ przerwane transakcje przed roz³¹czeniem */

	if( db_conn->transaction_present == 1)
	{
		ret = db2_bmd_db_rollback_transaction(db_conn, NULL);
		if (ret != BMD_OK)
			PRINT_ERROR("LIBBMDDB error while disconnecting from database\n");
	}


	rc = SQLFreeHandle(SQL_HANDLE_STMT, *(db_conn->hstmt));

	rc = SQLDisconnect(*(db_conn->hdbc));

	
	if (rc != SQL_SUCCESS)
	{
		PRINT_ERROR(">--- Error while disconnecting from database\n");
		extract_error("SQLDisconnect",*(db_conn->hdbc), SQL_HANDLE_DBC);
		ret = BMD_DB_CONNECTION_ERROR;
	}
   
	rc = SQLFreeHandle(SQL_HANDLE_DBC, *(db_conn->hdbc));
	rc = SQLFreeHandle(SQL_HANDLE_ENV, *(db_conn->henv));

	free0(*hDB);
	return ret;
}

long db2_bmd_db_reconnect(void *hDB)
{
	printf("db2_bmd_db_reconnect\n");
	return BMD_OK;
}

long db2_bmd_db_set_lost_connection_callback(void *hDB, bmd_db_restart_callback callbackFunction)
{
	printf("db2_bmd_db_set_lost_connection_callback\n");
	
	DB_bmd_db_conn_t *db_conn = NULL;	
	if(hDB == NULL)
		{ return BMD_ERR_PARAM1; }
	// jako callbackFunction mozna przypisac NULL, wiec brak sprawdzania		
	db_conn=(DB_bmd_db_conn_t *)hDB;
	db_conn->lostConnectionCallback=callbackFunction;
	return BMD_OK;
}

void extract_error(
    char *fn,
    SQLHANDLE handle,
    SQLSMALLINT type)
{
    SQLINTEGER	 i = 0;
    SQLINTEGER	 native;
    SQLCHAR	 state[ 7 ];
    SQLCHAR	 text[256];
    SQLSMALLINT	 len;
    SQLRETURN	 ret;

    PRINT_ERROR("The driver reported the following diagnostics while running:\n");

    do
    {
        ret = SQLGetDiagRec(type, handle, ++i, state, &native, text,
                            sizeof(text), &len );
        if (SQL_SUCCEEDED(ret))
            PRINT_ERROR("%s:%ld:%ld:%s\n", state, (long)i, (long)native, text);
    }
    while( ret == SQL_SUCCESS );
}


