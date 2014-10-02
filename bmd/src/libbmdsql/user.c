#include <bmd/libbmdsql/user.h>
#include <bmd/libbmdsql/role.h>
#include <bmd/libbmdsql/group.h>
#include <bmd/libbmdsql/class.h>
#include <bmd/libbmdsql/security.h>
#include <bmd/libbmdglobals/libbmdglobals.h>
#include <signal.h>
#ifndef WIN32
	#include <sys/wait.h>
#endif /* ifndef WIN32 */

#ifdef WIN32
#pragma warning(disable:4204)
#pragma warning(disable:4100)
#pragma warning(disable:4127)
#endif

long CheckUserNumber()
{
long userCount	= 0;

	if (_GLOBAL_bmd_configuration->max_registered_users>0)
	{
		getCountWithCondition(	_GLOBAL_UsersDictionary, 0, NULL, &userCount);

		if (userCount>=_GLOBAL_bmd_configuration->max_registered_users)
		{
			return LIBBMDSQL_USER_TOO_MANY_REGISTERED_USERS;
		}
	}

	return BMD_OK;
}

/**
@author WSz
@brief Funkcja sprawdza, czy mozna zarejestrowac jeszcze tozsamosc (w odniesieniu do limitu w licencji)
@note Funkcja wykonuje zapytanie na bazie, aby zliczyc ilosc istniejacych tozsamosci. 
@param hDB[in] uchwyt do przygotowanego polaczenia bazodanowego
@retval 0 jesli nie mozna zarejestrowac wiecej tozsamosci (jest dokladnie tyle, na ile pozwala licencja)
@retval 1 jesli mozna jeszcze zarejestrowac tozsamosc
@retval LIBBMDSQL_USER_TOO_MANY_REGISTERED_IDENTITIES, jesli zarejestrowych wiecej tozsamosci, niz pozwala licencja
@retval inna_wartosc_ujemna (dla innego bledu)
*/
long IsPossibleIdentitityRegistration(void* hDB)
{
char* sqlQuery = "SELECT count(id) FROM identities";
char* countString = NULL;
long count = 0;

	if(hDB == NULL)
		{ BMD_FOK(BMD_ERR_PARAM1); }
	if(_GLOBAL_bmd_configuration == NULL)
		{ BMD_FOK(BMD_ERR_OP_FAILED); }


	if(_GLOBAL_bmd_configuration->max_registered_identities <= 0) // brak ograniczenia
	{
		return 1;
	}

	BMD_FOK(ExecuteSQLQueryWithAnswerKnownDBConnection(hDB, sqlQuery, &countString));
	count = strtoul(countString, NULL, 10);
	free(countString); countString = NULL;

	if(count > _GLOBAL_bmd_configuration->max_registered_identities)
	{
		BMD_FOK(LIBBMDSQL_USER_TOO_MANY_REGISTERED_IDENTITIES);
	}
	else if(count == _GLOBAL_bmd_configuration->max_registered_identities)
	{
		return 0;
	}
	else
	{
		return 1;
	}

	return 0;
}

/****************************************/
/* sprawdzenie, czy uzytkownik istnieje */
/****************************************/
long CheckIfUserExist(	char *userId)
{
char *name	= NULL;
	/************************/
	/* walidacja parametrow */
	/************************/
	if (userId==NULL)		{	BMD_FOK(BMD_ERR_PARAM3);	}

	BMD_FOK(IsNumericIdentifier(userId));

	//SELECT id, name, identity, cert_serial, cert_dn, cert_dn||cert_serial, accepted FROM users;
	BMD_FOK_CHG_NP(getElementWithCondition(	_GLOBAL_UsersDictionary, 0, userId, 1, &name), LIBBMDSQL_USER_NOT_REGISTERED_IN_SYSTEM);

	/************/
	/* porzadki */
	/************/
	free0(name);

	return BMD_OK;

}

/****************************************/
/* sprawdzenie, czy uzytkownik istnieje */
/****************************************/
long CheckIfUserExist2(	char *certIssuerSerial,
			char **userId)
{
	/************************/
	/* walidacja parametrow */
	/************************/
	if (certIssuerSerial==NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (userId==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if ((*userId)!=NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	//SELECT id, name, identity, cert_serial, cert_dn, cert_dn||cert_serial, accepted FROM users;
	BMD_FOK_CHG(getElementWithCondition(	_GLOBAL_UsersDictionary, 5, certIssuerSerial, 0, userId),
			LIBBMDSQL_DICT_USER_VALUE_NOT_FOUND);

	if ((*userId)==NULL)	{	return LIBBMDSQL_USER_NOT_REGISTERED_IN_SYSTEM;	}

	return BMD_OK;
}

long CheckIfCertInfoFree(	char *certIssuer,
				char *certSerial)
{
char *certIssuerSerial	= NULL;
char *tmp		= NULL;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (certIssuer==NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (certSerial==NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	asprintf(&certIssuerSerial, "%s%s", certIssuer, certSerial);
	if (certIssuerSerial==NULL)	{	BMD_FOK(NO_MEMORY);	}

	//SELECT id, name, identity, cert_serial, cert_dn, cert_dn||cert_serial, accepted FROM users;
	getElementWithCondition(	_GLOBAL_UsersDictionary, 5, certIssuerSerial, 0, &tmp);

	if (tmp!=NULL)	{	BMD_FOK(LIBBMDSQL_USER_CERT_ALREADY_REGISTERED);	}

	/************/
	/* porzadki */
	/************/
	free0(certIssuerSerial);
	free0(tmp);

	return BMD_OK;
}


long CheckIfIdentityExists(void *hDB, char* identityId)
{
long retVal		= 0;
char* SQLTemplate	= "SELECT id FROM identities WHERE id=%s AND location_id=%s;";
char* SQLQuery		= NULL;
void* queryResult	= NULL;
long rowsCount		= 0;

	if(hDB == NULL)
		{ return BMD_ERR_PARAM1; }
	if(identityId == NULL)
		{ return BMD_ERR_PARAM2; }
	if(_GLOBAL_bmd_configuration == NULL || _GLOBAL_bmd_configuration->location_id == NULL)
		{ return BMD_ERR_OP_FAILED; }

	
	retVal=IsNumericIdentifier(identityId);
	if(retVal < BMD_OK)
		{ return retVal; }
	
	asprintf(&SQLQuery, SQLTemplate, identityId, _GLOBAL_bmd_configuration->location_id);
	if(SQLQuery == NULL)
		{ return BMD_ERR_MEMORY; }

	retVal=bmd_db_execute_sql(hDB, SQLQuery, &rowsCount, NULL, &queryResult);
	bmd_db_result_free(&queryResult);

	if(retVal != BMD_OK)
		{ return retVal; }

	if(rowsCount > 0)
		{ return BMD_OK; }
	else
		{ return LIBBMDSQL_USER_IDENTITY_NOT_FOUND; }
}


/************************************/
/* wylaczenia/wlaczenie certyfikatu uzytkownika */
/************************************/
long JS_bmdDatagramSet_to_disableUser(	void *hDB,
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

		status=JS_bmdDatagram_to_disableUser(hDB, bmdJSRequestSet->bmdDatagramSetTable[i],
			&((*bmdJSResponseSet)->bmdDatagramSetTable[i]),req);

		/**************************************/
		/* ustawienie statusu błędu datagramu */
		/**************************************/
		(*bmdJSResponseSet)->bmdDatagramSetTable[i]->datagramType = bmdJSRequestSet->bmdDatagramSetTable[i]->datagramType+100;
		(*bmdJSResponseSet)->bmdDatagramSetTable[i]->datagramStatus = status;

		/****************************************/
		/* zarejestrowanie akcji w bazie danych */
		/****************************************/
		if (_GLOBAL_bmd_configuration->messaging_plugin_conf.bmd_messaging_plugin_register_action)
		{
			status=_GLOBAL_bmd_configuration->messaging_plugin_conf.bmd_messaging_plugin_register_action(hDB, bmdJSRequestSet->bmdDatagramSetTable[i], (*bmdJSResponseSet)->bmdDatagramSetTable[i], req);
			if (status<BMD_OK)
			{
				(*bmdJSResponseSet)->bmdDatagramSetTable[i]->datagramStatus = status;
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

#ifndef WIN32
	kill(getppid(),SIGDICTRELOAD);
#endif //ifndef WIN32


	return BMD_OK;

}


long JS_bmdDatagram_to_disableUser(	void *hDB,
					bmdDatagram_t *bmdJSRequest,
					bmdDatagram_t **bmdJSResponse,
					server_request_data_t *req)
{
long retVal			= 0;
long iter			= 0;
void *query_result		= NULL;
char *SQLQuery			= NULL;
char *certificateAccepted	= NULL;
char *userId			= NULL;
char* escapeTemp		= NULL;

	/************************/
	/* walidacja parametrow */
	/************************/
	if(hDB==NULL)
		{ BMD_FOK(BMD_ERR_PARAM1); }
	if(bmdJSRequest==NULL)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(req == NULL || req->ua == NULL || req->ua->id == NULL)
		{ BMD_FOK(BMD_ERR_PARAM4); }
	if(_GLOBAL_bmd_configuration == NULL || _GLOBAL_bmd_configuration->location_id == NULL)
		{ BMD_FOK(BMD_ERR_OP_FAILED); }

	/********************************************************************/
	/* sprawdzenie dentyfikatora uzytkownika, ktorego chcemy updateowac */
	/********************************************************************/
	if(	(bmdJSRequest->protocolDataFileId!=NULL) &&
		(bmdJSRequest->protocolDataFileId->buf!=NULL) &&
		(bmdJSRequest->protocolDataFileId->size>0) )
	{
		userId=strdup(bmdJSRequest->protocolDataFileId->buf);
		if(userId==NULL)
			{ BMD_FOK(NO_MEMORY); }
	}

	if(userId==NULL)
		{ BMD_FOK(LIBBMDSQL_USER_ID_UNDEFINED); }

	/****************************************/
	/* sprawdzenie, czy uzytkownik istnieje */
	/****************************************/
	retVal=CheckIfUserExist(userId);
	if(retVal < BMD_OK)
	{
		free(userId); userId=NULL;
		BMD_FOK(retVal);
	}

	/******************************************************/
	/* poszukiwanie poszczególnych elementów w datagramie */
	/******************************************************/
	for(iter=0; iter<bmdJSRequest->no_of_sysMetaData; iter++)
	{
		if(bmdJSRequest->sysMetaData[iter] != NULL && bmdJSRequest->sysMetaData[iter]->OIDTableBuf != NULL)
		{
			if(strcmp(bmdJSRequest->sysMetaData[iter]->OIDTableBuf, OID_SYS_METADATA_CERT_USER_ACCEPTED)==0)
			{
				if(bmdJSRequest->sysMetaData[iter]->AnyBuf != NULL)
				{
					if( strcmp(bmdJSRequest->sysMetaData[iter]->AnyBuf, "0") == 0 ||
					strcmp(bmdJSRequest->sysMetaData[iter]->AnyBuf, "1") == 0)
					{
						certificateAccepted=strdup(bmdJSRequest->sysMetaData[iter]->AnyBuf);
						break;
					}
				}
			}
		}
	}

	/*domyslnie wylaczenie certyfikatu*/
	if(certificateAccepted == NULL)
	{
		certificateAccepted=strdup("0");
		if(certificateAccepted == NULL)
		{
			free(userId); userId=NULL;
			BMD_FOK(BMD_ERR_MEMORY);
		}
	}

	/*nie mozna zablokowac certyfikatu, na ktory uzytkownik jest aktualnie zalogowany*/
	if(strcmp(certificateAccepted, "0") == 0 )
	{
		if(req->ua->id->buf != NULL && req->ua->id->size > 0)
		{
			if(strcmp(req->ua->id->buf, userId) == 0)
			{
				free(userId); userId=NULL;
				free(certificateAccepted), certificateAccepted=NULL;
				BMD_FOK( DISABLE_CURRENT_CERTIFICATE_FORBIDDEN );
			}
		}
		else
		{
			free(userId); userId=NULL;
			free(certificateAccepted); certificateAccepted=NULL;
			BMD_FOK(BMD_ERR_OP_FAILED);
		}
	}

	/******************************/
	/*  zmiana wartosci accepted  */
	/******************************/
	retVal=bmd_db_escape_string(hDB, certificateAccepted, STANDARD_ESCAPING, &escapeTemp);
	if(retVal < BMD_OK)
	{
		free(userId); userId=NULL;
		free(certificateAccepted); certificateAccepted=NULL;
		BMD_FOK(retVal);
	}
	asprintf(&SQLQuery, "UPDATE users SET accepted=%s WHERE id='%s' AND location_id='%s';", escapeTemp, userId, _GLOBAL_bmd_configuration->location_id);
	free(escapeTemp); escapeTemp=NULL;
	free(certificateAccepted); certificateAccepted=NULL;
	free(userId); userId=NULL;
	if (SQLQuery==NULL)
		{ BMD_FOK(NO_MEMORY); }

	retVal=bmd_db_execute_sql( hDB, SQLQuery,NULL,NULL, &query_result);
	free(SQLQuery); SQLQuery=NULL;
	bmd_db_result_free(&query_result);
	
	BMD_FOK(retVal);

	return BMD_OK;
}


long JS_bmdDatagram_to_disableIdentity(	void *hDB,
					bmdDatagram_t *bmdJSRequest,
					bmdDatagram_t **bmdJSResponse,
					server_request_data_t *req)
{

char *userIdentity		= NULL;
char *userCertId		= NULL;
void* selectResult		= NULL;
char* selectQuery		= NULL;
void* updateResult		= NULL;
char *updateQuery		= NULL;
long retVal			= 0;
long rowsCount			= 0;
long iter			= 0;
long fetchedCount		= 0;
char *certificateAccepted	= NULL;
char* escapeTemp		= NULL;


	if(hDB==NULL)
		{ BMD_FOK(BMD_ERR_PARAM1); }
	if(bmdJSRequest==NULL)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(req == NULL || req->ua == NULL || req->ua->identityId == NULL)
		{ BMD_FOK(BMD_ERR_PARAM4); }
	if(_GLOBAL_bmd_configuration == NULL || _GLOBAL_bmd_configuration->location_id == NULL)
		{ BMD_FOK(BMD_ERR_OP_FAILED); }


	if(	(bmdJSRequest->protocolDataFileId!=NULL) &&
		(bmdJSRequest->protocolDataFileId->buf!=NULL) &&
		(bmdJSRequest->protocolDataFileId->size>0) )
	{
		userIdentity=strdup(bmdJSRequest->protocolDataFileId->buf);
		if(userIdentity==NULL)
			{ BMD_FOK(NO_MEMORY); }
	}
	else
	{
		BMD_FOK(LIBBMDSQL_USER_IDENTITY_UNDEFINED);
	}

	retVal=CheckIfIdentityExists(hDB, userIdentity);
	if(retVal != BMD_OK)
	{
		free(userIdentity); userIdentity=NULL;
		BMD_FOK(retVal;);
	}
	
	for(iter=0; iter<bmdJSRequest->no_of_sysMetaData; iter++)
	{
		if(bmdJSRequest->sysMetaData[iter] != NULL && bmdJSRequest->sysMetaData[iter]->OIDTableBuf != NULL)
		{
			if(strcmp(bmdJSRequest->sysMetaData[iter]->OIDTableBuf, OID_SYS_METADATA_CERT_USER_ACCEPTED)==0)
			{
				if(bmdJSRequest->sysMetaData[iter]->AnyBuf != NULL)
				{
					if(strcmp(bmdJSRequest->sysMetaData[iter]->AnyBuf, "0")==0 ||
					strcmp(bmdJSRequest->sysMetaData[iter]->AnyBuf, "1")==0)
					{
						certificateAccepted=strdup(bmdJSRequest->sysMetaData[iter]->AnyBuf);
					}
					break;
				}
			}
		}
	}
	
	/*domyslnie wylaczenie certyfikatow*/
	if(certificateAccepted == NULL)
	{
		certificateAccepted=strdup("0");
		if(certificateAccepted == NULL)
		{
			free(userIdentity); userIdentity=NULL;
			bmd_db_result_free(&selectResult);
			BMD_FOK(BMD_ERR_MEMORY);
		}
	}

	/*nie mozna pozwolic na wylaczenie wlasnej tozsamosci*/
	if(strcmp(certificateAccepted, "0") == 0)
	{
		if(req->ua->identityId->buf != NULL && req->ua->identityId->size > 0)
		{
			if(strcmp(req->ua->identityId->buf, userIdentity) == 0)
			{
				free(certificateAccepted); certificateAccepted=NULL;
				free(userIdentity); userIdentity=NULL;
				BMD_FOK(DISABLE_OWN_IDENTITY_FORBIDDEN);
			}
		}
		else
		{
			free(certificateAccepted); certificateAccepted=NULL;
			free(userIdentity); userIdentity=NULL;
			BMD_FOK(BMD_ERR_OP_FAILED);
		}
	}

	/*wylaczanie dotyczy tych certyfikatow, ktore nie sa jeszcze wylaczone (ewentualnie wlaczenie niewlaczonych)*/
	escapeTemp=certificateAccepted; certificateAccepted=NULL;
	retVal=bmd_db_escape_string(hDB, escapeTemp, STANDARD_ESCAPING, &certificateAccepted);
	free(escapeTemp); escapeTemp=NULL;
	if(retVal < BMD_OK)
	{
		free(certificateAccepted); certificateAccepted=NULL;
		free(userIdentity); userIdentity=NULL;
		BMD_FOK(retVal);
	}
	asprintf(&selectQuery, "SELECT id FROM users WHERE location_id=%s AND identity=%s AND accepted<>%s;",
			 _GLOBAL_bmd_configuration->location_id, userIdentity, certificateAccepted);
	free(userIdentity); userIdentity=NULL;

	retVal=bmd_db_execute_sql( hDB, selectQuery, &rowsCount, NULL, &selectResult);
	free(selectQuery); selectQuery=NULL;

	BMD_FOK(retVal);
	
	if(rowsCount == 0)
	{
		free(certificateAccepted); certificateAccepted=NULL;
		bmd_db_result_free(&selectResult);
		return BMD_OK;
	}

	for(iter=0; iter<rowsCount ; iter++)
	{
		retVal=bmd_db_result_get_value(hDB, selectResult, iter, 0, &userCertId, FETCH_ABSOLUTE, &fetchedCount);
		if(retVal == BMD_ERR_NODATA)
			{ break; }
		else if(retVal != BMD_OK)
		{
			free(certificateAccepted); certificateAccepted=NULL;
			bmd_db_result_free(&selectResult);
			BMD_FOK(retVal);
		}

		asprintf(&updateQuery, "UPDATE users SET accepted=%s WHERE id=%s AND location_id=%s;",
				certificateAccepted, userCertId, _GLOBAL_bmd_configuration->location_id);
		free(userCertId); userCertId=NULL;
		retVal=bmd_db_execute_sql( hDB, updateQuery, NULL, NULL, &updateResult);
		free(updateQuery); updateQuery=NULL;
		bmd_db_result_free(&updateResult);
		if(retVal != BMD_OK)
		{
			free(certificateAccepted); certificateAccepted=NULL;
			bmd_db_result_free(&selectResult);
			BMD_FOK(retVal);
		}
	}

	free(certificateAccepted); certificateAccepted=NULL;
	bmd_db_result_free(&selectResult);

	return BMD_OK;
}


/************************************/
/* wylaczenia/wlaczenie tozsamosci */
/************************************/
long JS_bmdDatagramSet_to_disableIdentity(void *hDB,
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

	for (i=0; i<bmdJSRequestSet->bmdDatagramSetSize; i++)
	{
		/***********************/
		/* poczatek transakcji */
		/***********************/
		BMD_FOK(bmd_db_start_transaction(hDB,BMD_TRANSACTION_LEVEL_NONE));

		status=JS_bmdDatagram_to_disableIdentity(hDB, bmdJSRequestSet->bmdDatagramSetTable[i],
			&((*bmdJSResponseSet)->bmdDatagramSetTable[i]),req);

		/**************************************/
		/* ustawienie statusu błędu datagramu */
		/**************************************/
		(*bmdJSResponseSet)->bmdDatagramSetTable[i]->datagramType = bmdJSRequestSet->bmdDatagramSetTable[i]->datagramType+100;
		(*bmdJSResponseSet)->bmdDatagramSetTable[i]->datagramStatus = status;

		/****************************************/
		/* zarejestrowanie akcji w bazie danych */
		/****************************************/
		if (_GLOBAL_bmd_configuration->messaging_plugin_conf.bmd_messaging_plugin_register_action)
		{
			status=_GLOBAL_bmd_configuration->messaging_plugin_conf.bmd_messaging_plugin_register_action(hDB, bmdJSRequestSet->bmdDatagramSetTable[i], (*bmdJSResponseSet)->bmdDatagramSetTable[i], req);
			if (status<BMD_OK)
			{
				(*bmdJSResponseSet)->bmdDatagramSetTable[i]->datagramStatus = status;
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

#ifndef WIN32
	kill(getppid(),SIGDICTRELOAD);
#endif //ifndef WIN32


	return BMD_OK;
}


/*************************/
/* usuniecie uzytkownika */
/*************************/
long JS_bmdDatagramSet_to_deleteUser(	void *hDB,
					bmdDatagramSet_t *bmdJSRequestSet,
					bmdDatagramSet_t **bmdJSResponseSet,
					server_request_data_t *req)
{
long status		= 0;
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

		status=JS_bmdDatagram_to_deleteUser(hDB, bmdJSRequestSet->bmdDatagramSetTable[i],
			&((*bmdJSResponseSet)->bmdDatagramSetTable[i]),req);

		/**************************************/
		/* ustawienie statusu błędu datagramu */
		/**************************************/
		(*bmdJSResponseSet)->bmdDatagramSetTable[i]->datagramType = bmdJSRequestSet->bmdDatagramSetTable[i]->datagramType+100;
		(*bmdJSResponseSet)->bmdDatagramSetTable[i]->datagramStatus = status;

		/****************************************/
		/* zarejestrowanie akcji w bazie danych */
		/****************************************/
		if (_GLOBAL_bmd_configuration->messaging_plugin_conf.bmd_messaging_plugin_register_action)
		{
			status=_GLOBAL_bmd_configuration->messaging_plugin_conf.bmd_messaging_plugin_register_action(hDB, bmdJSRequestSet->bmdDatagramSetTable[i], (*bmdJSResponseSet)->bmdDatagramSetTable[i], req);
			if (status<BMD_OK)
			{
				(*bmdJSResponseSet)->bmdDatagramSetTable[i]->datagramStatus = status;
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

#ifndef WIN32
	kill(getppid(),SIGDICTRELOAD);
#endif //ifndef WIN32

	return BMD_OK;
}

long JS_bmdDatagram_to_deleteUser(	void *hDB,
						bmdDatagram_t *bmdJSRequest,
						bmdDatagram_t **bmdJSResponse,
					server_request_data_t *req)
{
void *query_result	= NULL;
char *SQLQuery		= NULL;
char *userId		= NULL;
char *ans		= NULL;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (hDB==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (bmdJSRequest==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}

	/********************************************************************/
	/* sprawdzenie dentyfikatora uzytkownika, ktorego chcemy updateowac */
	/********************************************************************/
	if (	(bmdJSRequest->protocolDataFileId!=NULL) &&
		(bmdJSRequest->protocolDataFileId->buf!=NULL) &&
		(bmdJSRequest->protocolDataFileId->size>0) )
	{
		asprintf(&userId, "%s", bmdJSRequest->protocolDataFileId->buf);
		if (userId==NULL)	{	BMD_FOK(NO_MEMORY);	}
	}

	if (userId==NULL)	{	BMD_FOK(LIBBMDSQL_USER_ID_UNDEFINED);	}

	/****************************************/
	/* sprawdzenie, czy uzytkownik istnieje */
	/****************************************/
	BMD_FOK(CheckIfUserExist(userId));

	/***************************************************/
	/* sprawdzenie, czy uzytkownik ma jakies dokumenty */
	/***************************************************/
	asprintf(&SQLQuery, 	"SELECT count(id) FROM crypto_objects WHERE location_id=%s AND fk_owner = (SELECT identity FROM users WHERE id=%s AND location_id=%s);", _GLOBAL_bmd_configuration->location_id, userId, _GLOBAL_bmd_configuration->location_id);
	if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}

	BMD_FOK(ExecuteSQLQueryWithAnswerKnownDBConnection(hDB, SQLQuery, &ans));
	if (strcmp(ans, "0")!=0)	{	BMD_FOK(LIBBMDSQL_USER_FILE_OWNER);	}

	free0(ans);

	/************************************************/
	/* usuniecie powiazan uzytkownika z tabela grup */
	/************************************************/
	asprintf(&SQLQuery, 	"DELETE FROM users_and_groups WHERE fk_users='%s' AND location_id=%s;", userId, _GLOBAL_bmd_configuration->location_id);
	if (SQLQuery==NULL)
		{	BMD_FOK(NO_MEMORY);	}
	BMD_FOK(bmd_db_execute_sql( hDB, SQLQuery,NULL,NULL, &query_result));
	bmd_db_result_free(&query_result);
	free(SQLQuery); SQLQuery=NULL;

	/***********************************************/
	/* usuniecie powiazan uzytkownika z tabela rol */
	/***********************************************/
	asprintf(&SQLQuery, 	"DELETE FROM users_and_roles WHERE fk_users='%s' AND location_id=%s;", userId, _GLOBAL_bmd_configuration->location_id);
	if (SQLQuery==NULL)
		{	BMD_FOK(NO_MEMORY);	}
	BMD_FOK(bmd_db_execute_sql( hDB, SQLQuery,NULL,NULL, &query_result));
	bmd_db_result_free(&query_result);
	free(SQLQuery); SQLQuery=NULL;

	/***********************************************/
	/* usuniecie powiazan uzytkownika z tabela klas */
	/***********************************************/
	asprintf(&SQLQuery, 	"DELETE FROM users_and_classes WHERE fk_users='%s' AND location_id=%s;", userId, _GLOBAL_bmd_configuration->location_id);
	if (SQLQuery==NULL)
		{	BMD_FOK(NO_MEMORY);	}
	BMD_FOK(bmd_db_execute_sql( hDB, SQLQuery,NULL,NULL, &query_result));
	bmd_db_result_free(&query_result);
	free(SQLQuery); SQLQuery=NULL;

	/**********************************************************/
	/* usuniecie powiazan uzytkownika z tabela bezpieczenstwa */
	/**********************************************************/
	asprintf(&SQLQuery, 	"DELETE FROM users_and_security WHERE fk_users='%s' AND location_id=%s;", userId, _GLOBAL_bmd_configuration->location_id);
	if (SQLQuery==NULL)
		{	BMD_FOK(NO_MEMORY);	}
	BMD_FOK(bmd_db_execute_sql( hDB, SQLQuery,NULL,NULL, &query_result));
	bmd_db_result_free(&query_result);
	free(SQLQuery); SQLQuery=NULL;

	/*********************************/
	/* usuniecie wpisu o uzytkowniku */
	/*********************************/
	asprintf(&SQLQuery, 	"DELETE FROM users WHERE id='%s' AND location_id=%s;", userId, _GLOBAL_bmd_configuration->location_id);
	if (SQLQuery==NULL)
		{	BMD_FOK(NO_MEMORY);	}
	BMD_FOK(bmd_db_execute_sql( hDB, SQLQuery,NULL,NULL, &query_result));
	bmd_db_result_free(&query_result);
	free(SQLQuery); SQLQuery=NULL;

	free(userId); userId=NULL;
	return BMD_OK;
}


/**************************************************************/
/*          pobieranie informacji o uzytkownikach             */
/**************************************************************/
long JS_bmdDatagramSet_to_getUserList(	void *hDB,
					bmdDatagramSet_t *bmdJSRequestSet,
					bmdDatagramSet_t **bmdJSResponseSet,
					server_request_data_t *req)
{
long status		= 0;
long i			= 0;
long usersNumber	= 0;
char *userId		= NULL;
char *SQLQuery		= NULL;
char *ans		= NULL;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (hDB==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (bmdJSRequestSet==NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (bmdJSRequestSet->bmdDatagramSetSize != 1)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(_GLOBAL_bmd_configuration == NULL)
		{ BMD_FOK(BMD_ERR_OP_FAILED); }

	/***************************************/
	/* sprawdzenie, czy okreslono id tozsamosci usera */
	/***************************************/
	if (	(bmdJSRequestSet->bmdDatagramSetTable[0]->protocolDataFileId!=NULL) &&
		(bmdJSRequestSet->bmdDatagramSetTable[0]->protocolDataFileId->buf!=NULL) &&
		(bmdJSRequestSet->bmdDatagramSetTable[0]->protocolDataFileId->size>0) )
	{
		userId=STRNDUP(bmdJSRequestSet->bmdDatagramSetTable[0]->protocolDataFileId->buf, bmdJSRequestSet->bmdDatagramSetTable[0]->protocolDataFileId->size);
		if(userId==NULL)
			{ BMD_FOK(NO_MEMORY); }

		status=CheckIfIdentityExists(hDB, userId);
		if(status < BMD_OK)
		{
			free(userId); userId=NULL;
			BMD_FOK(status);
		}
	}

	/***********************************************/
	/* odczytanie informacji o ilosci uzytkownikow */
	/***********************************************/
	if (userId==NULL)
	{
		asprintf(&SQLQuery, "SELECT count(id) FROM users WHERE location_id=%s;", _GLOBAL_bmd_configuration->location_id);
	}
	else
	{
		asprintf(&SQLQuery, "SELECT count(id) FROM users WHERE identity=%s AND location_id=%s;", userId, _GLOBAL_bmd_configuration->location_id );
	}

	if (SQLQuery==NULL)
	{
		free(userId); userId=NULL;
		BMD_FOK(NO_MEMORY);
	}
	
	ExecuteSQLQueryWithAnswerKnownDBConnection( hDB, SQLQuery, &ans);
	free(SQLQuery); SQLQuery=NULL;
	if (ans==NULL)
	{
		free(userId); userId=NULL;
		BMD_FOK(LIBBMDSQL_USER_LIST_NO_USERS);
	}
	
	usersNumber=strtol(ans, NULL, 10);
	free(ans); ans=NULL;
	if (usersNumber<=0)
	{
		free(userId); userId=NULL;
		BMD_FOK(LIBBMDSQL_USER_LIST_NO_USERS);
	}

	/***********************/
	/* poczatek transakcji */
	/***********************/
	BMD_FOK(bmd_db_start_transaction(hDB,BMD_TRANSACTION_LEVEL_NONE));

	/*****************************************/
	/* przygotowanie datagramsetu odpowiedzi */
	/*****************************************/
	BMD_FOK(PR2_bmdDatagramSet_fill_with_datagrams(usersNumber, bmdJSResponseSet));

	/*******************************************/
	/* pobieranie danych poszczegolnych userow */
	/*******************************************/
	for (i=0; i<usersNumber; i++)
	{
		status=JS_bmdDatagram_to_getUserList(hDB, i, userId, &((*bmdJSResponseSet)->bmdDatagramSetTable[i]),req);

		/**************************************/
		/* ustawienie statusu bledu datagramu */
		/**************************************/
		(*bmdJSResponseSet)->bmdDatagramSetTable[i]->datagramType = bmdJSRequestSet->bmdDatagramSetTable[0]->datagramType = BMD_DATAGRAM_TYPE_GET_USER_LIST_RESPONSE;
		(*bmdJSResponseSet)->bmdDatagramSetTable[i]->datagramStatus = status;

	}
	free(userId); userId=NULL;
	/**************************/
	/* zakonczenie transakcji */
	/**************************/
	bmd_db_end_transaction(hDB);
	
	return BMD_OK;
}

long JS_bmdDatagram_to_getUserList(	void *hDB,
					long number,
					char *userId,
					bmdDatagram_t **bmdJSResponse,
					server_request_data_t *req)
{
long ansCount		= 0;
long secCatsCount	= 0;
long secLevelsCount	= 0;
long i			= 0;
char **anss		= NULL;
char **secCats		= NULL;
char **secLevels	= NULL;
char *SQLQuery		= NULL;
char *certificate	= NULL;
char *userIdentityId		= NULL;
char *userCertificateId		= NULL;
char *userClearance	= NULL;
char *userDefaultGroup	= NULL;
char *userDefaultRole	= NULL;
char *userDefaultClass	= NULL;
char *tmp2		= NULL;
char* escapedUserId		= NULL;

	/**********************************************/
	/* pobranie glownych informacji o uzytkowniku */
	/**********************************************/
	if(userId != NULL)
	{
		BMD_FOK(bmd_db_escape_string(hDB, userId, STANDARD_ESCAPING, &escapedUserId));
	}
	
	asprintf(&SQLQuery, 	"SELECT id, identity, name, cert_serial, cert_dn, accepted FROM users %s%s %s%s %s%s LIMIT 1 OFFSET %li;",
					_GLOBAL_bmd_configuration->location_id==NULL?"":"WHERE location_id=",
					_GLOBAL_bmd_configuration->location_id==NULL?"":_GLOBAL_bmd_configuration->location_id,
					(userId!=NULL && _GLOBAL_bmd_configuration->location_id!=NULL)?"AND identity=":"",
					(userId!=NULL && _GLOBAL_bmd_configuration->location_id!=NULL)?escapedUserId:"",
					(userId!=NULL && _GLOBAL_bmd_configuration->location_id==NULL)?"WHERE identity=":"",
					(userId!=NULL && _GLOBAL_bmd_configuration->location_id==NULL)?escapedUserId:"",
					number);
	free(escapedUserId); escapedUserId=NULL;

	if(SQLQuery == NULL)
		{ BMD_FOK(NO_MEMORY); }

	BMD_FOK(ExecuteSQLQueryWithRowAnswersKnownDBConnection( hDB, SQLQuery, &ansCount, &anss));
	free(SQLQuery); SQLQuery=NULL;

	userIdentityId=strdup(anss[1]); 
	if(userIdentityId==NULL)
		{ BMD_FOK(NO_MEMORY); }
	userCertificateId=strdup(anss[0]);
	if(userCertificateId == NULL)
	{
		free(userIdentityId); userIdentityId=NULL;
		BMD_FOK(NO_MEMORY);
	}

	BMD_FOK(set_gen_buf2(userIdentityId, (long)strlen(userIdentityId), &((*bmdJSResponse)->protocolDataFileId)));
	free(userIdentityId); userIdentityId=NULL;

	asprintf(&certificate, "%s:%s", anss[3], anss[4]);
	if (certificate==NULL)	{	BMD_FOK(NO_MEMORY);	}
	BMD_FOK(set_gen_buf2(certificate, (long)strlen(certificate), &((*bmdJSResponse)->protocolDataFilename)));
	free0(certificate);

	for (i=0; i<ansCount; i++)
	{
		switch(i)
		{
			case 0 : BMD_FOK(bmd_datagram_add_metadata_char(OID_SYS_METADATA_CERT_USER_ID, anss[i], bmdJSResponse)); break;
			case 1 : BMD_FOK(bmd_datagram_add_metadata_char(OID_SYS_METADATA_CERT_USER_IDENTITY, anss[i], bmdJSResponse)); break;
			case 2 : BMD_FOK(bmd_datagram_add_metadata_char(OID_SYS_METADATA_CERT_USER_NAME, anss[i], bmdJSResponse)); break;
			case 3 : BMD_FOK(bmd_datagram_add_metadata_char(OID_SYS_METADATA_CERT_USER_SN, anss[i], bmdJSResponse)); break;
			case 4 : BMD_FOK(bmd_datagram_add_metadata_char(OID_SYS_METADATA_CERT_USER_ISSUER, anss[i], bmdJSResponse)); break;
			case 5 : BMD_FOK(bmd_datagram_add_metadata_char(OID_SYS_METADATA_CERT_USER_ACCEPTED, anss[i], bmdJSResponse)); break;
		}
		free0(anss[i]);
	}
	free0(anss);

	/********************************************/
	/* pobranie informacji o rolach uzytkownika */
	/********************************************/
	//SELECT users_and_roles.fk_users, users_and_roles.default_role, roles.id, roles.name, users_and_roles.fk_users::varchar||'|'||roles.name::varchar FROM roles LEFT JOIN users_and_roles ON (users_and_roles.fk_roles=roles.id);
	BMD_FOK(getColumnWithCondition(	_GLOBAL_UserRolesDictionary, 0, userCertificateId, 2, &ansCount, &anss));

	for (i=0; i<ansCount; i++)
	{
		BMD_FOK(bmd_datagram_add_metadata_char(OID_SYS_METADATA_CERT_ROLE_ID, anss[i], bmdJSResponse));

		//SELECT users_and_roles.fk_users, users_and_roles.default_role, roles.id, roles.name, users_and_roles.fk_users::varchar||'|'||roles.name::varchar FROM roles LEFT JOIN users_and_roles ON (users_and_roles.fk_roles=roles.id);
		BMD_FOK_CHG(getElementWithCondition(_GLOBAL_UserRolesDictionary, 2, anss[i], 3, &tmp2), LIBBMDSQL_DICT_ROLE_VALUE_NOT_FOUND);
		BMD_FOK(bmd_datagram_add_metadata_char(OID_SYS_METADATA_CERT_ROLE_NAME, tmp2, bmdJSResponse));

		free0(tmp2);
		free0(anss[i]);

	}
	free0(anss);

	/***************************************************/
	/* pobranie informacji o domylnej roli uzytkownika */
	/***************************************************/
	asprintf(&SQLQuery, "SELECT id FROM roles WHERE id IN (SELECT fk_roles FROM users_and_roles WHERE location_id=%s AND fk_users=%s AND default_role=1) AND location_id=%s;", _GLOBAL_bmd_configuration->location_id, userCertificateId, _GLOBAL_bmd_configuration->location_id);
	if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}

	ExecuteSQLQueryWithAnswerKnownDBConnection( hDB, SQLQuery, &userDefaultRole);

	if (userDefaultRole)
	{
		BMD_FOK(bmd_datagram_add_metadata_char(OID_SYS_METADATA_CERT_DEFAULT_ROLE_ID, userDefaultRole, bmdJSResponse));

		//SELECT users_and_roles.fk_users, users_and_roles.default_role, roles.id, roles.name, users_and_roles.fk_users::varchar||'|'||roles.name::varchar FROM roles LEFT JOIN users_and_roles ON (users_and_roles.fk_roles=roles.id);
		BMD_FOK_CHG(getElementWithCondition(_GLOBAL_UserRolesDictionary, 2, userDefaultRole, 3, &tmp2), LIBBMDSQL_DICT_ROLE_VALUE_NOT_FOUND);
		BMD_FOK(bmd_datagram_add_metadata_char(OID_SYS_METADATA_CERT_DEFAULT_ROLE_NAME, tmp2, bmdJSResponse));

		free0(tmp2);
		free0(userDefaultRole);
	}
	free0(SQLQuery);

	/*********************************************/
	/* pobranie informacji o grupach uzytkownika */
	/*********************************************/
	//SELECT users_and_groups.fk_users, users_and_groups.default_group,groups.id, groups.name, users_and_groups.fk_users::varchar||'|'||groups.id::varchar FROM groups LEFT JOIN users_and_groups ON (users_and_groups.fk_groups=groups.id);
	BMD_FOK(getColumnWithCondition(	_GLOBAL_UserGroupsDictionary, 0, userCertificateId, 2, &ansCount, &anss));

	for (i=0; i<ansCount; i++)
	{
		BMD_FOK(bmd_datagram_add_metadata_char(OID_SYS_METADATA_CERT_GROUP_ID, anss[i], bmdJSResponse));

		//SELECT users_and_roles.fk_users, users_and_roles.default_role, roles.id, roles.name, users_and_roles.fk_users::varchar||'|'||roles.name::varchar FROM roles LEFT JOIN users_and_roles ON (users_and_roles.fk_roles=roles.id);
		BMD_FOK_CHG(getElementWithCondition(_GLOBAL_UserGroupsDictionary, 2, anss[i], 3, &tmp2), LIBBMDSQL_DICT_ROLE_VALUE_NOT_FOUND);
		BMD_FOK(bmd_datagram_add_metadata_char(OID_SYS_METADATA_CERT_GROUP_NAME, tmp2, bmdJSResponse));

		free0(anss[i]);
		free0(tmp2);
	}
	free0(anss);

	/*****************************************************/
	/* pobranie informacji o domylnej grupie uzytkownika */
	/*****************************************************/
	asprintf(&SQLQuery, "SELECT id FROM groups WHERE id IN (SELECT fk_groups FROM users_and_groups WHERE location_id=%s AND  fk_users=%s  AND default_group=1) AND location_id=%s;", _GLOBAL_bmd_configuration->location_id, userCertificateId, _GLOBAL_bmd_configuration->location_id);
	if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}

	ExecuteSQLQueryWithAnswerKnownDBConnection( hDB, SQLQuery, &userDefaultGroup);

	if (userDefaultGroup)
	{
		BMD_FOK(bmd_datagram_add_metadata_char(OID_SYS_METADATA_CERT_DEFAULT_GROUP_ID, userDefaultGroup, bmdJSResponse));

		//SELECT users_and_roles.fk_users, users_and_roles.default_role, roles.id, roles.name, users_and_roles.fk_users::varchar||'|'||roles.name::varchar FROM roles LEFT JOIN users_and_roles ON (users_and_roles.fk_roles=roles.id);
		BMD_FOK_CHG(getElementWithCondition(_GLOBAL_UserGroupsDictionary, 2, userDefaultGroup, 3, &tmp2), LIBBMDSQL_DICT_ROLE_VALUE_NOT_FOUND);
		BMD_FOK(bmd_datagram_add_metadata_char(OID_SYS_METADATA_CERT_DEFAULT_GROUP_NAME, tmp2, bmdJSResponse));

		free0(tmp2);
		free0(userDefaultGroup);
	}
	free0(SQLQuery);

	/*********************************************/
	/* pobranie informacji o klasach uzytkownika */
	/*********************************************/
	//SELECT users_and_classes.fk_users, users_and_classes.default_class, classes.id, classes.name, users_and_classes.fk_users::varchar||'|'||classes.id::varchar FROM classes LEFT JOIN users_and_classes ON (users_and_classes.fk_classes=classes.id);
	BMD_FOK(getColumnWithCondition(	_GLOBAL_UserClassesDictionary, 0, userCertificateId, 2, &ansCount, &anss));

	for (i=0; i<ansCount; i++)
	{
		BMD_FOK(bmd_datagram_add_metadata_char(OID_ACTION_METADATA_CERT_CLASS_ID, anss[i], bmdJSResponse));

		//SELECT users_and_classes.fk_users, users_and_classes.default_class, classes.id, classes.name, users_and_classes.fk_users::varchar||'|'||classes.id::varchar FROM classes LEFT JOIN users_and_classes ON (users_and_classes.fk_classes=classes.id);
		BMD_FOK_CHG(getElementWithCondition(_GLOBAL_UserClassesDictionary, 2, anss[i], 3, &tmp2), LIBBMDSQL_DICT_CLASS_VALUE_NOT_FOUND);
		BMD_FOK(bmd_datagram_add_metadata_char(OID_ACTION_METADATA_CERT_CLASS_NAME, tmp2, bmdJSResponse));

		free0(anss[i]);
		free0(tmp2);
	}
	free0(anss);

	/*****************************************************/
	/* pobranie informacji o domylnej klasie uzytkownika */
	/*****************************************************/
	asprintf(&SQLQuery, "SELECT id FROM classes WHERE id IN (SELECT fk_classes FROM users_and_classes WHERE location_id=%s AND  fk_users=%s  AND default_class=1) AND location_id=%s;", _GLOBAL_bmd_configuration->location_id, userCertificateId, _GLOBAL_bmd_configuration->location_id);
	if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}

	ExecuteSQLQueryWithAnswerKnownDBConnection( hDB, SQLQuery, &userDefaultClass);

	if (userDefaultClass)
	{
		BMD_FOK(bmd_datagram_add_metadata_char(OID_ACTION_METADATA_CERT_DEFAULT_CLASS_ID, userDefaultClass, bmdJSResponse));

		//SELECT users_and_classes.fk_users, users_and_classes.default_class, classes.id, classes.name, users_and_classes.fk_users::varchar||'|'||classes.id::varchar FROM classes LEFT JOIN users_and_classes ON (users_and_classes.fk_classes=classes.id);
		BMD_FOK_CHG(getElementWithCondition(_GLOBAL_UserClassesDictionary, 2, userDefaultClass, 3, &tmp2), LIBBMDSQL_DICT_CLASS_VALUE_NOT_FOUND);
		BMD_FOK(bmd_datagram_add_metadata_char(OID_ACTION_METADATA_CERT_DEFAULT_CLASS_NAME, tmp2, bmdJSResponse));

		free0(tmp2);
		free0(userDefaultClass);
	}
	free0(SQLQuery);

	/****************************************************************************/
	/* pobranie informacji o poziomach i kategoriach bezpieczenstwa uzytkownika */
	/****************************************************************************/
	asprintf(&SQLQuery, "SELECT sec_categories.name FROM users_and_security LEFT JOIN sec_categories ON (users_and_security.fk_sec_categories=sec_categories.id) WHERE users_and_security.fk_users=%s AND users_and_security.location_id=%s AND sec_categories.location_id=%s;", userCertificateId, _GLOBAL_bmd_configuration->location_id, _GLOBAL_bmd_configuration->location_id);
	if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}

	BMD_FOK(ExecuteSQLQueryWithAnswersKnownDBConnection( hDB, SQLQuery, &secCatsCount, &secCats));
	free0(SQLQuery);

	asprintf(&SQLQuery, "SELECT sec_levels.name FROM users_and_security LEFT JOIN sec_levels ON (users_and_security.fk_sec_levels=sec_levels.id) WHERE users_and_security.fk_users=%s AND users_and_security.location_id=%s AND sec_levels.location_id=%s;", userCertificateId, _GLOBAL_bmd_configuration->location_id, _GLOBAL_bmd_configuration->location_id);
	if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}

	BMD_FOK(ExecuteSQLQueryWithAnswersKnownDBConnection( hDB, SQLQuery, &secLevelsCount, &secLevels));
	free0(SQLQuery);

	for (i=0; i<secLevelsCount; i++)
	{
		asprintf(&userClearance, "%s|%s", secCats[i], secLevels[i]);
		if (userClearance==NULL)	{	BMD_FOK(NO_MEMORY);	}

		BMD_FOK(bmd_datagram_add_metadata_char(OID_SYS_METADATA_CERT_SEC_CATEGORY_NAME, userClearance, bmdJSResponse));

		free0(userClearance);
		free0(secCats[i]);
		free0(secLevels[i]);

	}

	free0(secCats);
	free0(secLevels);
	free0(SQLQuery);

	/************/
	/* porzadki */
	/************/
	free0(anss);
	free0(SQLQuery);
	free0(certificate);
	free0(userCertificateId);
	free0(userDefaultGroup);
	free0(userDefaultRole);


	return BMD_OK;
}



/**************************************************************/
/*          rejestracja uzytkownika uzytkownikach             */
/**************************************************************/
long JS_bmdDatagramSet_to_registerUser(	void *hDB,
					bmdDatagramSet_t *bmdJSRequestSet,
					bmdDatagramSet_t **bmdJSResponseSet,
					server_request_data_t *req)
{
long status				= 0;
long i				= 0;

	PRINT_INFO("LIBBMDSQLINF Registering user certificate (datagramset)\n");
	/************************/
	/* walidacja parametrow */
	/************************/
	if (hDB==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (bmdJSRequestSet==NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	/*****************************************/
	/* przygotowanie datagramsetu odpowiedzi */
	/*****************************************/
	BMD_FOK(PR2_bmdDatagramSet_fill_with_datagrams(bmdJSRequestSet->bmdDatagramSetSize, bmdJSResponseSet));

	/*************************************************/
	/* obsługa poszczegolnych datagramow z zadaniami */
	/*************************************************/
	for(i=0; i<(int)bmdJSRequestSet->bmdDatagramSetSize; i++)
	{
		/*********************************************************************/
		/* sprawdzenie, czy wczesniej nie wykryto bledu w datagramie zadania */
		/*********************************************************************/
		if (bmdJSRequestSet->bmdDatagramSetTable[i]->datagramStatus<BMD_OK)
		{
			PR_bmdDatagram_PrepareResponse(&((*bmdJSResponseSet)->bmdDatagramSetTable[i]), bmdJSRequestSet->bmdDatagramSetTable[i]->datagramType+100, bmdJSRequestSet->bmdDatagramSetTable[i]->datagramStatus);
			continue;
		}

		/******************************/
		/*	poczatek transakcji	*/
		/******************************/
		BMD_FOK(bmd_db_start_transaction(hDB,BMD_TRANSACTION_LEVEL_NONE));

		/*************************************/
		/* obsluzenie pojedynczego datagramu */
		/*************************************/
		if (bmdJSRequestSet->bmdDatagramSetTable[i]->datagramType==BMD_DATAGRAM_TYPE_REGISTER_USER ||
			bmdJSRequestSet->bmdDatagramSetTable[i]->datagramType==BMD_DATAGRAM_TYPE_ADD_CERTIFICATE_TO_IDENTITY)
		{
			status=JS_bmdDatagram_to_registerUser(hDB,bmdJSRequestSet->bmdDatagramSetTable[i],
				&((*bmdJSResponseSet)->bmdDatagramSetTable[i]),req);

		}
		else if (bmdJSRequestSet->bmdDatagramSetTable[i]->datagramType==BMD_DATAGRAM_TYPE_UPDATE_USER)
		{
			status=JS_bmdDatagram_to_updateUser(hDB,bmdJSRequestSet->bmdDatagramSetTable[i],
				&((*bmdJSResponseSet)->bmdDatagramSetTable[i]),req);

		}

		/**************************************/
		/* ustawienie statusu błędu datagramu */
		/**************************************/
		(*bmdJSResponseSet)->bmdDatagramSetTable[i]->datagramType = bmdJSRequestSet->bmdDatagramSetTable[i]->datagramType+100;
		(*bmdJSResponseSet)->bmdDatagramSetTable[i]->datagramStatus = status;

		/****************************************/
		/* zarejestrowanie akcji w bazie danych */
		/****************************************/
		if (_GLOBAL_bmd_configuration->messaging_plugin_conf.bmd_messaging_plugin_register_action)
		{

			status=_GLOBAL_bmd_configuration->messaging_plugin_conf.bmd_messaging_plugin_register_action(hDB, bmdJSRequestSet->bmdDatagramSetTable[i], (*bmdJSResponseSet)->bmdDatagramSetTable[i], req);
			if (status<BMD_OK)
			{

				(*bmdJSResponseSet)->bmdDatagramSetTable[i]->datagramStatus = status;
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

#ifndef WIN32
	kill(getppid(),SIGDICTRELOAD);
#endif // ifndef WIN32

	return BMD_OK;
}


/**
* funkcja tworzy nowy wpis w tabeli identities
* mozliwe jest opisanie tozsamosci przez identityDescription (NULL oznacza brak opisu definiowanego przez uzytkownika)
* jesli zostanie podany parametr registeredIdentityId (rozny od NULL), to zostanie do niego przypisana wartosc id stworzonego rekordu
*/
long RegisterNewIdentity(void *hDB, char *identityDescription, char **registeredIdentityId)
{
long retVal 		= 0;
char* SQLcurrval	= "SELECT currval('users_id_seq');";
char* SQLTemplate	= "INSERT INTO identities VALUES(nextval('users_id_seq'), '%s', '%s');";
char* SQLQuery		= NULL;
void *queryResult	= NULL;
char* escapedIdentityDescription	= NULL;

	if(hDB == NULL)
		{ return -1; }
	if(identityDescription != NULL)
	{
		if(strlen(identityDescription) >= 256)
			{ return -2; }
	}
	

	if(_GLOBAL_bmd_configuration == NULL || _GLOBAL_bmd_configuration->location_id==NULL)
	{
		return -11;
	}

	// sprawdzenie, czy rejestracja nowej tozsamosci nie doprowadzi do przekroczenia maksymalnej ilosci dozwolonej przez licencje
	retVal = IsPossibleIdentitityRegistration(hDB);
	BMD_FOK(retVal);
	if(retVal == 0) // nie mozna zarejsetrowac kolejnej tozsamosci, bo limit wyczerpany
	{
		BMD_FOK(LIBBMDSQL_USER_LICENSE_FORBID_REGISTER_MORE_IDENTITIES);
	}


	if(identityDescription != NULL)
	{
		retVal=bmd_db_escape_string(hDB, identityDescription, STANDARD_ESCAPING, &escapedIdentityDescription);
		if(retVal < BMD_OK)
		{
			return -30;
		}
	}
	asprintf(&SQLQuery, SQLTemplate, _GLOBAL_bmd_configuration->location_id, (identityDescription!=NULL ? escapedIdentityDescription : ""));
	free(escapedIdentityDescription); escapedIdentityDescription=NULL;
	if(SQLQuery == NULL)
		{ return -12; }

	retVal=bmd_db_execute_sql( hDB, SQLQuery, NULL, NULL, &queryResult);
	free(SQLQuery); SQLQuery=NULL;
	if(retVal < BMD_OK)
	{
		return -13;
	}
	bmd_db_result_free(&queryResult);

	
	if(registeredIdentityId != NULL)
	{
		retVal=bmd_db_execute_sql(hDB, SQLcurrval, NULL, NULL, &queryResult);
		if(retVal != BMD_OK)
			{ return -21; }
		retVal=bmd_db_result_get_value(hDB, queryResult, 0, 0,registeredIdentityId, FETCH_ABSOLUTE, &retVal);
		bmd_db_result_free(&queryResult);
		if(retVal < BMD_OK)
			{ return -22; }
	}

	return 0;
}

/**
rejestracja nowego uzytkownika: BMD_DATAGRAM_TYPE_REGISTER_USER
oraz dodawanie certyfikatu do tozsamosci: BMD_DATAGRAM_TYPE_ADD_CERTIFICATE_TO_IDENTITY
*/
long JS_bmdDatagram_to_registerUser(	void *hDB,
					bmdDatagram_t *bmdJSRequest,
					bmdDatagram_t **bmdJSResponse,
					server_request_data_t *req)
{
long i				= 0;
long h				= 0;
long certificateRolesCount	= 0;
long certificateGroupsCount	= 0;
long certificateClassesCount	= 0;
long certificateClearanceCount	= 0;
long userDefaultRoleIdFound	= 0;
long userDefaultGroupIdFound	= 0;
long userDefaultClassIdFound	= 0;
void *query_result		= NULL;
char *SQLQuery			= NULL;
char **certificateRoles		= NULL;
char **certificateGroups	= NULL;
char **certificateClasses	= NULL;
char ***certificateClearance	= NULL;
char *certificateIssuer		= NULL;
char *certificateSerial		= NULL;
char *certificateName		= NULL;
char *certificateIdentity	= NULL;
char *roleName			= NULL;
char *groupName			= NULL;
char *className			= NULL;
char *secCategoryId		= NULL;
char *secLevelId		= NULL;
char **secCategory		= NULL;
char *tmpClearance		= NULL;
char *userDefaultRoleId		= NULL;
char *userDefaultGroupId	= NULL;
char *userDefaultClassId	= NULL;
char *userId			= NULL;
char *newGroupName		= NULL;
char *newGroupId		= NULL;
char **newGroupParentIds	= NULL;
char **newGroupChildIds		= NULL;
long newGroupParentCount	= 0;
long newGroupChildCount		= 0;
char *identityId		= NULL;
long retVal			= 0;
char* escapedCertName	= NULL;
char* escapedCertIssuer	= NULL;
char* escapedCertSerial	= NULL;
char* escapeTemp		= NULL;


	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if (hDB==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (bmdJSRequest==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	if(bmdJSRequest->datagramType == BMD_DATAGRAM_TYPE_ADD_CERTIFICATE_TO_IDENTITY)
	{
		if(bmdJSRequest->protocolDataFileId == NULL)
			{ BMD_FOK(BMDSOAP_SERVER_USER_IDENTITY_UNDEFINED); }
		if(bmdJSRequest->protocolDataFileId->size <=0 || bmdJSRequest->protocolDataFileId->buf == NULL)
			{ BMD_FOK(BMDSOAP_SERVER_USER_IDENTITY_UNDEFINED); }
		
		identityId=strdup(bmdJSRequest->protocolDataFileId->buf);
		if(identityId == NULL)
			{ BMD_FOK(BMD_ERR_MEMORY); }
	
		retVal=CheckIfIdentityExists(hDB, identityId);
		if(retVal < BMD_OK)
		{
			free(identityId);
			BMD_FOK(retVal);
		}
	}


	/******************************************************/
	/* poszukiwanie poszczególnych elementów w datagramie */
	/******************************************************/
	for (i=0; i<bmdJSRequest->no_of_actionMetaData; i++)
	{
		/**********************************************/
		/* odczytanie klas rejestrowanego certyfikatu */
		/**********************************************/
		if (strcmp(bmdJSRequest->actionMetaData[i]->OIDTableBuf, OID_ACTION_METADATA_CERT_CLASS_ID)==0)
		{
			certificateClassesCount++;
			certificateClasses=(char**)realloc(certificateClasses, sizeof(char *) * certificateClassesCount);
			asprintf(&certificateClasses[certificateClassesCount-1], "%s", bmdJSRequest->actionMetaData[i]->AnyBuf);
			if (certificateClasses[certificateClassesCount-1]==NULL)	{	BMD_FOK(NO_MEMORY);	}
		}
		/*********************************************************/
		/* odczytanie domyślnej klasy rejestrowanego certyfikatu */
		/*********************************************************/
		else if (strcmp(bmdJSRequest->actionMetaData[i]->OIDTableBuf, OID_ACTION_METADATA_CERT_DEFAULT_CLASS_ID)==0)
		{
			if (userDefaultClassId)		{	BMD_FOK(LIBBMDSQL_USER_DEFAULT_CLASS_TOO_MANY);	}
			asprintf(&userDefaultClassId, "%s", bmdJSRequest->actionMetaData[i]->AnyBuf);
			if (userDefaultClassId==NULL)	{	BMD_FOK(NO_MEMORY);	}
		}
	}

	/******************************************************/
	/* poszukiwanie poszczególnych elementów w datagramie */
	/******************************************************/
	for (i=0; i<bmdJSRequest->no_of_sysMetaData; i++)
	{
		/**************************************************/
		/* odczytanie wystawcy rejestrowanego certyfikatu */
		/**************************************************/
		if (strcmp(bmdJSRequest->sysMetaData[i]->OIDTableBuf, OID_SYS_METADATA_CERT_USER_ISSUER)==0)
		{
			if (certificateIssuer!=NULL)	{	BMD_FOK(LIBBMDSQL_USER_ISSUER_TOO_MANY);	}
			asprintf(&certificateIssuer, "%s", bmdJSRequest->sysMetaData[i]->AnyBuf);
			if (certificateIssuer==NULL)	{	BMD_FOK(NO_MEMORY);	}
		}
		/*************************************************/
		/* odczytanie seriala rejestrowanego certyfikatu */
		/*************************************************/
		else if (strcmp(bmdJSRequest->sysMetaData[i]->OIDTableBuf, OID_SYS_METADATA_CERT_USER_SN)==0)
		{

			if (certificateSerial)		{	BMD_FOK(LIBBMDSQL_USER_SN_TOO_MANY);	}
			asprintf(&certificateSerial, "%s", bmdJSRequest->sysMetaData[i]->AnyBuf);
			if (certificateSerial==NULL)	{	BMD_FOK(NO_MEMORY);	}
		}
		/***********************************************************/
		/* odczytanie nazwy uśytkownika rejestrowanego certyfikatu */
		/***********************************************************/
		else if (strcmp(bmdJSRequest->sysMetaData[i]->OIDTableBuf, OID_SYS_METADATA_CERT_USER_NAME)==0)
		{

			if (certificateName)		{	BMD_FOK(LIBBMDSQL_USER_NAME_TOO_MANY);	}
			asprintf(&certificateName, "%s", bmdJSRequest->sysMetaData[i]->AnyBuf);
			if (certificateName==NULL)	{	BMD_FOK(NO_MEMORY);	}
		}
		/********************************************************/
		/* odczytanie opisu tozsamosci dla rejestrowanego certyfikatu */
		/********************************************************/
		else if (strcmp(bmdJSRequest->sysMetaData[i]->OIDTableBuf, OID_SYS_METADATA_CERT_USER_IDENTITY)==0)
		{

			if (certificateIdentity)	{	BMD_FOK(LIBBMDSQL_USER_IDENTITY_TOO_MANY);	}
			asprintf(&certificateIdentity, "%s", bmdJSRequest->sysMetaData[i]->AnyBuf);
			if (certificateIdentity==NULL){	BMD_FOK(NO_MEMORY);	}
		}
		/*********************************************/
		/* odczytanie ról rejestrowanego certyfikatu */
		/*********************************************/
		else if (strcmp(bmdJSRequest->sysMetaData[i]->OIDTableBuf, OID_SYS_METADATA_CERT_ROLE_ID)==0)
		{
			certificateRolesCount++;
			certificateRoles=(char**)realloc(certificateRoles, sizeof(char *) * certificateRolesCount);
			asprintf(&certificateRoles[certificateRolesCount-1], "%s", bmdJSRequest->sysMetaData[i]->AnyBuf);
			if (certificateRoles[certificateRolesCount-1]==NULL)	{	BMD_FOK(NO_MEMORY);	}
		}
		/**********************************************/
		/* odczytanie grup rejestrowanego certyfikatu */
		/**********************************************/
		else if (strcmp(bmdJSRequest->sysMetaData[i]->OIDTableBuf, OID_SYS_METADATA_CERT_GROUP_ID)==0)
		{
			certificateGroupsCount++;
			certificateGroups=(char**)realloc(certificateGroups, sizeof(char *) * certificateGroupsCount);
			asprintf(&certificateGroups[certificateGroupsCount-1], "%s", bmdJSRequest->sysMetaData[i]->AnyBuf);
			if (certificateGroups[certificateGroupsCount-1]==NULL)	{	BMD_FOK(NO_MEMORY);	}
		}
		/*****************************************************************************/
		/* odczytanie kategorii i poziomów bezpieczeństwa rejestrowanego certyfikatu */
		/*****************************************************************************/
		else if (strcmp(bmdJSRequest->sysMetaData[i]->OIDTableBuf, OID_SYS_METADATA_CERT_SEC_CATEGORY_NAME)==0)
		{

			asprintf(&tmpClearance, "%s", bmdJSRequest->sysMetaData[i]->AnyBuf);
			if (tmpClearance==NULL)	{	BMD_FOK(NO_MEMORY);	}

			certificateClearanceCount++;
			if (certificateClearanceCount==1)
			{
				certificateClearance=(char***)malloc(sizeof(char**)*2);
				if (certificateClearance==NULL) {	BMD_FOK(NO_MEMORY);	}
				memset(certificateClearance, 0, sizeof(char**)*2);
			}

			bmd_strsep(	tmpClearance, '|', &secCategory, &h);

			certificateClearance[0]=(char**)realloc(certificateClearance[0], sizeof(char *)*certificateClearanceCount);
			if (certificateClearance[0]==NULL)	{	BMD_FOK(NO_MEMORY);	}

			certificateClearance[1]=(char**)realloc(certificateClearance[1], sizeof(char *)*certificateClearanceCount);
			if (certificateClearance[1]==NULL)	{	BMD_FOK(NO_MEMORY);	}

			asprintf(&(certificateClearance[0][certificateClearanceCount-1]), "%s", secCategory[0]);
			if (certificateClearance[0][certificateClearanceCount-1]==NULL)	{	BMD_FOK(NO_MEMORY);	}

			asprintf(&(certificateClearance[1][certificateClearanceCount-1]), "%s", secCategory[1]);
			if (certificateClearance[1][certificateClearanceCount-1]==NULL)	{	BMD_FOK(NO_MEMORY);	}

			free(secCategory[0]); secCategory[0]=NULL;
			free(secCategory[1]); secCategory[1]=NULL;
			free(secCategory); secCategory=NULL;
		}
		/*********************************************************/
		/* odczytanie domyślnej grupy rejestrowanego certyfikatu */
		/*********************************************************/
		else if (strcmp(bmdJSRequest->sysMetaData[i]->OIDTableBuf, OID_SYS_METADATA_CERT_DEFAULT_GROUP_ID)==0)
		{
			if (userDefaultGroupId)		{	BMD_FOK(LIBBMDSQL_USER_DEFAULT_GROUP_TOO_MANY);	}
			asprintf(&userDefaultGroupId, "%s", bmdJSRequest->sysMetaData[i]->AnyBuf);
			if (userDefaultGroupId==NULL)	{	BMD_FOK(NO_MEMORY);	}
		}
		/********************************************************/
		/* odczytanie domyślnej roli rejestrowanego certyfikatu */
		/********************************************************/
		else if (strcmp(bmdJSRequest->sysMetaData[i]->OIDTableBuf, OID_SYS_METADATA_CERT_DEFAULT_ROLE_ID)==0)
		{
			if (userDefaultRoleId)		{	BMD_FOK(LIBBMDSQL_USER_DEFAULT_ROLE_TOO_MANY);	}
			asprintf(&userDefaultRoleId, "%s", bmdJSRequest->sysMetaData[i]->AnyBuf);
			if (userDefaultRoleId==NULL)	{	BMD_FOK(NO_MEMORY);	}
		}
		else if (strcmp(bmdJSRequest->sysMetaData[i]->OIDTableBuf, OID_SYS_METADATA_CERT_NEW_GROUP_NAME)==0)
		{
			if (newGroupName)	{	BMD_FOK(LIBBMDSQL_USER_NEW_GROUP_NAME_TOO_MANY);	}
			asprintf(&newGroupName, "%s", bmdJSRequest->sysMetaData[i]->AnyBuf);
			if (newGroupName==NULL)	{	BMD_FOK(NO_MEMORY);	}
		}
		else if (strcmp(bmdJSRequest->sysMetaData[i]->OIDTableBuf, OID_SYS_METADATA_CERT_NEW_GROUP_PARENT_ID)==0)
		{
			newGroupParentCount++;
			newGroupParentIds=(char**)realloc(newGroupParentIds, sizeof(char *) * newGroupParentCount);
			asprintf(&newGroupParentIds[newGroupParentCount-1], "%s", bmdJSRequest->sysMetaData[i]->AnyBuf);
			if (newGroupParentIds[newGroupParentCount-1]==NULL)	{	BMD_FOK(NO_MEMORY);	}
		}
		else if (strcmp(bmdJSRequest->sysMetaData[i]->OIDTableBuf, OID_SYS_METADATA_CERT_NEW_GROUP_CHILD_ID)==0)
		{
			newGroupChildCount++;
			newGroupChildIds=(char**)realloc(newGroupChildIds, sizeof(char *) * newGroupChildCount);
			asprintf(&newGroupChildIds[newGroupChildCount-1], "%s", bmdJSRequest->sysMetaData[i]->AnyBuf);
			if (newGroupChildIds[newGroupChildCount-1]==NULL)	{	BMD_FOK(NO_MEMORY);	}
		}
	}

	if (certificateIssuer==NULL)	{	BMD_FOK(LIBBMDSQL_USER_ISSUER_UNDEFINED);	}
	if (certificateSerial==NULL)	{	BMD_FOK(LIBBMDSQL_USER_SN_UNDEFINED);	}
	if (certificateName==NULL)	{	BMD_FOK(LIBBMDSQL_USER_NAME_UNDEFINED);	}
	if (userDefaultRoleId==NULL)	{	BMD_FOK(LIBBMDSQL_USER_DEFAULT_ROLE_UNDEFINED);	}
	if (userDefaultClassId==NULL)	{	BMD_FOK(LIBBMDSQL_USER_DEFAULT_CLASS_UNDEFINED);	}
	if (certificateRolesCount==0)	{	BMD_FOK(LIBBMDSQL_USER_ROLE_UNDEFINED);	}
	if (certificateClassesCount==0)	{	BMD_FOK(LIBBMDSQL_USER_CLASS_UNDEFINED);	}
	if ((userDefaultGroupId==NULL) && (newGroupName==NULL))	{	BMD_FOK(LIBBMDSQL_USER_DEFAULT_GROUP_UNDEFINED);	}
	if ((certificateGroupsCount==0) && (newGroupName==NULL)){	BMD_FOK(LIBBMDSQL_USER_GROUP_UNDEFINED);	}

	/**********************************************************************/
	/* sprawdzenie, czy certyfikat nie jest juz zarejestrowany w archiwum */
	/**********************************************************************/
	BMD_FOK(CheckIfCertInfoFree(	certificateIssuer, certificateSerial));
	BMD_FOK(CheckUserNumber());

	if(bmdJSRequest->datagramType != BMD_DATAGRAM_TYPE_ADD_CERTIFICATE_TO_IDENTITY)
	{
		PRINT_INFO("LIBBMDSQLINF Registering new identity\n");
		BMD_FOK(RegisterNewIdentity(hDB, certificateIdentity, &identityId));
	}
	
	/*****************************************/
	/* wstawienie informacji do tabeli users */
	/*****************************************/
	// @author WSZ
	// Tabele identities i users korzystaja z tej samej sekwencji tj. users_id_seq.
	// Jesli rejestrowany jest nowy uzytkownik (certyfikat), to otrzymuje id rowne id stworzonej tozsamosci (dlatego currval()).
	// Jesli dodawany jest certyfikat (uzytkownik) do istniejacej tozsamosci, to jego id musi byc rozne od id tozsamosci (dlatego nextval()).
	BMD_FOK(bmd_db_escape_string(hDB, certificateName, STANDARD_ESCAPING, &escapedCertName));
	BMD_FOK(bmd_db_escape_string(hDB, certificateSerial, STANDARD_ESCAPING, &escapedCertSerial));
	BMD_FOK(bmd_db_escape_string(hDB, certificateIssuer, STANDARD_ESCAPING, &escapedCertIssuer));
	asprintf(&SQLQuery, 	"INSERT INTO users VALUES (%s,%s,'%s','%s','%s','%s',1);",
					(bmdJSRequest->datagramType != BMD_DATAGRAM_TYPE_ADD_CERTIFICATE_TO_IDENTITY ? "currval('users_id_seq')" : "nextval('users_id_seq')"), 
					_GLOBAL_bmd_configuration->location_id==NULL?"DEFAULT":_GLOBAL_bmd_configuration->location_id,
					identityId,
					escapedCertName,
					escapedCertSerial,
					escapedCertIssuer);
	free(escapedCertName); escapedCertName=NULL;
	free(escapedCertSerial); escapedCertSerial=NULL;
	free(escapedCertIssuer); escapedCertIssuer=NULL;
	if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}

	BMD_FOK(bmd_db_execute_sql( hDB, SQLQuery,NULL,NULL, &query_result));

	bmd_db_result_free(&query_result);
	free0(SQLQuery);

	/*********************************/
	/* pobranie aktualnego id user'a */
	/*********************************/
	BMD_FOK(bmd_db_get_max_entry(hDB, "id", "users", &userId));

	/**********************************/
	/* wstawienie informacji o rolach */
	/**********************************/
	BMD_FOK(CheckIfRoleExist( userDefaultRoleId, &roleName));
	free0(roleName);

	for (i=0; i<certificateRolesCount; i++)
	{
		BMD_FOK(CheckIfRoleExist( certificateRoles[i], &roleName));

		if (strcmp(certificateRoles[i], userDefaultRoleId)==0)
		{
			asprintf(&SQLQuery, 	"INSERT INTO users_and_roles VALUES (nextval('users_and_roles_id_seq'),%s,currval('users_id_seq'),%s,1);",
							_GLOBAL_bmd_configuration->location_id==NULL?"DEFAULT":_GLOBAL_bmd_configuration->location_id,
							certificateRoles[i]);
			userDefaultRoleIdFound=1;
		}
		else
		{
			asprintf(&SQLQuery, 	"INSERT INTO users_and_roles VALUES (nextval('users_and_roles_id_seq'),%s,currval('users_id_seq'),%s,0);",
							_GLOBAL_bmd_configuration->location_id==NULL?"DEFAULT":_GLOBAL_bmd_configuration->location_id,
							certificateRoles[i]);
		}
		if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}

		BMD_FOK_CHG(bmd_db_execute_sql( hDB, SQLQuery,NULL,NULL, &query_result), LIBBMDSQL_USER_ROLE_ERROR);
		bmd_db_result_free(&query_result);

		bmd_db_result_free(&query_result);
		free0(SQLQuery);
		free0(roleName);
		free0(certificateRoles[i]);

	}
	free0(certificateRoles);

	if (userDefaultRoleIdFound!=1)	{	BMD_FOK(LIBBMDSQL_USER_DEFAULT_ROLE_NOT_ROLE);	}

	/**********************************************************************************/
	/* stworzenie nowej grupy dla uzytkownika, jesli taka zostala okreslona w zadaniu */
	/**********************************************************************************/
	if (newGroupName!=NULL)
	{
		/*******************************/
		/* odznaczenie grupy domyslnej */
		/*******************************/
		asprintf(&SQLQuery, 	"UPDATE users_and_groups SET default_group=0 WHERE fk_users=%s%s%s;",
					userId,
					_GLOBAL_bmd_configuration->location_id==NULL?"":" AND location_id=",
					_GLOBAL_bmd_configuration->location_id==NULL?"":_GLOBAL_bmd_configuration->location_id);
		if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}

		BMD_FOK(bmd_db_execute_sql( hDB, SQLQuery,NULL,NULL, &query_result));

		bmd_db_result_free(&query_result);
		free0(SQLQuery);

		/***************************/
		/* rejestracja nowej grupy */
		/***************************/
		BMD_FOK(bmd_db_escape_string(hDB, newGroupName, STANDARD_ESCAPING, &escapeTemp));
		asprintf(&SQLQuery, 	"INSERT INTO groups VALUES (nextval('groups_id_seq'),%s,'%s');",
					_GLOBAL_bmd_configuration->location_id==NULL?"DEFAULT":_GLOBAL_bmd_configuration->location_id,
					escapeTemp);
		free(escapeTemp); escapeTemp=NULL;
		if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}

		BMD_FOK(bmd_db_execute_sql( hDB, SQLQuery,NULL,NULL, &query_result));

		bmd_db_result_free(&query_result);
		free0(SQLQuery);

		/***************************/
		/* pobranie id nowej grupy */
		/***************************/
		asprintf(&SQLQuery, 	"SELECT currval('groups_id_seq');");
		if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}
		BMD_FOK(ExecuteSQLQueryWithAnswerKnownDBConnection(hDB, SQLQuery, &newGroupId));

		free0(SQLQuery);

		/***************************************************/
		/* powiazanie grupy z uzytkownikiem jako domyslnej */
		/***************************************************/
		asprintf(&SQLQuery, 	"INSERT INTO users_and_groups VALUES (nextval('users_and_groups_id_seq'),%s,currval('users_id_seq'),currval('groups_id_seq'),1);",
					_GLOBAL_bmd_configuration->location_id==NULL?"DEFAULT":_GLOBAL_bmd_configuration->location_id);
		if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}

		BMD_FOK(bmd_db_execute_sql( hDB, SQLQuery,NULL,NULL, &query_result));

		bmd_db_result_free(&query_result);
		free0(SQLQuery);

		/*************************************************************/
		/* wpisanie informacji o nowej grupie do drzewa groups_graph */
		/*************************************************************/
		if ((newGroupParentCount>0) && (newGroupParentIds!=NULL))
		{
			for (i=0; i<newGroupParentCount; i++)
			{
				if (newGroupParentIds[i]!=NULL)
				{
					BMD_FOK(bmd_db_escape_string(hDB, newGroupParentIds[i], STANDARD_ESCAPING, &escapeTemp));
					asprintf(&SQLQuery, 	"INSERT INTO groups_graph VALUES (nextval('groups_graph_id_seq'),%s,%s,currval('groups_id_seq'));",
								_GLOBAL_bmd_configuration->location_id==NULL?"DEFAULT":_GLOBAL_bmd_configuration->location_id,
								escapeTemp);
					free(escapeTemp); escapeTemp=NULL;
					if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}

					BMD_FOK(bmd_db_execute_sql( hDB, SQLQuery,NULL,NULL, &query_result));
					bmd_db_result_free(&query_result);
					free0(SQLQuery);
					free0(newGroupParentIds[i]);
				}
			}
			free0(newGroupParentIds);
		}

		if ((newGroupChildCount>0) && (newGroupChildIds!=NULL))
		{
			for (i=0; i<newGroupChildCount; i++)
			{
				if (newGroupChildIds[i]!=NULL)
				{
					BMD_FOK(bmd_db_escape_string(hDB, newGroupChildIds[i], STANDARD_ESCAPING, &escapeTemp));
					asprintf(&SQLQuery, 	"INSERT INTO groups_graph VALUES (nextval('groups_graph_id_seq'),%s,currval('groups_id_seq'),%s);",
								_GLOBAL_bmd_configuration->location_id==NULL?"DEFAULT":_GLOBAL_bmd_configuration->location_id,
								escapeTemp);
					free(escapeTemp); escapeTemp=NULL;
					if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}

					BMD_FOK(bmd_db_execute_sql( hDB, SQLQuery,NULL,NULL, &query_result));
					bmd_db_result_free(&query_result);
					free0(SQLQuery);
					free0(newGroupChildIds[i]);
				}
			}
			free0(newGroupChildIds);
		}
	}

	/***********************************/
	/* wstawienie informacji o grupach */
	/***********************************/
	if ((userDefaultGroupId==NULL) || ((userDefaultGroupId!=NULL) && (strcmp(userDefaultGroupId, "-1")==0)))
	{

		if (newGroupId==NULL)	{	BMD_FOK(LIBBMDSQL_USER_DEFAULT_GROUP_UNDEFINED);	}

		free0(userDefaultGroupId);
		asprintf(&userDefaultGroupId, "%s", newGroupId);
		userDefaultGroupIdFound=1;
		free0(newGroupId);
	}
	else
	{

		BMD_FOK(CheckIfGroupExist( userDefaultGroupId, &groupName));
		free0(groupName);
	}

	for (i=0; i<certificateGroupsCount; i++)
	{

		BMD_FOK(CheckIfGroupExist( certificateGroups[i], &groupName));

		if (strcmp(certificateGroups[i], userDefaultGroupId)==0)
		{
			asprintf(&SQLQuery, 	"INSERT INTO users_and_groups VALUES (nextval('users_and_groups_id_seq'),%s,currval('users_id_seq'),%s,1);",
							_GLOBAL_bmd_configuration->location_id==NULL?"DEFAULT":_GLOBAL_bmd_configuration->location_id,
							certificateGroups[i]);
			userDefaultGroupIdFound=1;
		}
		else
		{
			asprintf(&SQLQuery, 	"INSERT INTO users_and_groups VALUES (nextval('users_and_groups_id_seq'),%s,currval('users_id_seq'),%s,0);",
							_GLOBAL_bmd_configuration->location_id==NULL?"DEFAULT":_GLOBAL_bmd_configuration->location_id,
							certificateGroups[i]);
		}
		if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}

		BMD_FOK_CHG(bmd_db_execute_sql( hDB, SQLQuery,NULL,NULL, &query_result), LIBBMDSQL_USER_GROUP_ERROR);
		bmd_db_result_free(&query_result);

		bmd_db_result_free(&query_result);
		free0(SQLQuery);
		free0(groupName);
		free0(certificateGroups[i]);
	}
	free0(certificateGroups);

	if (userDefaultGroupIdFound!=1)	{	BMD_FOK(LIBBMDSQL_USER_DEFAULT_GROUP_NOT_GROUP);	}

	/***********************************/
	/* wstawienie informacji o klasach */
	/***********************************/
	BMD_FOK(CheckIfClassExist( userDefaultClassId, &className));
	free0(className);

	for (i=0; i<certificateClassesCount; i++)
	{
		BMD_FOK(CheckIfClassExist( certificateClasses[i], &className));

		if (strcmp(certificateClasses[i], userDefaultClassId)==0)
		{
			asprintf(&SQLQuery, 	"INSERT INTO users_and_classes VALUES (nextval('users_and_classes_id_seq'),%s,currval('users_id_seq'),%s,1);",
							_GLOBAL_bmd_configuration->location_id==NULL?"DEFAULT":_GLOBAL_bmd_configuration->location_id,
							certificateClasses[i]);
			userDefaultClassIdFound=1;
		}
		else
		{
			asprintf(&SQLQuery, 	"INSERT INTO users_and_classes VALUES (nextval('users_and_classes_id_seq'),%s,currval('users_id_seq'),%s,0);",
							_GLOBAL_bmd_configuration->location_id==NULL?"DEFAULT":_GLOBAL_bmd_configuration->location_id,
							certificateClasses[i]);
		}
		if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}

		BMD_FOK_CHG(bmd_db_execute_sql( hDB, SQLQuery,NULL,NULL, &query_result), LIBBMDSQL_USER_CLASS_ERROR);
		bmd_db_result_free(&query_result);

		bmd_db_result_free(&query_result);
		free0(SQLQuery);
		free0(className);
		free0(certificateClasses[i]);
	}
	free0(certificateClasses);

	if (userDefaultClassIdFound!=1)	{	BMD_FOK(LIBBMDSQL_USER_DEFAULT_CLASS_NOT_CLASS);	}

	/******************************************************************/
	/* wstawienie informacji o kategoriach i poziomach bezpieczenstwa */
	/******************************************************************/
	if (certificateClearanceCount>0)
	{
		for (i=0; i<certificateClearanceCount; i++)
		{
			BMD_FOK(CheckIfSecCategoryExist( certificateClearance[0][i], &secCategoryId));
			BMD_FOK(CheckIfSecLevelExist( certificateClearance[1][i], &secLevelId));

			asprintf(&SQLQuery, 	"INSERT INTO users_and_security VALUES (nextval('users_and_security_id_seq'),%s,currval('users_id_seq'),%s,%s);",
							_GLOBAL_bmd_configuration->location_id==NULL?"DEFAULT":_GLOBAL_bmd_configuration->location_id,
							secCategoryId,
							secLevelId);
			if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}

			BMD_FOK_CHG(bmd_db_execute_sql( hDB, SQLQuery,NULL,NULL, &query_result),
					LIBBMDSQL_USER_SECURITY_ERROR);
			bmd_db_result_free(&query_result);

			bmd_db_result_free(&query_result);

			free0(SQLQuery);
			free0(secCategoryId);
			free0(secLevelId);
			free0(certificateClearance[0][i]);
			free0(certificateClearance[1][i]);

		}
	}
	else
	{
		asprintf(&SQLQuery, 	"INSERT INTO users_and_security VALUES (nextval('users_and_security_id_seq'),%s,currval('users_id_seq'),1,1);",
						_GLOBAL_bmd_configuration->location_id==NULL?"DEFAULT":_GLOBAL_bmd_configuration->location_id);
		if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}

		BMD_FOK_CHG(bmd_db_execute_sql( hDB, SQLQuery,NULL,NULL, &query_result), LIBBMDSQL_USER_SECURITY_ERROR);

		bmd_db_result_free(&query_result);
		free0(SQLQuery);
	}
	free0(certificateClearance);

	BMD_FOK(bmd_datagram_add_metadata_char(OID_SYS_METADATA_CERT_USER_ID, userId, bmdJSResponse));
	BMD_FOK(bmd_datagram_add_metadata_char(OID_SYS_METADATA_CERT_USER_IDENTITY, identityId, bmdJSResponse));



	/************/
	/* porzadki */
	/************/
	bmd_db_result_free(&query_result);
	free0(newGroupId);
	free0(SQLQuery);
	free0(certificateRoles);
	free0(certificateGroups);
	free0(certificateClasses);
	free0(certificateClearance);
	free0(certificateIssuer);
	free0(certificateSerial);
	free0(certificateName);
	free0(certificateIdentity);
	free0(roleName);
	free0(groupName);
	free0(className);
	free0(secCategoryId);
	free0(secLevelId);
	free0(secCategory);
	free0(tmpClearance);
	free0(userDefaultRoleId);
	free0(userDefaultGroupId);
	free0(userDefaultClassId);
	free0(userId);
	free0(identityId);
	free0(newGroupName);

	return BMD_OK;
}



/***************************************************************/
/*          update danych i praw dostepu uzytkownikow          */
/***************************************************************/
long JS_bmdDatagram_to_updateUser(	void *hDB,
					bmdDatagram_t *bmdJSRequest,
					bmdDatagram_t **bmdJSResponse,
					server_request_data_t *req)
{
long i					= 0;
long h					= 0;
long certificateRolesCount		= 0;
long certificateGroupsCount		= 0;
long certificateClassCount		= 0;
long certificateClearanceCount		= 0;
void *query_result			= NULL;
char *SQLQuery				= NULL;
char **certificateRoles			= NULL;
char **certificateGroups		= NULL;
char **certificateClass			= NULL;
char ***certificateClearance		= NULL;
char *certificateIssuer			= NULL;
char *certificateSerial			= NULL;
char *certificateName			= NULL;
char *certificateIdentity		= NULL;
char **secCategory			= NULL;
char *tmpClearance			= NULL;
char *secLevelId			= NULL;
char *secCategoryId			= NULL;
char *userId				= NULL;
char *userDefaultGroupId		= NULL;
char *userDefaultRoleId			= NULL;
char *userDefaultClassId		= NULL;
char *roleName				= NULL;
char *groupName				= NULL;
char *className				= NULL;
char* escapeTemp			= NULL;

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if (hDB==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (bmdJSRequest==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	/********************************************************************/
	/* sprawdzenie dentyfikatora uzytkownika, ktorego chcemy updateowac */
	/********************************************************************/
	if (	(bmdJSRequest->protocolDataFileId!=NULL) &&
		(bmdJSRequest->protocolDataFileId->buf!=NULL) &&
		(bmdJSRequest->protocolDataFileId->size>0) )
	{
		asprintf(&userId, "%s", bmdJSRequest->protocolDataFileId->buf);
		if (userId==NULL)	{	BMD_FOK(NO_MEMORY);	}
	}

	if (userId==NULL)	{	BMD_FOK(LIBBMDSQL_USER_ID_UNDEFINED);	}

	/****************************************/
	/* sprawdzenie, czy uzytkownik istnieje */
	/****************************************/
	BMD_FOK(CheckIfUserExist(userId));

	/******************************************************/
	/* poszukiwanie poszczególnych elementów w datagramie */
	/******************************************************/
	for (i=0; i<bmdJSRequest->no_of_actionMetaData; i++)
	{
		/**********************************************/
		/* odczytanie klas rejestrowanego certyfikatu */
		/**********************************************/
		if (strcmp(bmdJSRequest->actionMetaData[i]->OIDTableBuf, OID_ACTION_METADATA_CERT_CLASS_ID)==0)
		{
			certificateClassCount++;
			certificateClass=(char**)realloc(certificateClass, sizeof(char *) * certificateClassCount);
			asprintf(&certificateClass[certificateClassCount-1], "%s", bmdJSRequest->actionMetaData[i]->AnyBuf);
			if (certificateClass[certificateClassCount-1]==NULL)	{	BMD_FOK(NO_MEMORY);	}
		}
		/*********************************************************/
		/* odczytanie domyślnej klasy rejestrowanego certyfikatu */
		/*********************************************************/
		else if (strcmp(bmdJSRequest->actionMetaData[i]->OIDTableBuf, OID_ACTION_METADATA_CERT_DEFAULT_CLASS_ID)==0)
		{
			if (userDefaultClassId)		{	BMD_FOK(LIBBMDSQL_USER_DEFAULT_ROLE_TOO_MANY);	}
			asprintf(&userDefaultClassId, "%s", bmdJSRequest->actionMetaData[i]->AnyBuf);
			if (userDefaultClassId==NULL)	{	BMD_FOK(NO_MEMORY);	}
		}
	}

	/******************************************************/
	/* poszukiwanie poszczególnych elementów w datagramie */
	/******************************************************/
	for (i=0; i<bmdJSRequest->no_of_sysMetaData; i++)
	{
		/**************************************************/
		/* odczytanie wystawcy rejestrowanego certyfikatu */
		/**************************************************/
		if (strcmp(bmdJSRequest->sysMetaData[i]->OIDTableBuf, OID_SYS_METADATA_CERT_USER_ISSUER)==0)
		{

			if (certificateIssuer!=NULL)	{	BMD_FOK(LIBBMDSQL_USER_ISSUER_TOO_MANY);	}
			asprintf(&certificateIssuer, "%s", bmdJSRequest->sysMetaData[i]->AnyBuf);
			if (certificateIssuer==NULL)	{	BMD_FOK(NO_MEMORY);	}
		}
		/*************************************************/
		/* odczytanie seriala rejestrowanego certyfikatu */
		/*************************************************/
		else if (strcmp(bmdJSRequest->sysMetaData[i]->OIDTableBuf, OID_SYS_METADATA_CERT_USER_SN)==0)
		{

			if (certificateSerial)		{	BMD_FOK(LIBBMDSQL_USER_SN_TOO_MANY);	}
			asprintf(&certificateSerial, "%s", bmdJSRequest->sysMetaData[i]->AnyBuf);
			if (certificateSerial==NULL)	{	BMD_FOK(NO_MEMORY);	}
		}
		/***********************************************************/
		/* odczytanie nazwy uśytkownika rejestrowanego certyfikatu */
		/***********************************************************/
		else if (strcmp(bmdJSRequest->sysMetaData[i]->OIDTableBuf, OID_SYS_METADATA_CERT_USER_NAME)==0)
		{

			if (certificateName)		{	BMD_FOK(LIBBMDSQL_USER_NAME_TOO_MANY);	}
			asprintf(&certificateName, "%s", bmdJSRequest->sysMetaData[i]->AnyBuf);
			if (certificateName==NULL)	{	BMD_FOK(NO_MEMORY);	}
		}
		/********************************************************/
		/* odczytanie identyfikatora rejestrowanego certyfikatu */
		/********************************************************/
		else if (strcmp(bmdJSRequest->sysMetaData[i]->OIDTableBuf, OID_SYS_METADATA_CERT_USER_IDENTITY)==0)
		{

			if (certificateIdentity)	{	BMD_FOK(LIBBMDSQL_USER_IDENTITY_TOO_MANY);	}
			asprintf(&certificateIdentity, "%s", bmdJSRequest->sysMetaData[i]->AnyBuf);
			if (certificateIdentity==NULL){	BMD_FOK(NO_MEMORY);	}
		}
		/*********************************************/
		/* odczytanie ról rejestrowanego certyfikatu */
		/*********************************************/
		else if (strcmp(bmdJSRequest->sysMetaData[i]->OIDTableBuf, OID_SYS_METADATA_CERT_ROLE_ID)==0)
		{
			certificateRolesCount++;
			certificateRoles=(char**)realloc(certificateRoles, sizeof(char *) * certificateRolesCount);
			asprintf(&certificateRoles[certificateRolesCount-1], "%s", bmdJSRequest->sysMetaData[i]->AnyBuf);
			if (certificateRoles[certificateRolesCount-1]==NULL)	{	BMD_FOK(NO_MEMORY);	}
		}
		/**********************************************/
		/* odczytanie grup rejestrowanego certyfikatu */
		/**********************************************/
		else if (strcmp(bmdJSRequest->sysMetaData[i]->OIDTableBuf, OID_SYS_METADATA_CERT_GROUP_ID)==0)
		{
			certificateGroupsCount++;
			certificateGroups=(char**)realloc(certificateGroups, sizeof(char *) * certificateGroupsCount);
			asprintf(&certificateGroups[certificateGroupsCount-1], "%s", bmdJSRequest->sysMetaData[i]->AnyBuf);
			if (certificateGroups[certificateGroupsCount-1]==NULL)	{	BMD_FOK(NO_MEMORY);	}
		}
		/*****************************************************************************/
		/* odczytanie kategorii i poziomów bezpieczeństwa rejestrowanego certyfikatu */
		/*****************************************************************************/
		else if (strcmp(bmdJSRequest->sysMetaData[i]->OIDTableBuf, OID_SYS_METADATA_CERT_SEC_CATEGORY_NAME)==0)
		{

			asprintf(&tmpClearance, "%s", bmdJSRequest->sysMetaData[i]->AnyBuf);
			if (tmpClearance==NULL)	{	BMD_FOK(NO_MEMORY);	}

			certificateClearanceCount++;
			if (certificateClearanceCount==1)
			{
				certificateClearance=(char***)malloc(sizeof(char**)*2);
				if (certificateClearance==NULL) {	BMD_FOK(NO_MEMORY);	}
				memset(certificateClearance, 0, sizeof(char**)*2);
			}

			bmd_strsep(	tmpClearance, '|', &secCategory, &h);

			certificateClearance[0]=(char**)realloc(certificateClearance[0], sizeof(char *)*certificateClearanceCount);
			if (certificateClearance[0]==NULL)	{	BMD_FOK(NO_MEMORY);	}

			certificateClearance[1]=(char**)realloc(certificateClearance[1], sizeof(char *)*certificateClearanceCount);
			if (certificateClearance[1]==NULL)	{	BMD_FOK(NO_MEMORY);	}

			asprintf(&(certificateClearance[0][certificateClearanceCount-1]), "%s", secCategory[0]);
			if (certificateClearance[0][certificateClearanceCount-1]==NULL)	{	BMD_FOK(NO_MEMORY);	}

			asprintf(&(certificateClearance[1][certificateClearanceCount-1]), "%s", secCategory[1]);
			if (certificateClearance[1][certificateClearanceCount-1]==NULL)	{	BMD_FOK(NO_MEMORY);	}

			free0(secCategory[0]);
			free0(secCategory[1]);
			free0(secCategory);
		}
		/*********************************************************/
		/* odczytanie domyślnej grupy rejestrowanego certyfikatu */
		/*********************************************************/
		else if (strcmp(bmdJSRequest->sysMetaData[i]->OIDTableBuf, OID_SYS_METADATA_CERT_DEFAULT_GROUP_ID)==0)
		{
			if (userDefaultGroupId)		{	BMD_FOK(LIBBMDSQL_USER_DEFAULT_GROUP_TOO_MANY);	}
			asprintf(&userDefaultGroupId, "%s", bmdJSRequest->sysMetaData[i]->AnyBuf);
			if (userDefaultGroupId==NULL)	{	BMD_FOK(NO_MEMORY);	}
		}
		/********************************************************/
		/* odczytanie domyślnej roli rejestrowanego certyfikatu */
		/********************************************************/
		else if (strcmp(bmdJSRequest->sysMetaData[i]->OIDTableBuf, OID_SYS_METADATA_CERT_DEFAULT_ROLE_ID)==0)
		{
			if (userDefaultRoleId)		{	BMD_FOK(LIBBMDSQL_USER_DEFAULT_ROLE_TOO_MANY);	}
			asprintf(&userDefaultRoleId, "%s", bmdJSRequest->sysMetaData[i]->AnyBuf);
			if (userDefaultRoleId==NULL)	{	BMD_FOK(NO_MEMORY);	}
		}
	}


	if (certificateIdentity != NULL)
	{
		BMD_FOK(CheckIfIdentityExists(hDB, certificateIdentity));
	}


	/***********************************************************************/
	/* update odpowiednich pol, jesli zostaly okreslone w zadaniu update'u */
	/***********************************************************************/
	if (certificateIssuer!=NULL)
	{
		BMD_FOK(bmd_db_escape_string(hDB, certificateIssuer, STANDARD_ESCAPING, &escapeTemp));
		asprintf(&SQLQuery, 	"UPDATE users SET cert_dn='%s' WHERE id=%s%s%s;",
						escapeTemp,
						userId,
						_GLOBAL_bmd_configuration->location_id==NULL?"":" AND location_id=",
						_GLOBAL_bmd_configuration->location_id==NULL?"":_GLOBAL_bmd_configuration->location_id);
		free(escapeTemp); escapeTemp=NULL;
		if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}

		BMD_FOK(bmd_db_execute_sql( hDB, SQLQuery,NULL,NULL, &query_result));

		bmd_db_result_free(&query_result);
		free0(SQLQuery);
	}

	if (certificateSerial!=NULL)
	{
		BMD_FOK(bmd_db_escape_string(hDB, certificateSerial, STANDARD_ESCAPING, &escapeTemp));
		asprintf(&SQLQuery, 	"UPDATE users SET cert_serial='%s' WHERE id=%s%s%s;",
						escapeTemp,
						userId,
						_GLOBAL_bmd_configuration->location_id==NULL?"":" AND location_id=",
						_GLOBAL_bmd_configuration->location_id==NULL?"":_GLOBAL_bmd_configuration->location_id);
		free(escapeTemp); escapeTemp=NULL;
		if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}

		BMD_FOK(bmd_db_execute_sql( hDB, SQLQuery,NULL,NULL, &query_result));

		bmd_db_result_free(&query_result);
		free0(SQLQuery);
	}

	if (certificateIdentity!=NULL)
	{
		asprintf(&SQLQuery, 	"UPDATE users SET identity='%s' WHERE id=%s%s%s;",
						certificateIdentity,
						userId,
						_GLOBAL_bmd_configuration->location_id==NULL?"":" AND location_id=",
						_GLOBAL_bmd_configuration->location_id==NULL?"":_GLOBAL_bmd_configuration->location_id);

		if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}

		BMD_FOK(bmd_db_execute_sql( hDB, SQLQuery,NULL,NULL, &query_result));

		bmd_db_result_free(&query_result);
		free0(SQLQuery);
	}

	if (certificateName!=NULL)
	{
		BMD_FOK(bmd_db_escape_string(hDB, certificateName, STANDARD_ESCAPING, &escapeTemp));
		asprintf(&SQLQuery, 	"UPDATE users SET name='%s' WHERE id=%s%s%s;",
						escapeTemp,
						userId,
						_GLOBAL_bmd_configuration->location_id==NULL?"":" AND location_id=",
						_GLOBAL_bmd_configuration->location_id==NULL?"":_GLOBAL_bmd_configuration->location_id);
		free(escapeTemp); escapeTemp=NULL;
		if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}

		BMD_FOK(bmd_db_execute_sql( hDB, SQLQuery,NULL,NULL, &query_result));

		bmd_db_result_free(&query_result);
		free0(SQLQuery);
	}

	/******************************/
	/* update informacji o rolach */
	/******************************/
	if (certificateRolesCount>0)
	{
		asprintf(&SQLQuery, 	"DELETE FROM users_and_roles WHERE fk_users=%s%s%s;",
						userId,
						_GLOBAL_bmd_configuration->location_id==NULL?"":" AND location_id=",
						_GLOBAL_bmd_configuration->location_id==NULL?"":_GLOBAL_bmd_configuration->location_id);
		if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}
		BMD_FOK(bmd_db_execute_sql( hDB, SQLQuery,NULL,NULL, &query_result));

		bmd_db_result_free(&query_result);
		free0(SQLQuery);

		/**********************************/
		/* wstawienie informacji o rolach */
		/**********************************/
		for (i=0; i<certificateRolesCount; i++)
		{

			BMD_FOK(CheckIfRoleExist( certificateRoles[i], &roleName));
			free0(roleName);

			if ((userDefaultRoleId!=NULL) && (strcmp(certificateRoles[i], userDefaultRoleId)==0))
			{

				asprintf(&SQLQuery, 	"INSERT INTO users_and_roles VALUES (nextval('users_and_roles_id_seq'), %s, %s, %s, 1);",
								_GLOBAL_bmd_configuration->location_id==NULL?"DEFAULT":_GLOBAL_bmd_configuration->location_id,
								userId,
								certificateRoles[i]);

				free0(userDefaultRoleId);
			}
			else
			{

				asprintf(&SQLQuery, 	"INSERT INTO users_and_roles VALUES (nextval('users_and_roles_id_seq'), %s, %s, %s, 0);",
								_GLOBAL_bmd_configuration->location_id==NULL?"DEFAULT":_GLOBAL_bmd_configuration->location_id,
								userId,
								certificateRoles[i]);

			}

			if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}

			BMD_FOK(bmd_db_execute_sql( hDB, SQLQuery,NULL,NULL, &query_result));
			bmd_db_result_free(&query_result);

			bmd_db_result_free(&query_result);
			free0(SQLQuery);
			free0(certificateRoles[i]);
		}

		if (userDefaultRoleId!=NULL)	{	BMD_FOK(LIBBMDSQL_USER_DEFAULT_ROLE_INVALID);	}
	}

	/*******************************/
	/* update informacji o grupach */
	/*******************************/
	if (certificateGroupsCount>0)
	{
		asprintf(&SQLQuery, 	"DELETE FROM users_and_groups WHERE fk_users=%s%s%s;",
						userId,
						_GLOBAL_bmd_configuration->location_id==NULL?"":" AND location_id=",
						_GLOBAL_bmd_configuration->location_id==NULL?"":_GLOBAL_bmd_configuration->location_id);
		if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}
		BMD_FOK(bmd_db_execute_sql( hDB, SQLQuery,NULL,NULL, &query_result));

		bmd_db_result_free(&query_result);
		free0(SQLQuery);

		/***********************************/
		/* wstawienie informacji o grupach */
		/***********************************/
		for (i=0; i<certificateGroupsCount; i++)
		{

			BMD_FOK(CheckIfGroupExist( certificateGroups[i], &groupName));
			free0(groupName);

			if ((userDefaultGroupId!=NULL) && (strcmp(certificateGroups[i], userDefaultGroupId)==0))
			{
				asprintf(&SQLQuery, 	"INSERT INTO users_and_groups VALUES (nextval('users_and_groups_id_seq'),%s,%s,%s,1);",
								_GLOBAL_bmd_configuration->location_id==NULL?"DEFAULT":_GLOBAL_bmd_configuration->location_id,
								userId,
								certificateGroups[i]);
				free0(userDefaultGroupId);
			}
			else
			{
				asprintf(&SQLQuery, 	"INSERT INTO users_and_groups VALUES (nextval('users_and_groups_id_seq'),%s,%s,%s,0);",
								_GLOBAL_bmd_configuration->location_id==NULL?"DEFAULT":_GLOBAL_bmd_configuration->location_id,
								userId,
								certificateGroups[i]);
			}
			if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}

			BMD_FOK(bmd_db_execute_sql( hDB, SQLQuery,NULL,NULL, &query_result));
			bmd_db_result_free(&query_result);

			bmd_db_result_free(&query_result);
			free0(SQLQuery);
			free0(certificateGroups[i]);
		}

		if (userDefaultGroupId!=NULL)	{	BMD_FOK(LIBBMDSQL_USER_DEFAULT_GROUP_INVALID);	}
	}

	/*******************************/
	/* update informacji o klasach */
	/*******************************/
	if (certificateClassCount>0)
	{
		asprintf(&SQLQuery, 	"DELETE FROM users_and_classes WHERE fk_users=%s%s%s;",
						userId,
						_GLOBAL_bmd_configuration->location_id==NULL?"":" AND location_id=",
						_GLOBAL_bmd_configuration->location_id==NULL?"":_GLOBAL_bmd_configuration->location_id);
		if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}
		BMD_FOK(bmd_db_execute_sql( hDB, SQLQuery,NULL,NULL, &query_result));

		bmd_db_result_free(&query_result);
		free0(SQLQuery);

		/***********************************/
		/* wstawienie informacji o grupach */
		/***********************************/
		for (i=0; i<certificateClassCount; i++)
		{

			BMD_FOK(CheckIfClassExist( certificateClass[i], &className));
			free0(className);

			if ((userDefaultClassId!=NULL) && (strcmp(certificateClass[i], userDefaultClassId)==0))
			{
				asprintf(&SQLQuery, 	"INSERT INTO users_and_classes VALUES (nextval('users_and_classes_id_seq'),%s,%s,%s,1);",
								_GLOBAL_bmd_configuration->location_id==NULL?"DEFAULT":_GLOBAL_bmd_configuration->location_id,
								userId,
								certificateClass[i]);
				free0(userDefaultClassId);
			}
			else
			{
				asprintf(&SQLQuery, 	"INSERT INTO users_and_classes VALUES (nextval('users_and_classes_id_seq'),%s,%s,%s,0);",
								_GLOBAL_bmd_configuration->location_id==NULL?"DEFAULT":_GLOBAL_bmd_configuration->location_id,
								userId,
								certificateClass[i]);
			}
			if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}

			BMD_FOK(bmd_db_execute_sql( hDB, SQLQuery,NULL,NULL, &query_result));
			bmd_db_result_free(&query_result);

			bmd_db_result_free(&query_result);
			free0(SQLQuery);
			free0(certificateClass[i]);
		}

		if (userDefaultClassId!=NULL)	{	BMD_FOK(LIBBMDSQL_USER_DEFAULT_CLASS_INVALID);	}
	}

	/**************************************************************/
	/* update informacji o kategoriach i poziomach bezpieczenstwa */
	/**************************************************************/
	if (certificateClearanceCount>0)
	{

		asprintf(&SQLQuery, 	"DELETE FROM users_and_security WHERE fk_users=%s%s%s;",
						userId,
						_GLOBAL_bmd_configuration->location_id==NULL?"":" AND location_id=",
						_GLOBAL_bmd_configuration->location_id==NULL?"":_GLOBAL_bmd_configuration->location_id);

		if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}

		BMD_FOK(bmd_db_execute_sql( hDB, SQLQuery,NULL,NULL, &query_result));

		bmd_db_result_free(&query_result);
		free0(SQLQuery);

		/******************************************************************/
		/* wstawienie informacji o kategoriach i poziomach bezpieczenstwa */
		/******************************************************************/
		for (i=0; i<certificateClearanceCount; i++)
		{

			BMD_FOK(CheckIfSecCategoryExist( certificateClearance[0][i], &secCategoryId));
			BMD_FOK(CheckIfSecLevelExist( certificateClearance[1][i], &secLevelId));


			asprintf(&SQLQuery, 	"INSERT INTO users_and_security VALUES (nextval('users_and_security_id_seq'),%s,%s,%s,%s);",
							_GLOBAL_bmd_configuration->location_id==NULL?"DEFAULT":_GLOBAL_bmd_configuration->location_id,
							userId,
							secCategoryId,
							secLevelId);

			if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}

			BMD_FOK(bmd_db_execute_sql( hDB, SQLQuery,NULL,NULL, &query_result));
			bmd_db_result_free(&query_result);

			bmd_db_result_free(&query_result);
			free0(SQLQuery);
			free0(secCategoryId);
			free0(secLevelId);
			free0(certificateClearance[0][i]);
			free0(certificateClearance[1][i]);
		}
		free0(certificateClearance[0]);
		free0(certificateClearance[1]);
	}
	
	/************/
	/* porzadki */
	/************/
	bmd_db_result_free(&query_result);
	free0(SQLQuery);
	free0(certificateRoles);
	free0(certificateGroups);
	free0(certificateClearance);
	free0(certificateIssuer);
	free0(certificateSerial);
	free0(certificateName);
	free0(certificateIdentity);
	free0(secCategory);
	free0(tmpClearance);
	free0(secLevelId);
	free0(secCategoryId);
	free0(userId);
	free0(userDefaultGroupId);
	free0(userDefaultRoleId);
	free0(roleName);
	free0(groupName);

	return BMD_OK;
}
