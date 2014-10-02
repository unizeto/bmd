#ifdef WIN32
#pragma warning(disable:4206)
#pragma warning(disable:4100)
#endif

#include <bmd/libbmddb/db2/db_libbmddb.h>
#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>

#define SBT(hDB,db_conn) if( db_conn->transaction_present != 1 ) \
						 { \
							long s=0; \
							s=db2_bmd_db_start_transaction(hDB,BMD_TRANSACTION_LEVEL_NONE); \
							if( s != BMD_OK ) \
							{ \
								PRINT_ERROR("Cannot start transaction for BLOB operation\n"); \
								return BMD_ERR_FORMAT; \
							} \
							else \
							{ \
								db_conn->transaction_for_blob=1; \
							} \
						 }

#define RBT(hDB,db_conn) if( db_conn->transaction_for_blob == 1 ) \
		  		         { \
							db2_bmd_db_rollback_transaction(hDB,NULL); \
							db_conn->transaction_for_blob=0;\
						 }

#define EBT(hDB,db_conn) if( db_conn->transaction_for_blob == 1 ) \
		  		         { \
							db2_bmd_db_end_transaction(hDB); \
							db_conn->transaction_for_blob=0;\
						 }

/* Funkcja pobiera GenBuf do wyslania i umieszczenia w bazie jako LO	*/
/* zwraca kod bledu a w zmiennej blob_number OID(db2owy) nowo wstawionego pliku */
/* dla kodu skompilowanego z opcj� BLOB_IN_BYTEA funkcja zwraca */
/* w blob_number identyfikator obiektu binarnego w tabeli tblob */
long db2_bmd_db_export_blob(void *hDB,GenBuf_t *dataGenBuf,char **blob_number)
{

	DB_bmd_db_conn_t *db_conn=NULL;
		long err=0;
	#ifndef BLOB_IN_BYTEA
		long lobj_fd=0,tmp=0;
		Oid lobjId ;
	#else
		char *SQLQuery			= NULL;
		char *c_tmp				= NULL;
		long fetched_counter = 0;
		long rows = 0;
		long cols = 0;
		//Oid in_oid[]={0,17}; /* varchar, bytea */
		char* params[]={NULL,NULL};	//{chunk_size,bobj};
		int params_length[2]={0,0};	//nalezy ustawic rozmiary przynajmniej dla danych nie tekstowych
		//int params_format[2]={0,1}; /* text,binary */
		void *bmd_db_res	= NULL;
		SQLRETURN     rc;
	#endif

		PRINT_INFO("LIBBMDPGINF Exporting file to LO\n");

		if (hDB==NULL)
			return BMD_ERR_PARAM1;
		if (dataGenBuf == NULL)
			{	BMD_FOK(BMD_ERR_PARAM2);	}
		if (blob_number == NULL)
			{	BMD_FOK(BMD_ERR_PARAM3);	}
		if ((*blob_number) != NULL)
			{	BMD_FOK(BMD_ERR_PARAM3);	}
		if( (*((long *)hDB)) != BMD_DBCONN_MAGIC )
			return BMD_ERR_INVALID_PTR;

		db_conn=(DB_bmd_db_conn_t *)hDB;
		if( db_conn->hdbc == NULL )
			return BMD_ERR_NODATA;

		SBT(hDB,db_conn);
		/*
		 * Utworz large object.
		 */
	#ifndef BLOB_IN_BYTEA
		//lobjId = lo_creat(db_conn->pg_conn, INV_READ | INV_WRITE);
		if( lobjId == 0)
		{
			RBT(hDB,db_conn);
			PRINT_ERROR("LIBBMDPGERR Error in creating large object - lo_creat.\n");
			return BMD_ERR_OP_FAILED;
		}

		//lobj_fd = lo_open(db_conn->pg_conn, lobjId, INV_WRITE);
		//if(lobj_fd == -1)
		{
			RBT(hDB,db_conn);
			BMD_FOK(BMD_ERR_OP_FAILED);
		}

		//tmp = lo_write(db_conn->pg_conn, lobj_fd, (char *)dataGenBuf->buf, dataGenBuf->size);
		if(tmp < dataGenBuf->size)
		{
			RBT(hDB,db_conn);
			BMD_FOK(BMD_ERR_OP_FAILED);
		}

		//(void) lo_close(db_conn->pg_conn, lobj_fd);
		asprintf(blob_number, "%li", (long)lobjId);
	#else
		/* pobierz sekwencj� numeru obiektu binarnego lobjId (blob_number)*/
		asprintf(&SQLQuery, "VALUES NEXT VALUE FOR co_raster_id");
		 ;
		if(SQLQuery == NULL) return NO_MEMORY;
		err = db2_bmd_db_execute_sql(	hDB, SQLQuery,
											&rows, &cols, (void **)&bmd_db_res);
		/**if(SQLQuery != NULL) */{ free(SQLQuery); SQLQuery=NULL; }
		if(err<BMD_OK)
		{
			PRINT_ERROR("LIBBMDSQLERR Error in while execute VALUES NEXT VALUE FOR co_raster_id\n");
			db2_bmd_db_rollback_transaction(hDB, NULL);
			if(SQLQuery != NULL) { free(SQLQuery); SQLQuery=NULL; }
			return err;
		}
		err=db2_bmd_db_result_get_value(hDB,(void *)bmd_db_res,0,0,&c_tmp,FETCH_ABSOLUTE,&fetched_counter);
		if(err<BMD_OK)
		{
			PRINT_ERROR("LIBBMDSQLERR Error while getting value\n");
			db2_bmd_db_rollback_transaction(hDB, NULL);
			db2_bmd_db_result_free((void**)&bmd_db_res);
			if(SQLQuery != NULL) { free(SQLQuery); SQLQuery=NULL; }
			return err;
		}
		db2_bmd_db_result_free((void**)&bmd_db_res);
		bmd_db_res = NULL;

		//wlasciwy etap zapisu
		params[0] = (char *)c_tmp;
		params[1] = (char *) dataGenBuf->buf;

		params_length[0] = (int)strlen(c_tmp);
		params_length[1] = dataGenBuf->size;

		db_conn = (DB_bmd_db_conn_t *) hDB;
		SQLCHAR *stmt = ( SQLCHAR * )"INSERT INTO tblob(fk_co_raster_id,bobj) VALUES (?, ?)";
				//sizeof(params)/sizeof(params[0]),
				//in_oid, (const char *const * )params, (const int*)params_length,
				//(const int*)params_format, 1

		rc = SQLAllocHandle(SQL_HANDLE_STMT, *(db_conn->hdbc), db_conn->hstmt);
			if (rc != SQL_SUCCESS)
			{
				PRINT_ERROR("Error while esec sql query: INSERT INTO tblob...\n");
				extract_error("SQLAllocHandle", *(db_conn->hstmt), SQL_HANDLE_STMT);
				db2_bmd_db_rollback_transaction(hDB, NULL);
				return BMD_DB_EXECUTION_ERROR;
			}

		rc = SQLBindParameter(*(db_conn->hstmt), 1, SQL_PARAM_INPUT, SQL_C_CHAR,
		                             SQL_VARCHAR, params_length[0], 0, params[0], params_length[0], NULL);
		if (rc != SQL_SUCCESS)
					{
						PRINT_ERROR(">--- Error while SQLBindParameter, (SQL_VARCHAR)\n");
						extract_error("SQLAllocHandle", *(db_conn->hstmt), SQL_HANDLE_STMT);
						db2_bmd_db_rollback_transaction(hDB, NULL);
						return BMD_DB_EXECUTION_ERROR;
					}
		rc = SQLBindParameter(*(db_conn->hstmt), 2, SQL_PARAM_INPUT, SQL_C_BINARY,
					SQL_BLOB, params_length[1], 0, params[1], params_length[1], NULL);
		if (rc != SQL_SUCCESS)
					{
						PRINT_ERROR(">--- Error while SQLBindParameter, (SQL_BLOB)\n");
						extract_error("SQLAllocHandle", *(db_conn->hstmt), SQL_HANDLE_STMT);
						db2_bmd_db_rollback_transaction(hDB, NULL);
						return BMD_DB_EXECUTION_ERROR;
					}
		rc = SQLExecDirect(*(db_conn->hstmt), stmt, SQL_NTS);
		if (rc != SQL_SUCCESS)
					{
						PRINT_ERROR("Driver reported the following diagnostics: ");
						extract_error("SQLAllocHandle", *(db_conn->hstmt), SQL_HANDLE_STMT);
						db2_bmd_db_rollback_transaction(hDB, NULL);
						return BMD_DB_EXECUTION_ERROR;
					}

		*blob_number = c_tmp;
		c_tmp = NULL;	//przekazywane na zewnatrz funkcji
		params[0] = NULL;
		if (rc == SQL_SUCCESS)

		{
			/* success */
			err = BMD_OK;
		}
		else
		{
			PRINT_ERROR("LIBBMDPINFO DBresultStatus(pq_db_res)\n");
			db2_bmd_db_rollback_transaction(hDB, NULL);
			BMD_FOK(BMD_ERR_NODATA);
		}

	#endif
		EBT(hDB,db_conn);

		return err;

}

/* Funkcja wywolywana przed sekwencja callbackow zapisujacych kolejne
 * fragmenty pliku do bazy. Nastepnie towrzony jest pusty LOB i zwracany
 * jego deskryptor (nie identyfikator!!!) do zmiennej blob_locator, ktory nalezy
 * pozniej podawac jako argument callbacka by baza wiedziala do jakiego LOBa zapisywac dane.
 * Deklaracja blob_locator: void *bl=NULL; i podajemy do funkcji jako &bl.
 */
long db2_bmd_db_export_blob_begin(void *hDB,char **blob_number_str,void **blob_locator)
{

	return -1;
}

/* Funkcja jest pojedyncza iteracja zapisujaca dane do LOBa.
 * Dane do zapisu powinny byc umieszczone w buforze generycznym chunkGenBuf.
 * Dodatkowo nalezy podac deskryptor LOBa blob_fd (nie identyfikator!!!).
 */
long db2_bmd_db_export_blob_continue(void *hDB,GenBuf_t *chunkGenBuf,void *blob_locator,EnumDbWhichPiece_t which_piece)
{

	return -1;
}

/* Funkcja zamyka BLOBa dodtakowo zwalniany i nullowany jest blob_locator dlatego podajemy go jako &bl.
 */
long db2_bmd_db_export_blob_end(void *hDB,void **blob_locator)
{

	return -1;
}

long db2_bmd_db_import_blob(void *hDB,char *blob_number,GenBuf_t **dataGenBuf)
{


	return -1;
}

/* Funkcja inicjalizuje operacje pobierania zawartsci bloba o identyfikatorze wskazanym przez blob_number.
 * zwracany jest rozmiar pliku, ktory bedzie importowany z bazy i jego lokator oraz zaalokowany wewnatrz
 * funkcji GenBuf z zaalokowanym buforem o dlugosci LARGE_FILE_DB_CHUNK_SIZE, ktory bedzie podawany jako
 * argument do funkcji BFiterateImportFile. Parametr with_transaction == WITH_TRANSACTION wlacza dodatkowo
 * transakcje.
 */
long db2_bmd_db_import_blob_begin(void *hDB,char *blob_number,long *blob_size,void **blob_locator,GenBuf_t **dataGenBuf)
{

	return -1;
}

long db2_bmd_db_import_blob_continue(void *hDB,GenBuf_t *chunkGenBuf,void *blob_locator,EnumDbWhichPiece_t which_piece)
{

	return -1;
}

long db2_bmd_db_import_blob_end(void *hDB,void **blob_locator,GenBuf_t **dataGenBuf)
{

	return -1;
}

long db2_bmd_db_delete_blob(void *hDB,char *blob_number)
{

		return -1;
}
