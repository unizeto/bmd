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

#include "wsseapi.h"
#include "bmd_wsse.h"

//#define ENABLE_WS_SECURITY

int main()
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
struct bmd230__mtdsSearchValues *mtds = NULL;
struct bmd230__mtdsValues *formMtds = NULL;
struct bmd230__searchSortInfoList *sortInfo;
struct bmd230__searchResults *searchResults = NULL;
struct xsd__base64Binary *base64Cert = NULL;

int iter=0;
int jupiter=0;

#ifdef ENABLE_WS_SECURITY
bmdWSSecurityClient_t WSSEclientStruct;
#endif /*ifdef ENABLE_WS_SECURITY*/
struct soap *soap = soap_new1(SOAP_XML_CANONICAL); /// !!!!istotne aby wlaczona postac kanoninczna

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

#ifdef ENABLE_WS_SECURITY	
	/**WS SECURITY BEGIN ******/
	status=WSSecurity_client_init(soap, "wszczygiel072010_priv.pem", "12345678", "wszczygiel072010_cert.pem", "HereYouCanSetUserName", "HerYouCanSetPassword", &WSSEclientStruct);
	if(status != 0)
	{
		printf("WS Security client side error\n");
		return -1;
	}
	/**WS SECURITY END   ******/
#endif /*ifdef ENABLE_WS_SECURITY*/

	/*formularz - wydobywane metadane*/
	formMtds = (struct bmd230__mtdsValues *)malloc(sizeof(struct bmd230__mtdsValues));
 	formMtds->__size = 2;
 	formMtds->__ptr = (struct bmd230__mtdSingleValue *)malloc(sizeof(struct bmd230__mtdSingleValue)*(formMtds->__size));

	/*wlasciciel*/
 	formMtds->__ptr[0].mtdOid = OID_SYS_METADATA_CRYPTO_OBJECTS_OWNER;
	formMtds->__ptr[0].mtdValue = "";
	formMtds->__ptr[0].mtdDesc = "";
	/*numer faktury*/
	formMtds->__ptr[1].mtdOid = "1.2.616.1.113527.4.3.3.1";
	formMtds->__ptr[1].mtdValue = "";
	formMtds->__ptr[1].mtdDesc = "";

	
	

	/*przygotowanie danych niezbędnych do uruchomienia funkcji web-owej*/
 	mtds = (struct bmd230__mtdsSearchValues *)malloc(sizeof(struct bmd230__mtdsSearchValues));
 	mtds->__size = 3;
	mtds->__ptr=(struct bmd230__mtdSearchSingleValue*)malloc(sizeof(struct bmd230__mtdSearchSingleValue)*mtds->__size);
	
	
	mtds->__ptr[0].mtdOid = OID_ACTION_METADATA_USER_CLASS;
  	mtds->__ptr[0].mtdValue = "2";
  	mtds->__ptr[0].mtdStatement = NULL; //"POSITIVE";
	mtds->__ptr[0].mtdAdjustment = NULL;//"NONE";
	mtds->__ptr[0].noOfOpenBrackets = 0;
	mtds->__ptr[0].noOfCloseBrackets = 0;
	mtds->__ptr[0].mtdDesc = NULL; //"LIKE";
	mtds->__ptr[0].mtdLogicalOperator = NULL;
	

	mtds->__ptr[1].mtdOid = OID_ACTION_METADATA_ACTION;
	mtds->__ptr[1].mtdValue = "16";
	mtds->__ptr[1].mtdStatement = NULL; //"POSITIVE";
	mtds->__ptr[1].mtdAdjustment = NULL; //"NONE";
	mtds->__ptr[1].noOfOpenBrackets = 0;
	mtds->__ptr[1].noOfCloseBrackets = 0;
	mtds->__ptr[1].mtdDesc = NULL; //"LIKE";
	mtds->__ptr[1].mtdLogicalOperator = NULL; //"AND";
	
	
	mtds->__ptr[2].mtdOid = OID_ACTION_METADATA_ACTION;
	mtds->__ptr[2].mtdValue = "12";
	mtds->__ptr[2].mtdStatement = NULL; //"POSITIVE";
	mtds->__ptr[2].mtdAdjustment = NULL; //"NONE";
	mtds->__ptr[2].noOfOpenBrackets = 0;
	mtds->__ptr[2].noOfCloseBrackets = 0;
	mtds->__ptr[2].mtdDesc = NULL; //"LIKE";
	mtds->__ptr[2].mtdLogicalOperator = NULL; //"AND";
	

	/*mtds->__ptr[2].mtdOid = OID_SYS_METADATA_CRYPTO_OBJECTS_FILE_TYPE;
	mtds->__ptr[2].mtdValue = "";
	mtds->__ptr[2].mtdDesc = NULL; //"LIKE";
	mtds->__ptr[2].mtdStatement = NULL; //"POSITIVE";
	mtds->__ptr[2].mtdLogicalOperator = NULL; //"AND";
	mtds->__ptr[2].noOfOpenBrackets = 0;
	mtds->__ptr[2].noOfCloseBrackets = 0;
	mtds->__ptr[2].mtdAdjustment = NULL; //"NONE";
	
	mtds->__ptr[3].mtdOid = OID_SYS_METADATA_CRYPTO_OBJECTS_FILE_TYPE;
	mtds->__ptr[3].mtdValue = "link";
	mtds->__ptr[3].mtdDesc = NULL; //"LIKE";
	mtds->__ptr[3].mtdStatement = NULL; //"POSITIVE";
	mtds->__ptr[3].mtdLogicalOperator = "OR"; //"AND";
	mtds->__ptr[3].noOfOpenBrackets = 0;
	mtds->__ptr[3].noOfCloseBrackets = 0;
	mtds->__ptr[3].mtdAdjustment = NULL; //"NONE";*/

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

	/*sortowanie po nazwie pliku*/
	sortInfo = (struct bmd230__searchSortInfoList *)malloc(sizeof(struct bmd230__searchSortInfoList));
	sortInfo->__size = 1;
	sortInfo->__ptr = (struct bmd230__searchSortInfo *)malloc( sizeof(struct bmd230__searchSortInfo) * (sortInfo->__size) );
 	sortInfo->__ptr[0].sortOid = OID_SYS_METADATA_CRYPTO_OBJECTS_FILENAME;
 	sortInfo->__ptr[0].sortOrder = "ASC";


	soap_set_namespaces(soap, bmd230_namespaces);


	/* wywolanie uslugi */
	status=soap_call_bmd230__bmdSearchUnreceived(soap, host, NULL, base64Cert,
						"1"/*userRoleId*/,
						"1"/*userGroupId*/,
						NULL/*userClassId*/,
						0/*offset*/,
						1000/*limit*/,
						mtds,
						formMtds,
						sortInfo,
						&searchResults);

#ifdef ENABLE_WS_SECURITY
	WSSecurity_client_clear(&WSSEclientStruct);
#endif /*ifdef ENABLE_WS_SECURITY*/

	if (status == SOAP_OK)
	{
		printf("\nIlość znalezionych dokumentów: %i\n",searchResults->__size);
		
		for (iter=0; iter<searchResults->__size; iter++)
		{
			printf("\n---\n#%i:   id = <%li> --> nazwa pliku: <%s>   \n", iter,  searchResults->__ptr[iter].id, searchResults->__ptr[iter].filename);
			if(searchResults->__ptr[iter].mtds != NULL)
			{
				for (jupiter=0; jupiter<searchResults->__ptr[iter].mtds->__size; jupiter++)
				{
					printf("\n OID:<%s> | wartosc:<%s>",searchResults->__ptr[iter].mtds->__ptr[jupiter].mtdOid,searchResults->__ptr[iter].mtds->__ptr[jupiter].mtdValue);
				}
			}
			else
			{
				printf("\nMETADATA = NULL\n");
			}
		}
		printf("\n\nkoniec testu\n");
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

