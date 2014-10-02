#ifdef WIN32
#pragma warning(disable:4206)
#pragma warning(disable:4100)
#endif

#include <bmd/libbmddb/postgres/pq_libbmddb.h>
#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>

long postgres_bmd_db_start_transaction(void *hDB,long transaction_isolation_level)
{
	printf("postgres_bmd_db_start_transaction\n");

	long status;
	PQ_bmd_db_conn_t *db_conn=NULL;
	void *hRes=NULL;

	if( hDB == NULL )
		return BMD_ERR_PARAM1;
	if( (*((long *)hDB)) != BMD_DBCONN_MAGIC )
		return BMD_ERR_INVALID_PTR;

	db_conn=(PQ_bmd_db_conn_t *)hDB;
	if( db_conn->pg_conn == NULL )
	{
		PRINT_ERROR("LIBBMDDB error NULL pq ptr\n");
		return BMD_ERR_OP_FAILED;
	}
	if( db_conn->transaction_present != 0 )
	{
		PRINT_ERROR("LIBBMDDB error transaction already started\n");
		return BMD_ERR_FORMAT;
	}

	status=postgres_bmd_db_execute_sql(hDB,"BEGIN;",NULL,NULL,&hRes);
	if( status != BMD_OK )
	{
		PRINT_ERROR("LIBBMDDB error failed to start transaction\n");
		return BMD_ERR_OP_FAILED;
	}
	db_conn->transaction_present=1;
	postgres_bmd_db_result_free(&hRes);

	if( transaction_isolation_level == BMD_TRANSACTION_LEVEL_SERIALIZABLE )
	{
		status=postgres_bmd_db_execute_sql(hDB,"SET TRANSACTION ISOLATION LEVEL SERIALIZABLE;",NULL,NULL,&hRes);
		if( status != BMD_OK )
		{
			PRINT_ERROR("LIBBMDDB error failed to start serializable transaction\n");
			return BMD_ERR_OP_FAILED;
		}
		postgres_bmd_db_result_free(&hRes);
		db_conn->transaction_isolation_level=transaction_isolation_level;
	}

	return BMD_OK;
}

long postgres_bmd_db_end_transaction(void *hDB)
{
	printf("postgres_bmd_db_end_transaction\n");
	long status;
	PQ_bmd_db_conn_t *db_conn=NULL;
	void *hRes=NULL;

	if( hDB == NULL )
		return BMD_ERR_PARAM1;
	if( (*((long *)hDB)) != BMD_DBCONN_MAGIC )
		return BMD_ERR_INVALID_PTR;

	db_conn=(PQ_bmd_db_conn_t *)hDB;
	if( db_conn->pg_conn == NULL )
	{
		PRINT_ERROR("LIBBMDDB error NULL pq ptr\n");
		return BMD_ERR_OP_FAILED;
	}
	if( db_conn->transaction_present != 1 )
	{
		PRINT_ERROR("LIBBMDDB error transaction not started\n");
		return BMD_ERR_FORMAT;
	}

	status=postgres_bmd_db_execute_sql(hDB,"COMMIT;",NULL,NULL,&hRes);
	if( status != BMD_OK )
	{
		PRINT_ERROR("LIBBMDDB error failed to commit transaction\n");
		return BMD_ERR_OP_FAILED;
	}
	db_conn->transaction_present=0;
	postgres_bmd_db_result_free(&hRes);

	return BMD_OK;
}

long postgres_bmd_db_rollback_transaction(void *hDB,void *params)
{
	printf("postgres_bmd_db_rollback_transaction\n");

	long status;
	PQ_bmd_db_conn_t *db_conn=NULL;
	void *hRes=NULL;

	if( hDB == NULL )
		return BMD_ERR_PARAM1;
	if( (*((long *)hDB)) != BMD_DBCONN_MAGIC )
		return BMD_ERR_INVALID_PTR;

	db_conn=(PQ_bmd_db_conn_t *)hDB;
	if( db_conn->pg_conn == NULL )
	{
		PRINT_ERROR("LIBBMDDB error NULL pq ptr\n");
		return BMD_ERR_OP_FAILED;
	}
	if( db_conn->transaction_present != 1 )
	{
		PRINT_ERROR("LIBBMDDB error transaction not started\n");
		return BMD_ERR_FORMAT;
	}

	status=postgres_bmd_db_execute_sql(hDB,"ROLLBACK;",NULL,NULL,&hRes);
	if( status != BMD_OK )
	{
		PRINT_ERROR("LIBBMDDB error failed to rollback transaction\n");
		return BMD_ERR_OP_FAILED;
	}
	db_conn->transaction_present=0;
	postgres_bmd_db_result_free(&hRes);

	return BMD_OK;
}
