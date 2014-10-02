#include <bmd/libbmdsql/raport.h>
#include <bmd/libbmdsql/select.h>
#include <bmd/libbmdsql/delete.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>
#include <bmd/libbmddb/libbmddb.h>
#include <time.h>
#include <bmd/libbmderr/libbmderr.h>

long JS_bmdDatagramSet_to_count(	void *hDB,
					bmdDatagramSet_t *bmdJSRequestSet,
					bmdDatagramSet_t **bmdJSResponseSet,
					server_request_data_t *req)
{
int i					= 0;
int status				= 0;

	PRINT_INFO("LIBBMDSQLINF Counting file(s) (datagramset)\n");
	/********************************/
	/*	walidacja parametrów	*/
	/********************************/
	if (hDB==NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (bmdJSRequestSet==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}

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

		/******************************************/
		/*	obsluzenie pojedynczego datagramu	*/
		/******************************************/
		status=JS_bmdDatagram_to_count(  hDB, bmdJSRequestSet->bmdDatagramSetTable[i], &((*bmdJSResponseSet)->bmdDatagramSetTable[i]), req);

		/******************************************/
		/*	ustawienie statusu błędu datagramu	*/
		/******************************************/

		/*PR_bmdDatagram_PrepareResponse(&((*bmdJSResponseSet)->bmdDatagramSetTable[i]), bmdJSRequestSet->bmdDatagramSetTable[i]->datagramType+100, status);*/
		(*bmdJSResponseSet)->bmdDatagramSetTable[i]->datagramType = bmdJSRequestSet->bmdDatagramSetTable[i]->datagramType+100;
		(*bmdJSResponseSet)->bmdDatagramSetTable[i]->datagramStatus = status;

	}

	return BMD_OK;
}


/**
* @author WSz
* @brief Funkcja okreslajaca na podstawie zadania (ustawionych kryteriow), czy zliczanie ma byc wykonane na podstawie tabeli raportowej
* @param requestDatagram[in] struktura zadania zliczania dokumentow
* @retval 0 zliczanie ma byc wykonane na podstawie crypto_objects (standardowe)
* @retval 1 zliczanie ma byc wykonane na podstawie tabeli raportowej
* @retval wartosc_ujemna to kod bledu
*/
long IsReportModeCounting(bmdDatagram_t *requestDatagram)
{
long iter			= 0;
long insertDate_index		= -1;
long insertDateFrom_index	= -1;
long insertDateTo_index		= -1;
long otherMetadata		= 0;
long detectedOperatorOR		= 0;

	if(_GLOBAL_bmd_configuration == NULL) // zalozenie, ze enable_report_counting = 0
		{ return 0; }
	if(_GLOBAL_bmd_configuration->enable_report_counting == 0)
		{ return 0; }

	if(requestDatagram == NULL)
		{ BMD_FOK(BMD_ERR_PARAM1); }


	if(requestDatagram->no_of_additionalMetaData > 0)
		{ return 0; }
	if(requestDatagram->no_of_actionMetaData > 0)
		{ return 0; }
	if(requestDatagram->no_of_sysMetaData == 0 || requestDatagram->sysMetaData == NULL)
		{ return 0; }

	for(iter=0; iter<requestDatagram->no_of_sysMetaData; iter++)
	{
		if(requestDatagram->sysMetaData[iter] == NULL)
			{ continue; }
		if(requestDatagram->sysMetaData[iter]->OIDTableBuf == NULL)
			{ continue; }

		if(strcmp(OID_SYS_METADATA_DTG_SYMKEY_HASH, requestDatagram->sysMetaData[iter]->OIDTableBuf) == 0)
		{
			// ta metadana zawsze jest ustawiana, wiec trzeba ja pominac
			continue;
		}
		else if(strcmp(OID_SYS_METADATA_CRYPTO_OBJECTS_TIMESTAMP, requestDatagram->sysMetaData[iter]->OIDTableBuf) == 0) // konkretna data
		{
			if(requestDatagram->sysMetaData[iter]->ownerType >= BMD_REQ_DB_SELECT_ADVANCED_LOGICAL_OR &&
				requestDatagram->sysMetaData[iter]->ownerType < BMD_REQ_DB_SELECT_ADVANCED_LOGICAL_AND) // logical OR
			{
				detectedOperatorOR = 1;
				break;
			}
			if(insertDate_index >= 0) // powtorzone kryterium
			{
				BMD_FOK(BMD_ERR_OP_FAILED);
			}
			insertDate_index = iter;
		}
		else if(strcmp(OID_SYS_METADATA_CRYPTO_OBJECTS_TIMESTAMP_LESS, requestDatagram->sysMetaData[iter]->OIDTableBuf) == 0) // dolna granica przedzialu
		{
			if(requestDatagram->sysMetaData[iter]->ownerType >= BMD_REQ_DB_SELECT_ADVANCED_LOGICAL_OR &&
				requestDatagram->sysMetaData[iter]->ownerType < BMD_REQ_DB_SELECT_ADVANCED_LOGICAL_AND) // logical OR
			{
				detectedOperatorOR = 1;
				break;
			}
			if(insertDateFrom_index >= 0) // powtorzone kryterium
			{
				BMD_FOK(BMD_ERR_OP_FAILED);
			}
			insertDateFrom_index = iter;
		}
		else if(strcmp(OID_SYS_METADATA_CRYPTO_OBJECTS_TIMESTAMP_MORE, requestDatagram->sysMetaData[iter]->OIDTableBuf) == 0) // gorna granica przedzialu
		{
			if(requestDatagram->sysMetaData[iter]->ownerType >= BMD_REQ_DB_SELECT_ADVANCED_LOGICAL_OR &&
				requestDatagram->sysMetaData[iter]->ownerType < BMD_REQ_DB_SELECT_ADVANCED_LOGICAL_AND) // logical OR
			{
				detectedOperatorOR = 1;
				break;
			}
			if(insertDateTo_index >= 0) // powtorzone kryterium
			{
				BMD_FOK(BMD_ERR_OP_FAILED);
			}
			insertDateTo_index = iter;
		}
		else
		{
			//PRINT_ERROR("DEBUG oid metadanej= %s\n", requestDatagram->sysMetaData[iter]->OIDTableBuf);
			otherMetadata = 1;
			break;
		}
	}

	if(detectedOperatorOR != 0) // jesli uzyto operator OR, to zliczanie standardowe
		{ return 0; }

	if(otherMetadata == 0)
	{
		if(insertDate_index >= 0 && insertDateFrom_index < 0 && insertDateTo_index < 0) // konkretna data
			{ return 1; }
		else if(insertDate_index < 0 && (insertDateFrom_index >= 0 || insertDateTo_index >= 0) ) // przedzial a takze tylko dolna lub tylko gorna granica
			{ return 1; }
		else if(insertDate_index < 0 && insertDateFrom_index < 0 && insertDateTo_index < 0 ) // brak jakichkolwiek kryteriow
			{ return 1; } 
		else
			{ return 0; }
	}
	else
	{
		return 0;
	}

	return 0;
}


/**
* @author WSz
* @brief Funkcja wydobywajaca z zadania kryterium daty wstawienia (data albo przedzial)
* @param requestDatagram[in] struktura zadania zliczania dokumentow
* @param insertDate[out] wydobyta data (jesli okreslony przedzial, to ta wartosc bedzie NULL na wyjsciu)
* @param insertDateFrom[out] wydobyta dolna granica przedzialu daty wstawienia (jesli okreslona konkretna data, to ta wartosc bedzie NULL na wyjsciu)
* @param insertDateTo[out] wydobyta gorna granica przedzialu daty wstawienia (jesli okreslona konkretna data, to ta wartosc bedzie NULL na wyjsciu)
* @note Funkcja nie dopuszcza, aby w zadaniu byly okreslone jednoczesnie konkretna data i przedzial dat wstawienia - w takiej sytuacji zwroci blad.
*       Zglosi zas sukces (BMD_OK) przy zupelnym braku kryterium daty wstawienia.
* @return Zwraca BMD_OK lub wartosc ujemna (kod bledu).
*/
long GetInsertDateCriteria(bmdDatagram_t* requestDatagram, char** insertDate, char** insertDateFrom, char** insertDateTo)
{
long iter = 0;
char* insertDate_temp = NULL;
char* insertDateFrom_temp = NULL;
char* insertDateTo_temp = NULL;

	if(requestDatagram == NULL)
		{ BMD_FOK(BMD_ERR_PARAM1); }
	if(insertDate == NULL)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(*insertDate != NULL)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(insertDateFrom == NULL)
		{ BMD_FOK(BMD_ERR_PARAM3); }
	if(*insertDateFrom != NULL)
		{ BMD_FOK(BMD_ERR_PARAM3); }
	if(insertDateTo == NULL)
		{ BMD_FOK(BMD_ERR_PARAM4); }
	if(*insertDateTo != NULL)
		{ BMD_FOK(BMD_ERR_PARAM4); }

	if(requestDatagram->no_of_sysMetaData == 0) // brak kryterium
		{ return BMD_OK; }

	if(requestDatagram->no_of_sysMetaData > 0 && requestDatagram->sysMetaData == NULL)
		{ BMD_FOK(BMD_ERR_PARAM1); }
	

	for(iter=0; iter<requestDatagram->no_of_sysMetaData; iter++)
	{
		if(requestDatagram->sysMetaData[iter] == NULL)
			{ continue; }
		if(requestDatagram->sysMetaData[iter]->OIDTableBuf == NULL)
			{ continue; }
		if(requestDatagram->sysMetaData[iter]->AnyBuf == NULL)
			{ continue; }
		if(requestDatagram->sysMetaData[iter]->AnySize <= 0)
			{ continue; }

		if(strcmp(OID_SYS_METADATA_CRYPTO_OBJECTS_TIMESTAMP, requestDatagram->sysMetaData[iter]->OIDTableBuf) == 0) // konkretna data
		{
			if(insertDate_temp != NULL) // powtorzone kryterium
			{
				free(insertDate_temp); insertDate_temp = NULL;
				free(insertDateFrom_temp); insertDateFrom_temp = NULL;
				free(insertDateTo_temp); insertDateTo_temp = NULL;
				BMD_FOK(BMD_ERR_OP_FAILED);
			}
			insertDate_temp = strndup(requestDatagram->sysMetaData[iter]->AnyBuf, requestDatagram->sysMetaData[iter]->AnySize);
			if(insertDate_temp == NULL)
			{
				free(insertDateFrom_temp); insertDateFrom_temp = NULL;
				free(insertDateTo_temp); insertDateTo_temp = NULL;
				BMD_FOK(BMD_ERR_MEMORY);
			}
		}
		else if(strcmp(OID_SYS_METADATA_CRYPTO_OBJECTS_TIMESTAMP_LESS, requestDatagram->sysMetaData[iter]->OIDTableBuf) == 0) // dolna granica przedzialu
		{
			if(insertDateFrom_temp != NULL) // powtorzone kryterium
			{
				free(insertDate_temp); insertDate_temp = NULL;
				free(insertDateFrom_temp); insertDateFrom_temp = NULL;
				free(insertDateTo_temp); insertDateTo_temp = NULL;
				BMD_FOK(BMD_ERR_OP_FAILED);
			}
			insertDateFrom_temp = strndup(requestDatagram->sysMetaData[iter]->AnyBuf, requestDatagram->sysMetaData[iter]->AnySize);
			if(insertDateFrom_temp == NULL)
			{
				free(insertDate_temp); insertDate_temp = NULL;
				free(insertDateTo_temp); insertDateTo_temp = NULL;
				BMD_FOK(BMD_ERR_MEMORY);
			}
		}
		else if(strcmp(OID_SYS_METADATA_CRYPTO_OBJECTS_TIMESTAMP_MORE, requestDatagram->sysMetaData[iter]->OIDTableBuf) == 0) // gorna granica przedzialu
		{
			if(insertDateTo_temp != NULL) // powtorzone kryterium
			{
				free(insertDate_temp); insertDate_temp = NULL;
				free(insertDateFrom_temp); insertDateFrom_temp = NULL;
				free(insertDateTo_temp); insertDateTo_temp = NULL;
				BMD_FOK(BMD_ERR_OP_FAILED);
			}
			insertDateTo_temp = strndup(requestDatagram->sysMetaData[iter]->AnyBuf, requestDatagram->sysMetaData[iter]->AnySize);
			if(insertDateTo_temp == NULL)
			{
				free(insertDate_temp); insertDate_temp = NULL;
				free(insertDateFrom_temp); insertDateFrom_temp = NULL;
				BMD_FOK(BMD_ERR_MEMORY);
			}
		}
	}

	if(insertDate_temp != NULL && (insertDateFrom_temp != NULL || insertDateTo_temp != NULL) ) // nie moze byc konkretna data i przedzial jednoczesnie
	{
		free(insertDate_temp); insertDate_temp = NULL;
		free(insertDateFrom_temp); insertDateFrom_temp = NULL;
		free(insertDateTo_temp); insertDateTo_temp = NULL;
		BMD_FOK(BMD_ERR_OP_FAILED);
	}

	*insertDate = insertDate_temp; insertDate_temp = NULL;
	*insertDateFrom = insertDateFrom_temp; insertDateFrom_temp = NULL;
	*insertDateTo = insertDateTo_temp; insertDateTo_temp = NULL;

	return BMD_OK;
}


/**
* @author WSz
* @brief Funkcja generuje zapytanie SQL do zliczenia dokumentow na podstawie tabeli raportowej bmd_report_insert_docs.
* @param hDB[in] uchwyt do przygotowanego polaczenia bazodanowego
* @param requestDatagram[in] struktura zadania zliczania dokumentow
* @param req[in]
* @param query[out] wygenerowane zapytanie SQL
* @return BMD_OK albo wartosc ujemna (kod bledu)
*/
long GenerateReportCountQuery(void *hDB, bmdDatagram_t* requestDatagram, server_request_data_t *req, char** query)
{
long retVal = 0;
char* insertDate = NULL;
char* insertDateFrom = NULL;
char* insertDateTo = NULL;

// doc_type = 1 (dla widocznych)
// deleted = 0 (liczniki nieusunietych plikow)
char* queryBegin=  "SELECT sum(doc_cnt) FROM bmd_report_insert_docs WHERE deleted=0 AND doc_type='1' AND ";
char* ownerCondition = NULL;
char* groupsCondition = NULL;
char* dateCondition = NULL;


	if(hDB == NULL)
		{ BMD_FOK(BMD_ERR_PARAM1); }
	if(requestDatagram == NULL)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(req == NULL)
		{ BMD_FOK(BMD_ERR_PARAM3); }
	if(query == NULL)
		{ BMD_FOK(BMD_ERR_PARAM4); }
	if(*query != NULL)
		{ BMD_FOK(BMD_ERR_PARAM4); }

	// warunek na date wstawienia
	BMD_FOK(GetInsertDateCriteria(requestDatagram, &insertDate, &insertDateFrom, &insertDateTo));
	if(insertDateFrom != NULL && insertDateTo != NULL) // przedzial dat
	{
		retVal = asprintf(&dateCondition, "\n AND (insert_day >= '%s'::timestamp AND insert_day < '%s'::timestamp + '1 day'::interval) ", insertDateFrom, insertDateTo);
	}
	else if(insertDateFrom != NULL && insertDateTo == NULL) // tylko dolna granica przedzialu
	{
		retVal = asprintf(&dateCondition, "\n AND (insert_day >= '%s'::timestamp) ", insertDateFrom);
	}
	else if(insertDateFrom == NULL && insertDateTo != NULL) // tylko gorna granica przedzialu
	{
		retVal = asprintf(&dateCondition, "\n AND (insert_day < '%s'::timestamp + '1 day'::interval) ", insertDateTo);
	}
	else if(insertDate != NULL)// konkretna data
	{
		retVal = asprintf(&dateCondition, "\n AND (insert_day::text LIKE '%s%%' ESCAPE '/') ", insertDate);
	}
	else // zupelny brak kryterium
	{
		retVal = asprintf(&dateCondition, " ");
	}
	free(insertDate); insertDate = NULL;
	free(insertDateFrom); insertDateFrom = NULL;
	free(insertDateTo); insertDateTo = NULL;
	if(retVal == -1)
	{
		BMD_FOK(BMD_ERR_MEMORY);
	}


	// warunek na wlasciciela/grupe
	if(requestDatagram->datagramStatus & BMD_REQ_DB_SELECT_ALL_SYSMTD) // tylko dokumenty grupy uzytkownika (drzewa grup)
	{
		retVal = BuildSQLConditionPartForGroupsGraph(0/*ignoreDependsGroups*/, req, &groupsCondition);
		if(retVal < BMD_OK)
		{
			free(dateCondition); dateCondition = NULL;
			BMD_FOK(retVal);
		}

		if(groupsCondition != NULL) // wlaczonie sprawdzanie grup (w konfiguracji serwera)
		{
			retVal = bmd_str_replace(&groupsCondition, "crypto_objects", "bmd_report_insert_docs");
			if(retVal < BMD_OK)
			{
				free(groupsCondition); groupsCondition = NULL;
				free(dateCondition); dateCondition = NULL;
				BMD_FOK(retVal);
			}
		}
		else // wylaczonie sprawdzanie grup (w konfiguracji serwera)
		{
			groupsCondition = strdup(" 1=1 ");
			if(groupsCondition == NULL)
			{
				free(dateCondition); dateCondition = NULL;
				BMD_FOK(BMD_ERR_MEMORY);
			}
		}
	}
	else if(requestDatagram->datagramStatus & BMD_REQ_DB_SELECT_ONLY_OWNER) // tylko dokumenty uzytkownika (wlasciciela)
	{
		if(req->ua == NULL)
		{
			free(dateCondition); dateCondition = NULL;
			BMD_FOK(BMD_ERR_PARAM3);
		}
		if(req->ua->identityId == NULL)
		{
			free(dateCondition); dateCondition = NULL;
			BMD_FOK(BMD_ERR_PARAM3);
		}
		if(req->ua->identityId->buf == NULL || req->ua->identityId->size <= 0)
		{
			free(dateCondition); dateCondition = NULL;
			BMD_FOK(BMD_ERR_PARAM3);
		}
		
		retVal = asprintf(&ownerCondition, " AND fk_owner='%s' ", req->ua->identityId->buf);
		if(retVal == -1)
		{
			free(dateCondition); dateCondition = NULL;
			BMD_FOK(BMD_ERR_MEMORY);
		}
	}
	else
	{
		free(dateCondition); dateCondition = NULL;
		BMD_FOK(BMD_ERR_UNIMPLEMENTED);
	}

	retVal = asprintf(query, "%s %s %s %s", queryBegin, (groupsCondition!=NULL ? groupsCondition : "1=1"), (ownerCondition!=NULL ? ownerCondition : ""), dateCondition);
	free(groupsCondition); groupsCondition = NULL;
	free(ownerCondition); ownerCondition = NULL;
	free(dateCondition); dateCondition = NULL;
	if(retVal == -1)
	{
		*query = NULL;
		BMD_FOK(BMD_ERR_MEMORY);
	}

	return BMD_OK;
}


long JS_bmdDatagram_to_count(	void *hDB,
				bmdDatagram_t *bmdJSRequest,
				bmdDatagram_t **bmdJSResponse,
				server_request_data_t *req)
{
long retVal			= 0;
char *SQLQueryFull		= NULL;
char *ans			= NULL;
char *SelectLimits		= NULL;
char *SelectQueryColumns	= NULL;
char *SQLQueryInner		= NULL;
char *SQLQueryOuter		= NULL;
char *SQLQueryInner2		= NULL;
char *tmp			= NULL;


	/************************/
	/* walidacja parametrow */
	/************************/
	if(hDB == NULL)						{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(bmdJSRequest == NULL)				{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(bmdJSResponse == NULL && *bmdJSResponse != NULL)	{	BMD_FOK(BMD_ERR_PARAM3);	}

	/***************************/
	/* rozpoczynamy transakcje */
	/***************************/
	BMD_FOK(bmd_db_start_transaction(hDB,BMD_TRANSACTION_LEVEL_SERIALIZABLE));

	retVal = IsReportModeCounting(bmdJSRequest);
	BMD_FOK_TRANS(retVal);
	if(retVal == 1) // zliczanie na podstawie tabeli raportowej
	{
		BMD_FOK_TRANS(GenerateReportCountQuery(hDB, bmdJSRequest, req, &SQLQueryOuter));
	}
	else // zliczanie na podstawie crypto_objects (standardowe)
	{

		/******************************************/
		/* wygenerowanie stringa z zapytaniem SQL */
		/******************************************/
		BMD_FOK_TRANS(GenerateSearchQuery( hDB, bmdJSRequest, SEARCH_RESULT_COUNT, 0, &SQLQueryFull, &SelectLimits, &SelectQueryColumns, req));
	
		asprintf(&SQLQueryInner, "%s", SQLQueryFull);
		if (SQLQueryInner==NULL)	{	BMD_FOK(NO_MEMORY);	}
	
		asprintf(&SQLQueryOuter, "%s", SQLQueryFull);
		if (SQLQueryOuter==NULL)	{	BMD_FOK(NO_MEMORY);	}
	
		BMD_FOK_TRANS(bmd_str_replace(&SQLQueryInner, SelectQueryColumns, "SELECT DISTINCT crypto_objects.id FROM crypto_objects\n"));
		BMD_FOK_TRANS(bmd_str_replace(&SQLQueryInner, "<first_step>", "AND ( crypto_objects.file_type = 'link')"));
		BMD_FOK_TRANS(bmd_str_replace(&SQLQueryInner, SelectLimits, "\n"));
	
		asprintf(&SQLQueryInner2, "%s", SQLQueryInner);	if (SQLQueryInner2==NULL)	{	BMD_FOK(NO_MEMORY); }
		BMD_FOK_TRANS(bmd_str_replace(&SQLQueryInner2, "<second_step>", ""));
		BMD_FOK_TRANS(bmd_str_replace(&SQLQueryInner2, "AND ( crypto_objects.file_type = 'link')", "AND ( crypto_objects.file_type <> 'link')"));
		asprintf(&tmp, "AND (crypto_objects.pointing_id IN (\n\n%s\n\t)\n)", SQLQueryInner2); if (tmp==NULL)	{	BMD_FOK(NO_MEMORY);	}
		BMD_FOK_TRANS(bmd_str_replace(&SQLQueryInner, "<second_step>", tmp));
		free0(tmp);
	
	
		asprintf(&SQLQueryOuter, "%s", SQLQueryFull);	if (SQLQueryOuter==NULL)	{	BMD_FOK(NO_MEMORY);	}
		BMD_FOK_TRANS(bmd_str_replace(&SQLQueryOuter, "<first_step>", ""));
	
		asprintf(&tmp, "AND (crypto_objects.id NOT IN (\n%s\n)\n)", SQLQueryInner); if (tmp==NULL)	{	BMD_FOK(NO_MEMORY);	}
	
		BMD_FOK_TRANS(bmd_str_replace(&SQLQueryOuter, "<second_step>", tmp));
	
		free0(tmp);
		free0(SelectLimits);
		free0(SelectQueryColumns);
	}
	/*************************************************/
	/* wyslanie zaptania zliczajacego do bazy danych */
	/*************************************************/
	BMD_FOK_TRANS(ExecuteSQLQueryWithAnswerKnownDBConnection( hDB, SQLQueryOuter, &ans));
	free(SQLQueryOuter); SQLQueryOuter = NULL;

	/**********************************************************/
	/* przepisanie zwroconego wyniku do datagaramu odpowiedzi */
	/**********************************************************/
	if (ans!=NULL)
	{
		(*bmdJSResponse)->filesRemaining=atoi(ans);
	}

	/***********************/
	/* konczymy transakcje */
	/***********************/
	bmd_db_end_transaction(hDB);

	/************/
	/* porzadki */
	/************/
	free0(SQLQueryFull);
	free0(ans);
	free0(SelectLimits);
	free0(SelectQueryColumns);

	return BMD_OK;
}
