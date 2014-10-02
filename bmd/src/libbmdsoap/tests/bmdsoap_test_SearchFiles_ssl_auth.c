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

extern long _GLOBAL_debug_level;

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
struct bmd230__mtdsInfo *userMtds = NULL;
struct bmd230__mtdsValues *mtds = NULL;
struct bmd230__mtdsValues *formmtds = NULL;
struct bmd230__searchSortInfoList *sortInfo;
struct bmd230__searchResults *searchResults = NULL;
struct xsd__base64Binary *base64Cert = NULL;
char *serverResponse = NULL;
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

	formmtds = (struct bmd230__mtdsValues *)malloc(sizeof(struct bmd230__mtdsValues));
 	formmtds->__size = 0;
 	formmtds->__ptr = (struct bmd230__mtdsSingleValue *)malloc(sizeof(struct bmd230__mtdSingleValue)*formmtds->__size);

// 	formmtds->__ptr[0].mtdOid = "1.2.616.1.113527.4.3.2.5.1";
// 	formmtds->__ptr[0].mtdValue = "";
//  	formmtds->__ptr[0].mtdDesc = "";
// 	formmtds->__ptr[1].mtdOid = "1.2.616.1.113527.4.3.2.5.2";
// 	formmtds->__ptr[1].mtdValue = "";
//  	formmtds->__ptr[1].mtdDesc = "";
// 	formmtds->__ptr[2].mtdOid = "1.2.616.1.113527.4.3.2.5.3";
// 	formmtds->__ptr[2].mtdValue = "";
//  	formmtds->__ptr[2].mtdDesc = "";
// 	formmtds->__ptr[3].mtdOid = "1.2.616.1.113527.4.3.2.5.4";
// 	formmtds->__ptr[3].mtdValue = "";
//  	formmtds->__ptr[3].mtdDesc = "";
// 	formmtds->__ptr[4].mtdOid = "1.2.616.1.113527.4.3.2.5.5";
// 	formmtds->__ptr[4].mtdValue = "";
//  	formmtds->__ptr[4].mtdDesc = "";
// 	formmtds->__ptr[5].mtdOid = "1.2.616.1.113527.4.3.2.5.6";
// 	formmtds->__ptr[5].mtdValue = "";
//  	formmtds->__ptr[5].mtdDesc = "";
// 	formmtds->__ptr[6].mtdOid = "1.2.616.1.113527.4.3.2.5.7";
// 	formmtds->__ptr[6].mtdValue = "";
//  	formmtds->__ptr[6].mtdDesc = "";
// 	formmtds->__ptr[7].mtdOid = "1.2.616.1.113527.4.3.2.5.8";
// 	formmtds->__ptr[7].mtdValue = "";
//  	formmtds->__ptr[7].mtdDesc = "";
// 	formmtds->__ptr[8].mtdOid = "1.2.616.1.113527.4.3.2.5.9";
// 	formmtds->__ptr[8].mtdValue = "";
//  	formmtds->__ptr[8].mtdDesc = "";



	/*przygotowanie danych niezbędnych do uruchomienia funkcji web-owej*/
 	mtds = (struct bmd230__mtdsValues *)malloc(sizeof(struct bmd230__mtdsValues));
 	mtds->__size = 6;
  	mtds->__ptr = (struct bmd230__mtdsSingleValue *)malloc(sizeof(struct bmd230__mtdSingleValue)*mtds->__size);
//     	mtds->__ptr[0].mtdOid = OID_SYS_METADATA_CRYPTO_OBJECTS_ID;
//    	mtds->__ptr[0].mtdValue = "10";
//    	mtds->__ptr[0].mtdDesc = "EXACT";

 	mtds->__ptr[0].mtdOid = "1.2.616.1.113527.4.3.3.8";
  	mtds->__ptr[0].mtdValue = "paw.lesniak@gmail.com";
  	mtds->__ptr[0].mtdDesc = "LIKE";

	mtds->__ptr[1].mtdOid = "1.2.616.1.113527.4.3.3.11.2";
 	mtds->__ptr[1].mtdValue = "123.43	";
 	mtds->__ptr[1].mtdDesc = "LIKE";

 	mtds->__ptr[2].mtdOid = "1.2.616.1.113527.4.3.3.11.2";
 	mtds->__ptr[2].mtdValue = "123.43	";
 	mtds->__ptr[2].mtdDesc = "LIKE";

	mtds->__ptr[3].mtdOid = "1.2.616.1.113527.4.3.2.5.2";
	mtds->__ptr[3].mtdValue = "filename";
	mtds->__ptr[3].mtdDesc = "EQUAL";

	mtds->__ptr[4].mtdOid = "1.2.616.1.113527.4.3.3.14.1";
	mtds->__ptr[4].mtdValue = "2000-01-01";
	mtds->__ptr[4].mtdDesc = "";

	mtds->__ptr[5].mtdOid = "1.2.616.1.113527.4.3.3.14.2";
	mtds->__ptr[5].mtdValue = "2010-01-01";
	mtds->__ptr[5].mtdDesc = "";


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

	sortInfo = (struct bmd230__searchSortInfoList *)malloc(sizeof(struct bmd230__searchSortInfoList));
	sortInfo->__size = 1;
	sortInfo->__ptr = (struct bmd230__searchSortInfo *)malloc(sizeof(struct bmd230__searchSortInfo)*sortInfo->__size);
	sortInfo->__ptr[0].sortOid = "1.2.616.1.113527.4.3.2.5.3";
	sortInfo->__ptr[0].sortOrder = "DESC";
// 	sortInfo->__ptr[1].sortOid = "1.2.616.1.113527.4.3.2.5.2";
// 	sortInfo->__ptr[1].sortOrder = "ASC";
// 	sortInfo->__ptr[2].sortOid = "1.2.616.1.113527.4.3.3.8";
// 	sortInfo->__ptr[2].sortOrder = "DESC";
// 	sortInfo->__ptr[3].sortOid = "1.2.616.1.113527.4.3.3.4";
// 	sortInfo->__ptr[3].sortOrder = "ASC";
// 	sortInfo->__ptr[4].sortOid = "1.2.616.1.113527.4.3.2.5.3";
// 	sortInfo->__ptr[4].sortOrder = "ASC";


	soap_set_namespaces(&soap, bmd230_namespaces);

	/*********************************************************************************/
	/************************ funkcja testowa ****************************************/
	status=soap_call_bmd230__bmdSearchFiles(&soap, host, NULL, base64Cert, NULL, NULL, NULL, mtds, formmtds, 0,200, "AND","EXACT",NULL, "GROUP", sortInfo,&searchResults);

	/*********************************************************************************/
	/*********************************************************************************/
	if (status == SOAP_OK)
	{
		/*********************************************************************************/
		/************************ funkcja odpowiedź ****************************************/
		printf("Znalezionych dokumentów: %i\n",searchResults->__size);
		int i=0;
		for (i=0; i<searchResults->__size; i++)
		{
			printf("\n%s \n",searchResults->__ptr[i].filename);
			int j=0;
			for (j=0; j<searchResults->__ptr[i].mtds->__size; j++)
			{
				printf("\t\t%s\t\t%s\t\t%s\n",searchResults->__ptr[i].mtds->__ptr[j].mtdOid,searchResults->__ptr[i].mtds->__ptr[j].mtdDesc,searchResults->__ptr[i].mtds->__ptr[j].mtdValue);
			}
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

