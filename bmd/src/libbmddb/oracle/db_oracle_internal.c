#ifdef WIN32
#pragma warning(disable:4206)
#pragma warning(disable:4100)
#endif

#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>
#include <bmd/libbmddb/oracle/libbmddb_oracle.h>

void freeOCIBMDHandles(OCIBMDHandles_t *OCIBMDHandles)
{
	/********************************
	* Zwolnij zaalokowane handlery	*
	********************************/
	if (OCIBMDHandles->SessionAuthHandle)
		OCIHandleFree((dvoid *) OCIBMDHandles->SessionAuthHandle, (ub4) OCI_HTYPE_SESSION);
	if (OCIBMDHandles->ServerHandle)
		OCIHandleFree((dvoid *) OCIBMDHandles->ServerHandle, (ub4) OCI_HTYPE_SERVER);
	if (OCIBMDHandles->ServiceContextHandle)
		OCIHandleFree((dvoid *) OCIBMDHandles->ServiceContextHandle, (ub4) OCI_HTYPE_SVCCTX);
	if (OCIBMDHandles->ErrorHandle)
		OCIHandleFree((dvoid *) OCIBMDHandles->ErrorHandle, (ub4) OCI_HTYPE_ERROR);
	if (OCIBMDHandles->EnviromentHandle)
		OCIHandleFree((dvoid *) OCIBMDHandles->EnviromentHandle, (ub4) OCI_HTYPE_ENV);
}

long checkerr(OCIError *ErrorHandle, sword status)
{
text errbuf[512];
sb4 errcode = 0;
long err=0;

	switch (status)
	{
	case OCI_SUCCESS:
				err = ERR_OCI_SUCCESS;
				break;
	case OCI_SUCCESS_WITH_INFO:
				err = ERR_OCI_SUCCESS_WITH_INFO;
				PRINT_ERROR("LIBBMDOCIERR[%s:%i:checkerr] Error: OCI success with info.\n", __FILE__, __LINE__);
				break;
	case OCI_NEED_DATA:
				err = ERR_OCI_NEED_DATA;
				PRINT_ERROR("LIBBMDOCIERR[%s:%i:checkerr] Error: OCI need data.\n", __FILE__, __LINE__);
				break;
	case OCI_NO_DATA:
				err = BMD_ERR_NODATA;
				PRINT_ERROR("LIBBMDOCIINF[%s:%i:checkerr] Warning: OCI no data.\n", __FILE__, __LINE__);
				break;
	case OCI_ERROR:
				err = ERR_OCI_ERROR;
				OCIErrorGet((dvoid *)ErrorHandle, (ub4) 1, (text *) NULL, &errcode, errbuf, (ub4) sizeof(errbuf), OCI_HTYPE_ERROR);
				PRINT_ERROR("LIBBMDOCIERR[%s:%i:checkerr] Unknown Error - info: %s.\n", __FILE__, __LINE__, errbuf);
				break;
	case OCI_INVALID_HANDLE:
				err = ERR_OCI_INVALID_HANDLE;
				PRINT_ERROR("LIBBMDOCIERR[%s:%i:checkerr] Error: OCI invalid handle.\n", __FILE__, __LINE__);
				break;
	case OCI_STILL_EXECUTING:
				err = ERR_OCI_STILL_EXECUTING;
				PRINT_ERROR("LIBBMDOCIERR[%s:%i:checkerr] Error: OCI still execute.\n", __FILE__, __LINE__);
				break;
	case OCI_CONTINUE:
				err = ERR_OCI_CONTINUE;
				PRINT_ERROR("LIBBMDOCIERR[%s:%i:checkerr] Error: OCI continue.\n", __FILE__, __LINE__);
				break;
	default:
				err = ERR_OCI_UNKNOWN_ERROR_CODE;
				PRINT_ERROR("LIBBMDOCIERR[%s:%i:checkerr] Error: Unknown error code.\n", __FILE__, __LINE__);
				break;
	}
	return err;
}

long SQLQueryNoOfReturnedRows(void *connection_handle, char *SQLQuery, long *rows)
{
char *SQLQueryNoOfRows			= NULL;
long err					= 0;
long execute_rows			= 0;
long execute_cols			= 0;

void *database_result			= NULL;
char *tmp				= NULL;
long fetched_counter	= 0;

	/* Sparsuj zapytanie SQL i utworz takie ktore zawiera funkcje count() */
	err = PrepareCountSQLQuery(SQLQuery, &SQLQueryNoOfRows);
	if(err<0) {
		return err;
	}

	err = oracle_bmd_db_execute_sql(connection_handle,
			SQLQueryNoOfRows,&execute_rows, &execute_cols, &database_result);
	if(err<0) {
		oracle_bmd_db_result_free(&database_result);
		PRINT_ERROR("LIBBMDOCIERR[%s:%i:SQLQueryNoOfReturnedRows] Error in executing SELECT count(*) query. RecievedErrorCode=%i.\n", __FILE__, __LINE__, err);
		if(SQLQueryNoOfRows) {free(SQLQueryNoOfRows); SQLQueryNoOfRows=NULL; }
		if(err != OCI_SUCCESS && err != OCI_SUCCESS_WITH_INFO) {
			*rows=0;
			err=0;	
		}
		return err;
	}

	fetched_counter = 0;
	err = oracle_bmd_db_result_get_value(connection_handle, database_result, 0, 0, &tmp, FETCH_NEXT, &fetched_counter);
	PRINT_DEBUG("MSILEWICZ_COUNT_ROWS %s\n",tmp);
	if(err<0) {
		oracle_bmd_db_result_free(&database_result);
		PRINT_ERROR("LIBBMDOCIERR[%s:%i:SQLQueryNoOfReturnedRows] Error fetching result from SELECT count(*) query. RecievedErrorCode=%i.\n", __FILE__, __LINE__, err);
		if(SQLQueryNoOfRows) { free(SQLQueryNoOfRows); SQLQueryNoOfRows=NULL; }
		if(tmp) { free(tmp); tmp=NULL; }
		return err;
	}

	*rows = strtol(tmp, (char **)NULL, 10);
	/*PRINT_ERROR("LIBBMDOCIINF[%s:%i:SQLQueryNoOfReturnedRows] String returned from SELECT count(*)={%i}\n", __FILE__, __LINE__, *rows);*/
	oracle_bmd_db_result_free(&database_result);
	if(SQLQueryNoOfRows) { free(SQLQueryNoOfRows); SQLQueryNoOfRows=NULL; }
	if(tmp) { free(tmp); tmp=NULL; }
	return err;
}

long PrepareCountSQLQuery(char *SQLQuery, char **SQLQueryNoOfRows)
{
long err		= 0;
char *tmp1	= NULL;
char *tmp2	= NULL;
long overalllen	= 0;
long tmp2len	= 0;

	if(SQLQuery==NULL )
		return BMD_ERR_PARAM1;
	if(SQLQueryNoOfRows==NULL)
		return BMD_ERR_PARAM2;

	*SQLQueryNoOfRows = (char *) malloc (sizeof(char) * (10+strlen(SQLQuery)));
	if(*SQLQueryNoOfRows==NULL)
		return NO_MEMORY;
	memset(*SQLQueryNoOfRows, 0, 10+strlen(SQLQuery));

	tmp1=SQLQuery; overalllen=0;
	/* szukam ostatniego slowa wyrazu SELECT lub NULLa	*/
	/* TODO dopisac szukanie calosci slowa SELECT 		*/
	while(*tmp1!='T' && *tmp1!='t' && *tmp1!=0) { tmp1++; overalllen++; }
	if(overalllen == (long)strlen(SQLQuery)) return BMD_ERR_FORMAT;

	strncpy(*SQLQueryNoOfRows, SQLQuery, overalllen+2);
	strcat(*SQLQueryNoOfRows, "COUNT(");

	tmp2=tmp1;
	/* szukam pocztku slowa FROM tmp2len bada offset tego co mam wkleic w nawiasy count()*/
	/* przejdz sie po reszczie stringu az do NULL */
	while((*tmp2)!=0) {
		/*printf("loop begin = %c\n", (*tmp2));*/
		if(*tmp2!='F' && *tmp2!='f') {tmp2++; tmp2len++; overalllen++; continue;}
		if(strlen(SQLQuery)-overalllen > 3) { /* mamy jeszcze miejsce na ROM i nie chodzimy za daleko po pamieci */
			if(	   ( *(tmp2+1)=='R' || *(tmp2+1)=='r' )
				&& ( *(tmp2+2)=='O' || *(tmp2+2)=='o' )
				&& ( *(tmp2+3)=='M' || *(tmp2+3)=='m' )
			) {
				/*strncat(*SQLQueryNoOfRows, tmp1+2, tmp2len-3);*/
				strcat(*SQLQueryNoOfRows, "*");	/* musialem to zmienic bo bylo count(a,b) */
				break;
			}
		}
		tmp2++; tmp2len++; overalllen++;
	}
	if(overalllen == (long)strlen(SQLQuery)) return BMD_ERR_PARAM1;

	strcat(*SQLQueryNoOfRows, ") ");
	strcat(*SQLQueryNoOfRows, tmp2);
	return err;
}

long ColumnParamDesc_free(OCIParam **ColumnParamDesc)
{
long err=0;
	if(ColumnParamDesc == NULL)
		return BMD_ERR_PARAM1;

	if(*ColumnParamDesc) {
		OCIDescriptorFree((dvoid *) *ColumnParamDesc, (ub4) OCI_DTYPE_PARAM);
		*ColumnParamDesc = NULL;
	}
	return err;
}

long GetColumnParamDescriptor(void *connection_handle, void *database_result,
			OCIParam **ColumnParamDesc, long oracle_column)
{
	OCI_bmd_db_conn_t *db_conn=NULL;
	OCI_bmd_db_result_t *db_res=NULL;
	OCIBMDHandles_t *OCIBMDHandles	= NULL;//(OCIBMDHandles_t *) connection_handle;
	OCIStmt *SqlStatementHandle	= NULL;//(OCIStmt   *) database_result;
	long err = 0;

	if(connection_handle == NULL)
		return BMD_ERR_PARAM1;
	if(database_result == NULL)
		return BMD_ERR_PARAM2;
	if(ColumnParamDesc==NULL || *ColumnParamDesc!=NULL)
		return BMD_ERR_PARAM3;
	if(oracle_column < 1)
		return BMD_ERR_PARAM4;
	if( (*(long *)connection_handle) != BMD_DBCONN_MAGIC )
		return BMD_ERR_INVALID_PTR;
	if( (*(long *)database_result) != BMD_DBRES_MAGIC )
		return BMD_ERR_INVALID_PTR;

	db_conn=(OCI_bmd_db_conn_t *)connection_handle;
	db_res=(OCI_bmd_db_result_t *)database_result;

	OCIBMDHandles=db_conn->bmdHandles;
	SqlStatementHandle=db_res->oci_res;

	/* pobierz zestaw paramatrow dla odpowiedzi - kolumna oracle_column */
	err = checkerr(OCIBMDHandles->ErrorHandle,
		OCIParamGet(SqlStatementHandle, (ub4) OCI_HTYPE_STMT,
			OCIBMDHandles->ErrorHandle,
			(dvoid **)ColumnParamDesc, (ub4) oracle_column)
	);
	if(err<0) {
		PRINT_DEBUG("LIBBMDOCIERR Error in OCIParamGet. RecievedErrorCode=%i.\n",err);
		if(*ColumnParamDesc) {
			OCIDescriptorFree((dvoid *) *ColumnParamDesc, (ub4) OCI_DTYPE_PARAM);
			*ColumnParamDesc = NULL; }
		return BMD_ERR_OP_FAILED;
	}
	return err;
}

long GetColumnWidth(void *connection_handle, OCIParam *ColumnParamDesc, long oracle_column, ub4 *column_width)
{
	OCI_bmd_db_conn_t *db_conn=NULL;
	OCIBMDHandles_t *OCIBMDHandles	= NULL;//(OCIBMDHandles_t *) connection_handle;
	long err=0;
	long char_semantics		= 0;
	long row_type			= 0;

	if(connection_handle == NULL)
		return BMD_ERR_PARAM1;
	if(ColumnParamDesc==NULL)
		return BMD_ERR_PARAM2;
	if(oracle_column < 1)
		return BMD_ERR_PARAM3;
	if(column_width == NULL || *column_width != 0)
		return BMD_ERR_PARAM4;
	if( (*(long *)connection_handle) != BMD_DBCONN_MAGIC )
		return BMD_ERR_INVALID_PTR;

	db_conn=(OCI_bmd_db_conn_t *)connection_handle;
	OCIBMDHandles=db_conn->bmdHandles;

	char_semantics = 0;
	err = checkerr(OCIBMDHandles->ErrorHandle,
		OCIAttrGet((dvoid*) ColumnParamDesc, (ub4) OCI_DTYPE_PARAM,
			(dvoid*) &char_semantics, (ub4 *) 0,
			(ub4) OCI_ATTR_CHAR_USED,
			(OCIError *) OCIBMDHandles->ErrorHandle)
	);
	if(err<0) {
		PRINT_DEBUG("LIBBMDOCIERR Error in OCIAttrGet OCI_ATTR_CHAR_USED. "
			"RecievedErrorCode=%i.\n", err);
		return BMD_ERR_OP_FAILED;
	}

	/* Czy kolumna nie jest przypadkiem znacznikiem czasu? 					*/
	row_type = 0;
	err = checkerr(OCIBMDHandles->ErrorHandle,
		OCIAttrGet((dvoid*) ColumnParamDesc, (ub4) OCI_DTYPE_PARAM,
			(dvoid*) &row_type, (ub4 *) 0,
			(ub4) OCI_ATTR_DATA_TYPE,
			(OCIError *) OCIBMDHandles->ErrorHandle)
	);
	if(err<0) {
		PRINT_DEBUG("LIBBMDOCIERR Error in OCIAttrGet DATA_TYPE. "
			"RecievedErrorCode=%i.\n", err);
		return BMD_ERR_OP_FAILED;
	}

	/* Pobierz wielkosc kolumny */
	/* Typ TIMESTAMP jest zaliczany do typow binarnych. W zwiazku z tym char_semantics=0 i 	*/
	/* rozmiar pola w bajtach wynosi 11B. Po konwersji na char* w funkcji DefineByPos z 	*/
	/* flaga SQLT_STR string zawiera jednak wiecej niz 11 bajtow - ok. 35 znakow.		*/
	if( (row_type == SQLT_TIMESTAMP_TZ) || ( row_type == SQLT_TIMESTAMP ) ){
		/*PRINT_DEBUG("LIBBMDOCIINF Selected Column has SQLT_TIMESTAMP* type.\n");*/
		*column_width = OCI_TIMESTAMP_TZ_CHAR_WIDTH;
	}
	else {
		/* Jaka jest szerokosc kolumny? */
		*column_width = 0;
		if (char_semantics) {
			/*PRINT_DEBUG("LIBBMDOCIINF Data is char semantics.\n");*/
			err = checkerr(OCIBMDHandles->ErrorHandle,
				OCIAttrGet((dvoid*) ColumnParamDesc, (ub4) OCI_DTYPE_PARAM,
					(dvoid*) column_width, (ub4 *) 0,
					(ub4) OCI_ATTR_CHAR_SIZE,
					(OCIError *) OCIBMDHandles->ErrorHandle)
			);
			if(err<0) {
				PRINT_DEBUG("LIBBMDOCIERR Error in OCIAttrGet CHAR_SIZE. RecievedErrorCode=%i.\n",err);
				return BMD_ERR_OP_FAILED;
			}
		}
		else {
			/*PRINT_DEBUG("LIBBMDOCIINF Data is binary semantics.\n");*/
			err = checkerr(OCIBMDHandles->ErrorHandle,
				OCIAttrGet((dvoid*) ColumnParamDesc, (ub4) OCI_DTYPE_PARAM,
					(dvoid*) column_width, (ub4 *) 0,
					(ub4) OCI_ATTR_DATA_SIZE,
					(OCIError *) OCIBMDHandles->ErrorHandle)
			);
			if(err<0) {
				PRINT_DEBUG("LIBBMDOCIERR Error in OCIAttrGet DATA_SIZE. RecievedErrorCode=%i.\n",err);
				return BMD_ERR_OP_FAILED;
			}
		}
	}
	return err;
}

long GetNumberOfColumns(void *connection_handle, void *database_result, long *no_of_cols)
{
	long err = 0;
	OCI_bmd_db_conn_t *db_conn=NULL;
	OCI_bmd_db_result_t *db_res=NULL;
	OCIBMDHandles_t *OCIBMDHandles	= NULL;//(OCIBMDHandles_t *) connection_handle;
	OCIStmt *SqlStatementHandle	= NULL;//(OCIStmt   *) database_result;

	if(connection_handle == NULL)
		return BMD_ERR_PARAM1;
	if(database_result == NULL)
		return BMD_ERR_PARAM2;
	if(no_of_cols == NULL || *no_of_cols != 0)
		return BMD_ERR_PARAM3;
	if( (*(long *)connection_handle) != BMD_DBCONN_MAGIC )
		return BMD_ERR_INVALID_PTR;
	if( (*(long *)database_result) != BMD_DBRES_MAGIC )
		return BMD_ERR_INVALID_PTR;

	db_conn=(OCI_bmd_db_conn_t *)connection_handle;
	db_res=(OCI_bmd_db_result_t *)database_result;
	OCIBMDHandles=db_conn->bmdHandles;
	SqlStatementHandle=db_res->oci_res;

	err = checkerr(OCIBMDHandles->ErrorHandle,
			OCIAttrGet((dvoid *)SqlStatementHandle,
				(ub4) OCI_HTYPE_STMT,
				(dvoid *) no_of_cols,
				(ub4 *) 0,
				(ub4) OCI_ATTR_PARAM_COUNT,
				(OCIError *) OCIBMDHandles->ErrorHandle)
	);
	return err;
}

long LinkStringBufferWithOutputSQLVariable(void *connection_handle, void *database_result,
					char *string, long string_length, long oracle_column)
{
long err = 0;
OCI_bmd_db_conn_t *db_conn=NULL;
OCI_bmd_db_result_t *db_res=NULL;
OCIBMDHandles_t *OCIBMDHandles	= NULL;//(OCIBMDHandles_t *) connection_handle;
OCIStmt *SqlStatementHandle	= NULL;//(OCIStmt   *) database_result;
OCIDefine *DefineHandle		= (OCIDefine *) NULL;
ub2 rstrlen			= 0;
sb2 indicator			= 0;

	if(connection_handle == NULL)
		return BMD_ERR_PARAM1;
	if(database_result == NULL)
		return BMD_ERR_PARAM2;
	if(string == NULL) /* TODO - to jak wrzuce cos pustego jako aditional metadata to mam nulla w C bo zamallokowal mi 0?*/
		return BMD_ERR_PARAM3;
	if(string_length < 0)
		return BMD_ERR_PARAM4;
	if(oracle_column < 1)
		return BMD_ERR_PARAM5;
	if( (*(long *)connection_handle) != BMD_DBCONN_MAGIC )
		return BMD_ERR_INVALID_PTR;
	if( (*(long *)database_result) != BMD_DBRES_MAGIC )
		return BMD_ERR_INVALID_PTR;

	db_conn=(OCI_bmd_db_conn_t *)connection_handle;
	db_res=(OCI_bmd_db_result_t *)database_result;
	OCIBMDHandles=db_conn->bmdHandles;
	SqlStatementHandle=db_res->oci_res;

	/* Zdefiniuj zaalokowany poprzednio bufor jako zmienna wyjsciowa */
	err = checkerr(OCIBMDHandles->ErrorHandle,
		OCIDefineByPos(SqlStatementHandle, &DefineHandle, OCIBMDHandles->ErrorHandle,
			oracle_column, (dvoid *) string, (sb4) string_length,
			SQLT_STR,
			(dvoid *) &indicator,	/* 1 - NULL, 0-dane, x!=0 - obcieto dane do x 	*/
			(ub2 *) &rstrlen,	/* aktualna dlugosc danych wyjsciowych		*/
			(ub2 *) 0, OCI_DEFAULT)
	);
	if(err<0) {
		PRINT_DEBUG("LIBBMDOCIERR Error in OCIDefineByPos. "
			"RecievedErrorCode=%i.\n", err);
		if (DefineHandle) {
			(void) OCIHandleFree((dvoid *) DefineHandle, (ub4) OCI_HTYPE_DEFINE);
			DefineHandle = NULL; }
		return BMD_ERR_OP_FAILED;
	}
	return err;
}

long FillAllLinkedBuffers(void *connection_handle, void *database_result, EnumDbFetchMode_t fetch_mode, long oracle_row)
{
OCI_bmd_db_conn_t *db_conn=NULL;
OCI_bmd_db_result_t *db_res=NULL;
OCIBMDHandles_t *OCIBMDHandles	= NULL;//(OCIBMDHandles_t *) connection_handle;
OCIStmt *SqlStatementHandle	= NULL;//(OCIStmt   *) database_result;
long err = 0;

	if(connection_handle == NULL)
		return BMD_ERR_PARAM1;
	if(database_result == NULL)
		return BMD_ERR_PARAM2;
	db_conn=(OCI_bmd_db_conn_t *)connection_handle;
	db_res=(OCI_bmd_db_result_t *)database_result;
	OCIBMDHandles=db_conn->bmdHandles;
	SqlStatementHandle=db_res->oci_res;

	/* Wczytaj dane z bazy do bufora */
	if(fetch_mode == FETCH_ABSOLUTE) {
		PRINT_DEBUG("LIBBMDOCIINF Using absolute fetching.\n");
		err = checkerr(OCIBMDHandles->ErrorHandle,
			OCIStmtFetch2(SqlStatementHandle, OCIBMDHandles->ErrorHandle, (ub4) 1,
				OCI_FETCH_ABSOLUTE, (sb4) oracle_row, OCI_DEFAULT));
	}
	else {
		/*PRINT_DEBUG("LIBBMDOCIINF Using sequence fetching.\n");*/
		err = checkerr(OCIBMDHandles->ErrorHandle,
			OCIStmtFetch2(SqlStatementHandle, OCIBMDHandles->ErrorHandle, (ub4) 1,
				OCI_FETCH_NEXT, (sb4) 0, OCI_DEFAULT));
	}
	if(err<0) {
		if(err == BMD_ERR_NODATA) {
			PRINT_DEBUG("LIBBMDOCIINF OCIStmtFetch2 returned no data. "
				"RecievedErrorCode=%i.\n", err);
		}
		else {
			PRINT_DEBUG("LIBBMDOCIERR Error in OCIStmtFetch2. "
				"RecievedErrorCode=%i.\n", err);
		}
		return err;
	}
	return err;
}
