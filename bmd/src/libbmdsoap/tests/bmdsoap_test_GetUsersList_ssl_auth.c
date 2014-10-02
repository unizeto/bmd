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
struct bmd230__userListInfo *result;
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
struct bmd230__mtdsValues *certificateAttributes = NULL;
struct xsd__base64Binary *base64Cert = NULL;
char *serverResponse = NULL;
int i=0;
int j=0;

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

	certificateAttributes = (struct bmd230__mtdsValues *)malloc(sizeof(struct bmd230__mtdsValues));
 	certificateAttributes->__size = 13;
 	certificateAttributes->__ptr = (struct bmd230__mtdsSingleValue *)malloc(sizeof(struct bmd230__mtdSingleValue)*certificateAttributes->__size);
	certificateAttributes->__ptr[0].mtdOid = "1.2.616.1.113527.4.3.2.11.2"; // issuer
	certificateAttributes->__ptr[0].mtdValue = "Lesioo";
	certificateAttributes->__ptr[0].mtdDesc = "";
 	certificateAttributes->__ptr[1].mtdOid = "1.2.616.1.113527.4.3.2.11.3"; // serial
 	certificateAttributes->__ptr[1].mtdValue = "09 02 10";
 	certificateAttributes->__ptr[1].mtdDesc = "";
	certificateAttributes->__ptr[2].mtdOid = "1.2.616.1.113527.4.3.2.11.4"; // user name
 	certificateAttributes->__ptr[2].mtdValue = "Paweł Leśniak";
 	certificateAttributes->__ptr[2].mtdDesc = "";
	certificateAttributes->__ptr[3].mtdOid = "1.2.616.1.113527.4.3.2.11.6"; // user identity
 	certificateAttributes->__ptr[3].mtdValue = "CHERMANCDSADC";
 	certificateAttributes->__ptr[3].mtdDesc = "";
	certificateAttributes->__ptr[4].mtdOid = "2.5.4.72"; // rola
 	certificateAttributes->__ptr[4].mtdValue = "administrator";
 	certificateAttributes->__ptr[4].mtdDesc = "";
	certificateAttributes->__ptr[5].mtdOid = "1.3.6.1.5.5.7.10.4"; // grupa
 	certificateAttributes->__ptr[5].mtdValue = "GR2";
 	certificateAttributes->__ptr[5].mtdDesc = "";
	certificateAttributes->__ptr[6].mtdOid = "1.3.6.1.5.5.7.10.4"; // grupa
 	certificateAttributes->__ptr[6].mtdValue = "GR0";
 	certificateAttributes->__ptr[6].mtdDesc = "";
	certificateAttributes->__ptr[7].mtdOid = "1.3.6.1.5.5.7.10.4"; // grupa
 	certificateAttributes->__ptr[7].mtdValue = "GR11";
 	certificateAttributes->__ptr[7].mtdDesc = "";
	certificateAttributes->__ptr[8].mtdOid = "rr.1.2.616.1.113527.4.3.3.8";
 	certificateAttributes->__ptr[8].mtdValue = "00000000011";
 	certificateAttributes->__ptr[8].mtdDesc = "";
	certificateAttributes->__ptr[9].mtdOid = "rr.1.2.616.1.113527.4.3.3.5";
 	certificateAttributes->__ptr[9].mtdValue = "00000000011";
 	certificateAttributes->__ptr[9].mtdDesc = "";
	certificateAttributes->__ptr[10].mtdOid = "rr.1.2.616.1.113527.4.3.3.9";
 	certificateAttributes->__ptr[10].mtdValue = "00000000011";
 	certificateAttributes->__ptr[10].mtdDesc = "";
	certificateAttributes->__ptr[11].mtdOid = "2.5.1.5.55";
 	certificateAttributes->__ptr[11].mtdValue = "deployment|restricted";
 	certificateAttributes->__ptr[11].mtdDesc = "";
	certificateAttributes->__ptr[12].mtdOid = "2.5.1.5.55";
 	certificateAttributes->__ptr[12].mtdValue = "management|topsecret";
 	certificateAttributes->__ptr[12].mtdDesc = "";


	GenBuf_t *cert_pem=NULL;
	base64Cert = (struct xsd__base64Binary *)malloc(sizeof(struct xsd__base64Binary));
	status = bmd_load_binary_content(keyfile,&cert_pem);

	if (status != BMD_OK)
	{
		PRINT_DEBUG("SOAPCLIENTERR Error while reading certificate file\n");
		return 0;
	}

	base64Cert->__ptr=cert_pem->buf;
	base64Cert->__size=cert_pem->size;

	soap_set_namespaces(&soap, bmd230_namespaces);

	/*********************************************************************************/
	/************************ funkcja testowa ****************************************/
	status=soap_call_bmd230__bmdGetUsersList(		&soap, host, NULL, base64Cert, NULL, NULL,NULL,NULL,&result);
	/*********************************************************************************/
	/*********************************************************************************/
	if (status == SOAP_OK)
	{
		/*********************************************************************************/
		/************************ funkcja odpowiedź ****************************************/
		printf("Pobrano listę uzytkowników %i:\n",result->__size);

		for (i=0; i<result->__size; i++)
		{
			printf("-----------------------------------------\n");
			printf("userId:\t\t\t%s\n",result->__ptr[i].userId);
			printf("userCertIssuer:\t\t%s\n",result->__ptr[i].userCertIssuer);
			printf("userCertSerial:\t\t%s\n",result->__ptr[i].userCertSerial);
			printf("userName:\t\t%s\n",result->__ptr[i].userName);
			printf("userIdentity:\t\t%s\n",result->__ptr[i].userIdentity);
			printf("userAccepted:\t\t%s\n\n",result->__ptr[i].userAccepted);
			for (j=0; j<result->__ptr[i].userRoleNameList->__size; j++)
			{
				printf("\tuserRoleList: %s\t%s\n", result->__ptr[i].userRoleNameList->__ptr[j], result->__ptr[i].userRoleIdList->__ptr[j]);
			}
			for (j=0; j<result->__ptr[i].userGroupNameList->__size; j++)
			{
				printf("\tuserGroupList: %s\t%s\n", result->__ptr[i].userGroupNameList->__ptr[j], result->__ptr[i].userGroupIdList->__ptr[j]);
			}
			if ((result->__ptr[i].userClassNameList!=NULL) && (result->__ptr[i].userClassIdList!=NULL))
			{
				for (j=0; j<result->__ptr[i].userClassNameList->__size; j++)
				{
					printf("\tuserClassList: %s\t%s\n", result->__ptr[i].userClassNameList->__ptr[j], result->__ptr[i].userClassIdList->__ptr[j]);
				}
			}

			if (result->__ptr[i].userSecurityNameList!=NULL)
			{
				for (j=0; j<result->__ptr[i].userSecurityNameList->__size; j++)
				{
					printf("\tuserSecurityList: %s\n", result->__ptr[i].userSecurityNameList->__ptr[j]);
				}
			}

			printf("defaultRole:\t\t%s\t%s\n",result->__ptr[i].userDefaultRoleName, result->__ptr[i].userDefaultRoleId );
			printf("defaultGroup:\t\t%s\t%s\n",result->__ptr[i].userDefaultGroupName, result->__ptr[i].userDefaultGroupId );
			printf("defaultClass:\t\t%s\t%s\n",result->__ptr[i].userDefaultClassName, result->__ptr[i].userDefaultClassId );

			printf("-----------------------------------------\n\n\n\n\n\n");
     		}

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

