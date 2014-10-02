/* *************************************** */
/*               awizo_jms.c               */
/*                                         */
/*         Created on: 2010-01-13          */
/*         Author: Tomasz Klimek           */
/* *************************************** */

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <mqcrt.h>
#include <sys/types.h>
#include <semaphore.h>
#include <bmd/libbmdglobals/libbmdglobals.h>
#include <bmd/mail_sender/queue.h>
#include <bmd/mail_sender/shmem.h>
#include <bmd/libbmddb/libbmddb.h>
#include <bmd/mail_sender/mail_sender.h>
#include <bmd/mail_sender/configuration.h>
#include <bmd/mail_sender/configuration.dic>

#define MQ_ERR_CHK(mqCall)                             \
  if (MQStatusIsError(status = (mqCall)) == MQ_TRUE) { \
    goto Cleanup; \
 }

long sendAwizo(char const * const);

/* ******************************************** */
/*        Obsluga sygnalu SIGINT i SIGTERM      */
/* ******************************************** */
void master_proc_sig(int stat)
{
/* 	------------------------------  */

	long last_children_number = 0;

/* 	------------------------------  */

	if(_GLOBAL_awizo_twg_parent_pid == getpid())
	{
		PRINT_INFO("AWIZOJMSSERVERINF Finishing jms awizoserver program work... %i\n",stat);

		kill(0,SIGUSR);
		while (_GLOBAL_awizo_children_number>0)
		{
			last_children_number = _GLOBAL_awizo_children_number;
			PRINT_NOTICE("AWIZOJMSSERVERNOTICE Waiting for all children to die %li\n",_GLOBAL_awizo_children_number);
			sleep(1);
			if(last_children_number == _GLOBAL_awizo_children_number)
			{
				PRINT_WARNING("AWIZOJMSSERVERWARNING Ignoring children number\n");
				break;
			}
		}
		PRINT_NOTICE("AWIZOJMSSERVERNOTICE All children died\n");

		PRINT_LOG("AWIZOJMSSERVERLOG Shutting down jms server\n");
	}
	else
	{
		if(_GLOBAL_awizo_twg_child_own_pid == 0 || _GLOBAL_awizo_twg_child_own_pid == getpid())
		{
			PRINT_VDEBUG("AWIZOJMSSERVERVDEBUG Sigterm in child catched");
		}
		else
		{
			PRINT_VDEBUG("AWIZOJMSSERVERVDEBUG Sigterm in stream child catched");
			exit(1);
		}
	}

	destroyShRegion(&_GLOBAL_shared, _GLOBAL_shptr);
	sem_destroy(&(_GLOBAL_shptr->clientMutex));
        bmd_db_disconnect(&(_GLOBAL_awizoConfig.dbase_handler));

	bmd_db_end();

  exit(0);
}


/********************************************************/
/*      Obslugha sygnalu SIGUSR zabijajacego dzieci     */
/********************************************************/

void kill_me(int status)
{
	PRINT_VDEBUG("AWIZOJMSVDEBUG kill_me handler in child catched\n")
	exit(0);
}


/****************************************************************/
/*   Obsluga sygnalu SIGCHLD - eliminowanie procesow zombi	*/
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
			PRINT_VDEBUG("AWIZOJMSSERVERVDEBUG Children number decrased to: %li\n", _GLOBAL_awizo_children_number);
		}
	}
	else
	{
		if(_GLOBAL_awizo_twg_child_own_pid == 0 || _GLOBAL_awizo_twg_child_own_pid == getpid())
		{
			PRINT_VDEBUG("AWIZOJMSSERVERVDEBUG Sigchld in normal child catched");
			while ((pid = waitpid (-1, &id, WNOHANG)) > 0);
		}
	}
}

/* ********************************** */
/*   Główna funkcja kolejki JMSowej   */
/* ********************************** */

MQStatus consume(char * brokerHost, int brokerPort,
         char * destinationName, MQDestinationType destinationType)
{
  MQStatus status;

  /* Declare handles and initialize them */
  MQPropertiesHandle propertiesHandle = MQ_INVALID_HANDLE;
  MQConnectionHandle connectionHandle = MQ_INVALID_HANDLE;
  MQSessionHandle sessionHandle = MQ_INVALID_HANDLE;
  MQDestinationHandle destinationHandle = MQ_INVALID_HANDLE;
  MQConsumerHandle consumerHandle = MQ_INVALID_HANDLE;
  MQMessageHandle messageHandle = MQ_INVALID_HANDLE;
  MQPropertiesHandle headersHandle = MQ_INVALID_HANDLE;

  /* Setup connection properties */
  MQ_ERR_CHK( MQCreateProperties(&propertiesHandle) );
  MQ_ERR_CHK( MQSetStringProperty(propertiesHandle,
                                  MQ_BROKER_HOST_PROPERTY, brokerHost) );
  MQ_ERR_CHK( MQSetInt32Property(propertiesHandle,
                                 MQ_BROKER_PORT_PROPERTY, brokerPort) );
  MQ_ERR_CHK( MQSetStringProperty(propertiesHandle,
                                  MQ_CONNECTION_TYPE_PROPERTY, "TCP") );

  /* Create a connection to Message Queue broker */
  MQ_ERR_CHK( MQCreateConnection(propertiesHandle, "guest", "guest", NULL,
                                 NULL, NULL, &connectionHandle) );

  /* Create a session for synchronous message receiving */
  MQ_ERR_CHK( MQCreateSession(connectionHandle, MQ_FALSE, MQ_CLIENT_ACKNOWLEDGE,
                              MQ_SESSION_SYNC_RECEIVE, &sessionHandle) );

  /* Create a destination */
  MQ_ERR_CHK( MQCreateDestination(sessionHandle, destinationName,
                                  destinationType, &destinationHandle) );

  /* Create a synchronous messagse consumer on the destination */
  MQ_ERR_CHK( MQCreateMessageConsumer(sessionHandle, destinationHandle,
                                      NULL, MQ_TRUE, &consumerHandle) );

  /* Free the destination handle */
  MQ_ERR_CHK( MQFreeDestination(destinationHandle) );

  /* Start the connection */
  MQ_ERR_CHK( MQStartConnection(connectionHandle) );

  /* Receiving messages */
  while (1)
  {
    fprintf(stdout, "Waiting for message ...\n");
    MQ_ERR_CHK( MQReceiveMessageWait(consumerHandle, &messageHandle) );
    {
      MQBool redelivered;
      //ConstMQString my_msgtype;
      ConstMQString msgtext;
      MQMessageType messageType;

      /* Check message type */
      MQ_ERR_CHK( MQGetMessageType(messageHandle, &messageType) );
      if (messageType != MQ_TEXT_MESSAGE)
      {
        fprintf(stdout, "Received mesage is not MQ_TEXT_MESSAGE type.\n");
        MQ_ERR_CHK( MQAcknowledgeMessages(sessionHandle, messageHandle) );
		MQ_ERR_CHK( MQFreeMessage(messageHandle) );
        continue;
      }

      /* Get message body */
      MQ_ERR_CHK( MQGetTextMessageText(messageHandle, &msgtext) );
      //fprintf(stdout, "Received message: %s\n", msgtext);

      /* Get message headers, for example */
      MQ_ERR_CHK( MQGetMessageHeaders(messageHandle, &headersHandle) );
      MQ_ERR_CHK( MQGetBoolProperty(headersHandle,
                            MQ_REDELIVERED_HEADER_PROPERTY, &redelivered) );
      //fprintf(stdout, "\tHeader redelivered=%d\n", redelivered);

//      status = MQGetStringProperty(headersHandle,
//                            MQ_MESSAGE_TYPE_HEADER_PROPERTY, &my_msgtype);
//      if (MQGetStatusCode(status) != MQ_NOT_FOUND)
//      {
//        MQ_ERR_CHK( status );
//        fprintf(stdout, "\tHeader message-type=%s\n", my_msgtype);
//      }

      /* Free the headers handle */
      MQ_ERR_CHK( MQFreeProperties(headersHandle) );

      sendAwizo((char*)msgtext);

      /* Acknowledge the message */
      MQ_ERR_CHK( MQAcknowledgeMessages(sessionHandle, messageHandle) );

      /* Free the message handle */
      MQ_ERR_CHK( MQFreeMessage(messageHandle) );
    }

  } /* while */

  /* Close the message consumer */
  MQ_ERR_CHK( MQCloseMessageConsumer(consumerHandle) );

  /* Close the session */
  MQ_ERR_CHK( MQCloseSession(sessionHandle) );

  /* Close the connection */

  MQ_ERR_CHK( MQCloseConnection(connectionHandle) );

  /* Free the connection handle */

  MQ_ERR_CHK( MQFreeConnection(connectionHandle) );

  return status;

Cleanup:
  {
  MQString errorString = MQGetStatusString(status);
  fprintf(stderr, "consumer(): Error: %s\n",
                  (errorString == NULL) ? "NULL":errorString);
  MQFreeString(errorString);
  }
  MQFreeProperties(propertiesHandle);
  MQFreeProperties(headersHandle);
  MQFreeMessage(messageHandle);
  MQFreeDestination( destinationHandle );
  MQCloseConnection(connectionHandle);
  MQFreeConnection(connectionHandle);

  return status;
}

/********************************************************/
/*	Zaladowanie ustawień konfiguracynjych serwera	*/
/********************************************************/

long load_serwer_configuration(char *path)
{

/* 	-------------------------------------	*/

	char*	error_string	=	NULL;
	long	status		=	0;

/* 	-------------------------------------	*/


	PRINT_LOG("Server:\tLoading configuration settings\n");
	BMD_FOK(bmdconf_init(&_GLOBAL_awizoSoapKonfiguracja));
	BMD_FOK(bmdconf_add_section2dict(_GLOBAL_awizoSoapKonfiguracja,"jms",configuration_dic,BMDCONF_SECTION_REQ));

	PRINT_LOG("server:\tLoading configuration from file %s\n",path);
	status=bmdconf_load_file(path,_GLOBAL_awizoSoapKonfiguracja,&error_string);
	if (status<BMD_OK)
	{
		PRINT_ERROR("%s\n", error_string);
		return status;
	}

	return BMD_OK;
}





int main(int argc, char *argv[])
{

  MQDestinationType  destinationType = MQ_QUEUE_DESTINATION;
  struct sigaction	mailToQueueAdd;
  struct sigaction	mailQueueClear;
  struct sigaction         templateAdd;
  struct sigaction          cacheClear;
  char *brokerHost = NULL; 
  long   brokerPort = 0; 
  char *destinationName	= NULL;


  char path[100]; 
  long status = 0;
  int i;


 	 _GLOBAL_debug_level = 0;

        /*********************************************/
	/*     zapamietuje pid glownego serwera.     */
	/*********************************************/
	_GLOBAL_awizo_twg_parent_pid = getpid();


	/*********************************************/
	/*   zainicjowanie pamięci cache i kolejki   */
	/*********************************************/
	InitQueue(&_GLOBAL_myCache);
	InitQueue(&_GLOBAL_myXmlQueue);

	/************************************************/
	/*	obsluga sygnalu od procesow potomnych	*/
	/************************************************/
	if(signal(SIGUSR, kill_me) == SIG_ERR)
	{
		PRINT_FATAL("AWIZOJMSSERVERFATAL Error in signal setting - SIGUSR!\n");
		return 0;
	}

	if(signal(SIGCHLD, sigchld_handler) == SIG_ERR)
	{
		PRINT_FATAL("AWIZOJMSSERVERFATAL Error in signal setting - SIGCHLD!\n");
		return 0;
	}

	if(signal(SIGTERM, master_proc_sig) == SIG_ERR)
	{
		PRINT_FATAL("AWIZOJMSSERVERFATAL Error in signal setting - SIGTERM!\n");
		return 0;
	}

	if(signal(SIGINT, master_proc_sig) == SIG_ERR)
	{
		PRINT_FATAL("AWIZOJMSSERVERFATAL Error in signal setting - SIGINT!\n");
		return 0;
	}

	if(signal(SIGALRM, sendMailQueue) == SIG_ERR)
	{
		PRINT_FATAL("AWIZOJMSSERVERFATAL Error in signal setting - SIGALRM!\n");
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

        /**********************************************/
	/* 	Obsługa dodania nowego szablonu       */
	/**********************************************/
	templateAdd.sa_sigaction = addTemplate;
	templateAdd.sa_flags = SA_SIGINFO | SA_RESTART;
	sigaction( SIGRTMIN+4, &templateAdd, NULL );

        /**********************************************/
	/* 	Obsługa czyszczenia pamięci cache     */
	/**********************************************/
	cacheClear.sa_sigaction = clearCache;
	cacheClear.sa_flags = SA_SIGINFO | SA_RESTART;
	sigaction( SIGRTMIN+5, &cacheClear, NULL ); 

        /**********************************************/
	/* 	Obsługa dodania maila do kolejki      */
	/**********************************************/
	mailToQueueAdd.sa_sigaction = addMailToQueue;
 	mailToQueueAdd.sa_flags = SA_SIGINFO | SA_RESTART;
	sigaction( SIGRTMIN+6, &mailToQueueAdd, NULL ); 

	/**********************************************/
	/*    Obsługa czyszczenia kolejki z mailami   */
	/**********************************************/
	mailQueueClear.sa_sigaction = clearMailQueue;
	mailQueueClear.sa_flags = SA_SIGINFO | SA_RESTART;
	sigaction( SIGRTMIN+7, &mailQueueClear, NULL ); 

	/****************************************/
	/*	parsoawnie linii polecen	*/
	/****************************************/
	if (argc<2)
	{
		printf("******* AWIZOJMS SERVER for archive ver. 2.0 - Unizeto Technologies SA *******\n");
		printf("*                                                                            *\n");
		printf("*       -c [configuration file path] default:  jms_server.conf               *\n");
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
				printf("******* AWIZOJMS SERVER for archive ver. 2.0 - Unizeto Technologies SA *******\n");
				printf("*                                                                            *\n");
				printf("*       -c [configuration file path] default: jms_server.conf                *\n");
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
		strcpy(path,"jms_server.conf");
	}

        bmd_init();

	/************************************************/
	/*	wczytanie ustawien konfiguracyjnych	*/
	/************************************************/
	status=load_serwer_configuration(path);
	if (status<BMD_OK)
	{
		PRINT_ERROR("Error:\terror in loading configuration\n");
		return -1;
	}

	status=bmdconf_get_value(_GLOBAL_awizoSoapKonfiguracja,"jms","ip",&brokerHost);
	if (status<BMD_OK)
	{
		PRINT_FATAL("AWIZOJMSSERVERFATAL %s\n",GetErrorMsg(BMDSOAP_SERVER_CONF_READ_ERROR));
		return BMDSOAP_SERVER_CONF_READ_ERROR;
	}

	status=bmdconf_get_value_int(_GLOBAL_awizoSoapKonfiguracja,"jms","port",&brokerPort);
	if (status<BMD_OK)
	{
		PRINT_FATAL("AWIZOJMSSERVERFATAL %s\n",GetErrorMsg(BMDSOAP_SERVER_CONF_READ_ERROR));
		return BMDSOAP_SERVER_CONF_READ_ERROR;
	}

	status=bmdconf_get_value(_GLOBAL_awizoSoapKonfiguracja,"jms","dest",&destinationName);
	if (status<BMD_OK)
	{
		PRINT_FATAL("AWIZOJMSSERVERFATAL %s\n",GetErrorMsg(BMDSOAP_SERVER_CONF_READ_ERROR));
		return BMDSOAP_SERVER_CONF_READ_ERROR;
	}

	status = getAwizoConfiguration( _GLOBAL_awizoSoapKonfiguracja, &_GLOBAL_awizoConfig );

        //exit(1);

	if (status!=BMD_OK)
	{
		PRINT_FATAL("AWIZOJMSSERVERFATAL %s\n",GetErrorMsg(BMDSOAP_SERVER_CONF_READ_ERROR));
		return BMDSOAP_SERVER_CONF_READ_ERROR;
	}

        /*********************************************************/
	/* Uruchomienie procesu synchronicznego wysyłania eAwiza */
	/*********************************************************/	

	switch (fork()) {

		case  0 : {
			       /*Do obslugi sygnalow*/

				_GLOBAL_awizo_twg_child_own_pid = getpid();

				if(signal(SIGUSR, kill_me) == SIG_ERR)
				{
					PRINT_ERROR("AWIZOJMSSERVER Error in signal setting - SIGUSR!\n");
					exit(0);
				}

				if(signal(SIGCHLD, SIG_IGN) == SIG_ERR)
				{
					PRINT_ERROR("AWIZOJMSSERVER Error in signal setting - SIGCHLD!\n");
					exit(0);
				}

				if(signal(SIGINT,SIG_IGN) == SIG_ERR)
				{
					PRINT_ERROR("AWIZOJMSSERVER Error in signal setting - SIGINT!\n");
					exit(0);
				}

				if(signal(SIGTERM, SIG_IGN) == SIG_ERR)
				{
					PRINT_ERROR("AWIZOJMSSERVER Error in signal setting - SIGTERM!\n");
					exit(0);
				}

				if(signal(SIGHUP,SIG_IGN) == SIG_ERR)
				{
					PRINT_ERROR("AWIZOJMSSERVER Error in signal setting - SIGHUP!\n");
					exit(0);
				}
			        mailQueueFunc();
			}

		case -1 : {
				PRINT_FATAL("AWIZOJMSSERVERFATAL Error in running awizo timer !\n");
				return 0;
			}
	}


	if (MQStatusIsError( consume(brokerHost, brokerPort, destinationName, destinationType) ))
	{
		return 1;
	}

	bmd_end();

   return 0;
}
