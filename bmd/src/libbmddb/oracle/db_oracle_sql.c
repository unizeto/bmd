#ifdef WIN32
#pragma warning(disable:4206)
#pragma warning(disable:4100)
#endif

#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>
#include <bmd/libbmddb/oracle/libbmddb_oracle.h>

long oracle_bmd_db_execute_sql(void *hDB,const char *SQLQuery,long *rows,long *cols,void **hRes)
{
	OCI_bmd_db_result_t *db_res=NULL;
	OCI_bmd_db_conn_t *db_conn=NULL;
	char *OracleSQLQuery			= NULL;
	ub2 StatementType			= 0;
	ub4 no_of_inserted_prefetched_rows	= 0;
	ub4 StatementMode			= 0;
	long err					= 0;
	/* bindowanie zmiennych */
	OCIBind *bindVariable=NULL;
	sb2 indp=0;
	long i;

	if(hDB == NULL)
		return BMD_ERR_PARAM1;
	if(SQLQuery == NULL)
		return BMD_ERR_PARAM2;
	if(hRes == NULL)
		return BMD_ERR_PARAM5;
	if((*hRes) != NULL )
		return BMD_ERR_PARAM5;
	if( (*(long *)hDB) != BMD_DBCONN_MAGIC )
		return BMD_ERR_INVALID_PTR;

	db_conn=(OCI_bmd_db_conn_t *)hDB;
	if(db_conn->bmdHandles == NULL )
		return BMD_ERR_NODATA;

	asprintf(&OracleSQLQuery,"%s",SQLQuery);
	if(OracleSQLQuery[strlen(OracleSQLQuery)-1] == ';' )
		OracleSQLQuery[strlen(OracleSQLQuery)-1] = '\0';
	PRINT_ERROR("LIBBMDOCIINF SQL Query [%s].\n",OracleSQLQuery);

	db_res=(OCI_bmd_db_result_t *)malloc(sizeof(OCI_bmd_db_result_t));
	if( db_res == NULL )
		return BMD_ERR_MEMORY;
	memset(db_res,0,sizeof(OCI_bmd_db_result_t));
	db_res->magic=BMD_DBRES_MAGIC;

	/* Alokcaja handlera zapytania SQL - nie zwalniac bo jest to zmienna wyjsciowa!!!*/
	err = checkerr(db_conn->bmdHandles->ErrorHandle,
			OCIHandleAlloc( (dvoid *) db_conn->bmdHandles->EnviromentHandle,
					(dvoid **) &(db_res->oci_res),
					OCI_HTYPE_STMT,
					(size_t) 0,
					(dvoid **) 0)
	);
	if(err<0) {
		PRINT_ERROR("LIBBMDOCIERR Error in allocating statement handle - OCIHandleAlloc. "
			"RecievedErrorCode=%li.\n", err);
		if(OracleSQLQuery) {free(OracleSQLQuery); OracleSQLQuery=NULL;}
		if (db_res->oci_res) {
			OCIHandleFree((dvoid *) db_res->oci_res, (ub4) OCI_HTYPE_STMT);
			db_res->oci_res=NULL;
		}
		if( rows != NULL )
			*rows=-1; 
		if( cols != NULL )
			*cols=-1;
		return BMD_ERR_OP_FAILED;
	}

	/* Przyotowanie zapytania */
	err = checkerr(db_conn->bmdHandles->ErrorHandle,
			OCIStmtPrepare(db_res->oci_res,
				db_conn->bmdHandles->ErrorHandle,
				(text *) OracleSQLQuery,
				(ub4) strlen((char *) OracleSQLQuery),
				(ub4) OCI_NTV_SYNTAX,
				(ub4) OCI_DEFAULT)
	);
	if(err<0) {
		PRINT_ERROR("LIBBMDOCIERR Error in preparing statement handle. "
			"RecievedErrorCode=%li.\n", err);
		if(OracleSQLQuery) {free(OracleSQLQuery); OracleSQLQuery=NULL;}
		if (db_res->oci_res) {
			OCIHandleFree((dvoid *)db_res->oci_res, (ub4) OCI_HTYPE_STMT);
			db_res->oci_res=NULL;
		}
		if( rows != NULL )
			*rows=-1; 
		if( cols != NULL )
			*cols=-1;
		return BMD_ERR_OP_FAILED;
	}

	/********************************************************************************
	* Sprawdz typ zapytania gdyz jest to potrzebne do ustwienia odpowiedniej	*
	* wartosci w OCIStmtExecute w zmiennej no_of_prefetched_rows, ktora 		*
	* dla SELECT w trybie implicit powinna byc rowna 0 zas dla inserta pojedynczego	*
	* powinna miec wartosc 1							*
	********************************************************************************/
	err = checkerr(db_conn->bmdHandles->ErrorHandle,
				   OCIAttrGet((dvoid *)db_res->oci_res,
				(ub4) OCI_HTYPE_STMT,
				(dvoid *) &StatementType,
				(ub4 *) 0,
				(ub4) OCI_ATTR_STMT_TYPE,
				(OCIError *) db_conn->bmdHandles->ErrorHandle)
	);
	if(err<0) {
		PRINT_ERROR("LIBBMDOCIERR Error in checking statement type. "
			"RecievedErrorCode=%li.\n", err);
		if(OracleSQLQuery) {free(OracleSQLQuery); OracleSQLQuery=NULL;}
		if (db_res->oci_res) {
			OCIHandleFree((dvoid *)db_res->oci_res, (ub4) OCI_HTYPE_STMT);
			db_res->oci_res=NULL;
		}
		*rows=-1; *cols=-1;
		return BMD_ERR_OP_FAILED;
	}

	/* W zaleznosci od typu zapytania ustaw flage StatementMode i no_of_inserted_prefetched_rows	*/
	switch(StatementType) {
		case OCI_STMT_SELECT:
					no_of_inserted_prefetched_rows=0;
					StatementMode=OCI_DEFAULT;
					break;
		case OCI_STMT_INSERT: 	no_of_inserted_prefetched_rows=1;
					StatementMode=OCI_DEFAULT;
					break;
		case OCI_STMT_UPDATE:	no_of_inserted_prefetched_rows=1;
					StatementMode=OCI_DEFAULT;
					break;
		case OCI_STMT_DELETE:	no_of_inserted_prefetched_rows=1;
					StatementMode=OCI_DEFAULT;
					break;
		case OCI_STMT_CREATE:	no_of_inserted_prefetched_rows=1;
					StatementMode=OCI_DEFAULT;
					break;
		case OCI_STMT_ALTER: no_of_inserted_prefetched_rows=1;
					StatementMode=OCI_DEFAULT;
					break;
		default: 		PRINT_ERROR("LIBBMDOCIERR Unsupported query type\n");
					no_of_inserted_prefetched_rows=0;
					StatementMode=OCI_DEFAULT;
					if(OracleSQLQuery!=NULL) { free(OracleSQLQuery); OracleSQLQuery=NULL; }
					return BMD_ERR_UNIMPLEMENTED;
					break;
	}

	if( db_conn->bind_vars != NULL )
	{
		for(i=0;i<db_conn->bind_count;i++)
		{
			err=checkerr(db_conn->bmdHandles->ErrorHandle,
						OCIBindByPos(db_res->oci_res,&bindVariable,db_conn->bmdHandles->ErrorHandle,
									(ub4)(i+1),
									(dvoid *)db_conn->bind_vars[i],(sb4)strlen(db_conn->bind_vars[i]),
									SQLT_CHR,&indp,0,0,0,0,OCI_DEFAULT));
			if( err < 0 )
			{
				PRINT_ERROR("LIBBMDOCIERR Error in binding variable - RecievedErrorCode=%li.\n", err);
				PRINT_ERROR("LiBBMDOCIERR bind var value == %s count %li\n",
							db_conn->bind_vars[i],i+1);
			}
			/* pointer jest juz podpiety do db_res->oci_res i zostanie zwolniony automatycznie */
			bindVariable=NULL; 
			indp=0;
		}
	}
	else
	{
		for(i=0;i<db_conn->user_bind_count;i++)
		{
			err=checkerr(db_conn->bmdHandles->ErrorHandle,
						OCIBindByPos(db_res->oci_res,&bindVariable,db_conn->bmdHandles->ErrorHandle,
									(ub4)(i+1),
									(dvoid *)db_conn->user_bind_vars[i],
									(sb4)strlen(db_conn->user_bind_vars[i]),
									SQLT_CHR,&indp,0,0,0,0,OCI_DEFAULT));
			if( err < 0 )
			{
				PRINT_ERROR("LIBBMDOCIERR Error in binding variable - RecievedErrorCode=%li.\n", err);
				PRINT_ERROR("LiBBMDOCIERR bind var value == %s count %li\n",
							db_conn->user_bind_vars[i],i+1);
			}
			/* pointer jest juz podpiety do db_res->oci_res i zostanie zwolniony automatycznie */
			bindVariable=NULL; 
			indp=0;
		}
	}
	/* Wykonaj zapytanie SQL */
	err = checkerr(db_conn->bmdHandles->ErrorHandle,
		OCIStmtExecute(db_conn->bmdHandles->ServiceContextHandle,
				db_res->oci_res,
				db_conn->bmdHandles->ErrorHandle,
				(ub4) no_of_inserted_prefetched_rows,	/* no of prefetch rows */
				(ub4) 0,
				(CONST OCISnapshot *) NULL,
				(OCISnapshot *) NULL,
				(ub4) StatementMode)
	);
	if( db_conn->user_bind_vars != NULL )
	{
		for(i=0;i<db_conn->user_bind_count;i++)
		{
			free0(db_conn->user_bind_vars[i]);
		}
		free0(db_conn->user_bind_vars);
		db_conn->user_bind_count=0;
	}
	if(err<0) {
		PRINT_ERROR("LIBBMDOCIERR Error in executing statement - OCIStmtExecute. "
			"RecievedErrorCode=%li.\n", err);
		if(OracleSQLQuery) {free(OracleSQLQuery); OracleSQLQuery=NULL;}
		if (db_res->oci_res) {
			OCIHandleFree((dvoid *)db_res->oci_res, (ub4) OCI_HTYPE_STMT);
			db_res->oci_res=NULL;
		}
		if( rows != NULL )
			*rows=-1;
		if( cols != NULL )
			*cols=-1;
		return BMD_ERR_OP_FAILED;
	}

	/* pobierz liczbe zwroconych przez zapytanie kolumn */
	if(cols != NULL) {
		err = checkerr(db_conn->bmdHandles->ErrorHandle,
			OCIAttrGet((dvoid *)db_res->oci_res,
				(ub4) OCI_HTYPE_STMT,
				(dvoid *) cols,
				(ub4 *) 0,
				(ub4) OCI_ATTR_PARAM_COUNT,
				(OCIError *)db_conn->bmdHandles->ErrorHandle)
		);
		if(err<0) {
			PRINT_ERROR("LIBBMDOCIERR Error in getting number of columns - OCIAttrGet. "
				"RecievedErrorCode=%li.\n", err);
			if(OracleSQLQuery) {free(OracleSQLQuery); OracleSQLQuery=NULL;}
			if (db_res->oci_res) {
				OCIHandleFree((dvoid *)db_res->oci_res, (ub4) OCI_HTYPE_STMT);
				db_res->oci_res=NULL;
			}
			if( rows != NULL )
				*rows=-1; 
			if( cols != NULL )
				*cols=-1;
			return BMD_ERR_OP_FAILED;
		}
	}
	else 
	{
		if( cols != NULL )
			*cols = -1;
	}

	/* pobierz liczbe zwroconych przez zapytanie wierszy jako SELECT count(x) */
	if(rows != NULL)
	{
		if(StatementType == OCI_STMT_SELECT) 
		{
			char select_count_buf[100];
			long sc_limit,i;

			memset(select_count_buf,0,100);
			if(strlen(SQLQuery)<12)
				sc_limit=(long)strlen(SQLQuery);
			else
				sc_limit=12;
			for(i=0;i<sc_limit;i++)
			{
				select_count_buf[i]=(char)toupper(SQLQuery[i]);
			}
			/*PRINT_ERROR("SELECT_COUNT_BUF|%s|SELECT_COUNT_BUF",select_count_buf);*/
			if( memcmp(select_count_buf,"SELECT COUNT",12) == 0 )
			{
				PRINT_ERROR("LIBBMDOCIINF TO JUZ JEST ZAPYTANIE TYPU SELECT COUNT");
				(*hRes)=db_res;
				return BMD_OK;
			}
			else
			{
				err = SQLQueryNoOfReturnedRows(hDB, OracleSQLQuery, rows);
				PRINT_DEBUG("MSILEWICZ_ORACLE %li\n",*rows);
				if(err<0) {
					PRINT_ERROR("LIBBMDOCIERR Error in getting number of rows - SQLQueryNoOfReturnedRows. "
						"RecievedErrorCode=%li.\n", err);
					if(OracleSQLQuery) {free(OracleSQLQuery); OracleSQLQuery=NULL;}
					if (db_res->oci_res) {
						OCIHandleFree((dvoid *)db_res->oci_res, (ub4) OCI_HTYPE_STMT);
						db_res->oci_res=NULL;
					}
					if( rows != NULL )
						*rows=-1; 
					if( cols != NULL )
						*cols=-1;
					return BMD_ERR_OP_FAILED;
				}
			}
		}
		else 
		{ 
			if( rows != NULL )
				*rows=1; 
		} /* Dla DDL zapytanie zwraca jeden wiersz statusu */
	}
	else 
	{
		if( rows != NULL )
			*rows = -1;
	}

	if(OracleSQLQuery!=NULL) { free0(OracleSQLQuery); }
	/*if( ( rows != NULL ) && ( cols != NULL ) )
		PRINT_ERROR("LIBBMDOCIINF Query executed succesfully. "
			"No of rows=%li, no of columns=%li\n", *rows, *cols);*/

	(*hRes)=db_res;
	return err;
}

long oracle_bmd_db_add_bind_var(const char *var,const char *var_type,void *hDB,char **bind_sql_str,
 						        char ***bv,long *bc)
{
	OCI_bmd_db_conn_t *db_conn=NULL;
	/* var_type jest ignorowany w przypadku oracle */

	if( var == NULL ) { BMD_FOK(BMD_ERR_PARAM1); };
	if( bind_sql_str == NULL ) { BMD_FOK(BMD_ERR_PARAM4); };
	if( ( hDB == NULL ) && ( bv == NULL ) )
	{
		PRINT_ERROR("LIBBMDOCIERR hDB or bv should be NOT NULL\n");
		BMD_FOK(BMD_ERR_FORMAT);
	}
	if( hDB != NULL )
	{
		if( (*(long *)hDB) != BMD_DBCONN_MAGIC )
			return BMD_ERR_INVALID_PTR;

		db_conn=(OCI_bmd_db_conn_t *)hDB;
		if(db_conn->bmdHandles == NULL )
			return BMD_ERR_NODATA;
	}

	if( db_conn != NULL )
	{
		/* umieszczenie zmiennej bindowanej w hDB */
		db_conn->bind_vars=(char **)realloc(db_conn->bind_vars,sizeof(char *)*(db_conn->bind_count+1));
		db_conn->bind_vars[db_conn->bind_count]=NULL;
		asprintf(&(db_conn->bind_vars[db_conn->bind_count]),"%s",var);
		db_conn->bind_count++;
		asprintf(bind_sql_str,":%li",db_conn->bind_count);
	}
	else
	{
		/* umieszczenie zmiennych na liscie */
		(*bv)=(char **)realloc(*bv,((*bc)+1)*sizeof(char *));
		(*bv)[*bc]=NULL;
		asprintf(&((*bv)[*bc]),"%s",var);
		(*bc)++;
		asprintf(bind_sql_str,":%li",*bc);
	}
	return BMD_OK;
}

long oracle_bmd_db_link_bind_var_list(char **var_list,long var_count,void *hDB)
{
	OCI_bmd_db_conn_t *db_conn=NULL;
	long i;
	if( var_list == NULL )
	{
		BMD_FOK(BMD_ERR_PARAM1);
	}
	if( hDB == NULL )
	{
		BMD_FOK(BMD_ERR_PARAM3);
	}
	if( (*((long *)hDB)) != BMD_DBCONN_MAGIC )
	{
		BMD_FOK(BMD_ERR_INVALID_PTR);
	}

	db_conn=(OCI_bmd_db_conn_t *)hDB;

	db_conn->user_bind_vars=(char **)malloc(var_count*sizeof(char *));
	for(i=0;i<var_count;i++)
	{
		asprintf(&(db_conn->user_bind_vars[i]),"%s",var_list[i]);
		PRINT_DEBUG("INSERT_BIND VARIABLE |%s|\n",db_conn->user_bind_vars[i]);
	}
	db_conn->user_bind_count=var_count;
	
	return BMD_OK;
}

long oracle_bmd_db_add_bind_var_long(long var,const char *var_type,void *hDB,char **bind_sql_str,
 						      char ***bv,long *bc)
{
	return BMD_OK;
}

long oracle_bmd_db_free_bind_var_list(void *hDB)
{
	OCI_bmd_db_conn_t *db_conn=NULL;
	long i=0;

	db_conn=(OCI_bmd_db_conn_t *)hDB;

	if( db_conn->bind_vars != NULL )
	{
		for(i=0;i<db_conn->bind_count;i++)
		{
			free0(db_conn->bind_vars[i]);
		}
		free0(db_conn->bind_vars);
		db_conn->bind_count=0;
	}
	return BMD_OK;
}

long oracle_bmd_db_free_user_bind_var_list(void *hDB)
{
	OCI_bmd_db_conn_t *db_conn=NULL;
	long i=0;

	db_conn=(OCI_bmd_db_conn_t *)hDB;

	if( db_conn->user_bind_vars != NULL )
	{
		for(i=0;i<db_conn->user_bind_count;i++)
		{
			free0(db_conn->user_bind_vars[i]);
		}
		free0(db_conn->user_bind_vars);
		db_conn->user_bind_count=0;
	}

	return BMD_OK;
}