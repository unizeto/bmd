#include "bmdsoap_client.h"
#include <bmd/common/bmd-sds.h>
#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdconfig/libbmdconfig.h>
#include <conf_sections.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#define POSITIVE_STATEMENT		0
#define NEGATIVE_STATEMENT		1


/**
*Załadowanie ustawień konfiguracynjych serwera z pliku konfiguracyjnego
*@param brak
*@retval 0 - poprawne wykonanie funkcji
*@retval -1 - niepoprawne wykonanie funkcji
*/

int main(int argc, char *argv[])
{
bmd_conf *konfiguracja=NULL;
struct soap soap;
int result;
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

struct xsd__base64Binary *base64Cert = NULL;
int i=0;

	_GLOBAL_debug_level=0;
	if (argc==3)
	{
		for (i=1; i<argc; i++)
		{
			if (strcmp(argv[i],"-d")==0)
			{
				if (argc>i+1) _GLOBAL_debug_level=atoi(argv[i+1]);
			}
		}
	}
	else
	{
		printf("%s\n",argv[0]);
		printf("\nniepoprawne wywołanie\n\nuzyj ponizszych parametrow\n");
		printf("-------------------------------------------------------\n");
		printf("\t-d liczba\tpoziom logowania\n");
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

	struct bmd230__singleUserInfo *certificateAttributes = NULL;
	certificateAttributes = (struct bmd230__singleUserInfo *)calloc(1, sizeof(struct bmd230__singleUserInfo));

	certificateAttributes->userId=NULL;
	certificateAttributes->userCertIssuer="whatever";
	certificateAttributes->userCertSerial="abcd";
	certificateAttributes->userName="nieistotne";
	certificateAttributes->userIdentity="2";
	certificateAttributes->userAccepted=NULL;
// 	certificateAttributes->userRoleNameList=(struct bmd230__stringList *)malloc(sizeof(struct bmd230__stringList));
// 	certificateAttributes->userRoleNameList->__size=1;
// 	certificateAttributes->userRoleNameList->__ptr=(xsd__string *)malloc(sizeof(xsd__string)*certificateAttributes->userRoleNameList->__size);
// 	certificateAttributes->userRoleNameList->__ptr[0]="1";
// 	certificateAttributes->userGroupNameList=(struct bmd230__stringList *)malloc(sizeof(struct bmd230__stringList));
// 	certificateAttributes->userGroupNameList->__size=1;
// 	certificateAttributes->userGroupNameList->__ptr=(xsd__string *)malloc(sizeof(xsd__string)*certificateAttributes->userGroupNameList->__size);
// 	certificateAttributes->userGroupNameList->__ptr[0]="1";
// 	certificateAttributes->userSecurityNameList=(struct bmd230__stringList *)malloc(sizeof(struct bmd230__stringList));
// 	certificateAttributes->userSecurityNameList->__size=1;
// 	certificateAttributes->userSecurityNameList->__ptr=(xsd__string *)malloc(sizeof(xsd__string)*certificateAttributes->userSecurityNameList->__size);
// 	certificateAttributes->userSecurityNameList->__ptr[0]="1";


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

	/*********************************************************************************/
	/************************ funkcja testowa ****************************************/
	status=soap_call_bmd230__bmdUpdateUserRights(&soap, host, NULL, base64Cert, NULL, NULL, NULL, "2", certificateAttributes, &result);
	/*********************************************************************************/
	/*********************************************************************************/
	if (status == SOAP_OK)
	{
		/*********************************************************************************/
		/************************ funkcja odpowiedź ****************************************/
		printf("Rejestracja użytkownika powiodła się %i\n",result);

		/*********************************************************************************/
		/*********************************************************************************/
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

