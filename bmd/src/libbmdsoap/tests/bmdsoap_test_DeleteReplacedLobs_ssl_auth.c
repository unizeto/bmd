#include "bmdsoap_client.h"
#include <bmd/libbmderr/libbmderr.h>

int main()
{
bmd_conf *konfiguracja=NULL;
struct soap soap;
char *serverURL = NULL;
char *host = NULL;
long int ssl_enabled=0;
long int authenticate=0;
char *keyfile=NULL;
char *keyfile_passwd=NULL;
char *keyfile_ssl=NULL;
char *keyfile_passwd_ssl=NULL;
char *cacert=NULL;
char *capath=NULL;
int status = 0;

struct xsd__base64Binary *base64Cert = NULL;

long deletedLobsCounter			= 0;


	/*za≥adowanie bibliotek ssl-owych*/
	SSL_load_error_strings();
        SSL_library_init();

	/*funkcje konfiguracyjne*/
	load_soap_configuration(&konfiguracja);
	configuration(konfiguracja,&host,&keyfile,&keyfile_passwd,&keyfile_ssl,&keyfile_passwd_ssl,&cacert,&capath,&ssl_enabled);

	/*funkcja ustanowienia po≥aczenia z serwerem*/
	status=connection(&soap,ssl_enabled,authenticate,keyfile_ssl,keyfile_passwd_ssl,cacert,capath);
	if (status!=SOAP_OK)
	{
		PRINT_DEBUG("SOAPCLIENTERR Connection error\n");
		return 0;
	}

	GenBuf_t *cert_pem=NULL;
	base64Cert = (struct xsd__base64Binary *)malloc(sizeof(struct xsd__base64Binary));
	status = bmd_load_binary_content(keyfile,&cert_pem);
	if (status != BMD_OK)
	{
		PRINT_DEBUG("SOAPCLIENTERR Error while reading certificate file\n");
		return 0;
	}
	base64Cert->__ptr=(unsigned char*)cert_pem->buf;
	base64Cert->__size=cert_pem->size;

	soap_set_namespaces(&soap, bmd230_namespaces);

	printf("Wysylanie zadania...\n");
	status = soap_call_bmd230__bmdDeleteReplacedLobs(&soap, host, NULL, base64Cert, NULL, NULL, NULL, &deletedLobsCounter);
	if (status == SOAP_OK)
	{
		printf("Usunieto %li lobow\n", deletedLobsCounter);
	}
	else
	{
		/*odpowiedü w razie b≥Ídu*/
		soap_print_fault(&soap, stderr);
		soap_end(&soap);
		soap_done(&soap);
		free(serverURL);
		return -1;
	}

	soap_end(&soap);
	soap_done(&soap);
	free(serverURL);
	return 0;
}
