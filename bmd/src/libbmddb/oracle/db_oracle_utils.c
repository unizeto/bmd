#ifdef WIN32
#pragma warning(disable:4206)
#pragma warning(disable:4100)
#endif

#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>
#include <bmd/libbmddb/oracle/libbmddb_oracle.h>

long oracle_bmd_db_escape_string(void *hDB,const char *src, char **dest)
{
	/* MS_DB */
	if( src == NULL )
		return BMD_ERR_PARAM2;
	if( dest == NULL )
		return BMD_ERR_PARAM3;

	asprintf(dest,"%s",src);

	return BMD_OK;
}

long oracle_bmd_db_date_to_timestamp(const char *Date, char **Timestamp)
{
long err = 0;
	asprintf(Timestamp, "TIMESTAMP '%s.00'", Date);
	if(*Timestamp == NULL) return NO_MEMORY;
	return(err);
}

char *oracle_bmd_db_get_actualtime_sql(void *hDB)
{
	char *TimeNow = NULL;
	void *hRes=NULL;
	long f=0;
	long status;
	char *tmp=NULL;

	if(hDB == NULL)
		return NULL;

	status=oracle_bmd_db_execute_sql(hDB,"SELECT LOCALTIMESTAMP AS LTS FROM DUAL",
							  NULL,NULL,&hRes);
	status=oracle_bmd_db_result_get_value(hDB,hRes,0,0,&tmp,FETCH_NEXT,&f);
	oracle_bmd_db_result_free(&hRes);
	asprintf(&TimeNow,"'%s'",tmp);
	free0(tmp);

	return TimeNow;
}

char *oracle_bmd_db_get_sequence_nextval(void *connection_handle, char *SequenceName)
{
	char *SequenceNextVal = NULL;

	if(connection_handle == NULL || SequenceName == NULL)
		return NULL;

	asprintf(&SequenceNextVal,"%s.nextval",SequenceName);

	return SequenceNextVal;
}

char *oracle_bmd_db_get_sequence_currval(void *connection_handle, char *SequenceName)
{
	char *SequenceCurVal = NULL;

	if(connection_handle == NULL || SequenceName == NULL)
		return NULL;

	asprintf(&SequenceCurVal,"%s.currval",SequenceName);

	return SequenceCurVal;
}

long oracle_bmd_db_get_max_entry(void *connection_handle, char *column_name, char *table_name,char **id)
{
	long err 			= 0;
	char *SQLQuery			= NULL;
	void *database_result		= NULL;
	char *tmp			= NULL;
	long fetched_counter		= 0;

	if(connection_handle == NULL)
		return BMD_ERR_PARAM1;
	if(column_name == NULL)
		return BMD_ERR_PARAM2;
	if(table_name == NULL)
		return BMD_ERR_PARAM3;
	if(id == NULL)
		return BMD_ERR_PARAM4;

	asprintf(&SQLQuery, "SELECT %s_%s_seq.currval FROM dual;", table_name, column_name);
	if(SQLQuery == NULL) {
		return NO_MEMORY;
	}

	err = oracle_bmd_db_execute_sql(connection_handle,SQLQuery,NULL,NULL,(void **)&database_result);
	if(err < 0) {
		PRINT_ERROR("LIBBMDOCIERR Error in Executing SQL Query. RecievedErrorCode=%li.\n", err);
		if(SQLQuery != NULL) { free(SQLQuery); SQLQuery = NULL; }
		return err;
	}

	fetched_counter = 0;
	err = oracle_bmd_db_result_get_value(connection_handle, database_result, 0, 0, &tmp, FETCH_NEXT, &fetched_counter);
	if(err<0) {
		oracle_bmd_db_result_free(&database_result);
		if(SQLQuery != NULL) { free(SQLQuery); SQLQuery = NULL; }
		if(tmp) {free(tmp); tmp=NULL;}
		return err;
	}
	asprintf(id,"%s",tmp);

	oracle_bmd_db_result_free(&database_result);
	if(tmp != NULL)		{ free(tmp);		tmp=NULL;}
	if(SQLQuery != NULL)	{ free(SQLQuery); 	SQLQuery = NULL; }
	PRINT_ERROR("LIBBMDOCIINF[%s:%li:bmd_db_get_max_entry] Max entry number succesfully got from sequence.\n", __FILE__, __LINE__);
	return err;
}

long oracle_bmd_db_generate_order_clause(bmdDatagram_t *dtg, server_request_data_t* req, char **order_by)
{
	long i;
	char *tmp_sort=NULL,**sort_oids=NULL,**sort_orders=NULL,**order_table_names=NULL;
	char **order_column_names=NULL;
	long sort_oids_count=0,sort_orders_count=0;

	if( dtg == NULL )
		return BMD_ERR_PARAM1;
	if( order_by == NULL )
		return BMD_ERR_PARAM2;
	if( (*order_by) != NULL )
		return BMD_ERR_PARAM2;

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
				asprintf(&(order_table_names[i]), "add_metadata");
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

	asprintf(order_by, "ORDER BY ");
	for (i=0; i<sort_orders_count; i++)
	{
		asprintf(order_by, "%s %s.%s %s,",*order_by,order_table_names[i],order_column_names[i],
				                          sort_orders[i]);
	}

	(*order_by)[strlen((*order_by))-1]='\0';

	return BMD_OK;
}

long oracle_bmd_db_paginate_sql_query(	char *sql_begin,
					char *order_by,
					bmdDatagram_t *dtg,
					long sql_offset,
					long sql_limit,
					char **sql_final,
					char **sql_limits)
{
	char *ol_str=NULL;
	long status;
	char **str_N=NULL;
	long tmp_N=0;
	long offset;
	long limit;
	long lim_for_opt=0;

	if( sql_begin == NULL )
		return BMD_ERR_PARAM1;
	if( sql_final == NULL )
		return BMD_ERR_PARAM4;
	if( (*sql_final) != NULL )
		return BMD_ERR_PARAM4;

	if( dtg != NULL )
	{
		ol_str=(char *)malloc(dtg->protocolDataFileId->size+1);
		memset(ol_str,0,dtg->protocolDataFileId->size+1);
		memmove(ol_str,dtg->protocolDataFileId->buf,dtg->protocolDataFileId->size);

		status = bmd_strsep(ol_str,'|',&str_N,&tmp_N);
		offset=strtol(str_N[0],NULL,10);
		limit=strtol(str_N[1],NULL,10);
		lim_for_opt=limit;
		bmd_strsep_destroy(&str_N,tmp_N);
	}
	else
	{
		offset=sql_offset;
		limit=sql_limit;
	}

	limit=limit+offset;
	if( order_by != NULL )
		asprintf(sql_final,"SELECT * FROM (SELECT /*+ FIRST_ROWS(%li) */ hlp.*, ROWNUM rnum FROM (%s %s) hlp WHERE ROWNUM<=%li) WHERE rnum>%li",
						   lim_for_opt-1,sql_begin,order_by,limit,offset);
	else
		asprintf(sql_final,"SELECT * FROM (SELECT /*+ FIRST_ROWS(%li) */ hlp.*, ROWNUM rnum FROM (%s) hlp WHERE ROWNUM<=%li) WHERE rnum>%li",
						   lim_for_opt-1,sql_begin,limit,offset);

	return BMD_OK;
}

long oracle_bmd_db_connect_get_db_type(void *hDB)
{
	OCI_bmd_db_conn_t *db_conn=NULL;

	if( hDB == NULL )
		return BMD_DB_UNKNOWN;
	if( (*((long *)hDB)) != BMD_DBCONN_MAGIC )
		return BMD_DB_UNKNOWN;

	db_conn=(OCI_bmd_db_conn_t *)hDB;

	switch(db_conn->db_type) {
		case BMD_DB_POSTGRES:
		case BMD_DB_ORACLE:
			return db_conn->db_type;
	}

	return BMD_DB_UNKNOWN;
}
