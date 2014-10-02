#include <bmd/libbmdsql/group.h>
#include <bmd/libbmdglobals/libbmdglobals.h>
#include <signal.h>

#ifdef WIN32
#pragma warning(disable:4204)
#pragma warning(disable:4100)
#pragma warning(disable:4127)
#endif

long CheckGroupNumber()
{
long groupCount	= 0;

	if (_GLOBAL_bmd_configuration->max_registered_groups>0)
	{
		getCountWithCondition(	_GLOBAL_GroupsDictionary, 0, NULL, &groupCount);

		if (groupCount>=_GLOBAL_bmd_configuration->max_registered_groups)
		{
			return LIBBMDSQL_GROUP_TOO_MANY_REGISTERED_GROUPS;
		}
	}

	return BMD_OK;
}

/****************************************/
/* sprawdzenie, czy uzytkownik istnieje */
/****************************************/
/**
* @param[in] groupId to identyfikator grupy, ktorej istnienie jest sprawdzane
* @param[out] groupName, jesli nazwa grupy nie jest potrzebna, mozna podac NULL
* @return BMD_OK, jesli grupa istnieje, w przeciwnym razie wartosc ujemna
*/
long CheckIfGroupExist(	char *groupId,
			char **groupName)
{
char* tempGroupName=NULL;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (groupId==NULL)
		{ BMD_FOK(BMD_ERR_PARAM1); }
	if (groupName!=NULL && (*groupName)!=NULL)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	

	//SELECT users_and_groups.fk_users, users_and_groups.default_group,groups.id, groups.name, users_and_groups.fk_users::varchar||'|'||groups.id::varchar FROM groups LEFT JOIN users_and_groups ON (users_and_groups.fk_groups=groups.id);
	getElementWithCondition(	_GLOBAL_UserGroupsDictionary, 2, groupId, 3, &tempGroupName);

	if (tempGroupName==NULL)
		{ return LIBBMDSQL_GROUP_NOT_REGISTERED_IN_SYSTEM; }
	
	if(groupName != NULL)
	{
		*groupName=tempGroupName;
		tempGroupName=NULL;
	}

	return BMD_OK;
}

long CheckIfGroupExistByName(	char *groupName)
{
char *groupId	= NULL;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (groupName==NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}

	//SELECT users_and_groups.fk_users, users_and_groups.default_group,groups.id, groups.name, users_and_groups.fk_users::varchar||'|'||groups.id::varchar FROM groups LEFT JOIN users_and_groups ON (users_and_groups.fk_groups=groups.id);
	getElementWithCondition(	_GLOBAL_UserGroupsDictionary, 3, groupName, 2, &groupId);

	if (groupId==NULL)	{	return BMD_OK;	}

	/************/
	/* porzadki */
	/************/
	free0(groupId);

	return LIBBMDSQL_GROUP_GROUP_ALREADY_REGISTERED;

}

long CheckIfGroupHasChild(	char *groupId)
{
char *ans			= NULL;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (groupId==NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}

	//SELECT src, dest FROM groups_graph;
	getElementWithCondition(	_GLOBAL_GroupsGraphDictionary, 0, groupId, 1, &ans);

	if (ans==NULL)	{	return BMD_OK;	}
	free0(ans);

	return LIBBMDSQL_GROUP_DEST_GROUPS_FOUND;
}

long CheckIfUserHasGroup(	char *groupId,
				char *userId)
{
char *ans	= NULL;
char *tmp	= NULL;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (groupId==NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (userId==NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	asprintf(&tmp, "%s|%s", userId, groupId);
	if (tmp==NULL)	{	BMD_FOK(NO_MEMORY);	}

	//SELECT users_and_groups.fk_users, users_and_groups.default_group, groups.id, groups.name, users_and_groups.fk_users||groups.id FROM groups LEFT JOIN users_and_groups ON (users_and_groups.fk_groups=groups.id) WHERE groups.location_id=%s;
	getElementWithCondition(_GLOBAL_UserGroupsDictionary, 4, tmp, 4, &ans);

	if (ans==NULL)
	{
		return LIBBMDSQL_GROUP_NOT_OWNED;
	}

	/************/
	/* porzadki */
	/************/
	free0(ans);
	free0(tmp);

	return BMD_OK;
}

long CheckIfGroupHasAnyUser(	char *groupId)
{
char *ans	= NULL;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (groupId==NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}

	//SELECT users_and_groups.fk_users, users_and_groups.default_group, groups.id, groups.name, users_and_groups.fk_users||groups.id FROM groups LEFT JOIN users_and_groups ON (users_and_groups.fk_groups=groups.id) WHERE groups.location_id=%s;
	getElementWithCondition(_GLOBAL_UserGroupsDictionary, 2, groupId, 0, &ans);

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
long JS_bmdDatagramSet_to_deleteGroup(	void *hDB,
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

		status=JS_bmdDatagram_to_deleteGroup(hDB, bmdJSRequestSet->bmdDatagramSetTable[i], &((*bmdJSResponseSet)->bmdDatagramSetTable[i]));

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

long JS_bmdDatagram_to_deleteGroup(	void *hDB,
					bmdDatagram_t *bmdJSRequest,
					bmdDatagram_t **bmdJSResponse)
{
long i					= 0;
char **anss				= NULL;
long ansCount				= 0;
void *query_result			= NULL;
char *SQLQuery				= NULL;
char *groupId				= NULL;
char *groupName				= NULL;

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
		asprintf(&groupId, "%s", bmdJSRequest->protocolDataFileId->buf);
		if (groupId==NULL)	{	BMD_FOK(NO_MEMORY);	}
	}

	if (groupId==NULL)	{	BMD_FOK(LIBBMDSQL_GROUP_ID_UNDEFINED);	}

	/***************************************/
	/* sprawdzenie, czy grupe mozna usunac */
	/***************************************/
	BMD_FOK_CHG(CheckIfGroupExist( groupId, &groupName), LIBBMDSQL_GROUP_NOT_FOUND);
	BMD_FOK_CHG(CheckIfGroupHasAnyUser(groupId), LIBBMDSQL_GROUP_USERS_WITH_GROUP_EXIST);
	BMD_FOK(CheckIfGroupHasChild(groupId));

	/***********************************/
	/* usuniecie wpisow z groups_graph */
	/***********************************/
	asprintf(&SQLQuery, 	"DELETE FROM groups_graph WHERE dest=%s%s%s;",
					groupId,
					_GLOBAL_bmd_configuration->location_id==NULL?"":" AND location_id=",
					_GLOBAL_bmd_configuration->location_id==NULL?"":_GLOBAL_bmd_configuration->location_id);

	if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}

	BMD_FOK_CHG(bmd_db_execute_sql( hDB, SQLQuery,NULL,NULL, &query_result), LIBBMDSQL_GROUP_CLEAR_GROUP_GRAPH_TABLE_ERR);
	bmd_db_result_free(&query_result);
	free0(SQLQuery);

	asprintf(&SQLQuery, 	"DELETE FROM groups_graph WHERE src=%s%s%s;",
					groupId,
					_GLOBAL_bmd_configuration->location_id==NULL?"":" AND location_id=",
					_GLOBAL_bmd_configuration->location_id==NULL?"":_GLOBAL_bmd_configuration->location_id);

	if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}

	BMD_FOK_CHG(bmd_db_execute_sql( hDB, SQLQuery,NULL,NULL, &query_result),
			LIBBMDSQL_GROUP_CLEAR_GROUP_GRAPH_TABLE_ERR);
	bmd_db_result_free(&query_result);

	bmd_db_result_free(&query_result);
	free0(SQLQuery);

	/***************************************/
	/* usuniecie wpisow z users_and_groups */
	/***************************************/
	asprintf(&SQLQuery, 	"DELETE FROM users_and_groups WHERE fk_groups=%s%s%s;",
					groupId,
					_GLOBAL_bmd_configuration->location_id==NULL?"":" AND location_id=",
					_GLOBAL_bmd_configuration->location_id==NULL?"":_GLOBAL_bmd_configuration->location_id);

	if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}

	BMD_FOK_CHG(bmd_db_execute_sql( hDB, SQLQuery,NULL,NULL, &query_result),
			LIBBMDSQL_GROUP_CLEAR_USERS_AND_GROUPS_TABLE_ERR);
	bmd_db_result_free(&query_result);

	bmd_db_result_free(&query_result);
	free0(SQLQuery);

	/*****************************/
	/* usuniecie wpisow z groups */
	/*****************************/
	asprintf(&SQLQuery, 	"DELETE FROM groups WHERE id=%s%s%s;",
					groupId,
					_GLOBAL_bmd_configuration->location_id==NULL?"":" AND location_id=",
					_GLOBAL_bmd_configuration->location_id==NULL?"":_GLOBAL_bmd_configuration->location_id);

	if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}

	BMD_FOK_CHG(bmd_db_execute_sql( hDB, SQLQuery,NULL,NULL, &query_result),
			LIBBMDSQL_GROUP_CLEAR_GROUPS_TABLE_ERR);
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
	free0(groupId);
	free0(groupName);
	free0(SQLQuery);

	return BMD_OK;
}


/*******************************************************/
/*          pobieranie informacji o rolach             */
/*******************************************************/
long JS_bmdDatagramSet_to_getGroupList(	void *hDB,
							bmdDatagramSet_t *bmdJSRequestSet,
							bmdDatagramSet_t **bmdJSResponseSet,
							server_request_data_t *req)
{
long status		= 0;
long i			= 0;
long groupNumber	= 0;
char *groupId		= NULL;
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
		asprintf(&groupId, "%s", bmdJSRequestSet->bmdDatagramSetTable[0]->protocolDataFileId->buf);
		if (groupId==NULL)	{	BMD_FOK(NO_MEMORY);	}
	}

	if (groupId==NULL)
	{
		/***************************************/
		/* odczytanie informacji o ilosci grup */
		/***************************************/
		//SELECT  id, name FROM groups WHERE groups.location_id=%s
		BMD_FOK_CHG(getCountWithCondition(	_GLOBAL_GroupsDictionary, 0, NULL, &groupNumber), LIBBMDSQL_GROUP_LIST_NO_GROUP);
	}
	else
	{
		groupNumber=1;
	}

	/*****************************************/
	/* przygotowanie datagramsetu odpowiedzi */
	/*****************************************/
	BMD_FOK(PR2_bmdDatagramSet_fill_with_datagrams(groupNumber, bmdJSResponseSet));

	/*******************************************/
	/* pobieranie danych poszczególnych userów */
	/*******************************************/
	for (i=0; i<groupNumber; i++)
	{
		/***********************/
		/* poczatek transakcji */
		/***********************/
		BMD_FOK(bmd_db_start_transaction(hDB,BMD_TRANSACTION_LEVEL_NONE));

		status=JS_bmdDatagram_to_getGroupList(hDB, i, groupId, &((*bmdJSResponseSet)->bmdDatagramSetTable[i]));

		/**************************************/
		/* ustawienie statusu błędu datagramu */
		/**************************************/
		(*bmdJSResponseSet)->bmdDatagramSetTable[i]->datagramType = bmdJSRequestSet->bmdDatagramSetTable[0]->datagramType+100;
		(*bmdJSResponseSet)->bmdDatagramSetTable[i]->datagramStatus = status;

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

		if ((*bmdJSResponseSet)->bmdDatagramSetTable[i]->datagramStatus<BMD_OK)
		{
			bmd_db_rollback_transaction(hDB,NULL);
		}
		else
		{
			bmd_db_end_transaction(hDB);
		}

	}

	/************/
	/* porzadki */
	/************/
	free0(groupId);
	free0(SQLQuery);

	return BMD_OK;
}

long JS_bmdDatagram_to_getGroupList(	void *hDB,
					long number,
					char *groupId,
					bmdDatagram_t **bmdJSResponse)
{
long ansCount		= 0;
long i			= 0;
GenBuf_t *tmp		= NULL;
char **anss		= NULL;
char *ans		= NULL;
char *SQLQuery		= NULL;
char *groupsId		= NULL;
char* escapeTemp	= NULL;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (hDB==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
 	if (number<0)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	/***************************************/
	/* pobranie glownych informacji o roli */
	/***************************************/
	
	if(groupId != NULL)
	{
		BMD_FOK(bmd_db_escape_string(hDB, groupId, STANDARD_ESCAPING, &escapeTemp));
	}
		
	asprintf(&SQLQuery, 	"SELECT id, name FROM groups %s%s %s%s %s%s LIMIT 1 OFFSET %li;",
					_GLOBAL_bmd_configuration->location_id!=NULL?"WHERE location_id=":"",
					_GLOBAL_bmd_configuration->location_id!=NULL?_GLOBAL_bmd_configuration->location_id:"",
					(groupId!=NULL && _GLOBAL_bmd_configuration->location_id!=NULL)?"AND id=":"",
					(groupId!=NULL && _GLOBAL_bmd_configuration->location_id!=NULL)?escapeTemp:"",
					(groupId!=NULL && _GLOBAL_bmd_configuration->location_id==NULL)?"WHERE id=":"",
					(groupId!=NULL && _GLOBAL_bmd_configuration->location_id==NULL)?escapeTemp:"",
					number);
	free(escapeTemp); escapeTemp=NULL;
	if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}

	BMD_FOK_CHG(ExecuteSQLQueryWithRowAnswersKnownDBConnection( hDB, SQLQuery, &ansCount, &anss), LIBBMDSQL_GROUP_NOT_REGISTERED_IN_SYSTEM);


	asprintf(&groupsId, "%s", anss[0]);
	if (groupsId==NULL)	{	BMD_FOK(NO_MEMORY);	}

	BMD_FOK(set_gen_buf2(anss[0], (long)strlen(anss[0]), &((*bmdJSResponse)->protocolDataFileId)));
	BMD_FOK(set_gen_buf2(anss[1], (long)strlen(anss[1]), &((*bmdJSResponse)->protocolDataFilename)));

	if (_GLOBAL_bmd_configuration->location_id)
	{
		BMD_FOK(set_gen_buf2(_GLOBAL_bmd_configuration->location_id, (long)strlen(_GLOBAL_bmd_configuration->location_id), &((*bmdJSResponse)->protocolDataFileLocationId)));
	}

	free0(anss);
	free0(SQLQuery);

	/*******************************************/
	/* pobranie informacji o grupie nadrzędnej */
	/*******************************************/
	//SELECT src, dest FROM groups_graph WHERE groups.location_id=%s
	getColumnWithCondition(	_GLOBAL_GroupsGraphDictionary, 1, groupsId, 0, &ansCount, &anss);

	for (i=0; i<ansCount; i++)
	{
		BMD_FOK(set_gen_buf2(anss[i], (long)strlen(anss[i]), &tmp));
		BMD_FOK(bmd_datagram_add_metadata_2(OID_SYS_METADATA_CERT_PARENT_GROUP_ID, tmp, bmdJSResponse, 1));
		free_gen_buf(&tmp);

		//SELECT  id, name FROM groups WHERE groups.location_id=%s
		BMD_FOK_CHG(getElementWithCondition(	_GLOBAL_GroupsDictionary, 0, anss[i], 1, &ans), LIBBMDSQL_DICT_GROUP_VALUE_NOT_FOUND);

		BMD_FOK(set_gen_buf2(ans, (long)strlen(ans), &tmp));
		BMD_FOK(bmd_datagram_add_metadata_2(OID_SYS_METADATA_CERT_PARENT_GROUP_NAME, tmp, bmdJSResponse, 1));
		free_gen_buf(&tmp);



		free0(ans);
		free0(anss[i]);
	}
	free0(anss);

	/*******************************************/
	/* pobranie informacji o grupie podrzędnej */
	/*******************************************/
	//SELECT src, dest FROM groups_graph WHERE groups.location_id=%s
	getColumnWithCondition(	_GLOBAL_GroupsGraphDictionary, 0, groupsId, 1, &ansCount, &anss);

	for (i=0; i<ansCount; i++)
	{
		BMD_FOK(set_gen_buf2(anss[i], (long)strlen(anss[i]), &tmp));
		BMD_FOK(bmd_datagram_add_metadata_2(OID_SYS_METADATA_CERT_CHILD_GROUP_ID, tmp, bmdJSResponse, 1));
		free_gen_buf(&tmp);

		//SELECT  id, name FROM groups WHERE groups.location_id=%s
		BMD_FOK_CHG(getElementWithCondition(	_GLOBAL_GroupsDictionary, 0, anss[i], 1, &ans), LIBBMDSQL_DICT_GROUP_VALUE_NOT_FOUND);

		BMD_FOK(set_gen_buf2(ans, (long)strlen(ans), &tmp));
		BMD_FOK(bmd_datagram_add_metadata_2(OID_SYS_METADATA_CERT_CHILD_GROUP_NAME, tmp, bmdJSResponse, 1));
		free_gen_buf(&tmp);

		free0(anss[i]);
		free0(ans);
	}
	free0(anss);


	/************/
	/* porzadki */
	/************/
	free0(anss);
	free0(SQLQuery);
	free0(groupsId);

	return BMD_OK;
}


/*****************************************/
/*          rejestracja roli             */
/*****************************************/
long JS_bmdDatagramSet_to_registerGroup(	void *hDB,
						bmdDatagramSet_t *bmdJSRequestSet,
						bmdDatagramSet_t **bmdJSResponseSet,
						server_request_data_t *req)
{
long status	= 0;
long i		= 0;

	PRINT_INFO("LIBBMDSQLINF Registering new group (datagramset)\n");
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
	for(i=0; i<bmdJSRequestSet->bmdDatagramSetSize; i++)
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
		if (bmdJSRequestSet->bmdDatagramSetTable[i]->datagramType==BMD_DATAGRAM_TYPE_REGISTER_GROUP)
		{
			status=JS_bmdDatagram_to_registerGroup(hDB,bmdJSRequestSet->bmdDatagramSetTable[i], &((*bmdJSResponseSet)->bmdDatagramSetTable[i]));
		}
 		else if (bmdJSRequestSet->bmdDatagramSetTable[i]->datagramType==BMD_DATAGRAM_TYPE_UPDATE_GROUP)
		{
			status=JS_bmdDatagram_to_updateGroup(hDB,bmdJSRequestSet->bmdDatagramSetTable[i], &((*bmdJSResponseSet)->bmdDatagramSetTable[i]));
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
#endif //ifndef WIN32


	return BMD_OK;
}

long JS_bmdDatagram_to_registerGroup(	void *hDB,
					bmdDatagram_t *bmdJSRequest,
					bmdDatagram_t **bmdJSResponse)
{
long i					= 0;
long groupParentsCount			= 0;
long groupChildrenCount			= 0;
void *query_result			= NULL;
char *groupName				= NULL;
char *SQLQuery				= NULL;
char *groupId				= NULL;
char *groupParentName			= NULL;
char *groupChildName			= NULL;
char **groupParents			= NULL;
char **groupChildren			= NULL;
char* escapeTemp				= NULL;

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if (hDB==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (bmdJSRequest==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	/******************************************************/
	/* poszukiwanie poszczególnych elementów w datagramie */
	/******************************************************/
	for (i=0; i<bmdJSRequest->no_of_sysMetaData; i++)
	{
		/****************************************/
		/* odczytanie nazwy rejestrowanej grupy */
		/****************************************/
		if (strcmp(bmdJSRequest->sysMetaData[i]->OIDTableBuf, OID_SYS_METADATA_CERT_GROUP_NAME)==0)
		{
			if (groupName!=NULL)	{	BMD_FOK(LIBBMDSQL_GROUP_NAME_TOO_MANY);	}
			asprintf(&groupName, "%s", bmdJSRequest->sysMetaData[i]->AnyBuf);
			if (groupName==NULL)	{	BMD_FOK(NO_MEMORY);	}
		}
		/**********************************/
		/* odczytanie nazwy grupy rodzica */
		/**********************************/
		else if (strcmp(bmdJSRequest->sysMetaData[i]->OIDTableBuf, OID_SYS_METADATA_CERT_PARENT_GROUP_ID)==0)
		{
			groupParentsCount++;
			groupParents=(char**)realloc(groupParents, sizeof(char *) * groupParentsCount);
			asprintf(&groupParents[groupParentsCount-1], "%s", bmdJSRequest->sysMetaData[i]->AnyBuf);
			if (groupParents[groupParentsCount-1]==NULL)	{	BMD_FOK(NO_MEMORY);	}
		}
		/***********************************/
		/* odczytanie nazwy grupy potomnej */
		/***********************************/
		else if (strcmp(bmdJSRequest->sysMetaData[i]->OIDTableBuf, OID_SYS_METADATA_CERT_CHILD_GROUP_ID)==0)
		{
			groupChildrenCount++;
			groupChildren=(char**)realloc(groupChildren, sizeof(char *) * groupChildrenCount);
			asprintf(&groupChildren[groupChildrenCount-1], "%s", bmdJSRequest->sysMetaData[i]->AnyBuf);
			if (groupChildren[groupChildrenCount-1]==NULL)	{	BMD_FOK(NO_MEMORY);	}
		}
	}

	if (groupName==NULL)	{	BMD_FOK(LIBBMDSQL_GROUP_NAME_UNDEFINED);	}

	BMD_FOK(CheckGroupNumber());

	/******************************************/
	/* wstawienie informacji do tabeli groups */
	/******************************************/
	BMD_FOK(bmd_db_escape_string(hDB, groupName, STANDARD_ESCAPING, &escapeTemp));
	asprintf(&SQLQuery, 	"INSERT INTO groups VALUES (nextval('groups_id_seq'),%s,'%s');",
					_GLOBAL_bmd_configuration->location_id==NULL?"DEFAULT":_GLOBAL_bmd_configuration->location_id,
					escapeTemp);
	free(escapeTemp); escapeTemp=NULL;
	if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}

	BMD_FOK(bmd_db_execute_sql( hDB, SQLQuery,NULL,NULL, &query_result));

	bmd_db_result_free(&query_result);
	free0(SQLQuery);

	/**************************/
	/* pobranie id nowej roli */
	/**************************/
	asprintf(&SQLQuery, 	"SELECT last_value FROM groups_id_seq;");
	if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}

	BMD_FOK(ExecuteSQLQueryWithAnswerKnownDBConnection(	hDB, SQLQuery, &groupId));
	free0(SQLQuery);
	BMD_FOK(set_gen_buf2(groupId, (long)strlen(groupId), &((*bmdJSResponse)->protocolDataFileId)));

	/***********************************************/
	/* wstawienie informacji o grupach nadrzędnych */
	/***********************************************/
	for (i=0; i<groupParentsCount; i++)
	{
		BMD_FOK_CHG(CheckIfGroupExist(	groupParents[i], &groupParentName),
				LIBBMDSQL_GROUP_PARENT_GROUP_NOT_REGISTERED);

		asprintf(&SQLQuery, 	"INSERT INTO groups_graph VALUES (nextval('groups_graph_id_seq'),%s,%s,currval('groups_id_seq'));",
						_GLOBAL_bmd_configuration->location_id==NULL?"DEFAULT":_GLOBAL_bmd_configuration->location_id,
						groupParents[i]);
		if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}

		BMD_FOK_CHG(bmd_db_execute_sql( hDB, SQLQuery,NULL,NULL, &query_result), LIBBMDSQL_GROUP_PARENT_GROUP_ERR);
		bmd_db_result_free(&query_result);

		bmd_db_result_free(&query_result);
		free0(SQLQuery);
		free0(groupParents[i]);
		free0(groupParentName);
	}
	free0(groupParents);

	/***********************************************/
	/* wstawienie informacji o grupach podrzędnych */
	/***********************************************/
	for (i=0; i<groupChildrenCount; i++)
	{
		BMD_FOK_CHG(CheckIfGroupExist(	groupChildren[i], &groupChildName), LIBBMDSQL_GROUP_CHILD_GROUP_NOT_REGISTERED);

		asprintf(&SQLQuery, 	"INSERT INTO groups_graph VALUES (nextval('groups_graph_id_seq'),%s,currval('groups_id_seq'),%s);",
						_GLOBAL_bmd_configuration->location_id==NULL?"DEFAULT":_GLOBAL_bmd_configuration->location_id,
						groupChildren[i]);
		if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}

		BMD_FOK_CHG(bmd_db_execute_sql( hDB, SQLQuery,NULL,NULL, &query_result), LIBBMDSQL_GROUP_CHILD_GROUP_ERR);
		bmd_db_result_free(&query_result);

		bmd_db_result_free(&query_result);
		free0(SQLQuery);
		free0(groupChildren[i]);
		free0(groupChildName);
	}
	free0(groupChildren);

	/************/
	/* porzadki */
	/************/
	free0(groupId);
	free0(groupName);
	free0(SQLQuery);

	return BMD_OK;
}


long JS_bmdDatagram_to_updateGroup(	void *hDB,
					bmdDatagram_t *bmdJSRequest,
					bmdDatagram_t **bmdJSResponse)
{
long i					= 0;
long groupParentsCount			= 0;
long groupChildrenCount			= 0;
void *query_result			= NULL;
char *SQLQuery				= NULL;
char *groupName				= NULL;
char *groupId				= NULL;
char *tmp				= NULL;
char **groupParents			= NULL;
char **groupChildren			= NULL;
long emptyParentGroupList		= 0;
long emptyChildGroupList		= 0;
char* escapedGroupName			= NULL;

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if (hDB==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (bmdJSRequest==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	if (	(bmdJSRequest->protocolDataFileId!=NULL) &&
		(bmdJSRequest->protocolDataFileId->buf!=NULL) &&
		(bmdJSRequest->protocolDataFileId->size>0) )
	{
		asprintf(&groupId, "%s", bmdJSRequest->protocolDataFileId->buf);
		if (groupId==NULL)	{	BMD_FOK(NO_MEMORY);	}
	}

	if (groupId==NULL)	{	BMD_FOK(LIBBMDSQL_GROUP_ID_UNDEFINED);	}

	/******************************************************/
	/* poszukiwanie poszczególnych elementów w datagramie */
	/******************************************************/
	for (i=0; i<bmdJSRequest->no_of_sysMetaData; i++)
	{
		/**************************************/
		/* odczytanie nazwy updatowanej grupy */
		/**************************************/
		if (strcmp(bmdJSRequest->sysMetaData[i]->OIDTableBuf, OID_SYS_METADATA_CERT_GROUP_NAME)==0)
		{
			if (groupName!=NULL)	{	BMD_FOK(LIBBMDSQL_GROUP_NAME_TOO_MANY);	}
			asprintf(&groupName, "%s", bmdJSRequest->sysMetaData[i]->AnyBuf);
			if (groupName==NULL)	{	BMD_FOK(NO_MEMORY);	}
		}
		/****************************************************/
		/* odczytanie , czy grupa ma nie miec grupy rodzica */
		/****************************************************/
		else if (strcmp(bmdJSRequest->sysMetaData[i]->OIDTableBuf, OID_SYS_METADATA_CERT_EMPTY_PARENT_GROUP_LIST)==0)
		{
			emptyParentGroupList=1;
		}
		/***********************************************/
		/* odczytanie , czy grupa ma nie miec potomkow */
		/***********************************************/
		else if (strcmp(bmdJSRequest->sysMetaData[i]->OIDTableBuf, OID_SYS_METADATA_CERT_EMPTY_CHILD_GROUP_LIST)==0)
		{
			emptyChildGroupList=1;
		}
		/*******************************/
		/* odczytanie id grupy rodzica */
		/*******************************/
		else if (strcmp(bmdJSRequest->sysMetaData[i]->OIDTableBuf, OID_SYS_METADATA_CERT_PARENT_GROUP_ID)==0)
		{
			groupParentsCount++;
			groupParents=(char**)realloc(groupParents, sizeof(char *) * groupParentsCount);
			asprintf(&groupParents[groupParentsCount-1], "%s", bmdJSRequest->sysMetaData[i]->AnyBuf);
			if (groupParents[groupParentsCount-1]==NULL)	{	BMD_FOK(NO_MEMORY);	}
		}
		/********************************/
		/* odczytanie id grupy potomnej */
		/********************************/
		else if (strcmp(bmdJSRequest->sysMetaData[i]->OIDTableBuf, OID_SYS_METADATA_CERT_CHILD_GROUP_ID)==0)
		{
			groupChildrenCount++;
			groupChildren=(char**)realloc(groupChildren, sizeof(char *) * groupChildrenCount);
			asprintf(&groupChildren[groupChildrenCount-1], "%s", bmdJSRequest->sysMetaData[i]->AnyBuf);
			if (groupChildren[groupChildrenCount-1]==NULL)	{	BMD_FOK(NO_MEMORY);	}
		}
	}

	if (groupId==NULL)	{	BMD_FOK(LIBBMDSQL_GROUP_ID_UNDEFINED);	}

	BMD_FOK(CheckIfGroupExist(	groupId, &tmp));
	free0(tmp);
	
	/**********************/
	/* zmiana nazwy grupy */
	/**********************/
	if (groupName!=NULL)
	{
		BMD_FOK(bmd_db_escape_string(hDB, groupName, STANDARD_ESCAPING, &escapedGroupName));
		asprintf(&SQLQuery, 	"UPDATE groups SET name='%s' WHERE id=%s%s%s;",
						escapedGroupName,
						groupId,
						_GLOBAL_bmd_configuration->location_id==NULL?"":" AND location_id=",
						_GLOBAL_bmd_configuration->location_id==NULL?"":_GLOBAL_bmd_configuration->location_id);
		free(escapedGroupName); escapedGroupName=NULL;
		BMD_FOK(bmd_db_execute_sql( hDB, SQLQuery,NULL,NULL, &query_result));
		bmd_db_result_free(&query_result);
		free0(SQLQuery);
	}

	/**********************************************/
	/* usunięcie informacji o grupach nadrzędnych */
	/**********************************************/
	if( (groupParentsCount>0) || (groupParentsCount==0 && emptyParentGroupList==1) )
	{
		asprintf(&SQLQuery, 	"DELETE FROM groups_graph WHERE dest=%s%s%s;",
					groupId,
					_GLOBAL_bmd_configuration->location_id==NULL?"":" AND location_id=",
					_GLOBAL_bmd_configuration->location_id==NULL?"":_GLOBAL_bmd_configuration->location_id);
	
		BMD_FOK(bmd_db_execute_sql( hDB, SQLQuery,NULL,NULL, &query_result));
		bmd_db_result_free(&query_result);
		free0(SQLQuery);
	}

	/************************************************/
	/* usunięcie informacji o grupach podrzędnych */
	/***********************************************/
	if( (groupChildrenCount>0) || (groupChildrenCount==0 && emptyChildGroupList==1) )
	{
		asprintf(&SQLQuery, 	"DELETE FROM groups_graph WHERE src=%s%s%s;",
					groupId,
					_GLOBAL_bmd_configuration->location_id==NULL?"":" AND location_id=",
					_GLOBAL_bmd_configuration->location_id==NULL?"":_GLOBAL_bmd_configuration->location_id);
	
		BMD_FOK(bmd_db_execute_sql( hDB, SQLQuery,NULL,NULL, &query_result));
		bmd_db_result_free(&query_result);
		free0(SQLQuery);
	}

	/***********************************************/
	/* wstawienie informacji o grupach nadrzędnych */
	/***********************************************/
	if( groupParentsCount>0 && emptyParentGroupList==0 )
	{
		for (i=0; i<groupParentsCount; i++)
		{
			BMD_FOK_CHG(CheckIfGroupExist(	groupParents[i], &tmp), LIBBMDSQL_GROUP_PARENT_GROUP_NOT_REGISTERED);
			free0(tmp);
	
			asprintf(&SQLQuery, 	"INSERT INTO groups_graph VALUES (nextval('groups_graph_id_seq'),%s,%s,%s);",
							_GLOBAL_bmd_configuration->location_id==NULL?"DEFAULT":_GLOBAL_bmd_configuration->location_id,
							groupParents[i],
							groupId);
			if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}
	
			BMD_FOK_CHG(bmd_db_execute_sql( hDB, SQLQuery,NULL,NULL, &query_result), LIBBMDSQL_GROUP_PARENT_GROUP_ERR);
			bmd_db_result_free(&query_result);
			free0(SQLQuery);
			free0(groupParents[i]);
		}
		free0(groupParents);
	}

	/***********************************************/
	/* wstawienie informacji o grupach podrzędnych */
	/***********************************************/
	if( groupChildrenCount>0 && emptyChildGroupList==0 )
	{
		for (i=0; i<groupChildrenCount; i++)
		{
			BMD_FOK_CHG(CheckIfGroupExist(	groupChildren[i], &tmp), LIBBMDSQL_GROUP_CHILD_GROUP_NOT_REGISTERED);
			free0(tmp);
	
			asprintf(&SQLQuery, 	"INSERT INTO groups_graph VALUES (nextval('groups_graph_id_seq'),%s,%s,%s);",
							_GLOBAL_bmd_configuration->location_id==NULL?"DEFAULT":_GLOBAL_bmd_configuration->location_id,
							groupId,
							groupChildren[i]);
			if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}
	
			BMD_FOK_CHG(bmd_db_execute_sql( hDB, SQLQuery,NULL,NULL, &query_result), LIBBMDSQL_GROUP_CHILD_GROUP_ERR);
			bmd_db_result_free(&query_result);
			free0(SQLQuery);
			free0(groupChildren[i]);
		}
		free0(groupChildren);
	}

	/************/
	/* porzadki */
	/************/
	free0(groupId);
	free0(groupName);
	free0(SQLQuery);

	return BMD_OK;
}
