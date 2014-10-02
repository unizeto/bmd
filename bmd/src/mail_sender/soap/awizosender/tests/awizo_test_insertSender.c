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
	char *host = NULL;
	long status = 0;
	long result = 0;
	long i = 0;

	struct ns__senderComplex* sender = NULL;


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

	sender=(struct ns__senderComplex*)calloc(1, sizeof(struct ns__senderComplex));
	sender->name = "PLAY4";
	sender->email = "tomasz.klimek@unizeto.pl";
	sender->smtpaddr = "127.0.0.1"; 
	sender->smtpport = "25";
	sender->pfxfile = "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";
	sender->pfxpin = "tklimek:6";
	sender->awizoaction = 1;
	sender->allowimageabsent = 1;
	sender->smsapiurl = "http://api.smsapi.pl/send.do?username=unisms&password=555e877b41eb20ff3bdb65a7811b2b2a&to=__TO__&message=__MESSAGE__&eco=1&test=1";
	sender->smsapierror = "ERROR";

	/*********************************************************************************/
	 status = soap_call_ns__awizoInsertSender(&soap, host, NULL, sender, &result);
	/*********************************************************************************/

	if (status == SOAP_OK) {
	    fprintf(stderr, "\n\n+++ OK +++\n\n");
	 }
	else
	{
		/*odpowiedź w razie błędu*/
		soap_print_fault(&soap, stderr);
		soap_end(&soap);
		soap_done(&soap);
		return -1;
	}
	soap_end(&soap);
	soap_done(&soap);


	return 0;
}
