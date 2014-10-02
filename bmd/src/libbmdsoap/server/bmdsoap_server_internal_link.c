#include <bmd/libbmdsoap/libbmdsoap.h>
#include <bmd/libbmdglobals/libbmdglobals.h>


int bmd230__bmdCreateLink(	struct soap *soap,
			struct xsd__base64Binary *cert,
			char *roleName,
			char *groupName,
			char *userClassId,
			struct bmd230__mtdsValues *mtds,
			char *description,
			char *linkName,
			char *transactionId,
			/*enum groupAuthorizationType_e groupAuthorization,*/
			enum grantingType_e granting,
			struct bmd230__DateTime* expirationDate,
			long *id)
{
long i						= 0;
long max_datagram_in_set_transmission		= 0;
long deserialisation_max_memory			= 0;
long max_metadata_value_length			= 0;
long start_transaction_required			= 0;
char *transaction_dir_name			= NULL;
char *tmp_dir					= NULL;
bmd_info_t *bi					= NULL;
bmdDatagram_t *dtg				= NULL;
bmdDatagramSet_t *dtg_set			= NULL;
bmdDatagramSet_t *resp_dtg_set			= NULL;
GenBuf_t tmp_gb1;
char* expirationDateString			= NULL;

char *derUserCert=NULL;
long derUserCertLen=0;
long retVal=0;
char* paramCert=NULL;
long paramCertLen=0;

	/************************/
	/* walidacja parametrow */
	/************************/
	if (description==NULL)		{	SOAP_FOK(BMD_ERR_PARAM6);	}
	if (linkName==NULL)		{	SOAP_FOK(BMD_ERR_PARAM7);	}

	(*id)=-1;

	PRINT_SOAP("Request:\tcreate link in archive\n");

	/***************************************/
	/* wczytanie ustawien konfiguracyjnych */
	/***************************************/
	if (bmdconf_get_value_int(_GLOBAL_konfiguracja,"soap","max_metadata_value_length",&max_metadata_value_length)<BMD_OK)
	{
		max_metadata_value_length=256;
	}
	bmdconf_get_value_int(_GLOBAL_konfiguracja,"bmd","max_datagram_in_set_transmission",
	&max_datagram_in_set_transmission);
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

	SOAP_FOK(bmdconf_get_value(_GLOBAL_konfiguracja,"soap","temp_dir",&tmp_dir));
	if(bmdconf_get_value_int(_GLOBAL_konfiguracja,"soap","start_transaction_required",&start_transaction_required) < BMD_OK)
	{
		start_transaction_required = 0;
	}

	/*Ustalam czy jest katalog transakcji*/
	if(transactionId != NULL)
	{/*if(transactionId != NULL)*/
		SOAP_FOK(twfun_check_transaction_directory( tmp_dir, transactionId, start_transaction_required,	&transaction_dir_name));
		SOAP_FOK(twfun_mark_start_of_insert(    transaction_dir_name, transactionId));
	}/*\if(transactionId != NULL)*/


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

	/************************************/
	/*	stworzenie datagramu zadania	*/
	/************************************/
	SOAP_FOK_LOGOUT_TRANSACTION(bmd_datagram_create(BMD_DATAGRAM_LINK_CREATE,&dtg));
	if (userClassId!=NULL)
	{
		retVal=bmd_datagram_add_metadata_char(OID_ACTION_METADATA_CERT_CHOSEN_CLASS_ID, userClassId, &dtg);
		if(retVal < BMD_OK)
		{
			bmd_datagram_free(&dtg);
			SOAP_FOK_LOGOUT_TRANSACTION(retVal);
		}
	}

	retVal=bmd_datagram_set_filename(linkName, &dtg);
	if(retVal < BMD_OK)
	{
		bmd_datagram_free(&dtg);
		SOAP_FOK_LOGOUT_TRANSACTION(retVal);
	}

	/*if(groupAuthorization > 0)
	{
		dtg->datagramStatus=BMD_LINK_CREATION_IGNORE_DEPENDENT_GROUPS;
	}*/

	if(granting == GRANTING_FORBIDDEN)
	{
		retVal=bmd_datagram_add_metadata_char(OID_SYS_METADATA_CRYPTO_OBJECTS_FOR_GRANT, "0", &dtg);
		if(retVal < BMD_OK)
		{
			bmd_datagram_free(&dtg);
			SOAP_FOK_LOGOUT_TRANSACTION(retVal);
		}
	}
	else if(granting == GRANTING_ALLOWED)
	{
		retVal=bmd_datagram_add_metadata_char(OID_SYS_METADATA_CRYPTO_OBJECTS_FOR_GRANT, "1", &dtg);
		if(retVal < BMD_OK)
		{
			bmd_datagram_free(&dtg);
			SOAP_FOK_LOGOUT_TRANSACTION(retVal);
		}
	}

	if(expirationDate != NULL)
	{
		retVal=bmdsoapGenerateDateTimeString(expirationDate, &expirationDateString);
		if(retVal < BMD_OK)
		{
			bmd_datagram_free(&dtg);
			SOAP_FOK_LOGOUT_TRANSACTION(retVal);
		}

		retVal=bmd_datagram_add_metadata_char(OID_SYS_METADATA_CRYPTO_OBJECTS_EXPIRATION_DATE, expirationDateString, &dtg);
		free(expirationDateString); expirationDateString=NULL;
		if(retVal < BMD_OK)
		{
			bmd_datagram_free(&dtg);
			SOAP_FOK_LOGOUT_TRANSACTION(retVal);
		}
	}

	/*************************/
	/* okreslenie metadanych */
	/*************************/
	if ((mtds!=NULL) && (mtds->__ptr!=NULL) && (mtds->__size))
	{
		for (i=0; i<mtds->__size; i++)
		{
			if ((mtds->__ptr[i].mtdOid==NULL) || (mtds->__ptr[i].mtdValue==NULL))
			{
				continue;
			}
			tmp_gb1.buf=(char *)mtds->__ptr[i].mtdValue;
			tmp_gb1.size=strlen(mtds->__ptr[i].mtdValue);

			if (tmp_gb1.size>max_metadata_value_length)
			{
				bmd_datagram_free(&dtg);
				SOAP_FOK_LOGOUT_TRANSACTION(BMDSOAP_SERVER_METADATA_VALUE_TOO_LONG);
			}

			retVal=bmd_datagram_add_metadata_2((char *)mtds->__ptr[i].mtdOid,&tmp_gb1,&dtg, 0);
			if(retVal < BMD_OK)
			{
				bmd_datagram_free(&dtg);
				SOAP_FOK_LOGOUT_TRANSACTION(retVal);
			}
		}
	}

	if (transactionId!=NULL)
	{
		tmp_gb1.buf=(char *)transactionId;
		tmp_gb1.size=strlen(transactionId);

		retVal=bmd_datagram_add_metadata_2((char *)OID_SYS_METADATA_CRYPTO_OBJECTS_TRANSACTION_ID,&tmp_gb1,&dtg, 0);
		if(retVal < BMD_OK)
		{
			bmd_datagram_free(&dtg);
			SOAP_FOK_LOGOUT_TRANSACTION(retVal);
		}
	}

	/*****************************/
	/* wstawienie opisu katalogu */
	/*****************************/
	if (description!=NULL)
	{
		retVal=set_gen_buf2( (char *)description, strlen(description), &(dtg->protocolData));
		if(retVal < BMD_OK)
		{
			bmd_datagram_free(&dtg);
			SOAP_FOK_LOGOUT_TRANSACTION(retVal);
		}
	}
	else
	{
		bmd_datagram_free(&dtg);
		SOAP_FOK_LOGOUT_TRANSACTION(-666);
	}

	retVal=bmd_datagram_add_to_set(dtg,&dtg_set);
	if(retVal < BMD_OK)
	{
		bmd_datagram_free(&dtg);
		SOAP_FOK_LOGOUT_TRANSACTION(retVal);
	}

	/************************/
	/*	wyslanie zadania	*/
	/************************/
	retVal=bmd_send_request(bi,dtg_set, max_datagram_in_set_transmission, &resp_dtg_set, 1, deserialisation_max_memory);
	bmd_datagramset_free(&dtg_set);
	SOAP_FOK_LOGOUT_TRANSACTION(retVal);

	/******************************/
	/*	odebranie odpowiedzi	*/
	/******************************/
	if((retVal=resp_dtg_set->bmdDatagramSetTable[0]->datagramStatus) < BMD_OK)
	{
		bmd_datagramset_free(&resp_dtg_set);
		
		// jesli ze wzgedu na duplikat nie stworzono linka, nie moze zwracac bledu
		if(retVal == LIBBMDSQL_NO_LINK_CREATED)
		{
			if(transactionId != NULL)
			{
				SOAP_FOK_LOGOUT(twfun_unmark_start_of_insert(	transaction_dir_name, transactionId, -1));
				free(transaction_dir_name); transaction_dir_name = NULL;
			}
			bmd_logout(&bi, deserialisation_max_memory);
			bmd_end();

			PRINT_SOAP("Status:\t\tNo link was created.\n");
			return SOAP_OK;
		}
		else
		{
			SOAP_FOK_LOGOUT_TRANSACTION(retVal);
		}
	}


	if (resp_dtg_set->bmdDatagramSetTable[0]->protocolDataFileId->buf!=NULL)
	{
		(*id)=atol((char *)resp_dtg_set->bmdDatagramSetTable[0]->protocolDataFileId->buf);
	}
	else
	{
		(*id)=-1;
	}
	bmd_datagramset_free(&resp_dtg_set);

	if(transactionId != NULL)
	{
		SOAP_FOK_LOGOUT(twfun_unmark_start_of_insert(	transaction_dir_name, transactionId, -1));
		free(transaction_dir_name); transaction_dir_name = NULL;
	}

	bmd_logout(&bi, deserialisation_max_memory);
	bmd_end();

	PRINT_SOAP("Status:\t\trequest served correctly\n");
	return SOAP_OK;
}


int bmd230__bmdCreateLinks(struct soap *soap,
			struct xsd__base64Binary *cert,
			char* userRoleId,
			char* userGroupId,
			char* userClassId,
			struct bmd230__linksInfo* linksInfo,
			char* transactionId,
			struct bmd230__idList** ids)
{
long iter	= 0;
char* defaultDescription = "<no description>";
char* expirationDateString = NULL;

long max_datagram_in_set_transmission		= 0;
long deserialisation_max_memory			= 0;
long max_metadata_value_length			= 0;

bmd_info_t *bi					= NULL;
bmdDatagram_t *dtg				= NULL;
bmdDatagramSet_t *dtg_set			= NULL;
bmdDatagramSet_t *resp_dtg_set			= NULL;

char *derUserCert=NULL;
long derUserCertLen=0;
long retVal=0;
char* paramCert=NULL;
long paramCertLen=0;


	PRINT_SOAP("Request:\tcreate links in archive\n");

	if(linksInfo == NULL)
		{ SOAP_FOK(BMD_ERR_PARAM5); }
	if(linksInfo->__size <= 0)
		{ SOAP_FOK(BMD_ERR_PARAM5); }
	if(ids == NULL)
		{ SOAP_FOK(BMD_ERR_PARAM6); }


	/***************************************/
	/* wczytanie ustawien konfiguracyjnych */
	/***************************************/
	if(bmdconf_get_value_int(_GLOBAL_konfiguracja,"soap","max_metadata_value_length",&max_metadata_value_length)<BMD_OK)
	{
		max_metadata_value_length=256;
	}
	bmdconf_get_value_int(_GLOBAL_konfiguracja,"bmd","max_datagram_in_set_transmission",
	&max_datagram_in_set_transmission);
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

	//sprawdzenie, czy podano porzebne dane wejsciowe
	for(iter=0; iter<linksInfo->__size; iter++)
	{
		if(linksInfo->__ptr[iter].pointingId == NULL)
			{ SOAP_FOK(BMDSOAP_POINTING_ID_NOT_SPECIFIED); }
		if(linksInfo->__ptr[iter].ownerGroupId == NULL)
			{ SOAP_FOK(BMDSOAP_OWNER_GROUP_NO_SPECIFIED); }
		if(linksInfo->__ptr[iter].linkName == NULL)
			{ SOAP_FOK(BMDSOAP_LINK_NAME_NOT_SPECIFIED); }
		if(linksInfo->__ptr[iter].ownerIdentityId == NULL && linksInfo->__ptr[iter].ownerCertificateId == NULL)
			{ SOAP_FOK(BMDSOAP_OWNER_NOT_SPECIFIED); }
	}

	for(iter=0; iter<linksInfo->__size; iter++)
	{
		retVal=bmd_datagram_create(BMD_DATAGRAM_LINK_CREATE, &dtg);
		if(retVal < BMD_OK)
		{
			bmd_datagram_free(&dtg);
			bmd_datagramset_free(&dtg_set);
			SOAP_FOK(retVal);
		}
		
		if (userClassId!=NULL)
		{
			retVal=bmd_datagram_add_metadata_char(OID_ACTION_METADATA_CERT_CHOSEN_CLASS_ID, userClassId, &dtg);
			if(retVal < BMD_OK)
			{
				bmd_datagram_free(&dtg);
				bmd_datagramset_free(&dtg_set);
				SOAP_FOK(retVal);
			}
		}

		retVal=bmd_datagram_set_filename(linksInfo->__ptr[iter].linkName, &dtg);
		if(retVal < BMD_OK)
		{
			bmd_datagram_free(&dtg);
			bmd_datagramset_free(&dtg_set);
			SOAP_FOK(retVal);
		}

		if(linksInfo->__ptr[iter].linkDescription != NULL)
		{
			retVal=set_gen_buf2( linksInfo->__ptr[iter].linkDescription, strlen(linksInfo->__ptr[iter].linkDescription), &(dtg->protocolData));
			if(retVal < BMD_OK)
			{
				bmd_datagram_free(&dtg);
				bmd_datagramset_free(&dtg_set);
				SOAP_FOK(retVal);
			}
		}
		else
		{
			retVal=set_gen_buf2( defaultDescription, strlen(defaultDescription), &(dtg->protocolData));
			if(retVal < BMD_OK)
			{
				bmd_datagram_free(&dtg);
				bmd_datagramset_free(&dtg_set);
				SOAP_FOK(retVal);
			}
		}
	
		if(linksInfo->__ptr[iter].granting == GRANTING_FORBIDDEN)
		{
			retVal=bmd_datagram_add_metadata_char(OID_SYS_METADATA_CRYPTO_OBJECTS_FOR_GRANT, "0", &dtg);
			if(retVal < BMD_OK)
			{
				bmd_datagram_free(&dtg);
				bmd_datagramset_free(&dtg_set);
				SOAP_FOK(retVal);
			}
		}
		else if(linksInfo->__ptr[iter].granting == GRANTING_ALLOWED)
		{
			retVal=bmd_datagram_add_metadata_char(OID_SYS_METADATA_CRYPTO_OBJECTS_FOR_GRANT, "1", &dtg);
			if(retVal < BMD_OK)
			{
				bmd_datagram_free(&dtg);
				bmd_datagramset_free(&dtg_set);
				SOAP_FOK(retVal);
			}
		}

		if(linksInfo->__ptr[iter].expirationDate != NULL)
		{
			retVal=bmdsoapGenerateDateTimeString(linksInfo->__ptr[iter].expirationDate, &expirationDateString);
			if(retVal < BMD_OK)
			{
				bmd_datagram_free(&dtg);
				bmd_datagramset_free(&dtg_set);
				SOAP_FOK(retVal);
			}

			retVal=bmd_datagram_add_metadata_char(OID_SYS_METADATA_CRYPTO_OBJECTS_EXPIRATION_DATE, expirationDateString, &dtg);
			free(expirationDateString); expirationDateString=NULL;
			if(retVal < BMD_OK)
			{
				bmd_datagram_free(&dtg);
				bmd_datagramset_free(&dtg_set);
				SOAP_FOK(retVal);
			}
		}


		retVal=bmd_datagram_add_metadata_char(OID_SYS_METADATA_CRYPTO_OBJECTS_POINTING_ID, linksInfo->__ptr[iter].pointingId, &dtg);
		if(retVal < BMD_OK)
		{
			bmd_datagram_free(&dtg);
			bmd_datagramset_free(&dtg_set);
			SOAP_FOK(retVal);
		}

		if(linksInfo->__ptr[iter].ownerIdentityId != NULL)
		{
			retVal=bmd_datagram_add_metadata_char(OID_SYS_METADATA_CERT_USER_IDENTITY, linksInfo->__ptr[iter].ownerIdentityId, &dtg);
			if(retVal < BMD_OK)
			{
				bmd_datagram_free(&dtg);
				bmd_datagramset_free(&dtg_set);
				SOAP_FOK(retVal);
			}
		}

		if(linksInfo->__ptr[iter].ownerCertificateId != NULL)
		{
			retVal=bmd_datagram_add_metadata_char(OID_SYS_METADATA_CERT_USER_ID, linksInfo->__ptr[iter].ownerCertificateId, &dtg);
			if(retVal < BMD_OK)
			{
				bmd_datagram_free(&dtg);
				bmd_datagramset_free(&dtg_set);
				SOAP_FOK(retVal);
			}
		}

		retVal=bmd_datagram_add_metadata_char(OID_SYS_METADATA_CERT_GROUP_ID, linksInfo->__ptr[iter].ownerGroupId, &dtg);
		if(retVal < BMD_OK)
		{
			bmd_datagram_free(&dtg);
			bmd_datagramset_free(&dtg_set);
			SOAP_FOK(retVal);
		}
	
		if(transactionId != NULL)
		{
			retVal=bmd_datagram_add_metadata_char(OID_SYS_METADATA_CRYPTO_OBJECTS_TRANSACTION_ID, transactionId, &dtg);
			if(retVal < BMD_OK)
			{
				bmd_datagram_free(&dtg);
				bmd_datagramset_free(&dtg_set);
				SOAP_FOK(retVal);
			}
		}

		retVal=bmd_datagram_add_to_set(dtg, &dtg_set);
		if(retVal < BMD_OK)
		{
			bmd_datagram_free(&dtg);
			bmd_datagramset_free(&dtg_set);
			SOAP_FOK(retVal);
		}

		dtg=NULL;
	}


	if(_GLOBAL_soap_wssecurity_enabled == 1)
	{
		retVal=GetWSSEInformation(soap, NULL, NULL, NULL, &derUserCert, &derUserCertLen);
		if(retVal < 0)
		{
			bmd_datagramset_free(&dtg_set);
			PRINT_ERROR("Unable to get certificate from WS Security header %li\n", retVal);
			SOAP_FOK(BMDSOAP_SERVER_NO_WSSECURITY_CERTIFICATE);
		}
	}
	else
	{
		if(_GLOBAL_soap_certificate_required == 1)
		{
			if(cert == NULL)
			{
				bmd_datagramset_free(&dtg_set);
				SOAP_FOK(BMDSOAP_SERVER_EMPTY_LOGIN_CERTIFICATE);
			}
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


	retVal=PrepareDataForBMDLoginAndRequest(&bi,
			(_GLOBAL_soap_wssecurity_enabled == 1  ? derUserCert : paramCert),
			(_GLOBAL_soap_wssecurity_enabled == 1  ? derUserCertLen : paramCertLen),
			userRoleId, userGroupId, deserialisation_max_memory);
	free(derUserCert); derUserCert=NULL;
	paramCert=NULL; paramCertLen=0;
	if(retVal < BMD_OK)
	{
		bmd_datagramset_free(&dtg_set);
		SOAP_FOK(retVal);
	}


	retVal=bmd_send_request(bi, dtg_set, max_datagram_in_set_transmission, &resp_dtg_set, 1, deserialisation_max_memory);
	if(retVal < BMD_OK)
	{
		bmd_datagramset_free(&dtg_set);
		SOAP_FOK_LOGOUT(retVal);
	}

	bmd_logout(&bi, deserialisation_max_memory);
	bmd_datagramset_free(&dtg_set);

	*ids=(struct bmd230__idList*)calloc(1, sizeof(struct bmd230__idList));
	(*ids)->__size=resp_dtg_set->bmdDatagramSetSize;
	(*ids)->__ptr=(int*)calloc((*ids)->__size, sizeof(int));
	
	for(iter=0; iter<resp_dtg_set->bmdDatagramSetSize; iter++)
	{
		if(resp_dtg_set->bmdDatagramSetTable[iter]->datagramStatus >= BMD_OK)
		{
			if(resp_dtg_set->bmdDatagramSetTable[iter]->protocolDataFileId == NULL)
				{ (*ids)->__ptr[iter]=-1; }
			else if(resp_dtg_set->bmdDatagramSetTable[iter]->protocolDataFileId->buf == NULL)
				{ (*ids)->__ptr[iter]=-1; }
			else
			{
				(*ids)->__ptr[iter]=strtol(resp_dtg_set->bmdDatagramSetTable[iter]->protocolDataFileId->buf, NULL, 10);
			}
		}
		else
		{
			(*ids)->__ptr[iter]=resp_dtg_set->bmdDatagramSetTable[iter]->datagramStatus;
		}
	}

	bmd_datagramset_free(&resp_dtg_set);
	bmd_end();

	PRINT_SOAP("Status:\t\trequest served correctly\n");
	return SOAP_OK;
}


int bmd230__bmdCreateCascadeLinks(	struct soap *soap,
			struct xsd__base64Binary *cert,
			char *roleName,
			char *groupName,
			char *userClassId,
			struct bmd230__singleLinkInfo* linkInfo,
			char *transactionId,
			long correspondingFilesMaxLevel,
			enum visibilityType_t visible,
			/*enum groupAuthorizationType_e groupAuthorization,*/
			struct bmd230__idList **createdIds)
{
long i						= 0;
long max_datagram_in_set_transmission		= 0;
long deserialisation_max_memory			= 0;
long max_metadata_value_length			= 0;
long start_transaction_required			= 0;
char *transaction_dir_name			= NULL;
char *tmp_dir					= NULL;
bmd_info_t *bi					= NULL;
bmdDatagram_t *dtg				= NULL;
bmdDatagramSet_t *dtg_set			= NULL;
bmdDatagramSet_t *resp_dtg_set			= NULL;
struct bmd230__idList *tempIds			= NULL;
char* correspondingFilesMaxLevelString		= NULL;
char* expirationDateString			= NULL;

char *derUserCert=NULL;
long derUserCertLen=0;
long retVal=0;
char* paramCert=NULL;
long paramCertLen=0;

	/************************/
	/* walidacja parametrow */
	/************************/
	
	
	PRINT_SOAP("Request:\tcreate link in archive\n");

	if(linkInfo == NULL)
		{ SOAP_FOK(BMD_ERR_PARAM5); }
	if(linkInfo->pointingId == NULL || strlen(linkInfo->pointingId) <= 0)
		{ SOAP_FOK(BMD_ERR_PARAM5); }
	if(linkInfo->ownerIdentityId == NULL && linkInfo->ownerCertificateId == NULL)
		{ SOAP_FOK(BMD_ERR_PARAM5); }
	if(linkInfo->ownerGroupId == NULL || strlen(linkInfo->ownerGroupId) <= 0 )
		{ SOAP_FOK(BMD_ERR_PARAM5); }

	asprintf(&correspondingFilesMaxLevelString, "%li", correspondingFilesMaxLevel);
	if(correspondingFilesMaxLevelString == NULL)
		{  SOAP_FOK(NO_MEMORY); }
	/***************************************/
	/* wczytanie ustawien konfiguracyjnych */
	/***************************************/
	if (bmdconf_get_value_int(_GLOBAL_konfiguracja,"soap","max_metadata_value_length",&max_metadata_value_length)<BMD_OK)
	{
		max_metadata_value_length=256;
	}
	bmdconf_get_value_int(_GLOBAL_konfiguracja,"bmd","max_datagram_in_set_transmission",
	&max_datagram_in_set_transmission);
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

	SOAP_FOK(bmdconf_get_value(_GLOBAL_konfiguracja,"soap","temp_dir",&tmp_dir));
	if(bmdconf_get_value_int(_GLOBAL_konfiguracja,"soap","start_transaction_required",&start_transaction_required) < BMD_OK)
	{
		start_transaction_required = 0;
	}

	/*Ustalam czy jest katalog transakcji*/
	if(transactionId != NULL)
	{/*if(transactionId != NULL)*/
		SOAP_FOK(twfun_check_transaction_directory( tmp_dir, transactionId, start_transaction_required,	&transaction_dir_name));
		SOAP_FOK(twfun_mark_start_of_insert(    transaction_dir_name, transactionId));
	}/*\if(transactionId != NULL)*/


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

	/************************************/
	/*	stworzenie datagramu zadania	*/
	/************************************/
	SOAP_FOK_LOGOUT_TRANSACTION(bmd_datagram_create(BMD_DATAGRAM_TYPE_CREATE_CASCADE_LINKS, &dtg));
	if (userClassId!=NULL)
	{
		retVal=bmd_datagram_add_metadata_char(OID_ACTION_METADATA_CERT_CHOSEN_CLASS_ID, userClassId, &dtg);
		if(retVal < BMD_OK)
		{
			bmd_datagram_free(&dtg);
			SOAP_FOK_LOGOUT_TRANSACTION(retVal);
		}
	}

	/*if(groupAuthorization > 0)
	{
		dtg->datagramStatus=BMD_LINK_CREATION_IGNORE_DEPENDENT_GROUPS;
	}*/

	if(linkInfo->granting == GRANTING_FORBIDDEN)
	{
		retVal=bmd_datagram_add_metadata_char(OID_SYS_METADATA_CRYPTO_OBJECTS_FOR_GRANT, "0", &dtg);
		if(retVal < BMD_OK)
		{
			bmd_datagram_free(&dtg);
			SOAP_FOK_LOGOUT_TRANSACTION(retVal);
		}
	}
	else if(linkInfo->granting == GRANTING_ALLOWED)
	{
		retVal=bmd_datagram_add_metadata_char(OID_SYS_METADATA_CRYPTO_OBJECTS_FOR_GRANT, "1", &dtg);
		if(retVal < BMD_OK)
		{
			bmd_datagram_free(&dtg);
			SOAP_FOK_LOGOUT_TRANSACTION(retVal);
		}
	}

	if(linkInfo->expirationDate != NULL)
	{
		retVal=bmdsoapGenerateDateTimeString(linkInfo->expirationDate, &expirationDateString);
		if(retVal < BMD_OK)
		{
			bmd_datagram_free(&dtg);
			SOAP_FOK_LOGOUT_TRANSACTION(retVal);
		}
		retVal=bmd_datagram_add_metadata_char(OID_SYS_METADATA_CRYPTO_OBJECTS_EXPIRATION_DATE, expirationDateString, &dtg);
		free(expirationDateString); expirationDateString=NULL;
		if(retVal < BMD_OK)
		{
			bmd_datagram_free(&dtg);
			SOAP_FOK_LOGOUT_TRANSACTION(retVal);
		}
	}

	//czy szukac dokumentow widocznych, niewidocznych, czy wszystkich
	
	if(visible == ALL)
	{
		//dla wszystkich metadana visible wcale nie jest ustawiana
	}
	else if(visible == VISIBLE)
	{
		retVal=bmd_datagram_add_metadata_char(OID_SYS_METADATA_CRYPTO_OBJECTS_VISIBLE, "1", &dtg);
		if(retVal < BMD_OK)
		{
			bmd_datagram_free(&dtg);
			SOAP_FOK_LOGOUT_TRANSACTION(retVal);
		}
	}
	else if(visible == INVISIBLE)
	{
		retVal=bmd_datagram_add_metadata_char(OID_SYS_METADATA_CRYPTO_OBJECTS_VISIBLE, "0", &dtg);
		if(retVal < BMD_OK)
		{
			bmd_datagram_free(&dtg);
			SOAP_FOK_LOGOUT_TRANSACTION(retVal);
		}
	}
	else
	{
		bmd_datagram_free(&dtg);
		SOAP_FOK_LOGOUT_TRANSACTION(BMD_ERR_PARAM8);
	}



	
	/* ustawienie pointing id */
	retVal=bmd_datagram_add_metadata_char(	OID_SYS_METADATA_CRYPTO_OBJECTS_POINTING_ID, linkInfo->pointingId, &dtg);
	if(retVal != BMD_OK)
	{
		bmd_datagram_free(&dtg);
		SOAP_FOK_LOGOUT_TRANSACTION(retVal);
	}

	/* ustawienie cert user id */
	if(linkInfo->ownerIdentityId != NULL)
	{
		retVal=bmd_datagram_add_metadata_char(	OID_SYS_METADATA_CERT_USER_IDENTITY, linkInfo->ownerIdentityId, &dtg);
		if(retVal < BMD_OK)
		{
			bmd_datagram_free(&dtg);
			SOAP_FOK_LOGOUT_TRANSACTION(retVal);
		}
	}
	else
	{
		retVal=bmd_datagram_add_metadata_char(	OID_SYS_METADATA_CERT_USER_ID, linkInfo->ownerCertificateId, &dtg);
		if(retVal < BMD_OK)
		{
			bmd_datagram_free(&dtg);
			SOAP_FOK_LOGOUT_TRANSACTION(retVal);
		}
	}
		
	/* ustawienie user group id */
	retVal=bmd_datagram_add_metadata_char(	OID_SYS_METADATA_CERT_GROUP_ID, linkInfo->ownerGroupId, &dtg);
	if(retVal < BMD_OK)
	{
		bmd_datagram_free(&dtg);
		SOAP_FOK_LOGOUT_TRANSACTION(retVal);
	}

	if (transactionId!=NULL)
	{
		retVal=bmd_datagram_add_metadata_char(	OID_SYS_METADATA_CRYPTO_OBJECTS_TRANSACTION_ID, transactionId, &dtg);
		if(retVal != BMD_OK)
		{
			bmd_datagram_free(&dtg);
			SOAP_FOK_LOGOUT_TRANSACTION(retVal);
		}
	}

	/*****************************/
	/* wstawienie wspolnego opisu linkow */
	/*****************************/
	if (linkInfo->linkDescription!=NULL)
	{
		retVal=set_gen_buf2( linkInfo->linkDescription, strlen(linkInfo->linkDescription), &(dtg->protocolData));
		if(retVal != BMD_OK)
		{
			bmd_datagram_free(&dtg);
			SOAP_FOK_LOGOUT_TRANSACTION(retVal);
		}
	}
	else
	{
		retVal=set_gen_buf2( "no description", strlen("no description"), &(dtg->protocolData));
		if(retVal != BMD_OK)
		{
			bmd_datagram_free(&dtg);
			SOAP_FOK_LOGOUT_TRANSACTION(retVal);
		}
	}

	retVal=bmd_datagram_add_metadata_char(OID_SYS_METADATA_CORESPONDING_FILE_LEVEL, correspondingFilesMaxLevelString, &dtg);
	if(retVal != BMD_OK)
	{
		bmd_datagram_free(&dtg);
		SOAP_FOK_LOGOUT_TRANSACTION(retVal);
	}
	free(correspondingFilesMaxLevelString); correspondingFilesMaxLevelString=NULL;


	retVal=bmd_datagram_add_to_set(dtg,&dtg_set);
	if(retVal != BMD_OK)
	{
		bmd_datagram_free(&dtg);
		SOAP_FOK_LOGOUT_TRANSACTION(retVal);
	}

	/************************/
	/*	wyslanie zadania	*/
	/************************/
	retVal=bmd_send_request(bi,dtg_set, max_datagram_in_set_transmission, &resp_dtg_set, 1, deserialisation_max_memory);
	bmd_datagramset_free(&dtg_set);
	if(retVal != BMD_OK)
		{ SOAP_FOK_LOGOUT_TRANSACTION(retVal); }

	/******************************/
	/*	odebranie odpowiedzi	*/
	/******************************/
	retVal=resp_dtg_set->bmdDatagramSetTable[0]->datagramStatus;
	if(retVal < BMD_OK)
	{
		bmd_datagramset_free(&resp_dtg_set);
		
		// jesli ze wzgedu na duplikaty lub for_grant=0 nie stworzono zadnego linka, nie moze zwracac bledu
		// zamiast tego pusta tablica wynikowa(0 elementow)
		if(retVal == LIBBMDSQL_NO_CASCADE_LINK_CREATED)
		{
			tempIds=(struct bmd230__idList *)soap_malloc(soap, sizeof(struct bmd230__idList));
			if(tempIds == NULL)
				{ SOAP_FOK_LOGOUT_TRANSACTION(retVal); }
			tempIds->__size=0;
			tempIds->__ptr=NULL;
			*createdIds=tempIds; tempIds=NULL;
			
			bmd_logout(&bi, deserialisation_max_memory);
			bmd_end();
			if(transactionId != NULL)
			{
				SOAP_FOK_LOGOUT(twfun_unmark_start_of_insert(	transaction_dir_name, transactionId, -1));
				free(transaction_dir_name); transaction_dir_name = NULL;
			}

			PRINT_SOAP("Status:\t\tNo cascade link was created\n");
			return SOAP_OK;
		}
		else
		{
			SOAP_FOK_LOGOUT_TRANSACTION(retVal);
		}
	}

	tempIds=(struct bmd230__idList *)soap_malloc(soap, sizeof(struct bmd230__idList));
	tempIds->__size=(int)resp_dtg_set->bmdDatagramSetSize;
	tempIds->__ptr=(int*)soap_malloc(soap, tempIds->__size * sizeof(int));
	
	for(i=0; i<resp_dtg_set->bmdDatagramSetSize; i++)
	{
		if (	resp_dtg_set->bmdDatagramSetTable[i]->protocolDataFileId != NULL &&
			resp_dtg_set->bmdDatagramSetTable[i]->protocolDataFileId->buf != NULL)
		{
			tempIds->__ptr[i]=atoi((char *)resp_dtg_set->bmdDatagramSetTable[i]->protocolDataFileId->buf);
		}
	}

	bmd_datagramset_free(&resp_dtg_set);

	if(transactionId != NULL)
	{
		SOAP_FOK_LOGOUT(twfun_unmark_start_of_insert(	transaction_dir_name, transactionId, -1));
		free(transaction_dir_name); transaction_dir_name = NULL;
	}

	*createdIds=tempIds;
	tempIds=NULL;

	bmd_logout(&bi, deserialisation_max_memory);
	bmd_end();

	PRINT_SOAP("Status:\t\trequest served correctly\n");
	return SOAP_OK;
}

int bmd230__bmdCreateManyCascadeLinks(	struct soap *soap,
					struct xsd__base64Binary *cert,
					char *userRoleId, char *userGroupId, char *userClassId,
					struct bmd230__cascadeLinksInfo* cascadeLinksInfoList, char *transactionId,
					struct bmd230__manyCascadeLinksResult **result)
{
long iter					= 0;
long i						= 0;
long max_datagram_in_set_transmission		= 0;
long deserialisation_max_memory			= 0;
long max_metadata_value_length			= 0;
long start_transaction_required			= 0;
char *transaction_dir_name			= NULL;
char *tmp_dir					= NULL;
bmd_info_t *bi					= NULL;
bmdDatagram_t *dtg				= NULL;
bmdDatagramSet_t *dtg_set			= NULL;
bmdDatagramSet_t *resp_dtg_set			= NULL;
char* correspondingFilesMaxLevelString		= NULL;
char* expirationDateString			= NULL;
struct bmd230__singleCascadeLinkInfo* cascadeLinkInfoPtr	= NULL;
struct bmd230__manyCascadeLinksResult* tempResult		= NULL;

char *derUserCert=NULL;
long derUserCertLen=0;
long retVal=0;
char* paramCert=NULL;
long paramCertLen=0;

	PRINT_SOAP("Request:\tcreate many cascade links in archive\n");


	/***************************************/
	/* wczytanie ustawien konfiguracyjnych */
	/***************************************/
	if (bmdconf_get_value_int(_GLOBAL_konfiguracja,"soap","max_metadata_value_length",&max_metadata_value_length)<BMD_OK)
	{
		max_metadata_value_length=256;
	}
	bmdconf_get_value_int(_GLOBAL_konfiguracja,"bmd","max_datagram_in_set_transmission",
	&max_datagram_in_set_transmission);
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

	SOAP_FOK(bmdconf_get_value(_GLOBAL_konfiguracja,"soap","temp_dir",&tmp_dir));
	if(bmdconf_get_value_int(_GLOBAL_konfiguracja,"soap","start_transaction_required",&start_transaction_required) < BMD_OK)
	{
		start_transaction_required = 0;
	}

	/*Ustalam czy jest katalog transakcji*/
	if(transactionId != NULL)
	{/*if(transactionId != NULL)*/
		SOAP_FOK(twfun_check_transaction_directory( tmp_dir, transactionId, start_transaction_required,	&transaction_dir_name));
		SOAP_FOK(twfun_mark_start_of_insert(    transaction_dir_name, transactionId));
	}/*\if(transactionId != NULL)*/


	/************************/
	/* walidacja parametrow */
	/************************/
	if(cascadeLinksInfoList == NULL)
		{ SOAP_FOK(BMD_ERR_PARAM5); }
	if(cascadeLinksInfoList->__size <= 0 || cascadeLinksInfoList->__ptr == NULL)
		{ SOAP_FOK(BMD_ERR_PARAM5); }
	for(iter=0; iter < cascadeLinksInfoList->__size; iter++)
	{
		if(cascadeLinksInfoList->__ptr[iter].linkInfo->pointingId == NULL || strlen(cascadeLinksInfoList->__ptr[iter].linkInfo->pointingId) <= 0)
			{ SOAP_FOK(BMD_ERR_PARAM5); }
		if(cascadeLinksInfoList->__ptr[iter].linkInfo->ownerIdentityId == NULL && cascadeLinksInfoList->__ptr[iter].linkInfo->ownerCertificateId == NULL)
			{ SOAP_FOK(BMD_ERR_PARAM5); }
		if(cascadeLinksInfoList->__ptr[iter].linkInfo->ownerGroupId == NULL || strlen(cascadeLinksInfoList->__ptr[iter].linkInfo->ownerGroupId) <= 0 )
			{ SOAP_FOK(BMD_ERR_PARAM5); }
	}

	tempResult=(struct bmd230__manyCascadeLinksResult*)soap_malloc(soap, sizeof(struct bmd230__manyCascadeLinksResult));
	if(tempResult == NULL)
		{ SOAP_FOK(NO_MEMORY); }
	tempResult->__size=cascadeLinksInfoList->__size;
	tempResult->__ptr=(struct bmd230__manyCascadeLinksElement*)soap_malloc(soap, tempResult->__size*sizeof(struct bmd230__manyCascadeLinksElement));
	if(tempResult->__ptr == NULL)
	{
		tempResult->__size=0;
		SOAP_FOK(NO_MEMORY);
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
	BMD_FOK(retVal);

	// przygotowanie i wysylka kolejnych zadan
	for(iter=0; iter<cascadeLinksInfoList->__size; iter++)
	{
		dtg=NULL;
		dtg_set=NULL;
		cascadeLinkInfoPtr= (&(cascadeLinksInfoList->__ptr[iter]));
		
		retVal=bmd_datagram_create(BMD_DATAGRAM_TYPE_CREATE_CASCADE_LINKS, &dtg);
		if(retVal < BMD_OK)
		{
			tempResult->__ptr[iter].errorCode=retVal;
			tempResult->__ptr[iter].idsList=NULL;
			PRINT_ERROR("Element %li: Unable to create datagram\n", iter);
			continue;
		}
		if (userClassId!=NULL)
		{
			retVal=bmd_datagram_add_metadata_char(OID_ACTION_METADATA_CERT_CHOSEN_CLASS_ID, userClassId, &dtg);
			if(retVal < BMD_OK)
			{
				bmd_datagram_free(&dtg);
				tempResult->__ptr[iter].errorCode=retVal;
				tempResult->__ptr[iter].idsList=NULL;
				PRINT_ERROR("Element %li: Unable to add data to datagram\n", iter);
				continue;
			}
		}

		if(cascadeLinkInfoPtr->linkInfo->granting == GRANTING_FORBIDDEN)
		{
			retVal=bmd_datagram_add_metadata_char(OID_SYS_METADATA_CRYPTO_OBJECTS_FOR_GRANT, "0", &dtg);
			if(retVal < BMD_OK)
			{
				bmd_datagram_free(&dtg);
				tempResult->__ptr[iter].errorCode=retVal;
				tempResult->__ptr[iter].idsList=NULL;
				PRINT_ERROR("Element %li: Unable to add data to datagram\n", iter);
				continue;
			}
		}
		else if(cascadeLinkInfoPtr->linkInfo->granting == GRANTING_ALLOWED)
		{
			retVal=bmd_datagram_add_metadata_char(OID_SYS_METADATA_CRYPTO_OBJECTS_FOR_GRANT, "1", &dtg);
			if(retVal < BMD_OK)
			{
				bmd_datagram_free(&dtg);
				tempResult->__ptr[iter].errorCode=retVal;
				tempResult->__ptr[iter].idsList=NULL;
				PRINT_ERROR("Element %li: Unable to add data to datagram\n", iter);
				continue;
			}
		}

		asprintf(&correspondingFilesMaxLevelString, "%li", cascadeLinkInfoPtr->correspondingFilesMaxLevel);
		if(correspondingFilesMaxLevelString == NULL)
		{
				bmd_datagram_free(&dtg);
				tempResult->__ptr[iter].errorCode=NO_MEMORY;
				tempResult->__ptr[iter].idsList=NULL;
				PRINT_ERROR("Element %li: Unable to allocate memory\n", iter);
				continue;
		}

		retVal=bmd_datagram_add_metadata_char(OID_SYS_METADATA_CORESPONDING_FILE_LEVEL, correspondingFilesMaxLevelString, &dtg);
		free(correspondingFilesMaxLevelString); correspondingFilesMaxLevelString=NULL;
		if(retVal < BMD_OK)
		{
			bmd_datagram_free(&dtg);
			tempResult->__ptr[iter].errorCode=retVal;
			tempResult->__ptr[iter].idsList=NULL;
			PRINT_ERROR("Element %li: Unable to add data to datagram\n", iter);
			continue;
		}
		
		if(cascadeLinkInfoPtr->linkInfo->expirationDate != NULL)
		{
			retVal=bmdsoapGenerateDateTimeString(cascadeLinkInfoPtr->linkInfo->expirationDate, &expirationDateString);
			if(retVal < BMD_OK)
			{
				bmd_datagram_free(&dtg);
				tempResult->__ptr[iter].errorCode=retVal;
				tempResult->__ptr[iter].idsList=NULL;
				PRINT_ERROR("Element %li: Unable to process expiration date\n", iter);
				continue;
			}
			retVal=bmd_datagram_add_metadata_char(OID_SYS_METADATA_CRYPTO_OBJECTS_EXPIRATION_DATE, expirationDateString, &dtg);
			free(expirationDateString); expirationDateString=NULL;
			if(retVal < BMD_OK)
			{
				bmd_datagram_free(&dtg);
				tempResult->__ptr[iter].errorCode=retVal;
				tempResult->__ptr[iter].idsList=NULL;
				PRINT_ERROR("Element %li: Unable to add data to datagram\n", iter);
				continue;
			}
		}

		//czy szukac dokumentow widocznych, niewidocznych, czy wszystkich
		if(cascadeLinkInfoPtr->visible == ALL)
		{
			//dla wszystkich metadana visible wcale nie jest ustawiana
		}
		else if(cascadeLinkInfoPtr->visible == VISIBLE)
		{
			retVal=bmd_datagram_add_metadata_char(OID_SYS_METADATA_CRYPTO_OBJECTS_VISIBLE, "1", &dtg);
			if(retVal < BMD_OK)
			{
				bmd_datagram_free(&dtg);
				tempResult->__ptr[iter].errorCode=retVal;
				tempResult->__ptr[iter].idsList=NULL;
				PRINT_ERROR("Element %li: Unable to add data to datagram\n", iter);
				continue;
			}
		}
		else if(cascadeLinkInfoPtr->visible == INVISIBLE)
		{
			retVal=bmd_datagram_add_metadata_char(OID_SYS_METADATA_CRYPTO_OBJECTS_VISIBLE, "0", &dtg);
			if(retVal < BMD_OK)
			{
				bmd_datagram_free(&dtg);
				tempResult->__ptr[iter].errorCode=retVal;
				tempResult->__ptr[iter].idsList=NULL;
				PRINT_ERROR("Element %li: Unable to add data to datagram\n", iter);
				continue;
			}
		}
		else
		{
			bmd_datagram_free(&dtg);
			tempResult->__ptr[iter].errorCode=BMD_ERR_OP_FAILED;
			tempResult->__ptr[iter].idsList=NULL;
			PRINT_ERROR("Element %li: Improper visible value\n", iter);
			continue;
		}

		/* ustawienie pointing id */
		retVal=bmd_datagram_add_metadata_char(	OID_SYS_METADATA_CRYPTO_OBJECTS_POINTING_ID, cascadeLinkInfoPtr->linkInfo->pointingId, &dtg);
		if(retVal < BMD_OK)
		{
			bmd_datagram_free(&dtg);
			tempResult->__ptr[iter].errorCode=retVal;
			tempResult->__ptr[iter].idsList=NULL;
			PRINT_ERROR("Element %li: Unable to add data to datagram\n", iter);
			continue;
		}

		/* ustawienie cert user id */
		if(cascadeLinkInfoPtr->linkInfo->ownerIdentityId != NULL)
		{
			retVal=bmd_datagram_add_metadata_char(	OID_SYS_METADATA_CERT_USER_IDENTITY, cascadeLinkInfoPtr->linkInfo->ownerIdentityId, &dtg);
			if(retVal < BMD_OK)
			{
				bmd_datagram_free(&dtg);
				tempResult->__ptr[iter].errorCode=retVal;
				tempResult->__ptr[iter].idsList=NULL;
				PRINT_ERROR("Element %li: Unable to add data to datagram\n", iter);
				continue;
			}
		}
		else
		{
			retVal=bmd_datagram_add_metadata_char(	OID_SYS_METADATA_CERT_USER_ID, cascadeLinkInfoPtr->linkInfo->ownerCertificateId, &dtg);
			if(retVal < BMD_OK)
			{
				bmd_datagram_free(&dtg);
				tempResult->__ptr[iter].errorCode=retVal;
				tempResult->__ptr[iter].idsList=NULL;
				PRINT_ERROR("Element %li: Unable to add data to datagram\n", iter);
				continue;
			}
		}

		/* ustawienie user group id */
		retVal=bmd_datagram_add_metadata_char(	OID_SYS_METADATA_CERT_GROUP_ID, cascadeLinkInfoPtr->linkInfo->ownerGroupId, &dtg);
		if(retVal < BMD_OK)
		{
			bmd_datagram_free(&dtg);
			tempResult->__ptr[iter].errorCode=retVal;
			tempResult->__ptr[iter].idsList=NULL;
			PRINT_ERROR("Element %li: Unable to add data to datagram\n", iter);
			continue;
		}
	
		if (transactionId!=NULL)
		{
			retVal=bmd_datagram_add_metadata_char(	OID_SYS_METADATA_CRYPTO_OBJECTS_TRANSACTION_ID, transactionId, &dtg);
			if(retVal < BMD_OK)
			{
				bmd_datagram_free(&dtg);
				tempResult->__ptr[iter].errorCode=retVal;
				tempResult->__ptr[iter].idsList=NULL;
				PRINT_ERROR("Element %li: Unable to add data to datagram\n", iter);
				continue;
			}
		}

		/*****************************/
		/* wstawienie wspolnego opisu linkow */
		/*****************************/
		if (cascadeLinkInfoPtr->linkInfo->linkDescription!=NULL)
		{
			retVal=set_gen_buf2(cascadeLinkInfoPtr->linkInfo->linkDescription, strlen(cascadeLinkInfoPtr->linkInfo->linkDescription), &(dtg->protocolData));
			if(retVal < BMD_OK)
			{
				bmd_datagram_free(&dtg);
				tempResult->__ptr[iter].errorCode=retVal;
				tempResult->__ptr[iter].idsList=NULL;
				PRINT_ERROR("Element %li: Unable to copy buffer\n", iter);
				continue;
			}
		}
		else
		{
			retVal=set_gen_buf2( "no description", strlen("no description"), &(dtg->protocolData));
			if(retVal < BMD_OK)
			{
				bmd_datagram_free(&dtg);
				tempResult->__ptr[iter].errorCode=retVal;
				tempResult->__ptr[iter].idsList=NULL;
				PRINT_ERROR("Element %li: Unable to copy buffer\n", iter);
				continue;
			}
		}

		retVal=bmd_datagram_add_to_set(dtg,&dtg_set);
		dtg=NULL;
		if(retVal < BMD_OK)
		{
			bmd_datagram_free(&dtg);
			tempResult->__ptr[iter].errorCode=retVal;
			tempResult->__ptr[iter].idsList=NULL;
			PRINT_ERROR("Element %li: Unable to add datagram\n", iter);
			continue;
		}

		/************************/
		/*	wyslanie zadania	*/
		/************************/
		resp_dtg_set=NULL;
		retVal=bmd_send_request(bi,dtg_set, max_datagram_in_set_transmission, &resp_dtg_set, 1, deserialisation_max_memory);
		bmd_datagramset_free(&dtg_set);
		if(retVal < BMD_OK)
		{
			tempResult->__ptr[iter].errorCode=retVal;
			tempResult->__ptr[iter].idsList=NULL;
			PRINT_ERROR("Element %li: Unable to send request\n", iter);
			continue;
		}
	
		/******************************/
		/*	odebranie odpowiedzi	*/
		/******************************/
		retVal=resp_dtg_set->bmdDatagramSetTable[0]->datagramStatus;
		if(retVal < BMD_OK)
		{
			bmd_datagramset_free(&resp_dtg_set);
			tempResult->__ptr[iter].errorCode=retVal;
			tempResult->__ptr[iter].idsList=NULL;
			PRINT_ERROR("Element %li: Response error %li\n", iter, retVal);
			continue;
		}

		tempResult->__ptr[iter].idsList=(struct bmd230__idList*)soap_malloc(soap, sizeof(struct bmd230__idList));
		if(tempResult->__ptr[iter].idsList == NULL)
		{
			bmd_datagramset_free(&resp_dtg_set);
			tempResult->__ptr[iter].errorCode=NO_MEMORY;
			tempResult->__ptr[iter].idsList=NULL;
			PRINT_ERROR("Element %li: Unable to allocate memory\n", iter);
			continue;
		}
		tempResult->__ptr[iter].idsList->__size=(int)resp_dtg_set->bmdDatagramSetSize;
		tempResult->__ptr[iter].idsList->__ptr=(int*)soap_malloc(soap, tempResult->__ptr[iter].idsList->__size * sizeof(int));
		if(tempResult->__ptr[iter].idsList->__ptr == NULL)
		{
			tempResult->__ptr[iter].idsList->__size=0;
			bmd_datagramset_free(&resp_dtg_set);
			tempResult->__ptr[iter].errorCode=NO_MEMORY;
			tempResult->__ptr[iter].idsList=NULL;
			PRINT_ERROR("Element %li: Unable to allocate memory\n", iter);
			continue;
		}
		memset(tempResult->__ptr[iter].idsList->__ptr, 0, (tempResult->__ptr[iter].idsList->__size) * sizeof(int));
		tempResult->__ptr[iter].errorCode=BMD_OK;
		
		for(i=0; i<resp_dtg_set->bmdDatagramSetSize; i++)
		{
			if (	resp_dtg_set->bmdDatagramSetTable[i]->protocolDataFileId != NULL &&
				resp_dtg_set->bmdDatagramSetTable[i]->protocolDataFileId->buf != NULL)
			{
				tempResult->__ptr[iter].idsList->__ptr[i]=atoi((char *)resp_dtg_set->bmdDatagramSetTable[i]->protocolDataFileId->buf);
			}
		}
	
		bmd_datagramset_free(&resp_dtg_set);
	}


	if(transactionId != NULL)
	{
		SOAP_FOK_LOGOUT(twfun_unmark_start_of_insert(	transaction_dir_name, transactionId, -1));
		free(transaction_dir_name); transaction_dir_name = NULL;
	}

	*result=tempResult;
	tempResult=NULL;

	bmd_logout(&bi, deserialisation_max_memory);
	bmd_end();

	PRINT_SOAP("Status:\t\trequest served correctly\n");
	return SOAP_OK;
}


int bmd230__bmdDeleteCascadeLinks(struct soap *soap,
			struct xsd__base64Binary *cert,
			char* userRoleId,
			char* userGroupId,
			char* userClassId,
			struct bmd230__deleteCascadeLinksList* filesList,
			struct bmd230__manyCascadeLinksResult** result)
{
long iter					= 0;
long jupiter					= 0;
char* correspondingFilesMaxLevelString		= NULL;
struct bmd230__manyCascadeLinksResult* tempResult	= NULL;
long deletedLinksCount				= 0;

long max_datagram_in_set_transmission		= 0;
long deserialisation_max_memory			= 0;
long max_metadata_value_length			= 0;

bmd_info_t *bi					= NULL;
bmdDatagram_t *dtg				= NULL;
bmdDatagramSet_t *dtg_set			= NULL;
bmdDatagramSet_t *resp_dtg_set			= NULL;

char *derUserCert=NULL;
long derUserCertLen=0;
long retVal=0;
char* paramCert=NULL;
long paramCertLen=0;


	PRINT_SOAP("Request:\tdelete cascade links in archive\n");

	if(filesList == NULL)
		{ SOAP_FOK(BMD_ERR_PARAM5); }
	if(filesList->__size <= 0)
		{ SOAP_FOK(BMD_ERR_PARAM5); }
	if(result == NULL)
		{ SOAP_FOK(BMD_ERR_PARAM6); }


	/***************************************/
	/* wczytanie ustawien konfiguracyjnych */
	/***************************************/
	if(bmdconf_get_value_int(_GLOBAL_konfiguracja,"soap","max_metadata_value_length",&max_metadata_value_length)<BMD_OK)
	{
		max_metadata_value_length=256;
	}
	bmdconf_get_value_int(_GLOBAL_konfiguracja,"bmd","max_datagram_in_set_transmission",
	&max_datagram_in_set_transmission);
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

	//sprawdzenie, czy podano porzebne dane wejsciowe
	for(iter=0; iter<filesList->__size; iter++)
	{
		if(filesList->__ptr[iter].fileId == NULL)
			{ SOAP_FOK(BMDSOAP_FILE_ID_NOT_SPECIFIED); }
		if(strlen(filesList->__ptr[iter].fileId) <= 0)
			{ SOAP_FOK(BMDSOAP_FILE_ID_NOT_SPECIFIED); }
		if(filesList->__ptr[iter].ownerIdentityId == NULL && filesList->__ptr[iter].ownerCertificateId == NULL)
			{ SOAP_FOK(BMDSOAP_OWNER_NOT_SPECIFIED); }
		if(filesList->__ptr[iter].ownerIdentityId != NULL)
		{
			if(strlen(filesList->__ptr[iter].ownerIdentityId) <= 0)
				{ SOAP_FOK(BMDSOAP_OWNER_NOT_SPECIFIED); }
		}
		else
		{
			if(strlen(filesList->__ptr[iter].ownerCertificateId) <= 0)
				{ SOAP_FOK(BMDSOAP_OWNER_NOT_SPECIFIED); }
		}
	}

	for(iter=0; iter<filesList->__size; iter++)
	{
		retVal=bmd_datagram_create(BMD_DATAGRAM_TYPE_DELETE_CASCADE_LINKS, &dtg);
		if(retVal < BMD_OK)
		{
			bmd_datagram_free(&dtg);
			bmd_datagramset_free(&dtg_set);
			SOAP_FOK(retVal);
		}
		
		if(userClassId!=NULL)
		{
			retVal=bmd_datagram_add_metadata_char(OID_ACTION_METADATA_CERT_CHOSEN_CLASS_ID, userClassId, &dtg);
			if(retVal < BMD_OK)
			{
				bmd_datagram_free(&dtg);
				bmd_datagramset_free(&dtg_set);
				SOAP_FOK(retVal);
			}
		}

		retVal=set_gen_buf2(filesList->__ptr[iter].fileId, strlen(filesList->__ptr[iter].fileId), &(dtg->protocolDataFileId));
		if(retVal < BMD_OK)
		{
			bmd_datagram_free(&dtg);
			bmd_datagramset_free(&dtg_set);
			SOAP_FOK(retVal);
		}

		asprintf(&correspondingFilesMaxLevelString, "%li", filesList->__ptr[iter].correspondingFilesMaxLevel);
		if(correspondingFilesMaxLevelString==NULL)
		{
			bmd_datagram_free(&dtg);
			bmd_datagramset_free(&dtg_set);
			SOAP_FOK(NO_MEMORY);
		}

		retVal=bmd_datagram_add_metadata_char(OID_SYS_METADATA_CORESPONDING_FILE_LEVEL, correspondingFilesMaxLevelString, &dtg);
		free(correspondingFilesMaxLevelString); correspondingFilesMaxLevelString=NULL;
		if(retVal < BMD_OK)
		{
			bmd_datagram_free(&dtg);
			bmd_datagramset_free(&dtg_set);
			SOAP_FOK(retVal);
		}

		if(filesList->__ptr[iter].ownerIdentityId != NULL)
		{
			retVal=bmd_datagram_add_metadata_char(OID_SYS_METADATA_CERT_USER_IDENTITY, filesList->__ptr[iter].ownerIdentityId, &dtg);
			if(retVal < BMD_OK)
			{
				bmd_datagram_free(&dtg);
				bmd_datagramset_free(&dtg_set);
				SOAP_FOK(retVal);
			}
		}
		else
		{
			retVal=bmd_datagram_add_metadata_char(OID_SYS_METADATA_CERT_USER_ID, filesList->__ptr[iter].ownerCertificateId, &dtg);
			if(retVal < BMD_OK)
			{
				bmd_datagram_free(&dtg);
				bmd_datagramset_free(&dtg_set);
				SOAP_FOK(retVal);
			}
		}

		if(filesList->__ptr[iter].visible == ALL)
		{
			//dla wszystkich metadana visible wcale nie jest ustawiana
		}
		else if(filesList->__ptr[iter].visible == VISIBLE)
		{
			retVal=bmd_datagram_add_metadata_char(OID_SYS_METADATA_CRYPTO_OBJECTS_VISIBLE, "1", &dtg);
			if(retVal < BMD_OK)
			{
				bmd_datagram_free(&dtg);
				bmd_datagramset_free(&dtg_set);
				SOAP_FOK(retVal);
			}
		}
		else if(filesList->__ptr[iter].visible == INVISIBLE)
		{
			retVal=bmd_datagram_add_metadata_char(OID_SYS_METADATA_CRYPTO_OBJECTS_VISIBLE, "0", &dtg);
			if(retVal < BMD_OK)
			{
				bmd_datagram_free(&dtg);
				bmd_datagramset_free(&dtg_set);
				SOAP_FOK(retVal);
			}
		}
		else
		{
			bmd_datagram_free(&dtg);
			bmd_datagramset_free(&dtg_set);
			SOAP_FOK(BMDSOAP_INCORRECT_VISIBILITY_VALUE);
		}

		retVal=bmd_datagram_add_to_set(dtg, &dtg_set);
		if(retVal < BMD_OK)
		{
			bmd_datagram_free(&dtg);
			bmd_datagramset_free(&dtg_set);
			SOAP_FOK(retVal);
		}

		dtg=NULL;
	}


	if(_GLOBAL_soap_wssecurity_enabled == 1)
	{
		retVal=GetWSSEInformation(soap, NULL, NULL, NULL, &derUserCert, &derUserCertLen);
		if(retVal < 0)
		{
			bmd_datagramset_free(&dtg_set);
			PRINT_ERROR("Unable to get certificate from WS Security header %li\n", retVal);
			SOAP_FOK(BMDSOAP_SERVER_NO_WSSECURITY_CERTIFICATE);
		}
	}
	else
	{
		if(_GLOBAL_soap_certificate_required == 1)
		{
			if(cert == NULL)
			{
				bmd_datagramset_free(&dtg_set);
				SOAP_FOK(BMDSOAP_SERVER_EMPTY_LOGIN_CERTIFICATE);
			}
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


	retVal=PrepareDataForBMDLoginAndRequest(&bi,
			(_GLOBAL_soap_wssecurity_enabled == 1  ? derUserCert : paramCert),
			(_GLOBAL_soap_wssecurity_enabled == 1  ? derUserCertLen : paramCertLen),
			userRoleId, userGroupId, deserialisation_max_memory);
	free(derUserCert); derUserCert=NULL;
	paramCert=NULL; paramCertLen=0;
	if(retVal < BMD_OK)
	{
		bmd_datagramset_free(&dtg_set);
		SOAP_FOK(retVal);
	}


	retVal=bmd_send_request(bi, dtg_set, max_datagram_in_set_transmission, &resp_dtg_set, 1, deserialisation_max_memory);
	bmd_logout(&bi, deserialisation_max_memory);
	bmd_datagramset_free(&dtg_set);
	SOAP_FOK(retVal);

	//wypelnianie listy wynikow
	tempResult=(struct bmd230__manyCascadeLinksResult*)soap_malloc(soap, sizeof(struct bmd230__manyCascadeLinksResult));
	if(tempResult == NULL)
	{
		bmd_datagramset_free(&resp_dtg_set);
		SOAP_FOK(NO_MEMORY);
	}
	tempResult->__size=resp_dtg_set->bmdDatagramSetSize;
	tempResult->__ptr=(struct bmd230__manyCascadeLinksElement*)soap_malloc(soap, (tempResult->__size)*sizeof(struct bmd230__manyCascadeLinksElement));
	if(tempResult->__ptr == NULL)
	{
		tempResult->__size=0;
		bmd_datagramset_free(&resp_dtg_set);
		SOAP_FOK(NO_MEMORY);
	}
	memset(tempResult->__ptr, 0 , (tempResult->__size)*sizeof(struct bmd230__manyCascadeLinksElement));
	
	for(iter=0; iter<resp_dtg_set->bmdDatagramSetSize; iter++)
	{
		if(resp_dtg_set->bmdDatagramSetTable[iter]->datagramStatus >= BMD_OK)
		{
			deletedLinksCount=0;
			//liczenie, ile linkow zostalo usunietych dla tego pliku i powiazanego drzewa plikow
			for(jupiter=0; jupiter<resp_dtg_set->bmdDatagramSetTable[iter]->no_of_sysMetaData; jupiter++)
			{
				if(resp_dtg_set->bmdDatagramSetTable[iter]->sysMetaData[jupiter] != NULL && resp_dtg_set->bmdDatagramSetTable[iter]->sysMetaData[jupiter]->OIDTableBuf != NULL)
				{
					if(strcmp(resp_dtg_set->bmdDatagramSetTable[iter]->sysMetaData[jupiter]->OIDTableBuf, OID_SYS_METADATA_CRYPTO_OBJECTS_ID) == 0)
						{ deletedLinksCount++; }
				}
			}

			tempResult->__ptr[iter].errorCode=BMD_OK;
			tempResult->__ptr[iter].idsList=(struct bmd230__idList*)soap_malloc(soap, sizeof(struct bmd230__idList));
			if(tempResult->__ptr[iter].idsList == NULL)
			{
				bmd_datagramset_free(&resp_dtg_set);
				SOAP_FOK(NO_MEMORY);
			}
			tempResult->__ptr[iter].idsList->__size=deletedLinksCount;
			tempResult->__ptr[iter].idsList->__ptr=(int*)soap_malloc(soap, (tempResult->__ptr[iter].idsList->__size)*sizeof(int));
			if(tempResult->__ptr[iter].idsList->__ptr == NULL && tempResult->__ptr[iter].idsList->__size > 0)
			{
				tempResult->__ptr[iter].idsList->__size=0;
				bmd_datagramset_free(&resp_dtg_set);
				SOAP_FOK(NO_MEMORY);
			}
			memset(tempResult->__ptr[iter].idsList->__ptr, 0, (tempResult->__ptr[iter].idsList->__size)*sizeof(int));

			deletedLinksCount=0;
			for(jupiter=0; jupiter<resp_dtg_set->bmdDatagramSetTable[iter]->no_of_sysMetaData; jupiter++)
			{
				if(resp_dtg_set->bmdDatagramSetTable[iter]->sysMetaData[jupiter] != NULL &&
				resp_dtg_set->bmdDatagramSetTable[iter]->sysMetaData[jupiter]->OIDTableBuf != NULL)
				{
					if(strcmp(resp_dtg_set->bmdDatagramSetTable[iter]->sysMetaData[jupiter]->OIDTableBuf, OID_SYS_METADATA_CRYPTO_OBJECTS_ID) == 0)
					{
						if(resp_dtg_set->bmdDatagramSetTable[iter]->sysMetaData[jupiter]->AnyBuf != NULL)
						{
							tempResult->__ptr[iter].idsList->__ptr[deletedLinksCount]=strtol(resp_dtg_set->bmdDatagramSetTable[iter]->sysMetaData[jupiter]->AnyBuf, NULL, 10);
							deletedLinksCount++;
						}
					}
				}
			}
		}
		else
		{
			tempResult->__ptr[iter].errorCode=resp_dtg_set->bmdDatagramSetTable[iter]->datagramStatus;
			tempResult->__ptr[iter].idsList=NULL;
		}
	}

	bmd_datagramset_free(&resp_dtg_set);
	*result=tempResult;
	tempResult=NULL;

	bmd_end();

	PRINT_SOAP("Status:\t\trequest served correctly\n");
	return SOAP_OK;
}
