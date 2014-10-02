#include <bmd/libbmdmail/libbmdmail.h>
#include <bmd/libbmdmime/libbmdmime.h>
#include <bmd/libbmderr/libbmderr.h>


/**
funkcja libesmtp_callback() potrzebna tylko do pracy na libesmtp (Linux) i nie trzeba jej portowac
*/
#ifndef WIN32
const char *libesmtp_callback(void **ctx, int *len, void *arg)
{
	const char *string = arg;
	struct state *state;

	if (*ctx == NULL)
		*ctx = malloc (sizeof (struct state));
	state = *ctx;

	if (len == NULL)
	{
		state->m_state = 0;
		state->length = (int)strlen (string);
		return NULL;
	}

	switch (state->m_state)
	{
		case 0:
			state->m_state = 1;
			*len = state->length;
			break;
		default:
			*len = 0;
			break;
	}	
	return string;
}
#endif /*ifndef WIN32*/

/**
funkcja authinteract() potrzebna tylko do pracy na libesmtp (Linux) i nie trzeba jej portowac
*/
#ifndef WIN32
int authinteract(auth_client_request_t request, char ** result, int fields, void * arg)
{
	long i;
	auth_data_struct_t *dupa=(auth_data_struct_t *)arg;

	if (!(dupa != NULL))
	{
		PRINT_ERROR("auth_data_struct_t variable is NULL!\n");
		abort();
	}
	if (!(fields == 2))
	{
		PRINT_ERROR("There should be exactly 2 fields!\n");
		abort();
	}
	
	PRINT_VDEBUG("auth interacting function\n");	
	
	for (i = 0; i < fields; i++) {
		PRINT_VDEBUG("%s\n",request[i].prompt);
		if (request[i].flags & AUTH_PASS) 
		{
			result[i] = dupa->password;
			// odwrócona kolejność powodowała wpisywanie hasła w miejsce użytkownika i na odwrót - w skutek czego nie działało wysyłanie z autoryzacją - RO
			// tylko wskazuję na miejsce przechowywania hasła - nie ma potrzeby go duplikować, bo i tak nie ma jak później tego zwolnić - RO
		} 
		else 
		{
			result[i] = dupa->username;
		}
	}
	PRINT_VDEBUG("End of auth interacting function\n");
	return 1;
}
#endif /*ifndef WIN32*/

#if 0 /* <OBSOLETE> */
/** Funkcja tworząca naglowek do tworzonej wiadomosci.
Funkcja alokuje pamięć i zapisuje pola nagłówka w zmiennej typu msg_header_info
do użycia funkcji należy przygotować wskaźnik typu msg_header_info
po wysłaniu wiadomości należy wywołać funkcję \c free_headers();

@param email_to	- adres email adresata wiadomości
@param email_from - adres email nadawcy
@param subject - temat wiadomości
@param msg_id - identyfikator wiadomości
@param date - data i czas wysłania wiadomości w formacie tekstowym (np. Thu, 12 Jul 2007 08:46:01 +0200)
@param output - wskaźnik na zmienną przechowującą informacje nagłówka
@retval 0 -ok
@retval -1 - nie udało się zaalokować pamięci na zmienną msg_header_info
@retval -2 - brak adresu email adresata
@retval -3 - brak adresu email nadawcy
@retval -4 - brak tematu wiadomości
*/
long set_header_info(char *email_to,char *email_from,char *subject,char *msg_id,char *date,msg_header_info **output)
{
	(*output)=(msg_header_info *)malloc(sizeof(msg_header_info));
	(*output)->email_to=NULL;
	(*output)->email_from=NULL;
	(*output)->date=NULL;
	(*output)->msg_id=NULL;
	(*output)->subject=NULL;
	if((*output)==NULL)
	{
		PRINT_ERROR("LIBBMDMAILERR Error. Error=%i\n",-1);
		return -1;
	}
	if(email_to==NULL)
	{
		PRINT_ERROR("LIBBMDMAILERR Error. Error=%i\n",-2);
		return -2;
	}
	asprintf(&((*output)->email_to),"%s",email_to);
	if(email_from==NULL)
	{
		PRINT_ERROR("LIBBMDMAILERR Error. Error=%i\n",-3);
		return -3;
	}
	asprintf(&((*output)->email_from),"%s",email_from);
	if(subject==NULL)
	{
		PRINT_ERROR("LIBBMDMAILERR Error. Error=%i\n",-4);
		return -4;
	}
	asprintf(&((*output)->subject),"%s",subject);
	asprintf(&((*output)->msg_id),"%s",msg_id);
	asprintf(&((*output)->date),"%s",date);
	return 0;
}
#endif /*if 0 </OBSOLETE>*/

#if 0 /* <OBSOLETE> */
/**Funkcja ustawiająca zawartość wiadomości.
Po wysłaniu wiadomości należy użyć funkcji 
@param msg_content - treść wiadomości
@param output - zmienna przechowująca informacje ciała wiadomości
@retval 0 - ok
*/
long set_body_info(char *msg_content,msg_body_info **output)
{
	(*output)=(msg_body_info *)malloc(sizeof(msg_body_info));
	asprintf(&((*output)->msg_content),"%s",msg_content);

	return 0;
}
#endif /* if 0 </OBSOLETE>*/

#if 0 /* <OBSOLETE> */
/** Funkcja przygotowania nagłówków wiadomości.
Funkcja tworzy tablicę znakową z odpowiednio sformatowanymi nagłówkami
przed wywołaniem funkcji przygotować zmienną \c header np. za pomocą \c set_header_info()

*/
long prepare_headers(msg_header_info *header,char **headers_content)
{
	asprintf(headers_content,"To: %s\r\nFrom: %s\r\nSubject: %s\r\n",
		 header->email_to,
		 header->email_from,
		 header->subject
		);
	return 0;
}
#endif /* </OBSOLETE> */

#if 0 /* <OBSOLETE> */
long prepare_body(msg_body_info *body,char **body_content)
{
	asprintf(body_content,"%s",body->msg_content);
	return 0;
}
#endif /* if 0 </OBSOLETE> */


#if 0 /* <OBSOLETE> */
void free_headers(msg_header_info **header)
{
	if(*header)
	{
		if( (*header)->email_to )
		{
			free( (*header)->email_to);(*header)->email_to=NULL;
		}
		if( (*header)->email_from )
		{
			free( (*header)->email_from);(*header)->email_from=NULL;
		}
		if( (*header)->subject )
		{
			free( (*header)->subject);(*header)->subject=NULL;
		}
		if( (*header)->msg_id )
		{
			free( (*header)->msg_id);(*header)->msg_id=NULL;
		}
		if( (*header)->date )
		{
			free( (*header)->date);(*header)->date=NULL;
		}
		free(*header);
		*header=NULL;
	}
}
#endif /* if 0 </OBSOLETE> */


#if 0 /* <OBSOLETE> */
void free_body(msg_body_info **body)
{
	if(*body)
	{
		if( (*body)->msg_content )
		{
			free( (*body)->msg_content);(*body)->msg_content=NULL;
		}
		free(*body);
		*body=NULL;
	}
}
#endif /* if 0 </OBSOLETE> */


/*by AK*/
long set_mail_send_info(mail_send_info_t **mi, char *_server_port, char *_recipient_to, char *_recipient_cc, char*_recipient_bcc, char *_mail_body, long _with_auth, char *_user, char* _password, char *_reverse_path)
{
	if (mi == NULL)
	{
		PRINT_DEBUG("Wrong argument 1\n");
		return BMD_ERR_PARAM1;
	}
	if (*mi != NULL)
	{
		PRINT_DEBUG("Wrong argument 1\n");
		return BMD_ERR_PARAM1;
	}
	if (_server_port == NULL)
	{
		PRINT_DEBUG("Wrong argument 2\n");
		return BMD_ERR_PARAM2;
	}
	if (_recipient_to == NULL)
	{
		PRINT_DEBUG("Wrong argument 3\n");
		return BMD_ERR_PARAM3;
	}
        if (_reverse_path == NULL)
        {
		PRINT_DEBUG("Wrong argument 10\n");
 	        return BMD_ERR_PARAM10;
        }
	if (_mail_body == NULL)
	{
		PRINT_DEBUG("Warning: no mail body given (param 6).\n");
	}
	if (_with_auth == BMD_MAIL_WITH_AUTH)
	{
		if (_user == NULL)
		{
			PRINT_DEBUG("Wrong argument 8\n");
			return BMD_ERR_PARAM8;
		}
		if (_password == NULL)
		{
			PRINT_DEBUG("Wrong argument 9\n");
			return BMD_ERR_PARAM9;
		}
	}
	
	(*mi) = (mail_send_info_t*) calloc (1, sizeof(mail_send_info_t));
	if (*mi == NULL)
	{
		PRINT_ERROR("No memory!\n");
		return BMD_ERR_MEMORY;
	}

	(*mi)->server_port = strdup(_server_port);
	if ((*mi)->server_port == NULL)
	{
		PRINT_ERROR("No memory!\n");
		return BMD_ERR_MEMORY;
	}

	(*mi)->recipient_to = strdup(_recipient_to);
	if ((*mi)->recipient_to == NULL)
	{
		PRINT_ERROR("No memory!\n");
		return BMD_ERR_MEMORY;
	}

	(*mi)->reverse_path = strdup(_reverse_path);
	if ((*mi)->reverse_path == NULL)
	{
		PRINT_ERROR("No memory!\n");
		return BMD_ERR_MEMORY;
	}

	if (_recipient_cc != NULL)
	{
		(*mi)->recipient_cc= strdup(_recipient_cc);
		if ((*mi)->recipient_cc == NULL)
		{
			PRINT_ERROR("No memory!\n");
			return BMD_ERR_MEMORY;
		}
	}

	if (_recipient_bcc != NULL)
	{
		(*mi)->recipient_bcc= strdup(_recipient_bcc);
		if ((*mi)->recipient_bcc == NULL)
		{
			PRINT_ERROR("No memory!\n");
			return BMD_ERR_MEMORY;
		}
	}

	if (_mail_body)
	{
		(*mi)->mail_body= strdup(_mail_body);
		if ((*mi)->mail_body == NULL)
		{
			PRINT_ERROR("No memory!\n");
			return BMD_ERR_MEMORY;
		}
	}
	
	if (_with_auth == BMD_MAIL_WITH_AUTH)
	{
		(*mi)->with_auth=BMD_MAIL_WITH_AUTH;
		(*mi)->auth_data.username = strdup(_user);
		if ((*mi)->auth_data.username == NULL)
		{
			PRINT_ERROR("No memory!\n");
			return BMD_ERR_MEMORY;
		}
		(*mi)->auth_data.password = strdup(_password);
		if ((*mi)->auth_data.password == NULL)
		{
			PRINT_ERROR("No memory!\n");
			return BMD_ERR_MEMORY;
		}
	}
	return 0;
}

#define FREE(x) {if(x){free(x); x=NULL;}}
long free_mail_send_info(mail_send_info_t **mi)
{
	if (mi == NULL)
	{
		PRINT_DEBUG("Wrong argument 1\n");
		return BMD_ERR_PARAM1;
	}
	if (*mi == NULL)
	{
		return 0;
	}

	FREE((*mi)->server_port);
	FREE((*mi)->recipient_to);
	FREE((*mi)->reverse_path);
	FREE((*mi)->recipient_cc);
	FREE((*mi)->recipient_bcc);
	FREE((*mi)->auth_data.username);
	FREE((*mi)->auth_data.password);
	FREE((*mi)->mail_body);
	free(*mi);
	(*mi) = NULL;
	
	return 0;
}
#undef FREE



long getSMTPResponseStatusCode(char *input, long *statusCode)
{
	long retVal=0;
	const char *error_pointer;
	int error_offset;
	int subpatterns[6];
	pcre *pcreStruct=NULL;
	char *regex="^.*?(\\d{3}).*";
	char statusCodeString[10];

	if(input == NULL)
		{ return -1; }
	if(statusCode == NULL)
		{ return -2; }

	memset(statusCodeString, 0, 10*sizeof(char));

	pcreStruct=pcre_compile(regex,PCRE_UTF8,&error_pointer,&error_offset,NULL);
	if(pcreStruct == NULL)
		{ return -3; }
	retVal=pcre_exec(pcreStruct, NULL, input, (int)strlen(input), 0, 0, subpatterns, 6);
	if(retVal <= 0)
	{
		pcre_free(pcreStruct);
		return -4;
	}

	if(strncpy(statusCodeString, input+subpatterns[2], subpatterns[3]-subpatterns[2]) == NULL)
	{
		pcre_free(pcreStruct);
		return -5;
	}

	*statusCode=atol(statusCodeString);
	return 0;
}
