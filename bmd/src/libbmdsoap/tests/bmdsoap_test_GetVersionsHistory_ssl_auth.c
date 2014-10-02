#include "bmdsoap_client.h"
#include <bmd/libbmderr/libbmderr.h>

#include "wsseapi.h"
#include "bmd_wsse.h"

int main(int argc, char** argv)
{
bmd_conf *konfiguracja=NULL;

char *serverURL			= NULL;
char *host				= NULL;
long int ssl_enabled	= 0;
long int authenticate	= 0;
char *keyfile			= NULL;
char *keyfile_passwd	= NULL;
char *keyfile_ssl		= NULL;
char *keyfile_passwd_ssl= NULL;
char *cacert			= NULL;
char *capath			= NULL;

int status									= 0;
struct xsd__base64Binary *base64Cert		= NULL;
long iter									= 0;
long jupiter								= 0;

struct bmd230__GetVersionsHistoryResultList* result	= NULL;
struct bmd230__LongNumbersList ids;


//bmdWSSecurityClient_t WSSEclientStruct;

struct soap *soap = soap_new1(SOAP_XML_CANONICAL); /// !!!!istotne aby wlaczona postac kanoninczna

	if(argc <= 1)
	{
		printf("Blad: Niepoprawne wywolanie testu!\nPoprawne wywolanie: %s id1 id2 id3 itd. \n", argv[0]);
		return -1;
	}

	ids.__size=argc-1;
	ids.__ptr=(long*)calloc(ids.__size, sizeof(long));
	assert(ids.__ptr != NULL);
	for(iter=1; iter<argc; iter++)
	{
		ids.__ptr[iter-1]=strtol(argv[iter], NULL, 10);
		if(ids.__ptr[iter-1] <=0 || ids.__ptr[iter-1] == LONG_MIN || ids.__ptr[iter-1] == LONG_MAX)
			{
				printf("Blad wywolania! Nieprawidlowe id nr %li w liscie argumentow (podales '%s')\n", iter, argv[iter]);
				return -1;
			}
	}

	/*zaladowanie bibliotek ssl-owych*/
	SSL_load_error_strings();
        SSL_library_init();


	/*funkcje konfiguracyjne*/
	load_soap_configuration(&konfiguracja);
	configuration(konfiguracja,&host,&keyfile,&keyfile_passwd,&keyfile_ssl,&keyfile_passwd_ssl,&cacert,&capath,&ssl_enabled);


	/*funkcja ustanowienia połaczenia z serwerem*/
	status=connection(soap,ssl_enabled,authenticate,keyfile_ssl,keyfile_passwd_ssl,cacert,capath);
	if (status!=SOAP_OK)
	{
		printf("SOAPCLIENTERR Connection error\n");
		return 0;
	}

	
	/**WS SECURITY BEGIN ******/
	/**
	status=WSSecurity_client_init(soap, "wszczygiel072010_priv.pem", "12345678", "wszczygiel072010_cert.pem", "HereYouCanSetUserName", "HerYouCanSetPassword", &WSSEclientStruct);
	if(status != 0)
	{
		printf("WS Security client side error\n");
		return -1;
	}
	*/
	/**WS SECURITY END   ******/	

	GenBuf_t *cert_pem=NULL;
	base64Cert = (struct xsd__base64Binary *)malloc(sizeof(struct xsd__base64Binary));
	status = bmd_load_binary_content(keyfile,&cert_pem);

	if (status != BMD_OK)
	{
		printf("SOAPCLIENTERR Error while reading certificate file\n");
		return 0;
	}

	base64Cert->__ptr=(unsigned char*)cert_pem->buf;
	base64Cert->__size=cert_pem->size;

	soap_set_namespaces(soap, bmd230_namespaces);


	/* wywolanie uslugi */
	status=soap_call_bmd230__bmdGetVersionsHistory(soap, host, NULL, base64Cert,
						NULL, NULL, NULL,
						&ids,
						&result);

	/**
	WSSecurity_client_clear(&WSSEclientStruct);
	*/

	if (status == SOAP_OK)
	{
		printf("\nLiczba odpowiedzi: %i\n", result->__size);
		
		for (iter=0; iter<result->__size; iter++)
		{
			if(result->__ptr[iter].errorCode == 0)
			{
				printf("Plik nr %li (id=%li): OK\n", iter, ids.__ptr[iter]);
				if(result->__ptr[iter].versionsInfoList->__size == 0)
				{
					printf("\tBrak wersji archiwalnych dla pliku nr %li (id=%li).\n", iter, ids.__ptr[iter]);
				}
				else
				{
					for(jupiter=0; jupiter<result->__ptr[iter].versionsInfoList->__size; jupiter++)
					{
						printf("\tZnaleziona wersja z id %li z dnia (%s)\n", result->__ptr[iter].versionsInfoList->__ptr[jupiter].id, result->__ptr[iter].versionsInfoList->__ptr[jupiter].insertDate);
					}
				}
			}
			else
			{
				printf("Plik nr %li (id=%li): blad = %li\n", iter, ids.__ptr[iter], result->__ptr[iter].errorCode);
			}
			
		}
	}
	else
	{
		/*odpowiedź w razie błędu*/
		soap_print_fault(soap, stderr);
		soap_end(soap);
		soap_done(soap);
		free(serverURL);
		return -1;
	}

	soap_end(soap);
	soap_done(soap);
	free(serverURL);

	return 0;
}

