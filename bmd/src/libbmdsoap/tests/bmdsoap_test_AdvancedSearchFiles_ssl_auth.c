#include "bmdsoap_client.h"
#include "bmd230H.h"
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
struct bmd230__mtdsSearchValues *mtds = NULL;
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
 	formmtds->__size = 7;
 	formmtds->__ptr = (struct bmd230__mtdsSingleValue *)malloc(sizeof(struct bmd230__mtdSingleValue)*formmtds->__size);
	formmtds->__ptr[0].mtdOid = "1.2.616.1.113527.4.3.3.25";
	formmtds->__ptr[0].mtdValue = "";
	formmtds->__ptr[0].mtdDesc = "";
 	formmtds->__ptr[1].mtdOid = "1.2.616.1.113527.4.3.3.26";
 	formmtds->__ptr[1].mtdValue = "";
 	formmtds->__ptr[1].mtdDesc = "";
	formmtds->__ptr[2].mtdOid = "1.2.616.1.113527.4.3.3.22";
 	formmtds->__ptr[2].mtdValue = "";
 	formmtds->__ptr[2].mtdDesc = "";
	formmtds->__ptr[3].mtdOid = "1.2.616.1.113527.4.3.3.23";
 	formmtds->__ptr[3].mtdValue = "";
 	formmtds->__ptr[3].mtdDesc = "";
	formmtds->__ptr[4].mtdOid = "1.2.616.1.113527.4.3.3.24";
 	formmtds->__ptr[4].mtdValue = "";
 	formmtds->__ptr[4].mtdDesc = "";
	formmtds->__ptr[5].mtdOid = "1.2.616.1.113527.4.3.3.56";
 	formmtds->__ptr[5].mtdValue = "";
 	formmtds->__ptr[5].mtdDesc = "";
	formmtds->__ptr[6].mtdOid = "1.2.616.1.113527.4.3.3.57";
 	formmtds->__ptr[6].mtdValue = "";
 	formmtds->__ptr[6].mtdDesc = "";

	/*przygotowanie danych niezbędnych do uruchomienia funkcji web-owej*/
 	mtds = (struct bmd230__mtdsSearchValues *)malloc(sizeof(struct bmd230__mtdsSearchValues));
 	mtds->__size = 5;
 	mtds->__ptr = (struct bmd230__mtdSearchSingleValue *)malloc(sizeof(struct bmd230__mtdSearchSingleValue)*mtds->__size);
  	mtds->__ptr[0].mtdOid = "1.2.616.1.113527.4.3.2.5.4";
 	mtds->__ptr[0].mtdValue = "0";
 	mtds->__ptr[0].mtdDesc = "LIKE";
 	mtds->__ptr[0].mtdStatement = "POSITIVE";
	mtds->__ptr[0].mtdLogicalOperator = NULL;
	mtds->__ptr[0].noOfOpenBrackets = 1;
	mtds->__ptr[0].noOfCloseBrackets = 1;
	mtds->__ptr[0].mtdAdjustment = "END";

  	mtds->__ptr[1].mtdOid = "1.2.616.1.113527.4.3.3.8";
  	mtds->__ptr[1].mtdValue = "Lesioo@gmial.com";
  	mtds->__ptr[1].mtdDesc = "LIKE";
	mtds->__ptr[1].mtdStatement = "NEGATIVE";
	mtds->__ptr[1].mtdLogicalOperator = "AND";
	mtds->__ptr[1].noOfOpenBrackets = 0;
	mtds->__ptr[1].noOfCloseBrackets = 0;
	mtds->__ptr[1].mtdAdjustment = "END";

	mtds->__ptr[2].mtdOid = "1.2.616.1.113527.4.3.3.7.1";
 	mtds->__ptr[2].mtdValue = "2000-10-10";
 	mtds->__ptr[2].mtdDesc = "LIKE";
	mtds->__ptr[2].mtdStatement = "NEGATIVE";
	mtds->__ptr[2].mtdLogicalOperator = "OR";
	mtds->__ptr[2].noOfOpenBrackets = 1;
	mtds->__ptr[2].noOfCloseBrackets = 0;
	mtds->__ptr[2].mtdAdjustment = "BOTH";

 	mtds->__ptr[3].mtdOid = "1.2.616.1.113527.4.3.3.2.2";
  	mtds->__ptr[3].mtdValue = "893-239-1293-12";
  	mtds->__ptr[3].mtdDesc = "ILIKE";
	mtds->__ptr[3].mtdStatement = "POSITIVE";
	mtds->__ptr[3].mtdLogicalOperator = "AND";
	mtds->__ptr[3].noOfOpenBrackets = 0;
	mtds->__ptr[3].noOfCloseBrackets = 0;
	mtds->__ptr[3].mtdAdjustment = "BOTH";

 	mtds->__ptr[4].mtdOid = "1.2.616.1.113527.4.3.3.11.2";
 	mtds->__ptr[4].mtdValue = "123.43	";
 	mtds->__ptr[4].mtdDesc = "ILIKE";
	mtds->__ptr[4].mtdStatement = "NEGATIVE";
	mtds->__ptr[4].mtdLogicalOperator = "AND";
	mtds->__ptr[4].noOfOpenBrackets = 0;
	mtds->__ptr[4].noOfCloseBrackets = 1;
	mtds->__ptr[4].mtdAdjustment = "BEGIN";
//
// 	mtds->__ptr[2].mtdOid = "1.2.616.1.113527.4.3.2.5.2";
//   	mtds->__ptr[2].mtdValue = "filename";
//   	mtds->__ptr[2].mtdDesc = "ILIKE";
// 	mtds->__ptr[2].mtdStatement = "POSITIVE";
// 	mtds->__ptr[2].mtdLogicalOperator = "AND";
// 	mtds->__ptr[2].noOfOpenBrackets = 1;
// 	mtds->__ptr[2].noOfCloseBrackets = 2;
// 	mtds->__ptr[2].mtdAdjustment = "NONE";
//
// 	mtds->__ptr[3].mtdOid = "1.2.616.1.113527.4.3.2.5.7";
//   	mtds->__ptr[3].mtdValue = "1203222";
//   	mtds->__ptr[3].mtdDesc = "ILIKE";
// 	mtds->__ptr[3].mtdStatement = "NEGATIVE";
// 	mtds->__ptr[3].mtdLogicalOperator = "AND";
// 	mtds->__ptr[3].noOfOpenBrackets = 1;
// 	mtds->__ptr[3].noOfCloseBrackets = 1;
// 	mtds->__ptr[3].mtdAdjustment = "BOTH";
//
// 	mtds->__ptr[4].mtdOid = "1.2.616.1.113527.4.3.3.7";
//   	mtds->__ptr[4].mtdValue = "2000-01-01";
//   	mtds->__ptr[4].mtdDesc = "LIKE";
// 	mtds->__ptr[4].mtdStatement = "POSITIVE";
// 	mtds->__ptr[4].mtdLogicalOperator = "AND";
// 	mtds->__ptr[4].noOfOpenBrackets = 1;
// 	mtds->__ptr[4].noOfCloseBrackets = 1;
// 	mtds->__ptr[4].mtdAdjustment = "END";
//
// 	mtds->__ptr[5].mtdOid = "1.2.616.1.113527.4.3.3.12.1";
//   	mtds->__ptr[5].mtdValue = "1290.32";
//   	mtds->__ptr[5].mtdDesc = "LIKE";
// 	mtds->__ptr[5].mtdStatement = "NEGATIVE";
// 	mtds->__ptr[5].mtdLogicalOperator = "AND";
// 	mtds->__ptr[5].noOfOpenBrackets = 1;
// 	mtds->__ptr[5].noOfCloseBrackets = 1;
// 	mtds->__ptr[5].mtdAdjustment = "BEGIN";



// 	mtds->__ptr[0].mtdOid = "1.2.616.1.113527.4.3.3.57";
// 	mtds->__ptr[0].mtdValue = "0";
// 	mtds->__ptr[0].mtdDesc = "LIKE";
//
//
// 	mtds->__ptr[3].mtdOid = "1.2.616.1.113527.4.3.3.24";
// 	mtds->__ptr[3].mtdValue = "0";
// 	mtds->__ptr[3].mtdDesc = "EQUAL";
//
// 	mtds->__ptr[0].mtdOid = "1.2.616.1.113527.4.3.3.14.1";
// 	mtds->__ptr[0].mtdValue = "2000-01-01";
// 	mtds->__ptr[0].mtdDesc = "";
//
// 	mtds->__ptr[1].mtdOid = "1.2.616.1.113527.4.3.3.14.2";
// 	mtds->__ptr[1].mtdValue = "2010-01-01";
// 	mtds->__ptr[1].mtdDesc = "";


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
	sortInfo->__size = 0;
	sortInfo->__ptr = (struct bmd230__searchSortInfo *)malloc(sizeof(struct bmd230__searchSortInfo)*sortInfo->__size);
// 	sortInfo->__ptr[0].sortOid = "1.2.616.1.113527.4.3.2.5.3";
// 	sortInfo->__ptr[0].sortOrder = "DESC";
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
// 	status=soap_call_bmd230__bmdAdvancedSearchFiles(	&soap, host, NULL, base64Cert, NULL, NULL, mtds, formmtds, 0, 200, sortInfo, &searchResults);
	status=soap_call_bmd230__bmdAdvancedSearchFiles(	&soap, host, NULL, base64Cert, NULL, NULL, NULL, mtds, formmtds, 0, 200, NULL, NULL, sortInfo, &searchResults);
// 	status=soap_call_bmd230__bmdSearchFiles(		&soap, host, NULL, base64Cert, NULL, NULL, mtds, formmtds, 0,200,"AND","EQUAL", NULL ,sortInfo,&searchResults);
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
