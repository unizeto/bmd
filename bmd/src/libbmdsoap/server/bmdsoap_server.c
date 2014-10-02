#include <bmd/libbmdsoap/libbmdsoap.h>
#include <bmd/libbmdglobals/libbmdglobals.h>
#include <conf_sections.h>

//starszy kompilator gSOAP wygenerowalby nazwe "bmd230.nsmap"
#include "bmd230.nsmap"
#include "wsseapi.h"


/******************************/
/*	Obsluga zadania klienta	*/
/******************************/
void *server_function(void *data)
{
struct soap *soap		= NULL;
long status			= 0;


	soap=(struct soap *)data;
	X509 *peer_cert=NULL;
	peer_cert=SSL_get_peer_certificate(soap->ssl);
	if(peer_cert!=NULL)
	{
		char issuer[256];
		char subject[256];
		X509_NAME_oneline(X509_get_issuer_name(peer_cert), issuer, 256);
		PRINT_NOTICE("SOAPSERVERNOTIC Client certificate's issuer: %s\n",issuer);
		X509_NAME_oneline(X509_get_subject_name(peer_cert), subject, 256);
		PRINT_NOTICE("SOAPSERVERNOTIC Client certificate's subject: %s\n",subject);
	}

	if(_GLOBAL_soap_wssecurity_enabled == 1)
	{
		soap_register_plugin(soap, soap_wsse);
		//soap_wsse_verify_auto(soap,  SOAP_SMD_VRFY_RSA_SHA1, NULL, 0);
		//soap_wsse_verify_auto(soap,  SOAP_SMD_NONE, NULL, 0);
	}
	
	status=bmd230_serve(soap);
	if(status!=SOAP_OK)
	{
		/*soap_print_fault(soap,stdout);*/
	}
	PRINT_SOAP("Connection:\tclosed\n");
	printf("\n");

	return NULL;
}

/************************************************************/
/*	Obsluga sygnalu SIGCHLD - eliminowanie procesow zombi	*/
/************************************************************/
void sigchld_handler(int id)
{
	long pid;

	if(_GLOBAL_twg_parent_pid == getpid())
	{

		while ((pid = waitpid (-1, &id, WNOHANG)) > 0)
		{
			/*rocesy strumieniowe zwracaja status 1*/
			if(id != 1)
			{
				_GLOBAL_children_number--;
				PRINT_VDEBUG("SOAPSERVERVDEBUG Children number decrased to: %li\n", _GLOBAL_children_number);
			}
			else
			{
				PRINT_VDEBUG("SOAPSERVERVDEBUG Stream child. Normal children number: %li\n",_GLOBAL_children_number);
			}
		}
	}
	else
	{
		if(_GLOBAL_twg_child_own_pid == 0 || _GLOBAL_twg_child_own_pid == getpid())
		{
			PRINT_VDEBUG("SOAPSERVERVDEBUG Sigchld in normal child catched");
			while ((pid = waitpid (-1, &id, WNOHANG)) > 0);
		}
		else
		{
			PRINT_VDEBUG("BMDSERVERVDEBUG Sigchld in stream child catched");
		}
	}
}


/******************************************/
/*      Obslugha sygnalu SIGINT i SIGTERM       */
/******************************************/
void master_proc_sig(int status)
{
long last_children_number	= 0;
long clean_orphan_directories	= 0;
char *tmp_dir			= NULL;
char *full_name			= NULL;
DIR *ptr			= NULL;
struct dirent *dirp		= NULL;

	bmdconf_get_value(_GLOBAL_konfiguracja,"soap","temp_dir",&tmp_dir);
	bmdconf_get_value_int(_GLOBAL_konfiguracja,"soap","clean_orphan_directories",&clean_orphan_directories);
	if(clean_orphan_directories < 0)
	{
		clean_orphan_directories = 1;
	}

	if(_GLOBAL_twg_parent_pid == getpid())
	{
		PRINT_INFO("SOAPSERVERINF Finishing soap server program work... %i\n",status);

		kill(0,SIGUSR);
		while (_GLOBAL_children_number>0)
		{
			last_children_number = _GLOBAL_children_number;
			PRINT_NOTICE("SOAPSERVERNOTICE Waiting for all children to die %li\n",_GLOBAL_children_number);
			sleep(1);
			if(last_children_number == _GLOBAL_children_number)
			{
				PRINT_WARNING("SOAPSERVERWARNING Ignoring children number\n");
				break;
			}
		}
		PRINT_NOTICE("SOAPSERVERNOTICE All children died\n");

		PRINT_LOG("SOAPSERVERLOG Shutting down soap server\n");
	}
	else
	{
		if(_GLOBAL_twg_child_own_pid == 0 || _GLOBAL_twg_child_own_pid == getpid())
		{
			PRINT_VDEBUG("SOAPSERVERVDEBUG Sigterm in child catched");
		}
		else
		{
			PRINT_VDEBUG("SOAPSERVERVDEBUG Sigterm in stream child catched");
			exit(1);
		}
	}

	/*Teraz szukam nie usunietych katalogow transmisji i transakcji.*/
	if(clean_orphan_directories == 1)
	{/*if(clean_orphan_directories == 1)*/
		PRINT_VDEBUG("SOAPSERVERVDEBUG Cleaning orphan directories enabled.\n");
		if(tmp_dir != NULL)
		{
			ptr=opendir(tmp_dir);
			if (ptr==NULL)	{PRINT_ERROR("SOAPSERVERERR Error in opening temporary directory %s\n", tmp_dir);}
			else
			{/*else ptr==NULL*/
				while((dirp = readdir(ptr))!=NULL)
				{
					if ((strcmp(dirp->d_name,"..")==0) || (strcmp(dirp->d_name,".")==0)) continue;

					if (strncmp(dirp->d_name, "soap_transaction_id_", strlen("soap_transaction_id_"))==0)
					{
						asprintf(&full_name,"%s%s",tmp_dir, dirp->d_name);
						if(full_name == NULL)	{PRINT_ERROR("SOAPSERVERERR No memory\n");}
						else
						{
							PRINT_VDEBUG("SOAPSERVERVDEBUG Orphan directory found %s\n",
							full_name);
							remove_dir(full_name);
							free(full_name); full_name = NULL;
						}
					}

					if (strncmp(dirp->d_name, "soap_insert_chunk_", strlen("soap_insert_chunk_"))==0)
					{
						asprintf(&full_name,"%s%s",tmp_dir, dirp->d_name);
						if(full_name == NULL)   {PRINT_ERROR("SOAPSERVERERR No memory\n");}
						else
						{
							PRINT_VDEBUG("SOAPSERVERVDEBUG Orphan directory found %s\n",
							full_name);
							remove_dir(full_name);
							free(full_name); full_name = NULL;
						}
					}

					if (strncmp(dirp->d_name, "soap_get_chunk_", strlen("soap_get_chunk_"))==0)
					{
						asprintf(&full_name,"%s%s",tmp_dir, dirp->d_name);
						if(full_name == NULL)   {PRINT_ERROR("SOAPSERVERERR No memory\n");}
						else
						{
							PRINT_VDEBUG("SOAPSERVERVDEBUG Orphan directory found %s\n",
							full_name);
							remove_dir(full_name);
							free(full_name); full_name = NULL;
						}
					}

					if (strncmp(dirp->d_name, "soap_insert_lock_", strlen("soap_insert_lock_"))==0)
					{
						asprintf(&full_name,"%s%s",tmp_dir, dirp->d_name);
						if(full_name == NULL)   {PRINT_ERROR("SOAPSERVERERR No memory\n");}
						else
						{
							PRINT_VDEBUG("SOAPSERVERVDEBUG Orphan lock file found %s\n",
							full_name);
							unlink(full_name);
							free(full_name); full_name = NULL;
						}
					}

					if (strncmp(dirp->d_name, "soap_get_lock_", strlen("soap_get_lock_"))==0)
					{
						asprintf(&full_name,"%s%s",tmp_dir, dirp->d_name);
						if(full_name == NULL)   {PRINT_ERROR("SOAPSERVERERR No memory\n");}
						else
						{
							PRINT_VDEBUG("SOAPSERVERVDEBUG Orphan lock file found %s\n",
							full_name);
							unlink(full_name);
							free(full_name); full_name = NULL;
						}
					}
				}
				closedir(ptr);
			}/*\else ptr==NULL*/
		}
		else
		{
			PRINT_ERROR("SOAPSERVERERR Null temporary directory.\n");
		}
	}/*\if(clean_orphan_directories == 1)*/
	else
	{/*else(clean_orphan_directories == 1)*/
		PRINT_VDEBUG("SOAPSERVERVDEBUG Cleaning orphan directories disabled.\n");
	}/*\else(clean_orphan_directories == 1)*/

	exit(0);
}

/********************************************************/
/*      Obslugha sygnalu SIGUSR zabijajacego dzieci     */
/********************************************************/
void kill_me(int status)
{
	if(_GLOBAL_twg_child_own_pid == 0 || _GLOBAL_twg_child_own_pid == getpid())
	{
		PRINT_VDEBUG("SOAPSERVERVDEBUG kill_me handler in child catched")
		exit(0);
	}
	else
	{
		PRINT_VDEBUG("BMDSERVERVDEBUG kill_me handler in stream child catched")
		exit(1);
	}
}

/******************************************************/
/*	Zaladowanie ustawień konfiguracynjych serwera	*/
/******************************************************/
long load_soap_configuration(char *path)
{
char *error_string	= NULL;
long status			= 0;

	PRINT_SOAP("Server:\tloading configuration settings\n");
	BMD_FOK(bmdconf_init(&_GLOBAL_konfiguracja));
	BMD_FOK(bmdconf_add_section2dict(_GLOBAL_konfiguracja,"bmd",ws_DICT_BMD,BMDCONF_SECTION_REQ));
	BMD_FOK(bmdconf_add_section2dict(_GLOBAL_konfiguracja,"soap",ws_DICT_SOAP,BMDCONF_SECTION_REQ));
	BMD_FOK(bmdconf_add_section2dict(_GLOBAL_konfiguracja,"timestamp",ws_DICT_TIMESTAMP,BMDCONF_SECTION_REQ));
	BMD_FOK(bmdconf_add_section2dict(_GLOBAL_konfiguracja,"convert",ws_DICT_CONVERT,BMDCONF_SECTION_OPTIONAL));
	BMD_FOK(bmdconf_add_section2dict(_GLOBAL_konfiguracja,"zip_package",ws_DICT_ZIP_PACKAGE,BMDCONF_SECTION_REQ));
	BMD_FOK(bmdconf_add_section2dict(_GLOBAL_konfiguracja,"csv",ws_DICT_CSV,BMDCONF_SECTION_OPTIONAL));
	BMD_FOK(bmdconf_add_section2dict(_GLOBAL_konfiguracja,"ssl",ws_DICT_SSL,BMDCONF_SECTION_REQ));
	BMD_FOK(bmdconf_add_section2dict(_GLOBAL_konfiguracja,"wssecurity",ws_DICT_WSSECURITY,BMDCONF_SECTION_OPTIONAL));
	BMD_FOK(bmdconf_add_section2dict(_GLOBAL_konfiguracja,"functionalities_behaviour",ws_DICT_FUNCTIONALITIES_BEHAVIOUR,BMDCONF_SECTION_OPTIONAL));

	PRINT_SOAP("server:\tloading configuration from file %s\n",path);
	status=bmdconf_load_file(path,_GLOBAL_konfiguracja,&error_string);
	if (status<BMD_OK)
	{
		PRINT_ERROR("%s\n", error_string);
		return status;
	}

	return BMD_OK;
}

/******************************/
/*	Główna funkcja serwera	*/
/******************************/
int main(int argc, char **argv)
{
_GLOBAL_debug_level		= 0;
long i				= 0;
long port			= 0;
long lowLevelErrorCode		= 0;
// long ssl			= 0;
long soap_accept_timeout	= 0;
long soap_send_timeout		= 0;
long soap_recv_timeout		= 0;
long max_connections		= 0;
// long status			= 0;
long m				= 0;
long s				= 0;

char *host			= NULL;
// char *keyfile			= NULL;
// char *keyfile_passwd		= NULL;
// char *capath			= NULL;
struct soap *psoap		= NULL;

pid_t pid;
struct soap soap;
char path[100];

long enable_ssl			= 0;
long enable_ssl_autentication	= 0;
char *ssl_certificate		= NULL;
char *ssl_certificate_password	= NULL;
char *ssl_cacert_file		= NULL;
char *ssl_capath_dir		= NULL;

char *wsse_ca_path		= NULL;

long retVal			= 0;

	memset(&soap, 0 , sizeof(struct soap));
	
	/*zapamietuje pid glownego serwera.*/
	_GLOBAL_twg_parent_pid = getpid();

	/************************************************/
	/*	obsluga sygnalu od procesow potomnych	*/
	/************************************************/
	if(signal(SIGUSR, SIG_IGN) == SIG_ERR)
	{
		PRINT_FATAL("SOAPSERVERFATAL Error in signal setting - SIGUSR!\n");
		return 0;
	}

	if(signal(SIGCHLD, sigchld_handler) == SIG_ERR)
	{
		PRINT_FATAL("SOAPSERVERFATAL Error in signal setting - SIGCHLD!\n");
		return 0;
	}

	if(signal(SIGTERM, master_proc_sig) == SIG_ERR)
	{
		PRINT_FATAL("SOAPSERVERFATAL Error in signal setting - SIGTERM!\n");
		return 0;
	}

	if(signal(SIGINT, master_proc_sig) == SIG_ERR)
	{
		PRINT_FATAL("SOAPSERVERFATAL Error in signal setting - SIGINT!\n");
		return 0;
	}


	/****************************************/
	/*	parsoawnie linii polecen	*/
	/****************************************/
	if (argc<2)
	{
		printf("****** SOAP SERVER for archive ver. 2.3 - Unizeto Technologies SA ******* \n");
		printf("*                                                                       *\n");
		printf("*    -c [configuration file path] default: conf/soap_server.conf        *\n");
		printf("*                                                                       *\n");
		printf("*************************************************************************\n");
		return 0;
	}
	else
	{
		strcpy(path,"");
		for (i=1; i<argc; i++)
		{
			if ((strcmp(argv[i],"-h")==0) || (strcmp(argv[i],"--help")==0))
			{
				printf("****** SOAP SERVER for archive ver. 2.3 - Unizeto Technologies SA ******* \n");
				printf("*                                                                       *\n");
				printf("*    -c [configuration file path] default: conf/soap_server.conf        *\n");
				printf("*                                                                       *\n");
				printf("*************************************************************************\n");
				return 0;
			}
			if (strcmp(argv[i],"-d")==0)
			{
				if (argc-2<i)
				{
					printf("\nInvalid parameter\n\n");
					return -1;
				}
				else
				{
					_GLOBAL_debug_level=atoi(argv[i+1]);
					i++;
				}
			}

			if (strcmp(argv[i],"-c")==0)
			{
				if (argc>i+1)
				{
					strcpy(path,argv[i+1]);
				}
				else
				{
					PRINT_SOAP("Error:\tno path to configuration file\n");
					return 0;
				}
			}
		}
	}
	if (strcmp(path,"")==0)
	{
		strcpy(path,"conf/soap_server.conf");
	}

	/************************************************/
	/*	wczytanie ustawien konfiguracyjnych	*/
	/************************************************/
	BMD_FOK(load_soap_configuration(path));
	BMD_FOK_CHG(bmdconf_get_value_int(_GLOBAL_konfiguracja,"soap","max_connections",&max_connections),BMDSOAP_SERVER_CONF_READ_ERROR);
	BMD_FOK_CHG(bmdconf_get_value_int(_GLOBAL_konfiguracja,"soap","accept_timeout",&soap_accept_timeout),BMDSOAP_SERVER_CONF_READ_ERROR);
	BMD_FOK_CHG(bmdconf_get_value_int(_GLOBAL_konfiguracja,"soap","send_timeout",&soap_send_timeout),BMDSOAP_SERVER_CONF_READ_ERROR);
	BMD_FOK_CHG(bmdconf_get_value_int(_GLOBAL_konfiguracja,"soap","recv_timeout",&soap_recv_timeout),BMDSOAP_SERVER_CONF_READ_ERROR);
	BMD_FOK_CHG(bmdconf_get_value(_GLOBAL_konfiguracja,"soap","host",&host),BMDSOAP_SERVER_CONF_READ_ERROR);
	BMD_FOK_CHG(bmdconf_get_value_int(_GLOBAL_konfiguracja,"soap","port",&port),BMDSOAP_SERVER_CONF_READ_ERROR);

	BMD_FOK_CHG(bmdconf_get_value_int(_GLOBAL_konfiguracja,"ssl","enable_ssl",&enable_ssl),BMDSOAP_SERVER_CONF_READ_ERROR);
	BMD_FOK_CHG(bmdconf_get_value_int(_GLOBAL_konfiguracja,"ssl","enable_ssl_authentication",&enable_ssl_autentication),BMDSOAP_SERVER_CONF_READ_ERROR);
	BMD_FOK_CHG(bmdconf_get_value(_GLOBAL_konfiguracja,"ssl","ssl_certificate",&ssl_certificate),BMDSOAP_SERVER_CONF_READ_ERROR);
	BMD_FOK_CHG(bmdconf_get_value(_GLOBAL_konfiguracja,"ssl","ssl_certificate_password",&ssl_certificate_password),BMDSOAP_SERVER_CONF_READ_ERROR);
	bmdconf_get_value(_GLOBAL_konfiguracja,"ssl","ssl_cacert_file",&ssl_cacert_file);
	bmdconf_get_value(_GLOBAL_konfiguracja,"ssl","ssl_capath_dir",&ssl_capath_dir);

	retVal=bmdconf_get_value_int(_GLOBAL_konfiguracja,"wssecurity","enable_wssecurity",&_GLOBAL_soap_wssecurity_enabled);
	if(retVal < 0)
		{ _GLOBAL_soap_wssecurity_enabled = 0; }
	retVal=bmdconf_get_value_int(_GLOBAL_konfiguracja,"soap","certificate_required",&_GLOBAL_soap_certificate_required);
	if(retVal < 0)
		{ _GLOBAL_soap_certificate_required = 1; }

	bmdconf_get_value(_GLOBAL_konfiguracja,"wssecurity","wsse_ca_path",&wsse_ca_path);


	/********************************/
	/*	inicjalizacja ssl'a	*/
	/********************************/
// 	if (enable_ssl==1)
// 	{
// 		PRINT_SOAP("Error:\tssl connections are not supported in this version. Change your configuration options\n");
// 		return -1;
// 	}

	if (enable_ssl==1)
	{
		soap_ssl_init();
	}

	soap_init2(&soap,SOAP_C_UTFSTRING|SOAP_XML_CANONICAL,SOAP_C_UTFSTRING);
	soap_set_namespaces(&soap, bmd230_namespaces);
	soap.bind_flags=SO_REUSEADDR;
	soap.send_timeout=soap_send_timeout;
	soap.recv_timeout=soap_recv_timeout;
	soap.max_keep_alive=max_connections; // max keep-alive sequence

	soap.dime_id_format = "uuid:UNIZETO-%d";
	soap.fdimewriteopen = dime_write_open;
	soap.fdimewriteclose = dime_write_close;
	soap.fdimewrite = dime_write;

	soap.fdimereadopen = dime_read_open;
	soap.fdimereadclose = dime_read_close;
	soap.fdimeread = dime_read;


	if(enable_ssl==1)
	{
		PRINT_LOG("\t\tSOAPSERVERLOG SSL enabled\n");
		PRINT_TEST(">>> PKL <<< enable_ssl: %li\n", enable_ssl);
		PRINT_TEST(">>> PKL <<< enable_ssl_autentication: %li\n", enable_ssl_autentication);
		PRINT_TEST(">>> PKL <<< ssl_certificate: %s\n", ssl_certificate);
		PRINT_TEST(">>> PKL <<< ssl_certificate_password: %s\n", ssl_certificate_password);
		PRINT_TEST(">>> PKL <<< ssl_cacert_file: %s\n", ssl_cacert_file);
		PRINT_TEST(">>> PKL <<< ssl_capath_dir: %s\n", ssl_capath_dir);

		if (soap_ssl_server_context(	&soap,
						enable_ssl_autentication==1?SOAP_SSL_REQUIRE_CLIENT_AUTHENTICATION:SOAP_SSL_DEFAULT,	/* use SOAP_SSL_REQUIRE_CLIENT_AUTHENTICATION to verify clients: client must provide a key file e.g. "client.pem" and "password" */
						ssl_certificate,	/* keyfile: see SSL docs on how to obtain this file */
						ssl_certificate_password,		/* password to read the key file */
						ssl_cacert_file,	/* cacert file to store trusted certificates (to authenticate clients) */
						ssl_capath_dir,		/* capath */
						NULL,	/* DH file, if NULL use RSA */
						NULL,		/* if randfile!=NULL: use a file with random data to seed randomness */
						"bmd_soap_server_unizeto"		/* server identification for SSL session cache (unique server name, e.g. use argv[0]) */
			))
		{
			PRINT_FATAL("SOAPSERVERFATAL Error in setting encryption context\n");
			soap_print_fault(&soap,stdout);
			return -1;
		}
	}
	else
	{
		if(_GLOBAL_soap_wssecurity_enabled == 1)
		{
			soap.cafile=NULL;
			soap.capath=wsse_ca_path;
		}
	}

	/********************************/
	/*	zbindowanie gniazda	*/
	/********************************/
	m=soap_bind(&soap,host,port,100);
	if(m<0)
	{
		lowLevelErrorCode = errno;
		PRINT_FATAL("SOAPSERVERFATAL Error:\t%s\n",GetErrorMsg(BMDSOAP_SERVER_SOCKET_BIND_ERROR));
		PRINT_SOAP("Low level error = %li : <%s>\n", lowLevelErrorCode, strerror(lowLevelErrorCode));
		soap_print_fault(&soap,stdout);
		return BMDSOAP_SERVER_SOCKET_BIND_ERROR;
	}
	else
	{
		PRINT_SOAP("Server:\tsocket binding successful\n");
		PRINT_SOAP("Server:\tserver started\n\n\n");
		for(;;)
		{
			/************************************************/
			/*	oczekiwanie na połaczenie klienta	*/
			/************************************************/
			s = soap_accept(&soap);
			if(s<0)
			{
				lowLevelErrorCode = errno;
				PRINT_SOAP("Error:\terror in accepting connection\n");
				PRINT_SOAP("Low level error = %li : <%s>\n", lowLevelErrorCode, strerror(lowLevelErrorCode));
				soap_print_fault(&soap,stdout);
				continue; // jesli nie zaakceptowal polaczenia, to serwer nie moze sie wylaczyc, tylko powinien nadal nasluchiwac i w miare mozliwosci przyjmowac polaczenia
                	}

			PRINT_SOAP("Connection:\tIP=%li.%li.%li.%li\n",(long)(soap.ip>>24)&0xFF,(long)(soap.ip>>16)&0xFF,
			(long)(soap.ip>>8)&0xFF,(long)soap.ip&0xFF);

			_GLOBAL_twg_child_own_pid = 0;
			pid=fork();
			_GLOBAL_children_number++;
			if(pid==0)
			{

				/************************************************/
				/*	duplikowanie znacznika polaczenia	*/
				/************************************************/
				psoap=soap_copy(&soap);
				if(psoap==NULL)
				{
					PRINT_SOAP("Error:\terror in duplication connection pointer\n");
					break;
				}
				if (enable_ssl==1)
				{
					soap_imode(psoap, SOAP_C_UTFSTRING);
  					soap_omode(psoap, SOAP_C_UTFSTRING);

					if(soap_ssl_accept(psoap))
					{
						PRINT_FATAL("SOAPSERVERFATAL Error in ssl accepting connection\n");
						soap_print_fault(psoap,stdout);
						soap_free(psoap);
						continue;
					}
				}
				else
				{

				}

				/*Do obslugi sygnalow*/
				_GLOBAL_twg_child_own_pid = getpid();

				if(signal(SIGUSR, kill_me) == SIG_ERR)
				{
					PRINT_ERROR("SOAPSERVER Error in signal setting - SIGUSR!\n");
					exit(0);
				}

				/*Na wszelki wypadek gdyby proces wnuka zakonczyl sie przedwczesnie.*/
				if(signal(SIGCHLD, SIG_IGN) == SIG_ERR)
				{
					PRINT_ERROR("SOAPSERVER Error in signal setting - SIGCHLD!\n");
					exit(0);
				}

				if(signal(SIGINT,SIG_IGN) == SIG_ERR)
				{
					PRINT_ERROR("SOAPSERVER Error in signal setting - SIGINT!\n");
					exit(0);
				}

				if(signal(SIGTERM, kill_me) == SIG_ERR)
				{
					PRINT_ERROR("SOAPSERVER Error in signal setting - SIGTERM!\n");
					exit(0);
				}

				if(signal(SIGHUP,SIG_IGN) == SIG_ERR)
				{
					PRINT_ERROR("SOAPSERVER Error in signal setting - SIGHUP!\n");
					exit(0);
				}

				server_function((void *)psoap);

   				//soap_destroy((void *)psoap);
   				soap_end((void *)psoap);
   				soap_done((void *)psoap);
				exit(0);
			}
			else // rodzic albo blad tworzenia procesu
			{
				if(pid == -1) // blad tworzenia procesu potomnego
				{
					lowLevelErrorCode = errno;
					PRINT_SOAP("Error: unable to create child process\n");
					PRINT_SOAP("Low level error = %li : <%s>\n", lowLevelErrorCode, strerror(lowLevelErrorCode));
				}

				PRINT_VDEBUG("SOAPSERVERVDEBUG Children number incrased to: %li\n", _GLOBAL_children_number);
				//soap_destroy((void *)psoap);
				close(s);
				// glowny proces serwera (nasluchujacy)
			}
		}
	}
	//soap_destroy(&soap);
	soap_end(&soap);
	soap_done(&soap);
	close(m);
	return 0;
}
