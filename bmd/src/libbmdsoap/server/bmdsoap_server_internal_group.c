#include <bmd/libbmdsoap/libbmdsoap.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>
#include <bmd/libbmdglobals/libbmdglobals.h>

int bmd230__bmdGetGroupsList(	struct soap *soap,
				struct xsd__base64Binary *cert,
				char *roleName,
				char *groupName,
				char *userClassId,
				char *groupId,
				struct bmd230__groupInfo **result)
{
long i					= 0;
long j					= 0;
long max_metadata_value_length		= 0;
long deserialisation_max_memory		= 0;
long max_datagram_in_set_transmission	= 0;
bmd_info_t *bi				= NULL;
bmdDatagram_t *dtg			= NULL;
bmdDatagramSet_t *dtg_set		= NULL;
bmdDatagramSet_t *resp_dtg_set		= NULL;

char *derUserCert=NULL;
long derUserCertLen=0;
long retVal=0;
char* paramCert=NULL;
long paramCertLen=0;

	PRINT_SOAP("Request:\tget groups list\n");
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
	SOAP_FOK_LOGOUT(bmd_datagram_create(BMD_DATAGRAM_TYPE_GET_GROUP_LIST, &dtg));
	if (userClassId!=NULL)
	{
		SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(	OID_ACTION_METADATA_CERT_CHOSEN_CLASS_ID, userClassId, &dtg));
	}

	/***********************************************************/
	/* dodanie identyfikatora grupy, ktorej info chcemy pobrac */
	/***********************************************************/
	if (groupId!=NULL)
	{
		BMD_FOK(set_gen_buf2(groupId, strlen(groupId), &(dtg->protocolDataFileId)));
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
	(*result)=(struct bmd230__groupInfo *)malloc(sizeof(struct bmd230__groupInfo));
	(*result)->__size=resp_dtg_set->bmdDatagramSetSize;

	(*result)->__ptr=(struct bmd230__singleGroupInfo *)malloc(sizeof(struct bmd230__singleGroupInfo)*((*result)->__size));
	memset((*result)->__ptr, 0, sizeof(struct bmd230__singleGroupInfo)*((*result)->__size));

	for (i=0; i<(*result)->__size; i++)
	{
		(*result)->__ptr[i].groupParentNames=(struct bmd230__stringList *)malloc(sizeof(struct bmd230__stringList));
		memset((*result)->__ptr[i].groupParentNames, 0, sizeof(struct bmd230__stringList));

		(*result)->__ptr[i].groupParentIds=(struct bmd230__stringList *)malloc(sizeof(struct bmd230__stringList));
		memset((*result)->__ptr[i].groupParentIds, 0, sizeof(struct bmd230__stringList));

		(*result)->__ptr[i].groupChildNames=(struct bmd230__stringList *)malloc(sizeof(struct bmd230__stringList));
		memset((*result)->__ptr[i].groupChildNames, 0, sizeof(struct bmd230__stringList));

		(*result)->__ptr[i].groupChildIds=(struct bmd230__stringList *)malloc(sizeof(struct bmd230__stringList));
		memset((*result)->__ptr[i].groupChildIds, 0, sizeof(struct bmd230__stringList));

		asprintf(&((*result)->__ptr[i].groupId), "%s", resp_dtg_set->bmdDatagramSetTable[i]->protocolDataFileId->buf);
		asprintf(&((*result)->__ptr[i].groupName), "%s", resp_dtg_set->bmdDatagramSetTable[i]->protocolDataFilename->buf);

		for (j=0; j<resp_dtg_set->bmdDatagramSetTable[i]->no_of_sysMetaData; j++)
		{

			if (strcmp(resp_dtg_set->bmdDatagramSetTable[i]->sysMetaData[j]->OIDTableBuf, OID_SYS_METADATA_CERT_PARENT_GROUP_NAME)==0)
			{
				(*result)->__ptr[i].groupParentNames->__size++;
				(*result)->__ptr[i].groupParentNames->__ptr=(xsd__string *)realloc((*result)->__ptr[i].groupParentNames->__ptr, sizeof(xsd__string)*(*result)->__ptr[i].groupParentNames->__size);
				asprintf(&((*result)->__ptr[i].groupParentNames->__ptr[(*result)->__ptr[i].groupParentNames->__size-1]), "%s", resp_dtg_set->bmdDatagramSetTable[i]->sysMetaData[j]->AnyBuf);
			}
			else if (strcmp(resp_dtg_set->bmdDatagramSetTable[i]->sysMetaData[j]->OIDTableBuf, OID_SYS_METADATA_CERT_PARENT_GROUP_ID)==0)
			{
				(*result)->__ptr[i].groupParentIds->__size++;
				(*result)->__ptr[i].groupParentIds->__ptr=(xsd__string *)realloc((*result)->__ptr[i].groupParentIds->__ptr, sizeof(xsd__string)*(*result)->__ptr[i].groupParentIds->__size);
				asprintf(&((*result)->__ptr[i].groupParentIds->__ptr[(*result)->__ptr[i].groupParentIds->__size-1]), "%s", resp_dtg_set->bmdDatagramSetTable[i]->sysMetaData[j]->AnyBuf);
			}
			else if (strcmp(resp_dtg_set->bmdDatagramSetTable[i]->sysMetaData[j]->OIDTableBuf, OID_SYS_METADATA_CERT_CHILD_GROUP_NAME)==0)
			{
				(*result)->__ptr[i].groupChildNames->__size++;
				(*result)->__ptr[i].groupChildNames->__ptr=(xsd__string *)realloc((*result)->__ptr[i].groupChildNames->__ptr, sizeof(xsd__string)*(*result)->__ptr[i].groupChildNames->__size);
				asprintf(&((*result)->__ptr[i].groupChildNames->__ptr[(*result)->__ptr[i].groupChildNames->__size-1]), "%s", resp_dtg_set->bmdDatagramSetTable[i]->sysMetaData[j]->AnyBuf);
			}
			else if (strcmp(resp_dtg_set->bmdDatagramSetTable[i]->sysMetaData[j]->OIDTableBuf, OID_SYS_METADATA_CERT_CHILD_GROUP_ID)==0)
			{
				(*result)->__ptr[i].groupChildIds->__size++;
				(*result)->__ptr[i].groupChildIds->__ptr=(xsd__string *)realloc((*result)->__ptr[i].groupChildIds->__ptr, sizeof(xsd__string)*(*result)->__ptr[i].groupChildIds->__size);
				asprintf(&((*result)->__ptr[i].groupChildIds->__ptr[(*result)->__ptr[i].groupChildIds->__size-1]), "%s", resp_dtg_set->bmdDatagramSetTable[i]->sysMetaData[j]->AnyBuf);
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


int bmd230__bmdRegisterNewGroup(	struct soap *soap,
				struct xsd__base64Binary *cert,
				char *roleName,
				char *groupName,
				char *userClassId,
				struct bmd230__singleGroupInfo *groupInfo,
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

char *derUserCert=NULL;
long derUserCertLen=0;
long retVal=0;
char* paramCert=NULL;
long paramCertLen=0;

	PRINT_SOAP("Request:\tregister new group\n");

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
	SOAP_FOK_LOGOUT(bmd_datagram_create(BMD_DATAGRAM_TYPE_REGISTER_GROUP, &dtg));
	if (userClassId!=NULL)
	{
		SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(	OID_ACTION_METADATA_CERT_CHOSEN_CLASS_ID, userClassId, &dtg));
	}

	/*************************************/
	/* ustawienie kryteriow wyszukiwania */
	/*************************************/
	if (groupInfo!=NULL)
	{
		/**************************************/
		/* wstawienie nazwy roli do datagramu */
		/**************************************/
		if (groupInfo->groupName)
		{
			if (strlen(groupInfo->groupName)>max_metadata_value_length)	{	SOAP_FOK_LOGOUT(BMDSOAP_SERVER_METADATA_VALUE_TOO_LONG);	}
			SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(OID_SYS_METADATA_CERT_GROUP_NAME, groupInfo->groupName, &dtg));
		}
		else
		{
			SOAP_FOK_LOGOUT(BMDSOAP_SERVER_GROUP_NAME_NOT_DEFINED);
		}

		/********************************************/
		/* wstawienie listy akcji roli do datagramu */
		/********************************************/
		if ((groupInfo->groupParentIds) && (groupInfo->groupParentIds->__size))
		{
			for (i=0; i<groupInfo->groupParentIds->__size; i++)
			{
				if (strlen(groupInfo->groupParentIds->__ptr[i])>max_metadata_value_length)	{	SOAP_FOK_LOGOUT(BMDSOAP_SERVER_METADATA_VALUE_TOO_LONG);	}
				SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(OID_SYS_METADATA_CERT_PARENT_GROUP_ID, groupInfo->groupParentIds->__ptr[i], &dtg));
			}
		}

		/**************************************************/
		/* wstawienie listy group_rights roli do datagramu */
		/**************************************************/
		if ((groupInfo->groupChildIds) && (groupInfo->groupChildIds->__size))
		{
			for (i=0; i<groupInfo->groupChildIds->__size; i++)
			{
				if (strlen(groupInfo->groupChildIds->__ptr[i])>max_metadata_value_length)	{	SOAP_FOK_LOGOUT(BMDSOAP_SERVER_METADATA_VALUE_TOO_LONG);	}
				SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(OID_SYS_METADATA_CERT_CHILD_GROUP_ID, groupInfo->groupChildIds->__ptr[i], &dtg));
			}
		}
	}
	else
	{
		SOAP_FOK_LOGOUT(BMDSOAP_SERVER_GROUP_INFO_NOT_DEFINED);
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

int bmd230__bmdUpdateGroup(	struct soap *soap,
			struct xsd__base64Binary *cert,
			char *roleName,
			char *groupName,
			char *userClassId,
			char *groupId,
			struct bmd230__singleGroupInfo *groupInfo,
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

char *derUserCert=NULL;
long derUserCertLen=0;
long retVal=0;
char* paramCert=NULL;
long paramCertLen=0;

	PRINT_SOAP("Request:\tupdate group\n");

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
	SOAP_FOK_LOGOUT(bmd_datagram_create(BMD_DATAGRAM_TYPE_UPDATE_GROUP, &dtg));
	if (userClassId!=NULL)
	{
		SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(	OID_ACTION_METADATA_CERT_CHOSEN_CLASS_ID, userClassId, &dtg));
	}

	/*************************************/
	/* ustawienie kryteriow wyszukiwania */
	/*************************************/
	if (groupId!=NULL)
	{
		BMD_FOK(set_gen_buf2(groupId, strlen(groupId), &(dtg->protocolDataFileId)));
	}
	else
	{
		SOAP_FOK_LOGOUT(BMDSOAP_SERVER_GROUP_ID_NOT_DEFINED);
	}

	if (groupInfo!=NULL)
	{
		/***************************************/
		/* wstawienie nazwy grupy do datagramu */
		/***************************************/
		if (groupInfo->groupName)
		{
			if (strlen(groupInfo->groupName)>max_metadata_value_length)	{	SOAP_FOK_LOGOUT(BMDSOAP_SERVER_METADATA_VALUE_TOO_LONG);	}
			SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(	OID_SYS_METADATA_CERT_GROUP_NAME, groupInfo->groupName, &dtg));
		}

		/********************************************/
		/* wstawienie listy grup rodzicow do datagramu */
		/********************************************/
		if(groupInfo->groupParentIds != NULL)
		{
			if(groupInfo->groupParentIds->__size > 0)
			{
				for (i=0; i<groupInfo->groupParentIds->__size; i++)
				{
					if (strlen(groupInfo->groupParentIds->__ptr[i])>max_metadata_value_length)	{	SOAP_FOK_LOGOUT(BMDSOAP_SERVER_METADATA_VALUE_TOO_LONG);	}
					SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(	OID_SYS_METADATA_CERT_PARENT_GROUP_ID, groupInfo->groupParentIds->__ptr[i], &dtg));
				}
			}
			else
			{
				SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(	OID_SYS_METADATA_CERT_EMPTY_PARENT_GROUP_LIST, "", &dtg));
			}
		}
		/**************************************************/
		/* wstawienie listy grup dzieci do datagramu */
		/**************************************************/
		if(groupInfo->groupChildIds != NULL)
		{
			if(groupInfo->groupChildIds->__size > 0)
			{
				for (i=0; i<groupInfo->groupChildIds->__size; i++)
				{
					if (strlen(groupInfo->groupChildIds->__ptr[i])>max_metadata_value_length)	{	SOAP_FOK_LOGOUT(BMDSOAP_SERVER_METADATA_VALUE_TOO_LONG);	}
					SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(	OID_SYS_METADATA_CERT_CHILD_GROUP_ID, groupInfo->groupChildIds->__ptr[i], &dtg));
				}
			}
			else
			{
				SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(	OID_SYS_METADATA_CERT_EMPTY_CHILD_GROUP_LIST, "", &dtg));
			}
		}
	}
	else
	{
		SOAP_FOK_LOGOUT(BMDSOAP_SERVER_GROUP_INFO_NOT_DEFINED);
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
	(*result)=BMD_OK;

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

int bmd230__bmdDeleteGroup(	struct soap *soap,
			struct xsd__base64Binary *cert,
			char *roleName,
			char *groupName,
			char *userClassId,
			char *groupId,
			int *result)
{
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

	PRINT_SOAP("Request:\tdelete group\n");

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
	SOAP_FOK_LOGOUT(bmd_datagram_create(BMD_DATAGRAM_TYPE_DELETE_GROUP, &dtg));
	if (userClassId!=NULL)
	{
		SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(	OID_ACTION_METADATA_CERT_CHOSEN_CLASS_ID, userClassId, &dtg));
	}

	/*************************************/
	/* ustawienie kryteriow wyszukiwania */
	/*************************************/
	if (groupId!=NULL)
	{
		/************************************/
		/* wstawienie id grupy do datagramu */
		/************************************/
		BMD_FOK(set_gen_buf2(groupId, strlen(groupId), &(dtg->protocolDataFileId)));
	}
	else
	{
		SOAP_FOK_LOGOUT(BMDSOAP_SERVER_GROUP_INFO_NOT_DEFINED);
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
	(*result)=BMD_OK;

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
