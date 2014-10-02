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
bmd_conf *konfiguracja=NULL;
struct soap soap;
char *command		= NULL;
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
int result= 0;
int status = 0;
struct bmd230__mtdsInfo *userMtds = NULL;
struct bmd230__searchResults *searchResults = NULL;
struct xsd__base64Binary *base64Cert = NULL;
char *serverResponse = NULL;
int i=0;
char *hash=NULL;
long int id=0;
GenBuf_t plik;
GenBuf_t save;
char *accountId		= NULL;
char *invoiceId		= NULL;
char *userIndentifier	= NULL;
int forceVerification	= 0;

	_GLOBAL_debug_level=0;
	if (argc==11)
	{
		for (i=1; i<argc; i++)
		{
			if (strcmp(argv[i],"-d")==0)
			{
				if (argc>i+1) _GLOBAL_debug_level=atoi(argv[i+1]);
			}
			if (strcmp(argv[i],"-p")==0)
			{
				if (argc>i+1) asprintf(&accountId,"%s",argv[i+1]);
				printf("%s\n",accountId);
			}
			if (strcmp(argv[i],"-s")==0)
			{
				if (argc>i+1) asprintf(&invoiceId,"%s",argv[i+1]);
				printf("%s\n",invoiceId);
			}
			if (strcmp(argv[i],"-r")==0)
			{
				if (argc>i+1) asprintf(&userIndentifier,"%s",argv[i+1]);
				printf("%s\n",userIndentifier);
			}
			if (strcmp(argv[i],"-t")==0)
			{
				if (argc>i+1) forceVerification=atoi(argv[i+1]);
				printf("%i\n",forceVerification);
			}
		}
	}
	else
	{
		printf("%s\n",argv[0]);
		printf("\nniepoprawne wywołanie\n\nuzyj ponizszych parametrow\n");
		printf("-------------------------------------------------------\n");
		printf("\t-d liczba\tpoziom logowania\n");
		printf("\t-p string\twartosc metadanej\n");
		printf("\t-s string\twartosc metadanej\n");
		printf("\t-r string\twartosc metadanej\n");
		printf("\t-t 0 lub 1\tforceVerification\n");
		printf("-------------------------------------------------------\n");
		return -1;
	}

	/*załadowanie bibliotek ssl-owych*/
	SSL_load_error_strings();
	SSL_library_init();

	/*funkcje konfiguracyjne*/
	load_soap_configuration(&konfiguracja);
	configuration(konfiguracja,&host,&keyfile,&keyfile_passwd,&keyfile_ssl,&keyfile_passwd_ssl,&cacert,&capath,&ssl_enabled);

	/*funkcja ustanowienia połaczenia z serwerem*/

	status=connection(&soap,ssl_enabled,authenticate,keyfile_ssl,keyfile_passwd_ssl,cacert,capath);
	if (status!=SOAP_OK)
	{
		PRINT_DEBUG("SOAPCLIENTERR Connection error\n");
		return 0;
	}

	/*przygotowanie danych niezbędnych do uruchomienia funkcji web-owej*/
	GenBuf_t *cert_pem=NULL;
	struct bmd230__fileInfo *fileResponse = NULL;
	base64Cert = (struct xsd__base64Binary *)malloc(sizeof(struct xsd__base64Binary));
	status = bmd_load_binary_content(keyfile,&cert_pem);
	if (status != BMD_OK)
	{
		PRINT_DEBUG("SOAPCLIENTERR Error while reading certificate file\n");
		return 0;
	}
	base64Cert->__ptr=cert_pem->buf;
	base64Cert->__size=cert_pem->size;

	soap_set_namespaces(&soap, bmd230_namespaces);

	/*********************************************************************************/
	/************************ funkcja testowa ****************************************/
	status=soap_call_bmd230__verifyInvoiceWithDetails(&soap, host, NULL, accountId, invoiceId, userIndentifier, forceVerification, base64Cert, NULL, NULL, NULL,  &fileResponse);
	/*********************************************************************************/
	/*********************************************************************************/

	if (status == SOAP_OK)
	{
		/*********************************************************************************/
		/************************ funkcja odpowiedź ****************************************/
		printf("Znalezione poświadczenie: %s\nWielkość pliku: %i\n", fileResponse->filename,fileResponse->file->__size);
		save.buf=(unsigned char *)malloc(fileResponse->file->__size+1);
		memset(save.buf, 0, fileResponse->file->__size+1);
		memcpy(save.buf, fileResponse->file->__ptr, fileResponse->file->__size);
		save.size=fileResponse->file->__size;
		status=bmd_save_buf(&save,"dvcs0.pkl");

	}
	else
	{
		/*odpowiedź w razie błędu*/
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
