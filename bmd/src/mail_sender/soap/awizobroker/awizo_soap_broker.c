#include <bmd/mail_sender/conf_sections.h>
#include <bmd/libbmddb/libbmddb.h>
#include <bmd/mail_sender/mail_sender.h>
#include <bmd/libbmdglobals/libbmdglobals.h>
#include <bmd/mail_sender/configuration.h>
#include "ns.nsmap"



/********************************/
/*   Obsluga zadania klienta	*/
/********************************/
void *server_function(void *data)
{

/* 	-------------------------------		*/

	struct soap *soap	= NULL;
	long status		= 0;

/* 	-------------------------------		*/

	soap=(struct soap *)data;
	status=soap_serve(soap);
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


	if(_GLOBAL_awizo_twg_parent_pid == getpid())
	{
		while ((pid = waitpid (-1, &id, WNOHANG)) > 0)
		{
			_GLOBAL_awizo_children_number--;
			PRINT_VDEBUG("AWIZOSOAPSERVERVDEBUG Children number decrased to: %li\n", _GLOBAL_awizo_children_number);
		}
	}
	else
	{
		if(_GLOBAL_awizo_twg_child_own_pid == 0 || _GLOBAL_awizo_twg_child_own_pid == getpid())
		{
			PRINT_VDEBUG("AWIZOSOAPSERVERVDEBUG Sigchld in normal child catched");
			while ((pid = waitpid (-1, &id, WNOHANG)) > 0);
		}
	}

}


/******************************************/
/*   Obsluga sygnalu SIGINT i SIGTERM    */
/******************************************/
void master_proc_sig(int status)
{
/* 	------------------------------	*/

	long last_children_number = 0;

/* 	------------------------------	*/

	if(_GLOBAL_awizo_twg_parent_pid == getpid())
	{
		PRINT_INFO("AWIZOSOAPSERVERINF Finishing soap awizoserver program work... %i\n",status);

		kill(0, SIGQUIT);
		while (_GLOBAL_awizo_children_number>0)
		{
			last_children_number = _GLOBAL_awizo_children_number;
			PRINT_NOTICE("AWIZOSOAPSERVERNOTICE Waiting for all children to die %li\n",_GLOBAL_awizo_children_number);
			sleep(1);
			if(last_children_number == _GLOBAL_awizo_children_number)
			{
				PRINT_WARNING("AWIZOSOAPSERVERWARNING Ignoring children number\n");
				break;
			}
		}
		PRINT_NOTICE("AWIZOSOAPSERVERNOTICE All children died\n");
		PRINT_LOG("AWIZOSOAPSERVERLOG Shutting down soap server\n");
	}
	else
	{
		if(_GLOBAL_awizo_twg_child_own_pid == 0 || _GLOBAL_awizo_twg_child_own_pid == getpid())
		{
			PRINT_VDEBUG("AWIZOSOAPSERVERVDEBUG Sigterm in child catched");
		}
		else
		{
			PRINT_VDEBUG("AWIZOSOAPSERVERVDEBUG Sigterm in stream child catched");
			exit(1);
		}
	}

	bmd_db_end();

	exit(0);
}

/********************************************************/
/*      Obslugha sygnalu SIGUSR zabijajacego dzieci     */
/********************************************************/

void kill_me(int status)
{
	PRINT_VDEBUG("AWIZOSERVERVDEBUG kill_me handler in child catched")
	exit(0);
}



/********************************************************/
/*	Zaladowanie ustawień konfiguracynjych serwera	*/
/********************************************************/

/*
long load_soap_configuration(char *path)
{
*/

/* 	-------------------------------------	*/

/*
	char*	error_string	=	NULL;
	long	status		=	0;
*/

/* 	-------------------------------------	*/

/*
	PRINT_LOG("Server:\tLoading configuration settings\n");
	
	BMD_FOK(bmdconf_init(&_GLOBAL_awizoSoapKonfiguracja));
	
	status = bmdconf_set_mode(_GLOBAL_awizoSoapKonfiguracja, BMDCONF_TOLERANT);
	
        if ( status < 0 ) {
	    PRINT_ERROR("Bład zmiany ustawien odczytu słownika. Error = %d\n", BMD_ERR_OP_FAILED);
	    return BMD_ERR_OP_FAILED;
	}
			
	BMD_FOK(bmdconf_add_section2dict(_GLOBAL_awizoSoapKonfiguracja,"soap",ws_DICT_SOAP ,BMDCONF_SECTION_REQ));

	PRINT_LOG("server:\tLoading configuration from file %s\n",path);
	status=bmdconf_load_file(path,_GLOBAL_awizoSoapKonfiguracja,&error_string);
	if (status<BMD_OK)
	{
*/
		/* PRINT_ERROR("%s\n", error_string); */
/*		return status;*/
/*	}

	return BMD_OK;
}
*/

/************************/
/*    Funkcja serwera	*/
/************************/


int main(int argc, char **argv)
{
 _GLOBAL_debug_level		    = 20;
 long i				    = 0;
 long port			    = 0;
 long soap_send_timeout		    = 0;
 long soap_recv_timeout		    = 0;
 long max_connections		    = 0;
 long status			    = 0;
 long m				    = 0;
 long s				    = 0;
 char *host			 = NULL;
 char *error_string		 = NULL;
 struct soap *psoap		 = NULL;
 
 pid_t pid;
 struct soap soap;
 char path[100];
 

        /*********************************************/
	/*     zapamietuje pid glownego serwera.     */
	/*********************************************/
	_GLOBAL_awizo_twg_parent_pid = getpid();

	/************************************************/
	/*	obsluga sygnalu od procesow potomnych	*/
	/************************************************/
	if(signal(SIGQUIT, SIG_IGN) == SIG_ERR)
	{
		PRINT_FATAL("AWIZOSOAPSERVERFATAL Error in signal setting - SIGQUIT!\n");
		return 0;
	}

	if(signal(SIGCHLD, sigchld_handler) == SIG_ERR)
	{
		PRINT_FATAL("AWIZOSOAPSERVERFATAL Error in signal setting - SIGCHLD!\n");
		return 0;
	}

	if(signal(SIGTERM, master_proc_sig) == SIG_ERR)
	{
		PRINT_FATAL("AWIZOSOAPSERVERFATAL Error in signal setting - SIGTERM!\n");
		return 0;
	}

	if(signal(SIGINT, master_proc_sig) == SIG_ERR)
	{
		PRINT_FATAL("AWIZOSOAPSERVERFATAL Error in signal setting - SIGINT!\n");
		return 0;
	}

	_GLOBAL_awizo_children_number = 1;

	/****************************************/
	/*	parsoawnie linii polecen	*/
	/****************************************/
	if (argc<2)
	{
		printf("****** AWIZOSOAP BROKER for archive ver. 2.0 - Unizeto Technologies SA *******\n");
		printf("*                                                                            *\n");
		printf("*      -c [configuration file path] default: conf/awizo_soap_broker.conf     *\n");
		printf("*                                                                            *\n");
		printf("******************************************************************************\n");
		return 0;
	}
	else
	{
		strcpy(path,"");
		for (i=1; i<argc; i++)
		{
			if ((strcmp(argv[i],"-h")==0) || (strcmp(argv[i],"--help")==0))
			{
				printf("****** AWIZOSOAP BROKER for archive ver. 2.0 - Unizeto Technologies SA *******\n");
				printf("*                                                                            *\n");
				printf("*       -c [configuration file path] default: conf/awizo_soap_broker.conf    *\n");
				printf("*                                                                            *\n");
				printf("******************************************************************************\n");
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
		strcpy(path,"conf/awizo_soap_broker.conf");
	}

	bmd_init();

	/************************************************/
	/*	wczytanie ustawien konfiguracyjnych	*/
	/************************************************/


	/* Konfiguracja do zrobienia dla brokera */	
	

	/* Zbędne */
/*	
	status=load_soap_configuration(path);
	if (status<BMD_OK)
	{
		PRINT_ERROR("Error:\terror in loading configuration\n");
		return -1;
	}
*/

	PRINT_LOG("Server:\tLoading configuration settings\n");
	
	BMD_FOK(bmdconf_init(&_GLOBAL_awizoSoapKonfiguracja));
	
	status = bmdconf_set_mode(_GLOBAL_awizoSoapKonfiguracja, BMDCONF_TOLERANT);
	
        if ( status < 0 ) {
	    PRINT_ERROR("Bład zmiany ustawien odczytu słownika. Error = %d\n", BMD_ERR_OP_FAILED);
	    return BMD_ERR_OP_FAILED;
	}
			
	BMD_FOK(bmdconf_add_section2dict(_GLOBAL_awizoSoapKonfiguracja,"soap",ws_DICT_SOAP ,BMDCONF_SECTION_REQ));

	PRINT_LOG("server:\tLoading configuration from file %s\n",path);

	status=bmdconf_load_file(path,_GLOBAL_awizoSoapKonfiguracja,&error_string);
	if (status<BMD_OK)
	{
		PRINT_ERROR("Bład odczytu pliku konfiguracyjnego. Error = %d\n", BMD_ERR_OP_FAILED);
		return status;
	}

	status=bmdconf_get_value_int(_GLOBAL_awizoSoapKonfiguracja,"soap","max_connections",&max_connections);
	if (status<BMD_OK)
	{
		PRINT_FATAL("AWIZOSOAPSERVERFATAL %s\n",GetErrorMsg(BMDSOAP_SERVER_CONF_READ_ERROR));
		return BMDSOAP_SERVER_CONF_READ_ERROR;
	}

	status=bmdconf_get_value_int(_GLOBAL_awizoSoapKonfiguracja,"soap","send_timeout",&soap_send_timeout);
	if (status<BMD_OK)
	{
		PRINT_FATAL("AWIZOSOAPSERVERFATAL %s\n",GetErrorMsg(BMDSOAP_SERVER_CONF_READ_ERROR));
		return BMDSOAP_SERVER_CONF_READ_ERROR;
	}

	status=bmdconf_get_value_int(_GLOBAL_awizoSoapKonfiguracja,"soap","recv_timeout",&soap_recv_timeout);
	if (status<BMD_OK)
	{
		PRINT_FATAL("AWIZOSOAPSERVERFATAL %s\n",GetErrorMsg(BMDSOAP_SERVER_CONF_READ_ERROR));
		return BMDSOAP_SERVER_CONF_READ_ERROR;
	}

	status=bmdconf_get_value(_GLOBAL_awizoSoapKonfiguracja,"soap","host",&host);
	if (status<BMD_OK)
	{
		PRINT_FATAL("AWIZOSOAPSERVERFATAL %s\n",GetErrorMsg(BMDSOAP_SERVER_CONF_READ_ERROR));
		return BMDSOAP_SERVER_CONF_READ_ERROR;
	}

	status=bmdconf_get_value_int(_GLOBAL_awizoSoapKonfiguracja,"soap","port",&port);
	if (status<BMD_OK)
	{
		PRINT_FATAL("AWIZOSOAPSERVERFATAL %s\n",GetErrorMsg(BMDSOAP_SERVER_CONF_READ_ERROR));
		return BMDSOAP_SERVER_CONF_READ_ERROR;
	}

	/* Pobierz lokalizację serwerów awiza z bazy danych */

	status = getAwizoBrokerConfiguration( _GLOBAL_awizoSoapKonfiguracja, path, &_GLOBAL_awizoConfig );
	if (status!=BMD_OK)
	{
		PRINT_FATAL("AWIZOSOAPSERVERFATAL %s\n",GetErrorMsg(BMDSOAP_SERVER_CONF_READ_ERROR));
		return BMDSOAP_SERVER_CONF_READ_ERROR;
	}

	bmdconf_destroy(&_GLOBAL_awizoSoapKonfiguracja);


	/*******************************/
	/*  Właściowa część serwerowa  */
	/*******************************/

	soap_init2(&soap,SOAP_C_UTFSTRING,SOAP_C_UTFSTRING);
	soap.bind_flags=SO_REUSEADDR;
	soap.send_timeout=soap_send_timeout;
	soap.recv_timeout=soap_recv_timeout;
	soap.max_keep_alive=max_connections; /* max keep-alive sequence */


	/********************************/
	/*	zbindowanie gniazda	*/
	/********************************/

	m=soap_bind(&soap,host,port,100);
	if(m<0)
	{
		PRINT_FATAL("AWIZOSOAPSERVERFATAL Error:\t%s\n",GetErrorMsg(BMDSOAP_SERVER_SOCKET_BIND_ERROR));
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
			_GLOBAL_awizo_twg_child_own_pid = 0;
			pid=fork();
			_GLOBAL_awizo_children_number++;
			if(pid==0)
			{
				/* *********************** */
				/*   Do obslugi sygnalow   */
				/* *********************** */

				_GLOBAL_awizo_twg_child_own_pid = getpid();

				if(signal(SIGQUIT, kill_me) == SIG_ERR)
				{
					PRINT_ERROR("AWIZOSOAPSERVER Error in signal setting - SIGUSR!\n");
					exit(0);
				}

				/* *********************************************** */
				/*     Na wszelki wypadek gdyby proces wnuka       */ 
				/*	   zakonczyl sie przedwczesnie.            */
				/* *********************************************** */

				if(signal(SIGCHLD, SIG_IGN) == SIG_ERR)
				{
					PRINT_ERROR("AWIZOSOAPSERVER Error in signal setting - SIGCHLD!\n");
					exit(0);
				}

				if(signal(SIGINT,SIG_IGN) == SIG_ERR)
				{
					PRINT_ERROR("AWIZOSOAPSERVER Error in signal setting - SIGINT!\n");
					exit(0);
				}

				if(signal(SIGTERM, kill_me) == SIG_ERR)
				{
					PRINT_ERROR("AWIZOSOAPSERVER Error in signal setting - SIGTERM!\n");
					exit(0);
				}

				server_function((void *)psoap);

   				soap_destroy((void *)psoap);
   				soap_end((void *)psoap);
   				soap_done((void *)psoap);
				exit(0);
			}
			else
			{
				PRINT_VDEBUG("AWIZOSOAPSERVERVDEBUG Children number incrased to: %li\n", _GLOBAL_awizo_children_number);
				free(psoap); psoap=NULL; 
				close(s);

			}
		}
	}
	
	/* bmd_db_disconnect(&(_GLOBAL_awizoConfig.dbase_handler)); */
	soap_destroy(&soap);
	soap_end(&soap);
	soap_done(&soap);
	close(m);

	bmd_end();

	return 0;
}

