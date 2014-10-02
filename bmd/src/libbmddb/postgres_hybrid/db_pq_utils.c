#ifdef WIN32
#pragma warning(disable:4206)
#pragma warning(disable:4100)
#endif

#include <bmd/libbmddb/postgres/pq_libbmddb.h>
#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>

long postgres_bmd_db_connect_get_db_type(void *hDB)
{
	PQ_bmd_db_conn_t *db_conn=NULL;

	if (hDB == NULL )
		return BMD_DB_UNKNOWN;
	if ((*((long *)hDB)) != BMD_DBCONN_MAGIC )
		return BMD_DB_UNKNOWN;

	db_conn=(PQ_bmd_db_conn_t *)hDB;

	switch(db_conn->db_type) {
		case BMD_DB_POSTGRES:
		case BMD_DB_ORACLE:
			return db_conn->db_type;
	}

	return BMD_DB_UNKNOWN;
}

long postgres_bmd_db_get_max_entry(void *connection_handle,char *column_name,char *table_name,char **id)
{
long err 				= 0;
long rows 				= 0;
long cols				= 0;
long fetched_counter		= 0;
char *SQLQuery			= NULL;
void   *Result_postgres	= NULL;

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if(connection_handle==NULL)			{	BMD_FOK(BMD_DB_INVALID_CONNECTION_HANDLE);	}
	if(column_name == NULL)				{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(table_name == NULL)				{	BMD_FOK(BMD_ERR_PARAM3);	}
	if(id == NULL)					{	BMD_FOK(BMD_ERR_PARAM4);	}
	if((*id) != NULL)					{	BMD_FOK(BMD_ERR_PARAM4);	}

	/*asprintf(&SQLQuery, "SELECT MAX(%s) FROM %s;", column_name, table_name);*/
	asprintf(&SQLQuery, "SELECT currval('%s_%s_seq');", table_name, column_name);

	err = postgres_bmd_db_execute_sql(connection_handle, SQLQuery, &rows, &cols, (void **)&Result_postgres);
	if(err < 0)
	{
		PRINT_ERROR("LIBBMDPGERR Error in executing SQL query - postgres_bmd_db_execute_sql. Error=%li\n", err);
		free(SQLQuery); SQLQuery = NULL;
		if(Result_postgres != NULL) { postgres_bmd_db_result_free(&Result_postgres); }
		return err;
	}
	free(SQLQuery); 	SQLQuery = NULL;

	fetched_counter = 0;
	if(rows > 0)
	{
		err = postgres_bmd_db_result_get_value(connection_handle, (void *)Result_postgres, 0, 0,id, FETCH_NEXT, &fetched_counter);
		if(err<0)
		{
			PRINT_ERROR("LIBBMDPGERR Error in reading number of entries in database - postgres_bmd_db_result_get_value. "
			"Error=%li\n",err);
			if(Result_postgres != NULL)
			{
				postgres_bmd_db_result_free(&Result_postgres);
			}
			return err;
		}
	}
	else
	{
		asprintf(id, "0");
	}

	if(Result_postgres != NULL)
	{
		postgres_bmd_db_result_free(&Result_postgres);
	}
	free(SQLQuery); SQLQuery=NULL;

	return err;
}

char *postgres_bmd_db_get_actualtime_sql(void *hDB)
{
	char *TimeNow = NULL;

	if (hDB == NULL )
		return NULL;

	asprintf(&TimeNow,"%s","\'now\'");

	return TimeNow;
}

char *postgres_bmd_db_get_sequence_nextval(void *connection_handle,char *SequenceName)
{
	char *SequenceNextVal = NULL;

	if(connection_handle==NULL)			{	return NULL;	}
	if(SequenceName==NULL)				{	return NULL;	}

	asprintf(&SequenceNextVal,"nextval('%s')",SequenceName);
	return SequenceNextVal;
}

char *postgres_bmd_db_get_sequence_currval(void *connection_handle, char *SequenceName)
{
	char *SequenceCurrVal = NULL;

	if(connection_handle==NULL)			{	return NULL;	}
	if(SequenceName==NULL)				{	return NULL;	}

	asprintf(&SequenceCurrVal,"currval('%s')",SequenceName);

	return SequenceCurrVal;
}

/**
implemented by WSZ
modified by MSil
Funkcja postgres_bmd_db_escape_string() escapuje string w pelni w oparciu o mechanizmy escapowania dostarczone przez API libpq
i sama alokuje wyjsciowy wyescepowany lancuch znakowy.

Funkcja zwraca:
0 w przypadku sukcesu
lub wartosc ujemna w przypadku bledu:
@retval -1 gdy za dbConnHandler podano NULL
@retval -2 gdy za src podano NULL
@retval -3 gdy za dest podano NULL
@retval -4 gdy wartosc wyluskana z dest nie jest wyNULLowana
@retval -5, -7 w przypadku problemow z alokacja pamieci
@retval -6 gdy escapowanie nie powiodlo sie

@example
const char *src="Lubimy apostrofy '''' \n";
char *dest=NULL;

bmd_db_escape_string(dbConnHandler, src, &dest);
...
*/
long postgres_bmd_db_escape_string(void *hDB,const char *src, char **dest)
{
	/* MS_DB */
	char *newStr=NULL;
	long srcLength=0;
	long retVal=0;

	if(hDB == NULL)
		{ return -1; }
	if(src == NULL)
		{ return -2; }
	if(dest == NULL)
		{ return -3; }
	if(*dest != NULL)
		{ return -4; }

	srcLength=(long)strlen(src);

	newStr=(char*)calloc( (srcLength*2+2), sizeof(char) );
	if(newStr == NULL)
		{ return -5; }

	if(srcLength == 0) //zwrocony pusty string dla pustego na wejsciu
	{
		*dest=newStr;
		return 0;
	}

	retVal=(long)PQescapeStringConn(((PQ_bmd_db_conn_t *)hDB)->pg_conn, newStr, src, srcLength, NULL);
	if(retVal <= 0)
	{
		free(newStr); newStr=NULL;
		return -6;
	}

#ifndef WIN32
	*dest=strdup(newStr); //skopiowanie do minimalnie wymaganego fragmentu pamieci
#else
	*dest=_strdup(newStr);
#endif
	free(newStr); newStr=NULL;
	if(*dest == NULL )
		{ return -7; }

	return 0;
}


long postgres_bmd_db_date_to_timestamp(const char *Date, char **Timestamp)
{
	if (Date!=NULL)
	{
		asprintf(Timestamp, "\'%s\'", Date);
		if(*Timestamp == NULL)
		{
			BMD_FOK(NO_MEMORY);
		}
	}

	return BMD_OK;
}

long postgres_bmd_db_add_bind_var(const char *var,const char *var_type,void *hDB,
						 char **bind_sql_str,char ***bv,long *bc)
{
	PQ_bmd_db_conn_t *db_conn=NULL;

	if (var == NULL )
		return BMD_ERR_PARAM1;
	if (( hDB == NULL ) && ( bv == NULL ) )
		return BMD_ERR_NODATA;
	if (( bv != NULL ) && ( bc == NULL ) )
		return BMD_ERR_PARAM4;
	if (hDB != NULL )
	{
		if ((*(long *)hDB) != BMD_DBCONN_MAGIC )
			return BMD_ERR_INVALID_PTR;
	}

	if (hDB != NULL )
	{
		db_conn=(PQ_bmd_db_conn_t *)hDB;
		if (db_conn->pg_conn == NULL )
			return BMD_ERR_NODATA;
	}

	if (hDB == NULL )
	{
		(*bv)=(char **)realloc((*bv),sizeof(char *)*((*bc)+1));
		(*bv)[*bc]=NULL;

		if (var != NULL )
			asprintf(&((*bv)[*bc]),"%s",var);

		if (bind_sql_str != NULL )
		{
			if (var_type != NULL )
				asprintf(bind_sql_str,"$%li::%s",(*bc)+1,var_type);
			else
				asprintf(bind_sql_str,"$%li",(*bc)+1);
		}
		(*bc)++;
	}
	else
	{
		db_conn->bind_vars=(char **)realloc(db_conn->bind_vars,sizeof(char *)*(db_conn->bind_count+1));
		db_conn->bind_vars[db_conn->bind_count]=NULL;

		if (var != NULL )
			asprintf(&(db_conn->bind_vars[db_conn->bind_count]),"%s",var);

		if (bind_sql_str != NULL )
		{
			if (var_type != NULL )
				asprintf(bind_sql_str,"$%li::%s",db_conn->bind_count+1,var_type);
			else
				asprintf(bind_sql_str,"$%li",db_conn->bind_count+1);
		}
		db_conn->bind_count++;
	}

	return BMD_OK;
}

long postgres_bmd_db_add_bind_var_long(long var,const char *var_type,void *hDB,char **bind_sql_str,
							  char ***bv,long *bc)
{
	PQ_bmd_db_conn_t *db_conn=NULL;

	if (( hDB == NULL ) && ( bv == NULL ) )
		return BMD_ERR_NODATA;
	if (( bv != NULL ) && ( bc == NULL ) )
		return BMD_ERR_PARAM4;
	if (hDB != NULL )
	{
		if ((*(long *)hDB) != BMD_DBCONN_MAGIC )
			return BMD_ERR_INVALID_PTR;
	}

	if (hDB != NULL )
	{
		db_conn=(PQ_bmd_db_conn_t *)hDB;
		if (db_conn->pg_conn == NULL )
			return BMD_ERR_NODATA;
	}

	if (hDB == NULL )
	{
		(*bv)=(char **)realloc((*bv),sizeof(char *)*((*bc)+1));
		(*bv)[*bc]=NULL;

		asprintf(&((*bv)[*bc]),"%li",var);

		if (bind_sql_str != NULL )
		{
			if (var_type != NULL )
				asprintf(bind_sql_str,"$%li::%s",(*bc)+1,var_type);
			else
				asprintf(bind_sql_str,"$%li",(*bc)+1);
		}
		(*bc)++;
	}
	else
	{
		db_conn->bind_vars=(char **)realloc(db_conn->bind_vars,sizeof(char *)*(db_conn->bind_count+1));
		db_conn->bind_vars[db_conn->bind_count]=NULL;

		asprintf(&(db_conn->bind_vars[db_conn->bind_count]),"%li",var);

		if (bind_sql_str != NULL )
		{
			if (var_type != NULL )
				asprintf(bind_sql_str,"$%li::%s",db_conn->bind_count+1,var_type);
			else
				asprintf(bind_sql_str,"$%li",db_conn->bind_count+1);
		}
		db_conn->bind_count++;
	}

	return BMD_OK;
}

long postgres_bmd_db_generate_order_clause(	bmdDatagram_t *dtg, server_request_data_t* req, char **order_by)
{
long i				= 0;
char *tmp_sort			= NULL;
char **sort_oids		= NULL;
char **sort_orders		= NULL;
char **order_table_names	= NULL;
char **order_column_names	= NULL;
long sort_oids_count		= 0;
long sort_orders_count		= 0;

	if (dtg == NULL )		{	BMD_FOK(BMD_ERR_PARAM1);	}
	
	
	
	if (order_by == NULL )		{	BMD_FOK(BMD_ERR_PARAM3);	}
	if ((*order_by) != NULL )	{	BMD_FOK(BMD_ERR_PARAM3);	}

	if(	(dtg->protocolData!=NULL) && (dtg->protocolData->buf!=NULL) && (dtg->protocolData->size>0) )
	{
		tmp_sort=(char *)malloc(dtg->protocolData->size+1);
		memset(tmp_sort,0,dtg->protocolData->size+1);
		memmove(tmp_sort,dtg->protocolData->buf,dtg->protocolData->size);

		BMD_FOK(GetSortConditions(tmp_sort, req, &sort_oids, &sort_oids_count, &sort_orders, &sort_orders_count));
		free0(tmp_sort);
		order_table_names=(char **)malloc(sizeof(char*)*sort_orders_count);
		order_column_names=(char **)malloc(sizeof(char*)*sort_orders_count);

		for (i=0; i<sort_orders_count; i++)
		{
			if (strcmp(sort_oids[i],OID_SYS_METADATA_CRYPTO_OBJECTS_FILENAME)==0)
			{
				asprintf(&(order_table_names[i]), "crypto_objects");
				asprintf(&(order_column_names[i]), "name");
			}
			else
			if (strcmp(sort_oids[i],OID_SYS_METADATA_CRYPTO_OBJECTS_FILESIZE)==0)
			{
				asprintf(&(order_table_names[i]), "crypto_objects");
				asprintf(&(order_column_names[i]), "filesize");
			}
			else
			if (strcmp(sort_oids[i],OID_SYS_METADATA_CRYPTO_OBJECTS_TIMESTAMP)==0)
			{
				asprintf(&(order_table_names[i]), "crypto_objects");
				asprintf(&(order_column_names[i]), "insert_date");
			}
			else
			if ( (strncmp(sort_oids[i], OID_PREFIX_SYS_METADATA,strlen(OID_PREFIX_SYS_METADATA))!=0) &&
				 (strncmp(sort_oids[i], OID_PREFIX_PKI_METADATA,strlen(OID_PREFIX_PKI_METADATA))!=0) &&
				 (strncmp(sort_oids[i], OID_PREFIX_ACTION_METADATA,strlen(OID_PREFIX_ACTION_METADATA))!=0) )
			{
				bmd_str_replace(&(sort_oids[i]),".","_");
				asprintf(&(sort_oids[i]), "amv_%s", sort_oids[i]);
				asprintf(&(order_table_names[i]), "crypto_objects");
				asprintf(&(order_column_names[i]), "%s", sort_oids[i]);
			}
		}
	}
	else
	{
		sort_orders_count=1;
		order_table_names=(char **)malloc(sizeof(char*)*sort_orders_count);
		order_column_names=(char **)malloc(sizeof(char*)*sort_orders_count);
		sort_orders=(char **)malloc(sizeof(char*)*sort_orders_count);
		asprintf(&(order_table_names[0]), "crypto_objects");
		asprintf(&(order_column_names[0]), "id");
		asprintf(&(sort_orders[0]), "DESC");
	}

	if (sort_orders_count>0)
	{

		asprintf(order_by, "\nORDER BY ");
		for (i=0; i<sort_orders_count; i++)
		{
			asprintf(order_by, "%s %s.%s %s,",*order_by,order_table_names[i],order_column_names[i],
									sort_orders[i]);
		}

		(*order_by)[strlen((*order_by))-1]='\0';
	}
	else
	{
		asprintf(order_by, "%s", "");
	}

	return BMD_OK;
}

long postgres_bmd_db_paginate_sql_query(	char *sql_begin,
						char *order_by,
						bmdDatagram_t *dtg,
						long sql_offset,
						long sql_limit,
						char **sql_final,
						char **sql_limits)
{
char *ol_str	= NULL;
char **str_N	= NULL;
long tmp_N	= 0;
long offset	= 0;
long limit	= 0;

	if (sql_begin == NULL )		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (sql_final == NULL )		{	BMD_FOK(BMD_ERR_PARAM4);	}
	if ((*sql_final) != NULL )	{	BMD_FOK(BMD_ERR_PARAM4);	}

	if (( dtg!=NULL ) && (dtg->protocolDataFileId!=NULL) && (dtg->protocolDataFileId->buf!=NULL) && (dtg->protocolDataFileId->size>0))
	{
		ol_str=(char *)malloc(dtg->protocolDataFileId->size+1);
		memset(ol_str,0,dtg->protocolDataFileId->size+1);
		memmove(ol_str,dtg->protocolDataFileId->buf,dtg->protocolDataFileId->size);

		BMD_FOK(bmd_strsep(ol_str,'|',&str_N,&tmp_N));
		if ((str_N[0]!=NULL) && (tmp_N > 1) && (str_N[1]!=NULL))
		{
			offset=strtol(str_N[0],NULL,10);
			limit=strtol(str_N[1]==NULL?"0":str_N[1],NULL,10);
		}
		else
		{
			offset=0;
			limit=1;
		}
	}
	else
	{
		offset=sql_offset;
		limit=sql_limit;
	}

	if (order_by != NULL )
	{
		asprintf(sql_final,"%s %s OFFSET %li LIMIT %li;",sql_begin,order_by,offset,limit);
		if (sql_limits!=NULL)
		{
			asprintf(sql_limits,"%s OFFSET %li LIMIT %li;",order_by,offset,limit);
		}
	}
	else
	{
		asprintf(sql_final,"%s OFFSET %li LIMIT %li;",sql_begin,offset,limit);
		if (sql_limits!=NULL)
		{
			asprintf(sql_limits,"OFFSET %li LIMIT %li;",offset,limit);
		}
	}

	return BMD_OK;
}
