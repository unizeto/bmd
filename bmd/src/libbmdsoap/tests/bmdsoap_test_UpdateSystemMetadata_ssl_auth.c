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

long result=0;
char *file=NULL;
int i=0;

	_GLOBAL_debug_level=0;
	if (argc==5)
	{
		for (i=1; i<argc; i++)
		{
			if (strcmp(argv[i],"-d")==0)
			{
				if (argc>i+1) _GLOBAL_debug_level=atoi(argv[i+1]);
			}
			if (strcmp(argv[i],"-id")==0)
			{
				if (argc>i+1) asprintf(&file,"%s",argv[i+1]);
			}
		}
	}
	else
	{
		printf("%s\n",argv[0]);
		printf("\nniepoprawne wywolanie\n\nuzyj ponizszych parametrow\n");
		printf("-------------------------------------------------------\n");
		printf("\t-d liczba\tpoziom logowania\n");
		printf("\t-id id\t\tid pliku do aktualizacji metadanych\n");
		printf("-------------------------------------------------------\n");
		return -1;
	}

	/*zaladowanie bibliotek ssl-owych*/
	SSL_load_error_strings();
        SSL_library_init();

	/*funkcje konfiguracyjne*/
	load_soap_configuration(&konfiguracja);
	configuration(konfiguracja,&host,&keyfile,&keyfile_passwd,&keyfile_ssl,&keyfile_passwd_ssl,&cacert,&capath,&ssl_enabled);

	/*funkcja ustanowienia polaczenia z serwerem*/
	status=connection(&soap,ssl_enabled,authenticate,keyfile_ssl,keyfile_passwd_ssl,cacert,capath);
	if (status!=SOAP_OK)
	{
		PRINT_DEBUG("SOAPCLIENTERR Connection error\n");
		return 0;
	}

	/*przygotowanie danych niezbednych do uruchomienia funkcji web-owej*/
	mtds = (struct bmd230__mtdsValues *)malloc(sizeof(struct bmd230__mtdsValues));
	mtds->__size = 2;
	mtds->__ptr = (struct bmd230__mtdSingleValue *)malloc(sizeof(struct bmd230__mtdSingleValue)*mtds->__size);
	mtds->__ptr[0].mtdOid = OID_SYS_METADATA_CRYPTO_OBJECTS_CORESPONDING_ID;
	mtds->__ptr[0].mtdValue = "57";
	mtds->__ptr[0].mtdDesc = "";
	mtds->__ptr[1].mtdOid = OID_SYS_METADATA_CRYPTO_OBJECTS_VISIBLE;
	mtds->__ptr[1].mtdValue = "0";
	mtds->__ptr[1].mtdDesc = "";

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

	/*********************************************************************************/
	/************************ funkcja testowa ****************************************/
	status = soap_call_bmd230__bmdUpdateSystemMetadata(&soap, host, NULL, base64Cert, NULL, NULL, NULL, atol(file), mtds, &result);
	/*********************************************************************************/
	/*********************************************************************************/

	if (status == SOAP_OK)
	{
		/*********************************************************************************/
		/************************ funkcja odpowiedz****************************************/
		printf("Metadane pliku zauktualizowane w archiwum; %li\n",result);

		/*********************************************************************************/
		/*********************************************************************************/
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
