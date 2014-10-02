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

struct bmd230__loginResponse *loginResponse=NULL;
struct xsd__base64Binary *base64Cert = NULL;

long i=0;
char *roleName	= NULL;
char *groupName	= NULL;

	_GLOBAL_debug_level=0;
	if (argc==7)
	{
		for (i=1; i<argc; i++)
		{
			if (strcmp(argv[i],"-d")==0)
			{
				if (argc>i+1) _GLOBAL_debug_level=atoi(argv[i+1]);
			}
			if (strcmp(argv[i],"-r")==0)
			{
				if (argc>i+1)
				{
					if (strcmp(argv[i+1], "0")!=0)
					{
						asprintf(&roleName,"%s",argv[i+1]);
					}
				}
			}
			if (strcmp(argv[i],"-g")==0)
			{
				if (argc>i+1)
				{
					if (strcmp(argv[i+1], "0")!=0)
					{
						asprintf(&groupName,"%s",argv[i+1]);
					}
				}
			}
		}
	}
	else
	{
		printf("%s\n",argv[0]);
		printf("\nniepoprawne wywołanie\n\nuzyj ponizszych parametrow\n");
		printf("-------------------------------------------------------\n");
		printf("\t-d liczba\tpoziom logowania\n");
		printf("\t-r roleId\tid roli\n");
		printf("\t-g groupId\tid grupy\n");
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

	/*przygotowanie danych niezbędnych do uruchomienia funkcji web-owej*/
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
	status=soap_call_bmd230__bmdLogin(&soap, host, NULL, base64Cert, roleName, groupName, NULL, &loginResponse);
	/*********************************************************************************/
	/*********************************************************************************/

	if (status == SOAP_OK)
	{

		/*********************************************************************************/
		/************************ funkcja odpowiedź ****************************************/
 		int i=0;
		printf("userRoleList %i\n--------------------------\n", loginResponse->userInfo->userRoleIdList->__size);
		for (i=0; i<loginResponse->userInfo->userRoleIdList->__size; i++)
		{
			printf("%s\t%s\n", loginResponse->userInfo->userRoleIdList->__ptr[i], loginResponse->userInfo->userRoleNameList->__ptr[i]);
		}
		printf("\n");

		printf("userDefaultRole \n--------------------------\n");
		printf("%s\t%s\n", loginResponse->userInfo->userDefaultRoleId, loginResponse->userInfo->userDefaultRoleName);
		printf("\n");

		printf("userGroupList %i\n--------------------------\n", loginResponse->userInfo->userGroupIdList->__size);
		for (i=0; i<loginResponse->userInfo->userGroupIdList->__size; i++)
		{
			printf("%s\t%s\n", loginResponse->userInfo->userGroupIdList->__ptr[i], loginResponse->userInfo->userGroupNameList->__ptr[i]);
		}
		printf("\n");

		printf("userDefaultGroup \n--------------------------\n");
		printf("%s\t%s\n", loginResponse->userInfo->userDefaultGroupId, loginResponse->userInfo->userDefaultGroupName);
		printf("\n");

		printf("userClassList %i\n--------------------------\n", loginResponse->userInfo->userClassIdList->__size);
		for (i=0; i<loginResponse->userInfo->userClassIdList->__size; i++)
		{
			printf("%s\t%s\n", loginResponse->userInfo->userClassIdList->__ptr[i], loginResponse->userInfo->userClassNameList->__ptr[i]);
		}
		printf("\n");

		printf("userDefaultClass \n--------------------------\n");
		printf("%s\t%s\n", loginResponse->userInfo->userDefaultClassId, loginResponse->userInfo->userDefaultClassName);
		printf("\n");

		printf("formSearch %i\n--------------------------\n", loginResponse->formSearch->__size);
		for (i=0; i<loginResponse->formSearch->__size; i++)
		{
			printf("%s\t%s\n", loginResponse->formSearch->__ptr[i].mtdOid, loginResponse->formSearch->__ptr[i].mtdDesc);
		}
		printf("\n");

		printf("formSend %i\n--------------------------\n", loginResponse->formSend->__size);
		for (i=0; i<loginResponse->formSend->__size; i++)
		{
			printf("%s\t%s\n", loginResponse->formSend->__ptr[i].mtdOid, loginResponse->formSend->__ptr[i].mtdDesc);
		}
		printf("\n");

		printf("formHistory %i\n--------------------------\n", loginResponse->formHistory->__size);
		for (i=0; i<loginResponse->formHistory->__size; i++)
		{
			printf("%s\t%s\n", loginResponse->formHistory->__ptr[i].mtdOid, loginResponse->formHistory->__ptr[i].mtdDesc);
		}
		printf("\n");

		printf("formUpdate %i\n--------------------------\n", loginResponse->formUpdate->__size);
		for (i=0; i<loginResponse->formUpdate->__size; i++)
		{
			printf("%s\t%s\n", loginResponse->formUpdate->__ptr[i].mtdOid, loginResponse->formUpdate->__ptr[i].mtdDesc);
		}
		printf("\n");

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
