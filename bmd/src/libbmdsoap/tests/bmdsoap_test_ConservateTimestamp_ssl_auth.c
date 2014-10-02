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
struct bmd230__mtdsSearchValues *mtds = NULL;
struct xsd__base64Binary *base64Cert = NULL;
char *serverResponse = NULL;
int i=0;
int result=0;

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

	/*przygotowanie danych niezbędnych do uruchomienia funkcji web-owej*/
 	mtds = (struct bmd230__mtdsSearchValues *)malloc(sizeof(struct bmd230__mtdsSearchValues));
 	mtds->__size = 0;
 	mtds->__ptr = (struct bmd230__mtdSearchSingleValue *)malloc(sizeof(struct bmd230__mtdSearchSingleValue)*mtds->__size);
//   	mtds->__ptr[0].mtdOid = "1.2.616.1.113527.4.4.2.3.2";
//  	mtds->__ptr[0].mtdValue = "0";
//  	mtds->__ptr[0].mtdDesc = "LIKE";
//  	mtds->__ptr[0].mtdStatement = "POSITIVE";
// 	mtds->__ptr[0].mtdLogicalOperator = NULL;
// 	mtds->__ptr[0].noOfOpenBrackets = 0;
// 	mtds->__ptr[0].noOfCloseBrackets = 0;
// 	mtds->__ptr[0].mtdAdjustment = "END";
//
//  	mtds->__ptr[1].mtdOid = OID_PKI_METADATA_TIMESTAMP_ID;
//  	mtds->__ptr[1].mtdValue = "100";
//  	mtds->__ptr[1].mtdDesc = "LIKE";
// 	mtds->__ptr[1].mtdStatement = "POSITIVE";
// 	mtds->__ptr[1].mtdLogicalOperator = "AND";
// 	mtds->__ptr[1].noOfOpenBrackets = 0;
// 	mtds->__ptr[1].noOfCloseBrackets = 0;
// 	mtds->__ptr[1].mtdAdjustment = "NONE";

/*	mtds->__ptr[5].mtdOid = OID_PKI_METADATA_TIMESTAMP_LOCATION_ID;
 	mtds->__ptr[5].mtdValue = "12345678";
 	mtds->__ptr[5].mtdDesc = "LIKE";
	mtds->__ptr[5].mtdStatement = "POSITIVE";
	mtds->__ptr[5].mtdLogicalOperator = "AND";
	mtds->__ptr[5].noOfOpenBrackets = 0;
	mtds->__ptr[5].noOfCloseBrackets = 0;
	mtds->__ptr[5].mtdAdjustment = "NONE";

	mtds->__ptr[6].mtdOid = OID_PKI_METADATA_TIMESTAMP_CRYPTO_OBJECTS;
 	mtds->__ptr[6].mtdValue = "15";
 	mtds->__ptr[6].mtdDesc = "LIKE";
	mtds->__ptr[6].mtdStatement = "POSITIVE";
	mtds->__ptr[6].mtdLogicalOperator = "AND";
	mtds->__ptr[6].noOfOpenBrackets = 0;
	mtds->__ptr[6].noOfCloseBrackets = 0;
	mtds->__ptr[6].mtdAdjustment = "NONE";

	mtds->__ptr[7].mtdOid = OID_PKI_METADATA_TIMESTAMP_OBJECTS;
 	mtds->__ptr[7].mtdValue = "10";
 	mtds->__ptr[7].mtdDesc = "LIKE";
	mtds->__ptr[7].mtdStatement = "POSITIVE";
	mtds->__ptr[7].mtdLogicalOperator = "AND";
	mtds->__ptr[7].noOfOpenBrackets = 0;
	mtds->__ptr[7].noOfCloseBrackets = 0;
	mtds->__ptr[7].mtdAdjustment = "NONE";

	mtds->__ptr[8].mtdOid = OID_PKI_METADATA_TIMESTAMP_VERSION;
 	mtds->__ptr[8].mtdValue = "1";
 	mtds->__ptr[8].mtdDesc = "LIKE";
	mtds->__ptr[8].mtdStatement = "POSITIVE";
	mtds->__ptr[8].mtdLogicalOperator = "AND";
	mtds->__ptr[8].noOfOpenBrackets = 0;
	mtds->__ptr[8].noOfCloseBrackets = 0;
	mtds->__ptr[8].mtdAdjustment = "NONE";

	mtds->__ptr[9].mtdOid = OID_PKI_METADATA_TIMESTAMP_POLICYOID;
 	mtds->__ptr[9].mtdValue = "1.1.1.1";
 	mtds->__ptr[9].mtdDesc = "LIKE";
	mtds->__ptr[9].mtdStatement = "POSITIVE";
	mtds->__ptr[9].mtdLogicalOperator = "AND";
	mtds->__ptr[9].noOfOpenBrackets = 0;
	mtds->__ptr[9].noOfCloseBrackets = 0;
	mtds->__ptr[9].mtdAdjustment = "NONE";

	mtds->__ptr[10].mtdOid = OID_PKI_METADATA_TIMESTAMP_SERIALNUMBER;
 	mtds->__ptr[10].mtdValue = "37 97 12";
 	mtds->__ptr[10].mtdDesc = "LIKE";
	mtds->__ptr[10].mtdStatement = "POSITIVE";
	mtds->__ptr[10].mtdLogicalOperator = "AND";
	mtds->__ptr[10].noOfOpenBrackets = 0;
	mtds->__ptr[10].noOfCloseBrackets = 0;
	mtds->__ptr[10].mtdAdjustment = "NONE";

	mtds->__ptr[11].mtdOid = OID_PKI_METADATA_TIMESTAMP_GENTIME;
 	mtds->__ptr[11].mtdValue = "2010-01-15";
 	mtds->__ptr[11].mtdDesc = "LIKE";
	mtds->__ptr[11].mtdStatement = "POSITIVE";
	mtds->__ptr[11].mtdLogicalOperator = "AND";
	mtds->__ptr[11].noOfOpenBrackets = 0;
	mtds->__ptr[11].noOfCloseBrackets = 0;
	mtds->__ptr[11].mtdAdjustment = "NONE";

	mtds->__ptr[12].mtdOid = OID_PKI_METADATA_TIMESTAMP_TIMESTAMPCERTVALIDTO;
 	mtds->__ptr[12].mtdValue = "2013-09-22";
 	mtds->__ptr[12].mtdDesc = "LIKE";
	mtds->__ptr[12].mtdStatement = "POSITIVE";
	mtds->__ptr[12].mtdLogicalOperator = "AND";
	mtds->__ptr[12].noOfOpenBrackets = 0;
	mtds->__ptr[12].noOfCloseBrackets = 0;
	mtds->__ptr[12].mtdAdjustment = "NONE";

	mtds->__ptr[13].mtdOid = OID_PKI_METADATA_TIMESTAMP_TSADN;
 	mtds->__ptr[13].mtdValue = "C=<PL> S=<Zachodniopomorskie> L=<Szczecin> O=<Unizeto Technologies SA> OU=<Dzial testow> CN=<Signing Key> emailAddress=<twojciechowski@unizeto.pl>";
 	mtds->__ptr[13].mtdDesc = "LIKE";
	mtds->__ptr[13].mtdStatement = "POSITIVE";
	mtds->__ptr[13].mtdLogicalOperator = "AND";
	mtds->__ptr[13].noOfOpenBrackets = 0;
	mtds->__ptr[13].noOfCloseBrackets = 0;
	mtds->__ptr[13].mtdAdjustment = "NONE";

	mtds->__ptr[14].mtdOid = OID_PKI_METADATA_TIMESTAMP_TSASN;
 	mtds->__ptr[14].mtdValue = "00 F4 48 60 2D 67 73 B1 4D";
 	mtds->__ptr[14].mtdDesc = "LIKE";
	mtds->__ptr[14].mtdStatement = "POSITIVE";
	mtds->__ptr[14].mtdLogicalOperator = "AND";
	mtds->__ptr[14].noOfOpenBrackets = 0;
	mtds->__ptr[14].noOfCloseBrackets = 0;
	mtds->__ptr[14].mtdAdjustment = "NONE";

	mtds->__ptr[15].mtdOid = OID_PKI_METADATA_TIMESTAMP_STANDARD;
 	mtds->__ptr[15].mtdValue = "1";
 	mtds->__ptr[15].mtdDesc = "LIKE";
	mtds->__ptr[15].mtdStatement = "POSITIVE";
	mtds->__ptr[15].mtdLogicalOperator = "AND";
	mtds->__ptr[15].noOfOpenBrackets = 0;
	mtds->__ptr[15].noOfCloseBrackets = 0;
	mtds->__ptr[15].mtdAdjustment = "NONE";

	mtds->__ptr[16].mtdOid = OID_PKI_METADATA_TIMESTAMP_USERORSYSTEM;
 	mtds->__ptr[16].mtdValue = "0";
 	mtds->__ptr[16].mtdDesc = "LIKE";
	mtds->__ptr[16].mtdStatement = "POSITIVE";
	mtds->__ptr[16].mtdLogicalOperator = "AND";
	mtds->__ptr[16].noOfOpenBrackets = 0;
	mtds->__ptr[16].noOfCloseBrackets = 0;
	mtds->__ptr[16].mtdAdjustment = "NONE";

	mtds->__ptr[17].mtdOid = OID_PKI_METADATA_TIMESTAMP_CONSERVED;
 	mtds->__ptr[17].mtdValue = "0";
 	mtds->__ptr[17].mtdDesc = "LIKE";
	mtds->__ptr[17].mtdStatement = "POSITIVE";
	mtds->__ptr[17].mtdLogicalOperator = "AND";
	mtds->__ptr[17].noOfOpenBrackets = 0;
	mtds->__ptr[17].noOfCloseBrackets = 0;
	mtds->__ptr[17].mtdAdjustment = "NONE";

	mtds->__ptr[18].mtdOid = OID_PKI_METADATA_TIMESTAMP_TIMESTAMP;
 	mtds->__ptr[18].mtdValue = "68867";
 	mtds->__ptr[18].mtdDesc = "LIKE";
	mtds->__ptr[18].mtdStatement = "POSITIVE";
	mtds->__ptr[18].mtdLogicalOperator = "AND";
	mtds->__ptr[18].noOfOpenBrackets = 0;
	mtds->__ptr[18].noOfCloseBrackets = 0;
	mtds->__ptr[18].mtdAdjustment = "NONE";

	mtds->__ptr[19].mtdOid = OID_PKI_METADATA_TIMESTAMP_TIMESTAMP_TYPE;
 	mtds->__ptr[19].mtdValue = "1.2.616.1.113527.4.4.8";
 	mtds->__ptr[19].mtdDesc = "LIKE";
	mtds->__ptr[19].mtdStatement = "POSITIVE";
	mtds->__ptr[19].mtdLogicalOperator = "AND";
	mtds->__ptr[19].noOfOpenBrackets = 0;
	mtds->__ptr[19].noOfCloseBrackets = 0;
	mtds->__ptr[19].mtdAdjustment = "NONE";

*/


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
	status=soap_call_bmd230__bmdConservateTimestamps(	&soap, host, NULL, base64Cert, NULL, NULL, NULL, mtds, &result);
	/*********************************************************************************/
	/*********************************************************************************/
	if (status == SOAP_OK)
	{
		/*********************************************************************************/
		/************************ funkcja odpowiedź ****************************************/
		printf("Wynik zadania: %i\n",result);
// 		int i=0;
// 		for (i=0; i<searchResults->__size; i++)
// 		{
// 			printf("\n%s \n",searchResults->__ptr[i].filename);
// 			int j=0;
// 			for (j=0; j<searchResults->__ptr[i].mtds->__size; j++)
// 			{
// 				printf("\t\t%s\t\t%s\t\t%s\n",searchResults->__ptr[i].mtds->__ptr[j].mtdOid,searchResults->__ptr[i].mtds->__ptr[j].mtdDesc,searchResults->__ptr[i].mtds->__ptr[j].mtdValue);
// 			}
// 		}

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

