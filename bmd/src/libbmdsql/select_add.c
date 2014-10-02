#include <bmd/libbmdsql/select.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>
#include <time.h>
#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdglobals/libbmdglobals.h>

#ifdef WIN32
#pragma warning(disable:4100)
#endif

long DetermineSearchConditionArguments(	MetaDataBuf_t *metaData,
					long defaultrecRelation,
					char **recColumnName,
					char **recLogical,
					char **recRelation,
					char **recOpenBracets,
					char **recCloseBrackets,
					char **recStartPerc,
					char **recStopPerc,
					char **metadataCast,
					long *add_met_count,
					long *sys_met_count,
					long *act_met_count,
					long *pki_met_count)
{
long metadataType		= 0;
long err			= 0;
long num_open_brackets		= 0;
long num_close_brackets		= 0;
long ownerId_sign		= 0;
long i				= 0;
char *oidString			= NULL;
char *oidStringSuffix		= NULL;
char *tmp			= NULL;
char *metadataId		= NULL;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (metaData==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (recColumnName==NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if ((*recColumnName)!=NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (recLogical==NULL)		{	BMD_FOK(BMD_ERR_PARAM3);	}
	if ((*recLogical)!=NULL)	{	BMD_FOK(BMD_ERR_PARAM3);	}
	if (recOpenBracets==NULL)	{	BMD_FOK(BMD_ERR_PARAM4);	}
	if ((*recOpenBracets)!=NULL)	{	BMD_FOK(BMD_ERR_PARAM4);	}
	if (recCloseBrackets==NULL)	{	BMD_FOK(BMD_ERR_PARAM5);	}
	if ((*recCloseBrackets)!=NULL)	{	BMD_FOK(BMD_ERR_PARAM5);	}
	if (recStartPerc==NULL)		{	BMD_FOK(BMD_ERR_PARAM6);	}
	if ((*recStartPerc)!=NULL)	{	BMD_FOK(BMD_ERR_PARAM6);	}
	if (recStopPerc==NULL)		{	BMD_FOK(BMD_ERR_PARAM7);	}
	if ((*recStopPerc)!=NULL)	{	BMD_FOK(BMD_ERR_PARAM7);	}
	if(metadataCast == NULL)
		{ BMD_FOK(BMD_ERR_PARAM9); }
	if(*metadataCast != NULL)
		{ BMD_FOK(BMD_ERR_PARAM9); }
	

	asprintf(&oidString, "%s", metaData->OIDTableBuf);
	if (oidString==NULL)		{	BMD_FOK(NO_MEMORY);	}

	/*****************************************************************/
	/* sprawdzenie poprawnosci oid'a metadanej - czy jest w slowniku */
	/*****************************************************************/
	PRINT_TEST("determining search condition for metadata oid: %s\n",oidString);
	metadataType=determine_oid_type_str(oidString);
	if ((metadataType==UNDEFINED_METADATA) || (metadataType<BMD_OK))	{	BMD_FOK(-666);	}

	switch(metadataType)
	{
		case ACTION_METADATA	:	;err=DatabaseDictionaryTypeToID(oidString, "action_metadata", &metadataId);	break;
		case SYS_METADATA	:	;err=DatabaseDictionaryTypeToID(oidString, "sys_metadata", &metadataId);	break;
		case ADDITIONAL_METADATA:	;err=DatabaseDictionaryTypeToID(oidString, "add_metadata", &metadataId);	break;
		case PKI_METADATA	:	;err=DatabaseDictionaryTypeToID(oidString, "pki_metadata", &metadataId);	break;
	}

	if(err < 0 && err != LIBBMDSQL_DICT_NO_ELEMENTS_IN_DICTIONARY && err != LIBBMDSQL_DICT_UNKNOWN_SYSMETADATA)			{	BMD_FOK(err);	}

	/**********************************************************************************/
	/* jesli oid'a nie ma w slowniku to moze trzeba z niego wyciagnac prefix i suffix */
	/**********************************************************************************/
	if ( (err==LIBBMDSQL_DICT_NO_ELEMENTS_IN_DICTIONARY) || (err==LIBBMDSQL_DICT_UNKNOWN_SYSMETADATA) )
	{

		free0(oidString);
		BMD_FOK(SplitOidStringToPrefixAndSufix(metaData->OIDTableBuf, &oidString, &oidStringSuffix));

		if (DatabaseDictionaryTypeToID(oidString, "add_metadata", &metadataId)<BMD_OK)
		{
			if (DatabaseDictionaryTypeToID(oidString, "sys_metadata", &metadataId)<BMD_OK)
			{
				if (DatabaseDictionaryTypeToID(oidString, "action_metadata", &metadataId)<BMD_OK)
				{
					BMD_FOK_NP(DatabaseDictionaryTypeToID(oidString, "pki_metadata", &metadataId));
					metadataType=PKI_METADATA;
				}
				else
				{
					metadataType=ACTION_METADATA;
				}
			}
			else
			{
				metadataType=SYS_METADATA;
			}
		}
		else
		{
			metadataType=ADDITIONAL_METADATA;
		}

		if (DatabaseDictionaryTypeToSQLCastString(oidString, "add_metadata", metadataCast)<BMD_OK)
		{
			if (DatabaseDictionaryTypeToSQLCastString(oidString, "sys_metadata", metadataCast)<BMD_OK)
			{
				if (DatabaseDictionaryTypeToSQLCastString(oidString, "action_metadata", metadataCast)<BMD_OK)
				{
					BMD_FOK_NP(DatabaseDictionaryTypeToSQLCastString(oidString, "pki_metadata", metadataCast));
				}
			}
		}

		BMD_FOK(OIDsufixToOperatorType(oidStringSuffix, recRelation));
	}
	else
	{

 		if (DatabaseDictionaryTypeToSQLCastString(oidString, "add_metadata", metadataCast)<BMD_OK)
 		{

 			if (DatabaseDictionaryTypeToSQLCastString(oidString, "sys_metadata", metadataCast)<BMD_OK)
			{

				if (DatabaseDictionaryTypeToSQLCastString(oidString, "action_metadata", metadataCast)<BMD_OK)
				{

					BMD_FOK_NP(DatabaseDictionaryTypeToSQLCastString(oidString, "pki_metadata", metadataCast));
				}
			}
 		}
	}

	switch(metadataType)
	{
		case ACTION_METADATA	:	(*act_met_count)++;	break;
		case SYS_METADATA	:	(*sys_met_count)++;	break;
		case ADDITIONAL_METADATA:	(*add_met_count)++;	break;
		case PKI_METADATA	:	(*pki_met_count)++;	break;
	}


	/**********************************************************/
	/* określenie, czy szukamy po twierdzeniu, czy przeczeniu */
	/**********************************************************/
	if (metaData->ownerId<0)
	{
		metaData->ownerId *= -1;
		ownerId_sign = -1;
	}
	else
	{
		ownerId_sign=1;
	}

	/***********************************/
	/* określenie operatora porównania */
	/***********************************/
	if (strcmp(metaData->AnyBuf, "NULL")==0)
	{
		free((*recRelation)); (*recRelation)=NULL;
		asprintf(recRelation, "%s", ownerId_sign>0?"IS":"IS NOT");
		if ((*recRelation)==NULL)	{	BMD_FOK(NO_MEMORY);	}
	}
	else
	{
		if (*recRelation==NULL) /* operator dopasowania nie zostal okreslony - wiec nie ma zakresow */
		{

			/* dla timestampow, integerow i floatow mozna zastosowac znaki = lub != */
			if (*metadataCast != NULL && (strcmp(*metadataCast, "integer")==0 || strcmp(*metadataCast, "float")==0) )
			{

				free((*recRelation)); (*recRelation)=NULL;
				asprintf(recRelation, "%s", ownerId_sign>0?"=":"!=");
				if ((*recRelation)==NULL)	{	BMD_FOK(NO_MEMORY);	}
			}
			else if (*metadataCast != NULL && strcmp(*metadataCast, "timestamp")==0)
			{

				free((*recRelation)); (*recRelation)=NULL;
				asprintf(recRelation, "%s", ownerId_sign>0?"LIKE":"NOT LIKE");
				if ((*recRelation)==NULL)	{	BMD_FOK(NO_MEMORY);	}
			}
			/* operator dopasowania został okreslony dla konkretnej metadanej */
			else if (metaData->myId == BMD_REQ_DB_SELECT_ADVANCED_LIKE_QUERY)
			{

				free((*recRelation)); (*recRelation)=NULL;
				asprintf(recRelation, "%s", ownerId_sign>0?"LIKE":"NOT LIKE");
				if ((*recRelation)==NULL)	{	BMD_FOK(NO_MEMORY);	}
				metaData->myId -= BMD_REQ_DB_SELECT_ADVANCED_LIKE_QUERY;

			}
			else if (metaData->myId == BMD_REQ_DB_SELECT_ADVANCED_ILIKE_QUERY)
			{

				free((*recRelation)); (*recRelation)=NULL;
				asprintf(recRelation, "%s", ownerId_sign>0?"ILIKE":"NOT ILIKE");
				if ((*recRelation)==NULL)	{	BMD_FOK(NO_MEMORY);	}
				metaData->myId -= BMD_REQ_DB_SELECT_ADVANCED_ILIKE_QUERY;

			}
			/* operator dopasowania nie został okreslony dla konkretnej metadanej  - bierzemy pod uwage defaultowy*/
			else
			{

				if (	(defaultrecRelation & BMD_REQ_DB_SELECT_ILIKE_QUERY) ||
					(defaultrecRelation & BMD_REQ_DB_SELECT_EXACT_ILIKE_QUERY))
				{

					free((*recRelation)); (*recRelation)=NULL;
					asprintf(recRelation, "%s", ownerId_sign>0?"ILIKE":"NOT ILIKE");
					if ((*recRelation)==NULL)	{	BMD_FOK(NO_MEMORY);	}

				}
				else if (	(defaultrecRelation & BMD_REQ_DB_SELECT_LIKE_QUERY) ||
						(defaultrecRelation & BMD_REQ_DB_SELECT_EQUAL_QUERY))
				{

					free((*recRelation)); (*recRelation)=NULL;
					asprintf(recRelation, "%s", ownerId_sign>0?"LIKE":"NOT LIKE");
					if ((*recRelation)==NULL)	{	BMD_FOK(NO_MEMORY);	}

				}
				else
				{
					if (*metadataCast != NULL && (strcmp(*metadataCast, "varchar")==0 || strcmp(*metadataCast, "timestamp")==0) )
					{
						free((*recRelation)); (*recRelation)=NULL;
						asprintf(recRelation, "%s", ownerId_sign>0?"LIKE":"NOT LIKE");
						if ((*recRelation)==NULL)	{	BMD_FOK(NO_MEMORY);	}
					}
					else
					{
						free((*recRelation)); (*recRelation)=NULL;
						asprintf(recRelation, "%s", ownerId_sign>0?"=":"!=");
						if ((*recRelation)==NULL)	{	BMD_FOK(NO_MEMORY);	}
					}
				}
			}

		}
		else if (*metadataCast != NULL && strcmp(*metadataCast, "varchar")==0)
		{

			free((*recRelation)); (*recRelation)=NULL;
			asprintf(recRelation, "%s", ownerId_sign>0?"LIKE":"NOT LIKE");
			if ((*recRelation)==NULL)	{	BMD_FOK(NO_MEMORY);	}
		}
		else if(*metadataCast != NULL && strcmp(*metadataCast, "float")==0)
		{
			if(strcmp(*recRelation, "<")==0)
			{
				free(*recRelation);
				*recRelation = strdup("<=");
				if(*recRelation == NULL)
					{ BMD_FOK(NO_MEMORY); }
			}
			else if(strcmp(*recRelation, ">")==0)
			{
				free(*recRelation);
				*recRelation = strdup(">=");
				if(*recRelation == NULL)
					{ BMD_FOK(NO_MEMORY); }
			}
		}
		else if(*metadataCast != NULL && strcmp(*metadataCast, "integer")==0)
		{
			if(strcmp(*recRelation, "<")==0)
			{
				free(*recRelation);
				*recRelation = strdup("<=");
				if(*recRelation == NULL)
					{ BMD_FOK(NO_MEMORY); }
			}
			else if(strcmp(*recRelation, ">")==0)
			{
				free(*recRelation);
				*recRelation = strdup(">=");
				if(*recRelation == NULL)
					{ BMD_FOK(NO_MEMORY); }
			}
		}
		else if(*metadataCast != NULL && strcmp(*metadataCast, "timestamp")==0)
		{
			// < (mniejsze niz) pozostaje bez zmian
			if(strcmp(*recRelation, ">")==0)
			{
				free(*recRelation);
				*recRelation = strdup(">=");
				if(*recRelation == NULL)
					{ BMD_FOK(NO_MEMORY); }
			}
		}
		
		if(*recRelation != NULL && ownerId_sign<0) // przeczenie
		{
			if(strcmp(*recRelation, "<")==0)
			{
				free(*recRelation);
				*recRelation = strdup(">=");
				if(*recRelation == NULL)
					{ BMD_FOK(NO_MEMORY); }
			}
			else if(strcmp(*recRelation, "<=")==0)
			{

				free(*recRelation);
				*recRelation = strdup(">");
				if(*recRelation == NULL)
					{ BMD_FOK(NO_MEMORY); }
			}
			else if(strcmp(*recRelation, ">")==0)
			{
				free(*recRelation);
				*recRelation = strdup("<=");
				if(*recRelation == NULL)
					{ BMD_FOK(NO_MEMORY); }
			}
			else if(strcmp(*recRelation, ">=")==0)
			{
				free(*recRelation);
				*recRelation = strdup("<");
				if(*recRelation == NULL)
					{ BMD_FOK(NO_MEMORY); }
			}
		}
	}
PRINT_TEST(">>>>> PKL <<<<<<<<<< oidString: %s\n", oidString)
	/*************************************************/
	/* okreslenie kolumny, z jakiej dotyczy metadana */
	/************************************************/
	asprintf(recColumnName, "%s", determine_oid_column_name(oidString));
	if (recColumnName==NULL)	{	BMD_FOK(NO_MEMORY);	}

	/******************************************************************/
	/* wydobycie ilosci nawiasow otwierajacych i operatora logicznego */
	/******************************************************************/
	if ((metaData->ownerType & BMD_REQ_DB_SELECT_ADVANCED_LOGICAL_OR) == BMD_REQ_DB_SELECT_ADVANCED_LOGICAL_OR)
	{
		asprintf(recLogical, "OR");
		num_open_brackets += metaData->ownerType - BMD_REQ_DB_SELECT_ADVANCED_LOGICAL_OR;
	}
	else if ((metaData->ownerType & BMD_REQ_DB_SELECT_ADVANCED_LOGICAL_AND) == BMD_REQ_DB_SELECT_ADVANCED_LOGICAL_AND)
	{
		asprintf(recLogical, "AND");
		num_open_brackets += metaData->ownerType - BMD_REQ_DB_SELECT_ADVANCED_LOGICAL_AND;
	}
	else
	{
		num_open_brackets += metaData->ownerType;
	}

	/********************************************************************/
	/* wydobycie ilosci nawiasow zamykajacych i argumaentów dopasowania */
	/********************************************************************/
	if ((metaData->ownerId & BMD_REQ_DB_SELECT_ADVANCED_PERC_NONE) == BMD_REQ_DB_SELECT_ADVANCED_PERC_NONE)
	{
		asprintf(recStartPerc, "%s", "'");
		asprintf(recStopPerc, "%s", "'");
		num_close_brackets += metaData->ownerId - BMD_REQ_DB_SELECT_ADVANCED_PERC_NONE;

	}
	else if ((metaData->ownerId & BMD_REQ_DB_SELECT_ADVANCED_PERC_BEGIN) == BMD_REQ_DB_SELECT_ADVANCED_PERC_BEGIN)
	{
		asprintf(recStartPerc, "%s", "'%");
		asprintf(recStopPerc, "%s", "'");
		num_close_brackets += metaData->ownerId - BMD_REQ_DB_SELECT_ADVANCED_PERC_BEGIN;

	}
	else if ((metaData->ownerId & BMD_REQ_DB_SELECT_ADVANCED_PERC_END) == BMD_REQ_DB_SELECT_ADVANCED_PERC_END)
	{
		asprintf(recStartPerc, "%s", "'");
		asprintf(recStopPerc, "%s", "%'");
		num_close_brackets += metaData->ownerId - BMD_REQ_DB_SELECT_ADVANCED_PERC_END;

	}
	else if ((metaData->ownerId & BMD_REQ_DB_SELECT_ADVANCED_PERC_BOTH) == BMD_REQ_DB_SELECT_ADVANCED_PERC_BOTH)
	{
		asprintf(recStartPerc, "%s", "'%");
		asprintf(recStopPerc, "%s", "%'");
		num_close_brackets += metaData->ownerId - BMD_REQ_DB_SELECT_ADVANCED_PERC_BOTH;

	}
	else if (	(defaultrecRelation & BMD_REQ_DB_SELECT_ILIKE_QUERY) ||
			(defaultrecRelation & BMD_REQ_DB_SELECT_LIKE_QUERY))
	{
		asprintf(recStartPerc, "%s", "'%");
		asprintf(recStopPerc, "%s", "%'");
		num_close_brackets += metaData->ownerId;

	}
	else if (	(defaultrecRelation & BMD_REQ_DB_SELECT_EXACT_ILIKE_QUERY) ||
			(defaultrecRelation & BMD_REQ_DB_SELECT_EQUAL_QUERY))
	{
		asprintf(recStartPerc, "%s", "'");
		asprintf(recStopPerc, "%s", "'");
		num_close_brackets += metaData->ownerId;

	}
	else
	{
		asprintf(recStartPerc, "%s", "'");
		asprintf(recStopPerc, "%s", "'");
		num_close_brackets += metaData->ownerId;

	}

	if (*metadataCast != NULL && strcmp(*metadataCast, "varchar")!=0)
	{
		if (*metadataCast != NULL && strcmp(*metadataCast, "timestamp")==0)
		{
			if (	(strcmp((*recRelation), "LIKE")==0) ||
				(strcmp((*recRelation), "ILIKE")==0))
			{
				free((*recStartPerc)); (*recStartPerc)=NULL;
				asprintf(recStartPerc, "%s", "'");
				free((*recStopPerc)); (*recStopPerc)=NULL;
				asprintf(recStopPerc, "%s", "%'");

			}
			else
			{
				free((*recStartPerc)); (*recStartPerc)=NULL;
				asprintf(recStartPerc, "%s", "'");
				free((*recStopPerc)); (*recStopPerc)=NULL;
				asprintf(recStopPerc, "%s", "'");
			}
		}
		else
		{
			free((*recStartPerc)); (*recStartPerc)=NULL;
			asprintf(recStartPerc, "%s", "");
			free((*recStopPerc)); (*recStopPerc)=NULL;
			asprintf(recStopPerc, "%s", "");

		}

	}

	metaData->ownerId *= ownerId_sign;

	for (i=0; i<num_open_brackets; i++)
	{
		free(tmp); tmp=NULL;
		asprintf(&tmp, "%s", (*recOpenBracets)==NULL?"":(*recOpenBracets));
		free((*recOpenBracets)); (*recOpenBracets)=NULL;
		asprintf(recOpenBracets, "%s(", tmp);
	}

	for (i=0; i<num_close_brackets; i++)
	{
		free(tmp); tmp=NULL;
		asprintf(&tmp, "%s", (*recCloseBrackets)==NULL?"":(*recCloseBrackets));
		free((*recCloseBrackets)); (*recCloseBrackets)=NULL;
		asprintf(recCloseBrackets, "%s)", tmp);
	}

	return BMD_OK;
}

long ConcatenateWithMetadataConditions(	void *hDB,
					char *SQLBegin,
					bmdDatagram_t *bmdDatagram,
					long *add_met_count,
					long *sys_met_count,
					long *act_met_count,
					long *pki_met_count,
					char **SQLFinish)
{
long i					= 0;
long k					= 0;
long idMetadataPresent			= 0;
long visibleMetadataPresent		= 0;
long presentMetadataPresent		= 0;
long filetypeMetadataPresent		= 0;
long correspondingIdMetadataPresent	= 0;
long transactionIdMetadataPresent	= 0;
long locationIdMetadataPresent		= 0;
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
char *metadataCast			= NULL;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (hDB==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (SQLBegin==NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (bmdDatagram==NULL)	{	BMD_FOK(BMD_ERR_PARAM3);	}
	if (SQLFinish==NULL)	{	BMD_FOK(BMD_ERR_PARAM6);	}
	if ((*SQLFinish)!=NULL)	{	BMD_FOK(BMD_ERR_PARAM6);	}

	for (k=0; k<4; k++)
	{
		switch(k)
		{
			case 0	:	no_of_metaData = bmdDatagram->no_of_sysMetaData; metaData = bmdDatagram->sysMetaData; break;
			case 1	:	no_of_metaData = bmdDatagram->no_of_additionalMetaData; metaData = bmdDatagram->additionalMetaData; break;
			case 2	:	no_of_metaData = bmdDatagram->no_of_actionMetaData; metaData = bmdDatagram->actionMetaData; break;
			case 3	:	no_of_metaData = bmdDatagram->no_of_pkiMetaData; metaData = bmdDatagram->pkiMetaData; break;
		}

		for (i=0; i<no_of_metaData; i++)
		{
			free(metadataCast); metadataCast=NULL;
			DetermineSearchConditionArguments(	metaData[i],
								bmdDatagram->datagramStatus,
								&recColumnName,
								&recLogical,
								&recRelation,
								&recOpenBracets,
								&recCloseBrackets,
								&recStartPerc,
								&recStopPerc,
								&metadataCast,
								add_met_count,
								sys_met_count,
								act_met_count,
								pki_met_count);

			PRINT_TEST(">>>>>>>>> PKL <<<<<<<<<<< recColumnName: %s\n", recColumnName);
			PRINT_TEST(">>>>>>>>> PKL <<<<<<<<<<< recLogical: %s\n", recLogical);
			PRINT_TEST(">>>>>>>>> PKL <<<<<<<<<<< recRelation: %s\n",  recRelation);
			PRINT_TEST(">>>>>>>>> PKL <<<<<<<<<<< recOpenBracets: %s\n", recOpenBracets);
			PRINT_TEST(">>>>>>>>> PKL <<<<<<<<<<< recCloseBrackets: %s\n", recCloseBrackets);
			PRINT_TEST(">>>>>>>>> PKL <<<<<<<<<<< recStartPerc: %s\n", recStartPerc);
			PRINT_TEST(">>>>>>>>> PKL <<<<<<<<<<< recStopPerc: %s\n", recStopPerc);

			if (recColumnName!=NULL)
			{
				if (strcmp(recColumnName, "crypto_objects.id")==0)			{	idMetadataPresent=1;	}
				else if (strcmp(recColumnName, "crypto_objects.corresponding_id")==0)	{	correspondingIdMetadataPresent=1;	}
				else if (strcmp(recColumnName, "crypto_objects.visible")==0)
				{
					visibleMetadataPresent=1;

					// sprawdzenie, czy widocznosc ma byc ignorowana
					if(metaData[i] != NULL && metaData[i]->AnyBuf != NULL && (metaData[i]->AnySize == strlen(IGNORE_DOC_VISIBILITY_STRING)+1) )
					{
						if(memcmp(metaData[i]->AnyBuf, IGNORE_DOC_VISIBILITY_STRING, metaData[i]->AnySize) == 0)
						{
							visibleMetadataPresent = IGNORE_DOC_VISIBILITY;
							(*sys_met_count) --;
							free0(recColumnName);
							free0(recLogical);
							free0(recRelation);
							free0(recOpenBracets);
							free0(recCloseBrackets);
							free0(recStartPerc);
							free0(recStopPerc);
							//metadataCast czyszczona na poczatku petli
							continue; // pomijany warunek na visible - zostanie dodany nizej
						}
					}
				}
				else if (strcmp(recColumnName, "crypto_objects.present")==0)		{	presentMetadataPresent=1;	}
				else if (strcmp(recColumnName, "crypto_objects.file_type")==0)		{	filetypeMetadataPresent=1;	}
				else if (strcmp(recColumnName, "crypto_objects.transaction_id")==0)	{	transactionIdMetadataPresent=1;	}
				else if (strcmp(recColumnName, "crypto_objects.location_id")==0) 	{	locationIdMetadataPresent=1;	}

				free(tmp); tmp=NULL;
				asprintf(&tmp, "%s", (*SQLFinish)==NULL?"":(*SQLFinish));
				free((*SQLFinish)); (*SQLFinish)=NULL;

				if 	((recLogical==NULL) && ((*sys_met_count)+(*add_met_count)+(*act_met_count)+(*pki_met_count)<=1)) asprintf(&recLogical, "%s", "");
				else if	(recLogical==NULL) asprintf(&recLogical, "%s", "AND");
				else if ( (recLogical!=NULL) && ((*sys_met_count)+(*add_met_count)+(*act_met_count)+(*pki_met_count)<=1))
				{
					free0(recLogical);
					asprintf(&recLogical, "%s", "");
				}

				if (recRelation==NULL) asprintf(&recRelation, "%s", "LIKE");
				if (recOpenBracets==NULL) asprintf(&recOpenBracets, "%s", "");
				if (recCloseBrackets==NULL) asprintf(&recCloseBrackets, "%s", "");
				if (recStartPerc==NULL) asprintf(&recStartPerc, "'");
				if (recStopPerc==NULL) asprintf(&recStopPerc, "'");


				if (strcmp(recRelation, "LIKE")==0 || strcmp(recRelation, "ILIKE")==0)
				{
					BMD_FOK(bmd_db_escape_string(hDB, metaData[i]->AnyBuf,ADDITIONAL_ESCAPING,&recMetadataValue));
					if( bmd_db_connect_get_db_type(hDB) == BMD_DB_POSTGRES )
					{
						if(strcmp(recRelation, "LIKE")==0)
						{
							asprintf(SQLFinish, "%s\t%s %s %s::text %s %s%s%s ESCAPE '/' %s\n",
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
							asprintf(SQLFinish, "%s\t%s %s lower(%s::text) %s lower(%s%s%s) ESCAPE '/' %s\n",
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
// 						/* ORACLE */
// 						char *oracle_recStartPerc=NULL,*oracle_recStopPerc=NULL;
// 						char *bss=NULL;
//
// 						if( strcmp(recStartPerc,"'%") == 0 )
// 							asprintf(&oracle_recStartPerc,"'%c'||",'%');
// 						else
// 							asprintf(&oracle_recStartPerc,"%s","");
// 						if( strcmp(recStopPerc,"%'") == 0 )
// 							asprintf(&oracle_recStopPerc,"||'%c'",'%');
// 						else
// 							asprintf(&oracle_recStopPerc,"%s","");
//
// 						BMD_FOK(bmd_db_add_bind_var(recMetadataValue,NULL,hDB,&bss,NULL,NULL));
//
// 						if( strcmp(recRelation,"ILIKE") == 0 )
// 						{
// 							/* ORACLE_ILIKE */
// 							asprintf(SQLFinish, "%s\t%s %s lower(%s) %s %slower(%s)%s %s\n",
// 									tmp,
// 									recLogical,recOpenBracets,
// 									recColumnName,"LIKE",
// 									oracle_recStartPerc,bss,
// 									oracle_recStopPerc,recCloseBrackets);
// 						}
// 						else
// 						{
// 							/* ORACLE_LIKE */
// 							asprintf(SQLFinish, "%s\t%s %s %s %s %s%s%s %s\n",
// 									tmp,
// 									recLogical,recOpenBracets,
// 									recColumnName,recRelation,
// 									oracle_recStartPerc,bss,
// 									oracle_recStopPerc,recCloseBrackets);
// 						}
// 						free0(bss);
// 						free0(oracle_recStartPerc);free0(oracle_recStopPerc);
					}
				}
				else
				{
					BMD_FOK(bmd_db_escape_string(hDB, metaData[i]->AnyBuf, STANDARD_ESCAPING, &recMetadataValue));
					if(metadataCast != NULL && strcmp(metadataCast, "timestamp") == 0 && recRelation != NULL && strcmp(recRelation, "<") == 0)
					{
						if( bmd_db_connect_get_db_type(hDB) == BMD_DB_POSTGRES )
						{
							asprintf(SQLFinish, "%s\t%s %s %s %s timestamp '%s' + interval '1 day' %s\n",
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
							asprintf(SQLFinish, "%s\t%s %s %s %s %s%s%s %s\n",
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
	// 						char *bss=NULL;
	// 						char *real_bss=NULL;
	// 						long flag_id=0;
	// 						char *oracle_recStartPerc=NULL;
	// 						char *oracle_recStopPerc=NULL;
	//
	// 						if( strcmp(recColumnName,"crypto_objects.insert_date") == 0 )
	// 						{
	// 							asprintf(&oracle_recStartPerc,"%s","");
	// 							asprintf(&oracle_recStopPerc,"%s","");
	// 							flag_id=1;
	// 						}
	// 						else
	// 						{
	// 							if( strcmp(recStartPerc,"'%") == 0 )
	// 								asprintf(&oracle_recStartPerc,"%c",'%');
	// 							else
	// 								asprintf(&oracle_recStartPerc,"%s","");
	// 							if( strcmp(recStopPerc,"%'") == 0 )
	// 								asprintf(&oracle_recStopPerc,"%c",'%');
	// 							else
	// 								asprintf(&oracle_recStopPerc,"%s","");
	// 						}
	// 						BMD_FOK(bmd_db_add_bind_var(recMetadataValue,NULL,hDB,&bss,NULL,NULL));
	// 						if( flag_id == 1 )
	// 							asprintf(&real_bss,"(SELECT CAST(%s AS TIMESTAMP(6) WITH TIME ZONE) FROM DUAL)",bss);
	// 						else
	// 							asprintf(&real_bss,"%s",bss);
	// 						asprintf(SQLFinish, "%s\t%s %s %s %s %s%s%s %s\n",
	// 								tmp,
	// 								recLogical,
	// 								recOpenBracets,
	// 								recColumnName,
	// 								recRelation,
	// 								oracle_recStartPerc,
	// 								real_bss,
	// 								oracle_recStopPerc,
	// 								recCloseBrackets);
	// 						free0(bss);
	// 						free0(real_bss);
	// 						free0(oracle_recStartPerc);free0(oracle_recStopPerc);
						}
					}
				}
				free(tmp); tmp=NULL;
				free(recMetadataValue); recMetadataValue=NULL;
			}
			else
			{

			}
			free0(recColumnName);
			free0(recLogical);
			free0(recOpenBracets);
			free0(recCloseBrackets);
			free0(recStartPerc);
			free0(recStopPerc);
			free0(recRelation);
		}

	}

	free0(tmp);
	asprintf(&tmp, "%s", (*SQLFinish)==NULL?"":(*SQLFinish));
	free0((*SQLFinish));
	asprintf(SQLFinish, "%s\n", tmp);
	free0(tmp);

	if (bmdDatagram->datagramType!=BMD_DATAGRAM_TYPE_CONSERVATE_TIMESTAMPS)
	{
		if (visibleMetadataPresent==0)
		{
			free(tmp); tmp=NULL;
			asprintf(&tmp, "%s", (*SQLFinish)==NULL?"":(*SQLFinish));
			free((*SQLFinish)); (*SQLFinish)=NULL;
			asprintf(SQLFinish, "%s\n\t%s ( crypto_objects.visible = 1 )\n", tmp, ((*add_met_count)+(*sys_met_count)+(*act_met_count))>0?"AND":"");
			free(tmp); tmp=NULL;
		}
		else if(visibleMetadataPresent == IGNORE_DOC_VISIBILITY) // ignorowanie widocznosci
		{
			free(tmp); tmp=NULL;
			asprintf(&tmp, "%s", (*SQLFinish)==NULL?"":(*SQLFinish));
			free((*SQLFinish)); (*SQLFinish)=NULL;
			asprintf(SQLFinish, "%s\n\t%s ( crypto_objects.visible = 1 OR crypto_objects.visible = 0 )\n", tmp, ((*add_met_count)+(*sys_met_count)+(*act_met_count))>0?"AND":"");
			free(tmp); tmp=NULL;
		}

		if (correspondingIdMetadataPresent==0)
		{
			if (	(bmdDatagram->datagramType==BMD_DATAGRAM_DIR_LIST) ||
				(bmdDatagram->datagramType==BMD_DATAGRAM_DIR_LIST_WITH_OR))
			{
				free(tmp); tmp=NULL;
				asprintf(&tmp, "%s", (*SQLFinish)==NULL?"":(*SQLFinish));
				free((*SQLFinish)); (*SQLFinish)=NULL;
				asprintf(SQLFinish, "%s\tAND ( crypto_objects.corresponding_id = 0 )\n", tmp);
				free(tmp); tmp=NULL;
			}
		}

		if (presentMetadataPresent==0)
		{
			free(tmp); tmp=NULL;
			asprintf(&tmp, "%s", (*SQLFinish)==NULL?"":(*SQLFinish));
			free((*SQLFinish)); (*SQLFinish)=NULL;
			asprintf(SQLFinish, "%s\tAND ( crypto_objects.present = 1 )\n", tmp);
			free(tmp); tmp=NULL;
		}

		if (	(bmdDatagram->datagramType!=BMD_DATAGRAM_DIR_LIST) &&
			(bmdDatagram->datagramType!=BMD_DATAGRAM_DIR_LIST_WITH_OR) &&
			(bmdDatagram->datagramType!=BMD_DATAGRAM_DIR_GET_DETAIL) &&
			(bmdDatagram->datagramType!=BMD_DATAGRAM_DIR_DELETE))
		{
			free(tmp); tmp=NULL;
			asprintf(&tmp, "%s", (*SQLFinish)==NULL?"":(*SQLFinish));
			free((*SQLFinish)); (*SQLFinish)=NULL;

			asprintf(SQLFinish, "%s\tAND ( crypto_objects.file_type <> 'dir')\n", tmp);
			free(tmp); tmp=NULL;
		}

		/****************************************************/
		/* miejsce gdzie wstawi sie warunek dotyczacy linka */
		/****************************************************/
		free0(tmp);
		asprintf(&tmp, "%s", (*SQLFinish)==NULL?"":(*SQLFinish));
		free0((*SQLFinish));

		asprintf(SQLFinish, "%s\t%s\n", tmp, "<first_step>");
		free0(tmp);


		if (transactionIdMetadataPresent==0)
		{
			free(tmp); tmp=NULL;
			asprintf(&tmp, "%s", (*SQLFinish)==NULL?"":(*SQLFinish));
			free((*SQLFinish)); (*SQLFinish)=NULL;
			asprintf(SQLFinish, "%s\tAND ( (crypto_objects.transaction_id IS NULL) OR (crypto_objects.transaction_id = '') )\n", tmp);
			free(tmp); tmp=NULL;
		}

		if (locationIdMetadataPresent==0)
		{

			if (_GLOBAL_bmd_configuration->location_id!=NULL)
			{

				if (_GLOBAL_bmd_configuration->enable_location_id_in_search==1)
				{
					free(tmp); tmp=NULL;
					asprintf(&tmp, "%s", (*SQLFinish)==NULL?"":(*SQLFinish));
					free((*SQLFinish)); (*SQLFinish)=NULL;
					asprintf(SQLFinish, "%s\tAND (crypto_objects.location_id = %s  )\n", tmp, _GLOBAL_bmd_configuration->location_id);
					free(tmp); tmp=NULL;
				}
			}
		}


		/****************************************************/
		/* miejsce gdzie wstawi sie warunek dotyczacy linka */
		/****************************************************/
		free0(tmp);
		asprintf(&tmp, "%s", (*SQLFinish)==NULL?"":(*SQLFinish));
		free0((*SQLFinish));

		asprintf(SQLFinish, "%s\t%s\n", tmp, "<second_step>");
		free0(tmp);
	}

	return BMD_OK;
}
