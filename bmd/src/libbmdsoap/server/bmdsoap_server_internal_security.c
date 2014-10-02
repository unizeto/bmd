#include <bmd/libbmdsoap/libbmdsoap.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>

int bmd230__bmdGetSecurityList(	struct soap *soap,
 				struct xsd__base64Binary *cert,
				char *roleName,
				char *groupName,
				char *userClassId,
 				struct bmd230__securityInfo **result)
{
long i					= 0;
long j					= 0;
long max_metadata_value_length		= 0;
long max_datagram_in_set_transmission	= 0;
long deserialisation_max_memory		= 0;
bmd_info_t *bi				= NULL;
bmdDatagram_t *dtg			= NULL;
bmdDatagramSet_t *dtg_set		= NULL;
bmdDatagramSet_t *resp_dtg_set		= NULL;

char *derUserCert=NULL;
long derUserCertLen=0;
long retVal=0;
char* paramCert=NULL;
long paramCertLen=0;

	PRINT_SOAP("Request:\tget security list\n");
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

	/************************/
	/* stworzenie datagramu */
	/************************/
	SOAP_FOK_LOGOUT(bmd_datagram_create(BMD_DATAGRAM_TYPE_GET_SECURITY_LIST, &dtg));
	if (userClassId!=NULL)
	{
		SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(	OID_ACTION_METADATA_CERT_CHOSEN_CLASS_ID, userClassId, &dtg));
	}

	/********************************/
	/* dodanie datagramu do zestawu */
	/********************************/
	SOAP_FOK_LOGOUT(bmd_datagram_add_to_set(dtg,&dtg_set));

	/***********************************/
	/* wyslanie zadania do serwera bmd */
	/***********************************/
	SOAP_FOK_LOGOUT(bmd_send_request(bi, dtg_set, max_datagram_in_set_transmission, &resp_dtg_set,1, deserialisation_max_memory));

	SOAP_FOK_LOGOUT(resp_dtg_set->bmdDatagramSetTable[0]->datagramStatus);

	/****************************/
	/* przygotowanie odpowiedzi */
	/****************************/
	(*result)=(struct bmd230__securityInfo *)malloc(sizeof(struct bmd230__securityInfo));
	memset((*result), 0, sizeof(struct bmd230__securityInfo));

	(*result)->secCategoryInfo=(struct bmd230__secCategoryInfo *)malloc(sizeof(struct bmd230__secCategoryInfo));
	(*result)->secCategoryInfo->__size=0;
	(*result)->secCategoryInfo->__ptr=(struct bmd230__singleSecCategoryInfo *)malloc(sizeof(struct bmd230__singleSecCategoryInfo)*(*result)->secCategoryInfo->__size);
	memset((*result)->secCategoryInfo->__ptr, 0, sizeof(struct bmd230__singleSecCategoryInfo)*(*result)->secCategoryInfo->__size);

	(*result)->secLevelInfo=(struct bmd230__secLevelInfo *)malloc(sizeof(struct bmd230__secLevelInfo));
	(*result)->secLevelInfo->__size=0;
	(*result)->secLevelInfo->__ptr=(struct bmd230__singleSecLevelInfo *)malloc(sizeof(struct bmd230__singleSecLevelInfo)*(*result)->secLevelInfo->__size);
	memset((*result)->secLevelInfo->__ptr, 0, sizeof(struct bmd230__singleSecLevelInfo)*(*result)->secLevelInfo->__size);

	for (i=0; i<resp_dtg_set->bmdDatagramSetSize; i++)
	{

		if (resp_dtg_set->bmdDatagramSetTable[i]->additionalMetaData!=NULL)
		{

			if (resp_dtg_set->bmdDatagramSetTable[i]->additionalMetaData[0]->OIDTableBuf!=NULL)
			{

				if (strcmp(resp_dtg_set->bmdDatagramSetTable[i]->additionalMetaData[0]->OIDTableBuf, OID_SYS_METADATA_CERT_SEC_CATEGORY_ID)==0)
				{

					(*result)->secCategoryInfo->__size++;
					(*result)->secCategoryInfo->__ptr=(struct bmd230__singleSecCategoryInfo *)realloc((*result)->secCategoryInfo->__ptr, sizeof(struct bmd230__singleSecCategoryInfo)*(*result)->secCategoryInfo->__size);
					asprintf(&((*result)->secCategoryInfo->__ptr[(*result)->secCategoryInfo->__size-1].secCategoryId), "%s", resp_dtg_set->bmdDatagramSetTable[i]->additionalMetaData[0]->AnyBuf);

					(*result)->secCategoryInfo->__ptr[i].secCategoryParentNames=(struct bmd230__stringList *)malloc(sizeof(struct bmd230__stringList));
					memset((*result)->secCategoryInfo->__ptr[i].secCategoryParentNames, 0, sizeof(struct bmd230__stringList));
					(*result)->secCategoryInfo->__ptr[i].secCategoryParentNames->__size=0;
					(*result)->secCategoryInfo->__ptr[i].secCategoryParentNames->__ptr=NULL;


					(*result)->secCategoryInfo->__ptr[i].secCategoryChildNames=(struct bmd230__stringList *)malloc(sizeof(struct bmd230__stringList));
					memset((*result)->secCategoryInfo->__ptr[i].secCategoryChildNames, 0, sizeof(struct bmd230__stringList));
					(*result)->secCategoryInfo->__ptr[i].secCategoryChildNames->__size=0;
					(*result)->secCategoryInfo->__ptr[i].secCategoryChildNames->__ptr=NULL;



					for (j=1; j<resp_dtg_set->bmdDatagramSetTable[i]->no_of_additionalMetaData; j++)
					{
						if (strcmp(resp_dtg_set->bmdDatagramSetTable[i]->additionalMetaData[j]->OIDTableBuf, OID_SYS_METADATA_CERT_SEC_CATEGORY_NAME)==0)
						{

							asprintf(&((*result)->secCategoryInfo->__ptr[(*result)->secCategoryInfo->__size-1].secCategoryName), "%s", resp_dtg_set->bmdDatagramSetTable[i]->additionalMetaData[j]->AnyBuf);

						}
						else if (strcmp(resp_dtg_set->bmdDatagramSetTable[i]->additionalMetaData[j]->OIDTableBuf, OID_SYS_METADATA_CERT_SEC_CATEGORY_PARENT_NAME)==0)
						{

							(*result)->secCategoryInfo->__ptr[i].secCategoryParentNames->__size++;

							(*result)->secCategoryInfo->__ptr[i].secCategoryParentNames->__ptr=(xsd__string *)realloc((*result)->secCategoryInfo->__ptr[i].secCategoryParentNames->__ptr, sizeof(xsd__string)*(*result)->secCategoryInfo->__ptr[i].secCategoryParentNames->__size);

							asprintf(&((*result)->secCategoryInfo->__ptr[(*result)->secCategoryInfo->__size-1].secCategoryParentNames->__ptr[(*result)->secCategoryInfo->__ptr[i].secCategoryParentNames->__size-1]), "%s", resp_dtg_set->bmdDatagramSetTable[i]->additionalMetaData[j]->AnyBuf);

						}
						else if (strcmp(resp_dtg_set->bmdDatagramSetTable[i]->additionalMetaData[j]->OIDTableBuf, OID_SYS_METADATA_CERT_SEC_CATEGORY_CHILD_NAME)==0)
						{

							(*result)->secCategoryInfo->__ptr[i].secCategoryChildNames->__size++;
							(*result)->secCategoryInfo->__ptr[i].secCategoryChildNames->__ptr=(xsd__string *)realloc((*result)->secCategoryInfo->__ptr[i].secCategoryChildNames->__ptr, sizeof(xsd__string)*(*result)->secCategoryInfo->__ptr[i].secCategoryChildNames->__size);
							asprintf(&((*result)->secCategoryInfo->__ptr[i].secCategoryChildNames->__ptr[(*result)->secCategoryInfo->__ptr[i].secCategoryChildNames->__size-1]), "%s", resp_dtg_set->bmdDatagramSetTable[i]->additionalMetaData[j]->AnyBuf);

						}

					}
				}
				else if (strcmp(resp_dtg_set->bmdDatagramSetTable[i]->additionalMetaData[0]->OIDTableBuf, OID_SYS_METADATA_CERT_SEC_LEVEL_ID)==0)
				{

					(*result)->secLevelInfo->__size++;
					(*result)->secLevelInfo->__ptr=(struct bmd230__singleSecLevelInfo *)realloc((*result)->secLevelInfo->__ptr, sizeof(struct bmd230__singleSecLevelInfo)*(*result)->secLevelInfo->__size);
					asprintf(&((*result)->secLevelInfo->__ptr[(*result)->secLevelInfo->__size-1].secLevelId), "%s", resp_dtg_set->bmdDatagramSetTable[i]->additionalMetaData[0]->AnyBuf);

					for (j=1; j<resp_dtg_set->bmdDatagramSetTable[i]->no_of_additionalMetaData; j++)
					{

						if (strcmp(resp_dtg_set->bmdDatagramSetTable[i]->additionalMetaData[j]->OIDTableBuf, OID_SYS_METADATA_CERT_SEC_LEVEL_NAME)==0)
						{
							asprintf(&((*result)->secLevelInfo->__ptr[(*result)->secLevelInfo->__size-1].secLevelName), "%s", resp_dtg_set->bmdDatagramSetTable[i]->additionalMetaData[j]->AnyBuf);

						}
						else if (strcmp(resp_dtg_set->bmdDatagramSetTable[i]->additionalMetaData[j]->OIDTableBuf, OID_SYS_METADATA_CERT_SEC_LEVEL_PRIORITY)==0)
						{
							asprintf(&((*result)->secLevelInfo->__ptr[(*result)->secLevelInfo->__size-1].secLevelPriority), "%s", resp_dtg_set->bmdDatagramSetTable[i]->additionalMetaData[j]->AnyBuf);

						}

					}

				}


			}

		}


	}

	PRINT_SOAP("Status:\t\trequest served correctly\n");

	/************/
	/* porzadki */
	/************/
 	bmd_datagramset_free(&resp_dtg_set);
 	bmd_datagramset_free(&dtg_set);
	bmd_logout(&bi, deserialisation_max_memory);
	bmd_end();

	return SOAP_OK;
}


int bmd230__bmdRegisterNewSecurityCategory(	struct soap *soap,
					struct xsd__base64Binary *cert,
					char *roleName,
					char *groupName,
					char *userClassId,
					struct bmd230__singleSecCategoryInfo *secCategoryInfo,
					int *result)

{
long i					= 0;
long max_metadata_value_length		= 0;
long max_datagram_in_set_transmission	= 0;
long deserialisation_max_memory		= 0;
bmd_info_t *bi				= NULL;
bmdDatagram_t *dtg			= NULL;
bmdDatagramSet_t *dtg_set		= NULL;
bmdDatagramSet_t *resp_dtg_set		= NULL;
GenBuf_t tmp_gb;

char *derUserCert=NULL;
long derUserCertLen=0;
long retVal=0;
char* paramCert=NULL;
long paramCertLen=0;

	PRINT_SOAP("Request:\tregister new security category\n");

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
	SOAP_FOK_LOGOUT(bmd_datagram_create(BMD_DATAGRAM_TYPE_REGISTER_SECURITY, &dtg));
	if (userClassId!=NULL)
	{
		SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(	OID_ACTION_METADATA_CERT_CHOSEN_CLASS_ID, userClassId, &dtg));
	}

	/*************************************/
	/* ustawienie kryteriow wyszukiwania */
	/*************************************/
	if (secCategoryInfo!=NULL)
	{
		/**************************************/
		/* wstawienie nazwy roli do datagramu */
		/**************************************/
		if (secCategoryInfo->secCategoryName)
		{
			tmp_gb.buf=(char *)secCategoryInfo->secCategoryName;
			tmp_gb.size=strlen(secCategoryInfo->secCategoryName);
			if (tmp_gb.size>max_metadata_value_length)	{	SOAP_FOK_LOGOUT(BMDSOAP_SERVER_METADATA_VALUE_TOO_LONG);	}
			SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_2_with_params(OID_SYS_METADATA_CERT_SEC_CATEGORY_NAME, &tmp_gb,&dtg, ADDITIONAL_METADATA, 0, 0, 0,1));
		}
		else
		{
			SOAP_FOK_LOGOUT(BMDSOAP_SERVER_SEC_CAT_NAME_NOT_DEFINED);
		}

		/********************************************/
		/* wstawienie listy akcji roli do datagramu */
		/********************************************/
		if ((secCategoryInfo->secCategoryParentNames) && (secCategoryInfo->secCategoryParentNames->__size))
		{
			for (i=0; i<secCategoryInfo->secCategoryParentNames->__size; i++)
			{
				tmp_gb.buf=(char *)secCategoryInfo->secCategoryParentNames->__ptr[i];
				tmp_gb.size=strlen(secCategoryInfo->secCategoryParentNames->__ptr[i]);
				if (tmp_gb.size>max_metadata_value_length)	{	SOAP_FOK_LOGOUT(BMDSOAP_SERVER_METADATA_VALUE_TOO_LONG);	}
				SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_2_with_params(OID_SYS_METADATA_CERT_SEC_CATEGORY_PARENT_NAME, &tmp_gb,&dtg, ADDITIONAL_METADATA, 0, 0, 0,1));
			}
		}

		/**************************************************/
		/* wstawienie listy group_rights roli do datagramu */
		/**************************************************/
		if ((secCategoryInfo->secCategoryChildNames) && (secCategoryInfo->secCategoryChildNames->__size))
		{
			for (i=0; i<secCategoryInfo->secCategoryChildNames->__size; i++)
			{
				tmp_gb.buf=(char *)secCategoryInfo->secCategoryChildNames->__ptr[i];
				tmp_gb.size=strlen(secCategoryInfo->secCategoryChildNames->__ptr[i]);
				if (tmp_gb.size>max_metadata_value_length)	{	SOAP_FOK_LOGOUT(BMDSOAP_SERVER_METADATA_VALUE_TOO_LONG);	}
				SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_2_with_params(OID_SYS_METADATA_CERT_SEC_CATEGORY_CHILD_NAME, &tmp_gb,&dtg, ADDITIONAL_METADATA, 0, 0, 0,1));
			}
		}
	}
	else
	{
		SOAP_FOK_LOGOUT(BMDSOAP_SERVER_SEC_CAT_INFO_NOT_DEFINED);
	}

	/************************************/
	/*	dodanie datagramu do zestawu	*/
	/************************************/
	SOAP_FOK_LOGOUT(bmd_datagram_add_to_set(dtg,&dtg_set));

	/******************************************/
	/*	wyslanie zadania do serwera bmd	*/
	/******************************************/
	SOAP_FOK_LOGOUT(bmd_send_request(bi, dtg_set, max_datagram_in_set_transmission, &resp_dtg_set,1, deserialisation_max_memory));
	SOAP_FOK_LOGOUT(resp_dtg_set->bmdDatagramSetTable[0]->datagramStatus);

	/****************************/
	/* przygotowanie odpowiedzi */
	/****************************/
	(*result)=strtol(resp_dtg_set->bmdDatagramSetTable[0]->protocolDataFileId->buf, NULL, 10);

	PRINT_SOAP("Status:\t\trequest served correctly\n");

	/************/
	/* porzadki */
	/************/
	bmd_datagramset_free(&resp_dtg_set);
	bmd_datagramset_free(&dtg_set);
	bmd_logout(&bi, deserialisation_max_memory);
	bmd_end();

	return SOAP_OK;
}
