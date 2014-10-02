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
	struct ns__dateTimeComplexList* dateTime = NULL;
	struct ns__awizoMessage* xmlMessage = NULL;
	struct soap soap;
	char *host = NULL;
	char *file = NULL;
	long status = 0;
	long result = 0;
	long i = 0;
	GenBuf_t *tmp_file = NULL;

	_GLOBAL_debug_level=0;

        if (argc==5) {
	    for (i=1; i<argc; i++) {
		if (strcmp(argv[i],"-d")==0) {
		    if (argc>i+1) _GLOBAL_debug_level=atoi(argv[i+1]);
                }
                if (strcmp(argv[i],"-p")==0) {
		    if (argc>i+1) asprintf(&file,"%s",argv[i+1]);
                }
            }
	}
        else {                                                                                                                                               
		printf("%s\n",argv[0]);
    		printf("\nniepoprawne wywołanie\n\nuzyj ponizszych parametrow\n");
	        printf("-------------------------------------------------------\n");
    		printf("\t-d liczba\tpoziom logowania\n");
	        printf("\t-p file\t\tkomunikat\n");
	        printf("-------------------------------------------------------\n");

	        return -1;
	}

	/* funkcje konfiguracyjne */
	load_soap_configuration(&konfiguracja);
	configuration(konfiguracja,&host);

	/* funkcja ustanowienia połaczenia z serwerem */
	status=connection(&soap);
	if (status!=SOAP_OK)
	{
		PRINT_DEBUG("SOAPLOGCLIENTERR Connection error\n");
		return 0;
	}


	/**********************************************************************************************/
	/****************************** funkcja testowa ***********************************************/
	
	xmlMessage = (struct ns__awizoMessage*)calloc(1,sizeof(struct ns__awizoMessage));
	xmlMessage->value = (struct xsd__base64Binary*)malloc(sizeof(struct xsd__base64Binary));
	status = bmd_load_binary_content(file,&tmp_file);

	if (status != 0) {
	    printf("coś nie tak z plikiem :)\n");
	    exit(-1);
	}

	xmlMessage->value->__ptr=(unsigned char*)tmp_file->buf;
	xmlMessage->value->__size=tmp_file->size;

	dateTime = (struct ns__dateTimeComplexList*)calloc(1,sizeof(struct ns__dateTimeComplexList));

	if(dateTime == NULL) {
	    PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
	    return -1;
	}

	dateTime->__size = 1;
	dateTime->__ptr = (struct ns_dateTime*)calloc(dateTime->__size, sizeof(struct ns_dateTime));

	if(dateTime->__ptr == NULL ) {
	    PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
            return -1;
	}

	dateTime->__ptr[0].dateTime = "2011-02-24 09:51:58";
//	dateTime->__ptr[1].dateTime = "2011-02-25 09:51:59";
	
	/***************************************************************************************************/

	  status = soap_call_ns__addAdviceDispatchSetting(&soap, host, NULL, xmlMessage, dateTime, &result);

	/***************************************************************************************************/

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
