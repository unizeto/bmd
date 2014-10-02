#include "bmdsoap_client.h"
#include "logH.h"
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
long result = 0;
struct log__logDatagram* logDatagram = NULL;



	_GLOBAL_debug_level=30;
	if (argc>1)
	{
		if (strcmp(argv[1],"-dd"))
		{
			_GLOBAL_debug_level=1;
		}
		else
		{
			if (strcmp(argv[1],"-d"))
			{
				_GLOBAL_debug_level=30;
			}
		}
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

	logDatagram = (struct log__logDatagram*)calloc(1,sizeof(struct log__logDatagram));
	logDatagram->logLevel = "11:4k";
	logDatagram->logOwner = "P1";
	logDatagram->service  = "service";
	logDatagram->operationType  = "operation";
	//logDatagram->logReferto  = "";
	//logDatagram->eventCode = "";
	//logDatagram->logRefertoEventVisible = "";
	//logDatagram->remoteHost = "";
	//logDatagram->remotePort = "";
	logDatagram->logString = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?><Envelope><Body><loguj><wpis id=\"315f061c-e2c4-4da8-9f73-16f46d627a5b\"><czasZdarzenia>2014-09-05T17:37:13.69Z</czasZdarzenia><idOsoby extension=\"ABC\" root=\"2.16.840.1.113883.3.4424.1\"/><kodPodsystemu extension=\"BDE\" root=\"2.16.840.1.113883.3.4424.12.3\"/><kodUslugi/><podmiotZdarzenia><adresIP>127.0.0.1</adresIP><idSystemuZewnetrznego extension=\"\" root=\"\"/><idUzytkownikaKoncowego extension=\"\" root=\"\"/><kodPodsystemuWolajacego extension=\"EFG\" root=\"2.16.840.1.113883.3.4424.12.3\"/><podmiot extension=\"\" root=\"\"/><sposobUwierzytelnieniaUzytkownikaKoncowego/><uzytkownikSystemuZewnetrznego/></podmiotZdarzenia><przetwarzaneDane><idObiektu>ABCDE</idObiektu><idDokumentu extension=\"\" root=\"\"/><numerSygnaturyKontrolnej/><opisZakresuDanych/><rodzajDanych/></przetwarzaneDane><uuidZadaniaInicjujacego/></wpis></loguj></Body></Envelope>";
	//logDatagram->logReason = "";
	//logDatagram->logSolution = "";
    //logDatagram->documentSize = "";
	//logDatagram->documentFilename = "";
	//logDatagram->logReferto = "";
	//logDatagram->logRefertoEventVisible = "";
	logDatagram->dateTimeBegin = "2014-09-17-09.05.56.124000";
	

	char dane[128] = "";
	
	soap_set_namespaces(&soap, log_namespaces);

	/*********************************************************************************/
	/************************ funkcja testowa ****************************************/
	
	status=soap_call_log__logServerSend(&soap, host, NULL, logDatagram, &result);
	//status=soap_call_ns__verifyTree(&soap, host, NULL, 23, &result);
	//status = soap_call_log__verifyLog(&soap, host, NULL, 12214, &result );
	
	/*********************************************************************************/
	/*********************************************************************************/

	if (status == SOAP_OK)
	{
	    fprintf(stderr, "\n\n+++ OK +++\n\n");
	    fprintf(stderr, "\n\n RESULT : %ld \n\n", result);
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
