#include <bmd/libbmdsoap/libbmdsoap.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>
#include <bmd/libbmdarchpack/libbmdarchpack.h>
#include <bmd/libbmdglobals/libbmdglobals.h>

/************************************************************************/
/* Obslugha sygnalu SIGUSR zabijajacego proces pobierania strumieniowgo */
/************************************************************************/
void kill_me_get(int status)
{
	PRINT_INFO("SOAPSERVERINF Killing get process.\n");
	/*
	if(_GLOBAL_current_dir[0] != 0)
	{
		PRINT_VDEBUG("SOAPSERVERVDEBUG current_dir: %s\n", _GLOBAL_current_dir);
		PRINT_VDEBUG("SOAPSERVERVDEBUG tmp_location: %s\n", twg_tmp_location);
		if(chdir(_GLOBAL_current_dir) == 0 && twg_tmp_location != NULL)
		{
			remove_dir(twg_tmp_location);
			PRINT_VDEBUG("SOAPSERVERVDEBUG %s directory removed by signal handler\n", twg_tmp_location);
			twg_tmp_location = NULL;
		}
	}
	if(twg_file_lock_name != NULL)
	{
		unlink(twg_file_lock_name);
		PRINT_VDEBUG("SOAPSERVERVDEBUG %s file removed by signal handler\n", twg_file_lock_name);
		twg_file_lock_name = NULL;
	}
	*/
	exit(1);
}

/**************************************************/
/* obsluga zadania pobrania pliku o okreslonym id */
/**************************************************/
int bmd230__bmdGetFileById(	struct soap *soap,
			struct xsd__base64Binary *cert,
			char *roleName,
			char *groupName,
			char *userClassId,
			long id,
			enum trans_t type,
			char* parameter,
			struct bmd230__fileInfo **output)
{
long size				= 0;
long max_datagram_in_set_transmission	= 0;
long deserialisation_max_memory		= 0;
char *buf				= NULL;
bmd_info_t *bi				= NULL;
lob_request_info_t *li			= NULL;
bmdDatagram_t *dtg			= NULL;
bmdDatagramSet_t *dtg_set		= NULL;
bmdDatagramSet_t *resp_dtg_set		= NULL;
GenBuf_t *gb				= NULL;
char *path				= NULL;
FILE* fd 				= NULL;
char* get_parameter_oid			= NULL;

char *derUserCert=NULL;
long derUserCertLen=0;
long retVal=0;
char* paramCert=NULL;
long paramCertLen=0;

	PRINT_SOAP("Request:\timport file with given id\n");
	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if(id<=0)		{	SOAP_FOK(BMDSOAP_SERVER_INVALID_FILE_ID);	}
	if(parameter != NULL)
	{
		bmdconf_get_value(_GLOBAL_konfiguracja, "soap", "get_parameter_oid", &get_parameter_oid);
		if(get_parameter_oid == NULL)
		{
			PRINT_ERROR("ERROR There have to be specified get_parameter_oid option to use parameter in bmdGetFileById request\n");
			SOAP_FOK(BMDSOAP_SERVER_INTERNAL_OPERATION_ERROR);
		}
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


	asprintf(&path,"soap_get_file_by_id_%li.tmp",(long)getpid());

	SOAP_FOK_LOGOUT(bmd_lob_request_info_add_filename(path, &li, BMD_RECV_LOB,1000000000));

	/******************************/
	/*	stworzenie datagramu	*/
	/******************************/
	SOAP_FOK_LOGOUT(bmd_datagram_create(BMD_DATAGRAM_GET_CGI_LO,&dtg));
	if (userClassId!=NULL)
	{
		SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(	OID_ACTION_METADATA_CERT_CHOSEN_CLASS_ID, userClassId, &dtg));
	}

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
	SOAP_FOK_LOGOUT(bmd_send_request_lo(bi, dtg_set, max_datagram_in_set_transmission, &resp_dtg_set, li, deserialisation_max_memory));
	
	SOAP_FOK_LOGOUT(resp_dtg_set->bmdDatagramSetTable[0]->datagramStatus);

	/************************/
	/*	ustawienie wyniku	*/
	/************************/
	(*output)=(struct bmd230__fileInfo *)soap_malloc(soap,sizeof(struct bmd230__fileInfo));
	if(*output == NULL)
	{
		SOAP_FOK_LOGOUT(NO_MEMORY);
	}
	memset(*output,0,sizeof(struct bmd230__fileInfo));

	/*	ustawienie nazwy pliku	*/
	(*output)->filename=(char *)soap_malloc(soap,(resp_dtg_set->bmdDatagramSetTable[0]->protocolDataFilename->size+1) * sizeof(char));
	if((*output)->filename == NULL)
	{
			SOAP_FOK_LOGOUT(NO_MEMORY);
	}
	memset((*output)->filename, 0, (resp_dtg_set->bmdDatagramSetTable[0]->protocolDataFilename->size+1) * sizeof(char));
	memcpy((*output)->filename, resp_dtg_set->bmdDatagramSetTable[0]->protocolDataFilename->buf, resp_dtg_set->bmdDatagramSetTable[0]->protocolDataFilename->size);

	/*	ustawienie pliku	*/
	(*output)->file=(struct bmd230__myBinaryDataType *)soap_malloc(soap,sizeof(struct bmd230__myBinaryDataType));
	if((*output)->file == NULL)
	{
		SOAP_FOK_LOGOUT(NO_MEMORY);
	}
	memset((*output)->file, 0, sizeof(struct bmd230__myBinaryDataType));


	switch(type)
	{

	    case 0 : {
			SOAP_FOK_LOGOUT(bmd_load_binary_content(path,&gb));
			SOAP_FOK_LOGOUT(bmd_genbuf_get_buf(gb,&buf));
			SOAP_FOK_LOGOUT(bmd_genbuf_get_size(gb,&size));
			(*output)->file->__size=size;
			(*output)->file->__ptr=(unsigned char *)soap_malloc(soap,((*output)->file->__size) * sizeof(unsigned char));
			if((*output)->file->__ptr == NULL)
			{
				(*output)->file->__size=0;
				free_gen_buf(&gb);
				SOAP_FOK_LOGOUT(NO_MEMORY);
			}
			memset((*output)->file->__ptr,0,((*output)->file->__size) * sizeof(unsigned char));
			memcpy((*output)->file->__ptr, buf, ((*output)->file->__size) * sizeof(unsigned char));
			buf=NULL; size=0;
			free_gen_buf(&gb);
			(*output)->file->type = NULL;
			(*output)->file->options = NULL;
						
			break;
		     }

	    case 1 : {  /* Transmisja strumieniowa DIME */
			soap_set_omode(soap, SOAP_IO_CHUNK);
			fd = fopen(path,"r");
			if(fd == NULL)
			{
				SOAP_FOK_LOGOUT(BMD_ERR_OPEN_FILE);
			}
			(*output)->file->__ptr=(unsigned char *)fd;
			(*output)->file->__size=0;
			(*output)->file->type = soap_malloc(soap, 1*sizeof(char));
			memset((*output)->file->type, 0 , 1*sizeof(char));
			(*output)->file->options = soap_dime_option(soap,0, (*output)->filename );

			break;
		     }

	}


	// aktualizacja metadanych - wartosci parametru
	if(parameter != NULL)
	{
		long retVal = 0;
		const char* defaultUpdateReason = "Parameter's update";
		const char* confUpdateReason = NULL;
		bmdDatagram_t* metadataUpdateRequest = NULL;
		bmdDatagramSet_t* metadataUpdateRequestSet = NULL;
		bmdDatagramSet_t* metadataUpdateResponseSet = NULL;

		retVal = bmd_datagram_create(BMD_DATAGRAM_TYPE_UPDATE, &metadataUpdateRequest);
		SOAP_FOK_LOGOUT(retVal);

		retVal = bmd_datagram_set_id(&id, &metadataUpdateRequest);
		if(retVal < BMD_OK)
		{
			bmd_datagram_free(&metadataUpdateRequest);
			SOAP_FOK_LOGOUT(retVal);
		}
		
		retVal = bmd_datagram_add_metadata_char(get_parameter_oid, parameter, &metadataUpdateRequest);
		if(retVal < BMD_OK)
		{
			bmd_datagram_free(&metadataUpdateRequest);
			SOAP_FOK_LOGOUT(retVal);
		}
	
		// ustawienie opisu aktualizacji
		confUpdateReason = bmdconf_get_value_static(_GLOBAL_konfiguracja,"soap","get_parameter_update_reason");
		if(confUpdateReason == NULL)
		{
			retVal=set_gen_buf2(defaultUpdateReason, strlen(defaultUpdateReason), &(metadataUpdateRequest->protocolData) );
		}
		else
		{
			retVal=set_gen_buf2(confUpdateReason, strlen(confUpdateReason), &(metadataUpdateRequest->protocolData) );
		}
		confUpdateReason = NULL;
		if(retVal < BMD_OK)
		{
			bmd_datagram_free(&metadataUpdateRequest);
			SOAP_FOK_LOGOUT(retVal);
		}
	
		retVal = bmd_datagram_add_to_set(metadataUpdateRequest, &metadataUpdateRequestSet);
		if(retVal < BMD_OK)
		{
			bmd_datagram_free(&metadataUpdateRequest);
			SOAP_FOK_LOGOUT(retVal);
		}
	
		retVal = bmd_send_request(bi, metadataUpdateRequestSet, max_datagram_in_set_transmission, &metadataUpdateResponseSet, 1, deserialisation_max_memory);
		bmd_datagramset_free(&metadataUpdateRequestSet);
		SOAP_FOK_LOGOUT(retVal);

		retVal = metadataUpdateResponseSet->bmdDatagramSetTable[0]->datagramStatus;
		bmd_datagramset_free(&metadataUpdateResponseSet);
		SOAP_FOK_LOGOUT(retVal);
	}

	bmd_logout(&bi, deserialisation_max_memory);
	bmd_info_destroy(&bi);
	bmd_end();

	/******************/
	/*	porzadki  */
	/******************/
	

	unlink(path);
	free(path); path=NULL;
	bmd_datagramset_free(&dtg_set);
	bmd_datagramset_free(&resp_dtg_set);
	bmd_lob_request_info_destroy(&li);

	PRINT_SOAP("Status:\t\tRequest served correctly\n");
	return SOAP_OK;
}


int bmd230__bmdGetArchiveFileVersion(	struct soap *soap,
			struct xsd__base64Binary *cert,
			char *userRoleId,
			char *userGroupId,
			char *userClassId,
			long currentVersionFileId,
			long archiveVersionFileId,
			enum trans_t type,
			struct bmd230__fileInfo **output)
{
long max_datagram_in_set_transmission	= 0;
long deserialisation_max_memory			= 0;

bmd_info_t *bi						= NULL;
lob_request_info_t *li				= NULL;
bmdDatagram_t *dtg					= NULL;
bmdDatagramSet_t *dtg_set			= NULL;
bmdDatagramSet_t *resp_dtg_set		= NULL;
GenBuf_t *gb						= NULL;
char *path							= NULL;
FILE* fd							= NULL;
char* currentVersionIdString		= NULL;
struct bmd230__fileInfo* tempOutput		= NULL;

char *derUserCert=NULL;
long derUserCertLen=0;
long retVal=0;
char* paramCert=NULL;
long paramCertLen=0;

	PRINT_SOAP("Request:\timport archive file version with given id\n");
	
	if(currentVersionFileId <= 0)
		{ SOAP_FOK(BMDSOAP_SERVER_INVALID_FILE_ID); }
	if(archiveVersionFileId <= 0)
		{ SOAP_FOK(BMDSOAP_SERVER_INVALID_ARCHIVE_FILE_ID);	 }
	if(output == NULL)
		{ SOAP_FOK(BMD_ERR_PARAM8); }
	

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

	
	SOAP_FOK(bmd_lob_request_info_create(&li));

	asprintf(&path,"soap_get_archive_file_%li.tmp",(long)getpid());

	retVal=bmd_lob_request_info_add_filename(path, &li, BMD_RECV_LOB,1000000000);
	if(retVal < BMD_OK)
	{
		free(path);
		bmd_lob_request_info_destroy(&li);
		SOAP_FOK(retVal);
	}

	retVal=bmd_datagram_create(BMD_DATAGRAM_TYPE_GET_ARCHIVE_FILE_VERSION_LO,&dtg);
	if(retVal < BMD_OK)
	{
		free(path);
		bmd_lob_request_info_destroy(&li);
		SOAP_FOK(retVal);
	}
	
	if(userClassId!=NULL)
	{
		retVal=bmd_datagram_add_metadata_char(OID_ACTION_METADATA_CERT_CHOSEN_CLASS_ID, userClassId, &dtg);
		if(retVal < BMD_OK)
		{
			free(path);
			bmd_datagram_free(&dtg);
			bmd_lob_request_info_destroy(&li);
			SOAP_FOK(retVal);
		}
	}

	// usatwienie id pobieranej wersji archiwalnej pliku
	retVal=bmd_datagram_set_id((const long *)&archiveVersionFileId,&dtg);
	if(retVal < BMD_OK)
	{
		free(path);
		bmd_datagram_free(&dtg);
		bmd_lob_request_info_destroy(&li);
		SOAP_FOK(retVal);
	}
		
	// usatwienie id aktualnej wersji (lub linku do niej) w datagramie
	asprintf(&currentVersionIdString, "%li", currentVersionFileId);
	if(currentVersionIdString == NULL)
	{
		free(path);
		bmd_datagram_free(&dtg);
		bmd_lob_request_info_destroy(&li);
		SOAP_FOK(NO_MEMORY);
	}
	
	retVal=bmd_datagram_add_metadata_char(OID_SYS_METADATA_CRYPTO_OBJECTS_ID, currentVersionIdString, &dtg);
	free(currentVersionIdString); currentVersionIdString=NULL;
	if(retVal < BMD_OK)
	{
		free(path);
		bmd_datagram_free(&dtg);
		bmd_lob_request_info_destroy(&li);
		SOAP_FOK(retVal);
	}

	retVal=bmd_datagram_add_to_set(dtg, &dtg_set);
	if(retVal < BMD_OK)
	{
		free(path);
		bmd_datagram_free(&dtg);
		bmd_lob_request_info_destroy(&li);
		SOAP_FOK(retVal);
	}

	// logowanie do bmd
	retVal=PrepareDataForBMDLoginAndRequest(&bi,
			(_GLOBAL_soap_wssecurity_enabled == 1  ? derUserCert : paramCert),
			(_GLOBAL_soap_wssecurity_enabled == 1  ? derUserCertLen : paramCertLen),
			userRoleId, userGroupId, deserialisation_max_memory);
	free(derUserCert); derUserCert=NULL;
	paramCert=NULL; paramCertLen=0;
	if(retVal < BMD_OK)
	{
		free(path);
		bmd_datagramset_free(&dtg_set);
		bmd_lob_request_info_destroy(&li);
		SOAP_FOK(retVal);
	}

	// wyslanie zadania lobowego
	retVal=bmd_send_request_lo(bi, dtg_set, max_datagram_in_set_transmission, &resp_dtg_set, li, deserialisation_max_memory);
	bmd_logout(&bi, deserialisation_max_memory);
	bmd_info_destroy(&bi);
	bmd_datagramset_free(&dtg_set);
	bmd_lob_request_info_destroy(&li);
	bmd_end();
	if(retVal < BMD_OK)
	{
		unlink(path);
		free(path);
		SOAP_FOK(retVal);
	}
	
	if( (retVal=resp_dtg_set->bmdDatagramSetTable[0]->datagramStatus) < BMD_OK)
	{
		unlink(path);
		free(path);
		bmd_datagramset_free(&resp_dtg_set);
		SOAP_FOK(retVal);
	}
	
	if(resp_dtg_set->bmdDatagramSetTable[0]->protocolDataFilename == NULL || 
		resp_dtg_set->bmdDatagramSetTable[0]->protocolDataFilename->size == 0 ||
		resp_dtg_set->bmdDatagramSetTable[0]->protocolDataFilename->buf == NULL)
	{
		unlink(path);
		free(path);
		bmd_datagramset_free(&resp_dtg_set);
		SOAP_FOK(BMDSOAP_SERVER_EMPTY_FILE_NAME);
	}

	// wypelnianie resultatu operacji
	tempOutput=(struct bmd230__fileInfo *)soap_malloc(soap, sizeof(struct bmd230__fileInfo));
	if(tempOutput == NULL)
	{
		unlink(path);
		free(path);
		bmd_datagramset_free(&resp_dtg_set);
		SOAP_FOK(NO_MEMORY);
	}
	memset(tempOutput, 0, sizeof(struct bmd230__fileInfo));

	
	tempOutput->filename=(char *)soap_malloc(soap, (resp_dtg_set->bmdDatagramSetTable[0]->protocolDataFilename->size+1) *sizeof(char) );
	if(tempOutput->filename == NULL)
	{
		unlink(path);
		free(path);
		bmd_datagramset_free(&resp_dtg_set);
		SOAP_FOK(NO_MEMORY);
	}
	memset(tempOutput->filename, 0, resp_dtg_set->bmdDatagramSetTable[0]->protocolDataFilename->size+1);
	memmove(tempOutput->filename, resp_dtg_set->bmdDatagramSetTable[0]->protocolDataFilename->buf, resp_dtg_set->bmdDatagramSetTable[0]->protocolDataFilename->size);
	bmd_datagramset_free(&resp_dtg_set);
	
	tempOutput->file=(struct bmd230__myBinaryDataType *)soap_malloc(soap, sizeof(struct bmd230__myBinaryDataType));
	if(tempOutput->file == NULL)
	{
		unlink(path);
		free(path);
		SOAP_FOK(NO_MEMORY);
	}
	memset(tempOutput->file, 0, sizeof(struct bmd230__myBinaryDataType));
	
	switch(type)
	{
	    case 0 :
		{
			retVal=bmd_load_binary_content(path, &gb);
			if(retVal < BMD_OK)
			{
				unlink(path);
				free(path);
				SOAP_FOK(retVal);
			}
			if(gb->size == 0 || gb->buf == NULL)
			{
				free_gen_buf(&gb);
				unlink(path);
				free(path);
				SOAP_FOK(BMD_ERR_OP_FAILED);
			}
			tempOutput->file->__ptr=(unsigned char *)soap_malloc(soap, (gb->size+1) * sizeof(unsigned char));
			if(tempOutput->file->__ptr == NULL)
			{
				free_gen_buf(&gb);
				unlink(path);
				free(path);
				SOAP_FOK(NO_MEMORY);
			}
			memset(tempOutput->file->__ptr, 0, (gb->size+1) * sizeof(unsigned char));
			tempOutput->file->__size=gb->size;
			memmove(tempOutput->file->__ptr, gb->buf, gb->size);
			free_gen_buf(&gb);
			tempOutput->file->type = NULL;
			tempOutput->file->options = NULL;

			break;
		}
		// Transmisja strumieniowa DIME
	    case 1 : 
		{  
			soap_set_omode(soap, SOAP_IO_CHUNK);
			fd = fopen(path,"r");
			if(fd == NULL)
			{
				unlink(path);
				free(path);
				SOAP_FOK(BMD_ERR_OPEN_FILE);
			}
			tempOutput->file->__ptr=(unsigned char *)fd;
			tempOutput->file->__size=0;
			tempOutput->file->type = "";
			tempOutput->file->options = soap_dime_option(soap, 0, tempOutput->filename );

			break;
		}
	}

	unlink(path);
	free(path); path=NULL;
	
	*output=tempOutput;
	tempOutput=NULL;
	
	PRINT_SOAP("Status:\t\tRequest served correctly\n");
	return SOAP_OK;
}



/*******************************************************/
/* obsluga zadania pobrania pliku o okreslonym skrocie */
/*******************************************************/
int bmd230__bmdGetFileByHash(	struct soap *soap,
				struct xsd__base64Binary *cert,
				char *roleName,
				char *groupName,
				char *userClassId,
				char *hash,
				struct bmd230__fileInfo **output)
{
long size				= 0;
long max_datagram_in_set_transmission	= 0;
long deserialisation_max_memory		= 0;
bmd_info_t *bi				= NULL;
lob_request_info_t *li			= NULL;
bmdDatagram_t *dtg			= NULL;
bmdDatagramSet_t *dtg_set		= NULL;
bmdDatagramSet_t *resp_dtg_set		= NULL;
GenBuf_t *gb				= NULL;
char *buf				= NULL;
char *path				= NULL;
GenBuf_t tmp_gb;

long retVal=0;
long derUserCertLen=0;
char *derUserCert=NULL;
char* paramCert=NULL;
long paramCertLen=0;

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if(hash==NULL)		{	SOAP_FOK(BMDSOAP_SERVER_UNDEFINED_FILE_HASH);	}
	if (strlen(hash)!=40)	{	SOAP_FOK(BMDSOAP_SERVER_INVALID_FILE_HASH);	}

	PRINT_SOAP("Request:\timport file with hash %s\n",hash);

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

	asprintf(&path,"soap_get_file_by_hash_%li.tmp",(long)getpid());

	SOAP_FOK_LOGOUT(bmd_lob_request_info_add_filename(path, &li, BMD_RECV_LOB,1000000000));

	/******************************/
	/*	stworzenie datagramu	*/
	/******************************/
	SOAP_FOK_LOGOUT(bmd_datagram_create(BMD_DATAGRAM_GET_CGI_LO,&dtg));
	if (userClassId!=NULL)
	{
		SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(	OID_ACTION_METADATA_CERT_CHOSEN_CLASS_ID, userClassId, &dtg));
	}

	tmp_gb.buf=(char *)hash;
	tmp_gb.size=strlen(hash);
	SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_2(OID_SYS_METADATA_CRYPTO_OBJECTS_HASH_VALUE,&tmp_gb,&dtg, 0));
	SOAP_FOK_LOGOUT(bmd_datagram_add_to_set(dtg,&dtg_set));

	/************************************/
	/*	wyslanie zadania lobowego	*/
	/************************************/
	SOAP_FOK_LOGOUT(bmd_send_request_lo(bi,dtg_set, max_datagram_in_set_transmission, &resp_dtg_set, li, deserialisation_max_memory));
	SOAP_FOK_LOGOUT(resp_dtg_set->bmdDatagramSetTable[0]->datagramStatus);

	/************************/
	/*	ustawienie wyniku	*/
	/************************/
	bmd_load_binary_content(path,&gb);
	bmd_genbuf_get_buf(gb,&buf);
	bmd_genbuf_get_size(gb,&size);

	(*output)=(struct bmd230__fileInfo *)soap_malloc(soap,sizeof(struct bmd230__fileInfo));
	memset(*output,0,sizeof(struct bmd230__fileInfo));

	/*	ustawienie nazwy pliku	*/
	(*output)->filename=(char *)soap_malloc(soap,resp_dtg_set->bmdDatagramSetTable[0]->protocolDataFilename->size+2);
	memset((*output)->filename, 0, resp_dtg_set->bmdDatagramSetTable[0]->protocolDataFilename->size+1);
	memmove((*output)->filename, resp_dtg_set->bmdDatagramSetTable[0]->protocolDataFilename->buf, resp_dtg_set->bmdDatagramSetTable[0]->protocolDataFilename->size);

	/*	ustawienie pliku	*/
	(*output)->file=(struct bmd230__myBinaryDataType *)soap_malloc(soap,sizeof(struct bmd230__myBinaryDataType));
	(*output)->file->__ptr=(char unsigned *)soap_malloc(soap,size+2);
	memset((*output)->file->__ptr,0,size+1);
	(*output)->file->__size=size;
	memmove((*output)->file->__ptr,buf,size);
	(*output)->file->id = NULL;
	(*output)->file->type = NULL;
	(*output)->file->options = NULL;


	/******************/
	/*	porzadki	*/
	/******************/
	bmd_logout(&bi, deserialisation_max_memory);
	bmd_info_destroy(&bi);
	bmd_end();

	unlink(path);
	free(path); path=NULL;
	free_gen_buf(&gb);
	bmd_datagramset_free(&dtg_set);
	bmd_datagramset_free(&resp_dtg_set);
	bmd_lob_request_info_destroy(&li);

	PRINT_SOAP("Status:\t\trequest served correctly\n");
	return SOAP_OK;
}


/*******************************************************************/
/* obsluzenie zadania pobrania wybranych wartosci dowodowych pliku */
/*******************************************************************/
int bmd230__bmdGetFilePropByID(	struct soap *soap,
				struct xsd__base64Binary *cert,
				char *roleName,
				char *groupName,
				char *userClassId,
				long id,
				int property,
				struct bmd230__fileInfo **output)
{
long size				= 0;
long status				= 0;
long max_datagram_in_set_transmission	= 0;
long deserialisation_max_memory		= 0;
bmd_info_t *bi				= NULL;
bmdDatagram_t *dtg			= NULL;
bmdDatagramSet_t *dtg_set		= NULL;
bmdDatagramSet_t *resp_dtg_set		= NULL;
bmdDatagram_t *tmp_dtg			= NULL;
GenBuf_t *gb				= NULL;
char *buf				= NULL;
char *filename				= NULL;

long retVal=0;
long derUserCertLen=0;
char *derUserCert=NULL;
char* paramCert=NULL;
long paramCertLen=0;

	PRINT_SOAP("Request:\tgetting metadata of file with id %li\n",id);
	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if(id<=0)		{	SOAP_FOK(BMDSOAP_SERVER_INVALID_FILE_ID);		}
	if(property<-2)	{	SOAP_FOK(BMDSOAP_SERVER_INVALID_FILE_PROPERTY);	}

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
	/*	stwoerzenie zadania	*/
	/******************************/
	SOAP_FOK_LOGOUT(bmd_datagram_create(BMD_DATAGRAM_GET_METADATA,&dtg));
	if (userClassId!=NULL)
	{
		SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(	OID_ACTION_METADATA_CERT_CHOSEN_CLASS_ID, userClassId, &dtg));
	}

	SOAP_FOK_LOGOUT(bmd_datagram_set_id((const long *)&id,&dtg));
	SOAP_FOK_LOGOUT(bmd_datagram_add_to_set(dtg,&dtg_set));

	/************************/
	/*	wyslaie zadania	*/
	/************************/
	SOAP_FOK_LOGOUT(bmd_send_request(bi,dtg_set, max_datagram_in_set_transmission, &resp_dtg_set, 1, deserialisation_max_memory));

	/******************************/
	/*	odczytanie odpowiedzi	*/
	/******************************/
	SOAP_FOK_LOGOUT(resp_dtg_set->bmdDatagramSetTable[0]->datagramStatus);

	/* ustawienie wyniku */
	(*output)=(struct bmd230__fileInfo *)soap_malloc(soap,sizeof(struct bmd230__fileInfo));
	memset(*output,0,sizeof(struct bmd230__fileInfo));

	/* pobranie datagramu zerowego */
	SOAP_FOK_LOGOUT(bmd_datagramset_get_datagram(resp_dtg_set,0,&tmp_dtg));
	if (property==0)
	{
		status=bmd_datagram_get_metadata(PKI_METADATA, OID_PKI_METADATA_SIGNATURE, tmp_dtg, 0, &gb);
	}
	else if (property==-1)
	{

		status=bmd_datagram_get_metadata(PKI_METADATA, OID_PKI_METADATA_DVCS, tmp_dtg, property, &gb);
	}
	else if (property==-2)
	{
		status=bmd_datagram_get_metadata(PKI_METADATA, OID_PKI_METADATA_SIGNATURE_XADES, tmp_dtg, 0, &gb);
	}
	else
	{

		status=bmd_datagram_get_metadata(PKI_METADATA, OID_PKI_METADATA_TIMESTAMP, tmp_dtg, property, &gb);
	}

	if (status<BMD_OK)
	{
		if (status==-5)
		{
			SOAP_FOK_LOGOUT(BMDSOAP_SERVER_NO_SELECTED_PKI_METADATA);
		}
		SOAP_FOK_LOGOUT(BMDSOAP_SERVER_GETPKI_METADATA_ERR);
	}

	/* pobranie nazwy pliku */
	SOAP_FOK_LOGOUT(bmd_datagram_get_filename(tmp_dtg,&filename));
	if (property==0)
	{
		asprintf(&filename,"Signature CMS");
	}
	else if (property==-2)
	{
		asprintf(&filename,"Signature XADES");
	}
	else if (property==-1)
	{
		asprintf(&filename,"DVCS");
	}
	else
	{
		asprintf(&filename,"Timestamp_%i",property);
	}

	(*output)->filename=(char *)soap_malloc(soap,strlen(filename)+2);
	memset((*output)->filename,0,strlen(filename)+1);
	memmove((*output)->filename,filename,strlen(filename));

	status=bmd_genbuf_get_buf(gb,&buf);
	status=bmd_genbuf_get_size(gb,&size);

	(*output)->file=(struct bmd230__myBinaryDataType *)soap_malloc(soap,sizeof(struct bmd230__myBinaryDataType));
	(*output)->file->__ptr=(unsigned char *)soap_malloc(soap,size+2);
	memset((*output)->file->__ptr,0,size+1);
	(*output)->file->__size=size;
	memcpy((*output)->file->__ptr,buf,size);
	(*output)->file->id = NULL;
	(*output)->file->type = NULL;
	(*output)->file->options = NULL;

	/******************/
	/*	porzadki	*/
	/******************/
	bmd_logout(&bi, deserialisation_max_memory);
	bmd_info_destroy(&bi);
	bmd_end();

	bmd_datagramset_free(&dtg_set);
	bmd_datagramset_free(&resp_dtg_set);
	free(filename);filename=NULL;
	return SOAP_OK;
}


/********************************************************************/
/* obsluzenie zadania pobrania wszystkich wartosci dowodowych pliku */
/********************************************************************/
int bmd230__bmdGetFilePKIById(	struct soap *soap,
				struct xsd__base64Binary *cert,
				char *roleName,
				char *groupName,
				char *userClassId,
				long id,
				struct bmd230__PkiFileInfo **output)
{

long i					= 0;
long j					= 0;
long k					= 0;
long max_datagram_in_set_transmission	= 0;
long deserialisation_max_memory		= 0;
bmd_info_t *bi				= NULL;
bmdDatagram_t *dtg			= NULL;
bmdDatagramSet_t *dtg_set		= NULL;
bmdDatagramSet_t *resp_dtg_set		= NULL;
lob_request_info_t *li			= NULL;

char *buf				= NULL;
char *filename				= NULL;

/*GenBuf_t *metadata_value		= NULL;*/
char *metadata_value			= NULL;
GenBuf_t *gb				= NULL;

// char timestamp[]			= {OID_PKI_METADATA_TIMESTAMP};
// char signature[]			= {OID_PKI_METADATA_SIGNATURE};
// char signature_xades[]			= {OID_PKI_METADATA_SIGNATURE_XADES};
// char dvcs[]				= {OID_PKI_METADATA_DVCS};
// char tim_sig[]			= {OID_PKI_METADATA_TIMESTAMP_FROM_SIG};
// char tim_dvcs[]			= {OID_PKI_METADATA_TIMESTAMP_FROM_DVCS};

long metadata_size		= 0;

long retVal=0;
long derUserCertLen=0;
char *derUserCert=NULL;
char* paramCert=NULL;
long paramCertLen=0;

	PRINT_SOAP("Request:\tgetting pki metadata of file with id %li\n",id);
	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if(id<=0)
	{
		SOAP_FOK(BMDSOAP_SERVER_INVALID_FILE_ID);
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
	/*	utworzenie zadania	*/
	/******************************/
	SOAP_FOK_LOGOUT(bmd_datagram_create(BMD_DATAGRAM_GET_METADATA,&dtg));
	if (userClassId!=NULL)
	{
		SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(	OID_ACTION_METADATA_CERT_CHOSEN_CLASS_ID, userClassId, &dtg));
	}

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

	/************************/
	/*	ustawienie wyniku	*/
	/************************/
	(*output)=(struct bmd230__PkiFileInfo *)malloc(sizeof(struct bmd230__PkiFileInfo)+2);
	memset((*output), 0, sizeof(struct bmd230__PkiFileInfo)+1);

	(*output)->timestamp=(struct bmd230__TimestampList *)malloc(sizeof(struct bmd230__TimestampList)+2);
	memset((*output)->timestamp, 0, sizeof(struct bmd230__TimestampList)+1);
	(*output)->timestamp->__size=0;

	(*output)->signature=(struct bmd230__SignatureList *)malloc(sizeof(struct bmd230__SignatureList)+2);
	memset((*output)->signature, 0, sizeof(struct bmd230__SignatureList)+1);
	(*output)->signature->__size=0;

	if (resp_dtg_set->bmdDatagramSetTable[0]->datagramStatus==BMD_OK)
	{
		/********************************/
		/*	przepisanie nazwy pliku	*/
		/********************************/
		asprintf(&((*output)->filename),"%s",resp_dtg_set->bmdDatagramSetTable[0]->protocolDataFilename->buf);
		/****************************************************************************************/
		/*	przepisanie wartosci dowodowej z datagramu odpowiedzi do struktury zwrotnej	*/
		/****************************************************************************************/
		for (i=0; i<resp_dtg_set->bmdDatagramSetTable[0]->no_of_pkiMetaData; i++)
		{
			/********************************/
			/*	znaczniki czasu pliku	*/
			/********************************/
			if (strcmp(	resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[i]->OIDTableBuf, OID_PKI_METADATA_TIMESTAMP)==0)
			{
				/************************************************************************/
				/*	alokacja pamieci oraz wypelnienie jej dla znacznika czasu	*/
				/************************************************************************/
				(*output)->timestamp->__size++;
				(*output)->timestamp->__ptr=(struct bmd230__TimestampSingle *)realloc((*output)->timestamp->__ptr, sizeof(struct bmd230__TimestampSingle )*(*output)->timestamp->__size);
				(*output)->timestamp->__ptr[(*output)->timestamp->__size-1].value=(struct xsd__base64Binary *)malloc(sizeof(struct xsd__base64Binary));

				/*tmpANY.buf=(unsigned char*)resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[i]->AnyBuf;
				tmpANY.size=resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[i]->AnySize;
				metadata_value=(GenBuf_t *)malloc(sizeof(GenBuf_t));
				Any2Octetstring2GenBuf(&tmpANY,metadata_value);*/
				metadata_value = resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[i]->AnyBuf;
				metadata_size = resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[i]->AnySize;

				(*output)->timestamp->__ptr[(*output)->timestamp->__size-1].value->__ptr =
				(unsigned char *)malloc(metadata_size + 1);
				if((*output)->timestamp->__ptr[(*output)->timestamp->__size-1].value->__ptr == NULL)
				{	SOAP_FOK_LOGOUT(NO_MEMORY);	}
				memset((*output)->timestamp->__ptr[(*output)->timestamp->__size-1].value->__ptr, 0,
				metadata_size + 1);

				memcpy((*output)->timestamp->__ptr[(*output)->timestamp->__size-1].value->__ptr,
				metadata_value, metadata_size);
				(*output)->timestamp->__ptr[(*output)->timestamp->__size-1].value->__size = metadata_size;

				metadata_value = NULL;
				metadata_size = 0;
				/*free_gen_buf(&metadata_value);*/

			}

			/************************/
			/*	podpisy pliku	*/
			/************************/
			if ( strcmp(	resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[i]->OIDTableBuf, OID_PKI_METADATA_SIGNATURE)==0 ||
			     strcmp(	resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[i]->OIDTableBuf, OID_PKI_METADATA_SIGNATURE_XADES)==0 ||
				 strcmp(	resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[i]->OIDTableBuf, OID_PKI_METADATA_SIGNATURE_PADES)==0)
			{
				/****************************************************************/
				/*	alokacja pamieci oraz wypelnienie jej dla podpisu	*/
				/****************************************************************/

				(*output)->signature->__size++;
				(*output)->signature->__ptr=(struct bmd230__SignatureSingle *)realloc((*output)->signature->__ptr, sizeof(struct bmd230__SignatureSingle )*(*output)->signature->__size);
				memset(&((*output)->signature->__ptr[(*output)->signature->__size-1]), 0, sizeof(struct bmd230__SignatureSingle ));
				
				// w przypadku PAdES wyodrebniony podpis nie bedzie umieszczany w zwracanej wartosci dowodowej
				if(strcmp(	resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[i]->OIDTableBuf, OID_PKI_METADATA_SIGNATURE_PADES) !=0 )
				{
					(*output)->signature->__ptr[(*output)->signature->__size-1].value=(struct xsd__base64Binary *)calloc(1, sizeof(struct xsd__base64Binary));

					/*tmpANY.buf=(unsigned char*)resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[i]->AnyBuf;
					tmpANY.size=resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[i]->AnySize;
					metadata_value=(GenBuf_t *)malloc(sizeof(GenBuf_t));
					Any2Octetstring2GenBuf(&tmpANY,metadata_value);*/
					metadata_value = resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[i]->AnyBuf;
					metadata_size = resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[i]->AnySize;


					(*output)->signature->__ptr[(*output)->signature->__size-1].value->__ptr=
					(unsigned char *)malloc(metadata_size + 1);

					if((*output)->signature->__ptr[(*output)->signature->__size-1].value->__ptr == NULL)
					{	BMD_FOK(NO_MEMORY);	}
					memset((*output)->signature->__ptr[(*output)->signature->__size-1].value->__ptr, 0,
					metadata_size + 1);

					memcpy((*output)->signature->__ptr[(*output)->signature->__size-1].value->__ptr,
					metadata_value, metadata_size);
					(*output)->signature->__ptr[(*output)->signature->__size-1].value->__size = metadata_size;
					/*free_gen_buf(&metadata_value);*/
					metadata_value=NULL;
					metadata_size=0;
				}

				/****************************************************************/
				/*	alokacja pamieci pod liste znacznikow czasu podpisu	*/
				/****************************************************************/
				(*output)->signature->__ptr[(*output)->signature->__size-1].timestamp=(struct bmd230__TimestampList *)malloc(sizeof(struct bmd230__TimestampList)+2);
				memset((*output)->signature->__ptr[(*output)->signature->__size-1].timestamp, 0, sizeof(struct bmd230__TimestampList)+1);
				(*output)->signature->__ptr[(*output)->signature->__size-1].timestamp->__size=0;

				/****************************************************************/
				/*	alokacja pamieci pod liste poswiadczen dvcs podpisu	*/
				/****************************************************************/
				(*output)->signature->__ptr[(*output)->signature->__size-1].dvcs=(struct bmd230__DVCSList *)malloc(sizeof(struct bmd230__DVCSList)+2);
				memset((*output)->signature->__ptr[(*output)->signature->__size-1].dvcs, 0, sizeof(struct bmd230__DVCSList)+1);
				(*output)->signature->__ptr[(*output)->signature->__size-1].dvcs->__size=0;

				/************************************************************************/
				/*	wyszukiwanie  poswiadczen i znacznikow czasu dla podpisu	*/
				/************************************************************************/
				for (j=0; j<resp_dtg_set->bmdDatagramSetTable[0]->no_of_pkiMetaData; j++)
				{
					/************************************************/
					/*	pobranie znacznika czasu podpisu	*/
					/************************************************/
					if ((strcmp(	resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[j]->OIDTableBuf, OID_PKI_METADATA_TIMESTAMP_FROM_SIG)==0) &&
					   (resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[i]->myId==resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[j]->ownerId))
					{
						/********************************************************************************/
						/*	alokacja pamieci oraz wypelnienie jej znacznika czasu dla podpisu	*/
						/********************************************************************************/
						(*output)->signature->__ptr[(*output)->signature->__size-1].timestamp->__size++;
						(*output)->signature->__ptr[(*output)->signature->__size-1].timestamp->__ptr=(struct bmd230__TimestampSingle *)realloc((*output)->signature->__ptr[(*output)->signature->__size-1].timestamp->__ptr, sizeof(struct bmd230__TimestampSingle )*(*output)->signature->__ptr[(*output)->signature->__size-1].timestamp->__size);
						(*output)->signature->__ptr[(*output)->signature->__size-1].timestamp->__ptr[(*output)->signature->__ptr[(*output)->signature->__size-1].timestamp->__size-1].value=(struct xsd__base64Binary *)malloc(sizeof(struct xsd__base64Binary));

						/*tmpANY.buf=(unsigned char*)resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[j]->AnyBuf;
						tmpANY.size=resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[j]->AnySize;
						metadata_value=(GenBuf_t *)malloc(sizeof(GenBuf_t));
						Any2Octetstring2GenBuf(&tmpANY,metadata_value);*/
						metadata_value = resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[j]->
						AnyBuf;
						metadata_size = resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[j]->
						AnySize;

						(*output)->signature->__ptr[(*output)->signature->__size-1].timestamp->
						__ptr[(*output)->signature->__ptr[(*output)->
						signature->__size-1].timestamp->__size-1].value->__ptr=
						(unsigned char *)malloc(metadata_size+1);
						memcpy((*output)->signature->__ptr[(*output)->signature->__size-1].timestamp->__ptr[(*output)->signature->__ptr[(*output)->signature->__size-1].timestamp->__size-1].value->__ptr, metadata_value, metadata_size);
						(*output)->signature->__ptr[(*output)->signature->__size-1].timestamp->__ptr[(*output)->signature->__ptr[(*output)->signature->__size-1].timestamp->__size-1].value->__size = metadata_size;

						/*free_gen_buf(&metadata_value);*/
					}

					/******************************************/
					/*	pobranie poswiadczenia dvcs podpisu	*/
					/******************************************/

					if ((strcmp(	resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[j]->OIDTableBuf, OID_PKI_METADATA_DVCS)==0) &&
					   (resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[i]->myId==resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[j]->ownerId))
					{
						/************************************************************************/
						/*	alokacja pamieci oraz wypelnienie jej dla poswiadzcenia dvcs	*/
						/************************************************************************/
						(*output)->signature->__ptr[(*output)->signature->__size-1].dvcs->__size++;
						(*output)->signature->__ptr[(*output)->signature->__size-1].dvcs->__ptr=(struct bmd230__DVCSSingle *)realloc((*output)->signature->__ptr[(*output)->signature->__size-1].dvcs->__ptr, sizeof(struct bmd230__DVCSSingle )*(*output)->signature->__ptr[(*output)->signature->__size-1].dvcs->__size);
						(*output)->signature->__ptr[(*output)->signature->__size-1].dvcs->__ptr[(*output)->signature->__ptr[(*output)->signature->__size-1].dvcs->__size-1].value=(struct xsd__base64Binary *)malloc(sizeof(struct xsd__base64Binary));

						metadata_value = resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[j]->
						AnyBuf;
						metadata_size = resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[j]->
						AnySize;

						/*tmpANY.buf=(unsigned char*)resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[j]->AnyBuf;
						tmpANY.size=resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[j]->AnySize;
						metadata_value=(GenBuf_t *)malloc(sizeof(GenBuf_t));
						Any2Octetstring2GenBuf(&tmpANY,metadata_value);*/

						(*output)->signature->__ptr[(*output)->signature->__size-1].dvcs->__ptr[(*output)->signature->__ptr[(*output)->signature->__size-1].dvcs->__size-1].value->__ptr=(unsigned char *)malloc(metadata_size+1);
						memcpy((*output)->signature->__ptr[(*output)->signature->__size-1].dvcs->__ptr[(*output)->signature->__ptr[(*output)->signature->__size-1].dvcs->__size-1].value->__ptr, metadata_value, metadata_size);

						(*output)->signature->__ptr[(*output)->signature->__size-1].dvcs->__ptr[(*output)->signature->__ptr[(*output)->signature->__size-1].dvcs->__size-1].value->__size = metadata_size;

						/*free_gen_buf(&metadata_value);*/

						/********************************************************************************/
						/*	alokacja pamieci pod liste znacznikow czasu poswiadzcenia dvcs podpisu	*/
						/********************************************************************************/
						(*output)->signature->__ptr[(*output)->signature->__size-1].dvcs->__ptr[(*output)->signature->__ptr[(*output)->signature->__size-1].dvcs->__size-1].timestamp=(struct bmd230__TimestampList *)malloc(sizeof(struct bmd230__TimestampList)+2);
						memset((*output)->signature->__ptr[(*output)->signature->__size-1].dvcs->__ptr[(*output)->signature->__ptr[(*output)->signature->__size-1].dvcs->__size-1].timestamp, 0, sizeof(struct bmd230__TimestampList)+1);
						(*output)->signature->__ptr[(*output)->signature->__size-1].dvcs->__ptr[(*output)->signature->__ptr[(*output)->signature->__size-1].dvcs->__size-1].timestamp->__size=0;


						/****************************************************************/
						/*	wyszukiwanie znacznikow czasu dla poswiadczenia dvcs	*/
						/****************************************************************/
						for (k=0; k<resp_dtg_set->bmdDatagramSetTable[0]->no_of_pkiMetaData; k++)
						{
							if ((strcmp(	resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[k]->OIDTableBuf, OID_PKI_METADATA_TIMESTAMP_FROM_DVCS)==0) &&
								(resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[j]->myId==resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[k]->ownerId))
							{
								/****************************************************************************************/
								/*	alokacja pamieci oraz wypelnienie jej dla znacznika czasu poswiadzcenia dvcs	*/
								/****************************************************************************************/
								(*output)->signature->__ptr[(*output)->signature->__size-1].dvcs->__ptr[(*output)->signature->__ptr[(*output)->signature->__size-1].dvcs->__size-1].timestamp->__size++;
								(*output)->signature->__ptr[(*output)->signature->__size-1].dvcs->__ptr[(*output)->signature->__ptr[(*output)->signature->__size-1].dvcs->__size-1].timestamp->__ptr=(struct bmd230__TimestampSingle *)realloc((*output)->signature->__ptr[(*output)->signature->__size-1].dvcs->__ptr[(*output)->signature->__ptr[(*output)->signature->__size-1].dvcs->__size-1].timestamp->__ptr, sizeof(struct bmd230__TimestampSingle )*(*output)->signature->__ptr[(*output)->signature->__size-1].dvcs->__ptr[(*output)->signature->__ptr[(*output)->signature->__size-1].dvcs->__size-1].timestamp->__size);
								(*output)->signature->__ptr[(*output)->signature->__size-1].dvcs->__ptr[(*output)->signature->__ptr[(*output)->signature->__size-1].dvcs->__size-1].timestamp->__ptr[(*output)->signature->__ptr[(*output)->signature->__size-1].dvcs->__ptr[(*output)->signature->__ptr[(*output)->signature->__size-1].dvcs->__size-1].timestamp->__size-1].value=(struct xsd__base64Binary *)malloc(sizeof(struct xsd__base64Binary));

								metadata_value = resp_dtg_set->bmdDatagramSetTable[0]->
								pkiMetaData[k]->AnyBuf;

								metadata_size = resp_dtg_set->bmdDatagramSetTable[0]->
								pkiMetaData[k]->AnySize;

								/*tmpANY.buf=(unsigned char*)(resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[k]->AnyBuf);
								tmpANY.size=resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[k]->AnySize;
								metadata_value=(GenBuf_t *)malloc(sizeof(GenBuf_t));
								Any2Octetstring2GenBuf(&tmpANY,metadata_value);*/

								(*output)->signature->__ptr[(*output)->signature->__size-1].dvcs->__ptr[(*output)->signature->__ptr[(*output)->signature->__size-1].dvcs->__size-1].timestamp->__ptr[(*output)->signature->__ptr[(*output)->signature->__size-1].dvcs->__ptr[(*output)->signature->__ptr[(*output)->signature->__size-1].dvcs->__size-1].timestamp->__size-1].value->__ptr=(unsigned char *)malloc(metadata_size + 1);
								memcpy((*output)->signature->__ptr[(*output)->signature->__size-1].dvcs->__ptr[(*output)->signature->__ptr[(*output)->signature->__size-1].dvcs->__size-1].timestamp->__ptr[(*output)->signature->__ptr[(*output)->signature->__size-1].dvcs->__ptr[(*output)->signature->__ptr[(*output)->signature->__size-1].dvcs->__size-1].timestamp->__size-1].value->__ptr, metadata_value, metadata_size);

								(*output)->signature->__ptr[(*output)->signature->__size-1].dvcs->__ptr[(*output)->signature->__ptr[(*output)->signature->__size-1].dvcs->__size-1].timestamp->__ptr[(*output)->signature->__ptr[(*output)->signature->__size-1].dvcs->__ptr[(*output)->signature->__ptr[(*output)->signature->__size-1].dvcs->__size-1].timestamp->__size-1].value->__size = metadata_size;

								/*free_gen_buf(&metadata_value);*/
							}

						}

					}

				}

			}

		}

	}

	/******************/
	/*	porzadki	*/
	/******************/
	bmd_logout(&bi, deserialisation_max_memory);
	bmd_info_destroy(&bi);
	bmd_end();

	bmd_datagramset_free(&dtg_set);
	bmd_datagramset_free(&resp_dtg_set);
	free_gen_buf(&gb);
	free(buf); buf=NULL;
	free(filename); filename=NULL;
	bmd_lob_request_info_destroy(&li);

	PRINT_SOAP("Status:\t\trequest served correctly\n");
	return SOAP_OK;
}


/**********************************************************************************/
/* obsluga zadania pobrania pliku i metadanych dowodowych w zipie o okreslonym ID */
/**********************************************************************************/
int bmd230__bmdGetFileByIdInZipPack(struct soap *soap,
				struct xsd__base64Binary *cert,
				char *roleName,
				char *groupName,
				char *userClassId,
				long id,
				enum trans_t type,
				struct bmd230__fileInfo **output)
{
long i						= 0;
long j						= 0;
long k						= 0;
long size					= 0;
long status					= 0;
long sig_no					= 0;
long dvcs_no					= 0;
long zip_list_size				= 0;
long max_datagram_in_set_transmission		= 0;
long deserialisation_max_memory			= 0;
char *buf					= NULL;
bmd_info_t *bi					= NULL;
lob_request_info_t *li				= NULL;
bmdDatagram_t *dtg				= NULL;
bmdDatagramSet_t *dtg_set			= NULL;
bmdDatagramSet_t *resp_dtg_set			= NULL;
GenBuf_t *gb					= NULL;
char *path					= NULL;
char *path_dest					= NULL;
char *path_dest_zip				= NULL;
char **zip_list					= NULL;
GenBuf_t *metadata_value			= NULL;
char *file_tsr					= NULL;
char *file_sig					= NULL;
char *file_sig2					= NULL;
char *file_sig_tsr				= NULL;
char *file_sig_dvcs				= NULL;
char *file_sig_dvcs_tsr				= NULL;
char *dir_sig					= NULL;
char *dir_dvcs					= NULL;
char *tmp_dir					= NULL;

char *zip_signature_directory_name		= NULL;
char *zip_dvcs_directory_name			= NULL;
char *zip_attachments_directory_name		= NULL;
char *tmp_location				= NULL;

char *tmp_file					= {"soap_get_file_zip_"};
FILE* fd = NULL;

char timestamp[]	= {OID_PKI_METADATA_TIMESTAMP};
char signature[]	= {OID_PKI_METADATA_SIGNATURE};
char signature_pades[]	= {OID_PKI_METADATA_SIGNATURE_PADES};
char dvcs[]		= {OID_PKI_METADATA_DVCS};
char tim_sig[]	= {OID_PKI_METADATA_TIMESTAMP_FROM_SIG};
char tim_dvcs[]	= {OID_PKI_METADATA_TIMESTAMP_FROM_DVCS};

long retVal=0;
long derUserCertLen=0;
char *derUserCert=NULL;
char* paramCert=NULL;
long paramCertLen=0;

	PRINT_SOAP("Request:\tget pki metadata of file with id %li in zip pack\n",id);
	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if(id<=0)			{	SOAP_FOK(BMDSOAP_SERVER_INVALID_FILE_ID);		}

	/******************************************/
	/*	wczytanie ustawien konfiguracyjnych	*/
	/******************************************/
	SOAP_FOK(bmdconf_get_value(_GLOBAL_konfiguracja,"soap","temp_dir",&tmp_dir));
	SOAP_FOK(bmdconf_get_value(_GLOBAL_konfiguracja,"zip_package","zip_signature_directory_name",&zip_signature_directory_name));
	SOAP_FOK(bmdconf_get_value(_GLOBAL_konfiguracja,"zip_package","zip_dvcs_directory_name",&zip_dvcs_directory_name));
	SOAP_FOK(bmdconf_get_value(_GLOBAL_konfiguracja,"zip_package","zip_attachments_directory_name",
	&zip_attachments_directory_name));

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

	/************************************/
	/*	odczytanie katalogu biezacego	*/
	/************************************/
	memset(_GLOBAL_current_dir, 0 ,256);
	if (getcwd(_GLOBAL_current_dir,255)==NULL)
	{
		bmd_logout(&bi, deserialisation_max_memory);
		bmd_end();
		PRINT_ERROR("SOAPSERVERERR %s\n","Error in getting current directory");
		return soap_receiver_fault(soap,"Soap server error","Error in getting current directory");
	}

	/************************************************************************************************/
	/*	stworzenie katalogu tymczasowego dla pobranych danych z serwera i przejscie do niego	*/
	/************************************************************************************************/
	asprintf(&tmp_location, "%s%s%li/", tmp_dir, tmp_file, (long)getpid());
	SOAP_FOK_LOGOUT(mkdir(tmp_location, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH));
	SOAP_FOK_LOGOUT_TMP(chdir(tmp_location));

	/************************************/
	/*	okreslenie pliku tymczasowego	*/
	/************************************/
	asprintf(&path, "%s%li", tmp_file, (long)getpid());
	SOAP_FOK_LOGOUT_CD_TMP(bmd_lob_request_info_add_filename(path, &li, BMD_RECV_LOB,1000000000));

	/******************************/
	/*	stworzenie datagramu	*/
	/******************************/
	SOAP_FOK_LOGOUT_CD_TMP(bmd_datagram_create(BMD_DATAGRAM_GET_CGI_LO,&dtg));
	if (userClassId!=NULL)
	{
		SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(	OID_ACTION_METADATA_CERT_CHOSEN_CLASS_ID, userClassId, &dtg));
	}

	/************************************/
	/*	ustawienie id w datagramie	*/
	/************************************/
	SOAP_FOK_LOGOUT_CD_TMP(bmd_datagram_set_id((const long *)&id,&dtg));

	/************************************************/
	/*	dodanie datagramu do zestawu datagramow	*/
	/************************************************/
	SOAP_FOK_LOGOUT_CD_TMP(bmd_datagram_add_to_set(dtg,&dtg_set));

	/************************************/
	/*	wyslanie zadania lobowego	*/
	/************************************/
	SOAP_FOK_LOGOUT_CD_TMP(bmd_send_request_lo(bi,dtg_set, max_datagram_in_set_transmission, &resp_dtg_set, li, deserialisation_max_memory));
	SOAP_FOK_LOGOUT_CD_TMP(resp_dtg_set->bmdDatagramSetTable[0]->datagramStatus);
	/************************************************************************/
	/*	przepisanie pobranego pliku z pliku tymczasowego do docelowego	*/
	/************************************************************************/
	asprintf(&path_dest, "%s", resp_dtg_set->bmdDatagramSetTable[0]->protocolDataFilename->buf);
	bmd_load_binary_content(path,&gb);
	bmd_save_buf(gb, path_dest);
	unlink(path);

	/************************************************/
	/*	stworzenie listy plikow do spakowania	*/
	/************************************************/
	zip_list_size++;
	zip_list=(char **)realloc(zip_list,sizeof(char *)*zip_list_size);

	/* --------- pobrany plik ------------------ */
	zip_list[zip_list_size-1]=(char*)malloc(strlen(path_dest)+2);
	memset(zip_list[zip_list_size-1], 0, strlen(path_dest)+1);
	memcpy(zip_list[zip_list_size-1], path_dest, strlen(path_dest));

	/* --------- wartosc dowodowa --------------------*/
	asprintf(&file_tsr,"%s",path_dest);
	asprintf(&file_sig,"%s",path_dest);

	for (i=0; i<resp_dtg_set->bmdDatagramSetTable[0]->no_of_pkiMetaData; i++)
	{
		/******************************/
		/*	znaczniki czasu pliku	*/
		/******************************/
		if (strcmp(	resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[i]->OIDTableBuf,timestamp)==0)
		{
			/*tmpANY.buf=(unsigned char*)resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[i]->AnyBuf;
			tmpANY.size=resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[i]->AnySize;
			metadata_value=(GenBuf_t *)malloc(sizeof(GenBuf_t));
			Any2Octetstring2GenBuf(&tmpANY,metadata_value);*/
			BMD_FOK(set_gen_buf2(resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[i]->AnyBuf,
			resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[i]->AnySize, &metadata_value));

			asprintf(&file_tsr,"%s.tsr",file_tsr);
			bmd_save_buf(metadata_value, file_tsr);
			free_gen_buf(&metadata_value);

			zip_list_size++;
			zip_list=(char **)realloc(zip_list,sizeof(char *)*zip_list_size);

			zip_list[zip_list_size-1]=(char*)malloc(strlen(file_tsr)+2);
			memset(zip_list[zip_list_size-1], 0, strlen(file_tsr)+1);
			memcpy(zip_list[zip_list_size-1], file_tsr, strlen(file_tsr));

		}

		/************************/
		/*	podpisy pliku	*/
		/************************/
		if (strcmp(	resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[i]->OIDTableBuf,signature)==0 ||
			strcmp(	resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[i]->OIDTableBuf,signature_pades)==0)
		{
			/*tmpANY.buf=(unsigned char*)resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[i]->AnyBuf;
			tmpANY.size=resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[i]->AnySize;
			metadata_value=(GenBuf_t *)malloc(sizeof(GenBuf_t));
			Any2Octetstring2GenBuf(&tmpANY,metadata_value);*/
			BMD_FOK(set_gen_buf2(resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[i]->AnyBuf,
			resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[i]->AnySize, &metadata_value));


			sig_no++;
			dvcs_no=0;

			asprintf(&dir_sig,"%s_%li/",zip_signature_directory_name, sig_no);
			SOAP_FOK_LOGOUT_CD_TMP(mkdir(dir_sig, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH));

			asprintf(&file_sig,"%s%s.sig",dir_sig,path_dest);
			asprintf(&file_sig2,"%s.sig",path_dest);
			// dla PAdES nie jest umieszczany w ZIPie wyodrebniony podpis (podpis jest wewnatrz pliku PDF)
			if(strcmp(	resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[i]->OIDTableBuf,signature_pades) != 0)
			{
				status=bmd_save_buf(metadata_value, file_sig);

				zip_list_size++;
				zip_list=(char **)realloc(zip_list,sizeof(char *)*zip_list_size);

				zip_list[zip_list_size-1]=(char*)malloc(strlen(file_sig)+2);
				memset(zip_list[zip_list_size-1], 0, strlen(file_sig)+1);
				memcpy(zip_list[zip_list_size-1], file_sig, strlen(file_sig));
			}
			free_gen_buf(&metadata_value);
			
			asprintf(&file_sig_tsr,"%s",file_sig);
			asprintf(&file_sig_dvcs,"%s",file_sig);


			for (j=0; j<resp_dtg_set->bmdDatagramSetTable[0]->no_of_pkiMetaData; j++)
			{
				/******************************************/
				/*	pobranie znacznika czasu podpisu	*/
				/******************************************/
				if ((strcmp(	resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[j]->OIDTableBuf,tim_sig)==0) &&
							(resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[i]->myId==resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[j]->ownerId))
				{
					/*tmpANY.buf=(unsigned char*)resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[j]->AnyBuf;
					tmpANY.size=resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[j]->AnySize;
					metadata_value=(GenBuf_t *)malloc(sizeof(GenBuf_t));
					Any2Octetstring2GenBuf(&tmpANY,metadata_value);*/
					BMD_FOK(set_gen_buf2(resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[j]->AnyBuf,
					resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[j]->AnySize, &metadata_value));


					asprintf(&file_sig_tsr,"%s.tsr",file_sig_tsr);
					bmd_save_buf(metadata_value, file_sig_tsr);
					free_gen_buf(&metadata_value);

					zip_list_size++;
					zip_list=(char **)realloc(zip_list,sizeof(char *)*zip_list_size);

					zip_list[zip_list_size-1]=(char*)malloc(strlen(file_sig_tsr)+2);
					memset(zip_list[zip_list_size-1], 0, strlen(file_sig_tsr)+1);
					memcpy(zip_list[zip_list_size-1], file_sig_tsr, strlen(file_sig_tsr));


					free_gen_buf(&metadata_value);
				}

				/******************************************/
				/*	pobranie poswiadczenia dvcs podpisu	*/
				/******************************************/
				if ((strcmp(	resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[j]->OIDTableBuf,dvcs)==0) &&
							(resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[i]->myId==resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[j]->ownerId))
				{
					/*tmpANY.buf=(unsigned char*)resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[j]->AnyBuf;
					tmpANY.size=resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[j]->AnySize;
					metadata_value=(GenBuf_t *)malloc(sizeof(GenBuf_t));
					Any2Octetstring2GenBuf(&tmpANY,metadata_value);*/
					BMD_FOK(set_gen_buf2(resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[j]->AnyBuf,
					resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[j]->AnySize, &metadata_value));

					dvcs_no++;

					asprintf(&dir_dvcs,"%s_%li/%s_%li/",zip_signature_directory_name,sig_no, zip_dvcs_directory_name, dvcs_no);
					SOAP_FOK_LOGOUT(mkdir(dir_dvcs, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH));

					asprintf(&file_sig_dvcs,"%s%s.cpd.dvcs",dir_dvcs,file_sig2);
					bmd_save_buf(metadata_value, file_sig_dvcs);
					free_gen_buf(&metadata_value);

					zip_list_size++;
					zip_list=(char **)realloc(zip_list,sizeof(char *)*zip_list_size);

					zip_list[zip_list_size-1]=(char*)malloc(strlen(file_sig_dvcs)+2);
					memset(zip_list[zip_list_size-1], 0, strlen(file_sig_dvcs)+1);
					memcpy(zip_list[zip_list_size-1], file_sig_dvcs, strlen(file_sig_dvcs));

					free_gen_buf(&metadata_value);

					asprintf(&file_sig_dvcs_tsr,"%s",file_sig_dvcs);


					/************************************************************/
					/*	wyszukiwanie znacznikow czasu dla poswiadczenia dvcs	*/
					/************************************************************/
					for (k=0; k<resp_dtg_set->bmdDatagramSetTable[0]->no_of_pkiMetaData; k++)
					{
						if ((strcmp(	resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[k]->OIDTableBuf,tim_dvcs)==0) &&
									(resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[j]->myId==resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[k]->ownerId))
						{
							/*tmpANY.buf=(unsigned char*)resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[k]->AnyBuf;
							tmpANY.size=resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[k]->AnySize;
							metadata_value=(GenBuf_t *)malloc(sizeof(GenBuf_t));
							Any2Octetstring2GenBuf(&tmpANY,metadata_value);*/

							BMD_FOK(set_gen_buf2(resp_dtg_set->bmdDatagramSetTable[0]->
							pkiMetaData[k]->AnyBuf,
							resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[k]->AnySize,
							&metadata_value));


							asprintf(&file_sig_dvcs_tsr,"%s.tsr",file_sig_dvcs_tsr);
							bmd_save_buf(metadata_value, file_sig_dvcs_tsr);
							free_gen_buf(&metadata_value);

							zip_list_size++;
							zip_list=(char **)realloc(zip_list,sizeof(char *)*zip_list_size);

							zip_list[zip_list_size-1]=(char*)malloc(strlen(file_sig_dvcs_tsr)+2);
							memset(zip_list[zip_list_size-1], 0, strlen(file_sig_dvcs_tsr)+1);
							memcpy(zip_list[zip_list_size-1], file_sig_dvcs_tsr, strlen(file_sig_dvcs_tsr));


							free_gen_buf(&metadata_value);

						}
					}
				}
			}
		}
	}

	/************************/
	/*	spakowanie plikow	*/
	/************************/
	asprintf(&path_dest_zip, "%s.zip", resp_dtg_set->bmdDatagramSetTable[0]->protocolDataFilename->buf);
	SOAP_FOK_LOGOUT_CD_TMP(zipCompressFile(zip_list, zip_list_size, path_dest_zip));

	/************************/
	/*	ustawienie wyniku	*/
	/************************/
	(*output)=(struct bmd230__fileInfo *)soap_malloc(soap,sizeof(struct bmd230__fileInfo));
	memset(*output,0,sizeof(struct bmd230__fileInfo));

	/*	ustawienie nazwy pliku	*/
	(*output)->filename=(char *)soap_malloc(soap,strlen(path_dest_zip)+2);
	memset((*output)->filename, 0, strlen(path_dest_zip)+1);
	memmove((*output)->filename, path_dest_zip, strlen(path_dest_zip));

	/*	ustawienie pliku	*/
	(*output)->file=(struct bmd230__myBinaryDataType *)soap_malloc(soap,sizeof(struct bmd230__myBinaryDataType));
	(*output)->file->id = NULL;

	switch(type)
	{

	    case 0 : {
			/************************************************/
			/* wczytanie spakowanego pliku do genbufa       */
			/************************************************/

			free_gen_buf(&gb);
			bmd_load_binary_content(path_dest_zip,&gb);
			bmd_genbuf_get_buf(gb,&buf);
			bmd_genbuf_get_size(gb,&size);

			(*output)->file->__ptr=(unsigned char *)soap_malloc(soap,size+2);
			 memset((*output)->file->__ptr,0,size+1);
			(*output)->file->__size=size;
			memmove((*output)->file->__ptr,buf,size);

			(*output)->file->type = NULL;
			(*output)->file->options = NULL;

			break;
		     }

	    case 1 : {  /* Transmisja strumieniowa DIME */
			soap_set_omode(soap, SOAP_IO_CHUNK);
			fd = fopen(path_dest_zip,"r");
			(*output)->file->__ptr=(unsigned char *)fd;
			(*output)->file->__size=0;
			(*output)->file->type = "";
			(*output)->file->options = soap_dime_option(soap,0, (*output)->filename );

			break;
		     }

	}

	/******************************************/
	/*	usuwanie katalogow tymczasowych	*/
	/******************************************/
	for (i=0; i<sig_no; i++)
	{
		for (j=0; j<dvcs_no; j++)
		{
			asprintf(&path, "%s_%li/%s_%li/",zip_signature_directory_name,i+1,zip_dvcs_directory_name,j+1);
			remove_dir(path);
		}
		asprintf(&path, "%s_%li/",zip_signature_directory_name,i+1);
		remove_dir(path);
	}

	/******************************************/
	/*	powrot do katalogu pierwotnego	*/
	/******************************************/
	SOAP_FOK_LOGOUT(chdir(_GLOBAL_current_dir));

	/************************************************/
	/*	usuniecie glownego katalogu tymczasowego	*/
	/************************************************/
	remove_dir(tmp_location);

	/******************/
	/*	porzadki	*/
	/******************/
	bmd_logout(&bi, deserialisation_max_memory);
	bmd_info_destroy(&bi);
	bmd_end();

	bmd_lob_request_info_destroy(&li);
	bmd_datagramset_free(&dtg_set);
	bmd_datagramset_free(&resp_dtg_set);
	free_gen_buf(&gb);
	free(path); path=NULL;
	free(path_dest); path_dest=NULL;
	free(path_dest_zip); path_dest_zip=NULL;
	for (i=0; i<zip_list_size; i++)
	{
		free(zip_list[i]); zip_list[i]=NULL;
	}
	free(zip_list); zip_list=NULL;
	free_gen_buf(&metadata_value);
	free(file_tsr); file_tsr=NULL;
	free(file_sig); file_sig=NULL;
	free(file_sig2); file_sig2=NULL;
	free(file_sig_tsr); file_sig2=NULL;
	free(file_sig_dvcs); file_sig_dvcs=NULL;
	free(file_sig_dvcs_tsr); file_sig_dvcs=NULL;
	free(dir_sig); file_sig_dvcs=NULL;
	free(dir_dvcs); file_sig_dvcs=NULL;
	free(tmp_dir); tmp_dir=NULL;

	free(zip_signature_directory_name); zip_signature_directory_name=NULL;
	free(zip_dvcs_directory_name); zip_dvcs_directory_name=NULL;
	free(zip_attachments_directory_name); zip_attachments_directory_name=NULL;
	free(tmp_location); tmp_location=NULL;

	PRINT_SOAP("Status:\t\trequest served correctly\n");
	return SOAP_OK;
}

/***********************************************/
/* obsluga zadania pobrania paczki archiwalnej */
/***********************************************/


int bmd230__bmdGetArchPack(	struct soap *soap,
                        struct xsd__base64Binary *cert,
			char *roleName,
			char *groupName,
			char *userClassId,
                        long int id,
                        enum trans_t type,
                        struct bmd230__fileInfo **output )
{

    /* ------------------------------------------------------------- */

	long                 size           =                      0;
	char*                buf            =                   NULL;
	bmd_info_t*          bi             =                   NULL;
	lob_request_info_t*  li             =                   NULL;
	bmdDatagram_t*       dtg            =                   NULL;
	bmdDatagramSet_t*    dtg_set        =                   NULL;
	bmdDatagramSet_t*    resp_dtg_set   =                   NULL;
	long                 max_datagram_in_set_transmission    = 0;
	long 		     deserialisation_max_memory		= 0;
	GenBuf_t*            gbzip          =                   NULL;
	char*                path           =                   NULL;
	char*                path_dest      =                   NULL;
	wchar_t*             wchar_temp     =                   NULL;
	char*                tmp_dir        =                   NULL;
	char*                archPackConf   =                   NULL;
	char*                tmp_location   =                   NULL;
	long                 longRet        =                      0;
	char*                                   path_dest_zip = NULL;
	char*                                         zip_dir = NULL;
	char*                                        zip_file = NULL;
	int                  fileout        =                      0;
	int                  filein         =                      0;

       GenBuf_t                                                  gb;
       char*                tmp_file       = {"soap_get_file_zip_"};
       FILE*                fd             =                   NULL;

long retVal=0;
char *derUserCert=NULL;
long derUserCertLen=0;
char* paramCert=NULL;
long paramCertLen=0;
    /* ------------------------------------------------------------- */

 	PRINT_SOAP("Request:\tget arch pack of file with id %li in zip pack\n",id);
 	/******************************/
 	/*	walidacja parametrow	*/
 	/******************************/
 	if(id<=0) {SOAP_FOK(BMDSOAP_SERVER_INVALID_FILE_ID);}

 	/******************************************/
 	/*	wczytanie ustawien konfiguracyjnych	*/
 	/******************************************/
 	SOAP_FOK(bmdconf_get_value(_GLOBAL_konfiguracja,"soap","temp_dir",&tmp_dir));
 	SOAP_FOK(bmdconf_get_value(_GLOBAL_konfiguracja,"soap","archpack_conf",&archPackConf));

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

// 	/************************/
// 	/*	logowanie do bmd	*/
// 	/************************/
  	SOAP_FOK(PrepareDataForBMDLoginAndRequest(&bi,
				(_GLOBAL_soap_wssecurity_enabled == 1  ? derUserCert : paramCert),
				(_GLOBAL_soap_wssecurity_enabled == 1  ? derUserCertLen : paramCertLen),
				roleName, groupName, deserialisation_max_memory));
	free(derUserCert); derUserCert=NULL;
	paramCert=NULL; paramCertLen=0;
//
// 	/******************************/
// 	/*	stworzenie zadania	*/
// 	/******************************/
 	SOAP_FOK_LOGOUT(bmd_lob_request_info_create(&li));
//
// 	/************************************/
// 	/*	odczytanie katalogu biezacego	*/
// 	/************************************/
	 memset(_GLOBAL_current_dir, 0 ,256);
	 if (getcwd(_GLOBAL_current_dir,255)==NULL)
	  {
             bmd_logout(&bi, deserialisation_max_memory);
             bmd_end();
             PRINT_ERROR("SOAPSERVERERR %s\n","Error in getting current directory");
             return soap_receiver_fault(soap,"Soap server error","Error in getting current directory");
          }
//
// 	/************************************************************************************************/
// 	/*	stworzenie katalogu tymczasowego dla pobranych danych z serwera i przejscie do niego	*/
// 	/************************************************************************************************/
 	asprintf(&tmp_location, "%s%s%i/", tmp_dir, tmp_file, (int)getpid());
 	SOAP_FOK_LOGOUT(mkdir(tmp_location, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH));
 	SOAP_FOK_LOGOUT_TMP(chdir(tmp_location));
//
// 	/************************************/
// 	/*	okreslenie pliku tymczasowego	*/
// 	/************************************/
 	asprintf(&path, "%s%i", tmp_file, (int)getpid());
 	//char2wcs(path,&wchar_temp);
 	SOAP_FOK_LOGOUT_CD_TMP(bmd_lob_request_info_add_filename(path, &li, BMD_RECV_LOB,1000000000));
//
// 	/******************************/
// 	/*	stworzenie datagramu	*/
// 	/******************************/
 	SOAP_FOK_LOGOUT_CD_TMP(bmd_datagram_create(BMD_DATAGRAM_GET_CGI_LO,&dtg));
	if (userClassId!=NULL)
	{
		SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(	OID_ACTION_METADATA_CERT_CHOSEN_CLASS_ID, userClassId, &dtg));
	}
//
// 	/************************************/
// 	/*	ustawienie id w datagramie	*/
// 	/************************************/
 	SOAP_FOK_LOGOUT_CD_TMP(bmd_datagram_set_id((const long int *)&id,&dtg));
//
// 	/************************************************/
// 	/*	dodanie datagramu do zestawu datagramow	*/
// 	/************************************************/
 	SOAP_FOK_LOGOUT_CD_TMP(bmd_datagram_add_to_set(dtg,&dtg_set));
//
// 	/************************************/
// 	/*	wyslanie zadania lobowego	*/
// 	/************************************/
        SOAP_FOK_LOGOUT_CD_TMP(bmd_send_request_lo(bi,dtg_set, max_datagram_in_set_transmission, &resp_dtg_set, li, deserialisation_max_memory));

 	SOAP_FOK_LOGOUT_CD_TMP(resp_dtg_set->bmdDatagramSetTable[0]->datagramStatus);
// 	/************************************************************************/
// 	/*	przepisanie pobranego pliku z pliku tymczasowego do docelowego	*/
// 	/************************************************************************/

//      /************************************************************************/
//  /* Symulacja strumieniowej transmisji */

	gb.buf = calloc(PACK, sizeof(char));

	if ( gb.buf == NULL )
	 {
	    PRINT_ERROR("SOAPSERVERERR %s\n","Error memory allocation");
 	    return soap_receiver_fault(soap,"Soap server error","Error memory allocation");
	 }

	gb.size = 0;

	longRet = makeArchPack(archPackConf, resp_dtg_set->bmdDatagramSetTable[0], \
                		&gb, startblk, &zip_dir, &zip_file );


  	filein = open( path, O_RDONLY|0100000, 0666 );

 	do{
  	    gb.size = read(filein, gb.buf, PACK);

  	    if ( gb.size <= 0 ) break;

    	    longRet = makeArchPack(archPackConf, resp_dtg_set->bmdDatagramSetTable[0], \
        	    		   &gb, continueblk, &zip_dir, &zip_file );

		addData2ArchpackFile(&gb, fileout);

  	  }while(1);

  	close(filein);

  	longRet = makeArchPack(archPackConf, resp_dtg_set->bmdDatagramSetTable[0], \
             		        &gb, stopblk, &zip_dir, &zip_file );

	closeArchpackFile( &fileout );
        unlink(path);

// 	/************************/
// 	/* ustawienie wyniku    */
// 	/************************/

	(*output)=(struct bmd230__fileInfo *)soap_malloc(soap,sizeof(struct bmd230__fileInfo));
	memset(*output,0,sizeof(struct bmd230__fileInfo));

// 	/*	ustawienie nazwy pliku	*/
 	(*output)->filename=(char *)soap_malloc(soap,strlen(zip_file)+2);
 	memset((*output)->filename, 0, strlen(zip_file)+1);
 	memmove((*output)->filename, zip_file, strlen(zip_file));
//
// 	/*	ustawienie pliku	*/
 	//(*output)->file=(struct xsd__base64Binary *)soap_malloc(soap,sizeof(struct xsd__base64Binary));

/*	ustawienie pliku	*/
	(*output)->file=(struct bmd230__myBinaryDataType *)soap_malloc(soap,sizeof(struct bmd230__myBinaryDataType));
	(*output)->file->id = NULL;

//
        asprintf(&path_dest_zip,"%s/%s", zip_dir, zip_file);

        switch(type)
	{

	    case 0 : {
			/************************************************/
			/* wczytanie spakowanego pliku do genbufa       */
			/************************************************/

	                bmd_load_binary_content(path_dest_zip,&gbzip);
 	                bmd_genbuf_get_buf(gbzip,&buf);
 	                bmd_genbuf_get_size(gbzip,&size);

 	                (*output)->file->__ptr=(unsigned char *)soap_malloc(soap,size+2);
 	                memset((*output)->file->__ptr,0,size+1);
 	                (*output)->file->__size=size;
 	                memmove((*output)->file->__ptr,buf,size);

			(*output)->file->type = NULL;
			(*output)->file->options = NULL;

			break;
		     }

	    case 1 : {  /* Transmisja strumieniowa DIME */
			soap_set_omode(soap, SOAP_IO_CHUNK);
			fd = fopen(path_dest_zip,"r");
			(*output)->file->__ptr=(unsigned char *)fd;
			(*output)->file->__size=0;
			(*output)->file->type = "";
			(*output)->file->options = soap_dime_option(soap,0, (*output)->filename );

			break;
		     }

	}

//
// 	/******************************************/
// 	/*	usuwanie katalogow tymczasowych	*/
// 	/******************************************/

// 	/******************************************/
// 	/*	powrot do katalogu pierwotnego	*/
// 	/******************************************/
 	SOAP_FOK_LOGOUT(chdir(_GLOBAL_current_dir));
//
// 	/************************************************/
// 	/*	usuniecie glownego katalogu tymczasowego	*/
// 	/************************************************/
 	remove_dir(tmp_location);
//
// 	/******************/
// 	/*	porzadki	*/
// 	/******************/
        bmd_logout(&bi, deserialisation_max_memory);
 	bmd_info_destroy(&bi);
 	bmd_end();
//
 	bmd_lob_request_info_destroy(&li);
 	bmd_datagramset_free(&dtg_set);
 	bmd_datagramset_free(&resp_dtg_set);
 	free_gen_buf(&gbzip);
 	free(path); path=NULL;
 	free(path_dest); path_dest=NULL;
 	free(path_dest_zip); path_dest_zip=NULL;
 	free(wchar_temp); wchar_temp=NULL;

 	PRINT_SOAP("Status:\t\trequest served correctly\n");

	return SOAP_OK;
 }




int bmd230__bmdGetArchPackInChunk(	struct soap *soap,
				struct xsd__base64Binary *cert,
				char *roleName,
				char *groupName,
				char *userClassId,
				long int id,
				enum chunkCntr_t cntrFlag,
				struct bmd230__archPackOut *input,
				struct bmd230__archPackOut **output)
{

    /* ------------------------------------------------------------- */

       unsigned int         size           =                      0;
       unsigned char*       buf            =                   NULL;
       bmd_info_t*          bi             =                   NULL;
       lob_request_info_t*  li             =                   NULL;
       bmdDatagram_t*       dtg            =                   NULL;
       bmdDatagramSet_t*    dtg_set        =                   NULL;
       bmdDatagramSet_t*    resp_dtg_set   =                   NULL;
       char*                path           =                   NULL;
       char*                path_dest      =                   NULL;
       wchar_t*             wchar_temp     =                   NULL;
       char*                tmp_dir        =                   NULL;
       char*                archPackConf   =                   NULL;
       char*         	    tmp_location   =                   NULL;
       long                 longRet        =                      0;
       char*                path_dest_zip  =                   NULL;
       char*                zip_dir        =                   NULL;
       char*                zip_file       =                   NULL;
       int                  fileout        =                      0;
       int                  filein         =                      0;
       GenBuf_t                                                  gb;
       char*                tmp_file       = {"soap_get_file_zip_"};
       int		    archPackFile   =                      0;
       off_t                offset         =                      0;
       off_t		    fsize 	   =                      0;
	long                 max_datagram_in_set_transmission    = 0;
	long			deserialisation_max_memory	= 0;

char *derUserCert=NULL;
long derUserCertLen=0;
long retVal=0;
char* paramCert=NULL;
long paramCertLen=0;
    /* ------------------------------------------------------------- */



    PRINT_SOAP("Request:\tget arch pack of file with id %li in zip pack\n",id);
    /******************************/
    /*	walidacja parametrow	*/
    /******************************/
    if(id<=0) {SOAP_FOK(BMDSOAP_SERVER_INVALID_FILE_ID);}


    switch(cntrFlag)
     {
       case start : {

                      if ( input )
                       {
// 	                  /******************************************/
// 	                  /*	powrot do katalogu pierwotnego	    */
// 	                  /******************************************/

                          if (input->currDir)
                           {
 	                      SOAP_FOK_LOGOUT(chdir(input->currDir));
                           }
//
// 	                  /************************************************/
// 	                  /*	usuniecie glownego katalogu tymczasowego  */
// 	                  /************************************************/

                          if (input->tmpLocation)
                           {
                              remove_dir(input->tmpLocation);
                           }

                          PRINT_ERROR("SOAPSERVERERR %s\n","Multiple start function call");
                          return soap_receiver_fault(soap,"Soap server error","Multiple start function call");

                        }

                      /*********************************************/
                      /*	wczytanie ustawien konfiguracyjnych */
                      /*********************************************/

                      SOAP_FOK(bmdconf_get_value(_GLOBAL_konfiguracja,"soap","temp_dir",&tmp_dir));
 	              SOAP_FOK(bmdconf_get_value(_GLOBAL_konfiguracja,"soap","archpack_conf",&archPackConf));

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
//
// 	              /************************/
// 	              /*   logowanie do bmd   */
// 	              /************************/
  	              SOAP_FOK(PrepareDataForBMDLoginAndRequest(&bi,
					(_GLOBAL_soap_wssecurity_enabled == 1  ? derUserCert : paramCert),
					(_GLOBAL_soap_wssecurity_enabled == 1  ? derUserCertLen : paramCertLen),
					roleName, groupName, deserialisation_max_memory));
			free(derUserCert); derUserCert=NULL;
			paramCert=NULL; paramCertLen=0;
//
//    	              /******************************/
// 	              /*  stworzenie zadania	    */
// 	              /******************************/
 	              SOAP_FOK_LOGOUT(bmd_lob_request_info_create(&li));
//
// 	              /************************************/
// 	              /*  odczytanie katalogu biezacego   */
// 	              /************************************/
	              memset(_GLOBAL_current_dir, 0 ,256);

	              if (getcwd(_GLOBAL_current_dir,255)==NULL)
                       {
 			  bmd_logout(&bi, deserialisation_max_memory);
                          bmd_end();
                          PRINT_ERROR("SOAPSERVERERR %s\n","Error in getting current directory");
                          return soap_receiver_fault(soap,"Soap server error","Error in getting current directory");
                       }
//
// 	              /************************************************************************************************/
// 	              /*    stworzenie katalogu tymczasowego dla pobranych danych z serwera i przejscie do niego      */
// 	              /************************************************************************************************/

                      asprintf(&tmp_location, "%s%s%i/", tmp_dir, tmp_file, (int)getpid());
 	              SOAP_FOK_LOGOUT(mkdir(tmp_location, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH));
 	              SOAP_FOK_LOGOUT_TMP(chdir(tmp_location));
//
// 	              /************************************/
// 	              /*  okreslenie pliku tymczasowego	  */
// 	              /************************************/

 	              asprintf(&path, "%s%i", tmp_file, (int)getpid());
 	              //char2wcs(path,&wchar_temp);
 	              SOAP_FOK_LOGOUT_CD_TMP(bmd_lob_request_info_add_filename(path, &li, BMD_RECV_LOB,1000000000));
//
// 	              /******************************/
// 	              /*  stworzenie datagramu      */
// 	              /******************************/

			SOAP_FOK_LOGOUT_CD_TMP(bmd_datagram_create(BMD_DATAGRAM_GET_CGI_LO,&dtg));
			if (userClassId!=NULL)
			{
				SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(	OID_ACTION_METADATA_CERT_CHOSEN_CLASS_ID, userClassId, &dtg));
			}

// 	              /************************************/
// 	              /*   ustawienie id w datagramie	  */
// 	              /************************************/
 	              SOAP_FOK_LOGOUT_CD_TMP(bmd_datagram_set_id((const long int *)&id,&dtg));
//
// 	              /************************************************/
// 	              /*  dodanie datagramu do zestawu datagramow     */
// 	              /************************************************/
                      SOAP_FOK_LOGOUT_CD_TMP(bmd_datagram_add_to_set(dtg,&dtg_set));
//
// 	              /************************************/
//                    /*  wyslanie zadania lobowego       */
// 	              /************************************/

			SOAP_FOK_LOGOUT_CD_TMP(bmd_send_request_lo(bi,dtg_set, max_datagram_in_set_transmission, &resp_dtg_set, li, deserialisation_max_memory));


 	              SOAP_FOK_LOGOUT_CD_TMP(resp_dtg_set->bmdDatagramSetTable[0]->datagramStatus);

// 	              /************************************************************************/
// 	              /*  przepisanie pobranego pliku z pliku tymczasowego do docelowego      */
// 	              /************************************************************************/

//                    /************************************************************************/
//                    /* Symulacja strumieniowej transmisji */

	              gb.buf = calloc(PACK, sizeof(char));

	              if ( gb.buf == NULL )
	               {
	                 PRINT_ERROR("SOAPSERVERERR %s\n","Error memory allocation");
 	                 return soap_receiver_fault(soap,"Soap server error","Error memory allocation");
	               }

	              gb.size = 0;

	              longRet = makeArchPack(archPackConf, resp_dtg_set->bmdDatagramSetTable[0], \
                		             &gb, startblk, &zip_dir, &zip_file );

  	              filein = open( path, O_RDONLY|0100000, 0666 );

 	              do{
  	                   gb.size = read(filein, gb.buf, PACK);

  	                   if ( gb.size <= 0 ) break;

    	                   longRet = makeArchPack(archPackConf, resp_dtg_set->bmdDatagramSetTable[0], \
        	    	    	                  &gb, continueblk, &zip_dir, &zip_file );

  	                   addData2ArchpackFile(&gb, fileout);

  	 	        }while(1);

  	               close(filein);

  	              longRet = makeArchPack(archPackConf, resp_dtg_set->bmdDatagramSetTable[0], \
             		                     &gb, stopblk, &zip_dir, &zip_file );

	              closeArchpackFile( &fileout );
                      unlink(path);

// 	              /************************/
// 	              /* ustawienie wyniku    */
// 	              /************************/

                      (*output)=(struct bmd230__archPackOut *)soap_malloc(soap,sizeof(struct bmd230__archPackOut));
	              memset(*output,0,sizeof(struct bmd230__archPackOut));

// 	              /* ustawienie nazwy pliku	*/
 	              (*output)->filename=(char *)soap_malloc(soap,strlen(zip_file)+2);
 	              memset((*output)->filename, 0, strlen(zip_file)+1);
 	              memmove((*output)->filename, zip_file, strlen(zip_file));
//
// 	              /* ustawienie pliku */
 	              (*output)->file=(struct xsd__base64Binary *)soap_malloc(soap,sizeof(struct xsd__base64Binary));
 	              (*output)->file->__ptr = NULL;
 	              (*output)->file->__size=0;

	              asprintf(&path_dest_zip,"%s/%s", zip_dir, zip_file);

                    /************************************/
// 	              /* ustawienie zmiennych sterujacych */
// 	              /************************************/

                    /* zapamietanie tymczasowej lokalizacji */
 	              (*output)->tmpLocation = (char *)soap_malloc(soap,strlen(tmp_location)+2);
 	              memset((*output)->tmpLocation, 0, strlen(tmp_location)+1);
 	              memmove((*output)->tmpLocation, tmp_location, strlen(tmp_location));

                    /* zapamietanie katalogu biezacego */
 	              (*output)->currDir = (char *)soap_malloc(soap,strlen(_GLOBAL_current_dir)+2);
 	              memset((*output)->currDir, 0, strlen(_GLOBAL_current_dir)+1);
 	              memmove((*output)->currDir, _GLOBAL_current_dir, strlen(_GLOBAL_current_dir));

                      /* zapamiętanie lokalizacji paczki archiwalnej  */
 	              (*output)->pathDestZip = (char *)soap_malloc(soap,strlen(path_dest_zip)+2);
 	              memset((*output)->pathDestZip, 0, strlen(path_dest_zip)+1);
 	              memmove((*output)->pathDestZip, path_dest_zip, strlen(path_dest_zip));


			bmd_logout(&bi, deserialisation_max_memory);
 	              bmd_info_destroy(&bi);
 	              bmd_end();
                      bmd_lob_request_info_destroy(&li);
 	              bmd_datagramset_free(&dtg_set);
 	              bmd_datagramset_free(&resp_dtg_set);

                      break;

       }

       case iter : {

			(*output)=(struct bmd230__archPackOut *)soap_malloc(soap,sizeof(struct bmd230__archPackOut));
			memset(*output,0,sizeof(struct bmd230__archPackOut));

// 	              /* ustawienie nazwy pliku	*/
			(*output)->filename=(char *)soap_malloc(soap,strlen(input->filename)+2);
			memset((*output)->filename, 0, strlen(input->filename)+1);
			memmove((*output)->filename, input->filename, strlen(input->filename));

                      /************************************/
// 	              /* ustawienie zmiennych sterujacych */
// 	              /************************************/

                      /* zapamietanie tymczasowej lokalizacji */
			(*output)->tmpLocation = (char *)soap_malloc(soap,strlen(input->tmpLocation)+2);
			memset((*output)->tmpLocation, 0, strlen(input->tmpLocation)+1);
			memmove((*output)->tmpLocation, input->tmpLocation, strlen(input->tmpLocation));

                      /* zapamietanie katalogu biezacego */
			(*output)->currDir = (char *)soap_malloc(soap,strlen(input->currDir)+2);
			memset((*output)->currDir, 0, strlen(input->currDir)+1);
			memmove((*output)->currDir, input->currDir, strlen(input->currDir));

                      /* zapamietanie lokalizacji paczki archiwalnej  */
			(*output)->pathDestZip = (char *)soap_malloc(soap,strlen(input->pathDestZip)+2);
			memset((*output)->pathDestZip, 0, strlen(input->pathDestZip)+1);
			memmove((*output)->pathDestZip, input->pathDestZip, strlen(input->pathDestZip));

			archPackFile = open( input->pathDestZip, O_RDONLY | 0100000, 0666 );

			if ( archPackFile == -1 )
			{
				PRINT_ERROR("SOAPSERVERERR %s\n","Error open file");
				return soap_receiver_fault(soap,"Soap server error","Error open file");
			}

			fsize = lseek(archPackFile, 0L, 2);

			if (input->pos >= fsize)
			{

			(*output)->file=(struct xsd__base64Binary *)soap_malloc(soap,sizeof(struct xsd__base64Binary));

		  	(*output)->file->__ptr = (unsigned char *)soap_malloc(soap,1);
			(*output)->file->__size = 0;

                          /******************************************/
// 	                  /*	powrot do katalogu pierwotnego	    */
// 	                  /******************************************/
 	                  SOAP_FOK_LOGOUT(chdir(input->currDir));
//
// 	                  /************************************************/
// 	                  /*	usuniecie glownego katalogu tymczasowego  */
// 	                  /************************************************/
 	                  remove_dir(input->tmpLocation);

			  break;
			}

			offset = lseek(archPackFile, input->pos, 0);

			buf = (unsigned char*)calloc(PACK, sizeof(unsigned char));

			if ( buf == NULL )
			{
				PRINT_ERROR("SOAPSERVERERR %s\n","Error memory allocation");
				return soap_receiver_fault(soap,"Soap server error","Error memory allocation");
			}

			size = read( archPackFile, buf, PACK );

  			if ( size <= 0 )
			{
			  (*output)->file=(struct xsd__base64Binary *)soap_malloc(soap,sizeof(struct xsd__base64Binary));
			  (*output)->file->__ptr = (unsigned char *)soap_malloc(soap,1);
			  (*output)->file->__size = 0;

                          /******************************************/
// 	                  /*	powrot do katalogu pierwotnego	    */
// 	                  /******************************************/
 	                  SOAP_FOK_LOGOUT(chdir(input->currDir));
//
// 	                  /************************************************/
// 	                  /*	usuniecie glownego katalogu tymczasowego  */
// 	                  /************************************************/
 	                  remove_dir(input->tmpLocation);
                       	}
		       else
		       {
// 	              	 /* ustawienie pliku */
			(*output)->file=(struct xsd__base64Binary *)soap_malloc(soap,sizeof(struct xsd__base64Binary));
			(*output)->file->__ptr = (unsigned char *)soap_malloc(soap,size+2);
			(*output)->file->__size = size;
			memmove((*output)->file->__ptr,buf,size);

			(*output)->pos = input->pos + size;

                       }

		close(archPackFile);
      }
   }
   	PRINT_SOAP("Status:\t\trequest served correctly\n");

	free0(path);
	free0(path_dest);
	free0(wchar_temp);
	free0(tmp_location);
	free0(path_dest_zip);
	free0(buf);


 return SOAP_OK;


}

/***************************************************************************/
/* obsluga zadania pobrania paczki fakturowej po id i metadanej dodatkowej */
/***************************************************************************/
int bmd230__getInvoicePackById(	struct soap *soap,
				struct xsd__base64Binary *cert,
				char *roleName,
				char *groupName,
				char *userClassId,
				long int id,
				struct bmd230__fileInfo **output)
{
long i						= 0;
long j						= 0;
long k						= 0;
long status					= 0;
long sig_no					= 0;
long dvcs_no					= 0;
long zip_list_size				= 0;
long press_timeout				= 0;
long add_pdf_to_zip				= 0;
long add_pdf_to_archive				= 0;
long max_datagram_in_set_transmission		= 0;
long deserialisation_max_memory			= 0;
bmd_info_t *bi					= NULL;
bmdDatagramSet_t *dtg_set			= NULL;
bmdDatagramSet_t *dtg_set2			= NULL;
bmdDatagramSet_t *dtg_set3			= NULL;
bmdDatagramSet_t *dtg_set4			= NULL;
bmdDatagramSet_t *resp_dtg_set			= NULL;
bmdDatagramSet_t *resp_dtg_set2			= NULL;
bmdDatagramSet_t *resp_dtg_set3			= NULL;
bmdDatagramSet_t *resp_dtg_set4			= NULL;
GenBuf_t *gb					= NULL;
GenBuf_t *metadata_value			= NULL;
char *path					= NULL;
char *path_dest					= NULL;
char *path_dest2				= NULL;
char *path_dest4				= NULL;
char *path_dest_zip				= NULL;
char **zip_list					= NULL;
char *file_tsr					= NULL;
char *file_sig				 	= NULL;
char *file_sig2					= NULL;
char *file_sig_tsr				= NULL;
char *file_sig_dvcs				= NULL;
char *file_sig_dvcs_tsr				= NULL;
char *dir_sig					= NULL;
char *dir_dvcs					= NULL;
char *tmp_dir					= NULL;
char *tmp_location				= NULL;
char *press_in_dir				= NULL;
char *press_out_dir				= NULL;
char *press_in_filename				= NULL;
char *press_out_filename			= NULL;
char *id_str					= NULL;
DIR *press_dir_ptr				= NULL;
struct dirent *dirp				= NULL;
char *src					= NULL;
char *dest					= NULL;
char *mtd_oid_str				= NULL;
char *press_date_metadata			= NULL;
char *press_date_separator_in			= NULL;
char *press_date_separator_out			= NULL;
char *pdf_file_type				= NULL;
char *xml_file_type				= NULL;

char *zip_signature_directory_name		= NULL;
char *zip_dvcs_directory_name			= NULL;
char *zip_attachments_directory_name		= NULL;

char tmp_file[]					= {"soap_get_pack_"};

char visible_oid[]			= {OID_SYS_METADATA_CRYPTO_OBJECTS_VISIBLE};	/* widzialnosc pliku */
char file_type_oid[]			= {OID_SYS_METADATA_CRYPTO_OBJECTS_FILE_TYPE};	/* typ pliku */
char coresonding_oid[]			= {OID_SYS_METADATA_CRYPTO_OBJECTS_CORESPONDING_ID};	/* plik korespondujacy */


char timestamp[]	= {OID_PKI_METADATA_TIMESTAMP};
char signature[]	= {OID_PKI_METADATA_SIGNATURE};
char signature_pades[]	= {OID_PKI_METADATA_SIGNATURE_PADES};
char dvcs[]		= {OID_PKI_METADATA_DVCS};
char tim_sig[]		= {OID_PKI_METADATA_TIMESTAMP_FROM_SIG};
char tim_dvcs[]		= {OID_PKI_METADATA_TIMESTAMP_FROM_DVCS};

char *derUserCert=NULL;
long derUserCertLen=0;
long retVal=0;
char* paramCert=NULL;
long paramCertLen=0;

	/******************************************/
	/*	wczytanie ustawien konfiguracyjnych	*/
	/******************************************/
	SOAP_FOK(bmdconf_get_value(_GLOBAL_konfiguracja,"soap","temp_dir",&tmp_dir));
	bmdconf_get_value(_GLOBAL_konfiguracja,"convert","pdf_file_type",&pdf_file_type);
	bmdconf_get_value(_GLOBAL_konfiguracja,"convert","xml_file_type",&xml_file_type);
	bmdconf_get_value(_GLOBAL_konfiguracja,"convert","press_in_dir",&press_in_dir);
	bmdconf_get_value(_GLOBAL_konfiguracja,"convert","press_out_dir",&press_out_dir);
	bmdconf_get_value(_GLOBAL_konfiguracja,"convert","press_in_filename",&press_in_filename);
	bmdconf_get_value(_GLOBAL_konfiguracja,"convert","press_out_filename",&press_out_filename);
	bmdconf_get_value(_GLOBAL_konfiguracja,"convert","press_date_metadata",&press_date_metadata);
	bmdconf_get_value(_GLOBAL_konfiguracja,"convert","press_date_separator_in",&press_date_separator_in);
	bmdconf_get_value(_GLOBAL_konfiguracja,"convert","press_date_separator_out",&press_date_separator_out);
	bmdconf_get_value_int(_GLOBAL_konfiguracja,"convert","press_timeout",&press_timeout);
	SOAP_FOK(bmdconf_get_value_int(_GLOBAL_konfiguracja,"convert","add_pdf_to_zip",&add_pdf_to_zip));
	SOAP_FOK(bmdconf_get_value_int(_GLOBAL_konfiguracja,"convert","add_pdf_to_archive",&add_pdf_to_archive));
	SOAP_FOK(bmdconf_get_value(_GLOBAL_konfiguracja,"zip_package","zip_signature_directory_name",&zip_signature_directory_name));
	SOAP_FOK(bmdconf_get_value(_GLOBAL_konfiguracja,"zip_package","zip_dvcs_directory_name",&zip_dvcs_directory_name));
	SOAP_FOK(bmdconf_get_value(_GLOBAL_konfiguracja,"zip_package","zip_attachments_directory_name",&zip_attachments_directory_name));


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
	

	/************************************/
	/*	odczytanie katalogu biezacego	*/
	/************************************/
	memset(_GLOBAL_current_dir, 0 ,256);
	if (getcwd(_GLOBAL_current_dir,255)==NULL)
	{
		SOAP_FOK_LOGOUT(BMDSOAP_SERVER_GCWD_ERROR);
	}

	/************************************************************************************************/
	/*	stworzenie katalogu tymczasowego dla pobranych danych z serwera i przejscie do niego	*/
	/************************************************************************************************/
	asprintf(&tmp_location, "%s%s%li/", tmp_dir, tmp_file, (long)getpid());
	SOAP_FOK_LOGOUT(mkdir(tmp_location, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH));
	SOAP_FOK_LOGOUT(chdir(tmp_location));

	/***************************/
	/* POBRANIE PLIKU GLOWNEGO */
	/***************************/
	asprintf(&id_str, "%li", id);

	SOAP_FOK_LOGOUT_CD_TMP(createDatagram(	userClassId,
						(char *[]){OID_SYS_METADATA_CRYPTO_OBJECTS_ID},
						(char *[]){id_str},
						1,
						BMD_DATAGRAM_GET_CGI_LO,
						BMD_ONLY_GROUP_DOC,
						BMD_QUERY_EQUAL,
						&dtg_set));
	free0(id_str);

	/******************************************/
	/*	ustawienie powodu pobrania faktury	*/
	/******************************************/
// 	if ((getReason!=NULL) && (strlen(getReason)>0))
// 	{
// 		dtg_set->bmdDatagramSetTable[0]->protocolData=(GenBuf_t *)malloc(sizeof(GenBuf_t));
// 		dtg_set->bmdDatagramSetTable[0]->protocolData->buf=(char *)malloc(strlen(getReason)+2);
// 		memset(dtg_set->bmdDatagramSetTable[0]->protocolData->buf, 0, strlen(getReason)+1);
// 		memcpy(dtg_set->bmdDatagramSetTable[0]->protocolData->buf, getReason, strlen(getReason));
// 		dtg_set->bmdDatagramSetTable[0]->protocolData->size=strlen(getReason);
// 	}

	asprintf(&path, "%s%li", tmp_file, (long)getpid());
	SOAP_FOK_LOGOUT_CD_TMP(getFileFromBMD(bi, dtg_set, &resp_dtg_set, path));

	/************************************************************************************************************/
	/*	przepisanie pobranego pliku z pliku tymczasowego do docelowego i dopisanie do listy do spakowania	*/
	/************************************************************************************************************/
	asprintf(&path_dest, "%s", resp_dtg_set->bmdDatagramSetTable[0]->protocolDataFilename->buf);
	fileMove(path,path_dest);

	if (path_dest!=NULL)
	{
		zip_list_size++;
		zip_list=(char **)realloc(zip_list,sizeof(char *)*zip_list_size);

		zip_list[zip_list_size-1]=(char*)malloc(strlen(path_dest)+2);
		memset(zip_list[zip_list_size-1], 0, strlen(path_dest)+1);
		memcpy(zip_list[zip_list_size-1], path_dest, strlen(path_dest));
	}

	if (xml_file_type!=NULL)
	{
		/************************************************************************************************/
		/*						POBRANIE PLIKU KORESPONDUJACEGO XML'A				*/
		/************************************************************************************************/
		SOAP_FOK_LOGOUT_CD_TMP(	createDatagram(	userClassId,
							(char *[]){visible_oid, file_type_oid, coresonding_oid},
							(char *[]){"0", xml_file_type, (char *)resp_dtg_set->bmdDatagramSetTable[0]->protocolDataFileId->buf},
							3,
							BMD_DATAGRAM_GET_CGI_LO,
							BMD_ONLY_GROUP_DOC,
							BMD_QUERY_EQUAL,
							&dtg_set4));

		asprintf(&path, "%s%li", tmp_file, (long)getpid());
		status=getFileFromBMD(bi, dtg_set4, &resp_dtg_set4, path);
		if (status<BMD_OK)
		{
			if (status==BMD_SQL_GET_CGI_NOT_FOUND_ERROR)
			{
				SOAP_FOK_LOGOUT_CD_TMP(BMDSOAP_SERVER_XML_FILE_NOT_FOUND);
			}
			else
			{
				SOAP_FOK_LOGOUT_CD_TMP(0);
				//SOAP_FOK_LOGOUT_CD_TMP(status);
			}
		}

		/************************************************************************************/
		/*	jesli plik zostal pobrany zapisujemy go i dopisujemy do listy do spakowania	*/
		/************************************************************************************/
		if (resp_dtg_set4->bmdDatagramSetTable[0]->protocolDataFilename!=NULL)
		{
			if (	(strcmp(zip_attachments_directory_name,".")!=0) &&
				(strcmp(zip_attachments_directory_name,"")!=0) )
			{
				SOAP_FOK_LOGOUT_CD_TMP(mkdir(zip_attachments_directory_name, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH));
				asprintf(&path_dest4, "%s/%s", zip_attachments_directory_name, resp_dtg_set4->bmdDatagramSetTable[0]->protocolDataFilename->buf);
			}
			else
			{
				asprintf(&path_dest4, "%s", resp_dtg_set4->bmdDatagramSetTable[0]->protocolDataFilename->buf);
			}
			fileMove(path,path_dest4);

			if (path_dest4!=NULL)
			{
				zip_list_size++;
				zip_list=(char **)realloc(zip_list,sizeof(char *)*zip_list_size);

				zip_list[zip_list_size-1]=(char*)malloc(strlen(path_dest4)+2);
				memset(zip_list[zip_list_size-1], 0, strlen(path_dest4)+1);
				memcpy(zip_list[zip_list_size-1], path_dest4, strlen(path_dest4));
			}
		}
	}

	if ((pdf_file_type!=NULL) && (add_pdf_to_zip==1))
	{
		/************************************************************************************************/
		/*						POBRANIE PLIKU KORESPONDUJACEGO PDF'A				*/
		/************************************************************************************************/
		SOAP_FOK_LOGOUT_CD_TMP(	createDatagram(	userClassId,
							(char *[]){visible_oid,file_type_oid, coresonding_oid},
							(char *[]){"0",pdf_file_type,
							(char *)resp_dtg_set->bmdDatagramSetTable[0]->
							protocolDataFileId->buf},
							3,
							BMD_DATAGRAM_GET_CGI_LO,
							BMD_ONLY_GROUP_DOC,
							BMD_QUERY_EQUAL,
							&dtg_set2));

		asprintf(&path, "%s%li", tmp_file, (long)getpid());
		status=getFileFromBMD(bi, dtg_set2, &resp_dtg_set2, path);

		if (status<BMD_OK)
		{
			if (status==BMD_SQL_GET_CGI_NOT_FOUND_ERROR)
			{
				SOAP_FOK_LOGOUT_CD_TMP(BMDSOAP_SERVER_NO_PDF_FILE);
			}
			else
			{
				SOAP_FOK_LOGOUT_CD_TMP(status);
			}
		}

		/************************************************************************************/
		/*	jesli plik zostal pobrany zapisujemy go i dopisujemy do listy do spakowania	*/
		/************************************************************************************/
		if (resp_dtg_set2->bmdDatagramSetTable[0]->protocolDataFilename!=NULL)
		{
			if (	(strcmp(zip_attachments_directory_name,".")!=0) &&
				(strcmp(zip_attachments_directory_name,"")!=0) )
			{
				mkdir(zip_attachments_directory_name, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
				asprintf(&path_dest2, "%s/%s", zip_attachments_directory_name, resp_dtg_set2->bmdDatagramSetTable[0]->protocolDataFilename->buf);
			}
			else
			{
				asprintf(&path_dest2, "%s", resp_dtg_set2->bmdDatagramSetTable[0]->protocolDataFilename->buf);
			}
			fileMove(path,path_dest2);

			if (path_dest2!=NULL)
			{
				zip_list_size++;
				zip_list=(char **)realloc(zip_list,sizeof(char *)*zip_list_size);

				zip_list[zip_list_size-1]=(char*)malloc(strlen(path_dest2)+2);
				memset(zip_list[zip_list_size-1], 0, strlen(path_dest2)+1);
				memcpy(zip_list[zip_list_size-1], path_dest2, strlen(path_dest2));
			}
		}
	}

	/******************************************************************/
	/*	jesli plik nie zostal pobrany generujemy go przez Press'a	*/
	/******************************************************************/
	if (	(pdf_file_type!=NULL) &&
		(resp_dtg_set2!=NULL) &&
		(resp_dtg_set2->bmdDatagramSetTable[0]!=NULL) &&
		(resp_dtg_set2->bmdDatagramSetTable[0]->protocolDataFilename==NULL))
	{

		/************************************************************/
		/*	wygenerowanie nazwy pliku (zamiast oidow - wartosci)	*/
		/************************************************************/
		for (i=0; i<resp_dtg_set->bmdDatagramSetTable[0]->no_of_additionalMetaData; i++)
		{
			asprintf(&mtd_oid_str, "%s", resp_dtg_set->bmdDatagramSetTable[0]->additionalMetaData[i]->OIDTableBuf);

			BMD_FOK(set_gen_buf2(resp_dtg_set->bmdDatagramSetTable[0]->additionalMetaData[i]->AnyBuf,
			resp_dtg_set->bmdDatagramSetTable[0]->additionalMetaData[i]->AnySize, &metadata_value));

			/************************************************************/
			/*	jesli metadana to data, zamieniamy separatory na	*/
			/************************************************************/
			if (strcmp(press_date_metadata, mtd_oid_str)==0)
			{
				bmd_str_replace((char **)&(metadata_value->buf),press_date_separator_in,press_date_separator_out);
				metadata_value->buf[12]='\0';
			}
			bmd_str_replace(&press_out_filename,mtd_oid_str,(char *)metadata_value->buf);
			free_gen_buf(&metadata_value);
		}

		if ((press_in_dir!=NULL) && (press_out_dir!=NULL))
		{
			/************************************************/
			/*	zapisanie xml'a w katalogu wejsciowym	*/
			/************************************************/
			asprintf(&src, "%s", path_dest4);
			asprintf(&dest, "%s/%s.xml", press_in_dir,press_in_filename);
			fileCopy(src, dest);

			/************************************************************************/
			/*	odczytywanie wyniku dzialania press'a z katalogu wyjsciowego	*/
			/************************************************************************/
			if ((press_dir_ptr=opendir(press_out_dir))!=NULL)
			{

				for (i=0; i<press_timeout; i++)
				{
					while((dirp = readdir(press_dir_ptr))!=NULL)
					{

						if (dirp==NULL) break;
						if ((strcmp(dirp->d_name,".")==0) || (strcmp(dirp->d_name,"..")==0)) continue;

						/******************************/
						/*	analiza nazwy pliku	*/
						/******************************/
						status=isSubstring(dirp->d_name, press_out_filename);
						if (status<BMD_OK) continue;

						asprintf(&src, "%s/%s", press_out_dir, dirp->d_name);
						asprintf(&dest, "%s", dirp->d_name);

						if (	(strcmp(zip_attachments_directory_name,".")!=0) &&
							(strcmp(zip_attachments_directory_name,"")!=0) )
						{
							mkdir(zip_attachments_directory_name, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
							asprintf(&dest, "%s/%s", zip_attachments_directory_name, dirp->d_name);
						}
						else
						{
							asprintf(&dest, "%s", dirp->d_name);
						}

						fileMove(src, dest);
						asprintf(&path_dest2,"%s",dest);
						break;
					}
					if (path_dest2!=NULL)
					{
						break;
					}
					sleep(1);
				}
				closedir(press_dir_ptr);
			}
			else
			{
				PRINT_WARNING("SOAPSERVERWRN Unable to open press out dir %s\n",press_out_dir);
			}
		}

		/************************************************************************/
		/*	jesli pdf nie bylo w bazie danych i press go nie wygenerowal	*/
		/************************************************************************/
		if (path_dest2==NULL)
		{
			SOAP_FOK_LOGOUT_CD_TMP(BMDSOAP_SERVER_NO_PDF_FILE);
		}

		/******************************************/
		/*	dodanie pdf'a do paczki zipowej	*/
		/******************************************/
		if (path_dest2!=NULL)
		{
			zip_list_size++;
			zip_list=(char **)realloc(zip_list,sizeof(char *)*zip_list_size);

			zip_list[zip_list_size-1]=(char*)malloc(strlen(path_dest2)+2);
			memset(zip_list[zip_list_size-1], 0, strlen(path_dest2)+1);
			memcpy(zip_list[zip_list_size-1], path_dest2, strlen(path_dest2));
		}

		/******************************************************************/
		/*	zapisanie wygenerowanego przez press'a pliku w bazie danych	*/
		/******************************************************************/
		if (add_pdf_to_archive==1)
		{
			if (pdf_file_type!=NULL)
			{
				if ((resp_dtg_set2->bmdDatagramSetTable[0]->protocolDataFilename==NULL) && (path_dest2!=NULL))
				{
					SOAP_FOK_LOGOUT_CD_TMP( createDatagramToInsertFile(	userClassId,
												(char *[]){visible_oid,file_type_oid,coresonding_oid},
												(char *[]){"0",pdf_file_type,(char *)resp_dtg_set->bmdDatagramSetTable[0]->protocolDataFileId->buf},
												3,
												path_dest2,
												&dtg_set3));

					SOAP_FOK_LOGOUT_CD_TMP(insertFileToBMD(	bi, dtg_set3, &resp_dtg_set3, path_dest2));
				}
				else
				{

				}
			}
		}

	}

	/************************************************************************************************/
	/*						STWORZENIE LISTY PLIKOW DO SPAKOWANIA				*/
	/************************************************************************************************/
	/* --------- wartosc dowodowa --------------------*/
	asprintf(&file_tsr,"%s",path_dest);
	asprintf(&file_sig,"%s",path_dest);

	for (i=0; i<resp_dtg_set->bmdDatagramSetTable[0]->no_of_pkiMetaData; i++)
	{
		/******************************/
		/*	znaczniki czasu pliku	*/
		/******************************/
		if (strcmp(	resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[i]->OIDTableBuf,timestamp)==0)
		{
			BMD_FOK(set_gen_buf2(resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[i]->AnyBuf,
			resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[i]->AnySize, &metadata_value));

			asprintf(&file_tsr,"%s.tsr",file_tsr);
			bmd_save_buf(metadata_value, file_tsr);
			free_gen_buf(&metadata_value);

			zip_list_size++;
			zip_list=(char **)realloc(zip_list,sizeof(char *)*zip_list_size);

			zip_list[zip_list_size-1]=(char*)malloc(strlen(file_tsr)+2);
			memset(zip_list[zip_list_size-1], 0, strlen(file_tsr)+1);
			memcpy(zip_list[zip_list_size-1], file_tsr, strlen(file_tsr));

		}

		/************************/
		/*	podpisy pliku	*/
		/************************/
		if (strcmp(	resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[i]->OIDTableBuf,signature)==0 ||
			strcmp(	resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[i]->OIDTableBuf,signature_pades) == 0)
		{
			BMD_FOK(set_gen_buf2(resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[i]->AnyBuf,
			resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[i]->AnySize, &metadata_value));

			sig_no++;
			dvcs_no=0;

			asprintf(&dir_sig,"%s_%li/",zip_signature_directory_name,sig_no);
			SOAP_FOK_LOGOUT_TMP(mkdir(dir_sig, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH));

			asprintf(&file_sig,"%s%s.sig",dir_sig,path_dest);
			asprintf(&file_sig2,"%s.sig",path_dest);

			// dla PAdES nie jest umieszczany w ZIPie wyodrebniony podpis (podpis jest wewnatrz pliku PDF)
			if(strcmp(	resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[i]->OIDTableBuf,signature_pades) != 0)
			{
				status=bmd_save_buf(metadata_value, file_sig);

				zip_list_size++;
				zip_list=(char **)realloc(zip_list,sizeof(char *)*zip_list_size);

				zip_list[zip_list_size-1]=(char*)malloc(strlen(file_sig)+2);
				memset(zip_list[zip_list_size-1], 0, strlen(file_sig)+1);
				memcpy(zip_list[zip_list_size-1], file_sig, strlen(file_sig));
			}
			free_gen_buf(&metadata_value);

			asprintf(&file_sig_tsr,"%s",file_sig);
			asprintf(&file_sig_dvcs,"%s",file_sig);


			for (j=0; j<resp_dtg_set->bmdDatagramSetTable[0]->no_of_pkiMetaData; j++)
			{
				/******************************************/
				/*	pobranie znacznika czasu podpisu	*/
				/******************************************/
				if ((strcmp(	resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[j]->OIDTableBuf,tim_sig)==0) &&
							(resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[i]->myId==resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[j]->ownerId))
				{
					BMD_FOK(set_gen_buf2(resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[j]->AnyBuf,
					resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[j]->AnySize, &metadata_value));

					asprintf(&file_sig_tsr,"%s.tsr",file_sig_tsr);
					bmd_save_buf(metadata_value, file_sig_tsr);
					free_gen_buf(&metadata_value);

					zip_list_size++;
					zip_list=(char **)realloc(zip_list,sizeof(char *)*zip_list_size);

					zip_list[zip_list_size-1]=(char*)malloc(strlen(file_sig_tsr)+2);
					memset(zip_list[zip_list_size-1], 0, strlen(file_sig_tsr)+1);
					memcpy(zip_list[zip_list_size-1], file_sig_tsr, strlen(file_sig_tsr));


					free_gen_buf(&metadata_value);
				}


				/******************************************/
				/*	pobranie poswiadczenia dvcs podpisu	*/
				/******************************************/
				if ((strcmp(	resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[j]->OIDTableBuf,dvcs)==0) &&
							(resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[i]->myId==resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[j]->ownerId))
				{
					BMD_FOK(set_gen_buf2(resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[j]->AnyBuf,
					resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[j]->AnySize, &metadata_value));

					dvcs_no++;

					asprintf(&dir_dvcs,"%s_%li/%s_%li/",zip_signature_directory_name, sig_no, zip_dvcs_directory_name, dvcs_no);
					SOAP_FOK_LOGOUT_TMP(mkdir(dir_dvcs, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH));

					asprintf(&file_sig_dvcs,"%s%s.cpd.dvcs",dir_dvcs,file_sig2);
					bmd_save_buf(metadata_value, file_sig_dvcs);
					free_gen_buf(&metadata_value);

					zip_list_size++;
					zip_list=(char **)realloc(zip_list,sizeof(char *)*zip_list_size);

					zip_list[zip_list_size-1]=(char*)malloc(strlen(file_sig_dvcs)+2);
					memset(zip_list[zip_list_size-1], 0, strlen(file_sig_dvcs)+1);
					memcpy(zip_list[zip_list_size-1], file_sig_dvcs, strlen(file_sig_dvcs));

					free_gen_buf(&metadata_value);

					asprintf(&file_sig_dvcs_tsr,"%s",file_sig_dvcs);



					/************************************************************/
					/*	wyszukiwanie znacznikow czasu dla poswiadczenia dvcs	*/
					/************************************************************/
					for (k=0; k<resp_dtg_set->bmdDatagramSetTable[0]->no_of_pkiMetaData; k++)
					{
						if ((strcmp(	resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[k]->OIDTableBuf,tim_dvcs)==0) &&
									(resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[j]->myId==resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[k]->ownerId))
						{
							BMD_FOK(set_gen_buf2(resp_dtg_set->bmdDatagramSetTable[0]->
							pkiMetaData[k]->AnyBuf, resp_dtg_set->bmdDatagramSetTable[0]->
							pkiMetaData[k]->AnySize, &metadata_value));

							asprintf(&file_sig_dvcs_tsr,"%s.tsr",file_sig_dvcs_tsr);
							bmd_save_buf(metadata_value, file_sig_dvcs_tsr);
							free_gen_buf(&metadata_value);

							zip_list_size++;
							zip_list=(char **)realloc(zip_list,sizeof(char *)*zip_list_size);

							zip_list[zip_list_size-1]=(char*)malloc(strlen(file_sig_dvcs_tsr)+2);
							memset(zip_list[zip_list_size-1], 0, strlen(file_sig_dvcs_tsr)+1);
							memcpy(zip_list[zip_list_size-1], file_sig_dvcs_tsr, strlen(file_sig_dvcs_tsr));


							free_gen_buf(&metadata_value);

						}
					}
				}


			}
		}
	}

	/************************/
	/*	spakowanie plikow	*/
	/************************/
	asprintf(&path_dest_zip, "%s.zip", resp_dtg_set->bmdDatagramSetTable[0]->protocolDataFilename->buf);
	SOAP_FOK_LOGOUT_TMP(zipCompressFile(zip_list, zip_list_size, path_dest_zip));

	/************************/
	/*	ustawienie wyniku	*/
	/************************/
	(*output)=(struct bmd230__fileInfo *)soap_malloc(soap,sizeof(struct bmd230__fileInfo));
	memset(*output,0,sizeof(struct bmd230__fileInfo));

	/*	ustawienie nazwy pliku	*/
	(*output)->filename=(char *)soap_malloc(soap,strlen(path_dest_zip)+2);
	memset((*output)->filename, 0, strlen(path_dest_zip)+1);
	memmove((*output)->filename, path_dest_zip, strlen(path_dest_zip));

	/*	ustawienie pliku	*/
	(*output)->file=(struct bmd230__myBinaryDataType *)soap_malloc(soap,sizeof(struct bmd230__myBinaryDataType));
	(*output)->file->id = NULL;

	/************************************************/
	/*	wczytanie spakowanego pliku do genbufa	*/
	/************************************************/

	free_gen_buf(&gb);
	bmd_load_binary_content(path_dest_zip,&gb);

	(*output)->file->__ptr=(unsigned char *)soap_malloc(soap,gb->size+2);
	memset((*output)->file->__ptr,0,gb->size+1);
	(*output)->file->__size=gb->size;
	memmove((*output)->file->__ptr,gb->buf,gb->size);

	(*output)->file->type = NULL;
	(*output)->file->options = NULL;

	/******************************************/
	/*	usuwanie katalogow tymczasowych	*/
	/******************************************/
	for (i=0; i<sig_no; i++)
	{
		for (j=0; j<dvcs_no; j++)
		{
			asprintf(&path, "%s_%li/%s_%li/",zip_signature_directory_name,i+1,zip_dvcs_directory_name,j+1);
			remove_dir(path);
		}
		asprintf(&path, "%s_%li/",zip_signature_directory_name,i+1);
		remove_dir(path);
	}

	if (	(strcmp(zip_attachments_directory_name,".")!=0) &&
		(strcmp(zip_attachments_directory_name,"")!=0) && (xml_file_type != NULL || pdf_file_type != NULL))
	{
		asprintf(&path, "%s/",zip_attachments_directory_name);
		remove_dir(path);
	}

	/******************************************************************/
	/*	powrot do katalogu pierwotnego i usuniecie tymczasowego	*/
	/******************************************************************/
	chdir(_GLOBAL_current_dir);
	asprintf(&path, "%s%s%li/", tmp_dir, tmp_file, (long)getpid());
	remove_dir(path);

	/******************/
	/*	porzadki	*/
	/******************/

	bmd_logout(&bi, deserialisation_max_memory);
	bmd_info_destroy(&bi);
	bmd_end();

	if (dtg_set!=NULL) bmd_datagramset_free(&dtg_set);
	if (resp_dtg_set!=NULL) bmd_datagramset_free(&resp_dtg_set);
	if (dtg_set2!=NULL) bmd_datagramset_free(&dtg_set2);
	if (resp_dtg_set2!=NULL) bmd_datagramset_free(&resp_dtg_set2);
	if (dtg_set3!=NULL) bmd_datagramset_free(&dtg_set3);
	if (resp_dtg_set3!=NULL) bmd_datagramset_free(&resp_dtg_set3);
	if (dtg_set4!=NULL) bmd_datagramset_free(&dtg_set4);
	if (resp_dtg_set4!=NULL) bmd_datagramset_free(&resp_dtg_set4);
	free_gen_buf(&gb);
	free_gen_buf(&metadata_value);
	free0(path);
	free0(path_dest);
	free0(path_dest2);
	free0(path_dest4);
	free0(path_dest_zip);
	for (i=0; i<zip_list_size; i++)
	{
		free0(zip_list[i]);
	}
	free0(zip_list);
	free0(file_tsr);
	free0(file_sig);
	free0(file_sig2);
	free0(file_sig_tsr);
	free0(file_sig_dvcs);
	free0(file_sig_dvcs_tsr);
	free0(dir_sig);
	free0(dir_dvcs);
	free0(tmp_dir);

	free0(tmp_location);
	free0(press_in_dir);
	free0(press_out_dir);
	free0(press_in_filename);
	free0(press_out_filename);
	free0(src);
	free0(dest);
	free0(mtd_oid_str);
	free0(press_date_metadata);
	free0(press_date_separator_in);
	free0(press_date_separator_out);
	free0(pdf_file_type);
	free0(xml_file_type);

	free0(zip_signature_directory_name);
	free0(zip_dvcs_directory_name);
	free0(zip_attachments_directory_name);

	PRINT_SOAP("Status:\t\trequest served correctly\n");
	return SOAP_OK;
}

/***************************************************************************/
/* obsluga zadania pobrania paczki fakturowej po id i metadanej dodatkowej */
/***************************************************************************/
int bmd230__getInvoicePack(	struct soap *soap,
			char *accountId,
			char *invoiceId,
			char *userIdentifier,
			char *getReason,
			struct xsd__base64Binary *cert,
			char *roleName,
			char *groupName,
			char *userClassId,
			enum trans_t type,
			char* parameter,
			struct bmd230__fileInfo **output)
{
long i						= 0;
long j						= 0;
long k						= 0;
long status					= 0;
long sig_no					= 0;
long dvcs_no					= 0;
long zip_list_size				= 0;
long press_timeout				= 0;
long add_pdf_to_zip				= 0;
long add_pdf_to_archive				= 0;
long max_datagram_in_set_transmission		= 0;
long deserialisation_max_memory			= 0;
long twl_select_operator			= 0;
bmd_info_t *bi					= NULL;
bmdDatagramSet_t *dtg_set			= NULL;
bmdDatagramSet_t *dtg_set2			= NULL;
bmdDatagramSet_t *dtg_set3			= NULL;
bmdDatagramSet_t *dtg_set4			= NULL;
bmdDatagramSet_t *resp_dtg_set			= NULL;
bmdDatagramSet_t *resp_dtg_set2			= NULL;
bmdDatagramSet_t *resp_dtg_set3			= NULL;
bmdDatagramSet_t *resp_dtg_set4			= NULL;
GenBuf_t *gb					= NULL;
GenBuf_t *metadata_value			= NULL;
char *path					= NULL;
char *path_dest					= NULL;
char *path_dest2				= NULL;
char *path_dest4				= NULL;
char *path_dest_zip				= NULL;
char **zip_list					= NULL;
char *file_tsr					= NULL;
char *file_sig				 	= NULL;
char *file_sig2					= NULL;
char *file_sig_tsr				= NULL;
char *file_sig_dvcs				= NULL;
char *file_sig_dvcs_tsr				= NULL;
char *dir_sig					= NULL;
char *dir_dvcs					= NULL;
char *tmp_dir					= NULL;
char *tmp_location				= NULL;
char *press_in_dir				= NULL;
char *press_out_dir				= NULL;
char *press_in_filename				= NULL;
char *press_out_filename			= NULL;
DIR *press_dir_ptr				= NULL;
struct dirent *dirp				= NULL;
char *src					= NULL;
char *dest					= NULL;
char *mtd_oid_str				= NULL;
char *press_date_metadata			= NULL;
char *press_date_separator_in			= NULL;
char *press_date_separator_out			= NULL;
char *pdf_file_type				= NULL;
char *xml_file_type				= NULL;
char *search_oid				= NULL;
char *search_oid2				= NULL;
char *search_oid3				= NULL;

char *zip_signature_directory_name		= NULL;
char *zip_dvcs_directory_name			= NULL;
char *zip_attachments_directory_name		= NULL;

char *getInvoicePack_select_type 		= NULL;

char tmp_file[]					= {"soap_get_pack_"};
FILE* fd					= NULL;

char visible_oid[]			= {OID_SYS_METADATA_CRYPTO_OBJECTS_VISIBLE};	/* widzialnosc pliku */
char file_type_oid[]			= {OID_SYS_METADATA_CRYPTO_OBJECTS_FILE_TYPE};	/* typ pliku */
char coresonding_oid[]			= {OID_SYS_METADATA_CRYPTO_OBJECTS_CORESPONDING_ID};	/* plik korespondujacy */


char timestamp[]	= {OID_PKI_METADATA_TIMESTAMP};
char signature[]	= {OID_PKI_METADATA_SIGNATURE};
char signature_pades[]	= {OID_PKI_METADATA_SIGNATURE_PADES};
char dvcs[]		= {OID_PKI_METADATA_DVCS};
char tim_sig[]		= {OID_PKI_METADATA_TIMESTAMP_FROM_SIG};
char tim_dvcs[]		= {OID_PKI_METADATA_TIMESTAMP_FROM_DVCS};

char* mainDocumentId		= NULL;
char* get_parameter_oid		= NULL;

char *derUserCert=NULL;
long derUserCertLen=0;
long retVal=0;
char* paramCert=NULL;
long paramCertLen=0;


      /******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if(accountId==NULL)	{	SOAP_FOK(BMDSOAP_SERVER_EMPTY1_PARAM_ERROR);	}
	if(invoiceId==NULL)	{	SOAP_FOK(BMDSOAP_SERVER_EMPTY2_PARAM_ERROR);	}

	if(parameter != NULL)
	{
		bmdconf_get_value(_GLOBAL_konfiguracja, "soap", "get_parameter_oid", &get_parameter_oid);
		if(get_parameter_oid == NULL)
		{
			PRINT_ERROR("ERROR There have to be specified get_parameter_oid option to use parameter in getInvoicePack request\n");
			SOAP_FOK(BMDSOAP_SERVER_INTERNAL_OPERATION_ERROR);
		}
	}


	PRINT_SOAP("Request:\tget file in zip package\n");
	PRINT_SOAP("accountId:\t%s\n",accountId);
	PRINT_SOAP("invoiceId:\t%s\n",invoiceId);
	if (userIdentifier==NULL)
	{
		PRINT_SOAP("userIdentifier:\t%s\n","<no userIdentifier>");
	}
	else
	{
		PRINT_SOAP("userIdentifier:\t%s\n",userIdentifier);
	}

	/******************************************/
	/*	wczytanie ustawien konfiguracyjnych	*/
	/******************************************/
	bmdconf_get_value(_GLOBAL_konfiguracja,"soap","search_oid1",&search_oid);
	bmdconf_get_value(_GLOBAL_konfiguracja,"soap","search_oid2",&search_oid2);
	bmdconf_get_value(_GLOBAL_konfiguracja,"soap","search_oid3",&search_oid3);
	SOAP_FOK(bmdconf_get_value(_GLOBAL_konfiguracja,"soap","temp_dir",&tmp_dir));
	bmdconf_get_value(_GLOBAL_konfiguracja,"convert","pdf_file_type",&pdf_file_type);
	bmdconf_get_value(_GLOBAL_konfiguracja,"convert","xml_file_type",&xml_file_type);
	bmdconf_get_value(_GLOBAL_konfiguracja,"convert","press_in_dir",&press_in_dir);
	bmdconf_get_value(_GLOBAL_konfiguracja,"convert","press_out_dir",&press_out_dir);
	bmdconf_get_value(_GLOBAL_konfiguracja,"convert","press_in_filename",&press_in_filename);
	bmdconf_get_value(_GLOBAL_konfiguracja,"convert","press_out_filename",&press_out_filename);
	bmdconf_get_value(_GLOBAL_konfiguracja,"convert","press_date_metadata",&press_date_metadata);
	bmdconf_get_value(_GLOBAL_konfiguracja,"convert","press_date_separator_in",&press_date_separator_in);
	bmdconf_get_value(_GLOBAL_konfiguracja,"convert","press_date_separator_out",&press_date_separator_out);
	bmdconf_get_value_int(_GLOBAL_konfiguracja,"convert","press_timeout",&press_timeout);
	SOAP_FOK(bmdconf_get_value_int(_GLOBAL_konfiguracja,"convert","add_pdf_to_zip",&add_pdf_to_zip));
	SOAP_FOK(bmdconf_get_value_int(_GLOBAL_konfiguracja,"convert","add_pdf_to_archive",&add_pdf_to_archive));
	SOAP_FOK(bmdconf_get_value(_GLOBAL_konfiguracja,"zip_package","zip_signature_directory_name",&zip_signature_directory_name));
	SOAP_FOK(bmdconf_get_value(_GLOBAL_konfiguracja,"zip_package","zip_dvcs_directory_name",&zip_dvcs_directory_name));
	SOAP_FOK(bmdconf_get_value(_GLOBAL_konfiguracja,"zip_package","zip_attachments_directory_name",&zip_attachments_directory_name));
	bmdconf_get_value(_GLOBAL_konfiguracja,"functionalities_behaviour","getInvoicePack_select_type",&getInvoicePack_select_type);
	if(getInvoicePack_select_type == NULL)
	{
		asprintf(&getInvoicePack_select_type,"EQUAL");
		if(getInvoicePack_select_type == NULL)
		{
			SOAP_FOK(NO_MEMORY);
		}
	}

	if(strcmp(getInvoicePack_select_type, "EQUAL") == 0)
	{
		twl_select_operator = BMD_QUERY_EQUAL;
	}
	else if(strcmp(getInvoicePack_select_type, "EXACT") == 0)
	{
		twl_select_operator = BMD_QUERY_EXACT_ILIKE;
	}
	else if(strcmp(getInvoicePack_select_type, "LIKE") == 0)
	{
		twl_select_operator = BMD_QUERY_LIKE;
	}
	else if(strcmp(getInvoicePack_select_type, "ILIKE") == 0)
	{
		twl_select_operator = BMD_QUERY_ILIKE;
	}
	else
	{
		SOAP_FOK(BMDSOAP_SERVER_SEARCH_OPERATOR_INVALID);
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

	/************************************/
	/*	odczytanie katalogu biezacego	*/
	/************************************/
	memset(_GLOBAL_current_dir, 0 ,256);
	if (getcwd(_GLOBAL_current_dir,255)==NULL)
	{
		SOAP_FOK_LOGOUT(BMDSOAP_SERVER_GCWD_ERROR);
	}

	/************************************************************************************************/
	/*	stworzenie katalogu tymczasowego dla pobranych danych z serwera i przejscie do niego	*/
	/************************************************************************************************/
	asprintf(&tmp_location, "%s%s%li/", tmp_dir, tmp_file, (long)getpid());
	SOAP_FOK_LOGOUT(mkdir(tmp_location, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH));
	SOAP_FOK_LOGOUT(chdir(tmp_location));

	/************************************************************************************************/
	/*						POBRANIE PLIKU GLOWNEGO							*/
	/************************************************************************************************/
	SOAP_FOK_LOGOUT_CD_TMP(createDatagram(	userClassId,
						(char *[]){search_oid,search_oid2,search_oid3},
						(char *[]){accountId,invoiceId,userIdentifier},
						3,
						BMD_DATAGRAM_GET_CGI_LO,
						BMD_ONLY_GROUP_DOC,
						twl_select_operator,
						&dtg_set));

	/******************************************/
	/*	ustawienie powodu pobrania faktury	*/
	/******************************************/
	if ((getReason!=NULL) && (strlen(getReason)>0))
	{
		dtg_set->bmdDatagramSetTable[0]->protocolData=(GenBuf_t *)malloc(sizeof(GenBuf_t));
		dtg_set->bmdDatagramSetTable[0]->protocolData->buf=(char *)malloc(strlen(getReason)+2);
		memset(dtg_set->bmdDatagramSetTable[0]->protocolData->buf, 0, strlen(getReason)+1);
		memcpy(dtg_set->bmdDatagramSetTable[0]->protocolData->buf, getReason, strlen(getReason));
		dtg_set->bmdDatagramSetTable[0]->protocolData->size=strlen(getReason);
	}

	asprintf(&path, "%s%li", tmp_file, (long)getpid());
	SOAP_FOK_LOGOUT_CD_TMP(getFileFromBMD(bi, dtg_set, &resp_dtg_set, path));

	if(resp_dtg_set->bmdDatagramSetSize > 0 &&
		resp_dtg_set->bmdDatagramSetTable[0]->datagramStatus >= BMD_OK &&
		resp_dtg_set->bmdDatagramSetTable[0]->protocolDataFileId != NULL &&
		resp_dtg_set->bmdDatagramSetTable[0]->protocolDataFileId->size > 0)
	{
		free(mainDocumentId); mainDocumentId = NULL;
		mainDocumentId = strdup((char*)(resp_dtg_set->bmdDatagramSetTable[0]->protocolDataFileId->buf));
		if(mainDocumentId == NULL)
		{
			SOAP_FOK_LOGOUT_CD_TMP(BMD_ERR_MEMORY);
		}
	}
	else
	{
		SOAP_FOK_LOGOUT_CD_TMP(BMDSOAP_SERVER_INTERNAL_ERROR);
	}

	/************************************************************************************************************/
	/*	przepisanie pobranego pliku z pliku tymczasowego do docelowego i dopisanie do listy do spakowania	*/
	/************************************************************************************************************/
	asprintf(&path_dest, "%s", resp_dtg_set->bmdDatagramSetTable[0]->protocolDataFilename->buf);
	fileMove(path,path_dest);

	if (path_dest!=NULL)
	{
		zip_list_size++;
		zip_list=(char **)realloc(zip_list,sizeof(char *)*zip_list_size);

		zip_list[zip_list_size-1]=(char*)malloc(strlen(path_dest)+2);
		memset(zip_list[zip_list_size-1], 0, strlen(path_dest)+1);
		memcpy(zip_list[zip_list_size-1], path_dest, strlen(path_dest));
	}

	if (xml_file_type!=NULL)
	{
		/************************************************************************************************/
		/*						POBRANIE PLIKU KORESPONDUJACEGO XML'A				*/
		/************************************************************************************************/
		SOAP_FOK_LOGOUT_CD_TMP(	createDatagram(	userClassId,
							(char *[]){visible_oid, file_type_oid, coresonding_oid},
							(char *[]){"0", xml_file_type, mainDocumentId},
							3,
							BMD_DATAGRAM_GET_CGI_LO,
							BMD_ONLY_GROUP_DOC,
							BMD_QUERY_EQUAL,
							&dtg_set4));

		asprintf(&path, "%s%li", tmp_file, (long)getpid());
		status=getFileFromBMD(bi, dtg_set4, &resp_dtg_set4, path);
		if (status<BMD_OK)
		{
			if (status==BMD_SQL_GET_CGI_NOT_FOUND_ERROR)
			{
				SOAP_FOK_LOGOUT_CD_TMP(BMDSOAP_SERVER_XML_FILE_NOT_FOUND);
			}
			else
			{
				SOAP_FOK_LOGOUT_CD_TMP(status);
			}
		}

		/************************************************************************************/
		/*	jesli plik zostal pobrany zapisujemy go i dopisujemy do listy do spakowania	*/
		/************************************************************************************/
		if (resp_dtg_set4->bmdDatagramSetTable[0]->protocolDataFilename!=NULL)
		{
			if (	(strcmp(zip_attachments_directory_name,".")!=0) &&
				(strcmp(zip_attachments_directory_name,"")!=0) )
			{
				SOAP_FOK_LOGOUT_CD_TMP(mkdir(zip_attachments_directory_name, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH));
				asprintf(&path_dest4, "%s/%s", zip_attachments_directory_name, resp_dtg_set4->bmdDatagramSetTable[0]->protocolDataFilename->buf);
			}
			else
			{
				asprintf(&path_dest4, "%s", resp_dtg_set4->bmdDatagramSetTable[0]->protocolDataFilename->buf);
			}
			fileMove(path,path_dest4);

			if (path_dest4!=NULL)
			{
				zip_list_size++;
				zip_list=(char **)realloc(zip_list,sizeof(char *)*zip_list_size);

				zip_list[zip_list_size-1]=(char*)malloc(strlen(path_dest4)+2);
				memset(zip_list[zip_list_size-1], 0, strlen(path_dest4)+1);
				memcpy(zip_list[zip_list_size-1], path_dest4, strlen(path_dest4));
			}
		}
	}

	if ((pdf_file_type!=NULL) && (add_pdf_to_zip==1))
	{
		/************************************************************************************************/
		/*						POBRANIE PLIKU KORESPONDUJACEGO PDF'A				*/
		/************************************************************************************************/
		SOAP_FOK_LOGOUT_CD_TMP(	createDatagram(	userClassId,
							(char *[]){visible_oid,file_type_oid, coresonding_oid},
							(char *[]){"0",pdf_file_type, mainDocumentId},
							3,
							BMD_DATAGRAM_GET_CGI_LO,
							BMD_ONLY_GROUP_DOC,
							BMD_QUERY_EQUAL,
							&dtg_set2));

		asprintf(&path, "%s%li", tmp_file, (long)getpid());
		status=getFileFromBMD(bi, dtg_set2, &resp_dtg_set2, path);
		if (status<BMD_OK)
		{
			if (status==BMD_SQL_GET_CGI_NOT_FOUND_ERROR)
			{
				SOAP_FOK_LOGOUT_CD_TMP(BMDSOAP_SERVER_NO_PDF_FILE);
			}
			else
			{
				SOAP_FOK_LOGOUT_CD_TMP(status);
			}
		}

		/************************************************************************************/
		/*	jesli plik zostal pobrany zapisujemy go i dopisujemy do listy do spakowania	*/
		/************************************************************************************/
		if (resp_dtg_set2->bmdDatagramSetTable[0]->protocolDataFilename!=NULL)
		{
			if (	(strcmp(zip_attachments_directory_name,".")!=0) &&
				(strcmp(zip_attachments_directory_name,"")!=0) )
			{
				mkdir(zip_attachments_directory_name, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
				asprintf(&path_dest2, "%s/%s", zip_attachments_directory_name, resp_dtg_set2->bmdDatagramSetTable[0]->protocolDataFilename->buf);
			}
			else
			{
				asprintf(&path_dest2, "%s", resp_dtg_set2->bmdDatagramSetTable[0]->protocolDataFilename->buf);
			}
			fileMove(path,path_dest2);

			if (path_dest2!=NULL)
			{
				zip_list_size++;
				zip_list=(char **)realloc(zip_list,sizeof(char *)*zip_list_size);

				zip_list[zip_list_size-1]=(char*)malloc(strlen(path_dest2)+2);
				memset(zip_list[zip_list_size-1], 0, strlen(path_dest2)+1);
				memcpy(zip_list[zip_list_size-1], path_dest2, strlen(path_dest2));
			}
		}
	}

	/******************************************************************/
	/*	jesli plik nie zostal pobrany generujemy go przez Press'a	*/
	/******************************************************************/
	if (	(pdf_file_type!=NULL) &&
		(resp_dtg_set2!=NULL) &&
		(resp_dtg_set2->bmdDatagramSetTable[0]!=NULL) &&
		(resp_dtg_set2->bmdDatagramSetTable[0]->protocolDataFilename==NULL))
	{

		/************************************************************/
		/*	wygenerowanie nazwy pliku (zamiast oidow - wartosci)	*/
		/************************************************************/
		for (i=0; i<resp_dtg_set->bmdDatagramSetTable[0]->no_of_additionalMetaData; i++)
		{
			asprintf(&mtd_oid_str, "%s", resp_dtg_set->bmdDatagramSetTable[0]->additionalMetaData[i]->OIDTableBuf);

			BMD_FOK(set_gen_buf2(resp_dtg_set->bmdDatagramSetTable[0]->additionalMetaData[i]->AnyBuf,
			resp_dtg_set->bmdDatagramSetTable[0]->additionalMetaData[i]->AnySize, &metadata_value));

			/************************************************************/
			/*	jesli metadana to data, zamieniamy separatory na	*/
			/************************************************************/
			if (strcmp(press_date_metadata, mtd_oid_str)==0)
			{
				bmd_str_replace((char **)&(metadata_value->buf),press_date_separator_in,press_date_separator_out);
				metadata_value->buf[12]='\0';
			}
			bmd_str_replace(&press_out_filename,mtd_oid_str,(char *)metadata_value->buf);
			free_gen_buf(&metadata_value);
		}

		if ((press_in_dir!=NULL) && (press_out_dir!=NULL))
		{
			/************************************************/
			/*	zapisanie xml'a w katalogu wejsciowym	*/
			/************************************************/
			asprintf(&src, "%s", path_dest4);
			asprintf(&dest, "%s/%s.xml", press_in_dir,press_in_filename);
			fileCopy(src, dest);

			/************************************************************************/
			/*	odczytywanie wyniku dzialania press'a z katalogu wyjsciowego	*/
			/************************************************************************/
			if ((press_dir_ptr=opendir(press_out_dir))!=NULL)
			{

				for (i=0; i<press_timeout; i++)
				{
					while((dirp = readdir(press_dir_ptr))!=NULL)
					{

						if (dirp==NULL) break;
						if ((strcmp(dirp->d_name,".")==0) || (strcmp(dirp->d_name,"..")==0)) continue;

						/******************************/
						/*	analiza nazwy pliku	*/
						/******************************/
						status=isSubstring(dirp->d_name, press_out_filename);
						if (status<BMD_OK) continue;

						asprintf(&src, "%s/%s", press_out_dir, dirp->d_name);
						asprintf(&dest, "%s", dirp->d_name);

						if (	(strcmp(zip_attachments_directory_name,".")!=0) &&
							(strcmp(zip_attachments_directory_name,"")!=0) )
						{
							mkdir(zip_attachments_directory_name, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
							asprintf(&dest, "%s/%s", zip_attachments_directory_name, dirp->d_name);
						}
						else
						{
							asprintf(&dest, "%s", dirp->d_name);
						}

						fileMove(src, dest);
						asprintf(&path_dest2,"%s",dest);
						break;
					}
					if (path_dest2!=NULL)
					{
						break;
					}
					sleep(1);
				}
				closedir(press_dir_ptr);
			}
			else
			{
				PRINT_WARNING("SOAPSERVERWRN Unable to open press out dir %s\n",press_out_dir);
			}
		}

		/************************************************************************/
		/*	jesli pdf nie bylo w bazie danych i press go nie wygenerowal	*/
		/************************************************************************/
		if (path_dest2==NULL)
		{
			SOAP_FOK_LOGOUT_CD_TMP(BMDSOAP_SERVER_NO_PDF_FILE);
		}

		/******************************************/
		/*	dodanie pdf'a do paczki zipowej	*/
		/******************************************/
		if (path_dest2!=NULL)
		{
			zip_list_size++;
			zip_list=(char **)realloc(zip_list,sizeof(char *)*zip_list_size);

			zip_list[zip_list_size-1]=(char*)malloc(strlen(path_dest2)+2);
			memset(zip_list[zip_list_size-1], 0, strlen(path_dest2)+1);
			memcpy(zip_list[zip_list_size-1], path_dest2, strlen(path_dest2));
		}

		/******************************************************************/
		/*	zapisanie wygenerowanego przez press'a pliku w bazie danych	*/
		/******************************************************************/
		if (add_pdf_to_archive==1)
		{
			if (pdf_file_type!=NULL)
			{
				if ((resp_dtg_set2->bmdDatagramSetTable[0]->protocolDataFilename==NULL) && (path_dest2!=NULL))
				{
					SOAP_FOK_LOGOUT_CD_TMP( createDatagramToInsertFile(	userClassId,
												(char *[]){visible_oid,file_type_oid,coresonding_oid},
												(char *[]){"0",pdf_file_type, mainDocumentId},
												3,
												path_dest2,
												&dtg_set3));

					SOAP_FOK_LOGOUT_CD_TMP(insertFileToBMD(	bi, dtg_set3, &resp_dtg_set3, path_dest2));
				}
				else
				{

				}
			}
		}

	}

	/************************************************************************************************/
	/*						STWORZENIE LISTY PLIKOW DO SPAKOWANIA				*/
	/************************************************************************************************/
	/* --------- wartosc dowodowa --------------------*/
	asprintf(&file_tsr,"%s",path_dest);
	asprintf(&file_sig,"%s",path_dest);

	for (i=0; i<resp_dtg_set->bmdDatagramSetTable[0]->no_of_pkiMetaData; i++)
	{
		/******************************/
		/*	znaczniki czasu pliku	*/
		/******************************/
		if (strcmp(	resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[i]->OIDTableBuf,timestamp)==0)
		{
			BMD_FOK(set_gen_buf2(resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[i]->AnyBuf,
			resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[i]->AnySize, &metadata_value));

			asprintf(&file_tsr,"%s.tsr",file_tsr);
			bmd_save_buf(metadata_value, file_tsr);
			free_gen_buf(&metadata_value);

			zip_list_size++;
			zip_list=(char **)realloc(zip_list,sizeof(char *)*zip_list_size);

			zip_list[zip_list_size-1]=(char*)malloc(strlen(file_tsr)+2);
			memset(zip_list[zip_list_size-1], 0, strlen(file_tsr)+1);
			memcpy(zip_list[zip_list_size-1], file_tsr, strlen(file_tsr));

		}

		/************************/
		/*	podpisy pliku	*/
		/************************/
		if (strcmp(	resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[i]->OIDTableBuf,signature)==0 ||
			strcmp(	resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[i]->OIDTableBuf,signature_pades) == 0)
		{
			BMD_FOK(set_gen_buf2(resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[i]->AnyBuf,
			resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[i]->AnySize, &metadata_value));

			sig_no++;
			dvcs_no=0;

			asprintf(&dir_sig,"%s_%li/",zip_signature_directory_name,sig_no);
			SOAP_FOK_LOGOUT_TMP(mkdir(dir_sig, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH));

			asprintf(&file_sig,"%s%s.sig",dir_sig,path_dest);
			asprintf(&file_sig2,"%s.sig",path_dest);
			
			// dla PAdES nie jest umieszczany w ZIPie wyodrebniony podpis (podpis jest wewnatrz pliku PDF)
			if(strcmp(	resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[i]->OIDTableBuf,signature_pades) != 0)
			{
				status=bmd_save_buf(metadata_value, file_sig);

				zip_list_size++;
				zip_list=(char **)realloc(zip_list,sizeof(char *)*zip_list_size);

				zip_list[zip_list_size-1]=(char*)malloc(strlen(file_sig)+2);
				memset(zip_list[zip_list_size-1], 0, strlen(file_sig)+1);
				memcpy(zip_list[zip_list_size-1], file_sig, strlen(file_sig));
			}
			free_gen_buf(&metadata_value);

			asprintf(&file_sig_tsr,"%s",file_sig);
			asprintf(&file_sig_dvcs,"%s",file_sig);


			for (j=0; j<resp_dtg_set->bmdDatagramSetTable[0]->no_of_pkiMetaData; j++)
			{
				/******************************************/
				/*	pobranie znacznika czasu podpisu	*/
				/******************************************/
				if ((strcmp(	resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[j]->OIDTableBuf,tim_sig)==0) &&
							(resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[i]->myId==resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[j]->ownerId))
				{
					BMD_FOK(set_gen_buf2(resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[j]->AnyBuf,
					resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[j]->AnySize, &metadata_value));

					asprintf(&file_sig_tsr,"%s.tsr",file_sig_tsr);
					bmd_save_buf(metadata_value, file_sig_tsr);
					free_gen_buf(&metadata_value);

					zip_list_size++;
					zip_list=(char **)realloc(zip_list,sizeof(char *)*zip_list_size);

					zip_list[zip_list_size-1]=(char*)malloc(strlen(file_sig_tsr)+2);
					memset(zip_list[zip_list_size-1], 0, strlen(file_sig_tsr)+1);
					memcpy(zip_list[zip_list_size-1], file_sig_tsr, strlen(file_sig_tsr));


					free_gen_buf(&metadata_value);
				}


				/******************************************/
				/*	pobranie poswiadczenia dvcs podpisu	*/
				/******************************************/
				if ((strcmp(	resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[j]->OIDTableBuf,dvcs)==0) &&
							(resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[i]->myId==resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[j]->ownerId))
				{
					BMD_FOK(set_gen_buf2(resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[j]->AnyBuf,
					resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[j]->AnySize, &metadata_value));

					dvcs_no++;

					asprintf(&dir_dvcs,"%s_%li/%s_%li/",zip_signature_directory_name, sig_no, zip_dvcs_directory_name, dvcs_no);
					SOAP_FOK_LOGOUT_TMP(mkdir(dir_dvcs, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH));

					asprintf(&file_sig_dvcs,"%s%s.cpd.dvcs",dir_dvcs,file_sig2);
					bmd_save_buf(metadata_value, file_sig_dvcs);
					free_gen_buf(&metadata_value);

					zip_list_size++;
					zip_list=(char **)realloc(zip_list,sizeof(char *)*zip_list_size);

					zip_list[zip_list_size-1]=(char*)malloc(strlen(file_sig_dvcs)+2);
					memset(zip_list[zip_list_size-1], 0, strlen(file_sig_dvcs)+1);
					memcpy(zip_list[zip_list_size-1], file_sig_dvcs, strlen(file_sig_dvcs));

					free_gen_buf(&metadata_value);

					asprintf(&file_sig_dvcs_tsr,"%s",file_sig_dvcs);



					/************************************************************/
					/*	wyszukiwanie znacznikow czasu dla poswiadczenia dvcs	*/
					/************************************************************/
					for (k=0; k<resp_dtg_set->bmdDatagramSetTable[0]->no_of_pkiMetaData; k++)
					{
						if ((strcmp(	resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[k]->OIDTableBuf,tim_dvcs)==0) &&
									(resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[j]->myId==resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[k]->ownerId))
						{
							BMD_FOK(set_gen_buf2(resp_dtg_set->bmdDatagramSetTable[0]->
							pkiMetaData[k]->AnyBuf, resp_dtg_set->bmdDatagramSetTable[0]->
							pkiMetaData[k]->AnySize, &metadata_value));

							asprintf(&file_sig_dvcs_tsr,"%s.tsr",file_sig_dvcs_tsr);
							bmd_save_buf(metadata_value, file_sig_dvcs_tsr);
							free_gen_buf(&metadata_value);

							zip_list_size++;
							zip_list=(char **)realloc(zip_list,sizeof(char *)*zip_list_size);

							zip_list[zip_list_size-1]=(char*)malloc(strlen(file_sig_dvcs_tsr)+2);
							memset(zip_list[zip_list_size-1], 0, strlen(file_sig_dvcs_tsr)+1);
							memcpy(zip_list[zip_list_size-1], file_sig_dvcs_tsr, strlen(file_sig_dvcs_tsr));


							free_gen_buf(&metadata_value);

						}
					}
				}


			}
		}
	}

	/************************/
	/*	spakowanie plikow	*/
	/************************/
	asprintf(&path_dest_zip, "%s.zip", resp_dtg_set->bmdDatagramSetTable[0]->protocolDataFilename->buf);
	SOAP_FOK_LOGOUT_TMP(zipCompressFile(zip_list, zip_list_size, path_dest_zip));

	/************************/
	/*	ustawienie wyniku	*/
	/************************/
	(*output)=(struct bmd230__fileInfo *)soap_malloc(soap,sizeof(struct bmd230__fileInfo));
	memset(*output,0,sizeof(struct bmd230__fileInfo));

	/*	ustawienie nazwy pliku	*/
	(*output)->filename=(char *)soap_malloc(soap,strlen(path_dest_zip)+2);
	memset((*output)->filename, 0, strlen(path_dest_zip)+1);
	memmove((*output)->filename, path_dest_zip, strlen(path_dest_zip));

	/*	ustawienie pliku	*/
	(*output)->file=(struct bmd230__myBinaryDataType *)soap_malloc(soap,sizeof(struct bmd230__myBinaryDataType));
	(*output)->file->id = NULL;

	switch(type)
	{

	    case 0 : {
			/************************************************/
			/*	wczytanie spakowanego pliku do genbufa	*/
			/************************************************/

			free_gen_buf(&gb);
			bmd_load_binary_content(path_dest_zip,&gb);

			(*output)->file->__ptr=(unsigned char *)soap_malloc(soap,gb->size+2);
			memset((*output)->file->__ptr,0,gb->size+1);
			(*output)->file->__size=gb->size;
			memmove((*output)->file->__ptr,gb->buf,gb->size);

			(*output)->file->type = NULL;
			(*output)->file->options = NULL;

			break;
		     }

	    case 1 : {  /* Transmisja strumieniowa DIME */
			soap_set_omode(soap, SOAP_IO_CHUNK);
			fd = fopen(path_dest_zip,"r");
			(*output)->file->__ptr=(unsigned char *)fd;
			(*output)->file->__size=0;
			(*output)->file->type = "";
			(*output)->file->options = soap_dime_option(soap,0, (*output)->filename );

			break;
		     }

	}

	/******************************************/
	/*	usuwanie katalogow tymczasowych	*/
	/******************************************/
	for (i=0; i<sig_no; i++)
	{
		for (j=0; j<dvcs_no; j++)
		{
			asprintf(&path, "%s_%li/%s_%li/",zip_signature_directory_name,i+1,zip_dvcs_directory_name,j+1);
			remove_dir(path);
		}
		asprintf(&path, "%s_%li/",zip_signature_directory_name,i+1);
		remove_dir(path);
	}

	if (	(strcmp(zip_attachments_directory_name,".")!=0) &&
		(strcmp(zip_attachments_directory_name,"")!=0) && (xml_file_type != NULL || pdf_file_type != NULL))
	{
		asprintf(&path, "%s/",zip_attachments_directory_name);
		remove_dir(path);
	}

	/******************************************************************/
	/*	powrot do katalogu pierwotnego i usuniecie tymczasowego	*/
	/******************************************************************/
	chdir(_GLOBAL_current_dir);
	asprintf(&path, "%s%s%li/", tmp_dir, tmp_file, (long)getpid());
	remove_dir(path);


	// aktualizacja metadanych - wartosci parametru
	if(parameter != NULL)
	{
		long retVal = 0;
		long bmdId = strtoul(mainDocumentId, NULL, 10);
		const char* defaultUpdateReason = "Parameter's update";
		const char* confUpdateReason = NULL;
		bmdDatagram_t* metadataUpdateRequest = NULL;
		bmdDatagramSet_t* metadataUpdateRequestSet = NULL;
		bmdDatagramSet_t* metadataUpdateResponseSet = NULL;

		retVal = bmd_datagram_create(BMD_DATAGRAM_TYPE_UPDATE, &metadataUpdateRequest);
		SOAP_FOK_LOGOUT(retVal);

		retVal = bmd_datagram_set_id(&bmdId, &metadataUpdateRequest);
		if(retVal < BMD_OK)
		{
			bmd_datagram_free(&metadataUpdateRequest);
			SOAP_FOK_LOGOUT(retVal);
		}
		
		retVal = bmd_datagram_add_metadata_char(get_parameter_oid, parameter, &metadataUpdateRequest);
		if(retVal < BMD_OK)
		{
			bmd_datagram_free(&metadataUpdateRequest);
			SOAP_FOK_LOGOUT(retVal);
		}
	
		// ustawienie opisu aktualizacji
		confUpdateReason = bmdconf_get_value_static(_GLOBAL_konfiguracja,"soap","get_parameter_update_reason");
		if(confUpdateReason == NULL)
		{
			retVal=set_gen_buf2(defaultUpdateReason, strlen(defaultUpdateReason), &(metadataUpdateRequest->protocolData) );
		}
		else
		{
			retVal=set_gen_buf2(confUpdateReason, strlen(confUpdateReason), &(metadataUpdateRequest->protocolData) );
		}
		confUpdateReason = NULL;
		if(retVal < BMD_OK)
		{
			bmd_datagram_free(&metadataUpdateRequest);
			SOAP_FOK_LOGOUT(retVal);
		}
	
		retVal = bmd_datagram_add_to_set(metadataUpdateRequest, &metadataUpdateRequestSet);
		if(retVal < BMD_OK)
		{
			bmd_datagram_free(&metadataUpdateRequest);
			SOAP_FOK_LOGOUT(retVal);
		}

		retVal = bmd_send_request(bi, metadataUpdateRequestSet, max_datagram_in_set_transmission, &metadataUpdateResponseSet, 1, deserialisation_max_memory);
		bmd_datagramset_free(&metadataUpdateRequestSet);
		SOAP_FOK_LOGOUT(retVal);

		retVal = metadataUpdateResponseSet->bmdDatagramSetTable[0]->datagramStatus;
		bmd_datagramset_free(&metadataUpdateResponseSet);
		SOAP_FOK_LOGOUT(retVal);
	}


	/******************/
	/*	porzadki	*/
	/******************/
	bmd_logout(&bi, deserialisation_max_memory);
	bmd_info_destroy(&bi);
	bmd_end();

	if (dtg_set!=NULL) bmd_datagramset_free(&dtg_set);
	if (resp_dtg_set!=NULL) bmd_datagramset_free(&resp_dtg_set);
	if (dtg_set2!=NULL) bmd_datagramset_free(&dtg_set2);
	if (resp_dtg_set2!=NULL) bmd_datagramset_free(&resp_dtg_set2);
	if (dtg_set3!=NULL) bmd_datagramset_free(&dtg_set3);
	if (resp_dtg_set3!=NULL) bmd_datagramset_free(&resp_dtg_set3);
	if (dtg_set4!=NULL) bmd_datagramset_free(&dtg_set4);
	if (resp_dtg_set4!=NULL) bmd_datagramset_free(&resp_dtg_set4);
	free_gen_buf(&gb);
	free_gen_buf(&metadata_value);
	free(path); path=NULL;
	free(path_dest); path_dest=NULL;
	free(path_dest2); path_dest2=NULL;
	free(path_dest4); path_dest4=NULL;
	free(path_dest_zip); path_dest_zip=NULL;
	for (i=0; i<zip_list_size; i++)
	{
		free(zip_list[i]); zip_list[i]=NULL;
	}
	free(zip_list); zip_list=NULL;
	free(file_tsr); file_tsr=NULL;
	free(file_sig); file_sig=NULL;
	free(file_sig2); file_sig2=NULL;
	free(file_sig_tsr); file_sig_tsr=NULL;
	free(file_sig_dvcs); file_sig_dvcs=NULL;
	free(file_sig_dvcs_tsr); file_sig_dvcs_tsr=NULL;
	free(dir_sig); dir_sig=NULL;
	free(dir_dvcs); dir_dvcs=NULL;
	free(tmp_dir); tmp_dir=NULL;
	free(search_oid); search_oid=NULL;
	free(search_oid2); search_oid2=NULL;

	free(tmp_location); tmp_location=NULL;
	free(press_in_dir); press_in_dir=NULL;
	free(press_out_dir); press_out_dir=NULL;
	free(press_in_filename); press_in_filename=NULL;
	free(press_out_filename); press_out_filename=NULL;
	free(src); src=NULL;
	free(dest); dest=NULL;
	free(mtd_oid_str); mtd_oid_str=NULL;
	free(press_date_metadata); press_date_metadata=NULL;
	free(press_date_separator_in); press_date_separator_in=NULL;
	free(press_date_separator_out); press_date_separator_out=NULL;
	free(pdf_file_type); pdf_file_type=NULL;
	free(xml_file_type); xml_file_type=NULL;

	free(zip_signature_directory_name); zip_signature_directory_name=NULL;
	free(zip_dvcs_directory_name); zip_dvcs_directory_name=NULL;
	free(zip_attachments_directory_name); zip_attachments_directory_name=NULL;

	free(mainDocumentId); mainDocumentId = NULL;

	PRINT_SOAP("Status:\t\trequest served correctly\n");
	return SOAP_OK;
}


/***************************************************************************/
/* obsluga zadania pobrania paczki fakturowej po id i metadanej dodatkowej */
/***************************************************************************/
int bmd230__getHTMLImageById(	struct soap *soap,
				struct xsd__base64Binary *cert,
				char *roleName,
				char *groupName,
				char *userClassId,
				long int id,
				struct bmd230__fileInfo **output)
{
bmd_info_t *bi					= NULL;
lob_request_info_t *li				= NULL;
bmdDatagramSet_t *dtg_set			= NULL;
bmdDatagramSet_t *resp_dtg_set			= NULL;
GenBuf_t *gb					= NULL;
char *path					= NULL;
char *tmp_dir					= NULL;
char *xsl_pattern				= NULL;
char *xsl_version				= NULL;
char *file_html					= NULL;
char *id_str					= NULL;
char *tmp_location				= NULL;
char *xsl_pattern_dictionary			= NULL;
translate_t *translate				= NULL;
char tmp_file[]					= {"soap_get_html_"};
char oid_add_metadata_vis_version[]		= {OID_ADD_VISUALIZATION_VERSION};
long translate_count				= 0;
long i						= 0;
long max_datagram_in_set_transmission		= 0;
long deserialisation_max_memory			= 0;

char *derUserCert=NULL;
long derUserCertLen=0;
long retVal=0;
char* paramCert=NULL;
long paramCertLen=0;

	/***************************************/
	/* wczytanie ustawien konfiguracyjnych */
	/***************************************/
	bmdconf_get_value(_GLOBAL_konfiguracja,"soap","temp_dir",&tmp_dir);
	SOAP_FOK(bmdconf_get_value(_GLOBAL_konfiguracja,"convert","xsl_pattern",&xsl_pattern));
	bmdconf_get_value(_GLOBAL_konfiguracja,"convert","xsl_pattern_dictionary",&xsl_pattern_dictionary);

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
	asprintf(&tmp_location, "%s%s%li/", tmp_dir, tmp_file, (long)getpid());

	/**********************/
	/* stworzenie zadania */
	/**********************/
	SOAP_FOK_LOGOUT(bmd_lob_request_info_create(&li));

	/*********************************/
	/* odczytanie katalogu biezacego */
	/*********************************/
	memset(_GLOBAL_current_dir, 0 ,256);
	if (getcwd(_GLOBAL_current_dir,255)==NULL)
	{
		bmd_logout(&bi, deserialisation_max_memory);
		bmd_end();
		PRINT_ERROR("SOAPSERVERERR %s\n","Error in getting current directory");
		return soap_receiver_fault(soap,"Soap server error","Error in getting current directory");
	}

	/****************************************************************************************/
	/* stworzenie katalogu tymczasowego dla pobranych danych z serwera i przejscie do niego */
	/****************************************************************************************/
	asprintf(&path, "%s%s%li/", tmp_dir, tmp_file, (long)getpid());
	SOAP_FOK_LOGOUT(mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH));
	SOAP_FOK_LOGOUT(chdir(path));

	/************************************/
	/*	okreslenie pliku tymczasowego	*/
	/************************************/
	asprintf(&path, "%s%li", tmp_file, (long)getpid());
	SOAP_FOK_LOGOUT_TMP(bmd_lob_request_info_add_filename(path, &li, BMD_RECV_LOB,1000000000));


	/******************************/
	/*	stworzenie datagramu	*/
	/******************************/
	asprintf(&id_str, "%li", id);

	SOAP_FOK_LOGOUT_TMP(createDatagram(	userClassId,
						(char *[]){OID_SYS_METADATA_CRYPTO_OBJECTS_ID},
						(char *[]){id_str},
						1,
						BMD_DATAGRAM_GET_CGI_LO,
						BMD_ONLY_GROUP_DOC,
						BMD_QUERY_EQUAL,
						&dtg_set));
	free0(id_str);

	/*****************************/
	/* wyslanie zadania lobowego */
	/*****************************/
	SOAP_FOK_LOGOUT_TMP(bmd_send_request_lo(bi,dtg_set, max_datagram_in_set_transmission, &resp_dtg_set, li, deserialisation_max_memory));
	SOAP_FOK_LOGOUT_TMP(resp_dtg_set->bmdDatagramSetTable[0]->datagramStatus);

	/****************************/
	/* okreslenie wersji html'a */
	/****************************/
	for (i=0; i<resp_dtg_set->bmdDatagramSetTable[0]->no_of_additionalMetaData; i++)
	{
		if (strcmp(oid_add_metadata_vis_version, resp_dtg_set->bmdDatagramSetTable[0]->additionalMetaData[i]->
		OIDTableBuf)==0)
		{
			asprintf(&xsl_version, "%s", resp_dtg_set->bmdDatagramSetTable[0]->additionalMetaData[i]->AnyBuf);
			break;
		}
	}

	/**********************************/
	/* wczytanie słownika wzorców xsl */
	/**********************************/
	if ((xsl_pattern_dictionary!=NULL) && (xsl_version!=NULL))
	{
		if (load_translation( xsl_pattern_dictionary, &translate, &translate_count)<BMD_OK)
		{
			SOAP_FOK_LOGOUT_TMP(BMDSOAP_SERVER_XSLT_PATTERN_DICTIONARY_OPEN_ERROR);
		}

		for (i=0; i<translate_count; i++)
		{
			if (strcmp(xsl_version, translate[i].from)==0)
			{
				free(xsl_pattern); xsl_pattern=NULL;
				asprintf(&xsl_pattern, "%s", translate[i].to);
				break;
			}
		}
	}

	/******************************************************************************/
	/*	przeksztalcenie pobranego pliku na html zgodnie ze wzorem xsl_pattern	*/
	/******************************************************************************/
	asprintf(&file_html, "%s.html", resp_dtg_set->bmdDatagramSetTable[0]->protocolDataFilename->buf);
	SOAP_FOK_LOGOUT_TMP(convertXmlToHtml(path, xsl_pattern, file_html));

	/************************/
	/*	ustawienie wyniku	*/
	/************************/
	(*output)=(struct bmd230__fileInfo *)soap_malloc(soap,sizeof(struct bmd230__fileInfo));
	memset(*output,0,sizeof(struct bmd230__fileInfo));

	/*	ustawienie nazwy pliku	*/
	(*output)->filename=(char *)soap_malloc(soap,strlen(file_html)+2);
	memset((*output)->filename, 0, strlen(file_html)+1);
	memmove((*output)->filename, file_html, strlen(file_html));

	/*	ustawienie pliku	*/
	(*output)->file=(struct bmd230__myBinaryDataType *)soap_malloc(soap,sizeof(struct bmd230__myBinaryDataType));
	(*output)->file->id = NULL;

	/**************************************************/
	/*	wczytanie obrazu html faktury	do genbufa*/
	/**************************************************/

	free_gen_buf(&gb);
	bmd_load_binary_content(file_html,&gb);

	(*output)->file->__ptr=(unsigned char *)soap_malloc(soap,gb->size+2);
	memset((*output)->file->__ptr,0,gb->size+1);
	(*output)->file->__size=gb->size;
	memmove((*output)->file->__ptr,gb->buf,gb->size);
	(*output)->file->type = NULL;
	(*output)->file->options = NULL;

	/********************************************/
	/* usuniecie glownego katalogu tymczasowego */
	/********************************************/
	remove_dir(tmp_location);

	/************/
	/* porzadki */
	/************/
	bmd_logout(&bi, deserialisation_max_memory);
	bmd_info_destroy(&bi);
	bmd_end();
	bmd_lob_request_info_destroy(&li);

	bmd_datagramset_free(&dtg_set);
	bmd_datagramset_free(&resp_dtg_set);
	free_gen_buf(&gb);
	free0(path);
	free0(tmp_dir);
	free0(xsl_version);
	free0(xsl_pattern);
	free0(xsl_pattern_dictionary);
	free0(file_html);

	for (i=0; i<translate_count; i++)
	{
		free0(translate[i].from);
		free0(translate[i].to);
	}
	free0(translate);

	PRINT_SOAP("Status:\t\trequest served correctly\n");
	return SOAP_OK;
}

/***************************************************************************/
/* obsluga zadania pobrania paczki fakturowej po id i metadanej dodatkowej */
/***************************************************************************/
int bmd230__getHTMLImage(	struct soap *soap,
			char *accountId,
			char *invoiceId,
			char *userIdentifier,
			char *getReason,
			struct xsd__base64Binary *cert,
			char *roleName,
			char *groupName,
			char *userClassId,
			enum trans_t type,
			struct bmd230__fileInfo **output)
{
bmd_info_t *bi					= NULL;
lob_request_info_t *li				= NULL;
bmdDatagramSet_t *dtg_set			= NULL;
bmdDatagramSet_t *resp_dtg_set			= NULL;
GenBuf_t *gb					= NULL;
char *path					= NULL;
char *tmp_dir					= NULL;
char *xsl_pattern				= NULL;
char *xsl_version				= NULL;
char *file_html					= NULL;
char *search_oid				= NULL;
char *search_oid2				= NULL;
char *search_oid3				= NULL;
char *tmp_location				= NULL;
char *xsl_pattern_dictionary			= NULL;
char *getHTMLImage_select_type			= NULL;
translate_t *translate				= NULL;
char tmp_file[]					= {"soap_get_html_"};
char oid_add_metadata_vis_version[]		= {OID_ADD_VISUALIZATION_VERSION};
long translate_count				= 0;
FILE* fd 					= NULL;
long i						= 0;
long max_datagram_in_set_transmission		= 0;
long deserialisation_max_memory			= 0;
long twl_select_operator			= 0;

char *derUserCert=NULL;
long derUserCertLen=0;
long retVal=0;
char* paramCert=NULL;
long paramCertLen=0;

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if(accountId==NULL)		{	SOAP_FOK(BMDSOAP_SERVER_EMPTY1_PARAM_ERROR);	}
	if(invoiceId==NULL)		{	SOAP_FOK(BMDSOAP_SERVER_EMPTY2_PARAM_ERROR);	}

	PRINT_SOAP("Request:\tget file's html image\n");
	PRINT_SOAP("accountId:\t%s\n",accountId);
	PRINT_SOAP("invoiceId:\t%s\n",invoiceId);
	PRINT_SOAP("userIdentifier:\t%s\n",userIdentifier);
	/******************************************/
	/*	wczytanie ustawien konfiguracyjnych	*/
	/******************************************/
	bmdconf_get_value(_GLOBAL_konfiguracja,"soap","temp_dir",&tmp_dir);
	bmdconf_get_value(_GLOBAL_konfiguracja,"soap","search_oid1",&search_oid);
	bmdconf_get_value(_GLOBAL_konfiguracja,"soap","search_oid2",&search_oid2);
	bmdconf_get_value(_GLOBAL_konfiguracja,"soap","search_oid3",&search_oid3);
	SOAP_FOK(bmdconf_get_value(_GLOBAL_konfiguracja,"convert","xsl_pattern",&xsl_pattern));
	bmdconf_get_value(_GLOBAL_konfiguracja,"convert","xsl_pattern_dictionary",&xsl_pattern_dictionary);
	bmdconf_get_value(_GLOBAL_konfiguracja,"functionalities_behaviour","getHTMLImage_select_type",&getHTMLImage_select_type);
	if(getHTMLImage_select_type == NULL)
	{
		asprintf(&getHTMLImage_select_type,"EQUAL");
		if(getHTMLImage_select_type == NULL)
		{
			SOAP_FOK(NO_MEMORY);
		}
	}

	if(strcmp(getHTMLImage_select_type, "EQUAL") == 0)
	{
		twl_select_operator = BMD_QUERY_EQUAL;
	}
	else if(strcmp(getHTMLImage_select_type, "EXACT") == 0)
	{
		twl_select_operator = BMD_QUERY_EXACT_ILIKE;
	}
	else if(strcmp(getHTMLImage_select_type, "LIKE") == 0)
	{
		twl_select_operator = BMD_QUERY_LIKE;
	}
	else if(strcmp(getHTMLImage_select_type, "ILIKE") == 0)
	{
		twl_select_operator = BMD_QUERY_ILIKE;
	}
	else
	{
		SOAP_FOK(BMDSOAP_SERVER_SEARCH_OPERATOR_INVALID);
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

	/************************/
	/*	logowanie do bmd	*/
	/************************/
 	SOAP_FOK(PrepareDataForBMDLoginAndRequest(&bi,
				(_GLOBAL_soap_wssecurity_enabled == 1  ? derUserCert : paramCert),
				(_GLOBAL_soap_wssecurity_enabled == 1  ? derUserCertLen : paramCertLen),
				roleName, groupName, deserialisation_max_memory));
	free(derUserCert); derUserCert=NULL;
	paramCert=NULL; paramCertLen=0;
	asprintf(&tmp_location, "%s%s%li/", tmp_dir, tmp_file, (long)getpid());

	/******************************/
	/*	stworzenie zadania	*/
	/******************************/
	SOAP_FOK_LOGOUT(bmd_lob_request_info_create(&li));

	/************************************/
	/*	odczytanie katalogu biezacego	*/
	/************************************/
	memset(_GLOBAL_current_dir, 0 ,256);
	if (getcwd(_GLOBAL_current_dir,255)==NULL)
	{
		bmd_logout(&bi, deserialisation_max_memory);
		bmd_end();
		PRINT_ERROR("SOAPSERVERERR %s\n","Error in getting current directory");
		return soap_receiver_fault(soap,"Soap server error","Error in getting current directory");
	}

	/************************************************************************************************/
	/*	stworzenie katalogu tymczasowego dla pobranych danych z serwera i przejscie do niego	*/
	/************************************************************************************************/
	asprintf(&path, "%s%s%li/", tmp_dir, tmp_file, (long)getpid());
	SOAP_FOK_LOGOUT(mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH));
	SOAP_FOK_LOGOUT(chdir(path));

	/************************************/
	/*	okreslenie pliku tymczasowego	*/
	/************************************/
	asprintf(&path, "%s%li", tmp_file, (long)getpid());
	SOAP_FOK_LOGOUT_TMP(bmd_lob_request_info_add_filename(path, &li, BMD_RECV_LOB,1000000000));


	/******************************/
	/*	stworzenie datagramu	*/
	/******************************/
	SOAP_FOK_LOGOUT_TMP(createDatagram(	userClassId,
						(char *[]){search_oid,search_oid2,search_oid3},
						(char *[]){accountId,invoiceId,userIdentifier},
						3,
						BMD_DATAGRAM_GET_CGI_LO,
						BMD_ONLY_GROUP_DOC,
						twl_select_operator,
						&dtg_set));

	/******************************************/
	/*	ustawienie powodu pobrania faktury	*/
	/******************************************/
	if ((getReason!=NULL) && (strlen(getReason)>0))
	{
		dtg_set->bmdDatagramSetTable[0]->protocolData=(GenBuf_t *)malloc(sizeof(GenBuf_t));
		dtg_set->bmdDatagramSetTable[0]->protocolData->buf=(char *)malloc(strlen(getReason)+2);
		memset(dtg_set->bmdDatagramSetTable[0]->protocolData->buf, 0, strlen(getReason)+1);
		memcpy(dtg_set->bmdDatagramSetTable[0]->protocolData->buf, getReason, strlen(getReason));
		dtg_set->bmdDatagramSetTable[0]->protocolData->size=strlen(getReason);
	}

	/************************************/
	/*	wyslanie zadania lobowego	*/
	/************************************/
	SOAP_FOK_LOGOUT_TMP(bmd_send_request_lo(bi,dtg_set, max_datagram_in_set_transmission, &resp_dtg_set, li, deserialisation_max_memory));
	SOAP_FOK_LOGOUT_TMP(resp_dtg_set->bmdDatagramSetTable[0]->datagramStatus);

	/****************************/
	/* okreslenie wersji html'a */
	/****************************/
	for (i=0; i<resp_dtg_set->bmdDatagramSetTable[0]->no_of_additionalMetaData; i++)
	{
		if (strcmp(oid_add_metadata_vis_version, resp_dtg_set->bmdDatagramSetTable[0]->additionalMetaData[i]->
		OIDTableBuf)==0)
		{
			asprintf(&xsl_version, "%s", resp_dtg_set->bmdDatagramSetTable[0]->additionalMetaData[i]->AnyBuf);
			break;
		}
	}

	/**********************************/
	/* wczytanie słownika wzorców xsl */
	/**********************************/
	if ((xsl_pattern_dictionary!=NULL) && (xsl_version!=NULL))
	{
		if (load_translation( xsl_pattern_dictionary, &translate, &translate_count)<BMD_OK)
		{
			SOAP_FOK_LOGOUT_TMP(BMDSOAP_SERVER_XSLT_PATTERN_DICTIONARY_OPEN_ERROR);
		}

		for (i=0; i<translate_count; i++)
		{
			if (strcmp(xsl_version, translate[i].from)==0)
			{
				free(xsl_pattern); xsl_pattern=NULL;
				asprintf(&xsl_pattern, "%s", translate[i].to);
				break;
			}
		}
	}

	/******************************************************************************/
	/*	przeksztalcenie pobranego pliku na html zgodnie ze wzorem xsl_pattern	*/
	/******************************************************************************/
	asprintf(&file_html, "%s.html", resp_dtg_set->bmdDatagramSetTable[0]->protocolDataFilename->buf);
	SOAP_FOK_LOGOUT_TMP(convertXmlToHtml(path, xsl_pattern, file_html));

	/************************/
	/*	ustawienie wyniku	*/
	/************************/
	(*output)=(struct bmd230__fileInfo *)soap_malloc(soap,sizeof(struct bmd230__fileInfo));
	memset(*output,0,sizeof(struct bmd230__fileInfo));

	/*	ustawienie nazwy pliku	*/
	(*output)->filename=(char *)soap_malloc(soap,strlen(file_html)+2);
	memset((*output)->filename, 0, strlen(file_html)+1);
	memmove((*output)->filename, file_html, strlen(file_html));

	/*	ustawienie pliku	*/
	(*output)->file=(struct bmd230__myBinaryDataType *)soap_malloc(soap,sizeof(struct bmd230__myBinaryDataType));
	(*output)->file->id = NULL;

	switch(type)
	{

	    case 0 : {
			/**************************************************/
			/*	wczytanie obrazu html faktury	do genbufa*/
			/**************************************************/

			free_gen_buf(&gb);
			bmd_load_binary_content(file_html,&gb);

			(*output)->file->__ptr=(unsigned char *)soap_malloc(soap,gb->size+2);
			memset((*output)->file->__ptr,0,gb->size+1);
			(*output)->file->__size=gb->size;
			memmove((*output)->file->__ptr,gb->buf,gb->size);
			(*output)->file->type = NULL;
			(*output)->file->options = NULL;

			break;
		     }

	    case 1 : {
			soap_set_omode(soap, SOAP_IO_CHUNK);
			fd = fopen(file_html,"r");
			(*output)->file->__ptr=(unsigned char *)fd;
			(*output)->file->__size=0;
			(*output)->file->type = "";
			(*output)->file->options = soap_dime_option(soap,0, (*output)->filename );

			break;
		     }

	}

	/************************************************/
	/*	usuniecie glownego katalogu tymczasowego	*/
	/************************************************/
	remove_dir(tmp_location);

	/******************/
	/*	porzadki	*/
	/******************/
	bmd_logout(&bi, deserialisation_max_memory);
	bmd_info_destroy(&bi);
	bmd_end();
	bmd_lob_request_info_destroy(&li);
	bmd_datagramset_free(&dtg_set);
	bmd_datagramset_free(&resp_dtg_set);
	free_gen_buf(&gb);
	free(path); path=NULL;
	free(tmp_dir); tmp_dir=NULL;
	free(xsl_version); xsl_version=NULL;
	free(xsl_pattern); xsl_pattern=NULL;
	free(xsl_pattern_dictionary); xsl_pattern_dictionary=NULL;
	free(file_html); file_html=NULL;
	free(search_oid); search_oid=NULL;
	free(search_oid2); search_oid2=NULL;
	for (i=0; i<translate_count; i++)
	{
		free(translate[i].from); translate[i].from=NULL;
		free(translate[i].to); translate[i].to=NULL;
	}
	free(translate); translate=NULL;

	PRINT_SOAP("Status:\t\trequest served correctly\n");
	return SOAP_OK;
}

int _verifyInvoice(	struct soap *soap,
			char *accountId,
			char *invoiceId,
			char *userIdentifier,
			int forceVerification,
			struct xsd__base64Binary *cert,
			char *roleName,
			char *groupName,
			char *userClassId,
			int simplifiedVerificationFlag,
			int *result)
{
bmdDatagramSet_t *dtg_set		= NULL;
bmdDatagramSet_t *resp_dtg_set	= NULL;
bmd_info_t *bi				= NULL;
char *search_oid				= NULL;
char *search_oid2				= NULL;
char *search_oid3				= NULL;
char *verifyInvoice_select_type			= NULL;
long dvcs_status				= 0;
long status					= 0;
long max_datagram_in_set_transmission		= 0;
long deserialisation_max_memory			= 0;
long twl_select_operator			= 0;

char *derUserCert=NULL;
long derUserCertLen=0;
long retVal=0;
char* paramCert=NULL;
long paramCertLen=0;

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if(accountId==NULL)	{	SOAP_FOK(BMDSOAP_SERVER_EMPTY1_PARAM_ERROR);	}
	if(invoiceId==NULL)	{	SOAP_FOK(BMDSOAP_SERVER_EMPTY2_PARAM_ERROR);	}

	PRINT_SOAP("Request:\tget document's signature status\n");
	PRINT_SOAP("accountId:\t%s\n",accountId);
	PRINT_SOAP("invoiceId:\t%s\n",invoiceId);
	PRINT_SOAP("userIdentifier:\t%s\n",userIdentifier);
	/******************************************/
	/*	wczytanie ustawien konfiguracyjnych	*/
	/******************************************/
	bmdconf_get_value(_GLOBAL_konfiguracja,"soap","search_oid1",&search_oid);
	bmdconf_get_value(_GLOBAL_konfiguracja,"soap","search_oid2",&search_oid2);
	bmdconf_get_value(_GLOBAL_konfiguracja,"soap","search_oid3",&search_oid3);
	bmdconf_get_value(_GLOBAL_konfiguracja,"functionalities_behaviour","verifyInvoice_select_type",&verifyInvoice_select_type);
	if(verifyInvoice_select_type == NULL)
	{
		asprintf(&verifyInvoice_select_type,"EQUAL");
		if(verifyInvoice_select_type == NULL)
		{
			SOAP_FOK(NO_MEMORY);
		}
	}

	if(strcmp(verifyInvoice_select_type, "EQUAL") == 0)
	{
		twl_select_operator = BMD_QUERY_EQUAL;
	}
	else if(strcmp(verifyInvoice_select_type, "EXACT") == 0)
	{
		twl_select_operator = BMD_QUERY_EXACT_ILIKE;
	}
	else if(strcmp(verifyInvoice_select_type, "LIKE") == 0)
	{
		twl_select_operator = BMD_QUERY_LIKE;
	}
	else if(strcmp(verifyInvoice_select_type, "ILIKE") == 0)
	{
		twl_select_operator = BMD_QUERY_ILIKE;
	}
	else
	{
		SOAP_FOK(BMDSOAP_SERVER_SEARCH_OPERATOR_INVALID);
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

	if (forceVerification==1)
	{
		/************************************/
		/*	stworzenie datagramu zadania	*/
		/************************************/
		SOAP_FOK(createDatagram(	userClassId,
						(char *[]){search_oid,search_oid2,search_oid3},
						(char *[]){accountId,invoiceId,userIdentifier},
						3,
						BMD_DATAGRAM_TYPE_VALIDATE_SIGNATURE,
						BMD_ONLY_GROUP_DOC,
						twl_select_operator,
						&dtg_set));

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

		/***********************************************************************/
		/* dla uproszczonej weryfikacji nie bedzie uwzgledniony znacznik czasu */
		/***********************************************************************/
		if(simplifiedVerificationFlag != 0)
		{
			SOAP_FOK(set_gen_buf2("VERIFY_WITHOUT_TIMESTAMP", strlen("VERIFY_WITHOUT_TIMESTAMP"), &(dtg_set->bmdDatagramSetTable[0]->protocolData)) );
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
		/*	wyslanie zadania do archiwum	*/
		/************************************/
		SOAP_FOK_LOGOUT(bmd_send_request(bi,dtg_set, max_datagram_in_set_transmission, &resp_dtg_set, 1, deserialisation_max_memory));

		/************************/
		/*	ustawienie wyniku */
		/************************/
		if (resp_dtg_set->bmdDatagramSetTable[0]->datagramStatus != BMD_OK)
		{
			SOAP_FOK_LOGOUT(resp_dtg_set->bmdDatagramSetTable[0]->datagramStatus);
		}

		SOAP_FOK_LOGOUT(bmd_datagram_get_metadata_ownerType(PKI_METADATA, OID_PKI_METADATA_DVCS,resp_dtg_set->bmdDatagramSetTable[0], -1, &dvcs_status));
		switch (dvcs_status)
		{
			case PLUGIN_DVCS_SIGNATURE_INVALID : 			(*result)=2; break;
			case PLUGIN_DVCS_SIGNATURE_VERIFIED :			(*result)=1; break;
			case PLUGIN_DVCS_SIGNATURE_VERIFIED_CONDITIONALLY :	(*result)=3; break;
			default :	(*result)=dvcs_status; break;
		}
	}
	else
	{
		/************************************/
		/*	stworzenie datagramu zadania	*/
		/************************************/
		SOAP_FOK(createDatagram(	userClassId,
						(char *[]){search_oid,search_oid2,search_oid3},
						(char *[]){accountId,invoiceId,userIdentifier},
						3,
						BMD_DATAGRAM_GET_METADATA,
						BMD_ONLY_GROUP_DOC,
						twl_select_operator,
						&dtg_set));

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

		/***********************************************************************/
		/* dla uproszczonej weryfikacji nie bedzie uwzgledniony znacznik czasu */
		/***********************************************************************/
		if(simplifiedVerificationFlag != 0)
		{
			SOAP_FOK(set_gen_buf2("VERIFY_WITHOUT_TIMESTAMP", strlen("VERIFY_WITHOUT_TIMESTAMP"), &(dtg_set->bmdDatagramSetTable[0]->protocolData)) );
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
		/*	wyslanie zadania do archiwum	*/
		/************************************/
		SOAP_FOK_LOGOUT(bmd_send_request(bi, dtg_set, max_datagram_in_set_transmission, &resp_dtg_set, 1, deserialisation_max_memory));

		/************************/
		/*	ustawienie wyniku */
		/************************/
		if(resp_dtg_set->bmdDatagramSetTable[0]->datagramStatus != BMD_OK)
		{
			SOAP_FOK_LOGOUT(resp_dtg_set->bmdDatagramSetTable[0]->datagramStatus);
		}

		status=bmd_datagram_get_metadata_ownerType(PKI_METADATA, OID_PKI_METADATA_DVCS,resp_dtg_set->bmdDatagramSetTable[0], -1, &dvcs_status);
		if (status<BMD_OK)
		{
			if (status==-5)
			{
				SOAP_FOK_LOGOUT(BMDSOAP_SERVER_NO_SELECTED_PKI_METADATA);
			}
			SOAP_FOK_LOGOUT(BMDSOAP_SERVER_GETPKI_METADATA_ERR);
		}

		switch (dvcs_status)
		{
			case PLUGIN_DVCS_INSERTION_ACCEPTED_INVALID_SIGNATURE: //fall down
			case PLUGIN_DVCS_SIGNATURE_INVALID : 			(*result)=2; break;
			case PLUGIN_DVCS_INSERTION_SIGNATURE_VERIFIED: //fall down
			case PLUGIN_DVCS_SIGNATURE_VERIFIED :			(*result)=1; break;
			case PLUGIN_DVCS_INSERTION_SIGNATURE_VERIFIED_CONDITIONALLY: //fall down
			case PLUGIN_DVCS_SIGNATURE_VERIFIED_CONDITIONALLY :	(*result)=3; break;
			default :	(*result)=dvcs_status; break;
		}
	}

	/************/
	/* porzadki */
	/************/
	bmd_logout(&bi, deserialisation_max_memory);
	bmd_info_destroy(&bi);
	bmd_end();

	bmd_datagramset_free(&dtg_set);
	bmd_datagramset_free(&resp_dtg_set);
	free(search_oid); search_oid=NULL;
	free(search_oid2); search_oid2=NULL;

	PRINT_SOAP("Status:\t\trequest served correctly\n");
	return BMD_OK;
}

/*****************************************************************************************/
/* weryfkikacja dokumentu przez plugin DVCS i zwrócenie statusu odpowiedzi uzytkownikowi */
/*****************************************************************************************/
int bmd230__verifyInvoice(	struct soap *soap,
			char *accountId,
			char *invoiceId,
			char *userIdentifier,
			int forceVerification,
			struct xsd__base64Binary *cert,
			char *roleName,
			char *groupName,
			char *userClassId,
			int *result)
{
	return _verifyInvoice(soap, accountId, invoiceId, userIdentifier, forceVerification, cert, roleName, groupName, userClassId, 0/*standard verification*/, result);
}

/************************************************************************************************/
/*	weryfkikacja dokumentu przez plugin DVCS i zwrócenie statusu odpowiedzi uzytkownikowi	*/
/*	BEZ UWZGLEDNIANIA ZNACZNIKA CZASU PRZY WERYFIKACJI					*/
/************************************************************************************************/
int bmd230__verifyInvoiceSimplified(struct soap *soap,
				char *accountId,
				char *invoiceId,
				char *userIdentifier,
				int forceVerification,
				struct xsd__base64Binary *cert,
				char *roleName,
				char *groupName,
				char *userClassId,
				int *result)
{
	return _verifyInvoice(soap, accountId, invoiceId, userIdentifier, forceVerification, cert, roleName, groupName, userClassId, 1/*simplified verification*/, result);
}


/*****************************************************************************************/
/* weryfkikacja dokumentu przez plugin DVCS i zwrócenie statusu odpowiedzi uzytkownikowi */
/*****************************************************************************************/
int bmd230__verifyInvoiceById(	struct soap *soap,
				struct xsd__base64Binary *cert,
				char *roleName,
				char *groupName,
				char *userClassId,
				long int id,
				int forceVerification,
				int completeVerification,
				struct bmd230__stringList **result)
{
bmdDatagramSet_t *dtg_set		= NULL;
bmdDatagramSet_t *resp_dtg_set		= NULL;
bmd_info_t *bi				= NULL;
char *id_str				= NULL;
long dvcs_status			= 0;
long max_datagram_in_set_transmission	= 0;
long deserialisation_max_memory		= 0;
long i					= 0;

char *derUserCert=NULL;
long derUserCertLen=0;
long retVal=0;
char* paramCert=NULL;
long paramCertLen=0;

	PRINT_SOAP("Request:\tget invoice verification status\n");
 	/***************************************/
 	/* wczytanie ustawien konfiguracyjnych */
 	/***************************************/
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

	/********************************/
	/* stworzenie datagramu zadania */
	/********************************/
	asprintf(&id_str, "%li", id);
	if (id_str==NULL)	{	SOAP_FOK(NO_MEMORY);	}

	SOAP_FOK(createDatagram(	userClassId,
					NULL,
					NULL,
					0,
					forceVerification==1?BMD_DATAGRAM_TYPE_VALIDATE_SIGNATURE:BMD_DATAGRAM_GET_METADATA,
					BMD_ONLY_GROUP_DOC,
					BMD_QUERY_EQUAL,
					&dtg_set));

	/********************************************/
	/* ustawienie id badanego liku w datagramie */
	/********************************************/
	SOAP_FOK(set_gen_buf2(id_str, strlen(id_str), &(dtg_set->bmdDatagramSetTable[0]->protocolDataFileId)));

	if (completeVerification!=1)
	{
		SOAP_FOK(set_gen_buf2("single_signature", strlen("single_signature"), &(dtg_set->bmdDatagramSetTable[0]->protocolDataFilename)));
	}
	free0(id_str);

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
	/* wyslanie zadania do archiwum */
	/********************************/
	SOAP_FOK_LOGOUT(bmd_send_request(bi,dtg_set, max_datagram_in_set_transmission, &resp_dtg_set, 1, deserialisation_max_memory));
	SOAP_FOK_LOGOUT(resp_dtg_set->bmdDatagramSetTable[0]->datagramStatus);

	/*********************/
	/* ustawienie wyniku */
	/*********************/
	(*result)=(struct bmd230__stringList *)malloc(sizeof(struct bmd230__stringList));
	(*result)->__size=0;
	(*result)->__ptr=(xsd__string *)malloc(sizeof(xsd__string)*(*result)->__size);

	for (i=0; i<resp_dtg_set->bmdDatagramSetTable[0]->no_of_pkiMetaData; i++)
	{

		if (strcmp(resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[i]->OIDTableBuf, OID_PKI_METADATA_DVCS)==0)
		{
			if (	completeVerification!=1 &&
				forceVerification!=1 &&
				resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[i]->ownerId!=id)
			{
				continue;
			}

			(*result)->__size++;
			(*result)->__ptr=(xsd__string *)realloc((*result)->__ptr, sizeof(xsd__string)*(*result)->__size);

			dvcs_status=resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[i]->ownerType;

			switch (dvcs_status)
			{
				case PLUGIN_DVCS_SIGNATURE_INVALID : 			asprintf(&((*result)->__ptr[(*result)->__size-1]), "2"); break;
				case PLUGIN_DVCS_SIGNATURE_VERIFIED :			asprintf(&((*result)->__ptr[(*result)->__size-1]), "1"); break;
				case PLUGIN_DVCS_SIGNATURE_VERIFIED_CONDITIONALLY :	asprintf(&((*result)->__ptr[(*result)->__size-1]), "3"); break;
				case BMD_OK :						asprintf(&((*result)->__ptr[(*result)->__size-1]), "1"); break;
				default :	asprintf(&((*result)->__ptr[(*result)->__size-1]), "%li", dvcs_status); break;
			}

			if ((*result)->__ptr[(*result)->__size-1]==NULL)		{	SOAP_FOK_LOGOUT(NO_MEMORY);	}
		}

	}
	if ((*result)->__size==0)
	{
		SOAP_FOK_LOGOUT(BMDSOAP_SERVER_NO_SELECTED_PKI_METADATA);
	}

	/************/
	/* porzadki */
	/************/
	bmd_logout(&bi, deserialisation_max_memory);
	bmd_info_destroy(&bi);
	bmd_end();

	bmd_datagramset_free(&dtg_set);
	bmd_datagramset_free(&resp_dtg_set);

	PRINT_SOAP("Status:\t\trequest served correctly\n");
	return BMD_OK;
}


/*****************************************************************************************/
/* weryfkikacja dokumentu przez plugin DVCS i zwrócenie statusu odpowiedzi uzytkownikowi */
/*****************************************************************************************/
int bmd230__verifyInvoiceWithDetailsById(	struct soap *soap,
					struct xsd__base64Binary *cert,
					char *roleName,
					char *groupName,
					char *userClassId,
					long int id,
					int forceVerification,
					int completeVerification,
					struct bmd230__fileInfoList **output)
{
bmdDatagramSet_t *dtg_set		= NULL;
bmdDatagramSet_t *resp_dtg_set		= NULL;
bmd_info_t *bi				= NULL;
char *id_str				= NULL;
long max_datagram_in_set_transmission	= 0;
long deserialisation_max_memory		= 0;
long i					= 0;

char *derUserCert=NULL;
long derUserCertLen=0;
long retVal=0;
char* paramCert=NULL;
long paramCertLen=0;

	PRINT_SOAP("Request:\tget invoice verification\n");
 	/***************************************/
 	/* wczytanie ustawien konfiguracyjnych */
 	/***************************************/
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

	/********************************/
	/* stworzenie datagramu zadania */
	/********************************/
	asprintf(&id_str, "%li", id);
	if (id_str==NULL)	{	SOAP_FOK(NO_MEMORY);	}

	SOAP_FOK(createDatagram(	userClassId,
					NULL,
					NULL,
					0,
					forceVerification==1?BMD_DATAGRAM_TYPE_VALIDATE_SIGNATURE:BMD_DATAGRAM_GET_METADATA,
					BMD_ONLY_GROUP_DOC,
					BMD_QUERY_EQUAL,
					&dtg_set));

	/********************************************/
	/* ustawienie id badanego liku w datagramie */
	/********************************************/
	SOAP_FOK(set_gen_buf2(id_str, strlen(id_str), &(dtg_set->bmdDatagramSetTable[0]->protocolDataFileId)));

	if (completeVerification!=1)
	{
		SOAP_FOK(set_gen_buf2("single_signature", strlen("single_signature"), &(dtg_set->bmdDatagramSetTable[0]->protocolDataFilename)));
	}
	free0(id_str);

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
	/* wyslanie zadania do archiwum */
	/********************************/
	SOAP_FOK_LOGOUT(bmd_send_request(bi,dtg_set, max_datagram_in_set_transmission, &resp_dtg_set, 1, deserialisation_max_memory));
	SOAP_FOK_LOGOUT(resp_dtg_set->bmdDatagramSetTable[0]->datagramStatus);

	/*********************/
	/* ustawienie wyniku */
	/*********************/
	(*output)=(struct bmd230__fileInfoList *)malloc(sizeof(struct bmd230__fileInfoList));
	(*output)->__size=0;
	(*output)->__ptr=(struct bmd230__fileInfo *)malloc(sizeof(struct bmd230__fileInfo)*(*output)->__size);

	for (i=0; i<resp_dtg_set->bmdDatagramSetTable[0]->no_of_pkiMetaData; i++)
	{

		if (strcmp(resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[i]->OIDTableBuf, OID_PKI_METADATA_DVCS)==0)
		{

			if (	completeVerification!=1 &&
				forceVerification!=1 &&
				resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[i]->ownerId!=id)
			{
				continue;
			}

			(*output)->__size++;
			(*output)->__ptr=(struct bmd230__fileInfo *)realloc((*output)->__ptr, sizeof(struct bmd230__fileInfo)*(*output)->__size);

			(*output)->__ptr[(*output)->__size-1].file=(struct bmd230__myBinaryDataType *)malloc(sizeof(struct bmd230__myBinaryDataType));
			memset((*output)->__ptr[(*output)->__size-1].file, 0, sizeof(struct bmd230__myBinaryDataType));

			(*output)->__ptr[(*output)->__size-1].filename=NULL;

			(*output)->__ptr[(*output)->__size-1].file->id = NULL;
			(*output)->__ptr[(*output)->__size-1].file->type = NULL;
			(*output)->__ptr[(*output)->__size-1].file->options = NULL;

			(*output)->__ptr[(*output)->__size-1].file->__ptr=(unsigned char *)malloc(sizeof(unsigned char)*(resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[i]->AnySize+1));
			memset((*output)->__ptr[(*output)->__size-1].file->__ptr, 0, sizeof(unsigned char)*(resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[i]->AnySize));

			(*output)->__ptr[(*output)->__size-1].file->__size=resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[i]->AnySize;
			memmove((*output)->__ptr[(*output)->__size-1].file->__ptr, resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[i]->AnyBuf, resp_dtg_set->bmdDatagramSetTable[0]->pkiMetaData[i]->AnySize);
		}

	}

	if ((*output)->__size==0)
	{
		SOAP_FOK_LOGOUT(BMDSOAP_SERVER_NO_SELECTED_PKI_METADATA);
	}

	/************/
	/* porzadki */
	/************/
	bmd_logout(&bi, deserialisation_max_memory);
	bmd_info_destroy(&bi);
	bmd_end();

	bmd_datagramset_free(&dtg_set);
	bmd_datagramset_free(&resp_dtg_set);

	PRINT_SOAP("Status:\t\trequest served correctly\n");
	return BMD_OK;
}


/*****************************************************************************************/
/* weryfkikacja dokumentu przez plugin DVCS i zwrócenie statusu odpowiedzi uzytkownikowi */
/*****************************************************************************************/
int bmd230__verifyInvoiceWithDetails(	struct soap *soap,
					char *accountId,
					char *invoiceId,
					char *userIdentifier,
					int forceVerification,
					struct xsd__base64Binary *cert,
					char *roleName,
					char *groupName,
					char *userClassId,
					struct bmd230__fileInfo **output)
{
bmdDatagramSet_t *dtg_set			= NULL;
bmdDatagramSet_t *resp_dtg_set			= NULL;
bmd_info_t *bi					= NULL;
GenBuf_t *dvcs_resp				= NULL;
char *search_oid				= NULL;
char *search_oid2				= NULL;
char *search_oid3				= NULL;
char *verifyInvoiceWithDetails_select_type	= NULL;
long status					= 0;
long max_datagram_in_set_transmission		= 0;
long deserialisation_max_memory			= 0;
long twl_select_operator			= 0;

char *derUserCert=NULL;
long derUserCertLen=0;
long retVal=0;
char* paramCert=NULL;
long paramCertLen=0;

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if(accountId==NULL)	{	SOAP_FOK(BMDSOAP_SERVER_EMPTY1_PARAM_ERROR);	}
	if(invoiceId==NULL)	{	SOAP_FOK(BMDSOAP_SERVER_EMPTY2_PARAM_ERROR);	}

	PRINT_SOAP("Request:\tget document signature's dvcs validity aknowledgment\n");
	PRINT_SOAP("accountId:\t%s\n",accountId);
	PRINT_SOAP("invoiceId:\t%s\n",invoiceId);
	PRINT_SOAP("userIdentifier:\t%s\n",userIdentifier);
	/******************************************/
	/*	wczytanie ustawien konfiguracyjnych	*/
	/******************************************/
	bmdconf_get_value(_GLOBAL_konfiguracja,"soap","search_oid1",&search_oid);
	bmdconf_get_value(_GLOBAL_konfiguracja,"soap","search_oid2",&search_oid2);
	bmdconf_get_value(_GLOBAL_konfiguracja,"soap","search_oid3",&search_oid3);
	bmdconf_get_value(_GLOBAL_konfiguracja,"functionalities_behaviour","verifyInvoiceWithDetails_select_type",&verifyInvoiceWithDetails_select_type);
	if(verifyInvoiceWithDetails_select_type == NULL)
	{
		asprintf(&verifyInvoiceWithDetails_select_type,"EQUAL");
		if(verifyInvoiceWithDetails_select_type == NULL)
		{
			SOAP_FOK(NO_MEMORY);
		}
	}

	if(strcmp(verifyInvoiceWithDetails_select_type, "EQUAL") == 0)
	{
		twl_select_operator = BMD_QUERY_EQUAL;
	}
	else if(strcmp(verifyInvoiceWithDetails_select_type, "EXACT") == 0)
	{
		twl_select_operator = BMD_QUERY_EXACT_ILIKE;
	}
	else if(strcmp(verifyInvoiceWithDetails_select_type, "LIKE") == 0)
	{
		twl_select_operator = BMD_QUERY_LIKE;
	}
	else if(strcmp(verifyInvoiceWithDetails_select_type, "ILIKE") == 0)
	{
		twl_select_operator = BMD_QUERY_ILIKE;
	}
	else
	{
		SOAP_FOK(BMDSOAP_SERVER_SEARCH_OPERATOR_INVALID);
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

	if (forceVerification==1)
	{
		/************************************/
		/*	stworzenie datagramu zadania	*/
		/************************************/
		SOAP_FOK(createDatagram(	userClassId,
						(char *[]){search_oid,search_oid2,search_oid3},
						(char *[]){accountId,invoiceId,userIdentifier},
						3,
						BMD_DATAGRAM_TYPE_VALIDATE_SIGNATURE,
						BMD_ONLY_GROUP_DOC,
						twl_select_operator,
						&dtg_set));

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
		/*	wyslanie zadania do archiwum	*/
		/************************************/
		SOAP_FOK_LOGOUT(bmd_send_request(bi, dtg_set, max_datagram_in_set_transmission, &resp_dtg_set, 1, deserialisation_max_memory));

		if (	(resp_dtg_set->bmdDatagramSetTable[0]->datagramStatus!=PLUGIN_DVCS_SIGNATURE_INVALID) &&
			(resp_dtg_set->bmdDatagramSetTable[0]->datagramStatus!=PLUGIN_DVCS_SIGNATURE_VERIFIED) &&
			(resp_dtg_set->bmdDatagramSetTable[0]->datagramStatus!=PLUGIN_DVCS_SIGNATURE_VERIFIED_CONDITIONALLY))
		{
			SOAP_FOK_LOGOUT(resp_dtg_set->bmdDatagramSetTable[0]->datagramStatus);
		}

		/************************/
		/*	ustawienie wyniku */
		/************************/
		(*output)=(struct bmd230__fileInfo *)soap_malloc(soap,sizeof(struct bmd230__fileInfo));
		memset(*output,0,sizeof(struct bmd230__fileInfo));

		status=bmd_datagram_get_metadata(PKI_METADATA, OID_PKI_METADATA_DVCS, resp_dtg_set->bmdDatagramSetTable[0], -1, &dvcs_resp);
		if (status<BMD_OK)
		{
			if (status==-5)
			{
				SOAP_FOK_LOGOUT(BMDSOAP_SERVER_NO_SELECTED_PKI_METADATA);
			}
			SOAP_FOK_LOGOUT(BMDSOAP_SERVER_GETPKI_METADATA_ERR);
		}

		(*output)->filename=(char *)soap_malloc(soap,strlen("DVCS")+2);
		memset((*output)->filename,0,strlen("DVCS")+1);
		memmove((*output)->filename,"DVCS",strlen("DVCS"));

		(*output)->file=(struct bmd230__myBinaryDataType *)soap_malloc(soap,sizeof(struct bmd230__myBinaryDataType));
		(*output)->file->__ptr=(unsigned char *)soap_malloc(soap,dvcs_resp->size+2);
		memset((*output)->file->__ptr,0,dvcs_resp->size+1);
		(*output)->file->__size=dvcs_resp->size;
		memcpy((*output)->file->__ptr,dvcs_resp->buf,dvcs_resp->size);
		(*output)->file->id = NULL;
		(*output)->file->type = NULL;
		(*output)->file->options = NULL;

	}
	else
	{
		/************************************/
		/*	stworzenie datagramu zadania	*/
		/************************************/
		SOAP_FOK(createDatagram(	userClassId,
						(char *[]){search_oid,search_oid2,search_oid3},
						(char *[]){accountId,invoiceId,userIdentifier},
						3,
						BMD_DATAGRAM_GET_METADATA,
						BMD_ONLY_GROUP_DOC,
						twl_select_operator,
						&dtg_set));

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
		/*	wyslanie zadania do archiwum	*/
		/************************************/
		SOAP_FOK_LOGOUT(bmd_send_request(bi, dtg_set, max_datagram_in_set_transmission, &resp_dtg_set, 1, deserialisation_max_memory));

		/************************/
		/*	ustawienie wyniku */
		/************************/
		(*output)=(struct bmd230__fileInfo *)soap_malloc(soap,sizeof(struct bmd230__fileInfo));
		memset(*output,0,sizeof(struct bmd230__fileInfo));

		status=bmd_datagram_get_metadata(PKI_METADATA, OID_PKI_METADATA_DVCS,resp_dtg_set->bmdDatagramSetTable[0], -1, &dvcs_resp);
		if (status<BMD_OK)
		{
			if (status==-5)
			{
				SOAP_FOK_LOGOUT(BMDSOAP_SERVER_NO_SELECTED_PKI_METADATA);
			}
			SOAP_FOK_LOGOUT(BMDSOAP_SERVER_GETPKI_METADATA_ERR);
		}

		(*output)->filename=(char *)soap_malloc(soap,strlen("DVCS")+2);
		memset((*output)->filename,0,strlen("DVCS")+1);
		memmove((*output)->filename,"DVCS",strlen("DVCS"));

		(*output)->file=(struct bmd230__myBinaryDataType *)soap_malloc(soap,sizeof(struct bmd230__myBinaryDataType));
		(*output)->file->__ptr=(unsigned char *)soap_malloc(soap,dvcs_resp->size+2);
		memset((*output)->file->__ptr,0,dvcs_resp->size+1);
		(*output)->file->__size=dvcs_resp->size;
		memcpy((*output)->file->__ptr,dvcs_resp->buf,dvcs_resp->size);
		(*output)->file->id = NULL;
		(*output)->file->type = NULL;
		(*output)->file->options = NULL;
	}

	/******************/
	/*	porzadki	*/
	/******************/
	bmd_logout(&bi, deserialisation_max_memory);
	bmd_info_destroy(&bi);
	bmd_end();

	bmd_datagramset_free(&dtg_set);
	bmd_datagramset_free(&resp_dtg_set);
	free(search_oid); search_oid=NULL;
	free(search_oid2); search_oid2=NULL;

	PRINT_SOAP("Status:\t\trequest served correctly\n");
	return BMD_OK;
}

/**********************************************/
/* zliczenie dokumentów spelniajacych warunki */
/**********************************************/
int bmd230__countInvoice(	struct soap *soap,
			char *accountId,
			char *invoiceId,
			char *userIdentifier,
			struct xsd__base64Binary *cert,
			char *roleName,
			char *groupName,
			char *userClassId,
			int *count)
{
bmdDatagramSet_t *dtg_set			= NULL;
bmdDatagramSet_t *resp_dtg_set			= NULL;
bmd_info_t *bi					= NULL;
char *search_oid				= NULL;
char *search_oid2				= NULL;
char *search_oid3				= NULL;
char *countInvoice_select_type			= NULL;
long status					= 0;
long max_datagram_in_set_transmission		= 0;
long deserialisation_max_memory			= 0;
long twl_select_operator			= 0;

char *derUserCert=NULL;
long derUserCertLen=0;
long retVal=0;
char* paramCert=NULL;
long paramCertLen=0;

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if(accountId==NULL)	{	SOAP_FOK(BMDSOAP_SERVER_EMPTY1_PARAM_ERROR);	}
	if(invoiceId==NULL)	{	SOAP_FOK(BMDSOAP_SERVER_EMPTY2_PARAM_ERROR);	}

	PRINT_SOAP("Request:\tget number of documents with specified conditions\n");
	PRINT_SOAP("accountId:\t%s\n",accountId);
	PRINT_SOAP("invoiceId:\t%s\n",invoiceId);
	PRINT_SOAP("userIdentifier:\t%s\n",userIdentifier);
	/******************************************/
	/*	wczytanie ustawien konfiguracyjnych	*/
	/******************************************/
	bmdconf_get_value(_GLOBAL_konfiguracja,"soap","search_oid1",&search_oid);
	bmdconf_get_value(_GLOBAL_konfiguracja,"soap","search_oid2",&search_oid2);
	bmdconf_get_value(_GLOBAL_konfiguracja,"soap","search_oid3",&search_oid3);
	bmdconf_get_value(_GLOBAL_konfiguracja,"functionalities_behaviour","countInvoice_select_type",&countInvoice_select_type);
	if(countInvoice_select_type == NULL)
	{
		asprintf(&countInvoice_select_type,"EQUAL");
		if(countInvoice_select_type == NULL)
		{
			SOAP_FOK(NO_MEMORY);
		}
	}

	if(strcmp(countInvoice_select_type, "EQUAL") == 0)
	{
		twl_select_operator = BMD_QUERY_EQUAL;
	}
	else if(strcmp(countInvoice_select_type, "EXACT") == 0)
	{
		twl_select_operator = BMD_QUERY_EXACT_ILIKE;
	}
	else if(strcmp(countInvoice_select_type, "LIKE") == 0)
	{
		twl_select_operator = BMD_QUERY_LIKE;
	}
	else if(strcmp(countInvoice_select_type, "ILIKE") == 0)
	{
		twl_select_operator = BMD_QUERY_ILIKE;
	}
	else
	{
		SOAP_FOK(BMDSOAP_SERVER_SEARCH_OPERATOR_INVALID);
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
	SOAP_FOK_LOGOUT(createDatagram(	userClassId,
					(char *[]){search_oid,search_oid2,search_oid3},
					(char *[]){accountId,invoiceId,userIdentifier},
					3,
					BMD_DATAGRAM_TYPE_COUNT_DOCS,
					BMD_ONLY_GROUP_DOC,
					twl_select_operator,
					&dtg_set));

	/******************************************/
	/*	wyszukanie plikow w bazie danych	*/
	/******************************************/
	status=sendRequestToBMD(	bi,
					dtg_set,
					max_datagram_in_set_transmission,
					&resp_dtg_set,
					1,
					deserialisation_max_memory);



	/************************/
	/*	ustawienie wyniku */
	/************************/
	if (status<BMD_OK)
	{
		if (status==BMD_ERR_NODATA)
		{
			(*count)=0;
		}
		else
		{
			SOAP_FOK_LOGOUT(status);
		}
	}
	else
	{
// 		(*count)=resp_dtg_set->bmdDatagramSetSize;
		(*count)=resp_dtg_set->bmdDatagramSetTable[0]->filesRemaining;
	}

	/******************/
	/*	porzadki	*/
	/******************/
	bmd_logout(&bi, deserialisation_max_memory);
	bmd_info_destroy(&bi);
	bmd_end();

	bmd_datagramset_free(&dtg_set);
	bmd_datagramset_free(&resp_dtg_set);
	free(search_oid); search_oid=NULL;
	free(search_oid2); search_oid2=NULL;

	PRINT_SOAP("Status:\t\trequest served correctly\n");
	return BMD_OK;
}


/******************************************************/
/*	pobieranie plikow w kawalkach - inicjalizacja	*/
/******************************************************/
int bmd230__bmdGetFileChunkStart(	struct soap *soap,
				struct xsd__base64Binary *cert,
				char *roleName,
				char *groupName,
				char *userClassId,
				long id,
				char **tmpdir)
{
long fd						= 0;
long i						= 0;
long count_prev					= 0;
long count_now					= 0;
long tmp_pid					= 0;
long lob_session_hash				= 0;
long file_lock_desc				= 0;
long max_datagram_in_set_transmission		= 0;
long deserialisation_max_memory			= 0;
char *filename_name				= 0;
long stream_timeout				= 0;
char tmp_file[]					= {"soap_get_chunk_"};
char file_lock[]				= {"soap_get_lock_"};
char *tmp_dir					= NULL;
char *filename					= NULL;
char *tmp_location				= NULL;
char *file_lock_name				= NULL;
bmd_info_t *bi					= NULL;
lob_request_info_t *li				= NULL;
bmdDatagram_t *dtg				= NULL;
bmdDatagramSet_t *dtg_set			= NULL;
bmdDatagramSet_t *resp_dtg_set			= NULL;
struct timeval now;
struct tm czas;

char *derUserCert=NULL;
long derUserCertLen=0;
long retVal=0;
char* paramCert=NULL;
long paramCertLen=0;

	PRINT_SOAP("Request:\tstart get chunk transmission\n");
	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if (id<0)					{	SOAP_FOK(BMDSOAP_SERVER_INVALID_FILE_ID);	}

	/******************************************/
	/*	wczytanie ustawien konfiguracyjnych	*/
	/******************************************/
	SOAP_FOK(bmdconf_get_value(_GLOBAL_konfiguracja,"soap","temp_dir",&tmp_dir));
	SOAP_FOK(bmdconf_get_value_int(_GLOBAL_konfiguracja,"soap","stream_timeout",&stream_timeout));

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

	/************************************/
	/*	odczytanie katalogu biezacego	*/
	/************************************/
	memset(_GLOBAL_current_dir, 0 ,256);
	if (getcwd(_GLOBAL_current_dir,255)==NULL)	{	SOAP_FOK(BMDSOAP_SERVER_GCWD_ERROR);	}

	/******************************************************/
	/*	wygenerowanie losowego hasha dla sesji lobowej	*/
	/******************************************************/
	gettimeofday(&now,NULL);
	localtime_r(&(now.tv_sec),&czas);
	srand(now.tv_usec+getpid()+now.tv_sec);
	lob_session_hash=(1+(long) (100.0*rand()/(RAND_MAX+1.0)))*1000+czas.tm_sec*100+getpid()*10+now.tv_usec;

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

	/************************************/
	/*	logowanie do serwera BMD	*/
	/************************************/
	SOAP_FOK(PrepareDataForBMDLoginAndRequest(&bi,
					(_GLOBAL_soap_wssecurity_enabled == 1  ? derUserCert : paramCert),
					(_GLOBAL_soap_wssecurity_enabled == 1  ? derUserCertLen : paramCertLen),
					roleName, groupName, deserialisation_max_memory));
	free(derUserCert); derUserCert=NULL;
	paramCert=NULL; paramCertLen=0;

	asprintf(&file_lock_name,"%s%s%li%li", tmp_dir, file_lock, (long)getpid(), lob_session_hash);
	if(file_lock_name == NULL)		{	 SOAP_FOK(NO_MEMORY);				}
	PRINT_DEBUG("SOAPSERVERDEBUG Lock file %s\n", file_lock_name);
	/*twg_file_lock_name = file_lock_name;*/

	file_lock_desc = open(file_lock_name, O_RDONLY | O_CREAT | O_EXCL, S_IWUSR | S_IRUSR);
	if(file_lock_desc < 0)		{	SOAP_FOK(BMDSOAP_SERVER_LOCK_FILE_ERROR);	}
	if(close(file_lock_desc) < 0)	{	SOAP_FOK(BMDSOAP_SERVER_LOCK_FILE_ERROR);	}
	file_lock_desc = 0;

	/************************************************************************************/
	/*	stworzenie podprocesu przeszukujacego plik tymczasowy w poszukiwaniu plikow	*/
	/************************************************************************************/
	tmp_pid=fork();
	if (tmp_pid==0)	/* dzieciak */
	{
		/************************************************************************************************************/
		/*	stworzenie katalogu tymczasowego dla pobranych danych z serwera i przejscie do katalogu tymczasowego	*/
		/************************************************************************************************************/
		if(signal(SIGUSR, kill_me_get) == SIG_ERR)
		{
			PRINT_ERROR("SOAPSERVERERR Error in signal setting - kill_me_get!\n");
			bmd_logout(&bi, deserialisation_max_memory);
			bmd_end();
			unlink(file_lock_name);
			/*twg_file_lock_name = NULL;*/

			exit(1);
		}

		if(signal(SIGTERM, kill_me_get) == SIG_ERR)
		{
			PRINT_ERROR("SOAPSERVERERR Error in signal setting - kill_me_get!\n");
			bmd_logout(&bi, deserialisation_max_memory);
			bmd_end();
			unlink(file_lock_name);
			/*twg_file_lock_name = NULL;*/

			exit(1);
		}


		asprintf(&tmp_location, "%s%s%li%li/", tmp_dir, tmp_file, (long)getpid(), lob_session_hash);
		if(tmp_location == NULL)
		{
			PRINT_ERROR("SOAPSERVERERR Error in allocating memory\n");
			bmd_logout(&bi, deserialisation_max_memory);
			bmd_end();
			unlink(file_lock_name);
			/*twg_file_lock_name = NULL;*/

			exit(1);
		}
		/*twg_tmp_location = tmp_location;*/

		umask(000);
		if( mkdir(tmp_location, 0777) < 0)
		{
			PRINT_ERROR("SOAPSERVERERR Error in creating temporary directory: %s\n", tmp_location);
			bmd_logout(&bi, deserialisation_max_memory);
			bmd_end();
			unlink(file_lock_name);
			/*twg_file_lock_name = NULL;*/

			exit(1);
		}
		PRINT_VDEBUG("SOAPSERVERVDEBUG %s directory created\n",tmp_location);

		/*Moge usunac plik blokady*/
		if(unlink(file_lock_name) < 0)
		{
			PRINT_ERROR("SOAPSERVERERR Error in deleting lock file: %s\n", file_lock_name);
			bmd_logout(&bi, deserialisation_max_memory);
			bmd_end();

			exit(1);
		}
		/*twg_file_lock_name = NULL;*/

		SOAP_FOK_LOGOUT_TMP_EXT_1(chdir(tmp_location));

		/************************************************************************************************/
		/*	przeszukiwanie katalogu w poszukiwaniu kolejnych czesci pliku do dodania do archiwum	*/
		/************************************************************************************************/
		SOAP_FOK_LOGOUT_TMP_EXT_1(bmd_datagram_create(BMD_DATAGRAM_GET_CGI_LO,&dtg));
		if (userClassId!=NULL)
		{
			SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(	OID_ACTION_METADATA_CERT_CHOSEN_CLASS_ID, userClassId, &dtg));
		}

		SOAP_FOK_LOGOUT_TMP_EXT_1(bmd_datagram_set_id(&id,&dtg));
		SOAP_FOK_LOGOUT_TMP_EXT_1(bmd_datagram_add_to_set(dtg,&dtg_set));
		SOAP_FOK_LOGOUT_TMP_EXT_1(bmd_lob_request_info_create(&li));
		SOAP_FOK_LOGOUT_TMP_EXT_1(bmd_lob_request_info_add_filename(tmp_location,&li,BMD_RECV_LOB,0));
		SOAP_FOK_LOGOUT_TMP_EXT_1(bmd_send_request_lo_in_parts(bi,dtg_set, max_datagram_in_set_transmission,&resp_dtg_set,li,NULL,0, lob_session_hash,0, deserialisation_max_memory));
		SOAP_FOK_LOGOUT_TMP_EXT_1(resp_dtg_set->bmdDatagramSetTable[0]->datagramStatus);

		asprintf(&filename_name,"%s_filename",resp_dtg_set->bmdDatagramSetTable[0]->protocolDataFilename->buf);
		if(filename_name == NULL)			{	BMD_FOK(NO_MEMORY);	}

		if ((fd=open(filename_name, O_CREAT))!=-1)	{	close(fd); 	}

		bmd_datagramset_free(&dtg_set);
		bmd_datagramset_free(&resp_dtg_set);
		bmd_lob_request_info_destroy(&li);
		bmd_logout(&bi, deserialisation_max_memory);
		bmd_end();

		/******************************************************************/
		/*	powrot do katalogu pierwotnego i usuniecie tymczasowego	*/
		/******************************************************************/
		chdir(_GLOBAL_current_dir);
		i=0;
		while(1)
		{
			count_now=count_files_in_dir(tmp_location);
			if (count_now<0)
			{
				break;
			}
			if (count_now!=count_prev)
			{
				i=0;
			}
			i++;
			sleep(1);
			if (i>=stream_timeout)
			{
				break;
			}
			count_prev=count_now;
		}
		if (i>=stream_timeout)
		{
			PRINT_ERROR("SOAPSERVERERR Error %s directory removed by timeout %li. %li files in directory.\n",
			tmp_location, i, count_now);
			remove_dir(tmp_location);
		}

		/******************/
		/*	porzadki	*/
		/******************/
		free(tmp_dir); tmp_dir=NULL;
		free(filename); filename=NULL;
		free(tmp_location); tmp_location=NULL;
		free(file_lock_name); file_lock_name = NULL;

		/*Zeby rozroznic proces obslugujacy strumieniowosc*/
		exit(1);
	}

	/*Musze miec pewnosc ze usunieto plik blokady i stworzono katalog tymczasowy*/
	i = 0;
	while( (file_lock_desc = open(file_lock_name, O_RDONLY, S_IWUSR | S_IRUSR)) >= 0)
	{
		close(file_lock_desc);
		file_lock_desc = 0;
		PRINT_VDEBUG("SOAPSERVERVDEBUG Lock file %s still exists. Waiting...\n", file_lock_name);
		sleep(1);
		i++;
		if (i>=stream_timeout)
		{
			unlink(file_lock_name);
			PRINT_ERROR("SOAPSERVERERR Error %s lock file removed by timeout %li\n", file_lock_name, i);
			/*twg_file_lock_name = NULL;*/
			SOAP_FOK(BMDSOAP_SERVER_LOCK_FILE_ERROR);
		}
	}

	/******************************************/
	/*	okreslenie tymczasowej nazwy pliku	*/
	/******************************************/
	asprintf(&filename,"%li%li", tmp_pid, lob_session_hash);
	if(filename == NULL)				{	BMD_FOK(NO_MEMORY);	}

	/************************************************/
	/*	ustawienie tymczasowej nazwy zwrotnej	*/
	/************************************************/
	*tmpdir = (char*)soap_malloc(soap, strlen(filename)+1);
	if(*tmpdir == NULL)				{	BMD_FOK(NO_MEMORY);	}
	memset(*tmpdir, 0, strlen(filename)+1);
	memcpy(*tmpdir, filename, strlen(filename));

	/******************/
	/*	porzadki	*/
	/******************/
	free(tmp_dir); tmp_dir=NULL;
	free(filename); filename=NULL;
	free(tmp_location); tmp_location=NULL;
	free(file_lock_name); file_lock_name = NULL;

	PRINT_SOAP("Status:\t\trequest served correctly\n");
	return BMD_OK;
}


/*****************************************/
/* iteracja transmisji pliku w kawalkach */
/*****************************************/
int bmd230__bmdGetFileChunkIter(	struct soap *soap,
				struct xsd__base64Binary *cert,
				char *roleName,
				char *groupName,
				char *userClassId,
				char *tmpdir,
				char *count,
				struct bmd230__fileInfo **output)
{
long j					= 0;
long file				= 0;
long files_in_dir			= 0;
long count_is			= 0;
long final_is			= 0;
long stream_timeout		= 0;
char *tmp_dir			= NULL;
char *tmp_location		= NULL;
char *final_filename		= NULL;
char *count_filename		= NULL;
char *file_to_pass		= NULL;
char *filename			= NULL;
DIR *ptr				= NULL;
struct dirent *dirp		= NULL;
GenBuf_t *gb			= NULL;
char *tmp_lock			= NULL;
char tmp_file[]			= {"soap_get_chunk_"};

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if (tmpdir==NULL)			{	SOAP_FOK(BMDSOAP_SERVER_EMPTY1_PARAM_ERROR);	}
	if (count==NULL)			{	SOAP_FOK(BMDSOAP_SERVER_EMPTY2_PARAM_ERROR);	}

	PRINT_SOAP("Request:\titer get chunk transmission %s\n",count);
	/******************************************/
	/*	wczytanie ustawien konfiguracyjnych	*/
	/******************************************/
	SOAP_FOK(bmdconf_get_value(_GLOBAL_konfiguracja,"soap","temp_dir",&tmp_dir));
	SOAP_FOK(bmdconf_get_value_int(_GLOBAL_konfiguracja,"soap","stream_timeout",&stream_timeout));

	/************************************************************/
	/*	okreslenie katalogu tymczasowego i szukanych plikow	*/
	/************************************************************/
	asprintf(&tmp_location, "%s%s%s/",tmp_dir, tmp_file, tmpdir);
	if(tmp_location == NULL)		{	BMD_FOK(NO_MEMORY);	}

	asprintf(&count_filename, "%s%s_%s",tmp_file, tmpdir, count);
	if(count_filename == NULL)		{	BMD_FOK(NO_MEMORY);	}

	asprintf(&final_filename, "%s%s_final",tmp_file, tmpdir);
	if(final_filename == NULL)		{	BMD_FOK(NO_MEMORY);	}

	PRINT_VDEBUG("SOAPSERVERVDEBUG tmp_location: %s\n",tmp_location);
	PRINT_VDEBUG("SOAPSERVERVDEBUG count_filename: %s\n", count_filename);
	PRINT_VDEBUG("SOAPSERVERVDEBUG final_filename: %s\n", final_filename);

	/************************************************/
	/*	odczytanie plikow z katalogu tymczasowego	*/
	/************************************************/
	j = 0;
	while(1)
	{
		ptr=opendir(tmp_location);
		if (ptr==NULL)
		{
			PRINT_ERROR("SOAPSERVERERR Error in opening temporary folder %s\n",tmp_location);
			SOAP_FOK_TMP(BMDSOAP_SERVER_TMP_DIR_OPEN_ERROR);
		}
		else
		{
			/************************************/
			/*	wczytanie plikow z katalogu	*/
			/************************************/
			while( (dirp = readdir(ptr))!=NULL )
			{
				if ((strcmp(dirp->d_name,"..")==0) || (strcmp(dirp->d_name,".")==0))
				{
					continue;
				}
				if (strcmp(dirp->d_name,count_filename)==0)
				{
					PRINT_VDEBUG("SOAPSERVERVDEBUG count_filename %s found\n", count_filename);
					count_is=1;
				}
				if (strcmp(dirp->d_name,final_filename)==0)
				{
					PRINT_VDEBUG("SOAPSERVERVDEBUG final_filename %s found\n", final_filename);
					final_is=1;
				}
				if (isSubstring(dirp->d_name,"filename")==0)
				{
					asprintf(&filename, "%s", dirp->d_name);
					if(filename == NULL)	{	BMD_FOK(NO_MEMORY);	}
				}
				files_in_dir++;
			}
			closedir(ptr);
		}

		if ((count_is==1) || (final_is==1))
		{
			break;
		}
		else
		{
			files_in_dir=0;
			count_is=0;
			final_is=0;
			j++;
			sleep(1);
		}

		if(j >= stream_timeout)
		{
			PRINT_ERROR("SOAPSERVERERR Error stream timeout %li for %s\n", j, tmp_location);
			SOAP_FOK_TMP(BMDSOAP_SERVER_STREAM_TIMEOUT);
		}
	}

	if (count_is==1)
	{

		asprintf(&file_to_pass, "%s/%s", tmp_location, count_filename);
		if(file_to_pass == NULL)	{	BMD_FOK(NO_MEMORY);	}
		count[0]='\0';
	}
	else if (final_is==1)
	{

		asprintf(&file_to_pass, "%s/%s", tmp_location, final_filename);
		if(file_to_pass == NULL)	{	BMD_FOK(NO_MEMORY);	}
		filename[strlen(filename)-strlen("_filename")]='\0';

		asprintf(&count, filename);
		if(count == NULL)		{	BMD_FOK(NO_MEMORY);	}

	}
	else
	{
		PRINT_ERROR("SOAPSERVERERR Error stream timeout %li for %s\n", j, tmp_location);
		SOAP_FOK_TMP(BMDSOAP_SERVER_GET_CHUNK_NOT_FOUND);
	}

	/************************************************/
	/*	wszytanie znalezionego pliku do genbufa	*/
	/************************************************/
	/*	czekam na zwolnienie locka	*/
	/************************************/
	asprintf(&tmp_lock, "%s.lock", file_to_pass);
	if(tmp_lock == NULL)			{	BMD_FOK(NO_MEMORY);	}
	j=0;

	while(1)
	{

		if ((file=open(tmp_lock, O_RDONLY))<0)
		{
			break;
		}

		close(file);
		sleep(1);
		j++;

		if (j>=stream_timeout)
		{
			PRINT_ERROR("SOAPSERVERERR Error stream timeout %li for %s\n", j, tmp_location);
			SOAP_FOK_TMP(BMDSOAP_SERVER_STREAM_TIMEOUT);
		}

	}

	BMD_FOK(bmd_load_binary_content(file_to_pass,&gb));

	/************************************/
	/*	usuniecie wczytanego pliku	*/
	/************************************/
	BMD_FOK(unlink(file_to_pass));

	/************************/
	/*	ustawienie wyniku	*/
	/************************/
	(*output)=(struct bmd230__fileInfo *)soap_malloc(soap,sizeof(struct bmd230__fileInfo));
	if(*output == NULL)					{	BMD_FOK(NO_MEMORY);	}
	memset((*output),0,sizeof(struct bmd230__fileInfo));

	/*	ustawienie nazwy pliku	*/
	(*output)->filename=(char *)soap_malloc(soap,strlen(count)+1);
	if( (*output)->filename == NULL)			{	BMD_FOK(NO_MEMORY);	}
	memset((*output)->filename, 0, strlen(count)+1);
	memmove((*output)->filename, count, strlen(count));

	/*	ustawienie pliku	*/
	(*output)->file=(struct bmd230__myBinaryDataType *)soap_malloc(soap,sizeof(struct bmd230__myBinaryDataType));
	if( (*output)->file == NULL)				{	BMD_FOK(NO_MEMORY);	}

	(*output)->file->__ptr=(unsigned char *)soap_malloc(soap,gb->size+1);
	if( (*output)->file->__ptr == NULL)			{	BMD_FOK(NO_MEMORY);	}

	memset((*output)->file->__ptr,0,gb->size+1);
	(*output)->file->__size=gb->size;
	memmove((*output)->file->__ptr,gb->buf,gb->size);
	(*output)->file->id = NULL;
	(*output)->file->type = NULL;
	(*output)->file->options = NULL;

	if ((final_is==1) && (count_is!=1))
	{
		BMD_FOK(remove_dir(tmp_location));
	}


	/******************/
	/*	porzadki	*/
	/******************/

	free(tmp_dir); tmp_dir=NULL;
	free(tmp_location); tmp_location=NULL;
	free(final_filename); final_filename=NULL;
	free(count_filename); count_filename=NULL;
	free(file_to_pass); file_to_pass=NULL;
	free(filename); filename=NULL;
	free_gen_buf(&gb);

	PRINT_SOAP("Status:\t\trequest served correctly\n");
	return BMD_OK;
}

int bmd230__getFile(struct soap *soap, char *accountId, char *invoiceId, char *userIdentifier, char *getReason, struct xsd__base64Binary *cert, char *userRoleId, char *userGroupId, char *userClassId, enum trans_t type, struct bmd230__fileInfo **output)
{
long size				= 0;
long max_datagram_in_set_transmission	= 0;
long deserialisation_max_memory		= 0;
char *buf				= NULL;
bmd_info_t *bi				= NULL;
lob_request_info_t *li			= NULL;
bmdDatagram_t *dtg			= NULL;
bmdDatagramSet_t *dtg_set		= NULL;
bmdDatagramSet_t *resp_dtg_set		= NULL;
GenBuf_t *gb				= NULL;
char *path				= NULL;
FILE* fd 				= NULL;
char *search_oid		= NULL;

char *getFile_select_type	= NULL;
long select_operator		= 0;

char *derUserCert=NULL;
long derUserCertLen=0;
long retVal=0;
char* paramCert=NULL;
long paramCertLen=0;

	PRINT_SOAP("Request:\timport file with given metadata\n");
	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	
	
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

	/************************/
	/*	logowanie do bmd	*/
	/************************/
	SOAP_FOK(PrepareDataForBMDLoginAndRequest(&bi,
			(_GLOBAL_soap_wssecurity_enabled == 1  ? derUserCert : paramCert),
			(_GLOBAL_soap_wssecurity_enabled == 1  ? derUserCertLen : paramCertLen),
			userRoleId, userGroupId, deserialisation_max_memory));
	free(derUserCert); derUserCert=NULL;
	paramCert=NULL; paramCertLen=0;

	/******************************/
	/*	stworzenie zadania	*/
	/******************************/
	SOAP_FOK_LOGOUT(bmd_lob_request_info_create(&li));


	asprintf(&path,"soap_get_file_%li.tmp",(long)getpid());

	SOAP_FOK_LOGOUT(bmd_lob_request_info_add_filename(path, &li, BMD_RECV_LOB,1000000000));

	/******************************/
	/*	stworzenie datagramu	*/
	/******************************/
	SOAP_FOK_LOGOUT(bmd_datagram_create(BMD_DATAGRAM_GET_CGI_LO,&dtg));
	if (userClassId!=NULL)
	{
		SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(	OID_ACTION_METADATA_CERT_CHOSEN_CLASS_ID, userClassId, &dtg));
	}

	
	if(accountId != NULL)
	{
		bmdconf_get_value(_GLOBAL_konfiguracja,"soap","search_oid1",&search_oid);
		if(search_oid != NULL)
		{
			retVal = bmd_datagram_add_metadata_char(search_oid, accountId, &dtg);
			free(search_oid); search_oid = NULL;
			if(retVal < BMD_OK)
			{
				bmd_datagram_free(&dtg);
				SOAP_FOK_LOGOUT(retVal);
			}
		}
	}
	
	if(invoiceId != NULL)
	{
		bmdconf_get_value(_GLOBAL_konfiguracja,"soap","search_oid2",&search_oid);
		if(search_oid != NULL)
		{
			retVal = bmd_datagram_add_metadata_char(search_oid, invoiceId, &dtg);
			free(search_oid); search_oid = NULL;
			if(retVal < BMD_OK)
			{
				bmd_datagram_free(&dtg);
				SOAP_FOK_LOGOUT(retVal);
			}
		}
	}
	
	if(userIdentifier != NULL)
	{
		bmdconf_get_value(_GLOBAL_konfiguracja,"soap","search_oid3",&search_oid);
		if(search_oid != NULL)
		{
			retVal = bmd_datagram_add_metadata_char(search_oid, userIdentifier, &dtg);
			free(search_oid); search_oid = NULL;
			if(retVal < BMD_OK)
			{
				bmd_datagram_free(&dtg);
				SOAP_FOK_LOGOUT(retVal);
			}
		}
	}
	
	/******************************************/
	/*	ustawienie powodu pobrania faktury	*/
	/******************************************/
	if ((getReason!=NULL) && (strlen(getReason)>0))
	{
		dtg->protocolData=(GenBuf_t *)calloc(1, sizeof(GenBuf_t));
		if(dtg->protocolData == NULL)
		{
			bmd_datagram_free(&dtg);
			SOAP_FOK_LOGOUT(BMD_ERR_MEMORY);
		}
		dtg->protocolData->size=strlen(getReason);
		dtg->protocolData->buf=(char *)calloc(dtg->protocolData->size + 1, sizeof(char));
		if(dtg->protocolData->buf == NULL)
		{
			bmd_datagram_free(&dtg);
			SOAP_FOK_LOGOUT(BMD_ERR_MEMORY);
		}
		memset(dtg->protocolData->buf, 0, dtg->protocolData->size + 1);
		memcpy(dtg->protocolData->buf, getReason, dtg->protocolData->size);
	}
	
	//
	// okreslenie sposobu wyszukiwania
	//
	bmdconf_get_value(_GLOBAL_konfiguracja, "functionalities_behaviour", "getFile_select_type", &getFile_select_type);
	if(getFile_select_type == NULL)
	{
		getFile_select_type= strdup("EQUAL");
		if(getFile_select_type == NULL)
		{
			bmd_datagram_free(&dtg);
			SOAP_FOK_LOGOUT(NO_MEMORY);
		}
	}

	if(strcmp(getFile_select_type, "EQUAL") == 0)
	{
		free(getFile_select_type); getFile_select_type = NULL;
		select_operator = BMD_QUERY_EQUAL;
	}
	else if(strcmp(getFile_select_type, "EXACT") == 0)
	{
		free(getFile_select_type); getFile_select_type = NULL;
		select_operator = BMD_QUERY_EXACT_ILIKE;
	}
	else if(strcmp(getFile_select_type, "LIKE") == 0)
	{
		free(getFile_select_type); getFile_select_type = NULL;
		select_operator = BMD_QUERY_LIKE;
	}
	else if(strcmp(getFile_select_type, "ILIKE") == 0)
	{
		free(getFile_select_type); getFile_select_type = NULL;
		select_operator = BMD_QUERY_ILIKE;
	}
	else
	{
		free(getFile_select_type); getFile_select_type = NULL;
		bmd_datagram_free(&dtg);
		SOAP_FOK_LOGOUT(BMDSOAP_SERVER_SEARCH_OPERATOR_INVALID);
	}

	retVal = bmd_datagram_set_status(BMD_ONLY_GROUP_DOC, select_operator, &dtg);
	if(retVal < BMD_OK)
	{
		bmd_datagram_free(&dtg);
		SOAP_FOK_LOGOUT(retVal);
	}
	
	/************************************************/
	/*	dodanie datagramu do zestawu datagramow	*/
	/************************************************/
	retVal = bmd_datagram_add_to_set(dtg,&dtg_set);
	if(retVal < BMD_OK)
	{
		bmd_datagram_free(&dtg);
		SOAP_FOK_LOGOUT(retVal);
	}

	/************************************/
	/*	wyslanie zadania lobowego	*/
	/************************************/
	retVal = bmd_send_request_lo(bi, dtg_set, max_datagram_in_set_transmission, &resp_dtg_set, li, deserialisation_max_memory);
	bmd_datagramset_free(&dtg_set);
	SOAP_FOK_LOGOUT(retVal);
	
	bmd_logout(&bi, deserialisation_max_memory);
	bmd_info_destroy(&bi);
	SOAP_FOK(resp_dtg_set->bmdDatagramSetTable[0]->datagramStatus);

	/************************/
	/*	ustawienie wyniku	*/
	/************************/
	(*output)=(struct bmd230__fileInfo *)soap_malloc(soap,sizeof(struct bmd230__fileInfo));
	if(*output == NULL)
	{
		bmd_datagramset_free(&resp_dtg_set);
		SOAP_FOK(NO_MEMORY);
	}
	memset(*output,0,sizeof(struct bmd230__fileInfo));

	/*	ustawienie nazwy pliku	*/
	(*output)->filename=(char *)soap_malloc(soap,(resp_dtg_set->bmdDatagramSetTable[0]->protocolDataFilename->size+1) * sizeof(char));
	if((*output)->filename == NULL)
	{
		bmd_datagramset_free(&resp_dtg_set);
		SOAP_FOK(NO_MEMORY);
	}
	memset((*output)->filename, 0, (resp_dtg_set->bmdDatagramSetTable[0]->protocolDataFilename->size+1) * sizeof(char));
	memcpy((*output)->filename, resp_dtg_set->bmdDatagramSetTable[0]->protocolDataFilename->buf, resp_dtg_set->bmdDatagramSetTable[0]->protocolDataFilename->size);

	/*	ustawienie pliku	*/
	(*output)->file=(struct bmd230__myBinaryDataType *)soap_malloc(soap,sizeof(struct bmd230__myBinaryDataType));
	if((*output)->file == NULL)
	{
		bmd_datagramset_free(&resp_dtg_set);
		SOAP_FOK(NO_MEMORY);
	}
	memset((*output)->file, 0, sizeof(struct bmd230__myBinaryDataType));


	switch(type)
	{

	    case 0 : {
			SOAP_FOK(bmd_load_binary_content(path,&gb));
			SOAP_FOK(bmd_genbuf_get_buf(gb,&buf));
			SOAP_FOK(bmd_genbuf_get_size(gb,&size));
			(*output)->file->__size=size;
			(*output)->file->__ptr=(unsigned char *)soap_malloc(soap,((*output)->file->__size) * sizeof(unsigned char));
			if((*output)->file->__ptr == NULL)
			{
				bmd_datagramset_free(&resp_dtg_set);
				(*output)->file->__size=0;
				free_gen_buf(&gb);
				SOAP_FOK(NO_MEMORY);
			}
			memset((*output)->file->__ptr,0,((*output)->file->__size) * sizeof(unsigned char));
			memcpy((*output)->file->__ptr, buf, ((*output)->file->__size) * sizeof(unsigned char));
			buf=NULL; size=0;
			free_gen_buf(&gb);
			(*output)->file->type = NULL;
			(*output)->file->options = NULL;
						
			break;
		     }

	    case 1 : {  /* Transmisja strumieniowa DIME */
			soap_set_omode(soap, SOAP_IO_CHUNK);
			fd = fopen(path,"r");
			if(fd == NULL)
			{
				bmd_datagramset_free(&resp_dtg_set);
				SOAP_FOK(BMD_ERR_OPEN_FILE);
			}
			(*output)->file->__ptr=(unsigned char *)fd;
			(*output)->file->__size=0;
			(*output)->file->type = soap_malloc(soap, 1*sizeof(char));
			memset((*output)->file->type, 0 , 1*sizeof(char));
			(*output)->file->options = soap_dime_option(soap,0, (*output)->filename );

			break;
		     }

	}

	/******************/
	/*	porzadki  */
	/******************/
	

	unlink(path);
	free(path); path=NULL;
	bmd_datagramset_free(&resp_dtg_set);
	bmd_lob_request_info_destroy(&li);

	PRINT_SOAP("Status:\t\tRequest served correctly\n");
	return SOAP_OK;
}


