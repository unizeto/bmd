#include <bmd/libbmdsoap/libbmdsoap.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>
#include <bmd/libbmdglobals/libbmdglobals.h>

int bmd230__bmdGetRolesList(	struct soap *soap,
				struct xsd__base64Binary *cert,
				char *roleName,
				char *groupName,
				char *userClassId,
				char *roleId,
				struct bmd230__userRoleInfo **result)
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

	PRINT_SOAP("Request:\tget users list\n");
	/************************/
	/* walidacja parametrow */
	/************************/

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
	SOAP_FOK_LOGOUT(bmd_datagram_create(BMD_DATAGRAM_TYPE_GET_ROLE_LIST, &dtg));
	if (userClassId!=NULL)
	{
		SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(	OID_ACTION_METADATA_CERT_CHOSEN_CLASS_ID, userClassId, &dtg));
	}

	if (roleId!=NULL)
	{
		BMD_FOK(set_gen_buf2(roleId, strlen(roleId), &(dtg->protocolDataFileId)));
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
	(*result)=(struct bmd230__userRoleInfo *)malloc(sizeof(struct bmd230__userRoleInfo));
	(*result)->__size=resp_dtg_set->bmdDatagramSetSize;

	(*result)->__ptr=(struct bmd230__singleRoleInfo *)malloc(sizeof(struct bmd230__singleRoleInfo)*((*result)->__size));
	memset((*result)->__ptr, 0, sizeof(struct bmd230__singleRoleInfo)*((*result)->__size));

	for (i=0; i<(*result)->__size; i++)
	{
		(*result)->__ptr[i].roleActionList=(struct bmd230__stringList *)malloc(sizeof(struct bmd230__stringList));
		memset((*result)->__ptr[i].roleActionList, 0, sizeof(struct bmd230__stringList));

		(*result)->__ptr[i].roleRightsOids=(struct bmd230__stringList *)malloc(sizeof(struct bmd230__stringList));
		memset((*result)->__ptr[i].roleRightsOids, 0, sizeof(struct bmd230__stringList));

		(*result)->__ptr[i].roleRightsCodes=(struct bmd230__stringList *)malloc(sizeof(struct bmd230__stringList));
		memset((*result)->__ptr[i].roleRightsCodes, 0, sizeof(struct bmd230__stringList));

		asprintf(&((*result)->__ptr[i].roleId), "%s", resp_dtg_set->bmdDatagramSetTable[i]->protocolDataFileId->buf);
		asprintf(&((*result)->__ptr[i].roleName), "%s", resp_dtg_set->bmdDatagramSetTable[i]->protocolDataFilename->buf);

		for (j=0; j<resp_dtg_set->bmdDatagramSetTable[i]->no_of_sysMetaData; j++)
		{
			if (strcmp(resp_dtg_set->bmdDatagramSetTable[i]->sysMetaData[j]->OIDTableBuf, OID_SYS_METADATA_ACTION)==0)
			{
				(*result)->__ptr[i].roleActionList->__size++;
				(*result)->__ptr[i].roleActionList->__ptr=(xsd__string *)realloc((*result)->__ptr[i].roleActionList->__ptr, sizeof(xsd__string)*(*result)->__ptr[i].roleActionList->__size);
				asprintf(&((*result)->__ptr[i].roleActionList->__ptr[(*result)->__ptr[i].roleActionList->__size-1]), "%s", resp_dtg_set->bmdDatagramSetTable[i]->sysMetaData[j]->AnyBuf);
			}
			else if (strncmp(resp_dtg_set->bmdDatagramSetTable[i]->sysMetaData[j]->OIDTableBuf, "rr_",3)==0)
			{
				(*result)->__ptr[i].roleRightsOids->__size++;
				(*result)->__ptr[i].roleRightsOids->__ptr=(xsd__string *)realloc((*result)->__ptr[i].roleRightsOids->__ptr, sizeof(xsd__string)*(*result)->__ptr[i].roleRightsOids->__size);
				asprintf(&((*result)->__ptr[i].roleRightsOids->__ptr[(*result)->__ptr[i].roleRightsOids->__size-1]), "%s", resp_dtg_set->bmdDatagramSetTable[i]->sysMetaData[j]->OIDTableBuf);

				(*result)->__ptr[i].roleRightsCodes->__size++;
				(*result)->__ptr[i].roleRightsCodes->__ptr=(xsd__string *)realloc((*result)->__ptr[i].roleRightsCodes->__ptr, sizeof(xsd__string)*(*result)->__ptr[i].roleRightsCodes->__size);
				asprintf(&((*result)->__ptr[i].roleRightsCodes->__ptr[(*result)->__ptr[i].roleRightsCodes->__size-1]), "%s", resp_dtg_set->bmdDatagramSetTable[i]->sysMetaData[j]->AnyBuf);
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


/********************************************************************************************************************/
int bmd230__bmdRegisterNewRole(	struct soap *soap,
				struct xsd__base64Binary *cert,
				char *roleName,
				char *groupName,
				char *userClassId,
				struct bmd230__singleRoleInfo *roleInfo,
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

	PRINT_SOAP("Request:\tregister new role\n");
	/************************/
	/* walidacja parametrow */
	/************************/
	if (roleInfo==NULL)		{	SOAP_FOK(BMDSOAP_SERVER_ROLE_INFO_NOT_DEFINED);	}

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
	SOAP_FOK_LOGOUT(bmd_datagram_create(BMD_DATAGRAM_TYPE_REGISTER_ROLE, &dtg));
	if (userClassId!=NULL)
	{
		SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(	OID_ACTION_METADATA_CERT_CHOSEN_CLASS_ID, userClassId, &dtg));
	}

	/*************************************/
	/* ustawienie kryteriow wyszukiwania */
	/*************************************/
	if (roleInfo!=NULL)
	{
		/**************************************/
		/* wstawienie nazwy roli do datagramu */
		/**************************************/
		if (roleInfo->roleName)
		{
			if (strlen(roleInfo->roleName)>max_metadata_value_length)	{	SOAP_FOK_LOGOUT(BMDSOAP_SERVER_METADATA_VALUE_TOO_LONG);	}
			SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(OID_SYS_METADATA_CERT_ROLE_NAME, roleInfo->roleName, &dtg));
		}
		else
		{
			SOAP_FOK_LOGOUT(BMDSOAP_SERVER_ROLE_NAME_NOT_DEFINED);
		}

		/********************************************/
		/* wstawienie listy akcji roli do datagramu */
		/********************************************/
		if ((roleInfo->roleActionList) && (roleInfo->roleActionList->__size))
		{
			for (i=0; i<roleInfo->roleActionList->__size; i++)
			{
				if (strlen(roleInfo->roleActionList->__ptr[i])>max_metadata_value_length)	{	SOAP_FOK_LOGOUT(BMDSOAP_SERVER_METADATA_VALUE_TOO_LONG);	}
				SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(OID_SYS_METADATA_ACTION, roleInfo->roleActionList->__ptr[i], &dtg));
			}
		}
		else
		{
			SOAP_FOK_LOGOUT(BMDSOAP_SERVER_ROLE_ACTIONS_NOT_DEFINED);
		}

		/**************************************************/
		/* wstawienie listy role_rights roli do datagramu */
		/**************************************************/
		if ((roleInfo->roleRightsOids) && (roleInfo->roleRightsCodes) && (roleInfo->roleRightsOids->__size))
		{
			if (roleInfo->roleRightsOids->__size!=roleInfo->roleRightsCodes->__size)
			{
				SOAP_FOK_LOGOUT(BMDSOAP_SERVER_ROLE_ROLE_RIGHTS_NOT_DEFINED);
			}

			for (i=0; i<roleInfo->roleRightsOids->__size; i++)
			{
				if (strlen(roleInfo->roleRightsCodes->__ptr[i])>max_metadata_value_length)	{	SOAP_FOK_LOGOUT(BMDSOAP_SERVER_METADATA_VALUE_TOO_LONG);	}
				SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char_type(roleInfo->roleRightsOids->__ptr[i], SYS_METADATA, roleInfo->roleRightsCodes->__ptr[i], &dtg));
			}
		}
		else
		{
			SOAP_FOK_LOGOUT(BMDSOAP_SERVER_ROLE_ROLE_RIGHTS_NOT_DEFINED);
		}
	}
	else
	{
		SOAP_FOK_LOGOUT(BMDSOAP_SERVER_ROLE_INFO_NOT_DEFINED);
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


int bmd230__bmdUpdateRole(	struct soap *soap,
			struct xsd__base64Binary *cert,
			char *roleName,
			char *groupName,
			char *userClassId,
			char *roleId,
			struct bmd230__singleRoleInfo *roleInfo,
			int *result)
{
long i						= 0;
long max_metadata_value_length			= 0;
long max_datagram_in_set_transmission		= 0;
long deserialisation_max_memory			= 0;
bmd_info_t *bi					= NULL;
bmdDatagram_t *dtg				= NULL;
bmdDatagramSet_t *dtg_set			= NULL;
bmdDatagramSet_t *resp_dtg_set			= NULL;

char *derUserCert=NULL;
long derUserCertLen=0;
long retVal=0;
char* paramCert=NULL;
long paramCertLen=0;

	PRINT_SOAP("Request:\tupdate role\n");
	/************************/
	/* walidacja parametrow */
	/************************/
	if (roleInfo==NULL)		{	SOAP_FOK(BMDSOAP_SERVER_ROLE_INFO_NOT_DEFINED);	}

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
	SOAP_FOK_LOGOUT(bmd_datagram_create(BMD_DATAGRAM_TYPE_UPDATE_ROLE, &dtg));
	if (userClassId!=NULL)
	{
		SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(	OID_ACTION_METADATA_CERT_CHOSEN_CLASS_ID, userClassId, &dtg));
	}

	if (roleId!=NULL)
	{
		BMD_FOK(set_gen_buf2(roleId, strlen(roleId), &(dtg->protocolDataFileId)));
	}
	else
	{
		SOAP_FOK_LOGOUT(BMDSOAP_SERVER_ROLE_ID_NOT_DEFINED);
	}


	/*************************************/
	/* ustawienie kryteriow wyszukiwania */
	/*************************************/
	if (roleInfo!=NULL)
	{
		/**************************************/
		/* wstawienie nazwy roli do datagramu */
		/**************************************/
		if (roleInfo->roleName)
		{
			if (strlen(roleInfo->roleName)>max_metadata_value_length)	{	SOAP_FOK_LOGOUT(BMDSOAP_SERVER_METADATA_VALUE_TOO_LONG);	}
			SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(OID_SYS_METADATA_CERT_ROLE_NAME, roleInfo->roleName, &dtg));
		}

		/********************************************/
		/* wstawienie listy akcji roli do datagramu */
		/********************************************/
		if ((roleInfo->roleActionList) && (roleInfo->roleActionList->__size))
		{
			for (i=0; i<roleInfo->roleActionList->__size; i++)
			{
				if (strlen(roleInfo->roleActionList->__ptr[i])>max_metadata_value_length)	{	SOAP_FOK_LOGOUT(BMDSOAP_SERVER_METADATA_VALUE_TOO_LONG);	}
				SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(OID_SYS_METADATA_ACTION, roleInfo->roleActionList->__ptr[i], &dtg));
			}
		}

		/**************************************************/
		/* wstawienie listy role_rights roli do datagramu */
		/**************************************************/
		if ((roleInfo->roleRightsOids) && (roleInfo->roleRightsCodes) && (roleInfo->roleRightsOids->__size))
		{
			if (roleInfo->roleRightsOids->__size!=roleInfo->roleRightsCodes->__size)
			{
				SOAP_FOK_LOGOUT(BMDSOAP_SERVER_ROLE_ROLE_RIGHTS_NOT_DEFINED);
			}

			for (i=0; i<roleInfo->roleRightsOids->__size; i++)
			{
				if (strlen(roleInfo->roleRightsCodes->__ptr[i])>max_metadata_value_length)	{	SOAP_FOK_LOGOUT(BMDSOAP_SERVER_METADATA_VALUE_TOO_LONG);	}
				SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(roleInfo->roleRightsOids->__ptr[i], roleInfo->roleRightsCodes->__ptr[i], &dtg));
			}
		}
	}
	else
	{
		SOAP_FOK_LOGOUT(BMDSOAP_SERVER_ROLE_INFO_NOT_DEFINED);
	}

	/********************************/
	/* dodanie datagramu do zestawu */
	/********************************/
	SOAP_FOK_LOGOUT(bmd_datagram_add_to_set(dtg,&dtg_set));

	/******************************************/
	/*	wyslanie zadania do serwera bmd	*/
	/******************************************/
	SOAP_FOK_LOGOUT(bmd_send_request(bi, dtg_set, max_datagram_in_set_transmission, &resp_dtg_set,1, deserialisation_max_memory));
	SOAP_FOK_LOGOUT(resp_dtg_set->bmdDatagramSetTable[0]->datagramStatus);

	/****************************/
	/* przygotowanie odpowiedzi */
	/****************************/
	(*result)=0;

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

int bmd230__bmdDeleteRole(	struct soap *soap,
			struct xsd__base64Binary *cert,
			char *roleName,
			char *groupName,
			char *userClassId,
			char *roleId,
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

	PRINT_SOAP("Request:\tdelete role\n");
	/************************/
	/* walidacja parametrow */
	/************************/

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
	SOAP_FOK_LOGOUT(bmd_datagram_create(BMD_DATAGRAM_TYPE_DELETE_ROLE, &dtg));
	if (userClassId!=NULL)
	{
		SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(	OID_ACTION_METADATA_CERT_CHOSEN_CLASS_ID, userClassId, &dtg));
	}

	if (roleId!=NULL)
	{
		BMD_FOK(set_gen_buf2(roleId, strlen(roleId), &(dtg->protocolDataFileId)));
	}
	else
	{
		SOAP_FOK_LOGOUT(BMDSOAP_SERVER_ROLE_ID_NOT_DEFINED);
	}


	/********************************/
	/* dodanie datagramu do zestawu */
	/********************************/
	SOAP_FOK_LOGOUT(bmd_datagram_add_to_set(dtg,&dtg_set));

	/******************************************/
	/*	wyslanie zadania do serwera bmd	*/
	/******************************************/
	SOAP_FOK_LOGOUT(bmd_send_request(bi, dtg_set, max_datagram_in_set_transmission,&resp_dtg_set,1, deserialisation_max_memory));
	SOAP_FOK_LOGOUT(resp_dtg_set->bmdDatagramSetTable[0]->datagramStatus);

	/****************************/
	/* przygotowanie odpowiedzi */
	/****************************/
	(*result)=0;

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
