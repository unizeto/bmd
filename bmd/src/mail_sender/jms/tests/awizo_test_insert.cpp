#include "bmdsoap_client.h"
#include "soapH.h"
#include <bmd/common/bmd-sds.h>
#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdconfig/libbmdconfig.h>
#include <stdio.h>

/* #include <sys/types.h> */
/* #include <sys/stat.h> */
/* #include <fcntl.h> */
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
	struct soap* soap;
	
	soap = soap_new();
	
//	char *serverURL = NULL; 
	char *host = NULL;
	char *file = NULL;
	int status = 0;
	long i = 0;
        struct ns1__putResponse result; 
	GenBuf_t *tmp_file = NULL;
	char* tmp = NULL;

	_GLOBAL_debug_level=0;

        using namespace std;
        
        std::string param2("Udostępniono fakturę");
        std::string param3("awizo");

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

	/*funkcje konfiguracyjne*/
	
	load_soap_configuration(&konfiguracja);
	configuration(konfiguracja,&host);


	/*funkcja ustanowienia połaczenia z serwerem*/
	
	status=connection(soap);
	if (status!=SOAP_OK)
	{
		PRINT_DEBUG("SOAPLOGCLIENTERR Connection error\n");
		return 0;
	}


	status = bmd_load_binary_content(file,&tmp_file);

        if (status != 0) {
         printf("coś nie tak z plikiem :)\n");
         exit(-1);
        }

        tmp = (char*)calloc(tmp_file->size+1, sizeof(char));
        memcpy(tmp, tmp_file->buf, tmp_file->size);
        tmp[tmp_file->size+1] = '\0';
			
	std::string param1(tmp);

	/*********************************************************************************/
	/************************ funkcja testowa ****************************************/
	
	//status=soap_call_ns__logServerSend(&soap, host, NULL, logDatagram, &result);
	//status = soap_call_ns__awizoSend(&soap,host,NULL, xmlMessage, &result);
        //status = soap_call_ns1__put(&soap, host, NULL, tmp_file->buf, "Udostępniono fakturę", "awizo", result);

        status = soap_call_ns1__put(soap, host, NULL, param1, param2, param3, result);
	
	/*********************************************************************************/
	/*********************************************************************************/

	if (status == SOAP_OK)
	{
	    fprintf(stderr, "\n\n+++ OK +++\n\n");
	}
	else
	{
	
		/*odpowiedź w razie błędu*/
		
		//soap_print_fault(&soap, stderr);
		//soap_end(&soap);
		//soap_done(&soap);
		/*free(serverURL);*/
		return -1;
	}
//	soap_end(&soap);
//	soap_done(&soap);
//	free(serverURL);

	return 0;
}

