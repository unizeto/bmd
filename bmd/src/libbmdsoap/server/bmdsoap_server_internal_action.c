#include <bmd/libbmdsoap/libbmdsoap.h>

int bmd230__bmdGetActionsHistoryById(struct soap *soap,
			struct xsd__base64Binary *cert,
			char *roleId,
			char *groupId,
			char *userClassId,
			char* id,
			struct bmd230__ActionsHistoryList **output)
{
long max_datagram_in_set_transmission	= 0;
long deserialisation_max_memory		= 0;
bmd_info_t *bi				= NULL;
bmdDatagram_t *requestDatagram		= NULL;
bmdDatagramSet_t *requestSet		= NULL;
bmdDatagramSet_t *responseSet		= NULL;
long fileId				= 0;
bmdDatagram_t *tmpDatagramPtr		= NULL;
long iter				= 0;
long jupiter				= 0;
struct bmd230__ActionsHistoryList* tempOutput = NULL;

char *derUserCert=NULL;
long derUserCertLen=0;
long retVal=0;
char* paramCert=NULL;
long paramCertLen=0;


	PRINT_SOAP("Request:\tget history of actions for given id\n");

	if(id == NULL)
		{ SOAP_FOK(BMDSOAP_FILE_ID_NOT_SPECIFIED); }
	if(strlen(id) <= 0)
		{ SOAP_FOK(BMDSOAP_FILE_ID_NOT_SPECIFIED); }

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


	SOAP_FOK(PrepareDataForBMDLoginAndRequest(&bi,
				(_GLOBAL_soap_wssecurity_enabled == 1  ? derUserCert : paramCert),
				(_GLOBAL_soap_wssecurity_enabled == 1  ? derUserCertLen : paramCertLen),
				roleId, groupId, deserialisation_max_memory));
	free(derUserCert); derUserCert=NULL;
	paramCert=NULL; paramCertLen=0;

	SOAP_FOK_LOGOUT(bmd_datagram_create(BMD_DATAGRAM_TYPE_GET_ACTIONS_HISTORY, &requestDatagram));

	fileId = strtol(id, NULL, 10);
	retVal=bmd_datagram_set_id(&fileId, &requestDatagram);
	if(retVal < BMD_OK)
	{
		PR2_bmdDatagram_free(&requestDatagram);
		SOAP_FOK_LOGOUT(retVal);
	}

	retVal=bmd_datagram_add_to_set(requestDatagram, &requestSet);
	if(retVal < BMD_OK)
	{
		PR2_bmdDatagram_free(&requestDatagram);
		SOAP_FOK_LOGOUT(retVal);
	}

	retVal = bmd_send_request(bi, requestSet, max_datagram_in_set_transmission, &responseSet, 1, deserialisation_max_memory);
	PR2_bmdDatagramSet_free(&requestSet);
	SOAP_FOK_LOGOUT(retVal);

	retVal = responseSet->bmdDatagramSetTable[0]->datagramStatus;
	if(retVal < BMD_OK)
	{
		PR2_bmdDatagramSet_free(&responseSet);
		SOAP_FOK_LOGOUT(retVal);
	}

	// uzupelnianie wyniku
	tempOutput = (struct bmd230__ActionsHistoryList*)soap_malloc(soap, sizeof(struct bmd230__ActionsHistoryList));
	if(tempOutput == NULL)
	{
		PR2_bmdDatagramSet_free(&responseSet);
		SOAP_FOK_LOGOUT(BMD_ERR_MEMORY);
	}
	memset(tempOutput, 0, sizeof(struct bmd230__ActionsHistoryList));
	
	tempOutput->__ptr = (struct bmd230__ActionsHistoryElement*)soap_malloc(soap, responseSet->bmdDatagramSetSize * sizeof(struct bmd230__ActionsHistoryElement));
	if(tempOutput->__ptr == NULL)
	{
		PR2_bmdDatagramSet_free(&responseSet);
		SOAP_FOK_LOGOUT(BMD_ERR_MEMORY);
	}
	memset(tempOutput->__ptr, 0, responseSet->bmdDatagramSetSize * sizeof(struct bmd230__ActionsHistoryElement));
	tempOutput->__size = responseSet->bmdDatagramSetSize;
	
	for(iter=0; iter<responseSet->bmdDatagramSetSize; iter++)
	{
		tmpDatagramPtr = responseSet->bmdDatagramSetTable[iter];
		if(tmpDatagramPtr == NULL)
		{
			PRINT_ERROR("Error: Reponse datagram %li is NULL\n", iter);
			continue;
		}
		
		for(jupiter=0; jupiter<tmpDatagramPtr->no_of_actionMetaData; jupiter++)
		{
			if(tmpDatagramPtr->actionMetaData != NULL && tmpDatagramPtr->actionMetaData[jupiter] != NULL && tmpDatagramPtr->actionMetaData[jupiter]->OIDTableBuf != NULL)
			{
				if(strcmp(OID_ACTION_METADATA_DATE, tmpDatagramPtr->actionMetaData[jupiter]->OIDTableBuf) == 0)
				{
					if(tmpDatagramPtr->actionMetaData[jupiter]->AnyBuf != NULL && tmpDatagramPtr->actionMetaData[jupiter]->AnySize > 0)
					{
						tempOutput->__ptr[iter].actionDate = (char*)soap_malloc(soap, (tmpDatagramPtr->actionMetaData[jupiter]->AnySize + 1) * sizeof(char));
						if(tempOutput->__ptr[iter].actionDate != NULL)
						{
							memset(tempOutput->__ptr[iter].actionDate, 0, (tmpDatagramPtr->actionMetaData[jupiter]->AnySize + 1) * sizeof(char));
							memcpy(tempOutput->__ptr[iter].actionDate, tmpDatagramPtr->actionMetaData[jupiter]->AnyBuf, tmpDatagramPtr->actionMetaData[jupiter]->AnySize);
						}
						else
						{
							SOAP_FOK_LOGOUT(BMD_ERR_MEMORY);
						}
					}
				}
				else if(strcmp(OID_ACTION_NAME, tmpDatagramPtr->actionMetaData[jupiter]->OIDTableBuf) == 0)
				{
					if(tmpDatagramPtr->actionMetaData[jupiter]->AnyBuf != NULL && tmpDatagramPtr->actionMetaData[jupiter]->AnySize > 0)
					{
						tempOutput->__ptr[iter].actionName = (char*)soap_malloc(soap, (tmpDatagramPtr->actionMetaData[jupiter]->AnySize + 1) * sizeof(char));
						if(tempOutput->__ptr[iter].actionName != NULL)
						{
							memset(tempOutput->__ptr[iter].actionName, 0, (tmpDatagramPtr->actionMetaData[jupiter]->AnySize + 1) * sizeof(char));
							memcpy(tempOutput->__ptr[iter].actionName, tmpDatagramPtr->actionMetaData[jupiter]->AnyBuf, tmpDatagramPtr->actionMetaData[jupiter]->AnySize);
						}
						else
						{
							SOAP_FOK_LOGOUT(BMD_ERR_MEMORY);
						}
					}
				}
				//else if(strcmp(OID_ACTION_METADATA_OBJECT_TYPE, tmpDatagramPtr->actionMetaData[jupiter]->OIDTableBuf) == 0)
				//{}
				else if(strcmp(OID_ACTION_METADATA_CERT_CLASS_NAME, tmpDatagramPtr->actionMetaData[jupiter]->OIDTableBuf) == 0)
				{
					if(tmpDatagramPtr->actionMetaData[jupiter]->AnyBuf != NULL && tmpDatagramPtr->actionMetaData[jupiter]->AnySize > 0)
					{
						tempOutput->__ptr[iter].executorClassName = (char*)soap_malloc(soap, (tmpDatagramPtr->actionMetaData[jupiter]->AnySize + 1) * sizeof(char));
						if(tempOutput->__ptr[iter].executorClassName != NULL)
						{
							memset(tempOutput->__ptr[iter].executorClassName, 0, (tmpDatagramPtr->actionMetaData[jupiter]->AnySize + 1) * sizeof(char));
							memcpy(tempOutput->__ptr[iter].executorClassName, tmpDatagramPtr->actionMetaData[jupiter]->AnyBuf, tmpDatagramPtr->actionMetaData[jupiter]->AnySize);
						}
						else
						{
							SOAP_FOK_LOGOUT(BMD_ERR_MEMORY);
						}
					}
				}
				else if(strcmp(OID_ACTION_METADATA_STATUS, tmpDatagramPtr->actionMetaData[jupiter]->OIDTableBuf) == 0)
				{
					if(tmpDatagramPtr->actionMetaData[jupiter]->AnyBuf != NULL && tmpDatagramPtr->actionMetaData[jupiter]->AnySize > 0)
					{
						tempOutput->__ptr[iter].actionStatus = (char*)soap_malloc(soap, (tmpDatagramPtr->actionMetaData[jupiter]->AnySize + 1) * sizeof(char));
						if(tempOutput->__ptr[iter].actionStatus != NULL)
						{
							memset(tempOutput->__ptr[iter].actionStatus, 0, (tmpDatagramPtr->actionMetaData[jupiter]->AnySize + 1) * sizeof(char));
							memcpy(tempOutput->__ptr[iter].actionStatus, tmpDatagramPtr->actionMetaData[jupiter]->AnyBuf, tmpDatagramPtr->actionMetaData[jupiter]->AnySize);
						}
						else
						{
							SOAP_FOK_LOGOUT(BMD_ERR_MEMORY);
						}
					}
				}
				else if(strcmp(OID_ACTION_METADATA_STATUS_MESSAGE, tmpDatagramPtr->actionMetaData[jupiter]->OIDTableBuf) == 0)
				{
					if(tmpDatagramPtr->actionMetaData[jupiter]->AnyBuf != NULL && tmpDatagramPtr->actionMetaData[jupiter]->AnySize > 0)
					{
						tempOutput->__ptr[iter].actionStatusMessage = (char*)soap_malloc(soap, (tmpDatagramPtr->actionMetaData[jupiter]->AnySize + 1) * sizeof(char));
						if(tempOutput->__ptr[iter].actionStatusMessage != NULL)
						{
							memset(tempOutput->__ptr[iter].actionStatusMessage, 0, (tmpDatagramPtr->actionMetaData[jupiter]->AnySize + 1) * sizeof(char));
							memcpy(tempOutput->__ptr[iter].actionStatusMessage, tmpDatagramPtr->actionMetaData[jupiter]->AnyBuf, tmpDatagramPtr->actionMetaData[jupiter]->AnySize);
						}
						else
						{
							SOAP_FOK_LOGOUT(BMD_ERR_MEMORY);
						}
					}
				}
				else if(strcmp(OID_ACTION_METADATA_OBJECT_ID, tmpDatagramPtr->actionMetaData[jupiter]->OIDTableBuf) == 0)
				{
					if(tmpDatagramPtr->actionMetaData[jupiter]->AnyBuf != NULL && tmpDatagramPtr->actionMetaData[jupiter]->AnySize > 0)
					{
						tempOutput->__ptr[iter].objectId = (char*)soap_malloc(soap, (tmpDatagramPtr->actionMetaData[jupiter]->AnySize + 1) * sizeof(char));
						if(tempOutput->__ptr[iter].objectId != NULL)
						{
							memset(tempOutput->__ptr[iter].objectId, 0, (tmpDatagramPtr->actionMetaData[jupiter]->AnySize + 1) * sizeof(char));
							memcpy(tempOutput->__ptr[iter].objectId, tmpDatagramPtr->actionMetaData[jupiter]->AnyBuf, tmpDatagramPtr->actionMetaData[jupiter]->AnySize);
						}
						else
						{
							SOAP_FOK_LOGOUT(BMD_ERR_MEMORY);
						}
					}
				}
			}
		}

		for(jupiter=0; jupiter<tmpDatagramPtr->no_of_sysMetaData; jupiter++)
		{
			if(tmpDatagramPtr->sysMetaData != NULL && tmpDatagramPtr->sysMetaData[jupiter] != NULL && tmpDatagramPtr->sysMetaData[jupiter]->OIDTableBuf != NULL)
			{
				if(strcmp(OID_SYS_METADATA_CRYPTO_OBJECTS_FILE_TYPE, tmpDatagramPtr->sysMetaData[jupiter]->OIDTableBuf) == 0)
				{
					if(tmpDatagramPtr->sysMetaData[jupiter]->AnyBuf != NULL && tmpDatagramPtr->sysMetaData[jupiter]->AnySize > 0)
					{
						tempOutput->__ptr[iter].objectType = (char*)soap_malloc(soap, (tmpDatagramPtr->sysMetaData[jupiter]->AnySize + 1) * sizeof(char));
						if(tempOutput->__ptr[iter].objectType != NULL)
						{
							memset(tempOutput->__ptr[iter].objectType, 0, (tmpDatagramPtr->sysMetaData[jupiter]->AnySize + 1) * sizeof(char));
							memcpy(tempOutput->__ptr[iter].objectType, tmpDatagramPtr->sysMetaData[jupiter]->AnyBuf, tmpDatagramPtr->sysMetaData[jupiter]->AnySize);
						}
						else
						{
							SOAP_FOK_LOGOUT(BMD_ERR_MEMORY);
						}
					}
				}
				else if(strcmp(OID_SYS_METADATA_CERT_USER_IDENTITY_DESCRIPTION, tmpDatagramPtr->sysMetaData[jupiter]->OIDTableBuf) == 0)
				{
					if(tmpDatagramPtr->sysMetaData[jupiter]->AnyBuf != NULL && tmpDatagramPtr->sysMetaData[jupiter]->AnySize > 0)
					{
						tempOutput->__ptr[iter].executorIdentityName = (char*)soap_malloc(soap, (tmpDatagramPtr->sysMetaData[jupiter]->AnySize + 1) * sizeof(char));
						if(tempOutput->__ptr[iter].executorIdentityName != NULL)
						{
							memset(tempOutput->__ptr[iter].executorIdentityName, 0, (tmpDatagramPtr->sysMetaData[jupiter]->AnySize + 1) * sizeof(char));
							memcpy(tempOutput->__ptr[iter].executorIdentityName, tmpDatagramPtr->sysMetaData[jupiter]->AnyBuf, tmpDatagramPtr->sysMetaData[jupiter]->AnySize);
						}
						else
						{
							SOAP_FOK_LOGOUT(BMD_ERR_MEMORY);
						}
					}
				}
			}
		}
	}

	// pusta historia akcji
	if(responseSet->bmdDatagramSetSize == 1 &&
		tempOutput->__ptr[0].objectId == NULL && tempOutput->__ptr[0].objectType == NULL &&
		tempOutput->__ptr[0].executorIdentityName == NULL && tempOutput->__ptr[0].executorClassName == NULL &&
		tempOutput->__ptr[0].actionName == NULL && tempOutput->__ptr[0].actionDate == NULL &&
		tempOutput->__ptr[0].actionStatus == NULL && tempOutput->__ptr[0].actionStatusMessage == NULL )
	{
		soap_dealloc(soap, tempOutput->__ptr);
		tempOutput->__ptr = NULL;
		tempOutput->__size = 0;
	}
	
	PR2_bmdDatagramSet_free(&responseSet);
	bmd_logout(&bi, deserialisation_max_memory);
	bmd_end();

	*output = tempOutput;
	tempOutput = NULL;

	PRINT_SOAP("Status:\t\trequest served correctly\n");
	return SOAP_OK;
}
