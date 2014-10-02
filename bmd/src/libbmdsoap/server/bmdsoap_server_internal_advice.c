#include <bmd/libbmdsoap/libbmdsoap.h>

/************************************************************************/
/*	obsluga zadania wysylki e-awizo dla wskazanego dokumentu	*/
/************************************************************************/
int bmd230__SendAdvice(	struct soap *soap,
			struct xsd__base64Binary *cert,
			char *userRoleId,
			char *userGroupId,
			char *userClassId,
			long int bmdID,
			int *returnCode)
{
bmd_info_t *bi				= NULL;
bmdDatagram_t *dtg			= NULL;
bmdDatagramSet_t *requestDatagramSet = NULL;
bmdDatagramSet_t *responseDatagramSet = NULL;
long max_datagram_in_set_transmission	= 0;
long deserialisation_max_memory		= 0;

char *derUserCert=NULL;
long derUserCertLen=0;
long retVal=0;
char* paramCert=NULL;
long paramCertLen=0;

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if(bmdID <= 0)
	{
		PRINT_ERROR("SOAPSERVERERR Invalid second parameter value\n");
		return soap_receiver_fault(soap,"Internal error","Invalid second parameter value");
	}
	if(returnCode == NULL)
	{
		PRINT_ERROR("SOAPSERVERERR Invalid third parameter value\n");
		return soap_receiver_fault(soap,"Internal error","Invalid third parameter value");
	}
	*returnCode = -1;

	PRINT_SOAP("Request:\tSend e-advice for document with id=%li\n", bmdID);

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

	/************************/
	/*	logowanie do bmd	*/
	/************************/
	SOAP_FOK(PrepareDataForBMDLoginAndRequest(&bi,
			(_GLOBAL_soap_wssecurity_enabled == 1  ? derUserCert : paramCert),
			(_GLOBAL_soap_wssecurity_enabled == 1  ? derUserCertLen : paramCertLen),
			userRoleId, userGroupId, deserialisation_max_memory));
	free(derUserCert); derUserCert=NULL;
	paramCert=NULL; paramCertLen=0;

	SOAP_FOK_LOGOUT(bmd_datagram_create(BMD_DATAGRAM_TYPE_SEND_AVIZO, &dtg));
	retVal=bmd_datagram_set_id((const long int *)&bmdID, &dtg);
	if(retVal < BMD_OK)
	{
		bmd_datagram_free(&dtg);
		SOAP_FOK_LOGOUT(retVal);
	}
	retVal=bmd_datagram_add_to_set(dtg, &requestDatagramSet);
	if(retVal < BMD_OK)
	{
		bmd_datagram_free(&dtg);
		SOAP_FOK_LOGOUT(retVal);
	}

	retVal=bmd_send_request(bi, requestDatagramSet, max_datagram_in_set_transmission, &responseDatagramSet, 1, deserialisation_max_memory);
	bmd_datagramset_free(&requestDatagramSet);
	bmd_logout(&bi, deserialisation_max_memory);
	bmd_info_destroy(&bi);
	bmd_end();
	
	SOAP_FOK(retVal);

	*returnCode=responseDatagramSet->bmdDatagramSetTable[0]->datagramStatus;
	bmd_datagramset_free(&responseDatagramSet);

	SOAP_FOK(*returnCode);

	PRINT_SOAP("Status:\t\trequest served correctly\n");
	return SOAP_OK;
}


int bmd230__SendAdviceWithParameters(struct soap *soap, struct xsd__base64Binary *cert,  char *userRoleId, char *userGroupId, char *userClassId,
					char* bmdID, char *accountId, char *invoiceId, char *userIdentifier, char* adviceTemplateName, char* adviceTitle, int *returnCode)
{
bmd_info_t *bi = NULL;
bmdDatagram_t *dtg = NULL;
bmdDatagramSet_t *requestDatagramSet = NULL;
bmdDatagramSet_t *responseDatagramSet = NULL;
long max_datagram_in_set_transmission = 0;
long deserialisation_max_memory = 0;

long bmdId_Long = 0;
char *search_oid1 = NULL;
char *search_oid2 = NULL;
char *search_oid3 = NULL;
char* SendAdviceWithParameters_select_type = NULL;
long select_operator = 0;

char *derUserCert=NULL;
long derUserCertLen=0;
long retVal=0;
char* paramCert=NULL;
long paramCertLen=0;


	PRINT_SOAP("Request:\tsend e-advice with parameters\n");

	if(bmdID == NULL && accountId == NULL && invoiceId == NULL && userIdentifier == NULL)
	{
		SOAP_FOK(BMDSOAP_NO_CRITERIA_SPECIFIED);
	}
	if(returnCode == NULL)
	{
		PRINT_ERROR("SOAPSERVERERR Invalid output parameter\n");
		return soap_receiver_fault(soap,"Internal error","Invalid output parameter");
	}
	*returnCode = -1;


	// wypisywanie informacji o wywolaniu
	if(bmdID != NULL)
	{
		PRINT_SOAP("bmdID:\t\t\t%s\n", bmdID);
		if(accountId != NULL)
			{ PRINT_SOAP("accountId ignored\n"); }
		if(invoiceId != NULL)
			{ PRINT_SOAP("invoiceId ignored\n"); }
		if(userIdentifier != NULL)
			{ PRINT_SOAP("userIdentifier ignored\n"); }
	}
	else
	{
		PRINT_SOAP("accountId:\t%s\n", accountId);
		PRINT_SOAP("invoiceId:\t%s\n", invoiceId);
		PRINT_SOAP("userIdentifier:\t%s\n", userIdentifier);
	}

	PRINT_SOAP("adviceTemplateName:\t%s\n", adviceTemplateName);
	PRINT_SOAP("adviceTitle:\t\t%s\n", adviceTitle);


	// odczyt ustawien
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


	// przygotowanie zadania
	SOAP_FOK_LOGOUT(bmd_datagram_create(BMD_DATAGRAM_TYPE_SEND_ADVICE_WITH_PARAMETERS, &dtg));

	if(bmdID != NULL) // jesli podano id dokumentu
	{
		bmdId_Long = strtol(bmdID, NULL, 10);
		if(bmdId_Long == LONG_MIN || bmdId_Long == LONG_MAX)
		{
			bmd_datagram_free(&dtg);
			SOAP_FOK_LOGOUT(BMD_ERR_PARAM5);
		}

		retVal = bmd_datagram_set_id(&bmdId_Long, &dtg);
		if(retVal < BMD_OK)
		{
			bmd_datagram_free(&dtg);
			SOAP_FOK_LOGOUT(retVal);
		}
	}
	else // jesli dokument opisany wartosciami metadanych
	{
		retVal = bmdconf_get_value(_GLOBAL_konfiguracja, "soap", "search_oid1", &search_oid1);
		if(retVal < BMD_OK)
		{
			bmd_datagram_free(&dtg);
			SOAP_FOK_LOGOUT(retVal);
		}
		retVal = bmdconf_get_value(_GLOBAL_konfiguracja, "soap", "search_oid2", &search_oid2);
		if(retVal < BMD_OK)
		{
			bmd_datagram_free(&dtg);
			free(search_oid1); search_oid1 = NULL;
			SOAP_FOK_LOGOUT(retVal);
		}
		bmdconf_get_value(_GLOBAL_konfiguracja, "soap", "search_oid3", &search_oid3); // ustawienie opcjonalne

		bmdconf_get_value(_GLOBAL_konfiguracja, "functionalities_behaviour", "SendAdviceWithParameters_select_type", &SendAdviceWithParameters_select_type); // ustawienie opcjonalne
		if(SendAdviceWithParameters_select_type == NULL) // domyslnie EQUAL
		{
			select_operator = BMD_QUERY_EQUAL;
		}
		else if(strcmp(SendAdviceWithParameters_select_type, "EQUAL") == 0)
		{
			select_operator = BMD_QUERY_EQUAL;
		}
		else if(strcmp(SendAdviceWithParameters_select_type, "EXACT") == 0)
		{
			select_operator = BMD_QUERY_EXACT_ILIKE;
		}
		else if(strcmp(SendAdviceWithParameters_select_type, "LIKE") == 0)
		{
			select_operator = BMD_QUERY_LIKE;
		}
		else if(strcmp(SendAdviceWithParameters_select_type, "ILIKE") == 0)
		{
			select_operator = BMD_QUERY_ILIKE;
		}
		else // kazdy inny (nierozpoznany) przypadek - domyslnie EQUAL
		{
			PRINT_SOAP("Unrecognized value \"%s\" of SendAdviceWithParameters_select_type option (using EQUAL by default)\n", SendAdviceWithParameters_select_type);
			select_operator = BMD_QUERY_EQUAL;
		}
		free(SendAdviceWithParameters_select_type);
		SendAdviceWithParameters_select_type = NULL;


		if(search_oid1 != NULL && accountId != NULL)
		{
			retVal = bmd_datagram_add_metadata_char(search_oid1, accountId, &dtg);
			if(retVal < BMD_OK)
			{
				bmd_datagram_free(&dtg);
				free(search_oid1); search_oid1 = NULL;
				free(search_oid2); search_oid2 = NULL;
				free(search_oid3); search_oid3 = NULL;
				SOAP_FOK_LOGOUT(retVal);
			}
		}
		if(search_oid2 != NULL && invoiceId != NULL)
		{
			retVal = bmd_datagram_add_metadata_char(search_oid2, invoiceId, &dtg);
			if(retVal < BMD_OK)
			{
				bmd_datagram_free(&dtg);
				free(search_oid1); search_oid1 = NULL;
				free(search_oid2); search_oid2 = NULL;
				free(search_oid3); search_oid3 = NULL;
				SOAP_FOK_LOGOUT(retVal);
			}
		}
		if(search_oid3 != NULL && userIdentifier != NULL)
		{
			retVal = bmd_datagram_add_metadata_char(search_oid3, userIdentifier, &dtg);
			if(retVal < BMD_OK)
			{
				bmd_datagram_free(&dtg);
				free(search_oid1); search_oid1 = NULL;
				free(search_oid2); search_oid2 = NULL;
				free(search_oid3); search_oid3 = NULL;
				SOAP_FOK_LOGOUT(retVal);
			}
		}
		free(search_oid1); search_oid1 = NULL;
		free(search_oid2); search_oid2 = NULL;
		free(search_oid3); search_oid3 = NULL;

		retVal = bmd_datagram_set_status(BMD_ONLY_GROUP_DOC, select_operator, &dtg);
		if(retVal < BMD_OK)
		{
			bmd_datagram_free(&dtg);
			SOAP_FOK_LOGOUT(retVal);
		}
	}

	if(adviceTemplateName != NULL)
	{
		retVal = bmd_datagram_add_metadata_char(OID_SYS_METADATA_SEND_ADVICE_PARAM_TEMPLATE_NAME, adviceTemplateName, &dtg);
		if(retVal < BMD_OK)
		{
			bmd_datagram_free(&dtg);
			SOAP_FOK_LOGOUT(retVal);
		}
	}

	if(adviceTitle != NULL)
	{
		retVal = bmd_datagram_add_metadata_char(OID_SYS_METADATA_SEND_ADVICE_PARAM_TITLE, adviceTitle, &dtg);
		if(retVal < BMD_OK)
		{
			bmd_datagram_free(&dtg);
			SOAP_FOK_LOGOUT(retVal);
		}
	}
	
	retVal = bmd_datagram_add_to_set(dtg, &requestDatagramSet);
	if(retVal < BMD_OK)
	{
		bmd_datagram_free(&dtg);
		SOAP_FOK_LOGOUT(retVal);
	}

	// wyslanie zadania bmd
	retVal = bmd_send_request(bi, requestDatagramSet, max_datagram_in_set_transmission, &responseDatagramSet, 1, deserialisation_max_memory);
	bmd_datagramset_free(&requestDatagramSet);
	bmd_logout(&bi, deserialisation_max_memory); // wylogowanie z bmd
	bmd_info_destroy(&bi);
	bmd_end();
	
	SOAP_FOK(retVal);

	if(responseDatagramSet != NULL && responseDatagramSet->bmdDatagramSetSize > 0 && responseDatagramSet->bmdDatagramSetTable != NULL && responseDatagramSet->bmdDatagramSetTable[0] != NULL)
	{
		*returnCode = responseDatagramSet->bmdDatagramSetTable[0]->datagramStatus;
	}
	else
	{
		bmd_datagramset_free(&responseDatagramSet);
		SOAP_FOK(BMDSOAP_SERVER_INTERNAL_ERROR);
	}
	bmd_datagramset_free(&responseDatagramSet);

	SOAP_FOK(*returnCode);

	PRINT_SOAP("Status:\t\trequest served correctly\n");
	return SOAP_OK;
}
