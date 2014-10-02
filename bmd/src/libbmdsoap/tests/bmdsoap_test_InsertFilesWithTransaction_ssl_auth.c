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
struct bmd230__mtdsInfo *userMtds = NULL;
struct bmd230__searchResults *searchResults = NULL;
struct xsd__base64Binary *base64Cert = NULL;
char *serverResponse = NULL;
int i=0;
struct bmd230__mtdsValues *mtds = NULL;
char *file=NULL;
struct bmd230__fileInfo *input = NULL;
GenBuf_t plik;
GenBuf_t *tmp_file = NULL;
long int result=0;

	_GLOBAL_debug_level=0;
	if (argc==5)
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
				printf("%s\n",file);
			}
		}
	}
	else
	{
		printf("%s\n",argv[0]);
		printf("\nniepoprawne wywołanie\n\nuzyj ponizszych parametrow\n");
		printf("-------------------------------------------------------\n");
		printf("\t-d liczba\tpoziom logowania\n");
		printf("\t-p pobierany plik\n");
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

	/*przygotowanie pliku do wysyłki*/
	input=(struct bmd230__fileInfo *)malloc(sizeof(struct bmd230__fileInfo));
	status = bmd_load_binary_content(file,&tmp_file);
	if (status != 0)
		printf("coś nie tak z plikiem :)\n");
	input->file=(struct xsd__base64Binary *)malloc(sizeof(struct xsd__base64Binary));
	input->file->__ptr=tmp_file->buf;
	input->file->__size=tmp_file->size;
	asprintf(&(input->filename),"%s",file);
	free(tmp_file); tmp_file=NULL;

	mtds = (struct bmd230__mtdsValues *)malloc(sizeof(struct bmd230__mtdsValues));
	mtds->__ptr = (struct bmd230__mtdsSingleValue *)malloc(sizeof(struct bmd230__mtdSingleValue));
//	mtds->__ptr[0].mtdOid = "1.2.616.1.113527.4.3.3.24";
//	mtds->__ptr[0].mtdValue = "1";
//	mtds->__ptr[0].mtdDesc = "";
	mtds->__ptr[0].mtdOid = "1.2.616.1.113527.4.3.3.8";
	mtds->__ptr[0].mtdValue = "plesniak@unet.pl";
	mtds->__ptr[0].mtdDesc = "";
	mtds->__ptr[1].mtdOid = "1.2.616.1.113527.4.3.3.26";
	mtds->__ptr[1].mtdValue = "38";
	mtds->__ptr[1].mtdDesc = "";
	mtds->__ptr[2].mtdOid = "1.2.616.1.113527.4.3.3.24";
	mtds->__ptr[2].mtdValue = "0";
	mtds->__ptr[2].mtdDesc = "";
	/*mtds->__ptr[3].mtdOid = "1,2,616,1,113527,4,3,2,1";
	mtds->__ptr[3].mtdValue = "management";
	mtds->__ptr[3].mtdDesc = "";
	mtds->__ptr[4].mtdOid = "2,5,1,5,55";
	mtds->__ptr[4].mtdValue = "topsecret";
	mtds->__ptr[4].mtdDesc = "";*/

	mtds->__size = 3;

	char *transactionId =NULL;

	soap_set_namespaces(&soap, bmd230_namespaces);

	/*********************************************************************************/
	/************************ funkcja testowa ****************************************/
	status=soap_call_bmd230__bmdStartTransaction( &soap, host, NULL, base64Cert, NULL, NULL, NULL, &transactionId);
	/*********************************************************************************/
	/*********************************************************************************/

	if (status == SOAP_OK)
	{
		printf("identyfikator transakcji = |%s|\n",transactionId);
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


	/*********************************************************************************/
	/************************ funkcja testowa ****************************************/
	status = soap_call_bmd230__bmdInsertFile(&soap, host, NULL, mtds, base64Cert, NULL, NULL, NULL, input, transactionId, nostream, &result);
	/*********************************************************************************/
	/*********************************************************************************/
	if (status == SOAP_OK)
	{
		/*********************************************************************************/
		/************************ funkcja odpowiedź ****************************************/
		printf("Plik zapisany w archiwum; id %i\n",result);
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

	/*********************************************************************************/
	/************************ funkcja testowa ****************************************/
	status = soap_call_bmd230__bmdInsertFile(&soap, host, NULL, mtds, base64Cert, NULL, NULL, NULL, input, transactionId, nostream, &result);
	/*********************************************************************************/
	/*********************************************************************************/
	if (status == SOAP_OK)
	{
		/*********************************************************************************/
		/************************ funkcja odpowiedź ****************************************/
		printf("Plik zapisany w archiwum; id %i\n",result);
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

	/*********************************************************************************/
	/************************ funkcja testowa ****************************************/
	status = soap_call_bmd230__bmdInsertFile(&soap, host, NULL, mtds, base64Cert, NULL, NULL, NULL, input, transactionId, nostream, &result);
	/*********************************************************************************/
	/*********************************************************************************/
	if (status == SOAP_OK)
	{
		/*********************************************************************************/
		/************************ funkcja odpowiedź ****************************************/
		printf("Plik zapisany w archiwum; id %i\n",result);
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

	getchar();

	status=soap_call_bmd230__bmdStopTransaction(&soap, host, NULL, base64Cert, NULL, NULL, NULL, transactionId, &result);
	if (status == SOAP_OK)
	{
		printf("identyfikator transakcji = |%s|\n",transactionId);
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
