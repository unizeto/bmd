#include <bmd/libbmdsoap/libbmdsoap.h>
#include <bmd/libbmdglobals/libbmdglobals.h>

/********************************************************/
/*      Obslugha sygnalu SIGUSR zabijajacego proces pobierania strumieniowgo     */
/********************************************************/
void kill_me_insert(int status)
{
	PRINT_INFO("SOAPSERVERINF Killing insert process.\n");
	/*if(_GLOBAL_current_dir[0] != 0)
	{
		PRINT_VDEBUG("SOAPSERVERVDEBUG current_dir: %s\n", _GLOBAL_current_dir);
		PRINT_VDEBUG("SOAPSERVERVDEBUG tmp_location: %s\n", twg_tmp_location);
		if(chdir(_GLOBAL_current_dir) == 0 && twg_tmp_location != NULL)
		{
			remove_dir(twg_tmp_location); twg_tmp_location = NULL;
		}
	}
	if(twg_file_lock_name != NULL)
	{
		PRINT_VDEBUG("SOAPSERVERVDEBUG file_lock_name: %s\n", twg_file_lock_name);
		unlink(twg_file_lock_name); twg_file_lock_name = NULL;
	}*/
	exit(1);
}

/********************************************************/
/*      Obslugha sygnalu SIGUSR zabijajacego proces obslugi katalogu transakcji */
/********************************************************/
void kill_me_transaction(int status)
{
/*long twl_status;*/

	PRINT_INFO("SOAPSERVERINF Killing transaction handling process.\n");
	/*
	if(_GLOBAL_twg_transaction_dir_name != NULL)
	{
		PRINT_VDEBUG("SOAPSERVERVDEBUG twg_transaction_dir_name: %s\n", _GLOBAL_twg_transaction_dir_name);
		status = remove_dir(_GLOBAL_twg_transaction_dir_name); _GLOBAL_twg_transaction_dir_name = NULL;
		PRINT_VDEBUG("SOAPSERVERVDEBUG Removing transaction directory status: %li", twl_status)
	}
	else
	{
		PRINT_VDEBUG("SOAPSERVERVDEBUG No transaction directory name.\n");
	}
	*/
	exit(1);
}



/*Funkcja tworzy katalog transackji.
twf_transaction_dir_name - nazwa katalogu wraz z pelna sciezka.
twf_error_if_failure - jezeli 0 i katalog juz istnieje lub nie udalo sie go utworzyc nic sie nie stanie.
Jezeli 1 i katalog istnieje lub nie udalo sie go utworzyc zostanie zwrocony blad.*/

long twfun_make_transaction_directory(char *twf_transaction_dir_name, long twf_error_if_failure)
{
long i					= 0;
long tmp_pid				= 0;
long idle_transaction_timeout		= 0;
long twl_file_count			= 0;
long twl_last_file_count		= 0;
DIR *twl_transaction_dir_descriptor	= NULL;
struct dirent *dirp			= NULL;

	PRINT_INFO("SOAPSERVERINF twfun_make_transaction_directory.\n");

	if(twf_transaction_dir_name == NULL)				{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(twf_error_if_failure < 0 || twf_error_if_failure > 1)	{	BMD_FOK(BMD_ERR_PARAM2);	}

	if(bmdconf_get_value_int(_GLOBAL_konfiguracja,"soap","idle_transaction_timeout",&idle_transaction_timeout) < BMD_OK)
	{
		idle_transaction_timeout = 0;
	}

	if(signal(SIGUSR, kill_me_transaction) == SIG_ERR)
	{
		PRINT_ERROR("SOAPSERVERERR Error in signal setting - SIGUSR!\n");
		return BMDSOAP_SERVER_INTERNAL_ERROR;
	}


	twl_transaction_dir_descriptor = opendir(twf_transaction_dir_name);
	if(twl_transaction_dir_descriptor != NULL)
	{/*if(twf_transaction_dir_descriptor != NULL)*/
		if(twf_error_if_failure == 1)
		{
			PRINT_ERROR("SOAPSERVERERR Transaction marker %s already exists!\n", twf_transaction_dir_name);
			closedir(twl_transaction_dir_descriptor);
			return BMDSOAP_SERVER_TRANSACTION_ALREADY_STARTED;
		}
		else
		{
			return BMD_OK;
		}
	}/*\if(twl_transaction_dir_descriptor != NULL)*/

	umask(000);

	/*Jakby ktos zabil serwer wczesniej to to jest informacja zeby usunac katalog. Najwyzej nic nie usunie jak
	go nie bedzie.*/
	_GLOBAL_twg_transaction_dir_name = twf_transaction_dir_name;
	if(mkdir(twf_transaction_dir_name, 0777) < 0)
	{/*if(mkdir(transaction_dir_name, 0777) < 0)*/
		_GLOBAL_twg_transaction_dir_name = NULL;
		if(twf_error_if_failure == 1)
		{
			PRINT_ERROR("SOAPSERVERERR Error in creating transaction marker %s!\n", twf_transaction_dir_name);
			return BMDSOAP_SERVER_CANNOT_CREATE_TRANSACTION_MARKER;
		}
		else
		{
			return BMD_OK;
		}
	}/*\if(mkdir(transaction_dir_name, 0777) < 0)*/
	PRINT_VDEBUG("SOAPSERVERVDEBUG Created transaction directory %s\n", twf_transaction_dir_name);
	PRINT_VDEBUG("twg_transaction_dir_name %s\n", _GLOBAL_twg_transaction_dir_name);

	/*Jezeli ustawiono timeout na transakcje to musi byc ona sprawdzana przez proces obslugi*/
	tmp_pid = fork();
	if(tmp_pid == 0)
	{/*if(tmp_pid == 0)*/
		/*dzieciak*/
		if(idle_transaction_timeout > 0)
		{/*if(idle_transaction_timeout > 0)*/
			PRINT_VDEBUG("SOAPSERVERVDEBUG Idle transaction timeout value %li\n", idle_transaction_timeout);
			while(1)
			{/*while(1)*/
				twl_transaction_dir_descriptor = opendir(twf_transaction_dir_name);
				twl_file_count = 0;

				if(twl_transaction_dir_descriptor == NULL)
				{
					exit(1);
				}

				while( (dirp = readdir(twl_transaction_dir_descriptor))!=NULL )
				{
					if ((strcmp(dirp->d_name,"..")==0) || (strcmp(dirp->d_name,".")==0))
					{
						continue;
					}
					twl_file_count++;
				}
				closedir(twl_transaction_dir_descriptor);

				if(twl_last_file_count != twl_file_count)
				{
					PRINT_VDEBUG("SOAPSERVERVDEBUG Idle transaction timeout reseted. "
					"Last file count %li, file count %li\n", twl_last_file_count, twl_file_count);

					twl_last_file_count = twl_file_count;
					i = 0;
					sleep(1);
				}
				else
				{
					if(idle_transaction_timeout > i)
					{/*if(idle_transaction_timeout > i)*/
						i++;
						PRINT_VDEBUG("SOAPSERVERVDEBUG Idle transaction time %li.\n", i);
						sleep(1);
					}/*\if(idle_transaction_timeout > i)*/
					else
					{
						/*Timeout*/
						remove_dir(twf_transaction_dir_name);
						PRINT_VDEBUG("SOAPSERVERVDEBUG Idle transaction timeout occured with value "
						"%li\n", idle_transaction_timeout);
						PRINT_ERROR("SOAPSERVERERR Idle transaction timeout %i\n",
						BMDSOAP_SERVER_IDLE_TRANSACTION_TIMEOUT);
						exit(1);
					}
				}
			}/*\while(1)*/
		}/*\if(idle_transaction_timeout > 0)*/
		else
		{/*else(idle_transaction_timeout > 0)*/
			PRINT_VDEBUG("SOAPSERVERVDEBUG Idle transaction timeout turned off.\n");
			/*Trzeba utrzymywac ten proces, gdyz cos musi usunac katalog transakcji w razie wylaczenia
			serwera. Obsluga sygnalu podpieta jest pod ten proces.*/
			/*while(1)
			{
				twl_transaction_dir_descriptor = opendir(twf_transaction_dir_name);

				if(twl_transaction_dir_descriptor == NULL)
				{
					exit(1);
				}
				else
				{
					closedir(twl_transaction_dir_descriptor);
					sleep(1);
				}
			}*/
		}/*\else(idle_transaction_timeout > 0)*/
		exit(1);
	}/*\if(tmp_pid == 0)*/

	PRINT_VDEBUG("SOAPSERVERVDEBUG Created transaction directory %s\n", twf_transaction_dir_name);

	return BMD_OK;
}

long twfun_check_transaction_directory(	char *twf_temporary_dir, char *twf_transaction_id,
					long twf_start_transaction_required, char **twf_transaction_dir_name)
{
DIR *twl_transaction_dir_descriptor	= NULL;
char *twl_transaction_dir_name		= NULL;

	PRINT_INFO("SOAPSERVERINF twfun_check_transaction_directory.\n");

	if(twf_temporary_dir == NULL)					{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(twf_transaction_id == NULL)					{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(twf_start_transaction_required < 0 ||
	twf_start_transaction_required > 1)				{	BMD_FOK(BMD_ERR_PARAM3);	}
	if(twf_transaction_dir_name == NULL)				{	BMD_FOK(BMD_ERR_PARAM4);	}
	if(*twf_transaction_dir_name != NULL)				{	BMD_FOK(BMD_ERR_PARAM4);	}


	asprintf(&twl_transaction_dir_name, "%ssoap_transaction_id_%s", twf_temporary_dir, twf_transaction_id);
	if(twl_transaction_dir_name == NULL)        			{	BMD_FOK(NO_MEMORY);		};
	twl_transaction_dir_descriptor = opendir(twl_transaction_dir_name);

	if(twl_transaction_dir_descriptor == NULL)
	{
		if(twf_start_transaction_required == 1)
		{
			PRINT_ERROR("SOAPSERVERERR Error in opening transaction marker %s\n", twl_transaction_dir_name);
			free(twl_transaction_dir_name); twl_transaction_dir_name = NULL;
			BMD_FOK(BMDSOAP_SERVER_NO_TRANSACTION_STARTED);
		}
		else
		{
			BMD_FOK(twfun_make_transaction_directory(twl_transaction_dir_name, 0));
		}
	}
	else
	{
		PRINT_VDEBUG("SOAPSERVERVDEBUG Transaction directory %s already exists.\n", twl_transaction_dir_name);
		closedir(twl_transaction_dir_descriptor); twl_transaction_dir_descriptor = NULL;
		_GLOBAL_twg_transaction_dir_name = twl_transaction_dir_name;
	}

	*twf_transaction_dir_name = twl_transaction_dir_name;
	twl_transaction_dir_name = NULL;

	return BMD_OK;
}


/*Wstaw marker wysylania pliku w katalogu transakcji.*/
long twfun_mark_start_of_insert( char *twf_transaction_dir_name, char *twf_transaction_id)
{
long twl_marker_file_descriptor			= 0;
char *twl_marker_file_name			= NULL;
char twl_directory[256];

	PRINT_INFO("SOAPSERVERINF twfun_mark_start_of_insert.\n");

	if(twf_transaction_dir_name == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);		}
	if(twf_transaction_id == NULL)			{	BMD_FOK(BMD_ERR_PARAM2);		}

	twl_directory[255] = '\0';

	asprintf(&twl_marker_file_name, "insert_%li.lock", (long)getpid());
	if(twl_marker_file_name == NULL)		{	BMD_FOK(NO_MEMORY);			}

	/*Pobranie aktualnego katalogu*/
	if(getcwd(twl_directory, 255) == NULL)		{	BMD_FOK(BMDSOAP_SERVER_GCWD_ERROR);	}

	/*wejscie do katalogu transakcji*/
	if(chdir(twf_transaction_dir_name) == -1)	{	BMD_FOK(BMDSOAP_SERVER_CHDIR_ERROR);	}

	twl_marker_file_descriptor = open(twl_marker_file_name, O_RDONLY | O_CREAT | O_EXCL, S_IWUSR | S_IRUSR);

	if(twl_marker_file_descriptor < 0)
	{
		chdir(twl_directory);
		PRINT_ERROR("SOAPSERVERERR Error in creating transaction marker %s\n", twl_marker_file_name);
		free(twl_marker_file_name); twl_marker_file_name = NULL;
		BMD_FOK(BMDSOAP_SERVER_ERR_CREATING_TRANSMISSION_MARKER);
	}
	free(twl_marker_file_name); twl_marker_file_name = NULL;
	close(twl_marker_file_descriptor);
	twl_marker_file_descriptor = 0;

	/*Powrot do poprzedniego katalogu*/
	if(chdir(twl_directory) == -1)			{	BMD_FOK(BMDSOAP_SERVER_CHDIR_ERROR);	}

	return BMD_OK;
}


/*
twf_transaction_dir_name - nazwa katalogu transakcji
twf_transaction_id - identyfikator transakcji
twf_pid - inna wartosc pid niz procesu, w przypadku wartosci mniejszej od zera zostanie wziety aktualny pid procesu.
	Argument ten jest przydatny w odznaczaniu transmisji pliku zaczetej przez inny proces.*/
long twfun_unmark_start_of_insert( char *twf_transaction_dir_name, char *twf_transaction_id, long twf_pid)
{
long twl_marker_file_descriptor			= 0;
char *twl_marker_file_name			= NULL;
char twl_directory[256];

	PRINT_INFO("SOAPSERVERINF twfun_unmark_start_of_insert.\n");

	if(twf_transaction_dir_name == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);		}
	if(twf_transaction_id == NULL)			{	BMD_FOK(BMD_ERR_PARAM2);		}

	twl_directory[255] = '\0';

	/*Pobranie aktualnego katalogu*/
	if(getcwd(twl_directory, 255) == NULL)          {	BMD_FOK(BMDSOAP_SERVER_GCWD_ERROR);	}

	/*wejscie do katalogu transakcji*/
	if(chdir(twf_transaction_dir_name) == -1)	{	BMD_FOK(BMDSOAP_SERVER_CHDIR_ERROR);	}

	if(twf_pid < 0)
	{
		asprintf(&twl_marker_file_name, "transmitted_%li.lock", (long)getpid());
		if(twl_marker_file_name == NULL)		{	BMD_FOK(NO_MEMORY);			}
		twl_marker_file_descriptor = open(twl_marker_file_name, O_RDONLY | O_CREAT, S_IWUSR | S_IRUSR);
		if(twl_marker_file_descriptor < 0)
		{
			chdir(twl_directory);
			PRINT_ERROR("SOAPSERVERERR Error in creating transmitted marker %s\n", twl_marker_file_name);
			free(twl_marker_file_name); twl_marker_file_name = NULL;
			BMD_FOK(BMDSOAP_SERVER_ERR_CREATING_TRANSMITTED_MARKER);
		}
		free(twl_marker_file_name); twl_marker_file_name = NULL;
		close(twl_marker_file_descriptor);
		twl_marker_file_descriptor = 0;
	}

	if(twf_pid < 0)
	{
		asprintf(&twl_marker_file_name, "insert_%li.lock", (long)getpid());
	}
	else
	{
		asprintf(&twl_marker_file_name, "insert_%li.lock", twf_pid);
	}

	if(twl_marker_file_name == NULL)                {	BMD_FOK(NO_MEMORY);			}
	twl_marker_file_descriptor = unlink(twl_marker_file_name);
	if(twl_marker_file_descriptor < 0)
	{
		chdir(twl_directory);
		PRINT_ERROR("SOAPSERVERERR Error in removing transmission marker %s\n", twl_marker_file_name);
		free(twl_marker_file_name); twl_marker_file_name = NULL;
		BMD_FOK(BMDSOAP_SERVER_ERR_REMOVING_TRANSMISSION_MARKER);
	}
	free(twl_marker_file_name); twl_marker_file_name = NULL;

	/*Powrot do poprzedniego katalogu*/
	if(chdir(twl_directory) == -1)			{	BMD_FOK(BMDSOAP_SERVER_CHDIR_ERROR);	}

	return BMD_OK;

}

/*Funkcja zaznacza wystapienie bledu w trakcie transakcji. Nie podanie nazwy katalogu transakcji oznacza od razu
wyjscie prawidlowe funkcji.*/
long twfun_mark_error_in_transaction(char *twf_transaction_dir_name, long twf_error_code)
{
long twl_marker_file_descriptor		= 0;
char *twl_marker_file_name		= NULL;
char twl_directory[256];

	PRINT_INFO("SOAPSERVERINF twfun_mark_error_in_transaction.\n");

	if(twf_transaction_dir_name == NULL || twf_error_code >= 0)
	{
		PRINT_VDEBUG("SOAPSERVERVDEBUG Nothing to do pointer:%p, code %li.\n",
		twf_transaction_dir_name, twf_error_code);
		return BMD_OK;
	}

	twl_directory[255] = '\0';

	PRINT_VDEBUG("SOAPSERVERVDEBUG Transaction directory name: %s\n", twf_transaction_dir_name);

	/*Pobranie aktualnego katalogu*/
	if(getcwd(twl_directory, 255) == NULL)		{	BMD_FOK(BMDSOAP_SERVER_GCWD_ERROR);	}

	/*wejscie do katalogu transakcji*/
	if(chdir(twf_transaction_dir_name) == -1)	{	BMD_FOK(BMDSOAP_SERVER_CHDIR_ERROR);	}

	asprintf(&twl_marker_file_name, "error_%li_%li.lock",  -1 * twf_error_code, (long)getpid());
	if(twl_marker_file_name == NULL)		{	BMD_FOK(NO_MEMORY);			}
	twl_marker_file_descriptor = open(twl_marker_file_name, O_RDONLY | O_CREAT, S_IWUSR | S_IRUSR);
	if(twl_marker_file_descriptor < 0)
	{
		chdir(twl_directory);
		PRINT_ERROR("SOAPSERVERERR Error in creating transmitted marker %s\n", twl_marker_file_name);
		free(twl_marker_file_name); twl_marker_file_name = NULL;
		BMD_FOK(BMDSOAP_SERVER_ERR_CREATING_TRANSMITTED_MARKER);
	}
	close(twl_marker_file_descriptor);
	twl_marker_file_descriptor = 0;

	/*Powrot do poprzedniego katalogu*/
	if(chdir(twl_directory) == -1)			{	BMD_FOK(BMDSOAP_SERVER_CHDIR_ERROR);	}

	return BMD_OK;
}


/**********************************************/
/* obsluga zadania wyslania pliku do archiwum */
/**********************************************/
int bmd230__bmdInsertFile(	struct soap *soap,
			struct xsd__base64Binary *cert,
			char *roleName,
			char *groupName,
			char *userClassId,
			struct bmd230__mtdsValues *mtds,
			struct bmd230__fileInfo *input,
			char *transactionId,
			enum trans_t type,
			long *id)
{
long i					= 0;
long status				= 0;
long max_datagram_in_set_transmission	= 0;
long deserialisation_max_memory		= 0;
long max_metadata_value_length		= 0;
long start_transaction_required		= 0;
bmd_info_t *bi				= NULL;
bmdDatagram_t *dtg			= NULL;
bmdDatagramSet_t *dtg_set		= NULL;
bmdDatagramSet_t *resp_dtg_set		= NULL;
lob_request_info_t *li			= NULL;
char *tmp_dir				= NULL;
char *transaction_dir_name		= NULL;
char *filename				= NULL;
char *dirname				= NULL;

GenBuf_t tmp_gb;
GenBuf_t tmp_gb1;

char *derUserCert=NULL;
long derUserCertLen=0;
long retVal=0;
char* paramCert=NULL;
long paramCertLen=0;



	/************************/
	/* walidacja parametrow */
	/************************/
	if (input==NULL)			{	SOAP_FOK(BMDSOAP_SERVER_INPUT_FILE_UNDEF);	}
	if (input->file==NULL)			{	SOAP_FOK(BMDSOAP_SERVER_INPUT_FILE_UNDEF);	}
	if (input->filename==NULL)		{	SOAP_FOK(BMDSOAP_SERVER_INPUT_FILE_UNDEF);	}
	if (input->file->__ptr==NULL)		{	SOAP_FOK(BMDSOAP_SERVER_INPUT_FILE_UNDEF);	}
	if (input->file->__size<=0)		{	SOAP_FOK(BMDSOAP_SERVER_INPUT_FILE_UNDEF);	}

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

	switch(type)
	{
		case nostream :
		{
			/******************************/
			/*	walidacja parametrow      */
			/******************************/
			if ((input==NULL) || (input->file==NULL) || (input->filename==NULL) || (input->file->__ptr==NULL) || (input->file->__size<=0))
			{
				SOAP_FOK(BMDSOAP_SERVER_INPUT_FILE_UNDEF);
			}

			/************************/
			/*	plik tymczasowy	     */
			/************************/
			asprintf(&filename,"tmp%li.tmp", (long)getpid());
			tmp_gb.buf=(char*)input->file->__ptr;
			tmp_gb.size=input->file->__size;
			status=bmd_save_buf(&tmp_gb,filename);

			break;
		}
		case stream :
		{
			/******************************/
			/*	walidacja parametrow      */
			/******************************/
			if ((input==NULL) || (input->file==NULL) || (input->filename==NULL))
			{
				SOAP_FOK(BMDSOAP_SERVER_INPUT_FILE_UNDEF);
			}

			asprintf(&filename,"tmp_%li/%s", (long)getpid(), input->filename );
			asprintf(&dirname,"tmp_%li", (long)getpid() );

			break;
		}

		default :
		{
			SOAP_FOK(BMDSOAP_SERVER_INVALID_TRANSMISSION_TYPE);
		}

	}

	if(id != NULL)
	{
		(*id)=-1;
	}

	PRINT_SOAP("Request:\tinsert document to archive\n");

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
	/*	logowanie do bmd*/
	/************************/
	SOAP_FOK(PrepareDataForBMDLoginAndRequest(&bi,
				(_GLOBAL_soap_wssecurity_enabled == 1  ? derUserCert : paramCert),
				(_GLOBAL_soap_wssecurity_enabled == 1  ? derUserCertLen : paramCertLen),
				roleName, groupName, deserialisation_max_memory));
	free(derUserCert); derUserCert=NULL;
	paramCert=NULL; paramCertLen=0;
	if (status<BMD_OK)
	{
		unlink(filename);
		remove(dirname);
		free(filename); filename=NULL;
		free(dirname); dirname=NULL;
		twfun_mark_error_in_transaction(transaction_dir_name, status);
		PRINT_ERROR("SOAPSERVERERR %s\n",GetErrorMsg(status));
        	return soap_receiver_fault(soap,"Archive server error",GetErrorMsg(status));
	}

	status=bmd_lob_request_info_create(&li);
	if(status<BMD_OK)
	{
		bmd_logout(&bi, deserialisation_max_memory);
		bmd_end();
		unlink(filename);
		remove(dirname);
		free(filename); filename=NULL;
		free(dirname); dirname=NULL;
		twfun_mark_error_in_transaction(transaction_dir_name, BMDSOAP_SERVER_LOB_REQUEST_ERROR);
		PRINT_ERROR("SOAPSERVERERR %s\n",GetErrorMsg(BMDSOAP_SERVER_LOB_REQUEST_ERROR));
		return soap_receiver_fault(soap,"Soap server error",GetErrorMsg(BMDSOAP_SERVER_LOB_REQUEST_ERROR));
	}

	status=bmd_lob_request_info_add_filename(filename,&li,BMD_SEND_LOB,0);
	if(status<BMD_OK)
	{
		bmd_logout(&bi, deserialisation_max_memory);
		bmd_end();
		unlink(filename);
		remove(dirname);
		free(filename); filename=NULL;
		free(dirname); dirname=NULL;
		twfun_mark_error_in_transaction(transaction_dir_name, status);
		PRINT_ERROR("SOAPSERVERERR %s\n",GetErrorMsg(BMDSOAP_SERVER_ADD_FILE_LOB_REQUEST_ERROR));
		return soap_receiver_fault(soap,"Soap server error",GetErrorMsg(BMDSOAP_SERVER_ADD_FILE_LOB_REQUEST_ERROR));
	}

	status=bmd_datagram_create(BMD_DATAGRAM_TYPE_INSERT_LO,&dtg);
	if(status<BMD_OK)
	{
		bmd_logout(&bi, deserialisation_max_memory);
		bmd_end();
		unlink(filename);
		remove(dirname);
		free(filename); filename=NULL;
		free(dirname); dirname=NULL;
		twfun_mark_error_in_transaction(transaction_dir_name, status);
		PRINT_ERROR("SOAPSERVERERR %s\n",GetErrorMsg(status));
		return soap_receiver_fault(soap,"Archive server error",GetErrorMsg(status));
	}

	if (userClassId!=NULL)
	{
		SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(	OID_ACTION_METADATA_CERT_CHOSEN_CLASS_ID, userClassId, &dtg));
	}

// 	status=AddCertLoginAsToDatagram(	cert,	&dtg);
// 	if(status<BMD_OK)
// 	{
// 		bmd_logout(&bi, deserialisation_max_memory);
// 		bmd_end();
// 		unlink(filename);
// 		remove(dirname);
// 		free(filename); filename=NULL;
// 		free(dirname); dirname=NULL;
// 		twfun_mark_error_in_transaction(transaction_dir_name, status);
// 		PRINT_ERROR("SOAPSERVERERR %s\n",GetErrorMsg(status));
// 		return soap_receiver_fault(soap,"Archive server error",GetErrorMsg(status));
// 	}

	status=bmd_datagram_set_filename(input->filename, &dtg);
	if(status<BMD_OK)
	{
		bmd_logout(&bi, deserialisation_max_memory);
		bmd_end();
		unlink(filename);
		remove(dirname);
		free(filename); filename=NULL;
		free(dirname); dirname=NULL;
		twfun_mark_error_in_transaction(transaction_dir_name, status);
		PRINT_ERROR("SOAPSERVERERR %s\n",GetErrorMsg(status));
		return soap_receiver_fault(soap,"Archive server error",GetErrorMsg(status));
	}

	if ((mtds!=NULL) && (mtds->__ptr!=NULL) && (mtds->__size))
	{
		for (i=0; i<mtds->__size; i++)
		{
			if ((mtds->__ptr[i].mtdOid==NULL) || (mtds->__ptr[i].mtdValue==NULL))
			{
				continue;
			}
			tmp_gb.buf=(char *)mtds->__ptr[i].mtdOid;
			tmp_gb.size=(long)strlen(mtds->__ptr[i].mtdOid);
			tmp_gb1.buf=(char *)mtds->__ptr[i].mtdValue;
			tmp_gb1.size=(long)strlen(mtds->__ptr[i].mtdValue);

			if (tmp_gb1.size>max_metadata_value_length)
			{
				bmd_logout(&bi, deserialisation_max_memory);
				bmd_end();
				unlink(filename);
				remove(dirname);
				free(filename); filename=NULL;
				free(dirname); dirname=NULL;
				twfun_mark_error_in_transaction(transaction_dir_name,BMDSOAP_SERVER_METADATA_VALUE_TOO_LONG);
				PRINT_ERROR("SOAPSERVERERR %s\n",GetErrorMsg(BMDSOAP_SERVER_METADATA_VALUE_TOO_LONG));
				return soap_receiver_fault(soap,"Archive server error",
				GetErrorMsg(BMDSOAP_SERVER_METADATA_VALUE_TOO_LONG));
			}

			status=bmd_datagram_add_metadata_2((char *)tmp_gb.buf,&tmp_gb1,&dtg, 0);
			if(status<BMD_OK)
			{
				bmd_logout(&bi, deserialisation_max_memory);
				bmd_end();
				unlink(filename);
				remove(dirname);
				free(filename); filename=NULL;
				free(dirname); dirname=NULL;
				twfun_mark_error_in_transaction(transaction_dir_name, status);
				PRINT_ERROR("SOAPSERVERERR %s\n",GetErrorMsg(status));
				return soap_receiver_fault(soap,"Archive server error",GetErrorMsg(status));
			}
		}
	}

	if (transactionId!=NULL)
	{
		tmp_gb.buf=(char *)OID_SYS_METADATA_CRYPTO_OBJECTS_TRANSACTION_ID;
		tmp_gb.size=strlen(OID_SYS_METADATA_CRYPTO_OBJECTS_TRANSACTION_ID);
		tmp_gb1.buf=(char *)transactionId;
		tmp_gb1.size=strlen(transactionId);

		status=bmd_datagram_add_metadata_2((char *)tmp_gb.buf,&tmp_gb1,&dtg, 0);
		if(status<BMD_OK)
		{
			bmd_logout(&bi, deserialisation_max_memory);
			bmd_end();
			unlink(filename);
			remove(dirname);
			free(filename); filename=NULL;
			free(dirname); dirname=NULL;
			twfun_mark_error_in_transaction(transaction_dir_name, status);
			PRINT_ERROR("SOAPSERVERERR %s\n",GetErrorMsg(status));
			return soap_receiver_fault(soap,"Archive server error",GetErrorMsg(status));
		}
/*
		tmp_gb.buf=(char *)OID_ADD_VISIBLE_STR;
                        tmp_gb.size=strlen(OID_ADD_VISIBLE_STR);
                        tmp_gb1.buf=(char *)"0";
                        tmp_gb1.size=strlen("0");

		status=bmd_datagram_add_metadata_2((char *)tmp_gb.buf,&tmp_gb1,&dtg, 0);
		if(status<BMD_OK)
		{
			bmd_logout(&bi, deserialisation_max_memory);
			bmd_end();
			unlink(filename);
			remove(dirname);
			free(filename); filename=NULL;
			free(dirname); dirname=NULL;
			twfun_mark_error_in_transaction(transaction_dir_name, status);
			PRINT_ERROR("SOAPSERVERERR %s\n",GetErrorMsg(status));
			return soap_receiver_fault(soap,"Archive server error",GetErrorMsg(status));
		}
*/
	}

	status=bmd_datagram_add_to_set(dtg,&dtg_set);
	if(status<BMD_OK)
	{
		bmd_logout(&bi, deserialisation_max_memory);
		bmd_end();
		unlink(filename);
		remove(dirname);
		free(filename); filename=NULL;
		free(dirname); dirname=NULL;
		twfun_mark_error_in_transaction(transaction_dir_name, status);
		PRINT_ERROR("SOAPSERVERERR %s\n",GetErrorMsg(status));
		return soap_receiver_fault(soap,"Archive server error",GetErrorMsg(status));
	}

	status=bmd_send_request_lo(bi, dtg_set, max_datagram_in_set_transmission, &resp_dtg_set, li, deserialisation_max_memory);
	if (status==BMD_OK)
	{
		status=resp_dtg_set->bmdDatagramSetTable[0]->datagramStatus;
	}
	if(status<BMD_OK)
	{
		bmd_logout(&bi, deserialisation_max_memory);
		bmd_end();
		unlink(filename);
		remove(dirname);
		free(filename); filename=NULL;
		free(dirname); dirname=NULL;
		twfun_mark_error_in_transaction(transaction_dir_name, status);
		PRINT_ERROR("SOAPSERVERERR %s\n",GetErrorMsg(status));
		return soap_receiver_fault(soap,"Error serving request",GetErrorMsg(status));
	}

	if(id != NULL)
	{
		if (resp_dtg_set->bmdDatagramSetTable[0]->protocolDataFileId->buf!=NULL)
		{
			(*id)=atol((char *)resp_dtg_set->bmdDatagramSetTable[0]->protocolDataFileId->buf);
		}
		else
			(*id)=-1;
	}

	bmd_logout(&bi, deserialisation_max_memory);
	bmd_datagramset_free(&dtg_set);
	bmd_datagramset_free(&resp_dtg_set);
	bmd_lob_request_info_destroy(&li);
	bmd_end();

	unlink(filename);
	remove(dirname);
	if(transactionId != NULL)
	{
		SOAP_FOK(twfun_unmark_start_of_insert(	transaction_dir_name, transactionId, -1));
		free(transaction_dir_name); transaction_dir_name = NULL;
	}
	free(filename); filename=NULL;
	free(dirname); dirname=NULL;
	free(tmp_dir); tmp_dir=NULL;
	PRINT_SOAP("Status:\t\trequest served correctly\n");
	return SOAP_OK;
}


/**********************************************************/
/* obsluga zadania wyslania podpisanego pliku do archiwum */
/**********************************************************/
int bmd230__bmdInsertSignedFile(	struct soap *soap,
				struct xsd__base64Binary *cert,
				char *roleName,
				char *groupName,
				char *userClassId,
				struct bmd230__mtdsValues *mtds,
				struct bmd230__fileInfo *input,
				struct bmd230__myBinaryDataType *binarySignature,
				char* signatureType,
				char *transactionId,
				enum trans_t type,
				long *id)
{
long i					= 0;
long status				= 0;
long max_datagram_in_set_transmission	= 0;
long deserialisation_max_memory		= 0;
long max_metadata_value_length		= 0;
long start_transaction_required		= 0;
bmd_info_t *bi				= NULL;
bmdDatagram_t *dtg			= NULL;
bmdDatagramSet_t *dtg_set		= NULL;
bmdDatagramSet_t *resp_dtg_set		= NULL;
lob_request_info_t *li			= NULL;
char *tmp_dir				= NULL;
char *transaction_dir_name		= NULL;
char *filename				= NULL;
char* dirname				= NULL;
char *sigFileName			= NULL;
char* signatureTypeOid		= NULL;

GenBuf_t tmp_gb;
GenBuf_t tmp_gb1;

GenBuf_t *signature			= NULL;

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


	SOAP_FOK(bmdconf_get_value(_GLOBAL_konfiguracja,"soap","temp_dir",&tmp_dir));
	if(bmdconf_get_value_int(_GLOBAL_konfiguracja,"soap","start_transaction_required",&start_transaction_required) < BMD_OK)
	{
		start_transaction_required = 0;
	}


	if(signatureType == NULL)
	{
		free(tmp_dir);
		SOAP_FOK( BMDSOAP_SERVER_SIGNATURE_TYPE_NOT_SPECIFIED );
	}
	else
	{
		if( strcmp(signatureType, "CADES") == 0 )
		{
			signatureTypeOid=OID_PKI_METADATA_SIGNATURE;
		}
		else if(strcmp(signatureType, "XADES") == 0)
		{
			signatureTypeOid=OID_PKI_METADATA_SIGNATURE_XADES;
		}
		else if(strcmp(signatureType, "PADES") == 0)
		{
			signatureTypeOid=OID_PKI_METADATA_SIGNATURE_PADES;
		}
		else
		{
			free(tmp_dir);
			SOAP_FOK( BMDSOAP_SERVER_BAD_SIGNATURE_TYPE_PARAM );
		}
	}

	/*Ustalam czy jest katalog transakcji*/
	if(transactionId != NULL)
	{/*if(transactionId != NULL)*/
		SOAP_FOK(twfun_check_transaction_directory( tmp_dir, transactionId, start_transaction_required,
		&transaction_dir_name));
		SOAP_FOK(twfun_mark_start_of_insert(    transaction_dir_name, transactionId));
	}/*\if(transactionId != NULL)*/


	switch(type)
	{
		case nostream :
		{
			/******************************/
			/*	walidacja parametrow      */
			/******************************/
			if ((input==NULL) || (input->file==NULL) || (input->filename==NULL) || (input->file->__ptr==NULL) || (input->file->__size<=0))
			{
				twfun_mark_error_in_transaction(transaction_dir_name, BMDSOAP_SERVER_INPUT_FILE_UNDEF);
				SOAP_FOK(BMDSOAP_SERVER_INPUT_FILE_UNDEF);
			}

			if ((binarySignature==NULL) || (binarySignature->__ptr==NULL) || (binarySignature->__size<=0))
			{
				twfun_mark_error_in_transaction(transaction_dir_name, BMDSOAP_SERVER_INPUT_SIG_UNDEF);
				SOAP_FOK(BMDSOAP_SERVER_INPUT_SIG_UNDEF);
			}

			/**************************/
			/*	plik tymczasowy	  */
			/**************************/
			asprintf(&filename,"tmp%li.tmp", (long)getpid());

			tmp_gb.buf=(char*)input->file->__ptr;
			tmp_gb.size=input->file->__size;
			status=bmd_save_buf(&tmp_gb,filename);
			signature = (GenBuf_t*)malloc(sizeof(GenBuf_t));

			if ( signature == NULL )
			{
				twfun_mark_error_in_transaction(transaction_dir_name, BMD_ERR_MEMORY);
				PRINT_ERROR("SOAPSERVERERR %s. Error=%i\n",GetErrorMsg(BMD_ERR_MEMORY),BMD_ERR_MEMORY);
				return soap_receiver_fault(soap,"Error",GetErrorMsg(BMD_ERR_MEMORY));
			}

			signature->buf=(char *)binarySignature->__ptr;
			signature->size=binarySignature->__size;

			break;
		}
		case stream :
		{
			/******************************/
			/*	walidacja parametrow      */
			/******************************/
			if ((input==NULL) || (input->file==NULL) || (input->filename==NULL))
			{
				twfun_mark_error_in_transaction(transaction_dir_name, BMDSOAP_SERVER_INPUT_FILE_UNDEF);
				SOAP_FOK(BMDSOAP_SERVER_INPUT_FILE_UNDEF);
			}
			if ( binarySignature==NULL )
			{
				twfun_mark_error_in_transaction(transaction_dir_name, BMDSOAP_SERVER_INPUT_SIG_UNDEF);
				SOAP_FOK(BMDSOAP_SERVER_INPUT_SIG_UNDEF);
			}

			asprintf(&filename,"tmp_%li/%s", (long)getpid(), input->filename );
			asprintf(&dirname,"tmp_%li", (long)getpid() );
			asprintf(&sigFileName,"tmp_%li/%s", (long)getpid(), "podpis.sig" );

			status = bmd_load_binary_content( sigFileName, &signature );

			if (status != BMD_OK)
			{
				twfun_mark_error_in_transaction(transaction_dir_name, BMD_ERR_DISK);
				BMD_FOK(BMD_ERR_DISK);
			}
			break;
		}
		default :
		{
			twfun_mark_error_in_transaction(transaction_dir_name, BMDSOAP_SERVER_INVALID_TRANSMISSION_TYPE);
			SOAP_FOK(BMDSOAP_SERVER_INVALID_TRANSMISSION_TYPE);
		}
	}

	if(id != NULL)
	{
		(*id)=-1;
	}


	PRINT_SOAP("Request:\tinsert signed document to archive\n");

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
	if (status<BMD_OK)
	{
		unlink(filename);
		unlink(sigFileName);
		remove(dirname);
	        free(filename); filename=NULL;
		free(dirname); dirname=NULL;
		twfun_mark_error_in_transaction(transaction_dir_name, status);
        	PRINT_ERROR("SOAPSERVERERR %s\n",GetErrorMsg(status));
        	return soap_receiver_fault(soap,"Archive server error",GetErrorMsg(status));
	}

	status=bmd_lob_request_info_create(&li);
	if(status<BMD_OK)
	{
		bmd_logout(&bi, deserialisation_max_memory);
		bmd_end();
		unlink(filename);
		unlink(sigFileName);
		remove(dirname);
	        free(filename); filename=NULL;
		free(dirname); dirname=NULL;
		twfun_mark_error_in_transaction(transaction_dir_name, BMDSOAP_SERVER_LOB_REQUEST_ERROR);
		PRINT_ERROR("SOAPSERVERERR %s\n",GetErrorMsg(BMDSOAP_SERVER_LOB_REQUEST_ERROR));
		return soap_receiver_fault(soap,"Soap server error",GetErrorMsg(BMDSOAP_SERVER_LOB_REQUEST_ERROR));
	}

	status=bmd_lob_request_info_add_filename(filename,&li,BMD_SEND_LOB,0);
	if(status<BMD_OK)
	{
		bmd_logout(&bi, deserialisation_max_memory);
		bmd_end();
		unlink(filename);
		unlink(sigFileName);
		remove(dirname);
	        free(filename); filename=NULL;
		free(dirname); dirname=NULL;
		twfun_mark_error_in_transaction(transaction_dir_name, BMDSOAP_SERVER_ADD_FILE_LOB_REQUEST_ERROR);
		PRINT_ERROR("SOAPSERVERERR %s\n",GetErrorMsg(BMDSOAP_SERVER_ADD_FILE_LOB_REQUEST_ERROR));
		return soap_receiver_fault(soap,"Soap server error",GetErrorMsg(BMDSOAP_SERVER_ADD_FILE_LOB_REQUEST_ERROR));
	}

/*
	signature.buf=(char *)binarySignature->__ptr;
	signature.size=binarySignature->__size;*/

	status=bmd_datagram_create(BMD_DATAGRAM_TYPE_INSERT_LO,&dtg);
	if(status<BMD_OK)
	{
		bmd_logout(&bi, deserialisation_max_memory);
		bmd_end();
		twfun_mark_error_in_transaction(transaction_dir_name, status);
		PRINT_ERROR("SOAPSERVERERR Error while creating datagram\n");
		unlink(filename);
		unlink(sigFileName);
		remove(dirname);
	        free(filename); filename=NULL;
		free(dirname); dirname=NULL;
		return soap_receiver_fault(soap,"Internal error","Error while creating datagram");
	}
	if (userClassId!=NULL)
	{
		SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(	OID_ACTION_METADATA_CERT_CHOSEN_CLASS_ID, userClassId, &dtg));
	}

// 	status=AddCertLoginAsToDatagram(	cert,	&dtg);
// 	if(status<BMD_OK)
// 	{
// 		bmd_logout(&bi, deserialisation_max_memory);
// 		bmd_end();
// 		unlink(filename);
// 		remove(dirname);
// 		free(filename); filename=NULL;
// 		free(dirname); dirname=NULL;
// 		twfun_mark_error_in_transaction(transaction_dir_name, status);
// 		PRINT_ERROR("SOAPSERVERERR %s\n",GetErrorMsg(status));
// 		return soap_receiver_fault(soap,"Archive server error",GetErrorMsg(status));
// 	}

	status=bmd_datagram_set_filename(input->filename,&dtg);
	if(status<BMD_OK)
	{
		bmd_logout(&bi, deserialisation_max_memory);
		bmd_end();
		twfun_mark_error_in_transaction(transaction_dir_name, status);
		PRINT_ERROR("SOAPSERVERERR Error while setting datagram's filename\n");
		unlink(filename);
		unlink(sigFileName);
		remove(dirname);
	        free(filename); filename=NULL;
		free(dirname); dirname=NULL;
		PRINT_ERROR("SOAPSERVERERR %s\n",GetErrorMsg(status));
		return soap_receiver_fault(soap,"Archive server error",GetErrorMsg(status));
	}

	if ((mtds!=NULL) && (mtds->__ptr!=NULL) && (mtds->__size))
	{
		for (i=0; i<mtds->__size; i++)
		{
			if ((mtds->__ptr[i].mtdOid==NULL) || (mtds->__ptr[i].mtdValue==NULL))
			{
				continue;
			}
			tmp_gb.buf=(char *)mtds->__ptr[i].mtdOid;
			tmp_gb.size=strlen(mtds->__ptr[i].mtdOid);
			tmp_gb1.buf=(char *)mtds->__ptr[i].mtdValue;
			tmp_gb1.size=strlen(mtds->__ptr[i].mtdValue);

			if (tmp_gb1.size>max_metadata_value_length)
			{
				bmd_logout(&bi, deserialisation_max_memory);
				bmd_end();
				twfun_mark_error_in_transaction(transaction_dir_name,
				BMDSOAP_SERVER_METADATA_VALUE_TOO_LONG);
				unlink(filename);
				unlink(sigFileName);
				remove(dirname);
				free(filename); filename=NULL;
				free(dirname); dirname=NULL;
				return soap_receiver_fault(soap,"Archive server error",
				GetErrorMsg(BMDSOAP_SERVER_METADATA_VALUE_TOO_LONG));
			}

			status=bmd_datagram_add_metadata_2((char *)tmp_gb.buf,&tmp_gb1,&dtg, 0);
			if(status<BMD_OK)
			{
				bmd_logout(&bi, deserialisation_max_memory);
				bmd_end();
				twfun_mark_error_in_transaction(transaction_dir_name, status);
				PRINT_ERROR("SOAPSERVERERR Error while adding metadata to datagram\n");
				unlink(filename);
				unlink(sigFileName);
				remove(dirname);
				free(filename); filename=NULL;
				free(dirname); dirname=NULL;
				return soap_receiver_fault(soap,"Internal error","Error while adding metadata to datagram");
			}
		}
	}

	if (transactionId!=NULL)
	{
		tmp_gb.buf=(char *)OID_SYS_METADATA_CRYPTO_OBJECTS_TRANSACTION_ID;
		tmp_gb.size=strlen(OID_SYS_METADATA_CRYPTO_OBJECTS_TRANSACTION_ID);
		tmp_gb1.buf=(char *)transactionId;
		tmp_gb1.size=strlen(transactionId);

		status=bmd_datagram_add_metadata_2((char *)tmp_gb.buf,&tmp_gb1,&dtg, 0);
		if(status<BMD_OK)
		{
			bmd_logout(&bi, deserialisation_max_memory);
			bmd_end();
			unlink(filename);
			unlink(sigFileName);
			remove(dirname);
			free(filename); filename=NULL;
			free(dirname); dirname=NULL;
			twfun_mark_error_in_transaction(transaction_dir_name, status);
			PRINT_ERROR("SOAPSERVERERR %s\n",GetErrorMsg(status));
			return soap_receiver_fault(soap,"Archive server error",GetErrorMsg(status));
		}
/*
		tmp_gb.buf=(char *)OID_ADD_VISIBLE_STR;
                        tmp_gb.size=strlen(OID_ADD_VISIBLE_STR);
                        tmp_gb1.buf=(char *)"0";
                        tmp_gb1.size=strlen("0");

		status=bmd_datagram_add_metadata_2((char *)tmp_gb.buf,&tmp_gb1,&dtg, 0);
		if(status<BMD_OK)
		{
			bmd_logout(&bi, deserialisation_max_memory);
			bmd_end();
			unlink(filename);
			unlink(sigFileName);
			remove(dirname);
			free(filename); filename=NULL;
			free(dirname); dirname=NULL;
			twfun_mark_error_in_transaction(transaction_dir_name, status);
			PRINT_ERROR("SOAPSERVERERR %s\n",GetErrorMsg(status));
			return soap_receiver_fault(soap,"Archive server error",GetErrorMsg(status));
		}
*/
	}

	status=bmd_datagram_add_metadata_2(signatureTypeOid, signature, &dtg, 0);
	if(status<BMD_OK)
	{
		bmd_logout(&bi, deserialisation_max_memory);
		bmd_end();
		twfun_mark_error_in_transaction(transaction_dir_name, status);
		PRINT_ERROR("SOAPSERVERERR Error while adding metadata to datagram\n");
		unlink(filename);
		unlink(sigFileName);
		remove(dirname);
		free(filename); filename=NULL;
		free(dirname); dirname=NULL;
		return soap_receiver_fault(soap,"Internal error","Error while adding metadata to datagram");
	}

	status=bmd_datagram_add_to_set(dtg,&dtg_set);
	if(status<BMD_OK)
	{
		bmd_logout(&bi, deserialisation_max_memory);
		bmd_end();
		twfun_mark_error_in_transaction(transaction_dir_name, status);
		PRINT_ERROR("SOAPSERVERERR Error while adding datagram to set\n");
		unlink(filename);
		unlink(sigFileName);
		remove(dirname);
		free(filename); filename=NULL;
		free(dirname); dirname=NULL;
		return soap_receiver_fault(soap,"Internal error","Error while adding datagram to set");
	}

	status=bmd_send_request_lo(bi, dtg_set, max_datagram_in_set_transmission, &resp_dtg_set, li, deserialisation_max_memory);
	if (status==BMD_OK)
	{
		status=resp_dtg_set->bmdDatagramSetTable[0]->datagramStatus;
	}
	if(status<BMD_OK)
	{
		bmd_logout(&bi, deserialisation_max_memory);
		bmd_end();
		unlink(filename);
		unlink(sigFileName);
		remove(dirname);
		free(filename); filename=NULL;
		free(dirname); dirname=NULL;
		twfun_mark_error_in_transaction(transaction_dir_name, status);
		PRINT_ERROR("SOAPSERVERERR %s\n",GetErrorMsg(status));
		return soap_receiver_fault(soap,"Error serving request",GetErrorMsg(status));
	}

	if(id != NULL)
	{
		if (resp_dtg_set->bmdDatagramSetTable[0]->protocolDataFileId->buf!=NULL)
		{
			(*id)=atol((char *)resp_dtg_set->bmdDatagramSetTable[0]->protocolDataFileId->buf);
		}
		else
		{
			(*id)=-1;
		}
	}

	bmd_logout(&bi, deserialisation_max_memory);
	bmd_datagramset_free(&dtg_set);
	bmd_datagramset_free(&resp_dtg_set);
	bmd_lob_request_info_destroy(&li);
	bmd_end();
	PRINT_SOAP("Status:\t\trequest served correctly\n");

	unlink(filename);
	unlink(sigFileName);
	if(transactionId != NULL)
	{
		SOAP_FOK(twfun_unmark_start_of_insert(	transaction_dir_name, transactionId, -1));
		free(transaction_dir_name); transaction_dir_name = NULL;
	}
	remove(dirname);
	free(filename); filename=NULL;
	free(dirname); dirname=NULL;
	free(tmp_dir); tmp_dir=NULL;

	return SOAP_OK;
}


/*****************************************************/
/* obsluga zadania wyslania listy plikow do archiwum */
/*****************************************************/
int bmd230__bmdInsertFileList(	struct soap *soap,
				struct xsd__base64Binary *cert,
				char *roleName,
				char *groupName,
				char *userClassId,
				struct bmd230__fileComplexList *input,
				char *transactionId,
				enum trans_t type,
				struct bmd230__idList **ids)
{

long i					= 0;
long j					= 0;
long status				= 0;
long max_datagram_in_set_transmission	= 0;
long deserialisation_max_memory		= 0;
long start_transaction_required		= 0;
bmd_info_t *bi				= NULL;
bmdDatagram_t *dtg			= NULL;
bmdDatagramSet_t *dtg_set		= NULL;
bmdDatagramSet_t *resp_dtg_set		= NULL;
lob_request_info_t *li			= NULL;
char *tmp_dir				= NULL;
char *transaction_dir_name		= NULL;
char **filename				= NULL;
char* dirname				= NULL;

GenBuf_t tmp_gb;
GenBuf_t tmp_gb1;

char *derUserCert=NULL;
long derUserCertLen=0;
long retVal=0;
char* paramCert=NULL;
long paramCertLen=0;

	PRINT_SOAP("Request:\tinsert list of documents to archive\n");

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

	SOAP_FOK(bmdconf_get_value(_GLOBAL_konfiguracja,"soap","temp_dir",&tmp_dir));
	if(bmdconf_get_value_int(_GLOBAL_konfiguracja,"soap","start_transaction_required",&start_transaction_required) < BMD_OK)
	{
		start_transaction_required = 0;
	}

       /*Ustalam czy jest katalog transakcji*/
        if(transactionId != NULL)
        {/*if(transactionId != NULL)*/
                SOAP_FOK(twfun_check_transaction_directory( tmp_dir, transactionId, start_transaction_required,
                &transaction_dir_name));
                SOAP_FOK(twfun_mark_start_of_insert(    transaction_dir_name, transactionId));
        }/*\if(transactionId !=*/

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
	if (status<BMD_OK)
	{
		twfun_mark_error_in_transaction(transaction_dir_name, status);
		SOAP_FOK(status);
	}

	status=bmd_lob_request_info_create(&li);
	if(status<BMD_OK)
	{
		bmd_logout(&bi, deserialisation_max_memory);
		bmd_end();
		twfun_mark_error_in_transaction(transaction_dir_name, BMDSOAP_SERVER_LOB_REQUEST_ERROR);
		PRINT_ERROR("SOAPSERVERERR Error while preapring lob request\n");
		return soap_receiver_fault(soap,"Internal error","Error while preapring lob request");
	}

	/************************************/
	/*	alokacja pamieci na odpowiedz	*/
	/************************************/
	(*ids)=(struct bmd230__idList *)malloc(sizeof(struct bmd230__idList));
	(*ids)->__ptr=(int *)malloc(sizeof(int)*input->__size);
	(*ids)->__size=input->__size;

	filename=(char **)malloc(sizeof(char *)*input->__size);
	memset(filename, 0, sizeof(char *)*input->__size);


	for (i=0 ; i<input->__size; i++)
	{
		(*ids)->__ptr[i]=-1;
		switch(type)
		{
			case nostream :
			{
				/************************************************/
				/* tworzenie plików tymczasowych na dysku       */
				/* dla kazdego z przesylanych plikow            */
				/************************************************/
				asprintf(&(filename[i]),"tmp%li_%li.tmp", (long)getpid(), i);
				tmp_gb.buf=(char*)input->__ptr[i].file->file->__ptr;
				tmp_gb.size=input->__ptr[i].file->file->__size;
				bmd_save_buf(&tmp_gb,filename[i]);

				break;
			}

			case stream :
			{
				if ( input->__ptr[i].file->filename == NULL )
				{
					twfun_mark_error_in_transaction(transaction_dir_name,
					BMDSOAP_SERVER_INPUT_FILE_UNDEF);
					PRINT_ERROR("SOAPSERVERERR %s. Error=%i\n",
					GetErrorMsg(BMDSOAP_SERVER_INPUT_FILE_UNDEF),BMDSOAP_SERVER_INPUT_FILE_UNDEF);
					return soap_receiver_fault(soap,"Error",
					GetErrorMsg(BMDSOAP_SERVER_INPUT_FILE_UNDEF));
				}
				asprintf(&(filename[i]),"tmp_%li/%s", (long)getpid(), input->__ptr[i].file->filename );
				asprintf(&dirname,"tmp_%li", (long)getpid() );
				break;
			}
			default :
			{
				twfun_mark_error_in_transaction(transaction_dir_name,
				BMDSOAP_SERVER_INVALID_TRANSMISSION_TYPE);
				SOAP_FOK(BMDSOAP_SERVER_INVALID_TRANSMISSION_TYPE);
			}
		}

		status=bmd_lob_request_info_add_filename(filename[i],&li,BMD_SEND_LOB,0);
		if(status<BMD_OK)
		{
			bmd_logout(&bi, deserialisation_max_memory);
			bmd_end();
			twfun_mark_error_in_transaction(transaction_dir_name, BMDSOAP_SERVER_ADD_FILE_LOB_REQUEST_ERROR);
			unlink(filename[i]);
			remove(dirname);
			return soap_receiver_fault(soap,"Wewnetrzny blad","Wystapil wewnetrzny blad nr 6");
		}

		status=bmd_datagram_create(BMD_DATAGRAM_TYPE_INSERT_LO,&dtg);
		if(status<BMD_OK)
		{
			bmd_logout(&bi, deserialisation_max_memory);
			bmd_end();
			unlink(filename[i]);
			remove(dirname);
			twfun_mark_error_in_transaction(transaction_dir_name, status);
			PRINT_ERROR("SOAPSERVERERR Error while creating datagram\n");
			return soap_receiver_fault(soap,"Internal error","Error while creating datagram");
		}
		if (userClassId!=NULL)
		{
			SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(	OID_ACTION_METADATA_CERT_CHOSEN_CLASS_ID, userClassId, &dtg));
		}


		fwide(stdout,1);
		status=bmd_datagram_set_filename(input->__ptr[i].file->filename,&dtg);
		if(status<BMD_OK)
		{
			bmd_logout(&bi, deserialisation_max_memory);
			bmd_end();
			unlink(filename[i]);
			twfun_mark_error_in_transaction(transaction_dir_name, status);
			PRINT_ERROR("SOAPSERVERERR Error while setting datagram's filename\n");
			return soap_receiver_fault(soap,"Internal error","Error while setting datagram's filename");
		}


		if ((input->__ptr[i].mtds!=NULL) && (input->__ptr[i].mtds->__ptr!=NULL) && (input->__ptr[i].mtds->__size>0))
		{
			for (j=0; j<input->__ptr[i].mtds->__size; j++)
			{
				if ((input->__ptr[i].mtds->__ptr[j].mtdOid==NULL) || (input->__ptr[i].mtds->__ptr[j].mtdValue==NULL))
				{
					continue;
				}
				tmp_gb.buf=(char *)input->__ptr[i].mtds->__ptr[j].mtdOid;
				tmp_gb.size=strlen(input->__ptr[i].mtds->__ptr[j].mtdOid);
				tmp_gb1.buf=(char *)input->__ptr[i].mtds->__ptr[j].mtdValue;
				tmp_gb1.size=strlen(input->__ptr[i].mtds->__ptr[j].mtdValue);
				status=bmd_datagram_add_metadata_2((char *)tmp_gb.buf,&tmp_gb1,&dtg, 0);
				if(status<BMD_OK)
				{
					bmd_logout(&bi, deserialisation_max_memory);
					bmd_end();
					unlink(filename[i]);
					remove(dirname);
					twfun_mark_error_in_transaction(transaction_dir_name, status);
					PRINT_ERROR("SOAPSERVERERR %s\n",GetErrorMsg(status));
					return soap_receiver_fault(soap,"Archive server error",GetErrorMsg(status));
				}
			}
		}

		if (transactionId!=NULL)
		{
			tmp_gb.buf=(char *)OID_SYS_METADATA_CRYPTO_OBJECTS_TRANSACTION_ID;
			tmp_gb.size=strlen(OID_SYS_METADATA_CRYPTO_OBJECTS_TRANSACTION_ID);
			tmp_gb1.buf=(char *)transactionId;
			tmp_gb1.size=strlen(transactionId);

			status=bmd_datagram_add_metadata_2((char *)tmp_gb.buf,&tmp_gb1,&dtg, 0);
			if(status<BMD_OK)
			{
				bmd_logout(&bi, deserialisation_max_memory);
				bmd_end();
				unlink(filename[i]);
				remove(dirname);

				twfun_mark_error_in_transaction(transaction_dir_name, status);
				PRINT_ERROR("SOAPSERVERERR %s\n",GetErrorMsg(status));
				return soap_receiver_fault(soap,"Archive server error",GetErrorMsg(status));
			}

		}

		status=bmd_datagram_add_to_set(dtg,&dtg_set);
		if(status<BMD_OK)
		{
			bmd_logout(&bi, deserialisation_max_memory);
			bmd_end();
			unlink(filename[i]);
			remove(dirname);
			twfun_mark_error_in_transaction(transaction_dir_name, status);
			PRINT_ERROR("SOAPSERVERERR Error while adding datagram to set\n");
			return soap_receiver_fault(soap,"Internal error","Error while adding datagram to set");
		}
	}

	status=bmd_send_request_lo(bi, dtg_set, max_datagram_in_set_transmission, &resp_dtg_set, li, deserialisation_max_memory);
	if(status<BMD_OK)
	{

		bmd_logout(&bi, deserialisation_max_memory);
		bmd_end();
		for (i=0 ; i<input->__size; i++)
		{
			(*ids)->__ptr[i]=status;
			unlink(filename[i]);
			remove(dirname);

			free(filename[i]); filename[i]=NULL;
		}

		remove(dirname);
		twfun_mark_error_in_transaction(transaction_dir_name, status);
		PRINT_ERROR("SOAPSERVERERR %s\n",GetErrorMsg(status));
		return soap_receiver_fault(soap,"Error serving request",GetErrorMsg(status));

	}

	for (i=0 ; i<input->__size; i++)
	{
		if (resp_dtg_set->bmdDatagramSetTable[i]->datagramStatus<BMD_OK)
		{
			(*ids)->__ptr[i]=resp_dtg_set->bmdDatagramSetTable[i]->datagramStatus;
		}
		else if (resp_dtg_set->bmdDatagramSetTable[i]->protocolDataFileId->buf!=NULL)
		{
			(*ids)->__ptr[i]=atol((char*)resp_dtg_set->bmdDatagramSetTable[i]->protocolDataFileId->buf);
		}
	}

	bmd_logout(&bi, deserialisation_max_memory);
	bmd_datagramset_free(&dtg_set);
	bmd_datagramset_free(&resp_dtg_set);
	bmd_lob_request_info_destroy(&li);
	bmd_end();


	for (i=0 ; i<input->__size; i++)
	{
		unlink(filename[i]);
		free(filename[i]); filename[i]=NULL;
	}
	free(filename); filename=NULL;
	free(tmp_dir); tmp_dir=NULL;
	remove(dirname);

	/*Ustalam czy jest katalog transakcji*/
	if(transactionId != NULL)
	{/*if(transactionId != NULL)*/
		SOAP_FOK(twfun_check_transaction_directory( tmp_dir, transactionId, start_transaction_required,
		&transaction_dir_name));
		SOAP_FOK(twfun_unmark_start_of_insert(    transaction_dir_name, transactionId, -1));
	}/*\if(transactionId != NULL)*/

	PRINT_SOAP("Status:\t\trequest served correctly\n");

	return SOAP_OK;
}


/**********************************************/
/* obsluga aktualizacji pliku w archiwum */
/**********************************************/
int bmd230__bmdUpdateFileVersion(	struct soap *soap,
			struct xsd__base64Binary *cert,
			char *userRoleId,
			char *userGroupId,
			char *userClassId,
			char* fileId,
			struct bmd230__fileInfo *input,
			enum trans_t type,
			long* updatedFileId)
{
long status				= 0;
long max_datagram_in_set_transmission	= 0;
long deserialisation_max_memory		= 0;
long max_metadata_value_length		= 0;

bmd_info_t *bi				= NULL;
bmdDatagram_t *dtg			= NULL;
bmdDatagramSet_t *dtg_set		= NULL;
bmdDatagramSet_t *resp_dtg_set		= NULL;
lob_request_info_t *li			= NULL;
char *filename				= NULL;
GenBuf_t tmp_gb;

char *derUserCert=NULL;
long derUserCertLen=0;
long retVal=0;
char* paramCert=NULL;
long paramCertLen=0;



	/************************/
	/* walidacja parametrow */
	/************************/
	if (input==NULL)			{	SOAP_FOK(BMDSOAP_SERVER_INPUT_FILE_UNDEF);	}
	if (input->file==NULL)			{	SOAP_FOK(BMDSOAP_SERVER_INPUT_FILE_UNDEF);	}
	if (input->filename==NULL)		{	SOAP_FOK(BMDSOAP_SERVER_INPUT_FILE_UNDEF);	}
	if (input->file->__ptr==NULL)		{	SOAP_FOK(BMDSOAP_SERVER_INPUT_FILE_UNDEF);	}
	if (input->file->__size<=0)		{	SOAP_FOK(BMDSOAP_SERVER_INPUT_FILE_UNDEF);	}

	if(fileId == NULL)
		{ SOAP_FOK(BMD_ERR_PARAM5); }
	if(strlen(fileId) <= 0)
		{ SOAP_FOK(BMD_ERR_PARAM5); }

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


	switch(type)
	{
		case nostream :
		{
			/******************************/
			/*	walidacja parametrow      */
			/******************************/
			if ((input==NULL) || (input->file==NULL) || (input->filename==NULL) || (input->file->__ptr==NULL) || (input->file->__size<=0))
			{
				SOAP_FOK(BMDSOAP_SERVER_INPUT_FILE_UNDEF);
			}

			/************************/
			/*	plik tymczasowy	     */
			/************************/
			asprintf(&filename,"tmp%li.tmp", (long)getpid());
			tmp_gb.buf=(char*)input->file->__ptr;
			tmp_gb.size=input->file->__size;
			status=bmd_save_buf(&tmp_gb,filename);

			break;
		}
		case stream :
		{
			/******************************/
			/*	walidacja parametrow      */
			/******************************/
			if ((input==NULL) || (input->file==NULL) || (input->filename==NULL))
			{
				SOAP_FOK(BMDSOAP_SERVER_INPUT_FILE_UNDEF);
			}

			asprintf(&filename,"tmp_%li/%s", (long)getpid(), input->filename );
			break;
		}

		default :
		{
			SOAP_FOK(BMDSOAP_SERVER_INVALID_TRANSMISSION_TYPE);
		}

	}

	PRINT_SOAP("Request:\tupdate file version\n");

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
	/*	logowanie do bmd*/
	/************************/
	SOAP_FOK(PrepareDataForBMDLoginAndRequest(&bi,
				(_GLOBAL_soap_wssecurity_enabled == 1  ? derUserCert : paramCert),
				(_GLOBAL_soap_wssecurity_enabled == 1  ? derUserCertLen : paramCertLen),
				userRoleId, userGroupId, deserialisation_max_memory));
	free(derUserCert); derUserCert=NULL;
	paramCert=NULL; paramCertLen=0;
	if (status<BMD_OK)
	{
		unlink(filename);
		free(filename); filename=NULL;
		PRINT_ERROR("SOAPSERVERERR %s\n",GetErrorMsg(status));
        	return soap_receiver_fault(soap,"Archive server error",GetErrorMsg(status));
	}

	status=bmd_lob_request_info_create(&li);
	if(status<BMD_OK)
	{
		bmd_logout(&bi, deserialisation_max_memory);
		bmd_info_destroy(&bi);
		bmd_end();
		unlink(filename);
		free(filename); filename=NULL;
		PRINT_ERROR("SOAPSERVERERR %s\n",GetErrorMsg(BMDSOAP_SERVER_LOB_REQUEST_ERROR));
		return soap_receiver_fault(soap,"Soap server error",GetErrorMsg(BMDSOAP_SERVER_LOB_REQUEST_ERROR));
	}

	status=bmd_lob_request_info_add_filename(filename,&li,BMD_SEND_LOB,0);
	if(status<BMD_OK)
	{
		bmd_logout(&bi, deserialisation_max_memory);
		bmd_info_destroy(&bi);
		bmd_end();
		unlink(filename);
		free(filename); filename=NULL;
		bmd_lob_request_info_destroy(&li);
		PRINT_ERROR("SOAPSERVERERR %s\n",GetErrorMsg(BMDSOAP_SERVER_ADD_FILE_LOB_REQUEST_ERROR));
		return soap_receiver_fault(soap,"Soap server error",GetErrorMsg(BMDSOAP_SERVER_ADD_FILE_LOB_REQUEST_ERROR));
	}

	status=bmd_datagram_create(BMD_DATAGRAM_TYPE_UPDATE_FILE_VERSION_LO,&dtg);
	if(status<BMD_OK)
	{
		bmd_logout(&bi, deserialisation_max_memory);
		bmd_info_destroy(&bi);
		bmd_end();
		unlink(filename);
		free(filename); filename=NULL;
		bmd_lob_request_info_destroy(&li);
		PRINT_ERROR("SOAPSERVERERR %s\n",GetErrorMsg(status));
		return soap_receiver_fault(soap,"Archive server error",GetErrorMsg(status));
	}

	if (userClassId!=NULL)
	{
		SOAP_FOK_LOGOUT(bmd_datagram_add_metadata_char(	OID_ACTION_METADATA_CERT_CHOSEN_CLASS_ID, userClassId, &dtg));
	}


	status=set_gen_buf2(fileId, strlen(fileId), &(dtg->protocolDataFileId) );
	if(status<BMD_OK)
	{
		bmd_logout(&bi, deserialisation_max_memory);
		bmd_info_destroy(&bi);
		bmd_end();
		unlink(filename);
		free(filename); filename=NULL;
		bmd_lob_request_info_destroy(&li);
		bmd_datagram_free(&dtg);
		PRINT_ERROR("SOAPSERVERERR %s\n",GetErrorMsg(status));
		return soap_receiver_fault(soap,"Archive server error",GetErrorMsg(status));
	}


	status=bmd_datagram_add_to_set(dtg,&dtg_set);
	if(status<BMD_OK)
	{
		bmd_logout(&bi, deserialisation_max_memory);
		bmd_info_destroy(&bi);
		bmd_end();
		unlink(filename);
		free(filename); filename=NULL;
		bmd_lob_request_info_destroy(&li);
		bmd_datagram_free(&dtg);
		PRINT_ERROR("SOAPSERVERERR %s\n",GetErrorMsg(status));
		return soap_receiver_fault(soap,"Archive server error",GetErrorMsg(status));
	}

	status=bmd_send_request_lo(bi, dtg_set, max_datagram_in_set_transmission, &resp_dtg_set, li, deserialisation_max_memory);
	bmd_lob_request_info_destroy(&li);
	bmd_datagramset_free(&dtg_set);
	bmd_logout(&bi, deserialisation_max_memory);
	bmd_info_destroy(&bi);
	if (status==BMD_OK)
	{
		status=resp_dtg_set->bmdDatagramSetTable[0]->datagramStatus;
	}
	if(status<BMD_OK)
	{
		bmd_end();
		unlink(filename);
		free(filename); filename=NULL;
		PRINT_ERROR("SOAPSERVERERR %s\n",GetErrorMsg(status));
		return soap_receiver_fault(soap,"Error serving request",GetErrorMsg(status));
	}

	if(updatedFileId != NULL)
	{
		if(resp_dtg_set->bmdDatagramSetTable[0]->protocolDataFileId != NULL && resp_dtg_set->bmdDatagramSetTable[0]->protocolDataFileId->buf!=NULL)
		{
			(*updatedFileId)=atol((char *)resp_dtg_set->bmdDatagramSetTable[0]->protocolDataFileId->buf);
		}
		else
			(*updatedFileId)=-1;
	}

	bmd_datagramset_free(&resp_dtg_set);
	bmd_end();

	unlink(filename);
	free(filename); filename=NULL;
	PRINT_SOAP("Status:\t\trequest served correctly\n");
	return SOAP_OK;
}


/********************************************/
/* rozpoczecie transmisji pliku w kawalkach */
/********************************************/
int bmd230__bmdInsertFileChunkStart(struct soap *soap,
				struct xsd__base64Binary *cert,
				char *roleName,
				char *groupName,
				char *userClassId,
				struct bmd230__mtdsValues *mtds,
				char *finalfilename,
				long totalfilesize,
				char *transactionId,
				char **tmpfilename)

{
long tmp_pid					= 0;
long lob_session_hash				= 0;
long i						= 0;
long stream_timeout				= 0;
long file_lock_desc				= 0;
long max_datagram_in_set_transmission		= 0;
long deserialisation_max_memory			= 0;
long max_metadata_value_length			= 0;
long start_transaction_required			= 0;
long twl_parent_pid				= 0;

char tmp_file[]					= {"soap_insert_chunk_"};
char file_lock[]				= {"soap_insert_lock_"};
char *tmp_dir					= NULL;
char *filename					= NULL;
char *tmp_location				= NULL;
char *file_lock_name				= NULL;
char *transaction_dir_name			= NULL;
char *twl_pid_file				= NULL;
bmd_info_t *bi					= NULL;
lob_request_info_t *li				= NULL;
bmdDatagram_t *dtg				= NULL;
bmdDatagramSet_t *dtg_set			= NULL;
bmdDatagramSet_t *resp_dtg_set			= NULL;
FILE *fd					= NULL;
struct timeval now;
struct tm czas;
GenBuf_t tmp_gb;
GenBuf_t tmp_gb1;
long twl_status					= 0;

char *derUserCert=NULL;
long derUserCertLen=0;
long retVal=0;
char* paramCert=NULL;
long paramCertLen=0;

	PRINT_SOAP("Request:\tstart insert chunk transmission\n");
	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if (finalfilename==NULL)			{	SOAP_FOK(BMDSOAP_SERVER_FINAL_FILENAME_UNDEF);	}
	if (totalfilesize<=0)				{	SOAP_FOK(BMDSOAP_SERVER_TOTAL_FILESIZE_INVALID);	}

	/******************************************/
	/*	wczytanie ustawien konfiguracyjnych	*/
	/******************************************/
	SOAP_FOK(bmdconf_get_value(_GLOBAL_konfiguracja,"soap","temp_dir",&tmp_dir));
	bmdconf_get_value_int(_GLOBAL_konfiguracja,"soap","stream_timeout",&stream_timeout);

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

	if (bmdconf_get_value_int(_GLOBAL_konfiguracja,"soap","max_metadata_value_length",&max_metadata_value_length)<BMD_OK)
	{
		max_metadata_value_length=256;
	}

	if(bmdconf_get_value_int(_GLOBAL_konfiguracja,"soap","start_transaction_required",&start_transaction_required) < BMD_OK)
	{
		start_transaction_required = 0;
	}

	/*Ustalam czy jest katalog transakcji*/
	if(transactionId != NULL)
	{/*if(transactionId != NULL)*/
		SOAP_FOK(twfun_check_transaction_directory( tmp_dir, transactionId, start_transaction_required,
				&transaction_dir_name));
		SOAP_FOK(twfun_mark_start_of_insert(    transaction_dir_name, transactionId));
	}/*\if(transactionId != NULL)*/

	PRINT_VDEBUG("twg_transaction_dir_name %s\n", _GLOBAL_twg_transaction_dir_name);

	/************************************/
	/*	odczytanie katalogu biezacego	*/
	/************************************/
	memset(_GLOBAL_current_dir, 0, 256);
	if (getcwd(_GLOBAL_current_dir,255)==NULL)
	{
		twfun_mark_error_in_transaction(transaction_dir_name, BMDSOAP_SERVER_GCWD_ERROR);
		SOAP_FOK(BMDSOAP_SERVER_GCWD_ERROR);
	}

	/******************************************************/
	/*	wygenerowanie losowego hasha dla sesji lobowej	*/
	/******************************************************/
	gettimeofday(&now,NULL);
	localtime_r(&(now.tv_sec),&czas);
     	srand(now.tv_usec);
	lob_session_hash=(1+(long) (10000.0*rand()/(RAND_MAX+1.0)))*1000+czas.tm_hour*100+czas.tm_min*10+czas.tm_sec;

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
	twl_status = PrepareDataForBMDLoginAndRequest(&bi,
				(_GLOBAL_soap_wssecurity_enabled == 1  ? derUserCert : paramCert),
				(_GLOBAL_soap_wssecurity_enabled == 1  ? derUserCertLen : paramCertLen),
				roleName, groupName, deserialisation_max_memory);
	free(derUserCert); derUserCert=NULL;
	paramCert=NULL; paramCertLen=0;
	if(twl_status < 0)
	{
		twfun_mark_error_in_transaction(transaction_dir_name, twl_status);
		SOAP_FOK(twl_status);
	}

	asprintf(&file_lock_name,"%s%s%li%li", tmp_dir, file_lock, (long)getpid(), lob_session_hash);
	if(file_lock_name == NULL)
	{
		twfun_mark_error_in_transaction(transaction_dir_name, NO_MEMORY);
		SOAP_FOK(NO_MEMORY);
	}
	PRINT_DEBUG("SOAPSERVERDEBUG Lock file %s\n", file_lock_name);
	/*twg_file_lock_name = file_lock_name;*/

	file_lock_desc = open(file_lock_name, O_RDONLY | O_CREAT | O_EXCL, S_IWUSR | S_IRUSR);
	if(file_lock_desc < 0)
	{
		twfun_mark_error_in_transaction(transaction_dir_name, BMDSOAP_SERVER_LOCK_FILE_ERROR);
		SOAP_FOK(BMDSOAP_SERVER_LOCK_FILE_ERROR);
	}
	if(close(file_lock_desc) < 0)
	{
		twfun_mark_error_in_transaction(transaction_dir_name, BMDSOAP_SERVER_LOCK_FILE_ERROR);
		SOAP_FOK(BMDSOAP_SERVER_LOCK_FILE_ERROR);
	}
	file_lock_desc = 0;

	/************************************************************************************/
	/*	stworzenie podprocesu przeszukujacego plik tymczasowy w poszukiwaniu plikow	*/
	/************************************************************************************/
	twl_parent_pid = getpid();

	tmp_pid=fork();
	if (tmp_pid==0)	/* dzieciak */
	{
	/************************************************************************************************************/
	/*	stworzenie katalogu tymczasowego dla pobranych danych z serwera i przejscie do katalogu tymczasowego	*/
	/************************************************************************************************************/
		if(signal(SIGUSR, kill_me_insert) == SIG_ERR)
		{
			twfun_mark_error_in_transaction(transaction_dir_name, BMDSOAP_SERVER_INTERNAL_ERROR);
			PRINT_ERROR("SOAPSERVERERR Error in signal setting - SIGUSR!\n");
			bmd_logout(&bi, deserialisation_max_memory);
			bmd_end();
			unlink(file_lock_name);
			/*twg_file_lock_name = NULL;*/

			exit(1);
		}

		/*Zmieniam marker otwartej transmisji na marker z pid aktualnego procesu*/
		if(transactionId != NULL)
		{
			SOAP_FOK(twfun_mark_start_of_insert(	transaction_dir_name, transactionId));
			SOAP_FOK(twfun_unmark_start_of_insert(	transaction_dir_name, transactionId, twl_parent_pid));
		}

		asprintf(&tmp_location, "%s/%s%li%li/", tmp_dir, tmp_file, (long)getpid(), lob_session_hash);
		if(tmp_location == NULL)
		{
			twfun_mark_error_in_transaction(transaction_dir_name, NO_MEMORY);
			PRINT_ERROR("SOAPSERVERERR Error in allocating memory\n");
			bmd_logout(&bi, deserialisation_max_memory);
			bmd_end();
			unlink(file_lock_name);
			/*twg_file_lock_name = NULL;*/

			exit(1);
		}
		/*twg_tmp_location = tmp_location;*/

		umask(000);
		if(mkdir(tmp_location, 0777) < 0)
		{
			twfun_mark_error_in_transaction(transaction_dir_name, BMDSOAP_SERVER_INTERNAL_ERROR);
			PRINT_ERROR("SOAPSERVERERR Error in creating temporary directory: %s\n", tmp_location);
			bmd_logout(&bi, deserialisation_max_memory);
			bmd_end();
			unlink(file_lock_name);
			/*twg_file_lock_name = NULL;*/

			exit(1);
		}

		asprintf(&twl_pid_file, "%s%s%li", tmp_location, "pid_", (long)getpid());
		if(twl_pid_file == NULL)
		{
			twfun_mark_error_in_transaction(transaction_dir_name, NO_MEMORY);
			PRINT_ERROR("SOAPSERVERERR Error in allocating memory\n");
			bmd_logout(&bi, deserialisation_max_memory);
			bmd_end();
			unlink(file_lock_name);
			/*twg_file_lock_name = NULL;*/

			exit(1);
		}

		PRINT_DEBUG("SOAPSERVERDEBUG Pid file %s\n", twl_pid_file);
		file_lock_desc = open(twl_pid_file, O_RDONLY | O_CREAT | O_EXCL, S_IWUSR | S_IRUSR);

		if(file_lock_desc < 0)
		{
			twfun_mark_error_in_transaction(transaction_dir_name, BMDSOAP_SERVER_PID_FILE_ERROR);
			PRINT_ERROR("SOAPSERVERERR Error in creating pid file\n");
			bmd_logout(&bi, deserialisation_max_memory);
			bmd_end();
			unlink(file_lock_name);
			/*twg_file_lock_name = NULL;*/

			exit(1);
		}
		if(close(file_lock_desc) < 0)
		{
			twfun_mark_error_in_transaction(transaction_dir_name, BMDSOAP_SERVER_PID_FILE_ERROR);
			PRINT_ERROR("SOAPSERVERERR Error in closing pid file\n");
			bmd_logout(&bi, deserialisation_max_memory);
			bmd_end();
			unlink(file_lock_name);
			/*twg_file_lock_name = NULL;*/

			exit(1);
		}
		file_lock_desc = 0;

		/*Moge usunac plik blokady*/
		if(unlink(file_lock_name) < 0)
		{
			twfun_mark_error_in_transaction(transaction_dir_name, BMDSOAP_SERVER_INTERNAL_ERROR);
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
		SOAP_FOK_LOGOUT_TMP_EXT_1(bmd_datagram_create(BMD_DATAGRAM_TYPE_INSERT_LO,&dtg));
		if (userClassId!=NULL)
		{
			SOAP_FOK_LOGOUT_TMP_EXT_1(bmd_datagram_add_metadata_char(	OID_ACTION_METADATA_CERT_CHOSEN_CLASS_ID, userClassId, &dtg));
		}

		SOAP_FOK_LOGOUT_TMP_EXT_1(bmd_datagram_set_filename(finalfilename,&dtg));
		if ((mtds!=NULL) && (mtds->__ptr!=NULL) && (mtds->__size>0))
		{
			for (i=0; i<mtds->__size; i++)
			{
				if ((mtds->__ptr[i].mtdOid==NULL) || (mtds->__ptr[i].mtdValue==NULL))
				{
					continue;
				}
				tmp_gb.buf=(char *)mtds->__ptr[i].mtdOid;
				tmp_gb.size=strlen(mtds->__ptr[i].mtdOid);
				tmp_gb1.buf=(char *)mtds->__ptr[i].mtdValue;
				tmp_gb1.size=strlen(mtds->__ptr[i].mtdValue);

				if (tmp_gb1.size>max_metadata_value_length)
				{
					SOAP_FOK_LOGOUT_TMP_EXT_1(BMDSOAP_SERVER_METADATA_VALUE_TOO_LONG);
				}

				SOAP_FOK_LOGOUT_TMP_EXT_1(bmd_datagram_add_metadata_2((char *)tmp_gb.buf,&tmp_gb1,&dtg, 0));
			}
		}
		if (transactionId!=NULL)
		{
			tmp_gb.buf=(char *)OID_SYS_METADATA_CRYPTO_OBJECTS_TRANSACTION_ID;
			tmp_gb.size=strlen(OID_SYS_METADATA_CRYPTO_OBJECTS_TRANSACTION_ID);
			tmp_gb1.buf=(char *)transactionId;
			tmp_gb1.size=strlen(transactionId);

			SOAP_FOK_LOGOUT_TMP_EXT_1(bmd_datagram_add_metadata_2((char *)tmp_gb.buf,&tmp_gb1,&dtg, 0));

			asprintf(&filename,"transaction_id_%s", transactionId);
			if ((fd=fopen(filename, "w+"))==NULL)
			{
				SOAP_FOK_LOGOUT_TMP_EXT_1(BMDSOAP_SERVER_ERR_CREATING_TRANSACTION_MARKER);
			}
			else
			{
				fclose(fd);
			}
			free(filename); filename=NULL;
		}

		SOAP_FOK_LOGOUT_TMP_EXT_1(bmd_datagram_add_to_set(dtg,&dtg_set));
		SOAP_FOK_LOGOUT_TMP_EXT_1(bmd_lob_request_info_create(&li));
		SOAP_FOK_LOGOUT_TMP_EXT_1(bmd_lob_request_info_add_filename(tmp_location,&li,BMD_SEND_LOB,0));
		SOAP_FOK_LOGOUT_TMP_EXT_1(bmd_send_request_lo_in_parts(bi,dtg_set, max_datagram_in_set_transmission,&resp_dtg_set, li, NULL, totalfilesize, lob_session_hash,stream_timeout, deserialisation_max_memory));
		PRINT_VDEBUG("twg_transaction_dir_name %s\n", _GLOBAL_twg_transaction_dir_name);
		SOAP_FOK_LOGOUT_TMP_EXT_1(resp_dtg_set->bmdDatagramSetTable[0]->datagramStatus);

		/******************************************************************/
		/*	powrot do katalogu pierwotnego i usuniecie tymczasowego	*/
		/******************************************************************/
		chdir(_GLOBAL_current_dir);
		remove_dir(tmp_location);

		/******************/
		/*	porzadki w dzieciaku	*/
		/******************/
		if(transactionId != NULL)
		{
			SOAP_FOK(twfun_unmark_start_of_insert(    transaction_dir_name, transactionId, -1));
			free(transaction_dir_name); transaction_dir_name = NULL;
		}

		free(tmp_dir); tmp_dir=NULL;
		free(filename); filename=NULL;
		free(tmp_location); tmp_location=NULL;
		free(file_lock_name); file_lock_name = NULL;

		bmd_datagramset_free(&dtg_set);
		bmd_datagramset_free(&resp_dtg_set);
		bmd_lob_request_info_destroy(&li);
		bmd_logout(&bi, deserialisation_max_memory);
		bmd_end();

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
			/*twg_file_lock_name = NULL;*/
			twfun_mark_error_in_transaction(transaction_dir_name, BMDSOAP_SERVER_LOCK_FILE_ERROR);
			SOAP_FOK(BMDSOAP_SERVER_LOCK_FILE_ERROR);
		}
	}
	/*twg_file_lock_name = NULL;*/

	/**************************************/
	/* okreslenie tymczasowej nazwy pliku */
	/**************************************/
	asprintf(&filename,"%li%li", tmp_pid, lob_session_hash);

	/************************************************/
	/*	ustawienie tymczasowej natmp_dirzwy zwrotnej	*/
	/************************************************/
	*tmpfilename = (char*)soap_malloc(soap, strlen(filename)+1);
	memset(*tmpfilename, 0, strlen(filename)+1);
	memcpy(*tmpfilename, filename, strlen(filename));

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
int bmd230__bmdInsertFileChunkIter(	struct soap *soap,
				struct xsd__base64Binary *cert,
				char *roleName,
				char *groupName,
				char *userClassId,
				struct bmd230__fileInfo *input,
				char *tmpfilename,
				char *count,
				long *result)
{
long file				= 0;
long file2				= 0;
long i					= 0;
long stream_timeout			= 0;
char *filename				= NULL;
char *tmp_dir				= NULL;
char *tmp_catalog			= NULL;
char *tmp_location			= NULL;
char *tmp_location_lock			= NULL;
char tmp_file[]				= {"soap_insert_chunk_"};


	*result=-1;
	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if (tmpfilename==NULL)		{	SOAP_FOK(BMDSOAP_SERVER_TEMP_FILENAME_UNDEF);	}
	if (count<=0)			{	SOAP_FOK(BMDSOAP_SERVER_PACK_NUMBER_INVALID);	}
	if (input==NULL)		{	SOAP_FOK(BMDSOAP_SERVER_INPUT_FILE_UNDEF);	}
	if (input->file==NULL)		{	SOAP_FOK(BMDSOAP_SERVER_INPUT_FILE_UNDEF);	}
	if (input->file->__size<=0)	{	SOAP_FOK(BMDSOAP_SERVER_INPUT_FILE_UNDEF);	}
	if (input->file->__ptr==NULL)	{	SOAP_FOK(BMDSOAP_SERVER_INPUT_FILE_UNDEF);	}

	PRINT_SOAP("Request:\tcontinue insert chunk transmission\n");
	/******************************************/
	/*	wczytanie ustawien konfiguracyjnych	*/
	/******************************************/
	SOAP_FOK(bmdconf_get_value(_GLOBAL_konfiguracja,"soap","temp_dir",&tmp_dir));
	SOAP_FOK(bmdconf_get_value_int(_GLOBAL_konfiguracja,"soap","stream_timeout",&stream_timeout));

	/******************************************/
	/*	okreslenie tymczasowej nazwy pliku	*/
	/******************************************/
	asprintf(&tmp_catalog, "%s%s%s/",  tmp_dir, tmp_file, tmpfilename);
	if(tmp_catalog == NULL)
	{
		twfun_mark_error_in_transaction(_GLOBAL_twg_transaction_dir_name, NO_MEMORY);
		SOAP_FOK(NO_MEMORY);
	}
	PRINT_VDEBUG("SOAPSERVERERR tmp_catalog: %s\n", tmp_catalog);

	asprintf(&tmp_location, "%s%s%s/%s_%s", tmp_dir, tmp_file, tmpfilename,tmpfilename,count);
	if(tmp_location == NULL)
	{
		twfun_mark_error_in_transaction(_GLOBAL_twg_transaction_dir_name, NO_MEMORY);
		SOAP_FOK(NO_MEMORY);
	}
	PRINT_VDEBUG("SOAPSERVERERR tmp_location: %s\n", tmp_location);

	asprintf(&tmp_location_lock, "%s%s%s/%s_%s.lock", tmp_dir, tmp_file, tmpfilename,tmpfilename,count);
	if(tmp_location_lock == NULL)
	{
		twfun_mark_error_in_transaction(_GLOBAL_twg_transaction_dir_name, NO_MEMORY);
		SOAP_FOK(NO_MEMORY);
	}
	PRINT_VDEBUG("SOAPSERVERERR tmp_location_lock: %s\n", tmp_location_lock);

	if( (file=open(tmp_catalog, O_RDONLY)) < 0)
	{
		twfun_mark_error_in_transaction(_GLOBAL_twg_transaction_dir_name, BMDSOAP_SERVER_TMP_DIR_OPEN_ERROR);
		SOAP_FOK(BMDSOAP_SERVER_TMP_DIR_OPEN_ERROR);
	}
	if(close(file) < 0)
	{
		twfun_mark_error_in_transaction(_GLOBAL_twg_transaction_dir_name, BMDSOAP_SERVER_TMP_DIR_OPEN_ERROR);
		SOAP_FOK(BMDSOAP_SERVER_TMP_DIR_OPEN_ERROR);
	}
	file = 0;

	/************************************************************/
	/*	zapisanie przeslanych danych w katalogu tymczasowym	*/
	/************************************************************/
	if (input->file->__size>0)
	{
		/************************************/
		/*	założenie lock'a na plik	*/
		/************************************/
		i=0;
		while(1)
		{
			if ((file2=open(tmp_location_lock, O_WRONLY|O_CREAT, S_IRWXU|S_IRWXG|S_IRWXO))>0)
			{
				close(file2);
				break;
			}
			else
			{
				sleep(1);
				i++;
				if (i>=stream_timeout)
			{
					break;
				}
			}
		}

		/************************************/
		/*	otwieranie pliku do zapisu	*/
		/************************************/
		i=0;
		while(1)
		{
			if ((file=open(tmp_location, O_WRONLY|O_CREAT, S_IRWXU|S_IRWXG|S_IRWXO))>0)
			{
				break;
			}
			sleep(1);
			i++;
			if (i>=stream_timeout)
			{
				break;
			}
		}

		/************************/
		/*	zapis do pliku	*/
		/************************/
		if (file<0)
		{
			twfun_mark_error_in_transaction(_GLOBAL_twg_transaction_dir_name, BMDSOAP_SERVER_INTERNAL_ERROR);
			PRINT_ERROR("SOAPSERVERERR %s\n%s\n","Error in writing data to temporary file for chunk soap "
			"transmision",tmp_location);

			perror("error ");
			return soap_receiver_fault(soap,"Soap server error",
			"Error in writing data to temporary file for chunk soap transmision");
		}
		else
		{
			if (file2<0)
			{
				twfun_mark_error_in_transaction(_GLOBAL_twg_transaction_dir_name, BMDSOAP_SERVER_INTERNAL_ERROR);
				PRINT_ERROR("SOAPSERVERERR %s %s\n","Error in setting lock file for chunk soap transmision",
				tmp_location_lock);

				perror("error ");
				return soap_receiver_fault(soap,"Soap server error",
				"Error in setting lock file for chunk soap transmision");
			}

			if (write(file, (const void *)input->file->__ptr, input->file->__size)!=input->file->__size)
			{
				twfun_mark_error_in_transaction(_GLOBAL_twg_transaction_dir_name, BMDSOAP_SERVER_INTERNAL_ERROR);
				PRINT_ERROR("SOAPSERVERERR %s\n%s\n","Error in writing data to temporary file for chunk "
				"soap transmision",tmp_location);
				perror("error ");
				close(file);
				return soap_receiver_fault(soap,"Soap server error",
				"Error in writing data to temporary file for chunk soap transmision");
			}
			close(file);

			/************************/
			/*	usuniecie locka	*/
			/************************/
			unlink(tmp_location_lock);
		}
	}

	/************************************/
	/*	ustawienie wyniku operacji	*/
	/************************************/
	*result=0;

	/******************/
	/*	porzadki	*/
	/******************/
	free(tmp_catalog); tmp_catalog = NULL;
	free(filename); filename=NULL;
	free(tmp_dir); tmp_dir=NULL;
	free(tmp_location); tmp_location=NULL;


	PRINT_SOAP("Status:\t\trequest served correctly\n");
	return BMD_OK;
}


/********************************************************/
/* rozpoczecie transakcji - pobranie jej identyfikatora */
/********************************************************/
int bmd230__bmdStartTransaction(	struct soap *soap,
				struct xsd__base64Binary *cert,
				char *roleName,
				char *groupName,
				char *userClassId,
				char **transactionId)
{
long rand_num				= 0;
long deserialisation_max_memory		= 0;
bmd_info_t *bi				= NULL;
char *tmp_dir				= NULL;
char *transaction_dir_name		= NULL;
struct timeval now;
struct tm czas;

char *derUserCert=NULL;
long derUserCertLen=0;
long retVal=0;
char* paramCert=NULL;
long paramCertLen=0;

	PRINT_SOAP("Request:\topen file transaction\n");

	/************************************/
	/*	logowanie do serwera BMD	*/
	/************************************/
	SOAP_FOK(bmdconf_get_value(_GLOBAL_konfiguracja,"soap","temp_dir",&tmp_dir));
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
			roleName, groupName, deserialisation_max_memory));
	free(derUserCert); derUserCert=NULL;
	paramCert=NULL; paramCertLen=0;

	/******************************/
	/*	wylogowanie z serwera	*/
	/******************************/
	SOAP_FOK(bmd_logout(&bi, deserialisation_max_memory));
	SOAP_FOK(bmd_end());

	/************************************/
	/*	pobranie aktualnego czasu	*/
	/************************************/
	gettimeofday(&now,NULL);
	localtime_r(&(now.tv_sec),&czas);

	/************************************/
	/*	wygenerowanie liczby losowej	*/
	/************************************/
	srand(time(NULL));
	rand_num=1+(long)(100000.0*rand()/(RAND_MAX+1.0));


	asprintf(transactionId, "%02i%02i%02i%02i%02i%02i%li%li",
				czas.tm_year+1900,
				czas.tm_mon+1,
				czas.tm_mday,
				czas.tm_hour,
				czas.tm_min,
				czas.tm_sec,
				rand_num,(long)getpid());

	if(*transactionId == NULL)	{	SOAP_FOK(NO_MEMORY);		}

	asprintf(&transaction_dir_name, "%ssoap_transaction_id_%s", tmp_dir, *transactionId);
	if(transaction_dir_name == NULL)	{	SOAP_FOK(NO_MEMORY);		};

	SOAP_FOK(twfun_make_transaction_directory(transaction_dir_name, 1));

	free(transaction_dir_name); transaction_dir_name = NULL;

	PRINT_SOAP("Status:\t\trequest served correctly\n");
	return BMD_OK;
}


int bmd230__bmdStopTransaction(	struct soap *soap,
				struct xsd__base64Binary *cert,
				char *roleName,
				char *groupName,
				char *userClassId,
				char *transactionId,
				int *result)
{
long twl_array_elements				= 0;
long lock_file_exists				= 0;
long i						= 0;
long err					= 0;
long transaction_timeout			= 0;
long max_datagram_in_set_transmission		= 0;
long deserialisation_max_memory			= 0;
long empty_transaction_impossible		= 0;
long twl_transmitted_file_exists		= 0;
long start_transaction_required			= 0;
long stop_transaction_check_proc		= 0;
long twl_transmission_pid			= 0;
bmdDatagramSet_t *dtg_set			= NULL;
bmdDatagramSet_t *resp_dtg_set			= NULL;
bmd_info_t *bi					= NULL;
char tmp_file[]					= {"soap_insert_chunk_"};
char pid_file[]					= {"pid_"};
DIR *ptr					= NULL;
DIR *ptr2					= NULL;
DIR *ptr_pid					= NULL;
DIR *twl_transaction_dir_descriptor		= NULL;
struct dirent *dirp				= NULL;
struct dirent *dirp2				= NULL;
char *tmp_dir					= NULL;
char *tmp_dir2					= NULL;
char *transaction_id				= NULL;
char *twl_transaction_dir_name			= NULL;
char **twl_array_string				= NULL;
char *proc_dir					= NULL;
char *full_proc_dir				= NULL;

char *derUserCert=NULL;
long derUserCertLen=0;
long retVal=0;
char* paramCert=NULL;
long paramCertLen=0;

	PRINT_SOAP("Request:\tclose file transaction %s\n",transactionId);
	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if (transactionId==NULL)	{	SOAP_FOK(BMDSOAP_SERVER_INVALID_TRANSACTION_ID);	}

	/**************************************/
	/* pobranie ustawien konfiguracyjnych */
	/**************************************/
	bmdconf_get_value_int(_GLOBAL_konfiguracja,"bmd","max_datagram_in_set_transmission", &max_datagram_in_set_transmission);
	SOAP_FOK(bmdconf_get_value(_GLOBAL_konfiguracja,"soap","temp_dir",&tmp_dir));
	SOAP_FOK(bmdconf_get_value_int(_GLOBAL_konfiguracja,"soap","stoptransaction_timeout",&transaction_timeout));

	bmdconf_get_value_int(_GLOBAL_konfiguracja, "bmd", "deserialisation_max_memory", &deserialisation_max_memory);
	if(deserialisation_max_memory < 0)
	{
		deserialisation_max_memory = 0;
	}
	else
	{
		deserialisation_max_memory = deserialisation_max_memory * 1024 * 1024;
	}

	bmdconf_get_value_int(_GLOBAL_konfiguracja,"soap","empty_transaction_impossible",&empty_transaction_impossible);
	if(empty_transaction_impossible < 0)
	{
		empty_transaction_impossible = 0;
	}
	PRINT_VDEBUG("SOAPSERVERVDEBUG empty_transaction_impossible: %li\n", empty_transaction_impossible);

	bmdconf_get_value_int(_GLOBAL_konfiguracja,"soap","start_transaction_required",&start_transaction_required);
	if(start_transaction_required < 0)
	{
		start_transaction_required = 0;
	}

	bmdconf_get_value_int(_GLOBAL_konfiguracja,"soap","stop_transaction_check_proc",&stop_transaction_check_proc);
	if(stop_transaction_check_proc < 0)
	{
		stop_transaction_check_proc = 1;
	}

	bmdconf_get_value(_GLOBAL_konfiguracja,"soap","proc_dir",&proc_dir);
	if(proc_dir == NULL)
	{
		asprintf(&proc_dir,"/proc/");
		if(proc_dir == NULL)				{	BMD_FOK(NO_MEMORY);	}
	}

	/*Sprawdzam czy jest jakas transakcja w ogole otwarta*/
	asprintf(&twl_transaction_dir_name, "%ssoap_transaction_id_%s", tmp_dir, transactionId);
	if(twl_transaction_dir_name == NULL)			{	BMD_FOK(NO_MEMORY);		};

	twl_transaction_dir_descriptor = opendir(twl_transaction_dir_name);
	if(twl_transaction_dir_descriptor == NULL)
	{
		SOAP_FOK(BMDSOAP_SERVER_NO_TRANSACTION_STARTED);
	}


	/*****************************************************************/
	/* poszukiwanie katalogow tymczasowych dla poszczegolnych plikow */
	/*****************************************************************/
	asprintf(&transaction_id, "transaction_id_%s", transactionId);
	ptr=opendir(tmp_dir);
	if (ptr==NULL)	{	SOAP_FOK(BMDSOAP_SERVER_OPENDIR_ERROR);	}
	while((dirp = readdir(ptr))!=NULL)
	{
		if ((strcmp(dirp->d_name,"..")==0) || (strcmp(dirp->d_name,".")==0)) continue;

		if (strncmp(dirp->d_name, tmp_file, strlen(tmp_file))==0)
		{
			asprintf(&tmp_dir2, "%s/%s", tmp_dir, dirp->d_name);

			while(1)
			{/*while(1)*/
				ptr2=opendir(tmp_dir2);
				if (ptr2!=NULL)
				{
					while((dirp2 = readdir(ptr2))!=NULL)
					{
						PRINT_VDEBUG("SOAPSERVERVDEBUG Readed position: %s\n", dirp2->d_name);

						if ((strcmp(dirp2->d_name,"..")==0) || (strcmp(dirp2->d_name,".")==0))
						{
							continue;
						}

						if (strncmp(dirp2->d_name, pid_file, strlen(pid_file)) == 0)
						{
							bmd_strsep(dirp2->d_name,'_', &twl_array_string,&twl_array_elements);
							if(twl_array_elements != 2)
							{
								SOAP_FOK(BMDSOAP_SERVER_INTERNAL_ERROR);
							}
							twl_transmission_pid = atol(twl_array_string[1]);
							PRINT_VDEBUG("SOAPSERVERVDEBUG Transmission proces pid %s\n",
							twl_array_string[1]);
							bmd_strsep_destroy(&twl_array_string, twl_array_elements);
						}
					}
					closedir(ptr2); ptr2 = NULL;
				}
				else
				{
					PRINT_VDEBUG("SOAPSERVERVDEBUG No directory %s", tmp_dir2);
					break;
				}

				if(twl_transmission_pid == 0)
				{
					SOAP_FOK(BMDSOAP_SERVER_PID_FILE_ERROR);
				}

				ptr2=opendir(tmp_dir2);
				if (ptr2!=NULL)
				{
					while((dirp2 = readdir(ptr2))!=NULL)
					{
						if ((strcmp(dirp2->d_name,"..")==0) || (strcmp(dirp2->d_name,".")==0))
						{
							continue;
						}

						if (strcmp(dirp2->d_name, transaction_id)==0)
						{
							if(stop_transaction_check_proc == 1)
							{
								asprintf(&full_proc_dir, "%s%li", proc_dir,
								twl_transmission_pid);
								if(full_proc_dir == NULL)	{BMD_FOK(NO_MEMORY);	}
								PRINT_VDEBUG("SOAPSERVERVDEBUG full_proc_dir: %s\n",
								full_proc_dir);
								ptr_pid = opendir(full_proc_dir);
								free(full_proc_dir); full_proc_dir = NULL;

								if(ptr_pid == NULL)
								{
				PRINT_ERROR("SOAPSERVERERR No transmission process of pid %li\n", twl_transmission_pid);
									SOAP_FOK(BMDSOAP_SERVER_TRANSMISSION_BROKEN);
								}
								else
								{
									closedir(ptr_pid);
									ptr_pid = NULL;
								}
							}


							lock_file_exists = 1;
							if(i >= transaction_timeout)
							{
								closedir(ptr2);
								closedir(ptr);
								if(transaction_timeout > 0)
								{
									PRINT_ERROR("SOAPSERVERERR Transaction timeout "
									"%li\n", i);
								}
								SOAP_FOK(BMDSOAP_SERVER_LOBS_NOT_FINISHED_TRANSACTION);
							}
							else
							{
								PRINT_VDEBUG("SOAPSERVERVDEBUG Waiting for transaction "
								"timeout %li\n", i);
								sleep(1);
								i++;
								break;
							}
						}
					}
					closedir(ptr2);
					if(lock_file_exists == 0)
					{
						break;
					}
					else
					{
						lock_file_exists = 0;
					}
				}
				else
				{
					break;
				}
			}/*\while(1)*/
			free(tmp_dir2); tmp_dir2=NULL;

		}
	}
	closedir(ptr);

	free(transaction_id); transaction_id=NULL;

	lock_file_exists = 0;
	while(1)
	{
		ptr=opendir(twl_transaction_dir_name);
		if(ptr==NULL)		{	SOAP_FOK(BMDSOAP_SERVER_OPENDIR_ERROR);		}

		while((dirp = readdir(ptr))!=NULL)
		{/*while((dirp = readdir(ptr))!=NULL)*/
			PRINT_VDEBUG("SOAPSERVERVDEBUG first dirp->d_name: %s\n", dirp->d_name);

			if ((strcmp(dirp->d_name,"..")==0) || (strcmp(dirp->d_name,".")==0)) continue;

			if ( strncmp(dirp->d_name, "error_", strlen("error_")) == 0 )
			{
				/*Error in inserting file occured*/
				bmd_strsep(dirp->d_name, '_', &twl_array_string, &twl_array_elements);
				if(twl_array_elements != 3)
				{
					SOAP_FOK(BMDSOAP_SERVER_INTERNAL_ERROR);
				}
				err = -1 * atol(twl_array_string[1]);
				bmd_strsep_destroy(&twl_array_string, twl_array_elements);

				if(err >= 0)
				{
					SOAP_FOK(BMDSOAP_SERVER_INTERNAL_ERROR);
				}
				PRINT_VDEBUG("twl_transaction_dir_name %s\n", twl_transaction_dir_name);
				_GLOBAL_twg_transaction_dir_name = twl_transaction_dir_name;
				SOAP_FOK_TRANSACTION_INSERT(err);
			}
		}
		closedir(ptr);

		ptr=opendir(twl_transaction_dir_name);
		if(ptr==NULL)		{	SOAP_FOK(BMDSOAP_SERVER_OPENDIR_ERROR);		}

		while((dirp = readdir(ptr))!=NULL)
		{/*while((dirp = readdir(ptr))!=NULL)*/
			PRINT_VDEBUG("SOAPSERVERVDEBUG second dirp->d_name: %s\n", dirp->d_name);
			if ( strncmp(dirp->d_name, "insert_", strlen("insert_")) == 0 )
			{
				lock_file_exists = 1;
				/*Transmission in progress*/
				if( i >= transaction_timeout)
				{/*if( i >= transaction_timeout)*/
					if(transaction_timeout > 0)
					{
						PRINT_ERROR("SOAPSERVERERR Transaction timeout %li\n", i);
					}
					closedir(ptr);
					SOAP_FOK(BMDSOAP_SERVER_LOBS_NOT_FINISHED_TRANSACTION);
				}/*\if( i >= transaction_timeout)*/
				else
				{
					PRINT_VDEBUG("SOAPSERVERVDEBUG Waiting for transaction timeout %li\n", i);
					sleep(1);
					i++;
					break;
				}
			}

			if( strncmp(dirp->d_name, "transmitted_", strlen("transmitted_")) == 0 )
			{
				twl_transmitted_file_exists = 1;
			}
		}/*\while((dirp = readdir(ptr))!=NULL)*/

		PRINT_VDEBUG("SOAPSERVERVDEBUG twl_transmitted_file_exists: %li empty_transaction_impossible: %li\n",
		twl_transmitted_file_exists, empty_transaction_impossible);

		closedir(ptr);
		if(lock_file_exists == 0)
		{
			if(twl_transmitted_file_exists == 0 && empty_transaction_impossible == 0)
			{
				remove_dir(twl_transaction_dir_name);
				SOAP_FOK(BMDSOAP_CLOSING_EMPTY_TRANSACTION);
			}
			break;
		}
		else
		{
			lock_file_exists = 0;
			twl_transmitted_file_exists = 0;
		}
	}


	if(max_datagram_in_set_transmission <= 0)
	{
		max_datagram_in_set_transmission = 100;
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

	/************************************/
	/*	logowanie do serwera BMD	*/
	/************************************/
	SOAP_FOK(PrepareDataForBMDLoginAndRequest(&bi,
			(_GLOBAL_soap_wssecurity_enabled == 1  ? derUserCert : paramCert),
			(_GLOBAL_soap_wssecurity_enabled == 1  ? derUserCertLen : paramCertLen),
			roleName, groupName, deserialisation_max_memory));
	free(derUserCert); derUserCert=NULL;
	paramCert=NULL; paramCertLen=0;

	/*while (1)
	{*/
	/******************************************************/
	/*	stworzenie datagramu dla zakonczenia transakcji	*/
	/******************************************************/
	SOAP_FOK_LOGOUT(createDatagram(	userClassId,
					(char *[]){OID_SYS_METADATA_CRYPTO_OBJECTS_VISIBLE,
					OID_SYS_METADATA_CRYPTO_OBJECTS_TRANSACTION_ID},
					(char *[]){"0",transactionId},
					2,
					BMD_DATAGRAM_TYPE_END_TRANSACTION,
					BMD_ONLY_GROUP_DOC,
					BMD_QUERY_EQUAL,
					&dtg_set));

	/******************************************/
	/*	update metadanych plikow w archiwum	*/
	/******************************************/
	err=sendRequestToBMD(	bi, dtg_set, max_datagram_in_set_transmission, &resp_dtg_set, 1, deserialisation_max_memory);
	if (err==BMD_OK)
	{
		err=resp_dtg_set->bmdDatagramSetTable[0]->datagramStatus;
		/*
		if (err==BMD_OK)
		{
			break;
		}
		*/
	}

	bmd_datagramset_free(&dtg_set);
	bmd_datagramset_free(&resp_dtg_set);
	/*
	if (i>=transaction_timeout)
	{
			if(transaction_timeout > 0)
			{
				PRINT_ERROR("SOAPSERVERERR Transaction timeout %li\n", i);
			}
			err=BMDSOAP_SERVER_TRANSACTION_TIMEOUT;
			break;
		}
		i++;
		sleep(1);
	}*/

	/************/
	/* porzadki */
	/************/
	free(transaction_id); transaction_id=NULL;
	free(tmp_dir2); tmp_dir2=NULL;

	/******************************/
	/*	wylogowanie z serwera	*/
	/******************************/
	SOAP_FOK(bmd_logout(&bi, deserialisation_max_memory));
	SOAP_FOK(bmd_end());
	(*result)=err;

	/*Pozbycie sie markera transakcji*/
	if(err >= BMD_OK)
	{
		remove_dir(twl_transaction_dir_name);
		PRINT_VDEBUG("SOAPSERVERVDEBUG Removed transaction directory %s\n", twl_transaction_dir_name);
	}
	else
	{
		if(twl_transaction_dir_name != NULL)
		{
			PRINT_ERROR("Error in stopping Transaction. Please remove %s manually. "
			"Error from server: %s (%li) \n", twl_transaction_dir_name, GetErrorMsg(err),err);
		}
		else
		{
			PRINT_ERROR("Error in stopping Transaction. No transaction directory. "
			"Error from server: %s (%li) \n", GetErrorMsg(err),err);
		}
	}
	SOAP_FOK(err);

	free(twl_transaction_dir_name); twl_transaction_dir_name = NULL;

	PRINT_SOAP("Status:\t\trequest served correctly\n");
	return BMD_OK;
}
