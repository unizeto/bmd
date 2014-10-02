
#include <bmd/libbmdsql/role.h>
#include <bmd/libbmdsql/user.h>
#include <bmd/libbmdsql/group.h>
#include <bmd/libbmdprotocol/libbmdprotocol.h>
#include <bmd/libbmdpr/bmd_resp/bmd_resp.h>
#include <bmd/libbmdglobals/libbmdglobals.h>
#include <signal.h>

#ifdef WIN32
#pragma warning(disable:4204)
#pragma warning(disable:4100)
#pragma warning(disable:4127)
#endif

long CheckRoleNumber()
{
long roleCount	= 0;

	if (_GLOBAL_bmd_configuration->max_registered_roles>0)
	{
		//SELECT id, name FROM roles;
		getCountWithCondition(	_GLOBAL_RolesDictionary, 0, NULL, &roleCount);

		if (roleCount>=_GLOBAL_bmd_configuration->max_registered_roles)
		{
			return LIBBMDSQL_ROLE_TOO_MANY_REGISTERED_ROLES;
		}
	}

	return BMD_OK;
}

/****************************************/
/* sprawdzenie, czy uzytkownik istnieje */
/****************************************/
long CheckIfRoleExist(	char *roleId,
			char **roleName)
{
	/************************/
	/* walidacja parametrow */
	/************************/
	if (roleId==NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (roleName==NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if ((*roleName)!=NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	//SELECT users_and_roles.fk_users, users_and_roles.default_role, roles.id, roles.name, users_and_roles.fk_users::varchar||'|'||roles.name::varchar FROM roles LEFT JOIN users_and_roles ON (users_and_roles.fk_roles=roles.id);
	BMD_FOK_CHG(getElementWithCondition(	_GLOBAL_UserRolesDictionary, 2, roleId, 3, roleName), LIBBMDSQL_DICT_ROLE_VALUE_NOT_FOUND);

	if ((*roleName)==NULL)	{	return LIBBMDSQL_ROLE_NOT_REGISTERED_IN_SYSTEM;	}

	return BMD_OK;

}

/****************************************/
/* sprawdzenie, czy uzytkownik istnieje */
/****************************************/
long CheckIfRoleExistByName(	char *roleName)
{
char * roleId	= NULL;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (roleName==NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}

	//SELECT users_and_roles.fk_users, users_and_roles.default_role, roles.id, roles.name, users_and_roles.fk_users::varchar||'|'||roles.name::varchar FROM roles LEFT JOIN users_and_roles ON (users_and_roles.fk_roles=roles.id);
	getElementWithCondition(	_GLOBAL_UserRolesDictionary, 3, roleName, 2, &roleId);

	if (roleId==NULL)	{	return BMD_OK;	}

	/************/
	/* porzadki */
	/************/
	free0(roleId);

	return LIBBMDSQL_ROLE_ROLE_ALREADY_REGISTERED;
}

long CheckIfUserHasRole(	char *roleName,
				char *userId)
{
char *tmp	= NULL;
char *roleId	= NULL;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (roleName==NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (userId==NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	asprintf(&tmp, "%s|%s", userId, roleName);
	if (tmp==NULL)		{	BMD_FOK(NO_MEMORY);	}

	//SELECT users_and_roles.fk_users, users_and_roles.default_role, roles.id, roles.name, users_and_roles.fk_users||roles.name FROM roles LEFT JOIN users_and_roles ON (users_and_roles.fk_roles=roles.id);
	BMD_FOK_CHG(getElementWithCondition(	_GLOBAL_UserRolesDictionary, 4, tmp, 2, &roleId), LIBBMDSQL_ROLE_NOT_REGISTERED_IN_SYSTEM);

	if (roleId==NULL)	{	return LIBBMDSQL_ROLE_NOT_REGISTERED_IN_SYSTEM;	}

	/************/
	/* porzadki */
	/************/
	free0(roleId);
	free0(tmp);

	return BMD_OK;

}

/**********************************************************/
/* zmiana domyślnej (wybranej/aktualnej) roli użytkownika */
/**********************************************************/
long JS_bmdDatagramSet_to_changeChosenRole(	void *hDB,
						bmdDatagramSet_t *bmdJSRequestSet,
						bmdDatagramSet_t **bmdJSResponseSet,
						server_request_data_t *req)
{
long status	= 0;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (hDB==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (bmdJSRequestSet==NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	/*****************************************/
	/* przygotowanie datagramsetu odpowiedzi */
	/*****************************************/
	BMD_FOK(PR2_bmdDatagramSet_fill_with_datagrams(1, bmdJSResponseSet));

	/***********************/
	/* poczatek transakcji */
	/***********************/
	BMD_FOK(bmd_db_start_transaction(hDB,BMD_TRANSACTION_LEVEL_NONE));

	status=JS_bmdDatagram_to_changeChosenRole(	hDB,
							bmdJSRequestSet->bmdDatagramSetTable[0],
							&((*bmdJSResponseSet)->bmdDatagramSetTable[0]),
							req);
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
	(*bmdJSResponseSet)->bmdDatagramSetTable[0]->datagramType = bmdJSRequestSet->bmdDatagramSetTable[0]->datagramType+100;
	(*bmdJSResponseSet)->bmdDatagramSetTable[0]->datagramStatus = status;

	return BMD_OK;
}

long JS_bmdDatagram_to_changeChosenRole(	void *hDB,
						bmdDatagram_t *bmdJSRequest,
						bmdDatagram_t **bmdJSResponse,
						server_request_data_t *req)
{
long i			= 0;
char *roleId		= NULL;
char *roleName		= NULL;
char *groupId		= NULL;
char *groupName		= NULL;
char *userId		= NULL;
GenBuf_t *der_form	= NULL;

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
		/***************************************/
		/* odczytanie nazwy rejestrowanej roli */
		/***************************************/
		if (strcmp(bmdJSRequest->sysMetaData[i]->OIDTableBuf, OID_SYS_METADATA_CERT_CURRENT_ROLE_ID)==0)
		{
			if (roleId!=NULL)	{	BMD_FOK(LIBBMDSQL_ROLE_ID_TOO_MANY);	}
			asprintf(&roleId, "%s", bmdJSRequest->sysMetaData[i]->AnyBuf);
			if (roleId==NULL)	{	BMD_FOK(NO_MEMORY);	}
		}

		/****************************************/
		/* odczytanie nazwy rejestrowanej grupy */
		/****************************************/
		if (strcmp(bmdJSRequest->sysMetaData[i]->OIDTableBuf, OID_SYS_METADATA_CERT_CURRENT_GROUP_ID)==0)
		{
			if (groupId!=NULL)	{	BMD_FOK(LIBBMDSQL_GROUP_ID_TOO_MANY);	}
			asprintf(&groupId, "%s", bmdJSRequest->sysMetaData[i]->AnyBuf);
			if (groupId==NULL)	{	BMD_FOK(NO_MEMORY);	}
		}
	}

	if ((roleId==NULL) && (groupId==NULL))	{	BMD_FOK(LIBBMDSQL_ROLE_ID_UNDEFINED);	}

	/*********************************************************************/
	/* sprawdzenie poprawnosci danych umieszczonych w datagramie zadania */
	/*********************************************************************/
	if (roleId!=NULL)
	{
		BMD_FOK(CheckIfRoleExist(	roleId, &roleName));
		BMD_FOK(CheckIfUserExist2(	bmdJSRequest->protocolDataOwner->buf, &userId));
		BMD_FOK(CheckIfUserHasRole(	roleName, userId));

		/*****************************************/
		/* zmiana dodtychczasowej roli domyślnej */
		/*****************************************/
		free_gen_buf(&(req->ua->user_chosen_role));
		BMD_FOK(set_gen_buf2 (	roleName, (long)strlen(roleName), &(req->ua->user_chosen_role)));
		free0(userId);

	}
	if (groupId!=NULL)
	{
		BMD_FOK(CheckIfGroupExist(	groupId, &groupName));
		BMD_FOK(CheckIfUserExist2(	bmdJSRequest->protocolDataOwner->buf, &userId));
		BMD_FOK(CheckIfUserHasGroup(	groupId, userId));

		/*****************************************/
		/* zmiana dodtychczasowej roli domyślnej */
		/*****************************************/
		free_gen_buf(&(req->ua->user_chosen_group));
		BMD_FOK(set_gen_buf2 (	groupId, (long)strlen(groupId), &(req->ua->user_chosen_group)));
		free0(userId);
	}

	/*********************************************************************/
	/* ustawienie aktualnego formularza zgodnie z nowo zdefiniowana rola */
	/*********************************************************************/
	i=0;
	while (req->user_forms[i]!=NULL)
	{
		if (strcmp(req->user_forms[i]->roleName, req->ua->user_chosen_role->buf)==0)
		{
			req->formularz=req->user_forms[i]->formularz;
			req->no_of_formularz=req->user_forms[i]->no_of_formularz;
			break;
		}
		i++;
	}

	/****************************************************************************/
	/* przygotowanie i wstawienie do datagramu odpowiedzi aktualnego formularza */
	/****************************************************************************/
	BMD_FOK(bmd_kontrolki_serialize(req->formularz, &der_form));
	BMD_FOK(bmd2_datagram_set_form(der_form, bmdJSResponse));

	/*******************************************************************/
	/* ustawienie w datagramie odpowiedzi akcji dla nowo wybranej roli */
	/*******************************************************************/
	BMD_FOK(mask_user_action_with_roles_and_actions( 	&(req->actions),
								&(req->no_of_actions),
								req->ua->user_chosen_role));

	/*****************************************************************/
	/* ustawienie w datagramie odpowiedzi nowo wybranej roli i grupy */
	/*****************************************************************/
 	BMD_FOK(bmd2_datagram_set_current_role_and_group(	req->ua->user_chosen_role->buf,
								req->ua->user_chosen_group->buf,
								bmdJSResponse));

	/****************************************************************************/
	/* ustawienie w datagramie odpowiedzi akcji dosepnych dla nowowybranej roli */
	/****************************************************************************/
	BMD_FOK(bmd2_datagram_set_actions( req->actions, req->no_of_actions, bmdJSResponse));

	/****************************************************************************************************/
	/* ustawienie w datagramie odpowiedzi dostepne role, grupy i poziomy bezpieczenstwa dla uzytkownika */
	/****************************************************************************************************/
	BMD_FOK(bmd2_datagram_set_role_list(req->ua->roles, bmdJSResponse));
	BMD_FOK(bmd2_datagram_set_group_list(req->ua->groups, bmdJSResponse));
	BMD_FOK(bmd2_datagram_set_class_list(req->ua->serial, req->ua->cert_dn, bmdJSResponse));
	BMD_FOK(bmd2_datagram_set_security_list(req->ua->security, bmdJSResponse));

	/************/
	/* porzadki */
	/************/
	free0(groupId);
	free0(groupName);
	free0(roleId);
	free0(roleName);
	free0(userId);
	free_gen_buf(&der_form);

	return BMD_OK;
}


/*******************************************************/
/*          pobieranie informacji o rolach             */
/*******************************************************/
long JS_bmdDatagramSet_to_getRoleList(	void *hDB,
					bmdDatagramSet_t *bmdJSRequestSet,
					bmdDatagramSet_t **bmdJSResponseSet,
					server_request_data_t *req)
{
long status		= 0;
long i			= 0;
long rolesNumber	= 0;
char *roleId		= NULL;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (hDB==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (bmdJSRequestSet==NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	if (	(bmdJSRequestSet->bmdDatagramSetTable[0]->protocolDataFileId!=NULL) &&
		(bmdJSRequestSet->bmdDatagramSetTable[0]->protocolDataFileId->buf!=NULL) &&
		(bmdJSRequestSet->bmdDatagramSetTable[0]->protocolDataFileId->size>0) )
	{
		asprintf(&roleId, "%s", bmdJSRequestSet->bmdDatagramSetTable[0]->protocolDataFileId->buf);
		if (roleId==NULL)	{	BMD_FOK(NO_MEMORY);	}
	}

	if (roleId==NULL)
	{
		/**************************************/
		/* odczytanie informacji o ilosci rol */
		/**************************************/
		//SELECT * FROM role_rights LEFT JOIN roles ON (roles.id=role_rights.fk_roles AND roles.location_id=role_rights.location_id) WHERE roles.location_id=%s;
		BMD_FOK_CHG(getCountWithCondition(	_GLOBAL_RoleRightsDictionary, 0, NULL, &rolesNumber), LIBBMDSQL_ROLE_LIST_NO_ROLES);
	}
	else
	{
		rolesNumber=1;
	}

	/*****************************************/
	/* przygotowanie datagramsetu odpowiedzi */
	/*****************************************/
	BMD_FOK(PR2_bmdDatagramSet_fill_with_datagrams(rolesNumber, bmdJSResponseSet));

	/*******************************************/
	/* pobieranie danych poszczególnych userów */
	/*******************************************/
	for (i=0; i<rolesNumber; i++)
	{
		/**************************************/
		/* rozpoczecie transakcji bazy danych */
		/**************************************/
		BMD_FOK(bmd_db_start_transaction(hDB,BMD_TRANSACTION_LEVEL_NONE));

		status=JS_bmdDatagram_to_getRoleList(hDB, i, roleId, &((*bmdJSResponseSet)->bmdDatagramSetTable[i]),req);

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
	free0(roleId);

	return BMD_OK;
}

long JS_bmdDatagram_to_getRoleList(	void *hDB,
					long number,
					char *roleId,
					bmdDatagram_t **bmdJSResponse,
					server_request_data_t *req)
{
long ansCount		= 0;
long i			= 0;
char **anss		= NULL;
char *ans		= NULL;
char *SQLQuery		= NULL;
char *rolesId		= NULL;
char* escapedRoleId	= NULL;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (hDB==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
 	if (number<0)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	/***************************************/
	/* pobranie glownych informacji o roli */
	/***************************************/
	if(roleId != NULL)
	{
		BMD_FOK(bmd_db_escape_string(hDB, roleId, STANDARD_ESCAPING, &escapedRoleId));
	}
	asprintf(&SQLQuery, 	"SELECT id, name FROM roles %s%s %s%s %s%s LIMIT 1 OFFSET %li;",
					_GLOBAL_bmd_configuration->location_id!=NULL?"WHERE location_id=":"",
					_GLOBAL_bmd_configuration->location_id!=NULL?_GLOBAL_bmd_configuration->location_id:"",
					(roleId!=NULL && _GLOBAL_bmd_configuration->location_id!=NULL)?"AND id=":"",
					(roleId!=NULL && _GLOBAL_bmd_configuration->location_id!=NULL)?escapedRoleId:"",
					(roleId!=NULL && _GLOBAL_bmd_configuration->location_id==NULL)?"WHERE id=":"",
					(roleId!=NULL && _GLOBAL_bmd_configuration->location_id==NULL)?escapedRoleId:"",
					number);
	free(escapedRoleId); escapedRoleId=NULL;

	if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}

	BMD_FOK(ExecuteSQLQueryWithRowAnswersKnownDBConnection( hDB, SQLQuery, &ansCount, &anss));

	asprintf(&rolesId, "%s", anss[0]);
	if (rolesId==NULL)	{	BMD_FOK(NO_MEMORY);	}

	BMD_FOK(set_gen_buf2(anss[0], (long)strlen(anss[0]), &((*bmdJSResponse)->protocolDataFileId)));
	BMD_FOK(set_gen_buf2(anss[1], (long)strlen(anss[1]), &((*bmdJSResponse)->protocolDataFilename)));
	if (_GLOBAL_bmd_configuration->location_id)
	{
		BMD_FOK(set_gen_buf2(_GLOBAL_bmd_configuration->location_id, (long)strlen(_GLOBAL_bmd_configuration->location_id), &((*bmdJSResponse)->protocolDataFileLocationId)));
	}

	free0(anss[0]);
	free0(anss[1]);
	free0(anss);

	/********************************************/
	/* pobranie informacji o akcjach danej roli */
	/********************************************/
	//SELECT roles.id, roles.name, actions.id, actions.name, actions.registration, actions.notification FROM roles_and_actions LEFT JOIN roles ON (roles_and_actions.fk_roles_id=roles.id) LEFT JOIN actions ON (roles_and_actions.fk_actions_id=actions.id);
	BMD_FOK(getColumnWithCondition(	_GLOBAL_RolesAndActionsDictionary, 0, rolesId, 2, &ansCount, &anss));

	for (i=0; i<ansCount; i++)
	{
		if (strlen(anss[i])<=0)
		{
			continue;
		}
		BMD_FOK(bmd_datagram_add_metadata_char(OID_SYS_METADATA_ACTION, anss[i], bmdJSResponse));

		free0(anss[i]);
	}
	free0(anss);
	free0(SQLQuery);

	/*************************************/
	/* pobranie informacji o role_rights */
	/*************************************/
	BMD_FOK(getColumnWithCondition(	_GLOBAL_RoleRightsStructureDictionary, 0, NULL, 0, &ansCount, &anss));

	for (i=2; i<ansCount; i++)
	{
		//SELECT * FROM role_rights LEFT JOIN roles ON (roles.id=role_rights.fk_roles AND roles.location_id=role_rights.location_id) WHERE roles.location_id=%s;
		BMD_FOK_CHG(getElementWithCondition(	_GLOBAL_RoleRightsDictionary, 3, rolesId, i+1, &ans), LIBBMDSQL_DICT_ROLE_VALUE_NOT_FOUND);
		BMD_FOK(bmd_datagram_add_metadata_char_type(anss[i], SYS_METADATA, ans, bmdJSResponse));

		free0(ans);
		free0(anss[i]);
	}

	/************/
	/* porzadki */
	/************/
	free0(anss);
	free0(SQLQuery);
	free0(rolesId);

	return BMD_OK;
}


/*****************************************/
/*          rejestracja roli             */
/*****************************************/
long JS_bmdDatagramSet_to_registerRole(	void *hDB,
					bmdDatagramSet_t *bmdJSRequestSet,
					bmdDatagramSet_t **bmdJSResponseSet,
					server_request_data_t *req)
{
long status				= 0;
long i				= 0;

	PRINT_INFO("LIBBMDSQLINF Registering new role (datagramset)\n");
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

		/***********************/
		/* poczatek transakcji */
		/***********************/
		BMD_FOK(bmd_db_start_transaction(hDB,BMD_TRANSACTION_LEVEL_NONE));

		/*************************************/
		/* obsluzenie pojedynczego datagramu */
		/*************************************/
		if (bmdJSRequestSet->bmdDatagramSetTable[i]->datagramType==BMD_DATAGRAM_TYPE_REGISTER_ROLE)
		{
			status=JS_bmdDatagram_to_registerRole(hDB,bmdJSRequestSet->bmdDatagramSetTable[i],
				&((*bmdJSResponseSet)->bmdDatagramSetTable[i]),req);
		}
		else if (bmdJSRequestSet->bmdDatagramSetTable[i]->datagramType==BMD_DATAGRAM_TYPE_UPDATE_ROLE)
		{
			status=JS_bmdDatagram_to_updateRole(hDB,bmdJSRequestSet->bmdDatagramSetTable[i],
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
#endif //ifndef WIN32


	return BMD_OK;
}

long JS_bmdDatagram_to_registerRole(	void *hDB,
					bmdDatagram_t *bmdJSRequest,
					bmdDatagram_t **bmdJSResponse,
					server_request_data_t *req)
{
long i					= 0;
long roleActionsCount			= 0;
long roleRightsOidsCount		= 0;
long roleRightsCodesCount		= 0;
void *query_result			= NULL;
char *roleName				= NULL;
char *SQLQuery				= NULL;
char *roleId				= NULL;
char **roleActions			= NULL;
char **roleRightsOids			= NULL;
char **roleRightsCodes			= NULL;
char* escapeTemp			= NULL;
char* escapedRROid			= NULL;
char* escapedRRCode			= NULL;

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
		/***************************************/
		/* odczytanie nazwy rejestrowanej roli */
		/***************************************/
		if (strcmp(bmdJSRequest->sysMetaData[i]->OIDTableBuf, OID_SYS_METADATA_CERT_ROLE_NAME)==0)
		{
			if (roleName!=NULL)	{	BMD_FOK(LIBBMDSQL_ROLE_NAME_TOO_MANY);	}
			asprintf(&roleName, "%s", bmdJSRequest->sysMetaData[i]->AnyBuf);
			if (roleName==NULL)	{	BMD_FOK(NO_MEMORY);	}
		}
		/**************************************/
		/* odczytanie kolejnej akcji dla roli */
		/**************************************/
		else if (strcmp(bmdJSRequest->sysMetaData[i]->OIDTableBuf, OID_SYS_METADATA_ACTION)==0)
		{
			roleActionsCount++;
			roleActions=(char**)realloc(roleActions, sizeof(char *) * roleActionsCount);
			asprintf(&roleActions[roleActionsCount-1], "%s", bmdJSRequest->sysMetaData[i]->AnyBuf);
			if (roleActions[roleActionsCount-1]==NULL)	{	BMD_FOK(NO_MEMORY);	}
		}
		/***********************************/
		/* odczytanie role rights dla roli */
		/***********************************/
		else
		{
			if (strncmp(bmdJSRequest->sysMetaData[i]->OIDTableBuf, "rr", 2)==0)
			{
				roleRightsOidsCount++;
				roleRightsOids=(char**)realloc(roleRightsOids, sizeof(char *) * roleRightsOidsCount);
				asprintf(&roleRightsOids[roleRightsOidsCount-1], "%s", (bmdJSRequest->sysMetaData[i]->OIDTableBuf));
				if (roleRightsOids[roleRightsOidsCount-1]==NULL)	{	BMD_FOK(NO_MEMORY);	}

				roleRightsCodesCount++;
				roleRightsCodes=(char**)realloc(roleRightsCodes, sizeof(char *) * roleRightsCodesCount);
				asprintf(&roleRightsCodes[roleRightsCodesCount-1], "%s", bmdJSRequest->sysMetaData[i]->AnyBuf);
				if (roleRightsCodes[roleRightsCodesCount-1]==NULL)	{	BMD_FOK(NO_MEMORY);	}
			}
		}
	}

	if (roleName==NULL)	{	BMD_FOK(LIBBMDSQL_ROLE_NAME_UNDEFINED);	}

	BMD_FOK(CheckIfRoleExistByName(roleName));
	BMD_FOK(CheckRoleNumber());

	/*****************************************/
	/* wstawienie informacji do tabeli roles */
	/*****************************************/
	asprintf(&SQLQuery, 	"INSERT INTO roles VALUES (nextval('roles_id_seq'),%s,'%s');",
					_GLOBAL_bmd_configuration->location_id==NULL?"DEFAULT":_GLOBAL_bmd_configuration->location_id,
					roleName);
	if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}

	BMD_FOK(bmd_db_execute_sql( hDB, SQLQuery,NULL,NULL, &query_result));

	bmd_db_result_free(&query_result);
	free0(SQLQuery);

	/**************************/
	/* pobranie id nowej roli */
	/**************************/
	asprintf(&SQLQuery, 	"SELECT last_value FROM roles_id_seq;");
	if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}

	BMD_FOK(ExecuteSQLQueryWithAnswerKnownDBConnection(	hDB, SQLQuery, &roleId));
	free0(SQLQuery);

	BMD_FOK(set_gen_buf2(roleId, (long)strlen(roleId), &((*bmdJSResponse)->protocolDataFileId)));

	/****************************************/
	/* wstawienie informacji o akcjach roli */
	/****************************************/
	for (i=0; i<roleActionsCount; i++)
	{
		BMD_FOK(bmd_db_escape_string(hDB, roleActions[i], STANDARD_ESCAPING, &escapeTemp));
		asprintf(&SQLQuery, 	"INSERT INTO roles_and_actions VALUES (nextval('roles_and_actions_id_seq'),%s,currval('roles_id_seq'),%s);",
						_GLOBAL_bmd_configuration->location_id==NULL?"DEFAULT":_GLOBAL_bmd_configuration->location_id,
						escapeTemp);
		free(escapeTemp); escapeTemp=NULL;
		if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}

		BMD_FOK_CHG(bmd_db_execute_sql( hDB, SQLQuery,NULL,NULL, &query_result),
				LIBBMDSQL_ROLE_ACTION_ERROR);
		bmd_db_result_free(&query_result);

		bmd_db_result_free(&query_result);
		free(SQLQuery); SQLQuery=NULL;
		free(roleActions[i]); roleActions[i]=NULL;
	}
	free(roleActions); roleActions=NULL;

	/**********************************************/
	/* wstawienie informacji role_rights dla roli */
	/**********************************************/
	asprintf(&SQLQuery, 	"INSERT INTO role_rights (id, location_id, fk_roles) VALUES (nextval('role_rights_id_seq'), %s, currval('roles_id_seq'));",
					_GLOBAL_bmd_configuration->location_id==NULL?"DEFAULT":_GLOBAL_bmd_configuration->location_id);
	if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}

	BMD_FOK_CHG(bmd_db_execute_sql( hDB, SQLQuery,NULL,NULL, &query_result), LIBBMDSQL_ROLE_ROLE_RIGHTS_ERROR);
	bmd_db_result_free(&query_result);
	free0(SQLQuery);

	for (i=0; i<roleRightsOidsCount; i++)
	{
		BMD_FOK(bmd_str_replace(&(roleRightsOids[i]), ".", "_"));
		BMD_FOK(bmd_db_escape_string(hDB, roleRightsOids[i], STANDARD_ESCAPING, &escapedRROid));
		BMD_FOK(bmd_db_escape_string(hDB, roleRightsCodes[i], STANDARD_ESCAPING, &escapedRRCode));
		asprintf(&SQLQuery, 	"UPDATE role_rights SET %s = '%s' WHERE fk_roles=%s%s%s;",
						escapedRROid,
						escapedRRCode,
						roleId,
						_GLOBAL_bmd_configuration->location_id==NULL?"":" AND location_id=",
						_GLOBAL_bmd_configuration->location_id==NULL?"":_GLOBAL_bmd_configuration->location_id);
		free(escapedRROid); escapedRROid=NULL;
		free(escapedRRCode); escapedRRCode=NULL;
		if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}

		BMD_FOK_CHG(bmd_db_execute_sql( hDB, SQLQuery,NULL,NULL, &query_result), LIBBMDSQL_ROLE_ROLE_RIGHTS_ERROR);
		bmd_db_result_free(&query_result);

		bmd_db_result_free(&query_result);
		free0(SQLQuery);
		free0(roleRightsOids[i]);
		free0(roleRightsCodes[i]);
	}

	/************/
	/* porzadki */
	/************/
	free0(roleId);
	free0(roleRightsOids);
	free0(roleRightsCodes);
	free0(roleName);
	free0(SQLQuery);

	return BMD_OK;
}

/***********************************************/
/*          update danych i praw roli          */
/***********************************************/
long JS_bmdDatagram_to_updateRole(	void *hDB,
					bmdDatagram_t *bmdJSRequest,
					bmdDatagram_t **bmdJSResponse,
					server_request_data_t *req)
{
long i					= 0;
long roleActionsCount			= 0;
long roleRightsOidsCount		= 0;
long roleRightsCodesCount		= 0;
void *query_result			= NULL;
char *tmp				= NULL;
char *roleName				= NULL;
char *SQLQuery				= NULL;
char *roleId				= NULL;
char **roleActions			= NULL;
char **roleRightsOids			= NULL;
char **roleRightsCodes			= NULL;
char* escapedRROid			= NULL;
char* escapedRRCode			= NULL;
char* escapeTemp				= NULL;

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if (hDB==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (bmdJSRequest==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	if (	(bmdJSRequest->protocolDataFileId!=NULL) &&
		(bmdJSRequest->protocolDataFileId->buf!=NULL) &&
		(bmdJSRequest->protocolDataFileId->size>0) )
	{
		asprintf(&roleId, "%s", bmdJSRequest->protocolDataFileId->buf);
		if (roleId==NULL)	{	BMD_FOK(NO_MEMORY);	}
	}

	if (roleId==NULL)	{	BMD_FOK(LIBBMDSQL_ROLE_ID_UNDEFINED);	}

	/******************************************************/
	/* poszukiwanie poszczególnych elementów w datagramie */
	/******************************************************/
	for (i=0; i<bmdJSRequest->no_of_sysMetaData; i++)
	{
		/***************************************/
		/* odczytanie nazwy rejestrowanej roli */
		/***************************************/
		if (strcmp(bmdJSRequest->sysMetaData[i]->OIDTableBuf, OID_SYS_METADATA_CERT_ROLE_NAME)==0)
		{
			if (roleName!=NULL)	{	BMD_FOK(LIBBMDSQL_ROLE_NAME_TOO_MANY);	}
			asprintf(&roleName, "%s", bmdJSRequest->sysMetaData[i]->AnyBuf);
			if (roleName==NULL)	{	BMD_FOK(NO_MEMORY);	}
		}
		/**************************************/
		/* odczytanie kolejnej akcji dla roli */
		/**************************************/
		else if (strcmp(bmdJSRequest->sysMetaData[i]->OIDTableBuf, OID_SYS_METADATA_ACTION)==0)
		{
			roleActionsCount++;
			roleActions=(char**)realloc(roleActions, sizeof(char *) * roleActionsCount);
			asprintf(&roleActions[roleActionsCount-1], "%s", bmdJSRequest->sysMetaData[i]->AnyBuf);
			if (roleActions[roleActionsCount-1]==NULL)	{	BMD_FOK(NO_MEMORY);	}
		}
	}

	for (i=0; i<bmdJSRequest->no_of_additionalMetaData; i++)
	{
		/***********************************/
		/* odczytanie role rights dla roli */
		/***********************************/
		if (strncmp(bmdJSRequest->additionalMetaData[i]->OIDTableBuf, "rr", 2)==0)
		{
			roleRightsOidsCount++;
			roleRightsOids=(char**)realloc(roleRightsOids, sizeof(char *) * roleRightsOidsCount);
			asprintf(&roleRightsOids[roleRightsOidsCount-1], "%s", (bmdJSRequest->additionalMetaData[i]->OIDTableBuf));
			if (roleRightsOids[roleRightsOidsCount-1]==NULL)	{	BMD_FOK(NO_MEMORY);	}

			roleRightsCodesCount++;
			roleRightsCodes=(char**)realloc(roleRightsCodes, sizeof(char *) * roleRightsCodesCount);
			asprintf(&roleRightsCodes[roleRightsCodesCount-1], "%s", bmdJSRequest->additionalMetaData[i]->AnyBuf);
			if (roleRightsCodes[roleRightsCodesCount-1]==NULL)	{	BMD_FOK(NO_MEMORY);	}
		}
	}

	/**********************************************/
	/* sprawdzenie czy rola o podanym id istnieje */
	/**********************************************/
	BMD_FOK(CheckIfRoleExist(roleId, &tmp));
	free0(tmp);

	/*********************/
	/* zmiana nazwy roli */
	/*********************/
	if (roleName!=NULL)
	{
		BMD_FOK(CheckIfRoleExistByName(roleName));
		BMD_FOK(bmd_db_escape_string(hDB, roleName, STANDARD_ESCAPING, &escapeTemp));
		asprintf(&SQLQuery, 	"UPDATE roles SET name='%s' WHERE id=%s%s%s;",
					escapeTemp,
					roleId,
					_GLOBAL_bmd_configuration->location_id==NULL?"":" AND location_id=",
					_GLOBAL_bmd_configuration->location_id==NULL?"":_GLOBAL_bmd_configuration->location_id);
		free(escapeTemp); escapeTemp=NULL;
		BMD_FOK_CHG(bmd_db_execute_sql( hDB, SQLQuery,NULL,NULL, &query_result), LIBBMDSQL_ROLE_ACTION_ERROR);

		bmd_db_result_free(&query_result);
		free0(SQLQuery);
	}
	/***********************************/
	/* usuniecie wszystkich akcji roli */
	/***********************************/
	if (roleActionsCount>0)
	{
		asprintf(&SQLQuery, 	"DELETE FROM roles_and_actions WHERE fk_roles_id=%s%s%s;",
						roleId,
						_GLOBAL_bmd_configuration->location_id==NULL?"":" AND location_id=",
						_GLOBAL_bmd_configuration->location_id==NULL?"":_GLOBAL_bmd_configuration->location_id);

		BMD_FOK_CHG(bmd_db_execute_sql( hDB, SQLQuery,NULL,NULL, &query_result), LIBBMDSQL_ROLE_ACTION_ERROR);

		bmd_db_result_free(&query_result);
		free0(SQLQuery);

		/****************************************/
		/* wstawienie informacji o akcjach roli */
		/****************************************/
		for (i=0; i<roleActionsCount; i++)
		{
			BMD_FOK(bmd_db_escape_string(hDB, roleActions[i], STANDARD_ESCAPING, &escapeTemp));
			asprintf(&SQLQuery, 	"INSERT INTO roles_and_actions VALUES (nextval('roles_and_actions_id_seq'),%s,%s,%s);",
							_GLOBAL_bmd_configuration->location_id==NULL?"DEFAULT":_GLOBAL_bmd_configuration->location_id,
							roleId,
							escapeTemp);
			free(escapeTemp); escapeTemp=NULL;
			if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}

			BMD_FOK_CHG(bmd_db_execute_sql( hDB, SQLQuery,NULL,NULL, &query_result), LIBBMDSQL_ROLE_ACTION_ERROR);

			bmd_db_result_free(&query_result);
			free0(SQLQuery);
			free0(roleActions[i]);
		}
		free0(roleActions);
	}

	/**********************************************/
	/* wstawienie informacji role_rights dla roli */
	/**********************************************/
	for (i=0; i<roleRightsOidsCount; i++)
	{
		BMD_FOK(bmd_str_replace(&(roleRightsOids[i]), ".", "_"));
		BMD_FOK(bmd_db_escape_string(hDB, roleRightsOids[i], STANDARD_ESCAPING, &escapedRROid));
		BMD_FOK(bmd_db_escape_string(hDB, roleRightsCodes[i], STANDARD_ESCAPING, &escapedRRCode));
		asprintf(&SQLQuery, 	"UPDATE role_rights SET %s = '%s' WHERE fk_roles=%s%s%s;",
						escapedRROid,
						escapedRRCode,
						roleId,
						_GLOBAL_bmd_configuration->location_id==NULL?"":" AND location_id=",
						_GLOBAL_bmd_configuration->location_id==NULL?"":_GLOBAL_bmd_configuration->location_id);
		free(escapedRROid); escapedRROid=NULL;
		free(escapedRRCode); escapedRRCode=NULL;
		if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}

		BMD_FOK_CHG(bmd_db_execute_sql( hDB, SQLQuery,NULL,NULL, &query_result), LIBBMDSQL_ROLE_ROLE_RIGHTS_ERROR);

		bmd_db_result_free(&query_result);
		free0(SQLQuery);
		free0(roleRightsOids[i]);
		free0(roleRightsCodes[i]);
	}

	/************/
	/* porzadki */
	/************/
	free0(roleId);
	free0(roleRightsOids);
	free0(roleRightsCodes);
	free0(roleName);
	free0(SQLQuery);

	return BMD_OK;
}


/**********************************************************/
/* zmiana domyślnej (wybranej/aktualnej) roli użytkownika */
/**********************************************************/
long JS_bmdDatagramSet_to_deleteRole(	void *hDB,
					bmdDatagramSet_t *bmdJSRequestSet,
					bmdDatagramSet_t **bmdJSResponseSet,
					server_request_data_t *req)
{
long status		= 0;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (hDB==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (bmdJSRequestSet==NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	/*****************************************/
	/* przygotowanie datagramsetu odpowiedzi */
	/*****************************************/
	BMD_FOK(PR2_bmdDatagramSet_fill_with_datagrams(1, bmdJSResponseSet));

	/***********************/
	/* poczatek transakcji */
	/***********************/
	BMD_FOK(bmd_db_start_transaction(hDB,BMD_TRANSACTION_LEVEL_NONE));

	status=JS_bmdDatagram_to_deleteRole(hDB,
							bmdJSRequestSet->bmdDatagramSetTable[0],
							&((*bmdJSResponseSet)->bmdDatagramSetTable[0]),
							req);
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
	(*bmdJSResponseSet)->bmdDatagramSetTable[0]->datagramType = bmdJSRequestSet->bmdDatagramSetTable[0]->datagramType+100;
	(*bmdJSResponseSet)->bmdDatagramSetTable[0]->datagramStatus = status;

#ifndef WIN32
	kill(getppid(),SIGDICTRELOAD);
#endif //ifndef WIN32


	return BMD_OK;
}

long JS_bmdDatagram_to_deleteRole(	void *hDB,
					bmdDatagram_t *bmdJSRequest,
					bmdDatagram_t **bmdJSResponse,
					server_request_data_t *req)
{
char *roleId				= NULL;
char *roleName				= NULL;
char *SQLQuery				= NULL;
void *query_result			= NULL;
GenBuf_t *der_form			= NULL;

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if (hDB==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (bmdJSRequest==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	if (	(bmdJSRequest->protocolDataFileId!=NULL) &&
		(bmdJSRequest->protocolDataFileId->buf!=NULL) &&
		(bmdJSRequest->protocolDataFileId->size>0) )
	{
		asprintf(&roleId, "%s", bmdJSRequest->protocolDataFileId->buf);
		if (roleId==NULL)	{	BMD_FOK(NO_MEMORY);	}
	}

	if (roleId==NULL)	{	BMD_FOK(LIBBMDSQL_ROLE_ID_UNDEFINED);	}

	BMD_FOK(CheckIfRoleExist(	roleId, &roleName));

	/***********************************************************************************************/
	asprintf(&SQLQuery,	"DELETE FROM roles_and_actions WHERE fk_roles_id=%s%s%s;",
					roleId,
					_GLOBAL_bmd_configuration->location_id==NULL?"":" AND location_id=",
					_GLOBAL_bmd_configuration->location_id==NULL?"":_GLOBAL_bmd_configuration->location_id);
	if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}

	BMD_FOK_CHG(bmd_db_execute_sql( hDB, SQLQuery,NULL,NULL, &query_result),
			LIBBMDSQL_ROLE_CLEAR_ACTIONS_TABLE_ERR);
	bmd_db_result_free(&query_result);
	free0(SQLQuery);

	/***********************************************************************************************/
	asprintf(&SQLQuery,	"DELETE FROM role_rights WHERE fk_roles=%s%s%s;",
					roleId,
					_GLOBAL_bmd_configuration->location_id==NULL?"":" AND location_id=",
					_GLOBAL_bmd_configuration->location_id==NULL?"":_GLOBAL_bmd_configuration->location_id);
	if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}

	BMD_FOK_CHG(bmd_db_execute_sql( hDB, SQLQuery,NULL,NULL, &query_result),
			LIBBMDSQL_ROLE_CLEAR_RR_TABLE_ERR);
	bmd_db_result_free(&query_result);
	free0(SQLQuery);

	/***********************************************************************************************/
	asprintf(&SQLQuery,	"DELETE FROM roles WHERE id=%s%s%s;",
					roleId,
					_GLOBAL_bmd_configuration->location_id==NULL?"":" AND location_id=",
					_GLOBAL_bmd_configuration->location_id==NULL?"":_GLOBAL_bmd_configuration->location_id);
	if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}

	BMD_FOK_CHG(bmd_db_execute_sql( hDB, SQLQuery,NULL,NULL, &query_result),
			LIBBMDSQL_ROLE_CLEAR_ROLES_TABLE_ERR);
	bmd_db_result_free(&query_result);
	free0(SQLQuery);

	/************/
	/* porzadki */
	/************/
	free0(roleId);
	free0(roleName);
	free0(SQLQuery);
	free_gen_buf(&der_form);

	return BMD_OK;
}
