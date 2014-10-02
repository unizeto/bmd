#include "bmdsoap_client.h"
#include <bmd/common/bmd-sds.h>
#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdconfig/libbmdconfig.h>
#include <bmd/libbmdsoap/libbmdsoap.h>
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
struct bmd230__idList* result=NULL;
int i=0;
char *transactionId = NULL;
long correspondingFilesMaxLevel=0;

struct bmd230__singleLinkInfo linkInfo;

	_GLOBAL_debug_level=0;
	if (argc==13)
	{
		for (i=1; i<argc; i++)
		{
			if (strcmp(argv[i],"-d")==0)
			{
				if (argc>i+1) _GLOBAL_debug_level=atoi(argv[i+1]);
			}
			if (strcmp(argv[i],"-t")==0)
			{
				if (argc>i+1) asprintf(&transactionId,"%s",argv[i+1]);
			}
			if (strcmp(argv[i],"-id")==0)
			{
				if (argc>i+1) asprintf(&(linkInfo.pointingId),"%s",argv[i+1]);
			}
			if (strcmp(argv[i],"-u")==0)
			{
				if (argc>i+1) asprintf(&(linkInfo.ownerIdentityId),"%s",argv[i+1]);
				linkInfo.ownerCertificateId=NULL;
			}
			if (strcmp(argv[i],"-gr")==0)
			{
				if (argc>i+1) asprintf(&(linkInfo.ownerGroupId),"%s",argv[i+1]);
			}
			if (strcmp(argv[i],"-l")==0)
			{
				if (argc>i+1) (correspondingFilesMaxLevel=strtol(argv[i+1], NULL, 10));
			}
		}
	}
	else
	{
		printf("%s\n",argv[0]);
		printf("\nniepoprawne wywołanie\n\nuzyj ponizszych parametrow\n");
		printf("-------------------------------------------------------\n");
		printf("\t-d liczba\tpoziom logowania\n");
		printf("\t-id\tidentyfikator pliku\n");
		printf("\t-u uzytkownik, do ktorego zostanie przypisany link\n");
		printf("\t-gr grupa uzytkownika, do ktorej zostanie przypisany link\n");
		printf("\t-t file\tid transakcji\n");
		printf("\t-l poziom poziom zagniezdzenia drzewa dokumentow powiazanych\n");
		printf("-------------------------------------------------------\n");
		return -1;
	}


	if (strcmp(transactionId,"0")==0)
	{
		printf("nulluje transakcje\n");
		free(transactionId); transactionId=NULL;
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


	GenBuf_t *cert_pem=NULL;
	base64Cert = (struct xsd__base64Binary *)malloc(sizeof(struct xsd__base64Binary));
	status = bmd_load_binary_content(keyfile,&cert_pem);
	if (status != BMD_OK)
	{
		PRINT_DEBUG("SOAPCLIENTERR Error while reading certificate file\n");
		return 0;
	}
	base64Cert->__ptr=(unsigned char*)(cert_pem->buf);
	base64Cert->__size=cert_pem->size;

	soap_set_namespaces(&soap, bmd230_namespaces);
	
	linkInfo.linkDescription=strdup("link stworzony kaskadowo");
	linkInfo.granting=GRANTING_FORBIDDEN;//GRANTING_ALLOWED;
	linkInfo.expirationDate=(struct bmd230__DateTime*)calloc(1, sizeof(struct bmd230__DateTime));
	linkInfo.expirationDate->year=2011;
	linkInfo.expirationDate->month=3;
	linkInfo.expirationDate->day=10;
	linkInfo.expirationDate->hour=14;
	linkInfo.expirationDate->minute=53;
	linkInfo.expirationDate->second=21;

	status = soap_call_bmd230__bmdCreateCascadeLinks(&soap, host, NULL, base64Cert, NULL, NULL, NULL, &linkInfo, transactionId, correspondingFilesMaxLevel, ALL,  &result);

	if (status == SOAP_OK)
	{
		printf("Ilosc utworzonych linkow w archiwum: %i\n", result->__size);
		for(i=0; i< result->__size; i++)
		{
			printf("Utworzono link z id: %i\n", result->__ptr[i]);
		}
	}
	else
	{
		/*odpowiedz w razie bledu*/
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
