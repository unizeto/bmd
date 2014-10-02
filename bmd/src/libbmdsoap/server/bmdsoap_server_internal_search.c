#include <bmd/libbmdsoap/libbmdsoap.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>
#include <bmd/libbmdglobals/libbmdglobals.h>


/********************************************************************************************************************/
/* obsluzenie zadania wyszukiwania plikow w BMD wedlug okreslonych kryteriow za pomoca dowolnego zapytania doa bazy */
/********************************************************************************************************************/
int bmd230__bmdAdvancedSearchFiles(	struct soap *soap,
				struct xsd__base64Binary *cert,
				char *roleName,
				char *groupName,
				char *userClassId,
				struct bmd230__mtdsSearchValues *mtds,
				struct bmd230__mtdsValues *formMtds,
				int offset,
				int limit,
				char *range,
				char *ownerRange,
				struct bmd230__searchSortInfoList *sortInfo,
				struct bmd230__searchResults **searchResults)
{
long i					= 0;
long j					= 0;
long max_metadata_value_length		= 0;
long tmpOperator			= 0;
long tmpDesc				= 0;
long tmpAdjustment			= 0;
long tmpStatement			= 0;
long mtdPrefix				= 0;
long mtdPostfix				= 0;
long max_datagram_in_set_transmission	= 0;
long deserialisation_max_memory		= 0;
long k					= 0;
long mtd_result_count			= 0;
long form_mtds_count			= 0;
bmd_info_t *bi				= NULL;
bmdDatagram_t *dtg			= NULL;
bmdDatagramSet_t *dtg_set		= NULL;
bmdDatagramSet_t *resp_dtg_set		= NULL;
char *sort_str				= NULL;
char* tempSortStr			= NULL;
char *sort_oid				= NULL;
char *sort_order			= NULL;
char *metadata_oid_str			= NULL;
GenBuf_t *metadata_value_buf		= NULL;
GenBuf_t tmp_gb;

char *derUserCert=NULL;
long derUserCertLen=0;
long retVal=0;
char* paramCert=NULL;
long paramCertLen=0;


	PRINT_SOAP("Request:\tsearch for document\n");
	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if ((offset<0) || (limit<1))			{	SOAP_FOK(BMDSOAP_SERVER_INVALID_LIMIT_VALUE);		}
	if (sortInfo!=NULL)
	{
		for (j=0; j<sortInfo->__size; j++)
		{
			if (sortInfo->__ptr[j].sortOrder!=NULL)
			{
				if ((strcmp(sortInfo->__ptr[j].sortOrder,"ASC")!=0) && \
				(strcmp(sortInfo->__ptr[j].sortOrder,"DESC")!=0))
				{
					SOAP_FOK(BMDSOAP_SERVER_SEARCH_SORT_ORDER_INVALID);
				}
			}
			else
			{
				SOAP_FOK(BMDSOAP_SERVER_SEARCH_SORT_ORDER_INVALID);
			}

			if (sortInfo->__ptr[j].sortOid!=NULL)
			{
				for (i=0; i<strlen(sortInfo->__ptr[j].sortOid); i++)
				{
					if (((sortInfo->__ptr[j].sortOid[i]<'0') || (sortInfo->__ptr[j].sortOid[i]>'9')) \
					&& (sortInfo->__ptr[j].sortOid[i]!='.'))
					{
						SOAP_FOK(BMDSOAP_SERVER_SEARCH_SORT_OID_INVALID);
					}
				}
			}
			else
			{
				SOAP_FOK(BMDSOAP_SERVER_SEARCH_SORT_OID_INVALID);
			}
		}
	}

	if(mtds!=NULL)
	{
		for(i=0;i<mtds->__size;i++)
		{
			if (mtds->__ptr!=NULL)
			{
				/******BMD_ONLY_GROUP_DOC*****************************/
				/* walidacja operatora dopasowania */
				/***********************************/
				if(mtds->__ptr[i].mtdDesc != NULL)
				{
					if (	(strcmp(mtds->__ptr[i].mtdDesc, "LIKE")!=0) &&
						(strcmp(mtds->__ptr[i].mtdDesc, "ILIKE")!=0))
					{
						SOAP_FOK(BMDSOAP_SERVER_ADVANCED_SEARCH_INVALID_ADJUSTMENT_OPER);
					}
				}
				else
				{
					SOAP_FOK(BMDSOAP_SERVER_ADVANCED_SEARCH_INVALID_ADJUSTMENT_OPER);
				}

				/**********************************/
				/* walidacja operatora logicznego */
				/**********************************/
				if ((i==0) && (mtds->__ptr[i].mtdLogicalOperator!=NULL))
				{
					SOAP_FOK(BMDSOAP_SERVER_ADVANCED_SEARCH_INVALID_FIRST_LOG_OPER);
				}
				else if ((i!=0) && (mtds->__ptr[i].mtdLogicalOperator==NULL))
				{
					SOAP_FOK(BMDSOAP_SERVER_ADVANCED_SEARCH_INVALID_LOG_OPER_MISSING);
				}
				else if ((i!=0) && (strcmp(mtds->__ptr[i].mtdLogicalOperator, "AND")!=0) && (strcmp(mtds->__ptr[i].mtdLogicalOperator, "OR")!=0))
				{
					SOAP_FOK(BMDSOAP_SERVER_ADVANCED_SEARCH_INVALID_LOG_OPER);
				}

				/************************************/
				/* walidacja argumentów dopasowania */
				/************************************/
				if(mtds->__ptr[i].mtdAdjustment != NULL)
				{
					if (	(strcmp(mtds->__ptr[i].mtdAdjustment, "NONE")!=0) &&
						(strcmp(mtds->__ptr[i].mtdAdjustment, "BEGIN")!=0)  &&
						(strcmp(mtds->__ptr[i].mtdAdjustment, "END")!=0)  &&
						(strcmp(mtds->__ptr[i].mtdAdjustment, "BOTH")!=0))
					{
						SOAP_FOK(BMDSOAP_SERVER_ADVANCED_SEARCH_INVALID_ADJUSTMENT_ARG);
					}
				}
				else
				{
					SOAP_FOK(BMDSOAP_SERVER_ADVANCED_SEARCH_INVALID_ADJUSTMENT_ARG);
				}

				/************************************/
				/* walidacja argumentów dopasowania */
				/************************************/
				if(mtds->__ptr[i].mtdStatement != NULL)
				{
					if (	(strcmp(mtds->__ptr[i].mtdStatement, "NEGATIVE")!=0) &&
						(strcmp(mtds->__ptr[i].mtdStatement, "POSITIVE")!=0))
					{
						SOAP_FOK(BMDSOAP_SERVER_ADVANCED_SEARCH_INVALID_STATEMENT);
					}
				}
				else
				{
					SOAP_FOK(BMDSOAP_SERVER_ADVANCED_SEARCH_INVALID_STATEMENT);
				}
			}
		}
	}

	/***************************************/
	/* wczytanie ustawien konfiguracyjnych */
	/***************************************/
	if (bmdconf_get_value_int(_GLOBAL_konfiguracja,"soap","max_metadata_value_length",&max_metadata_value_length)<BMD_OK)
	{
		max_metadata_value_length=256;
	}

	bmdconf_get_value_int(_GLOBAL_konfiguracja,"bmd","max_datagram_in_set_transmission", &max_datagram_in_set_transmission);
	if(max_datagram_in_set_transmission <= 0)
	{
		max_datagram_in_set_transmission = 100;
	}

	bmdconf_get_value_int(_GLOBAL_konfiguracja, "bmd", "deserialisation_max_memory", &deserialisation_max_memory);
	if(deserialisation_max_memory < 0)
	{
		deserialisation_max_memory = 0;
	}
	else
	{
		deserialisation_max_memory = deserialisation_max_memory * 1024 * 1024;
	}

	if(_GLOBAL_soap_wssecurity_enabled == 1)
	{
		retVal=GetWSSEInformation(soap, NULL, NULL, NULL, &derUserCert, &derUserCertLen);
		if(retVal < 0)
		{
			PRINT_ERROR("Unable to get certificate from WS Security header %li\n", retVal);
			SOAP_FOK(BMDSOAP_SERVER_NO_WSSECURITY_CERTIFICATE);
		}
	}
	else
	{
		if(_GLOBAL_soap_certificate_required == 1)
		{
			if(cert == NULL)
				{ SOAP_FOK(BMDSOAP_SERVER_EMPTY_LOGIN_CERTIFICATE); }
			else
			{
				paramCert=(char*)cert->__ptr;
				paramCertLen=(long)cert->__size;
			}
		}
		else
		{
			if(cert != NULL)
			{
				paramCert=(char*)cert->__ptr;
				paramCertLen=(long)cert->__size;
			}
		}
	}

	/********************/
	/* logowanie do bmd */
	/********************/
	SOAP_FOK(PrepareDataForBMDLoginAndRequest(&bi,
			(_GLOBAL_soap_wssecurity_enabled == 1  ? derUserCert : paramCert),
			(_GLOBAL_soap_wssecurity_enabled == 1  ? derUserCertLen : paramCertLen),
			roleName, groupName, deserialisation_max_memory));
	free(derUserCert); derUserCert=NULL;
	paramCert=NULL; paramCertLen=0;

	/********************************/
	/*	stworzenie datagramu	*/
	/********************************/
	if ((range==NULL) || (strcmp(range,"DIR")!=0))
	{
		SOAP_FOK_LOGOUT(bmd_datagram_create(BMD_DATAGRAM_TYPE_SEARCH,&dtg));
	}
	else
	{
		SOAP_FOK_LOGOUT(bmd_datagram_create(BMD_DATAGRAM_DIR_LIST, &dtg));
	}
	if (userClassId!=NULL)
	{
		SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(	OID_ACTION_METADATA_CERT_CHOSEN_CLASS_ID, userClassId, &dtg));
	}

	/*************************************/
	/* ustawienie kryteriow wyszukiwania */
	/*************************************/
	if(mtds!=NULL)
	{
		for(i=0;i<mtds->__size;i++)
		{
			if (mtds->__ptr!=NULL)
			{
				if ( (mtds->__ptr[i].mtdValue!=NULL) && (mtds->__ptr[i].mtdOid!=NULL) )
				{
					/*********************************/
					/* usatwienie wartosci metadanej */
					/*********************************/
					tmp_gb.size=strlen(mtds->__ptr[i].mtdValue);
					tmp_gb.buf=strndup(mtds->__ptr[i].mtdValue, tmp_gb.size);
					if(tmp_gb.buf == NULL)
					{
						tmp_gb.size=0;
						SOAP_FOK_LOGOUT(BMD_ERR_MEMORY);
					}
					if (tmp_gb.size>max_metadata_value_length)	{	SOAP_FOK_LOGOUT(BMDSOAP_SERVER_METADATA_VALUE_TOO_LONG);	}

					if (mtds->__ptr[i].mtdDesc!=NULL)
					{
						if (strcmp(mtds->__ptr[i].mtdDesc,"LIKE")==0)		tmpDesc = BMD_REQ_DB_SELECT_ADVANCED_LIKE_QUERY;
						else if (strcmp(mtds->__ptr[i].mtdDesc,"ILIKE")==0)	tmpDesc = BMD_REQ_DB_SELECT_ADVANCED_ILIKE_QUERY;
						else									tmpDesc = 0;
					}
					else									tmpDesc = 0;

					if (mtds->__ptr[i].mtdLogicalOperator!=NULL)
					{
						if (strcmp(mtds->__ptr[i].mtdLogicalOperator, "OR")==0) 		tmpOperator = BMD_REQ_DB_SELECT_ADVANCED_LOGICAL_OR;
						else if (strcmp(mtds->__ptr[i].mtdLogicalOperator, "AND")==0) 	tmpOperator = BMD_REQ_DB_SELECT_ADVANCED_LOGICAL_AND;
						else 											tmpOperator = 0;
					}
					else 											tmpOperator = 0;

					if (mtds->__ptr[i].mtdAdjustment!=NULL)
					{
						if (strcmp(mtds->__ptr[i].mtdAdjustment, "NONE")==0)			tmpAdjustment = BMD_REQ_DB_SELECT_ADVANCED_PERC_NONE;
						else if (strcmp(mtds->__ptr[i].mtdAdjustment, "BEGIN")==0)		tmpAdjustment = BMD_REQ_DB_SELECT_ADVANCED_PERC_BEGIN;
						else if (strcmp(mtds->__ptr[i].mtdAdjustment, "END")==0)		tmpAdjustment = BMD_REQ_DB_SELECT_ADVANCED_PERC_END;
						else if (strcmp(mtds->__ptr[i].mtdAdjustment, "BOTH")==0)		tmpAdjustment = BMD_REQ_DB_SELECT_ADVANCED_PERC_BOTH;
						else											tmpAdjustment = 0;
					}
					else											tmpAdjustment = 0;

					if (mtds->__ptr[i].mtdStatement!=NULL)
					{
						if (strcmp(mtds->__ptr[i].mtdStatement, "POSITIVE")==0)		tmpStatement = 1;
						else if (strcmp(mtds->__ptr[i].mtdStatement, "NEGATIVE")==0)	tmpStatement = -1;
						else 											tmpStatement = 1;
					}
					else 											tmpStatement = 1;

					mtdPrefix = tmpOperator + mtds->__ptr[i].noOfOpenBrackets;
					mtdPostfix = (tmpAdjustment + mtds->__ptr[i].noOfCloseBrackets)*tmpStatement;

					/**********************************/
					/* dodanie metadanej do datagramu */
					/**********************************/
					SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_2_with_params(mtds->__ptr[i].mtdOid, &tmp_gb,&dtg, ADDITIONAL_METADATA, tmpDesc, mtdPrefix, mtdPostfix,1));
					//bindowanie wskaznika
					tmp_gb.buf=NULL; tmp_gb.size=0;
				}
			}
		}
	}

	if( ownerRange!=NULL && strcmp(ownerRange, "GROUP")==0 )
	{
		SOAP_FOK_LOGOUT(bmd_datagram_set_status(BMD_ONLY_GROUP_DOC,BMD_QUERY_LIKE,&dtg));
	}
	else if( ownerRange!=NULL && strcmp(ownerRange, "CREATOR")==0 )
	{
		SOAP_FOK_LOGOUT(bmd_datagram_set_status(BMD_ONLY_CREATOR_DOC,BMD_QUERY_LIKE,&dtg));
	}
	else if( ownerRange!=NULL && strcmp(ownerRange, "OWNER_OR_CREATOR")==0 )
	{
		SOAP_FOK_LOGOUT(bmd_datagram_set_status(BMD_OWNER_OR_CREATOR_DOC,BMD_QUERY_LIKE,&dtg));
	}
	else // OWNER i kazda inna wartosc
	{
		SOAP_FOK_LOGOUT(bmd_datagram_set_status(BMD_ONLY_OWNER_DOC,BMD_QUERY_LIKE,&dtg));
	}

	/************************************/
	/*	ustawienie trybu sortowania	*/
	/************************************/
	if (sortInfo!=NULL)
	{
		for (j=0; j<sortInfo->__size; j++)
		{
			sort_oid=strdup(sortInfo->__ptr[j].sortOid);
			sort_order=strdup(sortInfo->__ptr[j].sortOrder);
			
			if (sort_oid!=NULL)
			{
				if (sort_order!=NULL)
				{
					if (sort_str==NULL)
					{
						asprintf(&sort_str, "%s|%s", sort_oid, sort_order);
					}
					else
					{
						tempSortStr=sort_str; sort_str=NULL;
						asprintf(&sort_str, "%s|%s|%s", tempSortStr, sort_oid, sort_order);
						free(tempSortStr); tempSortStr=NULL;
					}
					
					if(sort_str == NULL)
					{
						SOAP_FOK_LOGOUT(NO_MEMORY);
					}
				}
				else
				{
					if (sort_str==NULL)
					{
						asprintf(&sort_str, "%s|%s", sort_oid, "ASC");
					}
					else
					{
						tempSortStr=sort_str; sort_str=NULL;
						asprintf(&sort_str, "%s|%s|%s", tempSortStr, sort_oid, "ASC");
						free(tempSortStr); tempSortStr=NULL;
					}
					
					if(sort_str == NULL)
					{
						SOAP_FOK_LOGOUT(NO_MEMORY);
					}
				}
			}
			
			free(sort_oid); sort_oid=NULL;
			free(sort_order); sort_order=NULL;
		}
		
		if(sort_str != NULL)
		{
			dtg->protocolData=(GenBuf_t *)calloc(1, sizeof(GenBuf_t));
			if(dtg->protocolData == NULL)
			{
				SOAP_FOK_LOGOUT(NO_MEMORY);
			}
			dtg->protocolData->buf=sort_str;
			sort_str=NULL;
			dtg->protocolData->size=strlen(dtg->protocolData->buf);
		}
	}

	/******************************************/
	/*	ustawienie limitow wyszukiwania	*/
	/******************************************/
	SOAP_FOK_LOGOUT(bmd_datagram_add_limits(offset,limit,&dtg));

	/************************************/
	/*	dodanie datagramu do zestawu	*/
	/************************************/
	SOAP_FOK_LOGOUT(bmd_datagram_add_to_set(dtg,&dtg_set));

	/******************************************/
	/*	wyslanie zadania do serwera bmd	*/
	/******************************************/
	SOAP_FOK_LOGOUT(bmd_send_request(bi, dtg_set, max_datagram_in_set_transmission, &resp_dtg_set,1, deserialisation_max_memory));
	bmd_datagramset_free(&dtg_set);
	bmd_logout(&bi, deserialisation_max_memory);
	bmd_info_destroy(&bi);
	bmd_end();
	SOAP_FOK(resp_dtg_set->bmdDatagramSetTable[0]->datagramStatus);

	/************************************************************/
	/*	alokacja pamieci na wyniki wyszukiwania (na drzewo)	*/
	/************************************************************/
	(*searchResults)=(struct bmd230__searchResults *)soap_malloc(soap,sizeof(struct bmd230__searchResults));
	if((*searchResults) == NULL)
	{
		SOAP_FOK(NO_MEMORY);
	}
	memset(*searchResults,0,sizeof(struct bmd230__searchResults));

	/******************************************************************************/
	/*	alokacja pamieci na strukture odpowiedzi - matadane wszystkich wynikow	*/
	/******************************************************************************/
	(*searchResults)->__size=resp_dtg_set->bmdDatagramSetSize;
	(*searchResults)->__ptr=(struct bmd230__searchSingleResult *)soap_malloc(soap, ((*searchResults)->__size)*sizeof(struct bmd230__searchSingleResult));
	if((*searchResults)->__ptr == NULL)
	{
		(*searchResults)->__size=0;
		SOAP_FOK(NO_MEMORY);
	}
	memset((*searchResults)->__ptr, 0, ((*searchResults)->__size)*sizeof(struct bmd230__searchSingleResult));

	/****************************************/
	/*	wszystkie znalezione pliki	*/
	/****************************************/
	for(i=0; i<resp_dtg_set->bmdDatagramSetSize; i++)
	{/* for(i=0; i<resp_dtg_set->bmdDatagramSetSize; i++)*/

		(*searchResults)->__ptr[i].filename=(char*)soap_malloc(soap, (resp_dtg_set->bmdDatagramSetTable[i]->protocolDataFilename->size + 1) * sizeof(char));
		if((*searchResults)->__ptr[i].filename == NULL)
		{
			SOAP_FOK(NO_MEMORY);
		}
		memset((*searchResults)->__ptr[i].filename, 0, (resp_dtg_set->bmdDatagramSetTable[i]->protocolDataFilename->size + 1) * sizeof(char));
		memcpy((*searchResults)->__ptr[i].filename, resp_dtg_set->bmdDatagramSetTable[i]->protocolDataFilename->buf, resp_dtg_set->bmdDatagramSetTable[i]->protocolDataFilename->size);
		

		(*searchResults)->__ptr[i].id = atoi((char *)resp_dtg_set->bmdDatagramSetTable[i]->protocolDataFileId->buf);
		(*searchResults)->__ptr[i].mtds=(struct bmd230__mtdsValues *)soap_malloc(soap, sizeof(struct bmd230__mtdsValues));
		if((*searchResults)->__ptr[i].mtds == NULL)
		{
			SOAP_FOK(NO_MEMORY);
		}
		memset((*searchResults)->__ptr[i].mtds, 0, sizeof(struct bmd230__mtdsValues));
		
		if(formMtds == NULL)
		{

			form_mtds_count = resp_dtg_set->bmdDatagramSetTable[i]->no_of_sysMetaData + \
			resp_dtg_set->bmdDatagramSetTable[i]->no_of_actionMetaData + \
			resp_dtg_set->bmdDatagramSetTable[i]->no_of_additionalMetaData;

			(*searchResults)->__ptr[i].mtds->__size = form_mtds_count;
			(*searchResults)->__ptr[i].mtds->__ptr=(struct bmd230__mtdSingleValue *)soap_malloc(soap, ((*searchResults)->__ptr[i].mtds->__size) * sizeof(struct bmd230__mtdSingleValue));
			if((*searchResults)->__ptr[i].mtds->__ptr == NULL)
			{
				(*searchResults)->__ptr[i].mtds->__size=0;
				SOAP_FOK(NO_MEMORY);
			}
			memset((*searchResults)->__ptr[i].mtds->__ptr, 0, ((*searchResults)->__ptr[i].mtds->__size) * sizeof(struct bmd230__mtdSingleValue));
			form_mtds_count = 1;
		}
		else
		{
			form_mtds_count = formMtds->__size;
			if(form_mtds_count == 0)
			{
				form_mtds_count = resp_dtg_set->bmdDatagramSetTable[i]->no_of_sysMetaData + \
				resp_dtg_set->bmdDatagramSetTable[i]->no_of_actionMetaData + \
				resp_dtg_set->bmdDatagramSetTable[i]->no_of_additionalMetaData;
				formMtds = NULL;
			}
			
			(*searchResults)->__ptr[i].mtds->__size = form_mtds_count;
			(*searchResults)->__ptr[i].mtds->__ptr=(struct bmd230__mtdSingleValue *)soap_malloc(soap, ((*searchResults)->__ptr[i].mtds->__size) * sizeof(struct bmd230__mtdSingleValue));
			if((*searchResults)->__ptr[i].mtds->__ptr == NULL)
			{
				(*searchResults)->__ptr[i].mtds->__size=0;
				SOAP_FOK(NO_MEMORY);
			}
			memset((*searchResults)->__ptr[i].mtds->__ptr, 0, ((*searchResults)->__ptr[i].mtds->__size) * sizeof(struct bmd230__mtdSingleValue));
			
			if(formMtds == NULL)
			{
				form_mtds_count=1;
			}
		}

		mtd_result_count = 0;

		for(k=0; k < form_mtds_count; k++)
		{
			/********************************************/
			/* Kopiuje metadane systemowe do odpowiedzi */
			/********************************************/
			if (resp_dtg_set->bmdDatagramSetTable[i]->no_of_sysMetaData>0)
			{
				SOAP_FOK( MtdsListToResponse(	soap,
							resp_dtg_set->bmdDatagramSetTable[i]->sysMetaData,
							resp_dtg_set->bmdDatagramSetTable[i]->no_of_sysMetaData,
							(*searchResults)->__ptr[i].mtds,
							formMtds,
							k,
							&mtd_result_count) );
			}

			/********************************************/
			/* Kopiuje metadane dodatkowe do odpowiedzi */
			/********************************************/
			if (resp_dtg_set->bmdDatagramSetTable[i]->no_of_additionalMetaData>0)
			{
				SOAP_FOK( MtdsListToResponse(  soap,
							resp_dtg_set->bmdDatagramSetTable[i]->additionalMetaData,
							resp_dtg_set->bmdDatagramSetTable[i]->no_of_additionalMetaData,
							(*searchResults)->__ptr[i].mtds,
							formMtds,
							k,
							&mtd_result_count) );
			}

			/******************************************/
			/* Kopiuje metadane actions do odpowiedzi */
			/******************************************/
			if (resp_dtg_set->bmdDatagramSetTable[i]->no_of_actionMetaData>0)
			{
				SOAP_FOK( MtdsListToResponse(  soap,
							resp_dtg_set->bmdDatagramSetTable[i]->actionMetaData,
							resp_dtg_set->bmdDatagramSetTable[i]->no_of_actionMetaData,
							(*searchResults)->__ptr[i].mtds,
							formMtds,
							k,
							&mtd_result_count) );
			}
		}

		if (mtd_result_count==0)
		{
			(*searchResults)->__ptr[i].mtds->__ptr[mtd_result_count].mtdOid=(char *)soap_malloc(soap,2*sizeof(char));
			if((*searchResults)->__ptr[i].mtds->__ptr[mtd_result_count].mtdOid == NULL)
			{
				SOAP_FOK(NO_MEMORY);
			}
			memset((*searchResults)->__ptr[i].mtds->__ptr[mtd_result_count].mtdOid,0,2*sizeof(char));

			(*searchResults)->__ptr[i].mtds->__ptr[mtd_result_count].mtdValue=(char *)soap_malloc(soap,2*sizeof(char));
			if((*searchResults)->__ptr[i].mtds->__ptr[mtd_result_count].mtdValue == NULL)
			{
				SOAP_FOK(NO_MEMORY);
			}
			memset((*searchResults)->__ptr[i].mtds->__ptr[mtd_result_count].mtdValue,0,2*sizeof(char));

			(*searchResults)->__ptr[i].mtds->__ptr[mtd_result_count].mtdDesc=(char *)soap_malloc(soap,2*sizeof(char));
			if((*searchResults)->__ptr[i].mtds->__ptr[mtd_result_count].mtdDesc == NULL)
			{
				SOAP_FOK(NO_MEMORY);
			}
			memset((*searchResults)->__ptr[i].mtds->__ptr[mtd_result_count].mtdDesc,0,2*sizeof(char));
		}
		(*searchResults)->__ptr[i].mtds->__size=mtd_result_count;

	}

	PRINT_SOAP("Status:\t\trequest served correctly\n");
	bmd_datagramset_free(&resp_dtg_set);
	

	/************************/
	/*	porzadki	*/
	/************************/
	free0(metadata_oid_str);
	free_gen_buf(&metadata_value_buf);

	return SOAP_OK;
}


int bmd230__bmdAdvancedCountFiles(	struct soap *soap,
				struct xsd__base64Binary *cert,
				char *roleName,
				char *groupName,
				char *userClassId,
				struct bmd230__mtdsSearchValues *mtds,
				char *ownerRange,
				long *result)

{
long i					= 0;
long max_metadata_value_length		= 0;
long tmpOperator			= 0;
long tmpDesc				= 0;
long tmpAdjustment			= 0;
long tmpStatement			= 0;
long mtdPrefix				= 0;
long mtdPostfix				= 0;
long max_datagram_in_set_transmission	= 0;
long deserialisation_max_memory		= 0;
bmd_info_t *bi				= NULL;
bmdDatagram_t *dtg			= NULL;
bmdDatagramSet_t *dtg_set		= NULL;
bmdDatagramSet_t *resp_dtg_set		= NULL;
char *metadata_oid_str			= NULL;
GenBuf_t *metadata_value_buf		= NULL;
GenBuf_t tmp_gb;

char *derUserCert=NULL;
long derUserCertLen=0;
long retVal=0;
char* paramCert=NULL;
long paramCertLen=0;


	PRINT_SOAP("Request:\tcounting documents\n");
	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if(mtds!=NULL)
	{
		for(i=0;i<mtds->__size;i++)
		{
			if (mtds->__ptr!=NULL)
			{
				/******BMD_ONLY_GROUP_DOC*****************************/
				/* walidacja operatora dopasowania */
				/***********************************/
				if(mtds->__ptr[i].mtdDesc != NULL)
				{
					if (	(strcmp(mtds->__ptr[i].mtdDesc, "LIKE")!=0) &&
						(strcmp(mtds->__ptr[i].mtdDesc, "ILIKE")!=0))
					{
						SOAP_FOK(BMDSOAP_SERVER_ADVANCED_SEARCH_INVALID_ADJUSTMENT_OPER);
					}
				}
				else
				{
					SOAP_FOK(BMDSOAP_SERVER_ADVANCED_SEARCH_INVALID_ADJUSTMENT_OPER);
				}

				/**********************************/
				/* walidacja operatora logicznego */
				/**********************************/
				if ((i==0) && (mtds->__ptr[i].mtdLogicalOperator!=NULL))
				{
					SOAP_FOK(BMDSOAP_SERVER_ADVANCED_SEARCH_INVALID_FIRST_LOG_OPER);
				}
				else if ((i!=0) && (mtds->__ptr[i].mtdLogicalOperator==NULL))
				{
					SOAP_FOK(BMDSOAP_SERVER_ADVANCED_SEARCH_INVALID_LOG_OPER_MISSING);
				}
				else if ((i!=0) && (strcmp(mtds->__ptr[i].mtdLogicalOperator, "AND")!=0) && (strcmp(mtds->__ptr[i].mtdLogicalOperator, "OR")!=0))
				{
					SOAP_FOK(BMDSOAP_SERVER_ADVANCED_SEARCH_INVALID_LOG_OPER);
				}

				/************************************/
				/* walidacja argumentów dopasowania */
				/************************************/
				if(mtds->__ptr[i].mtdAdjustment != NULL)
				{
					if (	(strcmp(mtds->__ptr[i].mtdAdjustment, "NONE")!=0) &&
						(strcmp(mtds->__ptr[i].mtdAdjustment, "BEGIN")!=0)  &&
						(strcmp(mtds->__ptr[i].mtdAdjustment, "END")!=0)  &&
						(strcmp(mtds->__ptr[i].mtdAdjustment, "BOTH")!=0))
					{
						SOAP_FOK(BMDSOAP_SERVER_ADVANCED_SEARCH_INVALID_ADJUSTMENT_ARG);
					}
				}
				else
				{
					SOAP_FOK(BMDSOAP_SERVER_ADVANCED_SEARCH_INVALID_ADJUSTMENT_ARG);
				}

				/************************************/
				/* walidacja argumentów dopasowania */
				/************************************/
				if(mtds->__ptr[i].mtdStatement != NULL)
				{
					if (	(strcmp(mtds->__ptr[i].mtdStatement, "NEGATIVE")!=0) &&
						(strcmp(mtds->__ptr[i].mtdStatement, "POSITIVE")!=0))
					{
						SOAP_FOK(BMDSOAP_SERVER_ADVANCED_SEARCH_INVALID_STATEMENT);
					}
				}
				else
				{
					SOAP_FOK(BMDSOAP_SERVER_ADVANCED_SEARCH_INVALID_STATEMENT);
				}
			}
		}
	}

	/***************************************/
	/* wczytanie ustawien konfiguracyjnych */
	/***************************************/
	if (bmdconf_get_value_int(_GLOBAL_konfiguracja,"soap","max_metadata_value_length",&max_metadata_value_length)<BMD_OK)
	{
		max_metadata_value_length=256;
	}

	bmdconf_get_value_int(_GLOBAL_konfiguracja,"bmd","max_datagram_in_set_transmission", &max_datagram_in_set_transmission);
	if(max_datagram_in_set_transmission <= 0)
	{
		max_datagram_in_set_transmission = 100;
	}

	bmdconf_get_value_int(_GLOBAL_konfiguracja, "bmd", "deserialisation_max_memory", &deserialisation_max_memory);
	if(deserialisation_max_memory < 0)
	{
		deserialisation_max_memory = 0;
	}
	else
	{
		deserialisation_max_memory = deserialisation_max_memory * 1024 * 1024;
	}

	if(_GLOBAL_soap_wssecurity_enabled == 1)
	{
		retVal=GetWSSEInformation(soap, NULL, NULL, NULL, &derUserCert, &derUserCertLen);
		if(retVal < 0)
		{
			PRINT_ERROR("Unable to get certificate from WS Security header %li\n", retVal);
			SOAP_FOK(BMDSOAP_SERVER_NO_WSSECURITY_CERTIFICATE);
		}
	}
	else
	{
		if(_GLOBAL_soap_certificate_required == 1)
		{
			if(cert == NULL)
				{ SOAP_FOK(BMDSOAP_SERVER_EMPTY_LOGIN_CERTIFICATE); }
			else
			{
				paramCert=(char*)cert->__ptr;
				paramCertLen=(long)cert->__size;
			}
		}
		else
		{
			if(cert != NULL)
			{
				paramCert=(char*)cert->__ptr;
				paramCertLen=(long)cert->__size;
			}
		}
	}

	/********************/
	/* logowanie do bmd */
	/********************/
	SOAP_FOK(PrepareDataForBMDLoginAndRequest(&bi,
			(_GLOBAL_soap_wssecurity_enabled == 1  ? derUserCert : paramCert),
			(_GLOBAL_soap_wssecurity_enabled == 1  ? derUserCertLen : paramCertLen),
			roleName, groupName, deserialisation_max_memory));
	free(derUserCert); derUserCert=NULL;
	paramCert=NULL; paramCertLen=0;

	/********************************/
	/*	stworzenie datagramu	*/
	/********************************/
	SOAP_FOK_LOGOUT(bmd_datagram_create(BMD_DATAGRAM_TYPE_COUNT_DOCS, &dtg));
	if (userClassId!=NULL)
	{
		SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(	OID_ACTION_METADATA_CERT_CHOSEN_CLASS_ID, userClassId, &dtg));
	}

	/*************************************/
	/* ustawienie kryteriow wyszukiwania */
	/*************************************/
	if(mtds!=NULL)
	{
		for(i=0;i<mtds->__size;i++)
		{
			if (mtds->__ptr!=NULL)
			{
				if ( (mtds->__ptr[i].mtdValue!=NULL) && (mtds->__ptr[i].mtdOid!=NULL) )
				{
					/*********************************/
					/* usatwienie wartosci metadanej */
					/*********************************/
					tmp_gb.size=strlen(mtds->__ptr[i].mtdValue);
					tmp_gb.buf=strndup(mtds->__ptr[i].mtdValue, tmp_gb.size);
					if(tmp_gb.buf == NULL)
					{
						tmp_gb.size=0;
						SOAP_FOK_LOGOUT(BMD_ERR_MEMORY);
					}
					if (tmp_gb.size>max_metadata_value_length)	{	SOAP_FOK_LOGOUT(BMDSOAP_SERVER_METADATA_VALUE_TOO_LONG);	}

					if (mtds->__ptr[i].mtdDesc!=NULL)
					{
						if (strcmp(mtds->__ptr[i].mtdDesc,"LIKE")==0)		tmpDesc = BMD_REQ_DB_SELECT_ADVANCED_LIKE_QUERY;
						else if (strcmp(mtds->__ptr[i].mtdDesc,"ILIKE")==0)	tmpDesc = BMD_REQ_DB_SELECT_ADVANCED_ILIKE_QUERY;
						else									tmpDesc = 0;
					}
					else									tmpDesc = 0;

					if (mtds->__ptr[i].mtdLogicalOperator!=NULL)
					{
						if (strcmp(mtds->__ptr[i].mtdLogicalOperator, "OR")==0) 		tmpOperator = BMD_REQ_DB_SELECT_ADVANCED_LOGICAL_OR;
						else if (strcmp(mtds->__ptr[i].mtdLogicalOperator, "AND")==0) 	tmpOperator = BMD_REQ_DB_SELECT_ADVANCED_LOGICAL_AND;
						else 											tmpOperator = 0;
					}
					else 											tmpOperator = 0;

					if (mtds->__ptr[i].mtdAdjustment!=NULL)
					{
						if (strcmp(mtds->__ptr[i].mtdAdjustment, "NONE")==0)			tmpAdjustment = BMD_REQ_DB_SELECT_ADVANCED_PERC_NONE;
						else if (strcmp(mtds->__ptr[i].mtdAdjustment, "BEGIN")==0)		tmpAdjustment = BMD_REQ_DB_SELECT_ADVANCED_PERC_BEGIN;
						else if (strcmp(mtds->__ptr[i].mtdAdjustment, "END")==0)		tmpAdjustment = BMD_REQ_DB_SELECT_ADVANCED_PERC_END;
						else if (strcmp(mtds->__ptr[i].mtdAdjustment, "BOTH")==0)		tmpAdjustment = BMD_REQ_DB_SELECT_ADVANCED_PERC_BOTH;
						else											tmpAdjustment = 0;
					}
					else											tmpAdjustment = 0;

					if (mtds->__ptr[i].mtdStatement!=NULL)
					{
						if (strcmp(mtds->__ptr[i].mtdStatement, "POSITIVE")==0)		tmpStatement = 1;
						else if (strcmp(mtds->__ptr[i].mtdStatement, "NEGATIVE")==0)	tmpStatement = -1;
						else 											tmpStatement = 1;
					}
					else 											tmpStatement = 1;

					mtdPrefix = tmpOperator + mtds->__ptr[i].noOfOpenBrackets;
					mtdPostfix = (tmpAdjustment + mtds->__ptr[i].noOfCloseBrackets)*tmpStatement;

					/**********************************/
					/* dodanie metadanej do datagramu */
					/**********************************/
					SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_2_with_params(mtds->__ptr[i].mtdOid, &tmp_gb,&dtg, ADDITIONAL_METADATA, tmpDesc, mtdPrefix, mtdPostfix,1));
					//bindowanie wskaznika
					tmp_gb.buf=NULL; tmp_gb.size=0;
				}
			}
		}
	}

	if( ownerRange!=NULL && strcmp(ownerRange, "GROUP")==0 )
	{
		SOAP_FOK_LOGOUT(bmd_datagram_set_status(BMD_ONLY_GROUP_DOC,BMD_QUERY_LIKE,&dtg));
	}
	else if( ownerRange!=NULL && strcmp(ownerRange, "CREATOR")==0 )
	{
		SOAP_FOK_LOGOUT(bmd_datagram_set_status(BMD_ONLY_CREATOR_DOC,BMD_QUERY_LIKE,&dtg));
	}
	else if( ownerRange!=NULL && strcmp(ownerRange, "OWNER_OR_CREATOR")==0 )
	{
		SOAP_FOK_LOGOUT(bmd_datagram_set_status(BMD_OWNER_OR_CREATOR_DOC,BMD_QUERY_LIKE,&dtg));
	}
	else // OWNER i kazda inna wartosc
	{
		SOAP_FOK_LOGOUT(bmd_datagram_set_status(BMD_ONLY_OWNER_DOC,BMD_QUERY_LIKE,&dtg));
	}

	/********************************/
	/* dodanie datagramu do zestawu */
	/********************************/
	SOAP_FOK_LOGOUT(bmd_datagram_add_to_set(dtg,&dtg_set));

	/***********************************/
	/* wyslanie zadania do serwera bmd */
	/***********************************/
	SOAP_FOK_LOGOUT(bmd_send_request(bi, dtg_set, max_datagram_in_set_transmission, &resp_dtg_set,1, deserialisation_max_memory));
	bmd_datagramset_free(&dtg_set);
	SOAP_FOK_LOGOUT(resp_dtg_set->bmdDatagramSetTable[0]->datagramStatus);

	/*******************************************************/
	/* alokacja pamieci na wyniki wyszukiwania (na drzewo) */
	/*******************************************************/
	(*result)=resp_dtg_set->bmdDatagramSetTable[0]->filesRemaining;

	PRINT_SOAP("Status:\t\trequest served correctly\n");
	bmd_datagramset_free(&resp_dtg_set);
	bmd_logout(&bi, deserialisation_max_memory);
	bmd_end();

	/************************/
	/*	porzadki	*/
	/************************/
	free0(metadata_oid_str);
	free_gen_buf(&metadata_value_buf);

	return SOAP_OK;
}


/*****************************************************************************/
/* obsluzenie zadania wyszukiwania plikow w BMD wedlug określonych kryteriow */
/*****************************************************************************/
int bmd230__bmdSearchFiles(		struct soap *soap,
				struct xsd__base64Binary *cert,
				char *roleName,
				char *groupName,
				char *userClassId,
				struct bmd230__mtdsValues *mtds,
				struct bmd230__mtdsValues *formMtds,
				int offset,
				int limit,
				char *logical,
				char *oper,
				char *range,
				char *ownerRange,
				struct bmd230__searchSortInfoList *sortInfo,
				struct bmd230__searchResults **searchResults)
{

long i					= 0;
long j					= 0;
long k					= 0;
long mtd_result_count			= 0;
long form_mtds_count			= 0;
long max_datagram_in_set_transmission	= 0;
long deserialisation_max_memory		= 0;
long max_metadata_value_length		= 0;
bmd_info_t *bi				= NULL;
bmdDatagram_t *dtg			= NULL;
bmdDatagramSet_t *dtg_set		= NULL;
bmdDatagramSet_t *resp_dtg_set	= NULL;
char *sort_str				= NULL;
char* tempSortStr			= NULL;
char *sort_oid				= NULL;
char *sort_order				= NULL;
char *metadata_oid_str			= NULL;
GenBuf_t *metadata_value_buf		= NULL;
GenBuf_t tmp_gb;
long chosenOper				= 0;
long chosenOwnerRange			= 0;

char *derUserCert=NULL;
long derUserCertLen=0;
long retVal=0;
char* paramCert=NULL;
long paramCertLen=0;

	PRINT_SOAP("Request:\tsearch for documents\n");
	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if (oper==NULL)					{	SOAP_FOK(BMDSOAP_SERVER_SEARCH_OPERATOR_INVALID);	}
	if (	(strcmp(oper,"LIKE")!=0) &&
		(strcmp(oper,"ILIKE")!=0) &&
		(strcmp(oper,"EXACT")!=0) &&
		(strcmp(oper,"EQUAL")!=0)	)	{	SOAP_FOK(BMDSOAP_SERVER_SEARCH_OPERATOR_INVALID);	}
	if (logical==NULL)				{	SOAP_FOK(BMDSOAP_SERVER_SEARCH_LOGICAL_INVALID);	}
	if (	(strcmp(logical,"OR")!=0) &&
		(strcmp(logical,"AND")!=0)	)	{	SOAP_FOK(BMDSOAP_SERVER_SEARCH_LOGICAL_INVALID);	}
	if ((offset<0) || (limit<1))			{	SOAP_FOK(BMDSOAP_SERVER_INVALID_LIMIT_VALUE);		}

	bmdconf_get_value_int(_GLOBAL_konfiguracja,"bmd","max_datagram_in_set_transmission", &max_datagram_in_set_transmission);
	if(max_datagram_in_set_transmission <= 0)
	{
		max_datagram_in_set_transmission = 100;
	}

	if (sortInfo!=NULL)
	{
		for (j=0; j<sortInfo->__size; j++)
		{
			if (sortInfo->__ptr[j].sortOrder!=NULL)
			{
				if ((strcmp(sortInfo->__ptr[j].sortOrder,"ASC")!=0) && \
				(strcmp(sortInfo->__ptr[j].sortOrder,"DESC")!=0))
				{
					SOAP_FOK(BMDSOAP_SERVER_SEARCH_SORT_ORDER_INVALID);
				}
			}
			else
			{
				SOAP_FOK(BMDSOAP_SERVER_SEARCH_SORT_ORDER_INVALID);
			}

			if (sortInfo->__ptr[j].sortOid!=NULL)
			{
				for (i=0; i<strlen(sortInfo->__ptr[j].sortOid); i++)
				{
					if (((sortInfo->__ptr[j].sortOid[i]<'0') || (sortInfo->__ptr[j].sortOid[i]>'9')) \
					&& (sortInfo->__ptr[j].sortOid[i]!='.'))
					{
						SOAP_FOK(BMDSOAP_SERVER_SEARCH_SORT_OID_INVALID);
					}
				}
			}
			else
			{
				SOAP_FOK(BMDSOAP_SERVER_SEARCH_SORT_OID_INVALID);
			}
		}
	}

	/***************************************/
	/* wczytanie ustawien konfiguracyjnych */
	/***************************************/
	if (bmdconf_get_value_int(_GLOBAL_konfiguracja,"soap","max_metadata_value_length",&max_metadata_value_length)<BMD_OK)
	{
		max_metadata_value_length=256;
	}

	bmdconf_get_value_int(_GLOBAL_konfiguracja, "bmd", "deserialisation_max_memory", &deserialisation_max_memory);
	if(deserialisation_max_memory < 0)
	{
		deserialisation_max_memory = 0;
	}
	else
	{
		deserialisation_max_memory = deserialisation_max_memory * 1024 * 1024;
	}

	if(_GLOBAL_soap_wssecurity_enabled == 1)
	{
		retVal=GetWSSEInformation(soap, NULL, NULL, NULL, &derUserCert, &derUserCertLen);
		if(retVal < 0)
		{
			PRINT_ERROR("Unable to get certificate from WS Security header %li\n", retVal);
			SOAP_FOK(BMDSOAP_SERVER_NO_WSSECURITY_CERTIFICATE);
		}
	}
	else
	{
		if(_GLOBAL_soap_certificate_required == 1)
		{
			if(cert == NULL)
				{ SOAP_FOK(BMDSOAP_SERVER_EMPTY_LOGIN_CERTIFICATE); }
			else
			{
				paramCert=(char*)cert->__ptr;
				paramCertLen=(long)cert->__size;
			}
		}
		else
		{
			if(cert != NULL)
			{
				paramCert=(char*)cert->__ptr;
				paramCertLen=(long)cert->__size;
			}
		}
	}

	/********************/
	/* logowanie do bmd */
	/********************/
	SOAP_FOK(PrepareDataForBMDLoginAndRequest(&bi,
			(_GLOBAL_soap_wssecurity_enabled == 1  ? derUserCert : paramCert),
			(_GLOBAL_soap_wssecurity_enabled == 1  ? derUserCertLen : paramCertLen),
			roleName, groupName, deserialisation_max_memory));
	free(derUserCert); derUserCert=NULL;
	paramCert=NULL; paramCertLen=0;

	/************************/
	/* stworzenie datagramu */
	/************************/
	if ((range==NULL) || (strcmp(range,"DIR")!=0))
	{
		if (strcmp(logical,"AND")==0)
		{
			SOAP_FOK_LOGOUT(bmd_datagram_create(BMD_DATAGRAM_TYPE_SEARCH,&dtg));
		}
		else if (strcmp(logical,"OR")==0)
		{
			SOAP_FOK_LOGOUT(bmd_datagram_create(BMD_DATAGRAM_TYPE_SEARCH,&dtg));
		}
	}
	else
	{
		if (strcmp(logical,"AND")==0)
		{
			SOAP_FOK_LOGOUT(bmd_datagram_create(BMD_DATAGRAM_DIR_LIST,&dtg));
		}
		else if (strcmp(logical,"OR")==0)
		{
			if ((mtds!=NULL) && (mtds->__size>0))
			{
				SOAP_FOK_LOGOUT(bmd_datagram_create(BMD_DATAGRAM_DIR_LIST_WITH_OR,&dtg));
			}
			else
			{
				SOAP_FOK_LOGOUT(bmd_datagram_create(BMD_DATAGRAM_DIR_LIST,&dtg));
			}
		}
	}

	if (userClassId!=NULL)
	{
		SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(	OID_ACTION_METADATA_CERT_CHOSEN_CLASS_ID, userClassId, &dtg));
	}

	/*************************************/
	/* ustawienie kryteriow wyszukiwania */
	/*************************************/
	if(mtds!=NULL)
	{
		for(i=0;i<mtds->__size;i++)
		{
			if (mtds->__ptr!=NULL)
			{
				if (mtds->__ptr[i].mtdValue!=NULL)
				{
					tmp_gb.buf=(char *)mtds->__ptr[i].mtdValue;
					tmp_gb.size=strlen(mtds->__ptr[i].mtdValue);

					if (tmp_gb.size>max_metadata_value_length)
					{
						SOAP_FOK_LOGOUT(BMDSOAP_SERVER_METADATA_VALUE_TOO_LONG);
					}

					if (mtds->__ptr[i].mtdDesc!=NULL)
					{
						if (strcmp(mtds->__ptr[i].mtdDesc,"LIKE")==0)
						{
							SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_2_with_params( mtds->__ptr[i].mtdOid,
																&tmp_gb,
																&dtg,
																UNDEFINED_METADATA,
																BMD_REQ_DB_SELECT_LIKE_QUERY,
																0,
																0,
																0));
						}
						else if (strcmp(mtds->__ptr[i].mtdDesc,"ILIKE")==0)
						{
							SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_2_with_params(mtds->__ptr[i].mtdOid,&tmp_gb,&dtg,UNDEFINED_METADATA,BMD_REQ_DB_SELECT_ILIKE_QUERY,0,0, 0));
						}
						else if (strcmp(mtds->__ptr[i].mtdDesc,"EXACT")==0)
						{
							SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_2_with_params(mtds->__ptr[i].mtdOid,&tmp_gb,&dtg,UNDEFINED_METADATA,BMD_REQ_DB_SELECT_EXACT_ILIKE_QUERY,0,0, 0));
						}
						else if (strcmp(mtds->__ptr[i].mtdDesc,"EQUAL")==0)
						{
							SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_2_with_params(mtds->__ptr[i].mtdOid,&tmp_gb,&dtg,UNDEFINED_METADATA,BMD_REQ_DB_SELECT_EQUAL_QUERY,0,0, 0));
						}
						else
						{
							SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_2_with_params(mtds->__ptr[i].mtdOid,&tmp_gb,&dtg,UNDEFINED_METADATA,0,0,0, 0));
						}
					}
					else
					{
						SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_2_with_params(mtds->__ptr[i].mtdOid,&tmp_gb,&dtg,UNDEFINED_METADATA,0,0,0, 0));
					}
				}
			}
		}
	}

	/******************************************/
	/*	ustawienie zakresu wyszukiwania	*/
	/******************************************/
	if( oper!=NULL && strcmp(oper,"ILIKE")==0 )
		{ chosenOper=BMD_QUERY_ILIKE; }
	if( oper!=NULL && strcmp(oper,"EXACT")==0 )
		{ chosenOper=BMD_QUERY_EXACT_ILIKE; }
	if( oper!=NULL && strcmp(oper,"EQUAL")==0 )
		{ chosenOper=BMD_QUERY_EQUAL; }
	else // LIKE i kazda inna wartosc
		{ chosenOper=BMD_QUERY_LIKE; }

	if( ownerRange!=NULL && strcmp(ownerRange, "GROUP")==0 )
		{ chosenOwnerRange=BMD_ONLY_GROUP_DOC; }
	else if( ownerRange!=NULL && strcmp(ownerRange, "CREATOR")==0 )
		{ chosenOwnerRange=BMD_ONLY_CREATOR_DOC; }
	else if( ownerRange!=NULL && strcmp(ownerRange, "OWNER_OR_CREATOR")==0 )
		{ chosenOwnerRange=BMD_OWNER_OR_CREATOR_DOC; }
	else // OWNER i kazda inna wartosc
		{ chosenOwnerRange=BMD_ONLY_OWNER_DOC; }

	SOAP_FOK_LOGOUT(bmd_datagram_set_status(chosenOwnerRange, chosenOper, &dtg));

	/************************************/
	/*	ustawienie trybu sortowania	*/
	/************************************/
	if (sortInfo!=NULL)
	{
		for (j=0; j<sortInfo->__size; j++)
		{
			sort_oid=strdup(sortInfo->__ptr[j].sortOid);
			sort_order=strdup(sortInfo->__ptr[j].sortOrder);
			
			if (sort_oid!=NULL)
			{
				if (sort_order!=NULL)
				{
					if (sort_str==NULL)
					{
						asprintf(&sort_str, "%s|%s", sort_oid, sort_order);
					}
					else
					{
						tempSortStr=sort_str; sort_str=NULL;
						asprintf(&sort_str, "%s|%s|%s", tempSortStr, sort_oid, sort_order);
						free(tempSortStr); tempSortStr=NULL;
					}
					
					if(sort_str == NULL)
					{
						SOAP_FOK_LOGOUT(NO_MEMORY);
					}
				}
				else
				{
					if (sort_str==NULL)
					{
						asprintf(&sort_str, "%s|%s", sort_oid, "ASC");
					}
					else
					{
						tempSortStr=sort_str; sort_str=NULL;
						asprintf(&sort_str, "%s|%s|%s", tempSortStr, sort_oid, "ASC");
						free(tempSortStr); tempSortStr=NULL;
					}
					
					if(sort_str == NULL)
					{
						SOAP_FOK_LOGOUT(NO_MEMORY);
					}
				}
			}
			
			free(sort_oid); sort_oid=NULL;
			free(sort_order); sort_order=NULL;
		}
		
		if(sort_str != NULL)
		{
			dtg->protocolData=(GenBuf_t *)calloc(1, sizeof(GenBuf_t));
			if(dtg->protocolData == NULL)
			{
				SOAP_FOK_LOGOUT(NO_MEMORY);
			}
			dtg->protocolData->buf=sort_str;
			sort_str=NULL;
			dtg->protocolData->size=strlen(dtg->protocolData->buf);
		}
	}

	/******************************************/
	/*	ustawienie limitow wyszukiwania	*/
	/******************************************/
 	SOAP_FOK_LOGOUT(bmd_datagram_add_limits(offset,limit,&dtg));

	/************************************/
	/*	dodanie datagramu do zestawu	*/
	/************************************/
	SOAP_FOK_LOGOUT(bmd_datagram_add_to_set(dtg,&dtg_set));

	/******************************************/
	/*	wyslanie zadania do serwera bmd	*/
	/******************************************/
	SOAP_FOK_LOGOUT(bmd_send_request(	bi, dtg_set, max_datagram_in_set_transmission, &resp_dtg_set, 1, deserialisation_max_memory));
	bmd_datagramset_free(&dtg_set);
	SOAP_FOK_LOGOUT(resp_dtg_set->bmdDatagramSetTable[0]->datagramStatus);

	/************************************************************/
	/*	alokacja pamieci na wyniki wyszukiwania (na drzewo)	*/
	/************************************************************/

	(*searchResults)=(struct bmd230__searchResults *)soap_malloc(soap,sizeof(struct bmd230__searchResults));
	if((*searchResults) == NULL)
	{
		SOAP_FOK_LOGOUT(NO_MEMORY);
	}
	memset(*searchResults,0,sizeof(struct bmd230__searchResults));

	/******************************************************************************/
	/*	alokacja pamieci na strukture odpowiedzi - matadane wszystkich wynikow	*/
	/******************************************************************************/

	(*searchResults)->__size=resp_dtg_set->bmdDatagramSetSize;
	(*searchResults)->__ptr=(struct bmd230__searchSingleResult *)soap_malloc(soap, \
	sizeof(struct bmd230__searchSingleResult)*(*searchResults)->__size);
	if((*searchResults)->__ptr == NULL)
	{
		(*searchResults)->__size=0;
		SOAP_FOK_LOGOUT(NO_MEMORY);
	}
	memset((*searchResults)->__ptr, 0, ((*searchResults)->__size)*sizeof(struct bmd230__searchSingleResult));

	/****************************************/
	/*	wszystkie znalezione pliki	*/
	/****************************************/
	for(i=0; i<resp_dtg_set->bmdDatagramSetSize; i++)
	{/* for(i=0; i<resp_dtg_set->bmdDatagramSetSize; i++)*/

		asprintf(&((*searchResults)->__ptr[i].filename), "%s", resp_dtg_set->bmdDatagramSetTable[i]->protocolDataFilename->buf);

		(*searchResults)->__ptr[i].id = atoi((char *)resp_dtg_set->bmdDatagramSetTable[i]->protocolDataFileId->buf);
		(*searchResults)->__ptr[i].mtds=(struct bmd230__mtdsValues *)soap_malloc(soap,sizeof(struct bmd230__mtdsValues));
		if((*searchResults)->__ptr[i].mtds == NULL)
		{
			SOAP_FOK_LOGOUT(NO_MEMORY);
		}
		memset((*searchResults)->__ptr[i].mtds, 0, sizeof(struct bmd230__mtdsValues));

		if(formMtds == NULL)
		{

			form_mtds_count = resp_dtg_set->bmdDatagramSetTable[i]->no_of_sysMetaData + \
			resp_dtg_set->bmdDatagramSetTable[i]->no_of_actionMetaData + \
			resp_dtg_set->bmdDatagramSetTable[i]->no_of_additionalMetaData;

			(*searchResults)->__ptr[i].mtds->__ptr=(struct bmd230__mtdSingleValue *)soap_malloc(soap, sizeof(struct bmd230__mtdSingleValue) * form_mtds_count);
			if((*searchResults)->__ptr[i].mtds->__ptr == NULL)
			{
				SOAP_FOK_LOGOUT(NO_MEMORY);
			}
			memset((*searchResults)->__ptr[i].mtds->__ptr, 0,  form_mtds_count*sizeof(struct bmd230__mtdSingleValue));
			(*searchResults)->__ptr[i].mtds->__size = form_mtds_count;
			form_mtds_count = 1;

		}
		else
		{
			form_mtds_count = formMtds->__size;
			if(form_mtds_count == 0)
			{
				form_mtds_count = resp_dtg_set->bmdDatagramSetTable[i]->no_of_sysMetaData + \
				resp_dtg_set->bmdDatagramSetTable[i]->no_of_actionMetaData + \
				resp_dtg_set->bmdDatagramSetTable[i]->no_of_additionalMetaData;
				formMtds = NULL;
			}
			(*searchResults)->__ptr[i].mtds->__ptr=(struct bmd230__mtdSingleValue *)soap_malloc(soap, sizeof(struct bmd230__mtdSingleValue) * form_mtds_count);
			if((*searchResults)->__ptr[i].mtds->__ptr == NULL)
			{
				SOAP_FOK_LOGOUT(NO_MEMORY);
			}
			memset((*searchResults)->__ptr[i].mtds->__ptr, 0, form_mtds_count*sizeof(struct bmd230__mtdSingleValue));

			if(formMtds == NULL)
			{
				form_mtds_count = 1;
			}
			else
			{
				(*searchResults)->__ptr[i].mtds->__size = form_mtds_count;
			}
		}

		mtd_result_count = 0;

		for(k=0; k < form_mtds_count; k++)
		{
			/********************************************************/
			/*	Kopiuje metadane systemowe do odpowiedzi	*/
			/********************************************************/
			if (resp_dtg_set->bmdDatagramSetTable[i]->no_of_sysMetaData>0)
			{
				SOAP_FOK( MtdsListToResponse(	soap,
							resp_dtg_set->bmdDatagramSetTable[i]->sysMetaData,
							resp_dtg_set->bmdDatagramSetTable[i]->no_of_sysMetaData,
							(*searchResults)->__ptr[i].mtds,
							formMtds,
							k,
							&mtd_result_count) );
			}

			/********************************************************/
			/*	Kopiuje metadane dodatkowe do odpowiedzi	*/
			/********************************************************/
			if (resp_dtg_set->bmdDatagramSetTable[i]->no_of_additionalMetaData>0)
			{
				SOAP_FOK( MtdsListToResponse(  soap,
							resp_dtg_set->bmdDatagramSetTable[i]->additionalMetaData,
							resp_dtg_set->bmdDatagramSetTable[i]->no_of_additionalMetaData,
							(*searchResults)->__ptr[i].mtds,
							formMtds,
							k,
							&mtd_result_count) );
			}
		}

		if (mtd_result_count==0)
		{
			(*searchResults)->__ptr[i].mtds->__ptr[mtd_result_count].mtdOid=(char *)soap_malloc(soap,1);
			if((*searchResults)->__ptr[i].mtds->__ptr[mtd_result_count].mtdOid == NULL)
			{
				SOAP_FOK_LOGOUT(NO_MEMORY);
			}
			memset((*searchResults)->__ptr[i].mtds->__ptr[mtd_result_count].mtdOid,0,1*sizeof(char));

			(*searchResults)->__ptr[i].mtds->__ptr[mtd_result_count].mtdValue=(char *)soap_malloc(soap,1);
			if((*searchResults)->__ptr[i].mtds->__ptr[mtd_result_count].mtdValue == NULL)
			{
				SOAP_FOK_LOGOUT(NO_MEMORY);
			}
			memset((*searchResults)->__ptr[i].mtds->__ptr[mtd_result_count].mtdValue,0,1*sizeof(char));

			(*searchResults)->__ptr[i].mtds->__ptr[mtd_result_count].mtdDesc=(char *)soap_malloc(soap,1);
			if((*searchResults)->__ptr[i].mtds->__ptr[mtd_result_count].mtdDesc == NULL)
			{
				SOAP_FOK_LOGOUT(NO_MEMORY);
			}
			memset((*searchResults)->__ptr[i].mtds->__ptr[mtd_result_count].mtdDesc,0,1*sizeof(char));
		}
		(*searchResults)->__ptr[i].mtds->__size=mtd_result_count;

	}

	PRINT_SOAP("Status:\t\trequest served correctly\n");
	bmd_datagramset_free(&resp_dtg_set);

	SOAP_FOK(bmd_logout(&bi, deserialisation_max_memory));
	bmd_end();

	/************************/
	/*	porzadki	*/
	/************************/
	free(metadata_oid_str); metadata_oid_str=NULL;
	free_gen_buf(&metadata_value_buf);

	return SOAP_OK;
}

/** wyszukiwanie plikow, na ktorych uzytkownicy danej klasy nie wykonali okreslonych akcji*/
int bmd230__bmdSearchUnreceived(	struct soap *soap,
				struct xsd__base64Binary *cert,
				char *userRoleId,
				char *userGroupId,
				char *userClassId,
				int offset,
				int limit,
				struct bmd230__mtdsSearchValues *mtds,
				struct bmd230__mtdsValues *formMtds,
				struct bmd230__searchSortInfoList *sortInfo,
				struct bmd230__searchResults **searchResults)
{

long iter=0;
long retVal=0;

long max_datagram_in_set_transmission=0;
long deserialisation_max_memory=0;

bmd_info_t *bi=NULL;
bmdDatagram_t *dtg=NULL;
bmdDatagramSet_t *dtg_set=NULL;
bmdDatagramSet_t *resp_dtg_set=NULL;

struct bmd230__searchResults *tempSearchResults=NULL;

long max_metadata_value_length=0;

char* sortString=NULL;
char* tempSortString=NULL;

long TMPmtdDesc=0;
long TMPmtdLogicalOperator=0;
long TMPmtdAdjustment=0;
long TMPmtdStatement=0;
long mtdPrefix=0;
long mtdPostfix=0;
GenBuf_t* tmpGenBuf=NULL;

long metadataType=0;

char *derUserCert=NULL;
long derUserCertLen=0;
char* paramCert=NULL;
long paramCertLen=0;


	if((offset<0) || (limit<1))
		{ SOAP_FOK(BMDSOAP_SERVER_INVALID_LIMIT_VALUE);}
	if(mtds == NULL)
		{ SOAP_FOK(BMDSOAP_SERVER_EMPTY7_PARAM_ERROR); }

	/***************************************/
	/* wczytanie ustawien konfiguracyjnych */
	/***************************************/
	if(bmdconf_get_value_int(_GLOBAL_konfiguracja,"soap","max_metadata_value_length",&max_metadata_value_length)<BMD_OK)
	{
		max_metadata_value_length=256;
	}

	bmdconf_get_value_int(_GLOBAL_konfiguracja,"bmd","max_datagram_in_set_transmission", &max_datagram_in_set_transmission);
	if(max_datagram_in_set_transmission <= 0)
	{
		max_datagram_in_set_transmission = 100;
	}

	bmdconf_get_value_int(_GLOBAL_konfiguracja, "bmd", "deserialisation_max_memory", &deserialisation_max_memory);
	if(deserialisation_max_memory < 0)
	{
		deserialisation_max_memory = 0;
	}
	else
	{
		deserialisation_max_memory = deserialisation_max_memory * 1024 * 1024;
	}


	if(_GLOBAL_soap_wssecurity_enabled == 1)
	{
		retVal=GetWSSEInformation(soap, NULL, NULL, NULL, &derUserCert, &derUserCertLen);
		if(retVal < 0)
		{
			PRINT_ERROR("Unable to get certificate from WS Security header %li\n", retVal);
			SOAP_FOK(BMDSOAP_SERVER_NO_WSSECURITY_CERTIFICATE);
		}
	}
	else
	{
		if(_GLOBAL_soap_certificate_required == 1)
		{
			if(cert == NULL)
				{ SOAP_FOK(BMDSOAP_SERVER_EMPTY_LOGIN_CERTIFICATE); }
			else
			{
				paramCert=(char*)cert->__ptr;
				paramCertLen=(long)cert->__size;
			}
		}
		else
		{
			if(cert != NULL)
			{
				paramCert=(char*)cert->__ptr;
				paramCertLen=(long)cert->__size;
			}
		}
	}

	/********************/
	/* logowanie do bmd */
	/********************/
	SOAP_FOK(PrepareDataForBMDLoginAndRequest(&bi,
			(_GLOBAL_soap_wssecurity_enabled == 1 ? derUserCert : paramCert),
			(_GLOBAL_soap_wssecurity_enabled == 1 ? derUserCertLen : paramCertLen),
			userRoleId, userGroupId, deserialisation_max_memory));
	free(derUserCert); derUserCert=NULL;
	paramCert=NULL; paramCertLen=0;

	/********************************/
	/*	stworzenie datagramu	*/
	/********************************/
	SOAP_FOK_LOGOUT(bmd_datagram_create(BMD_DATAGRAM_TYPE_SEARCH_UNRECEIVED,&dtg));

	if (userClassId!=NULL)
	{
		retVal=bmd_datagram_add_metadata_char(	OID_ACTION_METADATA_CERT_CHOSEN_CLASS_ID, userClassId, &dtg);
		if(retVal < BMD_OK)
		{
			bmd_datagram_free(&dtg);
			SOAP_FOK_LOGOUT(retVal);
		}
	}

	
	/*kryteria sortowania*/
	if(sortInfo != NULL)
	{
		for(iter=0; iter<sortInfo->__size; iter++)
		{
			if(sortInfo->__ptr[iter].sortOid != NULL)
			{
				if( IsOidFormat(sortInfo->__ptr[iter].sortOid) < 1)
				{
					bmd_datagram_free(&dtg);
					SOAP_FOK_LOGOUT(BMDSOAP_SERVER_SEARCH_SORT_OID_INVALID);
				}
			}
			else
			{
				SOAP_FOK_LOGOUT(BMDSOAP_SERVER_SEARCH_SORT_OID_INVALID);
			}

			if(sortInfo->__ptr[iter].sortOrder != NULL)
			{
				if( (strcmp(sortInfo->__ptr[iter].sortOrder,"ASC") != 0) && (strcmp(sortInfo->__ptr[iter].sortOrder,"DESC") != 0) )
				{
					bmd_datagram_free(&dtg);
					SOAP_FOK_LOGOUT(BMDSOAP_SERVER_SEARCH_SORT_ORDER_INVALID);
				}
				else
				{
					if(sortString == NULL)
					{
						retVal=asprintf(&sortString, "%s|%s", sortInfo->__ptr[iter].sortOid, sortInfo->__ptr[iter].sortOrder);
						if(retVal < 0)
						{
							bmd_datagram_free(&dtg);
							SOAP_FOK_LOGOUT(NO_MEMORY);
						}
					}
					else
					{
						retVal=asprintf(&tempSortString, "%s|%s|%s", sortString, sortInfo->__ptr[iter].sortOid, sortInfo->__ptr[iter].sortOrder);
						free(sortString);
						if(retVal < 0)
						{
							bmd_datagram_free(&dtg);
							SOAP_FOK_LOGOUT(NO_MEMORY);
						}
						sortString=tempSortString;
						tempSortString=NULL;
					}
				}
			}
		}

		if(sortString != NULL)
		{
			retVal=set_gen_buf2(sortString, strlen(sortString), &(dtg->protocolData));
			free(sortString); sortString=NULL;
			if(retVal < BMD_OK )
			{
				bmd_datagram_free(&dtg);
				SOAP_FOK_LOGOUT(NO_MEMORY);
			}
		}
	}
	
	
	/*************************************/
	/* ustawienie kryteriow wyszukiwania */
	/*************************************/
	if(mtds!=NULL)
	{
		for(iter=0; iter<mtds->__size; iter++)
		{
			if(mtds->__ptr != NULL)
			{
				if( (mtds->__ptr[iter].mtdValue != NULL) && (mtds->__ptr[iter].mtdOid != NULL) )
				{
					
					if(strlen(mtds->__ptr[iter].mtdValue) > max_metadata_value_length)
					{
						bmd_datagram_free(&dtg);
						SOAP_FOK_LOGOUT(BMDSOAP_SERVER_METADATA_VALUE_TOO_LONG);
					}


					/***********************************/
					/* walidacja operatora dopasowania */
					/***********************************/
					if(mtds->__ptr[iter].mtdDesc == NULL)
						{ TMPmtdDesc=0; }
					else
					{
						if( strcmp(mtds->__ptr[iter].mtdDesc, "LIKE") == 0 )
							{ TMPmtdDesc=BMD_REQ_DB_SELECT_ADVANCED_LIKE_QUERY; }
						else if( strcmp(mtds->__ptr[iter].mtdDesc, "ILIKE") == 0 )
							{ TMPmtdDesc=BMD_REQ_DB_SELECT_ADVANCED_ILIKE_QUERY; }
						else
						{
							bmd_datagram_free(&dtg);
							SOAP_FOK(BMDSOAP_SERVER_ADVANCED_SEARCH_INVALID_ADJUSTMENT_OPER);
						}
					}
					
	
					/**********************************/
					/* walidacja operatora logicznego */
					/**********************************/
					if(iter == 0)
					{
						if(mtds->__ptr[iter].mtdLogicalOperator == NULL)
							{ TMPmtdLogicalOperator=0; }
						else
						{
							bmd_datagram_free(&dtg);
							SOAP_FOK(BMDSOAP_SERVER_ADVANCED_SEARCH_INVALID_FIRST_LOG_OPER);
						}
					}
					else
					{
						if(mtds->__ptr[iter].mtdLogicalOperator == NULL)
							{ TMPmtdLogicalOperator=0; }
						else if(strcmp(mtds->__ptr[iter].mtdLogicalOperator, "AND") == 0)
							{ TMPmtdLogicalOperator=BMD_REQ_DB_SELECT_ADVANCED_LOGICAL_AND; }
						else if(strcmp(mtds->__ptr[iter].mtdLogicalOperator, "OR") == 0 )
							{ TMPmtdLogicalOperator=BMD_REQ_DB_SELECT_ADVANCED_LOGICAL_OR; }
						else
						{
							bmd_datagram_free(&dtg);
							SOAP_FOK(BMDSOAP_SERVER_ADVANCED_SEARCH_INVALID_LOG_OPER);
						}
					}
					
					/************************************/
					/* walidacja argumentow dopasowania */
					/************************************/
					if( mtds->__ptr[iter].mtdAdjustment == NULL )
						{ TMPmtdAdjustment=0; }
					else if(strcmp(mtds->__ptr[iter].mtdAdjustment, "NONE") == 0)
						{ TMPmtdAdjustment=BMD_REQ_DB_SELECT_ADVANCED_PERC_NONE; }
					else if(strcmp(mtds->__ptr[iter].mtdAdjustment, "BEGIN") == 0)
						{ TMPmtdAdjustment=BMD_REQ_DB_SELECT_ADVANCED_PERC_BEGIN; }
					else if(strcmp(mtds->__ptr[iter].mtdAdjustment, "END") == 0)
						{ TMPmtdAdjustment=BMD_REQ_DB_SELECT_ADVANCED_PERC_END; }
					else if(strcmp(mtds->__ptr[iter].mtdAdjustment, "BOTH") == 0)
						{ TMPmtdAdjustment=BMD_REQ_DB_SELECT_ADVANCED_PERC_BOTH; }
					else
					{
						bmd_datagram_free(&dtg);
						SOAP_FOK(BMDSOAP_SERVER_ADVANCED_SEARCH_INVALID_ADJUSTMENT_ARG);
					}
	
					/************************************/
					/* walidacja - negacja warunku */
					/************************************/
					if(mtds->__ptr[iter].mtdStatement == NULL)
						{ TMPmtdStatement=1; }
					else if(strcmp(mtds->__ptr[iter].mtdStatement, "NEGATIVE") == 0 )
						{ TMPmtdStatement=-1; }
					else if(strcmp(mtds->__ptr[iter].mtdStatement, "POSITIVE") == 0 )
						{ TMPmtdStatement=1; }
					else
					{
						bmd_datagram_free(&dtg);
						SOAP_FOK(BMDSOAP_SERVER_ADVANCED_SEARCH_INVALID_STATEMENT);
					}

					mtdPrefix = TMPmtdLogicalOperator + mtds->__ptr[iter].noOfOpenBrackets;
					mtdPostfix = (TMPmtdAdjustment + mtds->__ptr[iter].noOfCloseBrackets)*TMPmtdStatement;

					/**********************************/
					/* dodanie metadanej do datagramu */
					/**********************************/
					retVal=set_gen_buf2(mtds->__ptr[iter].mtdValue, strlen(mtds->__ptr[iter].mtdValue)+1, &tmpGenBuf);
					if(retVal < BMD_OK)
					{
						bmd_datagram_free(&dtg);
						SOAP_FOK_LOGOUT(retVal);
					}
					metadataType=determine_oid_type_str(mtds->__ptr[iter].mtdOid);
					if(metadataType == ACTION_METADATA)
					{
						retVal=bmd_datagram_add_metadata_2_with_params(mtds->__ptr[iter].mtdOid, tmpGenBuf, &dtg, ACTION_METADATA, TMPmtdDesc, mtdPrefix, mtdPostfix, 1);
					}
					else
					{
						retVal=bmd_datagram_add_metadata_2_with_params(mtds->__ptr[iter].mtdOid, tmpGenBuf, &dtg, ADDITIONAL_METADATA, TMPmtdDesc, mtdPrefix, mtdPostfix, 1);
					}
					//bindowanie wskaznika
					tmpGenBuf->buf=NULL; tmpGenBuf->size=0;
					free_gen_buf(&tmpGenBuf);
					TMPmtdDesc=0;
					TMPmtdLogicalOperator=0;
					TMPmtdAdjustment=0;
					TMPmtdStatement=0;
					mtdPrefix=0;
					mtdPostfix=0;
					if(retVal < BMD_OK)
					{
						bmd_datagram_free(&dtg);
						SOAP_FOK_LOGOUT(retVal);
					}
				}
			}
		}
	}

	/******************************************/
	/*	ustawienie limitow wyszukiwania	*/
	/******************************************/
	retVal=bmd_datagram_add_limits(offset, limit, &dtg);
	if(retVal < BMD_OK)
	{
		bmd_datagram_free(&dtg);
		SOAP_FOK_LOGOUT(retVal);
	}

	/************************************/
	/*	dodanie datagramu do zestawu	*/
	/************************************/
	retVal=bmd_datagram_add_to_set(dtg, &dtg_set);
	if(retVal < BMD_OK)
	{
		bmd_datagram_free(&dtg);
		SOAP_FOK_LOGOUT(retVal);
	}

	/******************************************/
	/*	wyslanie zadania do serwera bmd	*/
	/******************************************/
	retVal=bmd_send_request(bi, dtg_set, max_datagram_in_set_transmission, &resp_dtg_set, 1, deserialisation_max_memory);
	bmd_datagramset_free(&dtg_set);
	if(retVal < BMD_OK)
	{
		SOAP_FOK_LOGOUT(retVal);
	}
		
	if(resp_dtg_set == NULL)
	{
		SOAP_FOK_LOGOUT(BMDSOAP_SERVER_TRANSMISSION_BROKEN);
	}
	

	if( (retVal=resp_dtg_set->bmdDatagramSetTable[0]->datagramStatus) < BMD_OK)
	{
		bmd_datagramset_free(&resp_dtg_set);
		SOAP_FOK_LOGOUT(retVal);
	}

	
	/************************************************************/
	/* alokacja pamieci na wyniki wyszukiwania */
	/************************************************************/
	tempSearchResults=(struct bmd230__searchResults *)soap_malloc(soap,sizeof(struct bmd230__searchResults));
	if(tempSearchResults == NULL)
	{
		bmd_datagramset_free(&resp_dtg_set);
		SOAP_FOK_LOGOUT(NO_MEMORY);
	}
	memset(tempSearchResults,0,sizeof(struct bmd230__searchResults));

	tempSearchResults->__size=resp_dtg_set->bmdDatagramSetSize;
	tempSearchResults->__ptr=(struct bmd230__searchSingleResult *)soap_malloc(soap,
					sizeof(struct bmd230__searchSingleResult) * (tempSearchResults->__size) );
	if(tempSearchResults->__ptr == NULL)
	{
		bmd_datagramset_free(&resp_dtg_set);
		SOAP_FOK_LOGOUT(NO_MEMORY);
	}
	memset(tempSearchResults->__ptr, 0, sizeof(struct bmd230__searchSingleResult)*(tempSearchResults->__size));


	/****************************************/
	/* wypelnianie odpowiedzi */
	/****************************************/
	for(iter=0; iter<resp_dtg_set->bmdDatagramSetSize; iter++)
	{
		/*ustawienie nazwy pliku*/
		if(resp_dtg_set->bmdDatagramSetTable[iter]->protocolDataFilename != NULL)
			{ asprintf(&(tempSearchResults->__ptr[iter].filename), "%s", resp_dtg_set->bmdDatagramSetTable[iter]->protocolDataFilename->buf); }
		else
			{ tempSearchResults->__ptr[iter].filename=NULL; }

		/*ustawienie id pliku*/
		if(	resp_dtg_set->bmdDatagramSetTable[iter]->protocolDataFileId != NULL &&
			resp_dtg_set->bmdDatagramSetTable[iter]->protocolDataFileId->buf != NULL &&
			resp_dtg_set->bmdDatagramSetTable[iter]->protocolDataFileId->size > 0 )
			{ tempSearchResults->__ptr[iter].id = atoi( (char *)(resp_dtg_set->bmdDatagramSetTable[iter]->protocolDataFileId->buf) ); }
		else
			{ tempSearchResults->__ptr[iter].id = 0; }
		

		retVal=FillResponseWithMetadata( soap,
				resp_dtg_set->bmdDatagramSetTable[iter],
				formMtds,
				1 /*SYS*/, 1/*ADD*/, 0/*ACTION*/,
				&(tempSearchResults->__ptr[iter].mtds) );
		if(retVal < 0)
		{
			bmd_datagramset_free(&resp_dtg_set);
			SOAP_FOK_LOGOUT(retVal);
		}
	}

	*searchResults=tempSearchResults;
	PRINT_SOAP("Status:\t\trequest served correctly\n");
	bmd_datagramset_free(&resp_dtg_set);
	bmd_logout(&bi, deserialisation_max_memory);
	bmd_end();

	return SOAP_OK;
}



/*************************************************************************************************/
/* obsluzenie zadania wyszukiwania plikow w BMD wedlug określonych kryteriow - wynik w pliku CSV */
/*************************************************************************************************/
int bmd230__bmdSearchFilesCSV(	struct soap *soap,
				struct xsd__base64Binary *cert,
				char *roleName,
				char *groupName,
				char *userClassId,
				struct bmd230__mtdsValues *mtds,
				struct bmd230__mtdsValues *formMtds,
				int offset,
				int limit,
				char *logical,
				char *oper,
				char *ownerRange,
				struct bmd230__searchSortInfoList *sortInfo,
				struct bmd230__fileInfo **output)
{
long form_count				= 0;
long i					= 0;
long j					= 0;
long k					= 0;
long form_mtds_count			= 0;
long first_run_flag			= 0;
long max_metadata_value_length		= 0;
long max_datagram_in_set_transmission	= 0;
long deserialisation_max_memory		= 0;
bmd_info_t *bi				= NULL;
bmdDatagram_t *dtg			= NULL;
bmdDatagramSet_t *dtg_set		= NULL;
bmdDatagramSet_t *resp_dtg_set		= NULL;
char *sort_str				= NULL;
char* tempSortStr			= NULL;
char *sort_oid				= NULL;
char *sort_order			= NULL;
char *metadata_oid_str			= NULL;
char *tmp_csv_file			= NULL;
char *tmp_string			= NULL;
BMD_attr_t **form			= NULL;
GenBuf_t *metadata_value_buf			= NULL;
CSV_escape_conf_t CSV_escape_conf_struct;
GenBuf_t tmp_gb;
long chosenOper				= 0;
long chosenOwnerRange			= 0;

char *derUserCert=NULL;
long derUserCertLen=0;
long retVal=0;
char* paramCert=NULL;
long paramCertLen=0;

	PRINT_SOAP("Request:\tsearch for documents\n");
	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if (oper==NULL)					{	SOAP_FOK(BMDSOAP_SERVER_SEARCH_OPERATOR_INVALID);	}
	if (	(strcmp(oper,"LIKE")!=0) &&
		(strcmp(oper,"ILIKE")!=0) &&
		(strcmp(oper,"EXACT")!=0) &&
		(strcmp(oper,"EQUAL")!=0)	)	{	SOAP_FOK(BMDSOAP_SERVER_SEARCH_OPERATOR_INVALID);	}
	if (logical==NULL)				{	SOAP_FOK(BMDSOAP_SERVER_SEARCH_LOGICAL_INVALID);	}
	if (	(strcmp(logical,"OR")!=0) &&
		(strcmp(logical,"AND")!=0)	)	{	SOAP_FOK(BMDSOAP_SERVER_SEARCH_LOGICAL_INVALID);	}
	if ((offset<0) || (limit<1))			{	SOAP_FOK(BMDSOAP_SERVER_INVALID_LIMIT_VALUE);		}

	bmdconf_get_value_int(_GLOBAL_konfiguracja,"bmd","max_datagram_in_set_transmission", &max_datagram_in_set_transmission);
	if(max_datagram_in_set_transmission <= 0)
	{
		max_datagram_in_set_transmission = 100;
	}

	bmdconf_get_value_int(_GLOBAL_konfiguracja, "bmd", "deserialisation_max_memory", &deserialisation_max_memory);
	if(deserialisation_max_memory < 0)
	{
		deserialisation_max_memory = 0;
	}
	else
	{
		deserialisation_max_memory = deserialisation_max_memory * 1024 * 1024;
	}

	if (sortInfo!=NULL)
	{
		for (j=0; j<sortInfo->__size; j++)
		{
			if (sortInfo->__ptr[j].sortOrder!=NULL)
			{
				if ((strcmp(sortInfo->__ptr[j].sortOrder,"ASC")!=0) && \
				(strcmp(sortInfo->__ptr[j].sortOrder,"DESC")!=0))
				{
					SOAP_FOK(BMDSOAP_SERVER_SEARCH_SORT_ORDER_INVALID);
				}
			}
			else
			{
				SOAP_FOK(BMDSOAP_SERVER_SEARCH_SORT_ORDER_INVALID);
			}

			if (sortInfo->__ptr[j].sortOid!=NULL)
			{
				for (i=0; i<strlen(sortInfo->__ptr[j].sortOid); i++)
				{
					if (((sortInfo->__ptr[j].sortOid[i]<'0') || (sortInfo->__ptr[j].sortOid[i]>'9')) \
					&& (sortInfo->__ptr[j].sortOid[i]!='.'))
					{
						SOAP_FOK(BMDSOAP_SERVER_SEARCH_SORT_OID_INVALID);
					}
				}
			}
			else
			{
				SOAP_FOK(BMDSOAP_SERVER_SEARCH_SORT_OID_INVALID);
			}
		}
	}

	/***************************************/
	/* wczytanie ustawien konfiguracyjnych */
	/***************************************/
	if (bmdconf_get_value_int(_GLOBAL_konfiguracja,"soap","max_metadata_value_length",&max_metadata_value_length)<BMD_OK)
	{
		max_metadata_value_length=256;
	}

	if(_GLOBAL_soap_wssecurity_enabled == 1)
	{
		retVal=GetWSSEInformation(soap, NULL, NULL, NULL, &derUserCert, &derUserCertLen);
		if(retVal < 0)
		{
			PRINT_ERROR("Unable to get certificate from WS Security header %li\n", retVal);
			SOAP_FOK(BMDSOAP_SERVER_NO_WSSECURITY_CERTIFICATE);
		}
	}
	else
	{
		if(_GLOBAL_soap_certificate_required == 1)
		{
			if(cert == NULL)
				{ SOAP_FOK(BMDSOAP_SERVER_EMPTY_LOGIN_CERTIFICATE); }
			else
			{
				paramCert=(char*)cert->__ptr;
				paramCertLen=(long)cert->__size;
			}
		}
		else
		{
			if(cert != NULL)
			{
				paramCert=(char*)cert->__ptr;
				paramCertLen=(long)cert->__size;
			}
		}
	}

	/********************************/
	/*	logowanie do bmd	*/
	/********************************/
	SOAP_FOK(PrepareDataForBMDLoginAndRequest(&bi,
				(_GLOBAL_soap_wssecurity_enabled == 1  ? derUserCert : paramCert),
				(_GLOBAL_soap_wssecurity_enabled == 1  ? derUserCertLen : paramCertLen),
				roleName, groupName, deserialisation_max_memory));
	free(derUserCert); derUserCert=NULL;
	paramCert=NULL; paramCertLen=0;

	/************************************************************/
	/*      pobranie formularza w celu ustalenia typu metadanej */
	/************************************************************/
	SOAP_FOK_LOGOUT(bmd_info_get_form(BMD_FORM, bi, &form, &form_count));
	/*TODO*/
	/*for(j =0; j < 10; j++)
	{	char *oid = NULL;
		bmd_attr_get_oid(j, form, &oid);
		printf( "Tomasz type %lu, oid %s\n", form[j]->contentType, oid);
	}*/
	/*TODO*/

	/********************************/
	/*	stworzenie datagramu	*/
	/********************************/
	if (strcmp(logical,"AND")==0)
	{
		SOAP_FOK_LOGOUT(bmd_datagram_create(BMD_DATAGRAM_TYPE_SEARCH,&dtg));
	}
	if (strcmp(logical,"OR")==0)
	{
		SOAP_FOK_LOGOUT(bmd_datagram_create(BMD_DATAGRAM_TYPE_SEARCH,&dtg));
	}

	if (userClassId!=NULL)
	{
		SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(	OID_ACTION_METADATA_CERT_CHOSEN_CLASS_ID, userClassId, &dtg));
	}

	/************************************************/
	/*	ustawienie kryteriow wyszukiwania	*/
	/************************************************/
	if(mtds!=NULL)
	{
		for(i=0;i<mtds->__size;i++)
		{
			if (mtds->__ptr!=NULL)
			{
				if (mtds->__ptr[i].mtdValue!=NULL)
				{
					tmp_gb.buf=(char *)mtds->__ptr[i].mtdValue;
					tmp_gb.size=strlen(mtds->__ptr[i].mtdValue);

					if (tmp_gb.size>max_metadata_value_length)
					{
						SOAP_FOK_LOGOUT(BMDSOAP_SERVER_METADATA_VALUE_TOO_LONG);
					}

					if (mtds->__ptr[i].mtdDesc!=NULL)
					{
						if (strcmp(mtds->__ptr[i].mtdDesc,"LIKE")==0)
						{
							SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_2_with_params(mtds->__ptr[i].mtdOid,&tmp_gb,&dtg,UNDEFINED_METADATA,BMD_REQ_DB_SELECT_LIKE_QUERY,0,0, 0));
						}
						else if (strcmp(mtds->__ptr[i].mtdDesc,"ILIKE")==0)
						{
							SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_2_with_params(mtds->__ptr[i].mtdOid,&tmp_gb,&dtg,UNDEFINED_METADATA,BMD_REQ_DB_SELECT_ILIKE_QUERY,0,0, 0));
						}
						else if (strcmp(mtds->__ptr[i].mtdDesc,"EXACT")==0)
						{
							SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_2_with_params(mtds->__ptr[i].mtdOid,&tmp_gb,&dtg,UNDEFINED_METADATA,BMD_REQ_DB_SELECT_EXACT_ILIKE_QUERY,0,0, 0));
						}
						else if (strcmp(mtds->__ptr[i].mtdDesc,"EQUAL")==0)
						{
							SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_2_with_params(mtds->__ptr[i].mtdOid,&tmp_gb,&dtg,UNDEFINED_METADATA,BMD_REQ_DB_SELECT_EQUAL_QUERY,0,0, 0));
						}
						else
						{
							SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_2_with_params(mtds->__ptr[i].mtdOid,&tmp_gb,&dtg,UNDEFINED_METADATA,0,0,0, 0));
						}
					}
					else
					{
						SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_2_with_params(mtds->__ptr[i].mtdOid,&tmp_gb,&dtg,UNDEFINED_METADATA,0,0,0, 0));
					}
				}
			}
		}
	}

	/******************************************/
	/*	ustawienie zakresu wyszukiwania	*/
	/******************************************/
	if( oper!=NULL && strcmp(oper,"ILIKE")==0 )
		{ chosenOper=BMD_QUERY_ILIKE; }
	if( oper!=NULL && strcmp(oper,"EXACT")==0 )
		{ chosenOper=BMD_QUERY_EXACT_ILIKE; }
	if( oper!=NULL && strcmp(oper,"EQUAL")==0 )
		{ chosenOper=BMD_QUERY_EQUAL; }
	else // LIKE i kazda inna wartosc
		{ chosenOper=BMD_QUERY_LIKE; }

	if( ownerRange!=NULL && strcmp(ownerRange, "GROUP")==0 )
		{ chosenOwnerRange=BMD_ONLY_GROUP_DOC; }
	else if( ownerRange!=NULL && strcmp(ownerRange, "CREATOR")==0 )
		{ chosenOwnerRange=BMD_ONLY_CREATOR_DOC; }
	else if( ownerRange!=NULL && strcmp(ownerRange, "OWNER_OR_CREATOR")==0 )
		{ chosenOwnerRange=BMD_OWNER_OR_CREATOR_DOC; }
	else // OWNER i kazda inna wartosc
		{ chosenOwnerRange=BMD_ONLY_OWNER_DOC; }

	SOAP_FOK_LOGOUT(bmd_datagram_set_status(chosenOwnerRange, chosenOper, &dtg));

	/****************************************/
	/*	ustawienie trybu sortowania	*/
	/****************************************/
	if (sortInfo!=NULL)
	{
		for (j=0; j<sortInfo->__size; j++)
		{
			sort_oid=strdup(sortInfo->__ptr[j].sortOid);
			sort_order=strdup(sortInfo->__ptr[j].sortOrder);
			
			if (sort_oid!=NULL)
			{
				if (sort_order!=NULL)
				{
					if (sort_str==NULL)
					{
						asprintf(&sort_str, "%s|%s", sort_oid, sort_order);
					}
					else
					{
						tempSortStr=sort_str; sort_str=NULL;
						asprintf(&sort_str, "%s|%s|%s", tempSortStr, sort_oid, sort_order);
						free(tempSortStr); tempSortStr=NULL;
					}
					
					if(sort_str == NULL)
					{
						SOAP_FOK_LOGOUT(NO_MEMORY);
					}
				}
				else
				{
					if (sort_str==NULL)
					{
						asprintf(&sort_str, "%s|%s", sort_oid, "ASC");
					}
					else
					{
						tempSortStr=sort_str; sort_str=NULL;
						asprintf(&sort_str, "%s|%s|%s", tempSortStr, sort_oid, "ASC");
						free(tempSortStr); tempSortStr=NULL;
					}
					
					if(sort_str == NULL)
					{
						SOAP_FOK_LOGOUT(NO_MEMORY);
					}
				}
			}
			
			free(sort_oid); sort_oid=NULL;
			free(sort_order); sort_order=NULL;
		}
		
		if(sort_str != NULL)
		{
			dtg->protocolData=(GenBuf_t *)calloc(1, sizeof(GenBuf_t));
			if(dtg->protocolData == NULL)
			{
				SOAP_FOK_LOGOUT(NO_MEMORY);
			}
			dtg->protocolData->buf=sort_str;
			sort_str=NULL;
			dtg->protocolData->size=strlen(dtg->protocolData->buf);
		}
	}

	/******************************************/
	/*	ustawienie limitow wyszukiwania	*/
	/******************************************/
	SOAP_FOK_LOGOUT(bmd_datagram_add_limits(offset,limit,&dtg));

	/************************************/
	/*	dodanie datagramu do zestawu	*/
	/************************************/
	SOAP_FOK_LOGOUT(bmd_datagram_add_to_set(dtg,&dtg_set));

	/******************************************/
	/*	wyslanie zadania do serwera bmd	*/
	/******************************************/
	SOAP_FOK_LOGOUT(bmd_send_request(bi,dtg_set, max_datagram_in_set_transmission, &resp_dtg_set, 1, deserialisation_max_memory));
	bmd_datagramset_free(&dtg_set);
	SOAP_FOK_LOGOUT(resp_dtg_set->bmdDatagramSetTable[0]->datagramStatus);

	/************************************/
	/*	przygotowanie pliku csv		*/
	/************************************/
	memset(&CSV_escape_conf_struct, 0, sizeof(CSV_escape_conf_t));

	bmdconf_get_value(_GLOBAL_konfiguracja,"csv","csv_delimiter",&(CSV_escape_conf_struct.tws_csv_delimiter));
	bmdconf_get_value_int(_GLOBAL_konfiguracja,"csv","csv_enable_escape",&(CSV_escape_conf_struct.tws_enable_escaping));
	if(CSV_escape_conf_struct.tws_enable_escaping == -1)
	{
		CSV_escape_conf_struct.tws_enable_escaping = 0;
	}
	bmdconf_get_value(_GLOBAL_konfiguracja,"csv","csv_char_to_escape",&(CSV_escape_conf_struct.tws_char_to_escape));
	bmdconf_get_value(_GLOBAL_konfiguracja,"csv","csv_escape_char",&(CSV_escape_conf_struct.tws_escape_char));
	bmdconf_get_value(_GLOBAL_konfiguracja,"csv","csv_value_surrounding_char",&(CSV_escape_conf_struct.tws_m_value_escape));

	bmdconf_get_value_int(_GLOBAL_konfiguracja,"csv","csv_enable_string_escaping", \
	&(CSV_escape_conf_struct.tws_enable_string_escaping));
	if(CSV_escape_conf_struct.tws_enable_string_escaping == -1)
	{
		CSV_escape_conf_struct.tws_enable_string_escaping = 0;
	}
	bmdconf_get_value_int(_GLOBAL_konfiguracja,"csv","csv_enable_number_escaping", \
	&(CSV_escape_conf_struct.tws_enable_number_escaping));
	if(CSV_escape_conf_struct.tws_enable_number_escaping == -1)
	{
		CSV_escape_conf_struct.tws_enable_number_escaping = 0;
	}
	bmdconf_get_value_int(_GLOBAL_konfiguracja,"csv","csv_enable_date_escaping", \
	&(CSV_escape_conf_struct.tws_enable_date_escaping));
	if(CSV_escape_conf_struct.tws_enable_date_escaping == -1)
	{
		CSV_escape_conf_struct.tws_enable_date_escaping = 0;
	}

	bmdconf_get_value(_GLOBAL_konfiguracja,"csv","csv_src_delimeter", &(CSV_escape_conf_struct.src_delimeter));
	bmdconf_get_value(_GLOBAL_konfiguracja,"csv","csv_dst_delimeter", &(CSV_escape_conf_struct.dst_delimeter));

	if(formMtds == NULL)
	{
		form_mtds_count = 1;
	}
	else
	{
		form_mtds_count = formMtds->__size;
		if(form_mtds_count == 0)
		{
			formMtds = NULL;
			form_mtds_count = 1;
		}
	}

	for(i=0; i<resp_dtg_set->bmdDatagramSetSize; i++)
	{
		/****************************************************************/
		/*	przeszukujemy liste zadanych przez klienta metadanych	*/
		/****************************************************************/
		first_run_flag = 1;
		for(k=0; k < form_mtds_count; k++)
		{
			if (resp_dtg_set->bmdDatagramSetTable[i]->no_of_sysMetaData>0)
			{
				SOAP_FOK( MtdsListToCSV( 	soap,
							resp_dtg_set->bmdDatagramSetTable[i]->sysMetaData,
							resp_dtg_set->bmdDatagramSetTable[i]->no_of_sysMetaData,
							form,
							form_count,

							&tmp_csv_file,

							&first_run_flag,

							formMtds,
							k,
							&CSV_escape_conf_struct) );
			}

			if (resp_dtg_set->bmdDatagramSetTable[i]->no_of_actionMetaData>0)
			{
				SOAP_FOK( MtdsListToCSV(	soap,
							resp_dtg_set->bmdDatagramSetTable[i]->actionMetaData,
							resp_dtg_set->bmdDatagramSetTable[i]->no_of_actionMetaData,
							form,
							form_count,

							&tmp_csv_file,

							&first_run_flag,

							formMtds,
							k,
							&CSV_escape_conf_struct) );
			}

			if (resp_dtg_set->bmdDatagramSetTable[i]->no_of_additionalMetaData>0)
			{
				SOAP_FOK( MtdsListToCSV(  	soap,
							resp_dtg_set->bmdDatagramSetTable[i]->additionalMetaData,
							resp_dtg_set->bmdDatagramSetTable[i]->no_of_additionalMetaData,
							form,
							form_count,

							&tmp_csv_file,

							&first_run_flag,

							formMtds,
							k,
							&CSV_escape_conf_struct) );
			}
		}
		asprintf(&tmp_string, "%s\n", tmp_csv_file);
		free(tmp_csv_file);
		tmp_csv_file = tmp_string;
		tmp_string = NULL;
		if(tmp_csv_file == NULL)
		{
			SOAP_FOK_LOGOUT(NO_MEMORY);
		}
	}

	/********************************/
	/*	ustawienie wyniku	*/
	/********************************/
	(*output)=(struct bmd230__fileInfo *)soap_malloc(soap,sizeof(struct bmd230__fileInfo));
	if(*output == NULL)
	{
		SOAP_FOK_LOGOUT(NO_MEMORY);
	}
	memset(*output,0,sizeof(struct bmd230__fileInfo));

	/*	ustawienie nazwy pliku	*/
	(*output)->filename=(char *)soap_malloc(soap,strlen("csv")+2);
	if( (*output)->filename == NULL )
	{
		 SOAP_FOK_LOGOUT(NO_MEMORY);
	}
	memset((*output)->filename, 0, strlen("csv")+1);
	memmove((*output)->filename, "csv", strlen("csv"));

	/*	ustawienie pliku	*/
	(*output)->file=(struct bmd230__myBinaryDataType *)soap_malloc(soap,sizeof(struct bmd230__myBinaryDataType));
	if((*output)->file == NULL)
	{
		 SOAP_FOK_LOGOUT(NO_MEMORY);
	}
	memset((*output)->file, 0, sizeof(struct bmd230__myBinaryDataType));
	(*output)->file->id = NULL;

	(*output)->file->__ptr=(unsigned char *)soap_malloc(soap,strlen(tmp_csv_file)+1);
	if((*output)->file->__ptr == NULL)
	{
		SOAP_FOK_LOGOUT(NO_MEMORY);
	}
	memset((*output)->file->__ptr,0,strlen(tmp_csv_file)+1);
	(*output)->file->__size=strlen(tmp_csv_file);
	memmove((*output)->file->__ptr,tmp_csv_file,strlen(tmp_csv_file));
	(*output)->file->type = NULL;
	(*output)->file->options = NULL;

	PRINT_SOAP("Status:\t\trequest served correctly\n");
	bmd_logout(&bi, deserialisation_max_memory);
	bmd_info_destroy(&bi);
	bmd_end();

	/************************/
	/*	porzadki	*/
	/************************/
	free(metadata_oid_str); metadata_oid_str=NULL;
	free(tmp_csv_file); tmp_csv_file=NULL;
	free_gen_buf(&metadata_value_buf);

	free(CSV_escape_conf_struct.tws_csv_delimiter);
	free(CSV_escape_conf_struct.tws_char_to_escape);
	free(CSV_escape_conf_struct.tws_escape_char);
	free(CSV_escape_conf_struct.tws_m_value_escape);

	memset(&CSV_escape_conf_struct, 0, sizeof(CSV_escape_conf_t));
	bmd_datagramset_free(&resp_dtg_set);
	
	return SOAP_OK;
}

/**********************************************************************************************/
/* obsluzenie zadania wyszukiwania plikow w BMD wedlug określonych kryteriow z operatorem AND */
/**********************************************************************************************/
int bmd230__bmdHistory(		struct soap *soap,
				struct xsd__base64Binary *cert,
				char *roleName,
				char *groupName,
				char *userClassId,
				long id,
                            	struct bmd230__historyResults **historyResults)
{
long i					= 0;
long j					= 0;
long form_count				= 0;
bmd_info_t *bi				= NULL;
bmdDatagram_t *dtg			= NULL;
bmdDatagram_t *tmp			= NULL;
bmdDatagramSet_t *dtg_set		= NULL;
bmdDatagramSet_t *resp_dtg_set		= NULL;
GenBuf_t *mtd_buf			= NULL;
BMD_attr_t **form			= NULL;
char *oid				= NULL;
char *desc				= NULL;
char *sort_oid				= NULL;
char *sort_order			= NULL;
char *updateOwner			= NULL;
char *updateDate			= NULL;
char *updateReason			= NULL;
char **dest				= NULL;
long dest_count				= 0;
long max_datagram_in_set_transmission	= 0;
long deserialisation_max_memory		= 0;

char *derUserCert=NULL;
long derUserCertLen=0;
long retVal=0;
char* paramCert=NULL;
long paramCertLen=0;

	PRINT_SOAP("Request:\thistory of documents\n");
	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if(id<=0)		{	SOAP_FOK(BMDSOAP_SERVER_INVALID_FILE_ID);	}

	bmdconf_get_value_int(_GLOBAL_konfiguracja,"bmd","max_datagram_in_set_transmission", &max_datagram_in_set_transmission);
	if(max_datagram_in_set_transmission <= 0)
	{
		max_datagram_in_set_transmission = 100;
	}

	bmdconf_get_value_int(_GLOBAL_konfiguracja, "bmd", "deserialisation_max_memory", &deserialisation_max_memory);
	if(deserialisation_max_memory < 0)
	{
		deserialisation_max_memory = 0;
	}
	else
	{
		deserialisation_max_memory = deserialisation_max_memory * 1024 * 1024;
	}

	if(_GLOBAL_soap_wssecurity_enabled == 1)
	{
		retVal=GetWSSEInformation(soap, NULL, NULL, NULL, &derUserCert, &derUserCertLen);
		if(retVal < 0)
		{
			PRINT_ERROR("Unable to get certificate from WS Security header %li\n", retVal);
			SOAP_FOK(BMDSOAP_SERVER_NO_WSSECURITY_CERTIFICATE);
		}
	}
	else
	{
		if(_GLOBAL_soap_certificate_required == 1)
		{
			if(cert == NULL)
				{ SOAP_FOK(BMDSOAP_SERVER_EMPTY_LOGIN_CERTIFICATE); }
			else
			{
				paramCert=(char*)cert->__ptr;
				paramCertLen=(long)cert->__size;
			}
		}
		else
		{
			if(cert != NULL)
			{
				paramCert=(char*)cert->__ptr;
				paramCertLen=(long)cert->__size;
			}
		}
	}

	/************************/
	/*	logowanie do bmd	*/
	/************************/
	SOAP_FOK(PrepareDataForBMDLoginAndRequest(&bi,
				(_GLOBAL_soap_wssecurity_enabled == 1  ? derUserCert : paramCert),
				(_GLOBAL_soap_wssecurity_enabled == 1  ? derUserCertLen : paramCertLen),
				roleName, groupName, deserialisation_max_memory));
	free(derUserCert); derUserCert=NULL;
	paramCert=NULL; paramCertLen=0;

	/******************************/
	/*	stworzenie datagramu	*/
	/******************************/
	SOAP_FOK_LOGOUT(bmd_datagram_create(BMD_DATAGRAM_GET_METADATA_WITH_TIME,&dtg));
	if (userClassId!=NULL)
	{
		SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(	OID_ACTION_METADATA_CERT_CHOSEN_CLASS_ID, userClassId, &dtg));
	}

	/******************************/
	/*	ustawienie id pliku	*/
	/******************************/
	SOAP_FOK_LOGOUT(bmd_datagram_set_id((const long *)&id,&dtg));

	/************************************/
	/*	dodanie datagramu do zestawu	*/
	/************************************/
	SOAP_FOK_LOGOUT(bmd_datagram_add_to_set(dtg,&dtg_set));

	/******************************************/
	/*	wyslanie zadania do serwera bmd	*/
	/******************************************/
	SOAP_FOK_LOGOUT(bmd_send_request(bi,dtg_set, max_datagram_in_set_transmission, &resp_dtg_set, 1, deserialisation_max_memory));
	SOAP_FOK_LOGOUT(resp_dtg_set->bmdDatagramSetTable[0]->datagramStatus);

	(*historyResults)=(struct bmd230__historyResults *)soap_malloc(soap,sizeof(struct bmd230__historyResults));
	memset(*historyResults,0,sizeof(struct bmd230__historyResults));

	(*historyResults)->__size=resp_dtg_set->bmdDatagramSetSize;
	(*historyResults)->__ptr=(struct bmd230__historySingleResult *)soap_malloc(soap,sizeof(struct bmd230__historySingleResult)*(*historyResults)->__size);

	/**********************************/
	/* pobierz formularz wyszukiwania */
	/**********************************/
	SOAP_FOK_LOGOUT(bmd_info_get_form(BMD_FORM_HISTORY,bi,&form,&form_count));

	for(i=0;i<(*historyResults)->__size;i++)
	{
		memset(&((*historyResults)->__ptr[i]),0,sizeof(struct bmd230__historySingleResult));

		SOAP_FOK_LOGOUT(bmd_datagramset_get_datagram(resp_dtg_set,i,&tmp));

		/* alokacja listy metadanych */
		(*historyResults)->__ptr[i].mtds=(struct bmd230__mtdsValues *)soap_malloc(soap,sizeof(struct bmd230__mtdsValues));
		(*historyResults)->__ptr[i].mtds->__ptr=(struct bmd230__mtdSingleValue *)soap_malloc(soap,sizeof(struct bmd230__mtdSingleValue)*form_count);
		(*historyResults)->__ptr[i].mtds->__size=form_count;
		bmd_genbuf_free(&mtd_buf);

		if ((tmp->protocolDataOwner!=NULL) && (tmp->protocolDataOwner->buf!=NULL))
		{
			asprintf(&updateOwner, "%s", tmp->protocolDataOwner->buf);


			(*historyResults)->__ptr[i].updateOwner=(char *)soap_malloc(soap,strlen(updateOwner)+2);

			memset((*historyResults)->__ptr[i].updateOwner,0,strlen(updateOwner)+1);

			memmove((*historyResults)->__ptr[i].updateOwner,updateOwner,strlen(updateOwner));


		}

		if ((tmp->protocolData!=NULL) && (tmp->protocolData->buf!=NULL))
		{
			bmd_strsep((char*)tmp->protocolData->buf, '|', &dest, &dest_count);

			if (dest[0]!=NULL)
			{

				asprintf(&updateDate, "%s", dest[0]);
				free(dest[0]); dest[0]=NULL;
			}

			if (dest[1]!=NULL)
			{

				asprintf(&updateReason, "%s", dest[1]);
				free(dest[1]); dest[1]=NULL;
			}

			free(dest); dest=NULL;


			(*historyResults)->__ptr[i].updateDate=(char *)soap_malloc(soap,strlen(updateDate)+2);
			memset((*historyResults)->__ptr[i].updateDate,0,strlen(updateDate)+1);
			memmove((*historyResults)->__ptr[i].updateDate,updateDate,strlen(updateDate));


			(*historyResults)->__ptr[i].updateReason=(char *)soap_malloc(soap,strlen(updateReason)+2);
			memset((*historyResults)->__ptr[i].updateReason,0,strlen(updateReason)+1);
			memmove((*historyResults)->__ptr[i].updateReason,updateReason,strlen(updateReason));
		}

		/* pobranie z datagramu tych metadanych, ktore sa na formularzu do wyszukiwania */
		long inc=0;
		for(j=0;j<form_count;j++)
		{
			free(oid);oid=NULL;
			free(desc);desc=NULL;
			bmd_genbuf_free(&mtd_buf);

			bmd_attr_get_oid(j,form,&oid);
			bmd_attr_get_description(j,form,&desc);
			bmd_datagram_get_metadata_2(oid,tmp,&mtd_buf);
			//pominiecie pierwszej metadanej - nazwy dokumentu - nazwa jest w filename
			if (strcmp(oid,"1.2.616.1.113527.4.3.2.5.2") != 0)
			{

				if(oid)
				{
					(*historyResults)->__ptr[i].mtds->__ptr[j-inc].mtdOid=(char *)soap_malloc(soap,strlen(oid)+1);
					memset((*historyResults)->__ptr[i].mtds->__ptr[j-inc].mtdOid,0,strlen(oid)+1);
					memmove((*historyResults)->__ptr[i].mtds->__ptr[j-inc].mtdOid,oid,strlen(oid));
				}
				else
				{
					continue;
				}

				if(desc)
				{
					(*historyResults)->__ptr[i].mtds->__ptr[j-inc].mtdDesc=(char *)soap_malloc(soap,strlen(desc)+1);
					memset((*historyResults)->__ptr[i].mtds->__ptr[j-inc].mtdDesc,0,strlen(desc)+1);
					memmove((*historyResults)->__ptr[i].mtds->__ptr[j-inc].mtdDesc,desc,strlen(desc));
				}
				else
				{
					asprintf(&desc, "%s", "");
					(*historyResults)->__ptr[i].mtds->__ptr[j-inc].mtdDesc=(char *)soap_malloc(soap,strlen(desc)+1);
					memset((*historyResults)->__ptr[i].mtds->__ptr[j-inc].mtdDesc,0,strlen(desc)+1);
					memmove((*historyResults)->__ptr[i].mtds->__ptr[j-inc].mtdDesc,desc,strlen(desc));
				}


				if ((mtd_buf) && (mtd_buf->buf) && (mtd_buf->size>0))
				{

					(*historyResults)->__ptr[i].mtds->__ptr[j-inc].mtdValue=(char *)soap_malloc(soap,mtd_buf->size+1);

					memset((*historyResults)->__ptr[i].mtds->__ptr[j-inc].mtdValue,0,mtd_buf->size+1);

					memmove((*historyResults)->__ptr[i].mtds->__ptr[j-inc].mtdValue,mtd_buf->buf,mtd_buf->size);

				}
				else
				{

					set_gen_buf2(" ", 1, &mtd_buf);

					(*historyResults)->__ptr[i].mtds->__ptr[j-inc].mtdValue=(char *)soap_malloc(soap,mtd_buf->size+1);

					memset((*historyResults)->__ptr[i].mtds->__ptr[j-inc].mtdValue,0,mtd_buf->size+1);

					memmove((*historyResults)->__ptr[i].mtds->__ptr[j-inc].mtdValue,mtd_buf->buf,mtd_buf->size);

				}

			}
			else
			{
				inc++;
				(*historyResults)->__ptr[i].mtds->__size=form_count-inc;
			}


		}

		tmp=NULL;
		free(updateReason); updateReason=NULL;
		free(updateDate); updateDate=NULL;
		free(updateOwner); updateOwner=NULL;

	}

	PRINT_SOAP("Status:\t\trequest served correctly\n");
	bmd_logout(&bi, deserialisation_max_memory);
	bmd_end();

	/******************/
	/*	porzadki	*/
	/******************/
	free(updateReason); updateReason=NULL;
	free(updateDate); updateDate=NULL;
	free(updateOwner); updateOwner=NULL;
	free(sort_oid); sort_oid=NULL;
	free(sort_order); sort_order=NULL;
	free(desc); desc=NULL;
	free(oid); oid=NULL;

	return SOAP_OK;
}


int bmd230__bmdGetAccessObjects(struct soap *soap,
			struct xsd__base64Binary *cert,
			char *userRoleId, char *userGroupId, char *userClassId,
			long int bmdId, enum visibilityType_t visible, struct bmd230__accessObjectList** accessObjects)
{
long max_datagram_in_set_transmission	= 0;
long deserialisation_max_memory		= 0;

char *derUserCert=NULL;
long derUserCertLen=0;
char* paramCert=NULL;
long paramCertLen=0;

long iter = 0;
long jupiter = 0;
long retVal = 0;
struct bmd230__accessObjectList* tmpAccessObjects = NULL;
bmdDatagram_t *datagramPtr = NULL;

bmd_info_t *bi				= NULL;
bmdDatagram_t *dtg			= NULL;
bmdDatagramSet_t *req_dtg_set		= NULL;
bmdDatagramSet_t *resp_dtg_set		= NULL;

	if(bmdId<=0)
		{ SOAP_FOK(BMDSOAP_SERVER_INVALID_FILE_ID); }

	bmdconf_get_value_int(_GLOBAL_konfiguracja,"bmd","max_datagram_in_set_transmission", &max_datagram_in_set_transmission);
	if(max_datagram_in_set_transmission <= 0)
	{
		max_datagram_in_set_transmission = 100;
	}

	bmdconf_get_value_int(_GLOBAL_konfiguracja, "bmd", "deserialisation_max_memory", &deserialisation_max_memory);
	if(deserialisation_max_memory < 0)
	{
		deserialisation_max_memory = 0;
	}
	else
	{
		deserialisation_max_memory = deserialisation_max_memory * 1024 * 1024;
	}

	if(_GLOBAL_soap_wssecurity_enabled == 1)
	{
		retVal=GetWSSEInformation(soap, NULL, NULL, NULL, &derUserCert, &derUserCertLen);
		if(retVal < 0)
		{
			PRINT_ERROR("Unable to get certificate from WS Security header %li\n", retVal);
			SOAP_FOK(BMDSOAP_SERVER_NO_WSSECURITY_CERTIFICATE);
		}
	}
	else
	{
		if(_GLOBAL_soap_certificate_required == 1)
		{
			if(cert == NULL)
				{ SOAP_FOK(BMDSOAP_SERVER_EMPTY_LOGIN_CERTIFICATE); }
			else
			{
				paramCert=(char*)cert->__ptr;
				paramCertLen=(long)cert->__size;
			}
		}
		else
		{
			if(cert != NULL)
			{
				paramCert=(char*)cert->__ptr;
				paramCertLen=(long)cert->__size;
			}
		}
	}

	/************************/
	/*	logowanie do bmd	*/
	/************************/
	retVal=PrepareDataForBMDLoginAndRequest(&bi,
				(_GLOBAL_soap_wssecurity_enabled == 1  ? derUserCert : paramCert),
				(_GLOBAL_soap_wssecurity_enabled == 1  ? derUserCertLen : paramCertLen),
				userRoleId, userGroupId, deserialisation_max_memory);
	free(derUserCert); derUserCert=NULL;
	paramCert=NULL; paramCertLen=0;
	if(retVal < 0)
		{ SOAP_FOK(retVal); }

	/******************************/
	/*	stworzenie datagramu	*/
	/******************************/
	retVal=bmd_datagram_create(BMD_DATAGRAM_TYPE_GET_ACCESS_OBJECTS,&dtg);
	if(retVal < 0)
	{
		bmd_info_destroy(&bi);
		SOAP_FOK_LOGOUT(retVal);
	}

	if (userClassId!=NULL)
	{
		retVal=bmd_datagram_add_metadata_char(	OID_ACTION_METADATA_CERT_CHOSEN_CLASS_ID, userClassId, &dtg);
		if(retVal < 0)
		{
			bmd_info_destroy(&bi);
			bmd_datagram_free(&dtg);
			SOAP_FOK_LOGOUT(retVal);
		}
	}

	/******************************/
	/*	ustawienie id pliku	*/
	/******************************/
	retVal=bmd_datagram_set_id((const long *)&bmdId, &dtg);
	if(retVal < 0)
	{
		bmd_info_destroy(&bi);
		bmd_datagram_free(&dtg);
		SOAP_FOK_LOGOUT(retVal);
	}

	retVal=bmd_datagram_add_metadata_char(OID_SYS_METADATA_CRYPTO_OBJECTS_VISIBLE, (visible == INVISIBLE ? "0" : "1"), &dtg);
	//retVal=set_gen_buf2("INVISIBLE", strlen("INVISIBLE"), &(dtg->protocolData));
	if(retVal < 0)
	{
		bmd_info_destroy(&bi);
		bmd_datagram_free(&dtg);
		SOAP_FOK_LOGOUT(retVal);
	}

	/************************************/
	/*	dodanie datagramu do zestawu	*/
	/************************************/
	retVal=bmd_datagram_add_to_set(dtg,&req_dtg_set);
	if(retVal < 0)
	{
		bmd_info_destroy(&bi);
		bmd_datagram_free(&dtg);
		SOAP_FOK_LOGOUT(retVal);
	}

	/******************************************/
	/*	wyslanie zadania do serwera bmd	*/
	/******************************************/
	retVal=bmd_send_request(bi,req_dtg_set, max_datagram_in_set_transmission, &resp_dtg_set, 1, deserialisation_max_memory);
	bmd_datagramset_free(&req_dtg_set);
	bmd_logout(&bi, deserialisation_max_memory);
	bmd_info_destroy(&bi);
	if(retVal < 0)
	{
		SOAP_FOK(retVal);
	}

	retVal=resp_dtg_set->bmdDatagramSetTable[0]->datagramStatus;
	if(retVal < 0)
	{
		bmd_datagramset_free(&resp_dtg_set);
		SOAP_FOK(retVal);
	}

	/**************************************/
	/* konstruowanie struktury odpowiedzi */
	/**************************************/
	
	tmpAccessObjects=(struct bmd230__accessObjectList*)soap_malloc(soap, sizeof(struct bmd230__accessObjectList));
	if(tmpAccessObjects == NULL)
	{
		bmd_datagramset_free(&resp_dtg_set);
		SOAP_FOK(NO_MEMORY);
	}
	memset(tmpAccessObjects,0,sizeof(struct bmd230__accessObjectList));

	tmpAccessObjects->__size=resp_dtg_set->bmdDatagramSetSize;
	tmpAccessObjects->__ptr=(struct bmd230__accessObject*)soap_malloc(soap, sizeof(struct bmd230__accessObject)*(tmpAccessObjects->__size));
	if(tmpAccessObjects->__ptr == NULL)
	{
		bmd_datagramset_free(&resp_dtg_set);
		SOAP_FOK(NO_MEMORY);
	}
	memset(tmpAccessObjects->__ptr, 0, sizeof(struct bmd230__accessObject) * (tmpAccessObjects->__size));

	for(iter=0; iter<tmpAccessObjects->__size; iter++)
	{
		datagramPtr=resp_dtg_set->bmdDatagramSetTable[iter];

		for(jupiter=0; jupiter<datagramPtr->no_of_sysMetaData ; jupiter++)
		{
			if(strcmp(datagramPtr->sysMetaData[jupiter]->OIDTableBuf, OID_SYS_METADATA_CRYPTO_OBJECTS_ID) == 0)
			{
				tmpAccessObjects->__ptr[iter].id=soap_malloc(soap, sizeof(char)*( (datagramPtr->sysMetaData[jupiter]->AnySize)+1 ) );
				if(tmpAccessObjects->__ptr[iter].id == NULL)
				{
					bmd_datagramset_free(&resp_dtg_set);
					SOAP_FOK(NO_MEMORY);
				}
				memset(tmpAccessObjects->__ptr[iter].id, 0, sizeof(char)*((datagramPtr->sysMetaData[jupiter]->AnySize)+1));
				strcpy(tmpAccessObjects->__ptr[iter].id, datagramPtr->sysMetaData[jupiter]->AnyBuf);
			}
			else if(strcmp(datagramPtr->sysMetaData[jupiter]->OIDTableBuf, OID_SYS_METADATA_CRYPTO_OBJECTS_GROUP) == 0)
			{
				tmpAccessObjects->__ptr[iter].ownerGroupId=soap_malloc(soap, sizeof(char)*((datagramPtr->sysMetaData[jupiter]->AnySize)+1) );
				if(tmpAccessObjects->__ptr[iter].ownerGroupId == NULL)
				{
					bmd_datagramset_free(&resp_dtg_set);
					SOAP_FOK(NO_MEMORY);
				}
				memset(tmpAccessObjects->__ptr[iter].ownerGroupId, 0, sizeof(char)*((datagramPtr->sysMetaData[jupiter]->AnySize)+1) );
				strcpy(tmpAccessObjects->__ptr[iter].ownerGroupId, datagramPtr->sysMetaData[jupiter]->AnyBuf);
			}
			else if(strcmp(datagramPtr->sysMetaData[jupiter]->OIDTableBuf, OID_SYS_METADATA_CRYPTO_OBJECTS_FILE_TYPE) == 0)
			{
				tmpAccessObjects->__ptr[iter].fileType=soap_malloc(soap, sizeof(char)*((datagramPtr->sysMetaData[jupiter]->AnySize)+1) );
				if(tmpAccessObjects->__ptr[iter].fileType == NULL)
				{
					bmd_datagramset_free(&resp_dtg_set);
					SOAP_FOK(NO_MEMORY);
				}
				memset(tmpAccessObjects->__ptr[iter].fileType, 0, sizeof(char)*((datagramPtr->sysMetaData[jupiter]->AnySize)+1));
				strcpy(tmpAccessObjects->__ptr[iter].fileType, datagramPtr->sysMetaData[jupiter]->AnyBuf);
			}
			else if(strcmp(datagramPtr->sysMetaData[jupiter]->OIDTableBuf, OID_SYS_METADATA_CRYPTO_OBJECTS_OWNER) == 0)
			{
				tmpAccessObjects->__ptr[iter].ownerIdentityId=soap_malloc(soap, sizeof(char)*((datagramPtr->sysMetaData[jupiter]->AnySize)+1) );
				if(tmpAccessObjects->__ptr[iter].ownerIdentityId == NULL)
				{
					bmd_datagramset_free(&resp_dtg_set);
					SOAP_FOK(NO_MEMORY);
				}
				memset(tmpAccessObjects->__ptr[iter].ownerIdentityId, 0, sizeof(char)*((datagramPtr->sysMetaData[jupiter]->AnySize)+1));
				strcpy(tmpAccessObjects->__ptr[iter].ownerIdentityId, datagramPtr->sysMetaData[jupiter]->AnyBuf);
			}
			else if(strcmp(datagramPtr->sysMetaData[jupiter]->OIDTableBuf, OID_SYS_METADATA_CRYPTO_OBJECTS_CREATOR) == 0)
			{
				tmpAccessObjects->__ptr[iter].creatorIdentityId=soap_malloc(soap, sizeof(char)*((datagramPtr->sysMetaData[jupiter]->AnySize)+1) );
				if(tmpAccessObjects->__ptr[iter].creatorIdentityId == NULL)
				{
					bmd_datagramset_free(&resp_dtg_set);
					SOAP_FOK(NO_MEMORY);
				}
				memset(tmpAccessObjects->__ptr[iter].creatorIdentityId, 0, sizeof(char)*((datagramPtr->sysMetaData[jupiter]->AnySize)+1));
				strcpy(tmpAccessObjects->__ptr[iter].creatorIdentityId, datagramPtr->sysMetaData[jupiter]->AnyBuf);
			}
		}
	}

	*accessObjects=tmpAccessObjects;
	tmpAccessObjects=NULL;
	PRINT_SOAP("Status:\t\trequest served correctly\n");
	bmd_end();

	return SOAP_OK;
}


int bmd230__bmdGetVersionsHistory(struct soap *soap, struct xsd__base64Binary *cert, char *userRoleId, char *userGroupId, char *userClassId, struct bmd230__LongNumbersList *fileIds, struct bmd230__GetVersionsHistoryResultList** result)
{
long max_datagram_in_set_transmission	= 0;
long deserialisation_max_memory		= 0;

char *derUserCert=NULL;
long derUserCertLen=0;
char* paramCert=NULL;
long paramCertLen=0;

long iter = 0;
long jupiter = 0;
long retVal = 0;
struct bmd230__GetVersionsHistoryResultList* tempResult = NULL;
bmdDatagram_t *datagramPtr = NULL;
long versionsCount			= 0;
long idIndex				= 0;
long insertDateIndex		= 0;

bmd_info_t *bi				= NULL;
bmdDatagram_t *dtg			= NULL;
bmdDatagramSet_t *req_dtg_set		= NULL;
bmdDatagramSet_t *resp_dtg_set		= NULL;

	PRINT_SOAP("Request:\tget archive file versions history\n");
	
	if(fileIds == NULL)
		{ SOAP_FOK(BMDSOAP_SERVER_EMPTY5_PARAM_ERROR); }
	if(fileIds->__size <= 0 || fileIds->__ptr == NULL)
		{ SOAP_FOK(BMDSOAP_SERVER_EMPTY5_PARAM_ERROR); }
	for(iter=0; iter<fileIds->__size; iter++)
	{
		if(fileIds->__ptr[iter] <= 0)
			{ SOAP_FOK(BMDSOAP_SERVER_INVALID_FILE_ID); }
	}
	if(result == NULL)
		{ SOAP_FOK(BMDSOAP_SERVER_EMPTY6_PARAM_ERROR); }
	

	bmdconf_get_value_int(_GLOBAL_konfiguracja,"bmd","max_datagram_in_set_transmission", &max_datagram_in_set_transmission);
	if(max_datagram_in_set_transmission <= 0)
	{
		max_datagram_in_set_transmission = 100;
	}

	bmdconf_get_value_int(_GLOBAL_konfiguracja, "bmd", "deserialisation_max_memory", &deserialisation_max_memory);
	if(deserialisation_max_memory < 0)
	{
		deserialisation_max_memory = 0;
	}
	else
	{
		deserialisation_max_memory = deserialisation_max_memory * 1024 * 1024;
	}

	if(_GLOBAL_soap_wssecurity_enabled == 1)
	{
		retVal=GetWSSEInformation(soap, NULL, NULL, NULL, &derUserCert, &derUserCertLen);
		if(retVal < 0)
		{
			PRINT_ERROR("Unable to get certificate from WS Security header %li\n", retVal);
			SOAP_FOK(BMDSOAP_SERVER_NO_WSSECURITY_CERTIFICATE);
		}
	}
	else
	{
		if(_GLOBAL_soap_certificate_required == 1)
		{
			if(cert == NULL)
				{ SOAP_FOK(BMDSOAP_SERVER_EMPTY_LOGIN_CERTIFICATE); }
			else
			{
				paramCert=(char*)cert->__ptr;
				paramCertLen=(long)cert->__size;
			}
		}
		else
		{
			if(cert != NULL)
			{
				paramCert=(char*)cert->__ptr;
				paramCertLen=(long)cert->__size;
			}
		}
	}


	for(iter=0; iter<fileIds->__size; iter++)
	{
		retVal=bmd_datagram_create(BMD_DATAGRAM_TYPE_GET_VERSIONS_HISTORY, &dtg);
		if(retVal < 0)
		{
			bmd_datagramset_free(&req_dtg_set);
			SOAP_FOK(retVal);
		}

		if(userClassId != NULL)
		{
			retVal=bmd_datagram_add_metadata_char(OID_ACTION_METADATA_CERT_CHOSEN_CLASS_ID, userClassId, &dtg);
			if(retVal < 0)
			{
				bmd_datagramset_free(&req_dtg_set);
				bmd_datagram_free(&dtg);
				SOAP_FOK(retVal);
			}
		}

		retVal=bmd_datagram_set_id(&(fileIds->__ptr[iter]), &dtg);
		if(retVal < 0)
		{
			bmd_datagramset_free(&req_dtg_set);
			bmd_datagram_free(&dtg);
			SOAP_FOK(retVal);
		}

		retVal=bmd_datagram_add_to_set(dtg, &req_dtg_set);
		if(retVal < 0)
		{
			bmd_datagramset_free(&req_dtg_set);
			bmd_datagram_free(&dtg);
			SOAP_FOK(retVal);
		}
		dtg=NULL;
	}
	
	// logowanie do bmd
	retVal=PrepareDataForBMDLoginAndRequest(&bi,
				(_GLOBAL_soap_wssecurity_enabled == 1  ? derUserCert : paramCert),
				(_GLOBAL_soap_wssecurity_enabled == 1  ? derUserCertLen : paramCertLen),
				userRoleId, userGroupId, deserialisation_max_memory);
	free(derUserCert); derUserCert=NULL;
	paramCert=NULL; paramCertLen=0;
	if(retVal < 0)
	{
		bmd_datagramset_free(&req_dtg_set);
		SOAP_FOK(retVal);
	}

	retVal=bmd_send_request(bi, req_dtg_set, max_datagram_in_set_transmission, &resp_dtg_set, 1, deserialisation_max_memory);
	bmd_datagramset_free(&req_dtg_set);
	bmd_logout(&bi, deserialisation_max_memory);
	bmd_info_destroy(&bi);
	if(retVal < 0)
	{
		SOAP_FOK(retVal);
	}

	// konstruowanie struktury odpowiedzi
	tempResult=(struct bmd230__GetVersionsHistoryResultList*)soap_malloc(soap, sizeof(struct bmd230__GetVersionsHistoryResultList));
	if(tempResult == NULL)
	{
		bmd_datagramset_free(&resp_dtg_set);
		SOAP_FOK(NO_MEMORY);
	}
	tempResult->__size=resp_dtg_set->bmdDatagramSetSize;
	tempResult->__ptr=(struct bmd230__GetVersionsHistoryResult*)soap_malloc(soap, (tempResult->__size) * sizeof(struct bmd230__GetVersionsHistoryResult));
	if(tempResult->__ptr == NULL)
	{
		tempResult->__size=0;
		bmd_datagramset_free(&resp_dtg_set);
		SOAP_FOK(NO_MEMORY);
	}
	
	for(iter=0; iter<tempResult->__size; iter++)
	{
		datagramPtr=resp_dtg_set->bmdDatagramSetTable[iter];
		if(datagramPtr == NULL)
		{
			bmd_datagramset_free(&resp_dtg_set);
			SOAP_FOK(BMDSOAP_SERVER_INTERNAL_OPERATION_ERROR);
		}
		
		tempResult->__ptr[iter].errorCode=datagramPtr->datagramStatus;
		if(tempResult->__ptr[iter].errorCode < BMD_OK)
			{ tempResult->__ptr[iter].versionsInfoList=NULL; }
		else
		{
			tempResult->__ptr[iter].versionsInfoList=(struct bmd230__FileVersionInfoList*)soap_malloc(soap, sizeof(struct bmd230__FileVersionInfoList));
			if(tempResult->__ptr[iter].versionsInfoList == NULL)
			{
				bmd_datagramset_free(&resp_dtg_set);
				SOAP_FOK(NO_MEMORY);
			}
			memset(tempResult->__ptr[iter].versionsInfoList, 0, sizeof(struct bmd230__FileVersionInfoList));
			
			//policzenie, ile znaleziono wersji archiwalnych pliku
			versionsCount=0;
			for(jupiter=0; jupiter<datagramPtr->no_of_sysMetaData; jupiter++)
			{
				if(datagramPtr->sysMetaData[jupiter] != NULL && datagramPtr->sysMetaData[jupiter]->OIDTableBuf != NULL)
				{
					if(strcmp(datagramPtr->sysMetaData[jupiter]->OIDTableBuf, OID_SYS_METADATA_CRYPTO_OBJECTS_ID) == 0)
						{ versionsCount++; }
				}
			}
			
			tempResult->__ptr[iter].versionsInfoList->__size=versionsCount;
			tempResult->__ptr[iter].versionsInfoList->__ptr=(struct bmd230__FileVersionInfo*)soap_malloc(soap, (tempResult->__ptr[iter].versionsInfoList->__size) * sizeof(struct bmd230__FileVersionInfo));
			if(tempResult->__ptr[iter].versionsInfoList->__ptr == NULL)
			{
				tempResult->__ptr[iter].versionsInfoList->__size=0;
				bmd_datagramset_free(&resp_dtg_set);
				SOAP_FOK(NO_MEMORY);
			}
			memset(tempResult->__ptr[iter].versionsInfoList->__ptr, 0, (tempResult->__ptr[iter].versionsInfoList->__size) * sizeof(struct bmd230__FileVersionInfo));
			
			//uzupelnienie identyfikatorow wersji archiwalnych i czasow ich wstawienia
			idIndex=0;
			insertDateIndex=0;
			for(jupiter=0; jupiter<datagramPtr->no_of_sysMetaData; jupiter++)
			{
				if(datagramPtr->sysMetaData[jupiter] != NULL && datagramPtr->sysMetaData[jupiter]->OIDTableBuf != NULL )
				{
					if(strcmp(datagramPtr->sysMetaData[jupiter]->OIDTableBuf, OID_SYS_METADATA_CRYPTO_OBJECTS_ID) == 0)
					{
						if(datagramPtr->sysMetaData[jupiter]->AnyBuf != NULL && datagramPtr->sysMetaData[jupiter]->AnySize > 0)
						{
							tempResult->__ptr[iter].versionsInfoList->__ptr[idIndex].id=strtol(datagramPtr->sysMetaData[jupiter]->AnyBuf , NULL, 10);
							if(tempResult->__ptr[iter].versionsInfoList->__ptr[idIndex].id == LONG_MIN || tempResult->__ptr[iter].versionsInfoList->__ptr[idIndex].id == LONG_MAX)
							{
								bmd_datagramset_free(&resp_dtg_set);
								SOAP_FOK(BMDSOAP_SERVER_INTERNAL_OPERATION_ERROR);
							}
						}
						else
						{
							bmd_datagramset_free(&resp_dtg_set);
							SOAP_FOK(BMDSOAP_SERVER_INTERNAL_OPERATION_ERROR);
						}
						idIndex++;
					}
					else if(strcmp(datagramPtr->sysMetaData[jupiter]->OIDTableBuf, OID_SYS_METADATA_CRYPTO_OBJECTS_TIMESTAMP) == 0)
					{
						if(datagramPtr->sysMetaData[jupiter]->AnyBuf != NULL && datagramPtr->sysMetaData[jupiter]->AnySize > 0)
						{
							tempResult->__ptr[iter].versionsInfoList->__ptr[insertDateIndex].insertDate=(char*)soap_malloc(soap, (datagramPtr->sysMetaData[jupiter]->AnySize+1)*sizeof(char));
							if(tempResult->__ptr[iter].versionsInfoList->__ptr[insertDateIndex].insertDate != NULL)
							{
								memcpy(tempResult->__ptr[iter].versionsInfoList->__ptr[insertDateIndex].insertDate, datagramPtr->sysMetaData[jupiter]->AnyBuf, datagramPtr->sysMetaData[jupiter]->AnySize);
								tempResult->__ptr[iter].versionsInfoList->__ptr[insertDateIndex].insertDate[datagramPtr->sysMetaData[jupiter]->AnySize]='\0';
							}
							else
							{
								bmd_datagramset_free(&resp_dtg_set);
								SOAP_FOK(BMDSOAP_SERVER_INTERNAL_OPERATION_ERROR);
							}
						}
						else
						{
							bmd_datagramset_free(&resp_dtg_set);
							SOAP_FOK(BMDSOAP_SERVER_INTERNAL_OPERATION_ERROR);
						}
						insertDateIndex++;
					}
				}
			}
		}
	}
	
	bmd_datagramset_free(&resp_dtg_set);
	*result=tempResult;
	tempResult=NULL;
	
	PRINT_SOAP("Status:\t\trequest served correctly\n");
	bmd_end();

	return SOAP_OK;
}
