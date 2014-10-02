#include <bmd/libbmdsql/common.h>
#include <bmd/libbmdsql/cases.h>
#include <bmd/libbmdsql/proofs.h>
#include <bmd/libbmdprotocol/libbmdprotocol.h>
#include <bmd/libbmdsql/select.h>
#include <bmd/libbmdutils/libbmdutils.h>
#include <bmd/libbmddb/libbmddb.h>
#include <bmd/libbmdsql/common.h>
#include <bmd/libbmdsql/class.h>
#include <bmd/libbmdsql/update.h>
#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>
#include <bmd/libbmdglobals/libbmdglobals.h>

#ifdef WIN32
#pragma warning(disable:4100)
#pragma warning(disable:4127)
#endif

#include <signal.h>
#ifndef WIN32
	#include <sys/wait.h>
#endif //ifndef WIN32

long CreateAdditionalMetadataTable(	void *connection_handle,
					char *db_type);

long create_am_table(void *hDB)
{
char *db_type			= NULL;
char *db_host			= NULL;
char *db_port			= NULL;
char *db_name			= NULL;
char *db_user			= NULL;
char *db_password		= NULL;
long status;

	PRINT_INFO("BMDSERVERINF Creating Additional metadata history table...\n");
	if (hDB==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}

	if (bmd_db_connect_get_db_type(hDB) == BMD_DB_POSTGRES )
		asprintf(&db_type,"postgres");
	else
		if (bmd_db_connect_get_db_type(hDB) == BMD_DB_ORACLE )
			asprintf(&db_type,"oracle");
	else
		if (bmd_db_connect_get_db_type(hDB) == BMD_DB_DB2 )
			asprintf(&db_type,"db2");
	BMD_FOK_DB(bmd_db_start_transaction(hDB,BMD_TRANSACTION_LEVEL_NONE));
	status=CreateAdditionalMetadataTable(hDB,db_type);
	if ( status != BMD_OK )
	{
		PRINT_ERROR("CreateAdditionalMetadataTable returned %li\n",status);
		bmd_db_rollback_transaction(hDB,NULL);
		return BMD_ERR_OP_FAILED;
	}
	BMD_FOK(bmd_db_end_transaction(hDB));

	free0(db_type);free0(db_host);free0(db_port);free0(db_name);free0(db_user);free0(db_password);

	return BMD_OK;
}

long CreateAdditionalMetadataTable(	void *connection_handle,
					char *db_type)
{
long err 				= 0;
long fetched_counter			= 0;
long counter				= 0;
char *SQLQuery 				= NULL;
void *query_result			= NULL;
void *query_result2			= NULL;
db_row_strings_t *rowstruct 		= NULL;

char *addMetadataTypeId			= NULL;
char *addMetadataTypeLocationId		= NULL;
char *addMetadataTypeType		= NULL;
char *addMetadataTypeName		= NULL;
char *addMetadataTypeSqlCastString	= NULL;
char *addMetadataTypeSqlCastString2	= NULL;
char *addMetadataTypeDefaultValue	= NULL;


	/************************/
	/* walidacja parametrow */
	/************************/
	if (connection_handle==NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (db_type==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}

	/**********************************************************************************/
	/* wyszukanie wszystkich typow metadanych dodatkowych z tabeli add_metadata_types */
	/**********************************************************************************/
	asprintf(&SQLQuery, "SELECT id, location_id, type, name, sql_cast_string, default_value, index_regular, index_lower, index_varchar_pattern_ops, index_lower_varchar_pattern_ops FROM add_metadata_types;");
	if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}

	err=bmd_db_execute_sql(connection_handle, SQLQuery,NULL,NULL,&query_result);
	if (err<BMD_OK)
	{
		V_FATAL("Error getting record from add_metadata_types table.\n");
		printf("\n\n\nPlease run sql scripts numbered 1-5, and restart server...\n\n\n");
		return err;
	}
	free0(SQLQuery);

	while(err>=BMD_OK)
	{
		/*******************************************************/
		/* odczytanie poszczególnych danych z wyniku zapytania */
		/*******************************************************/
		err = bmd_db_result_get_row(connection_handle,query_result,0, 0, &rowstruct, FETCH_NEXT, &fetched_counter);
		if ((err==BMD_ERR_NODATA) && (counter>0)) {err=0; bmd_db_row_struct_free(&rowstruct); break;}
		else if (err==BMD_ERR_NODATA)
		{
			bmd_db_row_struct_free(&rowstruct);
			V_FATAL("Error getting record from add_metadata_types table.\n");
			printf("\n\nPlease run sql scripts numbered 1-5, and restart server...\n");
		}

		BMD_FOK(err);

		asprintf(&addMetadataTypeId,		"%s", rowstruct->colvals[0]);
		asprintf(&addMetadataTypeLocationId,	"%s", rowstruct->colvals[1]);
		asprintf(&addMetadataTypeType,		"%s", rowstruct->colvals[2]);
		asprintf(&addMetadataTypeName,		"%s", rowstruct->colvals[3]);
		asprintf(&addMetadataTypeSqlCastString,	"%s", rowstruct->colvals[4]);

		if ((rowstruct->colvals[5]!=NULL) && (strcmp(rowstruct->colvals[5], "")!=0))
		{
			asprintf(&addMetadataTypeDefaultValue, " DEFAULT %s", rowstruct->colvals[5]);
		}

		     if (strcmp(addMetadataTypeSqlCastString, "varchar")==0)		{	asprintf(&addMetadataTypeSqlCastString2, "(256)");	}
		/* metadane dodatkowe typu timestamp maja byc bez strefy czasowej
		*else if (strcmp(addMetadataTypeSqlCastString, "timestamp")==0)		{	asprintf(&addMetadataTypeSqlCastString2, " WITH time zone");	}
		*/

		BMD_FOK(bmd_str_replace(&addMetadataTypeType, ".", "_"));

		/**************************************/
		/* uzupelnianie tabeli crypto_objects */
		/**************************************/
		asprintf(&SQLQuery,	"ALTER TABLE crypto_objects ADD COLUMN amv_%s %s%s%s;",
					addMetadataTypeType,
					addMetadataTypeSqlCastString,
					addMetadataTypeSqlCastString2==NULL?"":addMetadataTypeSqlCastString2,
					addMetadataTypeDefaultValue==NULL?"":addMetadataTypeDefaultValue);
		BMD_FOK(bmd_db_execute_sql(connection_handle, SQLQuery,NULL,NULL,&query_result2));
		bmd_db_result_free(&query_result2);
		free0(SQLQuery);

		/*********************************************/
		/* tworzenie indexow w tabeli crytpo_objects */
		/*********************************************/
		/* index zwykly */
		if ((rowstruct->colvals[6]!=NULL) && (strcmp(rowstruct->colvals[6], "1")==0))
		{
			asprintf(&SQLQuery, 	"CREATE INDEX crypto_objects_amv_%s_index ON crypto_objects(amv_%s);",
						addMetadataTypeType,
						addMetadataTypeType);
			BMD_FOK(bmd_db_execute_sql(connection_handle, SQLQuery,NULL,NULL,&query_result2));
			bmd_db_result_free(&query_result2);
			free0(SQLQuery);
		}
		/* index lower */
		if ((rowstruct->colvals[7]!=NULL) && (strcmp(rowstruct->colvals[7], "1")==0) && (strcmp(addMetadataTypeSqlCastString, "varchar")==0))
		{
			asprintf(&SQLQuery, 	"CREATE INDEX crypto_objects_amv_%s_lower_index ON crypto_objects(lower(amv_%s));",
						addMetadataTypeType,
						addMetadataTypeType);
			BMD_FOK(bmd_db_execute_sql(connection_handle, SQLQuery,NULL,NULL,&query_result2));
			bmd_db_result_free(&query_result2);
			free0(SQLQuery);
		}
		/* index varchar opt */
		if ((rowstruct->colvals[8]!=NULL) && (strcmp(rowstruct->colvals[8], "1")==0) && (strcmp(addMetadataTypeSqlCastString, "varchar")==0))
		{
			asprintf(&SQLQuery, 	"CREATE INDEX crypto_objects_amv_%s_varchar_pattern_ops_index ON crypto_objects(amv_%s varchar_pattern_ops);",
						addMetadataTypeType,
						addMetadataTypeType);
			BMD_FOK(bmd_db_execute_sql(connection_handle, SQLQuery,NULL,NULL,&query_result2));
			bmd_db_result_free(&query_result2);
			free0(SQLQuery);
		}
		/* index lower varchar opt */
		if ((rowstruct->colvals[9]!=NULL) && (strcmp(rowstruct->colvals[9], "1")==0) && (strcmp(addMetadataTypeSqlCastString, "varchar")==0))
		{
			asprintf(&SQLQuery, 	"CREATE INDEX crypto_objects_amv_%s_lower_varchar_pattern_ops_index ON crypto_objects(lower(amv_%s) varchar_pattern_ops);",
						addMetadataTypeType,
						addMetadataTypeType);
			BMD_FOK(bmd_db_execute_sql(connection_handle, SQLQuery,NULL,NULL,&query_result2));
			bmd_db_result_free(&query_result2);
			free0(SQLQuery);
		}


		/********************************************/
		/* uzupelnianie tabeli add_metadata_history */
		/********************************************/
		asprintf(&SQLQuery,	"ALTER TABLE add_metadata_history ADD COLUMN amv_%s %s%s%s;",
					addMetadataTypeType,
					addMetadataTypeSqlCastString,
					addMetadataTypeSqlCastString2==NULL?"":addMetadataTypeSqlCastString2,
					addMetadataTypeDefaultValue==NULL?"":addMetadataTypeDefaultValue);
		BMD_FOK(bmd_db_execute_sql(connection_handle, SQLQuery,NULL,NULL,&query_result2));
		bmd_db_result_free(&query_result2);
		free0(SQLQuery);

		/***********************************/
		/* uzupelnianie tabeli role_rights */
		/***********************************/
		asprintf(&SQLQuery,	"ALTER TABLE role_rights ADD COLUMN rr_%s varchar(50) NOT NULL DEFAULT '000000000';",
					addMetadataTypeType);
		BMD_FOK(bmd_db_execute_sql(connection_handle, SQLQuery,NULL,NULL,&query_result2));
		bmd_db_result_free(&query_result2);
		free0(SQLQuery);


		asprintf(&SQLQuery, 	"CREATE INDEX role_rights_rr_%s_index ON role_rights(rr_%s);",
					addMetadataTypeType,
					addMetadataTypeType);
		BMD_FOK(bmd_db_execute_sql(connection_handle, SQLQuery,NULL,NULL,&query_result2));
		bmd_db_result_free(&query_result2);
		free0(SQLQuery);

		counter++;
		bmd_db_row_struct_free(&rowstruct);
		free0(addMetadataTypeId);
		free0(addMetadataTypeLocationId);
		free0(addMetadataTypeType);
		free0(addMetadataTypeName);
		free0(addMetadataTypeSqlCastString);
		free0(addMetadataTypeSqlCastString2);
		free0(addMetadataTypeDefaultValue);
	}

	/************/
	/* porzadki */
	/************/
	free0(addMetadataTypeId);
	free0(addMetadataTypeLocationId);
	free0(addMetadataTypeType);
	free0(addMetadataTypeName);
	free0(addMetadataTypeSqlCastString);
	free0(addMetadataTypeSqlCastString2);
	free0(addMetadataTypeDefaultValue);
	bmd_db_result_free(&query_result);
	bmd_db_result_free(&query_result2);
	bmd_db_row_struct_free(&rowstruct);

 	return BMD_OK;






// 	asprintf(&SQLQuery,"SELECT type FROM add_metadata_types;");
//
// 	BMD_FOK(bmd_db_execute_sql(connection_handle, SQLQuery,NULL,NULL,&query_result));
//
// 	fetched_counter = 0;
// 	while(err>=0)
// 	{
// 		/* zerowa kolumna zawiera OID - przekonwertuj string na tablice long */
// 		err = bmd_db_result_get_row(connection_handle,query_result,0, 0, &rowstruct, FETCH_NEXT, &fetched_counter);
// 		if (err==BMD_ERR_NODATA) {err=0; bmd_db_row_struct_free(&rowstruct); break;}
// 		if (err < 0)
// 		{
// 			BMD_FOK(err);
// 		}
// 		/* w kolumnie colvals nr 1 jest OIC a w colvals nr 0 jest ID */
//
// 		/*********** pobranie typu danych *************/
// 		asprintf(&SQLQuery2,"SELECT sql_cast_string FROM add_metadata_types WHERE type='%s';",rowstruct->colvals[0]);
// 		BMD_FOK(bmd_db_execute_sql(connection_handle, SQLQuery2,NULL,NULL, &query_result3));
// 		fetched_counter2=0;
//
// 		err = bmd_db_result_get_row(connection_handle,query_result3,0, 0, &rowstruct2, FETCH_NEXT, &fetched_counter2);
// 		if (err==BMD_ERR_NODATA) {err=0; bmd_db_row_struct_free(&rowstruct2); break;}
// 		if (err < 0)
// 		{
// 			BMD_FOK(err);
// 		}
// 		if (strcmp(rowstruct2->colvals[0],"undefined")==0)
// 		{
// 			strcpy(tmp2,"varchar");
// 		}
// 		else
// 		{
// 			strcpy(tmp2,rowstruct2->colvals[0]);
// 		}
// 		if (strcmp(tmp2,"varchar")==0)
// 		{
// 			strcat(tmp2,"(256)");
// 		}
//
// 		free(SQLQuery2); SQLQuery2=NULL;
// 		//free(query_result3); query_result3 = NULL;
// 		bmd_db_result_free(&query_result3);
// 		bmd_db_row_struct_free(&rowstruct2);
// 		/************************************************/
// 		BMD_FOK(CharReplace(rowstruct->colvals[0], &EscapedOid, '.', '_'));
//
// 		asprintf(&SQLChunk,
// 				"%s_%s	%s,\n",
// // 				"%s_%s	%s		%s DEFAULT %li,\n",
// 				colname_am_value, EscapedOid, tmp2);
// // 				colname_fk_am_types, EscapedOid, integer, fk_am_types, fetched_counter);
// 		if (SQLChunk == NULL)
// 		{
// 			return NO_MEMORY;
// 		}
//
// 		/******************************************/
// 		/*	tworzenie tabeli dla uprawnien roli	*/
// 		/******************************************/
// 		if ( bmd_db_connect_get_db_type(connection_handle) == BMD_DB_POSTGRES )
// 		{
// 			asprintf(&CreateRole, "%s, \nrr_%s varchar(50) NOT NULL DEFAULT '000000000'",
// 					 CreateRole, EscapedOid);
// 		}
// 		else
// 		{
// 			if ( bmd_db_connect_get_db_type(connection_handle) == BMD_DB_ORACLE )
// 			{
// 				asprintf(&CreateRole, "%s, \nrr_%s varchar(50) DEFAULT '000000000' NOT NULL",
// 						 CreateRole, EscapedOid);
// 			}
// 		}
//
// 		SQLAmValueIndexQuery = (char **) realloc (SQLAmValueIndexQuery, sizeof(char *) * (no_of_indexes+1));
// 		if (SQLAmValueIndexQuery == NULL)	{	BMD_FOK(NO_MEMORY);	}
//
// 		asprintf(&SQLAmValueIndexQuery[no_of_indexes], "CREATE INDEX amvi_%s ON add_metadata_history(%s_%s);",	EscapedOid, colname_am_value, EscapedOid);
// 		if (SQLAmValueIndexQuery == NULL)	{	BMD_FOK(NO_MEMORY);	}
// 		no_of_indexes++;
//
// 		if (fetched_counter == 1)
// 		{
// 			asprintf(&SQLFullQuery, "%s %s %s %s %s %s %s,\n%s %s %s %s %s %s,\n %s",
// 									CreateSQLPrefix1, integer,
// 									CreateSQLPrefix7, integer, " DEFAULT 0",
// 									CreateSQLPrefix2, integer, //fk_co, on_delcascade,
// 									CreateSQLPrefix3, "timestamp",
// 									CreateSQLPrefix4, "varchar(256)",
// // 									CreateSQLPrefix5, integer,
// 									CreateSQLPrefix6, "varchar(256)",
// 									SQLChunk);
// 			if (SQLFullQuery == NULL)	{	BMD_FOK(NO_MEMORY);	}
// 			free0(SQLChunk);
// 		}
// 		else
// 		{
// 			asprintf(&tmp, "%s", SQLFullQuery);
// 			if (tmp == NULL)		{	BMD_FOK(NO_MEMORY);	}
// 			free0(SQLFullQuery);
//
// 			asprintf(&SQLFullQuery, "%s%s", tmp, SQLChunk);
// 			free0(SQLChunk);
// 			free0(tmp);
//
// 		}
// 		bmd_db_row_struct_free(&rowstruct);
// 		free0(EscapedOid);
//
// 	}
// 	if (fetched_counter == 0)
// 	{
// 		PRINT_ERROR("ERR No additional metadata types present in add_metadata_types table so creating add_metadata table is not needed! Exiting now!\n");
// 		return err;
// 	}
//
// 	asprintf(&tmp, "%s", SQLFullQuery);
// 	if (tmp == NULL)
// 	{
// 		BMD_FOK(NO_MEMORY);
// 	}
// 	free(SQLFullQuery); SQLFullQuery=NULL;
//
// 	asprintf(&CreateRole, "%s,\nPRIMARY KEY(id, location_id ));",CreateRole);
//
// 	asprintf(&SQLFullQuery, "%sPRIMARY KEY(id, location_id ));", tmp);
// 	free(tmp); tmp = NULL;
// 	bmd_db_result_free(&query_result);
//
// 	/* Usun ostatni przeciniek ze stringu */
// 	SQLFullQuery[strlen(SQLFullQuery)-4] = ' ';
// PRINT_TEST(">>>>>>>>> PKL <<<<<<<<<<< SQLFullQuery: \n%s\n\n", SQLFullQuery);
// return BMD_OK;
// 	/* Teraz mozna juz wykonać wszystkie zapytania */
// 	BMD_FOK(bmd_db_execute_sql(connection_handle, SQLFullQuery,NULL,NULL, &query_result));
// 	bmd_db_result_free(&query_result);
//
// 	asprintf(&SQLSequenceQuery, "CREATE SEQUENCE add_metadata_id_seq %s;", seq_sufix);
// 	BMD_FOK(bmd_db_execute_sql( connection_handle, SQLSequenceQuery,NULL,NULL, &query_result));
// 	bmd_db_result_free(&query_result);
// 	free(SQLSequenceQuery); SQLSequenceQuery = NULL;
//
// 	BMD_FOK(bmd_db_execute_sql( connection_handle, "CREATE INDEX add_metadata_id_index ON add_metadata_history(id);", NULL,NULL, &query_result));
// 	bmd_db_result_free(&query_result);
//
// 	if ( bmd_db_connect_get_db_type(connection_handle) == BMD_DB_POSTGRES )
// 	{
// 		BMD_FOK(bmd_db_execute_sql( connection_handle, "CREATE INDEX add_mtd_id_loc_id_idx ON add_metadata_history(id,location_id);", NULL,NULL, &query_result));
// 		bmd_db_result_free(&query_result);
// 	}
//
// 	BMD_FOK(bmd_db_execute_sql(connection_handle, SQLCryptoObjectsIndexQuery,NULL,NULL, &query_result));
// 	bmd_db_result_free(&query_result);
//
// 	BMD_FOK(bmd_db_execute_sql(connection_handle, CreateRole, NULL,NULL, &query_result));
// 	bmd_db_result_free(&query_result);
//
// 	if ( bmd_db_connect_get_db_type(connection_handle) == BMD_DB_POSTGRES )
// 	{
// 		BMD_FOK(bmd_db_execute_sql(connection_handle,
// 								   "CREATE SEQUENCE role_rights_id_seq INCREMENT 1 MINVALUE 1;",
// 								   NULL,NULL, &query_result));
// 	}
// 	else
// 	{
// 		BMD_FOK(bmd_db_execute_sql(connection_handle,
// 								   "CREATE SEQUENCE role_rights_id_seq INCREMENT BY 1 MINVALUE 1;",
// 								   NULL,NULL, &query_result));
// 	}
// 	bmd_db_result_free(&query_result);
//
// 	BMD_FOK(bmd_db_execute_sql(connection_handle, "CREATE INDEX role_rights_fk_roles_index ON role_rights (fk_roles);", NULL,NULL, &query_result));
// 	bmd_db_result_free(&query_result);
//
// 	BMD_FOK(bmd_db_execute_sql( connection_handle, "CREATE INDEX role_rights_id_index ON role_rights(id);", NULL,NULL, &query_result));
// 	bmd_db_result_free(&query_result);
//
// 	if ( bmd_db_connect_get_db_type(connection_handle) == BMD_DB_POSTGRES )
// 	{
// 		BMD_FOK(bmd_db_execute_sql( connection_handle, "CREATE INDEX r_r_id_loc_id_idx ON role_rights(id,location_id);", NULL,NULL, &query_result));
// 		bmd_db_result_free(&query_result);
// 	}
// 	/**************************************************/
// 	/* stworzenie indeksow dla metadanych dodatkowych */
// 	/**************************************************/
// 	for(i=0; i<no_of_indexes; i++)
// 	{
// 		BMD_FOK(bmd_db_execute_sql(connection_handle, SQLAmValueIndexQuery[i],NULL,NULL, &query_result2));
// 		bmd_db_result_free(&query_result2);
// 		free(SQLAmValueIndexQuery[i]); SQLAmValueIndexQuery[i]=NULL;
// 	}
//
// 	/***********************************************/
// 	/* stworzenie indexow na stale elementy tabeli */
// 	/***********************************************/
// 	if ( bmd_db_connect_get_db_type(connection_handle) == BMD_DB_POSTGRES )
// 	{
// 		BMD_FOK(bmd_db_execute_sql(connection_handle, "CREATE INDEX am_id_index ON add_metadata_history (id);" ,NULL,NULL, &query_result2));
// 		bmd_db_result_free(&query_result2);
// 	}
//
// 	BMD_FOK(bmd_db_execute_sql(connection_handle, "CREATE INDEX am_insert_date_index ON add_metadata_history (insert_date);" ,NULL,NULL, &query_result2));
// 	bmd_db_result_free(&query_result2);
//
// 	BMD_FOK(bmd_db_execute_sql(connection_handle, "CREATE INDEX am_insert_user_dn_index ON add_metadata_history (insert_user_dn);" ,NULL,NULL, &query_result2));
// 	bmd_db_result_free(&query_result2);
//
// 	BMD_FOK(bmd_db_execute_sql(connection_handle, "CREATE INDEX am_insert_reason_index ON add_metadata_history (insert_reason);" ,NULL,NULL, &query_result2));
// 	bmd_db_result_free(&query_result2);
//
//
// 	free0(SQLAmValueIndexQuery);
// 	free0(SQLFullQuery);
// 	free0(SQLAmValueIndexQuery);
// 	free0(CreateRole);
//
// 	return BMD_OK;

}

long InsertStringToGenBufAlloc(	char *string,
				GenBuf_t **GenBuf)
{
	PRINT_INFO("LIBBMDSQLINF Inserting string to GenBuf\n");

	*GenBuf = (GenBuf_t *) malloc (sizeof(GenBuf_t));
	if (*GenBuf == NULL)
	{
		BMD_FOK(NO_MEMORY);
	}

	asprintf((char **)&((*GenBuf)->buf),"%s", string);

	if ((*GenBuf)->buf == NULL)
	{
		if (*GenBuf)
		{
			free(*GenBuf);
			*GenBuf=NULL;
		}
		BMD_FOK(NO_MEMORY);
	}
	(*GenBuf)->size = 1+(long)strlen(string);

	return BMD_OK;
}


/**********************************************/
/* funkcja wypelnia datagram metadanymi akcji */
/**********************************************/
long FillFromActionMetaData(	void *hDB,
				char *CryptoObjectNr,
				char *location_id,
				bmdDatagram_t *bmdJSResponse)
{
char *SQLQuery			= NULL;
void *database_result		= NULL;
db_row_strings_t *rowstruct	= NULL;
char *SQLCerts			= NULL;
char *tmp			= NULL;
long tmp_fk_actions		= 0;
long tmp_status			= 0;
long err			= 0;
long fetched_counter		= 0;
long cols			= 0;
char *first_get_status_msg	= NULL;
long first_get_status		= 0;
char *last_get_status_msg	= NULL;
long last_get_status		= 0;
char *first_send_status_msg	= NULL;
long first_send_status		= 0;
char *last_send_status_msg	= NULL;
long last_send_status		= 0;
// long ansCountCertsId		= 0;
// char **ansCertsId		= NULL;
// long i				= 0;
char *successStatusMsg		= NULL;
char *failureStatusMsg		= NULL;

GenBuf_t tmp_gb;
char* escapeTemp			= NULL;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (hDB==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (CryptoObjectNr==NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

// 	if (_GLOBAL_bmd_configuration->user_cert_time_registration!=NULL)
// 	{
// 		asprintf(&tmp, "%s", _GLOBAL_bmd_configuration->user_cert_time_registration);
// 		if (tmp==NULL)		{	BMD_FOK(NO_MEMORY);	}
//
// 		BMD_FOK(bmd_str_replace(&tmp, ";", "' OR cert_dn||cert_serial='"));
//
// 		asprintf(&SQLCerts, "SELECT id FROM users WHERE cert_dn||cert_serial='%s' AND location_id=%s;", tmp, location_id);
// 		if (SQLCerts==NULL)	{	BMD_FOK(NO_MEMORY);	}
//
// 		ExecuteSQLQueryWithAnswersKnownDBConnection(	hDB, SQLCerts, &ansCountCertsId, &ansCertsId);
// 	}

	BMD_FOK(bmd_db_escape_string(hDB, CryptoObjectNr, STANDARD_ESCAPING, &escapeTemp));
	asprintf(&SQLQuery, "SELECT fk_actions, status, status_msg, action_date, fk_users FROM action_register WHERE fk_objects=%s%s%s AND objects_type=%i ORDER BY action_date ASC;",
				escapeTemp,
				location_id==NULL?"":" AND location_id=",
				location_id==NULL?"":location_id,
				CRYPTO_OBJECT_OBJECT_TYPE);
	free(escapeTemp); escapeTemp=NULL;

	if (SQLQuery==NULL)		{	BMD_FOK(NO_MEMORY);	}

	BMD_FOK(bmd_db_execute_sql(hDB, SQLQuery,NULL, &cols, &database_result));
	free0(SQLQuery);
	free0(SQLCerts);

	asprintf(&successStatusMsg, "OK");
	if (successStatusMsg==NULL)	{	BMD_FOK(NO_MEMORY);	}

	asprintf(&failureStatusMsg, "FAIL");
	if (failureStatusMsg==NULL)	{	BMD_FOK(NO_MEMORY);	}

	fetched_counter = 0;

	while(err>=0)
	{
		/******************************************************/
		/*	pobranie calego wiersza z wyniku wyszukiwania	*/
		/******************************************************/
		err = bmd_db_result_get_row(hDB, database_result, 0, 0, &rowstruct, FETCH_NEXT, &fetched_counter);
		if (err==BMD_ERR_NODATA)
		{
			err=0;
			bmd_db_row_struct_free(&rowstruct);
			break;
		}
		if (err < 0)
		{
			BMD_FOK(err);
		}

		tmp_fk_actions = strtol(rowstruct->colvals[0], NULL, 10);
		tmp_status = strtol(rowstruct->colvals[1], NULL, 10);

		if (	(tmp_fk_actions==BMD_DATAGRAM_TYPE_GET) ||
			(tmp_fk_actions==BMD_DATAGRAM_TYPE_GET_LO) ||
			(tmp_fk_actions==BMD_DATAGRAM_GET_CGI) ||
			(tmp_fk_actions==BMD_DATAGRAM_GET_CGI_LO) ||
			(tmp_fk_actions==BMD_DATAGRAM_TYPE_GET_ARCHIVE_FILE_VERSION_LO) ||
			(tmp_fk_actions==BMD_DATAGRAM_GET_ONLY_FILE))
		{
			if (tmp_status==BMD_OK)
			{
				if ((first_get_status_msg==NULL) || (strncmp(first_get_status_msg, failureStatusMsg, strlen(failureStatusMsg))==0))
				{
// 					if (ansCountCertsId>0)
// 					{
// 						for (i=0; i<ansCountCertsId; i++)
// 						{
// 							if (strcmp(rowstruct->colvals[4], ansCertsId[i])==0)
// 							{
// 								asprintf(&first_get_status_msg, "%s (%s)", successStatusMsg, rowstruct->colvals[3]);
// 								if (first_get_status_msg==NULL)	{	BMD_FOK(NO_MEMORY);	}
// 								first_get_status=tmp_status;
// 								break;
// 							}
// 						}
// 					}
// 					else
// 					{
					asprintf(&first_get_status_msg, "%s (%s)", successStatusMsg, rowstruct->colvals[3]);
					if (first_get_status_msg==NULL)	{	BMD_FOK(NO_MEMORY);	}
					first_get_status=tmp_status;
// 					}
				}

				free(last_get_status_msg); last_get_status_msg=NULL;
				asprintf(&last_get_status_msg, "%s (%s)", successStatusMsg, rowstruct->colvals[3]);
				if (last_get_status_msg==NULL)	{	BMD_FOK(NO_MEMORY);	}
				last_get_status=tmp_status;
			}
			else
			{
				if (first_get_status_msg==NULL)
				{
// 					if (ansCountCertsId>0)
// 					{
// 						for (i=0; i<ansCountCertsId; i++)
// 						{
// 							if (strcmp(rowstruct->colvals[4], ansCertsId[i])==0)
// 							{
// 								asprintf(&first_get_status_msg, "%s (%s)", failureStatusMsg, rowstruct->colvals[3]);
// 								if (first_get_status_msg==NULL)	{	BMD_FOK(NO_MEMORY);	}
// 								first_get_status=tmp_status;
// 								break;
// 							}
// 						}
// 					}
// 					else
// 					{

					free(first_get_status_msg); first_get_status_msg=NULL;
					asprintf(&first_get_status_msg, "%s (%s)", failureStatusMsg, rowstruct->colvals[3]);
					if (first_get_status_msg==NULL)	{	BMD_FOK(NO_MEMORY);	}
					first_get_status=tmp_status;
// 					}
				}

				if (((last_get_status_msg!=NULL) && (strncmp(last_get_status_msg, failureStatusMsg, strlen(failureStatusMsg))==0)) || (last_get_status_msg==NULL))
				{
					free(last_get_status_msg); last_get_status_msg=NULL;
					asprintf(&last_get_status_msg, "%s (%s)", failureStatusMsg, rowstruct->colvals[3]);
					if (last_get_status_msg==NULL)	{	BMD_FOK(NO_MEMORY);	}
					last_get_status=tmp_status;
				}
			}
		}

		if(	tmp_fk_actions==BMD_DATAGRAM_TYPE_SEND_AVIZO ||
			tmp_fk_actions==BMD_DATAGRAM_TYPE_SEND_ADVICE_WITH_PARAMETERS )
		{
			if (tmp_status==BMD_OK)
			{
				if ((first_send_status_msg==NULL) || (strncmp(first_send_status_msg, failureStatusMsg, strlen(failureStatusMsg))==0))
				{
// 					if (ansCountCertsId>0)
// 					{
// 						for (i=0; i<ansCountCertsId; i++)
// 						{
// 							if (strcmp(rowstruct->colvals[4], ansCertsId[i])==0)
// 							{
// 								asprintf(&first_send_status_msg, "%s (%s)", successStatusMsg, rowstruct->colvals[3]);
// 								if (first_send_status_msg==NULL)	{	BMD_FOK(NO_MEMORY);	}
// 								first_send_status=tmp_status;
// 								break;
// 							}
// 						}
// 					}
// 					else
// 					{

					free(first_send_status_msg); first_send_status_msg=NULL;
					asprintf(&first_send_status_msg, "%s (%s)", successStatusMsg, rowstruct->colvals[3]);
					if (first_send_status_msg==NULL)	{	BMD_FOK(NO_MEMORY);	}
					first_send_status=tmp_status;
// 					}
				}

				free(last_send_status_msg); last_send_status_msg=NULL;
				asprintf(&last_send_status_msg, "%s (%s)", successStatusMsg, rowstruct->colvals[3]);
				if (last_send_status_msg==NULL)	{	BMD_FOK(NO_MEMORY);	}
				last_send_status=tmp_status;

			}
			else
			{
				if (first_send_status_msg==NULL)
				{
// 					if (ansCountCertsId>0)
// 					{
// 						for (i=0; i<ansCountCertsId; i++)
// 						{
// 							if (strcmp(rowstruct->colvals[4], ansCertsId[i])==0)
// 							{
// 								asprintf(&first_send_status_msg, "%s (%s)", failureStatusMsg, rowstruct->colvals[3]);
// 								if (first_send_status_msg==NULL)	{	BMD_FOK(NO_MEMORY);	}
// 								first_send_status=tmp_status;
// 								break;
// 							}
// 						}
// 					}
// 					else
// 					{

					free(first_send_status_msg); first_send_status_msg=NULL;
					asprintf(&first_send_status_msg, "%s (%s)", failureStatusMsg, rowstruct->colvals[3]);
					if (first_send_status_msg==NULL)	{	BMD_FOK(NO_MEMORY);	}
					first_send_status=tmp_status;
// 					}
				}

				if (((last_send_status_msg!=NULL) && (strncmp(last_send_status_msg, failureStatusMsg, strlen(failureStatusMsg))==0)) || (last_send_status_msg==NULL))
				{

					free(last_send_status_msg); last_send_status_msg=NULL;
					asprintf(&last_send_status_msg, "%s (%s)", failureStatusMsg, rowstruct->colvals[3]);
					if (last_send_status_msg==NULL)	{	BMD_FOK(NO_MEMORY);	}
					last_send_status=tmp_status;

				}
			}
		}

		bmd_db_row_struct_free(&rowstruct);
	}

	if (first_get_status_msg==NULL)
	{
		asprintf(&first_get_status_msg, "%s", failureStatusMsg);
		if (first_get_status_msg==NULL)	{	BMD_FOK(NO_MEMORY);	}
	}

	if (last_get_status_msg==NULL)
	{
		asprintf(&last_get_status_msg, "%s", failureStatusMsg);
		if (last_get_status_msg==NULL)	{	BMD_FOK(NO_MEMORY);	}
	}

	if (first_send_status_msg==NULL)
	{
		asprintf(&first_send_status_msg, "%s", failureStatusMsg);
		if (first_send_status_msg==NULL)	{	BMD_FOK(NO_MEMORY);	}
	}

	if (last_send_status_msg==NULL)
	{
		asprintf(&last_send_status_msg, "%s", failureStatusMsg);
		if (last_send_status_msg==NULL)	{	BMD_FOK(NO_MEMORY);	}
	}

	tmp_gb.buf=first_get_status_msg;
	tmp_gb.size=(long)strlen(first_get_status_msg);
	BMD_FOK(bmd_datagram_add_metadata_2_with_params(OID_ACTION_METADATA_DOCUMENT_GET_FIRST, &tmp_gb, &bmdJSResponse, ACTION_METADATA, first_get_status, 0, 0,0));

	tmp_gb.buf=last_get_status_msg;
	tmp_gb.size=(long)strlen(last_get_status_msg);
	BMD_FOK(bmd_datagram_add_metadata_2_with_params(OID_ACTION_METADATA_DOCUMENT_GET_LAST, &tmp_gb, &bmdJSResponse, ACTION_METADATA, last_get_status, 0, 0,0));

	tmp_gb.buf=first_send_status_msg;
	tmp_gb.size=(long)strlen(first_send_status_msg);
	BMD_FOK(bmd_datagram_add_metadata_2_with_params(OID_ACTION_METADATA_AWIZO_SENT_FIRST, &tmp_gb, &bmdJSResponse, ACTION_METADATA, first_send_status, 0, 0,0));

	tmp_gb.buf=last_send_status_msg;
	tmp_gb.size=(long)strlen(last_send_status_msg);
	BMD_FOK(bmd_datagram_add_metadata_2_with_params(OID_ACTION_METADATA_AWIZO_SENT_LAST, &tmp_gb, &bmdJSResponse, ACTION_METADATA, last_send_status, 0, 0,0));


	/************/
	/* porzadki */
	/************/
/*
a co to ponizej - to bez sensu zupelnie
#ifndef WIN32
	bmd_db_result_free(&database_result);
#else
	printf("Nie zaimplementowane",__FILE__,__LINE__);
#endif
*/
	bmd_db_result_free(&database_result);
	free0(SQLCerts);
	free0(tmp);
// 	for (i=0; i<ansCountCertsId; i++)
// 	{
// 		free0(ansCertsId[i]);
// 	}
// 	free0(ansCertsId);
	free0(successStatusMsg);
	free0(failureStatusMsg);
	free0(SQLQuery);
	free0(first_get_status_msg);
	free0(last_get_status_msg);
	free0(first_send_status_msg);
	free0(last_send_status_msg);

	return BMD_OK;
}


/************************************************************/
/*	funkcja wypelnia datagram metadanymi dodatkowymi	*/
/************************************************************/
long FillFromAdditionalMetaData(	void *hDB,
					char *CryptoObjectNr,
					char *location_id,
					bmdDatagram_t *bmdJSResponse,
					server_request_data_t *req)
{
char *SQLQuery		= NULL;
char *SQLQuery2		= NULL;
char **ans		= NULL;
char **ans2		= NULL;
GenBuf_t *tmp_gb	= NULL;
long ans_count		= 0;
long ans_count2		= 0;
long i			= 0;
long j			= 0;
char* escapeTemp	= NULL;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (hDB==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (CryptoObjectNr==NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	/***********************************************/
	/* pobranie listy oidow metadanych dodatkowych */
	/***********************************************/
	//SELECT id, type, name, sql_cast_string FROM add_metadata_types;;
	BMD_FOK(getColumnWithCondition(	_GLOBAL_AddMetadataTypesDictionary, 0, NULL, 1, &ans_count, &ans));
	if (ans_count==0)			{	BMD_FOK(BMD_ERR_NODATA);	}

	for (i=0; i<ans_count; i++)
	{
		bmd_str_replace(&(ans[i]), ".", "_");
		free(SQLQuery2); SQLQuery2=NULL;
		asprintf(&SQLQuery2, "%s", SQLQuery);
		free(SQLQuery); SQLQuery=NULL;
		if (i==0)
		{
			asprintf(&SQLQuery, "SELECT amv_%s", ans[i]);
		}
		else
		{
			asprintf(&SQLQuery, "%s, amv_%s", SQLQuery2, ans[i]);
		}
	}

	free(SQLQuery2); SQLQuery2=NULL;
	asprintf(&SQLQuery2, "%s", SQLQuery);
	free(SQLQuery); SQLQuery=NULL;
	
	BMD_FOK(bmd_db_escape_string(hDB, CryptoObjectNr, STANDARD_ESCAPING, &escapeTemp));
	asprintf(&SQLQuery, "	%s FROM crypto_objects WHERE id=%s%s%s;",
					SQLQuery2,
					escapeTemp,
					location_id==NULL?"":" AND location_id=",
					location_id==NULL?"":location_id);
	free(SQLQuery2); SQLQuery2=NULL;
	free(escapeTemp); escapeTemp=NULL;

	/**********************************************/
	/* pobranie wartosci metadanych z bazy danych */
	/**********************************************/
	ExecuteSQLQueryWithRowAnswersKnownDBConnection(hDB, SQLQuery, &ans_count2, &ans2);
	if (ans_count2==0)			{	BMD_FOK(BMD_ERR_NODATA);	}

	for (i=0; i<ans_count2; i++)
	{

		bmd_str_replace(&(ans[i]), "_", ".");

		for (j=0; j<req->no_of_formularz; j++)
		{
			if ( (strcmp(req->formularz[j]->oid_str, ans[i])==0) &&
				(req->formularz[j]->maxLength>-1) &&
				((long)strlen(ans2[i])>req->formularz[j]->maxLength) )
			{
				ans2[i][req->formularz[j]->maxLength]='\0';
				break;
			}
		}

		if ((ans2[i]!=NULL) && (strlen(ans2[i])>0))
		{
			BMD_FOK(set_gen_buf2 (ans2[i], (long)strlen(ans2[i]), &tmp_gb));
		}
		else
		{
			BMD_FOK(set_empty_gen_buf (&tmp_gb));
		}

		BMD_FOK(bmd_datagram_add_metadata_2(ans[i], tmp_gb, &bmdJSResponse,1));
		free_gen_buf(&tmp_gb);

	}

	/************/
	/* porzadki */
	/************/
	free(SQLQuery); SQLQuery=NULL;
	free(SQLQuery2); SQLQuery2=NULL;
	for (i=0; i<ans_count2; i++)
	{
		free(ans[i]); ans[i]=NULL;
		free(ans2[i]); ans2[i]=NULL;
	}
	free(ans); ans=NULL;
	free(ans2); ans2=NULL;
	free_gen_buf(&tmp_gb);

	return BMD_OK;
}

/************************************************************************************************/
/*	funkcja wypelnia datagram historycznymi metadanymi dodatkowymi o podanym id metadanych	*/
/************************************************************************************************/
long FillFromAdditionalMetaDataHistory(	void *hDB,
					char *MetadataId,
					char *location_id,
					bmdDatagram_t *bmdJSPrevResponse,
					bmdDatagram_t *bmdJSResponse,
					server_request_data_t *req)
{
long err				= 0;
long cols				= 0;
long am_cols				= 0;
long step_cols				= 1;
long i 					= 0;
long j					= 0;
long fetched_counter1			= 0;
char *SQLQuery				= NULL;
void *database_result			= NULL;
db_row_strings_t *rowstruct		= NULL;
GenBuf_t *tmp_gb			= NULL;
char *tmp_oid 				= NULL;
char *updateDate			= NULL;
char *updateOwner			= NULL;
char *updateReason			= NULL;
char **ans				= NULL;
long ans_count				= 0;
char* escapeTemp			= NULL;

	PRINT_INFO("LIBBMDSQLINF Filling additional metadata.\n");
	/************************/
	/* walidacja parametrow */
	/*************************/
	if (hDB == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (MetadataId==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (bmdJSResponse == NULL)	{	BMD_FOK(BMD_ERR_PARAM3);	}


	/**********************************************************************************/
	/* zapytanie o caly wiersz metadanych dodatkowych na podstawie add_metadata_types */
	/**********************************************************************************/
	//SELECT id, type, name, sql_cast_string FROM add_metadata_types;;
	BMD_FOK(getColumnWithCondition(	_GLOBAL_AddMetadataTypesDictionary, 0, NULL, 1, &ans_count, &ans));

	asprintf(&SQLQuery, "SELECT id, fk_crypto_objects, to_char(insert_date, 'YYYY-MM-DD HH24:MI:SS'), insert_user_dn, insert_reason");
	if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}

	for (i=0; i<ans_count; i++)
	{
		bmd_str_replace(&(ans[i]),".","_");
		asprintf(&SQLQuery, "%s, amv_%s ", SQLQuery, ans[i]);
	}
	
	BMD_FOK(bmd_db_escape_string(hDB, MetadataId, STANDARD_ESCAPING, &escapeTemp));
	asprintf(&SQLQuery, "%s FROM add_metadata_history WHERE id=%s;", SQLQuery, escapeTemp);
	free(escapeTemp); escapeTemp=NULL;

	BMD_FOK(bmd_db_execute_sql(hDB, SQLQuery,NULL, &cols, &database_result));
	free(SQLQuery); SQLQuery=NULL;

	am_cols = cols-5;
	fetched_counter1 = 0;
	err=0;

	while(err>=0)
	{
		/******************************************************/
		/*	pobranie calego wiersza z wyniku wyszukiwania	*/
		/******************************************************/
		err = bmd_db_result_get_row(hDB, database_result, 0, 0, &rowstruct, FETCH_NEXT, &fetched_counter1);
		if (err==BMD_ERR_NODATA)
		{
			err=0;
			bmd_db_row_struct_free(&rowstruct);
			break;
		}
		if (err < 0)
		{
			BMD_FOK(err);
		}

		/************************************************************************************************/
		/*	wstawienie do datagramu informacji o dacie, autorze i powodzie modyfikacji metadanych	*/
		/************************************************************************************************/
		asprintf(&updateDate, "%s", rowstruct->colvals[2]);
		asprintf(&updateOwner, "%s", rowstruct->colvals[3]);
		asprintf(&updateReason, "%s", rowstruct->colvals[4]);
		updateDate[19]='\0';
		asprintf(&updateDate, "%s|%s", updateDate, updateReason);

		bmdJSPrevResponse->protocolData=(GenBuf_t *)malloc(sizeof(GenBuf_t));
		bmdJSPrevResponse->protocolData->size=(long)strlen(updateDate);
		bmdJSPrevResponse->protocolData->buf=(char *)malloc(strlen(updateDate)+2);
		memset(bmdJSPrevResponse->protocolData->buf, 0, strlen(updateDate)+1);
		memcpy(bmdJSPrevResponse->protocolData->buf, updateDate, strlen(updateDate));

		bmdJSPrevResponse->protocolDataOwner=(GenBuf_t *)malloc(sizeof(GenBuf_t));
		bmdJSPrevResponse->protocolDataOwner->size=(long)strlen(updateOwner);
		bmdJSPrevResponse->protocolDataOwner->buf=(char *)malloc(strlen(updateOwner)+2);
		memset(bmdJSPrevResponse->protocolDataOwner->buf, 0, strlen(updateOwner)+1);
		memcpy(bmdJSPrevResponse->protocolDataOwner->buf, updateOwner, strlen(updateOwner));

		/************************************************************************************************************/
		/*	umiesc w datagramie tylko co 6 kolumne (pierwsze dwie to klucz podstawowy i obcy do crypto obcjects	*/
		/************************************************************************************************************/
		for(i=0; i<am_cols/step_cols; i++)
		{
			/********************************************/
			/* sprawdzenie dlugosci zwracanej metadanej */
			/********************************************/
			bmd_str_replace(&(ans[i]),"_",".");
			for (j=0; j<req->no_of_formularz; j++)
			{
				if ( (strcmp(req->formularz[j]->oid_str, ans[i])==0) &&
					(req->formularz[j]->maxLength>-1) &&
					((long)strlen(rowstruct->colvals[i+5])>req->formularz[j]->maxLength) )
				{
					rowstruct->colvals[i+5][req->formularz[j]->maxLength]='\0';
					break;
				}
			}

			if ((rowstruct->colvals[i+5]!=NULL) && (strlen(rowstruct->colvals[i+5])>0))
			{
				BMD_FOK(set_gen_buf2 (rowstruct->colvals[i+5], (long)strlen(rowstruct->colvals[i+5]), &tmp_gb));
				BMD_FOK(bmd_datagram_add_metadata_2(ans[i], tmp_gb, &bmdJSResponse,1));

				free_gen_buf(&tmp_gb);
			}

			free(tmp_oid); tmp_oid=NULL; /* tego nie zwalniamy */
		}
		bmd_db_row_struct_free(&rowstruct);
	}

	/******************/
	/*	porzadki	*/
	/******************/
	for (i=0; i<ans_count; i++)
	{
		free0(ans[i]);
	}
	free0(ans);

	free0(SQLQuery);
	bmd_db_result_free(&database_result);
	bmd_db_row_struct_free(&rowstruct);
	free_gen_buf(&tmp_gb);
	free0(tmp_oid);
	free0(updateDate);
	free0(updateOwner);
	free0(updateReason);

	return BMD_OK;
}


/************************************************************************/
/*	funkcja wypelnia protocolData jesli nie ma transmisji lobowej	*/
/*	lub wypelnia struktura do transmisji lobowej				*/
/************************************************************************/
long FillFromProtocolData(	void *hDB,
				char *CryptoObjectNr,
				char *location_id,
				LobsInDatagram_t LobsInDatagram,
				bmdFileVersion_e fileVersion,
				bmdDatagram_t *bmdJSResponse,
				JS_return_t *js_ret)
{
char *SQLQuery				= NULL;
void *database_result			= NULL;
char *tmp				= NULL;
char *lo_oid				= NULL;
char* escapeTemp			= NULL;


	PRINT_INFO("LIBBMDSQLINF Filling protocol data.\n");
	/************************/
	/* walidacja parametrow */
	/************************/
	if (hDB == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (CryptoObjectNr==NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (bmdJSResponse == NULL)	{	BMD_FOK(BMD_ERR_PARAM3);	}

	/*******************************************/
	/* pobranie oid'a rastra pobieranego pliku */
	/*******************************************/
	BMD_FOK(bmd_db_escape_string(hDB, CryptoObjectNr, STANDARD_ESCAPING, &escapeTemp));
	if(fileVersion == CURRENT_FILE_VERSION)
	{
		asprintf(&SQLQuery, "SELECT raster FROM crypto_objects WHERE id=%s AND location_id=%s;", escapeTemp, location_id);
	}
	else // ARCHIVE_VERSION
	{
		asprintf(&SQLQuery, "SELECT raster FROM crypto_objects_history WHERE id=%s AND location_id=%s;", escapeTemp, location_id);
	}
	free(escapeTemp); escapeTemp=NULL;
	BMD_FOK(ExecuteSQLQueryWithAnswerKnownDBConnection( hDB, SQLQuery, &lo_oid));

	if (lo_oid!=NULL)
	{
		if (js_ret)
		{
			if (js_ret->oid!=NULL)
			{
				asprintf(&(js_ret->oid[js_ret->cur_index]), "%s", lo_oid);
			}
		}

		if (LobsInDatagram == LOB_IN_DATAGRAM)
		{
			BMD_FOK(bmd_db_import_blob(hDB, lo_oid, &(bmdJSResponse->protocolData)));
		}
	}
	else
	{
		bmdJSResponse->protocolData = NULL;
	}

	/************/
	/* porzadki */
	/************/
	free0(SQLQuery);
	free0(tmp);
	free0(lo_oid);
	bmd_db_result_free(&database_result);

	return BMD_OK;
}


/**
* @author WSz
* @brief Funkcja wyszukuje w datagramie zrodlowym parametry wysylki awizo (tj. nazwe szablonu i tytul wiadomosci) i jesli znajdzie, to przekopiowuje do docelowego datagramu
* @note funkcja stosowana do przekopiowania parametrow z zadania do przygotowywanej odpowiedzi
* @param srcDatagram[in] datagram w z ktorego metadane (parametry wysylki awizo) zostana przekopiowane
* @param destDatagram[out] datagram, ktory zostanie uzupelniany parametrami wysylki awizo
* @return BMD_OK w przypadku porawnego wykonania funkcji albo wartosc ujemna (kod bledu)
*/
long CopySendAdviceParameters(bmdDatagram_t *srcDatagram, bmdDatagram_t* destDatagram)
{
long iter = 0;

	if(srcDatagram == NULL)
		{ BMD_FOK(BMD_ERR_PARAM1); }
	if(srcDatagram->no_of_sysMetaData > 0 && srcDatagram->sysMetaData == NULL)
		{ BMD_FOK(BMD_ERR_PARAM1); }
	if(destDatagram == NULL)
		{ BMD_FOK(BMD_ERR_PARAM2); }


	for(iter = 0; iter < srcDatagram->no_of_sysMetaData; iter++)
	{
		if(srcDatagram->sysMetaData[iter] == NULL)
		{
			BMD_FOK(BMD_ERR_OP_FAILED);
		}

		if(srcDatagram->sysMetaData[iter]->OIDTableBuf == NULL)
			{ continue; }

		if(strcmp(srcDatagram->sysMetaData[iter]->OIDTableBuf, OID_SYS_METADATA_SEND_ADVICE_PARAM_TEMPLATE_NAME) == 0)
		{
			if(srcDatagram->sysMetaData[iter]->AnySize > 0 && srcDatagram->sysMetaData[iter]->AnyBuf != NULL)
			{
				BMD_FOK(bmd_datagram_add_metadata_char(OID_SYS_METADATA_SEND_ADVICE_PARAM_TEMPLATE_NAME, srcDatagram->sysMetaData[iter]->AnyBuf, &destDatagram));
			}
		}
		else if(strcmp(srcDatagram->sysMetaData[iter]->OIDTableBuf, OID_SYS_METADATA_SEND_ADVICE_PARAM_TITLE) == 0)
		{
			if(srcDatagram->sysMetaData[iter]->AnySize > 0 && srcDatagram->sysMetaData[iter]->AnyBuf != NULL)
			{
				BMD_FOK(bmd_datagram_add_metadata_char(OID_SYS_METADATA_SEND_ADVICE_PARAM_TITLE, srcDatagram->sysMetaData[iter]->AnyBuf, &destDatagram));
			}
		}
	}

	return BMD_OK;
}


long FillFromCryptoObjects(	void *hDB,
				char *CryptoObjectNr,
				char *location_id,
				EnumWithSysMetadata_t WithSysMetadata,
				bmdDatagram_t *bmdJSResponse,
				server_request_data_t *req)
{
char **ans		= NULL;
char *SQLQuery		= NULL;
char *tmp_oid		= NULL;
GenBuf_t *tmp_gb	= NULL;
//long symkey_loid	= -1;
long ans_count		= 0;
long i			= 0;
long j			= 0;
char* escapeTemp	= NULL;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (hDB==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (CryptoObjectNr==NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	/***********************************************************/
	/* zapytanie do bazy danych o dane z tabeli crypto_objects */
	/***********************************************************/
	if (WithSysMetadata==WITH_SYS_METADATA)
	{
		BMD_FOK(bmd_db_escape_string(hDB, CryptoObjectNr, STANDARD_ESCAPING, &escapeTemp));
		asprintf(&SQLQuery,	"SELECT crypto_objects.name, crypto_objects.filesize, crypto_objects.symkey, crypto_objects.insert_date, crypto_objects.visible, crypto_objects.present, crypto_objects.file_type, crypto_objects.corresponding_id, crypto_objects.transaction_id, crypto_objects.pointing_id, crypto_objects.location_id, crypto_objects.pointing_location_id, crypto_objects.fk_group, crypto_objects.fk_owner, crypto_objects.creator, crypto_objects.for_grant, crypto_objects.hash_value, crypto_objects.expiration_date, crypto_objects.for_timestamping, groups.name, users.cert_dn||users.cert_serial, sec_categories.name, sec_levels.name FROM crypto_objects \n"
					"LEFT JOIN users ON (crypto_objects.fk_owner=users.identity)\n"
					"LEFT JOIN sec_categories ON (crypto_objects.fk_sec_categories=sec_categories.id)\n"
					"LEFT JOIN sec_levels ON (crypto_objects.fk_sec_levels=sec_levels.priority)\n"
					"LEFT JOIN groups ON (crypto_objects.fk_group=groups.id)\n"
					"WHERE crypto_objects.id=%s %s%s;",
					escapeTemp,
					location_id==NULL?"":"AND crypto_objects.location_id=",
					location_id==NULL?"":location_id);
		free(escapeTemp); escapeTemp=NULL;
		if (SQLQuery==NULL)				{	BMD_FOK(BMD_ERR_MEMORY);	}

		ExecuteSQLQueryWithRowAnswersKnownDBConnection( hDB, SQLQuery, &ans_count, &ans);
		if (ans_count==0)					{	BMD_FOK(BMD_ERR_NODATA);	}
	}
	else
	{
		BMD_FOK(bmd_db_escape_string(hDB, CryptoObjectNr, STANDARD_ESCAPING, &escapeTemp));
		asprintf(&SQLQuery,	"SELECT name, filesize, symkey, insert_date, visible, present, file_type, corresponding_id, transaction_id, pointing_id, location_id, pointing_location_id, fk_group, fk_owner, creator, for_grant, hash_value, expiration_date, for_timestamping FROM crypto_objects WHERE id=%s %s%s;",
					escapeTemp,
					location_id==NULL?"":"AND location_id=",
					location_id==NULL?"":location_id);
		free(escapeTemp); escapeTemp=NULL;
		if (SQLQuery==NULL)				{	BMD_FOK(BMD_ERR_MEMORY);	}

		ExecuteSQLQueryWithRowAnswersKnownDBConnection( hDB, SQLQuery, &ans_count, &ans);
		if (ans_count==0)					{	BMD_FOK(BMD_ERR_NODATA);	}
	}

	/************************************************/
	/* sprawdzenie, czy plik jest obecny w archiwum */
	/************************************************/
	if (_GLOBAL_bmd_configuration->enable_delete_history==1)
	{
		if (bmdJSResponse->datagramType!=BMD_DATAGRAM_TYPE_SEARCH_RESPONSE)
		{
			if (ans==NULL)				{	BMD_FOK(LIBBMDSQL_GET_DOCUMENT_ABSENT_IN_DB_ERR);	}
			else if (strlen(ans[5])==0)		{	BMD_FOK(LIBBMDSQL_GET_DOCUMENT_ABSENT_IN_DB_ERR);	}
			else if (ans[5][0]=='0')		{	BMD_FOK(LIBBMDSQL_GET_DOCUMENT_ABSENT_IN_DB_ERR);	}
			else if (ans[5][0]!='1')		{	BMD_FOK(LIBBMDSQL_GET_DOCUMENT_ABSENT_IN_DB_ERR);	}
		}
	}

	/**********************************/
	/* ustawienie danych w datagramie */
	/**********************************/
	BMD_FOK(InsertStringToGenBufAlloc(ans[0], &(bmdJSResponse->protocolDataFilename)));
	BMD_FOK(InsertStringToGenBufAlloc(CryptoObjectNr, &(bmdJSResponse->protocolDataFileId)));
	BMD_FOK(InsertStringToGenBufAlloc(location_id==NULL?"0":location_id, &(bmdJSResponse->protocolDataFileLocationId)));

	/********************************************************************/
	/* dodanie do datagramu DER klucza symetrycznego w postaci bmdKeyIV */
	/********************************************************************/
	if (	(bmdJSResponse->datagramType!=BMD_DATAGRAM_TYPE_SEARCH_RESPONSE)	&&
 		(bmdJSResponse->datagramType!=BMD_DATAGRAM_GET_METADATA_WITH_TIME_RESPONSE) &&
		(bmdJSResponse->datagramType!=BMD_DATAGRAM_GET_METADATA_RESPONSE) &&
		(bmdJSResponse->datagramType!=BMD_DATAGRAM_DIR_LIST_WITH_OR_RESPONSE) &&
		(bmdJSResponse->datagramType!=BMD_DATAGRAM_TYPE_INSERT_LO_RESPONSE) &&
		(bmdJSResponse->datagramType!=BMD_DATAGRAM_TYPE_UPDATE_FILE_VERSION_LO_RESPONSE) &&
		(bmdJSResponse->datagramType!=BMD_DATAGRAM_TYPE_CLOSE_SESSION_RESPONSE) &&
		(bmdJSResponse->datagramType!=BMD_DATAGRAM_TYPE_SEND_AVIZO_RESPONSE) &&
		(bmdJSResponse->datagramType!=BMD_DATAGRAM_TYPE_SEND_ADVICE_WITH_PARAMETERS_RESPONSE) )
	{

		//symkey_loid=strtoul(ans[2],NULL,10);
		//if (symkey_loid>0)
		{
			BMD_FOK_CHG(bmd_db_import_blob(hDB,ans[2], &tmp_gb),
					BMD_SQL_GET_METADATA_LOB_NOT_FOUND_ERROR);

			BMD_FOK(bmd2_datagram_set_symkey(tmp_gb,&bmdJSResponse));
			free_gen_buf(&tmp_gb);
		}
	}
	else
	{

		/**************************************/
		/* ustawienie metadanych w datagramie */
		/**************************************/
		for (i=0;i<ans_count; i++)
		{
			if(WithSysMetadata!=WITH_SYS_METADATA && i>=CRYPTO_OBJECTS_SYS_METADATA_COUNT)
				{ continue; }

			if ((ans[i]!=NULL) && (strlen(ans[i])))
			{
				switch (i)
				{
					case 0 :	asprintf(&tmp_oid, "%s", OID_SYS_METADATA_CRYPTO_OBJECTS_FILENAME); break;
					case 1 :	asprintf(&tmp_oid, "%s", OID_SYS_METADATA_CRYPTO_OBJECTS_FILESIZE); break;
					case 3 :	asprintf(&tmp_oid, "%s", OID_SYS_METADATA_CRYPTO_OBJECTS_TIMESTAMP); break;
					case 4 :	asprintf(&tmp_oid, "%s", OID_SYS_METADATA_CRYPTO_OBJECTS_VISIBLE); break;
					case 5 :	asprintf(&tmp_oid, "%s", OID_SYS_METADATA_CRYPTO_OBJECTS_PRESENT); break;
					case 6 :	asprintf(&tmp_oid, "%s", OID_SYS_METADATA_CRYPTO_OBJECTS_FILE_TYPE); break;
					case 7 :	asprintf(&tmp_oid, "%s", OID_SYS_METADATA_CRYPTO_OBJECTS_CORESPONDING_ID); break;
					case 8 :	asprintf(&tmp_oid, "%s", OID_SYS_METADATA_CRYPTO_OBJECTS_TRANSACTION_ID); break;
					case 9 :	asprintf(&tmp_oid, "%s", OID_SYS_METADATA_CRYPTO_OBJECTS_POINTING_ID); break;
					case 10 :	asprintf(&tmp_oid, "%s", OID_SYS_METADATA_CRYPTO_OBJECTS_LOCATION_ID); break;
					case 11 :	asprintf(&tmp_oid, "%s", OID_SYS_METADATA_CRYPTO_OBJECTS_POINTING_LOCATION_ID); break;
					case 12 :	asprintf(&tmp_oid, "%s", OID_SYS_METADATA_CRYPTO_OBJECTS_GROUP); break;
					case 13 :	asprintf(&tmp_oid, "%s", OID_SYS_METADATA_CRYPTO_OBJECTS_OWNER); break;
					case 14 :	asprintf(&tmp_oid, "%s", OID_SYS_METADATA_CRYPTO_OBJECTS_CREATOR); break;
					case 15 :	asprintf(&tmp_oid, "%s", OID_SYS_METADATA_CRYPTO_OBJECTS_FOR_GRANT); break;
					case 16 :	asprintf(&tmp_oid, "%s", OID_SYS_METADATA_CRYPTO_OBJECTS_HASH_VALUE); break;
					case 17 :	asprintf(&tmp_oid, "%s", OID_SYS_METADATA_CRYPTO_OBJECTS_EXPIRATION_DATE); break;
					case 18 :	asprintf(&tmp_oid, "%s", OID_SYS_METADATA_CRYPTO_OBJECTS_FOR_TIMESTAMPING); break;
					case 19 :	asprintf(&tmp_oid, "%s", OID_SYS_METADATA_BMD_GROUP); break;
					case 21 :	asprintf(&tmp_oid, "%s", OID_SYS_METADATA_SEC_CAT); break;
					case 22 :	asprintf(&tmp_oid, "%s", OID_SYS_METADATA_BMD_SEC_LEVELS); break;
					case 23 :	asprintf(&tmp_oid, "%s", OID_SYS_METADATA_CRYPTO_OBJECTS_GROUP); break;
					case 24 :	asprintf(&tmp_oid, "%s", OID_SYS_METADATA_CRYPTO_OBJECTS_OWNER); break;
					default :	continue;
				}

				for (j=0; j<req->no_of_formularz; j++)
				{

					if ( (strcmp(req->formularz[j]->oid_str, tmp_oid)==0) &&
						(req->formularz[j]->maxLength>-1) &&
						((long)strlen(ans[i])>req->formularz[j]->maxLength) )
					{

						ans[i][req->formularz[j]->maxLength]='\0';

						break;
					}

				}
				BMD_FOK(set_gen_buf2 (ans[i], (long)strlen(ans[i]), &tmp_gb));
				BMD_FOK(bmd_datagram_add_metadata_2(tmp_oid, tmp_gb, &bmdJSResponse,1));
				free(tmp_oid); tmp_oid=NULL;
				free_gen_buf(&tmp_gb);
			}
		}
	}

	/************/
	/* porzadki */
	/************/
	for (i=0; i<ans_count; i++)
	{
		free(ans[i]); ans[i]=NULL;
	}

	free(ans); ans=NULL;
	free(SQLQuery); SQLQuery=NULL;

	return BMD_OK;
}

long GetIdFromHash(	void *hDB,
			char *hash_value,
			char **FileId)
{
long err			= 0;
long fetched_counter1		= 0;
char *SQLQuery 			= NULL;
void *database_result		= NULL;
char *tmp			= NULL;
char *oid_einvoice_hash_und	= "1_2_616_1_113527_4_3_3_23";
char *colname_am_value =	BMD_ADD_METADATA_COLNAME_AM_VALUE;

	PRINT_INFO("LIBBMDSQLINF Getting ID from HASH.\n");

	asprintf(&SQLQuery, "SELECT id FROM crypto_objects WHERE %s_%s='%s';",colname_am_value, oid_einvoice_hash_und, hash_value);

	err = bmd_db_execute_sql(hDB,SQLQuery,NULL,NULL,&database_result);
	if (err < 0)
	{
		bmd_db_result_free(&database_result);
		return BMD_OPR_GET_HASH_NOT_FOUND;
	}
	/* UWAGA!!!! jesli sa 2 takie same hashe to na razie wypluwam ID pierwszego z plikow	*/
	/* (FIXME: dodac np. password obok hasha) */
	fetched_counter1 = 0;
	err = bmd_db_result_get_value(hDB, database_result, 0, 0, &tmp,FETCH_NEXT, &fetched_counter1);
	if (err<BMD_OK && err!=BMD_ERR_NODATA && err!=BMD_ERR_NODATA)
	{
		return err;
	}
	asprintf(FileId, "%s", tmp);

	if (fetched_counter1 == 0)
	{
		bmd_db_result_free(&database_result);
		return BMD_OPR_GET_HASH_NOT_FOUND;
	}
	bmd_db_result_free(&database_result);

	free(tmp); tmp=NULL;

	return err;
}

long GetHashFromId(	void *hDB,
				char *FileId,
				char *location_id,
				char **hash_value)
{
long err					= 0;
/*long rows					= 0;
long cols					= 0;*/
long fetched_counter1				= 0;
char *tmp					= NULL;
char *SQLQuery 				= NULL;
void  *database_result			= NULL;
char *oid_einvoice_hash_und		= "1_2_616_1_113527_4_3_3_23";
char *colname_am_value			= BMD_ADD_METADATA_COLNAME_AM_VALUE;
char* escapeTemp				= NULL;

	PRINT_INFO("LIBBMDSQLINF Getting HASH form ID.\n");

	BMD_FOK(bmd_db_escape_string(hDB, FileId, STANDARD_ESCAPING, &escapeTemp));
	asprintf(&SQLQuery, "SELECT %s_%s FROM crypto_objects WHERE id='%s' AND location_id=%s;",
	colname_am_value, oid_einvoice_hash_und, escapeTemp, location_id);
	free(escapeTemp); escapeTemp=NULL;

	if (SQLQuery == NULL)			{	BMD_FOK(NO_MEMORY);	}

	err = bmd_db_execute_sql(hDB,SQLQuery,NULL,NULL,&database_result);
	if (err < 0)
	{
		bmd_db_result_free(&database_result);
		BMD_FOK(BMD_ERR_NODATA);
	}
	fetched_counter1 = 0;
	err = bmd_db_result_get_value(hDB, database_result, 0, 0, &tmp,FETCH_NEXT, &fetched_counter1);
	if (err<BMD_OK && err!=BMD_ERR_NODATA && err!=BMD_ERR_NODATA)
	{
		return err;
	}
	asprintf(hash_value, "%s", tmp);
	if (tmp) {free(tmp); tmp=NULL;}
	if (*hash_value == NULL)
	{
		bmd_db_result_free(&database_result);
		BMD_FOK(NO_MEMORY);
	}
	if (fetched_counter1 == 0)
	{
		bmd_db_result_free(&database_result);
		return BMD_ERR_NODATA;
	}
	bmd_db_result_free(&database_result);
	return err;
}

long GetEmailFromId(	void *hDB,
				char *FileId,
				char **email)
{
long err					= 0;
/*long rows					= 0;
long cols					= 0;*/
long fetched_counter1	= 0;
void   *database_result			= NULL;
char *tmp					= NULL;
char *SQLQuery 				= NULL;
char *oid_einvoice_hash_und		= "1_2_616_1_113527_4_3_3_8";
char *colname_am_value			= BMD_ADD_METADATA_COLNAME_AM_VALUE;
char* escapeTemp			= NULL;

	PRINT_INFO("LIBBMDSQLINF Getting email form ID.\n");

	
	BMD_FOK(bmd_db_escape_string(hDB, FileId, STANDARD_ESCAPING, &escapeTemp));
	asprintf(&SQLQuery, "SELECT %s_%s FROM crypto_objects WHERE id='%s';", colname_am_value, oid_einvoice_hash_und, escapeTemp);
	free(escapeTemp); escapeTemp=NULL;
	if (SQLQuery == NULL)		{	BMD_FOK(NO_MEMORY);	}

	err = bmd_db_execute_sql(hDB,SQLQuery,NULL,NULL,&database_result);
	if (err < 0)
	{
		bmd_db_result_free(&database_result);
		BMD_FOK(BMD_ERR_NODATA);
	}
	fetched_counter1 = 0;
	err = bmd_db_result_get_value(hDB, database_result, 0, 0, &tmp,FETCH_NEXT, &fetched_counter1);
	if (err<BMD_OK && err!=BMD_ERR_NODATA && err!=BMD_ERR_NODATA)
	{
		return err;
	}
	asprintf(email, "%s", tmp);
	if (tmp) {free(tmp); tmp=NULL;}
	if (*email == NULL)
	{
		bmd_db_result_free(&database_result);
		BMD_FOK(NO_MEMORY);
	}
	if (fetched_counter1 == 0)
	{
		bmd_db_result_free(&database_result);
		return BMD_ERR_NODATA;
	}
	bmd_db_result_free(&database_result);
	return err;
}

long owner2id(	void *hDB,
		char *file_owner,
		char **owner_id)
{
	PRINT_INFO("LIBBMDSQLINF owner2id\n");

	/************************/
	/* walidacja parametrow */
	/************************/
	if (hDB==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (file_owner==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (owner_id==NULL)		{	BMD_FOK(BMD_ERR_PARAM3);	}
	if ((*owner_id)!=NULL)		{	BMD_FOK(BMD_ERR_PARAM3);	}

	//SELECT id, name, identity, cert_serial, cert_dn, cert_dn||cert_serial, accepted FROM users;
	BMD_FOK_CHG(getElementWithCondition(	_GLOBAL_UsersDictionary, 5, file_owner, 2, owner_id), ERR_NO_USER_WITH_GIVEN_ID);

	return BMD_OK;
}

long id2identify(	void *hDB,
			char *id,
			char **identify)
{
	PRINT_INFO("LIBBMDSQLINF owner2id\n");

	/************************/
	/* walidacja parametrow */
	/************************/
	if (hDB==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (id==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (identify==NULL)	{	BMD_FOK(BMD_ERR_PARAM3);	}
	if ((*identify)!=NULL)	{	BMD_FOK(BMD_ERR_PARAM3);	}

	//SELECT id, name, identity, cert_serial, cert_dn, cert_dn||cert_serial, accepted FROM users;
	BMD_FOK_CHG(getElementWithCondition(	_GLOBAL_UsersDictionary, 0, id, 2, identify), ERR_NO_USER_WITH_GIVEN_ID);

	return BMD_OK;
}

/*** OBSOLETE

long register_owner(void *hDB, char *file_owner, char **owner_id, char *groupId)
{
long err			= 0;
void *database_result	= NULL;
char *SQLQuery		= NULL;
char *SequenceNextVal	= NULL;
char *file_owner_esc	= NULL;

	PRINT_INFO("LIBBMDSQLINF Registering new owner\n");

	SequenceNextVal=bmd_db_get_sequence_nextval(hDB, "users_id_seq");
	if (SequenceNextVal==NULL)		{	BMD_FOK(NO_MEMORY);	}

	*owner_id=bmd_db_get_sequence_currval(hDB, "users_id_seq");
	if (*owner_id==NULL)			{	BMD_FOK(NO_MEMORY);	}

	BMD_FOK(bmd_db_escape_string(hDB,file_owner,ADDITIONAL_ESCAPING,&file_owner_esc));

	if (groupId!=NULL)
	{

		asprintf(&SQLQuery, "INSERT INTO users (id, dn, group_id) VALUES (%s, \'%s\', \'%s\');",SequenceNextVal, file_owner_esc, groupId);
	}
	else
	{

		asprintf(&SQLQuery, "INSERT INTO users (id, dn) VALUES (%s, \'%s\');",SequenceNextVal, file_owner_esc);
	}
	free(file_owner_esc); file_owner_esc=NULL;

	err = bmd_db_execute_sql(hDB, SQLQuery,NULL,NULL, &database_result);
	if (err < 0)
	{
		bmd_db_result_free(&database_result);
		free(SQLQuery); SQLQuery=NULL;
		return err;
	}

	free(SequenceNextVal); SequenceNextVal = NULL;
	free(SQLQuery); SQLQuery=NULL;

#ifndef WIN32
	kill(getppid(),SIGDICTRELOAD);
#endif //ifndef WIN32

	return BMD_OK;
}

OBSOLETE ***/


long CharReplace(	char *Oid,
			char **EscapedOid,
			char old_char,
			char new_char)
{
long err			= 0;
long i				= 0;

	//PRINT_INFO("LIBBMDSQLINF CharReplace\n");

	if (Oid == NULL)						{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (EscapedOid == NULL || *EscapedOid != NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	asprintf(EscapedOid, "%s", Oid);
	if (*EscapedOid == NULL)			{	BMD_FOK(NO_MEMORY);	}


	for(i=0; i<1+(long)strlen(*EscapedOid);  i++)
	{
		if ((*EscapedOid)[i] == old_char)
		{
			(*EscapedOid)[i] = new_char;
		}
	}

	return err;
}

long ExecuteSQLQueryWithAnswerKnownDBConnection(	void *db_conn_info,
							char *SQLQuery,
							char **ans)
{
long status		= 0;
long fetched_counter	= 0;
void *SelectResult	= NULL;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (db_conn_info==NULL)				{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (SQLQuery==NULL)				{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (ans == NULL)				{	BMD_FOK(BMD_ERR_PARAM3);	}
	if ( (*ans) !=NULL)				{	BMD_FOK(BMD_ERR_PARAM3);	}


	BMD_FOK(bmd_db_execute_sql(db_conn_info, SQLQuery,NULL,NULL, &SelectResult));

	status=bmd_db_result_get_value(db_conn_info, SelectResult, 0, 0, ans, FETCH_NEXT, &fetched_counter);
	bmd_db_result_free(&SelectResult);
	if (status==BMD_ERR_NODATA)
	{
		return status;
	}
	else
	{
		BMD_FOK(status);
	}

	return BMD_OK;
}

long ExecuteSQLQueryWithAnswersKnownDBConnection(	void *db_conn_info,
							char *SQLQuery,
							long *ans_count,
							char ***ans)
{
long status		= 0;
long fetched_counter	= 0;
void *SelectResult	= NULL;
char *odp		= NULL;

	PRINT_INFO("LIBBMDSQLINF ExecuteSQLQueryWithAnswersKnownDBConnection\n");
	/************************/
	/* walidacja parametrow */
	/************************/
	if (db_conn_info==NULL)		{       BMD_FOK(BMD_ERR_PARAM1);        }
	if (SQLQuery==NULL)		{       BMD_FOK(BMD_ERR_PARAM2);        }
	if (ans == NULL)		{       BMD_FOK(BMD_ERR_PARAM4);        }
	if ( (*ans) !=NULL)		{       BMD_FOK(BMD_ERR_PARAM4);        }


	(*ans_count)=0;
	BMD_FOK(bmd_db_execute_sql(db_conn_info, SQLQuery,NULL,NULL, &SelectResult));

	while (1)
	{
		(*ans_count)=(*ans_count)+1;
		(*ans)=(char**)realloc((*ans),sizeof(char *)*(*ans_count));
		status=bmd_db_result_get_value(db_conn_info, SelectResult, 0, 0, &odp, FETCH_NEXT, &fetched_counter);
		if (status<BMD_OK)
		{
			if (status==BMD_ERR_NODATA || status==BMD_ERR_NODATA)
			{
				if ((*ans_count)==1)
				{
					(*ans_count)=0;
					bmd_db_result_free(&SelectResult);
					free(odp); odp=NULL;
					return status;
				}
				else
				{
					(*ans_count)=(*ans_count)-1;
					break;
				}
			}
			else
			{
				BMD_FOK(status);
			}
		}
		asprintf(&((*ans)[(*ans_count)-1]), "%s", odp);
		free(odp); odp=NULL;
	}

	/************/
	/* porzadki */
	/************/
	bmd_db_result_free(&SelectResult);
	free0(odp);

	return BMD_OK;
}

/************************************************************************/
/*	wywolanie polecenia na bazie danych i pobranie wiersza wyniku	*/
/************************************************************************/
long ExecuteSQLQueryWithRowAnswersKnownDBConnection(	void *db_conn_info,
							char *SQLQuery,
							long *ans_count,
							char ***ans)
{
long i				= 0;
long select_cols		= 0;
long fetched_counter		= 0;
void *SelectResult		= NULL;
db_row_strings_t *rowstruct	= NULL;

	PRINT_INFO("LIBBMDSQLINF ExecuteSQLQueryWithRowAnswersKnownDBConnection\n");
	/************************/
	/* walidacja parametrow */
	/************************/
	if (db_conn_info==NULL)		{       BMD_FOK(BMD_ERR_PARAM1);        }
	if (SQLQuery==NULL)		{       BMD_FOK(BMD_ERR_PARAM2);        }
	if (ans == NULL)			{       BMD_FOK(BMD_ERR_PARAM4);        }
	if ( (*ans) !=NULL)		{       BMD_FOK(BMD_ERR_PARAM4);        }

	(*ans_count)=0;
	BMD_FOK(bmd_db_execute_sql(db_conn_info, SQLQuery,NULL, &select_cols, &SelectResult));

	/******************************************************/
	/*	pobranie calego wiersza zwrocnego w zapytaniu	*/
	/******************************************************/
	BMD_FOK(bmd_db_result_get_row(db_conn_info, SelectResult, 0, 0, &rowstruct, FETCH_NEXT, &fetched_counter));

	/************************************************/
	/*	pobranie poszczegolnych kolumn z wiersza	*/
	/************************************************/
	(*ans_count)=select_cols;
	(*ans)=(char**)malloc(sizeof(char *)*(*ans_count));
	memset((*ans), 0, sizeof(char *)*(*ans_count));
	for (i=0 ;i<select_cols; i++)
	{
		asprintf(&((*ans)[i]), "%s", rowstruct->colvals[i]);
		if ( (*ans)[i] == NULL)	{	BMD_FOK(NO_MEMORY);	}
	}

	/******************/
	/*	porzadki	*/
	/******************/
	bmd_db_result_free(&SelectResult);
	bmd_db_row_struct_free(&rowstruct);

	return BMD_OK;
}

/**
* @author WSZ
* Funkcja VerifyPermissionsForArchiveCryptoObject() jest wrapperem na funckje VerifyPermissionsForCryptoObject() wywolywanej dla currentVersionFileId. Dodatkowa operacja jest tutaj
* sprawdzenie, czy wersja pliku archiveVersionFileId jest faktycznie wersja archiwalna pliku currentVersionFileId. Parametr currentVersionFileId moze byc linkiem.
* Parametry wywolania i wartosci zwracane: patrz opis funkcji VerifyPermissionsForCryptoObject().
*/
long VerifyPermissionsForArchiveCryptoObject(	void *hDB,
				server_request_data_t *req,
				char* currentVersionFileId,
				char* archiveVersionFileId,
				bmdSearchType_e verificationType,
				char** fileName,
				char** fileType,
				char** pointingId,
				char** correspondingId,
				char** forGrant,
				long *additionalMetadataCount,
				char ***additionalMetadataValues)
{
long retVal				= 0;
long iter				= 0;
char* sqlTemplate		= "SELECT id FROM crypto_objects_history WHERE id=%s AND fk_crypto_object=%s AND location_id=%s;";
char* sqlQuery			= NULL;
char* queryResult			= NULL;

char* tempPointingId	= NULL;
char* escapeTemp		= NULL;


	if(archiveVersionFileId == NULL)
		{ BMD_FOK(BMD_ERR_PARAM4); }
	if(strlen(archiveVersionFileId) <= 0)
		{ BMD_FOK(BMD_ERR_PARAM4); }
	if(_GLOBAL_bmd_configuration == NULL)
		{ BMD_FOK(BMD_ERR_OP_FAILED); }

	BMD_FOK(VerifyPermissionsForCryptoObject(hDB, req, currentVersionFileId, verificationType,
				fileName, fileType, &tempPointingId, correspondingId, forGrant,
				additionalMetadataCount, additionalMetadataValues));

	
	BMD_FOK(bmd_db_escape_string(hDB, archiveVersionFileId, STANDARD_ESCAPING, &escapeTemp));
	//sprawdzenie, czy versja pliku archiveVersionFileId faktycznie jest wersja archiwalna pliku currentVersionFileId (pliku/linka), do ktorego uzytkownik ma dostep
	asprintf(&sqlQuery, sqlTemplate, escapeTemp, tempPointingId, _GLOBAL_bmd_configuration->location_id);
	free(escapeTemp); escapeTemp=NULL;
	if(sqlQuery == NULL)
	{
		if(fileName != NULL)
			{ free(*fileName); *fileName=NULL; }
		if(fileType != NULL)
			{ free(*fileType); *fileType=NULL; }
		free(tempPointingId); tempPointingId=NULL;
		if(correspondingId != NULL)
			{ free(*correspondingId); *correspondingId=NULL; }
		if(forGrant != NULL)
			{ free(*forGrant); *forGrant=NULL; }
		if(additionalMetadataCount != NULL && additionalMetadataValues != NULL)
		{
			for(iter=0; iter<*additionalMetadataCount; iter++)
				{ free( (*additionalMetadataValues)[iter] ); }
			free(*additionalMetadataValues); *additionalMetadataValues=NULL;
			*additionalMetadataCount=0;
		}
		BMD_FOK(NO_MEMORY);
	}
	
	retVal=ExecuteSQLQueryWithAnswerKnownDBConnection(hDB, sqlQuery, &queryResult);
	free(sqlQuery); sqlQuery=NULL;
	free(queryResult); queryResult=NULL;
	if(retVal < BMD_OK)
	{
		if(fileName != NULL)
			{ free(*fileName); *fileName=NULL; }
		if(fileType != NULL)
			{ free(*fileType); *fileType=NULL; }
		free(tempPointingId); tempPointingId=NULL;
		if(correspondingId != NULL)
			{ free(*correspondingId); *correspondingId=NULL; }
		if(forGrant != NULL)
			{ free(*forGrant); *forGrant=NULL; }
		if(additionalMetadataCount != NULL && additionalMetadataValues != NULL)
		{
			for(iter=0; iter<*additionalMetadataCount; iter++)
				{ free( (*additionalMetadataValues)[iter] ); }
			free(*additionalMetadataValues); *additionalMetadataValues=NULL;
			*additionalMetadataCount=0;
		}
		BMD_FOK(retVal);
	}
	
	
	if(pointingId != NULL)
	{
		*pointingId=tempPointingId;
		tempPointingId=NULL;
	}
	else
	{
		free(tempPointingId);
		tempPointingId=NULL;
	}

	return BMD_OK;
}


/**
* @author WSZ
* Funckja VerifyPermissionsForCryptoObject() pozwala sprawdzić, czy uzytkownik przesylajacy zadanie ma uprawnienia do pliku/linku verifyingFileId.
* Przy okzacji sprawdzenia uprawnien mozliwe jest pobranie interesujacych nas metadanych opisujacych obiekt verifyingFileId.
* @param[in] hDB to przygotowane wczesniej polaczenie bazodanowe
* @param[in] req to informacje na temat odebranego zadania bmd
* @param[in] verifyingFileId to identyfikator sprawdzanego pliku/linku
* @param[in] verificationType okresla sposob weryfikacji uprawnien. Akceptowane wartosci:
* @arg CONSIDER_GROUPS_GRAPH oznacza, ze plik musi by przyporzadkowany do grupy, na ktora zalogowal sie uzytkownik, badz grupa pliku jest jedna z grup podleglych tej, na ktora zalogowal sie uzytkownik
* @arg CONSIDER_CURRENT_GROUP oznacza, ze plik musi by przyporzadkowany do grupy, na ktora zalogowal sie uzytkownik
* @arg CONSIDER_ONLY_OWNER oznacza, ze uytkownik musi byc wlascicielem pliku
* @arg CONSIDER_ONLY_CREATOR oznacza, ze uzytkownik musi byc tworca pliku
* @arg CONSIDER_CREATOR_OR_OWNER oznacza, ze uzytkownik musi byc tworca lub wlascicielem pliku
* @arg CONSIDER_CREATOR_OR_OWNER_OR_GROUPS oznacza, ze uzytkownik musi byc tworca lub wlascicielem pliku lub tak jak w przypadku CONSIDER_GROUPS_GRAPH
* @arg CONSIDER_CREATOR_OR_OWNER_OR_CURRENT_GROUP oznacza, ze uzytkownik musi byc tworca lub wlascicielem pliku lub tak jak w przypadku CONSIDER_CURRENT_GROUP
* @arg SKIP_AUTHORIZATION oznacza, ze nieistotne sa uprawnienia uzytkownika do pliku (gdy potrzebujemy jedynie pobrac interesujace nas metadane opisujace plik)
* @arg CONSIDER_ONLY_DIRECTORY oznacza, ze weryfikowane beda uprawnienia do katalogu (nie pliku i nie linku). Ta flaga moze byc bitowo sumowana (OR) z flagami CONSIDER_xxx.
* @param[out] fileName to adres, pod ktory przypisana zostanie zaalokowana nazwa sprawdzanego obiektu. Parametr jest opcjonalny i mozna podac NULL.
* @param[out] fileType to adres, pod ktory przypisana zostanie zaalokowana nazwa typu sprawdzanego obiektu. Parametr jest opcjonalny i mozna podac NULL.
* @param[out] pointingId to adres, pod ktory przypisany zostanie zaalokowany identyfikator pliku wskazywanego przez obiekt verifyingFileId. Parametr jest opcjonalny i mozna podac NULL.
* @param[out] correspondingId to adres, pod ktory przypisany zostanie zaalokowany identyfikator pliku, z ktorym zwiazany jest plik verifyingFileId. Parametr jest opcjonalny i mozna podac NULL.
* @param[out] forGrant to adres, pod ktory przypisana zostanie zaalokowana wartosc okreslajaca, czy plik pozwala na tworzenie udostepnien. Parametr jest opcjonalny i mozna podac NULL.
* @param[out] additionalMetadataCount to adres licznika, pod ktory zostanie przypisana ilosc metadanych dodatkowych opisujacych plik verifyingFileId. Parametr jest opcjonalny, ale jesli podaje sie NULL,
* to wowczas wymagane jest, aby parametr additionalMetadataValues rowniez byl NULL (sytuacja, kiedy nie potrzebujemy wartosci metadanych dodatkowych).
* @param[out additionalMetadataValues to adres, pod ktory zostanie przepisana alokowana tablica wartosci metadanych dodatkowych opisujacych plik verificationType.
* Parametr jest opcjonalny, ale jesli podaje sie NULL, to wowczas wymagane jest, aby parametr additionalMetadataCount rowniez byl NULL (sytuacja, kiedy nie potrzebujemy wartosci metadanych dodatkowych).
* @return Funckja zwraca 0 w przypadku sukcesu operacji , gdy uzytkownik ma prawa do pliku verificationType. W przypadku, gdy uzytkownik nie ma uprawnien do pliku verificationType, funkcja zwraca blad BMD_ERR_NODATA.
* W przypadku bledu wykonywania funkcji zwracana jest wartosc ujemna inna niz BMD_ERR_NODATA.
*/

long VerifyPermissionsForCryptoObject(	void *hDB,
				server_request_data_t *req,
				char* verifyingFileId,
				bmdSearchType_e verificationType,
				char** fileName,
				char** fileType,
				char** pointingId,
				char** correspondingId,
				char** forGrant,

				long *additionalMetadataCount,
				char ***additionalMetadataValues)
{

long retVal				= 0;
char* sqlQuery				= NULL;
char** answers				= NULL;
long answersCount			= 0;
bmdDatagram_t* requestDatagram		= 0;

char* tempFileName			= NULL;
char* tempFileType			= NULL;
char* tempPointingId			= NULL;
char* tempCorrespondingId		= NULL;
char* tempForGrant			= NULL;

long iter				= 0;
char **tempAdditionalMetadata		= NULL;
long tempAdditionalMetadataCount	= 0;
long ignoreDependentGroups		= 0;

	if(hDB == NULL)
		{ BMD_FOK(BMD_ERR_PARAM1); }
	if(req == NULL)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(verifyingFileId == NULL)
		{ BMD_FOK(BMD_ERR_PARAM3); }
	if(strlen(verifyingFileId) <= 0)
		{ BMD_FOK(BMD_ERR_PARAM3); }

	if(_GLOBAL_bmd_configuration == NULL)
		{ BMD_FOK(BMD_ERR_OP_FAILED); }
	if(_GLOBAL_bmd_configuration->location_id == NULL)
		{ BMD_FOK(BMD_ERR_OP_FAILED); }


	if(verificationType & CONSIDER_ONLY_DIRECTORY ) //pominiecie w warunku file_type <> 'dir'
		{ BMD_FOK(bmd_datagram_create(BMD_DATAGRAM_DIR_LIST, &requestDatagram)); }
	else
		{ BMD_FOK(bmd_datagram_create(BMD_DATAGRAM_TYPE_SEARCH, &requestDatagram)); }
	retVal=set_gen_buf2(verifyingFileId, (long)strlen(verifyingFileId), &(requestDatagram->protocolDataFileId));
	if(retVal < BMD_OK)
	{
		bmd_datagram_free(&requestDatagram);
		BMD_FOK(BMD_ERR_OP_FAILED);
	}
	retVal=set_gen_buf2(_GLOBAL_bmd_configuration->location_id, (long)strlen(_GLOBAL_bmd_configuration->location_id), &(requestDatagram->protocolDataFileLocationId));
	if(retVal < BMD_OK)
	{
		bmd_datagram_free(&requestDatagram);
		BMD_FOK(BMD_ERR_OP_FAILED);
	}


	if(verificationType & SKIP_AUTHORIZATION)
		{ requestDatagram->datagramStatus |= BMD_REQ_DB_SELECT_SKIP_AUTHORIZATION; }
	else if(verificationType & CONSIDER_ONLY_OWNER)
		{ requestDatagram->datagramStatus |= BMD_REQ_DB_SELECT_ONLY_OWNER; }
	else if(verificationType & CONSIDER_ONLY_CREATOR)
		{ requestDatagram->datagramStatus |= BMD_REQ_DB_SELECT_ONLY_CREATOR; }
	else if(verificationType & CONSIDER_CREATOR_OR_OWNER)
		{ requestDatagram->datagramStatus |= BMD_REQ_DB_SELECT_CREATOR_OR_OWNER; }
	else if(verificationType & CONSIDER_CREATOR_OR_OWNER_OR_GROUPS || verificationType & CONSIDER_CREATOR_OR_OWNER_OR_CURRENT_GROUP)
		{ requestDatagram->datagramStatus |= BMD_REQ_DB_SELECT_CREATOR_OR_OWNER_OR_GROUPS; }
	else if(verificationType & CONSIDER_GROUPS_GRAPH || verificationType & CONSIDER_CURRENT_GROUP)
		{ requestDatagram->datagramStatus |= BMD_REQ_DB_SELECT_ONLY_GROUP; }
	else
	{
		bmd_datagram_free(&requestDatagram);
		BMD_FOK(BMD_ERR_UNIMPLEMENTED);
	}

	if(verificationType & CONSIDER_CURRENT_GROUP || verificationType & CONSIDER_CREATOR_OR_OWNER_OR_CURRENT_GROUP)
		{ ignoreDependentGroups=1; }
	else
		{ ignoreDependentGroups=0; }

	//wstawienie warunku file_type = 'dir'
	if(verificationType & CONSIDER_ONLY_DIRECTORY)
	{ 
		retVal=bmd_datagram_add_metadata_char(OID_SYS_METADATA_CRYPTO_OBJECTS_FILE_TYPE, "dir", &requestDatagram);
		if(retVal < BMD_OK)
		{
			bmd_datagram_free(&requestDatagram);
			BMD_FOK(BMD_ERR_OP_FAILED);
		}
	}

	// ignorowana widocznosc dokumentu - odnajdowany niezalezenie od tego czy widoczny czy nie
	retVal=bmd_datagram_add_metadata_char(OID_SYS_METADATA_CRYPTO_OBJECTS_VISIBLE, IGNORE_DOC_VISIBILITY_STRING, &requestDatagram);
	if(retVal < BMD_OK)
	{
		bmd_datagram_free(&requestDatagram);
		BMD_FOK(BMD_ERR_OP_FAILED);
	}

	retVal=GenerateSearchQuery(hDB, requestDatagram, SEARCH_RESULT_LIST, ignoreDependentGroups, &sqlQuery, NULL, NULL, req);
	bmd_datagram_free(&requestDatagram);
	if(retVal < BMD_OK)
		{ BMD_FOK(BMD_ERR_OP_FAILED); }
	retVal=bmd_str_replace(&sqlQuery, "<first_step>", "");
	if(retVal < BMD_OK)
	{
		free(sqlQuery);
		BMD_FOK(BMD_ERR_OP_FAILED);
	}
	retVal=bmd_str_replace(&sqlQuery, "<second_step>", "");
	if(retVal < BMD_OK)
	{
		free(sqlQuery);
		BMD_FOK(BMD_ERR_OP_FAILED);
	}

	/******************************************************/
	/* wykonanie wygenerowanego zapytania na bazie danych */
	/******************************************************/
	retVal=ExecuteSQLQueryWithRowAnswersKnownDBConnection( hDB, sqlQuery, &answersCount, &answers);
	free(sqlQuery); sqlQuery=NULL;
	if( bmd_db_connect_get_db_type(hDB) == BMD_DB_ORACLE )
		{ bmd_db_free_bind_var_list(hDB); }

	/*if (retVal == BMD_ERR_NODATA)
		{ BMD_FOK(retVal); }
	else*/ if(retVal < BMD_OK)
		{ BMD_FOK(retVal); }

	if(fileName != NULL)
	{
		if((answersCount>=6) && (answers[5]!=NULL ))
		{
			tempFileName=strdup(answers[5]);
			if(tempFileName == NULL)
			{
				for(iter=0; iter<answersCount; iter++)
					{ free(answers[iter]); }
				free(answers);
				BMD_FOK(NO_MEMORY);
			}
		}
	}
	
	if(fileType != NULL)
	{
		if((answersCount>=3) && (answers[2]!=NULL ))
		{
			tempFileType=strdup(answers[2]);
			if(tempFileType == NULL)
			{
				free(tempFileName);
				for(iter=0; iter<answersCount; iter++)
					{ free(answers[iter]); }
				free(answers);
				BMD_FOK(NO_MEMORY);
			}
		}
	}
	
	if(pointingId != NULL)
	{
		if((answersCount>=5) && (answers[4]!=NULL ))
		{
			tempPointingId=strdup(answers[4]);
			if(tempPointingId == NULL)
			{
				free(tempFileName);
				free(tempFileType);
				for(iter=0; iter<answersCount; iter++)
					{ free(answers[iter]); }
				free(answers);
				BMD_FOK(NO_MEMORY);
			}
		}
	}
	
	if(correspondingId != NULL)
	{
		if((answersCount>=11) && (answers[10]!=NULL ))
		{
			tempCorrespondingId=strdup(answers[10]);
			if(tempCorrespondingId == NULL)
			{
				free(tempFileName);
				free(tempFileType);
				free(tempPointingId);
				for(iter=0; iter<answersCount; iter++)
					{ free(answers[iter]); }
				free(answers);
				BMD_FOK(NO_MEMORY);
			}
		}
	}

	if(forGrant != NULL)
	{
		if((answersCount>=16) && (answers[15]!=NULL ))
		{
			tempForGrant=strdup(answers[15]);
			if(tempForGrant == NULL)
			{
				free(tempFileName);
				free(tempFileType);
				free(tempPointingId);
				free(tempCorrespondingId);
				for(iter=0; iter<answersCount; iter++)
					{ free(answers[iter]); }
				free(answers);
				BMD_FOK(NO_MEMORY);
			}
		}
	}

	if( additionalMetadataCount != NULL && additionalMetadataValues != NULL )
	{
		/*jesli jakiekolwiek metadatane sa przewidzanie na formularzu*/
		if(answersCount>CRYPTO_OBJECTS_SYS_METADATA_COUNT)
		{
			tempAdditionalMetadataCount=answersCount-CRYPTO_OBJECTS_SYS_METADATA_COUNT;
			tempAdditionalMetadata=(char**)calloc(tempAdditionalMetadataCount, sizeof(char*));
			if(tempAdditionalMetadata == NULL)
			{
				free(tempFileName);
				free(tempFileType);
				free(tempPointingId);
				free(tempCorrespondingId);
				free(tempForGrant);
				for(iter=0; iter<answersCount; iter++)
					{ free(answers[iter]); }
				free(answers);
				BMD_FOK(NO_MEMORY);
			}
			else
			{
				for(iter=CRYPTO_OBJECTS_SYS_METADATA_COUNT; iter<answersCount; iter++)
				{
					if(answers[iter] != NULL)
					{
						tempAdditionalMetadata[iter-CRYPTO_OBJECTS_SYS_METADATA_COUNT]=strdup(answers[iter]);
					}
				}
			}
		}
	}


	for(iter=0; iter<answersCount; iter++)
		{ free(answers[iter]); }
	free(answers); answers=NULL;

	if(fileName != NULL)
	{
		*fileName=tempFileName; tempFileName=NULL;
	}
	if(fileType != NULL)
	{
		*fileType=tempFileType; tempFileType=NULL;
	}
	if(pointingId != NULL)
	{
		*pointingId=tempPointingId; tempPointingId=NULL;
	}
	if(correspondingId != NULL)
	{
		*correspondingId=tempCorrespondingId; tempCorrespondingId=NULL;
	}
	if(forGrant != NULL)
	{
		*forGrant=tempForGrant; tempForGrant=NULL;
	}

	if(additionalMetadataValues != NULL && additionalMetadataCount != NULL)
	{
		*additionalMetadataValues=tempAdditionalMetadata; tempAdditionalMetadata=NULL;
		*additionalMetadataCount=tempAdditionalMetadataCount;
	}

	return BMD_OK;
}





/**
* @author WSz
* Funkcja VerifyPermissionsForCryptoObjectByMetadata() pozwala na sprawdzanie uprawnien
* na podstawie metadanych opisujacych plik zamiast na podstawie ID dokumentu.
* @note Musi byc podana co najmniej jedna metadana dodatkowa, inaczej zwrocony jest blad
* Parametry wywolania i wartosci zwracane: patrz opis funkcji VerifyPermissionsForCryptoObject().
* @param requestWithMetadata to datagram zadania z metadanymi (dodatkowymi i systemowymi), ktore posluza jako kryterium
*		wyszukiwania dokumentow.
*/
long VerifyPermissionsForCryptoObjectByMetadata(	void *hDB,
				server_request_data_t *req,
				bmdDatagram_t *requestWithMetadata,
				bmdSearchType_e verificationType,
				char** fileName,
				char** fileType,
				char** pointingId,
				char** correspondingId,
				char** forGrant,

				long *additionalMetadataCount,
				char ***additionalMetadataValues)
{

long retVal				= 0;
char* sqlQuery				= NULL;
void* queryResult		= NULL;
long answersCount			= 0;
bmdDatagram_t* requestDatagram		= 0;
GenBuf_t metadataBuf;

char* tempFileName			= NULL;
char* tempFileType			= NULL;
char* tempPointingId			= NULL;
char* tempCorrespondingId		= NULL;
char* tempForGrant			= NULL;

long iter				= 0;
char **tempAdditionalMetadata		= NULL;
long tempAdditionalMetadataCount	= 0;
long ignoreDependentGroups		= 0;

db_row_strings_t *rowStruct 	= NULL;
char* cmpString1				= NULL;
char* cmpString2				= NULL;
long fetched					= 0;

long sysCriteriaCount			= 0;

	if(hDB == NULL)
		{ BMD_FOK(BMD_ERR_PARAM1); }
	if(req == NULL)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(requestWithMetadata == NULL)
		{ BMD_FOK(BMD_ERR_PARAM3); }
	if(requestWithMetadata->no_of_additionalMetaData > 0 && requestWithMetadata->additionalMetaData == NULL)
		{ BMD_FOK(BMD_ERR_PARAM3); }
	if(requestWithMetadata->no_of_sysMetaData > 0 && requestWithMetadata->sysMetaData == NULL)
		{ BMD_FOK(BMD_ERR_PARAM3); }

	if(_GLOBAL_bmd_configuration == NULL)
		{ BMD_FOK(BMD_ERR_OP_FAILED); }
	if(_GLOBAL_bmd_configuration->location_id == NULL)
		{ BMD_FOK(BMD_ERR_OP_FAILED); }


	// sprawdzenie poprawnosci metadanych systemowych
	sysCriteriaCount = VerifySystemMetadataTypeAndValue(requestWithMetadata);
	BMD_FOK(sysCriteriaCount);

	// wymagana jest co najmniej jedna prawidlowa (tj. wystepujaca w formularzu) metadana dodatkowa (jesli brakuje systemowych)
	if(sysCriteriaCount == 0 && requestWithMetadata->no_of_additionalMetaData <= 0)
	{
		BMD_FOK(BMD_VERIFY_REQUEST_NO_ADD_METADATA_SPECIFIED);
	}

	// sprawdzenie, czy metadane dodatkowe (podane jako kryteria) ujete sa w formularzu
	BMD_FOK(VerifyAdditionalMetadataTypeAndValue(requestWithMetadata, req, NULL, 0));
	
	if(verificationType & CONSIDER_ONLY_DIRECTORY ) //pominiecie w warunku file_type <> 'dir'
		{ BMD_FOK(bmd_datagram_create(BMD_DATAGRAM_DIR_LIST, &requestDatagram)); }
	else
		{ BMD_FOK(bmd_datagram_create(BMD_DATAGRAM_TYPE_SEARCH, &requestDatagram)); }
	
	for(iter=0; iter<requestWithMetadata->no_of_additionalMetaData; iter++)
	{
		if(requestWithMetadata->additionalMetaData[iter] == NULL)
		{
			bmd_datagram_free(&requestDatagram);
			BMD_FOK(retVal);
		}
		metadataBuf.buf = requestWithMetadata->additionalMetaData[iter]->AnyBuf;
		metadataBuf.size = requestWithMetadata->additionalMetaData[iter]->AnySize;
		retVal = bmd_datagram_add_metadata_2(requestWithMetadata->additionalMetaData[iter]->OIDTableBuf, &metadataBuf, &requestDatagram, 0);
		if(retVal < BMD_OK)
		{
			bmd_datagram_free(&requestDatagram);
			BMD_FOK(retVal);
		}
	}
	
	for(iter=0; iter<requestWithMetadata->no_of_sysMetaData; iter++)
	{
		if(requestWithMetadata->sysMetaData[iter] == NULL)
		{
			bmd_datagram_free(&requestDatagram);
			BMD_FOK(retVal);
		}
		metadataBuf.buf = requestWithMetadata->sysMetaData[iter]->AnyBuf;
		metadataBuf.size = requestWithMetadata->sysMetaData[iter]->AnySize;
		retVal = bmd_datagram_add_metadata_2(requestWithMetadata->sysMetaData[iter]->OIDTableBuf, &metadataBuf, &requestDatagram, 0);
		if(retVal < BMD_OK)
		{
			bmd_datagram_free(&requestDatagram);
			BMD_FOK(retVal);
		}
	}
	metadataBuf.buf = NULL; metadataBuf.size = 0;
		
	retVal=set_gen_buf2(_GLOBAL_bmd_configuration->location_id, (long)strlen(_GLOBAL_bmd_configuration->location_id), &(requestDatagram->protocolDataFileLocationId));
	if(retVal < BMD_OK)
	{
		bmd_datagram_free(&requestDatagram);
		BMD_FOK(BMD_ERR_OP_FAILED);
	}


	if(verificationType & SKIP_AUTHORIZATION)
		{ requestDatagram->datagramStatus |= BMD_REQ_DB_SELECT_SKIP_AUTHORIZATION; }
	else if(verificationType & CONSIDER_ONLY_OWNER)
		{ requestDatagram->datagramStatus |= BMD_REQ_DB_SELECT_ONLY_OWNER; }
	else if(verificationType & CONSIDER_ONLY_CREATOR)
		{ requestDatagram->datagramStatus |= BMD_REQ_DB_SELECT_ONLY_CREATOR; }
	else if(verificationType & CONSIDER_CREATOR_OR_OWNER)
		{ requestDatagram->datagramStatus |= BMD_REQ_DB_SELECT_CREATOR_OR_OWNER; }
	else if(verificationType & CONSIDER_CREATOR_OR_OWNER_OR_GROUPS || verificationType & CONSIDER_CREATOR_OR_OWNER_OR_CURRENT_GROUP)
		{ requestDatagram->datagramStatus |= BMD_REQ_DB_SELECT_CREATOR_OR_OWNER_OR_GROUPS; }
	else if(verificationType & CONSIDER_GROUPS_GRAPH || verificationType & CONSIDER_CURRENT_GROUP)
		{ requestDatagram->datagramStatus |= BMD_REQ_DB_SELECT_ONLY_GROUP; }
	else
	{
		bmd_datagram_free(&requestDatagram);
		BMD_FOK(BMD_ERR_UNIMPLEMENTED);
	}

	if(verificationType & CONSIDER_CURRENT_GROUP || verificationType & CONSIDER_CREATOR_OR_OWNER_OR_CURRENT_GROUP)
		{ ignoreDependentGroups=1; }
	else
		{ ignoreDependentGroups=0; }

	//wstawienie warunku file_type = 'dir'
	if(verificationType & CONSIDER_ONLY_DIRECTORY)
	{ 
		retVal=bmd_datagram_add_metadata_char(OID_SYS_METADATA_CRYPTO_OBJECTS_FILE_TYPE, "dir", &requestDatagram);
		if(retVal < BMD_OK)
		{
			bmd_datagram_free(&requestDatagram);
			BMD_FOK(BMD_ERR_OP_FAILED);
		}
	}

	retVal=GenerateSearchQuery(hDB, requestDatagram, SEARCH_RESULT_LIST, ignoreDependentGroups, &sqlQuery, NULL, NULL, req);
	bmd_datagram_free(&requestDatagram);
	if(retVal < BMD_OK)
		{ BMD_FOK(BMD_ERR_OP_FAILED); }
	retVal=bmd_str_replace(&sqlQuery, "<first_step>", "");
	if(retVal < BMD_OK)
	{
		free(sqlQuery);
		BMD_FOK(BMD_ERR_OP_FAILED);
	}
	retVal=bmd_str_replace(&sqlQuery, "<second_step>", "");
	if(retVal < BMD_OK)
	{
		free(sqlQuery);
		BMD_FOK(BMD_ERR_OP_FAILED);
	}

	/******************************************************/
	/* wykonanie wygenerowanego zapytania na bazie danych */
	/******************************************************/
	retVal = bmd_db_execute_sql(hDB, sqlQuery, &answersCount, NULL, &queryResult);
	free(sqlQuery); sqlQuery=NULL;
	if( bmd_db_connect_get_db_type(hDB) == BMD_DB_ORACLE )
		{ bmd_db_free_bind_var_list(hDB); }

	if(retVal < BMD_OK)
		{ BMD_FOK(retVal); }
	
	// kryteria powinny byc tak okreslone, aby otrzymac dokladnie jeden wynik
	if(answersCount > 1)
	{
		retVal = bmd_db_result_get_value(hDB, queryResult, 0, 4/*pointing_id*/, &cmpString1, FETCH_ABSOLUTE, &fetched);
		if(retVal < BMD_OK || cmpString1 == NULL)
		{
			bmd_db_result_free(&queryResult);
			BMD_FOK(BMD_ERR_OP_FAILED);
		}
		// jesli wiecej niz jeden wynik (bo tez sa linki), to trzeba sprawdzic, czy wszystkie maja ten sam pointing_id (czy dotycza jednego dokumentu)
		for(iter=1; iter<answersCount; iter++)
		{
			retVal = bmd_db_result_get_value(hDB, queryResult, iter, 4/*pointing_id*/, &cmpString2, FETCH_ABSOLUTE, &fetched);
			if(retVal < BMD_OK || cmpString2 == NULL)
			{
				free(cmpString1); cmpString1 = NULL;
				bmd_db_result_free(&queryResult);
				BMD_FOK(BMD_ERR_OP_FAILED);
			}
			if(strcmp(cmpString1, cmpString2) != 0)
			{
				free(cmpString2); cmpString2 = NULL;
				free(cmpString1); cmpString1 = NULL;
				bmd_db_result_free(&queryResult);
				BMD_FOK(BMD_VERIFY_REQUEST_AMBIGUOUS_CRITERIA);
			}
			free(cmpString2); cmpString2 = NULL;
		}
		free(cmpString1); cmpString1 = NULL;
	}
	
	retVal = bmd_db_result_get_row(hDB, queryResult, 0, 0, &rowStruct, FETCH_ABSOLUTE, &fetched);
	if(retVal < BMD_OK)
	{
		bmd_db_result_free(&queryResult);
		BMD_FOK(BMD_ERR_OP_FAILED);
	}

	if(fileName != NULL)
	{
		if((rowStruct->size>=6) && (rowStruct->colvals[5]!=NULL ))
		{
			tempFileName=strdup(rowStruct->colvals[5]);
			if(tempFileName == NULL)
			{
				bmd_db_row_struct_free(&rowStruct);
				bmd_db_result_free(&queryResult);
				BMD_FOK(NO_MEMORY);
			}
		}
	}
	
	if(fileType != NULL)
	{
		if((rowStruct->size>=3) && (rowStruct->colvals[2]!=NULL ))
		{
			tempFileType=strdup(rowStruct->colvals[2]);
			if(tempFileType == NULL)
			{
				free(tempFileName);
				bmd_db_row_struct_free(&rowStruct);
				bmd_db_result_free(&queryResult);
				BMD_FOK(NO_MEMORY);
			}
		}
	}
	
	if(pointingId != NULL)
	{
		if((rowStruct->size>=5) && (rowStruct->colvals[4]!=NULL ))
		{
			tempPointingId=strdup(rowStruct->colvals[4]);
			if(tempPointingId == NULL)
			{
				free(tempFileName);
				free(tempFileType);
				bmd_db_row_struct_free(&rowStruct);
				bmd_db_result_free(&queryResult);
				BMD_FOK(NO_MEMORY);
			}
		}
	}
	
	if(correspondingId != NULL)
	{
		if((rowStruct->size>=11) && (rowStruct->colvals[10]!=NULL ))
		{
			tempCorrespondingId=strdup(rowStruct->colvals[10]);
			if(tempCorrespondingId == NULL)
			{
				free(tempFileName);
				free(tempFileType);
				free(tempPointingId);
				bmd_db_row_struct_free(&rowStruct);
				bmd_db_result_free(&queryResult);
				BMD_FOK(NO_MEMORY);
			}
		}
	}

	if(forGrant != NULL)
	{
		if((rowStruct->size>=16) && (rowStruct->colvals[15]!=NULL ))
		{
			tempForGrant=strdup(rowStruct->colvals[15]);
			if(tempForGrant == NULL)
			{
				free(tempFileName);
				free(tempFileType);
				free(tempPointingId);
				free(tempCorrespondingId);
				bmd_db_row_struct_free(&rowStruct);
				bmd_db_result_free(&queryResult);
				BMD_FOK(NO_MEMORY);
			}
		}
	}

	if( additionalMetadataCount != NULL && additionalMetadataValues != NULL )
	{
		/*jesli jakiekolwiek metadatane sa przewidzanie na formularzu*/
		if(rowStruct->size > CRYPTO_OBJECTS_SYS_METADATA_COUNT)
		{
			tempAdditionalMetadataCount = rowStruct->size - CRYPTO_OBJECTS_SYS_METADATA_COUNT;
			tempAdditionalMetadata=(char**)calloc(tempAdditionalMetadataCount, sizeof(char*));
			if(tempAdditionalMetadata == NULL)
			{
				free(tempFileName);
				free(tempFileType);
				free(tempPointingId);
				free(tempCorrespondingId);
				free(tempForGrant);
				bmd_db_row_struct_free(&rowStruct);
				bmd_db_result_free(&queryResult);
				BMD_FOK(NO_MEMORY);
			}
			else
			{
				for(iter=CRYPTO_OBJECTS_SYS_METADATA_COUNT; iter < rowStruct->size; iter++)
				{
					if(rowStruct->colvals[iter] != NULL)
					{
						tempAdditionalMetadata[iter-CRYPTO_OBJECTS_SYS_METADATA_COUNT]=strdup(rowStruct->colvals[iter]);
					}
				}
			}
		}
	}


	bmd_db_row_struct_free(&rowStruct);
	bmd_db_result_free(&queryResult);

	if(fileName != NULL)
	{
		*fileName=tempFileName; tempFileName=NULL;
	}
	if(fileType != NULL)
	{
		*fileType=tempFileType; tempFileType=NULL;
	}
	if(pointingId != NULL)
	{
		*pointingId=tempPointingId; tempPointingId=NULL;
	}
	if(correspondingId != NULL)
	{
		*correspondingId=tempCorrespondingId; tempCorrespondingId=NULL;
	}
	if(forGrant != NULL)
	{
		*forGrant=tempForGrant; tempForGrant=NULL;
	}

	if(additionalMetadataValues != NULL && additionalMetadataCount != NULL)
	{
		*additionalMetadataValues=tempAdditionalMetadata; tempAdditionalMetadata=NULL;
		*additionalMetadataCount=tempAdditionalMetadataCount;
	}

	return BMD_OK;
}



///OBSOLETE
#if 0
/******************************************************************************************/
/*	Sprawdzanie czy crypto object i podanym id istnieje i jest widoczny dla uzytkownika	*/
/*	nalezacego do danej grupy (z uwzgledenieniem grup podleglych wg drzewa zaleznosci)	*/
/******************************************************************************************/
long CheckIfCryptoObjectExist(	void *hDB,
				bmdDatagram_t *bmdJSRequest,
				long ignoreDependentGroups,
				char **id,
				char **file_type,
				char **location_id,
				char **file_name,
				char **corresponding_id,
				long *addMetadataCount,
				char ***addMetadata,
				server_request_data_t *req)
{
long status			= 0;
long error			= 0;
long ans_count			= 0;
long i				= 0;
char *SQLQuery			= NULL;
char **ans			= NULL;
char *pointing_id		= NULL;
char *pointing_location_id	= NULL;
char *file_type_tmp		= NULL;

	PRINT_INFO("LIBBMDSQLINF Checking if file's crypto object exists\n");
	/********************************/
	/*	walidacja parametrow	*/
	/********************************/
	if (hDB==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (bmdJSRequest==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (id==NULL)			{	BMD_FOK(BMD_ERR_PARAM3);	}
	if ((*id)!=NULL)		{	BMD_FOK(BMD_ERR_PARAM3);	}
	if (location_id==NULL)		{	BMD_FOK(BMD_ERR_PARAM5);	}
	if ((*location_id)!=NULL)	{	BMD_FOK(BMD_ERR_PARAM5);	}

	/*******************************************************************************/
	/* sprawdzanie, czy plik o podanym id istnieje i czy klient ma do niego dostep */
	/* (grupy, clearance'y)                                                        */
	/*******************************************************************************/
	BMD_FOK(GenerateSearchQuery(hDB, bmdJSRequest, SEARCH_RESULT_LIST, ignoreDependentGroups, &SQLQuery, NULL, NULL, req));
	BMD_FOK(bmd_str_replace(&SQLQuery, "<first_step>", ""));
	BMD_FOK(bmd_str_replace(&SQLQuery, "<second_step>", ""));

	/******************************************************/
	/* wykonanie wygenerowanego zapytania na bazie danych */
	/******************************************************/
	status=ExecuteSQLQueryWithRowAnswersKnownDBConnection( hDB, SQLQuery, &ans_count, &ans);
	if ( bmd_db_connect_get_db_type(hDB) == BMD_DB_ORACLE )
	bmd_db_free_bind_var_list(hDB);

	free0(SQLQuery);
	if (status != BMD_ERR_NODATA)		{		BMD_FOK(status);		}

	if (status==BMD_ERR_NODATA)
	{

		switch (bmdJSRequest->datagramType)
		{
			case BMD_DATAGRAM_TYPE_GET			: error=BMD_SQL_GET_ONLY_FILE_NOT_FOUND_ERROR; break;
			case BMD_DATAGRAM_TYPE_GET_LO			: error=BMD_SQL_GET_ONLY_FILE_NOT_FOUND_ERROR; break;
			case BMD_DATAGRAM_TYPE_GET_PKI			: error=BMD_SQL_GET_METADATA_NOT_FOUND_ERROR; break;
			case BMD_DATAGRAM_TYPE_GET_PKI_LO		: error=BMD_SQL_GET_METADATA_NOT_FOUND_ERROR; break;
			case BMD_DATAGRAM_TYPE_DELETE			: error=BMD_SQL_DELETE_FILE_NOT_FOUND; break;
			case BMD_DATAGRAM_GET_CGI			: error=BMD_SQL_GET_CGI_NOT_FOUND_ERROR; break;
			case BMD_DATAGRAM_GET_CGI_PKI			: error=BMD_SQL_GET_METADATA_CGI_NOT_FOUND_ERROR; break;
			case BMD_DATAGRAM_GET_CGI_LO			: error=BMD_SQL_GET_CGI_NOT_FOUND_ERROR; break;
			case BMD_DATAGRAM_TYPE_GET_ARCHIVE_FILE_VERSION_LO		: error=BMD_SQL_GET_CGI_NOT_FOUND_ERROR; break;
			case BMD_DATAGRAM_GET_CGI_PKI_LO		: error=BMD_SQL_GET_METADATA_CGI_NOT_FOUND_ERROR; break;
			case BMD_DATAGRAM_GET_METADATA			: error=BMD_SQL_GET_METADATA_NOT_FOUND_ERROR; break;
			case BMD_DATAGRAM_GET_METADATA_WITH_TIME	: error=BMD_SQL_GET_METADATA_NOT_FOUND_ERROR; break;
			case BMD_DATAGRAM_GET_ONLY_FILE			: error=BMD_SQL_GET_ONLY_FILE_NOT_FOUND_ERROR; break;
			case BMD_DATAGRAM_TYPE_VALIDATE_CERTIFICATE	: error=PLUGIN_DVCS_DATA_ABSENT; break;
			case BMD_DATAGRAM_TYPE_VALIDATE_SIGNATURE	: error=BMD_SQL_VERIFY_SIGNATURE_FILE_NOT_FOUND_ERROR; break;
			case BMD_DATAGRAM_TYPE_UPDATE			: error=BMD_SQL_UPDATE_FILE_NOT_FOUND_ERROR; break;
			case BMD_DATAGRAM_GET_METADATA_ADD		: error=BMD_SQL_GET_METADATA_NOT_FOUND_ERROR; break;
			case BMD_DATAGRAM_DIR_LIST			: error=BMD_SQL_GET_DIR_LIST_NOT_FOUND_ERROR; break;
			case BMD_DATAGRAM_DIR_DELETE			: error=BMD_SQL_DELETE_DIR_NOT_FOUND_ERROR; break;
			case BMD_DATAGRAM_DIR_GET_CONTENT		: error=BMD_SQL_GET_DIR_NOT_FOUND_ERROR; break;
			case BMD_DATAGRAM_DIR_GET_DETAIL		: error=BMD_SQL_GET_DIR_NOT_FOUND_ERROR; break;
			case BMD_DATAGRAM_ADD_SIGNATURE           	: error=LIBBMDSQL_ADD_SIGNATURE_NO_FILE_WITH_ID; break;
			default						: error=status; break;
		}
	}
	else if (status==BMD_OK)
	{
		if ((ans_count>=1) && (ans[0]!=NULL )) asprintf(id, "%s", ans[0]);
		if ((ans_count>=2) && (ans[1]!=NULL )) asprintf(location_id, "%s", ans[1]);
		if ((ans_count>=3) && (ans[2]!=NULL )) asprintf(&file_type_tmp, "%s", ans[2]);
		if ((ans_count>=4) && (ans[3]!=NULL )) asprintf(&pointing_location_id, "%s", ans[3]);
		if ((ans_count>=5) && (ans[4]!=NULL )) asprintf(&pointing_id, "%s", ans[4]);
		if ((ans_count>=6) && (ans[5]!=NULL ) && (file_name!=NULL)) asprintf(file_name, "%s", ans[5]);
		if( corresponding_id != NULL && ans_count>=11 && ans[10]!=NULL )
			{ asprintf(corresponding_id, "%s", ans[10]); }


		if ((file_type_tmp!=NULL) && (file_type!=NULL))
		{
			asprintf(file_type, "%s", file_type_tmp);
		}

		if (bmdJSRequest->datagramType!=BMD_DATAGRAM_TYPE_DELETE)
		{

			if ((file_type_tmp!=NULL) && (pointing_id!=NULL) && (strcmp(file_type_tmp, "link")==0))
			{
				free0(*id);
				asprintf(id, "%s", pointing_id);

				free0(*location_id);
				asprintf(location_id, "%s", pointing_location_id);

				free0(file_type_tmp);
			}
		}

		if ((ans_count>14) && (addMetadataCount!=NULL) && (addMetadata!=NULL))
		{
			(*addMetadataCount)=ans_count-14;
			(*addMetadata)=(char **)malloc(sizeof(char*)*(*addMetadataCount));
			memset((*addMetadata), 0, sizeof(char*)*(*addMetadataCount));

			for (i=14; i<ans_count; i++)
			{
				asprintf(&((*addMetadata)[i-14]), "%s", ans[i]);
				if ((*addMetadata)[i-14]==NULL)	{	BMD_FOK(NO_MEMORY);	}
			}
		}
	}

	/************/
	/* porzadki */
	/************/
	for (i=0 ;i <ans_count; i++)
	{
		free0(ans[i]);
	}
	free0(ans);
	free0(SQLQuery);
	free0(pointing_id);
	free0(pointing_location_id);

	return error;
}
#endif /*if 0*/

/******************************************************/
/*	wypelnianie datagramu zadanymi metadanymi pliku	*/
/******************************************************/
long FillFromAllMetadata(	void *hDB,
				char *CryptoObjectNr,
				char *location_id,
				EnumWithPkiMetadata_t WithPkiMetadata,
				EnumWithAddMetadata_t WithAddMetadata,
				EnumWithAddMetadata_t WithSysMetadata,
				EnumWithActionMetadata_t WithActionMetadata,
				bmdDatagram_t *bmdJSResponse,
				server_request_data_t *req)
{
GenBuf_t *file_type		= NULL;
GenBuf_t *pointing_id		= NULL;
GenBuf_t *pointing_location_id	= NULL;

	/******************************************************/
	/*	w przypadku zadania pobrania metadanych pki	*/
	/******************************************************/
	PRINT_INFO("LIBBMDSQLINF FillFromAllMetadata\n");

	BMD_FOK(FillFromCryptoObjects(hDB, CryptoObjectNr, location_id, WithSysMetadata, bmdJSResponse,req));
	bmd_datagram_get_metadata(SYS_METADATA, OID_SYS_METADATA_CRYPTO_OBJECTS_FILE_TYPE, bmdJSResponse, 0, &file_type);
	bmd_datagram_get_metadata(SYS_METADATA, OID_SYS_METADATA_CRYPTO_OBJECTS_POINTING_ID, bmdJSResponse, 0, &pointing_id);
	bmd_datagram_get_metadata(SYS_METADATA, OID_SYS_METADATA_CRYPTO_OBJECTS_POINTING_LOCATION_ID, bmdJSResponse, 0, &pointing_location_id);

	if ((file_type!=NULL) && (file_type->buf!=NULL) && (file_type->size>0) && (strcmp(file_type->buf, "link")==0))
	{

		if ((pointing_id!=NULL) && (pointing_id->buf!=NULL) && (pointing_location_id!=NULL) && (pointing_location_id->buf!=NULL))
		{
			if (WithPkiMetadata==WITH_PKI_METADATA)		{	BMD_FOK(prfGetAllProofs(hDB, pointing_id->buf, pointing_location_id->buf, bmdJSResponse));	}
			if (WithAddMetadata==WITH_ADD_METADATA)		{	BMD_FOK(FillFromAdditionalMetaData(hDB, pointing_id->buf, pointing_location_id->buf, bmdJSResponse,req)); }
			if (WithActionMetadata==WITH_ACTION_METADATA)	{	BMD_FOK(FillFromActionMetaData( hDB, pointing_id->buf, pointing_location_id->buf, bmdJSResponse)); }

			free_gen_buf(&pointing_id);
			free_gen_buf(&pointing_location_id);
		}
		else
		{
			BMD_FOK(-666);
		}
		free_gen_buf(&file_type);
	}
	else
	{
		if (WithPkiMetadata==WITH_PKI_METADATA)		{	BMD_FOK(prfGetAllProofs(hDB, CryptoObjectNr, location_id, bmdJSResponse));	}
		if (WithAddMetadata==WITH_ADD_METADATA)		{	BMD_FOK(FillFromAdditionalMetaData(hDB, CryptoObjectNr, location_id, bmdJSResponse,req)); }
		if (WithActionMetadata==WITH_ACTION_METADATA)	{	BMD_FOK(FillFromActionMetaData( hDB, CryptoObjectNr, location_id, bmdJSResponse)); }
	}

	return BMD_OK;
}

/************************************************************************/
/*	sprawdza czy dana metadana jest w tabeli add_metadata		*/
/************************************************************************/
/*
hDB - polaczenie do bazy danych
twf_metadata_oid_str - string z oidem sprawdzanej metadanej. Musi byc zakonczony NULL.
*/
long CheckAddMetadataExistenceInDb(	void *hDB,
						char *twf_metadata_oid_str)
{
char *SQLQuery			= NULL;
char **ans				= NULL;
long ans_count			= 0;

	/********************************************************************************************************/
	/*      polecenie sprawdzenia, czy w tabeli add_metadata_types jest metadana dodatkowa o podanym OID	*/
	/********************************************************************************************************/
	PRINT_INFO("LIBBMDSQLINF CheckAddMetadataExistenceInDb\n");
	if (hDB == NULL) {	BMD_FOK(BMD_ERR_PARAM1);	}
	if (twf_metadata_oid_str == NULL) {	BMD_FOK(BMD_ERR_PARAM2);	}

	//SELECT id, type, name, sql_cast_string FROM add_metadata_types;;
	BMD_FOK(getColumnWithCondition(	_GLOBAL_AddMetadataTypesDictionary, 1, twf_metadata_oid_str, 0, &ans_count, &ans));

	if (ans[0]==NULL || ans_count == 0)
	{
		PRINT_ERROR("LIBBMDSQLERR No metadata of oid %s in database. Status: %i.\n", twf_metadata_oid_str, NO_METADATA_FOUND);
		BMD_FOK(NO_METADATA_FOUND);
	}

	free(SQLQuery);
	SQLQuery = NULL;

	return BMD_OK;
}


long checkMandatoryMetadataExistenceInDb(void *hDB)
{
long i			= 0;
long status		= 0;
char *session_oids[]	= {	OID_ADD_MAIL_TEMPLATE,
				""};
char *archiving_oids[]	= {	OID_ADD_DOCUMENT_IN_USE,
				""};

	PRINT_INFO("LIBBMDSQLINF checkMandatoryMetadataExistenceInDb\n");

	if(_GLOBAL_bmd_configuration == NULL)
		{ return BMD_ERR_OP_FAILED; }

	/*istnienie metadanych dodatkowych einvoice_hash oraz mail_template sprawdzane tylko przy wlaczonym pluginie sesyjnym*/
	if(_GLOBAL_bmd_configuration->session_plugin_conf.plugin_library_handle != NULL)
	{
		for(i=0; ; i++)
		{
			if (strlen(session_oids[i])==0)
				{ break; }
			status = CheckAddMetadataExistenceInDb(hDB, session_oids[i]);
			if (status<BMD_OK)
			{
				PRINT_ERROR("CheckAddMetadataExistenceInDb failure OID: %s. Error: <%li>\n", session_oids[i], status);
				BMD_FOK(status);
			}
		}
	}

	/*istnienie metadanych dodatkowych document_in_use sprawdzane tylko przy wlaczonym pluginie archiwizacyjnym*/
	if(_GLOBAL_bmd_configuration->archiving_plugin_conf.archiving_plugin_library_handle != NULL)
	{
		for(i=0; ; i++)
		{
			if (strlen(archiving_oids[i])==0)
				{ break; }
			status = CheckAddMetadataExistenceInDb(hDB, archiving_oids[i]);
			if (status<BMD_OK)
			{
				PRINT_ERROR("CheckAddMetadataExistenceInDb failure OID: %s. Error: <%li>\n", archiving_oids[i], status);
				BMD_FOK(status);
			}
		}
	}

	return BMD_OK;
}


/**
* @author WSZ
* Funkcja bmdQuoteSqlConditionValue() sluzy do przygotowania wartosci przypisywanej do skladowej warunku w zapytaniu sql tj. ujmuje wartosc
* w pojedynczy cudzyslow np. jesli czescia warunku sql jest name='Paul' , to przekazujac za conditionValue lanuch "Paul", otrzymamy
* na wyjsciu "'Paul'".
*
* @param[in] conditionValue to lancuch znakowy, ktory ma byc ujety w pojedynczy cudzyslow
* @param[out] quotedConditionValue to adres wskaznika, do ktorego zostanie przypisany zaalokowany lancuch z wartoscia ujeta w cudzyslow
* @return Funkcja zwraca BMD_OK w przypadku sukcesu, lub odpowiednia wartosc mniejsza od BMD_OK w przypadku bledu.
*/
long bmdQuoteSqlConditionValue(char* conditionValue, char** quotedConditionValue)
{
char* tempString	= NULL;

	if(conditionValue == NULL)
		{ BMD_FOK(BMD_ERR_PARAM1); }
	if(quotedConditionValue == NULL)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(*quotedConditionValue != NULL)
		{ BMD_FOK(BMD_ERR_PARAM2); }

	asprintf(&tempString, "'%s'", conditionValue);
	if(tempString == NULL)
		{ BMD_FOK(NO_MEMORY); }

	*quotedConditionValue=tempString;
	tempString=NULL;
	return BMD_OK;
}


/**
* @author WSZ
* Funkcja bmdValidateSqlTimestamp() sluzy do sprawdzenia, czy dane daty przekazywane na wejsciu okreslaja prawidlowa date.
* Dane wejsciowe moga byc produktem funkcji bmdParseSqlTimestamp().
*
* @param[in] year to lancuch znakowy okreslajacy rok (wymagany format "yyyy") np. "2011"
* @param[in] month to lancuch znakowy okreslajacy miesiac w postaci liczbowej (wymagany format "mm") np. "02" czyli luty
* @param[in] day to lancuch znakowy okreslajacy dzien miesiaca (w postaci liczbowej, wymagany format "dd") np. "01"
* @param[in] hour to lancuh znakowy okreslajacy godzine (od "00" do "23", wymagany format "hh")
* @param[in] minute to lancuch znakowy okreslajacy minute godziny (od "00" do "59", wymagany format "mm")
* @param[in] second to lancuch znakowy okreslajacy sekunde minuty (od "00" do "59", wymagany format "ss")
* @param[in] validationType to typ walidacji wykonywanej walidacji.
* @arg GENERAL_VALIDATION pozwala na podstawowe sprawdzenie poprawnosci daty
* @arg LATER_THAN_NOW poza podstawowa walidacja sprawdza dodatkowo, czy podana data jest w przyszlosci (aktualny czas do porownania
* pobierany jest z maszyny, na ktorej urochomione jest oprogramowanie wykorzystujace niniejsza funkcje)
* @return Funkcja zwraca BMD_OK jesli data jest poprawna, lub odpowiednia wartosc mniejsza od BMD_OK w przypadku bledu wykonywania funkcji,
* badz stwierdzenia braku poprawnosci daty.
*/
long bmdValidateSqlTimestamp(char* year, char* month, char* day, char* hour, char* minute, char* second, SqlTimestampValidationType_e validationType)
{
struct tm actualTime;
long longYear		= 0;
long longMonth		= 0;
long longDay		= 0;
long longHour		= 0;
long longMinute		= 0;
long longSecond		= 0;

	if(year == NULL)
		{ BMD_FOK(BMD_ERR_PARAM1); }
	if(strlen(year) != 4)
		{ BMD_FOK(LIBBMDUTILS_INCORRECT_YEAR_VALUE); }

	if(month == NULL)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(strlen(month) != 2)
		{ BMD_FOK(LIBBMDUTILS_INCORRECT_MONTH_VALUE); }

	if(day == NULL)
		{ BMD_FOK(BMD_ERR_PARAM3); }
	if(strlen(day) != 2)
		{  BMD_FOK(LIBBMDUTILS_INCORRECT_DAY_VALUE); }
	
	if(hour == NULL)
		{ BMD_FOK(BMD_ERR_PARAM4); }
	if(strlen(hour) != 2)
		{  BMD_FOK(LIBBMDUTILS_INCORRECT_HOUR_VALUE); }

	if(minute == NULL)
		{ BMD_FOK(BMD_ERR_PARAM5); }
	if(strlen(minute) != 2)
		{  BMD_FOK(LIBBMDUTILS_INCORRECT_MINUTE_VALUE); }

	if(second == NULL)
		{ BMD_FOK(BMD_ERR_PARAM6); }
	if(strlen(second) != 2)
		{  BMD_FOK(LIBBMDUTILS_INCORRECT_SECOND_VALUE); }


	longYear=strtol(year, NULL, 10);
	if(longYear == LONG_MIN || longYear == LONG_MAX) //blad konwersji
		{ BMD_FOK(LIBBMDUTILS_INCORRECT_YEAR_VALUE); }
	
	longMonth=strtol(month, NULL, 10);
	if( longMonth == LONG_MIN || longMonth == LONG_MAX || longMonth < 1 || longMonth > 12 )
		{ BMD_FOK(LIBBMDUTILS_INCORRECT_MONTH_VALUE); }

	longDay=strtol(day, NULL, 10);
	if( longDay == LONG_MIN || longDay == LONG_MAX || longDay < 1 )
		{ BMD_FOK(LIBBMDUTILS_INCORRECT_DAY_VALUE); }

	switch(longMonth)
	{
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12:
		{
			if(longDay > 31)
				{ BMD_FOK(LIBBMDUTILS_INCORRECT_DAY_VALUE); }
			break;
		}
		case 4:
		case 6:
		case 9:
		case 11:
		{
			if(longDay > 30)
				{ BMD_FOK(LIBBMDUTILS_INCORRECT_DAY_VALUE); }
			break;
		}
		case 2:
		{
			//rok przestepny (luty 29 dni)
			if( (longYear % 4 == 0 && longYear % 100 != 0) || longYear % 400 == 0)
			{
				if(longDay > 29)
					{ BMD_FOK(LIBBMDUTILS_INCORRECT_DAY_VALUE); }
			}
			else //luty 28 dni
			{
				if(longDay > 28)
					{ BMD_FOK(LIBBMDUTILS_INCORRECT_DAY_VALUE); }
			}
			break;
		}
	}

	longHour=strtol(hour, NULL, 10);
	if( longHour == LONG_MIN || longHour == LONG_MAX || longHour < 0 || longHour > 23 )
		{ BMD_FOK(LIBBMDUTILS_INCORRECT_HOUR_VALUE); }

	longMinute=strtol(minute, NULL, 10);
	if( longMinute == LONG_MIN || longMinute == LONG_MAX || longMinute < 0 || longMinute > 59 )
		{ BMD_FOK(LIBBMDUTILS_INCORRECT_MINUTE_VALUE); }

	longSecond=strtol(second, NULL, 10);
	if( longSecond == LONG_MIN || longSecond == LONG_MAX || longSecond < 0 || longSecond > 59 )
		{ BMD_FOK(LIBBMDUTILS_INCORRECT_SECOND_VALUE); }

	if(validationType == LATER_THAN_NOW)
	{
		if(GetActualTime(&actualTime) < 0)
			{ BMD_FOK(BMD_ERR_OP_FAILED); }
		
		if(longYear < (actualTime.tm_year+1900) )
			{ BMD_FOK(LIBBMDUTILS_DATE_TIME_NOT_AFTER_NOW); }
		if(longYear == (actualTime.tm_year+1900) && longMonth < (actualTime.tm_mon+1))
			{ BMD_FOK(LIBBMDUTILS_DATE_TIME_NOT_AFTER_NOW); }
		if(longYear == (actualTime.tm_year+1900) && longMonth == (actualTime.tm_mon+1) && longDay < actualTime.tm_mday)
			{ BMD_FOK(LIBBMDUTILS_DATE_TIME_NOT_AFTER_NOW); }
		if(longYear == (actualTime.tm_year+1900) && longMonth == (actualTime.tm_mon+1) && longDay == actualTime.tm_mday && longHour < actualTime.tm_hour)
			{ BMD_FOK(LIBBMDUTILS_DATE_TIME_NOT_AFTER_NOW); }
		if(longYear == (actualTime.tm_year+1900) && longMonth == (actualTime.tm_mon+1) && longDay == actualTime.tm_mday && longHour == actualTime.tm_hour && longMinute < actualTime.tm_min)
			{ BMD_FOK(LIBBMDUTILS_DATE_TIME_NOT_AFTER_NOW); }
		if(longYear == (actualTime.tm_year+1900) && longMonth == (actualTime.tm_mon+1) && longDay == actualTime.tm_mday && longHour == actualTime.tm_hour && longMinute == actualTime.tm_min && longSecond <= actualTime.tm_sec)
			{ BMD_FOK(LIBBMDUTILS_DATE_TIME_NOT_AFTER_NOW); }
	}



	return BMD_OK;
}


/**
* @author WSZ
* Funkcja bmdParseSqlTimestamp() sluzy do parsowania (sprawdzenia formatu) daty w formacie timestampa sql ("yyyy-mm-dd hh:mm:ss") i umozliwia
* wydobywanie wydrebnionych czesci opisujacych date.

* @param[in] sqlTimestamp to lancuch znakowy z data spodziewana w formacie sql timestamp
* @param[out] year to adres wskaznika, do ktorego przypisany zostanie zaalokkowany lanuch znakowy okreslajacy rok wydobyty z daty (mozna
* podac NULL, jesli nie jest potrzebna taka informacja)
* @param[out] month to adres wskaznika, do ktorego przypisany zostanie zaalokowany lanuch znakowy okreslajacy miesiac wydobyty z daty (mozna
* podac NULL, jesli nie jest potrzebna taka informacja)
* @param[out] day to adres wskaznika, do ktorego przypisany zostanie zaalokowany lanuch znakowy okreslajacy dzien miesiaca wydobyty z daty (mozna
* podac NULL, jesli nie jest potrzebna taka informacja)
* @param[out] hour to adres wskaznika, do ktorego przypisany zostanie zaalokowany lanuch znakowy okreslajacy godzine wydobyta z daty (mozna
* podac NULL, jesli nie jest potrzebna taka informacja)
* @param[out] minute to adres wskaznika, do ktorego przypisany zostanie zaalokowany lanuch znakowy okreslajacy minute godziny wydobyta z daty (mozna
* podac NULL, jesli nie jest potrzebna taka informacja)
* @param[out] second to adres wskaznika, do ktorego przypisany zostanie zaalokowany lanuch znakowy okreslajacy sekunde minuty wydobyta z daty (mozna
* podac NULL, jesli nie jest potrzebna taka informacja)
* @return Funkcja zwraca BMD_OK jesli mozna rozparsowac podana na wejsciu date (jest w oczekiwanym formacie), lub odpowiednia wartosc mniejsza
* od BMD_OK w przypadku bledu wykonywania funkcji, badz stwierdzenia niepoprawnego formatu daty.
*/
#define SUBPATTCOUNT 24
long bmdParseSqlTimestamp(char* sqlTimestamp, char** year, char** month, char** day, char** hour, char** minute, char** second)
{
pcre *pcreStruct		= NULL;
char* sqlTimestampRegexp	= "^([0-9]{4})-([0-9]{2})-([0-9]{2}) ([0-9]{2}):([0-9]{2}):([0-9]{2})$";
const char* error_pointer	= NULL;
int error_offset		= 0;
const int subpatternsCount	= SUBPATTCOUNT;
int subpatterns[SUBPATTCOUNT];

char* tempYear			= NULL;
char* tempMonth			= NULL;
char* tempDay			= NULL;
char* tempHour			= NULL;
char* tempMinute		= NULL;
char* tempSecond		= NULL;

	if(sqlTimestamp == NULL)
		{ BMD_FOK(BMD_ERR_PARAM1); }
	
	memset(subpatterns, 0, subpatternsCount*sizeof(int));

	pcreStruct=pcre_compile(sqlTimestampRegexp, PCRE_UTF8, &error_pointer, &error_offset, NULL);
	if(pcre_exec(pcreStruct, NULL, sqlTimestamp,(int)strlen(sqlTimestamp), 0, 0, subpatterns, subpatternsCount) <= 0)
	{
		pcre_free(pcreStruct); pcreStruct=NULL;
		BMD_FOK(LIBBMDUTILS_INCORRECT_DATE_TIME_FORMAT);
	}

	if(	bmdGetSubstring(sqlTimestamp, subpatterns[2], subpatterns[3], &tempYear)	< BMD_OK ||
		bmdGetSubstring(sqlTimestamp, subpatterns[4], subpatterns[5], &tempMonth)	< BMD_OK ||
		bmdGetSubstring(sqlTimestamp, subpatterns[6], subpatterns[7], &tempDay)		< BMD_OK ||
		bmdGetSubstring(sqlTimestamp, subpatterns[8], subpatterns[9], &tempHour)	< BMD_OK ||
		bmdGetSubstring(sqlTimestamp, subpatterns[10], subpatterns[11], &tempMinute)	< BMD_OK ||
		bmdGetSubstring(sqlTimestamp, subpatterns[12], subpatterns[13], &tempSecond)	< BMD_OK )
	{
		pcre_free(pcreStruct); pcreStruct=NULL;
		BMD_FOK(BMD_ERR_OP_FAILED);
	}
	
	if(year != NULL)
	{
		*year=tempYear;
		tempYear=NULL;
	}
	else
		{ free(tempYear); tempYear=NULL; }

	if(month != NULL)
	{
		*month=tempMonth;
		tempMonth=NULL;
	}
	else
		{ free(tempMonth); tempMonth=NULL; }

	if(day != NULL)
	{
		*day=tempDay;
		tempDay=NULL;
	}
	else
		{ free(tempDay); tempDay=NULL; }

	if(hour != NULL)
	{
		*hour=tempHour;
		tempHour=NULL;
	}
	else
		{ free(tempHour); tempHour=NULL; }

	if(minute != NULL)
	{
		*minute=tempMinute;
		tempMinute=NULL;
	}
	else
		{ free(tempMinute); tempMinute=NULL; }

	if(second != NULL)
	{
		*second=tempSecond;
		tempSecond=NULL;
	}
	else
		{ free(tempSecond); tempSecond=NULL; }

	pcre_free(pcreStruct); pcreStruct=NULL;
	return BMD_OK;
}


/**
* @author WSz
* Funkcja sluzy do odnotowania w metadanej z OIDem noticeMetadataOid daty wykonania (pierwszy raz) czynnosci dla dokumentu cryptoObjectId.
* Przed data wstawiany jest tekst positiveNoticeString lub negativeNoticeString w zaleznosci od wartosci activityResult.
* Jesli w metadanej juz wczesniej zostala odnotowana data sukcesu wykonanej operacji, to funkcja nie nadpisze poprzedniej informacji.
* @param hDB to uchwyt do przygotowanego polaczenia bazodanowego
* @param transactionStatus wartosc TRANSACTION_NOT_STARTED jesli przed wywolaniem tej funkcji nie zostala otwarta transakcja bazodanowa; w przeciwnym razie TRANSACTION_ALREADY_STARTED;
* @param activityResult (wartosci ACTIVITY_SUCCEED albo ACTIVITY_FAILED)
* @param cryptoObjectId to indentyfikator dokumentu
* @param positiveNoticeString to tekst w przypadku sukcesu operacji (nigdy nie moze byc NULL)
* @param negativeNoticeString moze byc NULL, jesli activityResult == ACTIVITY_SUCCEED (inaczej blad)
* @param noticeMetadataOid to OID metadanej w ktorej ma byc odnotowana informacja
*/
long noticeFirstTimeActivity(void *hDB, TransactionStatus_e transactionStatus, ActivityResult_e activityResult, const char* cryptoObjectId, const char* positiveNoticeString, const char* negativeNoticeString, char* noticeMetadataOid)
{
long retVal						= 0;
char* noticeColumnName			= NULL;
const char* queryTemplate		= "SELECT %s FROM crypto_objects WHERE id='%s' AND location_id='%s' ; ";
const char* updateTemplate		= "UPDATE crypto_objects SET %s='%s' WHERE id='%s' AND location_id='%s' ; ";
char* sqlQuery					= NULL;
void* queryResult				= NULL;
long rowsCount					= 0;
db_row_strings_t *rowStruct		= NULL;
long fetched					= 0;
long firstActivityAlreadyNoticed	= 0;
char* columnNewValue			= NULL;
struct tm actualTime;

	if(hDB == NULL)
		{ BMD_FOK(BMD_ERR_PARAM1); }
	if(IsNumericIdentifier((char*)cryptoObjectId) < BMD_OK)
		{ BMD_FOK(BMD_ERR_PARAM3); }
	if(positiveNoticeString == NULL)
		{ BMD_FOK(BMD_ERR_PARAM4); }
	if(negativeNoticeString == NULL && activityResult == ACTIVITY_FAILED)
		{ BMD_FOK(BMD_ERR_PARAM5); }
	if(IsOidFormat(noticeMetadataOid) < 1)
		{ BMD_FOK(BMD_ERR_PARAM6); }
	if(_GLOBAL_bmd_configuration == NULL)
		{ BMD_FOK(BMD_ERR_OP_FAILED); }
	
	retVal = asprintf(&noticeColumnName, "amv_%s", noticeMetadataOid);
	if(retVal == -1)
	{
		BMD_FOK(BMD_ERR_MEMORY);
	}
	
	retVal = bmd_str_replace(&noticeColumnName, ".", "_");
	if(retVal < BMD_OK)
	{
		free(noticeColumnName); noticeColumnName = NULL;
		BMD_FOK(retVal);
	}
	
	if(transactionStatus == TRANSACTION_NOT_STARTED)
	{
		retVal = bmd_db_start_transaction(hDB,BMD_TRANSACTION_LEVEL_NONE); // na potrzebe zakladania lock na wiersz
		if(retVal < BMD_OK)
		{
			free(noticeColumnName); noticeColumnName = NULL;
			BMD_FOK(retVal);
		}
	}
	
	retVal = LockCryptoObjectsRow(hDB, (char*)cryptoObjectId);
	if(retVal < BMD_OK)
	{
		free(noticeColumnName); noticeColumnName = NULL;
		if(transactionStatus == TRANSACTION_NOT_STARTED)
			{ bmd_db_rollback_transaction(hDB,NULL); }
		BMD_FOK(retVal);
	}
	
	retVal = asprintf(&sqlQuery, queryTemplate, noticeColumnName, cryptoObjectId, _GLOBAL_bmd_configuration->location_id);
	if(retVal == -1)
	{
		free(noticeColumnName); noticeColumnName = NULL;
		if(transactionStatus == TRANSACTION_NOT_STARTED)
			{ bmd_db_rollback_transaction(hDB,NULL); }
		BMD_FOK(BMD_ERR_MEMORY);
	}
	
	retVal = bmd_db_execute_sql(hDB, sqlQuery, &rowsCount, NULL, &queryResult);
	free(sqlQuery); sqlQuery = NULL;
	if(retVal < BMD_OK)
	{
		free(noticeColumnName); noticeColumnName = NULL;
		if(transactionStatus == TRANSACTION_NOT_STARTED)
			{ bmd_db_rollback_transaction(hDB,NULL); }
		BMD_FOK(retVal);
	}
	
	if(rowsCount != 1)
	{
		bmd_db_result_free(&queryResult);
		free(noticeColumnName); noticeColumnName = NULL;
		if(transactionStatus == TRANSACTION_NOT_STARTED)
			{ bmd_db_rollback_transaction(hDB,NULL); }
		BMD_FOK(BMD_ERR_OP_FAILED);
	}
		
	retVal = bmd_db_result_get_row(hDB, queryResult, 0, 0, &rowStruct, FETCH_ABSOLUTE, &fetched);
	if(retVal < BMD_OK)
	{
		bmd_db_result_free(&queryResult);
		free(noticeColumnName); noticeColumnName = NULL;
		if(transactionStatus == TRANSACTION_NOT_STARTED)
			{ bmd_db_rollback_transaction(hDB,NULL); }
		BMD_FOK(retVal);
	}
	
	if(rowStruct->size != 1 || rowStruct->colvals == NULL)
	{
		bmd_db_row_struct_free(&rowStruct);
		bmd_db_result_free(&queryResult);
		free(noticeColumnName); noticeColumnName = NULL;
		if(transactionStatus == TRANSACTION_NOT_STARTED)
			{ bmd_db_rollback_transaction(hDB,NULL); }
		BMD_FOK(BMD_ERR_OP_FAILED);
	}
	
	if(rowStruct->colvals[0] == NULL)
		{ firstActivityAlreadyNoticed = 0; }
	else if(strcmp(rowStruct->colvals[0], "") == 0)
		{ firstActivityAlreadyNoticed = 0; }
	else
	{
		if(strncmp(rowStruct->colvals[0], positiveNoticeString, strlen(positiveNoticeString)) == 0)
			{ firstActivityAlreadyNoticed = 1; }
		else
			{ firstActivityAlreadyNoticed = 0; }
	}
	bmd_db_row_struct_free(&rowStruct);
	bmd_db_result_free(&queryResult);
	
	if(firstActivityAlreadyNoticed == 1) // wyslanie awiza juz odnotowane
	{
		free(noticeColumnName); noticeColumnName = NULL;
		if(transactionStatus == TRANSACTION_NOT_STARTED)
			{ bmd_db_rollback_transaction(hDB,NULL); }
		return BMD_OK;
	}
	
	retVal = GetActualTime(&actualTime);
	if(retVal < BMD_OK)
	{
		free(noticeColumnName); noticeColumnName = NULL;
		if(transactionStatus == TRANSACTION_NOT_STARTED)
			{ bmd_db_rollback_transaction(hDB,NULL); }
		BMD_FOK(BMD_ERR_OP_FAILED);
	}
	
	if(activityResult == ACTIVITY_SUCCEED)
	{
		retVal = asprintf(&columnNewValue, "%s (%04i-%02i-%02i %02i:%02i:%02i)", positiveNoticeString,
								actualTime.tm_year+1900, actualTime.tm_mon+1, actualTime.tm_mday,
								actualTime.tm_hour, actualTime.tm_min, actualTime.tm_sec);
	}
	else
	{
		retVal = asprintf(&columnNewValue, "%s (%04i-%02i-%02i %02i:%02i:%02i)", negativeNoticeString,
								actualTime.tm_year+1900, actualTime.tm_mon+1, actualTime.tm_mday,
								actualTime.tm_hour, actualTime.tm_min, actualTime.tm_sec);
	}
	
	if(retVal == -1)
	{
		free(noticeColumnName); noticeColumnName = NULL;
		if(transactionStatus == TRANSACTION_NOT_STARTED)
			{ bmd_db_rollback_transaction(hDB,NULL); }
		BMD_FOK(BMD_ERR_MEMORY);
	}

	retVal = asprintf(&sqlQuery, updateTemplate, noticeColumnName, columnNewValue, cryptoObjectId, _GLOBAL_bmd_configuration->location_id);
	free(columnNewValue); columnNewValue = NULL;
	free(noticeColumnName); noticeColumnName = NULL;
	if(retVal == -1)
	{
		if(transactionStatus == TRANSACTION_NOT_STARTED)
			{ bmd_db_rollback_transaction(hDB,NULL); }
		BMD_FOK(BMD_ERR_MEMORY);
	}
	
	retVal = bmd_db_execute_sql(hDB, sqlQuery, NULL, NULL, &queryResult);
	bmd_db_result_free(&queryResult);
	free(sqlQuery); sqlQuery = NULL;
	if(retVal < BMD_OK)
	{
		if(transactionStatus == TRANSACTION_NOT_STARTED)
			{ bmd_db_rollback_transaction(hDB,NULL); }
		BMD_FOK(retVal);
	}
	
	if(transactionStatus == TRANSACTION_NOT_STARTED)
	{
		retVal = bmd_db_end_transaction(hDB);
		if(retVal < BMD_OK)
		{
			bmd_db_rollback_transaction(hDB,NULL);
			BMD_FOK(retVal);
		}
	}
	
	return BMD_OK;
}

