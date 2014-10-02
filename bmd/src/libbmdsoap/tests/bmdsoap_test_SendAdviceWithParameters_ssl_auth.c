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



int main(int argc, char *argv[])
{
bmd_conf *konfiguracja=NULL;
struct soap soap;
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
int result = 0;
int i=0;

char* bmdId = NULL;
char* accountId = NULL;
char* invoiceId = NULL;
char* userIdentifier = NULL;
char* templateName = NULL;
char* subject = NULL;

	_GLOBAL_debug_level=0;
	
	for (i=1; i<argc; i++)
	{
		if (strcmp(argv[i],"-d")==0)
		{
			if (argc>i+1)
				{ _GLOBAL_debug_level = atoi(argv[i+1]); }
		}
		else if (strcmp(argv[i],"-bmdId")==0)
		{
			if (argc>i+1)
				{ bmdId = strdup(argv[i+1]); }
		}
		else if (strcmp(argv[i],"-accountId")==0)
		{
			if (argc>i+1)
				{ accountId = strdup(argv[i+1]); }
		}
		else if (strcmp(argv[i],"-invoiceId")==0)
		{
			if (argc>i+1)
				{ invoiceId = strdup(argv[i+1]); }
		}
		else if (strcmp(argv[i],"-userId")==0)
		{
			if (argc>i+1)
				{ userIdentifier = strdup(argv[i+1]); }
		}
		else if (strcmp(argv[i],"-t")==0)
		{
			if (argc>i+1)
				{ templateName = strdup(argv[i+1]); }
		}
		else if (strcmp(argv[i],"-s")==0)
		{
			if (argc>i+1)
				{ subject = strdup(argv[i+1]); }
		}
	}
	
	if(bmdId == NULL && accountId == NULL && invoiceId == NULL && userIdentifier == NULL)
	{
		printf("%s\n",argv[0]);
		printf("\nniepoprawne wywołanie\n\nuzyj ponizszych parametrow\n");
		printf("-------------------------------------------------------\n");
		printf("\t-d watosc\t - poziom logowania\n");
		printf("\t-bmdId wartosc\t - id pliku, dla ktorego ma byc wyslane e-awizo (ALBO mozna podac kryteria opisuje plik )\n");
		printf("\t-accountId wartosc\t - wartosc pierwszej metadanej opisujacej dokument (ALBO mozna podac bezposrednio bmdId)\n");
		printf("\t-invoiceId wartosc\t - wartosc drugiej metadanej opisujacej dokument (ALBO mozna podac bezposrednio bmdId)\n");
		printf("\t-userId wartosc\t - wartosc trzeciej metadanej opisujacej dokument (ALBO mozna podac bezposrednio bmdId)\n");
		printf("\t-t wartosc\t - nazwa szablonu e-awizo, ktory ma byc wykorzystany (OPCJONALNE)\n");
		printf("\t-s wartosc\t - tytul wysylanej wiadomosci e-awizo (OPCJONALNE)\n");
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


	// wywolanie uslugi
	status=soap_call_bmd230__SendAdviceWithParameters(&soap, host, NULL, base64Cert, NULL, NULL, NULL, bmdId, accountId, invoiceId, userIdentifier, templateName, subject, &result);
	free(bmdId); bmdId = NULL;
	free(accountId); accountId = NULL;
	free(invoiceId); invoiceId = NULL;
	free(userIdentifier); userIdentifier = NULL;
	free(templateName); templateName = NULL;
	free(subject); subject = NULL;
	if(status == SOAP_OK)
	{
		printf("Wynik: %i\n", result);
	}
	else
	{
		printf("Error \n");
		printf("Wynik: %i\n", result);
		soap_print_fault(&soap, stdout);
		soap_end(&soap);
		soap_done(&soap);
		return -1;
	}

	soap_end(&soap);
	soap_done(&soap);

	return 0;
}
