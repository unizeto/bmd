#include "bmdsoap_client.h"
#include <bmd/common/bmd-sds.h>
#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdconfig/libbmdconfig.h>
#include <conf_sections.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>


/**
*Załadowanie ustawień konfiguracynjych serwera z pliku konfiguracyjnego
*@param brak
*@retval 0 - poprawne wykonanie funkcji
*@retval -1 - niepoprawne wykonanie funkcji
*/

int main(int argc, char *argv[])
{
	struct soap soap;
	char *result	= NULL;

	soap_ssl_init();
	soap_init(&soap);

	if (soap_ssl_client_context(	&soap,
					SOAP_SSL_DEFAULT,	/* use SOAP_SSL_DEFAULT in production code, we don't want the host name checks since these will change from machine to machine */
					//SOAP_SSL_DEFAULT | SOAP_SSL_SKIP_HOST_CHECK,	/* use SOAP_SSL_DEFAULT in production code, we don't want the host name checks since these will change from machine to machine */
					NULL, 		/* keyfile: required only when client must authenticate to server (see SSL docs on how to obtain this file) */
					NULL, 		/* password to read the keyfile */
					"cacert.pem",	/* optional cacert file to store trusted certificates, use cacerts.pem for all public certificates issued by common CAs */
					NULL,		/* optional capath to directory with trusted certificates */
					NULL		/* if randfile!=NULL: use a file with random data to seed randomness */
	))
	{
		soap_print_fault(&soap, stderr);
		exit(1);
	}

	soap_set_namespaces(&soap, bmd230_namespaces);

	if (soap_call_bmd230__testConnection(&soap, "https://localhost:9999", NULL, &result) == SOAP_OK)
	{
		fprintf(stdout, "Result: %s\n", result);
	}
	else
	{
		soap_print_fault(&soap, stderr);
	}

	soap_end(&soap);
	soap_done(&soap);

	return 0;

}
