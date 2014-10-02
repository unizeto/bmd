#ifdef WIN32
#pragma warning(disable:4206)
#pragma warning(disable:4100)
#endif

#include <bmd/libbmddb/postgres/pq_libbmddb.h>
#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>

long ConnectionData2ConnectionString(	const char *hostaddr,
					const char *port,
					const char *dbname,
					const char *user,
					const char *password,
					char **connection_string)
{
char *tmp = NULL;
printf("ConnectionData2ConnectionString\n");

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
	return BMD_OK;
}

/**
	Funkcja nawiazuje polaczenie z baza, umieszczajac
	dane w strukturze PQ_bmd_db_conn_t, przekazujac
	ja dalej jako void, funkcje ustawia magiczny wskaznik
	na wartosc BMD_DBCONN_MAGIC.
 */
long postgres_bmd_db_connect(const char *conn_info,void **hDB)
{
	printf("postgres_bmd_db_connect\n");

	PQ_bmd_db_conn_t *db_conn=NULL;

	if( conn_info == NULL )
		return BMD_ERR_PARAM1;
	if( hDB == NULL )
		return BMD_ERR_PARAM2;
	if( (*hDB) != NULL )
		return BMD_ERR_PARAM2;

	db_conn=(PQ_bmd_db_conn_t *)malloc(sizeof(PQ_bmd_db_conn_t));
	if( db_conn == NULL )
		return BMD_ERR_MEMORY;
	memset(db_conn,0,sizeof(PQ_bmd_db_conn_t));
	db_conn->magic=BMD_DBCONN_MAGIC;
	db_conn->db_type=BMD_DB_POSTGRES;

	db_conn->pg_conn=PQconnectdb(conn_info);
	if( db_conn->pg_conn == NULL )
	{
		PRINT_ERROR("LIBBMDDBERR PQconnectdb returned NULL\n");
		return BMD_DB_CONNECTION_ERROR;
	}

	if( PQstatus(db_conn->pg_conn) != CONNECTION_OK )
	{
		PRINT_ERROR("LIBBMDDBERR Error while connecting with database %s.\n",
					PQerrorMessage(db_conn->pg_conn));
		postgres_bmd_db_disconnect(hDB);
		return BMD_DB_CONNECTION_ERROR;
	}

	PQsetClientEncoding(db_conn->pg_conn,"UTF8");
	asprintf(&(db_conn->conn_string),"%s",conn_info);
	(*hDB)=db_conn;

	return BMD_OK;
}

long postgres_bmd_db_connect2(const char *adr,char *port,const char *db_name,const char *user,
					 const char *pass,void **hDB)
{
	printf("postgres_bmd_db_connect2\n");

	char *tmp_conn_str=NULL;
	long status;
	
	if( adr == NULL )
		return BMD_ERR_PARAM1;
	if( db_name == NULL )
		return BMD_ERR_PARAM3;
	if( hDB == NULL )
		return BMD_ERR_PARAM6;
	if( (*hDB) != NULL )
		return BMD_ERR_PARAM6;

	status=ConnectionData2ConnectionString(adr,port,db_name,user,pass,&tmp_conn_str);
	if( status != BMD_OK )
	{
		PRINT_ERROR("LIBBMDDBERR postgres_bmd_db_connect2 %li\n",status);
		free0(tmp_conn_str);
		return BMD_ERR_OP_FAILED;
	}
	status=postgres_bmd_db_connect(tmp_conn_str,hDB);
	if( status != BMD_OK )
	{
		PRINT_ERROR("LIBBMDDBERR postgres_bmd_db_connect %li\n",status);
		free0(tmp_conn_str);
		return BMD_ERR_OP_FAILED;
	}

	free0(tmp_conn_str);

	return BMD_OK;
}

long postgres_bmd_db_disconnect(void **hDB)
{
	printf("postgres_bmd_db_disconnect\n");

	PQ_bmd_db_conn_t *db_conn=NULL;

	if( hDB == NULL )
		return BMD_OK;
	if( (*hDB) == NULL )
		return BMD_OK;
	if( (*((long *)hDB)) == BMD_DBCONN_MAGIC )
		return BMD_ERR_INVALID_PTR;
	if( (*((long *)(*hDB)) != BMD_DBCONN_MAGIC ) )
		return BMD_ERR_INVALID_PTR;

	db_conn=(PQ_bmd_db_conn_t *)(*hDB);

	PQfinish(db_conn->pg_conn);
	free0(db_conn->conn_string);

	free0(*hDB);

	return BMD_OK;
}

long postgres_bmd_db_reconnect(void *hDB)
{
	printf("postgres_bmd_db_reconnect\n");

PQ_bmd_db_conn_t *db_conn=NULL;

	if(hDB == NULL)
		{ return BMD_ERR_PARAM1; }
	
	db_conn=(PQ_bmd_db_conn_t *)hDB;
	PQreset(db_conn->pg_conn);
	
	if(PQstatus(db_conn->pg_conn) != CONNECTION_OK)
	{
		PRINT_NOTICE("LIBBMDDBERR %s\n", PQerrorMessage(db_conn->pg_conn));
		return BMD_DB_RECONNECT_ERROR;
	}
	
	return BMD_OK;
}

long postgres_bmd_db_set_lost_connection_callback(void *hDB, bmd_db_restart_callback callbackFunction)
{
	printf("postgres_bmd_db_set_lost_connection_callback\n");

PQ_bmd_db_conn_t *db_conn = NULL;
	
	if(hDB == NULL)
		{ return BMD_ERR_PARAM1; }
	// jako callbackFunction mozna przypisac NULL, wiec brak sprawdzania
		
	db_conn=(PQ_bmd_db_conn_t *)hDB;
	db_conn->lostConnectionCallback=callbackFunction;
	
	return BMD_OK;
}
