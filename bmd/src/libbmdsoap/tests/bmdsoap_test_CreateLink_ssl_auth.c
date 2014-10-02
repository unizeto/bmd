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
struct bmd230__mtdsValues *mtds = NULL;

long int result=0;
char *file=NULL;
int i=0;
char *transactionId = NULL;
char *description=NULL;
char* pointingId=NULL;

struct bmd230__DateTime expirationDate;
expirationDate.year=2011;
expirationDate.month=2;
expirationDate.day=9;
expirationDate.hour=16;
expirationDate.minute=2;
expirationDate.second=38;


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
				if (argc>i+1) asprintf(&file,"%s",argv[i+1]);
			}
			if (strcmp(argv[i],"-t")==0)
			{
				if (argc>i+1) asprintf(&transactionId,"%s",argv[i+1]);
			}
			if (strcmp(argv[i],"-r")==0)
			{
				if (argc>i+1) asprintf(&description,"%s",argv[i+1]);
			}
			if (strcmp(argv[i],"-id")==0)
			{
				if (argc>i+1) asprintf(&pointingId,"%s",argv[i+1]);
			}
		}
	}
	else
	{
		printf("%s\n",argv[0]);
		printf("\nniepoprawne wywołanie\n\nuzyj ponizszych parametrow\n");
		printf("-------------------------------------------------------\n");
		printf("\t-d liczba\tpoziom logowania\n");
		printf("\t-p nazwa\t\tnazwa linka\n");
		printf("\t-r opis\t\topis linka\n");
		printf("\t-t file\t\tid transakcji\n");
		printf("\t-id identyfikator\tid pliku, na ktory ma wskazywac link\n");
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

	/*przygotowanie danych niezbędnych do uruchomienia funkcji web-owej*/
	mtds = (struct bmd230__mtdsValues *)calloc(1, sizeof(struct bmd230__mtdsValues));
	mtds->__size = 3;
	mtds->__ptr = (struct bmd230__mtdSingleValue *)calloc(mtds->__size, sizeof(struct bmd230__mtdSingleValue));
	
	mtds->__ptr[0].mtdOid = OID_SYS_METADATA_CRYPTO_OBJECTS_POINTING_ID;
	mtds->__ptr[0].mtdValue = pointingId;
	mtds->__ptr[0].mtdDesc = "";
	mtds->__ptr[1].mtdOid = OID_SYS_METADATA_CERT_USER_ID;
	mtds->__ptr[1].mtdValue = "1";
	mtds->__ptr[1].mtdDesc = "";
	mtds->__ptr[2].mtdOid = OID_SYS_METADATA_CERT_GROUP_ID;
	mtds->__ptr[2].mtdValue = "1";
	mtds->__ptr[2].mtdDesc = "";


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

	status = soap_call_bmd230__bmdCreateLink(&soap, host, NULL, base64Cert, NULL, NULL, NULL, mtds, description, file, transactionId, GRANTING_ALLOWED, /*NULL*/&expirationDate, &result);

	if (status == SOAP_OK)
	{
		/*********************************************************************************/
		/************************ funkcja odpowiedź ****************************************/
		printf("Link utworzony w archiwum; id %li\n",result);

		/*********************************************************************************/
		/*********************************************************************************/
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
