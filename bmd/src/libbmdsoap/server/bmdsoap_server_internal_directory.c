#include <bmd/libbmdsoap/libbmdsoap.h>
#include <bmd/libbmdglobals/libbmdglobals.h>

/******************************************************/
/*	obsluga zadania wyslania pliku do archiwum	*/
/******************************************************/
int bmd230__bmdCreateDirectory(	struct soap *soap,
				struct xsd__base64Binary *cert,
				char *roleName,
				char *groupName,
				char *userClassId,
				struct bmd230__mtdsValues *mtds,
				char *description,
				char *dirName,
				char *transactionId,
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

char *derUserCert=NULL;
long derUserCertLen=0;
long retVal=0;
char* paramCert=NULL;
long paramCertLen=0;

	/******************************/
	/*	walidacja parametrow      */
	/******************************/
	(*id)=-1;

	PRINT_SOAP("Request:\tcreate directory in archive\n");

	/***************************************/
	/* wczytanie ustawien konfiguracyjnych */
	/***************************************/
	if (bmdconf_get_value_int(_GLOBAL_konfiguracja,"soap","max_metadata_value_length",&max_metadata_value_length)<BMD_OK)
	{
		max_metadata_value_length=256;
	}

	bmdconf_get_value_int(_GLOBAL_konfiguracja,"bmd","max_datagram_in_set_transmission",&max_datagram_in_set_transmission);
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
	SOAP_FOK_LOGOUT(bmd_datagram_create(BMD_DATAGRAM_DIR_CREATE,&dtg));
	if (userClassId!=NULL)
	{
		SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(	OID_ACTION_METADATA_CERT_CHOSEN_CLASS_ID, userClassId, &dtg));
	}
	SOAP_FOK_LOGOUT(bmd_datagram_set_filename(dirName, &dtg));

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
				SOAP_FOK_LOGOUT(BMDSOAP_SERVER_METADATA_VALUE_TOO_LONG);
			}

			SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_2((char *)mtds->__ptr[i].mtdOid,&tmp_gb1,&dtg, 0));
		}
	}

	if (transactionId!=NULL)
	{
		tmp_gb1.buf=(char *)transactionId;
		tmp_gb1.size=strlen(transactionId);

		SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_2((char *)OID_SYS_METADATA_CRYPTO_OBJECTS_TRANSACTION_ID,&tmp_gb1,&dtg, 0));
	}

	/*****************************/
	/* wstawienie opisu katalogu */
	/*****************************/
	if (description!=NULL)
	{
		set_gen_buf2( (char *)description, strlen(description), &(dtg->protocolData));
	}

	SOAP_FOK_LOGOUT(bmd_datagram_add_to_set(dtg,&dtg_set));

	/************************/
	/*	wyslanie zadania	*/
	/************************/
	SOAP_FOK_LOGOUT(bmd_send_request(bi,dtg_set, max_datagram_in_set_transmission, &resp_dtg_set, 1, deserialisation_max_memory));

	/******************************/
	/*	odebranie odpowiedzi	*/
	/******************************/
	SOAP_FOK_LOGOUT(resp_dtg_set->bmdDatagramSetTable[0]->datagramStatus);


	if (resp_dtg_set->bmdDatagramSetTable[0]->protocolDataFileId->buf!=NULL)
	{
		(*id)=atol((char *)resp_dtg_set->bmdDatagramSetTable[0]->protocolDataFileId->buf);
	}
	else
	{
		(*id)=-1;
	}


	/*************************************************/
	/* oznaczenie w transakcji zakończenia transferu */
	/*************************************************/
	if(transactionId != NULL)
	{
		SOAP_FOK(twfun_unmark_start_of_insert(	transaction_dir_name, transactionId, -1));
		free(transaction_dir_name); transaction_dir_name = NULL;
	}

	bmd_logout(&bi, deserialisation_max_memory);
	bmd_datagramset_free(&dtg_set);
	bmd_datagramset_free(&resp_dtg_set);
	bmd_end();

	PRINT_SOAP("Status:\t\trequest served correctly\n");
	return SOAP_OK;
}

/******************************************************/
/*	obsluga zadania pobrania pliku o okreslonym id	*/
/******************************************************/
int bmd230__bmdGetDirectoryDetails(	struct soap *soap,
				struct xsd__base64Binary *cert,
				char *roleName,
				char *groupName,
				char *userClassId,
				long id,
				struct bmd230__fileInfo **output)
{
long size				= 0;
char *buf				= NULL;
bmd_info_t *bi				= NULL;
lob_request_info_t *li			= NULL;
bmdDatagram_t *dtg			= NULL;
bmdDatagramSet_t *dtg_set		= NULL;
bmdDatagramSet_t *resp_dtg_set		= NULL;
GenBuf_t tmp_gb;
char *path				= NULL;
long max_datagram_in_set_transmission	= 0;
long deserialisation_max_memory		= 0;

char *derUserCert=NULL;
long derUserCertLen=0;
long retVal=0;
char* paramCert=NULL;
long paramCertLen=0;


	PRINT_SOAP("Request:\timport directory details with given id\n");
	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if(id<=0)		{	SOAP_FOK(BMDSOAP_SERVER_INVALID_FILE_ID);	}

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
			roleName, groupName, deserialisation_max_memory));
	free(derUserCert); derUserCert=NULL;
	paramCert=NULL; paramCertLen=0;

	/******************************/
	/*	stworzenie datagramu	*/
	/******************************/
	SOAP_FOK_LOGOUT(bmd_datagram_create(BMD_DATAGRAM_DIR_GET_DETAIL,&dtg));
	if (userClassId!=NULL)
	{
		SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(	OID_ACTION_METADATA_CERT_CHOSEN_CLASS_ID, userClassId, &dtg));
	}

	tmp_gb.buf=(char *)"dir";
	tmp_gb.size=strlen("dir");
	SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_2_with_params(OID_SYS_METADATA_CRYPTO_OBJECTS_FILE_TYPE,&tmp_gb,&dtg,UNDEFINED_METADATA,BMD_REQ_DB_SELECT_EQUAL_QUERY,0,0, 0));

	/************************************/
	/*	usatwienie id w datagramie	*/
	/************************************/
	SOAP_FOK_LOGOUT(bmd_datagram_set_id((const long *)&id,&dtg));

	/************************************************/
	/*	dodanie datagramu do zestawu datagramow	*/
	/************************************************/
	SOAP_FOK_LOGOUT(bmd_datagram_add_to_set(dtg,&dtg_set));

	/************************************/
	/*	wyslanie zadania lobowego	*/
	/************************************/
	SOAP_FOK_LOGOUT(bmd_send_request(bi,dtg_set, max_datagram_in_set_transmission, &resp_dtg_set,1, deserialisation_max_memory));
	SOAP_FOK_LOGOUT(resp_dtg_set->bmdDatagramSetTable[0]->datagramStatus);

	/************************/
	/*	ustawienie wyniku	*/
	/************************/
	(*output)=(struct bmd230__fileInfo *)soap_malloc(soap,sizeof(struct bmd230__fileInfo));
	memset(*output,0,sizeof(struct bmd230__fileInfo));

	/*	ustawienie nazwy pliku	*/
	(*output)->filename=(char *)soap_malloc(soap,resp_dtg_set->bmdDatagramSetTable[0]->protocolDataFilename->size+2);
	memset((*output)->filename, 0, resp_dtg_set->bmdDatagramSetTable[0]->protocolDataFilename->size+1);
	memmove((*output)->filename, resp_dtg_set->bmdDatagramSetTable[0]->protocolDataFilename->buf, resp_dtg_set->bmdDatagramSetTable[0]->protocolDataFilename->size);

	/*	ustawienie pliku	*/
	(*output)->file=(struct bmd230__myBinaryDataType *)soap_malloc(soap,sizeof(struct bmd230__myBinaryDataType));
	(*output)->file->id = NULL;


	bmd_genbuf_get_buf(resp_dtg_set->bmdDatagramSetTable[0]->protocolData,&buf);
	bmd_genbuf_get_size(resp_dtg_set->bmdDatagramSetTable[0]->protocolData,&size);
	(*output)->file->__ptr=(unsigned char *)soap_malloc(soap,size+2);
	memset((*output)->file->__ptr,0,size+1);
	(*output)->file->__size=size;
	memmove((*output)->file->__ptr,buf,size);
	(*output)->file->type = NULL;
	(*output)->file->options = NULL;


	/******************/
	/*	porzadki  */
	/******************/
	bmd_logout(&bi, deserialisation_max_memory);
	bmd_info_destroy(&bi);
	bmd_end();

	unlink(path);
	free(path); path=NULL;
	bmd_datagramset_free(&dtg_set);
	bmd_datagramset_free(&resp_dtg_set);
	bmd_lob_request_info_destroy(&li);

	PRINT_SOAP("Status:\t\tRequest served correctly\n");
	return SOAP_OK;
}


int bmd230__bmdDeleteDirByID(	struct soap *soap,
				struct xsd__base64Binary *binaryCert,
				char *userClassId,
				char *roleName,
				char *groupName,
				long id,
				int *result)
{
bmd_info_t *bi				= NULL;
bmdDatagram_t *dtg			= NULL;
bmdDatagramSet_t *dtg_set		= NULL;
bmdDatagramSet_t *resp_dtg_set		= NULL;
lob_request_info_t *li			= NULL;
long max_datagram_in_set_transmission	= 0;
long deserialisation_max_memory		= 0;

char *derUserCert=NULL;
long derUserCertLen=0;
long retVal=0;
char* paramCert=NULL;
long paramCertLen=0;

	(*result=-1);
	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if (id==0)
	{
		PRINT_ERROR("SOAPSERVERERR Invalid second parameter value\n");
		return soap_receiver_fault(soap,"Internal error","Invalid second parameter value");
	}

	PRINT_SOAP("Request:\tdelete file with id=%li\n",id);

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
			if(binaryCert == NULL)
				{ SOAP_FOK(BMDSOAP_SERVER_EMPTY_LOGIN_CERTIFICATE); }
			else
			{
				paramCert=(char*)binaryCert->__ptr;
				paramCertLen=(long)binaryCert->__size;
			}
		}
		else
		{
			if(binaryCert != NULL)
			{
				paramCert=(char*)binaryCert->__ptr;
				paramCertLen=(long)binaryCert->__size;
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

	/******************************/
	/*	stworzenie zadania	*/
	/******************************/
	SOAP_FOK_LOGOUT(bmd_lob_request_info_create(&li));
	SOAP_FOK_LOGOUT(bmd_datagram_create(BMD_DATAGRAM_DIR_DELETE,&dtg));

	SOAP_FOK_LOGOUT(bmd_datagram_set_id((const long *)&id,&dtg));
	SOAP_FOK_LOGOUT(bmd_datagram_add_to_set(dtg,&dtg_set));

	/************************/
	/*	wyslanie zadania	*/
	/************************/
	SOAP_FOK_LOGOUT(bmd_send_request(bi,dtg_set, max_datagram_in_set_transmission, &resp_dtg_set,1, deserialisation_max_memory));

	/******************************/
	/*	odebranie odpowiedzi	*/
	/******************************/
	SOAP_FOK_LOGOUT(resp_dtg_set->bmdDatagramSetTable[0]->datagramStatus);

	/******************/
	/*	porzadki	*/
	/******************/
	bmd_logout(&bi, deserialisation_max_memory);
	bmd_info_destroy(&bi);
	bmd_end();

	bmd_datagramset_free(&dtg_set);
	bmd_datagramset_free(&resp_dtg_set);

	(*result)=BMD_OK;
	PRINT_SOAP("Status:\t\trequest served correctly\n");
	return SOAP_OK;
}
