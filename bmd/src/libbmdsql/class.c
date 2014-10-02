#include <bmd/libbmdsql/class.h>
#include <bmd/libbmdglobals/libbmdglobals.h>
#include <signal.h>

#ifdef WIN32
#pragma warning(disable:4204)
#pragma warning(disable:4100)
#pragma warning(disable:4127)
#endif

long CheckClassNumber()
{
long classCount	= 0;

	if (_GLOBAL_bmd_configuration->max_registered_classes>0)
	{
		getCountWithCondition(	_GLOBAL_ClassesDictionary, 0, NULL, &classCount);

		if (classCount>=_GLOBAL_bmd_configuration->max_registered_classes)
		{
			return LIBBMDSQL_CLASS_TOO_MANY_REGISTERED_CLASSES;
		}
	}

	return BMD_OK;
}

/*******************************************/
/* pobranie defaultowej classy uzytkownika */
/*******************************************/
long GetUserDefaultClass(	char *userId,
				char **userDefaultClassId)
{
char **anss	= NULL;
char *ans	= NULL;
char *tmp	= NULL;
long ansCount	= 0;
long i		= 0;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (userId==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (userDefaultClassId==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if ((*userDefaultClassId)!=NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	//SELECT users_and_classes.fk_users, users_and_classes.default_class, classes.id, classes.name, users_and_classes.fk_users::varchar||'|'||classes.id::varchar FROM classes LEFT JOIN users_and_classes ON (users_and_classes.fk_classes=classes.id);
	getColumnWithCondition(	_GLOBAL_UserClassesDictionary, 0, userId, 2, &ansCount, &anss);

	for (i=0; i<ansCount; i++)
	{
		asprintf(&tmp, "%s|%s", userId, anss[i]);
		if (tmp==NULL)	{	BMD_FOK(NO_MEMORY);	}

		//SELECT users_and_classes.fk_users, users_and_classes.default_class, classes.id, classes.name, users_and_classes.fk_users::varchar||'|'||classes.id::varchar FROM classes LEFT JOIN users_and_classes ON (users_and_classes.fk_classes=classes.id);
		getElementWithCondition( _GLOBAL_UserClassesDictionary, 4, tmp, 1, &ans);

		if ((ans!=NULL) && (strcmp(ans, "1")==0))
		{
			asprintf(userDefaultClassId, "%s", anss[i]);
			if (*userDefaultClassId==NULL)	{	BMD_FOK(NO_MEMORY);	}

			break;
		}
		free0(tmp);
		free0(anss[i]);
		free0(ans);
	}

	if (*userDefaultClassId==NULL)	{	BMD_FOK(LIBBMDSQL_CLASS_DEFAULT_NOT_SPECIFIED);	}

	/************/
	/* porzadki */
	/************/
	free0(ans);
	free0(tmp);
	for (i=0; i<ansCount; i++)
	{
		free0(anss[i]);
	}
	free0(anss);

	return BMD_OK;
}

/****************************************/
/* sprawdzenie, czy uzytkownik istnieje */
/****************************************/
long CheckIfClassExist(	char *classId,
			char **className)
{
	/************************/
	/* walidacja parametrow */
	/************************/
	if (classId==NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (className==NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if ((*className)!=NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
PRINT_TEST(">>> PKL <<<< \n");
	//SELECT users_and_classes.fk_users, users_and_classes.default_class, classes.id, classes.name, users_and_classes.fk_users::varchar||'|'||classes.id::varchar FROM classes LEFT JOIN users_and_classes ON (users_and_classes.fk_classes=classes.id);
	getElementWithCondition(_GLOBAL_UserClassesDictionary, 2, classId, 3, className);
PRINT_TEST(">>> PKL <<<< \n");
	if ((*className)==NULL)	{	return LIBBMDSQL_CLASS_NOT_REGISTERED_IN_SYSTEM;	}
PRINT_TEST(">>> PKL <<<< \n");
	return BMD_OK;

}

/**
* @author WSz
* Funkcja wydobywa z datagramu klase uzytkownika lub pobiera defaultowa ze slownika
* @param requestDatagram[in] datgram zadania na podstawie ktorego podejmowana jest proba okreslenia klasy uzytkownika
* @param req[in] to struktura opisujaca nadchodzacee zadanie
* @param userClassId[out] to okreslony identyfikator klasy uzytkownika, w ramach ktorej jest wykonywane zadanie
*/
long DetermineUserClassForRequest(const bmdDatagram_t *requestDatagram, const server_request_data_t *req, char** userClassId)
{
long iter			= 0;
char *tempClassId	= NULL;

	if(requestDatagram == NULL)
		{ BMD_FOK(BMD_ERR_PARAM1); }
	if(req == NULL)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(req->ua == NULL)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(req->ua->id == NULL) // id certyfikatu
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(req->ua->id->buf == NULL || req->ua->id->size <= 0)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(userClassId == NULL)
		{ BMD_FOK(BMD_ERR_PARAM3); }
	if(*userClassId != NULL)
		{ BMD_FOK(BMD_ERR_PARAM3); }
	

	if(requestDatagram->actionMetaData != NULL && requestDatagram->no_of_actionMetaData > 0)
	{
		for(iter=0; iter<requestDatagram->no_of_actionMetaData; iter++)
		{
			if(requestDatagram->actionMetaData[iter] != NULL && requestDatagram->actionMetaData[iter]->OIDTableBuf != NULL)
			{
				if(strcmp(requestDatagram->actionMetaData[iter]->OIDTableBuf, OID_ACTION_METADATA_CERT_CHOSEN_CLASS_ID) == 0)
				{
					if(requestDatagram->actionMetaData[iter]->AnyBuf != NULL && requestDatagram->actionMetaData[iter]->AnySize > 0)
					{
						tempClassId = strndup(requestDatagram->actionMetaData[iter]->AnyBuf, requestDatagram->actionMetaData[iter]->AnySize);
						if(tempClassId==NULL)
							{ BMD_FOK(NO_MEMORY); }
					}
					break;
				}
			}
		}
	}

	if(tempClassId != NULL)
	{
		BMD_FOK(CheckIfUserHasClass(tempClassId, req->ua->id->buf));

	}
	else
	{
		BMD_FOK(GetUserDefaultClass(req->ua->id->buf, &tempClassId));
	}
	
	*userClassId = tempClassId;
	tempClassId = NULL;
	return BMD_OK;
}


long CheckIfUserHasClass(	char *classId,
				char *userId)
{
char *ans	= NULL;
char *tmp	= NULL;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (classId==NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (userId==NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	asprintf(&tmp, "%s|%s", userId, classId);
	if (tmp==NULL)	{	BMD_FOK(NO_MEMORY);	}

	//SELECT users_and_classes.fk_users, users_and_classes.default_class, classes.id, classes.name, users_and_classes.fk_users::varchar||'|'||classes.id::varchar FROM classes LEFT JOIN users_and_classes ON (users_and_classes.fk_classes=classes.id);
	getElementWithCondition(_GLOBAL_UserClassesDictionary, 4, tmp, 4, &ans);

	if (ans==NULL)
	{
		return LIBBMDSQL_CLASS_NOT_OWNED;
	}

	/************/
	/* porzadki */
	/************/
	free0(ans);
	free0(tmp);

	return BMD_OK;
}

long CheckIfClassHasAnyUser(	char *classId)
{
char *ans	= NULL;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (classId==NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}

	//SELECT users_and_classes.fk_users, users_and_classes.default_class, classes.id, classes.name, users_and_classes.fk_users::varchar||'|'||classes.id::varchar FROM classes LEFT JOIN users_and_classes ON (users_and_classes.fk_classes=classes.id);
	getElementWithCondition(_GLOBAL_UserClassesDictionary, 2, classId, 0, &ans);

	if (ans==NULL)
	{
		return BMD_OK;
	}
	if (strlen(ans)>0)
	{
		return LIBBMDSQL_GROUP_USERS_WITH_GROUP_EXIST;
	}

	/************/
	/* porzadki */
	/************/
	free0(ans);

	return BMD_OK;
}

/*******************/
/* usuniecie grupy */
/*******************/
long JS_bmdDatagramSet_to_deleteClass(	void *hDB,
					bmdDatagramSet_t *bmdJSRequestSet,
					bmdDatagramSet_t **bmdJSResponseSet)
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

		status=JS_bmdDatagram_to_deleteClass(hDB, bmdJSRequestSet->bmdDatagramSetTable[i], &((*bmdJSResponseSet)->bmdDatagramSetTable[i]));
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
		(*bmdJSResponseSet)->bmdDatagramSetTable[i]->datagramType = bmdJSRequestSet->bmdDatagramSetTable[i]->datagramType+100;
		(*bmdJSResponseSet)->bmdDatagramSetTable[i]->datagramStatus = status;

	}

#ifndef WIN32
	kill(getppid(),SIGDICTRELOAD);
#endif //ifndef WIN32


	return BMD_OK;
}


long JS_bmdDatagram_to_deleteClass(	void *hDB,
					bmdDatagram_t *bmdJSRequest,
					bmdDatagram_t **bmdJSResponse)
{
long i			= 0;
char **anss		= NULL;
long ansCount		= 0;
void *query_result	= NULL;
char *SQLQuery		= NULL;
char *classId		= NULL;
char *className		= NULL;

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if (hDB==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (bmdJSRequest==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	/******************************************************/
	/* poszukiwanie poszczególnych elementów w datagramie */
	/******************************************************/
	if (	(bmdJSRequest->protocolDataFileId!=NULL) &&
		(bmdJSRequest->protocolDataFileId->buf!=NULL) &&
		(bmdJSRequest->protocolDataFileId->size>0) )
	{
		asprintf(&classId, "%s", bmdJSRequest->protocolDataFileId->buf);
		if (classId==NULL)	{	BMD_FOK(NO_MEMORY);	}
	}

	if (classId==NULL)	{	BMD_FOK(LIBBMDSQL_CLASS_ID_UNDEFINED);	}

	/***************************************/
	/* sprawdzenie, czy grupe mozna usunac */
	/***************************************/
	BMD_FOK(CheckIfClassExist( classId, &className));
	BMD_FOK(CheckIfClassHasAnyUser(	classId));

	/****************************************/
	/* usuniecie wpisow z users_and_classes */
	/****************************************/
	asprintf(&SQLQuery, 	"DELETE FROM users_and_classes WHERE fk_classes=%s%s%s;",
					classId,
					_GLOBAL_bmd_configuration->location_id==NULL?"":" AND location_id=",
					_GLOBAL_bmd_configuration->location_id==NULL?"":_GLOBAL_bmd_configuration->location_id);

	if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}

	BMD_FOK_CHG(bmd_db_execute_sql( hDB, SQLQuery,NULL,NULL, &query_result), LIBBMDSQL_CLASS_CLEAR_USERS_AND_CLASSES_TABLE_ERR);
	bmd_db_result_free(&query_result);

	bmd_db_result_free(&query_result);
	free0(SQLQuery);

	/*****************************/
	/* usuniecie wpisow z class */
	/*****************************/
	asprintf(&SQLQuery, 	"DELETE FROM classes WHERE id=%s%s%s;",
					classId,
					_GLOBAL_bmd_configuration->location_id==NULL?"":" AND location_id=",
					_GLOBAL_bmd_configuration->location_id==NULL?"":_GLOBAL_bmd_configuration->location_id);

	if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}

	BMD_FOK_CHG(bmd_db_execute_sql( hDB, SQLQuery,NULL,NULL, &query_result), LIBBMDSQL_CLASS_CLEAR_CLASSES_TABLE_ERR);
	bmd_db_result_free(&query_result);

	bmd_db_result_free(&query_result);
	free0(SQLQuery);

	/************/
	/* porzadki */
	/************/
	for (i=0; i<ansCount; i++)
	{
		free0(anss[i]);
	}
	free0(anss);
	free0(classId);
	free0(className);
	free0(SQLQuery);

	return BMD_OK;
}


/*******************************************************/
/*          pobieranie informacji o rolach             */
/*******************************************************/
long JS_bmdDatagramSet_to_getClassList(	void *hDB,
					bmdDatagramSet_t *bmdJSRequestSet,
					bmdDatagramSet_t **bmdJSResponseSet,
					server_request_data_t *req)
{
long status		= 0;
long i			= 0;
long classNumber	= 0;
char *classId		= NULL;
char *SQLQuery		= NULL;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (hDB==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (bmdJSRequestSet==NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	if (	(bmdJSRequestSet->bmdDatagramSetTable[0]->protocolDataFileId!=NULL) &&
		(bmdJSRequestSet->bmdDatagramSetTable[0]->protocolDataFileId->buf!=NULL) &&
		(bmdJSRequestSet->bmdDatagramSetTable[0]->protocolDataFileId->size>0) )
	{
		asprintf(&classId, "%s", bmdJSRequestSet->bmdDatagramSetTable[0]->protocolDataFileId->buf);
		if (classId==NULL)	{	BMD_FOK(NO_MEMORY);	}
	}

	if (classId==NULL)
	{
		/***************************************/
		/* odczytanie informacji o ilosci grup */
		/***************************************/
		//SELECT  id, name FROM classs WHERE classes.location_id=%s
		BMD_FOK_CHG(getCountWithCondition(	_GLOBAL_ClassesDictionary, 0, NULL, &classNumber), LIBBMDSQL_CLASS_LIST_NO_CLASS);
	}
	else
	{
		classNumber=1;
	}

	/*****************************************/
	/* przygotowanie datagramsetu odpowiedzi */
	/*****************************************/
	BMD_FOK(PR2_bmdDatagramSet_fill_with_datagrams(classNumber, bmdJSResponseSet));

	/*******************************************/
	/* pobieranie danych poszczególnych userów */
	/*******************************************/
	for (i=0; i<classNumber; i++)
	{

		status=JS_bmdDatagram_to_getClassList(hDB, i, classId, &((*bmdJSResponseSet)->bmdDatagramSetTable[i]));

		/**************************************/
		/* ustawienie statusu błędu datagramu */
		/**************************************/
		(*bmdJSResponseSet)->bmdDatagramSetTable[i]->datagramType = bmdJSRequestSet->bmdDatagramSetTable[0]->datagramType+100;
		(*bmdJSResponseSet)->bmdDatagramSetTable[i]->datagramStatus = status;

		/************************************************/
		/* przepisanie zadajacego do odpowiedzi serwera */
		/************************************************/
		if (bmdJSRequestSet->bmdDatagramSetTable[0]->protocolDataOwner!=NULL)
		{
			if (bmdJSRequestSet->bmdDatagramSetTable[0]->protocolDataOwner->buf!=NULL)
			{
				(*bmdJSResponseSet)->bmdDatagramSetTable[i]->protocolDataOwner=(GenBuf_t *)malloc(sizeof(GenBuf_t));
				(*bmdJSResponseSet)->bmdDatagramSetTable[i]->protocolDataOwner->buf=(char*)malloc(bmdJSRequestSet->bmdDatagramSetTable[0]->protocolDataOwner->size+2);
				memset((*bmdJSResponseSet)->bmdDatagramSetTable[i]->protocolDataOwner->buf, 0,bmdJSRequestSet->bmdDatagramSetTable[0]->protocolDataOwner->size+1);
				memmove((*bmdJSResponseSet)->bmdDatagramSetTable[i]->protocolDataOwner->buf,bmdJSRequestSet->bmdDatagramSetTable[0]->protocolDataOwner->buf,bmdJSRequestSet->bmdDatagramSetTable[0]->protocolDataOwner->size);
				(*bmdJSResponseSet)->bmdDatagramSetTable[i]->protocolDataOwner->size=bmdJSRequestSet->bmdDatagramSetTable[0]->protocolDataOwner->size;
			}
		}

		/****************************************/
		/* zarejestrowanie akcji w bazie danych */
		/****************************************/
		if (_GLOBAL_bmd_configuration->messaging_plugin_conf.bmd_messaging_plugin_register_action)
		{
			status=_GLOBAL_bmd_configuration->messaging_plugin_conf.bmd_messaging_plugin_register_action(hDB, bmdJSRequestSet->bmdDatagramSetTable[0], (*bmdJSResponseSet)->bmdDatagramSetTable[i], req);
			if (status<BMD_OK)
			{
				(*bmdJSResponseSet)->bmdDatagramSetTable[i]->datagramStatus = status;
			}
		}
	}

	/************/
	/* porzadki */
	/************/
	free0(classId);
	free0(SQLQuery);

	return BMD_OK;
}

long JS_bmdDatagram_to_getClassList(	void *hDB,
					long number,
					char *classId,
					bmdDatagram_t **bmdJSResponse)
{
long ansCount		= 0;
char **anss		= NULL;
char *SQLQuery		= NULL;
char *classsId		= NULL;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (hDB==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
 	if (number<0)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	/***************************************/
	/* pobranie glownych informacji o roli */
	/***************************************/
	asprintf(&SQLQuery, 	"SELECT id, name FROM classes %s%s %s%s %s%s LIMIT 1 OFFSET %li;",
					_GLOBAL_bmd_configuration->location_id!=NULL?"WHERE location_id=":"",
					_GLOBAL_bmd_configuration->location_id!=NULL?_GLOBAL_bmd_configuration->location_id:"",
					(classId!=NULL && _GLOBAL_bmd_configuration->location_id!=NULL)?" AND id=":"",
					(classId!=NULL && _GLOBAL_bmd_configuration->location_id!=NULL)?classId:"",
					(classId!=NULL && _GLOBAL_bmd_configuration->location_id==NULL)?" WHERE id=":"",
					(classId!=NULL && _GLOBAL_bmd_configuration->location_id==NULL)?classId:"",
					number);
	if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}

	BMD_FOK_CHG(ExecuteSQLQueryWithRowAnswersKnownDBConnection( hDB, SQLQuery, &ansCount, &anss), LIBBMDSQL_CLASS_NOT_REGISTERED_IN_SYSTEM);


	asprintf(&classsId, "%s", anss[0]);
	if (classsId==NULL)	{	BMD_FOK(NO_MEMORY);	}

	BMD_FOK(set_gen_buf2(anss[0], (long)strlen(anss[0]), &((*bmdJSResponse)->protocolDataFileId)));
	BMD_FOK(set_gen_buf2(anss[1], (long)strlen(anss[1]), &((*bmdJSResponse)->protocolDataFilename)));

	if (_GLOBAL_bmd_configuration->location_id)
	{
		BMD_FOK(set_gen_buf2(_GLOBAL_bmd_configuration->location_id, (long)strlen(_GLOBAL_bmd_configuration->location_id), &((*bmdJSResponse)->protocolDataFileLocationId)));
	}

	/************/
	/* porzadki */
	/************/
	free0(anss);
	free0(SQLQuery);
	free0(classsId);

	return BMD_OK;
}


/******************************************/
/*          rejestracja klasy             */
/******************************************/
long JS_bmdDatagramSet_to_registerClass(	void *hDB,
						bmdDatagramSet_t *bmdJSRequestSet,
						bmdDatagramSet_t **bmdJSResponseSet)
{
long status	= 0;
long i		= 0;

	PRINT_INFO("LIBBMDSQLINF Registering new class (datagramset)\n");
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
		if (bmdJSRequestSet->bmdDatagramSetTable[i]->datagramType==BMD_DATAGRAM_TYPE_REGISTER_CLASS)
		{
			status=JS_bmdDatagram_to_registerClass(hDB,bmdJSRequestSet->bmdDatagramSetTable[i],
				&((*bmdJSResponseSet)->bmdDatagramSetTable[i]));
		}
 		else if (bmdJSRequestSet->bmdDatagramSetTable[i]->datagramType==BMD_DATAGRAM_TYPE_UPDATE_CLASS)
		{
			status=JS_bmdDatagram_to_updateClass(hDB,bmdJSRequestSet->bmdDatagramSetTable[i],
				&((*bmdJSResponseSet)->bmdDatagramSetTable[i]));
		}

		if (status<BMD_OK)
		{
			BMD_FOK(bmd_db_rollback_transaction(hDB,NULL));
		}
		else
		{
			/**************************/
			/* zakonczenie transakcji */
			/**************************/
			bmd_db_end_transaction(hDB);
		}

		/**************************************/
		/* ustawienie statusu błędu datagramu */
		/**************************************/
		(*bmdJSResponseSet)->bmdDatagramSetTable[i]->datagramType = bmdJSRequestSet->bmdDatagramSetTable[i]->datagramType+100;
		(*bmdJSResponseSet)->bmdDatagramSetTable[i]->datagramStatus = status;
	}

#ifndef WIN32
	kill(getppid(),SIGDICTRELOAD);
#endif //ifndef WIN32


	return BMD_OK;
}

long JS_bmdDatagram_to_registerClass(	void *hDB,
					bmdDatagram_t *bmdJSRequest,
					bmdDatagram_t **bmdJSResponse)
{
long i					= 0;
void *query_result			= NULL;
char *className				= NULL;
char *SQLQuery				= NULL;
char *classId				= NULL;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (hDB==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (bmdJSRequest==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	/******************************************************/
	/* poszukiwanie poszczególnych elementów w datagramie */
	/******************************************************/
	for (i=0; i<bmdJSRequest->no_of_actionMetaData; i++)
	{
		/****************************************/
		/* odczytanie nazwy rejestrowanej grupy */
		/****************************************/
		if (strcmp(bmdJSRequest->actionMetaData[i]->OIDTableBuf, OID_ACTION_METADATA_CERT_CLASS_NAME)==0)
		{
			if (className!=NULL)	{	BMD_FOK(LIBBMDSQL_CLASS_NAME_TOO_MANY);	}
			asprintf(&className, "%s", bmdJSRequest->actionMetaData[i]->AnyBuf);
			if (className==NULL)	{	BMD_FOK(NO_MEMORY);	}
		}
	}

	if (className==NULL)	{	BMD_FOK(LIBBMDSQL_CLASS_NAME_UNDEFINED);	}
	if (CheckIfClassExist(	className, &classId)==BMD_OK)
	{
		BMD_FOK(LIBBMDSQL_CLASS_CLASS_ALREADY_REGISTERED);
	}
	BMD_FOK(CheckClassNumber());

	/******************************************/
	/* wstawienie informacji do tabeli classes */
	/******************************************/
	asprintf(&SQLQuery, 	"INSERT INTO classes VALUES (nextval('classes_id_seq'),%s,'%s');",
					_GLOBAL_bmd_configuration->location_id==NULL?"DEFAULT":_GLOBAL_bmd_configuration->location_id,
					className);
	if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}

	BMD_FOK(bmd_db_execute_sql( hDB, SQLQuery,NULL,NULL, &query_result));

	bmd_db_result_free(&query_result);
	free0(SQLQuery);

	/**************************/
	/* pobranie id nowej roli */
	/**************************/
	asprintf(&SQLQuery, 	"SELECT last_value FROM classes_id_seq;");
	if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}

	BMD_FOK(ExecuteSQLQueryWithAnswerKnownDBConnection(	hDB, SQLQuery, &classId));
	free0(SQLQuery);
	BMD_FOK(set_gen_buf2(classId, (long)strlen(classId), &((*bmdJSResponse)->protocolDataFileId)));

	/************/
	/* porzadki */
	/************/
	free0(classId);
	free0(className);
	free0(SQLQuery);

	return BMD_OK;
}


long JS_bmdDatagram_to_updateClass(	void *hDB,
					bmdDatagram_t *bmdJSRequest,
					bmdDatagram_t **bmdJSResponse)
{
long i					= 0;
void *query_result			= NULL;
char *SQLQuery				= NULL;
char *className				= NULL;
char *classId				= NULL;
char *tmp				= NULL;

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if (hDB==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (bmdJSRequest==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	if (	(bmdJSRequest->protocolDataFileId!=NULL) &&
		(bmdJSRequest->protocolDataFileId->buf!=NULL) &&
		(bmdJSRequest->protocolDataFileId->size>0) )
	{
		asprintf(&classId, "%s", bmdJSRequest->protocolDataFileId->buf);
		if (classId==NULL)	{	BMD_FOK(NO_MEMORY);	}
	}

	if (classId==NULL)	{	BMD_FOK(LIBBMDSQL_CLASS_ID_UNDEFINED);	}

	/******************************************************/
	/* poszukiwanie poszczególnych elementów w datagramie */
	/******************************************************/
	for (i=0; i<bmdJSRequest->no_of_actionMetaData; i++)
	{
		/**************************************/
		/* odczytanie nazwy updatowanej grupy */
		/**************************************/
		if (strcmp(bmdJSRequest->actionMetaData[i]->OIDTableBuf, OID_ACTION_METADATA_CERT_CLASS_NAME)==0)
		{
			if (className!=NULL)	{	BMD_FOK(LIBBMDSQL_CLASS_NAME_TOO_MANY);	}
			asprintf(&className, "%s", bmdJSRequest->actionMetaData[i]->AnyBuf);
			if (className==NULL)	{	BMD_FOK(NO_MEMORY);	}
		}
	}

	if (classId==NULL)	{	BMD_FOK(LIBBMDSQL_CLASS_ID_UNDEFINED);	}

	BMD_FOK(CheckIfClassExist(	classId, &tmp));
	free0(tmp);

	/**********************/
	/* zmiana nazwy grupy */
	/**********************/
	if (className!=NULL)
	{
		asprintf(&SQLQuery, 	"UPDATE classes SET name='%s' WHERE id=%s%s%s;",
						className,
						classId,
						_GLOBAL_bmd_configuration->location_id==NULL?"":" AND location_id=",
						_GLOBAL_bmd_configuration->location_id==NULL?"":_GLOBAL_bmd_configuration->location_id);

		BMD_FOK(bmd_db_execute_sql( hDB, SQLQuery,NULL,NULL, &query_result));
		bmd_db_result_free(&query_result);
		free0(SQLQuery);
	}

	/************/
	/* porzadki */
	/************/
	free0(classId);
	free0(className);
	free0(SQLQuery);

	return BMD_OK;
}
