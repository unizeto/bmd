#include <semaphore.h>
#include <bmd/mail_sender/conf_sections.h>
#include <bmd/mail_sender/queue.h>
#include <bmd/mail_sender/shmem.h>
#include <bmd/libbmddb/libbmddb.h>
#include <bmd/mail_sender/mail_sender.h>
#include <bmd/libbmdglobals/libbmdglobals.h>
#include <bmd/mail_sender/configuration.h>
#include "ns.nsmap"

int GLOBAL_TEST = 0;

/********************************/
/*   Obsluga zadania klienta	*/
/********************************/
void *server_function(void *data)
{

/* 	-------------------------------		*/

	struct soap *soap	= NULL;
	long status		= 0;
//PRINT_INFO("GLOBAL_TEST aktualna wartość %d\n", GLOBAL_TEST);
/* 	-------------------------------		*/

	soap=(struct soap *)data;
	status=soap_serve(soap);
	if(status!=SOAP_OK)
	{
		soap_print_fault(soap,stderr);
	}
	bmd_db_disconnect(&(_GLOBAL_awizoConfig.dbase_handler));
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
			
			if(WIFEXITED(id) != 0) // potomek normalnie zakonczyl prace
			{
				if(WEXITSTATUS(id) == MAIL_SENDER_EXIT_STATUS_LOST_DB_CONNECTION) //potomek obslugujacy zadanie stwierdzil, ze stracil polaczenie z baza danych
				{
					_GLOBAL_awizo_restart_db_connection = 1;
				}
			}
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
/* 	-------------------------------------------------------------	*/

	long		last_children_number		=	0;

/* 	-------------------------------------------------------------	*/

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

	sem_destroy(&(_GLOBAL_shptr->clientMutex));
	destroyShRegion(&_GLOBAL_shared, _GLOBAL_shptr);
	bmd_db_disconnect(&(_GLOBAL_awizoConfig.dbase_handler));

	bmd_db_end();

	exit(0);
}

/********************************************************/
/*      Obslugha sygnalu SIGUSR zabijajacego dzieci     */
/********************************************************/

void kill_me(int status)
{
	PRINT_VDEBUG("AWIZOSERVERVDEBUG kill_me handler in child catched\n")
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
		/* PRINT_ERROR("%s\n", error_string); */
		return status;
	}

	return BMD_OK;
}

/* funkcja callback wywolywana w przypadku stwierdzenia utraty polaczenia podczas obslugi zadania */
long RequestHandlingProcess_LostConnectionFunction()
{
	_GLOBAL_awizo_restart_db_connection = 1;
	PRINT_DEBUG("Lost database connection callback\n");
	return BMD_OK;
}

/* funkcja callback wywolywana w przypadku stwierdzenia utraty polaczenia w procesie asynchronicznej wysylki eAwizo */
long AsyncProcess_LostConnectionFunction()
{
	PRINT_DEBUG("Lost database connection callback\n");
	
	PRINT_ERROR("Async-process error:\tDetected lost database connection...\n");
	while(bmd_db_connect2(_GLOBAL_awizoConfig.dbIP, \
				 _GLOBAL_awizoConfig.dbPort, \
				 _GLOBAL_awizoConfig.dbName, \
				 _GLOBAL_awizoConfig.dbUser, \
				 _GLOBAL_awizoConfig.dbPassword, \
				  &(_GLOBAL_awizoConfig.dbase_handler)) < BMD_OK)
	{
		PRINT_ERROR("Async-process error:\tReconnecting to database failed. Next attempt in 5 seconds...\n");
		sleep(5);
	}
	PRINT_ERROR("Async-process:\t Reconnected to database\n");

	return BMD_OK;
}



/************************/
/*    Funkcja serwera	*/
/************************/


int main(int argc, char **argv)
{
 _GLOBAL_debug_level		    = 30;
 long i				    = 0;
 long port			    = 0;
 long soap_send_timeout		    = 0;
 long soap_recv_timeout		    = 0;
 long max_connections		    = 0;
 long status			    = 0;
 long m				    = 0;
 long s				    = 0;

 char *host			 = NULL;
 struct soap *psoap		 = NULL;
 struct sigaction  	     cacheClear;
 struct sigaction        mailToQueueAdd;
 struct sigaction	 mailQueueClear;
 struct sigaction       mailSendingTime;
 sigset_t iset;

 pid_t pid;
 struct soap soap;
 char path[100];
 
    /*********************************************/
	/*     zapamietuje pid glownego serwera.     */
	/*********************************************/
	_GLOBAL_awizo_twg_parent_pid = getpid();

	/* _GLOBAL_awizoAction = queueNothing; */

	InitQueue(&_GLOBAL_myCache);
	InitQueue(&_GLOBAL_myXmlQueue);

	/************************************************/
	/*	obsluga sygnalu od procesow potomnych	*/
	/************************************************/
	if(signal(SIGQUIT, SIG_IGN) == SIG_ERR)
	{
		PRINT_FATAL("AWIZOSOAPSERVERFATAL Error in signal setting - SIGQUIT!\n");
		return 0;
	}

	if(signal(SIGRTMIN+5, SIG_IGN) == SIG_ERR)
	{
		PRINT_FATAL("AWIZOSOAPSERVERFATAL Error in signal setting - SIGUSR!\n");
		return 0;
	}

	if(signal(SIGRTMIN+7, SIG_IGN) == SIG_ERR)
	{
		PRINT_FATAL("AWIZOSOAPSERVERFATAL Error in signal setting - SIGUSR!\n");
		return 0;
	}

	if(signal(SIGUSR1, SIG_IGN) == SIG_ERR)
	{
		PRINT_FATAL("AWIZOSOAPSERVERFATAL Error in signal setting - SIGUSR!\n");
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

	if(signal(SIGUSR2, SIG_IGN) == SIG_ERR)
	{
		PRINT_FATAL("AWIZOSOAPSERVERFATAL Error in signal setting - SIGALRM!\n");
		return 0;
	}

	if(signal(SIGHUP, changeServerConfiguration) == SIG_ERR)
	{
		PRINT_FATAL("AWIZOSOAPSERVERFATAL Error in signal setting - SIGHUP!\n");
		return 0;
	}

	/**********************************************/
	/* 	Utworzenie obszaru pamięci dzielonej  */
	/**********************************************/
	if ( createShRegion("shmFile", &_GLOBAL_shared, (shRegion_t**)&_GLOBAL_shptr ) != BMD_OK ){
		PRINT_FATAL("AWIZOSOAPSERVERFATAL Error in shared memory initialize!\n");
		return 0;
	}

	_GLOBAL_awizo_children_number = 1;
	_GLOBAL_shptr->awizoAction = queueNothing;


	/****************************************/
	/*	parsoawnie linii polecen	*/
	/****************************************/
	if (argc<2)
	{
		printf("****** AWIZOSOAP SERVER for archive ver. 2.0 - Unizeto Technologies SA *******\n");
		printf("*                                                                            *\n");
		printf("*      -c [configuration file path] default: conf/soap_server.conf           *\n");
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
				printf("****** AWIZOSOAP SERVER for archive ver. 2.0 - Unizeto Technologies SA *******\n");
				printf("*                                                                            *\n");
				printf("*       -c [configuration file path] default: conf/soap_server.conf          *\n");
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
		strcpy(path,"conf/soap_server.conf");
	}

	bmd_init();

	/************************************************/
	/*	wczytanie ustawien konfiguracyjnych	*/
	/************************************************/
	status=load_soap_configuration(path);
	if (status<BMD_OK)
	{
		PRINT_ERROR("Error:\terror in loading configuration\n");
		return -1;
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

	status = getAwizoConfiguration( _GLOBAL_awizoSoapKonfiguracja, path, &_GLOBAL_awizoConfig );
	if (status!=BMD_OK)
	{
		PRINT_FATAL("AWIZOSOAPSERVERFATAL %s\n",GetErrorMsg(BMDSOAP_SERVER_CONF_READ_ERROR));
		return BMDSOAP_SERVER_CONF_READ_ERROR;
	}

	/**********************************************/
	/* 	Nawiązanie połączenia z bazą danych   */
	/**********************************************/

	
	//PRINT_INFO("GLOBAL_TEST = 1, aktualna wartość: %d\n", GLOBAL_TEST);
	//GLOBAL_TEST = 1;

	status = bmd_db_connect2(_GLOBAL_awizoConfig.dbIP, \
				 _GLOBAL_awizoConfig.dbPort, \
				 _GLOBAL_awizoConfig.dbName, \
				 _GLOBAL_awizoConfig.dbUser, \
				 _GLOBAL_awizoConfig.dbPassword, \
				  &(_GLOBAL_awizoConfig.dbase_handler));
 
	if ( status < 0 ){
		PRINT_ERROR("Błąd połączenia z bazą danych. Error = %d\n", BMD_ERR_OP_FAILED);
		return 0;
	}

	/*********************************************************/
	/* Obsługa procesu asynchronicznego wysyłania eAwiza     */
	/*********************************************************/

        switch (fork()) {

	    case  0 : {  
				/* ************************ */
				/*   Do obslugi sygnalow    */
				/* ************************ */

				_GLOBAL_awizo_twg_child_own_pid = getpid();

				if(signal(SIGQUIT, kill_me) == SIG_ERR)
				{
					PRINT_ERROR("AWIZOSOAPSERVER Error in signal setting - SIGQUIT!\n");
					exit(0);
				}

				/* ***************************************** */
				/*   Na wszelki wypadek gdyby proces wnuka   */ 
				/*       zakonczyl sie przedwczesnie.        */
				/* ***************************************** */

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

				if(signal(SIGHUP,SIG_IGN) == SIG_ERR)
				{
					PRINT_ERROR("AWIZOSOAPSERVER Error in signal setting - SIGHUP!\n");
					exit(0);
				}

				if(signal(SIGALRM,SIG_IGN) == SIG_ERR)
				{
					PRINT_ERROR("AWIZOSOAPSERVER Error in signal setting - SIGALRM!\n");
					exit(0);
				}
								
				// nowe polaczenie bazodanowe dla procesu asynchronicznego wysylania eAwiza
				_GLOBAL_awizoConfig.dbase_handler=NULL;
				//PRINT_INFO("GLOBAL_TEST = 2, aktualna wartość %d\n", GLOBAL_TEST);
				//GLOBAL_TEST = 2;
				//PRINT_INFO("GLOBAL_TEST: 2= %d\n", GLOBAL_TEST);
	
				//status = bmd_db_connect2( _GLOBAL_awizoConfig.dbIP, _GLOBAL_awizoConfig.dbPort, \
					_GLOBAL_awizoConfig.dbName, \
					_GLOBAL_awizoConfig.dbUser, _GLOBAL_awizoConfig.dbPassword, \
					&(_GLOBAL_awizoConfig.dbase_handler) );

				//if(status < 0)
				//{
				//	PRINT_ERROR("Błąd połączenia z bazą danych. Error = %li\n", status);
				//	exit(0);
				//}
				//bmd_db_set_lost_connection_callback(_GLOBAL_awizoConfig.dbase_handler, AsyncProcess_LostConnectionFunction);

				/**********************************************/
				/* 	Obsługa dodania maila do kolejki      */
				/**********************************************/

				sigfillset(&iset);
				mailToQueueAdd.sa_handler = addMailToQueue;
				mailToQueueAdd.sa_flags = SA_SIGINFO | SA_RESTART;
				mailToQueueAdd.sa_mask = iset;
				sigaction(SIGUSR1, &mailToQueueAdd, NULL ); 

				/* ***************************************** */
				/*     Funkcja obsługi kolejki mailingu po   */
				/*                  timeoucie		     */
				/* ***************************************** */

				sigfillset(&iset);
				mailSendingTime.sa_handler = sendMailQueue;
				mailSendingTime.sa_flags = SA_SIGINFO | SA_RESTART;
				mailSendingTime.sa_mask = iset;
				sigaction(SIGUSR2, &mailSendingTime, NULL ); 


				/**********************************************/
				/* 	Obsługa czyszczenia pamięci cache     */
				/**********************************************/

				sigfillset(&iset);
				cacheClear.sa_handler = clearCache;
				cacheClear.sa_flags = SA_SIGINFO | SA_RESTART;
				cacheClear.sa_mask = iset;
				sigaction( SIGRTMIN+5, &cacheClear, NULL ); 


				/**********************************************/
				/*    Obsługa czyszczenia kolejki z mailami   */
				/**********************************************/

				sigfillset(&iset);
				mailQueueClear.sa_handler = clearMailQueue;
				mailQueueClear.sa_flags = SA_SIGINFO | SA_RESTART;
				mailQueueClear.sa_mask = iset;
				sigaction( SIGRTMIN+7, &mailQueueClear, NULL ); 

			        mailQueueFunc();
		      }
	    case -1 : { 
			PRINT_FATAL("AWIZOSOAPSERVERFATAL Error in running awizo timer !\n");
			return 0;
		      } 	
	}

	/*********************************************************/
	/* Obsługa procesu synchronicznego wysyłania eAwiza      */
	/*********************************************************/

	switch (fork()) {

	    case  0 : {  
				/* ************************ */
				/*   Do obslugi sygnalow    */
				/* ************************ */

				_GLOBAL_awizo_twg_child_own_pid = getpid();

				if(signal(SIGQUIT, kill_me) == SIG_ERR)
				{
					PRINT_ERROR("AWIZOSOAPSERVER Error in signal setting - SIGQUIT!\n");
					exit(0);
				}

				/* ***************************************** */
				/*   Na wszelki wypadek gdyby proces wnuka   */ 
				/*       zakonczyl sie przedwczesnie.        */
				/* ***************************************** */

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

				if(signal(SIGHUP,SIG_IGN) == SIG_ERR)
				{
					PRINT_ERROR("AWIZOSOAPSERVER Error in signal setting - SIGHUP!\n");
					exit(0);
				}

				if(signal(SIGUSR1, SIG_IGN) == SIG_ERR)
				{
					PRINT_ERROR("AWIZOSOAPSERVER Error in signal setting - SIGUSR1!\n");
					exit(0);
				}

				if(signal(SIGUSR2, SIG_IGN) == SIG_ERR)
				{
					PRINT_ERROR("AWIZOSOAPSERVER Error in signal setting - SIGUSR2!\n");
					exit(0);
				}

				if(signal(SIGRTMIN+5, SIG_IGN) == SIG_ERR)
				{
					PRINT_FATAL("AWIZOSOAPSERVERFATAL Error in signal setting - SIGRTMIN+5!\n");
					return 0;
				}

				if(signal(SIGRTMIN+7, SIG_IGN) == SIG_ERR)
				{
					PRINT_FATAL("AWIZOSOAPSERVERFATAL Error in signal setting - SIGUSR!\n");
					return 0;
				}

				// proces synchronicznej wysylkie eAwizo nie korzysta z polaczenia bazodanowego
				_GLOBAL_awizoConfig.dbase_handler=NULL;
				//PRINT_INFO("GLOBAL_TEST = 3, aktualna wartość %d\n", GLOBAL_TEST);
				//GLOBAL_TEST = 3;
				
				
				/* wstawić funkcję która zasypia */

			        mailQueueTimerFunc();
		      }
	    case -1 : { 
			PRINT_FATAL("AWIZOSOAPSERVERFATAL Error in running awizo timer !\n");
			return 0;
		      } 	
	}

	
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
			//PRINT_INFO("Główna pętla serwera soap, GLOBAL_TEST aktualna wartość: %d\n", GLOBAL_TEST);
			s = soap_accept(&soap);
			if(s<0)
			{
				PRINT_ERROR("Error:\terror in accepting connection\n");
				soap_print_fault(&soap,stderr);
				break;
                	}
			if(_GLOBAL_awizo_restart_db_connection != 0)
			{
				// restart polaczenia (do skutku) - (zrestartuje sie tylko w masterprocesie, wiec nie wplynie to na stworzone juz wczesniej procesy potomne)
				PRINT_ERROR("Error:\tDetected lost database connection... accepting incoming request has been suspended\n");
				while(bmd_db_connect2(_GLOBAL_awizoConfig.dbIP, \
				 _GLOBAL_awizoConfig.dbPort, \
				 _GLOBAL_awizoConfig.dbName, \
				 _GLOBAL_awizoConfig.dbUser, \
				 _GLOBAL_awizoConfig.dbPassword, \
				  &(_GLOBAL_awizoConfig.dbase_handler)) < BMD_OK)
				{
					PRINT_ERROR("Error:\tReconnecting to database failed. Next attempt in 5 seconds...\n");
					sleep(5);
				}
				PRINT_ERROR("Server:\tReconnected to database\n");
				
				// oczekiwanie na zakonczenie wszystkich procesow obslugi (dekrementacja do 1 bo sync i async nie sa dodawane)
				// zeby juz zaden nie zglaszal niepotrzebnie
				while(_GLOBAL_awizo_children_number > 1)
				{
					PRINT_ERROR("Server:\tWaiting for finish already started request's handling processes... left %li process(es)\n", _GLOBAL_awizo_children_number);
					sleep(1);
				}
				
				_GLOBAL_awizo_restart_db_connection=0;
				
				PRINT_ERROR("Server: Accepting incoming request is active again\n");
			}
			
			PRINT_LOG("Connection:\tIP=%li.%li.%li.%li\n",(long)(soap.ip>>24)&0xFF,(long)(soap.ip>>16)&0xFF, 			(long)(soap.ip>>8)&0xFF,(long)soap.ip&0xFF);

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

				if(signal(SIGHUP,SIG_IGN) == SIG_ERR)
				{
					PRINT_ERROR("AWIZOSOAPSERVER Error in signal setting - SIGHUP!\n");
					exit(0);
				}

				if(signal(SIGRTMIN+5, SIG_IGN) == SIG_ERR)
				{
					PRINT_FATAL("AWIZOSOAPSERVERFATAL Error in signal setting - SIGUSR!\n");
					return 0;
				}

				if(signal(SIGRTMIN+7, SIG_IGN) == SIG_ERR)
				{
					PRINT_FATAL("AWIZOSOAPSERVERFATAL Error in signal setting - SIGUSR!\n");
					return 0;
				}

				if(signal(SIGUSR1, SIG_IGN) == SIG_ERR)
				{
					PRINT_FATAL("AWIZOSOAPSERVERFATAL Error in signal setting - SIGUSR!\n");
					return 0;
				}

				if(signal(SIGUSR2, SIG_IGN) == SIG_ERR)
				{
					PRINT_FATAL("AWIZOSOAPSERVERFATAL Error in signal setting - SIGUSR!\n");
					return 0;
				}

				bmd_db_set_lost_connection_callback(_GLOBAL_awizoConfig.dbase_handler, RequestHandlingProcess_LostConnectionFunction);
				
				server_function((void *)psoap);

   				soap_destroy((void *)psoap);
   				soap_end((void *)psoap);
   				soap_done((void *)psoap);
				
				if(_GLOBAL_awizo_restart_db_connection == 0)
				{
					exit(0);
				}
				else
				{
					exit(MAIL_SENDER_EXIT_STATUS_LOST_DB_CONNECTION);
				}
			}
			else
			{
				PRINT_VDEBUG("AWIZOSOAPSERVERVDEBUG Children number incrased to: %li\n", _GLOBAL_awizo_children_number);
				free(psoap); psoap=NULL; 
				close(s);

			}
		}
	}
	
	
	soap_destroy(&soap);
	soap_end(&soap);
	soap_done(&soap);
	close(m);

	bmd_end();

	return 0;
}











