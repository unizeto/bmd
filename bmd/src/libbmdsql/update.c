#include <bmd/libbmdsql/insert.h>
#include <bmd/libbmdsql/update.h>
#include <bmd/libbmdsql/delete.h>
#include <bmd/libbmdsql/common.h>
#include <bmd/libbmdsql/proofs.h>
#include <bmd/libbmdsql/version.h>
#include <bmd/common/global_types.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>
#include <bmd/libbmdlog/bmd_logs.h>
#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdglobals/libbmdglobals.h>
#include <bmd/libbmdprotocol/libbmdprotocol.h>

#ifdef WIN32
#pragma warning(disable:4100)
#endif
#define NO_ADD_METADATA_FOR_UPDATE	-10

/************************************/
/* wylaczenia/wlaczenie uzytkownika */
/************************************/
long JS_bmdDatagramSet_to_updateMetadataValues(	void *hDB,
						bmdDatagramSet_t *bmdJSRequestSet,
						bmdDatagramSet_t **bmdJSResponseSet,
						server_request_data_t *req)
{
long status	= 0;
long i		= 0;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (hDB==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (bmdJSRequestSet==NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	/*****************************************/
	/* przygotowanie datagramsetu odpowiedzi */
	/*****************************************/
	BMD_FOK(PR2_bmdDatagramSet_fill_with_datagrams(bmdJSRequestSet->bmdDatagramSetSize, bmdJSResponseSet));

	/*******************************************/
	/* pobieranie danych poszczególnych userów */
	/*******************************************/
	for (i=0; i<bmdJSRequestSet->bmdDatagramSetSize; i++)
	{
		/***********************/
		/* poczatek transakcji */
		/***********************/
		BMD_FOK(bmd_db_start_transaction(hDB,BMD_TRANSACTION_LEVEL_NONE));

		status=JS_bmdDatagram_to_updateMetadataValues(hDB, bmdJSRequestSet->bmdDatagramSetTable[i], &((*bmdJSResponseSet)->bmdDatagramSetTable[i]),req);
		if (status<BMD_OK)
		{
			bmd_db_rollback_transaction(hDB,NULL);
		}
		else
		{
			bmd_db_end_transaction(hDB);
		}
		/**************************************/
		/* ustawienie statusu błędu datagramu */
		/**************************************/
		(*bmdJSResponseSet)->bmdDatagramSetTable[i]->datagramType=bmdJSRequestSet->bmdDatagramSetTable[i]->datagramType+100;
		(*bmdJSResponseSet)->bmdDatagramSetTable[i]->datagramStatus=status;

	}

#ifndef WIN32
	kill(getppid(),SIGDICTRELOAD);
#endif //ifndef WIN32


	return BMD_OK;

}

long JS_bmdDatagram_to_updateMetadataValues(	void *hDB,
						bmdDatagram_t *bmdJSRequest,
						bmdDatagram_t **bmdJSResponse,
						server_request_data_t *req)
{
long i				= 0;
long k				= 0;
void *query_result		= NULL;
char *SQLQuery			= NULL;

long no_of_metaData		= 0;
MetaDataBuf_t **metaData	= NULL;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (hDB==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (bmdJSRequest==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}

	for (k=1; k<2; k++)
	{
		switch(k)
		{
// 			case 0	:	no_of_metaData=bmdDatagram->no_of_sysMetaData; metaData=bmdDatagram->sysMetaData; break;
			case 1	:	no_of_metaData=bmdJSRequest->no_of_additionalMetaData; metaData=bmdJSRequest->additionalMetaData; break;
// 			case 2	:	no_of_metaData=bmdDatagram->no_of_actionMetaData; metaData=bmdDatagram->actionMetaData; break;
// 			case 3	:	no_of_metaData=bmdDatagram->no_of_pkiMetaData; metaData=bmdDatagram->pkiMetaData; break;
		}

		for (i=0; i<no_of_metaData; i++)
		{
			asprintf(&SQLQuery, 	"DELETE FROM metadata_values WHERE type='%s';", metaData[i]->OIDTableBuf);
			if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}

			BMD_FOK(bmd_db_execute_sql( hDB, SQLQuery,NULL,NULL, &query_result));
			bmd_db_result_free(&query_result);

			free0(SQLQuery);
		}

		for (i=0; i<no_of_metaData; i++)
		{
			asprintf(&SQLQuery, 	"INSERT INTO metadata_values (id,%s type, value) VALUES (nextval('metadata_values_id_seq'), %s%s'%s', '%s');",
						_GLOBAL_bmd_configuration->location_id==NULL?"":" location_id,",
						_GLOBAL_bmd_configuration->location_id==NULL?"": _GLOBAL_bmd_configuration->location_id,
						_GLOBAL_bmd_configuration->location_id==NULL?"":", ",
						metaData[i]->OIDTableBuf,
						metaData[i]->AnyBuf);

			if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}

			BMD_FOK(bmd_db_execute_sql( hDB, SQLQuery,NULL,NULL, &query_result));
			bmd_db_result_free(&query_result);

			free0(SQLQuery);
		}
	}

	return BMD_OK;
}


/************************************************/
/*	update metadanych - zestaw datagramow	*/
/************************************************/
long JS_bmdDatagramSet_to_update(	void *hDB,
					bmdDatagramSet_t *bmdJSRequestSet,
					bmdDatagramSet_t **bmdJSResponseSet,
					server_request_data_t *req)
{
long status	= 0;
long i		= 0;

	PRINT_INFO("LIBBMDSQLINF Updating metadata (datagramset)\n");
	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if (hDB==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (bmdJSRequestSet==NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	/************************************************/
	/*	przygotowanie datagramsetu odpowiedzi	*/
	/************************************************/
	BMD_FOK(PR2_bmdDatagramSet_fill_with_datagrams(bmdJSRequestSet->bmdDatagramSetSize, bmdJSResponseSet));

	/********************************************************/
	/*	obsługa poszczegolnych datagramow z zadaniami	*/
	/********************************************************/
	for(i=0; i<(int)bmdJSRequestSet->bmdDatagramSetSize; i++)
	{
		/************************************************************************/
		/*	sprawdzenie, czy wczesniej nie wykryto bledu w datagramie zadania	*/
		/************************************************************************/
		if (bmdJSRequestSet->bmdDatagramSetTable[i]->datagramStatus<BMD_OK)
		{
			PR_bmdDatagram_PrepareResponse(&((*bmdJSResponseSet)->bmdDatagramSetTable[i]), bmdJSRequestSet->bmdDatagramSetTable[i]->datagramType+100, bmdJSRequestSet->bmdDatagramSetTable[i]->datagramStatus);
			continue;
		}

		/***********************/
		/* poczatek transakcji */
		/***********************/
		BMD_FOK(bmd_db_start_transaction(hDB,BMD_TRANSACTION_LEVEL_NONE));

		/************************************************/
		/*	obsluzenie pojedynczego datagramu	*/
		/************************************************/
		status=JS_bmdDatagram_to_update(hDB,bmdJSRequestSet->bmdDatagramSetTable[i],&((*bmdJSResponseSet)->bmdDatagramSetTable[i]),req);

		/************************************************/
		/*	ustawienie statusu błędu datagramu	*/
		/************************************************/
		(*bmdJSResponseSet)->bmdDatagramSetTable[i]->datagramType=bmdJSRequestSet->bmdDatagramSetTable[i]->datagramType+100;
		(*bmdJSResponseSet)->bmdDatagramSetTable[i]->datagramStatus=status;

		/****************************************/
		/* zarejestrowanie akcji w bazie danych */
		/****************************************/
		if (_GLOBAL_bmd_configuration->messaging_plugin_conf.bmd_messaging_plugin_register_action)
		{
			status=_GLOBAL_bmd_configuration->messaging_plugin_conf.bmd_messaging_plugin_register_action(hDB, bmdJSRequestSet->bmdDatagramSetTable[i], (*bmdJSResponseSet)->bmdDatagramSetTable[i], req);
			if (status<BMD_OK)
			{
				(*bmdJSResponseSet)->bmdDatagramSetTable[i]->datagramStatus=status;
			}
		}

		if ((*bmdJSResponseSet)->bmdDatagramSetTable[i]->datagramStatus<BMD_OK)
		{
			bmd_db_rollback_transaction(hDB,NULL);
		}
		else
		{
			status=bmd_db_end_transaction(hDB);
			if(status < BMD_OK)
			{
				(*bmdJSResponseSet)->bmdDatagramSetTable[i]->datagramStatus=status;
				bmd_db_rollback_transaction(hDB,NULL);
			}
		}
	}

	return BMD_OK;
}


/**
Funkcja LockCryptoObjectsRow() naklada lock (typu exclusive) na wskazany wiewsz w tabeli crypto_objects.
Funkcja musi byc wywolywana wewnatrz transakcji - w przeciwnym razie zalozenie lock nie jest mozliwe.
Lock jest zdejmowany automatycznie po zakonczeniu transakcji (commit) badz wykonaniu rollback.

@param db_connection_handle to handler polaczenia do bazy danych
@param CryptoObjectNr to identyfikator wiersza, ktory ma zaostac zablokowany

@retval BMD_OK jesli zalozona blokada
@retval <0 jesli wystapil blad
*/
long LockCryptoObjectsRow(	void *db_connection_handle,
				char *CryptoObjectNr	)
{
char *SQLQuery		= NULL;
char *tmpString		= NULL;

	if (db_connection_handle==NULL)	{ BMD_FOK(BMD_ERR_PARAM1); }
	if (CryptoObjectNr==NULL)		{ BMD_FOK(BMD_ERR_PARAM2); }

	asprintf(&SQLQuery, "SELECT id FROM crypto_objects WHERE id=%s FOR UPDATE OF crypto_objects;", CryptoObjectNr);
	if (SQLQuery==NULL)
	{
		BMD_FOK(NO_MEMORY);
	}
	BMD_FOK(ExecuteSQLQueryWithAnswerKnownDBConnection(db_connection_handle, SQLQuery, &tmpString));
	free(SQLQuery);
	free(tmpString);


	return BMD_OK;
}


/******************************************/
/*	update metadanych - jeden datagram	*/
/* wywolanie tej funkcji musi odbywac sie w ramach otwartej transakcji bazodanowej */
/******************************************/
long JS_bmdDatagram_to_update(	void *hDB,
				bmdDatagram_t *bmdJSRequest,
				bmdDatagram_t **bmdJSResponse,
				server_request_data_t *req)
{
char *CryptoObjectNr		= NULL;
char *HiddenMetadataRowId	= NULL;
char **addMetadata		= NULL;
char *SQLQuery			= NULL;
char **linkId			= NULL;
long linkIdCount		= 0;
long addMetadataCount		= 0;
long i				= 0;



	/************************/
	/* walidacja parametrow */
	/************************/
	if (hDB==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (bmdJSRequest==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}


	/*********************************************/
	/* sprawdzanie czy plik do update'a istnieje */
	/*********************************************/
	BMD_FOK(VerifyPermissionsForCryptoObject(hDB, req,
				bmdJSRequest->protocolDataFileId->buf,
				CONSIDER_CREATOR_OR_OWNER_OR_GROUPS,
				NULL /*fileName*/,
				NULL /*fileType*/,
				&CryptoObjectNr,
				NULL /*correspondingId*/,
				NULL /*forGrant*/,
				&addMetadataCount,
				&addMetadata));
	//BMD_FOK(CheckIfCryptoObjectExist(hDB, bmdJSRequest, 0, &CryptoObjectNr, NULL, &location_id, NULL, NULL,  &addMetadataCount, &addMetadata, req));

	// sprawdzenie, czy w zadaniu przekazane sa dopuszczalne metadane dodatkowe 
	BMD_FOK(VerifyAdditionalMetadataTypeAndValue(bmdJSRequest, req, addMetadata, addMetadataCount));
	
	

	/* <zalozenie lock na wiersz w tabeli crypto_objects>
	Wprowadzone, aby procesy rownolegle modyfikujace metadane nie falszowaly historii metadanych (by WSZ)
	Blokada uniemozliwi pominiecie w historii ktoregos ze stanu wartosci metadanych dodatkowych
	lock jest automatycznie zdejmowany przy zakonczeniu transakcji lub rollbacku */
	BMD_FOK(LockCryptoObjectsRow(hDB, CryptoObjectNr));
	/* </zalozenie lock na wiersz w tabeli crypto_objects> */


	/**********************************************************/
	/* wstawienie starych metadanych do tabeli historycznosci */
	/**********************************************************/
	BMD_FOK(JS_PrepareAndExecuteSQLQuery_Insert_AdditionalMetadataHistory( hDB, bmdJSRequest, CryptoObjectNr, _GLOBAL_bmd_configuration->location_id, addMetadataCount, addMetadata));

	/***************************************************/
	/* aktualizacja metadanych w tabeli crypto_objects */
	/***************************************************/
	BMD_FOK(JS_PrepareAndExecuteSQLQuery_Insert_AdditionalMetadata(	hDB, bmdJSRequest, CryptoObjectNr, req));

	/********************************************************************/
	/* znalezienie wszytskich linkow wskazujacych na aktualizowany plik */
	/********************************************************************/
	asprintf(&SQLQuery, 	"SELECT id FROM crypto_objects WHERE file_type='link' AND pointing_id=%s AND location_id='%s';", CryptoObjectNr, _GLOBAL_bmd_configuration->location_id);
	if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}

	/********************************************************************************/
	/* aktualizacja metadanych we wszystkich plikach (linkach) wskazujacych na plik */
	/********************************************************************************/
	ExecuteSQLQueryWithAnswersKnownDBConnection( hDB, SQLQuery, &linkIdCount, &linkId);
	for (i=0; i<linkIdCount; i++)
	{
		BMD_FOK(JS_PrepareAndExecuteSQLQuery_Insert_AdditionalMetadata(	hDB, bmdJSRequest, linkId[i], req));
		free0(linkId[i]);
	}
	free0(linkId);

	/************/
	/* porzadki */
	/************/
	for (i=0; i<addMetadataCount; i++)
	{
		free0(addMetadata[i]);
	}
	free0(addMetadata);
	free0(CryptoObjectNr);
	free0(SQLQuery);
	free0(HiddenMetadataRowId);

	return BMD_OK;
}

/***********************************************/
/* ukrycie metadanych w celu ich update'owania */
/***********************************************/
long JS_PrepareAndExecuteSQLQuery_Hide_AdditionalMetadata(	void *hDB,
								char *CryptoObjectNr,
								char *location_id,
								server_request_data_t *req)
{
char *SQLQuery 		= NULL;
char *ans		= NULL;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (hDB==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (CryptoObjectNr==NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	/*************************************/
	/* okreslenie pliku jako nieobecnego */
	/*************************************/
	asprintf(&SQLQuery, 	"UPDATE crypto_objects SET present=0 WHERE id=%s%s%s;",
			 		CryptoObjectNr,
					location_id==NULL?"":" AND location_id=",
					location_id==NULL?"":location_id);

	ExecuteSQLQueryWithAnswerKnownDBConnection(hDB, SQLQuery, &ans);

	/************/
	/* porzadki */
	/************/
	free0(SQLQuery);
	free0(ans);

	return BMD_OK;
}


long JS_PrepareAndExecuteSQLQuery_Insert_AdditionalMetadataHistory(	void *hDB,
									bmdDatagram_t *bmdJSRequest,
									char *CryptoObjectNr,
									char *location_id,
									long addMetadataCount,
									char **addMetadata)
{
long retVal		= 0;
long i			= 0;
long ans_count3		= 0;
char **ans3		= NULL;
void *database_result	= NULL;
char *SQLQuery 		= NULL;
char *insertOwner	= NULL;
char *insertReason	= NULL;
char *add_metadatas	= NULL;
char *tmp_string	= NULL;
char* escapeTemp	= NULL;


	PRINT_INFO("LIBBMDSQLINF Updating additional metadata(JS)\n");
	/******************************/
	/* walidacja parametrow */
	/******************************/
	if (hDB==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (bmdJSRequest==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (CryptoObjectNr==NULL)	{	BMD_FOK(BMD_ERR_PARAM3);	}

	/*************************************/
	/* pobranie powodu i autora update'a */
	/*************************************/
	if (bmdJSRequest->protocolData!=NULL)
	{
		retVal = asprintf(&insertReason, "%s", bmdJSRequest->protocolData->buf);
	}
	else
	{
		retVal = asprintf(&insertReason, "%s", "");
	}
	if (retVal == -1)	{	BMD_FOK(NO_MEMORY);	}

	retVal = bmd_db_escape_string(hDB, insertReason, STANDARD_ESCAPING, &escapeTemp);
	free(insertReason); insertReason = NULL;
	BMD_FOK(retVal);
	insertReason = escapeTemp;
	escapeTemp = NULL;

	if (bmdJSRequest->protocolDataOwner!=NULL)
	{
		//SELECT id, name, identity, cert_serial, cert_dn, cert_dn::varchar||cert_serial::varchar, accepted FROM users;
		BMD_FOK_CHG(getElementWithCondition( _GLOBAL_UsersDictionary, 5, bmdJSRequest->protocolDataOwner->buf, 5, &insertOwner), LIBBMDSQL_USER_NOT_REGISTERED_IN_SYSTEM);
	}
	else
	{
		retVal = asprintf(&insertOwner, "%s", "");
		if(retVal == -1)
		{
			free(insertReason); insertReason = NULL;
			BMD_FOK(BMD_ERR_MEMORY);
		}
	}

	retVal = bmd_db_escape_string(hDB, insertOwner, STANDARD_ESCAPING, &escapeTemp);
	free(insertOwner); insertOwner = NULL;
	if(retVal < BMD_OK)
	{
		free(insertReason); insertReason = NULL;
		BMD_FOK(retVal);
	}
	insertOwner = escapeTemp;
	escapeTemp = NULL;
	

	/****************************************************/
	/* pobranie wszystkich oidow metadanych dodatkowych */
	/****************************************************/
	//SELECT id, type, name, sql_cast_string, default_value FROM add_metadata_types;
	BMD_FOK(getColumnWithCondition(	_GLOBAL_AddMetadataTypesDictionary, 0, NULL, 1, &ans_count3, &ans3));

	/******************************************************/
	/*	skopiowanie poprzedniego updateowanego wiersza	*/
	/******************************************************/
	asprintf(&add_metadatas, "INSERT INTO add_metadata_history (id, location_id, fk_crypto_objects, insert_date, insert_user_dn, insert_reason");
	if (add_metadatas==NULL)	{	BMD_FOK(NO_MEMORY);		}

	if (addMetadataCount>0)
	{
		for (i=0; i<ans_count3; i++)
		{
			bmd_str_replace(&(ans3[i]),".","_");
			asprintf(&tmp_string, "%s, amv_%s ", add_metadatas, ans3[i]);
			if (tmp_string==NULL)	{		BMD_FOK(NO_MEMORY);		}
			free0(add_metadatas);
			add_metadatas=tmp_string; tmp_string=NULL;
		}
	}

	asprintf(&tmp_string, 	"%s) VALUES (nextval('add_metadata_history_id_seq'), %s, %s, now(), '%s', '%s'",
				add_metadatas,
				location_id==NULL?"DEFAULT":location_id,
				CryptoObjectNr,
				insertOwner,
				insertReason);
	if (tmp_string==NULL)		{	BMD_FOK(NO_MEMORY);	}
	free0(add_metadatas);
	add_metadatas=tmp_string; tmp_string=NULL;

	for (i=0; i<addMetadataCount; i++)
	{
		if ((addMetadata==NULL) || (strlen(addMetadata[i])==0))
		{
			asprintf(&tmp_string, "%s, NULL", add_metadatas);
			if (tmp_string==NULL)	{	BMD_FOK(NO_MEMORY);		}
			free0(add_metadatas);
			add_metadatas=tmp_string; tmp_string=NULL;
		}
		else
		{
			BMD_FOK(bmd_db_escape_string(hDB, addMetadata[i], STANDARD_ESCAPING, &escapeTemp));
			asprintf(&tmp_string, "%s, '%s'", add_metadatas, escapeTemp);
			free(escapeTemp); escapeTemp=NULL;
			if (tmp_string==NULL)  {	BMD_FOK(NO_MEMORY);		}
			free0(add_metadatas);
			add_metadatas=tmp_string; tmp_string=NULL;
		}
	}

	asprintf(&tmp_string, "%s);", add_metadatas);
	if (tmp_string==NULL)	{	BMD_FOK(NO_MEMORY);	}
	free0(add_metadatas);
	add_metadatas=tmp_string; tmp_string=NULL;

	/************************************************************************/
	/* wykonanie zapytania wstawienia danych do tabeli add_metadata_history */
	/************************************************************************/
	BMD_FOK(bmd_db_execute_sql( hDB, add_metadatas, NULL, NULL, &database_result));
	bmd_db_result_free(&database_result);
	free0(SQLQuery);

// 	/*******************************************************************************************/
// 	/* w przypadku usuwania dokumentu ustawiamy jego widocznosc i obecnosc w bazie danych na 0 */
// 	/*******************************************************************************************/
// 	if (bmdJSRequest->datagramType==BMD_DATAGRAM_TYPE_DELETE)
// 	{
// 		asprintf(&SQLQuery, "	UPDATE crypto_objects SET present=0 WHERE id=%s%s%s;",
// 						CryptoObjectNr,
// 						location_id==NULL?"":" AND location_id=",
// 						location_id==NULL?"":location_id);
// 		if (SQLQuery==NULL)    {       BMD_FOK(NO_MEMORY);     }
// 		BMD_FOK(bmd_db_execute_sql(hDB, SQLQuery,NULL,NULL, &database_result));
// 		bmd_db_result_free(&database_result);
// 		free0(SQLQuery);
// 	}

	return BMD_OK;
}

/******************************************************/
/*	zakonczenie transakcji - zestaw datagramow	*/
/******************************************************/
#ifndef WIN32
long JS_bmdDatagramSet_to_CloseTransaction(	void *hDB,
								bmdDatagramSet_t *bmdJSRequestSet,
								bmdDatagramSet_t **bmdJSResponseSet,
								server_request_data_t *req)
#else /*WIN32*/
long JS_bmdDatagramSet_to_CloseTransaction(	void *hDB,
								bmdDatagramSet_t *bmdJSRequestSet,
								bmdDatagramSet_t **bmdJSResponseSet,
								server_request_data_t *req,
								HANDLE *threadHandler)
#endif /*ifndef WIN32*/
{
long status				= 0;
long i				= 0;
long j				= 0;
long id_count			= 0;
long *twl_id_count_array	= 0;
char *SQLQuery			= NULL;
char *SQLQueryId			= NULL;
char **id_list			= NULL;
// char *location_id			= NULL;
char ***twl_id_list_array	= NULL;
#ifndef WIN32
bmdDatagramSet_t *tmpDtgSet	= NULL;
#endif /*ifndef WIN32*/

	PRINT_INFO("LIBBMDSQLINF Closing files upload transaction.\n");
	/************************/
	/* walidacja parametrów */
	/************************/
	if (hDB==NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (bmdJSRequestSet==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	/*****************************************/
	/* przygotowanie datagramsetu odpowiedzi */
	/*****************************************/
	BMD_FOK(PR2_bmdDatagramSet_fill_with_datagrams(bmdJSRequestSet->bmdDatagramSetSize, bmdJSResponseSet));

	/**************************/
	/* rozpoczecie transakcji */
	/**************************/
	BMD_FOK(bmd_db_start_transaction(hDB,BMD_TRANSACTION_LEVEL_NONE));

	twl_id_list_array=malloc( bmdJSRequestSet->bmdDatagramSetSize * sizeof(char **));
	if (twl_id_list_array==NULL)		{	BMD_FOK(NO_MEMORY);	};

	twl_id_count_array=malloc( bmdJSRequestSet->bmdDatagramSetSize * sizeof(long));
	if (twl_id_count_array==NULL)		{	BMD_FOK(NO_MEMORY);	};
	/*************************************************/
	/* obsługa poszczegolnych datagramow z zadaniami */
	/*************************************************/
	for(i=0; i<bmdJSRequestSet->bmdDatagramSetSize; i++)
	{
		/*************************************/
		/* obsluzenie pojedynczego datagramu */
		/*************************************/
		status=JS_bmdDatagram_to_CloseTransaction(	hDB,
										bmdJSRequestSet->bmdDatagramSetTable[i],
										&id_list,
										&id_count,
										req);
		BMD_FOK_NO_RETURN(status);
		twl_id_list_array[i]=id_list;
		twl_id_count_array[i]=id_count;

		id_list=NULL;
		id_count=0;
		/**************************************/
		/* przygotowanie datagramu odpowiedzi */
		/**************************************/
		(*bmdJSResponseSet)->bmdDatagramSetTable[i]->datagramStatus=status;
		(*bmdJSResponseSet)->bmdDatagramSetTable[i]->datagramType=bmdJSRequestSet->bmdDatagramSetTable[i]->datagramType+100;
	}

	/******************************/
	/*	zakonczenie transakcji	*/
	/******************************/
	bmd_db_end_transaction(hDB);

	for(i=0; i<bmdJSRequestSet->bmdDatagramSetSize; i++)
	{
		if (twl_id_count_array[i] > 0)
		{
/*pod Linux oddzielny tymczasowy datagramset jest wypelniany metadanymi, ktore sluza do przygotowania awiza*/
#ifndef WIN32
			/*********************************************************/
			/* przygotowanie datagramu tymczasowego do wysylki awizo */
			/*********************************************************/
			BMD_FOK(PR2_bmdDatagramSet_fill_with_datagrams(twl_id_count_array[i], &tmpDtgSet));
			for (j=0; j < twl_id_count_array[i]; j++)
			{
// 				/************************************************************/
// 				/*	pobranie metadanych pliku na podstawienie id pliku	*/
// 				/************************************************************/
// 				BMD_FOK(FillFromAllMetadata(	hDB,
// 									twl_id_list_array[i][j],
// 									_GLOBAL_bmd_configuration->location_id==NULL?"0":_GLOBAL_bmd_configuration->location_id,
// 									WITHOUT_PKI_METADATA,
// 									WITH_ADD_METADATA,
// 									WITH_SYS_METADATA,
// 									WITHOUT_ACTION_METADATA,
// 									tmpDtgSet->bmdDatagramSetTable[j],
// 									req));


				/******************************************************************************************/
				/*	ustawienie wlasciciela datagramu odpowiedzi na wlasciciela datagramu wejsciowego  */
				/******************************************************************************************/
				if (bmdJSRequestSet->bmdDatagramSetTable[0]->protocolDataOwner!=NULL)
				{
					if (bmdJSRequestSet->bmdDatagramSetTable[0]->protocolDataOwner->buf!=NULL)
					{
						tmpDtgSet->bmdDatagramSetTable[j]->protocolDataOwner=
						(GenBuf_t *)malloc(sizeof(GenBuf_t));

						tmpDtgSet->bmdDatagramSetTable[j]->protocolDataOwner->buf=(char*)
						malloc(bmdJSRequestSet->bmdDatagramSetTable[0]->protocolDataOwner->size+2);

						memset(tmpDtgSet->bmdDatagramSetTable[j]->protocolDataOwner->buf, 0,
						bmdJSRequestSet->bmdDatagramSetTable[0]->protocolDataOwner->size+1);

						memcpy(tmpDtgSet->bmdDatagramSetTable[j]->protocolDataOwner->buf,
						bmdJSRequestSet->bmdDatagramSetTable[0]->protocolDataOwner->buf,
						bmdJSRequestSet->bmdDatagramSetTable[0]->protocolDataOwner->size);

						tmpDtgSet->bmdDatagramSetTable[j]->protocolDataOwner->size=
						bmdJSRequestSet->bmdDatagramSetTable[0]->protocolDataOwner->size;
					}
				}


			}
/*
pod Windows metadane potrzebne do przygotowania awiza wstawione sa do datagramsetu odpowiedzi...
nie ma tworzonego dodatkowego datagramsetu i dzieki temu mozna zsynchronizowac niszczenie pamieci
*/
#else /*WIN32*/
			for (j=0; j < twl_id_count_array[i]; j++)
			{
// 				/************************************************************/
// 				/*	pobranie metadanych pliku na podstawienie id pliku	*/
// 				/************************************************************/
// 				BMD_FOK(FillFromAllMetadata(	hDB,
// 								twl_id_list_array[i][j],
// 								_GLOBAL_bmd_configuration->location_id==NULL?"0":_GLOBAL_bmd_configuration->location_id,
// 								WITHOUT_PKI_METADATA,
// 								WITH_ADD_METADATA,
// 								WITH_SYS_METADATA,
// 								WITHOUT_ACTION_METADATA,
// 								(*bmdJSResponseSet)->bmdDatagramSetTable[j],
// 								req));


				/******************************************************************************************/
				/*	ustawienie wlasciciela datagramu odpowiedzi na wlasciciela datagramu wejsciowego  */
				/******************************************************************************************/
				if (bmdJSRequestSet->bmdDatagramSetTable[0]->protocolDataOwner!=NULL)
				{
					if (bmdJSRequestSet->bmdDatagramSetTable[0]->protocolDataOwner->buf!=NULL)
					{
						(*bmdJSResponseSet)->bmdDatagramSetTable[j]->protocolDataOwner=
						(GenBuf_t *)malloc(sizeof(GenBuf_t));

						(*bmdJSResponseSet)->bmdDatagramSetTable[j]->protocolDataOwner->buf=(char*)
						malloc(bmdJSRequestSet->bmdDatagramSetTable[0]->protocolDataOwner->size+2);

						memset((*bmdJSResponseSet)->bmdDatagramSetTable[j]->protocolDataOwner->buf, 0,
						bmdJSRequestSet->bmdDatagramSetTable[0]->protocolDataOwner->size+1);

						memcpy((*bmdJSResponseSet)->bmdDatagramSetTable[j]->protocolDataOwner->buf,
						bmdJSRequestSet->bmdDatagramSetTable[0]->protocolDataOwner->buf,
						bmdJSRequestSet->bmdDatagramSetTable[0]->protocolDataOwner->size);

						(*bmdJSResponseSet)->bmdDatagramSetTable[j]->protocolDataOwner->size=
						bmdJSRequestSet->bmdDatagramSetTable[0]->protocolDataOwner->size;
					}
				}
			}
#endif /*ifndef WIN32*/

			/*****************/
			/* wysylka awizo */
			/*****************/
			if (_GLOBAL_bmd_configuration->session_plugin_conf.bmd_plugin_action_post)
			{
#ifndef WIN32
				_GLOBAL_bmd_configuration->session_plugin_conf.bmd_plugin_action_post(	tmpDtgSet,
													_GLOBAL_bmd_configuration->db_conninfo,
													1,0,0, req);
#else /*WIN32*/
				_GLOBAL_bmd_configuration->session_plugin_conf.bmd_plugin_action_post(	*bmdJSResponseSet,
													_GLOBAL_bmd_configuration->db_conninfo,
													1,0,0, req, threadHandler);
#endif /*ifndef WIN32*/
// 				}
			}
		}
	}


	/******************/
	/*	porzadki	*/
	/******************/

#ifndef WIN32

	PR2_bmdDatagramSet_free(&tmpDtgSet); //nie wiem, czy to jest potrzebne - w patchu nie bylo (WSZ)
#endif /*ifndef WIN32*/

	for(i=0; i < bmdJSRequestSet->bmdDatagramSetSize; i++)
	{
		for (j=0; j < twl_id_count_array[i]; j++)
		{
			free(twl_id_list_array[i][j]); twl_id_list_array[i][j]=NULL;
		}
		free(twl_id_list_array[i]); twl_id_list_array[i]=NULL;
	}
	free0(twl_id_list_array);
	free0(twl_id_count_array);

	free0(SQLQuery);
	free0(SQLQueryId);
// 	free0(location_id);

	return BMD_OK;
}

/************************************************/
/*	zakonczenie transakcji - jeden datagram	*/
/************************************************/
long JS_bmdDatagram_to_CloseTransaction(	void *hDB,
							bmdDatagram_t *bmdJSRequest,
							char ***id_list,
							long *id_count,
					server_request_data_t *req)
{
long i					= 0;
char *SQLQuery				= NULL;
char *ans				= NULL;
char **anss				= NULL;
long ans_count				= 0;
char *transactionId			= NULL;
char *tmp				= NULL;
long status				= 0;
char* escapeTemp		= NULL;
void *queryResult			= NULL;

	PRINT_INFO("LIBBMDSQLINF Closing single file upload transaction.\n");
	/**********************************************************************/
	/* sprawdzenie, czy w datagramie znajduje sie informacja o transakcji */
	/**********************************************************************/
	for(i=0; i<bmdJSRequest->no_of_sysMetaData; i++)
	{
		if (strcmp(bmdJSRequest->sysMetaData[i]->OIDTableBuf, OID_SYS_METADATA_CRYPTO_OBJECTS_TRANSACTION_ID)==0)
		{
			transactionId=malloc(bmdJSRequest->sysMetaData[i]->AnySize + 1);
			if (transactionId==NULL)	{	BMD_FOK(NO_MEMORY);	}
			memset(transactionId, 0, bmdJSRequest->sysMetaData[i]->AnySize + 1);
			memcpy(transactionId, bmdJSRequest->sysMetaData[i]->AnyBuf,bmdJSRequest->sysMetaData[i]->AnySize);
			break;
		}
	}

	if (transactionId==NULL)
	{
		BMD_FOK(BMD_SQL_CLOSE_TRANSACTION_ID_NOT_FOUND_IN_DTG_ERROR);
	}

	PRINT_VDEBUG("LIBBMDSQLVDEBUG Transaction id: %s\n", transactionId);

	escapeTemp=transactionId; transactionId=NULL;
	BMD_FOK(bmd_db_escape_string(hDB, escapeTemp, STANDARD_ESCAPING, &transactionId));
	free(escapeTemp); escapeTemp=NULL;
	
	/**********************************************/
	/* wyszukanie plikow do zamkniecia transakcji */
	/**********************************************/
	asprintf(&SQLQuery, 	"SELECT id FROM crypto_objects WHERE transaction_id='%s' AND (file_type = 'dir' OR file_type = 'link' OR file_type = '')%s%s;",
					transactionId,
					_GLOBAL_bmd_configuration->location_id==NULL?"":" AND location_id=",
					_GLOBAL_bmd_configuration->location_id==NULL?"":_GLOBAL_bmd_configuration->location_id);
	if (SQLQuery==NULL)		{	BMD_FOK(NO_MEMORY);	}

	status=ExecuteSQLQueryWithAnswersKnownDBConnection(hDB, SQLQuery, id_count, id_list);
	free(SQLQuery); SQLQuery=NULL;
	if (status != BMD_ERR_NODATA)	{	BMD_FOK(status);	}
	/**********************************************************/
	/* wyszukanie plikow dodatkowych do zamkniecia transakcji */
	/**********************************************************/
	asprintf(&SQLQuery, 	"SELECT id FROM crypto_objects WHERE transaction_id='%s' AND (file_type != 'dir' AND file_type != '')%s%s;",
					transactionId,
					_GLOBAL_bmd_configuration->location_id==NULL?"":" AND location_id=",
					_GLOBAL_bmd_configuration->location_id==NULL?"":_GLOBAL_bmd_configuration->location_id);
	if (SQLQuery==NULL)		{	BMD_FOK(NO_MEMORY);	}
	status=ExecuteSQLQueryWithAnswersKnownDBConnection(hDB, SQLQuery, &ans_count, &anss);
	free(SQLQuery); SQLQuery=NULL;
	if (status != BMD_ERR_NODATA)	{	BMD_FOK(status);	}

	if (((*id_count)<=0) && (ans_count<=0))
	{
		BMD_FOK(BMD_SQL_CLOSE_TRANSACTION_FILE_NOT_FOUND_ERROR);
	}

	PRINT_VDEBUG("Transaction id: %s\n", transactionId);
	/*********************************************************************************/
	/* zamkniecie transakcji dla dokumentow wysłanych w transakcji i nie bedacych dodatkowymi */
	/*********************************************************************************/
	if ((*id_count)>0)
	{
		// przywracanie widocznosci plikom, ktore nie czekaja na oznakowanie czasem
		status = asprintf(&SQLQuery, 	"UPDATE crypto_objects SET visible=1, transaction_id='' WHERE transaction_id='%s' AND (file_type = 'dir' OR file_type = 'link' OR file_type = '') AND for_timestamping=0 %s%s;",
						transactionId,
						_GLOBAL_bmd_configuration->location_id==NULL?"":" AND location_id=",
						_GLOBAL_bmd_configuration->location_id==NULL?"":_GLOBAL_bmd_configuration->location_id);
		if(status == -1)
			{ BMD_FOK(NO_MEMORY); }

		status = bmd_db_execute_sql(hDB, SQLQuery, NULL, NULL, &queryResult );
		free(SQLQuery); SQLQuery=NULL;
		bmd_db_result_free(&queryResult);
		BMD_FOK(status);
		
		// nie modyfikujemy widocznosci plikow, ktore oczekuja na oznakowanie czasem
		status = asprintf(&SQLQuery, 	"UPDATE crypto_objects SET transaction_id='' WHERE transaction_id='%s' AND (file_type = 'dir' OR file_type = 'link' OR file_type = '') AND for_timestamping=1 %s%s;",
						transactionId,
						_GLOBAL_bmd_configuration->location_id==NULL?"":" AND location_id=",
						_GLOBAL_bmd_configuration->location_id==NULL?"":_GLOBAL_bmd_configuration->location_id);
		if(status == -1)
			{ BMD_FOK(NO_MEMORY); }

		status = bmd_db_execute_sql(hDB, SQLQuery, NULL, NULL, &queryResult );
		free(SQLQuery); SQLQuery=NULL;
		bmd_db_result_free(&queryResult);
		BMD_FOK(status);
	}

	/*****************************************************************************/
	/* zamkniecie transakcji dla dokumentow wysłanych w transakcji i dodatkowych */
	/*****************************************************************************/
	if (ans_count>0)
	{
		status = asprintf(&SQLQuery, 	"UPDATE crypto_objects SET transaction_id='' WHERE transaction_id='%s' AND (file_type != 'dir' AND file_type != '')%s%s;",
						transactionId,
						_GLOBAL_bmd_configuration->location_id==NULL?"":" AND location_id=",
						_GLOBAL_bmd_configuration->location_id==NULL?"":_GLOBAL_bmd_configuration->location_id);
		if(status == -1)
			{ BMD_FOK(NO_MEMORY); }

		status = bmd_db_execute_sql(hDB, SQLQuery, NULL, NULL, &queryResult );
		free(SQLQuery); SQLQuery=NULL;
		bmd_db_result_free(&queryResult);
		BMD_FOK(status);
	}

	PRINT_VDEBUG("Main files found with given transaction id: %s count: %li\n", transactionId, (*id_count));
	PRINT_VDEBUG("Additional files found with given transaction id: %s count: %li\n", transactionId, ans_count);

	/************/
	/* porzadki */
	/************/
	for (i=0; i<ans_count; i++)
	{
		free(anss[i]); anss[i]=NULL;
	}
	free(anss); anss=NULL;
	free(tmp); tmp=NULL;
	free(transactionId); transactionId=NULL;
	free(ans); ans=NULL;
	free(SQLQuery); SQLQuery=NULL;

	return BMD_OK;
}


// long JS_PrepareAndExecuteSQLQuery_Update_Time(   void *hDB,
// 								char *twf_CryptoObjectNr,
//
// 								char *twf_insert_user_dn,
// 								char *twf_insert_reason,
//
// 								char *twf_metadataOID,
// 								char *twf_first_time_metadataOID,
// 								char *twf_first_time_user_dn,
//
// 								char *twf_no_updated_string,
// 								char *twf_after_update_string,
//
// 								long twf_enable_time_history,
// 								long twf_enable_first_time_history,
//
// 								long twf_enable_time,
// 								long twf_force_first_time,
// 								long twf_equal_first_check,
// 					server_request_data_t *req)
// {
// char *SQLQuery				= NULL;
// char **ans					= NULL;
// char **ans_add_metadata_name		= NULL;
// char **ans_add_metadata_value		= NULL;
// char *current_date			= NULL;
// char *sent_string				= NULL;
// char *add_metadatas			= NULL;
// char *SeqNextValAdditionalMetadata	= NULL;
// char *insert_reason			= NULL;
// char *insert_user_dn			= NULL;
// char *metadataOID_COL			= NULL;
// char *first_time_metadataOID_COL	= NULL;
// char *no_updated_string			= NULL;
// char *after_update_string		= NULL;
// char *first_time_user_dn		= NULL;
// char *ans_temp				= NULL;
//
// long status					= 0;
// long ans_count				= 0;
// long ans_add_metadata_name_count	= 0;
// long ans_add_metadata_value_count	= 0;
// long i					= 0;
// long einvoice_fetched_metadata_number	= 0;
// long first_time_flag			= 0;
// long char_count_1				= 0;
// long char_count_2				= 0;
// long enable_first_get			= 0;
//
// struct tm czas;
//
// 	PRINT_INFO("LIBBMDSQLINF Updating metadata time.\n");
// 	/********************************/
// 	/*      walidacja parametrów    */
// 	/********************************/
// 	if (hDB==NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
// 	if (twf_CryptoObjectNr==NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
//
// 	/*Certyfikat wykonujacy update*/
// 	if (twf_insert_user_dn==NULL)	{	BMD_FOK(BMD_ERR_PARAM3);	}
// 	else
// 	{
// 		if (strlen(twf_insert_user_dn) > 0)
// 		{
// 			insert_user_dn=twf_insert_user_dn;
// 		}
// 		else		{	 BMD_FOK(BMD_ERR_PARAM3);	}
// 	}
//
// 	/*certyfikat ktory tylko ma prawo do pierwszego pobierania*/
// 	if (twf_first_time_user_dn==NULL)
// 	{
// 		first_time_user_dn=NULL;
// 	}
// 	else
// 	{
// 		if (strlen(twf_first_time_user_dn) > 0)
// 		{
// 			first_time_user_dn=twf_first_time_user_dn;
// 		}
// 		else
// 		{
// 			first_time_user_dn=NULL;
// 		}
// 	}
//
// 	/*Powod wykonania update*/
// 	if (twf_insert_reason==NULL)
// 	{
// 		/*Nadaje pusty powod pobrania pliku.*/
// 		insert_reason=" ";
// 	}
// 	else
// 	{
// 		if (strlen(twf_insert_reason) > 0)
// 		{
// 			insert_reason=twf_insert_reason;
// 		}
// 		else
// 		{
// 			insert_reason=" ";
// 		}
// 	}
//
// 	/*Oid metadanej na ktorej wykonuje sie update*/
// 	if (twf_metadataOID==NULL)		{	BMD_FOK(BMD_ERR_PARAM5);	}
// 	else
// 	{
// 		asprintf(&metadataOID_COL, "%s", twf_metadataOID);
// 		if (metadataOID_COL==NULL)	{	 BMD_FOK(NO_MEMORY);	}
// 		bmd_str_replace(&metadataOID_COL,".","_");
// 	}
//
// 	/*Oid metadanej przechowujacej pierwszy czas.*/
// 	if (twf_first_time_metadataOID != NULL)
// 	{
// 		asprintf(&first_time_metadataOID_COL, "%s", twf_first_time_metadataOID);
// 		if (first_time_metadataOID_COL==NULL)		{	BMD_FOK(NO_MEMORY);	}
// 		bmd_str_replace(&first_time_metadataOID_COL,".","_");
// 	}
//
// 	/*String oznaczajacy ze metadana nie byla dotad updatowana czasem*/
// 	if (twf_no_updated_string==NULL)
// 	{
// 		no_updated_string="Nie";
// 	}
// 	else
// 	{
// 		if (strlen(twf_no_updated_string) > 0)
// 		{
// 			no_updated_string=twf_no_updated_string;
// 		}
// 		else
// 		{
// 			no_updated_string="Nie";
// 		}
// 	}
//
// 	/*String dodawany do wstawianego czasu*/
// 	if (twf_after_update_string==NULL)
// 	{
// 		after_update_string="Tak";
// 	}
// 	else
// 	{	if (strlen(twf_after_update_string) > 0)
// 		{
// 			after_update_string=twf_after_update_string;
// 		}
// 		else
// 		{
// 			after_update_string="Tak";
// 		}
// 	}
//
// 	/************************************************/
// 	/*      pobranie aktualnej daty i godziny	*/
// 	/************************************************/
// 	if (GetActualTime(&czas) != 0)
// 	{
// 		PRINT_ERROR("LIBBMDSQLERR Invalid localtime operation. Error=%i\n",FUNC_ERR);
// 		return FUNC_ERR;
// 	}
// 	asprintf(&current_date, "%02i-%02i-%02i %02i:%02i:%02i",czas.tm_year+1900,czas.tm_mon+1,czas.tm_mday, czas.tm_hour,czas.tm_min,czas.tm_sec);
// 	if (current_date==NULL)	{	BMD_FOK(NO_MEMORY);	}
//
// 	/*Sprawdzam czy trzeba dodac czas*/
// 	if (twf_enable_time==1)
// 	{
// 		asprintf(&sent_string,"%s (%s)",after_update_string, current_date);
// 		if (sent_string==NULL)	{	BMD_FOK(NO_MEMORY);	}
// 	}
// 	else
// 	{
// 		asprintf(&sent_string,"%s",after_update_string);
// 		if (sent_string==NULL)	{	BMD_FOK(NO_MEMORY);	}
// 	}
//
//
// 	/* <zalozenie lock na wiersz w tabeli crypto_objects> */
// 	/* aby procesy rownolegle modyfikujace metadane pozostawialy tylko jedna wartosc metadanej z wlaczona widocznoscia (by WSZ)*/
// 	/* lock jest automatycznie zdejmowany przy zakonczeniu transakcji lub rollbacku */
// 	BMD_FOK(LockCryptoObjectsRow(hDB, twf_CryptoObjectNr));
// 	/* </zalozenie lock na wiersz w tabeli crypto_objects> */
//
//
// 	/************************************************************************/
// 	/*      sprawdzamy, czy dokument byl juz pobierany/mail wysylany	*/
// 	/************************************************************************/
// 	asprintf(&SQLQuery, "SELECT amv_%s, id FROM crypto_objects WHERE id=%s AND present=1;",
// 	metadataOID_COL, twf_CryptoObjectNr);
// 	if (SQLQuery==NULL)
// 	{
// 		BMD_FOK(NO_MEMORY);
// 	}
// 	BMD_FOK(ExecuteSQLQueryWithRowAnswersKnownDBConnection( hDB, SQLQuery, &ans_count, &ans));
//
// 	free(SQLQuery);
// 	SQLQuery=NULL;
//
//
// 	/*Ustalam wartosc flagi decydujacej czy jest to pierwsze pobranie pliku*/
// 	if (ans[0]==NULL || twf_force_first_time==1)
// 	{/* if (ans[0]==NULL || twf_force_first_time==1)*/
// 		first_time_flag=1;
// 	}/*\ if (ans[0]==NULL || twf_force_first_time==1)*/
// 	else
// 	{/*else(ans[0]==NULL || twf_force_first_time==1)*/
// 		char_count_1=(int)strlen(ans[0]);
// 		char_count_2=(int)strlen(no_updated_string);
//
// 		if ( (strncmp(ans[0], no_updated_string, char_count_2)==0) || char_count_1==0 )
// 		{
// 			if ( (twf_equal_first_check==1 && ( char_count_1==char_count_2)) || char_count_1==0 )
// 			{
// 				first_time_flag=1;
// 			}
// 			else
// 			{
// 				first_time_flag=0;
// 			}
//
// 			/*Na potrzeby zapisow typu Nie (data)*/
// 			if (twf_equal_first_check==0)
// 			{
// 				first_time_flag=1;
// 			}
// 		}
// 		else
// 		{
// 			first_time_flag=0;
// 		}
// 	}/*\ else(ans[0]==NULL || twf_force_first_time==1)*/
//
// 	/****************************************************************************************/
// 	/*      updatowanie widocznosci wiersza metadanych, stanie sie wierszem historycznym	*/
// 	/****************************************************************************************/
// 	if ( (twf_enable_time_history==1 && twf_enable_first_time_history==1) ||
// 	(twf_enable_time_history==1 && first_time_flag==0) )
// 	{
// 		/*Zachowuje informacje o wierszu z informacja, ze nie pobrano faktury.*/
// 		asprintf(&SQLQuery, "UPDATE crypto_objects SET visible=0 WHERE id='%s' ;", ans[1]);
// 		if (SQLQuery==NULL)		{	BMD_FOK(NO_MEMORY);	}
//
// 	}
// 	else
// 	{
// 		/*Nie zachowuje wiersza z informacja ze nie pobrano faktury.*/
// 		asprintf(&SQLQuery, "UPDATE add_metadata SET amv_%s='%s' WHERE id=%s AND visible=1 ;",
// 		metadataOID_COL, sent_string, ans[1]);
// 		if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}
// 	}
//
// 	status=ExecuteSQLQueryWithAnswerKnownDBConnection(hDB, SQLQuery, &ans_temp);
// 	free(SQLQuery);
// 	SQLQuery=NULL;
// 	free(ans_temp);
// 	ans_temp=NULL;
//
// 	if (status!=BMD_ERR_NODATA && status!=BMD_ERR_NODATA)
// 	{
// 		BMD_FOK(status);
// 	}
//
// 	if ( twf_enable_time_history==1 && (twf_enable_first_time_history==1 || first_time_flag==0) )
// 	{/* if (twf_enable_first_time_history==1 || first_time_flag==0)*/
// 		/**************************************************************************************************/
// 		/*W tym momencie dodaje nowy wiersz z informacja o pobraniu pliku*/
// 		/**************************************************************************************************/
//
// 		/********************************************************/
// 		/*      pobranie id kolejnej metadanej z sekwencji      */
// 		/********************************************************/
// 		SeqNextValAdditionalMetadata=bmd_db_get_sequence_nextval(hDB, "add_metadata_id_seq");
// 		if (SeqNextValAdditionalMetadata==NULL)	{	BMD_FOK(NO_MEMORY);	}
//
// 		/****************************************************************/
// 		/*      pobranie wszystkich oidow metadanych dodatkowych	*/
// 		/****************************************************************/
// 		//SELECT id, type, name, sql_cast_string FROM add_metadata_types;;
// 		BMD_FOK(getColumnWithCondition(	_GLOBAL_AddMetadataTypesDictionary, 0, NULL, 1, &ans_add_metadata_name_count, &ans_add_metadata_name));
//
// 		/************************************************/
// 		/*	pobranie wartosci metadanych z wiersza	*/
// 		/************************************************/
// 		asprintf(&SQLQuery, "SELECT id, fk_crypto_objects, insert_date, insert_user_dn, visible,insert_reason");
// 		if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}
//
// 		for (i=0; i<ans_add_metadata_name_count; i++)
// 		{
// 				bmd_str_replace(&(ans_add_metadata_name[i]),".","_");
// 				asprintf(&add_metadatas, "%s, amv_%s ", SQLQuery, ans_add_metadata_name[i]);
// 				if (add_metadatas==NULL)	{	BMD_FOK(NO_MEMORY);	}
//
// 				if (	strncmp( ans_add_metadata_name[i], metadataOID_COL, strlen(metadataOID_COL) )==0 &&
// 					strlen(ans_add_metadata_name[i])==strlen(metadataOID_COL))
// 				{
// 					einvoice_fetched_metadata_number=i + 6;
// 				}
// 				/*Podmiana buforow*/
// 				free(SQLQuery);
// 				SQLQuery=add_metadatas;
// 				add_metadatas=NULL;
//
// 				/*free(ans_add_metadata_name[i]);
// 				ans_add_metadata_name[i]=NULL;*/
//
// 		}/* for (i=0; i<ans_add_metadata_count; i++)*/
//
// 		asprintf(&add_metadatas, "%s FROM add_metadata WHERE id=%s;", SQLQuery, ans[1]);
//
// 		free(ans[0]);
// 		ans[0]=NULL;
// 		free(ans[1]);
// 		ans[1]=NULL;
// 		free(ans);
// 		ans=NULL;
// 		if (add_metadatas==NULL)	{    	BMD_FOK(NO_MEMORY);	}
//
// 		/*Podmiana buforow*/
// 		free(SQLQuery);
// 		SQLQuery=add_metadatas;
// 		add_metadatas=NULL;
//
// 		BMD_FOK(ExecuteSQLQueryWithRowAnswersKnownDBConnection(hDB, SQLQuery, &ans_add_metadata_value_count, &ans_add_metadata_value));
//         	free(SQLQuery);	SQLQuery=NULL;
//
// 		/************************************************************************************************/
// 		/*Posiadam juz wszystkie potrzebne wartosci. Moge je wstawic wraz z data pobrania nowego pliku	*/
// 		/*jako metadane historyczne									*/
// 		/************************************************************************************************/
// 	        /********************************************************************************/
// 		/*      skopiowanie poprzedniego wiersza - bedzie on wierszem historycznym	*/
// 		/********************************************************************************/
// 		asprintf(&SQLQuery, "INSERT INTO add_metadata (id, fk_crypto_objects, insert_date,insert_user_dn, visible, "
// 		"insert_reason");
// 		if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}
//
// 		for (i=0; i< ans_add_metadata_name_count; i++)
// 		{
// 			asprintf(&add_metadatas, "%s, amv_%s ", SQLQuery,ans_add_metadata_name[i]);
// 			if (add_metadatas==NULL)	{	BMD_FOK(NO_MEMORY);	}
//
// 			/*Podmiana buforow*/
// 			free(SQLQuery);
// 			SQLQuery=add_metadatas;
// 			add_metadatas=NULL;
//
// 			free(ans_add_metadata_name[i]);
// 			ans_add_metadata_name[i]=NULL;
// 		}
//
// // 		bmd_db_escape_string(hDB,insert_reason, &insert_reason_escaped);
// // 		bmd_db_escape_string(hDB,insert_user_dn, &insert_user_dn_escaped);
//
// 		asprintf(&add_metadatas, "%s) VALUES (%s, %s, '%s', '%s', %s, '%s'",
// 						SQLQuery,
// 						SeqNextValAdditionalMetadata,
// 						twf_CryptoObjectNr,
// 						current_date,
// 						insert_user_dn,
// 						"1",
// 						insert_reason);
//
// 		if (add_metadatas==NULL)	{	BMD_FOK(NO_MEMORY);	}
//
// 		free(SQLQuery);
// 		SQLQuery=add_metadatas;
// 		add_metadatas=NULL;
//
// 		for (i=6; i<ans_add_metadata_value_count; i++)
// 		{/*for (i=6; i<ans_add_metadata_count; i++)*/
//
// 			/*Sprawdzam czy nie mam do czynienie z metadana czasu pobrania pliku.*/
// 			if ( einvoice_fetched_metadata_number != i )
// 			{
//                 		if (strlen(ans_add_metadata_value[i])==0)
// 				{
// 					asprintf(&add_metadatas, "%s, NULL", SQLQuery);
// 				}
// 				else
// 				{
// // 					BMD_FOK(bmd_db_escape_string(hDB, ans_add_metadata_value[i], &ans_escaped));
// 					asprintf(&add_metadatas, "%s, '%s'", SQLQuery, ans_add_metadata_value[i]);
// // 					free(ans_escaped);
// // 					ans_escaped=NULL;
// 				}
// 			}
// 			else
// 			{
//
// 				asprintf(&add_metadatas, "%s, '%s'", SQLQuery, sent_string);
// 			}
//
// 			if (add_metadatas==NULL)
// 			{
// 				BMD_FOK(NO_MEMORY);
// 			}
//
// 			free(SQLQuery);
// 			SQLQuery=add_metadatas;
// 			add_metadatas=NULL;
//
// 			free(ans_add_metadata_value[i]);
// 			ans_add_metadata_value[i]=NULL;
// 		}/*\for (i=6; i<ans_add_metadata_count; i++)*/
//
// 		asprintf(&add_metadatas, "%s);", SQLQuery);
// 		free(SQLQuery);
// 		SQLQuery=add_metadatas;
// 		add_metadatas=NULL;
//
// 		if (SQLQuery==NULL)
// 		{
// 			BMD_FOK(NO_MEMORY);
// 		}
//
// 		/****************************************************************/
// 		/*      Wykonanie inserta                               */
// 		/****************************************************************/
// 		status=ExecuteSQLQueryWithAnswerKnownDBConnection(hDB,SQLQuery, &ans_temp);
// 		free(SQLQuery);
// 		SQLQuery=NULL;
// 		free(ans_temp);
// 		ans_temp=NULL;
// 		if (status!=BMD_ERR_NODATA && status!=BMD_ERR_NODATA)
// 		{
// 			BMD_FOK(status);
// 		}
//
// 	/*\ if (twf_enable_first_time_history==1 || first_time_flag==0)*/
// 	}
//
// 	if (first_time_metadataOID_COL != NULL)
// 	{/* if (first_time_metadataOID_COL != NULL)*/
// 		/*****************************************************************************/
// 		/*      sprawdzamy, czy data pierwszego pobrania/mail zostala juz odnotowana */
// 		/*****************************************************************************/
// 		if (first_time_user_dn != NULL)
// 		{
// 			PRINT_DEBUG("Certificate driven first time update.\n");
// 			PRINT_VDEBUG("insert_user_dn: %s\n", insert_user_dn);
// 			PRINT_VDEBUG("first_time_user_dn: %s\n", first_time_user_dn);
//
// 			if ((strcmp(first_time_user_dn, insert_user_dn)==0) &&
// 			(strlen(first_time_user_dn)==strlen(insert_user_dn)) )
// 			{
// 				enable_first_get=1;
// 			}
// 			else
// 			{
// 				enable_first_get=0;
// 			}
// 		}
// 		else
// 		{
// 			PRINT_DEBUG("No certificate driven first time update.\n");
// 			enable_first_get=1;
// 		}
// 	}/*\ if (first_time_metadataOID_COL != NULL)*/
// 	else
// 	{
// 		PRINT_DEBUG("No first metadata oid given.\n");
// 		enable_first_get=0;
// 	}
//
// 	if (ans != NULL)
// 	{
// 		free(ans[0]);
// 		ans[0]=NULL;
// 		free(ans[1]);
// 		ans[1]=NULL;
// 		free(ans);
// 		ans=NULL;
// 	}
//
// 	if (enable_first_get==1)
// 	{/*if (first_flag==1)*/
// 		PRINT_DEBUG("First time of update executed.\n");
//
// 		asprintf(&SQLQuery, "SELECT amv_%s, id FROM add_metadata WHERE fk_crypto_objects=%s AND visible=1;",
// 		first_time_metadataOID_COL, twf_CryptoObjectNr);
// 		if (SQLQuery==NULL)
// 		{
// 			BMD_FOK(NO_MEMORY);
// 		}
// 		BMD_FOK(ExecuteSQLQueryWithRowAnswersKnownDBConnection( hDB, SQLQuery, &ans_count, &ans));
//
// 		free(SQLQuery);
// 		SQLQuery=NULL;
//
// 		first_time_flag=0;
// 		/*Ustalam wartosc flagi decydujacej czy jest to pierwsze pobranie pliku*/
// 		if (ans[0]==NULL || twf_force_first_time==1)
// 		{/* if (ans[0]==NULL || twf_force_first_time==1)*/
// 			first_time_flag=1;
// 		}/*\ if (ans[0]==NULL || twf_force_first_time==1)*/
// 		else
// 		{/*else(ans[0]==NULL || twf_force_first_time==1)*/
// 			char_count_1=(int)strlen(ans[0]);
// 			char_count_2=(int)strlen(no_updated_string);
//
// 			if ( (strncmp(ans[0], no_updated_string, char_count_2)==0) || char_count_1==0 )
// 			{
// 				if ( (twf_equal_first_check==1 && ( char_count_1==char_count_2)) || char_count_1==0 )
// 				{
// 					first_time_flag=1;
// 				}
// 				else
// 				{
// 					first_time_flag=0;
// 				}
//
// 				/*Na potrzeby zapisow typu Nie (data)*/
// 				if (twf_equal_first_check==0)
// 				{
// 					first_time_flag=1;
// 				}
// 			}
// 			else
// 			{
// 				first_time_flag=0;
// 			}
// 		}/*\ else(ans[0]==NULL || twf_force_first_time==1)*/
//
// 		/*Nalezy odnotowac pierwszy czas*/
// 		if ( first_time_flag==1)
// 		{
// 			asprintf(&SQLQuery, "UPDATE add_metadata SET amv_%s='%s' WHERE id='%s' ;",
// 			first_time_metadataOID_COL, sent_string, ans[1]);
// 			if (SQLQuery==NULL)
// 			{
// 				BMD_FOK(NO_MEMORY);
// 			}
// 			status=ExecuteSQLQueryWithAnswerKnownDBConnection(hDB, SQLQuery, &ans_temp);
// 			free(SQLQuery);
// 			SQLQuery=NULL;
// 			free(ans_temp);
// 			ans_temp=NULL;
//
// 			if (status!=BMD_ERR_NODATA && status!=BMD_ERR_NODATA)
// 			{
// 				BMD_FOK(status);
// 			}
// 			free(SQLQuery);
// 			SQLQuery=NULL;
// 		}
//
// 	}/*\ if (first_flag==1)*/
// 	else
// 	{
// 		PRINT_DEBUG("First time of update not executed.\n");
// 	}
//
// 	/************/
// 	/* porzadki */
// 	/************/
// 	free0(SQLQuery);
//  	free0(ans_add_metadata_name);
//  	free0(ans_add_metadata_value);
// 	free0(current_date);
// 	free0(sent_string);
// 	free0(add_metadatas);
// 	free0(SeqNextValAdditionalMetadata);
// 	free0(insert_reason);
// 	free0(insert_user_dn);
// 	free0(metadataOID_COL);
// 	free0(first_time_metadataOID_COL);
// 	free0(no_updated_string);
// 	free0(after_update_string);
// 	free0(first_time_user_dn);
// 	free0(ans_temp);
//
// 	if (ans != NULL)
// 	{
// 		free0(ans[0]);
// 		free0(ans[1]);
// 		free0(ans);
// 	}
//
// 	return BMD_OK;
// }

long JS_bmdDatagramSet_to_addSignature(	void *hDB,
					bmdDatagramSet_t *bmdJSRequestSet,
					bmdDatagramSet_t **bmdJSResponseSet,
					insert_data_t *data,
					server_request_data_t *req)
{
long status	= 0;
long i		= 0;

	PRINT_INFO("LIBBMDSQLINF Adding signature (datagramset)\n");

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if (hDB==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (bmdJSRequestSet==NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	/************************************************/
	/*	przygotowanie datagramsetu odpowiedzi	*/
	/************************************************/
	BMD_FOK(PR2_bmdDatagramSet_fill_with_datagrams(bmdJSRequestSet->bmdDatagramSetSize, bmdJSResponseSet));

	/********************************************************/
	/*	obsługa poszczegolnych datagramow z zadaniami	*/
	/********************************************************/
	for(i=0; i<(int)bmdJSRequestSet->bmdDatagramSetSize; i++)
	{
		/******************************/
		/*	poczatek transakcji	*/
		/******************************/
		BMD_FOK(bmd_db_start_transaction(hDB,BMD_TRANSACTION_LEVEL_NONE));

		/************************************************************************/
		/*	sprawdzenie, czy wczesniej nie wykryto bledu w datagramie zadania	*/
		/************************************************************************/
		if (bmdJSRequestSet->bmdDatagramSetTable[i]->datagramStatus<BMD_OK)
		{
			PR_bmdDatagram_PrepareResponse(&((*bmdJSResponseSet)->bmdDatagramSetTable[i]), bmdJSRequestSet->bmdDatagramSetTable[i]->datagramType+100, bmdJSRequestSet->bmdDatagramSetTable[i]->datagramStatus);
			continue;
		}

		/************************************************/
		/*	obsluzenie pojedynczego datagramu	*/
		/************************************************/
		status=JS_bmdDatagram_to_addSignature(hDB,bmdJSRequestSet->bmdDatagramSetTable[i], &((*bmdJSResponseSet)->bmdDatagramSetTable[i]), data,req);

		/**************************************/
		/* ustawienie statusu błędu datagramu */
		/**************************************/
		(*bmdJSResponseSet)->bmdDatagramSetTable[i]->datagramType=bmdJSRequestSet->bmdDatagramSetTable[i]->datagramType+100;
		(*bmdJSResponseSet)->bmdDatagramSetTable[i]->datagramStatus=status;

		/****************************************/
		/* zarejestrowanie akcji w bazie danych */
		/****************************************/
		if (_GLOBAL_bmd_configuration->messaging_plugin_conf.bmd_messaging_plugin_register_action)
		{
			status=_GLOBAL_bmd_configuration->messaging_plugin_conf.bmd_messaging_plugin_register_action(hDB, bmdJSRequestSet->bmdDatagramSetTable[i], (*bmdJSResponseSet)->bmdDatagramSetTable[i], req);
			if (status<BMD_OK)
			{
				(*bmdJSResponseSet)->bmdDatagramSetTable[i]->datagramStatus=status;
			}
		}

		if ((*bmdJSResponseSet)->bmdDatagramSetTable[i]->datagramStatus<BMD_OK)
		{
			bmd_db_rollback_transaction(hDB,NULL);
		}
		else
		{
			bmd_db_end_transaction(hDB);
		}

	}

	return BMD_OK;
}


/**
* @author WSz
* @brief
* Funckja wykorzysywana jest przy dodawaniu podpisu pades dla dokumentu umieszczonego wczesniej w bmd.
* Funkcja szyfruje i wstawia do bazy podpisany pdf, ktory zostapi niepodpisany,
* a jesli wlaczone jest w konfiguracji znakowanie czasem dokumentu glownego,
* rowniez znakuje i wstawia pozyskany znacznik do datagramu zadania,
* by byl uwzgledniony przy wstawainiu metadanych dowodowych dla nowej wersji.
* @param[in] hDB to uchwyt do przygotowanego polaczenia bazodanowego
* @param[in|out] requestDatagram to datagram zadania dodania podpisu (PAdES) [dodaje klucz symetryczny i opcjonalnie znacznik czasu]
* @param[out] rasterId funkcja przypisuje do niego id loba wstawionego podpisanego pdf
* @param[out] lobSize funkcja przypisuje rozmiar loba rasterId
* @param[out] hashValueAsHexString przypisuje skrot wyliczony z bloba podpisanego pdf w postaci hexadecymalnej
* @return zwraca BMD_OK lub wartosc ujemna w przypadku bledu 
*/
long addPades_InsertBlobAndTimestamp(void *hDB, bmdDatagram_t *requestDatagram, char** rasterId, long *lobSize, char** hashValueAsHexString)
{
long retVal				= 0;
long iter				= 0;
long tempEncryptedLobSize		= 0;
long indexInPkiMetadataTable		= -1; //indeskacja od 0 , wiec musi byc zaincjalizowane wartoscia ujemna
GenBuf_t* der_symkey			= NULL;
GenBuf_t* encryptedLob			= NULL;
bmd_crypt_ctx_t* sym_ctx		= NULL; // kontekst do zaszyfrowania wstawianego loba
bmd_crypt_ctx_t* hash_ctx		= NULL; // kontekst do wyliczenia skrótu z wstawianego dokumentu (podpisanego pdf'a)
bmd_crypt_ctx_t* creationCtx		= NULL; // kontekst potrzebny do wygenerowania nowego klucza symetrycznego
GenBuf_t *hashValue			= NULL;
GenBuf_t *timestampBuf			= NULL;
char* tempHashHexString			= NULL;
char* tempNewVersionLobId		= NULL;
GenBuf_t padesBindingBuffer;


	if(hDB == NULL)
		{ BMD_FOK(BMD_ERR_PARAM1); }
	if(requestDatagram == NULL)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(requestDatagram->pkiMetaData == NULL || requestDatagram->no_of_pkiMetaData <= 0)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(rasterId == NULL)
		{ BMD_FOK(BMD_ERR_PARAM3); }
	if(*rasterId != NULL)
		{ BMD_FOK(BMD_ERR_PARAM3); }
	if(lobSize == NULL)
		{ BMD_FOK(BMD_ERR_PARAM4); }
	if(hashValueAsHexString == NULL)
		{ BMD_FOK(BMD_ERR_PARAM5); }
	if(*hashValueAsHexString != NULL)
		{ BMD_FOK(BMD_ERR_PARAM5); }

	if(_GLOBAL_bmd_configuration == NULL)
		{ BMD_FOK(BMD_ERR_OP_FAILED); }
	if(_GLOBAL_bmd_configuration->crypto_conf == NULL)
		{ BMD_FOK(BMD_ERR_OP_FAILED); }
	if(_GLOBAL_bmd_configuration->crypto_conf->server_ctx == NULL)
		{ BMD_FOK(BMD_ERR_OP_FAILED); }
	

	//wyszukanie w zadaniu metadanej z podpisanym pdf
	for(iter=0; iter<requestDatagram->no_of_pkiMetaData; iter++)
	{
		if(requestDatagram->pkiMetaData[iter] == NULL)
		{
			BMD_FOK(BMD_ERR_OP_FAILED);
		}
		if(requestDatagram->pkiMetaData[iter]->OIDTableBuf == NULL)
		{
			BMD_FOK(BMD_ERR_OP_FAILED);
		}
		
		
		if(strcmp(requestDatagram->pkiMetaData[iter]->OIDTableBuf, OID_PKI_METADATA_PADES_NEW_CRYPTO_OBJECT) == 0)
		{
			if(requestDatagram->pkiMetaData[iter]->AnyBuf == NULL)
			{
				BMD_FOK(BMD_ERR_OP_FAILED);
			}
			if(requestDatagram->pkiMetaData[iter]->AnySize <= 0)
			{
				BMD_FOK(BMD_ERR_OP_FAILED);
			}

			if(indexInPkiMetadataTable >= 0) // jesli kilka razy umieszczony w zadaniu, to blad
			{
				BMD_FOK(LIBBMDSQL_PADES_TOO_MANY_NEW_CRYPTO_OBJECT);
			}
			indexInPkiMetadataTable = iter;
		}
	}

	if(indexInPkiMetadataTable < 0) // jesli brak podpisanego PDF w zadaniu, to blad
	{
		BMD_FOK(LIBBMDSQL_PADES_NEW_CRYPTO_OBJECT_NOT_SPECIFIED);
	}

	// generowanie nowego klucza symetrycznego
	// BMD_FOK(bmd2_datagram_get_symkey(bmdJSRequest, &der_symkey)); // dla dodawania podpisu brakuje w datagramie
	BMD_FOK(bmd_set_ctx_sym(&creationCtx, BMD_CRYPT_ALGO_DES3, NULL, NULL));

	retVal = bmd_create_der_bmdKeyIV(BMD_CRYPT_ALGO_RSA, BMD_CRYPT_ALGO_DES3, _GLOBAL_bmd_configuration->crypto_conf->server_ctx,
						creationCtx->sym->key, creationCtx->sym->IV, NULL, &der_symkey);
	bmd_ctx_destroy(&creationCtx);
	BMD_FOK(retVal);

	// ustawienie w zadaniu wygenerowanego klucza symetrycznego, aby mozliwe bylo jego podmienienie dla crypto_objectu
	retVal = bmd2_datagram_update_symkey(der_symkey, &requestDatagram);
	if(retVal < BMD_OK)
	{
		free_gen_buf(&der_symkey);
		BMD_FOK(retVal);
	}

	// podpinanie bufora (zeby nie kopiowac i nie zwiekszac zuzycia pamiecia - podpisany pdf moze byc spory)
	padesBindingBuffer.buf = requestDatagram->pkiMetaData[indexInPkiMetadataTable]->AnyBuf;
	padesBindingBuffer.size = requestDatagram->pkiMetaData[indexInPkiMetadataTable]->AnySize;


	// wyliczanie skrotu (do podmienienia crypto_objcets.hash_value i na potrzeby timestampowania)
	retVal = bmd_set_ctx_hash(&hash_ctx, BMD_HASH_ALGO_SHA1);
	if(retVal < BMD_OK)
	{
		free_gen_buf(&der_symkey);
		BMD_FOK(retVal);
	}
	
	retVal = bmd_hash_data(&padesBindingBuffer, &hash_ctx, &hashValue, NULL);
	bmd_ctx_destroy(&hash_ctx);
	if(retVal < BMD_OK)
	{
		free_gen_buf(&der_symkey);
		BMD_FOK(retVal);
	}
	
	// przygotowanie konktestu do zaszyfrowania wstawianego loba
	// _GLOBAL_bmd_configuration->crypto_conf->server_ctx tozsame z insert_data_t->serverPrivateKey
	retVal = bmd_decode_bmdKeyIV(der_symkey, NULL, _GLOBAL_bmd_configuration->crypto_conf->server_ctx, &sym_ctx);
	free_gen_buf(&der_symkey);
	if(retVal < BMD_OK)
	{
		free_gen_buf(&hashValue);
		BMD_FOK(retVal);
	}

	// zaszyfrowanie loba nowego dokumentu glownego (podpisanego pdf) przed wstawieniem
	retVal = bmdpki_symmetric_encrypt(sym_ctx, &padesBindingBuffer, BMDPKI_SYM_LAST_CHUNK, &encryptedLob);
	bmd_ctx_destroy(&sym_ctx);
	padesBindingBuffer.buf = NULL;
	padesBindingBuffer.size = 0;
	if(retVal < BMD_OK)
	{
		free_gen_buf(&hashValue);
		BMD_FOK(retVal);
	}

	tempEncryptedLobSize =  encryptedLob->size;

	// wstawienie zaszyfrowanego loba do bazy
	retVal = bmd_db_export_blob(hDB, encryptedLob, &tempNewVersionLobId);
	free_gen_buf(&encryptedLob);
	if(retVal < BMD_OK)
	{
		free_gen_buf(&hashValue);
		BMD_FOK(retVal);
	}


	// znakowanie czasem dokumentu glownego
	if(_GLOBAL_bmd_configuration->conservate_plugin_conf.bmd_do_timestamp_plugin)
	{
		// niezaleznie od wlaczonego/wylaczonego asynchronicznego znakowania, tutaj zawsze wykonywane synchroniczne
		retVal = _GLOBAL_bmd_configuration->conservate_plugin_conf.bmd_do_timestamp_plugin( hashValue, MAIN_TS, HASH_DATA, &timestampBuf);
		if(retVal < BMD_OK)
		{
			free_gen_buf(&hashValue);
			BMD_FOK(retVal);
		}

		// jesli oznakowany plik glowny, to wstawiamy znacznik do datagramu, aby pozniej funkcja insert_pki_metadata wstawilo za nas
		// jesli wlaczone znakowanie dokumentu glownego, to ponizszy warunek bedzie prawdziwy
		if(timestampBuf != NULL)
		{
			retVal = PR2_bmdDatagram_add_metadata(OID_PKI_METADATA_TIMESTAMP, timestampBuf->buf, timestampBuf->size, PKI_METADATA, requestDatagram, 0,0,0, 1);
			if(retVal < BMD_OK)
			{
				free_gen_buf(&timestampBuf);
				free_gen_buf(&hashValue);
				BMD_FOK(retVal);
			}
			
			timestampBuf->buf = NULL; timestampBuf->size = 0;
			free_gen_buf(&timestampBuf);
		}
	}

	retVal = bmd_genbuf_hex_dump(hashValue, &tempHashHexString, 0, 1);
	free_gen_buf(&hashValue);
	BMD_FOK(retVal);


	*rasterId = tempNewVersionLobId;
	tempNewVersionLobId = NULL;

	*lobSize = tempEncryptedLobSize;

	*hashValueAsHexString = tempHashHexString;
	tempHashHexString = NULL;

	return BMD_OK;
}


/**
* @author WSz
* @brief
* Funckja wykorzysywana jest przy dodawaniu podpisu pades dla dokumentu umieszczonego wczesniej w bmd.
* Funkcja przenosi porzednia wersje (niepodpisany pdf) do historii wersji i podmienia aktualna wersje na podpisana z nowa wartoscia dowodowa
* @param[in] hDB to uchwyt do przygotowanego polaczenia bazodanowego
* @param[in] requestDatagram to datagram zadania dodania podpisu (PAdES)
* @param[in] responseDatagram to datagram odpowiedzi na zadanie dodania podpisu (PAdES)
* @param[ignored] req
* @return zwraca BMD_OK lub wartosc ujemna w przypadku bledu
*/
long addPades_updateVersion(void *hDB, char* cryptoObjectId, bmdDatagram_t* requestDatagram, bmdDatagram_t* responseDatagram, server_request_data_t *req)
{
long retVal			= 0;
long lobSize			= 0;
char* lobSizeString		= NULL;
char* newVersionLobId		= NULL;
char* symKeyLobId		= NULL;
char* hashValueHexString	= NULL;


	if(hDB == NULL)
		{ BMD_FOK(BMD_ERR_PARAM1); }
	if(cryptoObjectId == NULL)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(requestDatagram == NULL)
		{ BMD_FOK(BMD_ERR_PARAM3); }
	if(responseDatagram == NULL)
		{ BMD_FOK(BMD_ERR_PARAM4); }
	if(req == NULL)
		{ BMD_FOK(BMD_ERR_PARAM6); }


	//wstawienie nowego loba (podpisanego pdf) i opcjonalne oznakowanie tego dokumentu glowenego
	retVal = addPades_InsertBlobAndTimestamp(hDB, requestDatagram, &newVersionLobId, &lobSize, &hashValueHexString);
	BMD_FOK(retVal);

	//umieszczenie w bazie danych loba klucza symetrycznego, ktorym zaszyfrowana jest nowa wersja pliku
	retVal=versionInsertSymKeyIntoDatabase(hDB, requestDatagram, &symKeyLobId);
	if(retVal < BMD_OK)
	{
		free(newVersionLobId); newVersionLobId=NULL;
		free(hashValueHexString); hashValueHexString=NULL;
		PRINT_ERROR("LIBBMDSQL Unable to insert symetric key to database (status=%li)\n", retVal);
		BMD_FOK(BMD_ERR_OP_FAILED);
	}

	//blokada, aby nie pomieszac rownoleglych operacji wstawiania nowej wersji
	retVal=LockCryptoObjectsRow(hDB, cryptoObjectId);
	if(retVal < BMD_OK)
	{
		free(symKeyLobId); symKeyLobId=NULL;
		free(newVersionLobId); newVersionLobId=NULL;
		free(hashValueHexString); hashValueHexString=NULL;
		PRINT_ERROR("LIBBMDSQL Unable to create row lock (status=%li)\n", retVal);
		BMD_FOK(BMD_ERR_OP_FAILED);
	}

	//wstawienie nowego rekordu do tabeli histori wersji pliku (crypto_objects_history)
	retVal=versionInsertToCryptoObjectsHistory(hDB, cryptoObjectId);
	if(retVal < BMD_OK)
	{
		free(symKeyLobId); symKeyLobId=NULL;
		free(newVersionLobId); newVersionLobId=NULL;
		free(hashValueHexString); hashValueHexString=NULL;
		PRINT_ERROR("LIBBMDSQL Unable to insert record to crypto_objects' history (status=%li)\n", retVal);
		BMD_FOK(BMD_ERR_OP_FAILED);
	}

	retVal = asprintf(&lobSizeString, "%lu", lobSize);
	if(retVal == -1)
	{
		free(symKeyLobId); symKeyLobId=NULL;
		free(newVersionLobId); newVersionLobId=NULL;
		free(hashValueHexString); hashValueHexString=NULL;
		BMD_FOK(NO_MEMORY);
	}

	//uaktualnienie metadanych systemowych dla aktualizowanego crypto_object
	retVal=versionUpdateCryptoObjectSystemMetadata(hDB, cryptoObjectId,
			lobSizeString, newVersionLobId/*raster*/, symKeyLobId, hashValueHexString);
	free(symKeyLobId); symKeyLobId=NULL;
	free(newVersionLobId); newVersionLobId=NULL;
	free(hashValueHexString); hashValueHexString=NULL;
	free(lobSizeString); lobSizeString=NULL;

	if(retVal < 0)
	{
		PRINT_ERROR("LIBBMDSQL Unable to update crypto objects (status=%li)\n", retVal);
		BMD_FOK(BMD_ERR_OP_FAILED);
	}

	//wkazanie (we wpisach dotychczasowej wartosci dowodowej) wersji przeniesionej do historii
	retVal=versionLinkProofsToCryptoObjectsHistoryEntry(hDB, cryptoObjectId);
	if(retVal < 0)
	{
		PRINT_ERROR("LIBBMDSQL Unable to link proofs for older version (status=%li)\n", retVal);
		BMD_FOK(BMD_ERR_OP_FAILED);
	}


	// przepisanie do odpowiedzi identyfikatora aktualizowanego pliku
	free_gen_buf( &(responseDatagram->protocolDataFileId) );
	BMD_FOK(set_gen_buf2(requestDatagram->protocolDataFileId->buf, requestDatagram->protocolDataFileId->size , &(responseDatagram->protocolDataFileId) ));

	return BMD_OK;
}

/***********************************/
/* dodanie podpsu - jeden datagram */
/***********************************/
long JS_bmdDatagram_to_addSignature(	void *hDB,
					bmdDatagram_t *bmdJSRequest,
					bmdDatagram_t **bmdJSResponse,
					insert_data_t *data,
					server_request_data_t *req)
{
long status			= 0;
long i				= 0;
//long signatureMetadataIndex	= 0;
long dvcs_status		= 0;
char *CryptoObjectNr		= NULL;
char *hashHexStr			= NULL;
char *SQLQuery			= NULL;
GenBuf_t *hash_buf		= NULL;
char *ignoredPtr		= NULL; // podawane tylko dlatego, ze wymagane, ale nie ma tutaj zadnego znaczenia
long isPades = 0;
long isCades = 0;
char* hashAlgorithmOid = NULL;
char* fileType = NULL;

const char* sqlTemplate		= "SELECT id FROM prf_signatures WHERE fk_crypto_objects='%s' AND location_id='%s' ";
char* sqlQuery			= NULL;
void* queryResults		= NULL;
long rowsCount			= 0;


	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if(hDB == NULL)
		{ BMD_FOK(BMD_ERR_PARAM1); }
	if(bmdJSRequest == NULL)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(bmdJSRequest->protocolDataFileId == NULL)
		{ BMD_FOK(LIBBMDSQL_ADD_SIGNATURE_NO_FILE_ID_SPECIFIED); }
	if(bmdJSResponse == NULL)
		{ BMD_FOK(BMD_ERR_PARAM3); }
	if(data == NULL)
		{ BMD_FOK(BMD_ERR_PARAM4); }
	if(req == NULL)
		{ BMD_FOK(BMD_ERR_PARAM5); }

	if(_GLOBAL_bmd_configuration == NULL)
		{ BMD_FOK(BMD_ERR_OP_FAILED); }
	if(_GLOBAL_bmd_configuration->location_id == NULL)
		{ BMD_FOK(BMD_ERR_OP_FAILED); }

	
	// sprawdzanie czy plik do update'a istnieje
	BMD_FOK(VerifyPermissionsForCryptoObject(hDB, req,
				bmdJSRequest->protocolDataFileId->buf,
				CONSIDER_CREATOR_OR_OWNER_OR_GROUPS,
				NULL/*fileName*/,
				&fileType/*fileType*/,
				&CryptoObjectNr,
				NULL/*correspondingId*/,
				NULL/*forGrant*/,
				NULL/*additionalMetadataCount*/,
				NULL/*additionalMetadataValues*/));
	
	// jesli katalog, to nie mozna wykonac operacji
	if(strcmp(fileType, "dir")==0)
	{
		free(fileType); fileType = NULL;
		free(CryptoObjectNr); CryptoObjectNr = NULL;
		BMD_FOK(BMD_ERR_OP_FAILED);
	}
	free(fileType); fileType=NULL;


	// sprawdzenie, czy juz jest podpisane - niedopuszczalne jest ponowne podpisywanie
	if(_GLOBAL_bmd_configuration->allow_only_one_signature_per_file == 1)
	{
		status = asprintf(&sqlQuery, sqlTemplate, CryptoObjectNr, _GLOBAL_bmd_configuration->location_id);
		if(status == -1)
		{
			free(CryptoObjectNr); CryptoObjectNr = NULL;
			BMD_FOK(BMD_ERR_MEMORY);
		}
		status = bmd_db_execute_sql(hDB, sqlQuery, &rowsCount, NULL, &queryResults);
		free(sqlQuery); sqlQuery = NULL;
		bmd_db_result_free(&queryResults);
		if(status < BMD_OK)
		{
			free(CryptoObjectNr); CryptoObjectNr = NULL;
			BMD_FOK(status);
		}
		if(rowsCount > 0)
		{
			free(CryptoObjectNr); CryptoObjectNr = NULL;
			BMD_FOK(LIBBMDSQL_ONLY_ONE_SIGNATURE_ALLOWED);
		}
	}

	// poszukiwanie podpisu w datagramie
	for (i=0; i<bmdJSRequest->no_of_pkiMetaData; i++)
	{
		if(bmdJSRequest->pkiMetaData[i] != NULL && bmdJSRequest->pkiMetaData[i]->OIDTableBuf != NULL)
		{
			if (strcmp(bmdJSRequest->pkiMetaData[i]->OIDTableBuf, OID_PKI_METADATA_SIGNATURE)==0)
			{
				//signatureMetadataIndex = i;
				isCades = 1;
				break;
			}
			else if (strcmp(bmdJSRequest->pkiMetaData[i]->OIDTableBuf, OID_PKI_METADATA_SIGNATURE_PADES)==0)
			{
				//signatureMetadataIndex = i;
				isPades = 1;
				break;
			}
		}
	}

	if(isCades == 0 && isPades == 0)
	{
		free(CryptoObjectNr); CryptoObjectNr = NULL;
		BMD_FOK(BMD_ERR_OP_FAILED);
	}

	// dla pades podmiana wersji
	if(isPades == 1)
	{
		status = addPades_updateVersion(hDB, CryptoObjectNr, bmdJSRequest, *bmdJSResponse, req);
		if(status < BMD_OK)
		{
			free(CryptoObjectNr); CryptoObjectNr = NULL;
			BMD_FOK(status);
		}
	}


	// sprawdzamy poprawnosc podpisu pod dokumentem na podstawe jego skrotu
	if (_GLOBAL_bmd_configuration->dvcs_plugin_conf.bmd_dvcs_plugin_validate_external_signature)
	{
		// pobranie skrotu do weryfikacji dvcs na podstawie skrotu z podpisanych danych
		if(isCades == 1)
		{
			status = asprintf(&SQLQuery, "SELECT hash_value FROM crypto_objects WHERE id='%s' AND location_id='%s' ", CryptoObjectNr, _GLOBAL_bmd_configuration->location_id);
			if(status == -1)
			{
				free(CryptoObjectNr); CryptoObjectNr = NULL;
				BMD_FOK(NO_MEMORY);
			}
		
			status = ExecuteSQLQueryWithAnswerKnownDBConnection(hDB, SQLQuery, &hashHexStr);
			free(SQLQuery); SQLQuery = NULL;
			if(status < BMD_OK)
			{
				free(CryptoObjectNr); CryptoObjectNr = NULL;
				BMD_FOK(status);
			}
		
			// przeksztalcenie skrotu z postaci hex do binarnej
			status = bmd_hex_genbuf_bin_dump(hashHexStr, &hash_buf);
			free(hashHexStr); hashHexStr = NULL;
			if(status < BMD_OK)
			{
				free(CryptoObjectNr); CryptoObjectNr = NULL;
				BMD_FOK(status);
			}
		}
		else if(isPades == 1)
		{
			status = GetPadesHashValueAndAlgorithm(bmdJSRequest, &hashHexStr, &hashAlgorithmOid);
			free(hashAlgorithmOid); hashAlgorithmOid = NULL;
			if(status < BMD_OK)
			{
				free(CryptoObjectNr); CryptoObjectNr = NULL;
				BMD_FOK(status);
			}
			
			status = bmd_hex_genbuf_bin_dump(hashHexStr, &hash_buf);
			free(hashHexStr); hashHexStr = NULL;
			if(status < BMD_OK)
			{
				free(CryptoObjectNr); CryptoObjectNr = NULL;
				BMD_FOK(status);
			}
		}
		else
		{
			free(CryptoObjectNr); CryptoObjectNr = NULL;
			BMD_FOK(BMD_ERR_UNIMPLEMENTED);
		}

		dvcs_status=_GLOBAL_bmd_configuration->dvcs_plugin_conf.bmd_dvcs_plugin_validate_external_signature(bmdJSRequest, hash_buf);
		free_gen_buf(&hash_buf);
		if (dvcs_status<BMD_OK)
		{
			free(CryptoObjectNr); CryptoObjectNr = NULL;
			BMD_FOK(dvcs_status);
		}
	}

	// wstawienie danych pki (podpis)
	status=JS_PrepareAndExecuteSQLQuery_Insert_PkiMetaData(hDB, bmdJSRequest, CryptoObjectNr, data, req, &ignoredPtr);
	free(ignoredPtr); ignoredPtr = NULL;
	free(CryptoObjectNr); CryptoObjectNr = NULL;
	BMD_FOK(status);

	// uzupelnienie odpowiedzi o metadane dodatkowe, bmdId itp.
	BMD_FOK(FillFromAllMetadata(hDB, bmdJSRequest->protocolDataFileId->buf, _GLOBAL_bmd_configuration->location_id,
						WITHOUT_PKI_METADATA,
						WITH_ADD_METADATA,
						WITHOUT_SYS_METADATA,
						WITHOUT_ACTION_METADATA,
						*bmdJSResponse,
						req));
	return BMD_OK;
}


long JS_bmdDatagram_to_updateSysMetadata(	void *hDB,
				bmdDatagram_t *requestDatagram,
				bmdDatagram_t *responseDatagram,
				server_request_data_t *req)
{
long iter					= 0;
long retVal					= 0;
char* correspondingId		= NULL;
char* visibility			= NULL;
char* fileType				= NULL;
char* pointingIdForCorrespondingFile	= NULL;
char* escapedFileId			= NULL;
char* escapedVisibility		= NULL;

char* sqlQuery				= NULL;
void* queryResult			= NULL;

char* correspondingIdSqlTemplate	= "UPDATE crypto_objects SET corresponding_id=%s WHERE\n\
id IN(SELECT id FROM crypto_objects WHERE pointing_id=%s AND file_type<>'dir' AND location_id=%s );";

char* visibleSqlTemplate	= "UPDATE crypto_objects SET visible=%s WHERE id=%s AND location_id=%s;";

	if(hDB == NULL)
		{ BMD_FOK(BMD_ERR_PARAM1); }
	if(requestDatagram == NULL)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(requestDatagram->datagramType != BMD_DATAGRAM_TYPE_UPDATE_SYS_METADATA)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(responseDatagram == NULL)
		{ BMD_FOK(BMD_ERR_PARAM3); }
	if(req == NULL)
		{ BMD_FOK(BMD_ERR_PARAM4); }
	
	
	if(requestDatagram->protocolDataFileId == NULL)
		{ BMD_FOK(BMD_ERR_FILE_ID_NOT_SPECIFIED); }
	if(requestDatagram->protocolDataFileId->buf == NULL || requestDatagram->protocolDataFileId->size <= 0)
		{ BMD_FOK(BMD_ERR_FILE_ID_NOT_SPECIFIED); }
	
	for(iter=0; iter<requestDatagram->no_of_sysMetaData; iter++)
	{
		if(requestDatagram->sysMetaData[iter] != NULL)
		{
			if(requestDatagram->sysMetaData[iter]->OIDTableBuf != NULL)
			{
				if(correspondingId == NULL && strcmp(requestDatagram->sysMetaData[iter]->OIDTableBuf, OID_SYS_METADATA_CRYPTO_OBJECTS_CORESPONDING_ID) == 0)
				{
					correspondingId=STRNDUP(requestDatagram->sysMetaData[iter]->AnyBuf, requestDatagram->sysMetaData[iter]->AnySize);
					if(correspondingId == NULL)
					{
						free(visibility);
						BMD_FOK(NO_MEMORY);
					}
				}
				else if(visibility == NULL && strcmp(requestDatagram->sysMetaData[iter]->OIDTableBuf, OID_SYS_METADATA_CRYPTO_OBJECTS_VISIBLE) == 0)
				{
					visibility=STRNDUP(requestDatagram->sysMetaData[iter]->AnyBuf, requestDatagram->sysMetaData[iter]->AnySize);
					if(visibility == NULL)
					{
						free(correspondingId);
						BMD_FOK(NO_MEMORY);
					}
					if(strcmp(visibility, "0") != 0 && strcmp(visibility, "1") != 0)
					{
						free(visibility);
						free(correspondingId);
						BMD_FOK(LIBBMDSQL_INCORRECT_VISIBILITY_VALUE);
					}
				}
			}
		}
	}
	
	if(correspondingId == NULL && visibility == NULL)
	{
		BMD_FOK(LIBBMDSQL_NO_METADATA_FOR_UPDATE);
	}

	retVal=VerifyPermissionsForCryptoObject(hDB, req, requestDatagram->protocolDataFileId->buf,
				CONSIDER_CREATOR_OR_OWNER, NULL, &fileType, NULL, NULL, NULL, NULL, NULL);
	if(retVal < BMD_OK)
	{
		free(visibility);
		free(correspondingId);
		BMD_FOK(retVal);
	}

	if(correspondingId != NULL)
	{
		if(strcmp(fileType, "link") == 0 || strcmp(fileType, "dir") == 0)
		{
			free(fileType);
			free(visibility);
			free(correspondingId);
			BMD_FOK(LIBBMDSQL_NOT_REGULAR_FILE_ID);
		}
		
		retVal=VerifyPermissionsForCryptoObject(hDB, req, correspondingId, CONSIDER_CREATOR_OR_OWNER,
				NULL, NULL, &pointingIdForCorrespondingFile, NULL, NULL, NULL, NULL);
		free(correspondingId); correspondingId=NULL;
		if(retVal < BMD_OK)
		{
			free(fileType);
			free(visibility);
			PRINT_ERROR("LIBBMDSQL ERROR: Unable to find file specified by correspondingId\n");
			BMD_FOK(retVal);
		}

	}
	
	retVal=bmd_db_start_transaction(hDB, BMD_TRANSACTION_LEVEL_NONE);
	if(retVal < BMD_OK)
	{
		free(pointingIdForCorrespondingFile);
		free(visibility);
		BMD_FOK(BMD_ERR_OP_FAILED);
	}
	
	if(pointingIdForCorrespondingFile != NULL)
	{
		retVal=bmd_db_escape_string(hDB, requestDatagram->protocolDataFileId->buf, STANDARD_ESCAPING, &escapedFileId);
		if(retVal < BMD_OK)
		{
			free(visibility);
			bmd_db_rollback_transaction(hDB,NULL);
			BMD_FOK(retVal);
		}
			
		asprintf(&sqlQuery, correspondingIdSqlTemplate, pointingIdForCorrespondingFile, escapedFileId, _GLOBAL_bmd_configuration->location_id);
		free(pointingIdForCorrespondingFile); pointingIdForCorrespondingFile=NULL;
		free(escapedFileId); escapedFileId=NULL;
		if(sqlQuery == NULL)
		{
			free(visibility);
			bmd_db_rollback_transaction(hDB,NULL);
			BMD_FOK(NO_MEMORY);
		}
		
		retVal=bmd_db_execute_sql(hDB, sqlQuery, NULL, NULL, &queryResult);
		free(sqlQuery); sqlQuery=NULL;
		bmd_db_result_free(&queryResult);
		if(retVal < BMD_OK)
		{
			free(visibility);
			bmd_db_rollback_transaction(hDB,NULL);
			BMD_FOK(retVal);
		}
	}
	
	if(visibility != NULL)
	{
		retVal=bmd_db_escape_string(hDB, requestDatagram->protocolDataFileId->buf, STANDARD_ESCAPING, &escapedFileId);
		if(retVal < BMD_OK)
		{
			free(visibility);
			bmd_db_rollback_transaction(hDB,NULL);
			BMD_FOK(retVal);
		}
		
		retVal=bmd_db_escape_string(hDB, visibility, STANDARD_ESCAPING, &escapedVisibility);
		free(visibility); visibility=NULL;
		if(retVal < BMD_OK)
		{
			free(escapedFileId);
			bmd_db_rollback_transaction(hDB,NULL);
			BMD_FOK(retVal);
		}
	
		asprintf(&sqlQuery, visibleSqlTemplate, escapedVisibility, escapedFileId, _GLOBAL_bmd_configuration->location_id);
		free(escapedVisibility); escapedVisibility=NULL;
		free(escapedFileId); escapedFileId=NULL;
		if(sqlQuery == NULL)
		{
			bmd_db_rollback_transaction(hDB,NULL);
			BMD_FOK(NO_MEMORY);
		}
		
		retVal=bmd_db_execute_sql(hDB, sqlQuery, NULL, NULL, &queryResult);
		free(sqlQuery); sqlQuery=NULL;
		bmd_db_result_free(&queryResult);
		if(retVal < BMD_OK)
		{
			bmd_db_rollback_transaction(hDB,NULL);
			BMD_FOK(retVal);
		}
		
	}
	
	BMD_FOK_TRANS(bmd_db_end_transaction(hDB));
	
	return BMD_OK;
}

long JS_bmdDatagramSet_to_updateSysMetadata(void *hDB,
					bmdDatagramSet_t *bmdJSRequestSet,
					bmdDatagramSet_t **bmdJSResponseSet,
					server_request_data_t *req)
{
long retVal		= 0;
long iter		= 0;

	PRINT_INFO("LIBBMDSQLINF Updating system metadata (datagramset)\n");
	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if(hDB == NULL)
		{ BMD_FOK(BMD_ERR_PARAM1); }
	if(bmdJSRequestSet == NULL)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(bmdJSResponseSet == NULL)
		{ BMD_FOK(BMD_ERR_PARAM3); }
	if(*bmdJSResponseSet != NULL)
		{ BMD_FOK(BMD_ERR_PARAM3); }
	if(req == NULL)
		{ BMD_FOK(BMD_ERR_PARAM4); }
	
	
	BMD_FOK(PR2_bmdDatagramSet_fill_with_datagrams(bmdJSRequestSet->bmdDatagramSetSize, bmdJSResponseSet));
	
	for(iter=0; iter< bmdJSRequestSet->bmdDatagramSetSize; iter++)
	{
	
		if(bmdJSRequestSet->bmdDatagramSetTable[iter]->protocolDataFileId != NULL)
		{
			// przekopiowanie fileId do datagramu odpowiedzi
			if(bmdJSRequestSet->bmdDatagramSetTable[iter]->protocolDataFileId->buf != NULL &&
				bmdJSRequestSet->bmdDatagramSetTable[iter]->protocolDataFileId->size > 0)
			{
				free_gen_buf(&((*bmdJSResponseSet)->bmdDatagramSetTable[iter]->protocolDataFileId));
				set_gen_buf2(bmdJSRequestSet->bmdDatagramSetTable[iter]->protocolDataFileId->buf,
							bmdJSRequestSet->bmdDatagramSetTable[iter]->protocolDataFileId->size,
							&((*bmdJSResponseSet)->bmdDatagramSetTable[iter]->protocolDataFileId) );
			}
		}
	
		retVal=JS_bmdDatagram_to_updateSysMetadata(hDB, bmdJSRequestSet->bmdDatagramSetTable[iter],
									(*bmdJSResponseSet)->bmdDatagramSetTable[iter], req);
		
		(*bmdJSResponseSet)->bmdDatagramSetTable[iter]->datagramType=BMD_DATAGRAM_TYPE_UPDATE_SYS_METADATA_RESPONSE;
		(*bmdJSResponseSet)->bmdDatagramSetTable[iter]->datagramStatus=retVal;
		
		if (_GLOBAL_bmd_configuration->messaging_plugin_conf.bmd_messaging_plugin_register_action)
		{
			for(iter=0; iter<bmdJSRequestSet->bmdDatagramSetSize; iter++)
				{ retVal=_GLOBAL_bmd_configuration->messaging_plugin_conf.bmd_messaging_plugin_register_action(hDB, bmdJSRequestSet->bmdDatagramSetTable[iter], (*bmdJSResponseSet)->bmdDatagramSetTable[iter], req);	}
		}
		
	}

	return BMD_OK;
}


/**
* Funkcja VerifyAdditionalMetadataTypeAndValue() sprawdza, czy metadane dodatkowe przekazane w zadaniu istnieja w systemie i czy zdefiniowane sa w zadaniu wartosci dla tych metadanych (rozne od NULL).
* Dla zadania updatu metadanych dodatkowych sprawdzane jest dodatkowo mozliwosc updatowania danej metadanej (przepuszczane sa metadane nieupdatowalne, dla ktorych podawana wartosc jest identyczna z aktualna)
* Funkcja nie zglasza bledu, gdy nie ma zadnej metadanej dodatkowej jako kryterium.
*
* @param[in] addMetadataValues to tablica wartosci metadanych dodatkowych (zgodnie z kolejnoscia metadanych w slowniku AddMetadataTypes)
* Należy podać tylko wowczas, gdy funkcji wywolywana dla obslugi BMD_DATAGRAM_TYPE_UPDATE, w inych przypadkach moze byc NULL.
* Tablice wartosci metadanych dodatkowych mozna uzyskac np. z wywolania funkcji VerifyPermissionsForCryptoObjects
* @param[in] addMetadataValuesCount to ilosc elementow tablicy addMetadataValues
*
* @return 0, jesle metadane sa prawidlowe, wartosc ujemna w przypadku bledu lub niepoprawnie wypelnionych metadanych dodatowych
*/
long VerifyAdditionalMetadataTypeAndValue(bmdDatagram_t *requestDatagram, server_request_data_t *req, char** addMetadataValues, long addMetadataValuesCount)
{
long iter						= 0;
long jupiter					= 0;
long foundInForm				= 0;

long oidIndex					= 0;
char** addMetadataOids			= NULL;
long addMetadataOidsCount		= 0;
long foundInAddMetadataOids		= 0;
long metadataForUpdateCounter	= 0; //okresla, ile metadanych faktycznie ma byc updatowanych (zlicza niezignorowane)

	PRINT_INFO("LIBBMDSQLINF Verifying additional metadata type and value \n");

	if(requestDatagram == NULL)
		{ BMD_FOK(BMD_ERR_PARAM1); }
	if(requestDatagram->no_of_additionalMetaData > 0 && requestDatagram->additionalMetaData == NULL)
		{ BMD_FOK(BMD_ERR_PARAM1); }
	if(req == NULL)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(req->no_of_formularz > 0 && req->formularz == NULL)
		{ BMD_FOK(BMD_ERR_PARAM2); }

	if(requestDatagram->datagramType == BMD_DATAGRAM_TYPE_UPDATE)
	{
		if(addMetadataValues == NULL)
			{ BMD_FOK(BMD_ERR_PARAM3); }
		if(addMetadataValuesCount <= 0)
			{ BMD_FOK(BMD_ERR_PARAM4); }
		
		BMD_FOK(getColumnWithCondition(_GLOBAL_AddMetadataTypesDictionary, 0, NULL, 1, &addMetadataOidsCount, &addMetadataOids));
		
		if(addMetadataOidsCount > 0 && addMetadataOids == NULL)
			{ BMD_FOK(BMD_ERR_OP_FAILED); }
		for(iter=0; iter<addMetadataOidsCount; iter++)
		{
			if(addMetadataOids[iter] == NULL)
			{
				DestroyTableOfStrings(&addMetadataOids, addMetadataOidsCount);
				BMD_FOK(BMD_ERR_OP_FAILED);
			}
		}
	}
	
	
	
	for(iter=0; iter<requestDatagram->no_of_additionalMetaData; iter++)
	{
		if(requestDatagram->additionalMetaData[iter] == NULL)
		{
			DestroyTableOfStrings(&addMetadataOids, addMetadataOidsCount);
			BMD_FOK(BMD_ERR_OP_FAILED);
		}
		
		if(requestDatagram->additionalMetaData[iter]->OIDTableBuf == NULL)
		{
			DestroyTableOfStrings(&addMetadataOids, addMetadataOidsCount);
			BMD_FOK(INVALID_ADDITIONAL_METADATA_OID_FORMAT);
		}
		
		//retVal=getElementWithCondition(_GLOBAL_AddMetadataTypesDictionary, 1, requestDatagram->additionalMetaData[iter]->OIDTableBuf, 0, &addMetadataTypeId);
		//free(addMetadataTypeId); addMetadataTypeId=NULL;
		//if(retVal < BMD_OK)
		//{
		//	PRINT_ERROR("Unallowable additional metadata with OID = <%s>\n", requestDatagram->additionalMetaData[iter]->OIDTableBuf);
		//	BMD_FOK(retVal);
		//}
		
		foundInForm=0;
		for(jupiter=0; jupiter<req->no_of_formularz; jupiter++)
		{
			if(req->formularz[jupiter] == NULL)
			{
				DestroyTableOfStrings(&addMetadataOids, addMetadataOidsCount);
				BMD_FOK(BMD_ERR_OP_FAILED);
			}
			if(req->formularz[jupiter]->oid_str == NULL)
			{
				DestroyTableOfStrings(&addMetadataOids, addMetadataOidsCount);
				BMD_FOK(BMD_ERR_OP_FAILED);
			}
			
			if(strcmp(requestDatagram->additionalMetaData[iter]->OIDTableBuf, req->formularz[jupiter]->oid_str) == 0)
			{
				foundInForm=1;
				if(requestDatagram->datagramType == BMD_DATAGRAM_TYPE_UPDATE)
				{
					// jesli pole nie jest updatowalne, ale nowa wartosc jest identyczna z aktualna w bazie, to nie jest to blad (mozna przepuscic)
					if(req->formularz[jupiter]->isUpdateEnabled == 0)
					{
						foundInAddMetadataOids = 0;
						for(oidIndex=0; oidIndex<addMetadataOidsCount ; oidIndex++)
						{
							// odnajdowanie numeru wpisu w slowniku metadanych dodatkowych, aby bylo wiadomo, ktora wartosc metadanej brac do porownania
							if(strcmp(requestDatagram->additionalMetaData[iter]->OIDTableBuf, addMetadataOids[oidIndex]) == 0)
							{
								foundInAddMetadataOids = 1;
								break;
							}
						}
						
						if(foundInAddMetadataOids == 0)
						{
							DestroyTableOfStrings(&addMetadataOids, addMetadataOidsCount);
							PRINT_ERROR("OID = <%s> is present in form but not found in dictionary\n", requestDatagram->additionalMetaData[iter]->OIDTableBuf);
							BMD_FOK(LIBBMDSQL_DICT_VALUE_NOT_FOUND);
						}
						
						if(requestDatagram->additionalMetaData[iter]->AnyBuf == NULL && requestDatagram->additionalMetaData[iter]->AnySize)
						{
							DestroyTableOfStrings(&addMetadataOids, addMetadataOidsCount);
							PRINT_ERROR("Update of additional metadata with OID = <%s> is not permitted\n", requestDatagram->additionalMetaData[iter]->OIDTableBuf);
							BMD_FOK(BMD_VERIFY_REQUEST_METADATA_NOT_UPDATEABLE);
						}
						
						// inna wartosc dla metadanej nieupdatowalnej
						if(strncmp(addMetadataValues[oidIndex], requestDatagram->additionalMetaData[iter]->AnyBuf, requestDatagram->additionalMetaData[iter]->AnySize) != 0)
						{
							DestroyTableOfStrings(&addMetadataOids, addMetadataOidsCount);
							PRINT_ERROR("Update of additional metadata with OID = <%s> is not permitted\n", requestDatagram->additionalMetaData[iter]->OIDTableBuf);
							BMD_FOK(BMD_VERIFY_REQUEST_METADATA_NOT_UPDATEABLE);
						}
						else
						{
							PRINT_INFO("Metadata with OID = <%s> has been ignored due to unchanged value\n", requestDatagram->additionalMetaData[iter]->OIDTableBuf);
						}
					}
					else
					{
						metadataForUpdateCounter++;
					}
				}
				
				break;
			}
		}
		
		if(foundInForm == 0)
		{
			DestroyTableOfStrings(&addMetadataOids, addMetadataOidsCount);
			PRINT_ERROR("Additional metadata with OID = <%s> is not defined in user form\n", requestDatagram->additionalMetaData[iter]->OIDTableBuf);
			BMD_FOK(UNKNOWN_METADATA_TYPE);
		}
		
		if(requestDatagram->additionalMetaData[iter]->AnyBuf == NULL && requestDatagram->additionalMetaData[iter]->AnySize)
		{
			DestroyTableOfStrings(&addMetadataOids, addMetadataOidsCount);
			PRINT_ERROR("Undefined value for additional metadata with OID = <%s>\n", requestDatagram->additionalMetaData[iter]->OIDTableBuf);
			BMD_FOK(BMD_VERIFY_REQUEST_INVALID_ADD_METADATA);
		}
	}

	DestroyTableOfStrings(&addMetadataOids, addMetadataOidsCount);
	
	if(requestDatagram->datagramType == BMD_DATAGRAM_TYPE_UPDATE)
	{
		// jesli nie ma czego updatowac (nawet jesli w zadaniu sa metadane dodatkowe - zignorowane nieupdatowalne metadane z taka sama wartoscia)
		if(metadataForUpdateCounter == 0)
		{
			BMD_FOK(LIBBMDSQL_NO_METADATA_FOR_UPDATE);
		}
	}
	
	return BMD_OK;
}


/**
* Funkcja VerifySystemMetadataTypeAndValue() sprawdza, czy metadane systemowe przekazane w zadaniu maja zdefiniowane wartosci (w zadaniu) (rozne od NULL).
* Funkcja nie zglasza bledu, gdy nie ma zadnej metadanej systemowej jako kryterium.
*
* @note Sprawdzenie nie moze odbywac sie na podstawie formularza, bo w formularzu wyszukiwania np. corresponding_id bedzie ukryte, albo nie bedzie wcale ujete itp.)
*
* @return ilosc metadanych systemowych (mogacych stanowic kryterium ) alob wartosc ujemna w przypadku bledu lub niepoprawnie podanych metadanych systemowych
*/
long VerifySystemMetadataTypeAndValue(bmdDatagram_t *requestDatagram)
{
long iter	= 0;
long sysCriteriaCount	= 0;

	if(requestDatagram == NULL)
		{ BMD_FOK(BMD_ERR_PARAM1); }
	if(requestDatagram->no_of_sysMetaData > 0 && requestDatagram->sysMetaData == NULL)
		{ BMD_FOK(BMD_ERR_PARAM1); }

	for(iter=0; iter<requestDatagram->no_of_sysMetaData; iter++)
	{
		if(requestDatagram->sysMetaData[iter] == NULL)
		{
			BMD_FOK(BMD_ERR_OP_FAILED);
		}
		
		if(requestDatagram->sysMetaData[iter]->OIDTableBuf == NULL)
		{
			BMD_FOK(INVALID_SYSTEM_METADATA_OID_FORMAT);
		}

		if(	strcmp(requestDatagram->sysMetaData[iter]->OIDTableBuf, OID_SYS_METADATA_CRYPTO_OBJECTS_TIMESTAMP) == 0 ||
			strcmp(requestDatagram->sysMetaData[iter]->OIDTableBuf, OID_SYS_METADATA_CRYPTO_OBJECTS_TIMESTAMP_MORE) == 0 ||
			strcmp(requestDatagram->sysMetaData[iter]->OIDTableBuf, OID_SYS_METADATA_CRYPTO_OBJECTS_TIMESTAMP_LESS) == 0 ||
			strcmp(requestDatagram->sysMetaData[iter]->OIDTableBuf, OID_SYS_METADATA_CRYPTO_OBJECTS_FILENAME) == 0 ||
			strcmp(requestDatagram->sysMetaData[iter]->OIDTableBuf, OID_SYS_METADATA_CRYPTO_OBJECTS_FILESIZE) == 0 ||
			strcmp(requestDatagram->sysMetaData[iter]->OIDTableBuf, OID_SYS_METADATA_CRYPTO_OBJECTS_FILESIZE_MORE) == 0 ||
			strcmp(requestDatagram->sysMetaData[iter]->OIDTableBuf, OID_SYS_METADATA_CRYPTO_OBJECTS_FILESIZE_LESS) == 0 ||
			strcmp(requestDatagram->sysMetaData[iter]->OIDTableBuf, OID_SYS_METADATA_CRYPTO_OBJECTS_VISIBLE) == 0 ||
			strcmp(requestDatagram->sysMetaData[iter]->OIDTableBuf, OID_SYS_METADATA_CRYPTO_OBJECTS_FILE_TYPE) == 0 ||
			strcmp(requestDatagram->sysMetaData[iter]->OIDTableBuf, OID_SYS_METADATA_CRYPTO_OBJECTS_CORESPONDING_ID) == 0 ||
			strcmp(requestDatagram->sysMetaData[iter]->OIDTableBuf, OID_SYS_METADATA_CRYPTO_OBJECTS_POINTING_ID) == 0 ||
			strcmp(requestDatagram->sysMetaData[iter]->OIDTableBuf, OID_SYS_METADATA_CRYPTO_OBJECTS_POINTING_LOCATION_ID) == 0 ||
			strcmp(requestDatagram->sysMetaData[iter]->OIDTableBuf, OID_SYS_METADATA_CRYPTO_OBJECTS_LOCATION_ID) == 0 ||
			strcmp(requestDatagram->sysMetaData[iter]->OIDTableBuf, OID_SYS_METADATA_CRYPTO_OBJECTS_HASH_VALUE) == 0 )
		{
			// OK - dopuszczalne kryteria
			sysCriteriaCount++;
		}
		/* warunek wykomentowany, bo nie sposob przewidziec, jakie metadane pojawia sie jeszcze w zadaniu (kazde zadanie ma inna specyfike)
		else if(strcmp(requestDatagram->sysMetaData[iter]->OIDTableBuf, OID_SYS_METADATA_DTG_SYMKEY) == 0 ||
			strcmp(requestDatagram->sysMetaData[iter]->OIDTableBuf, OID_SYS_METADATA_DTG_SYMKEY_HASH) == 0)
		{
			// Ok - pomijane metadane systemowe np. klucz symetryczny
			continue;
		}
		else
		{
			PRINT_ERROR("System metadata with OID = <%s> is not allowed\n", requestDatagram->sysMetaData[iter]->OIDTableBuf);
			BMD_FOK(UNKNOWN_METADATA_TYPE);
		}
		*/

		if(requestDatagram->sysMetaData[iter]->AnyBuf == NULL && requestDatagram->sysMetaData[iter]->AnySize <= 0)
		{
			PRINT_ERROR("Undefined value for system metadata with OID = <%s>\n", requestDatagram->sysMetaData[iter]->OIDTableBuf);
			BMD_FOK(BMD_VERIFY_REQUEST_INVALID_SYS_METADATA);
		}
	}

	return sysCriteriaCount;
}
