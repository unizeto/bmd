#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdsql/common.h>


/**
@author WSz
@brief Funkcja buduje wielokrotnie zlozony warunek OR dla wartosci ze wskazanej kolumny wynikow
@param hDB[in] uchwyt do polaczenia bazodanowego
@param queryResult[in] wyniki zapytania, na podstawie ktorych ma byc budowany warunek
@param rowsCount[in] ilosc wierszy wyniku queryResult (jesli mniej niz jest, to tylko tyle bedzie przetworzone)
@param whichColumn[in] indeks (od 0) kolumny wynikow, ktorej wartosci posluza do zbudowania warunku
@param conditionColumnName[in] nazwa kolumny tabeli , dla ktorej budowany warunek na wartosci
@param condition[out] zbudowany warunek
@return BMD_OK lub wartosc ujemna w przypadku bledu
@note jesli 0 wierszy wyniku to condition pozostaje NULL
*/
long generateMultiORCondition(void* hDB, void* queryResult, long rowsCount, long whichColumn, char* conditionColumnName, char** condition)
{
long retVal = 0;
long iter = 0;
char* conditionTemp = NULL;
char* stringTemp = NULL;
char* escapedTemp = NULL;

db_row_strings_t* rowStruct = NULL;
long fetched = 0;

	if(hDB == NULL)
		{ BMD_FOK(BMD_ERR_PARAM1); }
	if(queryResult == NULL)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(rowsCount < 0)
		{ BMD_FOK(BMD_ERR_PARAM3); }
	if(whichColumn < 0)
		{ BMD_FOK(BMD_ERR_PARAM4); }
	if(conditionColumnName == NULL)
		{ BMD_FOK(BMD_ERR_PARAM5); }
	if(strlen(conditionColumnName) <= 0)
		{ BMD_FOK(BMD_ERR_PARAM5); }
	if(condition == NULL)
		{ BMD_FOK(BMD_ERR_PARAM6); }
	if(*condition != NULL)
		{ BMD_FOK(BMD_ERR_PARAM6); }

	for(iter=0; iter<rowsCount; iter++)
	{
		retVal = bmd_db_result_get_row(hDB, queryResult, iter , 0, &rowStruct, FETCH_ABSOLUTE, &fetched);
		if(retVal < BMD_OK)
		{
			free(conditionTemp); conditionTemp = NULL;
			BMD_FOK(BMD_ERR_OP_FAILED);
		}

		if(whichColumn >= rowStruct->size)
		{
			bmd_db_row_struct_free(&rowStruct);
			free(conditionTemp); conditionTemp = NULL;
			BMD_FOK(BMD_ERR_PARAM4);
		}

		if(rowStruct->colvals[whichColumn] == NULL)
		{
			bmd_db_row_struct_free(&rowStruct);
			free(conditionTemp); conditionTemp = NULL;
			BMD_FOK(BMD_ERR_OP_FAILED);
		}

		retVal = bmd_db_escape_string(hDB, rowStruct->colvals[whichColumn], STANDARD_ESCAPING, &escapedTemp);
		if(retVal < BMD_OK)
		{
			bmd_db_row_struct_free(&rowStruct);
			free(conditionTemp); conditionTemp = NULL;
			BMD_FOK(BMD_ERR_OP_FAILED);
		}

		if(conditionTemp == NULL)
		{
			retVal = asprintf(&conditionTemp ,"%s='%s'", conditionColumnName, escapedTemp);
			if(retVal == -1)
			{
				bmd_db_row_struct_free(&rowStruct);
				free(escapedTemp); escapedTemp = NULL;
				BMD_FOK(BMD_ERR_MEMORY);
			}
		}
		else
		{
			retVal = asprintf(&stringTemp, "%s OR %s='%s'", conditionTemp, conditionColumnName, escapedTemp);
			free(conditionTemp);
			conditionTemp = stringTemp;
			stringTemp = NULL;
			if(retVal == -1)
			{
				bmd_db_row_struct_free(&rowStruct);
				free(escapedTemp); escapedTemp = NULL;
				BMD_FOK(BMD_ERR_MEMORY);
			}
		}

		free(escapedTemp); escapedTemp = NULL;
		bmd_db_row_struct_free(&rowStruct);
	}

	*condition = conditionTemp;
	conditionTemp = NULL;

	return BMD_OK;	
}


/**
* @author WSz
* @param hDB[in] uchwyt do przygotowanego polaczenia bazodanowego
* @param bmdJSRequest[in] datagram zadania
* @param bmdJSResponseSet[out] przygotowywana odpowiedz na zadanie (przypisana tylko, gdy wywolanie funkcji zakonczone sukcesem)
* @param req[in] struktura zadania
* @return BMD_OK albo wartosc ujemna w przypadku bledu
*/
long JS_bmdDatagram_GetActionsHistory(void *hDB, bmdDatagram_t *bmdJSRequest, bmdDatagramSet_t **bmdJSResponseSet, server_request_data_t *req)
{
char* CryptoObjectNr = NULL;
long cryptoObjectNr_long = 0;
char* file_type = NULL;
bmdDatagram_t* tmpDatagram = NULL;
bmdDatagramSet_t* tmpDatagramSet = NULL;
const char* selectSigTemplate = "SELECT id FROM prf_signatures WHERE fk_crypto_objects='%s' AND location_id=%s ";
const char* templateINpart = "SELECT id FROM crypto_objects WHERE pointing_id='%s' AND location_id=%s AND file_type<>'dir' AND present=1 AND (transaction_id=NULL OR transaction_id='') ";

const char* selectTemplate = "SELECT id, location_id, fk_objects, objects_type, fk_actions, fk_users, fk_users_class, status, status_msg, action_date FROM action_register WHERE \n\
(\n\
\t(fk_objects IN \n\
\t\t(%s)\n\
\tAND objects_type=%i) \n\
\t%s\n\
)\n\
AND location_id=%s ORDER BY action_date ";

char *sqlInPart = NULL;
char* sqlQuery = NULL;
long rowsCount = 0;
void *queryResult = NULL;
db_row_strings_t* rowStruct = NULL;
long iter = 0;
long retVal = 0;
long fetched = 0;
char* className = NULL;
char* actionName = NULL;
char* identityName = NULL;
char* sigORCondition = NULL;
char* sigFullCondition = NULL;
EnumActionRegisterObjectType_t objectType = UNDEFINED_OBJECT_TYPE;

	if(hDB == NULL)
		{ BMD_FOK(BMD_ERR_PARAM1); }
	if(bmdJSRequest == NULL)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(bmdJSRequest->protocolDataFileId == NULL)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(bmdJSResponseSet == NULL)
		{ BMD_FOK(BMD_ERR_PARAM3); }
	if(*bmdJSResponseSet != NULL)
		{ BMD_FOK(BMD_ERR_PARAM3); }
	if(_GLOBAL_bmd_configuration == NULL)
		{ BMD_FOK(BMD_ERR_OP_FAILED); }


	BMD_FOK(VerifyPermissionsForCryptoObject(hDB, req, bmdJSRequest->protocolDataFileId->buf, 
				CONSIDER_CREATOR_OR_OWNER_OR_GROUPS,
				NULL/*&file_name*/,
				&file_type,
				&CryptoObjectNr,
				NULL/*correspondingId*/,
				NULL/*forGrant*/,
				NULL/*&addMetadataCount*/,
				NULL/*&addMetadata*/));

	// wyszukanie podpisow
	retVal = asprintf(&sqlQuery, selectSigTemplate, CryptoObjectNr, _GLOBAL_bmd_configuration->location_id);
	if(retVal == -1)
	{
		free(file_type); file_type = NULL;
		free(CryptoObjectNr); CryptoObjectNr = NULL;
		BMD_FOK(BMD_ERR_MEMORY);
	}

	retVal = bmd_db_execute_sql(hDB, sqlQuery, &rowsCount, NULL, &queryResult);
	free(sqlQuery); sqlQuery = NULL;
	if(retVal < BMD_OK)
	{
		free(file_type); file_type = NULL;
		free(CryptoObjectNr); CryptoObjectNr = NULL;
		BMD_FOK(retVal);
	}
	
	// skladanie warunku dla podpisow
	retVal = generateMultiORCondition(hDB, queryResult, rowsCount, 0, "fk_objects", &sigORCondition);
	bmd_db_result_free(&queryResult);
	rowsCount = 0;
	if(retVal < BMD_OK)
	{
		free(file_type); file_type = NULL;
		free(CryptoObjectNr); CryptoObjectNr = NULL;
		BMD_FOK(retVal);
	}

	if(sigORCondition == NULL) // jesli nie ma podpisow
	{
		sigFullCondition = strdup("");
		if(sigFullCondition == NULL)
		{
			free(file_type); file_type = NULL;
			free(CryptoObjectNr); CryptoObjectNr = NULL;
			BMD_FOK(BMD_ERR_MEMORY);
		}
	}
	else // jesli sa podpisy
	{
		retVal = asprintf(&sigFullCondition, "OR ((%s) AND objects_type=%i)", sigORCondition, SIGNATURE_OBJECT_TYPE);
		free(sigORCondition); sigORCondition = NULL;
		if(retVal == -1)
		{
			free(file_type); file_type = NULL;
			free(CryptoObjectNr); CryptoObjectNr = NULL;
			BMD_FOK(BMD_ERR_MEMORY);
		}
	}

	// skladanie zapytania zagniezdzonego w warunku IN
	retVal = asprintf(&sqlInPart, templateINpart, CryptoObjectNr, _GLOBAL_bmd_configuration->location_id);
	if(retVal == -1)
	{
		free(sigFullCondition); sigFullCondition = NULL;
		free(file_type); file_type = NULL;
		free(CryptoObjectNr); CryptoObjectNr = NULL;
		BMD_FOK(BMD_ERR_MEMORY);
	}

	// skladanie calosci zapytania
	retVal = asprintf(&sqlQuery, selectTemplate, sqlInPart, CRYPTO_OBJECT_OBJECT_TYPE, sigFullCondition, _GLOBAL_bmd_configuration->location_id);
	free(sigFullCondition); sigFullCondition = NULL;
	free(sqlInPart); sqlInPart = NULL;
	if(retVal == -1)
	{
		free(file_type); file_type = NULL;
		free(CryptoObjectNr); CryptoObjectNr = NULL;
		BMD_FOK(BMD_ERR_MEMORY);
	}

	retVal = bmd_db_execute_sql(hDB, sqlQuery, &rowsCount, NULL, &queryResult);
	free(sqlQuery); sqlQuery = NULL;
	if(retVal < BMD_OK)
	{
		free(file_type); file_type = NULL;
		free(CryptoObjectNr); CryptoObjectNr = NULL;
		BMD_FOK(retVal);
	}
	
	// przygotowanie odpowiedzi na zadanie na podstawie uzyskanych wynikow zapytania (1 wpis z historii akcji = 1 datagram odpowiedzi)
	cryptoObjectNr_long = strtol(CryptoObjectNr, NULL, 10);
	for(iter=0; iter<rowsCount; iter++)
	{
		retVal = bmd_db_result_get_row(hDB, queryResult, iter , 0, &rowStruct, FETCH_ABSOLUTE, &fetched);
		if(retVal < BMD_OK)
		{
			bmd_db_result_free(&queryResult);
			free(file_type); file_type = NULL;
			free(CryptoObjectNr); CryptoObjectNr = NULL;
			PR2_bmdDatagramSet_free(&tmpDatagramSet);
			BMD_FOK(retVal);
		}

		// nazwa tozsamosci ze slownika uzytkownikow
		retVal = getElementWithCondition(_GLOBAL_UsersDictionary, 2, rowStruct->colvals[5], 7, &identityName);
		if(retVal < BMD_OK)
		{
			PRINT_ERROR("WARNING: unable to determine identity name (%li)\n", retVal);
		}
		
		// nazwa klasy uzytkownika ze slownika klas
		retVal = getElementWithCondition(_GLOBAL_ClassesDictionary, 0, rowStruct->colvals[6], 1, &className);
		if(retVal < BMD_OK)
		{
			PRINT_ERROR("WARNING: unable to determine class name (%li)\n", retVal);
		}
	
		// nazwa akcji ze slownika akcji
		//SELECT id, name, registration, notification FROM actions
		retVal = getElementWithCondition(_GLOBAL_ActionsDictionary, 0, rowStruct->colvals[4], 1, &actionName);
		if(retVal < BMD_OK)
		{
			PRINT_ERROR("WARNING: unable to determine action (%li)\n", retVal);
		}


		retVal = bmd_datagram_create(BMD_DATAGRAM_TYPE_GET_ACTIONS_HISTORY_RESPONSE, &tmpDatagram);
		if(retVal < BMD_OK)
		{
			free(identityName); identityName = NULL;
			free(actionName); actionName = NULL;
			free(className); className = NULL;
			bmd_db_row_struct_free(&rowStruct);
			bmd_db_result_free(&queryResult);
			free(file_type); file_type = NULL;
			free(CryptoObjectNr); CryptoObjectNr = NULL;
			PR2_bmdDatagramSet_free(&tmpDatagramSet);
			BMD_FOK(retVal);
		}

		// ustawienie id oryginalu dokumentu (czyli wskazywanego w przypadku linku)
		retVal = bmd_datagram_set_id(&cryptoObjectNr_long, &tmpDatagram);
		if(retVal < BMD_OK)
		{
			PR2_bmdDatagram_free(&tmpDatagram);
			free(identityName); identityName = NULL;
			free(actionName); actionName = NULL;
			free(className); className = NULL;
			bmd_db_row_struct_free(&rowStruct);
			bmd_db_result_free(&queryResult);
			free(file_type); file_type = NULL;
			free(CryptoObjectNr); CryptoObjectNr = NULL;
			PR2_bmdDatagramSet_free(&tmpDatagramSet);
			BMD_FOK(retVal);
		}

		// ustawienie nazwy tozsamosci uzytkownika wykonujacego akcje
		retVal = bmd_datagram_add_metadata_char(OID_SYS_METADATA_CERT_USER_IDENTITY_DESCRIPTION, (identityName != NULL ? identityName : ""), &tmpDatagram);
		free(identityName); identityName = NULL;
		if(retVal < BMD_OK)
		{
			PR2_bmdDatagram_free(&tmpDatagram);
			free(actionName); actionName = NULL;
			free(className); className = NULL;
			bmd_db_row_struct_free(&rowStruct);
			bmd_db_result_free(&queryResult);
			free(file_type); file_type = NULL;
			free(CryptoObjectNr); CryptoObjectNr = NULL;
			PR2_bmdDatagramSet_free(&tmpDatagramSet);
			BMD_FOK(retVal);
		}

		// ustawienie nazwy wykonanej akcji
		retVal = bmd_datagram_add_metadata_char(OID_ACTION_NAME, (actionName != NULL ? actionName : ""), &tmpDatagram);
		free(actionName); actionName = NULL;
		if(retVal < BMD_OK)
		{
			PR2_bmdDatagram_free(&tmpDatagram);
			free(className); className = NULL;
			bmd_db_row_struct_free(&rowStruct);
			bmd_db_result_free(&queryResult);
			free(file_type); file_type = NULL;
			free(CryptoObjectNr); CryptoObjectNr = NULL;
			PR2_bmdDatagramSet_free(&tmpDatagramSet);
			BMD_FOK(retVal);
		}

		// ustawienie nazwy klasy uzytkownika wykonujacego akcje
		retVal = bmd_datagram_add_metadata_char(OID_ACTION_METADATA_CERT_CLASS_NAME, (className != NULL ? className : ""), &tmpDatagram);
		free(className); className = NULL;
		if(retVal < BMD_OK)
		{
			PR2_bmdDatagram_free(&tmpDatagram);
			bmd_db_row_struct_free(&rowStruct);
			bmd_db_result_free(&queryResult);
			free(file_type); file_type = NULL;
			free(CryptoObjectNr); CryptoObjectNr = NULL;
			PR2_bmdDatagramSet_free(&tmpDatagramSet);
			BMD_FOK(retVal);
		}

		// ustawienie id obiektu, na ktorym wykonano akcje
		retVal = bmd_datagram_add_metadata_char(OID_ACTION_METADATA_OBJECT_ID, rowStruct->colvals[2], &tmpDatagram);
		if(retVal < BMD_OK)
		{
			PR2_bmdDatagram_free(&tmpDatagram);
			bmd_db_row_struct_free(&rowStruct);
			bmd_db_result_free(&queryResult);
			free(file_type); file_type = NULL;
			free(CryptoObjectNr); CryptoObjectNr = NULL;
			PR2_bmdDatagramSet_free(&tmpDatagramSet);
			BMD_FOK(retVal);
		}

		// ustawienie typu object, na ktorym wykonano akcje (numeryki odpowiadajace plik, podpis)
		retVal = bmd_datagram_add_metadata_char(OID_ACTION_METADATA_OBJECT_TYPE, rowStruct->colvals[3], &tmpDatagram);
		if(retVal < BMD_OK)
		{
			PR2_bmdDatagram_free(&tmpDatagram);
			bmd_db_row_struct_free(&rowStruct);
			bmd_db_result_free(&queryResult);
			free(file_type); file_type = NULL;
			free(CryptoObjectNr); CryptoObjectNr = NULL;
			PR2_bmdDatagramSet_free(&tmpDatagramSet);
			BMD_FOK(retVal);
		}

		// ustawienie id wykonywanej akcji
		retVal = bmd_datagram_add_metadata_char(OID_ACTION_METADATA_ACTION, rowStruct->colvals[4], &tmpDatagram);
		if(retVal < BMD_OK)
		{
			PR2_bmdDatagram_free(&tmpDatagram);
			bmd_db_row_struct_free(&rowStruct);
			bmd_db_result_free(&queryResult);
			free(file_type); file_type = NULL;
			free(CryptoObjectNr); CryptoObjectNr = NULL;
			PR2_bmdDatagramSet_free(&tmpDatagramSet);
			BMD_FOK(retVal);
		}

		// ustawienie id tozsamosci uzytkownika wykonujacego akcje
		retVal = bmd_datagram_add_metadata_char(OID_ACTION_METADATA_USER, rowStruct->colvals[5], &tmpDatagram);
		if(retVal < BMD_OK)
		{
			PR2_bmdDatagram_free(&tmpDatagram);
			bmd_db_row_struct_free(&rowStruct);
			bmd_db_result_free(&queryResult);
			free(file_type); file_type = NULL;
			free(CryptoObjectNr); CryptoObjectNr = NULL;
			PR2_bmdDatagramSet_free(&tmpDatagramSet);
			BMD_FOK(retVal);
		}

		// ustawienie id klasy uzytkownika wykonujacego akcje
		retVal = bmd_datagram_add_metadata_char(OID_ACTION_METADATA_USER_CLASS, rowStruct->colvals[6], &tmpDatagram);
		if(retVal < BMD_OK)
		{
			PR2_bmdDatagram_free(&tmpDatagram);
			bmd_db_row_struct_free(&rowStruct);
			bmd_db_result_free(&queryResult);
			free(file_type); file_type = NULL;
			free(CryptoObjectNr); CryptoObjectNr = NULL;
			PR2_bmdDatagramSet_free(&tmpDatagramSet);
			BMD_FOK(retVal);
		}

		// ustawienie statusu (wyniku) akcji
		retVal = bmd_datagram_add_metadata_char(OID_ACTION_METADATA_STATUS, rowStruct->colvals[7], &tmpDatagram);
		if(retVal < BMD_OK)
		{
			PR2_bmdDatagram_free(&tmpDatagram);
			bmd_db_row_struct_free(&rowStruct);
			bmd_db_result_free(&queryResult);
			free(file_type); file_type = NULL;
			free(CryptoObjectNr); CryptoObjectNr = NULL;
			PR2_bmdDatagramSet_free(&tmpDatagramSet);
			BMD_FOK(retVal);
		}

		// ustawienie opisu statusu wykonanej akcji
		retVal = bmd_datagram_add_metadata_char(OID_ACTION_METADATA_STATUS_MESSAGE, rowStruct->colvals[8], &tmpDatagram);
		if(retVal < BMD_OK)
		{
			PR2_bmdDatagram_free(&tmpDatagram);
			bmd_db_row_struct_free(&rowStruct);
			bmd_db_result_free(&queryResult);
			free(file_type); file_type = NULL;
			free(CryptoObjectNr); CryptoObjectNr = NULL;
			PR2_bmdDatagramSet_free(&tmpDatagramSet);
			BMD_FOK(retVal);
		}

		// ustawienie daty wykonania akcji
		retVal = bmd_datagram_add_metadata_char(OID_ACTION_METADATA_DATE, rowStruct->colvals[9], &tmpDatagram);
		if(retVal < BMD_OK)
		{
			PR2_bmdDatagram_free(&tmpDatagram);
			bmd_db_row_struct_free(&rowStruct);
			bmd_db_result_free(&queryResult);
			free(file_type); file_type = NULL;
			free(CryptoObjectNr); CryptoObjectNr = NULL;
			PR2_bmdDatagramSet_free(&tmpDatagramSet);
			BMD_FOK(retVal);
		}

		// ustawienie typu dokumentu (dokumentu, na bazie ktorego wykonanno akcje) - metadana systemowa !
		if(strcmp(rowStruct->colvals[2], CryptoObjectNr) == 0) // dokument
		{
			retVal = bmd_datagram_add_metadata_char(OID_SYS_METADATA_CRYPTO_OBJECTS_FILE_TYPE, file_type, &tmpDatagram);
			if(retVal < BMD_OK)
			{
				PR2_bmdDatagram_free(&tmpDatagram);
				bmd_db_row_struct_free(&rowStruct);
				bmd_db_result_free(&queryResult);
				free(file_type); file_type = NULL;
				free(CryptoObjectNr); CryptoObjectNr = NULL;
				PR2_bmdDatagramSet_free(&tmpDatagramSet);
				BMD_FOK(retVal);
			}
		}
		else
		{
			objectType = strtol(rowStruct->colvals[3], NULL, 10);
			if(objectType == SIGNATURE_OBJECT_TYPE) // podpis
			{
				retVal = bmd_datagram_add_metadata_char(OID_SYS_METADATA_CRYPTO_OBJECTS_FILE_TYPE, "signature", &tmpDatagram);
				if(retVal < BMD_OK)
				{
					PR2_bmdDatagram_free(&tmpDatagram);
					bmd_db_row_struct_free(&rowStruct);
					bmd_db_result_free(&queryResult);
					free(file_type); file_type = NULL;
					free(CryptoObjectNr); CryptoObjectNr = NULL;
					PR2_bmdDatagramSet_free(&tmpDatagramSet);
					BMD_FOK(retVal);
				}
			}
			else // link do dokumentu
			{
				retVal = bmd_datagram_add_metadata_char(OID_SYS_METADATA_CRYPTO_OBJECTS_FILE_TYPE, "link", &tmpDatagram);
				if(retVal < BMD_OK)
				{
					PR2_bmdDatagram_free(&tmpDatagram);
					bmd_db_row_struct_free(&rowStruct);
					bmd_db_result_free(&queryResult);
					free(file_type); file_type = NULL;
					free(CryptoObjectNr); CryptoObjectNr = NULL;
					PR2_bmdDatagramSet_free(&tmpDatagramSet);
					BMD_FOK(retVal);
				}
			}
		}

		bmd_db_row_struct_free(&rowStruct);

		retVal = bmd_datagram_add_to_set(tmpDatagram, &tmpDatagramSet);
		if(retVal < BMD_OK)
		{
			PR2_bmdDatagram_free(&tmpDatagram);
			bmd_db_result_free(&queryResult);
			free(file_type); file_type = NULL;
			free(CryptoObjectNr); CryptoObjectNr = NULL;
			PR2_bmdDatagramSet_free(&tmpDatagramSet);
			BMD_FOK(retVal);
		}
		tmpDatagram = NULL;
	}

	free(file_type); file_type = NULL;
	free(CryptoObjectNr); CryptoObjectNr = NULL;
	bmd_db_result_free(&queryResult);

	if(rowsCount == 0)
	{
		BMD_FOK(bmd_datagram_create(BMD_DATAGRAM_TYPE_GET_ACTIONS_HISTORY_RESPONSE, &tmpDatagram));

		retVal = bmd_datagram_set_id(&cryptoObjectNr_long, &tmpDatagram);
		if(retVal < BMD_OK)
		{
			PR2_bmdDatagram_free(&tmpDatagram);
			BMD_FOK(retVal);
		}

		retVal = bmd_datagram_add_to_set(tmpDatagram, &tmpDatagramSet);
		if(retVal < BMD_OK)
		{
			PR2_bmdDatagram_free(&tmpDatagram);
			BMD_FOK(retVal);
		}
		tmpDatagram = NULL;
	}

	*bmdJSResponseSet = tmpDatagramSet;
	tmpDatagramSet = NULL;

	return BMD_OK;
}


/**
* @author WSz
* @param hDB[in] uchwyt do przygotowanego polaczenia bazodanowego
* @param bmdJSRequestSet[in] zadanie
* @param bmdJSResponseSet[out] przygotowana odpowiedz na zadanie (przypisana tylko, gdy wywolanie funkcji zakonczone sukcesem)
* @param req[in] struktura zadania
* @return BMD_OK albo wartosc ujemna w przypadku bledu
*/
long JS_bmdDatagramSet_GetActionsHistory(void *hDB, bmdDatagramSet_t *bmdJSRequestSet, bmdDatagramSet_t **bmdJSResponseSet, server_request_data_t *req)
{
long status					= 0;
long retVal					= 0;
bmdDatagramSet_t *tmpResponseSet		= NULL;

	PRINT_INFO("LIBBMDSQLINF Getting history of actions\n");

	if(hDB == NULL)
		{ BMD_FOK(BMD_ERR_PARAM1); }
	if(bmdJSRequestSet == NULL)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	// obsluga tylko pojedynczego zadania
	if(bmdJSRequestSet->bmdDatagramSetSize != 1)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(bmdJSRequestSet->bmdDatagramSetTable == NULL)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(bmdJSRequestSet->bmdDatagramSetTable[0] == NULL)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(bmdJSResponseSet == NULL)
		{ BMD_FOK(BMD_ERR_PARAM3); }
	if(*bmdJSResponseSet != NULL)
		{ BMD_FOK(BMD_ERR_PARAM3); }

	 
	// sprawdzenie, czy wczesniej nie wykryto bledu w datagramie zadania
	BMD_FOK(bmdJSRequestSet->bmdDatagramSetTable[0]->datagramStatus);
	

	retVal = JS_bmdDatagram_GetActionsHistory(hDB, bmdJSRequestSet->bmdDatagramSetTable[0], &tmpResponseSet, req);
	
	// zarejestrowanie akcji w bazie danych
	if(retVal < BMD_OK)
	{
		/* to byloby powielone, bo blad propagowany jest wyzej do JS_main() i z JS_main() - a wtedy makro sprawdzajace wywoluje bmd_messaging_plugin_register_error()
		if(_GLOBAL_bmd_configuration != NULL && _GLOBAL_bmd_configuration->messaging_plugin_conf.bmd_messaging_plugin_register_error != NULL)
		{
			status = _GLOBAL_bmd_configuration->messaging_plugin_conf.bmd_messaging_plugin_register_error(hDB, retVal, bmdJSRequestSet, req);
			if(status < BMD_OK)
			{
				PRINT_ERROR("Error: unable to register action success\n");
			}
		}
		*/
		BMD_FOK(retVal);
	}
	else
	{
		if(_GLOBAL_bmd_configuration != NULL && _GLOBAL_bmd_configuration->messaging_plugin_conf.bmd_messaging_plugin_register_action != NULL)
		{
			status = _GLOBAL_bmd_configuration->messaging_plugin_conf.bmd_messaging_plugin_register_action(hDB, bmdJSRequestSet->bmdDatagramSetTable[0], tmpResponseSet->bmdDatagramSetTable[0], req);
			if(status < BMD_OK)
			{
				PRINT_ERROR("Error: unable to register action failure\n");
			}
		}
	}

	*bmdJSResponseSet = tmpResponseSet;
	tmpResponseSet = NULL;

	return BMD_OK;
}
