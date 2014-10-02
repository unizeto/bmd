#ifdef WIN32
#pragma warning(disable:4206)
#pragma warning(disable:4100)
#endif

#include <bmd/libbmddb/postgres/pq_libbmddb.h>
#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>

#define SBT(hDB,db_conn) if( db_conn->transaction_present != 1 ) \
						 { \
							long s=0; \
							s=postgres_bmd_db_start_transaction(hDB,BMD_TRANSACTION_LEVEL_NONE); \
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
							postgres_bmd_db_rollback_transaction(hDB,NULL); \
							db_conn->transaction_for_blob=0;\
						 }

#define EBT(hDB,db_conn) if( db_conn->transaction_for_blob == 1 ) \
		  		         { \
							postgres_bmd_db_end_transaction(hDB); \
							db_conn->transaction_for_blob=0;\
						 }

/* Funkcja pobiera GenBuf do wyslania i umieszczenia w bazie jako LO	*/
/* zwraca kod bledu a w zmiennej blob_number OID(postgresowy) nowo wstawionego pliku */
/* dla kodu skompilowanego z opcj� BLOB_IN_BYTEA funkcja zwraca */
/* w blob_number identyfikator obiektu binarnego w tabeli tblob */
long postgres_bmd_db_export_blob(void *hDB,GenBuf_t *dataGenBuf,char **blob_number)
{
	printf("postgres_bmd_db_export_blob\n");

	PQ_bmd_db_conn_t *db_conn=NULL;
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
	Oid in_oid[]={0,17}; /* varchar, bytea */ 
	char* params[]={NULL,NULL};	//{chunk_size,bobj};
	int params_length[2]={0,0};	//nalezy ustawic rozmiary przynajmniej dla danych nie tekstowych 
	int params_format[2]={0,1}; /* text,binary */
	PQ_bmd_db_result_t *bmd_db_res	= NULL;
	PGresult *pq_db_res				= NULL;
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

	db_conn=(PQ_bmd_db_conn_t *)hDB;
	if( db_conn->pg_conn == NULL )
		return BMD_ERR_NODATA;

	SBT(hDB,db_conn);
	/*
	 * Utworz large object.
	 */
#ifndef BLOB_IN_BYTEA
	lobjId = lo_creat(db_conn->pg_conn, INV_READ | INV_WRITE);
	if( lobjId == 0)
	{
		RBT(hDB,db_conn);
		PRINT_ERROR("LIBBMDPGERR Error in creating large object - lo_creat.\n");
		return BMD_ERR_OP_FAILED;
	}

	lobj_fd = lo_open(db_conn->pg_conn, lobjId, INV_WRITE);
	if(lobj_fd == -1)
	{
		RBT(hDB,db_conn);
		BMD_FOK(BMD_ERR_OP_FAILED);
	}

	tmp = lo_write(db_conn->pg_conn, lobj_fd, (char *)dataGenBuf->buf, dataGenBuf->size);
	if(tmp < dataGenBuf->size)
	{
		RBT(hDB,db_conn);
		BMD_FOK(BMD_ERR_OP_FAILED);
	}

	(void) lo_close(db_conn->pg_conn, lobj_fd);
	asprintf(blob_number, "%li", (long)lobjId);
#else
	/* pobierz sekwencj� numeru obiektu binarnego lobjId (blob_number)*/
	asprintf(&SQLQuery, 	"SELECT nextval('co_raster_id');");
	if(SQLQuery == NULL) return NO_MEMORY;
	err = postgres_bmd_db_execute_sql(	hDB, SQLQuery, 
										&rows, &cols, (void **)&bmd_db_res);
	/**if(SQLQuery != NULL) */{ free(SQLQuery); SQLQuery=NULL; }
	if(err<BMD_OK)
	{
		PRINT_ERROR("LIBBMDSQLERR Error in inserting CO. Error=%li\n", err);
		if(SQLQuery != NULL) { free(SQLQuery); SQLQuery=NULL; }
		return err;
	}
	err=postgres_bmd_db_result_get_value(hDB,(void *)bmd_db_res,0,0,&c_tmp,FETCH_ABSOLUTE,&fetched_counter);
	postgres_bmd_db_result_free((void**)&bmd_db_res);
	bmd_db_res = NULL;
	BMD_FOK(err);

	//wlasciwy etap zapisu
	params[0] = (char *)c_tmp;
	params[1] = (char *) dataGenBuf->buf;

	params_length[0] = (int)strlen(c_tmp);
	params_length[1] = dataGenBuf->size;

	db_conn = (PQ_bmd_db_conn_t *) hDB;
	pq_db_res = PQexecParams(db_conn->pg_conn,   
							"INSERT INTO tblob(fk_co_raster_id,bobj) VALUES (CAST( $1 AS bigint),$2::bytea);",
							sizeof(params)/sizeof(params[0]), 
							in_oid, (const char *const * )params, (const int*)params_length, 
							(const int*)params_format, 1);
	*blob_number = c_tmp;
	c_tmp = NULL;	//przekazywane na zewnatrz funkcji
	params[0] = NULL;
	if (pq_db_res && PQresultStatus(pq_db_res)==PGRES_COMMAND_OK) 
	{ 
		/* success */
		err = BMD_OK;
	}
	else
	{
		PRINT_ERROR("LIBBMDPINFO PQresultStatus(pq_db_res) = %i\n", PQresultStatus(pq_db_res));
		if (pq_db_res){PQclear(pq_db_res);pq_db_res = NULL;}
		BMD_FOK(BMD_ERR_NODATA);
	}
	if (pq_db_res){PQclear(pq_db_res);pq_db_res = NULL;}

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
long postgres_bmd_db_export_blob_begin(void *hDB,char **blob_number_str,void **blob_locator)
{
	printf("postgres_bmd_db_export_blob_begin\n");

	PQ_bmd_db_conn_t *db_conn	=NULL;
	int err 				= 0;
	long int blob_number		= 0;
	Oid *lobjId 			= (Oid *) &blob_number;
	int **blob_fd			= (int **) blob_locator;
	int deskryptor = 0;

	if(hDB==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(	blob_locator == NULL ||
		*blob_locator != NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(blob_number_str == NULL)			{	BMD_FOK(BMD_ERR_PARAM3);	}
	if( (*((long *)hDB)) != BMD_DBCONN_MAGIC )
		return BMD_ERR_INVALID_PTR;

	db_conn=(PQ_bmd_db_conn_t *)hDB;
	if( db_conn->pg_conn == NULL )
		return BMD_ERR_NODATA;
#ifdef BLOB_IN_BYTEA
	PRINT_ERROR("LIBBMDPERR postgres_bmd_db_export_blob_begin not implemented for BLOB_IN_BYTEA\n");
#endif
	SBT(hDB,db_conn);

	*blob_fd = (int *) malloc (sizeof(int));

	/* Utworz large object. */
	*lobjId = lo_creat(db_conn->pg_conn, INV_READ | INV_WRITE);
	if (*lobjId == 0)
	{
		RBT(hDB,db_conn);
		BMD_FOK(BMD_ERR_OP_FAILED);
	}

	deskryptor = lo_open(db_conn->pg_conn, *lobjId, INV_WRITE);
	**blob_fd = deskryptor;
	if(**blob_fd == -1)
	{
		RBT(hDB,db_conn);
		BMD_FOK(BMD_ERR_OP_FAILED);
	}

	asprintf(blob_number_str, "%li", blob_number);

	return err;
}

/* Funkcja jest pojedyncza iteracja zapisujaca dane do LOBa.
 * Dane do zapisu powinny byc umieszczone w buforze generycznym chunkGenBuf.
 * Dodatkowo nalezy podac deskryptor LOBa blob_fd (nie identyfikator!!!).
 */
long postgres_bmd_db_export_blob_continue(void *hDB,GenBuf_t *chunkGenBuf,void *blob_locator,EnumDbWhichPiece_t which_piece)
{
	printf("postgres_bmd_db_export_blob_continue\n");

	PQ_bmd_db_conn_t *db_conn=NULL;
	long err 			= 0;
	long *blob_fd		= (long *) blob_locator;

	if(hDB==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(chunkGenBuf == NULL)				{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(blob_locator == NULL)			{       BMD_FOK(BMD_ERR_PARAM3);    }
	if( *((long *)blob_locator) < 0)		{	BMD_FOK(BMD_ERR_PARAM3);	}
	if(which_piece < 0)				{	BMD_FOK(BMD_ERR_PARAM4);	}

	if( (*((long *)hDB)) != BMD_DBCONN_MAGIC )
		return BMD_ERR_INVALID_PTR;

	db_conn=(PQ_bmd_db_conn_t *)hDB;
	if( db_conn->pg_conn == NULL )
		return BMD_ERR_NODATA;
	if( db_conn->transaction_present != 1 )
	{
		PRINT_ERROR("Transaction not started\n");
		return BMD_ERR_FORMAT;
	}
#ifdef BLOB_IN_BYTEA
	PRINT_ERROR("LIBBMDPERR postgres_bmd_db_export_blob_continue not implemented for BLOB_IN_BYTEA\n");
#endif

	err = lo_write(db_conn->pg_conn, *blob_fd, (char *)chunkGenBuf->buf, chunkGenBuf->size);
	if(err < chunkGenBuf->size)
	{
		RBT(hDB,db_conn);
		BMD_FOK(BMD_ERR_OP_FAILED);
	}

	return err;
}

/* Funkcja zamyka BLOBa dodtakowo zwalniany i nullowany jest blob_locator dlatego podajemy go jako &bl.
 */
long postgres_bmd_db_export_blob_end(void *hDB,void **blob_locator)
{
	printf("postgres_bmd_db_export_blob_end\n");

	PQ_bmd_db_conn_t *db_conn=NULL;
	long err 			= 0;
	long **blob_fd			= (long **) blob_locator;

	if(hDB==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(blob_locator == NULL)			{       BMD_FOK(BMD_ERR_PARAM2);			}
	if(*blob_locator < 0)				{	BMD_FOK(BMD_ERR_PARAM2);			}

	if( (*((long *)hDB)) != BMD_DBCONN_MAGIC )
		return BMD_ERR_INVALID_PTR;

	db_conn=(PQ_bmd_db_conn_t *)hDB;
	if( db_conn->pg_conn == NULL )
		return BMD_ERR_NODATA;
	if( db_conn->transaction_present != 1 )
	{
		PRINT_ERROR("Transaction not started\n");
		return BMD_ERR_FORMAT;
	}
#ifdef BLOB_IN_BYTEA
	PRINT_ERROR("LIBBMDPERR postgres_bmd_db_export_blob_end not implemented for BLOB_IN_BYTEA\n");
#endif
	(void) lo_close(db_conn->pg_conn, **blob_fd);
	if(*blob_fd) {free(*blob_fd); *blob_fd=NULL;}

	EBT(hDB,db_conn);

	return err;
}

long postgres_bmd_db_import_blob(void *hDB,char *blob_number,GenBuf_t **dataGenBuf)
{
	printf("postgres_bmd_db_import_blob\n");

	PQ_bmd_db_conn_t *db_conn=NULL;

#ifndef BLOB_IN_BYTEA
	long lobj_fd		= 0;
	long nbytes			= 0;
	long lobj_size		= 0;
#else
	char *tmp			= NULL;
	int size			= 0;			//rozmiar pobrany z tabeli 
	int err				= 0;
	Oid in_oid[]		={0};			/* 0 oznacza, �e PSQL powinien sam rozpozna� OIDy*/ 
	char* params[]		={NULL};		//{raster_id};
	int params_length[1]={0};			//nalezy ustawic rozmiary przynajmniej dla danych nie tekstowych
	int params_format[1]={0};			/* textowo (dla formatu binarnego jest 1)*/
	PGresult *pq_db_res				= NULL;
#endif

	PRINT_INFO("LIBBMDPGINF Importing file from LO %s\n",blob_number);
	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if(hDB==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(blob_number == NULL)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(dataGenBuf == NULL)				{	BMD_FOK(BMD_ERR_PARAM3);			}
	if(*dataGenBuf != NULL)				{	BMD_FOK(BMD_ERR_PARAM3);			}
	if( (*((long *)hDB)) != BMD_DBCONN_MAGIC )
		return BMD_ERR_INVALID_PTR;

	(*dataGenBuf) = (GenBuf_t *) malloc(sizeof(GenBuf_t));
	if(*dataGenBuf == NULL)		{	BMD_FOK(NO_MEMORY);	}


	db_conn=(PQ_bmd_db_conn_t *)hDB;
	if( db_conn->pg_conn == NULL )
		return BMD_ERR_NODATA;

	SBT(hDB,db_conn);	//Start transakcji LOBOWEJ

#ifndef BLOB_IN_BYTEA

	Oid lobjId			= 0;
	lobjId = (Oid)strtoul(blob_number, NULL, 10);
	if(lobjId == ULONG_MAX)
	{
		PRINT_ERROR("Error Lob Oid too big (%s) !!!\n", blob_number);
		BMD_FOK(BMD_ERR_OP_FAILED);
	}

	/* otworz large object */
	lobj_fd = lo_open(db_conn->pg_conn, lobjId, INV_READ);
	if (lobj_fd < 0)
	{
		RBT(hDB,db_conn);
		return BMD_ERR_OP_FAILED;
	}
	lobj_size =  lo_lseek(db_conn->pg_conn, lobj_fd, 0, SEEK_END);

	/* zaalokuj pamiec w genbufie */
	(*dataGenBuf)->buf = (char *) malloc (sizeof(char) * lobj_size);
	if((*dataGenBuf)->buf == NULL)
	{
		RBT(hDB,db_conn);
		return BMD_ERR_MEMORY;
	}
	(*dataGenBuf)->size = lobj_size;

	/* wczytaj dane do bufora */
	lo_lseek(db_conn->pg_conn, lobj_fd, 0, SEEK_SET);
	lobj_size = 0;
	/* @todo - czy 512 to nie za maly bufor moze szybciej by bylo przy wiekszym */
	while ((nbytes = lo_read(db_conn->pg_conn, lobj_fd, (char *)((*dataGenBuf)->buf)+lobj_size, 512)) > 0)
	{
		lobj_size += nbytes;
	}
	(void) lo_close(db_conn->pg_conn, lobj_fd);

#else

	params[0] = blob_number;	//tymczasowo przypisujemy blob_number do tablicy przekazywanej do PQexecParams
	params_length[0] = (int)strnlen(blob_number,21);		// jesli stosujemy partycjonowanie tabeli tblob - tu zawsze powinno by� 19 znak�w (zakres BIGINT postgresa w zapisie dziesi�tkowym)

	/* pobierz BLOB z tabeli tblob (z partycji wybranej przez funkcj� sk�adow� sel_tblob */
	pq_db_res = PQexecParams(db_conn->pg_conn,   
							"SELECT bobj FROM sel_tblob($1::bigint) AS (bobj bytea);",
							sizeof(params)/sizeof(params[0]),
							in_oid, (const char *const * )params, (const int*)params_length, 
							(const int*)params_format, 1);	// 0 - format textowy, 1 - format binarny - dotyczy params format oraz ostatniego parametru PQexecParams

	params[0] = NULL;
	err = PQresultStatus(pq_db_res);
	if (pq_db_res && (err == PGRES_COMMAND_OK || err == PGRES_TUPLES_OK) )
	{ 
		/* success */ 
		size = PQgetlength(pq_db_res,0,0);
		(*dataGenBuf)->size = size;
		if (0 == PQgetisnull(pq_db_res,0,0))	//w wierszu s� niepuste dane binarne - co� si� odczyta�o
		{

			(*dataGenBuf)->buf = (char *) malloc (sizeof(char) * size);
			if((*dataGenBuf)->buf == NULL)
			{
				PRINT_ERROR("LIBBMDPGERR Memory error. Error=%i\n",NO_MEMORY);
				if(*dataGenBuf) { free(*dataGenBuf); *dataGenBuf=NULL; }
				if (pq_db_res){PQclear(pq_db_res);pq_db_res = NULL;}
				return NO_MEMORY;
			}
			(*dataGenBuf)->size = size;

			tmp = PQgetvalue(pq_db_res,0,0);
			if (tmp != NULL)
			{
				memcpy((*dataGenBuf)->buf, tmp, size);
				PRINT_INFO("LIBBMDPG INFO OK importFileFromLO size %i.\n",size);
				tmp = NULL;				//juz nie potrzebujemy wskaznika na fragment z tmp_result_postgres
				err = BMD_OK;
			}

		}
		else
		{
			PRINT_ERROR("LIBBMDPGERR importFileFromLO did not read any data.\n");
			err = -1;
		}
	}
	else
	{
		PRINT_ERROR("LIBBMDPGERR importFileFromLO PQresultStatus %i .\n",(int)PQresultStatus(pq_db_res));
		if (pq_db_res){PQclear(pq_db_res);pq_db_res = NULL;}
		BMD_FOK(BMD_ERR_MEMORY);
	}
	if (pq_db_res){PQclear(pq_db_res);pq_db_res = NULL;}

#endif

	EBT(hDB,db_conn);

	return BMD_OK;
}

/* Funkcja inicjalizuje operacje pobierania zawartsci bloba o identyfikatorze wskazanym przez blob_number.
 * zwracany jest rozmiar pliku, ktory bedzie importowany z bazy i jego lokator oraz zaalokowany wewnatrz
 * funkcji GenBuf z zaalokowanym buforem o dlugosci LARGE_FILE_DB_CHUNK_SIZE, ktory bedzie podawany jako
 * argument do funkcji BFiterateImportFile. Parametr with_transaction == WITH_TRANSACTION wlacza dodatkowo
 * transakcje.
 */
long postgres_bmd_db_import_blob_begin(void *hDB,char *blob_number,long *blob_size,void **blob_locator,GenBuf_t **dataGenBuf)
{
	printf("postgres_bmd_db_import_blob_begin\n");

	PQ_bmd_db_conn_t *db_conn	= NULL;
	long err					= 0;
	long **blob_fd				= (long **) blob_locator;

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if(hDB==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(blob_number ==NULL)				{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(blob_size == NULL || *blob_size != 0)	{	BMD_FOK(BMD_ERR_PARAM3);	}
	if(	blob_locator == NULL ||
		*blob_locator != NULL)			{	BMD_FOK(BMD_ERR_PARAM4);	}
	if(	dataGenBuf == NULL ||
		*dataGenBuf != NULL)			{	BMD_FOK(BMD_ERR_PARAM5);	}
	if( (*((long *)hDB)) != BMD_DBCONN_MAGIC )
		return BMD_ERR_INVALID_PTR;

	*blob_fd = (long *) malloc (sizeof(long));
	if(*blob_fd == NULL)
	{
		PRINT_ERROR("LIBBMDPGERR Memory error. Error=%i\n",NO_MEMORY);
		return NO_MEMORY;
	}

	db_conn=(PQ_bmd_db_conn_t *)hDB;
	if( db_conn->pg_conn == NULL )
		return BMD_ERR_NODATA;

#ifdef BLOB_IN_BYTEA
	*blob_fd = (long*)blob_number;			//funkcja BFiterateImportFileFromLO() bedzie rozpoznawala po tym ktore bobj pobierac
#endif

	SBT(hDB,db_conn);

#ifndef BLOB_IN_BYTEA
	/* otworz large object */
	Oid lobOid					= 0;
	lobOid = strtoul(blob_number, NULL, 10);
	if(lobOid == ULONG_MAX)
	{
		PRINT_ERROR("Error Lob Oid too big (%s) !!!\n", blob_number);
		BMD_FOK(BMD_ERR_OP_FAILED);
	}
	**blob_fd = lo_open(db_conn->pg_conn, lobOid, INV_READ);
	if (**blob_fd < 0)
	{
		PRINT_ERROR("LIBBMDPGERR Error in opening large object - lo_open.\n");
		RBT(hDB,db_conn);
		return BMD_ERR_OP_FAILED;
	}
	*blob_size =  lo_lseek(db_conn->pg_conn, **blob_fd, 0, SEEK_END);
#endif
	*dataGenBuf = (GenBuf_t *) malloc (sizeof(GenBuf_t));
	if(*dataGenBuf == NULL)
	{
		PRINT_ERROR("LIBBMDPGERR Memory error. Error=%i\n",NO_MEMORY);
		RBT(hDB,db_conn);
		return NO_MEMORY;
	}

	(*dataGenBuf)->buf = (char *) malloc (sizeof(char) * LARGE_FILE_DB_CHUNK_SIZE);
	if((*dataGenBuf)->buf == NULL)
	{
		if(*dataGenBuf) { free(*dataGenBuf); *dataGenBuf=NULL; }
		PRINT_ERROR("LIBBMDPGERR Memory error. Error=%i\n",NO_MEMORY);
		RBT(hDB,db_conn);
		return NO_MEMORY;
	}
	(*dataGenBuf)->size = LARGE_FILE_DB_CHUNK_SIZE;
#ifndef BLOB_IN_BYTEA
	/* ustaw sie na poczatek bufora */
	lo_lseek(db_conn->pg_conn, **blob_fd, 0, SEEK_SET);
#endif
	return err;
}

long postgres_bmd_db_import_blob_continue(void *hDB,GenBuf_t *chunkGenBuf,void *blob_locator,EnumDbWhichPiece_t which_piece)
{
	printf("postgres_bmd_db_import_blob_continue\n");

	PQ_bmd_db_conn_t *db_conn=NULL;
	long err			= 0;
#ifndef BLOB_IN_BYTEA
	long *blob_fd			= (long *) blob_locator;

#else
	char *tmp			= NULL;
	char *blob_number	= NULL;
	int size			= 0;			//rozmiar pobrany z tabeli 
	Oid in_oid[]		={0};			/* 0 oznacza, �e PSQL powinien sam rozpozna� OIDy*/ 
	char* params[]		={NULL};		//{raster_id};
	int params_length[1]={0};			//nalezy ustawic rozmiary przynajmniej dla danych nie tekstowych
	int params_format[1]={0};			/* textowo (dla formatu binarnego jest 1)*/
	PGresult *pq_db_res				= NULL;
#endif

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if(hDB==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(blob_locator == NULL)			{       BMD_FOK(BMD_ERR_PARAM2);                    }
	if( *((long *)blob_locator) < 0)		{	BMD_FOK(BMD_ERR_PARAM2);			}
	if(chunkGenBuf == NULL)				{	BMD_FOK(BMD_ERR_PARAM3);			}
	if(which_piece < 0)				{	BMD_FOK(BMD_ERR_PARAM4);			}
	if( (*((long *)hDB)) != BMD_DBCONN_MAGIC )
		return BMD_ERR_INVALID_PTR;

	db_conn=(PQ_bmd_db_conn_t *)hDB;
	if( db_conn->pg_conn == NULL )
		return BMD_ERR_NODATA;
	if( db_conn->transaction_present != 1 )
	{
		PRINT_ERROR("Transaction not started\n");
		return BMD_ERR_FORMAT;
	}
#ifndef BLOB_IN_BYTEA
	err = lo_read(db_conn->pg_conn, *blob_fd, (char *)chunkGenBuf->buf, LARGE_FILE_DB_CHUNK_SIZE);
#else
	//TODO w 2.3 dopisa� asercje (which_piece != LOB_FIRST_PIECE && raster_id == *blob_fd)
	//TODO w 2.3 dopisa� asercje osiagniecia last_chunk i proby pobrania LOB_NEST_PIECE
	blob_number = (char*)blob_locator;
	params[0] = blob_number;	//tymczasowo przypisujemy blob_number do tablicy przekazywanej do PQexecParams
	params_length[0] = (int)strnlen(blob_number,21);		// jesli stosujemy partycjonowanie tabeli tblob - tu zawsze powinno by� 19 znak�w (zakres BIGINT postgresa w zapisie dziesi�tkowym)

	/* pobierz BLOB z tabeli tblob (z partycji wybranej przez funkcj� sk�adow� sel_tblob */
	pq_db_res = PQexecParams(db_conn->pg_conn,   
							"SELECT bobj FROM sel_tblob($1::bigint) AS (bobj bytea);",
							sizeof(params)/sizeof(params[0]),
							in_oid, (const char *const * )params, (const int*)params_length, 
							(const int*)params_format, 1);	// 0 - format textowy, 1 - format binarny - dotyczy params format oraz ostatniego parametru PQexecParams

	params[0] = NULL;
	err = PQresultStatus(pq_db_res);
	if (pq_db_res && (err == PGRES_COMMAND_OK || err == PGRES_TUPLES_OK) )
	{ 
		/* success */ 
		size = PQgetlength(pq_db_res,0,0);
		chunkGenBuf->size = size;
		if (0 == PQgetisnull(pq_db_res,0,0))	//w wierszu s� niepuste dane binarne - co� si� odczyta�o
		{

			chunkGenBuf->buf = (char *) malloc (sizeof(char) * size);
			if(chunkGenBuf->buf == NULL)
			{
				PRINT_ERROR("LIBBMDPGERR Memory error. Error=%i\n",NO_MEMORY);
				if(chunkGenBuf) { free(chunkGenBuf); chunkGenBuf=NULL; }
				if (pq_db_res){PQclear(pq_db_res);pq_db_res = NULL;}
				return NO_MEMORY;
			}
			chunkGenBuf->size = (long)size;

			tmp = PQgetvalue(pq_db_res,0,0);
			if (tmp != NULL)
			{
				memcpy(chunkGenBuf->buf, tmp, size);
				PRINT_ERROR("LIBBMDPG INFO OK importFileFromLO size %i.\n",size);
				PRINT_ERROR("LIBBMDPG INFO OK chunkGenBuf->size %li.\n",chunkGenBuf->size);
				tmp = NULL;				//juz nie potrzebujemy wskaznika na fragment z tmp_result_postgres
				err = chunkGenBuf->size;
			}
		}
		else
		{
			PRINT_ERROR("LIBBMDPGERR importFileFromLO did not read any data.\n");
			err = -1;
		}
	}
	else
	{
		PRINT_ERROR("LIBBMDPGERR importFileFromLO PQresultStatus %i .\n",(int)PQresultStatus(pq_db_res));
		PRINT_ERROR("LIBBMDPGERR importFileFromLO PQresultErrorMessage %s .\n",PQresultErrorMessage(pq_db_res));
		PRINT_ERROR("LIBBMDPGERR importFileFromLO blob_number %s .\n",blob_number);
		if (pq_db_res){PQclear(pq_db_res);pq_db_res = NULL;}
		BMD_FOK(BMD_ERR_MEMORY);
	}
	if (pq_db_res){PQclear(pq_db_res);pq_db_res = NULL;}


#endif
	return err;
}

long postgres_bmd_db_import_blob_end(void *hDB,void **blob_locator,GenBuf_t **dataGenBuf)
{
	printf("postgres_bmd_db_import_blob_end\n");

	PQ_bmd_db_conn_t *db_conn=NULL;
#ifndef BLOB_IN_BYTEA
	long **blob_fd			= (long **) blob_locator;
#endif
	long err				= 0;

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if(hDB==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(	blob_locator == NULL ||
		*blob_locator == NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(	dataGenBuf == NULL ||
		*dataGenBuf == NULL)			{	BMD_FOK(BMD_ERR_PARAM3);	}
	if( (*((long *)hDB)) != BMD_DBCONN_MAGIC )
		return BMD_ERR_INVALID_PTR;



	db_conn=(PQ_bmd_db_conn_t *)hDB;
	if( db_conn->pg_conn == NULL )
		return BMD_ERR_NODATA;
	if( db_conn->transaction_present != 1 )
	{
		PRINT_ERROR("Transaction not started\n");
		return BMD_ERR_FORMAT;
	}
#ifndef BLOB_IN_BYTEA
	(void) lo_close(db_conn->pg_conn, **blob_fd);
	if(*blob_fd) {free(*blob_fd); *blob_fd=NULL;}
#endif
	free_gen_buf(dataGenBuf);

	EBT(hDB,db_conn);

	return err;
}

long postgres_bmd_db_delete_blob(void *hDB,char *blob_number)
{
	printf("postgres_bmd_db_delete_blob\n");
	long status			= 0;
	void *hRes	= NULL;
	PQ_bmd_db_conn_t *db_conn=NULL;
	char *SQLQuery		= NULL;

	if( hDB == NULL )
		return BMD_ERR_PARAM1;
	if( blob_number == NULL )
		return BMD_ERR_PARAM2;
	if( (*((long *)hDB)) != BMD_DBCONN_MAGIC )
		return BMD_ERR_INVALID_PTR;

	db_conn=(PQ_bmd_db_conn_t *)hDB;
	if( db_conn->pg_conn == NULL )
		return BMD_ERR_NODATA;

	SBT(hDB,db_conn);

	PRINT_INFO("LIBBMDDBINF Deleting lob from database\n");
#ifndef BLOB_IN_BYTEA
	asprintf(&SQLQuery, "SELECT lo_unlink(%s);", blob_number);
#else
	asprintf(&SQLQuery, "DELETE FROM tblob where fk_co_raster_id = %s;", blob_number);
#endif
	if(SQLQuery == NULL)
	{
		PRINT_ERROR("LIBBMDPGERR Memory error. Error=%i\n",NO_MEMORY);
		RBT(hDB,db_conn);
		return NO_MEMORY;
	}
	status=postgres_bmd_db_execute_sql(hDB, SQLQuery,NULL,NULL,&hRes);
	if(status<0)
	{
#ifndef BLOB_IN_BYTEA
		PRINT_ERROR("LIBBMDDBERR Error in executing SELECT lo_unlink\n");
#else
		PRINT_ERROR("LIBBMDDBERR Error in executing DELETE FROM tblob ...\n");
#endif
		free(SQLQuery); SQLQuery = NULL;
		postgres_bmd_db_result_free(&hRes);
		RBT(hDB,db_conn);
		return status;
	}
	free(SQLQuery); SQLQuery = NULL;
	postgres_bmd_db_result_free(&hRes);
	EBT(hDB,db_conn);

	return status;
}
