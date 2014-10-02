#include <bmd/libbmdaa2/db/database/AC_db_database.h>
#include <bmd/common/global_types.h>
#include <bmd/libbmddb/libbmddb.h>
#include <bmd/libbmddb/libbmddb.h>
#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdsql/common.h>
#include <bmd/libbmdglobals/libbmdglobals.h>


/**********************************************************/
/* funkcje sluzace do odczytywania atrybutow uzytkownikow */
/* do struktury, a nie certyfikatu atrybutow              */
/**********************************************************/
long ST_ReadAttributesFromDatabase(	const char *CertificateSerialNr,
					const char *CertificateIssuerDN,
					const ConfigData_t *ConfigData,
					certificate_attributes_t **UserAttributes)
{
	/************************/
	/* walidacja parametrow */
	/************************/
	if (CertificateSerialNr==NULL)	{	BMD_FOK(BMD_ERR_PARAM1); }
	if (CertificateIssuerDN==NULL)	{	BMD_FOK(BMD_ERR_PARAM2); }
	if (ConfigData==NULL)		{	BMD_FOK(BMD_ERR_PARAM3); }
	if (UserAttributes==NULL)	{	BMD_FOK(BMD_ERR_PARAM4); }

	/********************************************/
	/* odnalezienie uzytkownika w bazie dadnych */
	/********************************************/
	BMD_FOK(ST_FindUserInDatabase(CertificateSerialNr, CertificateIssuerDN, UserAttributes));
	BMD_FOK(ST_FindUsersAttrInDatabase(CertificateSerialNr, CertificateIssuerDN, "default_group", UserAttributes));
	BMD_FOK(ST_FindUsersAttrInDatabase(CertificateSerialNr, CertificateIssuerDN, "default_role", UserAttributes));
	BMD_FOK(ST_FindUsersAttrInDatabase(CertificateSerialNr, CertificateIssuerDN, "groups", UserAttributes));
	BMD_FOK(ST_FindUsersAttrInDatabase(CertificateSerialNr, CertificateIssuerDN, "roles", UserAttributes));
	BMD_FOK(ST_FindUsersAttrInDatabase(CertificateSerialNr, CertificateIssuerDN, "sec_categories", UserAttributes));
	BMD_FOK(ST_FindUsersAttrInDatabase(CertificateSerialNr, CertificateIssuerDN, "sec_levels", UserAttributes));

	if ((*UserAttributes)->user_chosen_role==NULL)		{	BMD_FOK(-666);	}
	if ((*UserAttributes)->user_chosen_group==NULL)	{	BMD_FOK(-666);	}

	return BMD_OK;
}

long ST_FindUserInDatabase(	const char *CertificateSerialNr,
				const char *CertificateIssuerDN,
				certificate_attributes_t **UserAttributes)
{
char *SQLQuery		= NULL;
char **ans		= NULL;
char *conditionValue	= NULL;
long ans_count		= 0;
long i			= 0;
long tabOfLongs[] = {0,1,2,3,4,6}; //przekazanie wartosci w klamrach bezposrednio do funkcji nie skompiluje sie pod Windows (by WSZ)

	/************************/
	/* walidacja parametrow */
	/************************/
	if (CertificateSerialNr==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (CertificateIssuerDN==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (UserAttributes==NULL)		{	BMD_FOK(BMD_ERR_PARAM3);	}

	/***************************************************/
	/* pobranie informacji o uzytkownikach ze slownika */
	/***************************************************/
	//SELECT id, name, identity, cert_serial, cert_dn, cert_dn||cert_serial, accepted FROM users;
	asprintf(&conditionValue, "%s%s", CertificateIssuerDN, CertificateSerialNr);
	if (conditionValue==NULL)	{	BMD_FOK(NO_MEMORY);	}

	BMD_FOK_CHG(getRowWithCondition(	_GLOBAL_UsersDictionary, 5, conditionValue, tabOfLongs, 6, &ans_count, &ans), LIBBMDAA2_ERR_USER_LOGIN_NOT_REGISTERED);

	if ((*UserAttributes)==NULL)
	{
		(*UserAttributes)=(certificate_attributes_t *)malloc(sizeof(certificate_attributes_t));
		(*UserAttributes)->user_chosen_role	= NULL;
		(*UserAttributes)->user_chosen_group	= NULL;
		(*UserAttributes)->id			= NULL;
		(*UserAttributes)->name			= NULL;
		(*UserAttributes)->identityId		= NULL;
		(*UserAttributes)->serial		= NULL;
		(*UserAttributes)->cert_dn		= NULL;
		(*UserAttributes)->accepted		= NULL;
	}
	else
	{
		free_gen_buf(&((*UserAttributes)->id));
		free_gen_buf(&((*UserAttributes)->name));
		free_gen_buf(&((*UserAttributes)->identityId));
		free_gen_buf(&((*UserAttributes)->serial));
		free_gen_buf(&((*UserAttributes)->cert_dn));
		free_gen_buf(&((*UserAttributes)->accepted));
	}

	BMD_FOK(set_gen_buf2(ans[0], (long)strlen(ans[0]), &((*UserAttributes)->id)));
	BMD_FOK(set_gen_buf2(ans[1], (long)strlen(ans[1]), &((*UserAttributes)->name)));
	BMD_FOK(set_gen_buf2(ans[2], (long)strlen(ans[2]), &((*UserAttributes)->identityId)));
	BMD_FOK(set_gen_buf2(ans[3], (long)strlen(ans[3]), &((*UserAttributes)->serial)));
	BMD_FOK(set_gen_buf2(ans[4], (long)strlen(ans[4]), &((*UserAttributes)->cert_dn)));
	BMD_FOK(set_gen_buf2(ans[5], (long)strlen(ans[5]), &((*UserAttributes)->accepted)));

	/************/
	/* porzadki */
	/************/
	for (i=0; i<ans_count; i++)
	{
		free0(ans[i]);
	}

	free0(ans);
	free0(SQLQuery);
	free0(conditionValue);


	return BMD_OK;
}

long ST_FindUsersAttrInDatabase(	const char *CertificateSerialNr,
					const char *CertificateIssuerDN,
					const char *attr_kind,
					certificate_attributes_t **UserAttributes)
{
char **ans		= NULL;
char *tmp		= NULL;
long ans_count		= 0;
long i			= 0;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (CertificateSerialNr==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (CertificateIssuerDN==NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (attr_kind==NULL)			{	BMD_FOK(BMD_ERR_PARAM3);	}
	if (UserAttributes==NULL)		{	BMD_FOK(BMD_ERR_PARAM4);	}
	if ((*UserAttributes)==NULL)		{	BMD_FOK(BMD_ERR_PARAM4);	}

	if (strcmp(attr_kind, "groups")==0)
	{

		//SELECT users_and_groups.fk_users, users_and_groups.default_group,groups.id, groups.name, users_and_groups.fk_users||groups.id FROM groups LEFT JOIN users_and_groups ON (users_and_groups.fk_groups=groups.id) WHERE groups.location_id=%s;
		BMD_FOK(getColumnWithCondition(	_GLOBAL_UserGroupsDictionary, 0, (*UserAttributes)->id->buf, 3, &ans_count, &ans));

	}
	else if (strcmp(attr_kind, "default_group")==0)
	{
		//SELECT users_and_groups.fk_users, users_and_groups.default_group,groups.id, groups.name, users_and_groups.fk_users||groups.id FROM groups LEFT JOIN users_and_groups ON (users_and_groups.fk_groups=groups.id) WHERE groups.location_id=%s;
		BMD_FOK(getColumnWithCondition(	_GLOBAL_UserGroupsDictionary, 1, "1", 4, &ans_count, &ans));
	}
	else if (strcmp(attr_kind, "roles")==0)
	{
		//SELECT users_and_roles.fk_users, users_and_roles.default_role, roles.id, roles.name, users_and_roles.fk_users::varchar||'|'||roles.name::varchar FROM roles LEFT JOIN users_and_roles ON (users_and_roles.fk_roles=roles.id);
		BMD_FOK(getColumnWithCondition(	_GLOBAL_UserRolesDictionary, 0, (*UserAttributes)->id->buf, 3, &ans_count, &ans));
	}
	else if (strcmp(attr_kind, "default_role")==0)
	{

		//SELECT users_and_roles.fk_users, users_and_roles.default_role, roles.id, roles.name, users_and_roles.fk_users::varchar||'|'||roles.name::varchar FROM roles LEFT JOIN users_and_roles ON (users_and_roles.fk_roles=roles.id);
		BMD_FOK(getColumnWithCondition(	_GLOBAL_UserRolesDictionary, 1, "1", 4, &ans_count, &ans));
	}
	else if (strcmp(attr_kind, "sec_categories")==0)
	{

		//SELECT users_and_security.fk_users, sec_levels.id, sec_levels.name, sec_categories.id, sec_categories.name FROM sec_categories LEFT JOIN users_and_security ON (users_and_security.fk_sec_categories=sec_categories.id) LEFT JOIN sec_levels ON (users_and_security.fk_sec_levels=sec_levels.id);
		BMD_FOK(getColumnWithCondition(	_GLOBAL_SecurityDictionary, 0, (*UserAttributes)->id->buf, 3, &ans_count, &ans));

	}
	else if (strcmp(attr_kind, "sec_levels")==0)
	{

		//SELECT users_and_security.fk_users, sec_levels.id, sec_levels.name, sec_categories.id, sec_categories.name FROM sec_categories LEFT JOIN users_and_security ON (users_and_security.fk_sec_categories=sec_categories.id) LEFT JOIN sec_levels ON (users_and_security.fk_sec_levels=sec_levels.id);
		BMD_FOK(getColumnWithCondition(	_GLOBAL_SecurityDictionary, 0, (*UserAttributes)->id->buf, 1, &ans_count, &ans));
	}

	if (strcmp(attr_kind, "groups")==0)
	{

		if (ans_count==0)
		{
			BMD_FOK(LIBBMDAA2_ERR_USER_GROUP_NOT_SPECIFIED);
		}

		(*UserAttributes)->groups=(GenBufList_t *)malloc(sizeof(GenBufList_t));
		(*UserAttributes)->groups->size=ans_count;
		(*UserAttributes)->groups->gbufs=(GenBuf_t **)malloc(sizeof(GenBuf_t *) * ((*UserAttributes)->groups->size));
		memset((*UserAttributes)->groups->gbufs, 0, sizeof(GenBuf_t *) * ((*UserAttributes)->groups->size));

		for (i=0; i<ans_count; i++)
		{
			BMD_FOK(set_gen_buf2 (	ans[i], (long)strlen(ans[i]), &((*UserAttributes)->groups->gbufs[i])));
		}
	}
	else if ((strcmp(attr_kind, "default_group")==0) && ((*UserAttributes)->user_chosen_group==NULL))
	{

		if (ans_count==0)
		{
			BMD_FOK(LIBBMDAA2_ERR_USER_DEFAULT_GROUP_NOT_SPECIFIED);
		}

		for (i=0; i<ans_count; i++)
		{
			asprintf(&tmp, "%s|", (*UserAttributes)->id->buf);
			if (tmp==NULL)	{	BMD_FOK(NO_MEMORY);	}
			if (strncmp(ans[i], tmp, strlen(tmp))==0)
			{
				BMD_FOK(set_gen_buf2 (	ans[i]+(*UserAttributes)->id->size+1, (long)strlen(ans[i]+(*UserAttributes)->id->size), &((*UserAttributes)->user_chosen_group)));
				break;
			}
			free0(tmp);
		}

	}
	else if (strcmp(attr_kind, "roles")==0)
	{

		if (ans_count==0)
		{
			BMD_FOK(LIBBMDAA2_ERR_USER_ROLE_NOT_SPECIFIED);
		}

		(*UserAttributes)->roles=(GenBufList_t *)malloc(sizeof(GenBufList_t));
		(*UserAttributes)->roles->size=ans_count;
		(*UserAttributes)->roles->gbufs=(GenBuf_t **)malloc(sizeof(GenBuf_t *) * ((*UserAttributes)->roles->size));
		memset((*UserAttributes)->roles->gbufs, 0, sizeof(GenBuf_t *) * ((*UserAttributes)->roles->size));

		for (i=0; i<ans_count; i++)
		{
			BMD_FOK(set_gen_buf2 (	ans[i], (long)strlen(ans[i]), &((*UserAttributes)->roles->gbufs[i])));
		}

	}
	else if ((strcmp(attr_kind, "default_role")==0) && ((*UserAttributes)->user_chosen_role==NULL))
	{
		if (ans_count==0)
		{
			BMD_FOK(LIBBMDAA2_ERR_USER_DEFAULT_ROLE_NOT_SPECIFIED);
		}

		for (i=0; i<ans_count; i++)
		{
			asprintf(&tmp, "%s|", (*UserAttributes)->id->buf);
			if (tmp==NULL)	{	BMD_FOK(NO_MEMORY);	}
			if (strncmp(ans[i], tmp, strlen(tmp))==0)
			{
				BMD_FOK(set_gen_buf2 (	ans[i]+(*UserAttributes)->id->size+1, (long)strlen(ans[i]+(*UserAttributes)->id->size), &((*UserAttributes)->user_chosen_role)));
				break;
			}
			free0(tmp);
		}
	}
	else if (strcmp(attr_kind, "sec_categories")==0)
	{

		if (ans_count==0)
		{
			BMD_FOK(LIBBMDAA2_ERR_USER_ROLE_NOT_SPECIFIED);
		}

		(*UserAttributes)->security=(GenBufList_t *)malloc(sizeof(GenBufList_t));
		(*UserAttributes)->security->size=ans_count*2;

		(*UserAttributes)->security->gbufs=(GenBuf_t **)malloc(sizeof(GenBuf_t *) * ((*UserAttributes)->security->size));
		memset((*UserAttributes)->security->gbufs, 0, sizeof(GenBuf_t *) * ((*UserAttributes)->security->size));

		for (i=0; i<ans_count/*(*UserAttributes)->security->size/2*/; i++)
		{
			BMD_FOK(set_gen_buf2 (	ans[i], (long)strlen(ans[i]), &((*UserAttributes)->security->gbufs[i*2])));
		}

	}
	else if (strcmp(attr_kind, "sec_levels")==0)
	{

		if (ans_count==0)
		{
			BMD_FOK(LIBBMDAA2_ERR_USER_ROLE_NOT_SPECIFIED);
		}

		for (i=0; i<ans_count/*(*UserAttributes)->security->size/2*/; i++)
		{
			BMD_FOK(set_gen_buf2 (	ans[i], (long)strlen(ans[i]), &((*UserAttributes)->security->gbufs[i*2+1])));
		}


	}

	/************/
	/* porzadki */
	/************/
	for (i=0; i<ans_count; i++)
	{
		free0(ans[i]);
	}
	free0(ans);

	return BMD_OK;
}




/************************************************/
/* Odczyt atrybutów z bazy danych		*/
/************************************************/

/* Funkcja odpytuje baze danych w poszukiwaniu atrybutów,
   dla wskazanego certyfikatu klucza publicznego.
   Certyfikat ten wskazuje jego numer seryjny i nazwa wystawcy.
   W efekcie generowana jest struktura AttributesSequence_t,
   którą można bezpośrednio wstawic do certyfikatu atrybutów
   lub zserializować i wysłać do AAauthority by zrobiło z tego
   certyfikat atrybutów */
long AA_ReadAttributesFromDatabase(	const char *CertificateSerialNr,
						const char *CertificateIssuerDN,
						const ConfigData_t *ConfigData,
						AttributesSequence_t **AttributesSequence)
{
void *hDB	= NULL;
char *UserId			= NULL;

	PRINT_VDEBUG("LIBBMDAA2INF Reading attributes from database\n");
	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if (CertificateSerialNr == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (CertificateIssuerDN == NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (ConfigData == NULL)					{	BMD_FOK(BMD_ERR_PARAM3);	}
	if (AttributesSequence == NULL)			{	BMD_FOK(BMD_ERR_PARAM4);	}
	if (*AttributesSequence != NULL)			{	BMD_FOK(BMD_ERR_PARAM4);	}

	/******************************/
	/*	polacz z baza danych	*/
	/******************************/
	BMD_FOK(bmd_db_connect(ConfigData->AAServerConfiguration->DataSourceSection->database_connection_string,&hDB));
	BMD_FOK_DB(bmd_db_start_transaction(hDB,BMD_TRANSACTION_LEVEL_NONE));

	/************************************************************/
	/*	znajdz uzytkownia, ktory chce sie zalogowac w bazie	*/
	/************************************************************/
	BMD_FOK_DB(AA_FindUserInDatabase(hDB,CertificateSerialNr, CertificateIssuerDN,&UserId));

	/******************************************************/
	/*	odczytaj atrybuty dla znalezionego uzytkownika	*/
	/******************************************************/
	BMD_FOK_DB(AA_ReadAttributesOwnedByUser(hDB, UserId, AttributesSequence, ConfigData));
	free(UserId); UserId=NULL;

	/******************************/
	/*	odłącz od bazy danych	*/
	/******************************/
	BMD_FOK_DB(bmd_db_end_transaction(hDB));
	BMD_FOK(bmd_db_disconnect(&hDB));

	return BMD_OK;
}

long AA_FindUserInDatabase(	void *DatabaseConnectionHandle,
					const char *CertificateSerialNr,
					const char *CertificateIssuerDN,
					char **UserId)
{
	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if(DatabaseConnectionHandle == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(CertificateSerialNr == NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(CertificateIssuerDN == NULL)			{	BMD_FOK(BMD_ERR_PARAM3);	}
	if(UserId == NULL || *UserId != NULL)		{	BMD_FOK(BMD_ERR_PARAM4);	}

	BMD_FOK(AA_SelectUserId(DatabaseConnectionHandle,CertificateSerialNr, CertificateIssuerDN, UserId));
// 	BMD_FOK(AA_QueryResult2UserId(DatabaseConnectionHandle, QueryResult, UserId));

	return BMD_OK;
}

long AA_ReadAttributesOwnedByUser(	void *DatabaseConnectionHandle,
						const char *UserId,
						AttributesSequence_t **AttributesSequence,
						const ConfigData_t *ConfigData)
{
	PRINT_VDEBUG("LIBBMDAA2INF Reading attributes owned by user\n");
	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if (DatabaseConnectionHandle == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (AttributesSequence == NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (*AttributesSequence != NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (ConfigData == NULL)					{	BMD_FOK(BMD_ERR_PARAM3);	}

	(*AttributesSequence)=(AttributesSequence_t *)malloc(sizeof(AttributesSequence_t));
	if( (*AttributesSequence) == NULL )		{	BMD_FOK(BMD_ERR_MEMORY);	}

	memset(*AttributesSequence,0,sizeof(AttributesSequence_t));
	BMD_FOK(AA_ReadGroupsFromDatabase(DatabaseConnectionHandle, UserId, *AttributesSequence, ConfigData));
	BMD_FOK(AA_ReadRolesFromDatabase(DatabaseConnectionHandle, UserId, *AttributesSequence, ConfigData));
	BMD_FOK(AA_ReadClearancesFromDatabase(DatabaseConnectionHandle, UserId, *AttributesSequence, ConfigData));
// 	BMD_FOK(AA_ReadUnizetosFromDatabase(DatabaseConnectionHandle, UserId, *AttributesSequence, ConfigData));

	return BMD_OK;
}


/***********************
 * USERS
 ***********************/
long AA_SelectUserId(	void *hDB,
			const char *CertificateSerialNr,
			const char *CertificateIssuerDN,
			char **UserId)
{
char *SQLQuery 		= NULL;
char **ans			= NULL;
long ans_count		= 0;
long i			= 0;

	PRINT_VDEBUG("LIBBMDAA2INF Selecting user id from database\n");
	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if (hDB == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (CertificateSerialNr == NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (CertificateIssuerDN == NULL)		{	BMD_FOK(BMD_ERR_PARAM3);	}
	if (UserId == NULL)				{	BMD_FOK(BMD_ERR_PARAM4);	}

	if ((*UserId) != NULL)
	{
		free(*UserId); (*UserId)=NULL;
	}

	asprintf(&SQLQuery, "SELECT id, name, cert_serial, cert_dn, accepted FROM users WHERE cert_serial='%s' AND cert_dn='%s';",CertificateSerialNr,CertificateIssuerDN);
	if (SQLQuery==NULL)			{	BMD_FOK(BMD_ERR_MEMORY);	}

	ExecuteSQLQueryWithRowAnswersKnownDBConnection( hDB, SQLQuery, &ans_count, &ans);
	if (ans_count==0)
	{
		BMD_FOK(LIBBMDAA2_ERR_USER_LOGIN_NOT_REGISTERED);
	}

	if (strcmp(ans[4], "1")!=0)
	{
		BMD_FOK(LIBBMDAA2_ERR_USER_LOGIN_NOT_ACCEPTED);
	}

	asprintf(UserId, "%s", ans[4]);
	if ((*UserId)==NULL)			{	BMD_FOK(BMD_ERR_MEMORY);	}

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

// // // // // long AA_QueryResult2UserId(void *DatabaseConnectionHandle, void *QueryResult, char **UserId)
// // // // // {
// // // // // long err = 0;
// // // // // long fetched_counter = 0;
// // // // // db_row_strings_t *rowstruct	= NULL;
// // // // //
// // // // // 	PRINT_VDEBUG("LIBBMDAA2INF Converting query result to user id\n");
// // // // // 	if(QueryResult == NULL)
// // // // // 	{
// // // // // 		PRINT_DEBUG("LIBBMDAA2ERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
// // // // // 		return BMD_ERR_PARAM1;
// // // // // 	}
// // // // // 	if(UserId == NULL || *UserId != NULL)
// // // // // 	{
// // // // // 		PRINT_DEBUG("LIBBMDAA2ERR Invalid second parameter value. Error=%i\n",BMD_ERR_PARAM2);
// // // // // 		return BMD_ERR_PARAM2;
// // // // // 	}
// // // // //
// // // // // 	/* Interesuje mnie tylko pierwszy rezultat SELECT'a - dlatego nie iteruje dalej */
// // // // // 	err = bmd_db_result_get_row(DatabaseConnectionHandle, QueryResult,0, 0, &rowstruct, FETCH_NEXT, &fetched_counter);
// // // // // 	if(err==BMD_ERR_NODATA)
// // // // // 	{
// // // // // 		PRINT_DEBUG("LIBBMDAA2ERR User not found found in database. Error=%li\n", err);
// // // // // 		bmd_db_row_struct_free(&rowstruct);
// // // // // 		return err;
// // // // // 	}
// // // // // 	if(err<0)
// // // // // 	{
// // // // // 		PRINT_DEBUG("LIBBMDAA2ERR Error in getting id of user. Errore=%li\n", err);
// // // // // 		return err;
// // // // // 	}
// // // // // 	asprintf(UserId, "%s", rowstruct->colvals[0]);
// // // // // 	if(*UserId == NULL)
// // // // // 	{
// // // // // 		PRINT_DEBUG("LIBBMDAA2ERR Memory error. Error=%i\n",NO_MEMORY);
// // // // // 		return NO_MEMORY;
// // // // // 	}
// // // // // 	bmd_db_row_struct_free(&rowstruct);
// // // // // 	return err;
// // // // // }

/***********************
 * GROUPS
 ***********************/
long AA_ReadGroupsFromDatabase(	void *hDB,
						const char *UserId,
						AttributesSequence_t *AttributesSequence,
						const ConfigData_t *ConfigData)
{

GenBufList_t *GenBufList	= NULL;

	/************************/
	/* walidacja parametrow */
	/************************/
	if(hDB == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(AttributesSequence == NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(ConfigData == NULL)				{	BMD_FOK(BMD_ERR_PARAM3);	}

	BMD_FOK(AA_SelectGroups(hDB, UserId, &GenBufList));

	if(GenBufList->size > 0)
	{
		BMD_FOK(AC_MakeAttribute_IetfAttrSyntax(GenBufList, AttributesSequence, ConfigData));
	}
	free_GenBufList(&GenBufList);

	return BMD_OK;
}

long AA_SelectGroups(	void *hDB,
				const char *UserId,
				GenBufList_t **GenBufList)
{
long ans_count		= 0;
long i			= 0;
char *SQLQuery 		= NULL;
char **ans			= NULL;

	PRINT_VDEBUG("LIBBMDAA2INF Reading groups from database\n");

	/************************/
	/* walidacja parametrow */
	/************************/
	if (hDB == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (UserId == NULL)				{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (GenBufList == NULL)				{	BMD_FOK(BMD_ERR_PARAM3);	}
	if (*GenBufList != NULL)			{	BMD_FOK(BMD_ERR_PARAM3);	}

	asprintf(&SQLQuery, "SELECT fk_groups FROM users_and_groups WHERE fk_users=%s;", UserId);
	if (SQLQuery==NULL)		{	BMD_FOK(BMD_ERR_MEMORY);	}

	ExecuteSQLQueryWithAnswersKnownDBConnection( hDB, SQLQuery, &ans_count, &ans);
	if (ans_count==0)
	{
		BMD_FOK(LIBBMDAA2_ERR_USER_GROUP_NOT_SPECIFIED);
	}

	(*GenBufList) = (GenBufList_t *) malloc (sizeof(GenBufList_t));
	memset(*GenBufList, 0, sizeof(GenBufList_t));
	(*GenBufList)->size=ans_count;
	(*GenBufList)->gbufs = (GenBuf_t **)malloc(sizeof(GenBuf_t *) * (*GenBufList)->size);

	for (i=0; i<ans_count; i++)
	{
		(*GenBufList)->gbufs[i]=(GenBuf_t *)malloc(sizeof(GenBuf_t));
		(*GenBufList)->gbufs[i]->buf=(char *)malloc(sizeof(char) * (strlen(ans[i])+1));
		memmove((*GenBufList)->gbufs[i]->buf, ans[i], strlen(ans[i])+1);
		(*GenBufList)->gbufs[i]->size=(long)strlen(ans[i]);
		free(ans[i]); ans[i]=NULL;
	}
	free(ans); ans=NULL;

	/************/
	/* porzadki */
	/************/
	free(SQLQuery); SQLQuery=NULL;

	return BMD_OK;
}

// long /*AA_GroupsRolesQueryResult2GenBufList*/(	void *DatabaseConnectionHandle,
// 								void *QueryResult,
// 								GenBufList_t **GenBufList)
// {
// long err 					= 0;
// long fetched_counter	= 0;
// db_row_strings_t *rowstruct		= NULL;
// GenBuf_t *tmp				= NULL;
//
// 	if(DatabaseConnectionHandle == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
// 	if(QueryResult == NULL)					{	BMD_FOK(BMD_ERR_PARAM2);	}
// 	if(GenBufList == NULL || *GenBufList != NULL)	{	BMD_FOK(BMD_ERR_PARAM3);	}
//
// 	*GenBufList = (GenBufList_t *) malloc (sizeof(GenBufList_t));
// 	memset(*GenBufList, 0, sizeof(GenBufList_t));
//
// 	while(err>=0)
// 	{
// 		err = bmd_db_result_get_row(DatabaseConnectionHandle, QueryResult,0, 0, &rowstruct, FETCH_NEXT, &fetched_counter);
// 		if(err==BMD_ERR_NODATA){ err=0; bmd_db_row_struct_free(&rowstruct); break; }
// 		BMD_FOK(err);
//
// 		(*GenBufList)->size++;
// 		(*GenBufList)->gbufs = (GenBuf_t **) realloc ((*GenBufList)->gbufs, sizeof(GenBuf_t *) * (*GenBufList)->size);
//
// 		tmp = (GenBuf_t *) malloc (sizeof(GenBuf_t));
// 		tmp->buf = (char *) malloc (sizeof(char) * (1 + strlen(rowstruct->colvals[0])));
//
// 		memmove(tmp->buf, rowstruct->colvals[0], 1+strlen(rowstruct->colvals[0]));
//
// 		tmp->size = 1+(long)strlen(rowstruct->colvals[0]);
// 		(*GenBufList)->gbufs[((*GenBufList)->size)-1] = tmp;
// 		bmd_db_row_struct_free(&rowstruct);
// 	}
//
// 	return err;
// }

/***********************
 * ROLES
 ***********************/
long AA_ReadRolesFromDatabase(	void *DatabaseConnectionHandle,
						const char *UserId,
						AttributesSequence_t *AttributesSequence,
						const ConfigData_t *ConfigData)
{
void *QueryResult = NULL;
GenBufList_t *GenBufList = NULL;

	if(DatabaseConnectionHandle == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(AttributesSequence == NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(ConfigData == NULL)					{	BMD_FOK(BMD_ERR_PARAM3);	}

	BMD_FOK(AA_SelectRoles(DatabaseConnectionHandle, UserId, &GenBufList));
// 	BMD_FOK(AA_GroupsRolesQueryResult2GenBufList(DatabaseConnectionHandle, QueryResult, &GenBufList));

	bmd_db_result_free(&QueryResult);
	if(GenBufList->size > 0)
	{
		BMD_FOK(AC_MakeAttribute_RoleSyntax(GenBufList, AttributesSequence, ConfigData));
	}
	free_GenBufList(&GenBufList);

	return BMD_OK;
}

long AA_SelectRoles(	void *hDB,
				const char *UserId,
				GenBufList_t **GenBufList)
{
char *SQLQuery 	= NULL;
char **ans		= NULL;
long ans_count	= 0;
long i		= 0;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (hDB==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (UserId == NULL)				{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (GenBufList == NULL)				{	BMD_FOK(BMD_ERR_PARAM3);	}
	if (*GenBufList != NULL)			{	BMD_FOK(BMD_ERR_PARAM3);	}

	asprintf(&SQLQuery, "SELECT fk_roles FROM users_and_roles WHERE fk_users=%s;", UserId);
	if (SQLQuery==NULL)		{	BMD_FOK(BMD_ERR_MEMORY);	}

	ExecuteSQLQueryWithAnswersKnownDBConnection( hDB, SQLQuery, &ans_count, &ans);
	if (ans_count==0)
	{
		BMD_FOK(LIBBMDAA2_ERR_USER_ROLE_NOT_SPECIFIED);
	}

	(*GenBufList) = (GenBufList_t *) malloc (sizeof(GenBufList_t));
	memset(*GenBufList, 0, sizeof(GenBufList_t));
	(*GenBufList)->size=ans_count;
	(*GenBufList)->gbufs = (GenBuf_t **)malloc(sizeof(GenBuf_t *) * (*GenBufList)->size);

	for (i=0; i<ans_count; i++)
	{
		(*GenBufList)->gbufs[i]=(GenBuf_t *)malloc(sizeof(GenBuf_t));
		(*GenBufList)->gbufs[i]->buf=(char *)malloc(sizeof(char) * (strlen(ans[i])+1));
		memmove((*GenBufList)->gbufs[i]->buf, ans[i], strlen(ans[i])+1);
		(*GenBufList)->gbufs[i]->size=(long)strlen(ans[i]);
		free(ans[i]); ans[i]=NULL;
	}
	free(ans); ans=NULL;

	/************/
	/* porzadki */
	/************/
	free(SQLQuery); SQLQuery=NULL;

	return BMD_OK;
}

/***********************
 * CLEARANCES
 ***********************/
long AA_ReadClearancesFromDatabase(	void *DatabaseConnectionHandle,
						const char *UserId,
						AttributesSequence_t *AttributesSequence,
						const ConfigData_t *ConfigData)
{
void *QueryResult			= NULL;
GenBufList_t *GenBufList	= NULL;

	if(DatabaseConnectionHandle == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(AttributesSequence == NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(ConfigData == NULL)					{	BMD_FOK(BMD_ERR_PARAM3);	}

	BMD_FOK(AA_SelectClearances(DatabaseConnectionHandle, UserId, &QueryResult));
 	BMD_FOK(AA_ClearancesQueryResult2GenBufList(DatabaseConnectionHandle, QueryResult, &GenBufList));
	bmd_db_result_free(&QueryResult);

	if(GenBufList->size > 0)
	{
		BMD_FOK(AC_MakeAttribute_Clearance(GenBufList, AttributesSequence, ConfigData));
	}

	free_GenBufList(&GenBufList);

	return BMD_OK;
}

long AA_SelectClearances(	void *DatabaseConnectionHandle,
					const char *UserId,
					void **QueryResult)
{
long err 		= 0;
long rows		= 0;
long cols		= 0;
char *SQLQuery 	= NULL;

	if(DatabaseConnectionHandle == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(UserId == NULL)					{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(QueryResult == NULL || *QueryResult != NULL)	{	BMD_FOK(BMD_ERR_PARAM3);	}

	asprintf(&SQLQuery, "SELECT sec_levels.name, sec_categories.type, sec_categories.name FROM users_and_security LEFT JOIN sec_categories ON (users_and_security.fk_sec_categories=sec_categories.id) LEFT JOIN sec_levels ON (users_and_security.fk_sec_levels=sec_levels.id) WHERE users_and_security.fk_users=1;");

	err = bmd_db_execute_sql(DatabaseConnectionHandle, SQLQuery, &rows, &cols, QueryResult);
	if(err<0 )
	{
		free(SQLQuery); SQLQuery=NULL;
		BMD_FOK(err);
	}
	free(SQLQuery); SQLQuery=NULL;

	return BMD_OK;
}

long AA_ClearancesQueryResult2GenBufList(void *DatabaseConnectionHandle, void *QueryResult, GenBufList_t **GenBufList)
{
long err					= 0;
long fetched_counter	= 0;
db_row_strings_t *rowstruct		= NULL;
GenBuf_t *tmp[3];
long i 					= 0;

	if(DatabaseConnectionHandle == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(QueryResult == NULL)					{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(GenBufList == NULL || *GenBufList != NULL)	{	BMD_FOK(BMD_ERR_PARAM3);	}

	*GenBufList = (GenBufList_t *) malloc (sizeof(GenBufList_t));
	memset(*GenBufList, 0, sizeof(GenBufList_t));

	while(err>=0)
	{
		err = bmd_db_result_get_row(DatabaseConnectionHandle, QueryResult, 0, 0, &rowstruct, FETCH_NEXT, &fetched_counter);
		if(err==BMD_ERR_NODATA)
		{
			err=0;
			bmd_db_row_struct_free(&rowstruct);
			break;
		}
		BMD_FOK(err);

		(*GenBufList)->size = (*GenBufList)->size +3;
		(*GenBufList)->gbufs = (GenBuf_t **) realloc ((*GenBufList)->gbufs, sizeof(GenBuf_t *) * (*GenBufList)->size);

		for(i=0; i<3; i++)
		{
			tmp[i] = (GenBuf_t *) malloc (sizeof(GenBuf_t));

			tmp[i]->buf = (char *) malloc (sizeof(char) * (1 + strlen(rowstruct->colvals[i])));

			memmove(tmp[i]->buf, rowstruct->colvals[i], 1+strlen(rowstruct->colvals[i]));
			tmp[i]->size = 1+(long)strlen(rowstruct->colvals[i]);
			(*GenBufList)->gbufs[((*GenBufList)->size)-3 + i] = tmp[i];
		}
		bmd_db_row_struct_free(&rowstruct);
	}

	return err;
}

/***********************
 * UNIZETOS
 ***********************/
long AA_ReadUnizetosFromDatabase(	void *DatabaseConnectionHandle,
						const char *UserId,
						AttributesSequence_t *AttributesSequence,
						const ConfigData_t *ConfigData)
{
long err = 0;
void *UnizetoIdQueryResult = NULL;
UnizetoAttrSyntaxInList_t *UnizetoAttrSyntaxInList = NULL;
UnizetoAttrSyntaxIn_t *UnizetoAttrSyntaxIn = NULL;
long fetched_counter = 0;
char *UnizetoAttrIdString = NULL;

	if(DatabaseConnectionHandle == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(AttributesSequence == NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(ConfigData == NULL)					{	BMD_FOK(BMD_ERR_PARAM3);	}

	BMD_FOK(AA_FindUnizetoIdsForGivenUser(DatabaseConnectionHandle, UserId, ConfigData, &UnizetoIdQueryResult));
	UnizetoAttrSyntaxInList = (UnizetoAttrSyntaxInList_t *) malloc (sizeof(UnizetoAttrSyntaxInList_t));

	memset(UnizetoAttrSyntaxInList, 0, sizeof(UnizetoAttrSyntaxInList_t));

	while(err>=0)
	{
		err = AA_UnizetoIdQuery2UnizetoIDChar(DatabaseConnectionHandle, UnizetoIdQueryResult, &fetched_counter, &UnizetoAttrIdString);
		if(err==BMD_ERR_NODATA) {err=0; break;} /* brak danych w bazie */
		BMD_FOK(err);

		BMD_FOK(AA_UnizetoIdQuery2UnizetoAttrSyntaxIn(DatabaseConnectionHandle, UnizetoAttrIdString, &UnizetoAttrSyntaxIn));

		UnizetoAttrSyntaxInList->size++;
		UnizetoAttrSyntaxInList->UnizetoAttrSyntaxIn = (UnizetoAttrSyntaxIn_t **) realloc
						(UnizetoAttrSyntaxInList->UnizetoAttrSyntaxIn,
						sizeof(UnizetoAttrSyntaxIn_t *) * UnizetoAttrSyntaxInList->size);

		UnizetoAttrSyntaxInList->UnizetoAttrSyntaxIn[UnizetoAttrSyntaxInList->size-1] = UnizetoAttrSyntaxIn;
		if(UnizetoAttrIdString){free(UnizetoAttrIdString); UnizetoAttrIdString=NULL;}
	}
	BMD_FOK(AC_MakeAttribute_UnizetoAttrSyntax(UnizetoAttrSyntaxInList, AttributesSequence, ConfigData));

	UnizetoAttrSyntaxInList_free_ptr(&UnizetoAttrSyntaxInList);
	bmd_db_result_free(&UnizetoIdQueryResult);
	return err;
}

long AA_FindUnizetoIdsForGivenUser(	void *DatabaseConnectionHandle,
						const char *UserId,
						const ConfigData_t *ConfigData,
						void **QueryResult)
{
long err		= 0;
long rows 		= 0;
long cols 		= 0;
char *SQLQuery	= NULL;

	if(DatabaseConnectionHandle == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(UserId == NULL)					{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(ConfigData == NULL)					{	BMD_FOK(BMD_ERR_PARAM3);	}
	if(QueryResult == NULL || *QueryResult != NULL)	{	BMD_FOK(BMD_ERR_PARAM4);	}

	asprintf(&SQLQuery, "SELECT id FROM uniattr_main WHERE fk_aaholder_main = '%s';", UserId);

	err = bmd_db_execute_sql(DatabaseConnectionHandle, SQLQuery, &rows, &cols, QueryResult);
	if(err<0 )
	{
		free(SQLQuery); SQLQuery=NULL;
		BMD_FOK(err);
	}
	free(SQLQuery); SQLQuery=NULL;

	return err;
}

long AA_UnizetoIdQuery2UnizetoIDChar(	void *DatabaseConnectionHandle,
							void *UnizetoIdQueryResult,
							long *fetched_counter,
							char **UnizetoAttrSyntaxId)
{
long err				= 0;
db_row_strings_t *rowstruct	= NULL;

	if(DatabaseConnectionHandle == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(UnizetoIdQueryResult == NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(UnizetoAttrSyntaxId == NULL ||
		*UnizetoAttrSyntaxId != NULL)			{	BMD_FOK(BMD_ERR_PARAM3);	}

	err = bmd_db_result_get_row(DatabaseConnectionHandle, UnizetoIdQueryResult, 0, 0, &rowstruct, FETCH_NEXT, fetched_counter);
	if(err<0)
	{
		bmd_db_row_struct_free(&rowstruct);
		return err;
	}

	*UnizetoAttrSyntaxId = (char *) malloc (sizeof(char) * (1+ strlen(rowstruct->colvals[0])));

	strcpy(*UnizetoAttrSyntaxId, rowstruct->colvals[0]);
	bmd_db_row_struct_free(&rowstruct);
	return err;
}


long AA_UnizetoIdQuery2UnizetoAttrSyntaxIn(	void *DatabaseConnectionHandle,
								char *UnizetoAttrIdString,
								UnizetoAttrSyntaxIn_t **UnizetoAttrSyntaxIn)
{
	if(UnizetoAttrIdString == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(UnizetoAttrSyntaxIn == NULL ||
		*UnizetoAttrSyntaxIn != NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}

	*UnizetoAttrSyntaxIn = (UnizetoAttrSyntaxIn_t *) malloc (sizeof(UnizetoAttrSyntaxIn_t));

	memset(*UnizetoAttrSyntaxIn, 0, sizeof(UnizetoAttrSyntaxIn_t));

	BMD_FOK(AA_UnizetoAttr_set_SyntaxOID(DatabaseConnectionHandle, UnizetoAttrIdString, *UnizetoAttrSyntaxIn));
	BMD_FOK(AA_UnizetoAttr_set_Slots(DatabaseConnectionHandle, UnizetoAttrIdString, *UnizetoAttrSyntaxIn));

	return BMD_OK;
}

long AA_UnizetoAttr_set_SyntaxOID(void *DatabaseConnectionHandle, char *UnizetoAttrIdString,
				UnizetoAttrSyntaxIn_t *UnizetoAttrSyntaxIn)
{
long err					= 0;
long fetched_counter		= 0;
long rows				= 0;
long cols				= 0;
char *SQLQuery				= NULL;
void *QueryResult			= NULL;
db_row_strings_t *rowstruct		= NULL;

	if(UnizetoAttrIdString == NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(UnizetoAttrSyntaxIn == NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}

	asprintf(&SQLQuery, "SELECT syntaxoid FROM uniattr_main LEFT JOIN uniattr_syntax_oid "
				"ON uniattr_main.fk_uniattr_syntax_oid = uniattr_syntax_oid.id "
				"WHERE uniattr_main.id = '%s';", UnizetoAttrIdString);

	err = bmd_db_execute_sql(DatabaseConnectionHandle,SQLQuery, &rows, &cols, &QueryResult);
	if(err<0 )
	{
		free(SQLQuery); SQLQuery=NULL;
		BMD_FOK(err);
	}
	free(SQLQuery); SQLQuery=NULL;

	BMD_FOK(bmd_db_result_get_row(DatabaseConnectionHandle, QueryResult, 0, 0, &rowstruct, FETCH_NEXT, &fetched_counter));

	UnizetoAttrSyntaxIn->SyntaxOid = (GenBuf_t *) malloc (sizeof(GenBuf_t));

	memset(UnizetoAttrSyntaxIn->SyntaxOid, 0, sizeof(GenBuf_t));

	UnizetoAttrSyntaxIn->SyntaxOid->buf = (char *) malloc (sizeof(char) * (1+strlen(rowstruct->colvals[0])));

	memmove(UnizetoAttrSyntaxIn->SyntaxOid->buf, rowstruct->colvals[0], 1+strlen(rowstruct->colvals[0]));
	UnizetoAttrSyntaxIn->SyntaxOid->size = 1+(long)strlen(rowstruct->colvals[0]);
	bmd_db_row_struct_free(&rowstruct);
	bmd_db_result_free(&QueryResult);

	return err;
}

long AA_UnizetoAttr_set_Slots(	void *DatabaseConnectionHandle,
					char *UnizetoAttrIdString,
					UnizetoAttrSyntaxIn_t *UnizetoAttrSyntaxIn)
{
long err					= 0;
long fetched_counter 		= 0;
long no_of_slots				= 0;
char *SlotIdString			= NULL;
char *SlotTypeString			= NULL;
void *UnizetoIdQueryResult		= NULL;

	if(DatabaseConnectionHandle == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(UnizetoAttrIdString == NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(UnizetoAttrSyntaxIn == NULL)			{	BMD_FOK(BMD_ERR_PARAM3);	}

	BMD_FOK(AA_FindSlotsForGivenUnizeto(DatabaseConnectionHandle, UnizetoAttrIdString, &UnizetoIdQueryResult));

	UnizetoAttrSyntaxIn->SlotList = (SlotListIn_t *) malloc (sizeof(SlotListIn_t));

	memset(UnizetoAttrSyntaxIn->SlotList, 0, sizeof(SlotListIn_t));

	while(err>=0)
	{
		err = AA_GetSlotTypeAndIdFromResult(DatabaseConnectionHandle, UnizetoIdQueryResult,	&fetched_counter, &SlotIdString, &SlotTypeString);
		if(err==BMD_ERR_NODATA) {err=0; break;} /* brak danych w bazie */
		BMD_FOK(err);

		no_of_slots = ++(UnizetoAttrSyntaxIn->SlotList->size);
		UnizetoAttrSyntaxIn->SlotList->Slots = (SlotIn_t **) realloc (UnizetoAttrSyntaxIn->SlotList->Slots, sizeof(SlotIn_t *) * no_of_slots);
		if(UnizetoAttrSyntaxIn->SlotList->Slots == NULL) return NO_MEMORY;

		BMD_FOK(AA_CreateEmptySlotIn(&(UnizetoAttrSyntaxIn->SlotList->Slots[no_of_slots-1])));
		BMD_FOK(AA_SlotIn_set_Type(SlotTypeString, UnizetoAttrSyntaxIn->SlotList->Slots[no_of_slots-1]));
		BMD_FOK(AA_SlotIn_set_Values(DatabaseConnectionHandle, SlotIdString, UnizetoAttrSyntaxIn->SlotList->Slots[no_of_slots-1]));

		free(SlotIdString); SlotIdString = NULL;
		free(SlotTypeString); SlotTypeString = NULL;
	}
	bmd_db_result_free(&UnizetoIdQueryResult);
	return err;
}

long AA_FindSlotsForGivenUnizeto(	void *DatabaseConnectionHandle,
						const char *UnizetoAttrIdString,
						void **UnizetoIdQueryResult)
{
long err			= 0;
char *SQLQuery		= NULL;
long rows			= 0;
long cols			= 0;

	if(DatabaseConnectionHandle == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(UnizetoAttrIdString == NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(UnizetoIdQueryResult == NULL ||
		*UnizetoIdQueryResult != NULL)		{	BMD_FOK(BMD_ERR_PARAM3);	}

	asprintf(&SQLQuery, "SELECT uniattr_slot.id, slottypeoid FROM "
		"uniattr_slot LEFT JOIN uniattr_main ON uniattr_slot.fk_uniattr_main = uniattr_main.id "
		"LEFT JOIN uniattr_slot_types ON uniattr_slot.fk_uniattr_slot_types = uniattr_slot_types.id "
		"WHERE uniattr_slot.fk_uniattr_main = '%s';", UnizetoAttrIdString);

	err = bmd_db_execute_sql(DatabaseConnectionHandle, SQLQuery, &rows, &cols, UnizetoIdQueryResult);
	if(err<0 )
	{
		free(SQLQuery); SQLQuery=NULL;
		BMD_FOK(err);
	}
	free(SQLQuery); SQLQuery=NULL;
	return err;
}

long AA_GetSlotTypeAndIdFromResult(	void *DatabaseConnectionHandle,
						void *UnizetoIdQueryResult,
						long *fetched_counter,
						char **SlotIdString, char **SlotTypeString)
{
long err				= 0;
db_row_strings_t *rowstruct	= NULL;

	if(DatabaseConnectionHandle == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(UnizetoIdQueryResult == NULL)			{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(fetched_counter == NULL ||
		*fetched_counter < 0)				{	BMD_FOK(BMD_ERR_PARAM3);	}
	if(SlotIdString == NULL ||
		*SlotIdString != NULL)				{	BMD_FOK(BMD_ERR_PARAM4);	}
	if(SlotTypeString == NULL ||
		*SlotTypeString != NULL)			{	BMD_FOK(BMD_ERR_PARAM5);	}

	BMD_FOK(bmd_db_result_get_row(DatabaseConnectionHandle, UnizetoIdQueryResult, 0, 0, &rowstruct, FETCH_NEXT, fetched_counter));

	*SlotIdString = (char *) malloc (sizeof(char) * (1+ strlen(rowstruct->colvals[0])));

	strcpy(*SlotIdString, rowstruct->colvals[0]);
	*SlotTypeString = (char *) malloc (sizeof(char) * (1+ strlen(rowstruct->colvals[1])));

	strcpy(*SlotTypeString, rowstruct->colvals[1]);
	bmd_db_row_struct_free(&rowstruct);

	return err;
}

long AA_CreateEmptySlotIn(SlotIn_t **SlotIn)
{
long err = 0;
	/* bez sprawdzania argumentu wejsciowego bo sa one realokowane
	   i nie memsetowane */
	*SlotIn = (SlotIn_t *) malloc (sizeof(SlotIn_t));

	memset(*SlotIn, 0, sizeof(SlotIn_t));
	return err;
}

long AA_SlotIn_set_Type(	const char *SlotTypeString,
				SlotIn_t *SlotIn)
{
long err = 0;
	if(SlotTypeString == NULL)				{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(SlotIn == NULL)					{	BMD_FOK(BMD_ERR_PARAM2);	}

	SlotIn->OidString = (GenBuf_t *) malloc (sizeof(GenBuf_t));

	memset(SlotIn->OidString, 0, sizeof(GenBuf_t));
	SlotIn->OidString->buf = (char *) malloc (sizeof(char) * (1+strlen(SlotTypeString)));
	strcpy((char *)SlotIn->OidString->buf, SlotTypeString);
	SlotIn->OidString->size = 1+(long)strlen(SlotTypeString);

	return err;
}

long AA_SlotIn_set_Values(	void *DatabaseConnectionHandle,
					const char *SlotIdString,
					SlotIn_t *SlotIn)
{
long err			= 0;
void *ValuesQueryResult = NULL;

	if(SlotIdString == NULL)				{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(SlotIn == NULL)					{	BMD_FOK(BMD_ERR_PARAM2);	}

	BMD_FOK(AA_FindSlotValuesForGivenSlot(DatabaseConnectionHandle, SlotIdString, &ValuesQueryResult));
	BMD_FOK(AA_SlotValues2SlotInGenBufList(DatabaseConnectionHandle, ValuesQueryResult, SlotIn));
	bmd_db_result_free(&ValuesQueryResult);

	return err;
}

long AA_FindSlotValuesForGivenSlot(	void *DatabaseConnectionHandle,
						const char *SlotIdString,
						void **UnizetoIdQueryResult)
{
long err 			= 0;
char *SQLQuery 		= NULL;
long rows 			= 0;
long cols 			= 0;

	if(DatabaseConnectionHandle == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(SlotIdString == NULL)				{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(UnizetoIdQueryResult == NULL ||
		*UnizetoIdQueryResult != NULL)		{	BMD_FOK(BMD_ERR_PARAM3);	}

	asprintf(&SQLQuery, "SELECT value FROM uniattr_slot_values WHERE fk_uniattr_slot = '%s';", SlotIdString);

	err = bmd_db_execute_sql(DatabaseConnectionHandle, SQLQuery, &rows, &cols, UnizetoIdQueryResult);
	if(err<0 )
	{
		free(SQLQuery); SQLQuery=NULL;
		BMD_FOK(err);
	}
	free(SQLQuery); SQLQuery=NULL;

	return err;
}

long AA_SlotValues2SlotInGenBufList(	void *DatabaseConnectionHandle,
						void *ValuesQueryResult,
						SlotIn_t *SlotIn)
{
long err					= 0;
long fetched_counter	= 0;
GenBuf_t *GenBuf				= NULL;
long no_of_slots				= 0;

	if(DatabaseConnectionHandle == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(ValuesQueryResult == NULL)				{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(SlotIn == NULL)					{	BMD_FOK(BMD_ERR_PARAM3);	}

	SlotIn->ValuesList = (GenBufList_t *) malloc (sizeof(GenBufList_t));
	memset(SlotIn->ValuesList, 0, sizeof(GenBufList_t));

	while(err>=0)
	{
		err = AA_SlotValues2GenBuf(DatabaseConnectionHandle, ValuesQueryResult,	&fetched_counter, &GenBuf);
		if(err==BMD_ERR_NODATA) {err=0; break;} /* brak danych w bazie */
		BMD_FOK(err);
		no_of_slots = ++(SlotIn->ValuesList->size);
		SlotIn->ValuesList->gbufs = (GenBuf_t **) realloc (SlotIn->ValuesList->gbufs,sizeof(GenBuf_t *) * no_of_slots);

		SlotIn->ValuesList->gbufs[no_of_slots-1] = GenBuf;
		GenBuf = NULL;
	}
	return err;
}

long AA_SlotValues2GenBuf(	void *DatabaseConnectionHandle,
					void *ValuesQueryResult,
					long *fetched_counter,
					GenBuf_t **GenBuf)
{
long err				= 0;
db_row_strings_t *rowstruct	= NULL;

	if(DatabaseConnectionHandle == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(ValuesQueryResult == NULL)				{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(fetched_counter == NULL ||
		*fetched_counter < 0)				{	BMD_FOK(BMD_ERR_PARAM3);	}
	if(GenBuf == NULL || *GenBuf != NULL)		{	BMD_FOK(BMD_ERR_PARAM4);	}

	BMD_FOK(bmd_db_result_get_row(DatabaseConnectionHandle, ValuesQueryResult, 0, 0, &rowstruct, FETCH_NEXT, fetched_counter));

	*GenBuf = (GenBuf_t *) malloc (sizeof(GenBuf_t));
	memset(*GenBuf, 0, sizeof(GenBuf_t));

	(*GenBuf)->buf = (char *) malloc (sizeof(char) * (1+ strlen(rowstruct->colvals[0])));
	strcpy((char *)(*GenBuf)->buf, rowstruct->colvals[0]);
	(*GenBuf)->size = 1+ (long)strlen(rowstruct->colvals[0]);
	bmd_db_row_struct_free(&rowstruct);

	return err;
}
