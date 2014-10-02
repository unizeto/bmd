#ifdef WIN32
#pragma warning(disable:4206)
#pragma warning(disable:4100)
#endif

#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>
#include <bmd/libbmddb/db2/db_libbmddb.h>

long db2_bmd_db_connect_get_db_type(void *hDB)
{

	return -1;
}

long db2_bmd_db_get_max_entry(void *connection_handle,char *column_name,char *table_name,char **id)
{
	return 1;
}

char *db2_bmd_db_get_actualtime_sql(void *hDB)
{
	char *TimeNow = NULL;

	return TimeNow;
}

char *db2_bmd_db_get_sequence_nextval(void *connection_handle,char *SequenceName)
{

	char *SequenceNextVal = NULL;

	if(connection_handle==NULL)			{	return NULL;	}
	if(SequenceName==NULL)				{	return NULL;	}

	asprintf(&SequenceNextVal,"NEXT VALUE FOR %s",SequenceName);
	return SequenceNextVal;
}

char *db2_bmd_db_get_sequence_currval(void *connection_handle, char *SequenceName)
{

	return NULL;
}

/**
implemented by WSZ
modified by MSil
Funkcja db2_bmd_db_escape_string() escapuje string w pelni w oparciu o mechanizmy escapowania dostarczone przez API libpq
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
long db2_bmd_db_escape_string(void *hDB,const char *src, char **dest)
{

	/* MS_DB */

		long srcLength=0;

		SQLCHAR *newStr=NULL;

		if(hDB == NULL)
			{ return -1; }
		if(src == NULL)
			{ return -2; }
		if(dest == NULL)
			{ return -3; }
		if(*dest != NULL)
			{ return -4; }

		srcLength=(long)strlen(src);

		newStr=(SQLCHAR*)calloc( (srcLength), sizeof(SQLCHAR) );

		if(srcLength == 0) //zwrocony pusty string dla pustego na wejsciu
		{
			*dest=(char *)newStr;
			return 0;
		}

		*dest=(char *)strdup((char*)newStr); //skopiowanie do minimalnie wymaganego fragmentu pamieci

		free(newStr);
		newStr=NULL;

		if(*dest == NULL )
			{ return -7; }

		return 0;
}


long db2_bmd_db_date_to_timestamp(const char *Date, char **Timestamp)
{

	return -1;
}

long db2_bmd_db_add_bind_var(const char *var,const char *var_type,void *hDB,
						 char **bind_sql_str,char ***bv,long *bc)
{

	return -1;
}

long db2_bmd_db_add_bind_var_long(long var,const char *var_type,void *hDB,char **bind_sql_str,
							  char ***bv,long *bc)
{
	return -1;
}

long db2_bmd_db_generate_order_clause(	bmdDatagram_t *dtg, server_request_data_t* req, char **order_by)
{
	return -1;
}

long db2_bmd_db_paginate_sql_query(	char *sql_begin,
						char *order_by,
						bmdDatagram_t *dtg,
						long sql_offset,
						long sql_limit,
						char **sql_final,
						char **sql_limits)
{
	return -1;
}


long print_error(SQLRETURN ret, char * info)
{
		if( ret == SQL_SUCCESS )
			return BMD_OK;
		else if(ret == SQL_SUCCESS_WITH_INFO )
			return BMD_OK;
		else if( ret == SQL_ERROR )
		{
			//PRINT_ERROR (info);
			PRINT_ERROR (", SQL_ERROR returned with error\n");
		}
		else if( ret == SQL_NO_DATA_FOUND )
		{
			PRINT_INFO(", SQL_NO_DATA_FOUND\n");
			return 5;
		}
		else if(ret == SQL_STILL_EXECUTING  )
		{
						PRINT_ERROR(", SQL_STILL_EXECUTING \n");
		}
		else if(ret == SQL_INVALID_HANDLE   )
		{			
			PRINT_ERROR(", SQL_INVALID_HANDLE  \n");
		}
		else if(ret == SQL_NEED_DATA   )
		{
			PRINT_ERROR(", SQL_NEED_DATA  \n");
		}
		return -1;
}
