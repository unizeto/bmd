#include <bmd/libbmdmail/libbmdmail.h>
#include <bmd/libbmdmime/libbmdmime.h>
#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdglobals/libbmdglobals.h>

/**kod w tym pliku nie jest nigdzie uzywany i oznaczam go jako obsolete (by WSZ)*/
#if 0 /*<OBSOLETE>*/

long print_mail_send_info(mail_send_info_t *mi);
#ifdef WIN32
#pragma warning(disable:4100)
#endif

/** Funkcja dodawania wiadomości do wysłania.

@author LKl
@param mi Struktura wiadomości mail_info
@param rcpt_to adresat wiadomości
@param body ciało wiadomości

@retval BMD_OK Operacja zakończona sukcesem
@retval BMD_ERR_PARAM1 Nieprawidłowy parametr \c mi
@retval BMD_ERR_PARAM2 Nieprawidłowy parametr \c rcpt_to
@retval BMD_ERR_PARAM3 Nieprawidłowy parametr \c body
@retval BMD_ERR_OP_FAILED Błąd podczas wywołania biblioteki libesmtp
*/

long bmdmail_add_message(	mail_send_info_t *mi,
					const char * rcpt_to,
					const char * body)
{
#ifndef WIN32
long status				= 0;
smtp_message_t message		= NULL;
smtp_recipient_t recipient	= NULL;

	/******************************/
	/*	walidacja prametrow	*/
	/******************************/
	if (mi == NULL) 					{	BMD_FOK(BMD_ERR_PARAM1);	}
	if (rcpt_to == NULL)				{	BMD_FOK(BMD_ERR_PARAM2);	}
	if (body == NULL)					{	BMD_FOK(BMD_ERR_PARAM3);	}

	if (mi->session == NULL)
	{
		auth_client_init();
		mi->session = smtp_create_session();
	}

	if (mi->session == NULL)			{	BMD_FOK(PLUGIN_SESSION_SESSION_CREATE_ERROR);	}
	message=smtp_add_message(mi->session);
	if (message==NULL)				{	BMD_FOK(PLUGIN_SESSION_ADD_MSG_TO_SESSION_ERROR);	}

	status=smtp_set_messagecb(message,libesmtp_callback,(void*)body);
	if (status==0)					{	BMD_FOK(PLUGIN_SESSION_SET_MSG_CALLBACK_ERROR);	}

	recipient=smtp_add_recipient (message, rcpt_to);
	if (recipient==NULL)				{	BMD_FOK(PLUGIN_SESSION_ADD_RECIPIENT_TO_MSG_ERROR);	}

#endif

	return BMD_OK;
}

#ifndef WIN32
/** Funkcja wewnętrzna, używana do callbacka przy listowaniu wiadomości.
Weryfikuje poprawność wysłania wiadomości; jeśli co najmniej jedna z wiadomości ma status wymagający
ponownej wysyłki, wykonuje *retry ++;
*/
static void enumerate_msg_cb (	smtp_message_t message,
						void * retry)
{
smtp_status_t *s_status	= NULL;
long * retry_int		= (long *) retry;

	s_status = (smtp_status_t *)smtp_message_transfer_status (message);

	if( (s_status->code >= 400 && s_status->code < 600) || s_status->code == 0 )
	{ /* temporary or permament error */
		PRINT_DEBUG("Temporary error for message \n");
		(*retry_int)++;
	}

	return;

}
#endif
/** Funkcja wysyła dodane wcześniej wiadomości.

@bug Jeśli wystąpi błąd przy którejś z wiadomości, to nie ma możliwości
sprawdzenia, przy której ani jaki był jego powód.

@param mi Struktura wskazująca na wiadomości do wysłania

@retval BMD_ERR_PARAM1 Struktura \c mi jest nieprawidłowa lub nie zawiera wszystkich wymaganych pól
@retval BMD_ERR_PROTOCOL Błąd podczas wysyłki
*/
long bmdmail_send(mail_send_info_t * mi)
{
#ifndef WIN32
	long status=0;
	long retry=0; /*< licznik ilości wiadomości, których nie udało się wysłać. */
	long retry_prev=0; /*< poprzedni licznik ilości wiadomości, których nie udało się wysłać. */
	auth_context_t authctx = NULL;
	char *buf = NULL;


	PRINT_INFO("LIBBMDMAILINF Sending mail (BMD)\n");
	if(mi==NULL)
	{
		PRINT_DEBUG("LIBBMDMAILERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}

	if(mi->session == NULL)
	{
		PRINT_DEBUG("LIBBMDMAILERR Session not initialized. Use bmdmail_add_message() first."
									" Error=%i\n",BMD_ERR_PARAM1);
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
	if(mi->mail_body==NULL)
	{
		PRINT_DEBUG("LIBBMDMAILERR Invalid first parameter value. Error=%i\n",BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}



	PRINT_DEBUG1("LIBBMDMAILINF SMTP setting smtp server port to %s.\n", mi->server_port);

	status=smtp_set_server(mi->session,mi->server_port);
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
		smtp_auth_set_context(mi->session, authctx);
	}

	retry = 255*255*255*100;
	retry_prev = retry + 1;
	while(retry > 0 && retry < retry_prev) { /* dopóki udaje się wysłać choć 1 maila w iteracji */
		PRINT_DEBUG1("LIBBMDMAILINF SMTP starting session, retry=%li.\n",retry);
		status=smtp_start_session(mi->session);
		if (status==0)
		{
			PRINT_ERROR("LIBBMDMAILERR Error in starting session. Error=%li\n",status);
			return BMD_ERR_OP_FAILED;
		}

		PRINT_DEBUG1("LIBBMDMAILINF SMTP session started. status = %li\n", status);


		#define BLEN 1024
		if (buf == NULL) {
			buf = (char *) malloc (sizeof(char) * BLEN);
		}

		long er=0;

		memset(buf, 0, sizeof(char)*BLEN);
		er=smtp_errno();
		buf = (char *)smtp_strerror(er, buf, BLEN);/** @BUG zewnętrznej biblioteki libesmtp (przynajmniej w wersji 1.0.4) - funkcja smtp_errno() powinna zwalniać pamięć zaalokowaną przez errno_ptr(), bo zwraca tylko long i pamięć pod wskaźnikiem value nie jest potrzebna*/
		if(er!=BMD_OK)
		{
			PRINT_ERROR("LIBBMDMAILERR Error in establishing connection. %s. Error=%li\n", buf,er);
			PRINT_DEBUG1("LIBBMDMAILINF SMTP destroying session\n");
			smtp_destroy_session (mi->session);
			mi->session = NULL;

			PRINT_DEBUG1("LIBBMDMAILINF SMTP destroying context\n");

			if(authctx!=NULL)
			{
				auth_destroy_context(authctx);
			}

			auth_client_exit();

			free0(buf);
			return BMD_ERR_OP_FAILED;
		}

		if (status == 1)
		{
			retry_prev = retry;
			retry = 0;
			smtp_enumerate_messages(mi->session,enumerate_msg_cb,&retry);
		} else {
			PRINT_DEBUG("Status = %li\n",status);
		}

		PRINT_DEBUG1("LIBBMDMAILINF SMTP session started. status = %li\n", status);
  	} /* koniec while retry < retry_prev */

	free0(buf);

	if (retry != 0) {
		PRINT_ERROR("Cannot send %li messages\n",retry);
		return BMD_ERR_PROTOCOL;
	}

	PRINT_DEBUG1("LIBBMDMAILINF SMTP destroying session\n");
	smtp_destroy_session (mi->session);
	mi->session = NULL;

	PRINT_DEBUG1("LIBBMDMAILINF SMTP destroying context\n");

	if(authctx != NULL)
	{
		auth_destroy_context(authctx);
	}
	auth_client_exit();

	PRINT_DEBUG1("LIBBMDMAILINFSMTP mail sent succesfully\n");

#endif
	return BMD_OK;

}

#endif /*if 0 </OBSOLETE>*/
