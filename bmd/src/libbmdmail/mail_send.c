#include <bmd/libbmdmail/libbmdmail.h>
#include <bmd/libbmdmime/libbmdmime.h>
#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdglobals/libbmdglobals.h>

#ifdef WIN32
	#include <windows.h>
	#include <winsock2.h>
#endif

#define BLEN 1024

/**
funkcja enumerate_msg_cb() wspolpracuej z libesmtp (Linux) i nie bedzie portowana
to callback wywolywany dla kazdej wiadomosci w sesji - sprawdza statusy wysylki wiadomosci w ramach sesji i ustawia odpowiednio komorki _GLOBAL_response)
*/
#ifndef WIN32
static void enumerate_msg_cb (	smtp_message_t message,
						void * not_sent_counter)
{
const smtp_status_t *s_status	= NULL;
const smtp_status_t *reverse_path_status = NULL;
long *not_sent_counter_int = (long*)not_sent_counter;

	while(1)
	{
		if (_GLOBAL_response[_GLOBAL_index2]==BMDMAIL_ADVICE_SENDING_STATUS_UNDEFINED) // ustawienie indexu na wiadomosc, ktorej jeszcze nie wysylano
		{
			break;
		}
		_GLOBAL_index2++;
	}
	
	s_status = (smtp_status_t *)smtp_message_transfer_status (message);
	if(s_status == NULL)
	{
		PRINT_ERROR("LIBBMDMAILERROR Unable to get tranfer status [%li]\n", _GLOBAL_index2);
		_GLOBAL_response[_GLOBAL_index2]=BMDMAIL_ADVICE_SENDING_STATUS_FAILED; // wiadomosc uznana za niewyslana, jesli nie mozna pobrac statusu
		_GLOBAL_index2++;
		(*not_sent_counter_int)++;
		return;
	}
	
	if(s_status->code == 0) // wiadomosc prawdopodobnie odrzucona
	{
		reverse_path_status =  smtp_reverse_path_status(message);
		if(reverse_path_status == NULL)
		{
			PRINT_ERROR("LIBBMDMAILERROR Unable to get tranfer status [%li]\n", _GLOBAL_index2);
			_GLOBAL_response[_GLOBAL_index2]=BMDMAIL_ADVICE_SENDING_STATUS_FAILED; // wiadomosc uznana za niewyslana, jesli nie mozna pobrac statusu
			_GLOBAL_index2++;
			(*not_sent_counter_int)++;
			return;
		}
		
		if(reverse_path_status->code >= 400 && reverse_path_status->code < 600) // blad wysylki
		{
			PRINT_ERROR("LIBBMDMAILERROR Unable to send message[%li] \n", _GLOBAL_index2);
			PRINT_ERROR("LIBBMDMAILERROR SMTP reverse path status code = %i (%i.%i.%i)\n", reverse_path_status->code, reverse_path_status->enh_class, reverse_path_status->enh_subject, reverse_path_status->enh_detail);
			if(reverse_path_status->text != NULL)
			{
				PRINT_ERROR("LIBBMDMAILERROR SMTP reverse path status information = %s \n", reverse_path_status->text);
			}
			_GLOBAL_response[_GLOBAL_index2]=BMDMAIL_ADVICE_SENDING_STATUS_FAILED;
			(*not_sent_counter_int)++;
		}
		else if(reverse_path_status->code >= 200 && reverse_path_status->code < 300) // wyslano
		{
			PRINT_TEST("LIBBMDMAIL Mail info: (%i) %s\n",reverse_path_status->code, reverse_path_status->text);
			_GLOBAL_response[_GLOBAL_index2]=BMDMAIL_ADVICE_SENDING_STATUS_SUCCESS;
		}
		else // nierozpoznany kod statusu
		{
			PRINT_ERROR("LIBBMDMAILERROR Unrecognized SMTP reverse path status code = %i [%li] \n", reverse_path_status->code, _GLOBAL_index2);
			_GLOBAL_response[_GLOBAL_index2]=BMDMAIL_ADVICE_SENDING_STATUS_FAILED; // wiadomosc uznana za niewyslana
			(*not_sent_counter_int)++;
		}
		
		reverse_path_status = NULL;
	}
	else if(s_status->code >= 400 && s_status->code < 600) // blad wysylki
	{
		PRINT_ERROR("LIBBMDMAILERROR Unable to send message[%li] \n", _GLOBAL_index2);
		PRINT_ERROR("LIBBMDMAILERROR SMTP tranfer status code = %i (%i.%i.%i)\n", s_status->code, s_status->enh_class, s_status->enh_subject, s_status->enh_detail);
		if(s_status->text != NULL)
		{
			PRINT_ERROR("LIBBMDMAILERROR SMTP tranfer status information = %s \n", s_status->text);
		}
		_GLOBAL_response[_GLOBAL_index2]=BMDMAIL_ADVICE_SENDING_STATUS_FAILED;
		(*not_sent_counter_int)++;
	}
	else if(s_status->code >= 200 && s_status->code < 300) // wyslano
	{
		PRINT_TEST("LIBBMDMAIL Mail info: (%i) %s\n",s_status->code, s_status->text);
		_GLOBAL_response[_GLOBAL_index2]=BMDMAIL_ADVICE_SENDING_STATUS_SUCCESS;
	}
	else // nierozpoznany kod statusu
	{
		PRINT_ERROR("LIBBMDMAILERROR Unrecognized SMTP tranfer status code = %i [%li] \n", s_status->code, _GLOBAL_index2);
		_GLOBAL_response[_GLOBAL_index2]=BMDMAIL_ADVICE_SENDING_STATUS_FAILED; // wiadomosc uznana za niewyslana
		(*not_sent_counter_int)++;
	}
	
	s_status = NULL;
	_GLOBAL_index2++;

	return;
}
#endif /*ifndef WIN32*/

/************************************************/
/*	dodawanie wielu odbiorcow awiza faktury	*/
/************************************************/
long bmd_get_recipients(	char *recipients,
				char ***recipient_list,
				long *recipients_count)
{
long i				= 0;
long index			= 0;
char single_recipient[100];

	PRINT_INFO("LIBBMDMAILINF bmd_get_recipients\n");

	(*recipients_count)=0;
	for (i=0; i<100; i++)
	{
			single_recipient[i]='\0';
	}
	for (i=0; i<(long)strlen(recipients); i++)
	{
		single_recipient[index++]=recipients[i];
		single_recipient[index]='\0';
		if (recipients[i]==' ')
		{
			index=0;
		}
		if (recipients[i]==';')
		{
			single_recipient[index-1]='\0';
			if (strlen(single_recipient)>0)
			{
				(*recipients_count)=(*recipients_count)+1;
				(*recipient_list)=(char **)realloc((*recipient_list),sizeof(char *)*(*recipients_count));
				asprintf(&((*recipient_list)[(*recipients_count)-1]), "%s", single_recipient);
			}
			index=0;
		}
		if ((recipients[i]=='\0') || ( i == ((long)strlen(recipients)-1) ))
		{
			single_recipient[index]='\0';
			index=0;
			if (strlen(single_recipient)>0)
			{
				(*recipients_count)=(*recipients_count)+1;
				(*recipient_list)=(char **)realloc((*recipient_list),sizeof(char *)*(*recipients_count));
				asprintf(&((*recipient_list)[(*recipients_count)-1]), "%s", single_recipient);
			}
			break;
		}
	}

	return BMD_OK;
}

/*wysylka wielu maili w jednej sesji SMTP*/

long bmd_send_envoice_avizo(	mail_send_info_t *mi,
					long count,
					char **result)
{

#ifndef WIN32
long status				= 0;
long i					= 0;
long j					= 0;
long k					= 0;
char *port				= NULL;
smtp_message_t *message		= NULL;
smtp_recipient_t recipient	= NULL;
char *buf				= NULL;
long not_sent_counter	= 0; // licznik wiadomosci, ktorych nie udalo sie wyslac
long er				= 0;
char *cos				= NULL;
long prop_data_index		= -1;
char **recipients_list		= NULL;
long recipients_count		= 0;

	/******************/
	/*	bufor bledu	*/
	/******************/
	PRINT_INFO("LIBBMDMAILINF bmd_send_envoice_avizo\n");

	buf = (char *) malloc (sizeof(char) * BLEN);
	memset(buf, 0, sizeof(char)*BLEN);

	/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if(mi==NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(count < 0)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(result==NULL)		{	BMD_FOK(BMD_ERR_PARAM3);	}
	if (*result!=NULL)	{	BMD_FOK(BMD_ERR_PARAM3);	}

	for (i=0; i< count; i++)
	{
		if((mi[i].server_port!=NULL) && (mi[i].mail_to_send==1))
		{
			asprintf(&port,"%s",mi[i].server_port);
			if(port == NULL)	{	BMD_FOK(NO_MEMORY);	}
			PRINT_VDEBUG("LIBBMDMAILVDEBUG Prop data index %li\n", i);
			prop_data_index = i;
			break;
		}
	}

	if(prop_data_index < 0)
	{
		PRINT_VDEBUG("LIBBMDMAILVDEBUG No mail to send\n");
		BMD_FOK(PLUGIN_SESSION_NO_MAIL_TO_SEND);
	}

	if (port==NULL)				{	BMD_FOK(PLUGIN_SESSION_MAIL_SERVER_PORT_NOT_SPECIFIED);	}



	if (_GLOBAL_response!=NULL)
	{
		free(_GLOBAL_response);
		_GLOBAL_response=NULL;
		_GLOBAL_index2=0;
	}

	_GLOBAL_response=(char *)malloc(sizeof(char)*count+2);
	memset(_GLOBAL_response,0,sizeof(char)*count+1);
	for (i=0; i<count; i++)
	{
		_GLOBAL_response[i]=BMDMAIL_ADVICE_SENDING_STATUS_UNDEFINED;
	}
	_GLOBAL_response[count]='\0';

	message=(smtp_message_t *)malloc(sizeof(smtp_message_t)*count+2);
	memset(message,0,sizeof(smtp_message_t)*count+1);

	/****************************************/
	/*	okreslenie szczegółów sesji	*/
	/****************************************/
	for (i=0; i<count; i++)
	{
		/****************************************/
		/*	stworzenie sesji wysylki maili	*/
		/****************************************/
		if (i==prop_data_index)
		{
			mi[prop_data_index].session=smtp_create_session();
			if(mi[prop_data_index].session == NULL)
			{
				BMD_FOK(BMD_ERR_OP_FAILED);
			}
		}

		if (	(mi[i].recipient_to==NULL) ||
			(mi[i].reverse_path==NULL) ||
			(mi[i].mail_body==NULL) ||
			(mi[i].mail_to_send==0))
		{
			_GLOBAL_response[i]=BMDMAIL_ADVICE_SENDING_STATUS_FAILED;
			PRINT_VDEBUG("LIBBMDMAILVDEBUG No recipent for %li iteration\n", i);
			continue;
		}
		PRINT_VDEBUG("LIBBMDMAILVDEBUG Mail recipent %s\n", mi[i].recipient_to);

		/************************************/
		/*	dodanie wiadomosci do wysylki	*/
		/************************************/
		message[i]=smtp_add_message(mi[prop_data_index].session);
		if (message==NULL)			{	BMD_FOK(PLUGIN_SESSION_ADD_MSG_TO_SESSION_ERROR);	}

		status=smtp_set_messagecb(message[i],libesmtp_callback,(void*)(mi[i].mail_body));
		if (status==0)
		{
			PRINT_ERROR("LIBBMDMAILERR Error in setting messagecb. Error=%li\n",status);
			er=smtp_errno();
			cos=(char *)smtp_strerror(er, buf, BLEN);
			PRINT_ERROR("LIBBMDMAILERR %s\n",cos);
			return BMD_ERR_OP_FAILED;
		}

		/******************************************************/
		/*	wuodrebnienie wielu adresow z jednej linii	*/
		/******************************************************/
		bmd_get_recipients(mi[i].recipient_to, &recipients_list, &recipients_count);

		/************************************/
		/*	dodanie adresatow maila awiza	*/
		/************************************/
		for (j=0; j<recipients_count; j++)
		{
			recipient=smtp_add_recipient(message[i], recipients_list[j]);
			if (recipient==NULL)
			{
				PRINT_ERROR("LIBBMDMAILERR Error in setting adding recipient. Error=%i\n",BMD_ERR_OP_FAILED);
				er=smtp_errno();
				cos=(char *)smtp_strerror(er, buf, BLEN);
				PRINT_ERROR("LIBBMDMAILERR %s\n",cos);
				return BMD_ERR_OP_FAILED;
			}
		}

		status=smtp_set_reverse_path(message[i], mi[i].reverse_path);
		if (status==0)
		{
			PRINT_ERROR("LIBBMDMAILERR Error in adding reverse_path. Error=%li\n",status);
			er=smtp_errno();
			cos=(char *)smtp_strerror(er, buf, BLEN);
			PRINT_ERROR("LIBBMDMAILERR %s\n",cos);
			return BMD_ERR_OP_FAILED;
		}

		if (i==prop_data_index)
		{
			status=smtp_set_server(mi[prop_data_index].session,port);
			if (status==0)
			{
				PRINT_ERROR("LIBBMDMAILERR Error in setting server's port number. Error=%li\n",status);
				er=smtp_errno();
				cos=(char *)smtp_strerror(er, buf, BLEN);
				PRINT_ERROR("LIBBMDMAILERR %s\n",cos);
				return BMD_ERR_OP_FAILED;

			}
		}
	}

	/********************************/
	/*	faktyczna wysylka maili	*/
	/********************************/
	if (prop_data_index>=0)
	{
		status=smtp_start_session(mi[prop_data_index].session);
		if (status==0)
		{
			er=smtp_errno();
			cos=(char *)smtp_strerror(er, buf, BLEN);
			PRINT_ERROR("LIBBMDMAILERR Error in starting session. %s. Error=%li\n", cos, er);
			smtp_destroy_session(mi[prop_data_index].session);mi[prop_data_index].session = NULL;

			for (k=0; k<count; k++)
			{
				_GLOBAL_response[k]=BMDMAIL_ADVICE_SENDING_STATUS_FAILED;
			}
		}

		// status == 1 zarowno gdy wyslane jak i odrzucone
		if (status == 1)
		{
			smtp_enumerate_messages(mi[prop_data_index].session,enumerate_msg_cb,&not_sent_counter); // sprawdzenie statusu wysylki wiadomosci (callback)
			smtp_destroy_session(mi[prop_data_index].session);
			mi[prop_data_index].session = NULL;
		}
	}

	/********************************/
	/*	bufor wynikowy z bledem	*/
	/********************************/
	*result=(char *)malloc(count+2);
	_GLOBAL_response[count]='\0';
	asprintf(result,"%s",_GLOBAL_response);

	/************************/
	/*	porzadki	*/
	/************************/
	free(port); port=NULL;
	free(buf); buf=NULL;
	free(message); message=NULL;

	if (status==0)
	{
		return er;
	}

	return BMD_OK;
#endif /*ifndef WIN32*/

/*************************************************/


#ifdef WIN32
	long         iProtocolPort        = 0;
	char        szSmtpServerName[64] = "";
	char        szBuffer[4096]       = "";
	char        szMsgLine[255]       = "";
	char		*username			 = NULL;
	char		*password			 = NULL;
	SOCKET      hServer;
	WSADATA     WSData;
	LPHOSTENT   lpHostEntry;
	LPSERVENT   lpServEntry;
	SOCKADDR_IN SockAddr;

	long status=0;
	long i=0, j=0;
	long recipientsIter=0;
	long statusCode=0;
	char *port=NULL; /* adres serwera SMTP */
	char *sentStatusTable=NULL; /* kazda komorka tablicy odpowiada jednej wiadomosci (3-zainicjowana, 1-wyslana, 0-niewyslana) */
	char **recipientsList=NULL;
	long recipientsCount=0;
	char *user=NULL;
	char *pass=NULL;

	PRINT_INFO("LIBBMDMAILINF bmd_send_mail\n");

/******************************/
	/*	walidacja parametrow	*/
	/******************************/
	if(mi==NULL)
		{ BMD_FOK(BMD_ERR_PARAM1); }
	if(count < 0)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(result==NULL)
		{ BMD_FOK(BMD_ERR_PARAM3); }
	if (*result!=NULL)
		{ BMD_FOK(BMD_ERR_PARAM3); }

	for (i=0; i<count; i++)
	{
		if((mi[i].server_port!=NULL) && (mi[i].mail_to_send==1))
		{
			asprintf(&port, "%s", mi[i].server_port);
			if (port==NULL)
				{ BMD_FOK(NO_MEMORY); }

			if (mi[i].with_auth == BMD_MAIL_WITH_AUTH)
			{
				asprintf(&user, "%s", mi[i].auth_data.username);
				if(user == NULL)
				{
					free(port);
					BMD_FOK(NO_MEMORY);
				}
				asprintf(&pass, "%s", mi[i].auth_data.password);
				if( pass == NULL)
				{
					free(port);
					free(user);
					BMD_FOK(NO_MEMORY);
				}
			}
			break;
		}
	}

	/*jesli nie ma w tablicy zadnej wiadomosci do wyslania*/
	if(port == NULL)
	{
		BMD_FOK(BMD_ERR_PARAM1);
	}

	for(i=0;;i++)
	{
		if (port[i] != 0 && port[i] != ':')
		{
			szSmtpServerName[i] = port[i];
		}
		else
		{
			szSmtpServerName[i] = 0;
			break;
		}
	}
	free(port); port=NULL;

	/* Attempt to intialize WinSock (1.1 or later) */
	if(WSAStartup(MAKEWORD(1, 1), &WSData))
	{
		/*Cannot find Winsock v. 1.1*/
		free(user); free(pass);
		return -1;
	}

	/* Lookup email server's IP address. */
	lpHostEntry = gethostbyname(szSmtpServerName);
	if(lpHostEntry == NULL)
	{
		/* Cannot find SMTP mail server */
		free(user); free(pass);
		WSACleanup();
	    return -2;
	}

	/* Create a TCP/IP socket, no specific protocol */
	hServer = socket(PF_INET, SOCK_STREAM, 0);
	if(hServer == INVALID_SOCKET)
	{
		/* Cannot open mail server socket */
		free(user); free(pass);
		WSACleanup();
		return -3;
	}

	/* Get the mail service port */
	lpServEntry = getservbyname("mail", 0);

	/*  Use the SMTP default port if no other port is specified */
	if(lpServEntry == NULL)
	{
		iProtocolPort = htons(IPPORT_SMTP);
	}
	else
	{
		iProtocolPort = lpServEntry->s_port;
	}

	/*  Setup a Socket Address structure */
	SockAddr.sin_family = AF_INET;
	SockAddr.sin_port   = (u_short) iProtocolPort;
	SockAddr.sin_addr   = *((LPIN_ADDR)*lpHostEntry->h_addr_list);

	/*  Connect the Socket */
	status = connect(hServer, (PSOCKADDR) &SockAddr, sizeof(SockAddr));
	if(status != 0)
	{
		/* Error connecting to Server socket */
		free(user); free(pass);
		closesocket(hServer);
		WSACleanup();
		return -4;
	}

	/* rozmowa z serwerem */
	/* Receive initial response from SMTP server */
	status = recv(hServer, szBuffer, sizeof(szBuffer), 0);
	if (status == SOCKET_ERROR)
	{
		free(user); free(pass);
		closesocket(hServer);
		WSACleanup();
		return -5;
	}

	status=getSMTPResponseStatusCode(szBuffer, &statusCode);
	if(statusCode >= 400)
	{
		free(user); free(pass);
		closesocket(hServer);
		WSACleanup();
		return -5;
	}

	if (mi->with_auth == BMD_MAIL_WITH_AUTH)
	{
		/*  Send EHLO server.com */
		sprintf(szMsgLine, "EHLO %s\r\n", szSmtpServerName);
	}
	else
	{
		/*  Send HELO server.com */
		sprintf(szMsgLine, "HELO %s\r\n", szSmtpServerName);
	}
	status = send(hServer, szMsgLine, (int)strlen(szMsgLine), 0);
	if (status == SOCKET_ERROR)
	{
		free(user); free(pass);
		closesocket(hServer);
		WSACleanup();
		return -6;
	}
	status = recv(hServer, szBuffer, sizeof(szBuffer), 0);
	if (status == SOCKET_ERROR)
	{
		free(user); free(pass);
		closesocket(hServer);
		WSACleanup();
		return -7;
	}


	status=getSMTPResponseStatusCode(szBuffer, &statusCode);
	if(statusCode >= 400)
	{
		free(user); free(pass);
		closesocket(hServer);
		WSACleanup();
		return -8;
	}

	/* ewentualne uwierzytelnianie */
	if (mi->with_auth == BMD_MAIL_WITH_AUTH)
	{
		/*  zadanie logowania */
		sprintf(szMsgLine, "AUTH LOGIN\r\n");
		status = send(hServer, szMsgLine, (int)strlen(szMsgLine), 0);
		if (status == SOCKET_ERROR)
		{
			free(user); free(pass);
			closesocket(hServer);
			WSACleanup();
			return -9;
		}
		status = recv(hServer, szBuffer, sizeof(szBuffer), 0);
		if (status == SOCKET_ERROR)
		{
			free(user); free(pass);
			closesocket(hServer);
			WSACleanup();
			return -10;
		}

		status=getSMTPResponseStatusCode(szBuffer, &statusCode);
		if(statusCode >= 400)
		{
			free(user); free(pass);
			closesocket(hServer);
			WSACleanup();
			return -11;
		}

		/* uzytkownik */
		username = (char *)spc_base64_encode((unsigned char*)user, strlen(user), 0);
		free(user); user=NULL;
		sprintf(szMsgLine, "%s\r\n", username);
		free(username); username=NULL;
		status = send(hServer, szMsgLine, (long)strlen(szMsgLine), 0);
		if (status == SOCKET_ERROR)
		{
			free(pass);
			closesocket(hServer);
			WSACleanup();
			return -12;
		}
		status = recv(hServer, szBuffer, sizeof(szBuffer), 0);
		if (status == SOCKET_ERROR)
		{
			free(pass);
			closesocket(hServer);
			WSACleanup();
			return -13;
		}

		status=getSMTPResponseStatusCode(szBuffer, &statusCode);
		if(statusCode >= 400)
		{
			free(pass);
			closesocket(hServer);
			WSACleanup();
			return -14;
		}

		/* haslo */
		password = (char *)spc_base64_encode((unsigned char*)pass, strlen(pass), 0);
		free(pass); pass=NULL;
		sprintf(szMsgLine, "%s\r\n", password);
		free(password); password=NULL;
		status = send(hServer, szMsgLine, (long)strlen(szMsgLine), 0);
		if (status == SOCKET_ERROR)
		{
			closesocket(hServer);
			WSACleanup();
			return -15;
		}
		status = recv(hServer, szBuffer, sizeof(szBuffer), 0);
		if (status == SOCKET_ERROR)
		{
			closesocket(hServer);
			WSACleanup();
			return -16;
		}

		status=getSMTPResponseStatusCode(szBuffer, &statusCode);
		if(statusCode >= 400)
		{
			closesocket(hServer);
			WSACleanup();
			return -17;
		}
	}

	sentStatusTable=(char *)calloc(count+1, sizeof(char));
	if(sentStatusTable == NULL)
	{
		WSACleanup();
		BMD_FOK(NO_MEMORY);
	}
	memset(sentStatusTable, (int)BMDMAIL_ADVICE_SENDING_STATUS_FAILED, count);
	*result=sentStatusTable;

	/*------- petla wysylki maili --------*/
	for(i=0; i<count; i++)
	{
		if ( (mi[i].recipient_to==NULL) || (mi[i].reverse_path==NULL) || (mi[i].mail_body==NULL) || (mi[i].mail_to_send==0) )
			{ continue;	}

		/* Send MAIL FROM: <sender@mydomain.com> */
		sprintf(szMsgLine, "MAIL FROM:<%s>\r\n", mi[i].reverse_path);
		status = send(hServer, szMsgLine, (int)strlen(szMsgLine), 0);
		if (status == SOCKET_ERROR)
		{
			closesocket(hServer);
			WSACleanup();
			return -18;
		}
		status = recv(hServer, szBuffer, sizeof(szBuffer), 0);
		if (status == SOCKET_ERROR)
		{
			closesocket(hServer);
			WSACleanup();
			return -19;
		}

		status=getSMTPResponseStatusCode(szBuffer, &statusCode);
		if(statusCode >= 400)
		{
			closesocket(hServer);
			WSACleanup();
			return -20;
		}

		// Send RCPT TO: <receiver@domain.com>
		bmd_get_recipients(mi[i].recipient_to, &recipientsList, &recipientsCount);
		for(recipientsIter=0; recipientsIter<recipientsCount; recipientsIter++)
		{
			sprintf(szMsgLine, "RCPT TO:<%s>\r\n", recipientsList[recipientsIter]);
			status = send(hServer, szMsgLine, (int)strlen(szMsgLine), 0);
			if (status == SOCKET_ERROR)
			{
				for(j=0; j<recipientsCount; j++)
					{ free(recipientsList[j]); }
				free(recipientsList); recipientsList=NULL;
				recipientsCount=0;
				closesocket(hServer);
				WSACleanup();
				return -21;
			}
			status = recv(hServer, szBuffer, sizeof(szBuffer), 0);
			if (status == SOCKET_ERROR)
			{
				for(j=0; j<recipientsCount; j++)
					{ free(recipientsList[j]); }
				free(recipientsList); recipientsList=NULL;
				recipientsCount=0;
				closesocket(hServer);
				WSACleanup();
				return -22;
			}

			status=getSMTPResponseStatusCode(szBuffer, &statusCode);
			if(statusCode >= 400)
			{
				for(j=0; j<recipientsCount; j++)
					{ free(recipientsList[j]); }
				free(recipientsList); recipientsList=NULL;
				recipientsCount=0;
				closesocket(hServer);
				WSACleanup();
				return -23;
			}
		}
		for(j=0; j<recipientsCount; j++)
			{ free(recipientsList[j]); }
		free(recipientsList); recipientsList=NULL;
		recipientsCount=0;

		// Send DATA
		sprintf(szMsgLine, "DATA%s", "\r\n");
		status = send(hServer, szMsgLine, (int)strlen(szMsgLine), 0);
		if (status == SOCKET_ERROR)
		{
			closesocket(hServer);
			WSACleanup();
			return -24;
		}
		status = recv(hServer, szBuffer, sizeof(szBuffer), 0);
		if (status == SOCKET_ERROR)
		{
			closesocket(hServer);
			WSACleanup();
			return -25;
		}

		status=getSMTPResponseStatusCode(szBuffer, &statusCode);
		if(statusCode >= 400)
		{
			closesocket(hServer);
			WSACleanup();
			return -26;
		}
		//wysylanie ciala wiadomosci
		status = send(hServer, mi[i].mail_body, (int)strlen(mi[i].mail_body), 0);
		if (status == SOCKET_ERROR)
		{
			closesocket(hServer);
			WSACleanup();
			return -27;
		}

		// Send blank line and a period
		sprintf(szMsgLine, "\r\n.\r\n");
		status = send(hServer, szMsgLine, (int)strlen(szMsgLine), 0);
		if (status == SOCKET_ERROR)
		{
			closesocket(hServer);
			WSACleanup();
			return -28;
		}
		status = recv(hServer, szBuffer, sizeof(szBuffer), 0);
		if (status == SOCKET_ERROR)
		{
			closesocket(hServer);
			WSACleanup();
			return -29;
		}

		status=getSMTPResponseStatusCode(szBuffer, &statusCode);
		if(statusCode >= 400)
		{
			closesocket(hServer);
			WSACleanup();
			return -30;
		}
		sentStatusTable[i]=BMDMAIL_ADVICE_SENDING_STATUS_SUCCESS;
	} /* end for - dla kazdej wiadomosci */

	// Send QUIT
	sprintf(szMsgLine, "QUIT\r\n");
	status = send(hServer, szMsgLine, (int)strlen(szMsgLine), 0);
	status = recv(hServer, szBuffer, sizeof(szBuffer), 0);

	// Report message has been sent

	// Close server socket and prepare to exit.
	closesocket(hServer);

	WSACleanup();
	return 0;

#endif /*ifdef WIN32*/
}




/** Funkcja wysyłanaia maila
Aby wysłać maila należy przygotować nagłówek i ciało wiadomości za pomocą funkcji \c set_header_info()
*/
/*funckja sportowana przez AK*/
long bmd_send_mail(mail_send_info_t *mi)
{
long status;

#ifdef WIN32
	long         iProtocolPort        = 0;
	char        szSmtpServerName[64] = "";
	char        szBuffer[4096]       = "";
	char        szMsgLine[255]       = "";
	char		*username			 = NULL;
	char		*password			 = NULL;
	SOCKET      hServer;
	WSADATA     WSData;
	LPHOSTENT   lpHostEntry;
	LPSERVENT   lpServEntry;
	SOCKADDR_IN SockAddr;
	long statusCode = 0;

	long i;

	PRINT_INFO("LIBBMDMAILINF bmd_send_mail\n");

	for (i=0;;i++)
	{
		if (mi->server_port[i] != 0 && mi->server_port[i] != ':')
		{
			szSmtpServerName[i] = mi->server_port[i];
		}
		else
		{
			szSmtpServerName[i] = 0;
			break;
		}
	}

	// Attempt to intialize WinSock (1.1 or later)
	if(WSAStartup(MAKEWORD(1, 1), &WSData))
	{
		//Cannot find Winsock v. 1.1
		//cout << "Cannot find Winsock v" << VERSION_MAJOR << "." << VERSION_MINOR << " or later!" << endl;
		return -1;
	}

	// Lookup email server's IP address.
	lpHostEntry = gethostbyname(szSmtpServerName);
	if(!lpHostEntry)
	{
		//cout << "Cannot find SMTP mail server " << szSmtpServerName << endl;
		WSACleanup();
	    return -2;
	}

	// Create a TCP/IP socket, no specific protocol
	hServer = socket(PF_INET, SOCK_STREAM, 0);
	if(hServer == INVALID_SOCKET)
	{
		//cout << "Cannot open mail server socket" << endl;
		WSACleanup();
		return -3;
	}

	// Get the mail service port
	lpServEntry = getservbyname("mail", 0);

	// Use the SMTP default port if no other port is specified
	if(!lpServEntry)
		iProtocolPort = htons(IPPORT_SMTP);
	else
		iProtocolPort = lpServEntry->s_port;

	// Setup a Socket Address structure
	SockAddr.sin_family = AF_INET;
	SockAddr.sin_port   = (u_short) iProtocolPort;
	SockAddr.sin_addr   = *((LPIN_ADDR)*lpHostEntry->h_addr_list);

	// Connect the Socket
	status = connect(hServer, (PSOCKADDR) &SockAddr, sizeof(SockAddr));
	if(status)
	{
		//cout << "Error connecting to Server socket" << endl;
		closesocket(hServer);
		WSACleanup();
		return -4;
	}

	//rozmowa z serwerem
	// Receive initial response from SMTP server
	status = recv(hServer, szBuffer, sizeof(szBuffer), 0);
	if (status == SOCKET_ERROR)
	{
		closesocket(hServer);
		WSACleanup();
		return -5;
	}

	status=getSMTPResponseStatusCode(szBuffer, &statusCode);
	if(statusCode >= 400)
	{
		closesocket(hServer);
		WSACleanup();
		return -6;
	}

	if (mi->with_auth == BMD_MAIL_WITH_AUTH)
	{
		// Send EHLO server.com
		sprintf(szMsgLine, "EHLO %s\r\n", szSmtpServerName);
	}
	else
	{
		// Send HELO server.com
		sprintf(szMsgLine, "HELO %s\r\n", szSmtpServerName);
	}
	status = send(hServer, szMsgLine, (int)strlen(szMsgLine), 0);
	if (status == SOCKET_ERROR)
	{
		closesocket(hServer);
		WSACleanup();
		return -7;
	}
	status = recv(hServer, szBuffer, sizeof(szBuffer), 0);
	if (status == SOCKET_ERROR)
	{
		closesocket(hServer);
		WSACleanup();
		return -8;
	}

	status=getSMTPResponseStatusCode(szBuffer, &statusCode);
	if(statusCode >= 400)
	{
		closesocket(hServer);
		WSACleanup();
		return -9;
	}

	// ewentualna autentykacja
	if (mi->with_auth == BMD_MAIL_WITH_AUTH)
	{
		// zadanie logowania
		sprintf(szMsgLine, "AUTH LOGIN\r\n");
		status = send(hServer, szMsgLine, (int)strlen(szMsgLine), 0);
		if (status == SOCKET_ERROR)
		{
			closesocket(hServer);
			WSACleanup();
			return -10;
		}
		status = recv(hServer, szBuffer, sizeof(szBuffer), 0);
		if (status == SOCKET_ERROR)
		{
			closesocket(hServer);
			WSACleanup();
			return -11;
		}

		status=getSMTPResponseStatusCode(szBuffer, &statusCode);
		if(statusCode >= 400)
		{
			closesocket(hServer);
			WSACleanup();
			return -12;
		}

		// uzytkownik
		username = (char *)spc_base64_encode((unsigned char*)mi->auth_data.username, strlen(mi->auth_data.username), 0);
		sprintf(szMsgLine, "%s\r\n", username);
		free0(username);
		status = send(hServer, szMsgLine, (long)strlen(szMsgLine), 0);
		if (status == SOCKET_ERROR)
		{
			closesocket(hServer);
			WSACleanup();
			return -13;
		}
		status = recv(hServer, szBuffer, sizeof(szBuffer), 0);
		if (status == SOCKET_ERROR)
		{
			closesocket(hServer);
			WSACleanup();
			return -14;
		}

		status=getSMTPResponseStatusCode(szBuffer, &statusCode);
		if(statusCode >= 400)
		{
			closesocket(hServer);
			WSACleanup();
			return -15;
		}

		// hasło
		password = (char *)spc_base64_encode((unsigned char*)mi->auth_data.password, strlen(mi->auth_data.password), 0);
		sprintf(szMsgLine, "%s\r\n", password);
		free0(password);
		status = send(hServer, szMsgLine, (long)strlen(szMsgLine), 0);
		if (status == SOCKET_ERROR)
		{
			closesocket(hServer);
			WSACleanup();
			return -16;
		}
		status = recv(hServer, szBuffer, sizeof(szBuffer), 0);
		if (status == SOCKET_ERROR)
		{
			closesocket(hServer);
			WSACleanup();
			return -17;
		}

		status=getSMTPResponseStatusCode(szBuffer, &statusCode);
		if(statusCode >= 400)
		{
			closesocket(hServer);
			WSACleanup();
			return -18;
		}
	}

	// Send MAIL FROM: <sender@mydomain.com>
	sprintf(szMsgLine, "MAIL FROM:<%s>\r\n", mi->reverse_path);
	status = send(hServer, szMsgLine, (int)strlen(szMsgLine), 0);
	if (status == SOCKET_ERROR)
	{
		closesocket(hServer);
		WSACleanup();
		return -19;
	}
	status = recv(hServer, szBuffer, sizeof(szBuffer), 0);
	if (status == SOCKET_ERROR)
	{
		closesocket(hServer);
		WSACleanup();
		return -20;
	}

	status=getSMTPResponseStatusCode(szBuffer, &statusCode);
	if(statusCode >= 400)
	{
		closesocket(hServer);
		WSACleanup();
		return -21;
	}

	// Send RCPT TO: <receiver@domain.com>
	sprintf(szMsgLine, "RCPT TO:<%s>\r\n", mi->recipient_to);
	status = send(hServer, szMsgLine, (int)strlen(szMsgLine), 0);
	if (status == SOCKET_ERROR)
	{
		closesocket(hServer);
		WSACleanup();
		return -22;
	}
	status = recv(hServer, szBuffer, sizeof(szBuffer), 0);
	if (status == SOCKET_ERROR)
	{
		closesocket(hServer);
		WSACleanup();
		return -23;
	}

	status=getSMTPResponseStatusCode(szBuffer, &statusCode);
	if(statusCode >= 400)
	{
		closesocket(hServer);
		WSACleanup();
		return -24;
	}

	// Send DATA
	sprintf(szMsgLine, "DATA%s", "\r\n");
	status = send(hServer, szMsgLine, (int)strlen(szMsgLine), 0);
	if (status == SOCKET_ERROR)
	{
		closesocket(hServer);
		WSACleanup();
		return -25;
	}
	status = recv(hServer, szBuffer, sizeof(szBuffer), 0);
	if (status == SOCKET_ERROR)
	{
		closesocket(hServer);
		WSACleanup();
		return -26;
	}

	status=getSMTPResponseStatusCode(szBuffer, &statusCode);
	if(statusCode >= 400)
	{
		closesocket(hServer);
		WSACleanup();
		return -27;
	}

	//wysylanie ciala wiadomosci
	status = send(hServer, mi->mail_body, (int)strlen(mi->mail_body), 0);
	if (status == SOCKET_ERROR)
	{
		closesocket(hServer);
		WSACleanup();
		return -28;
	}

	// Send blank line and a period
	sprintf(szMsgLine, "\r\n.\r\n");
	status = send(hServer, szMsgLine, (int)strlen(szMsgLine), 0);
	if (status == SOCKET_ERROR)
	{
		closesocket(hServer);
		WSACleanup();
		return -29;
	}
	status = recv(hServer, szBuffer, sizeof(szBuffer), 0);
	if (status == SOCKET_ERROR)
	{
		closesocket(hServer);
		WSACleanup();
		return -30;
	}

	status=getSMTPResponseStatusCode(szBuffer, &statusCode);
	if(statusCode >= 400)
	{
		closesocket(hServer);
		WSACleanup();
		return -31;
	}

	// Send QUIT
	sprintf(szMsgLine, "QUIT\r\n");
	status = send(hServer, szMsgLine, (int)strlen(szMsgLine), 0);
	status = recv(hServer, szBuffer, sizeof(szBuffer), 0);

	// Report message has been sent
	//cout << "Sent " << argv[4] << " as email message to " << szToAddr << endl;

	// Close server socket and prepare to exit.
	closesocket(hServer);
	WSACleanup();

	return 0;
#endif

#ifndef WIN32
smtp_session_t session=NULL;
smtp_message_t message=NULL;
smtp_recipient_t recipient=NULL;
smtp_status_t *s_status = NULL;
auth_context_t authctx = NULL;
long return_code;
	PRINT_INFO("LIBBMDMAILINF Sending mail (BMD)\n");
	if(mi==NULL)
	{
		PRINT_DEBUG("LIBBMDMAILERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}

	if(mi->server_port==NULL)
	{
		PRINT_DEBUG("LIBBMDMAILERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(mi->recipient_to==NULL)
	{
		PRINT_DEBUG("LIBBMDMAILERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(mi->reverse_path==NULL)
	{
		PRINT_DEBUG("LIBBMDMAILERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}
	if(mi->mail_body==NULL)
	{
		PRINT_DEBUG("LIBBMDMAILERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}

	/*print_mail_send_info(mi);*/

	if(mi->with_auth==BMD_MAIL_WITH_AUTH)
	{
		PRINT_VDEBUG("LIBBMDMAILINF SMTP preparing to authenticate with server.\n");
		auth_client_init();
	}

	PRINT_DEBUG1("LIBBMDMAILINF SMTP create session begin.\n");

	session=smtp_create_session();
	if(session == NULL)
	{
		PRINT_ERROR("LIBBMDMAILERR Error in creating SMPT session. Error=%i\n",BMD_ERR_OP_FAILED);
		return BMD_ERR_OP_FAILED;
	}


	PRINT_DEBUG1("LIBBMDMAILINF SMTP adding message.\n");
	message=smtp_add_message(session);
	if (message==NULL)
	{
		PRINT_ERROR("LIBBMDMAILERR Error in adding message. Error=%i\n",BMD_ERR_OP_FAILED);
		return BMD_ERR_OP_FAILED;
	}

	PRINT_DEBUG1("LIBBMDMAILINF SMTP setting smtp server port to %s.\n", mi->server_port);

	status=smtp_set_server(session,mi->server_port);
	if (status==0)
	{
		PRINT_ERROR("LIBBMDMAILERR Error in setting server's port number. Error=%li\n",status);
		return BMD_ERR_OP_FAILED;
	}

	if(mi->with_auth==BMD_MAIL_WITH_AUTH)
	{
		PRINT_DEBUG1("LIBBMDMAILINF SMTP creating authentication context.\n");
		authctx = auth_create_context();

		PRINT_DEBUG1("LIBBMDMAILINF SMTP Setting mechanism flags.\n");
		auth_set_mechanism_flags (authctx, AUTH_PLUGIN_PLAIN, 0);

		PRINT_DEBUG1("LIBBMDMAILINF SMTP Setting interaction context.\n");
		auth_set_interact_cb (authctx, authinteract,&(mi->auth_data));

		PRINT_VDEBUG("LIBBMDMAILINF SMTP setting authentication context.\n");
		smtp_auth_set_context(session, authctx);
	}

	PRINT_DEBUG1("LIBBMDMAILINF SMTP setting message callback.\n");
	status=smtp_set_messagecb(message,libesmtp_callback,mi->mail_body);
	if (status==0)
	{
		PRINT_ERROR("LIBBMDMAILERR Error in setting messagecb. Error=%li\n",status);
		return BMD_ERR_OP_FAILED;
	}

	PRINT_DEBUG1("LIBBMDMAILINF SMTP adding recipient %s.\n", mi->recipient_to);
	recipient=smtp_add_recipient (message, mi->recipient_to);
	if (recipient==NULL)
	{
		PRINT_ERROR("LIBBMDMAILERR Error in adding recipient. Error=%i\n",BMD_ERR_OP_FAILED);
		return BMD_ERR_OP_FAILED;
	}

        PRINT_DEBUG1("LIBBMDMAILINF SMTP adding reverse path %s.\n", mi->reverse_path);
        status = smtp_set_reverse_path(message, mi->reverse_path); /* Dodanie pola nadawcy TK */

	if (status==0)
	{
		PRINT_ERROR("LIBBMDMAILERR Error in adding reverse_path. Error=%li\n",status);
		return BMD_ERR_OP_FAILED;
	}

	PRINT_DEBUG1("LIBBMDMAILINF SMTP starting session.\n" );
	status=smtp_start_session(session);
	if (status==0)
	{
		PRINT_ERROR("LIBBMDMAILERR Error in starting session. Error=%li\n",status);
		return BMD_ERR_OP_FAILED;
	}

	PRINT_DEBUG1("LIBBMDMAILINF SMTP session started. status = %li\n", status);


	#define BLEN 1024
	char *buf = NULL;
	buf = (char *) malloc (sizeof(char) * BLEN);
	long er=0;

	memset(buf, 0, sizeof(char)*BLEN);
	er=smtp_errno();
	buf = (char *)smtp_strerror(er, buf, BLEN);/** @BUG zewnętrznej biblioteki libesmtp (przynajmniej w wersji 1.0.4) - funkcja smtp_errno() powinna zwalniać pamięć zaalokowaną przez errno_ptr(), bo zwraca tylko long i pamięć pod wskaźnikiem value nie jest potrzebna*/
	if(er!=BMD_OK)
	{
		PRINT_ERROR("LIBBMDMAILERR Error in establishing connection. %s. Error=%li\n", buf,er);
		PRINT_DEBUG1("LIBBMDMAILINF SMTP destroying session\n");
		smtp_destroy_session (session);

		PRINT_DEBUG1("LIBBMDMAILINF SMTP destroying context\n");

		if(mi->with_auth==BMD_MAIL_WITH_AUTH)
		{
			auth_destroy_context(authctx);
			auth_client_exit();
		}

		free0(buf);
		return BMD_ERR_OP_FAILED;
	}

	if (status == 1)
	{
		s_status = (smtp_status_t *)smtp_message_transfer_status (message);
		PRINT_DEBUG1("LIBBMDMAILINF SMTP getting message transfer status. status = %s\n", s_status->text);
	}
  	else
	{
		PRINT_ERROR("LIBBMDMAILERR Error in starting session. status = %li\n", status);
		return_code=-1;
	}

	if( (status==1) && (s_status->code==250) )
	{
		return_code=0;
	}
	else
	{
		//printf("%s\n", s_status->text);
		PRINT_ERROR("LIBBMDMAILERR SMTP error in sending email. status = %li\n", status);
		return_code=-1;
	}

	PRINT_DEBUG1("LIBBMDMAILINF SMTP session started. status = %li\n", status);

	if(s_status!=NULL)
	{
		if(s_status->text!=NULL)
		{
			PRINT_DEBUG1("LIBBMDMAILINF SMTP session started. s_status->text = %s\n", s_status->text);
			free(s_status->text);s_status->text=NULL;
		}
	}
	PRINT_DEBUG1("LIBBMDMAILINF SMTP destroying session\n");
	smtp_destroy_session (session);

	PRINT_DEBUG1("LIBBMDMAILINF SMTP destroying context\n");

	if(mi->with_auth==BMD_MAIL_WITH_AUTH)
	{
		auth_destroy_context(authctx);
		auth_client_exit();
	}

	if(return_code!=0)
	{	free0(buf);
		PRINT_ERROR("LIBBMDMAILERR SMTP error in sending email. Error=%li\n",return_code);
		return BMD_ERR_OP_FAILED;
	}

	PRINT_DEBUG1("LIBBMDMAILINFSMTP mail sent succesfully\n");

	free0(buf);
	return BMD_OK;
#endif
}



#if 0 /* kod nigdzie nie jest uzywany, wiec go wyciachalem z kompilacji (by WSZ)*/
long print_mail_send_info(mail_send_info_t *mi)
{
	PRINT_VDEBUG("LIBBMDMAILINFSMTP smtp server port = %s\n", (mi->server_port)?mi->server_port:"NULL");
	PRINT_VDEBUG("LIBBMDMAILINFSMTP smtp recipient_to = %s\n", (mi->recipient_to)?mi->recipient_to:"NULL");
	PRINT_VDEBUG("LIBBMDMAILINFSMTP smtp reverse_path = %s\n", (mi->reverse_path)?mi->reverse_path:"NULL");
	PRINT_VDEBUG("LIBBMDMAILINFSMTP smtp recipient_cc = %s\n", (mi->recipient_cc)?mi->recipient_cc:"NULL");
	PRINT_VDEBUG("LIBBMDMAILINFSMTP smtp recipient_bcc = %s\n", (mi->recipient_bcc)?mi->recipient_bcc:"NULL");
	PRINT_VDEBUG("LIBBMDMAILINFSMTP smtp with_auth = %li\n", mi->with_auth);
	PRINT_VDEBUG("LIBBMDMAILINFSMTP smtp auth_username = %s\n", (mi->auth_data.username)?mi->auth_data.username:"NULL");
	PRINT_VDEBUG("LIBBMDMAILINFSMTP smtp auth_password = %s\n", (mi->auth_data.password)?mi->auth_data.password:"NULL");
	PRINT_VDEBUG("LIBBMDMAILINFSMTP smtp mail_body = %s\n", (mi->mail_body)?mi->mail_body:"NULL");
	return 0;
}
#endif /*if 0*/
