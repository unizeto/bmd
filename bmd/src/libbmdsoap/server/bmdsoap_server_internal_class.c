#include <bmd/libbmdsoap/libbmdsoap.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>
#include <bmd/libbmdglobals/libbmdglobals.h>

int bmd230__bmdGetClassList(	struct soap *soap,
				struct xsd__base64Binary *cert,
				char *roleName,
				char *groupName,
				char *userClassId,
				char *classId,
				struct bmd230__classInfo **result)
{
long i					= 0;
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

	PRINT_SOAP("Request:\tget class list\n");
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
	SOAP_FOK_LOGOUT(bmd_datagram_create(BMD_DATAGRAM_TYPE_GET_CLASS_LIST, &dtg));
	if (userClassId!=NULL)
	{
		SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(	OID_ACTION_METADATA_CERT_CHOSEN_CLASS_ID, userClassId, &dtg));
	}

	/***********************************************************/
	/* dodanie identyfikatora klasy, ktorej info chcemy pobrac */
	/***********************************************************/
	if (classId!=NULL)
	{
		BMD_FOK(set_gen_buf2(classId, strlen(classId), &(dtg->protocolDataFileId)));
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
	(*result)=(struct bmd230__classInfo *)malloc(sizeof(struct bmd230__classInfo));
	(*result)->__size=resp_dtg_set->bmdDatagramSetSize;

	(*result)->__ptr=(struct bmd230__singleClassInfo *)malloc(sizeof(struct bmd230__singleClassInfo)*((*result)->__size));
	memset((*result)->__ptr, 0, sizeof(struct bmd230__singleClassInfo)*((*result)->__size));

	for (i=0; i<(*result)->__size; i++)
	{
		asprintf(&((*result)->__ptr[i].classId), "%s", resp_dtg_set->bmdDatagramSetTable[i]->protocolDataFileId->buf);
		asprintf(&((*result)->__ptr[i].className), "%s", resp_dtg_set->bmdDatagramSetTable[i]->protocolDataFilename->buf);
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


int bmd230__bmdRegisterNewClass(	struct soap *soap,
				struct xsd__base64Binary *cert,
				char *roleName,
				char *groupName,
				char *userClassId,
				struct bmd230__singleClassInfo *classInfo,
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

	PRINT_SOAP("Request:\tregister new class\n");

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
	SOAP_FOK_LOGOUT(bmd_datagram_create(BMD_DATAGRAM_TYPE_REGISTER_CLASS, &dtg));
	if (userClassId!=NULL)
	{
		SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(	OID_ACTION_METADATA_CERT_CHOSEN_CLASS_ID, userClassId, &dtg));
	}

	/*************************************/
	/* ustawienie kryteriow wyszukiwania */
	/*************************************/
	if (classInfo!=NULL)
	{
		/**************************************/
		/* wstawienie nazwy roli do datagramu */
		/**************************************/
		if (classInfo->className)
		{
			if (strlen(classInfo->className)>max_metadata_value_length)	{	SOAP_FOK_LOGOUT(BMDSOAP_SERVER_METADATA_VALUE_TOO_LONG);	}
			SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(OID_ACTION_METADATA_CERT_CLASS_NAME, classInfo->className, &dtg));
		}
		else
		{
			SOAP_FOK_LOGOUT(BMDSOAP_SERVER_CLASS_NAME_NOT_DEFINED);
		}
	}
	else
	{
		SOAP_FOK_LOGOUT(BMDSOAP_SERVER_CLASS_INFO_NOT_DEFINED);
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

int bmd230__bmdUpdateClass(	struct soap *soap,
			struct xsd__base64Binary *cert,
			char *roleName,
			char *groupName,
			char *userClassId,
			char *classId,
			struct bmd230__singleClassInfo *classInfo,
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

	PRINT_SOAP("Request:\tupdate class\n");

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
	SOAP_FOK_LOGOUT(bmd_datagram_create(BMD_DATAGRAM_TYPE_UPDATE_CLASS, &dtg));
	if (userClassId!=NULL)
	{

		SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(	OID_ACTION_METADATA_CERT_CHOSEN_CLASS_ID, userClassId, &dtg));
	}

	/*************************************/
	/* ustawienie kryteriow wyszukiwania */
	/*************************************/
	if (classId!=NULL)
	{

		BMD_FOK(set_gen_buf2(classId, strlen(classId), &(dtg->protocolDataFileId)));
	}
	else
	{

		SOAP_FOK_LOGOUT(BMDSOAP_SERVER_CLASS_ID_NOT_DEFINED);
	}

	if (classInfo!=NULL)
	{

		/***************************************/
		/* wstawienie nazwy grupy do datagramu */
		/***************************************/
		if (classInfo->className)
		{

			if (strlen(classInfo->className)>max_metadata_value_length)	{	SOAP_FOK_LOGOUT(BMDSOAP_SERVER_METADATA_VALUE_TOO_LONG);	}
			SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(	OID_ACTION_METADATA_CERT_CLASS_NAME, classInfo->className, &dtg));
		}
	}
	else
	{

		SOAP_FOK_LOGOUT(BMDSOAP_SERVER_CLASS_INFO_NOT_DEFINED);
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

int bmd230__bmdDeleteClass(	struct soap *soap,
			struct xsd__base64Binary *cert,
			char *roleName,
			char *groupName,
			char *userClassId,
			char *classId,
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

	PRINT_SOAP("Request:\tdelete class\n");

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
	SOAP_FOK_LOGOUT(bmd_datagram_create(BMD_DATAGRAM_TYPE_DELETE_CLASS, &dtg));
	if (userClassId!=NULL)
	{
		SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(	OID_ACTION_METADATA_CERT_CHOSEN_CLASS_ID, userClassId, &dtg));
	}

	/*************************************/
	/* ustawienie kryteriow wyszukiwania */
	/*************************************/
	if (classId!=NULL)
	{
		/************************************/
		/* wstawienie id grupy do datagramu */
		/************************************/
		BMD_FOK(set_gen_buf2(classId, strlen(classId), &(dtg->protocolDataFileId)));
	}
	else
	{
		SOAP_FOK_LOGOUT(BMDSOAP_SERVER_CLASS_INFO_NOT_DEFINED);
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
