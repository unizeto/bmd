#include "bmdsoap_client.h"
#include <bmd/libbmdsoap/libbmdsoap.h>

int main(int argc, char *argv[])
{
bmd_conf *konfiguracja=NULL;
struct soap soap;
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

struct xsd__base64Binary *base64Cert = NULL;
struct bmd230__ActionsHistoryList* result = NULL;

long i = 0;
long iter = 0;
char* bmdId = NULL;


	_GLOBAL_debug_level=3;
	if(argc == 5)
	{
		for (i=1; i<argc; i++)
		{
			if (strcmp(argv[i],"-d")==0)
			{
				if (argc>i+1)
				{
					_GLOBAL_debug_level=atoi(argv[i+1]);
				}
			}
			if (strcmp(argv[i],"-id")==0)
			{
				if (argc>i+1)
				{
					bmdId = strdup(argv[i+1]);
					if(bmdId == NULL)
						{ BMD_FOK(BMD_ERR_MEMORY); }
				}
			}
		}
	}
	else
	{
		printf("%s\n",argv[0]);
		printf("\nniepoprawne wywołanie\n\nużyj poniższych parametrów\n");
		printf("-------------------------------------------------------\n");
		printf("\t-d liczba\tpoziom logowania\n");
		printf("\t-id identyfikator\tid pliku, dla którego ma byc pobrania historia akcji\n");
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

	GenBuf_t *cert_pem=NULL;
	base64Cert = (struct xsd__base64Binary *)malloc(sizeof(struct xsd__base64Binary));
	status = bmd_load_binary_content(keyfile,&cert_pem);
	if (status != BMD_OK)
	{
		PRINT_DEBUG("SOAPCLIENTERR Error while reading certificate file\n");
		return 0;
	}
	base64Cert->__ptr=(unsigned char*)cert_pem->buf;
	base64Cert->__size=cert_pem->size;

	soap_set_namespaces(&soap, bmd230_namespaces);


	status = soap_call_bmd230__bmdGetActionsHistoryById(&soap, host, NULL, base64Cert, NULL, NULL, NULL, bmdId, &result);

	if (status == SOAP_OK)
	{
		printf("\nHistoria akcji:\n\n");
		for(iter=0; iter<result->__size; iter++)
		{
			printf("Akcja %li: objectId=<%s> objectType=<%s> excutor=<%s> class=<%s> action=<%s> actionDate=<%s> status=<%s> statusMessage=<%s>\n",iter+1, result->__ptr[iter].objectId, result->__ptr[iter].objectType,
			result->__ptr[iter].executorIdentityName,  result->__ptr[iter].executorClassName,
			result->__ptr[iter].actionName, result->__ptr[iter].actionDate,
			result->__ptr[iter].actionStatus, result->__ptr[iter].actionStatusMessage);
		}
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
