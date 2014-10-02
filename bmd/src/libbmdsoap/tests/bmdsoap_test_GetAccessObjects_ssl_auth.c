#include "bmdsoap_client.h"
#include <bmd/libbmderr/libbmderr.h>

#include "wsseapi.h"
#include "bmd_wsse.h"

int main(int argc, char** argv)
{
bmd_conf *konfiguracja=NULL;

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
struct bmd230__accessObjectList *accessObjects = NULL;
struct xsd__base64Binary *base64Cert = NULL;
long bmdId = 0;
long iter=0;

//bmdWSSecurityClient_t WSSEclientStruct;

struct soap *soap = soap_new1(SOAP_XML_CANONICAL); /// !!!!istotne aby wlaczona postac kanoninczna

	if(argc != 3)
	{
		printf("Blad: Niepoprawne wywolanie testu!\nPodaj identyfikator dokumentu wykorzystujac parametr -id\n");
		return -1;
	}

	if(strcmp(argv[1], "-id") != 0)
	{
		printf("Blad: Niepoprawne wywolanie testu!\nPodaj identyfikator dokumentu wykorzystujac parametr -id\n");
		return -2;
	}
	bmdId=strtol(argv[2], NULL, 10);
	

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
	status=soap_call_bmd230__bmdGetAccessObjects(soap, host, NULL, base64Cert,
						NULL, NULL, NULL,
						bmdId,
						VISIBLE,
						&accessObjects);

	/**
	WSSecurity_client_clear(&WSSEclientStruct);
	*/

	if (status == SOAP_OK)
	{
		printf("\nIlość wskazan na dokument: %i\n", accessObjects->__size);
		
		for (iter=0; iter<accessObjects->__size; iter++)
		{
			printf("\n---\n%li) id = <%s>\t| owner group id = <%s>\t | file type = <%s>\t | ownerIdentityId = <%s>\t | creatorIdentityId = <%s>\n", iter, accessObjects->__ptr[iter].id, accessObjects->__ptr[iter].ownerGroupId, accessObjects->__ptr[iter].fileType, accessObjects->__ptr[iter].ownerIdentityId, accessObjects->__ptr[iter].creatorIdentityId);
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

