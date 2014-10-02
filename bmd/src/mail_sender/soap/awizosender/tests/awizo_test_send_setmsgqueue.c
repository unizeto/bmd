#include "bmdsoap_client.h"
#include "soapH.h"
#include <bmd/common/bmd-sds.h>
#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdconfig/libbmdconfig.h>
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


int load_soap_configuration(bmd_conf **);
void configuration(bmd_conf*, char **);
int connection(struct soap*);


int main(int argc, char *argv[])
{
	bmd_conf *konfiguracja=NULL;
	struct soap soap;
	char *serverURL = NULL;
	char *host = NULL;
	long result = 0;
	int status = 0;
	long i = 0;

	_GLOBAL_debug_level=0;

	if (argc==3) {
 		for (i=1; i<argc; i++) {
			if (strcmp(argv[i],"-d")==0) {
				if (argc>i+1) _GLOBAL_debug_level=atoi(argv[i+1]);
			}
		} 
	}
	else {
		printf("%s\n",argv[0]);
		printf("\nniepoprawne wywołanie\n\nuzyj ponizszych parametrow\n");
		printf("-------------------------------------------------------\n");
		printf("\t-d liczba\tpoziom logowania\n");
		printf("-------------------------------------------------------\n");

		return -1;
 	}

	/*funkcje konfiguracyjne*/
	load_soap_configuration(&konfiguracja);
	configuration(konfiguracja,&host);

	/*funkcja ustanowienia połaczenia z serwerem*/
	status=connection(&soap);
	if (status!=SOAP_OK)
	{
		PRINT_DEBUG("SOAPLOGCLIENTERR Connection error\n");
		return 0;
	}


	/*********************************************************************************/
	/************************ funkcja testowa ****************************************/
	
	status = soap_call_ns__awizoSetFromMsgQueueSend(&soap,host,NULL, &result);
		
	/*********************************************************************************/
	/*********************************************************************************/

	if (status == SOAP_OK)
	{
	    fprintf(stderr, "\n\n+++ OK +++\n\n");
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
