#include <bmd/libbmdsoap/libbmdsoap.h>
#include <bmd/libbmdglobals/libbmdglobals.h>

/**********************************************/
/* obsluga zadania wyslania pliku do archiwum */
/**********************************************/
int bmd230__bmdUpdateDefaultMetadataValue(	struct soap *soap,
					struct xsd__base64Binary *cert,
					char *roleName,
					char *groupName,
					char *userClassId,
					struct bmd230__mtdsValues *mtds,
					int *result)
{
long i					= 0;
long max_datagram_in_set_transmission	= 0;
long deserialisation_max_memory		= 0;
long max_metadata_value_length		= 0;
bmdDatagram_t *dtg			= NULL;
bmdDatagramSet_t *req_dtg_set		= NULL;
bmdDatagramSet_t *res_dtg_set		= NULL;
bmd_info_t *bi				= NULL;

char *derUserCert=NULL;
long derUserCertLen=0;
long retVal=0;
char* paramCert=NULL;
long paramCertLen=0;

	PRINT_SOAP("Request:\tupdating metadata default values\n");
	/************************************************/
	/*	poczatkowe ustalenie zwracanej wartosci	*/
	/************************************************/
	(*result)=-1;

	/**************************/
	/* wczytanie konfiguracji */
	/**************************/
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
	/* stworzenie datagramu zadania */
	/********************************/
	SOAP_FOK_LOGOUT(bmd_datagram_create(BMD_DATAGRAM_TYPE_UPDATE_METADATA_DEFAULT_VALUE, &dtg));

	if (mtds!=NULL)
	{
		for (i=0; i<mtds->__size; i++)
		{
			if ((mtds->__ptr[i].mtdOid==NULL) || (mtds->__ptr[i].mtdValue==NULL))
			{
				continue;
			}
			if (strlen(mtds->__ptr[i].mtdValue)>max_metadata_value_length)
			{
				SOAP_FOK_LOGOUT(BMDSOAP_SERVER_METADATA_VALUE_TOO_LONG);
			}

			SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char_type(mtds->__ptr[i].mtdOid, ADDITIONAL_METADATA, mtds->__ptr[i].mtdValue, &dtg));
		}
	}

	SOAP_FOK_LOGOUT(bmd_datagram_add_to_set(dtg,&req_dtg_set));

	/***************************/
	/* wyslanie zadania do BMD */
	/***************************/
	SOAP_FOK_LOGOUT(bmd_send_request(bi,req_dtg_set, max_datagram_in_set_transmission, &res_dtg_set, 1, deserialisation_max_memory));

	/************************/
	/* odebranie odpowiedzi */
	/************************/
	SOAP_FOK_LOGOUT(res_dtg_set->bmdDatagramSetTable[0]->datagramStatus);

	/*********************/
	/* wylogowanie z BMD */
	/*********************/
	bmd_logout(&bi, deserialisation_max_memory);
	bmd_end();

	/************/
	/* porzadki */
	/************/
	bmd_datagramset_free(&req_dtg_set);
	bmd_datagramset_free(&res_dtg_set);

	(*result)=0;

	PRINT_SOAP("Status:\t\trequest served correctly\n");
	return SOAP_OK;
}

/**********************************************/
/* obsluga zadania wyslania pliku do archiwum */
/**********************************************/
int bmd230__bmdUpdateMetadata(	struct soap *soap,
				struct xsd__base64Binary *cert,
				char *userRoleId,
				char *userGroupId,
				char *userClassId,
				long id,
				struct bmd230__mtdsValues *mtds,
				char *updateReason,
				int *result)
{
char **oids_table			= NULL;
char **values_table			= NULL;
long oids_count				= 0;

bmdDatagramSet_t *req_dtg_set		= NULL;
bmdDatagramSet_t *res_dtg_set		= NULL;
bmd_info_t *bi						= NULL;

long max_datagram_in_set_transmission	= 0;
long deserialisation_max_memory			= 0;

char *derUserCert=NULL;
long derUserCertLen=0;
long retVal=0;
char* paramCert=NULL;
long paramCertLen=0;

	PRINT_SOAP("Request:\tupdating metadata values for file %li\n",id);
	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if (id<=0)			{	SOAP_FOK(BMDSOAP_SERVER_INVALID_FILE_ID);	}
	if (updateReason==NULL)		{	SOAP_FOK(BMDSOAP_SERVER_INVALID_UPDATE_REASON);	}
	if (strlen(updateReason)<=0)	{	SOAP_FOK(BMDSOAP_SERVER_INVALID_UPDATE_REASON);	}

	/************************************************/
	/*	poczatkowe ustalenie zwracanej wartosci	*/
	/************************************************/
	(*result)=-1;

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


	/******************************************/
	/*	przepisanie metadanych do tablic	*/
	/******************************************/
	SOAP_FOK(ConvertMtdsList(mtds, &oids_table, &values_table, &oids_count));


	/************************************/
	/*	stworzenie datagramu zadania	*/
	/************************************/
	retVal=createDatagram(	userClassId,
					oids_table,
					values_table,
					oids_count,
					BMD_DATAGRAM_TYPE_UPDATE,
					BMD_ONLY_GROUP_DOC,
					BMD_QUERY_EQUAL,
					&req_dtg_set);
	DestroyTableOfStrings(&oids_table, oids_count);
	DestroyTableOfStrings(&values_table, oids_count);
	if(retVal < BMD_OK)
		{ SOAP_FOK(retVal); }

	/************************************************/
	/*	umieszczenie powodu update'a w datagramie	*/
	/************************************************/
	req_dtg_set->bmdDatagramSetTable[0]->protocolData=(GenBuf_t*)calloc(1, sizeof(GenBuf_t));
	if(req_dtg_set->bmdDatagramSetTable[0]->protocolData == NULL)
	{
		bmd_datagramset_free(&req_dtg_set);
		SOAP_FOK(BMD_ERR_MEMORY);
	}
	req_dtg_set->bmdDatagramSetTable[0]->protocolData->buf=(char *)calloc(strlen(updateReason)+1, sizeof(char));
	if(req_dtg_set->bmdDatagramSetTable[0]->protocolData->buf == NULL)
	{
		bmd_datagramset_free(&req_dtg_set);
		SOAP_FOK(BMD_ERR_MEMORY);
	}
	memcpy(req_dtg_set->bmdDatagramSetTable[0]->protocolData->buf, updateReason, strlen(updateReason));
	req_dtg_set->bmdDatagramSetTable[0]->protocolData->size=strlen(updateReason);
	
	
	/************************************/
	/*	usatwienie id w datagramie	*/
	/************************************/
	retVal=bmd_datagram_set_id((long *)&id, &(req_dtg_set->bmdDatagramSetTable[0]));
	if(retVal < BMD_OK)
	{
		bmd_datagramset_free(&req_dtg_set);
		SOAP_FOK(retVal);
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
	if(retVal < BMD_OK)
	{
		bmd_datagramset_free(&req_dtg_set);
		SOAP_FOK(retVal);
	}


	/******************************/
	/*	wyslanie zadania do BMD	*/
	/******************************/
	retVal=sendRequestToBMD(bi, req_dtg_set, max_datagram_in_set_transmission, &res_dtg_set, 1, deserialisation_max_memory);
	bmd_logout(&bi, deserialisation_max_memory);
	bmd_info_destroy(&bi);
	bmd_end();
	bmd_datagramset_free(&req_dtg_set);
	bmd_datagramset_free(&res_dtg_set);
	SOAP_FOK(retVal);

	(*result)=0;

	PRINT_SOAP("Status:\t\trequest served correctly\n");
	return SOAP_OK;
}


int bmd230__bmdUpdateSystemMetadata(struct soap *soap,
				struct xsd__base64Binary *cert,
				char *userRoleId,
				char *userGroupId,
				char *userClassId,
				long id,
				struct bmd230__mtdsValues *mtds,
				long *result)
{
long iter								= 0;
long max_datagram_in_set_transmission	= 0;
long deserialisation_max_memory			= 0;

bmdDatagram_t* dtg					= NULL;
bmdDatagramSet_t *dtg_set			= NULL;
bmdDatagramSet_t *resp_dtg_set		= NULL;
bmd_info_t *bi						= NULL;

char *derUserCert=NULL;
long derUserCertLen=0;
long retVal=0;
char* paramCert=NULL;
long paramCertLen=0;

	PRINT_SOAP("Request:\tupdating system metadata values for file %li\n",id);
	
	if(id<=0)
		{ SOAP_FOK(BMDSOAP_SERVER_INVALID_FILE_ID); }
	if(mtds == NULL)
		{ SOAP_FOK(BMD_ERR_PARAM6); }
	if(mtds->__size <= 0 || mtds->__ptr == NULL)
		{ SOAP_FOK(BMD_ERR_PARAM6); }
	if(result == NULL)
		{ SOAP_FOK(BMD_ERR_PARAM7); }

	(*result)=-1;

	
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

	
	
	SOAP_FOK(bmd_datagram_create(BMD_DATAGRAM_TYPE_UPDATE_SYS_METADATA, &dtg));
	
	if(userClassId!=NULL)
	{
		retVal=bmd_datagram_add_metadata_char(OID_ACTION_METADATA_CERT_CHOSEN_CLASS_ID, userClassId, &dtg);
		if(retVal < BMD_OK)
		{
			bmd_datagram_free(&dtg);
			SOAP_FOK(retVal);
		}
	}
	
	retVal=bmd_datagram_set_id(&id,&dtg);
	if(retVal < BMD_OK)
	{
		bmd_datagram_free(&dtg);
		SOAP_FOK(retVal);
	}

	for(iter=0; iter<mtds->__size; iter++)
	{
		retVal=bmd_datagram_add_metadata_char(mtds->__ptr[iter].mtdOid, mtds->__ptr[iter].mtdValue, &dtg);
		if(retVal < BMD_OK)
		{
			bmd_datagram_free(&dtg);
			SOAP_FOK(retVal);
		}
	}
	
	retVal=bmd_datagram_add_to_set(dtg, &dtg_set);
	if(retVal < BMD_OK)
	{
		bmd_datagram_free(&dtg);
		SOAP_FOK(retVal);
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

	(*result)=resp_dtg_set->bmdDatagramSetTable[0]->datagramStatus;
	bmd_datagramset_free(&resp_dtg_set);
	bmd_end();

	SOAP_FOK(*result);
	

	PRINT_SOAP("Status:\t\trequest served correctly\n");
	return SOAP_OK;
}


/**********************************************************/
/* obsluga zadania wyslania podpisanego pliku do archiwum */
/**********************************************************/
int bmd230__bmdAddSignature(	struct soap *soap,
				struct xsd__base64Binary *cert,
				char *roleName,
				char *groupName,
				char *userClassId,
				struct xsd__base64Binary *signatureInput,
				long destFileId,
				long *result)
{
long max_datagram_in_set_transmission		= 0;
long max_metadata_value_length			= 0;
long deserialisation_max_memory			= 0;
bmd_info_t *bi					= NULL;
bmdDatagram_t *dtg				= NULL;
bmdDatagramSet_t *dtg_set			= NULL;
bmdDatagramSet_t *resp_dtg_set			= NULL;
GenBuf_t *signature				= NULL;
char *destFileIdStr				= NULL;

char *derUserCert=NULL;
long derUserCertLen=0;
long retVal=0;
char* paramCert=NULL;
long paramCertLen=0;

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

	PRINT_SOAP("Request:\tadd signature to document in archive\n");

	/************************/
	/* przygotowanie danych */
	/************************/
	signature = (GenBuf_t*)malloc(sizeof(GenBuf_t));
	signature->buf=(char *)signatureInput->__ptr;
	signature->size=signatureInput->__size;

	asprintf(&destFileIdStr, "%li", destFileId);
	if (destFileIdStr==NULL)	{	SOAP_FOK(NO_MEMORY); }

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

	/****************************/
	/* wykonanie zadania na BMD */
	/****************************/
	SOAP_FOK(PrepareDataForBMDLoginAndRequest(&bi,
			(_GLOBAL_soap_wssecurity_enabled == 1  ? derUserCert : paramCert),
			(_GLOBAL_soap_wssecurity_enabled == 1  ? derUserCertLen : paramCertLen),
			roleName, groupName, deserialisation_max_memory));
	free(derUserCert); derUserCert=NULL;
	paramCert=NULL; paramCertLen=0;

	SOAP_FOK_LOGOUT(bmd_datagram_create(BMD_DATAGRAM_ADD_SIGNATURE,&dtg));
	if (userClassId!=NULL)
	{
		SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(	OID_ACTION_METADATA_CERT_CHOSEN_CLASS_ID, userClassId, &dtg));
	}

	SOAP_FOK_LOGOUT(set_gen_buf2 (destFileIdStr, strlen(destFileIdStr), &(dtg->protocolDataFileId)));
	SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_2(OID_PKI_METADATA_SIGNATURE,signature,&dtg, 0));
	SOAP_FOK_LOGOUT(bmd_datagram_add_to_set(dtg,&dtg_set));

	/******************************/
	/*	wyslanie zadania do BMD	*/
	/******************************/
	SOAP_FOK_LOGOUT(sendRequestToBMD(bi, dtg_set, max_datagram_in_set_transmission, &resp_dtg_set, 1, deserialisation_max_memory));
	(*result)=0;

	/************/
	/* porzadki */
	/************/
	bmd_logout(&bi, deserialisation_max_memory);
	bmd_end();
	bmd_datagramset_free(&dtg_set);
	bmd_datagramset_free(&resp_dtg_set);
	free(destFileIdStr); destFileIdStr=NULL;

	PRINT_SOAP("Status:\t\trequest served correctly\n");

	return SOAP_OK;
}
