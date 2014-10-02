#include <bmd/libbmdsql/select.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>
#include <bmd/common/global_types.h>
#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdglobals/libbmdglobals.h>


long MergeORConditions(bmdStringArray_t* valuesArray, char* columnName, char** resultCondition)
{
long iter = 0;
long retVal = 0;
long itemsCount = 0;
char* condition=NULL;
char* tempCondition=NULL;
char *itemPtr=NULL;


	if(valuesArray == NULL)
		{ return -1; }
	if(columnName == NULL)
		{ return -2; }
	if(strlen(columnName) == 0)
		{ return -3; }
	if(resultCondition == NULL)
		{ return -4; }
	if(*resultCondition != NULL)
		{ return -5; }

	retVal=bmdStringArray_GetItemsCount(valuesArray, &itemsCount);
	if(retVal < 0)
		{ return -11; }

	if(itemsCount <= 0)
		{ return -12; }

	for(iter=0; iter<itemsCount; iter++)
	{
		if(condition == NULL)
		{
			retVal=bmdStringArray_GetItemPtr(valuesArray, iter, &itemPtr);
			if(retVal < 0)
				{ return -13; }
			if(strlen(itemPtr) <= 0)
				{ return -14; }
			asprintf(&condition, "%s=%s", columnName, itemPtr);
			itemPtr=NULL;
			if(condition==NULL)
				{ return -15; }
		}
		else
		{
			retVal=bmdStringArray_GetItemPtr(valuesArray, iter, &itemPtr);
			if(retVal < 0)
			{
				free(condition);
				return -16;
			}
			if(strlen(itemPtr) <= 0)
			{
				free(condition);
				return -17;
			}
			asprintf(&tempCondition, "%s OR %s=%s", condition, columnName, itemPtr);
			itemPtr=NULL;
			if(tempCondition == NULL)
			{
				free(condition);
				return -18;
			}
			free(condition);
			condition=tempCondition;
			tempCondition=NULL;
		}
	}

	*resultCondition=condition;
	condition=NULL;
	return 0;
}



long GenerateMetadataConditions(	void* hDB,
					bmdDatagram_t *bmdDatagram,
					char **metadataCondition)
{
long retVal				= 0;
long i					= 0;
long k					= 0;
long no_of_metaData			= 0;
MetaDataBuf_t **metaData		= NULL;
char *tmp				= NULL;
char *recColumnName			= NULL;
char *recLogical			= NULL;
char *recRelation			= NULL;
char *recOpenBracets			= NULL;
char *recCloseBrackets			= NULL;
char *recStartPerc			= NULL;
char *recStopPerc			= NULL;
char *recMetadataValue			= NULL;
char *tempMetadataCondition		= NULL;
long add_met_count=0;
long sys_met_count=0;
long pki_met_count=0;
long act_met_count=0;
long metadataType=0;
char *metadataCast	= NULL;

	/************************/
	/* walidacja parametrow */
	/************************/
	if(hDB == NULL)
		{ return -1; }
	if(bmdDatagram==NULL)
		{ return -2; }
	if(metadataCondition == NULL)
		{ return -3; }
	if((*metadataCondition) != NULL)
		{ return -4; }

	for(k=0; k<4; k++)
	{
		switch(k)
		{
			case 0	:	no_of_metaData = bmdDatagram->no_of_sysMetaData; metaData = bmdDatagram->sysMetaData; break;
			case 1	:	no_of_metaData = bmdDatagram->no_of_additionalMetaData; metaData = bmdDatagram->additionalMetaData; break;
			case 2	:	no_of_metaData = bmdDatagram->no_of_actionMetaData; metaData = bmdDatagram->actionMetaData; break;
			case 3	:	no_of_metaData = bmdDatagram->no_of_pkiMetaData; metaData = bmdDatagram->pkiMetaData; break;
		}

		for(i=0; i<no_of_metaData; i++)
		{
			
			metadataType=determine_oid_type_str(metaData[i]->OIDTableBuf);
			if(metadataType != ADDITIONAL_METADATA && metadataType != SYS_METADATA)
			{
				continue;
			}

			free(metadataCast); metadataCast=NULL;
			retVal=DetermineSearchConditionArguments(	metaData[i],
								bmdDatagram->datagramStatus,
								&recColumnName,
								&recLogical,
								&recRelation,
								&recOpenBracets,
								&recCloseBrackets,
								&recStartPerc,
								&recStopPerc,
								&metadataCast,
								&add_met_count,
								&sys_met_count,
								&act_met_count,
								&pki_met_count)	;
			
			if (recColumnName!=NULL)
			{
				/*ponizsze oidy nalezy ignorowac*/
				if(	strcmp(recColumnName, "crypto_objects.location_id") == 0 ||
					strcmp(recColumnName, "crypto_objects.fk_group") == 0 ||
					strcmp(recColumnName, "crypto_objects.fk_sec_categories") == 0 ||
					strcmp(recColumnName, "crypto_objects.fk_sec_levels") == 0 ||
					strcmp(recColumnName, "crypto_objects.visible") == 0 ||
					strcmp(recColumnName, "crypto_objects.present") == 0 ||
					strcmp(recColumnName, "crypto_objects.transaction_id") == 0 )
				{
					free(recColumnName); recColumnName=NULL;
					free(recLogical); recLogical=NULL;
					free(recRelation); recRelation=NULL;
					free(recOpenBracets); recOpenBracets=NULL;
					free(recCloseBrackets); recCloseBrackets=NULL;
					free(recStartPerc); recStartPerc=NULL;
					free(recStopPerc); recStopPerc=NULL;
					continue;
				}
				
				free(tmp); tmp=NULL;
				asprintf(&tmp, "%s", tempMetadataCondition==NULL ? "" : tempMetadataCondition);
				free(tempMetadataCondition); tempMetadataCondition=NULL;

				if((recLogical==NULL) && (sys_met_count+add_met_count+act_met_count+pki_met_count<=1))
					{ asprintf(&recLogical, "%s", ""); }
				else if(recLogical==NULL)
					{ asprintf(&recLogical, "%s", "AND"); }
				else if( (recLogical!=NULL) && (sys_met_count+add_met_count+act_met_count+pki_met_count<=1))
				{
					free(recLogical); recLogical=NULL;
					asprintf(&recLogical, "%s", "");
				}

				if (recRelation==NULL)
					{ asprintf(&recRelation, "%s", "LIKE"); }
				if (recOpenBracets==NULL)
					{ asprintf(&recOpenBracets, "%s", ""); }
				if (recCloseBrackets==NULL)
					{ asprintf(&recCloseBrackets, "%s", ""); }
				if (recStartPerc==NULL)
					{ asprintf(&recStartPerc, "'"); }
				if (recStopPerc==NULL)
					{ asprintf(&recStopPerc, "'"); }


				if (strcmp(recRelation, "LIKE")==0 || strcmp(recRelation, "ILIKE")==0)
				{
					BMD_FOK(bmd_db_escape_string(hDB, metaData[i]->AnyBuf,ADDITIONAL_ESCAPING,&recMetadataValue));
					if( bmd_db_connect_get_db_type(hDB) == BMD_DB_POSTGRES )
					{
						if(strcmp(recRelation, "LIKE")==0)
						{
							asprintf(&tempMetadataCondition, "%s\t%s %s %s::text %s %s%s%s ESCAPE '/' %s\n",
								tmp,
								recLogical,
								recOpenBracets,
								recColumnName,
								recRelation,
								recStartPerc,
								recMetadataValue,
								recStopPerc,
								recCloseBrackets);
						}
						else
						{
							asprintf(&tempMetadataCondition, "%s\t%s %s lower(%s::text) %s lower(%s%s%s) ESCAPE '/' %s\n",
								tmp,
								recLogical,
								recOpenBracets,
								recColumnName,
								"LIKE",
								recStartPerc,
								recMetadataValue,
								recStopPerc,
								recCloseBrackets);
						}
					}
					else
					{
						BMD_FOK(-666);
					}
				}
				else
				{
					asprintf(&recMetadataValue, "%s", metaData[i]->AnyBuf);
					if(metadataCast != NULL && strcmp(metadataCast, "timestamp") == 0 && recRelation != NULL && strcmp(recRelation, "<") == 0)
					{
						if( bmd_db_connect_get_db_type(hDB) == BMD_DB_POSTGRES )
						{
							asprintf(&tempMetadataCondition, "%s\t%s %s %s %s timestamp '%s' + interval '1 day' %s\n",
									tmp,
									recLogical,
									recOpenBracets,
									recColumnName,
									recRelation,
									recMetadataValue,
									recCloseBrackets);
						}
						else
						{
							BMD_FOK(-666);
						}
					}
					else
					{
						if( bmd_db_connect_get_db_type(hDB) == BMD_DB_POSTGRES )
						{
							asprintf(&tempMetadataCondition, "%s\t%s %s %s %s %s%s%s %s\n",
									tmp,
									recLogical,
									recOpenBracets,
									recColumnName,
									recRelation,
									recStartPerc,
									recMetadataValue,
									recStopPerc,
									recCloseBrackets);
						}
						else
						{
							BMD_FOK(-666);
						}
					}
				}
				free(tmp); tmp=NULL;
				free(recMetadataValue); recMetadataValue=NULL;
			}
			
			free(recColumnName); recColumnName=NULL;
			free(recLogical); recLogical=NULL;
			free(recRelation); recRelation=NULL;
			free(recOpenBracets); recOpenBracets=NULL;
			free(recCloseBrackets); recCloseBrackets=NULL;
			free(recStartPerc); recStartPerc=NULL;
			free(recStopPerc); recStopPerc=NULL;
		}
	}

	free(tmp); tmp=NULL;
	asprintf(&tmp, "%s %s %s",tempMetadataCondition==NULL ? "" : " AND (",
				tempMetadataCondition==NULL ? "": tempMetadataCondition,
				tempMetadataCondition==NULL ? "": ")");
	free(tempMetadataCondition); tempMetadataCondition=NULL;
	asprintf(&tempMetadataCondition, "%s", tmp);
	free(tmp); tmp=NULL;

	*metadataCondition=tempMetadataCondition;
	tempMetadataCondition=NULL;

	return BMD_OK;
}



long bmdUnreceived_Generate_SQL(void* hDB, bmdDatagram_t *datagramPtr, server_request_data_t *req, char **resultQuery)
{

bmdStringArray_t* actionIdsArray=NULL;
bmdStringArray_t* classIdsArray=NULL;
char* actionCondition=NULL;
char* classCondition=NULL;
char* groupId=NULL;
char* tempString=NULL;
long tempLong=0;

char* clearanceCondition=NULL;
char* tempClearanceCondition=NULL;
char* securityLevelPriority=NULL;
char* metadataCondition=NULL; /*kryteria wyszukiwania okreslone przez uzytkownika*/

char *orderClause=NULL;
char *paginatingStatement=NULL;

long addMetadataOidsCount=0;
char** addMetadataOids=NULL;
char *addMetadataColumns=NULL;
char *tempAddMetadataColumns=NULL;

char* location_id=_GLOBAL_bmd_configuration->location_id;
char* result=NULL;
long retVal=0;
long iter=0;


//
//\n
//\nSELECT id, location_id, name FROM crypto_objects
//\nWHERE
//\n	fk_group=%s /* parametr grupa wyszukujaca dokumenty */
//\n	%s /*poziomy i kategorie bezpieczenstwa*/
//\n	AND location_id=%s /*parametr*/
//\n	AND (transaction_id='' OR transaction_id IS NULL)
//\n	AND visible=1
//\n	AND present=1
//\n	AND (file_type='' OR file_type IS NULL OR file_type='link')
//\n	AND pointing_location_id=%s /*parametr*/
//\n	AND pointing_id NOT IN
//\n	(
//\n		/* (#2) */
//\n		SELECT DISTINCT pointing_id FROM crypto_objects
//\n		WHERE
//\n			location_id=%s /*parametr*/
//\n			AND (transaction_id='' OR transaction_id IS NULL)
//\n			AND visible=1
//\n			AND present=1
//\n			AND id IN
//\n			(
//\n				/* (#1) */
//\n				SELECT DISTINCT fk_objects FROM action_register
//\n				WHERE
//\n					location_id=%s /*parametr*/
//\n					AND ( %s ) /* parametr fk_actions=value [ OR fk_actions=value ...] */
//\n					AND ( %s ) /* parametr fk_users_class=value [ OR fk_users_class=value ...]  klasa wykonujaca akcje */
//\n			)
//\n	)
//\n;";
//


char* SQLtemplate="\n \
\nSELECT	crypto_objects.id, crypto_objects.location_id, crypto_objects.file_type, crypto_objects.pointing_location_id, \
\n		crypto_objects.pointing_id, crypto_objects.name, crypto_objects.filesize, crypto_objects.fk_owner, \
\n		crypto_objects.insert_date, crypto_objects.fk_group, crypto_objects.corresponding_id, crypto_objects.visible, \
\n		crypto_objects.present, crypto_objects.transaction_id, crypto_objects.creator, crypto_objects.for_grant, crypto_objects.hash_value, \
\n		crypto_objects.expiration_date, crypto_objects.for_timestamping \
\n		%s /*metadane dodatkowe*/ \
\n FROM crypto_objects \
\nWHERE \
\n	fk_group=%s /* parametr grupa wyszukujaca dokumenty */ \
\n	%s /*poziomy i kategorie bezpieczenstwa*/ \
\n	%s /*kryterium wyszukiwania okreslone przez uzytkownika*/ \
\n	AND location_id=%s /*parametr*/ \
\n	AND (transaction_id='' OR transaction_id IS NULL) \
\n	AND visible=1 \
\n	AND present=1 \
\n	AND (file_type <> 'dir') \
\n	AND pointing_location_id=%s /*parametr*/ \
\n	AND pointing_id NOT IN \
\n	( \
\n	 	/* (#2) */ \
\n		SELECT DISTINCT pointing_id FROM crypto_objects \
\n		WHERE \
\n			location_id=%s /*parametr*/ \
\n			AND (transaction_id='' OR transaction_id IS NULL) \
\n			AND visible=1 \
\n			AND present=1 \
\n			AND id IN \
\n			( \
\n				/* (#1) */ \
\n				SELECT DISTINCT fk_objects FROM action_register \
\n				WHERE \
\n					location_id=%s /*parametr*/ \
\n					AND ( %s ) /* parametr fk_actions=value [ OR fk_actions=value ...] */ \
\n					AND ( %s ) /* parametr fk_users_class=value [ OR fk_users_class=value ...]  klasa wykonujaca akcje */ \
\n			) \
\n	) \
\n%s /*sortowanie i stronicowanie*/ ";


	if(hDB==NULL)
		{ return -1; }
	if(datagramPtr==NULL)
		{ return -2; }
	if(resultQuery == NULL)
		{ return -3; }
	if(*resultQuery != NULL)
		{ return -4; }



	retVal=bmdStringArray_Create(&actionIdsArray);
	if(retVal != 0)
		{ return -11; }
	retVal=bmdStringArray_Create(&classIdsArray);
	if(retVal != 0)
	{
		bmdStringArray_DestroyList(&actionIdsArray);
		return -12;
	}


	/** TODO */
	groupId=strdup(req->ua->user_chosen_group->buf);
	if(groupId == NULL)
	{
		bmdStringArray_DestroyList(&actionIdsArray);
		bmdStringArray_DestroyList(&classIdsArray);
		return -13;
	}
	/*
	for(iter=0; iter<datagramPtr->no_of_sysMetaData; iter++)
	{
		//grupa wyszukujaca
		if(strcmp(datagramPtr->sysMetaData[iter]->OIDTableBuf, OID_SYS_METADATA_CERT_GROUP_ID) == 0)
		{
			if(groupId != NULL)
			{
				bmdStringArray_DestroyList(&actionIdsArray);
				bmdStringArray_DestroyList(&classIdsArray);
				return -14;
			}
			groupId=(char*)calloc(datagramPtr->sysMetaData[iter]->AnySize+1, sizeof(char));
			if(groupId == NULL)
			{
				bmdStringArray_DestroyList(&actionIdsArray);
				bmdStringArray_DestroyList(&classIdsArray);
				return -15;
			}
			memcpy(groupId, datagramPtr->sysMetaData[iter]->AnyBuf, (datagramPtr->sysMetaData[iter]->AnySize)*sizeof(char));
		}
		
	}
	*/


	/* kryteria wyszukiwania zgodne z Advanced Search */
	retVal=GenerateMetadataConditions(hDB, datagramPtr, &metadataCondition);
	if(retVal < 0)
	{
		bmdStringArray_DestroyList(&actionIdsArray);
		bmdStringArray_DestroyList(&classIdsArray);
		free(groupId);
		return -41;
	}
	

	/* nazwy kolumn metadanych dodatkowych */
	//SELECT id, type, name, sql_cast_string, default_value FROM add_metadata_types;
	retVal=getColumnWithCondition(_GLOBAL_AddMetadataTypesDictionary, 0, NULL, 1, &addMetadataOidsCount, &addMetadataOids);
	if(retVal < 0)
	{
		bmdStringArray_DestroyList(&actionIdsArray);
		bmdStringArray_DestroyList(&classIdsArray);
		free(groupId);
		free(metadataCondition);
		return -14;
	}

	for(iter=0; iter<addMetadataOidsCount; iter++)
	{

		retVal=bmd_str_replace(&(addMetadataOids[iter]), ".", "_");
		if(retVal < 0)
		{
			bmdStringArray_DestroyList(&actionIdsArray);
			bmdStringArray_DestroyList(&classIdsArray);
			free(groupId);
			free(metadataCondition);
			for(iter=0; iter<addMetadataOidsCount; iter++)
				{ free(addMetadataOids[iter]); }
			free(addMetadataOids); addMetadataOids=NULL;
			return -15;
		}

		if(addMetadataColumns == NULL)
		{
			retVal=asprintf(&addMetadataColumns, ", crypto_objects.amv_%s", addMetadataOids[iter]);
			if(retVal < 0)
			{
				bmdStringArray_DestroyList(&actionIdsArray);
				bmdStringArray_DestroyList(&classIdsArray);
				free(groupId);
				free(metadataCondition);
				for(iter=0; iter<addMetadataOidsCount; iter++)
					{ free(addMetadataOids[iter]); }
				free(addMetadataOids); addMetadataOids=NULL;
				return -16;
			}
		}
		else
		{
			retVal=asprintf(&tempAddMetadataColumns, "%s, crypto_objects.amv_%s ", addMetadataColumns, addMetadataOids[iter]);
			if(retVal < 0)
			{
				bmdStringArray_DestroyList(&actionIdsArray);
				bmdStringArray_DestroyList(&classIdsArray);
				free(groupId);
				free(metadataCondition);
				free(addMetadataColumns);
				for(iter=0; iter<addMetadataOidsCount; iter++)
					{ free(addMetadataOids[iter]); }
				free(addMetadataOids); addMetadataOids=NULL;
				return -17;
			}
			
			free(addMetadataColumns);
			addMetadataColumns=tempAddMetadataColumns;
			tempAddMetadataColumns=NULL;
		}
		
	}

	for(iter=0; iter<addMetadataOidsCount; iter++)
		{ free(addMetadataOids[iter]); }
	free(addMetadataOids); addMetadataOids=NULL;


	/* numery akcji i klasy uzytkownikow */
	for(iter=0; iter<datagramPtr->no_of_actionMetaData; iter++)
	{
		if(strcmp(datagramPtr->actionMetaData[iter]->OIDTableBuf, OID_ACTION_METADATA_USER_CLASS) == 0)
		{
			tempString=(char*)calloc(datagramPtr->actionMetaData[iter]->AnySize+1, sizeof(char));
			if(tempString == NULL)
			{
				free(groupId);
				free(metadataCondition);
				bmdStringArray_DestroyList(&actionIdsArray);
				bmdStringArray_DestroyList(&classIdsArray);
				free(addMetadataColumns);
				return -18;
			}
			memcpy(tempString, datagramPtr->actionMetaData[iter]->AnyBuf, (datagramPtr->actionMetaData[iter]->AnySize)*sizeof(char));
			retVal=bmdStringArray_AddItem(classIdsArray, tempString);
			free(tempString); tempString=NULL;
			if(retVal != 0)
			{
				free(groupId);
				free(metadataCondition);
				bmdStringArray_DestroyList(&actionIdsArray);
				bmdStringArray_DestroyList(&classIdsArray);
				free(addMetadataColumns);
				return -19;
			}
		}
		else if(strcmp(datagramPtr->actionMetaData[iter]->OIDTableBuf, OID_ACTION_METADATA_ACTION) == 0)
		{
			tempString=(char*)calloc(datagramPtr->actionMetaData[iter]->AnySize+1, sizeof(char));
			if(tempString == NULL)
			{
				free(groupId);
				free(metadataCondition);
				bmdStringArray_DestroyList(&actionIdsArray);
				bmdStringArray_DestroyList(&classIdsArray);
				free(addMetadataColumns);
				return -20;
			}
			memcpy(tempString, datagramPtr->actionMetaData[iter]->AnyBuf, (datagramPtr->actionMetaData[iter]->AnySize)*sizeof(char));
			retVal=bmdStringArray_AddItem(actionIdsArray, tempString);
			free(tempString); tempString=NULL;
			if(retVal != 0)
			{
				free(groupId);
				free(metadataCondition);
				bmdStringArray_DestroyList(&actionIdsArray);
				bmdStringArray_DestroyList(&classIdsArray);
				free(addMetadataColumns);
				return -21;
			}
		}
	}
	

	bmdStringArray_GetItemsCount(classIdsArray, &tempLong);
	if(tempLong != 1)
	{
		free(groupId);
		free(metadataCondition);
		bmdStringArray_DestroyList(&actionIdsArray);
		bmdStringArray_DestroyList(&classIdsArray);
		free(addMetadataColumns);
		return ACTION_METADATA_USER_CLASS_NOT_SPECIFIED;
	}

	retVal=MergeORConditions(actionIdsArray, "fk_actions", &actionCondition);
	bmdStringArray_DestroyList(&actionIdsArray);
	if(retVal != 0)
	{
		free(groupId);
		free(metadataCondition);
		bmdStringArray_DestroyList(&classIdsArray);
		free(addMetadataColumns);
		return ACTION_METADATA_ACTION_NOT_SPECIFIED;
	}
	retVal=MergeORConditions(classIdsArray, "fk_users_class", &classCondition);
	bmdStringArray_DestroyList(&classIdsArray);
	if(retVal != 0)
	{
		free(actionCondition);
		free(groupId);
		free(metadataCondition);
		free(addMetadataColumns);
		return -24;
	}

	/* kategorie i poziomy bezpieczenstwa */
	if (_GLOBAL_bmd_configuration->enable_clearance_check==1)
	{
		
		/*to na podstawie select_main*/
		tempClearanceCondition=strdup("(crypto_objects.fk_sec_categories=1 AND crypto_objects.fk_sec_levels=0)");
		if(tempClearanceCondition == NULL)
		{
			free(actionCondition);
			free(classCondition);
			free(groupId);
			free(metadataCondition);
			free(addMetadataColumns);
			return -25;
		}
		for(iter=0; iter<req->ua->security->size; iter+=2)
		{
			if ((req->ua->security->gbufs[iter]!=NULL) && (req->ua->security->gbufs[iter+1]!=NULL))
			{
				//SELECT id, type, name, priority FROM sec_levels
				retVal=getElementWithCondition(_GLOBAL_SecurityLevelsDictionary, 0, req->ua->security->gbufs[iter+1]->buf, 3, &securityLevelPriority);//, LIBBMDSQL_DICT_SEC_LEVEL_VALUE_NOT_FOUND);
				if(retVal < 0)
				{
					free(actionCondition);
					free(classCondition);
					free(groupId);
					free(metadataCondition);
					free(addMetadataColumns);
					free(tempClearanceCondition);
					return -26;
				}


				retVal=asprintf(&clearanceCondition,"%s OR (crypto_objects.fk_sec_categories=%s AND crypto_objects.fk_sec_levels<=%s)",
							tempClearanceCondition,
							req->ua->security->gbufs[iter]->buf,
							securityLevelPriority);

				free(securityLevelPriority); securityLevelPriority=NULL;
				free(tempClearanceCondition); tempClearanceCondition=NULL;
				if(retVal < 0)
				{
					free(actionCondition);
					free(classCondition);
					free(groupId);
					free(metadataCondition);
					free(addMetadataColumns);
					return -27;
				}
				
				tempClearanceCondition=clearanceCondition;
				clearanceCondition=NULL;
			}
		}

		retVal=asprintf(&clearanceCondition,"AND ( %s )", tempClearanceCondition);
		free(tempClearanceCondition); tempClearanceCondition=NULL;
		if(retVal < 0)
		{
			free(actionCondition);
			free(classCondition);
			free(groupId);
			free(metadataCondition);
			free(addMetadataColumns);
			return -28;
		}
		
	}

	/*sortowanie i stronicowanie*/
	retVal=bmd_db_generate_order_clause(datagramPtr, req, &orderClause);
	retVal=bmd_db_paginate_sql_query("" /*sqlbegin*/, orderClause, datagramPtr, 0/*ignorowane*/, 100/*ignorowane*/, &paginatingStatement, NULL);
	/*nie ma potrzeby sprawdzac rezultatu tej operacji - jesli sie nie powiedzie, to nie bedzie sortowania/stronicowania*/
	free(orderClause); orderClause=NULL;

	retVal=asprintf(&result, SQLtemplate, addMetadataColumns, groupId,
			(clearanceCondition!=NULL ? clearanceCondition : ""),
			(metadataCondition != NULL ? metadataCondition : ""),
			location_id, location_id, location_id, location_id, actionCondition, classCondition,
			(paginatingStatement!=NULL ? paginatingStatement : ";") );
	
	free(groupId); groupId=NULL;
	free(metadataCondition); metadataCondition=NULL;
	free(addMetadataColumns); addMetadataColumns=NULL;
	free(actionCondition); actionCondition=NULL;
	free(classCondition); classCondition=NULL;
	free(clearanceCondition); clearanceCondition=NULL;
	free(paginatingStatement); paginatingStatement=NULL;
	if(retVal < 0)
	{
		return -29;
	}
	
	*resultQuery=result;
	result=NULL;
	return 0;
}






long JS_bmdDatagramSet_to_select_unreceived(	void *hDB,
					bmdDatagramSet_t *bmdJSRequestSet,
					bmdDatagramSet_t **bmdJSResponseSet,
					server_request_data_t *req)
{
long iter=0;
long retVal=0;
char* SQLQuery=NULL;
void* queryResult=NULL;
bmdDatagram_t* datagramPtr=NULL;




	if (hDB == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (bmdJSRequestSet == NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (bmdJSResponseSet == NULL)	{	BMD_FOK(BMD_ERR_PARAM3);	}
	if (*bmdJSResponseSet != NULL)	{	BMD_FOK(BMD_ERR_PARAM3);	}
	if (req == NULL)		{	BMD_FOK(BMD_ERR_PARAM4);	}

	if(bmdJSRequestSet->bmdDatagramSetTable[0] == NULL)
	{
		BMD_FOK(BMD_ERR_PARAM2);
	}
	
	if (bmdJSRequestSet->bmdDatagramSetTable[0]->datagramStatus<BMD_OK)
	{
		BMD_FOK(PR2_bmdDatagramSet_fill_with_datagrams(1, bmdJSResponseSet));
		PR_bmdDatagram_PrepareResponse(	&((*bmdJSResponseSet)->bmdDatagramSetTable[0]),
						bmdJSRequestSet->bmdDatagramSetTable[0]->datagramType+100,
						bmdJSRequestSet->bmdDatagramSetTable[0]->datagramStatus);
		return BMD_OK;
	}


	datagramPtr=bmdJSRequestSet->bmdDatagramSetTable[0];


	BMD_FOK(bmdUnreceived_Generate_SQL(hDB, datagramPtr, req, &SQLQuery));
	
	retVal=bmd_db_execute_sql(hDB, SQLQuery,NULL,NULL, &queryResult);
	free(SQLQuery); SQLQuery=NULL;
	BMD_FOK(retVal);

	retVal=_PR2_create_bmdDatagram_set(bmdJSResponseSet);
	if(retVal < 0)
	{
		bmd_db_result_free(&queryResult);
		BMD_FOK(retVal);
	}

	retVal = FillbmdDatagramSetFromSelectResult(hDB, queryResult, *bmdJSResponseSet, req);
	bmd_db_result_free(&queryResult);
	if(retVal<BMD_OK && retVal!=BMD_ERR_NODATA)
	{
		BMD_FOK(retVal); /* mozliwe wartosci BMD_ERR_OP_FAILED, BMD_ERR_NODATA */
	}


	/*rejestrowanie w action_register*/
	if (_GLOBAL_bmd_configuration->messaging_plugin_conf.bmd_messaging_plugin_register_action)
	{
		for(iter=0; iter<(*bmdJSResponseSet)->bmdDatagramSetSize; iter++)
		{
			retVal=_GLOBAL_bmd_configuration->messaging_plugin_conf.bmd_messaging_plugin_register_action(hDB, bmdJSRequestSet->bmdDatagramSetTable[0], (*bmdJSResponseSet)->bmdDatagramSetTable[iter], req);
			if(retVal<BMD_OK)
			{
				(*bmdJSResponseSet)->bmdDatagramSetTable[iter]->datagramStatus = retVal;
			}
		}
	}

	return BMD_OK;
}


long JS_bmdDatagramSet_to_GetAccessObjects(	void *hDB,
					bmdDatagramSet_t *bmdJSRequestSet,
					bmdDatagramSet_t **bmdJSResponseSet,
					server_request_data_t *req)
{
long iter=0;
long retVal=0;
void* queryResult=NULL;
bmdDatagram_t* datagramPtr=NULL;
char* SQLQuery=NULL;

char* SQLTemplate="SELECT id, fk_group, file_type, fk_owner, creator \n \
FROM crypto_objects \n \
WHERE \n \
	pointing_id=%s \n \
	AND pointing_location_id=%s \n \
	AND visible=%s \n \
	AND (transaction_id='' OR transaction_id IS NULL) \n \
	AND file_type <> 'dir' \n \
	AND present=1 ;";

db_row_strings_t *rowstruct	= NULL;
long fetched_counter=0;
bmdDatagram_t* datagram=NULL;
long invisible=0;
char *originalDocumentId=NULL;


	if (hDB == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (bmdJSRequestSet == NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (bmdJSResponseSet == NULL)	{	BMD_FOK(BMD_ERR_PARAM3);	}
	if (*bmdJSResponseSet != NULL)	{	BMD_FOK(BMD_ERR_PARAM3);	}

	if(bmdJSRequestSet->bmdDatagramSetSize != 1)
		{ BMD_FOK(BMD_ERR_PARAM2); }

	datagramPtr=bmdJSRequestSet->bmdDatagramSetTable[0];

	if(datagramPtr == NULL)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(datagramPtr->protocolDataFileId == NULL)
		{ BMD_FOK(BMD_ERR_PARAM2); }


	if (datagramPtr->datagramStatus<BMD_OK)
	{
		BMD_FOK(PR2_bmdDatagramSet_fill_with_datagrams(1, bmdJSResponseSet));
		PR_bmdDatagram_PrepareResponse(	&((*bmdJSResponseSet)->bmdDatagramSetTable[0]),
						BMD_DATAGRAM_TYPE_GET_ACCESS_OBJECTS_RESPONSE,
						datagramPtr->datagramStatus);
		return BMD_OK;
	}

	/*sprawdzenie czy crypto_object o danym id istnieje i czy uzytkownik moze sie do niego odwolywac*/
	/*
	   jesli id w datagramie zadania jest identyfikatorem linka,
	   to originalDocumentId bedzie identyfikatorem dokumentu wskazywnego
	*/
	retVal=VerifyPermissionsForCryptoObject(hDB, req, datagramPtr->protocolDataFileId->buf,
				CONSIDER_CREATOR_OR_OWNER_OR_GROUPS,
				NULL, NULL, &originalDocumentId, NULL, NULL, NULL, NULL);
	if(retVal < 0)
	{
		BMD_FOK(retVal);
	}
	

	for(iter=0; iter<datagramPtr->no_of_sysMetaData; iter++)
	{
		if(datagramPtr->sysMetaData[iter] != NULL && datagramPtr->sysMetaData[iter]->OIDTableBuf != NULL)
		{
			if(strcmp(datagramPtr->sysMetaData[iter]->OIDTableBuf, OID_SYS_METADATA_CRYPTO_OBJECTS_VISIBLE) == 0)
			{
				if(datagramPtr->sysMetaData[iter]->AnyBuf != NULL)
				{
					if(strcmp(datagramPtr->sysMetaData[iter]->AnyBuf, "0") == 0)
					{
						invisible=1;
					}
				}
				break;
			}
		}
	}
// 	if(datagramPtr->protocolData != NULL)
// 	{
// 		if(datagramPtr->protocolData->buf != NULL)
// 		{
// 			if(strcmp(datagramPtr->protocolData->buf, "INVISIBLE") == 0)
// 			{
// 				invisible=1;
// 			}
// 		}
// 	}

	asprintf(&SQLQuery, SQLTemplate, originalDocumentId, _GLOBAL_bmd_configuration->location_id, ( invisible == 1 ? "0" : "1") );
	free(originalDocumentId); originalDocumentId=NULL;

	retVal=bmd_db_execute_sql(hDB, SQLQuery,NULL,NULL, &queryResult);
	free(SQLQuery); SQLQuery=NULL;
	BMD_FOK(retVal);

	while(retVal >= 0)
	{
		retVal = bmd_db_result_get_row(hDB, queryResult, 0, 0, &rowstruct, FETCH_NEXT, &fetched_counter);
		if(retVal==BMD_ERR_NODATA)
			{ break; }
		else if (retVal<BMD_OK)
		{
			bmd_datagramset_free(bmdJSResponseSet);
			bmd_db_result_free(&queryResult);
			BMD_FOK(retVal);
		}

		retVal=bmd_datagram_create(BMD_DATAGRAM_TYPE_GET_ACCESS_OBJECTS_RESPONSE,&datagram);
		if(retVal < 0)
		{
			bmd_datagramset_free(bmdJSResponseSet);
			bmd_db_result_free(&queryResult);
			BMD_FOK(retVal);
		}

		retVal=set_gen_buf2(rowstruct->colvals[0], (long) strlen(rowstruct->colvals[0]), &(datagram->protocolDataFileId));
		if(retVal < 0)
		{
			bmd_datagram_free(&datagram);
			bmd_datagramset_free(bmdJSResponseSet);
			bmd_db_result_free(&queryResult);
			BMD_FOK(retVal);
		}

		retVal=bmd_datagram_add_metadata_char(OID_SYS_METADATA_CRYPTO_OBJECTS_ID, rowstruct->colvals[0], &datagram);
		if(retVal < 0)
		{
			bmd_datagram_free(&datagram);
			bmd_datagramset_free(bmdJSResponseSet);
			bmd_db_result_free(&queryResult);
			BMD_FOK(retVal);
		}
		retVal=bmd_datagram_add_metadata_char(OID_SYS_METADATA_CRYPTO_OBJECTS_GROUP, rowstruct->colvals[1], &datagram);
		if(retVal < 0)
		{
			bmd_datagram_free(&datagram);
			bmd_datagramset_free(bmdJSResponseSet);
			bmd_db_result_free(&queryResult);
			BMD_FOK(retVal);
		}
		retVal=bmd_datagram_add_metadata_char(OID_SYS_METADATA_CRYPTO_OBJECTS_FILE_TYPE, rowstruct->colvals[2], &datagram);
		if(retVal < 0)
		{
			bmd_datagram_free(&datagram);
			bmd_datagramset_free(bmdJSResponseSet);
			bmd_db_result_free(&queryResult);
			BMD_FOK(retVal);
		}
		retVal=bmd_datagram_add_metadata_char(OID_SYS_METADATA_CRYPTO_OBJECTS_OWNER, rowstruct->colvals[3], &datagram);
		if(retVal < 0)
		{
			bmd_datagram_free(&datagram);
			bmd_datagramset_free(bmdJSResponseSet);
			bmd_db_result_free(&queryResult);
			BMD_FOK(retVal);
		}
		retVal=bmd_datagram_add_metadata_char(OID_SYS_METADATA_CRYPTO_OBJECTS_CREATOR, rowstruct->colvals[4], &datagram);
		if(retVal < 0)
		{
			bmd_datagram_free(&datagram);
			bmd_datagramset_free(bmdJSResponseSet);
			bmd_db_result_free(&queryResult);
			BMD_FOK(retVal);
		}


		bmd_db_row_struct_free(&rowstruct);
		retVal=bmd_datagram_add_to_set(datagram,bmdJSResponseSet);
		if(retVal < 0)
		{
			bmd_datagram_free(&datagram);
			bmd_datagramset_free(bmdJSResponseSet);
			bmd_db_result_free(&queryResult);
			BMD_FOK(retVal);
		}
		datagram = NULL;
	}
	bmd_db_result_free(&queryResult);

	if(fetched_counter == 0)
	{
		BMD_FOK(bmd_datagram_create(BMD_DATAGRAM_TYPE_GET_ACCESS_OBJECTS_RESPONSE, &datagram));
		datagram->datagramStatus = BMD_ERR_NODATA;
		retVal=bmd_datagram_add_to_set(datagram,bmdJSResponseSet);
		if(retVal < 0)
		{
			bmd_datagram_free(&datagram);
			BMD_FOK(retVal);
		}

		return BMD_OK;
	}


	/*rejestrowanie w action_register*/
	if (_GLOBAL_bmd_configuration->messaging_plugin_conf.bmd_messaging_plugin_register_action)
	{
		/*na jeden datagram zadania moze byc wiele datagramow odpowiedzi, dlatego rejestrowany tylko pierwszy*/
		retVal=_GLOBAL_bmd_configuration->messaging_plugin_conf.bmd_messaging_plugin_register_action(hDB, datagramPtr, (*bmdJSResponseSet)->bmdDatagramSetTable[0], req);
		if(retVal<BMD_OK)
		{
			(*bmdJSResponseSet)->bmdDatagramSetTable[0]->datagramStatus = retVal;
		}
	}

	return BMD_OK;
}
