/*
 * jmslogfetcher.c
 *
 *  Created on: 2009-07-23
 *      Author: Artur Karczmarczyk
 */

#include <bmd/libbmdlog/libbmdlog.h>
#include <bmd/libbmdglobals/libbmdglobals.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <mqcrt.h>


#define MQ_ERR_CHK(mqCall)                             \
  if (MQStatusIsError(status = (mqCall)) == MQ_TRUE) { \
    goto Cleanup;                                      \
  }

typedef struct {
	char *date_time_begin;
	char *date_time_end;
	char *service;
	char *log_owner;
	char *log_level;
	char *operation_type;
	char *log_string;
	char *log_reason;
	char *log_solution;
	char *src_file;
	char *src_line;
	char *src_function;
	char *remote_host;
	char *remote_port;
	char *remote_pid;
	char *document_filename;
	char *document_size;
} jmslogdata_t;

int createJmsLogData(char *xmlstr, jmslogdata_t **data_output)
{
	jmslogdata_t *data = NULL;
	xmlDocPtr doc = NULL;
	bmdxpath_info_t *info = NULL;

	PRINT_INFO("Tworzenie struktury logu.\n")
	if (xmlstr == NULL)
	{
		PRINT_ERROR("Argument xmlstr nie moze byc NULL!\n");
		return BMD_ERR_PARAM1;
	}
	if (data_output == NULL)
	{
		PRINT_ERROR("Argument data_output nie może byc NULL!\n")
		return BMD_ERR_PARAM2;
	}
	if (*data_output != NULL)
	{
		PRINT_ERROR("Argument *data_output musi byc NULL!\n")
		return BMD_ERR_PARAM2;
	}


	data = (jmslogdata_t*) calloc (1, sizeof(jmslogdata_t));

	if (data == NULL)
	{
		printf("Memory error!");
		return BMD_ERR_MEMORY;
	}

	doc = xmlParseDoc((const xmlChar*) xmlstr);
	BMD_FOK(bmdxpath_init(doc, &info));

	bmdxpath_get_string(info, "//logs/log/date_time_begin", &(data->date_time_begin));
	bmdxpath_get_string(info, "//logs/log/date_time_end", &(data->date_time_end));
	bmdxpath_get_string(info, "//logs/log/service", &(data->service));
	bmdxpath_get_string(info, "//logs/log/log_owner", &(data->log_owner));
	bmdxpath_get_string(info, "//logs/log/log_level", &(data->log_level));
	bmdxpath_get_string(info, "//logs/log/operation_type", &(data->operation_type));
	bmdxpath_get_string(info, "//logs/log/log_string", &(data->log_string));
	bmdxpath_get_string(info, "//logs/log/log_reason", &(data->log_reason));
	bmdxpath_get_string(info, "//logs/log/log_solution", &(data->log_solution));
	bmdxpath_get_string(info, "//logs/log/debug/src_file", &(data->src_file));
	bmdxpath_get_string(info, "//logs/log/debug/src_line", &(data->src_line));
	bmdxpath_get_string(info, "//logs/log/debug/src_function", &(data->src_function));
	bmdxpath_get_string(info, "//logs/log/remote_host", &(data->remote_host));
	bmdxpath_get_string(info, "//logs/log/remote_port", &(data->remote_port));
	bmdxpath_get_string(info, "//logs/log/remote_pid", &(data->remote_pid));
	bmdxpath_get_string(info, "//logs/log/document_filename", &(data->document_filename));
	bmdxpath_get_string(info, "//logs/log/document_size", &(data->document_size));

	bmdxpath_destroy(&info);
	xmlFreeDoc(doc);
	//printf("\n\n%s\n\n", xmlstr);

	*data_output = data;

	return 0;
}


int destroyJmsLogData(jmslogdata_t **data)
{
	PRINT_INFO("Usuwanie struktury logu.\n")
	if (data == NULL || *data == NULL)
	{
		PRINT_WARNING("Argument data ma wartosc NULL.\n");
		return 0;
	}

	if (*data == NULL)
	{
		PRINT_WARNING("Argument *data ma wartosc NULL.\n");
		return 0;
	}

	freeif((*data)->date_time_begin);
	freeif((*data)->date_time_end);
	freeif((*data)->service);
	freeif((*data)->log_owner);
	freeif((*data)->log_level);
	freeif((*data)->operation_type);
	freeif((*data)->log_string);
	freeif((*data)->log_reason);
	freeif((*data)->log_solution);
	freeif((*data)->src_file);
	freeif((*data)->src_line);
	freeif((*data)->src_function);
	freeif((*data)->remote_host);
	freeif((*data)->remote_port);
	freeif((*data)->remote_pid);
	freeif((*data)->document_filename);
	freeif((*data)->document_size);
	freeif(*data);

	return 0;
}

int sendLog(jmslogdata_t *data, log_t *log)
{
	err_t err = 0;

	PRINT_INFO("Ustawienie i wysylka logu.\n")

	if (data == NULL)
	{
		PRINT_ERROR("Argument data ma wartosc NULL.\n");
		return BMD_ERR_PARAM1;
	}
	if (log == NULL)
	{
		PRINT_ERROR("Argument log ma wartosc NULL.\n");
		return BMD_ERR_PARAM2;
	}

	err = LogCreate(log);
	if ( BMD_ERR( err ))
	{
		PRINT_ERROR("Blad tworzenia LOGu.\n");
		return 1;
	}

//	err = LogSetParam(date_time_begin, data->date_time_begin, log);
//	if (BMD_ERR(err))
//	{
//		BMD_BTERR( err);
//		BMD_FREEERR( err );
//	}
	PRINT_DEBUG5("Ustalanie wartosci <date_time_end> na '%s'.\n", data->date_time_end);
	err = LogSetParam(date_time_end, data->date_time_end, log);
	if (BMD_ERR(err))
	{
		BMD_BTERR( err );
		BMD_FREEERR( err );
	}
	PRINT_DEBUG5("Ustalanie wartosci <service> na '%s'.\n", data->service);
	err = LogSetParam(service, data->service, log);
	if (BMD_ERR(err))
	{
		BMD_BTERR( err );
		BMD_FREEERR( err );
	}
	PRINT_DEBUG5("Ustalanie wartosci <log_owner> na '%s'.\n", data->log_owner);
	err = LogSetParam(log_owner, data->log_owner, log);
	if (BMD_ERR(err))
	{
		BMD_BTERR( err );
		BMD_FREEERR( err );
	}
	PRINT_DEBUG5("Ustalanie wartosci <log_level> na '%s'.\n", data->log_level);
	err = LogSetParam(log_level, data->log_level, log);
	if (BMD_ERR(err))
	{
		BMD_BTERR( err );
		BMD_FREEERR( err );
	}
	PRINT_DEBUG5("Ustalanie wartosci <operation_type> na '%s'.\n", data->operation_type);
	err = LogSetParam(operation_type, data->operation_type, log);
	if (BMD_ERR(err))
	{
		BMD_BTERR( err );
		BMD_FREEERR( err );
	}
	PRINT_DEBUG5("Ustalanie wartosci <log_string> na '%s'.\n", data->log_string);
	err = LogSetParam(log_string, data->log_string, log);
	if (BMD_ERR(err))
	{
		BMD_BTERR( err );
		BMD_FREEERR( err );
	}
	PRINT_DEBUG5("Ustalanie wartosci <log_reason> na '%s'.\n", data->log_reason);
	err = LogSetParam(log_reason, data->log_reason, log);
	if (BMD_ERR(err))
	{
		BMD_BTERR( err );
		BMD_FREEERR( err );
	}
	PRINT_DEBUG5("Ustalanie wartosci <log_solution> na '%s'.\n", data->log_solution);
	err = LogSetParam(log_solution, data->log_solution, log);
	if (BMD_ERR(err))
	{
		BMD_BTERR( err );
		BMD_FREEERR( err );
	}
//	PRINT_DEBUG5("Ustalanie wartosci <src_file> na '%s'.\n", data->src_file);
//	err = LogSetParam(src_file, data->src_file, log);
//	if (BMD_ERR(err))
//	{
//		BMD_BTERR( err );
//		BMD_FREEERR( err );
//	}
//	PRINT_DEBUG5("Ustalanie wartosci <src_line> na '%s'.\n", data->src_line);
//	err = LogSetParam(src_line, data->src_line, log);
//	if (BMD_ERR(err))
//	{
//		BMD_BTERR( err );
//		BMD_FREEERR( err );
//	}
//	PRINT_DEBUG5("Ustalanie wartosci <src_function> na '%s'.\n", data->src_function);
//	err = LogSetParam(src_function, data->src_function, log);
//	if (BMD_ERR(err))
//	{
//		BMD_BTERR( err );
//		BMD_FREEERR( err );
//	}
	PRINT_DEBUG5("Ustalanie wartosci <remote_host> na '%s'.\n", data->remote_host);
	err = LogSetParam(remote_host, data->remote_host, log);
	if (BMD_ERR(err))
	{
		BMD_BTERR( err );
		BMD_FREEERR( err );
	}
	PRINT_DEBUG5("Ustalanie wartosci <remote_port> na '%s'.\n", data->remote_port);
	err = LogSetParam(remote_port, data->remote_port, log);
	if (BMD_ERR(err))
	{
		BMD_BTERR( err );
		BMD_FREEERR( err );
	}
//	PRINT_DEBUG5("Ustalanie wartosci <remote_pid> na '%s'.\n", data->remote_pid);
//	err = LogSetParam(remote_pid, data->remote_pid, log);
//	if (BMD_ERR(err))
//	{
//		BMD_BTERR( err );
//		BMD_FREEERR( err );
//	}
	PRINT_DEBUG5("Ustalanie wartosci <document_filename> na '%s'.\n", data->document_filename);
	err = LogSetParam(document_filename, data->document_filename, log);
	if (BMD_ERR(err))
	{
		BMD_BTERR( err );
		BMD_FREEERR( err );
	}
	PRINT_DEBUG5("Ustalanie wartosci <document_size> na '%s'.\n", data->document_size);
	err = LogSetParam(document_size, data->document_size, log);
	if (BMD_ERR(err))
	{
		BMD_BTERR( err );
		BMD_FREEERR( err );
	}

	err = LogCommit(log);
	if (BMD_ERR(err))
	{
		PRINT_ERROR("Blad podczas wysylki LOGU - niepowodzenie.\n");
		BMD_BTERR( err );
		BMD_FREEERR( err );
		return 0;
	}


	return 0;
}

MQStatus consume(char * brokerHost, int brokerPort,
         char * destinationName, MQDestinationType destinationType)
{
  MQStatus status;
  int istatus = 0;

  /* Declare handles and initialize them */
  MQPropertiesHandle propertiesHandle = MQ_INVALID_HANDLE;
  MQConnectionHandle connectionHandle = MQ_INVALID_HANDLE;
  MQSessionHandle sessionHandle = MQ_INVALID_HANDLE;
  MQDestinationHandle destinationHandle = MQ_INVALID_HANDLE;
  MQConsumerHandle consumerHandle = MQ_INVALID_HANDLE;
  MQMessageHandle messageHandle = MQ_INVALID_HANDLE;
  MQPropertiesHandle headersHandle = MQ_INVALID_HANDLE;

  log_t *log = NULL;
  err_t err = 0;

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

  err = LogInit("/etc/bmd/trunk/logserver_trunk/logclient.conf", &log);
  if ( BMD_ERR( err ) )
  {
	  PRINT_ERROR("Blad przy inicjalizacji logow.");
  }

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
      jmslogdata_t *data = NULL;

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


      /* Parse the XML and get data to log */
      istatus = createJmsLogData((char*) msgtext, &data);
      if (istatus != 0)
      {
    	  PRINT_ERROR("Nie udalo sie stworzyc struktury logu.\n");
    	  continue;
      }

      istatus = sendLog(data, log);
      if (istatus != 0)
      {
    	  PRINT_WARNING("Nie udalo sie wyslac logu.");
    	  continue;
      }

      /* Free the saved data */
      istatus = destroyJmsLogData(&data);
      if (istatus != 0)
      {
    	  PRINT_ERROR("Nie udalo sie zwolnic struktury logu.\n");
    	  break;
      }

      /* Acknowledge the message */
      MQ_ERR_CHK( MQAcknowledgeMessages(sessionHandle, messageHandle) );
      if (strcmp(msgtext, "END") == 0)
      {
         MQ_ERR_CHK( MQFreeMessage(messageHandle) );
         break;
      }

      /* Free the message handle */
      MQ_ERR_CHK( MQFreeMessage(messageHandle) );
    }

  } /* while */

  LogEnd(&log);

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

void usageExit() {
  fprintf(stderr, "Uzycie:\n\tjmslogfetcher [--hostname <broker-hostname>] [--port <broker-hostport>]\n");
  fprintf(stderr, "\t\t\t[--destination <destination-name>] [--d <debug-level>] [--help]\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "Wartosci domyslne:\n");
  fprintf(stderr, "\tHost:\t\tlocalhost\n");
  fprintf(stderr, "\tPort:\t\t7676\n");
  fprintf(stderr, "\tKolejka:\tLOGS\n");
  fprintf(stderr, "\tDebug level:\t0\n");
  exit(1);
}

int main(int argc, char *argv[])
{
  char defaultBrokerHost[]      = "localhost";
  int  defaultBrokerPort        = 7676;
  char defaultDestinationName[] = "LOGS";

  char    *brokerHost = defaultBrokerHost;
  int      brokerPort = defaultBrokerPort;
  char    *destinationName = NULL;
  MQDestinationType  destinationType = MQ_QUEUE_DESTINATION;
  int i;

  _GLOBAL_debug_level = 0;
	for (i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "--help") == 0)
		{
			usageExit();
		}
		if (i == argc - 1 || strncmp(argv[i+1], "-", 1) == 0)
		{
			usageExit();
		}
		if (strcmp(argv[i], "--hostname") == 0)
		{
			brokerHost = argv[++i];
			continue;
		}
		if (strcmp(argv[i], "--port") == 0)
		{
			brokerPort = atoi(argv[++i]);
			continue;
		}
		if (strcmp(argv[i], "--destination") == 0)
		{
			destinationName = argv[++i];
			continue;
		}
		if (strcmp(argv[i], "-d") == 0)
		{
			_GLOBAL_debug_level = atoi(argv[++i]);
			continue;
		}
		usageExit();
	}
	if (destinationName == NULL)
	{
		destinationName = &defaultDestinationName[0];
	}

	if (MQStatusIsError( consume(brokerHost, brokerPort, destinationName, destinationType) ))
	{
		return 1;
	}

	return 0;
}
