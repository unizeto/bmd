#include <bmd/libbmdsql/security.h>
#include <bmd/libbmdglobals/libbmdglobals.h>
#include <signal.h>

#ifdef WIN32
#pragma warning(disable:4204)
#pragma warning(disable:4100)
#pragma warning(disable:4127)
#endif

/****************************************/
/* sprawdzenie, czy uzytkownik istnieje */
/****************************************/
long CheckIfSecCategoryExist(	char *secCatName,
					char **secCatId)
{
	/************************/
	/* walidacja parametrow */
	/************************/
	if (secCatName==NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (secCatId==NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if ((*secCatId)!=NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	//SELECT id, type, name FROM sec_categories WHERE location_id=%s;
	BMD_FOK_CHG(getElementWithCondition(	_GLOBAL_SecurityCategoriesDictionary, 2, secCatName, 0, secCatId),
			LIBBMDSQL_DICT_SEC_CATEGORY_VALUE_NOT_FOUND);

	if ((*secCatId)==NULL)	{	return LIBBMDSQL_SEC_CATEGORY_NOT_REGISTERED_IN_SYSTEM;	}

	return BMD_OK;

}

long CheckIfSecLevelExist(	char *secLevelName,
					char **secLevelId)
{
	/************************/
	/* walidacja parametrow */
	/************************/
	if (secLevelName==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (secLevelId==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if ((*secLevelId)!=NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	//SELECT id, type, name, priority FROM sec_levels WHERE location_id=%s;
	BMD_FOK_CHG(getElementWithCondition(	_GLOBAL_SecurityLevelsDictionary, 2, secLevelName, 0, secLevelId),
			LIBBMDSQL_DICT_SEC_LEVEL_VALUE_NOT_FOUND);

	if (secLevelId==NULL)	{	return LIBBMDSQL_SEC_LEVEL_NOT_REGISTERED_IN_SYSTEM;	}

	return BMD_OK;

}

long CheckIfSecLevelPriorityExist(	char *secLevelPriority,
						char **secLevelId)
{
	/************************/
	/* walidacja parametrow */
	/************************/
	if (secLevelPriority==NULL)	{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (secLevelId==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if ((*secLevelId)!=NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	//SELECT id, type, name, priority FROM sec_levels WHERE location_id=%s;
	BMD_FOK_CHG(getElementWithCondition(	_GLOBAL_SecurityLevelsDictionary, 3, secLevelPriority, 0, secLevelId),
			LIBBMDSQL_DICT_SEC_LEVEL_VALUE_NOT_FOUND);

	if ((*secLevelId)==NULL)	{	return LIBBMDSQL_SEC_LEVEL_NOT_REGISTERED_IN_SYSTEM;	}

	return BMD_OK;

}

/***************************************************************************************/
/*          pobieranie informacji o poziomach i kategoriach bezpieczeństwa             */
/***************************************************************************************/
long JS_bmdDatagramSet_to_getSecurityList(	void *hDB,
						bmdDatagramSet_t *bmdJSRequestSet,
						bmdDatagramSet_t **bmdJSResponseSet,
						server_request_data_t *req)
{
long status			= 0;
long i			= 0;
char *SQLQuery		= NULL;
char *ans			= NULL;
long secCatsNumber	= 0;
long secLevelsNumber	= 0;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (hDB==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (bmdJSRequestSet==NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	/***************************************/
	/* odczytanie informacji o ilosci grup */
	/***************************************/
	//SELECT id, type, name FROM sec_categories WHERE location_id=%s;
	BMD_FOK(getCountWithCondition(	_GLOBAL_SecurityCategoriesDictionary, 0, NULL, &secCatsNumber));

	//SELECT id, type, name, priority FROM sec_levels WHERE location_id=%s;
	BMD_FOK(getCountWithCondition(	_GLOBAL_SecurityLevelsDictionary, 0, NULL, &secLevelsNumber));

	/*****************************************/
	/* przygotowanie datagramsetu odpowiedzi */
	/*****************************************/
	BMD_FOK(PR2_bmdDatagramSet_fill_with_datagrams(secCatsNumber+secLevelsNumber, bmdJSResponseSet));

	/*******************************************/
	/* pobieranie danych poszczególnych userów */
	/*******************************************/
	for (i=0; i<secCatsNumber+secLevelsNumber; i++)
	{

		status=JS_bmdDatagram_to_getSecurityList(hDB, i, secCatsNumber, secLevelsNumber, &((*bmdJSResponseSet)->bmdDatagramSetTable[i]));

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
	free0(SQLQuery);
	free0(ans);

	return BMD_OK;
}

long JS_bmdDatagram_to_getSecurityList(	void *hDB,
					long number,
					long secCatsNumber,
					long secLevelsNumber,
					bmdDatagram_t **bmdJSResponse)
{
long ansCount		= 0;
long i			= 0;
GenBuf_t *tmp		= NULL;
char **anss		= NULL;
char *ans		= NULL;
char *SQLQuery		= NULL;
char *secCatId		= NULL;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (hDB==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
 	if (number<0)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	/**************************************************/
	/* dodajemy informacje o poziomach bezpieczeństwa */
	/**************************************************/
	if (number<secCatsNumber)
	{
		/**********************************************************/
		/* pobranie glownych informacji o poziomie bezpieczeństwa */
		/**********************************************************/
		asprintf(&SQLQuery, 	"SELECT id , name FROM sec_categories %s%s LIMIT 1 OFFSET %li ;",
						_GLOBAL_bmd_configuration->location_id==NULL?"":"WHERE location_id=",
						_GLOBAL_bmd_configuration->location_id==NULL?"":_GLOBAL_bmd_configuration->location_id,
						number);
		if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}

		BMD_FOK(ExecuteSQLQueryWithRowAnswersKnownDBConnection( hDB, SQLQuery, &ansCount, &anss));

		asprintf(&secCatId, "%s", anss[0]);
		if (secCatId==NULL)	{	BMD_FOK(NO_MEMORY);	}

		BMD_FOK(set_gen_buf2(anss[0], (long)strlen(anss[0]), &((*bmdJSResponse)->protocolDataFileId)));
		BMD_FOK(set_gen_buf2(anss[1], (long)strlen(anss[1]), &((*bmdJSResponse)->protocolDataFilename)));
		if (_GLOBAL_bmd_configuration->location_id)
		{
			BMD_FOK(set_gen_buf2(_GLOBAL_bmd_configuration->location_id, (long)strlen(_GLOBAL_bmd_configuration->location_id), &((*bmdJSResponse)->protocolDataFileLocationId)));
		}

		BMD_FOK(set_gen_buf2(anss[0], (long)strlen(anss[0]), &tmp));
		BMD_FOK(bmd_datagram_add_metadata_2_with_params(OID_SYS_METADATA_CERT_SEC_CATEGORY_ID, tmp, bmdJSResponse, ADDITIONAL_METADATA, 0, 0, 0, 1));
		free_gen_buf(&tmp);

		BMD_FOK(set_gen_buf2(anss[1], (long)strlen(anss[1]), &tmp));
		BMD_FOK(bmd_datagram_add_metadata_2_with_params(OID_SYS_METADATA_CERT_SEC_CATEGORY_NAME, tmp, bmdJSResponse, ADDITIONAL_METADATA, 0, 0, 0, 1));
		free_gen_buf(&tmp);

		free0(anss[0]);
		free0(anss[1]);
		free0(anss);
		free0(SQLQuery);

		/**********************************************/
		/* pobranie informacji o kategorii nadrzędnej */
		/**********************************************/
		//SELECT src ,dest FROM categories_graph WHERE location_id=%s
		getColumnWithCondition(	_GLOBAL_SecurityCategoriesGraphDictionary, 1, secCatId, 0, &ansCount, &anss);

		for (i=0; i<ansCount; i++)
		{
			//SSELECT id, type, name FROM sec_categories WHERE location_id=%s
			BMD_FOK_CHG(getElementWithCondition(	_GLOBAL_SecurityCategoriesDictionary, 0, anss[i], 2, &ans), LIBBMDSQL_DICT_SEC_CATEGORY_VALUE_NOT_FOUND);

			BMD_FOK(set_gen_buf2(ans, (long)strlen(ans), &tmp));
			BMD_FOK(bmd_datagram_add_metadata_2_with_params(OID_SYS_METADATA_CERT_SEC_CATEGORY_PARENT_NAME, tmp, bmdJSResponse, ADDITIONAL_METADATA, 0, 0, 0, 1));

			free0(ans);
			free0(anss[i]);
			free_gen_buf(&tmp);
		}
		free0(anss);

		/**********************************************/
		/* pobranie informacji o kategorii podrzędnej */
		/**********************************************/
		//SELECT src ,dest FROM categories_graph WHERE location_id=%s
		getColumnWithCondition(	_GLOBAL_SecurityCategoriesGraphDictionary, 0, secCatId, 1, &ansCount, &anss);

		for (i=0; i<ansCount; i++)
		{
			//SELECT id, type, name FROM sec_categories WHERE location_id=%s
			BMD_FOK_CHG(getElementWithCondition(	_GLOBAL_SecurityCategoriesDictionary, 0, anss[i], 2, &ans), LIBBMDSQL_DICT_SEC_CATEGORY_VALUE_NOT_FOUND);

			BMD_FOK(set_gen_buf2(ans, (long)strlen(ans), &tmp));
			BMD_FOK(bmd_datagram_add_metadata_2_with_params(OID_SYS_METADATA_CERT_SEC_CATEGORY_CHILD_NAME, tmp, bmdJSResponse, ADDITIONAL_METADATA, 0, 0, 0, 1));

			free_gen_buf(&tmp);
			free0(anss[i]);
			free0(ans);
		}
		free0(anss);

	}
	else
	{
		/**********************************************************/
		/* pobranie glownych informacji o poziomie bezpieczeństwa */
		/**********************************************************/
		asprintf(&SQLQuery, 	"SELECT id , name, priority FROM sec_levels %s%s LIMIT 1 OFFSET %li ;",
						_GLOBAL_bmd_configuration->location_id==NULL?"":"WHERE location_id=",
						_GLOBAL_bmd_configuration->location_id==NULL?"":_GLOBAL_bmd_configuration->location_id,
						number-secCatsNumber);
		if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}

		BMD_FOK(ExecuteSQLQueryWithRowAnswersKnownDBConnection( hDB, SQLQuery, &ansCount, &anss));

		BMD_FOK(set_gen_buf2(anss[0], (long)strlen(anss[0]), &((*bmdJSResponse)->protocolDataFileId)));
		BMD_FOK(set_gen_buf2(anss[1], (long)strlen(anss[1]), &((*bmdJSResponse)->protocolDataFilename)));
		if (_GLOBAL_bmd_configuration->location_id)
		{
			BMD_FOK(set_gen_buf2(_GLOBAL_bmd_configuration->location_id, (long)strlen(_GLOBAL_bmd_configuration->location_id), &((*bmdJSResponse)->protocolDataFileLocationId)));
		}

		BMD_FOK(set_gen_buf2(anss[0], (long)strlen(anss[0]), &tmp));
		BMD_FOK(bmd_datagram_add_metadata_2_with_params(OID_SYS_METADATA_CERT_SEC_LEVEL_ID, tmp, bmdJSResponse, ADDITIONAL_METADATA, 0, 0, 0, 1));
		free_gen_buf(&tmp);

		BMD_FOK(set_gen_buf2(anss[1], (long)strlen(anss[1]), &tmp));
		BMD_FOK(bmd_datagram_add_metadata_2_with_params(OID_SYS_METADATA_CERT_SEC_LEVEL_NAME, tmp, bmdJSResponse, ADDITIONAL_METADATA, 0, 0, 0, 1));
		free_gen_buf(&tmp);

		BMD_FOK(set_gen_buf2(anss[2], (long)strlen(anss[2]), &tmp));
		BMD_FOK(bmd_datagram_add_metadata_2_with_params(OID_SYS_METADATA_CERT_SEC_LEVEL_PRIORITY, tmp, bmdJSResponse, ADDITIONAL_METADATA, 0, 0, 0, 1));
		free_gen_buf(&tmp);

		free0(anss[0]);
		free0(anss[1]);
		free0(anss[2]);
		free0(anss);
		free0(SQLQuery);
	}

	/************/
	/* porzadki */
	/************/
	free0(anss);
	free0(SQLQuery);
	free0(secCatId);

	return BMD_OK;
}









/*************************************************************/
/*          rejestracja atrybutow bezpieczenstwa             */
/*************************************************************/
long JS_bmdDatagramSet_to_registerSecurity(	void *hDB,
								bmdDatagramSet_t *bmdJSRequestSet,
								bmdDatagramSet_t **bmdJSResponseSet)
{
long status				= 0;
long i				= 0;

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
		status=JS_bmdDatagram_to_registerSecurity(	hDB,
											bmdJSRequestSet->bmdDatagramSetTable[i],
											&((*bmdJSResponseSet)->bmdDatagramSetTable[i]));
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

long JS_bmdDatagram_to_registerSecurity(	void *hDB,
						bmdDatagram_t *bmdJSRequest,
						bmdDatagram_t **bmdJSResponse)
{
long i					= 0;
long status				= 0;
long securityCatParentsCount		= 0;
long securityCatChildrenCount		= 0;
long rows				= 0;
long cols				= 0;
void *query_result			= NULL;
char *SQLQuery				= NULL;
char *securityCatName			= NULL;
char *securityCatId			= NULL;
char *securityCatParentId		= NULL;
char *securityCatChildId		= NULL;
char **securityCatParents		= NULL;
char **securityCatChildren		= NULL;
char *securityLevelName			= NULL;
char *securityLevelId			= NULL;
char *securityLevelPriority		= NULL;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (hDB==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (bmdJSRequest==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}

	/******************************************************/
	/* poszukiwanie poszczególnych elementów w datagramie */
	/******************************************************/
	for (i=0; i<bmdJSRequest->no_of_additionalMetaData; i++)
	{

		/********************************************/
		/* odczytanie nazwy rejestrowanego elementu */
		/********************************************/
		if (strcmp(bmdJSRequest->additionalMetaData[i]->OIDTableBuf, OID_SYS_METADATA_CERT_SEC_CATEGORY_NAME)==0)
		{
			if (securityCatName!=NULL)	{	BMD_FOK(LIBBMDSQL_SEC_CAT_NAME_TOO_MANY);	}
			asprintf(&securityCatName, "%s", bmdJSRequest->additionalMetaData[i]->AnyBuf);
			if (securityCatName==NULL)	{	BMD_FOK(NO_MEMORY);	}
		}
		/**********************************/
		/* odczytanie nazwy grupy rodzica */
		/**********************************/
		else if (strcmp(bmdJSRequest->additionalMetaData[i]->OIDTableBuf, OID_SYS_METADATA_CERT_SEC_CATEGORY_PARENT_NAME)==0)
		{
			securityCatParentsCount++;
			securityCatParents=(char**)realloc(securityCatParents, sizeof(char *) * securityCatParentsCount);
			asprintf(&securityCatParents[securityCatParentsCount-1], "%s", bmdJSRequest->additionalMetaData[i]->AnyBuf);
			if (securityCatParents[securityCatParentsCount-1]==NULL)	{	BMD_FOK(NO_MEMORY);	}
		}
		/***********************************/
		/* odczytanie nazwy grupy potomnej */
		/***********************************/
		else if (strcmp(bmdJSRequest->additionalMetaData[i]->OIDTableBuf, OID_SYS_METADATA_CERT_SEC_CATEGORY_CHILD_NAME)==0)
		{
			securityCatChildrenCount++;
			securityCatChildren=(char**)realloc(securityCatChildren, sizeof(char *) * securityCatChildrenCount);
			asprintf(&securityCatChildren[securityCatChildrenCount-1], "%s", bmdJSRequest->additionalMetaData[i]->AnyBuf);
			if (securityCatChildren[securityCatChildrenCount-1]==NULL)	{	BMD_FOK(NO_MEMORY);	}
		}

		/********************************************/
		/* odczytanie nazwy rejestrowanego elementu */
		/********************************************/
		else if (strcmp(bmdJSRequest->additionalMetaData[i]->OIDTableBuf, OID_SYS_METADATA_CERT_SEC_LEVEL_NAME)==0)
		{
			if (securityLevelName!=NULL)	{	BMD_FOK(LIBBMDSQL_SEC_LEVEL_NAME_TOO_MANY);	}
			asprintf(&securityLevelName, "%s", bmdJSRequest->additionalMetaData[i]->AnyBuf);
			if (securityLevelName==NULL)	{	BMD_FOK(NO_MEMORY);	}
		}
		else if (strcmp(bmdJSRequest->additionalMetaData[i]->OIDTableBuf, OID_SYS_METADATA_CERT_SEC_LEVEL_PRIORITY)==0)
		{
			if (securityLevelPriority!=NULL)	{	BMD_FOK(LIBBMDSQL_SEC_LEVEL_PRIORITY_TOO_MANY);	}
			asprintf(&securityLevelPriority, "%s", bmdJSRequest->additionalMetaData[i]->AnyBuf);
			if (securityLevelPriority==NULL)	{	BMD_FOK(NO_MEMORY);	}
		}
	}

	if ((securityCatName==NULL) && (securityLevelName==NULL)) 	{	BMD_FOK(LIBBMDSQL_SEC_NAME_NOT_DEFINED);	}

	if (securityCatName)
	{
		/**********************************************************/
		/* sprawdzenie, czy kategoria nie jest juz zarejestrowana */
		/**********************************************************/
		status=CheckIfSecCategoryExist(securityCatName, &securityCatId);
		if (status>=BMD_OK)
		{
			free0(securityCatId);
			BMD_FOK(LIBBMDSQL_SEC_CAT_ALREADY_REGISTERED);
		}
		free0(securityCatId);

		/**************************************************/
		/* wstawienie informacji do tabeli sec_categories */
		/**************************************************/
		asprintf(&SQLQuery, 	"INSERT INTO sec_categories VALUES (nextval('sec_categories_id_seq'),%s,'---', '%s');",
						_GLOBAL_bmd_configuration->location_id==NULL?"DEFAULT":_GLOBAL_bmd_configuration->location_id,
						securityCatName);
		if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}

		BMD_FOK(bmd_db_execute_sql( hDB, SQLQuery,NULL,NULL, &query_result));

		rows=0; cols=0;
		bmd_db_result_free(&query_result);
		free(SQLQuery); SQLQuery=NULL;

		/*******************************/
		/* pobranie id nowej kategorii */
		/*******************************/
		asprintf(&SQLQuery, 	"SELECT last_value FROM sec_categories_id_seq;");
		if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}

		BMD_FOK(ExecuteSQLQueryWithAnswerKnownDBConnection(	hDB, SQLQuery, &securityCatId));
		free(SQLQuery); SQLQuery=NULL;
		BMD_FOK(set_gen_buf2(securityCatId, (long)strlen(securityCatId), &((*bmdJSResponse)->protocolDataFileId)));
		free0(securityCatId);

		/***********************************************/
		/* wstawienie informacji o grupach nadrzędnych */
		/***********************************************/
		for (i=0; i<securityCatParentsCount; i++)
		{
			BMD_FOK_CHG(CheckIfSecCategoryExist(securityCatParents[i], &securityCatParentId),
					LIBBMDSQL_SEC_CAT_PARENT_CAT_NOT_REGISTERED);

			asprintf(&SQLQuery, 	"INSERT INTO categories_graph VALUES (nextval('categories_graph_id_seq'),%s,%s,currval('sec_categories_id_seq'));",
							_GLOBAL_bmd_configuration->location_id==NULL?"DEFAULT":_GLOBAL_bmd_configuration->location_id,
							securityCatParents[i]);
			if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}

			BMD_FOK_CHG(bmd_db_execute_sql( hDB, SQLQuery,NULL,NULL, &query_result),
					LIBBMDSQL_SEC_CAT_PARENT_SEC_CAT_ERR);
			bmd_db_result_free(&query_result);

			rows=0; cols=0;
			bmd_db_result_free(&query_result);
			free0(SQLQuery);
			free0(securityCatParents[i]);
			free0(securityCatParentId);
		}
		free0(securityCatParents);

		/***********************************************/
		/* wstawienie informacji o grupach podrzędnych */
		/***********************************************/
		for (i=0; i<securityCatChildrenCount; i++)
		{
			BMD_FOK_CHG(CheckIfSecCategoryExist(securityCatChildren[i], &securityCatChildId),
					LIBBMDSQL_SEC_CAT_CHILD_CAT_NOT_REGISTERED);

			asprintf(&SQLQuery, 	"INSERT INTO securitys_graph VALUES (nextval('categories_graph_id_seq'),%s,currval('sec_categories_id_seq'),%s);",
							_GLOBAL_bmd_configuration->location_id==NULL?"DEFAULT":_GLOBAL_bmd_configuration->location_id,
							securityCatChildId);
			if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}

			BMD_FOK_CHG(bmd_db_execute_sql( hDB, SQLQuery,NULL,NULL, &query_result),
					LIBBMDSQL_SEC_CAT_CHILD_SEC_CAT_ERR);
			bmd_db_result_free(&query_result);

			rows=0; cols=0;
			bmd_db_result_free(&query_result);
			free0(SQLQuery);
			free0(securityCatChildren[i]);
			free0(securityCatChildId);
		}
		free0(securityCatChildren);
	}

	if (securityLevelName)
	{
		status=CheckIfSecCategoryExist(securityLevelName, &securityLevelId);
		if (status>=BMD_OK)
		{
			free0(securityLevelId);
			BMD_FOK(LIBBMDSQL_SEC_LEVEL_ALREADY_REGISTERED);
		}
		free0(securityLevelId);

		status=CheckIfSecLevelPriorityExist( securityLevelPriority, &securityLevelId);
		if (status>=BMD_OK)
		{
			free0(securityLevelId);
			BMD_FOK(LIBBMDSQL_SEC_LEVEL_PRIORITY_ALREADY_REGISTERED);

		}
		free0(securityLevelId);

		/**************************************************/
		/* wstawienie informacji do tabeli sec_categories */
		/**************************************************/
		asprintf(&SQLQuery, 	"INSERT INTO sec_levels VALUES (nextval('sec_levels_id_seq'),%s,'%s',%s);",
						_GLOBAL_bmd_configuration->location_id==NULL?"DEFAULT":_GLOBAL_bmd_configuration->location_id,
						securityLevelName,
						securityLevelPriority);
		if (SQLQuery==NULL)	{	BMD_FOK(NO_MEMORY);	}

		BMD_FOK(bmd_db_execute_sql( hDB, SQLQuery,NULL,NULL, &query_result));

		rows=0; cols=0;
		bmd_db_result_free(&query_result);
		free0(SQLQuery);
	}

	/************/
	/* porzadki */
	/************/
	free0(SQLQuery);
	free0(securityCatName);
	free0(securityCatId);
	free0(securityCatParentId);
	free0(securityCatChildId);
	free0(securityCatParents);
	free0(securityCatChildren);
	free0(securityLevelName);
	free0(securityLevelId);
	free0(securityLevelPriority);

	return BMD_OK;
}
