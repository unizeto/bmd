#include <bmd/libbmdglobals/libbmdglobals.h>
#include <bmd/libbmderr/libbmderr.h>
#include <conf_sections.h>
#include "log.nsmap"


/******************************/
/*	Obsluga zadania klienta	*/
/******************************/
void *server_function(void *data)
{

/* 	-------------------------------		*/

	struct soap *soap	= NULL;
	long status		= 0;

/* 	-------------------------------		*/

	soap=(struct soap *)data;
	status=log_serve(soap);
	if(status!=SOAP_OK)
	{
		soap_print_fault(soap,stderr);
	}
	PRINT_LOG("Connection:\tclosed\n");
	printf("\n");

	return NULL;
}

/****************************************************************/
/*	Obsluga sygnalu SIGCHLD - eliminowanie procesow zombi	*/
/****************************************************************/
void sigchld_handler(int id)
{

/* 	---------	*/

	long pid;

/* 	---------	*/

	if(_GLOBAL_log_twg_parent_pid == getpid())
	{
		while ((pid = waitpid (-1, &id, WNOHANG)) > 0)
		{
			PRINT_VDEBUG("LOGSOAPSERVERVDEBUG Children number decrased to: %li\n", _GLOBAL_log_children_number);
			_GLOBAL_log_children_number--;
		}
	}
	else
	{
		if(_GLOBAL_log_twg_child_own_pid == 0 || _GLOBAL_log_twg_child_own_pid == getpid())
		{
			PRINT_VDEBUG("LOGSOAPSERVERVDEBUG Sigchld in normal child catched");
			while ((pid = waitpid (-1, &id, WNOHANG)) > 0);
		}
	}
}


/******************************************/
/*      Obslugha sygnalu SIGINT i SIGTERM       */
/******************************************/
void master_proc_sig(int status)
{
/* 	-------------------------------------------------------------	*/

	long		last_children_number		=	0;

/* 	-------------------------------------------------------------	*/

	if(_GLOBAL_log_twg_parent_pid == getpid())
	{
		PRINT_INFO("LOGSOAPSERVERINF Finishing soap logserver program work... %i\n",status);

		kill(0,SIGUSR);
		while (_GLOBAL_log_children_number>0)
		{
			last_children_number = _GLOBAL_log_children_number;
			PRINT_NOTICE("LOGSOAPSERVERNOTICE Waiting for all children to die %li\n",_GLOBAL_log_children_number);
			sleep(1);
			if(last_children_number == _GLOBAL_log_children_number)
			{
				PRINT_WARNING("LOGSOAPSERVERWARNING Ignoring children number\n");
				break;
			}
		}
		PRINT_NOTICE("LOGSOAPSERVERNOTICE All children died\n");

		PRINT_LOG("LOGSOAPSERVERLOG Shutting down soap server\n");
	}
	else
	{
		if(_GLOBAL_log_twg_child_own_pid == 0 || _GLOBAL_log_twg_child_own_pid == getpid())
		{
			PRINT_VDEBUG("LOGSOAPSERVERVDEBUG Sigterm in child catched");
		}
		else
		{
			PRINT_VDEBUG("LOGSOAPSERVERVDEBUG Sigterm in stream child catched");
			exit(1);
		}
	}



	exit(0);
}

/********************************************************/
/*      Obslugha sygnalu SIGUSR zabijajacego dzieci     */
/********************************************************/
void kill_me(int status)
{
	PRINT_VDEBUG("SOAPSERVERVDEBUG kill_me handler in child catched")
	exit(0);
}

/********************************************************/
/*	Zaladowanie ustawień konfiguracynjych serwera	*/
/********************************************************/

long load_soap_configuration(char *path)
{

/* 	-------------------------------------	*/

	char*	error_string	=	NULL;
	long	status		=	0;

/* 	-------------------------------------	*/


	PRINT_LOG("Server:\tLoading configuration settings\n");
	BMD_FOK(bmdconf_init(&_GLOBAL_logSoapKonfiguracja));
	BMD_FOK(bmdconf_add_section2dict(_GLOBAL_logSoapKonfiguracja,"logserver",ws_DICT_LOG,BMDCONF_SECTION_REQ));
	BMD_FOK(bmdconf_add_section2dict(_GLOBAL_logSoapKonfiguracja,"soap",ws_DICT_SOAP ,BMDCONF_SECTION_REQ));

	PRINT_LOG("server:\tLoading configuration from file %s\n",path);
	status=bmdconf_load_file(path,_GLOBAL_logSoapKonfiguracja,&error_string);
	
	if (status<BMD_OK && error_string) {
		PRINT_ERROR("%s\n", error_string);
	}

	return status;
}

/******************************/
/*	Główna funkcja serwera	*/
/******************************/
int main(int argc, char **argv)
{
_GLOBAL_debug_level		= 3;
long i				= 0;
long port			= 0;
long soap_accept_timeout	= 0;
long soap_send_timeout		= 0;
long soap_recv_timeout		= 0;
long max_connections		= 0;
long status			= 0;
long m				= 0;
long s				= 0;

char *host			= NULL;
char* log_client_conf		= NULL;
struct soap *psoap		= NULL;
err_t  err_tRet			=    0;

pid_t pid;
struct soap soap;
char path[100];



	/*zapamietuje pid glownego serwera.*/
	_GLOBAL_log_twg_parent_pid = getpid();

	/************************************************/
	/*	obsluga sygnalu od procesow potomnych	*/
	/************************************************/
	if(signal(SIGUSR, SIG_IGN) == SIG_ERR)
	{
		PRINT_FATAL("LOGSOAPSERVERFATAL Error in signal setting - SIGUSR!\n");
		return 0;
	}

	if(signal(SIGCHLD, sigchld_handler) == SIG_ERR)
	{
		PRINT_FATAL("LOGSOAPSERVERFATAL Error in signal setting - SIGCHLD!\n");
		return 0;
	}

	if(signal(SIGTERM, master_proc_sig) == SIG_ERR)
	{
		PRINT_FATAL("LOGSOAPSERVERFATAL Error in signal setting - SIGTERM!\n");
		return 0;
	}

	if(signal(SIGINT, master_proc_sig) == SIG_ERR)
	{
		PRINT_FATAL("LOGSOAPSERVERFATAL Error in signal setting - SIGINT!\n");
		return 0;
	}


	/****************************************/
	/*	parsoawnie linii polecen	*/
	/****************************************/
	if (argc<2)
	{
		printf("****** LOGSOAP SERVER for archive ver. 1.7 - Unizeto Technologies SA *******\n");
		printf("*                                                                          *\n");
		printf("*      -c [configuration file path] default: conf/soap_server.conf         *\n");
		printf("*                                                                          *\n");
		printf("****************************************************************************\n");
		return 0;
	}
	else
	{
		strcpy(path,"");
		for (i=1; i<argc; i++)
		{
			if ((strcmp(argv[i],"-h")==0) || (strcmp(argv[i],"--help")==0))
			{
				printf("****** LOGSOAP SERVER for archive ver. 1.4 - Unizeto Technologies SA *******\n");
				printf("*                                                                          *\n");
				printf("*       -c [configuration file path] default: conf/soap_server.conf        *\n");
				printf("*                                                                          *\n");
				printf("****************************************************************************\n");
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
					PRINT_LOG("Error:\tno path to configuration file\n");
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
	status=load_soap_configuration(path);
	if (status<BMD_OK)
	{
		PRINT_ERROR("Error:\terror in loading configuration\n");
		return -1;
	}

	status=bmdconf_get_value_int(_GLOBAL_logSoapKonfiguracja,"soap","max_connections",&max_connections);
	if (status<BMD_OK)
	{
		PRINT_FATAL("SOAPSERVERFATAL %s\n",GetErrorMsg(BMDSOAP_SERVER_CONF_READ_ERROR));
		return BMDSOAP_SERVER_CONF_READ_ERROR;
	}

	status=bmdconf_get_value_int(_GLOBAL_logSoapKonfiguracja,"soap","accept_timeout",&soap_accept_timeout);
	if (status<BMD_OK)
	{
		PRINT_FATAL("SOAPSERVERFATAL %s\n",GetErrorMsg(BMDSOAP_SERVER_CONF_READ_ERROR));
		return BMDSOAP_SERVER_CONF_READ_ERROR;
	}

	status=bmdconf_get_value_int(_GLOBAL_logSoapKonfiguracja,"soap","send_timeout",&soap_send_timeout);
	if (status<BMD_OK)
	{
		PRINT_FATAL("SOAPSERVERFATAL %s\n",GetErrorMsg(BMDSOAP_SERVER_CONF_READ_ERROR));
		return BMDSOAP_SERVER_CONF_READ_ERROR;
	}

	status=bmdconf_get_value_int(_GLOBAL_logSoapKonfiguracja,"soap","recv_timeout",&soap_recv_timeout);
	if (status<BMD_OK)
	{
		PRINT_FATAL("SOAPSERVERFATAL %s\n",GetErrorMsg(BMDSOAP_SERVER_CONF_READ_ERROR));
		return BMDSOAP_SERVER_CONF_READ_ERROR;
	}

	status=bmdconf_get_value(_GLOBAL_logSoapKonfiguracja,"soap","host",&host);
	if (status<BMD_OK)
	{
		PRINT_FATAL("SOAPSERVERFATAL %s\n",GetErrorMsg(BMDSOAP_SERVER_CONF_READ_ERROR));
		return BMDSOAP_SERVER_CONF_READ_ERROR;
	}

	status=bmdconf_get_value_int(_GLOBAL_logSoapKonfiguracja,"soap","port",&port);
	if (status<BMD_OK)
	{
		PRINT_FATAL("SOAPSERVERFATAL %s\n",GetErrorMsg(BMDSOAP_SERVER_CONF_READ_ERROR));
		return BMDSOAP_SERVER_CONF_READ_ERROR;
	}

	status=bmdconf_get_value(_GLOBAL_logSoapKonfiguracja,"logserver","conf_file",&log_client_conf);
	if (status<BMD_OK)
	{
		PRINT_FATAL("SOAPSERVERFATAL %s\n",GetErrorMsg(BMDSOAP_SERVER_CONF_READ_ERROR));
		return BMDSOAP_SERVER_CONF_READ_ERROR;
	}


	soap_init2(&soap,SOAP_C_UTFSTRING,SOAP_C_UTFSTRING);
	soap_set_namespaces(&soap, log_namespaces);
	soap.bind_flags=SO_REUSEADDR;
	soap.send_timeout=soap_send_timeout;
	soap.recv_timeout=soap_recv_timeout;
	soap.max_keep_alive=max_connections; // max keep-alive sequence

	/********************************/
	/*	zbindowanie gniazda	*/
	/********************************/
	m=soap_bind(&soap,host,port,100);
	if(m<0)
	{
		PRINT_FATAL("LOGSOAPSERVERFATAL Error:\t%s\n",GetErrorMsg(BMDSOAP_SERVER_SOCKET_BIND_ERROR));
		soap_print_fault(&soap,stderr);
		return BMDSOAP_SERVER_SOCKET_BIND_ERROR;
	}
	else
	{
		PRINT_ERROR("Server:\tsocket binding successful\n");
		PRINT_ERROR("Server:\tserver started\n\n\n");
		for(;;)
		{
			/************************************************/
			/*	oczekiwanie na połaczenie klienta	*/
			/************************************************/
			s = soap_accept(&soap);
			if(s<0)
			{
				PRINT_ERROR("Error:\terror in accepting connection\n");
				soap_print_fault(&soap,stderr);
				break;
                	}

			PRINT_LOG("Connection:\tIP=%li.%li.%li.%li\n",(long)(soap.ip>>24)&0xFF,(long)(soap.ip>>16)&0xFF,
			(long)(soap.ip>>8)&0xFF,(long)soap.ip&0xFF);

			/************************************************/
			/*	duplikowanie znacznika polaczenia	*/
			/************************************************/
			psoap=soap_copy(&soap);
			if(psoap==NULL)
			{
				PRINT_LOG("Error:\terror in duplication connection pointer\n");
				break;
			}
			_GLOBAL_log_twg_child_own_pid = 0;
			pid=fork();
			_GLOBAL_log_children_number++;
			if(pid==0)
			{
				/*Do obslugi sygnalow*/
				_GLOBAL_log_twg_child_own_pid = getpid();

				if(signal(SIGUSR, kill_me) == SIG_ERR)
				{
					PRINT_ERROR("LOGSOAPSERVER Error in signal setting - SIGUSR!\n");
					exit(0);
				}

				/*Na wszelki wypadek gdyby proces wnuka zakonczyl sie przedwczesnie.*/
				if(signal(SIGCHLD, SIG_IGN) == SIG_ERR)
				{
					PRINT_ERROR("LOGSOAPSERVER Error in signal setting - SIGCHLD!\n");
					exit(0);
				}

				if(signal(SIGINT,SIG_IGN) == SIG_ERR)
				{
					PRINT_ERROR("LOGSOAPSERVER Error in signal setting - SIGINT!\n");
					exit(0);
				}

				if(signal(SIGTERM, kill_me) == SIG_ERR)
				{
					PRINT_ERROR("LOGSOAPSERVER Error in signal setting - SIGTERM!\n");
					exit(0);
				}

				if(signal(SIGHUP,SIG_IGN) == SIG_ERR)
				{
					PRINT_ERROR("LOGSOAPSERVER Error in signal setting - SIGHUP!\n");
					exit(0);
				}


                                err_tRet = LogInit(log_client_conf, &_GLOBAL_soaplog_HANDLER);

                                if ( BMD_ERR( err_tRet )) {
                                  BMD_BTERR( err_tRet );
                                  BMD_FREEERR( err_tRet );
                                  exit(-1);
                                }

				server_function((void *)psoap);


                                err_tRet = LogEnd(&_GLOBAL_soaplog_HANDLER);

                                if ( BMD_ERR( err_tRet )) {
                                  BMD_BTERR( err_tRet );
                                  BMD_FREEERR( err_tRet );
                                }

   				soap_destroy(psoap);
   				soap_end(psoap);
   				soap_free(psoap);
   				
				exit(0);
			}
			else
			{
				PRINT_VDEBUG("LOGSOAPSERVERVDEBUG Children number incrased to: %li\n", _GLOBAL_log_children_number);
				free(psoap); psoap=NULL;
				close(s);
				/*czynności serwera :) */
			}
		}
	}
	soap_destroy(&soap);
	soap_end(&soap);
	soap_done(&soap);
	close(m);


	return 0;
}
