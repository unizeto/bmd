#include <bmd/libbmdsql/select.h>
#include <time.h>
#include <bmd/libbmdpr/prlib-common/PR_OBJECT_IDENTIFIER.h>
#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>
#include <bmd/libbmdglobals/libbmdglobals.h>

#ifdef WIN32
#pragma warning(disable:4100)
#endif

// long GenerateSystemWhereCondition(	void *hDB,
// 						MetaDataBuf_t **MetaDataBuf,
// 						long no_of_MetaData,
// 						char **SQLConditions,
// 						long SQL_operator_flag,
// 						EnumSearchMode_t SearchMode,
// 					server_request_data_t *req)
// {
// long i				= 0;
// long statement			= 0;
// char *column_name			= NULL;
// char *tmp				= NULL;
// char *relation_operator		= NULL;
// char *OIDStringPrefix		= NULL;
// char *OIDStringSufix		= NULL;
// char *concat_operator		= NULL;
// char *sysMetadataValue		= NULL;
//
// 	PRINT_INFO("LIBBMDSQLINF Generating system with conditions\n");
// 	/********************************/
// 	/*	walidacja parametrów	*/
// 	/********************************/
// 	if(hDB == NULL)							{	BMD_FOK(BMD_ERR_PARAM1);	}
// 	if(MetaDataBuf == NULL)						{	BMD_FOK(BMD_ERR_PARAM2);	}
// 	if(no_of_MetaData < 0)						{	BMD_FOK(BMD_ERR_PARAM3);	}
// 	if(SQLConditions == NULL || *SQLConditions != NULL)	{	BMD_FOK(BMD_ERR_PARAM4);	}
// 	if(SQL_operator_flag < 0)					{	BMD_FOK(BMD_ERR_PARAM5);	}
//
// 	/******************************************/
// 	/*	okreslenie operatora wyszukiwania	*/
// 	/******************************************/
// 	switch(SearchMode)
// 	{
// 		case SEARCH_WITH_AND	:	asprintf(&concat_operator,"%s","AND"); break;
// 		case SEARCH_WITH_OR	:	asprintf(&concat_operator,"%s","OR"); break;
// 	}
//
// 	asprintf(SQLConditions, "%s", "");
//
// 	/************************************************************/
// 	/*	przeszukiwanie po wszystkich metadanych systemowych 	*/
// 	/************************************************************/
// 	for(i=0; i<no_of_MetaData; i++)
// 	{
// 		/************************/
// 		/* okreslenie operatora */
// 		/************************/
// 		if (MetaDataBuf[i]->ownerType==POSITIVE_STATEMENT)	{	statement=POSITIVE_STATEMENT;	}
// 		if (MetaDataBuf[i]->ownerType==NEGATIVE_STATEMENT)	{	statement=NEGATIVE_STATEMENT;	}
//
// 		/************************************************/
// 		/*	przekształcanie oida na nazwe kolumny	*/
// 		/************************************************/
// 		BMD_FOK(OID_to_column_name(MetaDataBuf[i]->OIDTableBuf, &column_name));
//
// 		/*********************************/
// 		/* escapownie wartosci metadanej */
// 		/*********************************/
// 		free(sysMetadataValue); sysMetadataValue=NULL;
// 		free(tmp); tmp=NULL;
//
// 		sysMetadataValue=(char*)malloc(sizeof(char)*(MetaDataBuf[i]->AnySize+1));
// 		memset(sysMetadataValue, 0, sizeof(char)*(MetaDataBuf[i]->AnySize+1));
// 		memcpy(sysMetadataValue, MetaDataBuf[i]->AnyBuf, sizeof(char)*(MetaDataBuf[i]->AnySize));
//
// 		BMD_FOK(bmd_db_escape_string(hDB,sysMetadataValue,&tmp));
// 		free(sysMetadataValue); sysMetadataValue=NULL;
//
// 		asprintf(&sysMetadataValue, "%s", tmp);
// 		if (sysMetadataValue==NULL)	{	BMD_FOK(NO_MEMORY);	}
//
// 		/*************************************/
// 		/* jesli wyszukujemy po nazwie pliku */
// 		/*************************************/
// 		if(!strcmp(MetaDataBuf[i]->OIDTableBuf, OID_SYS_METADATA_CRYPTO_OBJECTS_FILENAME))
// 		{
// 			if (MetaDataBuf[i]->myId!=0)
// 			{
// 				SQL_operator_flag=MetaDataBuf[i]->myId;
// 			}
//
// 			switch(SQL_operator_flag)
// 			{
// 				case BMD_REQ_DB_SELECT_LIKE_QUERY :
// 					if( bmd_db_connect_get_db_type(hDB) == BMD_DB_POSTGRES )
// 						asprintf(&tmp, "%s \n\t%s %s::text %s LIKE '%c%s%c'",
// 										*SQLConditions,
// 										concat_operator,
// 										column_name,
// 										statement==POSITIVE_STATEMENT?"":"NOT",
// 										'%',
// 										sysMetadataValue,
// 										'%');
// 					else
// 						if( bmd_db_connect_get_db_type(hDB) == BMD_DB_ORACLE )
// 						{
// 							char *bss=NULL;
// 							bmd_db_add_bind_var(sysMetadataValue,NULL,hDB,&bss,NULL,NULL);
// 							asprintf(&tmp, "%s \n\t%s %s %s LIKE '%s%s%s'",
// 										*SQLConditions,
// 										concat_operator,
// 										column_name,
// 										statement==POSITIVE_STATEMENT?"":"NOT",
// 										"'%'||",
// 										bss,
// 										"||'%'");
// 							free0(bss);
// 						}
// 					break;
// 				case BMD_REQ_DB_SELECT_ILIKE_QUERY :
// 					if( bmd_db_connect_get_db_type(hDB) == BMD_DB_POSTGRES )
// 						asprintf(&tmp, "%s \n\t%s %s::text %s ILIKE '%c%s%c'",
// 									   *SQLConditions,
// 										concat_operator,
// 										column_name,
// 										statement==POSITIVE_STATEMENT?"":"NOT",
// 										'%',
// 										sysMetadataValue,
// 										'%');
// 					else
// 					{
// 						char *bss=NULL;
// 						bmd_db_add_bind_var(sysMetadataValue,NULL,hDB,&bss,NULL,NULL);
// 						asprintf(&tmp, "%s \n\t%s lower(%s) %s LIKE lower(%s%s%s)",
// 									   *SQLConditions,
// 										concat_operator,
// 										column_name,
// 										statement==POSITIVE_STATEMENT?"":"NOT",
// 										"'%'||",
// 										bss,
// 										"||'%'");
// 						free0(bss);
// 					}
// 					break;
// 				case BMD_REQ_DB_SELECT_EQUAL_QUERY :
// 					if( bmd_db_connect_get_db_type(hDB) == BMD_DB_POSTGRES )
// 						asprintf(&tmp, "%s \n\t%s %s::text %s '%s'",
// 										*SQLConditions,
// 										concat_operator,
// 										column_name,
// 										statement==POSITIVE_STATEMENT?"=":"<>",
// 										sysMetadataValue);
// 					else
// 					{
// 						char *bss=NULL;
// 						bmd_db_add_bind_var(sysMetadataValue,NULL,hDB,&bss,NULL,NULL);
// 						asprintf(&tmp, "%s \n\t%s %s %s %s",
// 										*SQLConditions,
// 										concat_operator,
// 										column_name,
// 										statement==POSITIVE_STATEMENT?"=":"<>",
// 										bss);
// 						free0(bss);
// 					}
// 					break;
// 				case BMD_REQ_DB_SELECT_EXACT_ILIKE_QUERY :
// 					if( bmd_db_connect_get_db_type(hDB) == BMD_DB_POSTGRES )
// 						asprintf(&tmp, "%s \n\t%s %s::text %s ILIKE '%s'",
// 										*SQLConditions,
// 										concat_operator,
// 										column_name,
// 										statement==POSITIVE_STATEMENT?"":"NOT",
// 										sysMetadataValue);
// 					else
// 					{
// 						char *bss=NULL;
// 						bmd_db_add_bind_var(sysMetadataValue,NULL,hDB,&bss,NULL,NULL);
// 						asprintf(&tmp, "%s \n\t%s lower(%s) %s LIKE lower(%s)",
// 										*SQLConditions,
// 										concat_operator,
// 										column_name,
// 										statement==POSITIVE_STATEMENT?"":"NOT",
// 										bss);
// 						free0(bss);
// 					}
// 					break;
// 			}
// 			free(*SQLConditions); *SQLConditions = NULL;
// 			asprintf(SQLConditions, "%s", tmp);
//
// 			free(tmp); tmp = NULL;
// 			free(column_name); column_name = NULL;
// 		}
// 		/************************************************************/
// 		/*	jesli wyszkujemy po rozmiarze pliku, kiedy równe	*/
// 		/************************************************************/
// 		else if( !strcmp(MetaDataBuf[i]->OIDTableBuf, OID_SYS_METADATA_CRYPTO_OBJECTS_FILESIZE))
// 		{
// 			if( bmd_db_connect_get_db_type(hDB) == BMD_DB_ORACLE )
// 			{
// 				char *bss=NULL;
// 				bmd_db_add_bind_var(sysMetadataValue,NULL,hDB,&bss,NULL,NULL);
// 				asprintf(&tmp, "%s \n\t%s %s %s %s",
// 				     *SQLConditions, concat_operator, column_name,
// 					 statement==POSITIVE_STATEMENT?"=":"<>",
// 					 bss);
// 				free0(bss);
// 			}
// 			else
// 			{
// 				asprintf(&tmp, "%s \n\t%s %s %s %s",
// 				     *SQLConditions, concat_operator, column_name,
// 					 statement==POSITIVE_STATEMENT?"=":"<>",
// 					 sysMetadataValue);
// 			}
// 			free(*SQLConditions); *SQLConditions = NULL;
// 			asprintf(SQLConditions, "%s", tmp);
//
// 			free(tmp); tmp = NULL;
// 			free(column_name); column_name = NULL;
//
// 		}
// 		/************************************************************/
// 		/*	jesli wyszkujemy po dacie wprowadzenia, kiedy równa	*/
// 		/************************************************************/
// 		else if(!strcmp(MetaDataBuf[i]->OIDTableBuf, OID_SYS_METADATA_CRYPTO_OBJECTS_TIMESTAMP))
// 		{
// 			if( bmd_db_connect_get_db_type(hDB) == BMD_DB_POSTGRES )
// 			{
// 				asprintf(&tmp, "%s \n\t%s %s::text %s LIKE '%s%c'",
// 							   *SQLConditions,
// 							   concat_operator,
// 							   column_name,
// 							   statement==POSITIVE_STATEMENT?"":"NOT",
// 							   sysMetadataValue,
// 							   '%');
// 			}
// 			else if( bmd_db_connect_get_db_type(hDB) == BMD_DB_ORACLE )
// 			{
// 				char *bss=NULL;
// 				bmd_db_add_bind_var(sysMetadataValue,NULL,hDB,&bss,NULL,NULL);
// 				asprintf(&tmp, "%s \n\t%s TO_CHAR((SELECT CAST(%s AS DATE) FROM DUAL),'YYYY-MM-DD') %s %s",
// 							   *SQLConditions,
// 							   concat_operator,
// 							   column_name,
// 							   statement==POSITIVE_STATEMENT?"=":"<>",
// 							   bss);
// 				free0(bss);
// 			}
//
// 			free(*SQLConditions); *SQLConditions = NULL;
// 			asprintf(SQLConditions, "%s", tmp);
// 			free(tmp); tmp = NULL;
// 			free(column_name); column_name = NULL;
//
// 		}
// 		/************************************************/
// 		/*	wyszukiwanie z zakresu znacznika czasu	*/
// 		/************************************************/
// 		else if(	!strcmp(MetaDataBuf[i]->OIDTableBuf, OID_SYS_METADATA_CRYPTO_OBJECTS_TIMESTAMP_MORE) ||
// 				!strcmp(MetaDataBuf[i]->OIDTableBuf, OID_SYS_METADATA_CRYPTO_OBJECTS_TIMESTAMP_LESS))
// 		{
// 			BMD_FOK(SplitOidStringToPrefixAndSufix(MetaDataBuf[i]->OIDTableBuf, &OIDStringPrefix, &OIDStringSufix));
// 			BMD_FOK(OIDsufixToOperatorType(OIDStringSufix, &relation_operator));
// 			free(OIDStringPrefix); OIDStringPrefix=NULL;
// 			free(OIDStringSufix); OIDStringSufix=NULL;
//
// 			if( bmd_db_connect_get_db_type(hDB) == BMD_DB_POSTGRES )
// 			{
// 				if (strlen(sysMetadataValue)==10)
// 				{
// 					if (strcmp(relation_operator,">")==0)
// 					{
// 						asprintf(&tmp, "%s \n\t%s %s %s= '%s 00:00:00'",
// 									*SQLConditions,
// 									concat_operator,
// 									column_name,
// 									relation_operator,
// 									sysMetadataValue);
// 					}
// 					if (strcmp(relation_operator,"<")==0)
// 					{
// 						asprintf(&tmp, "%s \n\t%s %s %s= '%s 23:59:59.999999'",
// 									*SQLConditions,
// 									concat_operator,
// 									column_name,
// 									relation_operator,
// 									sysMetadataValue);
// 					}
// 				}
// 				else
// 				{
// 					if (strcmp(relation_operator,">")==0)
// 					{
// 						asprintf(&tmp, "%s \n\t%s %s %s= '%s'",
// 									*SQLConditions,
// 									concat_operator,
// 									column_name,
// 									relation_operator,
// 									sysMetadataValue);
// 					}
// 					if (strcmp(relation_operator,"<")==0)
// 					{
// 						asprintf(&tmp, "%s \n\t%s %s %s= '%s.999999'",
// 									*SQLConditions,
// 									concat_operator,
// 									column_name,
// 									relation_operator,
// 									sysMetadataValue);
// 					}
// 				}
// 			}
// 			else
// 			if( bmd_db_connect_get_db_type(hDB) == BMD_DB_ORACLE )
// 			{
// 				char *bss=NULL;
// 				bmd_db_add_bind_var(sysMetadataValue,NULL,hDB,&bss,NULL,NULL);
// 				asprintf(&tmp, "%s \n\t%s TO_CHAR((SELECT CAST(%s AS DATE) FROM DUAL),'YYYY-MM-DD') %s= %s",
// 								*SQLConditions,
// 								concat_operator,
// 								column_name,
// 								relation_operator,
// 								bss);
// 				free0(bss);
// 			}
// 			free(*SQLConditions); *SQLConditions = NULL;
// 			asprintf(SQLConditions, "%s", tmp);
// 			free(tmp); tmp = NULL;
// 			free(column_name); column_name = NULL;
// 			free(relation_operator); relation_operator = NULL;
// 		}
// 		/************************************************/
// 		/*	wyszukiwanie z zakresu rozmiaru pliku	*/
// 		/************************************************/
// 		else if(	!strcmp(MetaDataBuf[i]->OIDTableBuf, OID_SYS_METADATA_CRYPTO_OBJECTS_FILESIZE_MORE) ||
// 				!strcmp(MetaDataBuf[i]->OIDTableBuf, OID_SYS_METADATA_CRYPTO_OBJECTS_FILESIZE_LESS) )
// 		{
// 			BMD_FOK(SplitOidStringToPrefixAndSufix(MetaDataBuf[i]->OIDTableBuf, &OIDStringPrefix, &OIDStringSufix));
// 			BMD_FOK(OIDsufixToOperatorType(OIDStringSufix, &relation_operator));
//
// 			free(OIDStringPrefix); OIDStringPrefix=NULL;
// 			free(OIDStringSufix); OIDStringSufix=NULL;
//
// 			if( bmd_db_connect_get_db_type(hDB) == BMD_DB_ORACLE )
// 			{
// 				char *bss=NULL;
// 				bmd_db_add_bind_var(sysMetadataValue,NULL,hDB,&bss,NULL,NULL);
// 				asprintf(&tmp, "%s \n\t%s %s %s= %s",
// 					     *SQLConditions, concat_operator, column_name,relation_operator,bss);
// 				free0(bss);
// 			}
// 			else
// 			{
// 				asprintf(&tmp, "%s \n\t%s %s %s= %s",
// 				     *SQLConditions, concat_operator, column_name,relation_operator, sysMetadataValue);
// 			}
// 			free(*SQLConditions); *SQLConditions = NULL;
// 			asprintf(SQLConditions, "%s", tmp);
//
// 			free(tmp); tmp = NULL;
// 			free(column_name); column_name = NULL;
// 			free(relation_operator); relation_operator = NULL;
// 		}
// 		else
// 		{
// 			free0(tmp);free0(column_name);
// 		}
// 		free0(column_name);free0(tmp);
// 	}
//
// 	free0(column_name);
// 	free0(tmp);
// 	free0(relation_operator);
// 	free0(OIDStringPrefix);
// 	free0(OIDStringSufix);
// 	free0(concat_operator);
//
// 	return BMD_OK;
// }


// long ConcatenateWithWhereSysMetadata(	void *hDB,
// 							char *SQLPrefix,
// 							bmdDatagram_t *bmdDatagram,
// 							EnumSearchMode_t SearchMode,
// 							char **SQLAfterSysMtd,
// 					server_request_data_t *req)
// {
// char *SQLConditionsSys				= NULL;
// char *SQLTemp					= NULL;
//
// char *visible_metadata_value			= NULL;
// char *corresponding_id_metadata_value	= NULL;
// char *file_type_metadata_value		= NULL;
// char *present_metadata_value			= NULL;
//
// long i						= 0;
//
// 	PRINT_INFO("LIBBMDSQLINF Concatenating with where system metadata\n");
// 	/******************************/
// 	/*	walidacja parametrow	*/
// 	/******************************/
// 	if(SQLPrefix == NULL)						{	BMD_FOK(BMD_ERR_PARAM2);	}
// 	if(bmdDatagram == NULL)						{	BMD_FOK(BMD_ERR_PARAM3);	}
// 	if(SQLAfterSysMtd == NULL)					{	BMD_FOK(BMD_ERR_PARAM4);	}
// 	if(*SQLAfterSysMtd != NULL)					{	BMD_FOK(BMD_ERR_PARAM4);	}
//
// 	if(bmdDatagram->no_of_sysMetaData > 0)
// 	{
// 		for(i=0; i<bmdDatagram->no_of_sysMetaData; i++)
// 		{
// 			/************************************************************************************/
// 			/*	sprawdzenie, czy w datagramie znajduje sie informacja o widocznosci pliku	*/
// 			/************************************************************************************/
// 			if (strcmp(	bmdDatagram->sysMetaData[i]->OIDTableBuf, OID_SYS_METADATA_CRYPTO_OBJECTS_VISIBLE)==0)
// 			{
// 				visible_metadata_value=(char *)malloc(sizeof(char)*(bmdDatagram->sysMetaData[i]->AnySize+1));
// 				memset(visible_metadata_value, 0, sizeof(char)*(bmdDatagram->sysMetaData[i]->AnySize+1));
// 				memcpy(visible_metadata_value, bmdDatagram->sysMetaData[i]->AnyBuf, sizeof(char)*(bmdDatagram->sysMetaData[i]->AnySize));
// 			}
// 			else if (strcmp(	bmdDatagram->sysMetaData[i]->OIDTableBuf, OID_SYS_METADATA_CRYPTO_OBJECTS_CORESPONDING_ID)==0)
// 			{
// 				corresponding_id_metadata_value=(char *)malloc(sizeof(char)*(bmdDatagram->sysMetaData[i]->AnySize+1));
// 				memset(corresponding_id_metadata_value, 0, sizeof(char)*(bmdDatagram->sysMetaData[i]->AnySize+1));
// 				memcpy(corresponding_id_metadata_value, bmdDatagram->sysMetaData[i]->AnyBuf, sizeof(char)*(bmdDatagram->sysMetaData[i]->AnySize));
// 			}
// 			else if (strcmp(	bmdDatagram->sysMetaData[i]->OIDTableBuf, OID_SYS_METADATA_CRYPTO_OBJECTS_FILE_TYPE)==0)
// 			{
// 				file_type_metadata_value=(char *)malloc(sizeof(char)*(bmdDatagram->sysMetaData[i]->AnySize+1));
// 				memset(file_type_metadata_value, 0, sizeof(char)*(bmdDatagram->sysMetaData[i]->AnySize+1));
// 				memcpy(file_type_metadata_value, bmdDatagram->sysMetaData[i]->AnyBuf, sizeof(char)*(bmdDatagram->sysMetaData[i]->AnySize));
// 			}
// 			else if (strcmp(	bmdDatagram->sysMetaData[i]->OIDTableBuf, OID_SYS_METADATA_CRYPTO_OBJECTS_PRESENT)==0)
// 			{
// 				present_metadata_value=(char *)malloc(sizeof(char)*(bmdDatagram->sysMetaData[i]->AnySize+1));
// 				memset(present_metadata_value, 0, sizeof(char)*(bmdDatagram->sysMetaData[i]->AnySize+1));
// 				memcpy(present_metadata_value, bmdDatagram->sysMetaData[i]->AnyBuf, sizeof(char)*(bmdDatagram->sysMetaData[i]->AnySize));
// 			}
// 		}
//
//
// 		if(bmdDatagram->datagramStatus & BMD_REQ_DB_SELECT_ILIKE_QUERY)
// 		{
// 			BMD_FOK(GenerateSystemWhereCondition(hDB,
// 				bmdDatagram->sysMetaData,
// 				bmdDatagram->no_of_sysMetaData,&SQLConditionsSys,
// 				BMD_REQ_DB_SELECT_ILIKE_QUERY,SearchMode,req));
// 		}
// 		else if(bmdDatagram->datagramStatus & BMD_REQ_DB_SELECT_LIKE_QUERY)
// 		{
// 			BMD_FOK(GenerateSystemWhereCondition(hDB,bmdDatagram->sysMetaData,bmdDatagram->no_of_sysMetaData,
// 				&SQLConditionsSys,BMD_REQ_DB_SELECT_LIKE_QUERY,SearchMode,req));
// 		}
// 		else if(bmdDatagram->datagramStatus & BMD_REQ_DB_SELECT_EXACT_ILIKE_QUERY)
// 		{
// 			BMD_FOK(GenerateSystemWhereCondition(hDB,bmdDatagram->sysMetaData,
// 				bmdDatagram->no_of_sysMetaData,&SQLConditionsSys,
// 				BMD_REQ_DB_SELECT_EXACT_ILIKE_QUERY,SearchMode,req));
// 		}
// 		else if(bmdDatagram->datagramStatus & BMD_REQ_DB_SELECT_EQUAL_QUERY)
// 		{
// 			BMD_FOK(GenerateSystemWhereCondition(hDB,bmdDatagram->sysMetaData,bmdDatagram->no_of_sysMetaData,
// 				&SQLConditionsSys,BMD_REQ_DB_SELECT_EQUAL_QUERY,SearchMode,req));
// 		}
// 		else
// 		{
// 			BMD_FOK(GenerateSystemWhereCondition(hDB,bmdDatagram->sysMetaData,bmdDatagram->no_of_sysMetaData,
// 				&SQLConditionsSys, BMD_REQ_DB_SELECT_EQUAL_QUERY,SearchMode,req));
// 		}
//
// 		if (SQLConditionsSys!=NULL)
// 		{
// 			asprintf(SQLAfterSysMtd, "%s %s\t)", SQLPrefix, SQLConditionsSys);
// 		}
//
// 		free(SQLConditionsSys); SQLConditionsSys=NULL;
// 	}
//
// 	/***********************************************/
// 	/* usatwienie standardowych opcji wyszukiwania */
// 	/***********************************************/
// 	if ((*SQLAfterSysMtd)==NULL)
// 	{
// 		asprintf(&SQLTemp, "%s",SQLPrefix);
// 		free((*SQLAfterSysMtd)); (*SQLAfterSysMtd)=NULL;
// 		asprintf(SQLAfterSysMtd, "%s\t)\nAND (\n\t    crypto_objects.visible=%s",SQLTemp, visible_metadata_value==NULL?"1":visible_metadata_value);
// 		free(SQLTemp); SQLTemp=NULL;
// 	}
// 	else
// 	{
// 		asprintf(&SQLTemp, "%s",(*SQLAfterSysMtd));
// 		free((*SQLAfterSysMtd)); (*SQLAfterSysMtd)=NULL;
// 		asprintf(SQLAfterSysMtd, "%s\nAND (\n\t    crypto_objects.visible=%s",SQLTemp, visible_metadata_value==NULL?"1":visible_metadata_value);
// 		free(SQLTemp); SQLTemp=NULL;
// 	}
//
//
//  	if (	(bmdDatagram->datagramType==BMD_DATAGRAM_DIR_LIST) ||
//  		(bmdDatagram->datagramType==BMD_DATAGRAM_DIR_LIST_WITH_OR))
//  	{
// 		asprintf(&SQLTemp, "%s",(*SQLAfterSysMtd));
// 		free((*SQLAfterSysMtd)); (*SQLAfterSysMtd)=NULL;
// 		asprintf(SQLAfterSysMtd, "%s\n\tAND crypto_objects.corresponding_id=%s",SQLTemp, corresponding_id_metadata_value==NULL?"0":corresponding_id_metadata_value);
// 		free(SQLTemp); SQLTemp=NULL;
// 	}
//
//  	if (	(bmdDatagram->datagramType!=BMD_DATAGRAM_TYPE_SEARCH) &&
//  		(bmdDatagram->datagramType!=BMD_DATAGRAM_GET_METADATA_WITH_TIME))
//   	{
//  		asprintf(&SQLTemp, "%s",(*SQLAfterSysMtd));
// 		free((*SQLAfterSysMtd)); (*SQLAfterSysMtd)=NULL;
// 		asprintf(SQLAfterSysMtd, "%s\n\tAND crypto_objects.present=%s",SQLTemp, present_metadata_value==NULL?"1":present_metadata_value);
// 		free(SQLTemp); SQLTemp=NULL;
//   	}
//  	else if (_GLOBAL_bmd_configuration->enable_delete_history==0)
//  	{
//  		asprintf(&SQLTemp, "%s",(*SQLAfterSysMtd));
// 		free((*SQLAfterSysMtd)); (*SQLAfterSysMtd)=NULL;
// 		asprintf(SQLAfterSysMtd, "%s\n\tAND crypto_objects.present=1",SQLTemp);
// 		free(SQLTemp); SQLTemp=NULL;
//   	}
//
// 	if (	(bmdDatagram->datagramType!=BMD_DATAGRAM_DIR_LIST) &&
//  		(bmdDatagram->datagramType!=BMD_DATAGRAM_DIR_LIST_WITH_OR))
// 	{
// 		if (file_type_metadata_value==NULL)
// 		{
// 			asprintf(&SQLTemp, "%s",(*SQLAfterSysMtd));
// 			free((*SQLAfterSysMtd)); (*SQLAfterSysMtd)=NULL;
// 			asprintf(SQLAfterSysMtd, "%s\n\tAND ( (crypto_objects.file_type<>'dir') OR (crypto_objects.file_type IS NULL) OR (crypto_objects.file_type='' ) )",SQLTemp);
// 			free(SQLTemp); SQLTemp=NULL;
// 		}
// 		else
// 		{
// 			if (strcmp(file_type_metadata_value, "link")==0)
// 			{
// 				asprintf(&SQLTemp, "%s",(*SQLAfterSysMtd));
// 				free((*SQLAfterSysMtd)); (*SQLAfterSysMtd)=NULL;
// 				asprintf(SQLAfterSysMtd, "%s\n\tAND ( (crypto_objects.file_type<>'%s') )",SQLTemp, file_type_metadata_value);
// 				free(SQLTemp); SQLTemp=NULL;
// 			}
// 			else
// 			{
// 				asprintf(&SQLTemp, "%s",(*SQLAfterSysMtd));
// 				free((*SQLAfterSysMtd)); (*SQLAfterSysMtd)=NULL;
// 				asprintf(SQLAfterSysMtd, "%s\n\tAND ( (crypto_objects.file_type='%s') )",SQLTemp, file_type_metadata_value);
// 				free(SQLTemp); SQLTemp=NULL;
// 			}
// 		}
// 	}
//
// 	asprintf(&SQLTemp, "%s",(*SQLAfterSysMtd));
// 	free((*SQLAfterSysMtd)); (*SQLAfterSysMtd)=NULL;
// 	asprintf(SQLAfterSysMtd, "%s\t)", SQLTemp);
// 	free(SQLTemp); SQLTemp=NULL;
//
// 	/******************/
// 	/*	porzadki	*/
// 	/******************/
// 	free(visible_metadata_value); visible_metadata_value=NULL;
// 	free(corresponding_id_metadata_value); corresponding_id_metadata_value=NULL;
// 	free(file_type_metadata_value); file_type_metadata_value=NULL;
// 	free(present_metadata_value); present_metadata_value=NULL;
//
// 	free(SQLTemp); SQLTemp=NULL;
// 	free(SQLConditionsSys); SQLConditionsSys=NULL;
//
// 	return BMD_OK;
// }



long GetChildren(	char *GroupId,
			char ***ChildrenList,
			long *no_of_children)
{
	PRINT_INFO("LIBBMDSQLINF Getting group children\n");
	/************************/
	/* walidacja parametrow */
	/************************/
	if (GroupId==NULL)						{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (ChildrenList == NULL || *ChildrenList != NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (no_of_children == NULL || *no_of_children != 0)	{	BMD_FOK(BMD_ERR_PARAM3);	}

	/**********************/
	/* odpytanie slownika */
	/**********************/
	//SELECT src, dest FROM groups_graph;
	getColumnWithCondition(	_GLOBAL_GroupsGraphDictionary, 0, GroupId, 1, no_of_children, ChildrenList);

	return BMD_OK;
}

/************************************************************************/
/*	funkcja rekurencyjnie przechodzi drzewo grup w topologii inorder	*/
/************************************************************************/
long GenerateDependentGroupsIdList(	char *GroupId,
						char ***DependentGroupsList,
						long *no_of_DependentGroups)
{
char **ChildrenList = NULL;
long no_of_children = 0;
long i = 0;

	PRINT_INFO("LIBBMDSQLINF Generating dependent group id list\n");

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if (GroupId==NULL)			{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (DependentGroupsList == NULL)	{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (no_of_DependentGroups == NULL)	{	BMD_FOK(BMD_ERR_PARAM3);	}

	BMD_FOK(GetChildren(GroupId, &ChildrenList, &no_of_children));

	/* TODO Dodawanie tylko DISTINCT - przy okazaji sprawdzanie czy cos juz jest w bazie - w celu unikniaca zapetlen rekurencji */
	*DependentGroupsList = (char **) realloc (*DependentGroupsList,sizeof(char*) * ((*no_of_DependentGroups)+no_of_children));
	if(*DependentGroupsList == NULL)
	{
		PRINT_ERROR("LIBBMDSQLERR Memory error. Error=%i\n",NO_MEMORY);
		return NO_MEMORY;
	}
	memmove((*DependentGroupsList)+(*no_of_DependentGroups), ChildrenList, sizeof(long) * no_of_children);
	(*no_of_DependentGroups) +=no_of_children;

	if(no_of_children > 0)
	{
		for(i=0; i<no_of_children; i++)
		{
			BMD_FOK(GenerateDependentGroupsIdList(ChildrenList[i],DependentGroupsList, no_of_DependentGroups));
		}
	}

	return BMD_OK;
}



