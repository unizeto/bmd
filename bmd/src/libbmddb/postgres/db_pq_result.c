#ifdef WIN32
#pragma warning(disable:4206)
#pragma warning(disable:4100)
#endif

#include <bmd/libbmddb/postgres/pq_libbmddb.h>
#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>

long postgres_bmd_db_result_free(void **hRes)
{
	printf("postgres_bmd_db_result_free\n");

	PQ_bmd_db_result_t *db_res=NULL;

	if( hRes == NULL )
		return BMD_OK;
	if( (*hRes) == NULL )
		return BMD_OK;
	if( (*((long *)hRes)) == BMD_DBRES_MAGIC )
		return BMD_ERR_INVALID_PTR;
	if( (*((long *)(*hRes)) != BMD_DBRES_MAGIC ) )
		return BMD_ERR_INVALID_PTR;

	db_res=(PQ_bmd_db_result_t *)(*hRes);

	PQclear(db_res->pg_res);

	free0(*hRes);

	return BMD_OK;
}

/* TODO zaimplementowac FETCH_NEXT */
long postgres_bmd_db_result_get_value(void *hDB,void *hRes,long which_row,long which_column,char **value,
							 EnumDbFetchMode_t fetch_mode,long *successfully_fetched)
{
	printf("postgres_bmd_db_result_get_value\n");

	long which_row_next		= 0;
	PQ_bmd_db_conn_t *db_conn=NULL;
	PQ_bmd_db_result_t *db_res=NULL;

	/* MSILEWICZ - zakomentowalem dla czytelnosci logow */
	//PRINT_INFO("LIBBMDPGINF Getting string from SQL result\n");
	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if(hDB==NULL)			{	BMD_FOK(BMD_DB_INVALID_CONNECTION_HANDLE);	}
	if(hRes == NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(which_row<0)					{	BMD_FOK(BMD_ERR_PARAM3);	}
	if(which_column<0)				{	BMD_FOK(BMD_ERR_PARAM4);	}
	if(value == NULL)				{	BMD_FOK(BMD_ERR_PARAM5);	}
	if(*value != NULL)				{       BMD_FOK(BMD_ERR_PARAM5);        }
	if(successfully_fetched == NULL)		{	BMD_FOK(BMD_ERR_PARAM6);	}
	if(*successfully_fetched < 0)			{	BMD_FOK(BMD_ERR_PARAM6);	}
	if( (*((long *)(hDB)) != BMD_DBCONN_MAGIC ) )
		return BMD_ERR_INVALID_PTR;
	if( (*((long *)(hRes)) != BMD_DBRES_MAGIC ) )
		return BMD_ERR_INVALID_PTR;

	db_conn=(PQ_bmd_db_conn_t *)hDB;
	db_res=(PQ_bmd_db_result_t *)hRes;

	if( fetch_mode == FETCH_ABSOLUTE )
		which_row_next=which_row;
	else
		if( fetch_mode == FETCH_NEXT )
			which_row_next=db_res->rows_fetched;
		else
		{
			return BMD_ERR_FORMAT;
		}

	if( (which_row_next+1) > PQntuples(db_res->pg_res))
	{
		return BMD_ERR_NODATA;
	}

	asprintf(value,"%s",PQgetvalue(db_res->pg_res, which_row_next, which_column));
	db_res->rows_fetched++;
	(*successfully_fetched) = db_res->rows_fetched;

	return BMD_OK;
}


long postgres_bmd_db_result_get_row(void *hDB,void *hRes,long which_row,long max_columns,db_row_strings_t **rowstruct,
						   EnumDbFetchMode_t fetch_mode,long *successfully_fetched)
{
	printf("postgres_bmd_db_result_get_row\n");

	PQ_bmd_db_conn_t *db_conn=NULL;
	PQ_bmd_db_result_t *db_res=NULL;

	long which_row_next		= 0;
	long colsreturned		= 0;
	long colsprocess		= 0;
	long err			= 0;
	long i				= 0;

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if(hDB==NULL)			{	BMD_FOK(BMD_DB_INVALID_CONNECTION_HANDLE);	}
	if(hRes == NULL)			{	BMD_FOK(BMD_ERR_PARAM2);			}
	if(which_row < 0)				{	BMD_FOK(BMD_ERR_PARAM3);			}
	if(max_columns < 0)				{	BMD_FOK(BMD_ERR_PARAM4);			}
	if(rowstruct == NULL)				{       BMD_FOK(BMD_ERR_PARAM5);       			}
	if(*rowstruct != NULL)				{       BMD_FOK(BMD_ERR_PARAM5);                        }
	if( (*((long *)(hDB)) != BMD_DBCONN_MAGIC ) )
		return BMD_ERR_INVALID_PTR;
	if( (*((long *)(hRes)) != BMD_DBRES_MAGIC ) )
		return BMD_ERR_INVALID_PTR;

	db_conn=(PQ_bmd_db_conn_t *)hDB;
	db_res=(PQ_bmd_db_result_t *)hRes;

	if( fetch_mode == FETCH_ABSOLUTE )
		which_row_next=which_row;
	else
		if( fetch_mode == FETCH_NEXT )
			which_row_next=db_res->rows_fetched;
		else
		{
			return BMD_ERR_FORMAT;
		}

	/* W postgresie w trybie FETCH_NEXT musimy sami sprawdzac czy nie chcemy czytac z komorki spoza rezultatu */
	if(which_row_next+1 > PQntuples(db_res->pg_res))
	{
		return BMD_ERR_NODATA;
	}

	*rowstruct = (db_row_strings_t *) malloc ( sizeof(db_row_strings_t) );
	if(*rowstruct == NULL)
	{
		PRINT_ERROR("LIBBMDPGERR Memory error. Error=%i\n",NO_MEMORY);
		return NO_MEMORY;
	}
	memset(*rowstruct, 0, sizeof(db_row_strings_t));

	colsreturned = PQnfields(db_res->pg_res);
	if(max_columns <= 0)
	{
		colsprocess = colsreturned;
	}
	else
	{
		colsprocess = (colsreturned < max_columns)? colsreturned:max_columns;
	}

	/* przetwarzam mniejsza wartosc z colsreturned i max_columns */
	(*rowstruct)->size = colsprocess;

	(*rowstruct)->colvals = (char **) malloc (sizeof(char *) * colsprocess);
	if((*rowstruct)->colvals == NULL)
	{
		PRINT_ERROR("LIBBMDPGERR Memory error. Error=%i\n",NO_MEMORY);
		return NO_MEMORY;
	}
	memset((*rowstruct)->colvals, 0, sizeof(char *) * colsprocess);

	/* Wypelnij wszystkie stringi z wiersza */
	for(i=0; i<colsprocess; i++)
	{
		asprintf(&((*rowstruct)->colvals[i]),"%s",PQgetvalue(db_res->pg_res, which_row_next, i));
	}

	/* uaktualnij licznik przerobionych prawidlowo wierszy */
	db_res->rows_fetched++;
	(*successfully_fetched) = db_res->rows_fetched;

	return err;
}

long postgres_bmd_db_row_struct_free(db_row_strings_t **rowstruct)
{
	printf("postgres_bmd_db_row_struct_free\n");

long err = 0;
long i = 0;

	if(rowstruct == NULL)		{	return BMD_ERR_PARAM1;	}

	if( *rowstruct==NULL)
	{
		/*PRINT_ERROR("LIBBMDPGERR Invalid first parameter. Error=%i\n",BMD_ERR_PARAM1);*/
		return BMD_ERR_PARAM1;
	}

	for(i=0; i<(*rowstruct)->size; i++)
	{
			free((*rowstruct)->colvals[i]); (*rowstruct)->colvals[i] = NULL;
	}
	free((*rowstruct)->colvals); (*rowstruct)->colvals = NULL;
	free(*rowstruct); *rowstruct = NULL;
	return err;
}
