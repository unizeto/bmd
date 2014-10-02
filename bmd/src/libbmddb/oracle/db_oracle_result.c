#ifdef WIN32
#pragma warning(disable:4206)
#pragma warning(disable:4100)
#endif

#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>
#include <bmd/libbmddb/oracle/libbmddb_oracle.h>

/*****
* Funcja czysci deskryptor zawierajacy rezultat zapytania zlozonego do bazy.
*/
long oracle_bmd_db_result_free(void **database_result)
{
	long err = 0;
	OCI_bmd_db_result_t *db_res=NULL;

	PRINT_ERROR("LIBBMDOCIINF Start clearing database_result.\n");
	if(database_result == NULL)
		return BMD_ERR_PARAM1;	
	if((*database_result) == NULL )
		return BMD_ERR_PARAM1;
	if( (*(long *)(database_result)) == BMD_DBRES_MAGIC )
		return BMD_ERR_INVALID_PTR;
	if( (*(long *)(*database_result)) != BMD_DBRES_MAGIC )
		return BMD_ERR_INVALID_PTR;

	db_res=(OCI_bmd_db_result_t *)(*database_result);

	(void) OCIHandleFree((dvoid *)db_res->oci_res, (ub4) OCI_HTYPE_STMT);
	PRINT_ERROR("LIBBMDOCIINF SQL statement handle cleared succesfully.\n");
	free0(*database_result);

	return err;
}

/*****
* WYKONAYWANE ZADANIA:
* Funkcja pobiera rezultat z wykonanego za pomc oracle_bmd_db_execute_sql() zapytania SQL.
* Rezultat jest domylnie konwertowany do typu string i umieszczany w zaaalokowanym
* wewntrz funkcji buforze.
* TRYBY PRACY:
* fetch_mode==FETCH_NEXT.
* Dostep do wierszy jest tu sekwencyjny i nalezy wywolywac ta funkcje az do otrzymania
* kodu bledu BMD_ERR_NODATA sygnalizujacego dotarcie do konca listy rezultatow.
* ARGUMENTY:
* connection_handle - uchwyt polaczenia z baza
* database result - rezultat funkcji oracle_bmd_db_execute_sql
* which_row, which_column - kolumna i wiersz z kt�ego ma zostac pobrany string (w przypadku
* trybu FETCH_NEXT argument which_row jest ignorowany,
* string - wskaznik do wskaznika pod ktorym bedzie umieszczony string z rezultatem
* fetch_mode - tryb pracy: FETCH_NEXT, FETCH_ABSOLUTE.
* successfully_fetched - zmienna pod ktora funkcja zwroci numer ostatnio pobranego prawidlowo wiersza rezultatu
* !!!zmienna ta przed pierwsym wywolaniem oracle_bmd_db_result_get_value MUSI BYC ROWNA 0!!!
* ZWRACANE WARTOSCI:
* long - kod bledu - patrz libbmddb_oracle.h
*/
long oracle_bmd_db_result_get_value(void *connection_handle, void *database_result,
			long which_row, long which_column, char **string,
			EnumDbFetchMode_t fetch_mode, long *successfully_fetched)
{
	OCI_bmd_db_conn_t *db_conn=NULL;
	OCI_bmd_db_result_t *db_res=NULL;

	OCIParam *ColumnParamDesc 	= (OCIParam *) 0;		/* parametry kolumny */
	long oracle_row			= which_row+1;
	long oracle_column		= which_column+1;

	ub4 col_width 			= 0;
	long err				= 0;
	ub4 StatementType=0;

	if(connection_handle == NULL)
		return BMD_ERR_PARAM1;
	if(database_result == NULL)
		return BMD_ERR_PARAM2;
	if(which_row<0)
		return BMD_ERR_PARAM3;
	if(which_column<0)
		return BMD_ERR_PARAM4;
	if(string == NULL || *string != NULL)
		return BMD_ERR_PARAM5;
	if(successfully_fetched == NULL || *successfully_fetched < 0)
		return BMD_ERR_PARAM6;

	db_conn=(OCI_bmd_db_conn_t *)connection_handle;
	db_res=(OCI_bmd_db_result_t *)database_result;

	/* sprawdzenie czy typu zapytania - kontynuacja gdy jest to OCI_STMT_SELECT */
	err = checkerr(db_conn->bmdHandles->ErrorHandle,
				   OCIAttrGet((dvoid *)db_res->oci_res,
				(ub4) OCI_HTYPE_STMT,
				(dvoid *) &StatementType,
				(ub4 *) 0,
				(ub4) OCI_ATTR_STMT_TYPE,
				(OCIError *) db_conn->bmdHandles->ErrorHandle)
	);
	if( err < 0 )
	{
		PRINT_ERROR("ERROR WHILE DETERMINING Statement type\n");
		return BMD_ERR_OP_FAILED;
	}
	if( StatementType != OCI_STMT_SELECT )
	{
		return BMD_OK; /* nie ma co pobierac z innych zapytan */
	}
	/* pobierz zestaw paramatrow dla odpowiedzi - kolumna = oracle_column */
	err = GetColumnParamDescriptor(connection_handle, database_result, &ColumnParamDesc, oracle_column);
	if(err<0){
		PRINT_ERROR("LIBBMDOCIERR Error in getting column parameters descriptor. "
			"RecievedErrorCode=%li.\n", err);
		ColumnParamDesc_free(&ColumnParamDesc);
		return BMD_ERR_OP_FAILED;
	}

	/* ustal rozmiar kolumny */
	err = GetColumnWidth(connection_handle, ColumnParamDesc, oracle_column, &col_width);
	if(err<0){
		PRINT_ERROR("LIBBMDOCIERR Error in getting column width. "
			"RecievedErrorCode=%li.\n", err);
		ColumnParamDesc_free(&ColumnParamDesc);
		return BMD_ERR_OP_FAILED;
	}

	/* utworz bufor */
	/*PRINT_ERROR("LIBBMDOCIINF Fetched column width = %li.\n", col_width);*/
	*string = (char *) malloc(sizeof(char) * (col_width+1));
	if(*string==NULL) {
		PRINT_ERROR("LIBBMDOCIERR No memory for dynamic allocation. "
			"RecievedErrorCode=%li.\n", err);
		ColumnParamDesc_free(&ColumnParamDesc);
		return NO_MEMORY;
	}
	memset(*string, 0, col_width+1);

	/* podepnij bufor do zmienej wyjsciowej SQL */
	err = LinkStringBufferWithOutputSQLVariable(connection_handle, database_result, *string, col_width+1, oracle_column);
	if(err<0){
		PRINT_ERROR("LIBBMDOCIERR No memory for dynamic allocation. "
			"RecievedErrorCode=%li.\n", err);
		ColumnParamDesc_free(&ColumnParamDesc);
		return BMD_ERR_MEMORY;
	}

	/* wypelnij bufor zmiennymi wyjsciowymi SQL */
	err = FillAllLinkedBuffers(connection_handle, database_result, fetch_mode, oracle_row);
	if(err<0){
		PRINT_ERROR("LIBBMDOCIERR Error in filling all linked buffers. RecievedErrorCode=%li.\n", err);
		ColumnParamDesc_free(&ColumnParamDesc);
		return err;
	}
	/*PRINT_ERROR("LIBBMDOCIINF Po OCIStmtFetch2 buf={%s}\n", *string);*/

	/* zaktualizuj liczniki */
	if(fetch_mode == FETCH_ABSOLUTE)	(*successfully_fetched) = oracle_row;
	else					(*successfully_fetched)++;

	ColumnParamDesc_free(&ColumnParamDesc);
	PRINT_ERROR("LIBBMDOCIINF String successfully got from result.\n");
	return err;
}


long oracle_bmd_db_result_get_row(void *connection_handle,
			void *database_result,
			long which_row, long max_columns,
			db_row_strings_t **rowstruct,
			EnumDbFetchMode_t fetch_mode,
			long *successfully_fetched)
{
OCIParam *ColumnParamDesc 	= (OCIParam *) 0;

long numcols			= 0;
long oracle_row			= which_row + 1;
long which_row_next		= 0;
long colsreturned		= 0;
long colsprocess			= 0;
long err				= 0;

ub4 col_width			= 0;
long i				= 0;

	if(connection_handle == NULL)
		return BMD_ERR_PARAM1;
	if(database_result == NULL)
		return BMD_ERR_PARAM2;
	if(which_row < 0)
		return BMD_ERR_PARAM3;
	if(max_columns < 0)
		return BMD_ERR_PARAM4;
	if(*rowstruct != NULL || rowstruct == NULL)
		return BMD_ERR_PARAM5;

	err = GetNumberOfColumns(connection_handle, database_result, &numcols);
	if(err<0) {
		PRINT_ERROR("LIBBMDOCIERR Error in getting number of columns. "
			"RecievedErrorCode=%li.\n",err);
		return BMD_ERR_OP_FAILED;
	}
	*rowstruct = (db_row_strings_t *) malloc ( sizeof(db_row_strings_t) );
	if(*rowstruct == NULL) return NO_MEMORY;

	colsreturned = numcols;
	if(max_columns <= 0)
		colsprocess = colsreturned;
	else
		colsprocess = (colsreturned < max_columns)? colsreturned:max_columns;

	/* przetwarzam mniejsza wartosc z colsreturned i max_columns */
	(*rowstruct)->size = colsprocess;
	(*rowstruct)->colvals = (char **) malloc (sizeof(char *) * colsprocess);
	if((*rowstruct)->colvals == NULL) return NO_MEMORY;
	for(i=0; i<colsprocess; i++) (*rowstruct)->colvals[i] = NULL;


	switch(fetch_mode) {
	case FETCH_ABSOLUTE:
		which_row_next = oracle_row;
		break;
	case FETCH_NEXT:
		which_row_next = (*successfully_fetched)+1;
		break;
	default:
		break;
	}

	/* Wypelnij wszystkie stringi z wiersza czyli przejdz sie po kolumnach	*/
	for(i=0; i<colsprocess; i++)
	{
		/* pobierz zestaw paramatrow dla odpowiedzi - kolumna oracle_column	*/
		/* indeks i liczy w konwencji 1..n a nie jak w "C" 0..n-1		*/
		err = GetColumnParamDescriptor(connection_handle, database_result, &ColumnParamDesc, i+1);
		if(err<0){
			PRINT_ERROR("LIBBMDOCIERR Error in getting column parameters descriptor. RecievedErrorCode=%li.\n", err);
			ColumnParamDesc_free(&ColumnParamDesc);
			return BMD_ERR_OP_FAILED;
		}

		/* ustal rozmiar kolumny */
		col_width=0;
		err = GetColumnWidth(connection_handle, ColumnParamDesc, i+1, &col_width);
		if(err<0){
			PRINT_ERROR("LIBBMDOCIERR Error in getting column width. RecievedErrorCode=%li.\n", err);
			ColumnParamDesc_free(&ColumnParamDesc);
			return BMD_ERR_OP_FAILED;
		}

		/* utworz bufor */
		/*PRINT_ERROR("LIBBMDOCIINF Fetched column width = %li.\n", col_width);*/
		(*rowstruct)->colvals[i] = (char *) malloc(sizeof(char) * (col_width+1));
		if((*rowstruct)->colvals[i]==NULL) {
			PRINT_ERROR("LIBBMDOCIERR[%s:%li:oracle_bmd_db_result_get_value] No memory for dynamic allocation. RecievedErrorCode=%li.\n", __FILE__, __LINE__, err);
			ColumnParamDesc_free(&ColumnParamDesc);
			return NO_MEMORY;
		}
		memset((*rowstruct)->colvals[i], 0, col_width+1);

		/* podepnij bufor do zmienej wyjsciowej SQL */
		err = LinkStringBufferWithOutputSQLVariable(connection_handle, database_result, (*rowstruct)->colvals[i], col_width+1, i+1);
		if(err<0){
			PRINT_ERROR("LIBBMDOCIERR[%s:%li:oracle_bmd_db_result_get_value] No memory for dynamic allocation. RecievedErrorCode=%li.\n", __FILE__, __LINE__, err);
			ColumnParamDesc_free(&ColumnParamDesc);
			return BMD_ERR_MEMORY;
		}
		ColumnParamDesc_free(&ColumnParamDesc);
	}

	/* A teraz pobierz rezultaty zapytania SQL do bufora */
	/* Wczytaj dane z bazy do bufora */
	/* wypelnij bufor zmiennymi wyjsciowymi SQL */
	err = FillAllLinkedBuffers(connection_handle, database_result, fetch_mode, oracle_row);
	if(err<0){
		PRINT_ERROR("LIBBMDOCIERR Error in filling all linked buffers. RecievedErrorCode=%li.\n", err);
		ColumnParamDesc_free(&ColumnParamDesc);
		return err;
	}


	if(fetch_mode == FETCH_ABSOLUTE)	(*successfully_fetched) = oracle_row;
	else					(*successfully_fetched)++;

	ColumnParamDesc_free(&ColumnParamDesc);
	PRINT_ERROR("LIBBMDOCIINF Row of strings successfully got from result.\n");
	return err;
}

long oracle_bmd_db_row_struct_free(db_row_strings_t **rowstruct)
{
long err = 0;
long i = 0;

	if(rowstruct == NULL || *rowstruct==NULL)
		return BMD_ERR_PARAM1;

	for(i=0; i<(*rowstruct)->size; i++){

		if((*rowstruct)->colvals[i]){
			free((*rowstruct)->colvals[i]); (*rowstruct)->colvals[i] = NULL;
		}
	}
	if((*rowstruct)->colvals){ free((*rowstruct)->colvals); (*rowstruct)->colvals = NULL; }
	if(*rowstruct) {free(*rowstruct); *rowstruct = NULL;}
	return err;
}
