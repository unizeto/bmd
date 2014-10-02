#include <bmd/mail_sender/queue.h>
#include <bmd/mail_sender/shmem.h>
#include <bmd/mail_sender/cache.h>
#include <bmd/mail_sender/attachment.h>
#include <bmd/libbmdcurl/libbmdcurl.h>
#include <bmd/mail_sender/mail_sender.h>
#include <bmd/libbmdglobals/libbmdglobals.h>
#include "soapH.h"


int ns__awizoSendSMS(struct soap* soap, char *message, char *to, long* result)
{
    int status 		= 0;
    GenBuf_t *gb 	= NULL;
    char *api_url 	= NULL;
    
    if (message == NULL)
    {
	PRINT_ERROR("Nie podano tresci wiadomosci SMS. Blad %i.\n", BMD_ERR_PARAM1);
	return BMD_ERR_PARAM1;
    }
    
    if (to == NULL)
    {
	PRINT_ERROR("Nie podano adresata wiadomosci SMS. Blad %i.\n", BMD_ERR_PARAM2);
	return BMD_ERR_PARAM2;
    }
    
    //pobieranie linka do API
    status = bmdconf_get_value(_GLOBAL_awizoSoapKonfiguracja,"sms","api_url",&api_url);
    if (status != BMD_OK)
    {
	PRINT_ERROR("Nie mozna pobrac adresu URL uslugi API SMS. Blad %i.\n", status);
	return status;
    }
    
    status = bmd_str_replace(&api_url, "__TO__", to);
    if (status != BMD_OK)
    {
	PRINT_ERROR("Nie mozna wstawic numeru %s do zadania SMS. Blad %i.\n", to, status);
	return status;
    }
    
    char *urlmessage = NULL; /*url-encoded message*/
    urlmessage = url_encode(message);
    status = bmd_str_replace(&api_url, "__MESSAGE__", urlmessage);
    free(urlmessage);
    if (status != BMD_OK)
    {
	PRINT_ERROR("Nie mozna wstawic tresci %s do zadania SMS. Blad %i.\n", to, status);
	return status;
    }
    
    status = set_gen_buf2(" ", 1, &gb);
    if (status != BMD_OK)
    {
	PRINT_ERROR("Nie mozna ustawic bufora z trescia wiadomosci. Blad %i.\n", status);
	return status;
    }
        
    
    status = SimpleCurlPost(gb, api_url);
    if (status != BMD_OK)
    {
	PRINT_ERROR("Blad wywolania uslugi SMS po curl. Blad %i.\n", status);
	return status;
    }
    
    
    return SOAP_OK;
}
