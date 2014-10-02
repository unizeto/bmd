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
	long id = 0;
	struct ns__senderComplex* sender = NULL;

	_GLOBAL_debug_level=0;

	if (argc==5) {
 		for (i=1; i<argc; i++) {
			if (strcmp(argv[i],"-d")==0) {
				if (argc>i+1) _GLOBAL_debug_level=atoi(argv[i+1]);
			}
			if (strcmp(argv[i],"-p")==0) {
				if (argc>i+1) id = atol(argv[i+1]);
			}
		} 
	}
	else {
		printf("%s\n",argv[0]);
		printf("\nniepoprawne wywołanie\n\nuzyj ponizszych parametrow\n");
		printf("-------------------------------------------------------\n");
		printf("\t-d liczba\tpoziom logowania\n");
		printf("\t-p liczba\t\tid wystawcy\n");
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
	
	fprintf(stderr, "\n Na razie OK\n");
	
	status = soap_call_ns__awizoGetSenderInfo(&soap, host, NULL, id, &sender);
	
	/*********************************************************************************/
	/*********************************************************************************/

	if (status == SOAP_OK) {
	    fprintf(stderr, "\n\n+++ OK +++\n\n");

	    fprintf(stderr, "\n ------------------------------------------------------------ \n");
	    fprintf(stderr, "ID : %d \n", id);
	    fprintf(stderr, "NAME :  %s \n", sender->name);
	    fprintf(stderr, "EMAIL : %s \n", sender->email);
	    fprintf(stderr, "CREATED : %s \n", sender->created);
	    fprintf(stderr, "DELETED : %s \n", sender->deleted);
	    fprintf(stderr, "SMTPADDR : %s \n", sender->smtpaddr);
	    fprintf(stderr, "SMTPPORT : %s \n", sender->smtpport);
	    fprintf(stderr, "SMTPUSER : %s \n", sender->smtpuser);
	    fprintf(stderr, "SMTPPASSWORD : %s \n", sender->smtppassword);
	    fprintf(stderr, "PFXFILE : %s \n", sender->pfxfile);
	    fprintf(stderr, "PFXPIN : %s \n", sender->pfxpin);
	    fprintf(stderr, "MAXIMAGESIZE : %s \n", sender->maximagesize);
	    fprintf(stderr, "CSVDIR : %s \n", sender->csvdir);
	    fprintf(stderr, "CSVSEPARATOR : %s \n", sender->csvseparator);
	    fprintf(stderr, "AWIZOACTION : %d \n", sender->awizoaction);
	    fprintf(stderr, "ALLOWIMAGEABSENT : %d \n", sender->allowimageabsent);
	    fprintf(stderr, "SMSAPIURL : %s \n", sender->smsapiurl);
	    fprintf(stderr, "SMSAPIERROR : %s \n", sender->smsapierror);
	    fprintf(stderr, "\n ------------------------------------------------------------ \n");
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
