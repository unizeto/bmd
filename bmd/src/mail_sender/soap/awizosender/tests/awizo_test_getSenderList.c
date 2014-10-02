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
	int status = 0;
	long i = 0;
	struct ns__senderComplexList* senderList = NULL;

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
	
	//status=soap_call_ns__logServerSend(&soap, host, NULL, logDatagram, &result);
	status = soap_call_ns__awizoGetSendersList(&soap,host,NULL,&senderList);
	
	/*********************************************************************************/
	/*********************************************************************************/

	if (status == SOAP_OK)
	{
	    fprintf(stderr, "\n\n+++ OK +++\n\n");
	    
	    fprintf(stderr, "\n %d \n", senderList->__size);
	    
	    for(i=0;i<senderList->__size;i++){
	        fprintf(stderr, "\n ------------------------------------------------------------ \n");
	        fprintf(stderr, " %d \n", senderList->__ptr[i].id);
	        fprintf(stderr, " %s \n", senderList->__ptr[i].name);
	        fprintf(stderr, " %s \n", senderList->__ptr[i].email);
	        fprintf(stderr, " %s \n", senderList->__ptr[i].created);
	        fprintf(stderr, " %s \n", senderList->__ptr[i].deleted);
	        fprintf(stderr, " %s \n", senderList->__ptr[i].smtpaddr);
	        fprintf(stderr, " %s \n", senderList->__ptr[i].smtpport);
	        fprintf(stderr, " %s \n", senderList->__ptr[i].smtpuser);
	        fprintf(stderr, " %s \n", senderList->__ptr[i].smtppassword);
	        fprintf(stderr, " %s \n", senderList->__ptr[i].pfxpin);
	        fprintf(stderr, " %s \n", senderList->__ptr[i].maximagesize);
	        fprintf(stderr, " %s \n", senderList->__ptr[i].csvdir);
	        fprintf(stderr, " %s \n", senderList->__ptr[i].csvseparator);
	        fprintf(stderr, " %d \n", senderList->__ptr[i].awizoaction);
	        fprintf(stderr, " %d \n", senderList->__ptr[i].allowimageabsent);
	        fprintf(stderr, " %s \n", senderList->__ptr[i].smsapiurl);
	        fprintf(stderr, " %s \n", senderList->__ptr[i].smsapierror);
	        fprintf(stderr, "\n ------------------------------------------------------------ \n");
	    }
	    
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