#ifdef WIN32
#pragma warning(disable:4206)
#pragma warning(disable:4100)
#endif

#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>
#include <bmd/libbmddb/oracle/libbmddb_oracle.h>

#define OFSH(x) 	if( x != NULL ) \
					{ \
						OCIHandleFree((dvoid *) x, (ub4) OCI_HTYPE_STMT); \
						x=NULL; \
					}

#define OFLL(x)		if( x != NULL ) \
					{ \
						OCIDescriptorFree((dvoid *) Lob_loc, (ub4) OCI_DTYPE_LOB); \
						Lob_loc = NULL; \
					}

/*****
 * Funkcja eksportuje zawartosc GenBufa do bazy do BLOBa.
 * Zwracany jest unikalny identyfikator BLOBa blob_number.
 */
long oracle_bmd_db_export_blob(void *connection_handle,GenBuf_t *dataGenBuf,char **blob_number)
{
long err = 0;
OCIStmt *SqlStatementHandle	= (OCIStmt *) NULL;
OCI_bmd_db_conn_t *db_conn=NULL;
OCIBind *BindHandle 		= (OCIBind *) NULL;
OCIDefine *DefineHandle		= (OCIDefine *) NULL;
OCILobLocator *Lob_loc		= (OCILobLocator *) NULL;
char *SQLQueryInsert 		= "INSERT INTO or_largeobject (id, data) VALUES (or_largeobject_id_seq.nextval, EMPTY_BLOB())";
char *SQLQuerySelect 		= NULL;
ub2 rstrlen			= 0;
sb2 indicator			= 0;
ub4 overall_file_length		= (ub4)-1;
long fsize			= 0; /* only for testing!!! */

	if(connection_handle == NULL)
		return BMD_ERR_PARAM1;
	if(dataGenBuf == NULL)
		return BMD_ERR_PARAM2;
	if(blob_number == NULL)
		return BMD_ERR_PARAM3;
	if( (*(long *)connection_handle) != BMD_DBCONN_MAGIC )
		return BMD_ERR_INVALID_PTR;

	db_conn=(OCI_bmd_db_conn_t *)connection_handle;

	err = checkerr(db_conn->bmdHandles->ErrorHandle,
		OCIHandleAlloc( (dvoid *) db_conn->bmdHandles->EnviromentHandle,
			(dvoid **) &SqlStatementHandle,
			OCI_HTYPE_STMT,
			(size_t) 0,
			(dvoid **) 0)
	);
	if(err<0) {
		PRINT_ERROR("LIBBMDOCIERR Error in allocating statement handle - OCIHandleAlloc. "
			"RecievedErrorCode=%li.\n", err);
		OFSH(SqlStatementHandle);
		return BMD_ERR_OP_FAILED;
	}

	/* Utworz Lob_locator */
	err = checkerr(db_conn->bmdHandles->ErrorHandle,
		OCIDescriptorAlloc((dvoid *) db_conn->bmdHandles->EnviromentHandle, (dvoid **) &Lob_loc,
                         (ub4)OCI_DTYPE_LOB, (size_t) 0, (dvoid **) 0)
	);
	if(err<0) {
		PRINT_ERROR("LIBBMDOCIERR Error in allocating LOB locator - OCIDescriptorAlloc. "
			"RecievedErrorCode=%li.\n", err);
		OFSH(SqlStatementHandle);
		OFLL(Lob_loc);
		return BMD_ERR_OP_FAILED;
	}

	/* Przygotuj zapytanie */
	err = checkerr (db_conn->bmdHandles->ErrorHandle,
		OCIStmtPrepare(SqlStatementHandle,
				db_conn->bmdHandles->ErrorHandle,
				(text *) SQLQueryInsert,
				(ub4)strlen((char *)SQLQueryInsert),
				(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT)
	);
	if(err<0) {
		PRINT_ERROR("LIBBMDOCIERR Error in preparing statement handle. "
			"RecievedErrorCode=%li.\n", err);
		OFSH(SqlStatementHandle);
		OFLL(Lob_loc);
		return BMD_ERR_OP_FAILED;
	}

	/* Wykonaj zapytanie */
	err = checkerr (db_conn->bmdHandles->ErrorHandle,
		OCIStmtExecute(db_conn->bmdHandles->ServiceContextHandle,
				SqlStatementHandle,
				db_conn->bmdHandles->ErrorHandle,
				(ub4) 1,
				(ub4) 0,
				(CONST OCISnapshot *) NULL,
				(OCISnapshot *) NULL,
				(ub4) OCI_COMMIT_ON_SUCCESS)
	);
	if(err<0) {
		PRINT_ERROR("LIBBMDOCIERR Error in executing statement. "
			"RecievedErrorCode=%li.\n", err);
		OFSH(SqlStatementHandle);
		OFLL(Lob_loc);
		return BMD_ERR_OP_FAILED;
	}

	/* Pobierz id nowowstawionego wiersza. Niestety nie mozna tu uzyc sekwencji.currval gdyz
	 * jest to zapytanie typu SELECT .. FOR UPDATE i korzystam z funkcji oracle_bmd_db_get_max_entry */
	err = oracle_bmd_db_get_max_entry(connection_handle, "id", "or_largeobject", blob_number);
	if(err<0) {
		PRINT_ERROR("LIBBMDOCIERR Error in getting max entry from table. "
			"RecievedErrorCode=%li.\n", err);
		OFSH(SqlStatementHandle);
		OFLL(Lob_loc);
		return err;
	}
	/* SELECT ... FOR UPDATE BY ZABLOKOWAC WIERSZ TABELI */
	asprintf(&SQLQuerySelect, "SELECT data FROM or_largeobject WHERE id = %s FOR UPDATE", *blob_number);
	if(SQLQuerySelect == NULL) {
		OFSH(SqlStatementHandle);
		OFLL(Lob_loc);
		return NO_MEMORY;
	}

	/* Pobierz BLOB locator ostatniego wiersza tabeli*/
	err = checkerr(db_conn->bmdHandles->ErrorHandle,
		OCIStmtPrepare(SqlStatementHandle,
				db_conn->bmdHandles->ErrorHandle,
				(text *) SQLQuerySelect,
				(ub4)strlen((char *)SQLQuerySelect),
				(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT)
	);
	if(err<0) {
		PRINT_ERROR("LIBBMDOCIERR Error in preparing statement handle 2nd time. "
			"RecievedErrorCode=%li.\n", err);
		free0(SQLQuerySelect);
		OFSH(SqlStatementHandle);
		OFLL(Lob_loc);
		return BMD_ERR_OP_FAILED;
	}
	PRINT_ERROR("LIBBMDOCIINF SQLQuery = %s\n",SQLQuerySelect);
	if(SQLQuerySelect) {free(SQLQuerySelect); SQLQuerySelect=NULL;}

	/* Zdefiniuj lokator BLOBA jako zmienna wyjsciowa */
	err = checkerr(db_conn->bmdHandles->ErrorHandle,
		OCIDefineByPos(SqlStatementHandle, &DefineHandle, db_conn->bmdHandles->ErrorHandle,
			1, (dvoid *) &Lob_loc, (sb4) -1,
			(ub2) SQLT_BLOB,
			(dvoid *) &indicator,	/* 1 - NULL, 0-dane, x!=0 - obcieto dane do x 	*/
			(ub2 *) &rstrlen,	/* aktualna dlugosc danych wyjsciowych		*/
			(ub2 *) 0, OCI_DEFAULT)
	);
	if(err<0) {
		PRINT_ERROR("LIBBMDOCIERR Error in defining by position blob locator. "
			"RecievedErrorCode=%li.\n", err);
		free0(SQLQuerySelect);
		OFSH(SqlStatementHandle);
		OFLL(Lob_loc);
		return BMD_ERR_OP_FAILED;
	}

	/* Wykonaj zapytanie */
	err = checkerr(db_conn->bmdHandles->ErrorHandle,
		OCIStmtExecute(db_conn->bmdHandles->ServiceContextHandle,
				SqlStatementHandle,
				db_conn->bmdHandles->ErrorHandle,
				(ub4) 1,
				(ub4) 0,
				(CONST OCISnapshot *) NULL,
				(OCISnapshot *) NULL,
				(ub4) OCI_DEFAULT)
	);
	if(err<0) {
		PRINT_ERROR("LIBBMDOCIERR[%s:%i:bmd_db_export_blob] Error in executing statement. RecievedErrorCode=%li.\n", __FILE__, __LINE__, err);
		free0(SQLQuerySelect);
		OFSH(SqlStatementHandle);
		OFLL(Lob_loc);
		return BMD_ERR_OP_FAILED;
	}

	/* Tu juz mamy lokator BLOBa */
	overall_file_length = dataGenBuf->size;
	/* Tu juz mamy BLOB Locator wiec zapisuje do niego plik - W POJEDYNCZEJ ITERACJI*/
	err = checkerr(db_conn->bmdHandles->ErrorHandle,
		OCILobWrite(db_conn->bmdHandles->ServiceContextHandle,
			db_conn->bmdHandles->ErrorHandle,
			Lob_loc,
			&overall_file_length, (ub4) 1,
			dataGenBuf->buf, dataGenBuf->size,
			OCI_ONE_PIECE,
			NULL, NULL,
			0, 0)
	);
	if(err<0) {
		PRINT_ERROR("LIBBMDOCIERR[%s:%li:bmd_db_export_blob] Error LOB write. RecievedErrorCode=%li.\n", __FILE__, __LINE__, err);
		free0(SQLQuerySelect);
		OFSH(SqlStatementHandle);
		OFLL(Lob_loc);
		return BMD_ERR_OP_FAILED;
	}

	/* Only for testing !!! BEGIN*/
	/*fsize=-1;
	checkerr(db_conn->bmdHandles->ErrorHandle, OCILobGetLength(db_conn->bmdHandles->ServiceContextHandle,
			db_conn->bmdHandles->ErrorHandle,
			Lob_loc,
			(ub4 *) &fsize));*/
	/*PRINT_ERROR("LIBBMDOCIINF %d bytes were written to the Oracle database.\n",fsize);*/
	/* Only for testing !!! END*/

	free0(SQLQuerySelect);
	OFSH(SqlStatementHandle);
	if (BindHandle) {
		(void) OCIHandleFree((dvoid *) BindHandle, (ub4) OCI_HTYPE_BIND);
		BindHandle = NULL; }
	if (DefineHandle) {
		(void) OCIHandleFree((dvoid *) DefineHandle, (ub4) OCI_HTYPE_DEFINE);
		DefineHandle = NULL; }

	PRINT_ERROR("LIBBMDOCIINF[%s:%li:bmd_db_export_blob] LOB has been succesfully written\n", __FILE__, __LINE__);
	return err;
}

/*****
 * Funkcja importuje BLOBa z bazy (o wskazanym przez blob_number identyfikatorze) i umieszcza
 * go w genBufie dataGenBuf alokujac jednoczesnie GenBuf i jego wnetrze.
 */
long oracle_bmd_db_import_blob(void *hDB,char *blob_number,GenBuf_t **dataGenBuf)
{
	OCI_bmd_db_conn_t *db_conn=NULL;
	OCIStmt *SqlStatementHandle	= (OCIStmt *) NULL;
	OCILobLocator *Lob_loc		= (OCILobLocator *) NULL;
	OCIDefine *DefineHandle		= (OCIDefine *) NULL;
	char *SQLQuery			= NULL;
	long err				= 0;
	long Lob_size		=-1;
	ub4 overall_file_length		= 0;

	if(hDB == NULL)
		return BMD_ERR_PARAM1;
	if( blob_number == NULL )
		return BMD_ERR_PARAM1;
	if( dataGenBuf == NULL )
		return BMD_ERR_PARAM3;
	if( (*dataGenBuf) != NULL )
		return BMD_ERR_PARAM3;

	if( (*(long *)hDB) != BMD_DBCONN_MAGIC )
		return BMD_ERR_INVALID_PTR;

	db_conn=(OCI_bmd_db_conn_t *)hDB;
	if( db_conn->bmdHandles == NULL )
		return BMD_ERR_NODATA;

	asprintf(&SQLQuery, "SELECT data FROM or_largeobject WHERE id=%s", blob_number);
	if(SQLQuery==NULL) 
		return BMD_ERR_MEMORY;

	err = checkerr(db_conn->bmdHandles->ErrorHandle,
				   OCIHandleAlloc((dvoid *) db_conn->bmdHandles->EnviromentHandle,
								  (dvoid **) &SqlStatementHandle,
								  OCI_HTYPE_STMT,
								  (size_t) 0,
								  (dvoid **) 0));
	if(err<0) 
	{
		PRINT_ERROR("LIBBMDOCIERR Error in statement handle allocation. "
			"RecievedErrorCode=%li.\n", err);
		free0(SQLQuery);
		if(SqlStatementHandle) 
		{
			(void) OCIHandleFree((dvoid *) SqlStatementHandle, (ub4) OCI_HTYPE_STMT);
			SqlStatementHandle = NULL; 
		}
		return BMD_ERR_OP_FAILED;
	}

	/* Utworz lokator BLOBa */
	err = checkerr(db_conn->bmdHandles->ErrorHandle,
				   OCIDescriptorAlloc((dvoid *)db_conn->bmdHandles->EnviromentHandle,
									  (dvoid **)&Lob_loc,
									  (ub4)OCI_DTYPE_LOB,
									  (size_t)0,
									  (dvoid **)0));
	if(err<0) 
	{
		PRINT_ERROR("LIBBMDOCIERR Error in allocating LOB locator - OCIDescriptorAlloc. "
			"RecievedErrorCode=%li.\n", err);
		free0(SQLQuery);
		if(SqlStatementHandle)
		{
			(void) OCIHandleFree((dvoid *) SqlStatementHandle, (ub4) OCI_HTYPE_STMT);
			SqlStatementHandle = NULL; 
		}
		return BMD_ERR_OP_FAILED;
	}


	/* Pobierz lokator z bazy z kolumny BLOB */
	err = checkerr (db_conn->bmdHandles->ErrorHandle,
					OCIStmtPrepare(SqlStatementHandle,
								   db_conn->bmdHandles->ErrorHandle,
								   (text *) SQLQuery,
								   (ub4)strlen((char *)SQLQuery),
							       (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT));
	if(err<0) 
	{
		PRINT_ERROR("LIBBMDOCIERR Error in preparing statement handle. "
			"RecievedErrorCode=%li.\n", err);
		free0(SQLQuery);
		if(SqlStatementHandle) 
		{
			(void) OCIHandleFree((dvoid *) SqlStatementHandle, (ub4) OCI_HTYPE_STMT);
			SqlStatementHandle = NULL; 
		}
		return BMD_ERR_OP_FAILED;
	}

	/* Wywolanie SELECT'a typu explicit wiec uzywam define juz tutaj */
	err = checkerr(db_conn->bmdHandles->ErrorHandle,
				   OCIDefineByPos(SqlStatementHandle,
								  &DefineHandle,
								  db_conn->bmdHandles->ErrorHandle,
								  (ub4) 1,
								  (dvoid *)&Lob_loc, (sb4)0, (ub2) SQLT_BLOB,
								  (dvoid *) 0, (ub2 *) 0, (ub2 *) 0,
							      (ub4) OCI_DEFAULT));
	if(err<0) 
	{
		PRINT_ERROR("LIBBMDOCIERR Error in defining LOB locator - OCIDefineByPos. "
			"RecievedErrorCode=%li.\n", err);
		free0(SQLQuery);
		if(SqlStatementHandle) 
		{
			(void) OCIHandleFree((dvoid *) SqlStatementHandle, (ub4) OCI_HTYPE_STMT);
			SqlStatementHandle = NULL; 
		}
		if(Lob_loc) 
		{
			OCIDescriptorFree((dvoid *) Lob_loc, (ub4) OCI_DTYPE_LOB);
			Lob_loc = NULL;	
		}
		return BMD_ERR_OP_FAILED;
	}

	/* Wywolaj selecta i od razu pobierz jeden wiersz do miennej Lob_loc*/
	err = checkerr(db_conn->bmdHandles->ErrorHandle,
				   OCIStmtExecute(db_conn->bmdHandles->ServiceContextHandle,
								  SqlStatementHandle,
								  db_conn->bmdHandles->ErrorHandle,
								  (ub4) 1,(ub4) 0,
								  (CONST OCISnapshot*) 0,
								  (OCISnapshot*) 0,
								  (ub4) OCI_DEFAULT));
	if(err<0) 
	{
		PRINT_ERROR("LIBBMDOCIERR Error in executing SQL statement. "
			"RecievedErrorCode=%li.\n", err);
		free0(SQLQuery);
		if(SqlStatementHandle) 
		{
			(void) OCIHandleFree((dvoid *) SqlStatementHandle, (ub4) OCI_HTYPE_STMT);
			SqlStatementHandle = NULL; 
		}
		if(Lob_loc) 
		{
			OCIDescriptorFree((dvoid *) Lob_loc, (ub4) OCI_DTYPE_LOB);
			Lob_loc = NULL;	
		}
		return BMD_ERR_OP_FAILED;
	}

	/* Pobierz rozmiar pliku znajdujacego sie w BLOBie i zaalokuj gen_buf */
	err = checkerr(db_conn->bmdHandles->ErrorHandle,
		OCILobGetLength(db_conn->bmdHandles->ServiceContextHandle,
			db_conn->bmdHandles->ErrorHandle,
			Lob_loc,
			(ub4 *)&Lob_size)
	);
	if(err<0) {
		PRINT_ERROR("LIBBMDOCIERR Error in getting LOB file length. "
			"RecievedErrorCode=%li.\n", err);
		if(*dataGenBuf) {free(*dataGenBuf);	*dataGenBuf=NULL;	}
		if(SQLQuery)	{free(SQLQuery);	SQLQuery=NULL;		}
		if(SqlStatementHandle) {
			(void) OCIHandleFree((dvoid *) SqlStatementHandle, (ub4) OCI_HTYPE_STMT);
			SqlStatementHandle = NULL; }
		if(Lob_loc) {
			OCIDescriptorFree((dvoid *) Lob_loc, (ub4) OCI_DTYPE_LOB);
			Lob_loc = NULL;	}
		return BMD_ERR_OP_FAILED;
	}

	/*PRINT_ERROR("LIBBMDOCIINF %li bytes were read from the Oracle database.\n",Lob_size);*/
	if(Lob_size > 0) {
		(*dataGenBuf)=(GenBuf_t *)malloc(sizeof(GenBuf_t));
		if( (*dataGenBuf) == NULL )
		{
			return BMD_ERR_MEMORY;
		}
		(*dataGenBuf)->buf = (char *) malloc (sizeof(unsigned char) * Lob_size);
		if((*dataGenBuf)->buf == NULL) {
			if(*dataGenBuf) {free(*dataGenBuf);	*dataGenBuf=NULL;	}
			if(SQLQuery)	{free(SQLQuery);	SQLQuery=NULL;		}
			if(SqlStatementHandle) {
				(void) OCIHandleFree((dvoid *) SqlStatementHandle, (ub4) OCI_HTYPE_STMT);
				SqlStatementHandle = NULL; }
			if(Lob_loc) {
				OCIDescriptorFree((dvoid *) Lob_loc, (ub4) OCI_DTYPE_LOB);
				Lob_loc = NULL;	}
			return NO_MEMORY;
		}
		(*dataGenBuf)->size = Lob_size;
	}
	else {
		(*dataGenBuf)->buf = NULL;
		(*dataGenBuf)->size = 0;
	}

	/* Wczytaj BLOBa do gen_buf */
	overall_file_length = (ub4)Lob_size;
	err = checkerr(db_conn->bmdHandles->ErrorHandle,
		OCILobRead(db_conn->bmdHandles->ServiceContextHandle,
			db_conn->bmdHandles->ErrorHandle,
			Lob_loc,
			&overall_file_length, (ub4) 1,
			(*dataGenBuf)->buf, (*dataGenBuf)->size,
			(dvoid *) NULL, NULL,
			(ub2) 0, (ub1) 0)
	);
	if(err<0) {
		PRINT_ERROR("LIBBMDOCIERR Error in reading LOB. RecievedErrorCode=%li.\n",err);
		if(*dataGenBuf) {
			if((*dataGenBuf)->buf) {
				free((*dataGenBuf)->buf);
				(*dataGenBuf)->buf = NULL;
			}
			free(*dataGenBuf); *dataGenBuf=NULL;
		}
		if(SQLQuery)	{free(SQLQuery);	SQLQuery=NULL;		}
		if(SqlStatementHandle) {
			(void) OCIHandleFree((dvoid *) SqlStatementHandle, (ub4) OCI_HTYPE_STMT);
			SqlStatementHandle = NULL; }
		if(Lob_loc) {
			OCIDescriptorFree((dvoid *) Lob_loc, (ub4) OCI_DTYPE_LOB);
			Lob_loc = NULL;	}
		return BMD_ERR_OP_FAILED;
	}

	if(Lob_loc) {
		OCIDescriptorFree((dvoid *) Lob_loc, (ub4) OCI_DTYPE_LOB);
		Lob_loc = NULL;	}
	if (DefineHandle) {
		(void) OCIHandleFree((dvoid *) DefineHandle, (ub4) OCI_HTYPE_DEFINE);
		DefineHandle = NULL; }
	if (SqlStatementHandle) {
		(void) OCIHandleFree((dvoid *) SqlStatementHandle, (ub4) OCI_HTYPE_STMT);
		SqlStatementHandle = NULL; }
	if(SQLQuery) {free(SQLQuery); SQLQuery=NULL;}
	PRINT_ERROR("LIBBMDOCIINF[%s:%li:bmd_db_import_blob] LOB has been succesfully read.\n", __FILE__, __LINE__);
	return err;
}

long oracle_bmd_db_export_blob_begin(void *connection_handle,char **blob_number,void **blob_locator)
{
	long err = 0;
	OCI_bmd_db_conn_t *db_conn=NULL;
	OCIStmt *SqlStatementHandle	= (OCIStmt *) NULL;
	OCIBind *BindHandle 		= (OCIBind *) NULL;
	OCIDefine *DefineHandle		= (OCIDefine *) NULL;
	OCILobLocator **Lob_loc		= (OCILobLocator **) blob_locator;
	char *SQLQueryInsert 		= "INSERT INTO or_largeobject (id, data) VALUES (or_largeobject_id_seq.nextval, EMPTY_BLOB())";
	char *SQLQuerySelect 		= NULL;
	ub2 rstrlen			= 0;
	sb2 indicator			= 0;

	if(connection_handle == NULL)
		return BMD_ERR_PARAM1;
	if(blob_number == NULL)
		return BMD_ERR_PARAM2;
	if(blob_locator == NULL || *blob_locator != NULL)
		return BMD_ERR_PARAM3;

	if( (*(long *)connection_handle) != BMD_DBCONN_MAGIC )
		return BMD_ERR_INVALID_PTR;
	db_conn=(OCI_bmd_db_conn_t *)connection_handle;


	err = checkerr(db_conn->bmdHandles->ErrorHandle,
		OCIHandleAlloc( (dvoid *) db_conn->bmdHandles->EnviromentHandle,
			(dvoid **) &SqlStatementHandle,
			OCI_HTYPE_STMT,
			(size_t) 0,
			(dvoid **) 0)
	);
	if(err<0) {
		PRINT_ERROR("LIBBMDOCIERR Error in allocating statement handle - OCIHandleAlloc. "
			"RecievedErrorCode=%li.\n", err);
		if(SqlStatementHandle) {
			OCIHandleFree((dvoid *) SqlStatementHandle, (ub4) OCI_HTYPE_STMT);
			SqlStatementHandle=NULL; }
		return BMD_ERR_OP_FAILED;
	}

	/* Utworz Lob_locator */
	err = checkerr(db_conn->bmdHandles->ErrorHandle,
		OCIDescriptorAlloc((dvoid *) db_conn->bmdHandles->EnviromentHandle, (dvoid **) Lob_loc,
                         (ub4)OCI_DTYPE_LOB, (size_t) 0, (dvoid **) 0)
	);
	if(err<0) {
		PRINT_ERROR("LIBBMDOCIERR Error in allocating LOB locator - OCIDescriptorAlloc. "
			"RecievedErrorCode=%li.\n", err);
		if(SqlStatementHandle) {
			OCIHandleFree((dvoid *) SqlStatementHandle, (ub4) OCI_HTYPE_STMT);
			SqlStatementHandle=NULL; }
		if(*Lob_loc) {
			OCIDescriptorFree((dvoid *) *Lob_loc, (ub4) OCI_DTYPE_LOB);
			*Lob_loc = NULL; }
		return BMD_ERR_OP_FAILED;
	}

	/* Przygotuj zapytanie */
	err = checkerr (db_conn->bmdHandles->ErrorHandle,
		OCIStmtPrepare(SqlStatementHandle,
				db_conn->bmdHandles->ErrorHandle,
				(text *) SQLQueryInsert,
				(ub4)strlen((char *)SQLQueryInsert),
				(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT)
	);
	if(err<0) {
		PRINT_ERROR("LIBBMDOCIERR Error in preparing statement handle. "
			"RecievedErrorCode=%li.\n", err);
		if(SqlStatementHandle) {
			OCIHandleFree((dvoid *) SqlStatementHandle, (ub4) OCI_HTYPE_STMT);
			SqlStatementHandle=NULL; }
		if(*Lob_loc) {
			OCIDescriptorFree((dvoid *) *Lob_loc, (ub4) OCI_DTYPE_LOB);
			*Lob_loc = NULL; }
		return BMD_ERR_OP_FAILED;
	}

	/* Wykonaj zapytanie */
	err = checkerr (db_conn->bmdHandles->ErrorHandle,
		OCIStmtExecute(db_conn->bmdHandles->ServiceContextHandle,
				SqlStatementHandle,
				db_conn->bmdHandles->ErrorHandle,
				(ub4) 1,
				(ub4) 0,
				(CONST OCISnapshot *) NULL,
				(OCISnapshot *) NULL,
				(ub4) /*OCI_COMMIT_ON_SUCCESS*/OCI_DEFAULT)
	);
	if(err<0) {
		PRINT_ERROR("LIBBMDOCIERR Error in executing statement. "
			"RecievedErrorCode=%li.\n", err);
		if(SqlStatementHandle) {
			OCIHandleFree((dvoid *) SqlStatementHandle, (ub4) OCI_HTYPE_STMT);
			SqlStatementHandle=NULL; }
		if(*Lob_loc) {
			OCIDescriptorFree((dvoid *) *Lob_loc, (ub4) OCI_DTYPE_LOB);
			*Lob_loc = NULL; }
		return BMD_ERR_OP_FAILED;
	}

	/* Pobierz id nowowstawionego wiersza. Niestety nie mozna tu uzyc sekwencji.currval gdyz
	 * jest to zapytanie typu SELECT .. FOR UPDATE i korzystam z funkcji oracle_bmd_db_get_max_entry */
	err = oracle_bmd_db_get_max_entry(connection_handle, "id", "or_largeobject", blob_number);
	if(err<0) {
		PRINT_ERROR("LIBBMDOCIERR Error in getting max entry from table. "
			"RecievedErrorCode=%li.\n", err);
		if(SqlStatementHandle) {
			OCIHandleFree((dvoid *) SqlStatementHandle, (ub4) OCI_HTYPE_STMT);
			SqlStatementHandle=NULL; }
		if(*Lob_loc) {
			OCIDescriptorFree((dvoid *) *Lob_loc, (ub4) OCI_DTYPE_LOB);
			*Lob_loc = NULL; }
		return err;
	}
	/* SELECT ... FOR UPDATE BY ZABLOKOWAC WIERSZ TABELI */
	asprintf(&SQLQuerySelect, "SELECT data FROM or_largeobject WHERE id = %s FOR UPDATE", *blob_number);
	if(SQLQuerySelect == NULL) {
		if(SqlStatementHandle) {
			OCIHandleFree((dvoid *) SqlStatementHandle, (ub4) OCI_HTYPE_STMT);
			SqlStatementHandle=NULL; }
		if(*Lob_loc) {
			OCIDescriptorFree((dvoid *) *Lob_loc, (ub4) OCI_DTYPE_LOB);
			*Lob_loc = NULL; }
		return NO_MEMORY;
	}

	/* Pobierz BLOB locator ostatniego wiersza tabeli*/
	err = checkerr(db_conn->bmdHandles->ErrorHandle,
		OCIStmtPrepare(SqlStatementHandle,
				db_conn->bmdHandles->ErrorHandle,
				(text *) SQLQuerySelect,
				(ub4)strlen((char *)SQLQuerySelect),
				(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT)
	);
	if(err<0) {
		PRINT_ERROR("LIBBMDOCIERR Error in preparing statement handle 2nd time. "
			"RecievedErrorCode=%li.\n", err);
		if(SQLQuerySelect) {free(SQLQuerySelect); SQLQuerySelect=NULL;}
		if(SqlStatementHandle) {
			OCIHandleFree((dvoid *) SqlStatementHandle, (ub4) OCI_HTYPE_STMT);
			SqlStatementHandle=NULL; }
		if(*Lob_loc) {
			OCIDescriptorFree((dvoid *) *Lob_loc, (ub4) OCI_DTYPE_LOB);
			*Lob_loc = NULL; }
		return BMD_ERR_OP_FAILED;
	}
	PRINT_ERROR("LIBBMDOCIINF SQLQuery = %s\n",SQLQuerySelect);
	if(SQLQuerySelect) {free(SQLQuerySelect); SQLQuerySelect=NULL;}

	/* Zdefiniuj lokator BLOBA jako zmienna wyjsciowa */
	err = checkerr(db_conn->bmdHandles->ErrorHandle,
		OCIDefineByPos(SqlStatementHandle, &DefineHandle, db_conn->bmdHandles->ErrorHandle,
			1, (dvoid *) Lob_loc, (sb4) -1,
			(ub2) SQLT_BLOB,
			(dvoid *) &indicator,	/* 1 - NULL, 0-dane, x!=0 - obcieto dane do x 	*/
			(ub2 *) &rstrlen,	/* aktualna dlugosc danych wyjsciowych		*/
			(ub2 *) 0, OCI_DEFAULT)
	);
	if(err<0) {
		PRINT_ERROR("LIBBMDOCIERR Error in defining by position blob locator. "
			"RecievedErrorCode=%li.\n", err);
		if(SQLQuerySelect) {free(SQLQuerySelect); SQLQuerySelect=NULL;}
		if(SqlStatementHandle) {
			OCIHandleFree((dvoid *) SqlStatementHandle, (ub4) OCI_HTYPE_STMT);
			SqlStatementHandle=NULL; }
		if(*Lob_loc) {
			OCIDescriptorFree((dvoid *) *Lob_loc, (ub4) OCI_DTYPE_LOB);
			*Lob_loc = NULL; }
		return BMD_ERR_OP_FAILED;
	}

	/* Wykonaj zapytanie */
	err = checkerr(db_conn->bmdHandles->ErrorHandle,
		OCIStmtExecute(db_conn->bmdHandles->ServiceContextHandle,
				SqlStatementHandle,
				db_conn->bmdHandles->ErrorHandle,
				(ub4) 1,
				(ub4) 0,
				(CONST OCISnapshot *) NULL,
				(OCISnapshot *) NULL,
				(ub4) OCI_DEFAULT)
	);
	if(err<0) {
		PRINT_ERROR("LIBBMDOCIERR Error in executing statement. RecievedErrorCode=%li.\n",err);
		if(SQLQuerySelect) {free(SQLQuerySelect); SQLQuerySelect=NULL;}
		if(SqlStatementHandle) {
			OCIHandleFree((dvoid *) SqlStatementHandle, (ub4) OCI_HTYPE_STMT);
			SqlStatementHandle=NULL; }
		if(*Lob_loc) {
			OCIDescriptorFree((dvoid *) *Lob_loc, (ub4) OCI_DTYPE_LOB);
			*Lob_loc = NULL; }
		return BMD_ERR_OP_FAILED;
	}

	if(SQLQuerySelect) {free(SQLQuerySelect); SQLQuerySelect=NULL;}
	if (BindHandle) {
		(void) OCIHandleFree((dvoid *) BindHandle, (ub4) OCI_HTYPE_BIND);
		BindHandle = NULL; }
	if (DefineHandle) {
		(void) OCIHandleFree((dvoid *) DefineHandle, (ub4) OCI_HTYPE_DEFINE);
		DefineHandle = NULL; }
	if (SqlStatementHandle) {
		(void) OCIHandleFree((dvoid *) SqlStatementHandle, (ub4) OCI_HTYPE_STMT);
		SqlStatementHandle = NULL; }

	PRINT_ERROR("LIBBMDOCIINF Begin LOB export finished successfully\n");
	return err;
}

long oracle_bmd_db_export_blob_continue(void *connection_handle,GenBuf_t *chunkGenBuf,void *blob_locator,
			                     EnumDbWhichPiece_t which_piece)
{
long err = 0;
OCI_bmd_db_conn_t *db_conn=NULL;
OCILobLocator *Lob_loc		= (OCILobLocator *) blob_locator;
ub4 amt			= 0;
ub4 offset		= 1;
ub1 piece		= 0;

	if(connection_handle == NULL)
		return BMD_ERR_PARAM1;
	if(chunkGenBuf == NULL)
		return BMD_ERR_PARAM2;
	if(blob_locator == NULL)
		return BMD_ERR_PARAM3;
	if(which_piece < 0)
		return BMD_ERR_PARAM4;
	
	if( (*(long *)connection_handle) != BMD_DBCONN_MAGIC )
		return BMD_ERR_INVALID_PTR;
	db_conn=(OCI_bmd_db_conn_t *)connection_handle;

	switch(which_piece){
	case LOB_FIRST_PIECE:
			piece = OCI_FIRST_PIECE;
			break;
	case LOB_NEXT_PIECE:
			piece = OCI_NEXT_PIECE;
			break;
	case LOB_LAST_PIECE:
			piece = OCI_LAST_PIECE;
			break;
	case LOB_ONE_PIECE:
			piece = OCI_ONE_PIECE;
			break;
	}
	err = checkerr(db_conn->bmdHandles->ErrorHandle,
		OCILobWrite(db_conn->bmdHandles->ServiceContextHandle,
			db_conn->bmdHandles->ErrorHandle,
			Lob_loc, &amt, offset,
			chunkGenBuf->buf, chunkGenBuf->size,
			piece,
			NULL, NULL, 0, SQLCS_IMPLICIT)
	);
	if(err<0) {
		if((which_piece == LOB_FIRST_PIECE || which_piece == LOB_NEXT_PIECE)  && err == ERR_OCI_NEED_DATA) {err = 0; return err;}
		PRINT_ERROR("LIBBMDOCIERR Error LOB chunk write. "
			"RecievedErrorCode=%li.\n", err);
		return BMD_ERR_OP_FAILED;
	}
	return err;
}

long oracle_bmd_db_export_blob_end(void *connection_handle,void **blob_locator)
{
long err = 0;
OCI_bmd_db_conn_t *db_conn=NULL;
OCILobLocator **Lob_loc		= (OCILobLocator **) blob_locator;
long fsize			= 0; /* only for testing!!! */

	if(connection_handle == NULL)
		return BMD_ERR_PARAM1;
	if(blob_locator == NULL || *blob_locator == NULL)
		return BMD_ERR_PARAM2;

	if( (*(long *)connection_handle) != BMD_DBCONN_MAGIC )
		return BMD_ERR_INVALID_PTR;

	db_conn=(OCI_bmd_db_conn_t *)connection_handle;

	/* Only for testing !!! BEGIN*/
	fsize=-1;
	checkerr(db_conn->bmdHandles->ErrorHandle, OCILobGetLength(db_conn->bmdHandles->ServiceContextHandle,
			db_conn->bmdHandles->ErrorHandle,
			*Lob_loc,
			(ub4 *) &fsize));
	PRINT_ERROR("LIBBMDOCIINF %d bytes were written to the Oracle database.\n",fsize);
	/* Only for testing !!! END*/

	if(*Lob_loc) {
		OCIDescriptorFree((dvoid *) *Lob_loc, (ub4) OCI_DTYPE_LOB);
		*Lob_loc = NULL; }
	PRINT_ERROR("LIBBMDOCIINF End LOB export finished successfully\n");
	return err;
}

long oracle_bmd_db_import_blob_begin(void *connection_handle,char *blob_number,long *blob_size,
							  void **blob_locator,GenBuf_t **dataGenBuf)
{
	long err = 0;
	OCI_bmd_db_conn_t *db_conn=NULL;
	OCIStmt *SqlStatementHandle	= (OCIStmt *) NULL;
	OCILobLocator **Lob_loc		= (OCILobLocator **) blob_locator;
	OCIDefine *DefineHandle		= (OCIDefine *) NULL;
	char *SQLQuery			= NULL;

	if(connection_handle == NULL)
		return BMD_ERR_PARAM1;
	if(dataGenBuf == NULL || *dataGenBuf != NULL)
		return BMD_ERR_PARAM3;
	if( (*(long *)connection_handle) != BMD_DBCONN_MAGIC )
		return BMD_ERR_INVALID_PTR;

	db_conn=(OCI_bmd_db_conn_t *)connection_handle;

	asprintf(&SQLQuery, "SELECT data FROM or_largeobject WHERE id=%s", blob_number);
	if(SQLQuery==NULL) {
		if(*dataGenBuf) {free(*dataGenBuf); *dataGenBuf=NULL;}
		return NO_MEMORY;
	}

	err = checkerr(db_conn->bmdHandles->ErrorHandle,
		OCIHandleAlloc( (dvoid *) db_conn->bmdHandles->EnviromentHandle,
			(dvoid **) &SqlStatementHandle,
			OCI_HTYPE_STMT,
			(size_t) 0,
			(dvoid **) 0)
	);
	if(err<0) {
		PRINT_ERROR("LIBBMDOCIERR Error in statement handle allocation. "
			"RecievedErrorCode=%li.\n", err);
		if(SQLQuery)	{free(SQLQuery);	SQLQuery=NULL;		}
		if(SqlStatementHandle) {
			(void) OCIHandleFree((dvoid *) SqlStatementHandle, (ub4) OCI_HTYPE_STMT);
			SqlStatementHandle = NULL; }
		return BMD_ERR_OP_FAILED;
	}

	/* Utworz lokator BLOBa */
	err = checkerr(db_conn->bmdHandles->ErrorHandle,
		OCIDescriptorAlloc((dvoid *)db_conn->bmdHandles->EnviromentHandle,
			(dvoid **)Lob_loc,
			(ub4)OCI_DTYPE_LOB,
			(size_t)0,
			(dvoid **)0)
	);
	if(err<0) {
		PRINT_ERROR("LIBBMDOCIERR Error in allocating LOB locator - OCIDescriptorAlloc. "
			"RecievedErrorCode=%li.\n", err);
		if(SQLQuery)	{free(SQLQuery);	SQLQuery=NULL;		}
		if(SqlStatementHandle) {
			(void) OCIHandleFree((dvoid *) SqlStatementHandle, (ub4) OCI_HTYPE_STMT);
			SqlStatementHandle = NULL; }
		return BMD_ERR_OP_FAILED;
	}


	/* Pobierz lokator z bazy z kolumny BLOB */
	err = checkerr (db_conn->bmdHandles->ErrorHandle,
		OCIStmtPrepare(SqlStatementHandle,
				db_conn->bmdHandles->ErrorHandle,
				(text *) SQLQuery,
				(ub4)strlen((char *)SQLQuery),
				(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT)
	);
	if(err<0) {
		PRINT_ERROR("LIBBMDOCIERR Error in preparing statement handle. "
			"RecievedErrorCode=%li.\n", err);
		if(SQLQuery)	{free(SQLQuery);	SQLQuery=NULL;		}
		if(SqlStatementHandle) {
			(void) OCIHandleFree((dvoid *) SqlStatementHandle, (ub4) OCI_HTYPE_STMT);
			SqlStatementHandle = NULL; }
		return BMD_ERR_OP_FAILED;
	}

	/* Wywolanie SELECT'a typu explicit wiec uzywam define juz tutaj */
	err = checkerr (db_conn->bmdHandles->ErrorHandle,
		OCIDefineByPos(SqlStatementHandle,
				&DefineHandle,
				db_conn->bmdHandles->ErrorHandle,
				(ub4) 1,
				(dvoid *)Lob_loc, (sb4)0, (ub2) SQLT_BLOB,
				(dvoid *) 0, (ub2 *) 0, (ub2 *) 0,
				(ub4) OCI_DEFAULT)
	);
	if(err<0) {
		PRINT_ERROR("LIBBMDOCIERR Error in defining LOB locator - OCIDefineByPos. "
			"RecievedErrorCode=%li.\n", err);
		if(SQLQuery)	{free(SQLQuery);	SQLQuery=NULL;		}
		if(SqlStatementHandle) {
			(void) OCIHandleFree((dvoid *) SqlStatementHandle, (ub4) OCI_HTYPE_STMT);
			SqlStatementHandle = NULL; }
		if(*Lob_loc) {
			OCIDescriptorFree((dvoid *) *Lob_loc, (ub4) OCI_DTYPE_LOB);
			*Lob_loc = NULL;	}
		return BMD_ERR_OP_FAILED;
	}

	/* Wywolaj selecta i od razu pobierz jeden wiersz do miennej Lob_loc*/
	err = checkerr(db_conn->bmdHandles->ErrorHandle,
		OCIStmtExecute(db_conn->bmdHandles->ServiceContextHandle,
				SqlStatementHandle,
				db_conn->bmdHandles->ErrorHandle,
				(ub4) 1,
				(ub4) 0,
				(CONST OCISnapshot*) 0,
				(OCISnapshot*) 0,
				(ub4) OCI_DEFAULT)
	);
	if(err<0) {
		PRINT_ERROR("LIBBMDOCIERR Error in executing SQL statement. "
			"RecievedErrorCode=%li.\n", err);
		if(SQLQuery)	{free(SQLQuery);	SQLQuery=NULL;		}
		if(SqlStatementHandle) {
			(void) OCIHandleFree((dvoid *) SqlStatementHandle, (ub4) OCI_HTYPE_STMT);
			SqlStatementHandle = NULL; }
		if(*Lob_loc) {
			OCIDescriptorFree((dvoid *) *Lob_loc, (ub4) OCI_DTYPE_LOB);
			*Lob_loc = NULL;	}
		return BMD_ERR_OP_FAILED;
	}

	err = checkerr(db_conn->bmdHandles->ErrorHandle,
		OCILobGetLength(db_conn->bmdHandles->ServiceContextHandle,
			db_conn->bmdHandles->ErrorHandle,
			*Lob_loc, (ub4 *) blob_size)
	);
	if(err<0) {
		PRINT_ERROR("LIBBMDOCIERR Error in getting LOB length. "
			"RecievedErrorCode=%li.\n", err);
		if(SQLQuery)	{free(SQLQuery);	SQLQuery=NULL;		}
		if(SqlStatementHandle) {
			(void) OCIHandleFree((dvoid *) SqlStatementHandle, (ub4) OCI_HTYPE_STMT);
			SqlStatementHandle = NULL; }
		if(*Lob_loc) {
			OCIDescriptorFree((dvoid *) *Lob_loc, (ub4) OCI_DTYPE_LOB);
			*Lob_loc = NULL;	}
		return BMD_ERR_OP_FAILED;
	}


	*dataGenBuf = (GenBuf_t *) malloc(sizeof(GenBuf_t));
	if(*dataGenBuf == NULL) return NO_MEMORY;
	(*dataGenBuf)->buf = (char *) malloc (sizeof(unsigned char) * LARGE_FILE_DB_CHUNK_SIZE);
	if((*dataGenBuf)->buf == NULL) return NO_MEMORY;
	(*dataGenBuf)->size = sizeof(unsigned char) * LARGE_FILE_DB_CHUNK_SIZE;

	if(SQLQuery){free(SQLQuery); SQLQuery=NULL;}
	if (DefineHandle) {
		(void) OCIHandleFree((dvoid *) DefineHandle, (ub4) OCI_HTYPE_DEFINE);
		DefineHandle = NULL; }
	if (SqlStatementHandle) {
		(void) OCIHandleFree((dvoid *) SqlStatementHandle, (ub4) OCI_HTYPE_STMT);
		SqlStatementHandle = NULL; }
	return err;
}

long oracle_bmd_db_import_blob_continue(void *connection_handle,GenBuf_t *chunkGenBuf,void *blob_locator,
								 EnumDbWhichPiece_t which_piece)
{
long err = 0;
ub4 amtp = 0;
ub4 offset = 1;
OCI_bmd_db_conn_t *db_conn=NULL;
OCILobLocator *Lob_loc		= (OCILobLocator *) blob_locator;

	if( connection_handle == NULL )
		return BMD_ERR_PARAM1;
	if( (*(long *)connection_handle) != BMD_DBCONN_MAGIC )
		return BMD_ERR_INVALID_PTR;

	db_conn=(OCI_bmd_db_conn_t *)connection_handle;

	/* Wczytaj BLOBa do gen_buf */
	err = checkerr(db_conn->bmdHandles->ErrorHandle,
		OCILobRead(db_conn->bmdHandles->ServiceContextHandle,
			db_conn->bmdHandles->ErrorHandle,
			Lob_loc,
			&amtp, offset,
			chunkGenBuf->buf, chunkGenBuf->size,
			(dvoid *) NULL, NULL,
			(ub2) 0, (ub1) SQLCS_IMPLICIT)
	);
	if(err<0) 
	{
		if(err == ERR_OCI_NEED_DATA) 
		{
			return amtp;
		}
		else
		{
			PRINT_ERROR("LIBBMDOCIERR Error in reading LOB. RecievedErrorCode=%li.\n",err);
			if(Lob_loc) 
			{
				OCIDescriptorFree((dvoid *) Lob_loc, (ub4) OCI_DTYPE_LOB);
				Lob_loc = NULL;	
			}
			return BMD_ERR_OP_FAILED;
		}
	}
	else
		if( err == ERR_OCI_SUCCESS )
		{	
			return amtp;
		}
	return amtp;
}

long oracle_bmd_db_import_blob_end(void *connection_handle,void **blob_locator,GenBuf_t **dataGenBuf)
{
	long err = 0;
	OCILobLocator **Lob_loc		= (OCILobLocator **) blob_locator;

	if(*Lob_loc) 
	{
		OCIDescriptorFree((dvoid *) *Lob_loc, (ub4) OCI_DTYPE_LOB);
		*Lob_loc = NULL;
	}
	PRINT_ERROR("LIBBMDOCIINF LOB reading in POOLING_MODE finished succesfully.\n");
	return err;
}


/*****
 * Funkcja usuwa BLOBa o wskazanym przez blob_number identyfikatorze
 */
long oracle_bmd_db_delete_blob(void *hDB, char *blob_number)
{
	long err=0;
	char *SQLQuery=NULL;
	void *hRes=NULL;

	if(hDB == NULL)
		return BMD_ERR_PARAM1;
	if(blob_number == NULL )
		return BMD_ERR_PARAM2;
	if( (*(long *)hDB) != BMD_DBCONN_MAGIC )
		return BMD_ERR_INVALID_PTR;

	asprintf(&SQLQuery, "DELETE FROM or_largeobject WHERE id=%s", blob_number);
	if(SQLQuery==NULL) 
		return BMD_ERR_MEMORY;

	err=oracle_bmd_db_execute_sql(hDB,SQLQuery,NULL,NULL,&hRes);
	if( err != BMD_OK )
	{
		oracle_bmd_db_result_free(&hRes);
		PRINT_ERROR("LIBBMDOCIERR Error while deleting BLOB %li\n",err);
		return BMD_ERR_OP_FAILED;
	}
	
	oracle_bmd_db_result_free(&hRes);

	PRINT_ERROR("LIBBMDOCIINF[%s:%li:deleteLO] LOB has been succesfully removed.\n", __FILE__, __LINE__);
	return err;
}
