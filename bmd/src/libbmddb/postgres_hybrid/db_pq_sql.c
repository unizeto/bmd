#include <bmd/libbmddb/postgres/pq_libbmddb.h>
#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>

#ifdef WIN32
#pragma warning(disable:4206)
#pragma warning(disable:4100)
#endif

long postgres_bmd_db_execute_sql(	void *hDB,
					const char *sql_query,
					long *rows,
					long *cols,
					void **hRes)
{
PQ_bmd_db_conn_t *db_conn		= NULL;
PQ_bmd_db_result_t *db_res		= NULL;

ExecStatusType pg_status		= 0;

	if (hDB==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (sql_query==NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (hRes==NULL)		{	BMD_FOK(BMD_ERR_PARAM5);	}
	if ((*hRes)!=NULL)	{	BMD_FOK(BMD_ERR_PARAM5);	}

	if (_GLOBAL_debug_level==-5)
	{
		PRINT_TEST("sql query: \n%s\n",sql_query);
	}

	if( (*((long *)hDB)) != BMD_DBCONN_MAGIC )
	{	BMD_FOK(BMD_ERR_INVALID_PTR);	}

	db_conn=(PQ_bmd_db_conn_t *)hDB;
	if( db_conn->pg_conn == NULL )
	{
		PRINT_ERROR("LIBBMDDB error. Null PQ pointer\n");
		BMD_FOK(BMD_ERR_OP_FAILED);
	}

	db_res=(PQ_bmd_db_result_t *)malloc(sizeof(PQ_bmd_db_result_t));
	if( db_res == NULL )	{	BMD_FOK(BMD_ERR_MEMORY);	}

	memset(db_res,0,sizeof(PQ_bmd_db_result_t));
	db_res->magic=BMD_DBRES_MAGIC;

	db_res->pg_res=PQexec(db_conn->pg_conn,sql_query);
	if( db_res->pg_res == NULL )
	{
		if(PQstatus(db_conn->pg_conn) == CONNECTION_BAD)
		{
			if(db_conn->lostConnectionCallback != NULL)
			{
				db_conn->lostConnectionCallback();
			}
		}
		
		free(db_res); db_res = NULL;
		PRINT_ERROR("PQexec returned NULL ptr\n");
		BMD_FOK(BMD_DB_EXECUTION_ERROR);
	}

	pg_status=PQresultStatus(db_res->pg_res);

	if( (pg_status != PGRES_COMMAND_OK) && (pg_status != PGRES_TUPLES_OK) )
	{
		if(PQstatus(db_conn->pg_conn) == CONNECTION_BAD)
		{
			if(db_conn->lostConnectionCallback != NULL)
			{
				db_conn->lostConnectionCallback();
			}
		}
		
		PQclear(db_res->pg_res);
		free(db_res); db_res = NULL;
		PRINT_ERROR("PQexec returned with error %s\n",PQerrorMessage(db_conn->pg_conn));
		BMD_FOK(BMD_ERR_OP_FAILED);
	}

	if( rows != NULL )
	{
		int tmp_int;
		tmp_int=PQntuples(db_res->pg_res);
		(*rows)=(long)tmp_int;
	}
	if( cols != NULL )
	{
		int tmp_int;
		tmp_int=PQnfields(db_res->pg_res);
		(*cols)=(long)tmp_int;
	}

	(*hRes)=db_res;

	return BMD_OK;
}

long postgres_bmd_db_execute_sql_bind(	void *hDB,
					char *SQLQuery,
					long nParams,
					const char * const *params_values,
					long *ms_rows,
					long *ms_cols,
					void **hRes)
{
	ExecStatusType res_status;
	PQ_bmd_db_conn_t *db_conn=NULL;
	PQ_bmd_db_result_t *db_res=NULL;

	if (hDB==NULL)				{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (SQLQuery==NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (params_values==NULL)		{	BMD_FOK(BMD_ERR_PARAM4);	}
	if (hRes==NULL)				{	BMD_FOK(BMD_ERR_PARAM7);	}
	if ((*hRes)!=NULL)			{	BMD_FOK(BMD_ERR_PARAM7);	}
	if ((*((long *)hDB))!=BMD_DBCONN_MAGIC)	{	BMD_FOK(BMD_ERR_INVALID_PTR);	}

	db_conn=(PQ_bmd_db_conn_t *)hDB;
	if( db_conn->pg_conn == NULL )		{	BMD_FOK(BMD_ERR_NODATA);	}

	db_res=(PQ_bmd_db_result_t *)malloc(sizeof(PQ_bmd_db_result_t));
	if( db_res == NULL )	{	BMD_FOK(NO_MEMORY);	}

	memset(db_res,0,sizeof(PQ_bmd_db_result_t));
	db_res->magic=BMD_DBRES_MAGIC;

	db_res->pg_res=PQexecParams(db_conn->pg_conn,SQLQuery,nParams,NULL,params_values,NULL,NULL,0);
	if( db_res->pg_res == NULL )
	{
		if(PQstatus(db_conn->pg_conn) == CONNECTION_BAD)
		{
			if(db_conn->lostConnectionCallback != NULL)
			{
				db_conn->lostConnectionCallback();
			}
		}
		
		BMD_FOK(BMD_DB_EXECUTION_ERROR);
	}

	res_status=PQresultStatus(db_res->pg_res);
	if( ( res_status != PGRES_COMMAND_OK ) && ( res_status != PGRES_TUPLES_OK ) )
	{
		if(PQstatus(db_conn->pg_conn) == CONNECTION_BAD)
		{
			if(db_conn->lostConnectionCallback != NULL)
			{
				db_conn->lostConnectionCallback();
			}
		}
		
		PRINT_ERROR("LIBBMDPGERR PQexecParams command %s failed: %s.\n",
			SQLQuery,PQerrorMessage(db_conn->pg_conn));
		BMD_FOK(BMD_ERR_OP_FAILED);
	}

	(*hRes)=db_res;

	if( ms_rows != NULL )
		(*ms_rows)=(long)PQntuples(db_res->pg_res);
	if( ms_cols != NULL )
		(*ms_cols)=(long)PQnfields(db_res->pg_res);

	return BMD_OK;
}
