#include <bmd/libbmdsql/select.h>
#include <bmd/libbmdsql/delete.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>
#include <bmd/libbmddb/libbmddb.h>
#include <time.h>
#include <bmd/common/global_types.h>
#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdglobals/libbmdglobals.h>

/** Funkcja generuje zapytanie SQL typu SELECT na podstawie danych umieszczonych w bmdJS|RequestSet.
	Rezultat zapytania zwracany jest do zbioru bmdJSResponseSet.
*/
long JS_bmdDatagramSet_to_select(	void *hDB,
					bmdDatagramSet_t *bmdJSRequestSet,
					bmdDatagramSet_t **bmdJSResponseSet,
					server_request_data_t *req)
{
long status 			= 0;
long i				= 0;
void *SelectResult 		= NULL;
char *SQLQueryFull 		= NULL;
char *SQLQueryInner 		= NULL;
char *SQLQueryInner2 		= NULL;
char *SQLQueryOuter 		= NULL;
char *tmp			= NULL;
char *SQLQueryLimits		= NULL;
char *SelectQueryColumns	= NULL;

	PRINT_INFO("LIBBMDSQLINF Kernel system selecting datagram\n");
	/************************/
	/* WALIDACJA PARAMETROW */
	/************************/
	if (hDB == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (bmdJSRequestSet == NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (bmdJSResponseSet == NULL)	{	BMD_FOK(BMD_ERR_PARAM3);	}
	if (*bmdJSResponseSet != NULL)	{       BMD_FOK(BMD_ERR_PARAM3);        }

	/*********************************************************************/
	/* sprawdzenie, czy wczesniej nie wykryto bledu w datagramie zadania */
	/*********************************************************************/
	if (bmdJSRequestSet->bmdDatagramSetTable[0]->datagramStatus<BMD_OK)
	{
		BMD_FOK_TRANS(PR2_bmdDatagramSet_fill_with_datagrams(1, bmdJSResponseSet));
		PR_bmdDatagram_PrepareResponse(	&((*bmdJSResponseSet)->bmdDatagramSetTable[0]),
						bmdJSRequestSet->bmdDatagramSetTable[0]->datagramType+100,
						bmdJSRequestSet->bmdDatagramSetTable[0]->datagramStatus);
		return BMD_OK;
	}

	/***************************/
	/* rozpoczynamy transakcje */
	/***************************/
	BMD_FOK(bmd_db_start_transaction(hDB,BMD_TRANSACTION_LEVEL_SERIALIZABLE));

	/******************************************/
	/* wygenerowanie stringa z zapytaniem SQL */
	/******************************************/
	BMD_FOK_TRANS(GenerateSearchQuery(	hDB,
						bmdJSRequestSet->bmdDatagramSetTable[0],
						SEARCH_RESULT_LIST,
						0,
						&SQLQueryFull,
						&SQLQueryLimits,
						&SelectQueryColumns,
						req));

	asprintf(&SQLQueryInner, "%s", SQLQueryFull);
	if (SQLQueryInner==NULL)	{	BMD_FOK(NO_MEMORY);	}

	asprintf(&SQLQueryOuter, "%s", SQLQueryFull);
	if (SQLQueryOuter==NULL)	{	BMD_FOK(NO_MEMORY);	}

	BMD_FOK_TRANS(bmd_str_replace(&SQLQueryInner, SelectQueryColumns, "SELECT DISTINCT crypto_objects.id FROM crypto_objects\n"));
	BMD_FOK_TRANS(bmd_str_replace(&SQLQueryInner, "<first_step>", "AND ( crypto_objects.file_type = 'link')"));
	BMD_FOK_TRANS(bmd_str_replace(&SQLQueryInner, SQLQueryLimits, "\n"));
	BMD_FOK_TRANS(bmd_str_replace(&SQLQueryInner, "(crypto_objects.id=action_register.fk_objects OR crypto_objects.pointing_id=action_register.fk_objects)", "(crypto_objects.pointing_id=action_register.fk_objects)"));

	asprintf(&SQLQueryInner2, "%s", SQLQueryInner);	if (SQLQueryInner2==NULL)	{	BMD_FOK(NO_MEMORY); }
	BMD_FOK_TRANS(bmd_str_replace(&SQLQueryInner2, "<second_step>", ""));
	BMD_FOK_TRANS(bmd_str_replace(&SQLQueryInner2, "AND ( crypto_objects.file_type = 'link')", "AND ( crypto_objects.file_type <> 'link')"));
	BMD_FOK_TRANS(bmd_str_replace(&SQLQueryInner2, "(crypto_objects.pointing_id=action_register.fk_objects)", "(crypto_objects.id=action_register.fk_objects)"));

	asprintf(&tmp, "AND (crypto_objects.pointing_id IN (\n\n%s\n\t)\n)", SQLQueryInner2); if (tmp==NULL)	{	BMD_FOK(NO_MEMORY);	}
	BMD_FOK_TRANS(bmd_str_replace(&SQLQueryInner, "<second_step>", tmp));
	free0(tmp);

	asprintf(&SQLQueryOuter, "%s", SQLQueryFull);	if (SQLQueryOuter==NULL)	{	BMD_FOK(NO_MEMORY);	}
	BMD_FOK_TRANS(bmd_str_replace(&SQLQueryOuter, "<first_step>", ""));

	asprintf(&tmp, "AND (crypto_objects.id NOT IN (\n%s\n)\n)", SQLQueryInner); if (tmp==NULL)	{	BMD_FOK(NO_MEMORY);	}

	BMD_FOK_TRANS(bmd_str_replace(&SQLQueryOuter, "<second_step>", tmp));

	free0(tmp);
	free0(SQLQueryLimits);
	free0(SelectQueryColumns);

	/***************************/
	/* wykonanie zapytania SQL */
	/***************************/
	BMD_FOK_TRANS(bmd_db_execute_sql(hDB, SQLQueryOuter,NULL,NULL,&SelectResult));

	free0(SQLQueryFull);
	free0(SQLQueryInner);
	free0(SQLQueryOuter);
	free0(tmp);

	/*********************************************************************/
	/* utworzenie zbioru datagramow do przechowania rezultatów zapytania */
	/*********************************************************************/
	BMD_FOK_TRANS(_PR2_create_bmdDatagram_set(bmdJSResponseSet));

	/*****************************************************************/
	/* wypelnienie slotu danymi o plikach na podstawie wskazanych ID */
	/*****************************************************************/
	status = FillbmdDatagramSetFromSelectResult(hDB, SelectResult,*bmdJSResponseSet,req);
	if(status<BMD_OK && status != BMD_ERR_NODATA)
	{
		BMD_FOK_TRANS(status); /* mozliwe wartosci BMD_ERR_OP_FAILED, BMD_ERR_NODATA */
	}

	bmd_db_result_free(&SelectResult);

	/****************************************/
	/* zarejestrowanie akcji w bazie danych */
	/****************************************/
	
	if (_GLOBAL_bmd_configuration->messaging_plugin_conf.bmd_messaging_plugin_register_action)
	{
		for (i=0; i<(*bmdJSResponseSet)->bmdDatagramSetSize; i++)
		{
			status=_GLOBAL_bmd_configuration->messaging_plugin_conf.bmd_messaging_plugin_register_action(hDB, bmdJSRequestSet->bmdDatagramSetTable[0], (*bmdJSResponseSet)->bmdDatagramSetTable[i], req);
			if (status<BMD_OK)
			{
				(*bmdJSResponseSet)->bmdDatagramSetTable[i]->datagramStatus = status;
			}
		}
	}

	/***********************/
	/* konczymy transakcje */
	/***********************/
	bmd_db_end_transaction(hDB);


	return BMD_OK;
}



long GenerateSearchQuery(	void *hDB,
				bmdDatagram_t *bmdDatagram,
				EnumSearchResultMode_t SearchResultMode,
				long ignoreDependentGroups,
				char **SelectQuery,
				char **SelectLimits,
				char **SelectQueryColumns,
				server_request_data_t *req)
{
long sort_oids_count			= 0;
long sort_orders_count			= 0;
long i					= 0;
long add_met_count			= 0;
long sys_met_count			= 0;
long act_met_count			= 0;
long pki_met_count			= 0;
char **sort_oids			= NULL;
char **sort_orders			= NULL;
char *sort				= NULL;
char *sort_oid				= NULL;
char *SQLBegin				= NULL;
char *SQLFinish				= NULL;
char *OffsetsAndLimits			= NULL;
char *string_temp			= NULL;
char *metadataConditions		= NULL;
char *string_temp2			= NULL;
char *tmp				= NULL;

char **anss				= NULL;
long ansCount				= 0;
char *order_clause			= NULL;

	PRINT_INFO("LIBBMDSQLINF Generating SQL query\n");

	if (hDB == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (SelectQuery == NULL)	{	BMD_FOK(BMD_ERR_PARAM3);	}
	if (*SelectQuery != NULL)	{	BMD_FOK(BMD_ERR_PARAM3);	}

	/*************************************************/
	/* wydobycie oid'a metadanej po ktorej sortujemy */
	/*************************************************/
	if (	(bmdDatagram->datagramType==BMD_DATAGRAM_DIR_LIST) ||
		(bmdDatagram->datagramType==BMD_DATAGRAM_DIR_LIST_WITH_OR) ||
		(bmdDatagram->datagramType==BMD_DATAGRAM_TYPE_SEARCH) ||
		(bmdDatagram->datagramType==BMD_DATAGRAM_TYPE_UPDATE) ||
		(bmdDatagram->datagramType==BMD_DATAGRAM_GET_METADATA_WITH_TIME))
	{
		/***************************************************************/
		/* budowanie naglowka zapytania (zakresu wynikow wyszukiwania) */
		/***************************************************************/
		//SELECT id, type, name, sql_cast_string, default_value FROM add_metadata_types;
		BMD_FOK(getColumnWithCondition(	_GLOBAL_AddMetadataTypesDictionary, 0, NULL, 1, &ansCount, &anss));

		asprintf(&SQLBegin, "SELECT DISTINCT crypto_objects.id, crypto_objects.location_id, crypto_objects.file_type, crypto_objects.pointing_location_id, crypto_objects.pointing_id, crypto_objects.name, crypto_objects.filesize, crypto_objects.fk_owner, crypto_objects.insert_date, crypto_objects.fk_group, crypto_objects.corresponding_id, crypto_objects.visible, crypto_objects.present, crypto_objects.transaction_id, crypto_objects.creator, crypto_objects.for_grant, crypto_objects.hash_value, crypto_objects.expiration_date, crypto_objects.for_timestamping " );
		if (SQLBegin==NULL)	{	BMD_FOK(NO_MEMORY);	}

		for (i=0; i<ansCount; i++)
		{

			BMD_FOK(bmd_str_replace(&(anss[i]), ".", "_"));

			asprintf(&tmp, "%s", SQLBegin);
			if (tmp==NULL)	{	BMD_FOK(NO_MEMORY);	}
			free0(SQLBegin);

			asprintf(&SQLBegin, "%s, crypto_objects.amv_%s", tmp, anss[i]);
			if (SQLBegin==NULL)	{	BMD_FOK(NO_MEMORY);	}
			free0(tmp);
			free0(anss[i]);
		}
		free0(anss);


		if (	(bmdDatagram->datagramType!=BMD_DATAGRAM_TYPE_UPDATE) &&
			(bmdDatagram->protocolData!=NULL) &&
			(bmdDatagram->protocolData->buf!=NULL) &&
			(bmdDatagram->protocolData->size>0) && (bmdDatagram->datagramType!=BMD_DATAGRAM_TYPE_UPDATE))
		{
			/**************************************************************************/
			/* dodanie opcjonalnych kolumn do wyniku (w przypadku sortowania po nich) */
			/**************************************************************************/
			free0(sort);
			asprintf(&sort, "%s", bmdDatagram->protocolData->buf);
			if(sort == NULL)	{	BMD_FOK(NO_MEMORY);	}
			BMD_FOK(GetSortConditions(sort, req, &sort_oids, &sort_oids_count, &sort_orders, &sort_orders_count));

			for (i=0; i<sort_orders_count; i++)
			{
				free0(sort_oid);
				asprintf(&sort_oid, "%s", sort_oids[i]);
				if(sort_oid == NULL)	{	BMD_FOK(NO_MEMORY);	}

				/***********************************/
				/* jesli sortujemy po nazwie pliku */
				/***********************************/
				if (strcmp(sort_oid,OID_SYS_METADATA_CRYPTO_OBJECTS_FILENAME)==0)
				{
					asprintf(&string_temp, "%s, crypto_objects.name", SQLBegin);
					free0(SQLBegin);
					SQLBegin = string_temp; string_temp = NULL;
					if(SQLBegin == NULL)	{	BMD_FOK(NO_MEMORY);	}
				}
				/**************************************/
				/* jesli sortujemy po rozmiarze pliku */
				/**************************************/
				else if (strcmp(sort_oid,OID_SYS_METADATA_CRYPTO_OBJECTS_FILESIZE)==0)
				{
					asprintf(&string_temp, "%s, crypto_objects.filesize", SQLBegin);
					free0(SQLBegin);
					SQLBegin = string_temp; string_temp = NULL;
					if(SQLBegin == NULL)	{	BMD_FOK(NO_MEMORY);	}
				}
				/*********************************************/
				/* jesli sortujemy po dacie wstawienia pliku */
				/*********************************************/
				else if (strcmp(sort_oid,OID_SYS_METADATA_CRYPTO_OBJECTS_TIMESTAMP)==0)
				{
					asprintf(&string_temp, "%s, crypto_objects.insert_date", SQLBegin);
					free0(SQLBegin);
					SQLBegin = string_temp; string_temp = NULL;
					if(SQLBegin == NULL)	{	BMD_FOK(NO_MEMORY);	}
				}
				/****************************************/
				/* jesli sortujemy po widocznosci pliku */
				/****************************************/
				else if (strcmp(sort_oid,OID_SYS_METADATA_CRYPTO_OBJECTS_VISIBLE)==0)
				{
					asprintf(&string_temp, "%s, crypto_objects.visible", SQLBegin);
					free0(SQLBegin);
					SQLBegin = string_temp; string_temp = NULL;
					if(SQLBegin == NULL)	{	BMD_FOK(NO_MEMORY);	}
				}
				/**************************************/
				/* jesli sortujemy po obecnosci pliku */
				/**************************************/
				else if (strcmp(sort_oid,OID_SYS_METADATA_CRYPTO_OBJECTS_PRESENT)==0)
				{
					asprintf(&string_temp, "%s, crypto_objects.present", SQLBegin);
					free0(SQLBegin);
					SQLBegin = string_temp; string_temp = NULL;
					if(SQLBegin == NULL)	{	BMD_FOK(NO_MEMORY);	}
				}
				/*********************************************/
				/* jesli sortujemy po corresponding id pliku */
				/*********************************************/
				else if (strcmp(sort_oid,OID_SYS_METADATA_CRYPTO_OBJECTS_CORESPONDING_ID)==0)
				{
					asprintf(&string_temp, "%s, crypto_objects.corresponding_id", SQLBegin);
					free0(SQLBegin);
					SQLBegin = string_temp; string_temp = NULL;
					if(SQLBegin == NULL)	{	BMD_FOK(NO_MEMORY);	}
				}
				/******************************************/
				/* jesli sortujemy po id transakcji pliku */
				/******************************************/
				else if (strcmp(sort_oid,OID_SYS_METADATA_CRYPTO_OBJECTS_TRANSACTION_ID)==0)
				{
					asprintf(&string_temp, "%s, crypto_objects.transaction_id", SQLBegin);
					free0(SQLBegin);
					SQLBegin = string_temp; string_temp = NULL;
					if(SQLBegin == NULL)	{	BMD_FOK(NO_MEMORY);	}
				}
			}
		}

		asprintf(&string_temp, "%s %s\n", SQLBegin, "FROM crypto_objects");
		free0(SQLBegin);

		SQLBegin = string_temp; string_temp = NULL;
		if(SQLBegin == NULL)	{	BMD_FOK(NO_MEMORY);	}
		if (SelectQueryColumns!=NULL)
		{
			asprintf(SelectQueryColumns, "%s", SQLBegin);
			if(*SelectQueryColumns == NULL)	{	BMD_FOK(NO_MEMORY);	}
		}


	}
	else if (bmdDatagram->datagramType==BMD_DATAGRAM_TYPE_CONSERVATE_TIMESTAMPS)
	{
		free0(SQLBegin);
		asprintf(&SQLBegin, "SELECT prf_timestamps.timestamp, prf_timestamps.id, prf_timestamps.fk_crypto_objects, prf_timestamps.fk_objects, prf_timestamps.timestamp_type FROM prf_timestamps");
		if(SQLBegin == NULL)	{	BMD_FOK(NO_MEMORY);	}
		if (SelectQueryColumns!=NULL)
		{
			asprintf(SelectQueryColumns, "%s", SQLBegin);
			if(*SelectQueryColumns == NULL)	{	BMD_FOK(NO_MEMORY);	}
		}
	}
	else
	{

		if (SearchResultMode==SEARCH_RESULT_LIST)
		{
			free0(SQLBegin);
			asprintf(&SQLBegin, "SELECT DISTINCT crypto_objects.id, crypto_objects.location_id, crypto_objects.file_type, crypto_objects.pointing_location_id, crypto_objects.pointing_id, crypto_objects.name FROM crypto_objects");
			if(SQLBegin == NULL)	{	BMD_FOK(NO_MEMORY);	}
			if (SelectQueryColumns!=NULL)
			{
				asprintf(SelectQueryColumns, "%s", SQLBegin);
				if(*SelectQueryColumns == NULL)	{	BMD_FOK(NO_MEMORY);	}
			}
		}
		else if (SearchResultMode==SEARCH_RESULT_COUNT)
		{
			free0(SQLBegin);
			asprintf(&SQLBegin, "SELECT count(DISTINCT crypto_objects.id) FROM crypto_objects");
			if(SQLBegin == NULL)	{	BMD_FOK(NO_MEMORY);	}
			if (SelectQueryColumns!=NULL)
			{
				asprintf(SelectQueryColumns, "%s", SQLBegin);
				if(*SelectQueryColumns == NULL)	{	BMD_FOK(NO_MEMORY);	}
			}
		}
		else
		{

			BMD_FOK(-666);
		}
	}

	/**************************************************************************************/
	/* stworzenie warunków wyszukiwania na podstawie metadanych systemowych i dodatkowych */
	/**************************************************************************************/
	BMD_FOK(ConcatenateWithMetadataConditions(	hDB,
							SQLBegin,
							bmdDatagram,
							&add_met_count,
							&sys_met_count,
							&act_met_count,
							&pki_met_count,
							&metadataConditions));

	/*********************************************************/
	/* warunkowe zjoinowanie z tabela metadanych dodatkowych */
	/*********************************************************/
	if ((sys_met_count>0) && (bmdDatagram->datagramType==BMD_DATAGRAM_TYPE_CONSERVATE_TIMESTAMPS))
	{
		free0(string_temp2);
		asprintf(&string_temp2, "%s", SQLBegin);
		free0(SQLBegin);
		asprintf(&SQLBegin, "%s \nLEFT JOIN crypto_objects ON (prf_timestamps.fk_crypto_objects=crypto_objects.id AND prf_timestamps.location_id=crypto_objects.location_id)", string_temp2);
		free0(string_temp2);
	}

	/**************************************************/
	/* warunkowe zjoinowanie z tabela action_register */
	/**************************************************/
	if ((act_met_count>0) && (bmdDatagram->datagramType!=BMD_DATAGRAM_TYPE_UPDATE))
	{
		if (bmdDatagram->datagramType==BMD_DATAGRAM_TYPE_CONSERVATE_TIMESTAMPS)
		{
			free0(string_temp2);
			asprintf(&string_temp2, "%s", SQLBegin);
			free0(SQLBegin);
			asprintf(&SQLBegin, "%s \nLEFT JOIN action_register ON (prf_timestamps.fk_crypto_objects=action_register.fk_objects AND prf_timestamps.location_id=action_register.location_id AND action_register.objects_type=%i)", string_temp2, CRYPTO_OBJECT_OBJECT_TYPE);
			free0(string_temp2);
		}
		else
		{
			free0(string_temp2);
			asprintf(&string_temp2, "%s", SQLBegin);
			free0(SQLBegin);
			asprintf(&SQLBegin, "%s \nLEFT JOIN action_register ON ((crypto_objects.id=action_register.fk_objects OR crypto_objects.pointing_id=action_register.fk_objects) AND crypto_objects.location_id=action_register.location_id AND action_register.objects_type=%i)", string_temp2, CRYPTO_OBJECT_OBJECT_TYPE);
			free0(string_temp2);
		}
	}


	if(SQLBegin==NULL)
	{
		BMD_FOK(NO_MEMORY);
	}

	if (bmdDatagram->datagramType!=BMD_DATAGRAM_TYPE_CONSERVATE_TIMESTAMPS)
	{
		/*************************************************************************/
		/* warunki wyszukiwania dla kolejnych danych z roznych rodzajow warunkow */
		/*************************************************************************/
		BMD_FOK(ConcatenateWithWhereUserId(hDB, SQLBegin, bmdDatagram, ignoreDependentGroups, &SQLFinish,req));
		free0(SQLBegin);
		asprintf(&SQLBegin, "\n%s", SQLFinish);
		free0(SQLFinish);
	}

	/*************************************************************/
	/* dolaczenie wygenerowanych wczesniej warunkow wyszukiwania */
	/*************************************************************/
	if (bmdDatagram->datagramType!=BMD_DATAGRAM_TYPE_UPDATE)
	{
		if (bmdDatagram->datagramType==BMD_DATAGRAM_TYPE_CONSERVATE_TIMESTAMPS)
		{
			if (pki_met_count>0)
			{
				free0(string_temp2);
				asprintf(&string_temp2, "\n%s", SQLBegin);
				free0(SQLBegin);
				asprintf(&SQLBegin, "%s\nWHERE 1 = 1\nAND (\n%s\t)", string_temp2,metadataConditions==NULL?"":metadataConditions);
				free0(string_temp2);
				free0(metadataConditions);
			}
			else
			{
				free0(string_temp2);
				asprintf(&string_temp2, "\n%s", SQLBegin);
				free0(SQLBegin);
				asprintf(&SQLBegin, "%s\n", string_temp2);
				free0(string_temp2);
			}
		}
		else
		{
			free0(string_temp2);
			asprintf(&string_temp2, "\n%s", SQLBegin);
			free0(SQLBegin);
			asprintf(&SQLBegin, "%s\nAND (\n%s\t)", string_temp2,metadataConditions==NULL?"":metadataConditions);
			free0(string_temp2);
			free0(metadataConditions);
		}
	}

	if(	(bmdDatagram->datagramType==BMD_DATAGRAM_TYPE_SEARCH) ||
		(bmdDatagram->datagramType==BMD_DATAGRAM_DIR_LIST) ||
		(bmdDatagram->datagramType==BMD_DATAGRAM_DIR_LIST_WITH_OR))
	{

		order_clause = NULL;
		BMD_FOK(bmd_db_generate_order_clause(bmdDatagram, req, &order_clause));
		BMD_FOK(bmd_db_paginate_sql_query(SQLBegin,order_clause,bmdDatagram,0,100,SelectQuery, SelectLimits));
		free0(order_clause);
	}
	else if (bmdDatagram->datagramType==BMD_DATAGRAM_TYPE_CONSERVATE_TIMESTAMPS)
	{
		asprintf(SelectQuery, "\n%s;", SQLBegin);
	}
	else
	{
		BMD_FOK(bmd_db_paginate_sql_query(SQLBegin,NULL,NULL,0,1,SelectQuery,SelectLimits));
	}

	/******************/
	/*	porzadki	*/
	/******************/
	for (i=0; i<sort_oids_count; i++)
	{
		free0(sort_oids[i]);
	}
	free0(sort_oids);

	for (i=0; i<sort_orders_count; i++)
	{
		free0(sort_orders[i]);
	}
	free0(sort_orders);
	free0(sort);
	free0(sort_oid);
	free0(string_temp);
	free0(metadataConditions);
	free0(string_temp2);
	free0(SQLBegin);
	free0(SQLFinish);
	free0(OffsetsAndLimits);

	return BMD_OK;
}

long FillbmdDatagramSetFromSelectResult(	void *hDB,
						void *SelectResult,
						bmdDatagramSet_t *bmdJSResponseSet,
						server_request_data_t *req)
{
long status			= 0;
long fetched_counter		= 0;
long i				= 0;
long j				= 0;
long addMetadataNumber		= 0;
char *cmpOid			= 0;
char **addMetadataOid		= NULL;
bmdDatagram_t *tmpbmdDatagram	= NULL;
db_row_strings_t *rowstruct	= NULL;

	PRINT_INFO("LIBBMDSQLINF Filling datagramset from SQL query result\n");
	/************************/
	/* walidacja parametrow */
	/************************/
	if(hDB == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(SelectResult == NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(bmdJSResponseSet == NULL)	{	BMD_FOK(BMD_ERR_PARAM3);	}

	/********************************************/
	/* okreslenie liczby metadanych dodatkowych */
	/********************************************/
	//SELECT id, type, name, sql_cast_string, default_value FROM add_metadata_types;
	BMD_FOK(getColumnWithCondition(	_GLOBAL_AddMetadataTypesDictionary, 0, NULL, 1, &addMetadataNumber, &addMetadataOid));

	/*****************************************************/
	/* odniesienie do wszystkich znalezionych dokumentow */
	/*****************************************************/
	while(status>=0)
	{
		status = bmd_db_result_get_row(hDB, SelectResult, 0, 0, &rowstruct, FETCH_NEXT, &fetched_counter);
		if(status==BMD_ERR_NODATA || status==BMD_ERR_NODATA)
		{
			status=0;
			break;
		}
		else if (status<BMD_OK)
		{
			BMD_FOK(status);
		}

		BMD_FOK(bmd_datagram_create(BMD_DATAGRAM_TYPE_SEARCH_RESPONSE,&tmpbmdDatagram));

		/*************************************************/
		/* wypelnianie pojedynczego datagramu odpowiedzi */
		/*************************************************/
		for (i=0; i<addMetadataNumber+CRYPTO_OBJECTS_SYS_METADATA_COUNT; i++)
		{
			if (i>=rowstruct->size)	break;
			if ((rowstruct->colvals[i]!=NULL) && (strlen(rowstruct->colvals[i])>0))
			{

// crypto_objects.filesize, crypto_objects.fk_owner, crypto_objects.insert_date, crypto_objects.fk_group, crypto_objects.corresponding_id, crypto_objects.visible, crypto_objects.present, crypto_objects.transaction_id,
				/************************************************************/
				/* ograniczenie dlugosci zwracanych metadanych wg kontrolek */
				/************************************************************/
				switch(i)
				{
					case 2	:	asprintf(&cmpOid, "%s", OID_SYS_METADATA_CRYPTO_OBJECTS_FILE_TYPE); break;
					case 3	:	asprintf(&cmpOid, "%s", OID_SYS_METADATA_CRYPTO_OBJECTS_POINTING_LOCATION_ID); break;
					case 4	:	asprintf(&cmpOid, "%s", OID_SYS_METADATA_CRYPTO_OBJECTS_POINTING_ID); break;
					case 6	:	asprintf(&cmpOid, "%s", OID_SYS_METADATA_CRYPTO_OBJECTS_FILESIZE); break;
					case 7	:	asprintf(&cmpOid, "%s", OID_SYS_METADATA_CRYPTO_OBJECTS_OWNER); break;
					case 8	:	asprintf(&cmpOid, "%s", OID_SYS_METADATA_CRYPTO_OBJECTS_TIMESTAMP); break;
					case 9	:	asprintf(&cmpOid, "%s", OID_SYS_METADATA_CRYPTO_OBJECTS_GROUP); break;
					case 10	:	asprintf(&cmpOid, "%s", OID_SYS_METADATA_CRYPTO_OBJECTS_CORESPONDING_ID); break;
					case 11	:	asprintf(&cmpOid, "%s", OID_SYS_METADATA_CRYPTO_OBJECTS_VISIBLE); break;
					case 12	:	asprintf(&cmpOid, "%s", OID_SYS_METADATA_CRYPTO_OBJECTS_PRESENT); break;
					case 13	:	asprintf(&cmpOid, "%s", OID_SYS_METADATA_CRYPTO_OBJECTS_TRANSACTION_ID); break;
					case 14	:	asprintf(&cmpOid, "%s", OID_SYS_METADATA_CRYPTO_OBJECTS_CREATOR); break;
					case 15	:	asprintf(&cmpOid, "%s", OID_SYS_METADATA_CRYPTO_OBJECTS_FOR_GRANT); break;
					case 16	:	asprintf(&cmpOid, "%s", OID_SYS_METADATA_CRYPTO_OBJECTS_HASH_VALUE); break;
					case 17	:	asprintf(&cmpOid, "%s", OID_SYS_METADATA_CRYPTO_OBJECTS_EXPIRATION_DATE); break;
					case 18	:	asprintf(&cmpOid, "%s", OID_SYS_METADATA_CRYPTO_OBJECTS_FOR_TIMESTAMPING); break;
					default	:	if (i>=CRYPTO_OBJECTS_SYS_METADATA_COUNT)	asprintf(&cmpOid, "%s", addMetadataOid[i-CRYPTO_OBJECTS_SYS_METADATA_COUNT]); break;
				}

				if (cmpOid!=NULL)
				{
					for (j=0; j<req->no_of_formularz; j++)
					{
						if (	(strcmp(req->formularz[j]->oid_str, cmpOid)==0) &&
							(req->formularz[j]->maxLength>-1) &&
							((long)strlen(rowstruct->colvals[i])>req->formularz[j]->maxLength) )
						{
							rowstruct->colvals[i][req->formularz[j]->maxLength]='\0';
							break;
						}
					}
				}

				if (i==0)	{	BMD_FOK(set_gen_buf2(rowstruct->colvals[i], (long)strlen(rowstruct->colvals[i]), &(tmpbmdDatagram->protocolDataFileId)));	}
				else if (i==1)	{	BMD_FOK(set_gen_buf2(rowstruct->colvals[i], (long)strlen(rowstruct->colvals[i]), &(tmpbmdDatagram->protocolDataFileLocationId)));	}
				else if (i==2)	{	BMD_FOK(bmd_datagram_add_metadata_char(	OID_SYS_METADATA_CRYPTO_OBJECTS_FILE_TYPE, rowstruct->colvals[i], &tmpbmdDatagram));	}
				else if (i==3)	{	BMD_FOK(bmd_datagram_add_metadata_char(	OID_SYS_METADATA_CRYPTO_OBJECTS_POINTING_LOCATION_ID, rowstruct->colvals[i], &tmpbmdDatagram));	}
				else if (i==4)	{	BMD_FOK(bmd_datagram_add_metadata_char(	OID_SYS_METADATA_CRYPTO_OBJECTS_POINTING_ID, rowstruct->colvals[i], &tmpbmdDatagram));	}
				else if (i==5)	{	BMD_FOK(set_gen_buf2(rowstruct->colvals[i], (long)strlen(rowstruct->colvals[i]), &(tmpbmdDatagram->protocolDataFilename)));	}
				else
				{
					BMD_FOK(bmd_datagram_add_metadata_char(	cmpOid, rowstruct->colvals[i], &tmpbmdDatagram));
				}
				free0(cmpOid);
			}
		}

		bmd_db_row_struct_free(&rowstruct);
		BMD_FOK(bmd_datagram_add_to_set(tmpbmdDatagram,&bmdJSResponseSet));
		tmpbmdDatagram = NULL;

	}

	if(fetched_counter == 0)
	{
		BMD_FOK(bmd_datagram_create(BMD_DATAGRAM_TYPE_SEARCH_RESPONSE,&tmpbmdDatagram));
		tmpbmdDatagram->datagramStatus = BMD_ERR_NODATA;
		BMD_FOK(bmd_datagram_add_to_set(tmpbmdDatagram,&bmdJSResponseSet));

		return BMD_OK;
	}

	/************/
	/* porzadki */
	/************/
	for (i=0; i<addMetadataNumber; i++)
	{
		free0(addMetadataOid[i]);
	}
	free0(addMetadataOid);
	free0(cmpOid);

	return BMD_OK;
}

/**
* @param ignoreDependentGroups wartosc <= 0 , ze brane bedzie pod uwage caly graf grup,
* w przeciwnym razie pod uwage wzieta zostanie jedyni grupa, na ktora uzytkownik sie zalogowal 
* @param conditionPart [out] NULL gdy grupy nie brane pod uwage,
* w przeciwnym razie lanuch znakowy z warunkiem fk_group=wartosc1 OR fk_group=wartosc2 ...
* 
* @retval BMD_OK gdy wygenerowany warunek
* @retval < BMD_OK, gdy wystapil blad 
*/
long BuildSQLConditionPartForGroupsGraph(long ignoreDependentGroups, server_request_data_t *req, char** conditionPart)
{
long iter			= 0;
char* sqlCondition		= NULL;
char* sqlConditionTemp		= NULL;

char **DependentGroupsList	= NULL;
long no_of_DependentGroups	= 0;

	if(req == NULL)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(req->ua == NULL)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(req->ua->user_chosen_group == NULL)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(req->ua->user_chosen_group->buf == NULL || req->ua->user_chosen_group->size <= 0)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(conditionPart == NULL)
		{ BMD_FOK(BMD_ERR_PARAM3); }
	if(*conditionPart != NULL)
		{ BMD_FOK(BMD_ERR_PARAM3); }
	if(_GLOBAL_bmd_configuration == NULL)
		{ BMD_FOK(BMD_ERR_OP_FAILED); }

	
	// warunek o grupach dodany tylko, jesli ustawienia konfiguracyjne na to pozwalaja
	if (_GLOBAL_bmd_configuration->enable_group_check==1)
	{
		if(ignoreDependentGroups <= 0)
		{
			//wygenerowanie listy grup podleglych grupie uzytkownika
			BMD_FOK(GenerateDependentGroupsIdList(req->ua->user_chosen_group->buf, &DependentGroupsList, &no_of_DependentGroups));
	
			asprintf(&sqlConditionTemp," (crypto_objects.fk_group='%s'", req->ua->user_chosen_group->buf);
			if(sqlConditionTemp == NULL)
			{
				for(iter=0; iter<no_of_DependentGroups; iter++)
					{ free(DependentGroupsList[iter]); }
				free(DependentGroupsList);
				BMD_FOK(NO_MEMORY);
			}
	
			for (iter=0; iter<no_of_DependentGroups; iter++)
			{
				asprintf(&sqlCondition,"%s OR crypto_objects.fk_group='%s'",sqlConditionTemp, DependentGroupsList[iter]);
				free(sqlConditionTemp); sqlConditionTemp=NULL;
				if(sqlCondition == NULL)
				{
					for(iter=0; iter<no_of_DependentGroups; iter++)
						{ free(DependentGroupsList[iter]); }
					free(DependentGroupsList);
					BMD_FOK(NO_MEMORY);
				}
				sqlConditionTemp=sqlCondition;
				sqlCondition = NULL;
			}
	
			for(iter=0; iter<no_of_DependentGroups; iter++)
				{ free(DependentGroupsList[iter]); }
			free(DependentGroupsList); DependentGroupsList=NULL;
			no_of_DependentGroups=0;

			asprintf(&sqlCondition,"%s)", sqlConditionTemp);
			free(sqlConditionTemp); sqlConditionTemp=NULL;
			if(sqlCondition == NULL)
				{ BMD_FOK(NO_MEMORY); }
		}
		else
		{
			asprintf(&sqlCondition, " (crypto_objects.fk_group='%s') ", req->ua->user_chosen_group->buf);
			if(sqlCondition == NULL)
				{ BMD_FOK(NO_MEMORY); }
		}
	}
	else
	{
		sqlCondition=NULL;
	}

	*conditionPart=sqlCondition;
	sqlCondition=NULL;

	return BMD_OK;
}



/************************************************************************************/
/*	funkcja generuje czesc zapytania sql do bazy danych zwiazana z id obiektu,	*/
/*	grupa uzytkownika, oraz poziomami poufnosci						*/
/************************************************************************************/
long ConcatenateWithWhereUserId(	void *hDB,
					char *SQLAfterAddMtd,
					bmdDatagram_t *bmdDatagram,
					long ignoreDependentGroups,
					char **SQLPrefix,
					server_request_data_t *req)
{
long status				= 0;
long i					= 0;
char *SQLPrefixGroups			= NULL;
char *SQLPrefixClearances		= NULL;
char *string_temp			= NULL;
char *sec_level_prio			= NULL;
char* groupsCondtionPart		= NULL;
char* escapeTemp				= NULL;

	PRINT_INFO("LIBBMDSQLINF Concatenating with user id\n");

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if(SQLAfterAddMtd == NULL)
		{ BMD_FOK(BMD_ERR_PARAM1); }
	if(bmdDatagram == NULL)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(SQLPrefix == NULL)
		{ BMD_FOK(BMD_ERR_PARAM3); }
	if(*SQLPrefix != NULL)
		{ BMD_FOK(BMD_ERR_PARAM3); }
	if(req == NULL)
		{ BMD_FOK(BMD_ERR_PARAM4); }
	if(req->ua == NULL)
		{ BMD_FOK(BMD_ERR_PARAM4); }
	if(req->ua->user_chosen_group == NULL)
		{ BMD_FOK(BMD_ERR_PARAM4); }
	if(req->ua->security == NULL)
		{ BMD_FOK(BMD_ERR_PARAM4); }

	//pominiecie autorayzacji w przypadku, gdy potrzebujemy wyciagnac informacje bez koniecznosci fitracji wzgledem uprawnien
	if(bmdDatagram->datagramStatus & BMD_REQ_DB_SELECT_SKIP_AUTHORIZATION)
	{
		asprintf(SQLPrefix,"%s WHERE 1=1 ", SQLAfterAddMtd);
		if(SQLPrefix==NULL)
			{ BMD_FOK(NO_MEMORY); }
	}
	else if(bmdDatagram->datagramStatus & BMD_REQ_DB_SELECT_ONLY_OWNER)
	{
		asprintf(SQLPrefix,"%s WHERE crypto_objects.fk_owner = '%s'", SQLAfterAddMtd, req->ua->identityId->buf);
		if(SQLPrefix==NULL)
			{ BMD_FOK(NO_MEMORY); }
	}
	else if(bmdDatagram->datagramStatus & BMD_REQ_DB_SELECT_ONLY_CREATOR)
	{
		asprintf(SQLPrefix,"%s WHERE crypto_objects.creator = '%s'", SQLAfterAddMtd, req->ua->identityId->buf);
		if(SQLPrefix==NULL)
			{ BMD_FOK(NO_MEMORY); }
	}
	else if(bmdDatagram->datagramStatus & BMD_REQ_DB_SELECT_CREATOR_OR_OWNER)
	{
		asprintf(SQLPrefix,"%s WHERE (crypto_objects.creator = '%s' OR crypto_objects.fk_owner = '%s')",
				SQLAfterAddMtd, req->ua->identityId->buf, req->ua->identityId->buf);
		if(SQLPrefix==NULL)
			{ BMD_FOK(NO_MEMORY); }
	}
	else if(bmdDatagram->datagramStatus & BMD_REQ_DB_SELECT_CREATOR_OR_OWNER_OR_GROUPS)
	{
		status=BuildSQLConditionPartForGroupsGraph(ignoreDependentGroups, req, &groupsCondtionPart);
		if(status < BMD_OK)
		{
			BMD_FOK(status);
		}
		
		if(groupsCondtionPart != NULL)
		{
			asprintf(&SQLPrefixGroups, " OR %s", groupsCondtionPart);
		}
		else
		{
			SQLPrefixGroups=strdup("");
		}
		free(groupsCondtionPart); groupsCondtionPart=NULL;

		if(SQLPrefixGroups == NULL)
		{
			BMD_FOK(NO_MEMORY);
		}

		//pamietac o zamknieciu nawiasu
		asprintf(SQLPrefix,"%s WHERE (crypto_objects.creator = '%s' OR crypto_objects.fk_owner = '%s' %s )", SQLAfterAddMtd, req->ua->identityId->buf, req->ua->identityId->buf, SQLPrefixGroups);
		free(SQLPrefixGroups); SQLPrefixGroups=NULL;
		if(SQLPrefix == NULL)
			{ BMD_FOK(NO_MEMORY); }

	}
	/******************************************************************/
	/*	w przypadku, gdy szukamy dokumentow grupy i grup podleglych	*/
	/******************************************************************/
	else
	{
		status=BuildSQLConditionPartForGroupsGraph(ignoreDependentGroups, req, &groupsCondtionPart);
		if(status < BMD_OK)
		{
			BMD_FOK(status);
		}
		
		if(groupsCondtionPart != NULL)
		{
			asprintf(&SQLPrefixGroups, " AND %s", groupsCondtionPart);
		}
		else
		{
			SQLPrefixGroups=strdup("");
		}
		free(groupsCondtionPart); groupsCondtionPart=NULL;

		if(SQLPrefixGroups == NULL)
		{
			BMD_FOK(NO_MEMORY);
		}
		
		asprintf(SQLPrefix,"%s WHERE 1=1 %s", SQLAfterAddMtd, SQLPrefixGroups);
		free(SQLPrefixGroups); SQLPrefixGroups=NULL;
		if(SQLPrefix == NULL)
			{ BMD_FOK(NO_MEMORY); }

	}

	/************************************************************************/
	/*	dodanie warunku o id pliku (jesli umieszczony jest w datagramie	*/
	/*	w przypadku pobierania pliku, jego metadanych lub usuwania		*/
	/************************************************************************/
	if ((bmdDatagram->protocolDataFileId!=NULL) && (bmdDatagram->protocolDataFileId->buf!=NULL) && (bmdDatagram->protocolDataFileId->size>0))
	{

		for (i=0; i<bmdDatagram->protocolDataFileId->size; i++)
		{
			if (bmdDatagram->protocolDataFileId->buf[i]=='\0')	break;
			if ((bmdDatagram->protocolDataFileId->buf[i]<'0') || (bmdDatagram->protocolDataFileId->buf[i]>'9'))
			{
				status=-1;
				break;
			}
		}

		if (status==BMD_OK)
		{
			/* ORACLE_BIND */
			asprintf(&string_temp,"%s \nAND crypto_objects.id=%s", *SQLPrefix,bmdDatagram->protocolDataFileId->buf);
			free0(*SQLPrefix);
			*SQLPrefix = string_temp; string_temp = NULL;
		}
		status=BMD_OK;

	}

	/********************************************************************************/
	/*	dodanie warunku o nazwie pliku (jesli umieszczony jest w datagramie	*/
	/*	w przypadku pobierania pliku, jego metadanych lub usuwania		*/
	/********************************************************************************/
	if ((bmdDatagram->protocolDataFilename!=NULL) && (bmdDatagram->protocolDataFilename->buf!=NULL) && (bmdDatagram->protocolDataFilename->size>0))
	{
		BMD_FOK(bmd_db_escape_string(hDB, bmdDatagram->protocolDataFilename->buf, STANDARD_ESCAPING, &escapeTemp));
		asprintf(&string_temp,"%s \nAND crypto_objects.name='%s'", *SQLPrefix, escapeTemp);
		free(escapeTemp); escapeTemp=NULL;
		free0(*SQLPrefix);
		*SQLPrefix = string_temp; string_temp = NULL;
	}

	/***************************************************/
	/* wygenerowanie czesci zapytania dla clearance'ow */
	/***************************************************/
	if ( (_GLOBAL_bmd_configuration->enable_clearance_check==1) && ((bmdDatagram->datagramStatus & BMD_REQ_DB_SELECT_SKIP_AUTHORIZATION) == 0) )
	{
		i=0;
		asprintf(&SQLPrefixClearances,"\nAND (\t   (crypto_objects.fk_sec_categories=1 AND crypto_objects.fk_sec_levels=0)  ");
		if(SQLPrefixClearances == NULL)
		{
			BMD_FOK(NO_MEMORY);
		}

		for (i=0; i<req->ua->security->size; i+=2)
		{
			if ((req->ua->security->gbufs[i]!=NULL)	&& (req->ua->security->gbufs[i+1]!=NULL))
			{
				//SELECT id, type, name, priority FROM sec_levels
				BMD_FOK_CHG(getElementWithCondition(_GLOBAL_SecurityLevelsDictionary, 0, req->ua->security->gbufs[i+1]->buf, 3, &sec_level_prio), LIBBMDSQL_DICT_SEC_LEVEL_VALUE_NOT_FOUND);


				asprintf(&string_temp,"	%s\n\tOR (crypto_objects.fk_sec_categories=%s AND crypto_objects.fk_sec_levels<=%s)",
							SQLPrefixClearances,
							req->ua->security->gbufs[i]->buf,
							sec_level_prio);
				free0(SQLPrefixClearances);
				free0(sec_level_prio);
				SQLPrefixClearances = string_temp; string_temp = NULL;
			}
			PRINT_TEST("req->ua->security: %s\n", req->ua->security->gbufs[i]->buf);
		}

		if (SQLPrefixClearances!=NULL)
		{
			asprintf(&string_temp,"%s %s)",*SQLPrefix,SQLPrefixClearances);
			free(*SQLPrefix); *SQLPrefix = NULL;
			*SQLPrefix = string_temp; string_temp = NULL;
		}
	}

	/************/
	/* porzadki */
	/************/
	free0(SQLPrefixGroups);
	free0(SQLPrefixClearances);

	return BMD_OK;
}
