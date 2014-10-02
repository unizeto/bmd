#include <bmd/libbmdsoap/libbmdsoap.h>
#include <bmd/libbmdglobals/libbmdglobals.h>

/*************************************************************/
/* obsluga zadania konserwacji wartosci dowodowej w archiwum */
/*************************************************************/
int bmd230__bmdConservateTimestamps(	struct soap *soap,
					struct xsd__base64Binary *cert,
					char *roleName,
					char *groupName,
					char *userClassId,
					struct bmd230__mtdsSearchValues *mtds,
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
long tmpOperator			= 0;
long tmpDesc				= 0;
long tmpAdjustment			= 0;
long tmpStatement			= 0;
long mtdPrefix				= 0;
long mtdPostfix				= 0;

GenBuf_t tmp_gb;

char *derUserCert=NULL;
long derUserCertLen=0;
long retVal=0;
char* paramCert=NULL;
long paramCertLen=0;

	PRINT_SOAP("Request:\tconservate timestamps\n");

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
	SOAP_FOK_LOGOUT(bmd_datagram_create(BMD_DATAGRAM_TYPE_CONSERVATE_TIMESTAMPS,&dtg));
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
					tmp_gb.buf=(char *)mtds->__ptr[i].mtdValue;
					tmp_gb.size=strlen(mtds->__ptr[i].mtdValue);
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
				}
			}
		}
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

	(*result)=BMD_OK;

	/***************/
	/* wylogowanie */
	/***************/
	PRINT_SOAP("Status:\t\trequest served correctly\n");
	bmd_datagramset_free(&resp_dtg_set);
	bmd_logout(&bi, deserialisation_max_memory);
	bmd_end();

	return SOAP_OK;
}


int bmd230__bmdDeleteReplacedLobs(	struct soap *soap,
					struct xsd__base64Binary *cert,
					char *userRoleId,
					char *userGroupId,
					char *userClassId,
					long *deletedLobsCount)
{
long max_datagram_in_set_transmission		= 0;
long deserialisation_max_memory				= 0;
bmd_info_t *bi								= NULL;

bmdDatagram_t *requestDatagram				= NULL;
bmdDatagramSet_t *requestSet				= NULL;
bmdDatagramSet_t *responseSet				= NULL;

char *derUserCert=NULL;
long derUserCertLen=0;
long retVal=0;
char* paramCert=NULL;
long paramCertLen=0;

	PRINT_SOAP("Request:\tdelete replaced lobs\n");
	
	if(deletedLobsCount == NULL)
		{ SOAP_FOK(BMD_ERR_PARAM4); }
	*deletedLobsCount = 0;
	
	// wczytanie ustawien konfiguracyjnych
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

	// logowanie do bmd
 	SOAP_FOK(PrepareDataForBMDLoginAndRequest(&bi,
			(_GLOBAL_soap_wssecurity_enabled == 1  ? derUserCert : paramCert),
			(_GLOBAL_soap_wssecurity_enabled == 1  ? derUserCertLen : paramCertLen),
			userRoleId, userGroupId, deserialisation_max_memory));
	free(derUserCert); derUserCert=NULL;
	paramCert=NULL; paramCertLen=0;

	SOAP_FOK_LOGOUT(bmd_datagram_create(BMD_DATAGRAM_TYPE_DELETE_REPLACED_LOBS, &requestDatagram));
	if(userClassId!=NULL)
	{
		retVal=bmd_datagram_add_metadata_char(OID_ACTION_METADATA_CERT_CHOSEN_CLASS_ID, userClassId, &requestDatagram);
		if(retVal < BMD_OK)
		{
			bmd_datagram_free(&requestDatagram);
			SOAP_FOK_LOGOUT(retVal);
		}
	}
	
	retVal=bmd_datagram_add_to_set(requestDatagram, &requestSet);
	if(retVal < BMD_OK)
	{
		bmd_datagram_free(&requestDatagram);
		SOAP_FOK_LOGOUT(retVal);
	}
	
	// wyslanie zadania
	retVal=bmd_send_request(bi, requestSet, max_datagram_in_set_transmission, &responseSet, 1, deserialisation_max_memory);
	bmd_logout(&bi, deserialisation_max_memory);
	bmd_info_destroy(&bi);
	bmd_datagramset_free(&requestSet);
	SOAP_FOK(retVal);
	
	if(responseSet->bmdDatagramSetSize != 1 || responseSet->bmdDatagramSetTable == NULL || responseSet->bmdDatagramSetTable[0] == NULL)
	{
		bmd_datagramset_free(&responseSet);
		SOAP_FOK(BMD_ERR_OP_FAILED);
	}
	
	if(responseSet->bmdDatagramSetTable[0]->protocolData != NULL && responseSet->bmdDatagramSetTable[0]->protocolData->size > 0 && responseSet->bmdDatagramSetTable[0]->protocolData->buf != NULL)
	{
		*deletedLobsCount = strtol(responseSet->bmdDatagramSetTable[0]->protocolData->buf, NULL, 10);
	}
	
	bmd_datagramset_free(&responseSet);
	bmd_end();

	PRINT_SOAP("Status:\t\trequest served correctly\n");	
	return SOAP_OK;
}

