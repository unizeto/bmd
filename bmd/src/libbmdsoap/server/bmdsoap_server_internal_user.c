#include <bmd/libbmdsoap/libbmdsoap.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>
#include <bmd/libbmdglobals/libbmdglobals.h>


int bmd230__bmdDisableUser(	struct soap *soap,
			struct xsd__base64Binary *cert,
			char *roleName,
			char *groupName,
			char *userClassId,
			char *userId,
			int enable,
			int *result)
{
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

	PRINT_SOAP("Request:\tdisabling user\n");
	/************************/
	/* walidacja parametrow */
	/************************/
	if (userId==NULL)			{	SOAP_FOK(BMDSOAP_SERVER_USER_ID_UNDEFINED);	}

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
	SOAP_FOK_LOGOUT(bmd_datagram_create(BMD_DATAGRAM_TYPE_DISABLE_USER, &dtg));
	if (userClassId!=NULL)
	{
		SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(	OID_ACTION_METADATA_CERT_CHOSEN_CLASS_ID, userClassId, &dtg));
	}

	/**********************/
	/* dodanie metadanych */
	/**********************/
	SOAP_FOK_LOGOUT(set_gen_buf2(userId, strlen(userId), &(dtg->protocolDataFileId)));

	if (enable==1)
	{
		SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(OID_SYS_METADATA_CERT_USER_ACCEPTED, "1", &dtg));
	}
	else if (enable==0)
	{
		SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(OID_SYS_METADATA_CERT_USER_ACCEPTED, "0", &dtg));
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
 	bmd_datagramset_free(&resp_dtg_set);
  	bmd_datagramset_free(&dtg_set);

	bmd_logout(&bi, deserialisation_max_memory);
	bmd_end();


	return SOAP_OK;
}


int bmd230__bmdDisableIdentity(	struct soap *soap,
			struct xsd__base64Binary *cert,
			char *userRoleId,
			char *userGroupId,
			char *userClassId,
			char *identityId,
			int disable,
			int *result)
{
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

	PRINT_SOAP("Request:\tdisabling/enabling user\n");
	/************************/
	/* walidacja parametrow */
	/************************/
	if (identityId==NULL)			{	SOAP_FOK(BMDSOAP_SERVER_USER_IDENTITY_UNDEFINED);	}

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
			userRoleId, userGroupId, deserialisation_max_memory));
	free(derUserCert); derUserCert=NULL;
	paramCert=NULL; paramCertLen=0;

	/********************************/
	/*	stworzenie datagramu	*/
	/********************************/
	SOAP_FOK_LOGOUT(bmd_datagram_create(BMD_DATAGRAM_TYPE_DISABLE_IDENTITY, &dtg));
	if (userClassId!=NULL)
	{
		SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(	OID_ACTION_METADATA_CERT_CHOSEN_CLASS_ID, userClassId, &dtg));
	}

	/**********************/
	/* dodanie metadanych */
	/**********************/
	SOAP_FOK_LOGOUT(set_gen_buf2(identityId, strlen(identityId), &(dtg->protocolDataFileId)));

	if(disable==1)
	{
		SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(OID_SYS_METADATA_CERT_USER_ACCEPTED, "0", &dtg));
	}
	else
	{
		SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(OID_SYS_METADATA_CERT_USER_ACCEPTED, "1", &dtg));
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
 	bmd_datagramset_free(&resp_dtg_set);
  	bmd_datagramset_free(&dtg_set);

	bmd_logout(&bi, deserialisation_max_memory);
	bmd_end();


	return SOAP_OK;
}


int bmd230__bmdDeleteUser(	struct soap *soap,
			struct xsd__base64Binary *cert,
			char *roleName,
			char *groupName,
			char *userClassId,
			char *userId,
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

	PRINT_SOAP("Request:\tdeleting user\n");
	/************************/
	/* walidacja parametrow */
	/************************/
	if (userId==NULL)			{	SOAP_FOK(BMDSOAP_SERVER_USER_ID_UNDEFINED);	}

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
	SOAP_FOK_LOGOUT(bmd_datagram_create(BMD_DATAGRAM_TYPE_DELETE_USER, &dtg));
	if (userClassId!=NULL)
	{
		SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(	OID_ACTION_METADATA_CERT_CHOSEN_CLASS_ID, userClassId, &dtg));
	}

	/**********************/
	/* dodanie metadanych */
	/**********************/
	SOAP_FOK_LOGOUT(set_gen_buf2(userId, strlen(userId), &(dtg->protocolDataFileId)));

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
	bmd_datagramset_free(&resp_dtg_set);
	bmd_datagramset_free(&dtg_set);

	bmd_logout(&bi, deserialisation_max_memory);
	bmd_end();


	return SOAP_OK;

}


int bmd230__bmdGetUsersList(struct soap *soap,
			struct xsd__base64Binary *cert,
			char *userRoleId,
			char *userGroupId,
			char *userClassId,
			char *userIdentityId,
			struct bmd230__userListInfo **result)
{
long i						= 0;
long j						= 0;
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

	PRINT_SOAP("Request:\tget users list\n");
	/******************************/
	/*	walidacja parametrow	*/
	/******************************/

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
			userRoleId, userGroupId, deserialisation_max_memory));
	free(derUserCert); derUserCert=NULL;
	paramCert=NULL; paramCertLen=0;

	/********************************/
	/*	stworzenie datagramu	*/
	/********************************/
	SOAP_FOK_LOGOUT(bmd_datagram_create(BMD_DATAGRAM_TYPE_GET_USER_LIST, &dtg));
	if (userClassId!=NULL)
	{
		SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(	OID_ACTION_METADATA_CERT_CHOSEN_CLASS_ID, userClassId, &dtg));
	}

	/************************************************************/
	/* dodanie identyfikatora usera, ktorego info chcemy pobrac */
	/************************************************************/
	if (userIdentityId!=NULL)
	{
		BMD_FOK(set_gen_buf2(userIdentityId, strlen(userIdentityId), &(dtg->protocolDataFileId)));
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
	(*result)=(struct bmd230__userListInfo *)malloc(sizeof(struct bmd230__userListInfo));
	(*result)->__size=resp_dtg_set->bmdDatagramSetSize;

	(*result)->__ptr=(struct bmd230__singleUserInfo *)malloc(sizeof(struct bmd230__singleUserInfo)*((*result)->__size));
	memset((*result)->__ptr, 0, sizeof(struct bmd230__singleUserInfo)*((*result)->__size));

	for (i=0; i<(*result)->__size; i++)
	{
		/********/
		/* role */
		/********/
		(*result)->__ptr[i].userRoleNameList=(struct bmd230__stringList *)malloc(sizeof(struct bmd230__stringList));
		memset((*result)->__ptr[i].userRoleNameList, 0, sizeof(struct bmd230__stringList));

		(*result)->__ptr[i].userRoleIdList=(struct bmd230__stringList *)malloc(sizeof(struct bmd230__stringList));
		memset((*result)->__ptr[i].userRoleIdList, 0, sizeof(struct bmd230__stringList));

		/*********/
		/* grupy */
		/*********/
		(*result)->__ptr[i].userGroupNameList=(struct bmd230__stringList *)malloc(sizeof(struct bmd230__stringList));
		memset((*result)->__ptr[i].userGroupNameList, 0, sizeof(struct bmd230__stringList));

		(*result)->__ptr[i].userGroupIdList=(struct bmd230__stringList *)malloc(sizeof(struct bmd230__stringList));
		memset((*result)->__ptr[i].userGroupIdList, 0, sizeof(struct bmd230__stringList));

		/*********/
		/* klasy */
		/*********/
		(*result)->__ptr[i].userClassNameList=(struct bmd230__stringList *)malloc(sizeof(struct bmd230__stringList));
		memset((*result)->__ptr[i].userClassNameList, 0, sizeof(struct bmd230__stringList));
		(*result)->__ptr[i].userClassNameList->__size=0;

		(*result)->__ptr[i].userClassIdList=(struct bmd230__stringList *)malloc(sizeof(struct bmd230__stringList));
		memset((*result)->__ptr[i].userClassIdList, 0, sizeof(struct bmd230__stringList));
		(*result)->__ptr[i].userClassIdList->__size=0;

		/*************************************/
		/* categorie i poiomy bezpieczenstwa */
		/*************************************/
		(*result)->__ptr[i].userSecurityNameList=(struct bmd230__stringList *)malloc(sizeof(struct bmd230__stringList));
		memset((*result)->__ptr[i].userSecurityNameList, 0, sizeof(struct bmd230__stringList));

		for (j=0; j<resp_dtg_set->bmdDatagramSetTable[i]->no_of_actionMetaData; j++)
		{
			if (strcmp(resp_dtg_set->bmdDatagramSetTable[i]->actionMetaData[j]->OIDTableBuf, OID_ACTION_METADATA_CERT_CLASS_NAME)==0)
			{
				(*result)->__ptr[i].userClassNameList->__size++;
				(*result)->__ptr[i].userClassNameList->__ptr=(xsd__string *)realloc((*result)->__ptr[i].userClassNameList->__ptr, sizeof(xsd__string)*(*result)->__ptr[i].userClassNameList->__size);
				asprintf(&((*result)->__ptr[i].userClassNameList->__ptr[(*result)->__ptr[i].userClassNameList->__size-1]), "%s", resp_dtg_set->bmdDatagramSetTable[i]->actionMetaData[j]->AnyBuf);
			}
			else if (strcmp(resp_dtg_set->bmdDatagramSetTable[i]->actionMetaData[j]->OIDTableBuf, OID_ACTION_METADATA_CERT_CLASS_ID)==0)
			{
				(*result)->__ptr[i].userClassIdList->__size++;
				(*result)->__ptr[i].userClassIdList->__ptr=(xsd__string *)realloc((*result)->__ptr[i].userClassIdList->__ptr, sizeof(xsd__string)*(*result)->__ptr[i].userClassIdList->__size);
				asprintf(&((*result)->__ptr[i].userClassIdList->__ptr[(*result)->__ptr[i].userClassIdList->__size-1]), "%s", resp_dtg_set->bmdDatagramSetTable[i]->actionMetaData[j]->AnyBuf);
			}
			else if (strcmp(resp_dtg_set->bmdDatagramSetTable[i]->actionMetaData[j]->OIDTableBuf, OID_ACTION_METADATA_CERT_DEFAULT_CLASS_NAME)==0)
			{
				asprintf(&((*result)->__ptr[i].userDefaultClassName), "%s", resp_dtg_set->bmdDatagramSetTable[i]->actionMetaData[j]->AnyBuf);
			}
			else if (strcmp(resp_dtg_set->bmdDatagramSetTable[i]->actionMetaData[j]->OIDTableBuf, OID_ACTION_METADATA_CERT_DEFAULT_CLASS_ID)==0)
			{
				asprintf(&((*result)->__ptr[i].userDefaultClassId), "%s", resp_dtg_set->bmdDatagramSetTable[i]->actionMetaData[j]->AnyBuf);
			}
		}

		for (j=0; j<resp_dtg_set->bmdDatagramSetTable[i]->no_of_sysMetaData; j++)
		{
			if (strcmp(resp_dtg_set->bmdDatagramSetTable[i]->sysMetaData[j]->OIDTableBuf, OID_SYS_METADATA_CERT_USER_ID)==0)
			{
				asprintf(&((*result)->__ptr[i].userId), "%s", resp_dtg_set->bmdDatagramSetTable[i]->sysMetaData[j]->AnyBuf);
			}
			else if (strcmp(resp_dtg_set->bmdDatagramSetTable[i]->sysMetaData[j]->OIDTableBuf, OID_SYS_METADATA_CERT_USER_ISSUER)==0)
			{
				asprintf(&((*result)->__ptr[i].userCertIssuer), "%s", resp_dtg_set->bmdDatagramSetTable[i]->sysMetaData[j]->AnyBuf);
			}
			else if (strcmp(resp_dtg_set->bmdDatagramSetTable[i]->sysMetaData[j]->OIDTableBuf, OID_SYS_METADATA_CERT_USER_SN)==0)
			{
				asprintf(&((*result)->__ptr[i].userCertSerial), "%s", resp_dtg_set->bmdDatagramSetTable[i]->sysMetaData[j]->AnyBuf);
			}
			else if (strcmp(resp_dtg_set->bmdDatagramSetTable[i]->sysMetaData[j]->OIDTableBuf, OID_SYS_METADATA_CERT_USER_NAME)==0)
			{
				asprintf(&((*result)->__ptr[i].userName), "%s", resp_dtg_set->bmdDatagramSetTable[i]->sysMetaData[j]->AnyBuf);
			}
			else if (strcmp(resp_dtg_set->bmdDatagramSetTable[i]->sysMetaData[j]->OIDTableBuf, OID_SYS_METADATA_CERT_USER_IDENTITY)==0)
			{
				asprintf(&((*result)->__ptr[i].userIdentity), "%s", resp_dtg_set->bmdDatagramSetTable[i]->sysMetaData[j]->AnyBuf);
			}
			else if (strcmp(resp_dtg_set->bmdDatagramSetTable[i]->sysMetaData[j]->OIDTableBuf, OID_SYS_METADATA_CERT_USER_ACCEPTED)==0)
			{
				asprintf(&((*result)->__ptr[i].userAccepted), "%s", resp_dtg_set->bmdDatagramSetTable[i]->sysMetaData[j]->AnyBuf);
			}
			else if (strcmp(resp_dtg_set->bmdDatagramSetTable[i]->sysMetaData[j]->OIDTableBuf, OID_SYS_METADATA_CERT_GROUP_NAME)==0)
			{
				(*result)->__ptr[i].userGroupNameList->__size++;
				(*result)->__ptr[i].userGroupNameList->__ptr=(xsd__string *)realloc((*result)->__ptr[i].userGroupNameList->__ptr, sizeof(xsd__string)*(*result)->__ptr[i].userGroupNameList->__size);
				asprintf(&((*result)->__ptr[i].userGroupNameList->__ptr[(*result)->__ptr[i].userGroupNameList->__size-1]), "%s", resp_dtg_set->bmdDatagramSetTable[i]->sysMetaData[j]->AnyBuf);
			}
			else if (strcmp(resp_dtg_set->bmdDatagramSetTable[i]->sysMetaData[j]->OIDTableBuf, OID_SYS_METADATA_CERT_GROUP_ID)==0)
			{
				(*result)->__ptr[i].userGroupIdList->__size++;
				(*result)->__ptr[i].userGroupIdList->__ptr=(xsd__string *)realloc((*result)->__ptr[i].userGroupIdList->__ptr, sizeof(xsd__string)*(*result)->__ptr[i].userGroupIdList->__size);
				asprintf(&((*result)->__ptr[i].userGroupIdList->__ptr[(*result)->__ptr[i].userGroupIdList->__size-1]), "%s", resp_dtg_set->bmdDatagramSetTable[i]->sysMetaData[j]->AnyBuf);
			}
			else if (strcmp(resp_dtg_set->bmdDatagramSetTable[i]->sysMetaData[j]->OIDTableBuf, OID_SYS_METADATA_CERT_ROLE_NAME)==0)
			{
				(*result)->__ptr[i].userRoleNameList->__size++;
				(*result)->__ptr[i].userRoleNameList->__ptr=(xsd__string *)realloc((*result)->__ptr[i].userRoleNameList->__ptr, sizeof(xsd__string)*(*result)->__ptr[i].userRoleNameList->__size);
				asprintf(&((*result)->__ptr[i].userRoleNameList->__ptr[(*result)->__ptr[i].userRoleNameList->__size-1]), "%s", resp_dtg_set->bmdDatagramSetTable[i]->sysMetaData[j]->AnyBuf);
			}
			else if (strcmp(resp_dtg_set->bmdDatagramSetTable[i]->sysMetaData[j]->OIDTableBuf, OID_SYS_METADATA_CERT_ROLE_ID)==0)
			{
				(*result)->__ptr[i].userRoleIdList->__size++;
				(*result)->__ptr[i].userRoleIdList->__ptr=(xsd__string *)realloc((*result)->__ptr[i].userRoleIdList->__ptr, sizeof(xsd__string)*(*result)->__ptr[i].userRoleIdList->__size);
				asprintf(&((*result)->__ptr[i].userRoleIdList->__ptr[(*result)->__ptr[i].userRoleIdList->__size-1]), "%s", resp_dtg_set->bmdDatagramSetTable[i]->sysMetaData[j]->AnyBuf);
			}
			else if (strcmp(resp_dtg_set->bmdDatagramSetTable[i]->sysMetaData[j]->OIDTableBuf, OID_SYS_METADATA_CERT_SEC_CATEGORY_NAME)==0)
			{
				(*result)->__ptr[i].userSecurityNameList->__size++;
				(*result)->__ptr[i].userSecurityNameList->__ptr=(xsd__string *)realloc((*result)->__ptr[i].userSecurityNameList->__ptr, sizeof(xsd__string)*(*result)->__ptr[i].userSecurityNameList->__size);
				asprintf(&((*result)->__ptr[i].userSecurityNameList->__ptr[(*result)->__ptr[i].userSecurityNameList->__size-1]), "%s", resp_dtg_set->bmdDatagramSetTable[i]->sysMetaData[j]->AnyBuf);
			}
			else if (strcmp(resp_dtg_set->bmdDatagramSetTable[i]->sysMetaData[j]->OIDTableBuf, OID_SYS_METADATA_CERT_DEFAULT_ROLE_NAME)==0)
			{
				asprintf(&((*result)->__ptr[i].userDefaultRoleName), "%s", resp_dtg_set->bmdDatagramSetTable[i]->sysMetaData[j]->AnyBuf);
			}
			else if (strcmp(resp_dtg_set->bmdDatagramSetTable[i]->sysMetaData[j]->OIDTableBuf, OID_SYS_METADATA_CERT_DEFAULT_ROLE_ID)==0)
			{
				asprintf(&((*result)->__ptr[i].userDefaultRoleId), "%s", resp_dtg_set->bmdDatagramSetTable[i]->sysMetaData[j]->AnyBuf);
			}
			else if (strcmp(resp_dtg_set->bmdDatagramSetTable[i]->sysMetaData[j]->OIDTableBuf, OID_SYS_METADATA_CERT_DEFAULT_GROUP_NAME)==0)
			{
				asprintf(&((*result)->__ptr[i].userDefaultGroupName), "%s", resp_dtg_set->bmdDatagramSetTable[i]->sysMetaData[j]->AnyBuf);
			}
			else if (strcmp(resp_dtg_set->bmdDatagramSetTable[i]->sysMetaData[j]->OIDTableBuf, OID_SYS_METADATA_CERT_DEFAULT_GROUP_ID)==0)
			{
				asprintf(&((*result)->__ptr[i].userDefaultGroupId), "%s", resp_dtg_set->bmdDatagramSetTable[i]->sysMetaData[j]->AnyBuf);
			}


		}
	}

	PRINT_SOAP("Status:\t\trequest served correctly\n");
	bmd_datagramset_free(&resp_dtg_set);
	bmd_datagramset_free(&dtg_set);

	bmd_logout(&bi, deserialisation_max_memory);
	bmd_end();

	/************/
	/* porzadki */
	/************/


	return SOAP_OK;
}


/********************************************************************************************************************/
int bmd230__bmdRegisterNewUser(	struct soap *soap,
				struct xsd__base64Binary *cert,
				char *roleName,
				char *groupName,
				char *userClassId,
				struct bmd230__singleUserInfo *userAttributes,
				struct bmd230__userRegistrationResult** result)
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


	PRINT_SOAP("Request:\tregister new user\n");
	/******************************/
	/*	walidacja parametrow	*/
	/******************************/

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
	SOAP_FOK_LOGOUT(bmd_datagram_create(BMD_DATAGRAM_TYPE_REGISTER_USER, &dtg));
	if (userClassId!=NULL)
	{
		SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(	OID_ACTION_METADATA_CERT_CHOSEN_CLASS_ID, userClassId, &dtg));
	}

	/*************************************/
	/* ustawienie kryteriow wyszukiwania */
	/*************************************/
	if (userAttributes)
	{
		if (userAttributes->userCertIssuer)
		{
			tmp_gb.buf=(char *)userAttributes->userCertIssuer;
			tmp_gb.size=strlen(userAttributes->userCertIssuer);
			if (tmp_gb.size>max_metadata_value_length)	{	SOAP_FOK_LOGOUT(BMDSOAP_SERVER_METADATA_VALUE_TOO_LONG);	}
			SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_2(OID_SYS_METADATA_CERT_USER_ISSUER, &tmp_gb, &dtg, 1));
		}
		else
		{
			SOAP_FOK_LOGOUT(BMDSOAP_SERVER_USER_ISSUER_UNDEFINED);
		}

		if (userAttributes->userCertSerial)
		{
			tmp_gb.buf=(char *)userAttributes->userCertSerial;
			tmp_gb.size=strlen(userAttributes->userCertSerial);
			if (tmp_gb.size>max_metadata_value_length)	{	SOAP_FOK_LOGOUT(BMDSOAP_SERVER_METADATA_VALUE_TOO_LONG);	}
			SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_2(OID_SYS_METADATA_CERT_USER_SN, &tmp_gb,&dtg, 1));
		}
		else
		{
			SOAP_FOK_LOGOUT(BMDSOAP_SERVER_USER_SERIAL_UNDEFINED);
		}

		if (userAttributes->userName)
		{
			tmp_gb.buf=(char *)userAttributes->userName;
			tmp_gb.size=strlen(userAttributes->userName);
			if (tmp_gb.size>max_metadata_value_length)	{	SOAP_FOK_LOGOUT(BMDSOAP_SERVER_METADATA_VALUE_TOO_LONG);	}
			SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_2(OID_SYS_METADATA_CERT_USER_NAME, &tmp_gb,&dtg, 1));
		}
		else
		{
			SOAP_FOK_LOGOUT(BMDSOAP_SERVER_USER_NAME_UNDEFINED);
		}

		if (userAttributes->userIdentity)
		{
			tmp_gb.buf=(char *)userAttributes->userIdentity;
			tmp_gb.size=strlen(userAttributes->userIdentity);
			if (tmp_gb.size>max_metadata_value_length)	{	SOAP_FOK_LOGOUT(BMDSOAP_SERVER_METADATA_VALUE_TOO_LONG);	}
			SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_2(OID_SYS_METADATA_CERT_USER_IDENTITY, &tmp_gb,&dtg, 1));
		}
// 		else
// 		{
// 			SOAP_FOK_LOGOUT(BMDSOAP_SERVER_USER_IDENTITY_UNDEFINED);
// 		}

		if (userAttributes->userDefaultGroupId)
		{
			tmp_gb.buf=(char *)userAttributes->userDefaultGroupId;
			tmp_gb.size=strlen(userAttributes->userDefaultGroupId);
			if (tmp_gb.size>max_metadata_value_length)	{	SOAP_FOK_LOGOUT(BMDSOAP_SERVER_METADATA_VALUE_TOO_LONG);	}
			SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_2(OID_SYS_METADATA_CERT_DEFAULT_GROUP_ID, &tmp_gb,&dtg, 1));
		}
// 		else
// 		{
// 			SOAP_FOK_LOGOUT(BMDSOAP_SERVER_USER_DEFAULT_GROUP_UNDEFINED);
// 		}

		if (userAttributes->userDefaultClassId)
		{
			if (strlen(userAttributes->userDefaultClassId)>max_metadata_value_length)	{	SOAP_FOK_LOGOUT(BMDSOAP_SERVER_METADATA_VALUE_TOO_LONG);	}
			SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(OID_ACTION_METADATA_CERT_DEFAULT_CLASS_ID, userAttributes->userDefaultClassId, &dtg));
		}
		else
		{
			SOAP_FOK_LOGOUT(BMDSOAP_SERVER_USER_DEFAULT_CLASS_UNDEFINED);
		}

		if (userAttributes->userDefaultRoleId)
		{
			if (strlen(userAttributes->userDefaultRoleId)>max_metadata_value_length)	{	SOAP_FOK_LOGOUT(BMDSOAP_SERVER_METADATA_VALUE_TOO_LONG);	}
			SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(OID_SYS_METADATA_CERT_DEFAULT_ROLE_ID, userAttributes->userDefaultRoleId, &dtg));
		}
		else
		{
			SOAP_FOK_LOGOUT(BMDSOAP_SERVER_USER_DEFAULT_ROLE_UNDEFINED);
		}

		if ((userAttributes->userRoleIdList) && (userAttributes->userRoleIdList->__size>0))
		{
			for (i=0; i<userAttributes->userRoleIdList->__size; i++)
			{
				if (strlen(userAttributes->userRoleIdList->__ptr[i])>max_metadata_value_length)	{	SOAP_FOK_LOGOUT(BMDSOAP_SERVER_METADATA_VALUE_TOO_LONG);	}
				SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(OID_SYS_METADATA_CERT_ROLE_ID, userAttributes->userRoleIdList->__ptr[i], &dtg));
			}
		}

		if ((userAttributes->userGroupIdList) && (userAttributes->userGroupIdList->__size>0))
		{

			for (i=0; i<userAttributes->userGroupIdList->__size; i++)
			{
				if (strlen(userAttributes->userGroupIdList->__ptr[i])>max_metadata_value_length)	{	SOAP_FOK_LOGOUT(BMDSOAP_SERVER_METADATA_VALUE_TOO_LONG);	}
				SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(OID_SYS_METADATA_CERT_GROUP_ID, userAttributes->userGroupIdList->__ptr[i], &dtg));
			}
		}

		if ((userAttributes->userClassIdList) && (userAttributes->userClassIdList->__size>0))
		{

			for (i=0; i<userAttributes->userClassIdList->__size; i++)
			{
				if (strlen(userAttributes->userClassIdList->__ptr[i])>max_metadata_value_length)	{	SOAP_FOK_LOGOUT(BMDSOAP_SERVER_METADATA_VALUE_TOO_LONG);	}
				SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(OID_ACTION_METADATA_CERT_CLASS_ID, userAttributes->userClassIdList->__ptr[i],&dtg));
			}
		}

		if ((userAttributes->userSecurityNameList) && (userAttributes->userSecurityNameList->__size>0))
		{
			for (i=0; i<userAttributes->userSecurityNameList->__size; i++)
			{
				if (strlen(userAttributes->userSecurityNameList->__ptr[i])>max_metadata_value_length)	{	SOAP_FOK_LOGOUT(BMDSOAP_SERVER_METADATA_VALUE_TOO_LONG);	}
				SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(OID_SYS_METADATA_CERT_SEC_CATEGORY_NAME, userAttributes->userSecurityNameList->__ptr[i],&dtg));
			}
		}

		/********************************************************************/
		/* w przypadku jednoczesnego utworzenia nowej grupy dla uzytkownika */
		/********************************************************************/
		if (userAttributes->userNewGroupInfo)
		{
			if (userAttributes->userNewGroupInfo->userNewGroupName)
			{
				if (strlen(userAttributes->userNewGroupInfo->userNewGroupName)>max_metadata_value_length)	{	SOAP_FOK_LOGOUT(BMDSOAP_SERVER_METADATA_VALUE_TOO_LONG);	}
				SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(OID_SYS_METADATA_CERT_NEW_GROUP_NAME, userAttributes->userNewGroupInfo->userNewGroupName, &dtg));
			}

			if (userAttributes->userNewGroupInfo->userNewGroupParentIds)
			{
				for (i=0; i<userAttributes->userNewGroupInfo->userNewGroupParentIds->__size; i++)
				{
					if (strlen(userAttributes->userNewGroupInfo->userNewGroupParentIds->__ptr[i])>max_metadata_value_length)	{	SOAP_FOK_LOGOUT(BMDSOAP_SERVER_METADATA_VALUE_TOO_LONG);	}
					SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(OID_SYS_METADATA_CERT_NEW_GROUP_PARENT_ID, userAttributes->userNewGroupInfo->userNewGroupParentIds->__ptr[i], &dtg));
				}
			}

			if (userAttributes->userNewGroupInfo->userNewGroupChildIds)
			{
				for (i=0; i<userAttributes->userNewGroupInfo->userNewGroupChildIds->__size; i++)
				{
					if (strlen(userAttributes->userNewGroupInfo->userNewGroupChildIds->__ptr[i])>max_metadata_value_length)	{	SOAP_FOK_LOGOUT(BMDSOAP_SERVER_METADATA_VALUE_TOO_LONG);	}
					SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(OID_SYS_METADATA_CERT_NEW_GROUP_CHILD_ID, userAttributes->userNewGroupInfo->userNewGroupChildIds->__ptr[i], &dtg));
				}
			}
		}
	}
	else
	{
		SOAP_FOK_LOGOUT(BMDSOAP_SERVER_USER_USER_INFO_UNDEFINED);
	}


	/************************************/
	/*	dodanie datagramu do zestawu	*/
	/************************************/
	SOAP_FOK_LOGOUT(bmd_datagram_add_to_set(dtg,&dtg_set));

	/******************************************/
	/*	wyslanie zadania do serwera bmd	*/
	/******************************************/
	SOAP_FOK_LOGOUT(bmd_send_request(bi, dtg_set, max_datagram_in_set_transmission, &resp_dtg_set,1, deserialisation_max_memory));
	bmd_logout(&bi, deserialisation_max_memory);
	bmd_end();
	bmd_datagramset_free(&dtg_set);
	SOAP_FOK(resp_dtg_set->bmdDatagramSetTable[0]->datagramStatus);

	/****************************/
	/* przygotowanie odpowiedzi */
	/****************************/
	*result=(struct bmd230__userRegistrationResult*)calloc(1, sizeof(struct bmd230__userRegistrationResult));
	if(*result == NULL)
		{ SOAP_FOK(BMD_ERR_MEMORY); }
	
	for(i=0; i<resp_dtg_set->bmdDatagramSetTable[0]->no_of_sysMetaData; i++)
	{
		if(resp_dtg_set->bmdDatagramSetTable[0]->sysMetaData[i] != NULL)
		{
			if(resp_dtg_set->bmdDatagramSetTable[0]->sysMetaData[i]->OIDTableBuf != NULL)
			{
				if( strcmp(resp_dtg_set->bmdDatagramSetTable[0]->sysMetaData[i]->OIDTableBuf, OID_SYS_METADATA_CERT_USER_IDENTITY) == 0 )
				{
					if(resp_dtg_set->bmdDatagramSetTable[0]->sysMetaData[i]->AnyBuf != NULL)
					{
						(*result)->userIdentityId=strtol(resp_dtg_set->bmdDatagramSetTable[0]->sysMetaData[i]->AnyBuf, NULL, 10);
					}
				}
				else if( strcmp(resp_dtg_set->bmdDatagramSetTable[0]->sysMetaData[i]->OIDTableBuf, OID_SYS_METADATA_CERT_USER_ID) == 0 )
				{
					if(resp_dtg_set->bmdDatagramSetTable[0]->sysMetaData[i]->AnyBuf != NULL)
					{
						(*result)->userCertificateId=strtol(resp_dtg_set->bmdDatagramSetTable[0]->sysMetaData[i]->AnyBuf, NULL, 10);
					}
				}
			}
		}
	}
	if((*result)->userIdentityId == 0 || (*result)->userCertificateId == 0)
	{ 
		free(*result); *result=NULL;
		SOAP_FOK(BMDSOAP_SERVER_INTERNAL_ERROR);
	}
	
	PRINT_SOAP("Status:\t\trequest served correctly\n");
	bmd_datagramset_free(&resp_dtg_set);
	
	return SOAP_OK;
}


int bmd230__bmdAddCertificateToIdentity(	struct soap *soap,
					struct xsd__base64Binary *cert,
					char *userRoleId,
					char *userGroupId,
					char *userClassId,
					char *identityId,
					struct bmd230__singleUserInfo *userAttributes,
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


	PRINT_SOAP("Request:\tadd certificate to identity\n");
	/******************************/
	/*	walidacja parametrow	*/
	/******************************/

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
			userRoleId, userGroupId, deserialisation_max_memory));
	free(derUserCert); derUserCert=NULL;
	paramCert=NULL; paramCertLen=0;

	if(identityId == NULL || strlen(identityId) <= 0)
	{
		SOAP_FOK_LOGOUT(BMDSOAP_SERVER_USER_IDENTITY_UNDEFINED);
	}

	/********************************/
	/*	stworzenie datagramu	*/
	/********************************/
	SOAP_FOK_LOGOUT(bmd_datagram_create(BMD_DATAGRAM_TYPE_ADD_CERTIFICATE_TO_IDENTITY, &dtg));
	if (userClassId!=NULL)
	{
		SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(	OID_ACTION_METADATA_CERT_CHOSEN_CLASS_ID, userClassId, &dtg));
	}

	SOAP_FOK_LOGOUT(set_gen_buf2(identityId, strlen(identityId), &(dtg->protocolDataFileId)));

	/*************************************/
	/* ustawienie kryteriow wyszukiwania */
	/*************************************/
	if (userAttributes)
	{
		if (userAttributes->userCertIssuer)
		{
			tmp_gb.buf=(char *)userAttributes->userCertIssuer;
			tmp_gb.size=strlen(userAttributes->userCertIssuer);
			if (tmp_gb.size>max_metadata_value_length)	{	SOAP_FOK_LOGOUT(BMDSOAP_SERVER_METADATA_VALUE_TOO_LONG);	}
			SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_2(OID_SYS_METADATA_CERT_USER_ISSUER, &tmp_gb, &dtg, 1));
		}
		else
		{
			SOAP_FOK_LOGOUT(BMDSOAP_SERVER_USER_ISSUER_UNDEFINED);
		}

		if (userAttributes->userCertSerial)
		{
			tmp_gb.buf=(char *)userAttributes->userCertSerial;
			tmp_gb.size=strlen(userAttributes->userCertSerial);
			if (tmp_gb.size>max_metadata_value_length)	{	SOAP_FOK_LOGOUT(BMDSOAP_SERVER_METADATA_VALUE_TOO_LONG);	}
			SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_2(OID_SYS_METADATA_CERT_USER_SN, &tmp_gb,&dtg, 1));
		}
		else
		{
			SOAP_FOK_LOGOUT(BMDSOAP_SERVER_USER_SERIAL_UNDEFINED);
		}

		if (userAttributes->userName)
		{
			tmp_gb.buf=(char *)userAttributes->userName;
			tmp_gb.size=strlen(userAttributes->userName);
			if (tmp_gb.size>max_metadata_value_length)	{	SOAP_FOK_LOGOUT(BMDSOAP_SERVER_METADATA_VALUE_TOO_LONG);	}
			SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_2(OID_SYS_METADATA_CERT_USER_NAME, &tmp_gb,&dtg, 1));
		}
		else
		{
			SOAP_FOK_LOGOUT(BMDSOAP_SERVER_USER_NAME_UNDEFINED);
		}

		if (userAttributes->userIdentity)
		{
			tmp_gb.buf=(char *)userAttributes->userIdentity;
			tmp_gb.size=strlen(userAttributes->userIdentity);
			if (tmp_gb.size>max_metadata_value_length)	{	SOAP_FOK_LOGOUT(BMDSOAP_SERVER_METADATA_VALUE_TOO_LONG);	}
			SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_2(OID_SYS_METADATA_CERT_USER_IDENTITY, &tmp_gb,&dtg, 1));
		}
// 		else
// 		{
// 			SOAP_FOK_LOGOUT(BMDSOAP_SERVER_USER_IDENTITY_UNDEFINED);
// 		}

		if (userAttributes->userDefaultGroupId)
		{
			tmp_gb.buf=(char *)userAttributes->userDefaultGroupId;
			tmp_gb.size=strlen(userAttributes->userDefaultGroupId);
			if (tmp_gb.size>max_metadata_value_length)	{	SOAP_FOK_LOGOUT(BMDSOAP_SERVER_METADATA_VALUE_TOO_LONG);	}
			SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_2(OID_SYS_METADATA_CERT_DEFAULT_GROUP_ID, &tmp_gb,&dtg, 1));
		}
// 		else
// 		{
// 			SOAP_FOK_LOGOUT(BMDSOAP_SERVER_USER_DEFAULT_GROUP_UNDEFINED);
// 		}

		if (userAttributes->userDefaultClassId)
		{
			if (strlen(userAttributes->userDefaultClassId)>max_metadata_value_length)	{	SOAP_FOK_LOGOUT(BMDSOAP_SERVER_METADATA_VALUE_TOO_LONG);	}
			SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(OID_ACTION_METADATA_CERT_DEFAULT_CLASS_ID, userAttributes->userDefaultClassId, &dtg));
		}
		else
		{
			SOAP_FOK_LOGOUT(BMDSOAP_SERVER_USER_DEFAULT_CLASS_UNDEFINED);
		}

		if (userAttributes->userDefaultRoleId)
		{
			if (strlen(userAttributes->userDefaultRoleId)>max_metadata_value_length)	{	SOAP_FOK_LOGOUT(BMDSOAP_SERVER_METADATA_VALUE_TOO_LONG);	}
			SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(OID_SYS_METADATA_CERT_DEFAULT_ROLE_ID, userAttributes->userDefaultRoleId, &dtg));
		}
		else
		{
			SOAP_FOK_LOGOUT(BMDSOAP_SERVER_USER_DEFAULT_ROLE_UNDEFINED);
		}

		if ((userAttributes->userRoleIdList) && (userAttributes->userRoleIdList->__size>0))
		{
			for (i=0; i<userAttributes->userRoleIdList->__size; i++)
			{
				if (strlen(userAttributes->userRoleIdList->__ptr[i])>max_metadata_value_length)	{	SOAP_FOK_LOGOUT(BMDSOAP_SERVER_METADATA_VALUE_TOO_LONG);	}
				SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(OID_SYS_METADATA_CERT_ROLE_ID, userAttributes->userRoleIdList->__ptr[i], &dtg));
			}
		}

		if ((userAttributes->userGroupIdList) && (userAttributes->userGroupIdList->__size>0))
		{

			for (i=0; i<userAttributes->userGroupIdList->__size; i++)
			{
				if (strlen(userAttributes->userGroupIdList->__ptr[i])>max_metadata_value_length)	{	SOAP_FOK_LOGOUT(BMDSOAP_SERVER_METADATA_VALUE_TOO_LONG);	}
				SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(OID_SYS_METADATA_CERT_GROUP_ID, userAttributes->userGroupIdList->__ptr[i], &dtg));
			}
		}

		if ((userAttributes->userClassIdList) && (userAttributes->userClassIdList->__size>0))
		{

			for (i=0; i<userAttributes->userClassIdList->__size; i++)
			{
				if (strlen(userAttributes->userClassIdList->__ptr[i])>max_metadata_value_length)	{	SOAP_FOK_LOGOUT(BMDSOAP_SERVER_METADATA_VALUE_TOO_LONG);	}
				SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(OID_ACTION_METADATA_CERT_CLASS_ID, userAttributes->userClassIdList->__ptr[i],&dtg));
			}
		}

		if ((userAttributes->userSecurityNameList) && (userAttributes->userSecurityNameList->__size>0))
		{
			for (i=0; i<userAttributes->userSecurityNameList->__size; i++)
			{
				if (strlen(userAttributes->userSecurityNameList->__ptr[i])>max_metadata_value_length)	{	SOAP_FOK_LOGOUT(BMDSOAP_SERVER_METADATA_VALUE_TOO_LONG);	}
				SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(OID_SYS_METADATA_CERT_SEC_CATEGORY_NAME, userAttributes->userSecurityNameList->__ptr[i],&dtg));
			}
		}

		/********************************************************************/
		/* w przypadku jednoczesnego utworzenia nowej grupy dla uzytkownika */
		/********************************************************************/
		if (userAttributes->userNewGroupInfo)
		{
			if (userAttributes->userNewGroupInfo->userNewGroupName)
			{
				if (strlen(userAttributes->userNewGroupInfo->userNewGroupName)>max_metadata_value_length)	{	SOAP_FOK_LOGOUT(BMDSOAP_SERVER_METADATA_VALUE_TOO_LONG);	}
				SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(OID_SYS_METADATA_CERT_NEW_GROUP_NAME, userAttributes->userNewGroupInfo->userNewGroupName, &dtg));
			}

			if (userAttributes->userNewGroupInfo->userNewGroupParentIds)
			{
				for (i=0; i<userAttributes->userNewGroupInfo->userNewGroupParentIds->__size; i++)
				{
					if (strlen(userAttributes->userNewGroupInfo->userNewGroupParentIds->__ptr[i])>max_metadata_value_length)	{	SOAP_FOK_LOGOUT(BMDSOAP_SERVER_METADATA_VALUE_TOO_LONG);	}
					SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(OID_SYS_METADATA_CERT_NEW_GROUP_PARENT_ID, userAttributes->userNewGroupInfo->userNewGroupParentIds->__ptr[i], &dtg));
				}
			}

			if (userAttributes->userNewGroupInfo->userNewGroupChildIds)
			{
				for (i=0; i<userAttributes->userNewGroupInfo->userNewGroupChildIds->__size; i++)
				{
					if (strlen(userAttributes->userNewGroupInfo->userNewGroupChildIds->__ptr[i])>max_metadata_value_length)	{	SOAP_FOK_LOGOUT(BMDSOAP_SERVER_METADATA_VALUE_TOO_LONG);	}
					SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(OID_SYS_METADATA_CERT_NEW_GROUP_CHILD_ID, userAttributes->userNewGroupInfo->userNewGroupChildIds->__ptr[i], &dtg));
				}
			}
		}
	}
	else
	{
		SOAP_FOK_LOGOUT(BMDSOAP_SERVER_USER_USER_INFO_UNDEFINED);
	}


	/************************************/
	/*	dodanie datagramu do zestawu	*/
	/************************************/
	SOAP_FOK_LOGOUT(bmd_datagram_add_to_set(dtg,&dtg_set));

	/******************************************/
	/*	wyslanie zadania do serwera bmd	*/
	/******************************************/
	SOAP_FOK_LOGOUT(bmd_send_request(bi, dtg_set, max_datagram_in_set_transmission, &resp_dtg_set,1, deserialisation_max_memory));
	bmd_logout(&bi, deserialisation_max_memory);
	bmd_end();
	bmd_datagramset_free(&dtg_set);
	SOAP_FOK(resp_dtg_set->bmdDatagramSetTable[0]->datagramStatus);

	/****************************/
	/* przygotowanie odpowiedzi */
	/****************************/
	for(i=0; i<resp_dtg_set->bmdDatagramSetTable[0]->no_of_sysMetaData; i++)
	{
		if(resp_dtg_set->bmdDatagramSetTable[0]->sysMetaData[i] != NULL)
		{
			if(resp_dtg_set->bmdDatagramSetTable[0]->sysMetaData[i]->OIDTableBuf != NULL)
			{
				if( strcmp(resp_dtg_set->bmdDatagramSetTable[0]->sysMetaData[i]->OIDTableBuf, OID_SYS_METADATA_CERT_USER_ID) == 0 )
				{
					if(resp_dtg_set->bmdDatagramSetTable[0]->sysMetaData[i]->AnyBuf != NULL)
					{
						(*result)=strtol(resp_dtg_set->bmdDatagramSetTable[0]->sysMetaData[i]->AnyBuf, NULL, 10);
					}
				}
			}
		}
	}
	if((*result) == 0)
	{ 
		SOAP_FOK(BMDSOAP_SERVER_INTERNAL_ERROR);
	}


	PRINT_SOAP("Status:\t\trequest served correctly\n");
	bmd_datagramset_free(&resp_dtg_set);
	
	return SOAP_OK;
}

/********************************************************************************************************************/
int bmd230__bmdUpdateUserRights(	struct soap *soap,
				struct xsd__base64Binary *cert,
				char *roleName,
				char *groupName,
				char *userClassId,
				char *userId,
				struct bmd230__singleUserInfo *userAttributes,
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

	PRINT_SOAP("Request:\tupdate user rights\n");
	/************************/
	/* walidacja parametrow */
	/************************/
	if (userId==NULL)			{	SOAP_FOK(BMDSOAP_SERVER_USER_ID_UNDEFINED);	}

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
	SOAP_FOK_LOGOUT(bmd_datagram_create(BMD_DATAGRAM_TYPE_UPDATE_USER, &dtg));
	if (userClassId!=NULL)
	{
		SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(	OID_ACTION_METADATA_CERT_CHOSEN_CLASS_ID, userClassId, &dtg));
	}
	SOAP_FOK_LOGOUT(set_gen_buf2(userId, strlen(userId), &(dtg->protocolDataFileId)));
	/************************************/
	/* ustawienie atrybutów użytkownika */
	/************************************/
	if (userAttributes)
	{
		if (userAttributes->userCertIssuer)
		{
			SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(	OID_SYS_METADATA_CERT_USER_ISSUER, userAttributes->userCertIssuer, &dtg));
		}

		if (userAttributes->userCertSerial)
		{
			SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(	OID_SYS_METADATA_CERT_USER_SN, userAttributes->userCertSerial, &dtg));
		}

		if (userAttributes->userName)
		{
			SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(	OID_SYS_METADATA_CERT_USER_NAME, userAttributes->userName, &dtg));
		}

		if (userAttributes->userIdentity)
		{
			SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(	OID_SYS_METADATA_CERT_USER_IDENTITY, userAttributes->userIdentity, &dtg));
		}

		if (userAttributes->userDefaultRoleId)
		{
			SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(	OID_SYS_METADATA_CERT_DEFAULT_ROLE_ID, userAttributes->userDefaultRoleId, &dtg));
		}

		if (userAttributes->userDefaultGroupId)
		{
			SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(	OID_SYS_METADATA_CERT_DEFAULT_GROUP_ID, userAttributes->userDefaultGroupId, &dtg));
		}

		if (userAttributes->userDefaultClassId)
		{
			SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(	OID_ACTION_METADATA_CERT_DEFAULT_CLASS_ID, userAttributes->userDefaultClassId, &dtg));
		}

		if ((userAttributes->userRoleIdList) && (userAttributes->userRoleIdList->__size>0))
		{
			for (i=0; i<userAttributes->userRoleIdList->__size; i++)
			{
				SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(	OID_SYS_METADATA_CERT_ROLE_ID, userAttributes->userRoleIdList->__ptr[i], &dtg));
			}
		}

		if ((userAttributes->userGroupIdList) && (userAttributes->userGroupIdList->__size>0))
		{
			for (i=0; i<userAttributes->userGroupIdList->__size; i++)
			{
				SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(	OID_SYS_METADATA_CERT_GROUP_ID, userAttributes->userGroupIdList->__ptr[i], &dtg));
			}
		}

		if ((userAttributes->userClassIdList) && (userAttributes->userClassIdList->__size>0))
		{
			for (i=0; i<userAttributes->userClassIdList->__size; i++)
			{
				SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(	OID_ACTION_METADATA_CERT_CLASS_ID, userAttributes->userClassIdList->__ptr[i], &dtg));
			}
		}

		if ((userAttributes->userSecurityNameList) && (userAttributes->userSecurityNameList->__size>0))
		{
			for (i=0; i<userAttributes->userSecurityNameList->__size; i++)
			{
				SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(	OID_SYS_METADATA_CERT_SEC_CATEGORY_NAME, userAttributes->userSecurityNameList->__ptr[i], &dtg));
			}
		}
	}
	else
	{
		SOAP_FOK_LOGOUT(BMDSOAP_SERVER_USER_USER_INFO_UNDEFINED);
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
	bmd_datagramset_free(&resp_dtg_set);
	bmd_datagramset_free(&dtg_set);

	bmd_logout(&bi, deserialisation_max_memory);
	bmd_end();

	/************/
	/* porzadki */
	/************/


	return SOAP_OK;
}
