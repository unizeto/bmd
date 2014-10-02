#include "bmdsoap_client.h"
#include <bmd/common/bmd-sds.h>
#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdconfig/libbmdconfig.h>
#include <bmd/libbmdsoap/libbmdsoap.h>
#include <conf_sections.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>


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

struct xsd__base64Binary *base64Cert = NULL;

int i=0;
char *transactionId = NULL;



struct bmd230__linksInfo* linksInfo=NULL;
struct bmd230__idList* ids=NULL;
long iter=0;

	_GLOBAL_debug_level=0;
	if (argc==5)
	{
		for (i=1; i<argc; i++)
		{
			if (strcmp(argv[i],"-d")==0)
			{
				if (argc>i+1) _GLOBAL_debug_level=atoi(argv[i+1]);
			}
			if (strcmp(argv[i],"-t")==0)
			{
				if (argc>i+1) asprintf(&transactionId,"%s",argv[i+1]);
			}
		}
	}
	else
	{
		printf("%s\n",argv[0]);
		printf("\nniepoprawne wywołanie\n\nuzyj ponizszych parametrow\n");
		printf("-------------------------------------------------------\n");
		printf("\t-d liczba\tpoziom logowania\n");
		printf("\t-t file\t\tid transakcji\n");
		printf("-------------------------------------------------------\n");
		return -1;
	}

	if (strcmp(transactionId,"0")==0)
	{
		printf("nulluje transakcje\n");
		free(transactionId); transactionId=NULL;
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

	linksInfo=(struct bmd230__linksInfo*)calloc(1, sizeof(struct bmd230__linksInfo));
	linksInfo->__size=2;
	linksInfo->__ptr=(struct bmd230__singleLinkInfo*)calloc(linksInfo->__size, sizeof(struct bmd230__singleLinkInfo));
	
	linksInfo->__ptr[0].pointingId=strdup("1");
	linksInfo->__ptr[0].ownerIdentityId=NULL;
	linksInfo->__ptr[0].ownerCertificateId=strdup("1");
	linksInfo->__ptr[0].ownerGroupId=strdup("1");
	linksInfo->__ptr[0].linkName=strdup("link1");
	linksInfo->__ptr[0].linkDescription=strdup("opis linka 1");
	linksInfo->__ptr[0].granting=GRANTING_ALLOWED;

// 	linksInfo->__ptr[0].expirationDate=(struct bmd230__DateTime*)calloc(1, sizeof(struct bmd230__DateTime));
// 	linksInfo->__ptr[0].expirationDate->year=2011;
// 	linksInfo->__ptr[0].expirationDate->month=2;
// 	linksInfo->__ptr[0].expirationDate->day=9;
// 	linksInfo->__ptr[0].expirationDate->hour=16;
// 	linksInfo->__ptr[0].expirationDate->minute=2;
// 	linksInfo->__ptr[0].expirationDate->second=38;


	linksInfo->__ptr[1].pointingId=strdup("20");
	linksInfo->__ptr[1].ownerIdentityId=NULL;
	linksInfo->__ptr[1].ownerCertificateId=strdup("1");
	linksInfo->__ptr[1].ownerGroupId=strdup("1");
	linksInfo->__ptr[1].linkName=strdup("link2");
	linksInfo->__ptr[1].linkDescription=strdup("opis linka 2");
	linksInfo->__ptr[1].granting=GRANTING_ALLOWED;
	linksInfo->__ptr[1].expirationDate=(struct bmd230__DateTime*)calloc(1, sizeof(struct bmd230__DateTime));
	linksInfo->__ptr[1].expirationDate->year=2012;
	linksInfo->__ptr[1].expirationDate->month=3;
	linksInfo->__ptr[1].expirationDate->day=10;
	linksInfo->__ptr[1].expirationDate->hour=17;
	linksInfo->__ptr[1].expirationDate->minute=3;
	linksInfo->__ptr[1].expirationDate->second=39;

	

	GenBuf_t *cert_pem=NULL;
	base64Cert = (struct xsd__base64Binary *)malloc(sizeof(struct xsd__base64Binary));
	status = bmd_load_binary_content(keyfile,&cert_pem);
	if (status != BMD_OK)
	{
		PRINT_DEBUG("SOAPCLIENTERR Error while reading certificate file\n");
		return 0;
	}
	base64Cert->__ptr=(unsigned char*)(cert_pem->buf);
	base64Cert->__size=cert_pem->size;

	soap_set_namespaces(&soap, bmd230_namespaces);

	status = soap_call_bmd230__bmdCreateLinks(&soap, host, NULL, base64Cert, NULL, NULL, NULL, linksInfo, transactionId, &ids);

	if (status == SOAP_OK)
	{
		/*********************************************************************************/
		/************************ funkcja odpowiedź ****************************************/
		for(iter=0; iter<ids->__size; iter++)
		{
			printf("Link nr %li z identyfikatorem id %i\n", iter, ids->__ptr[iter]);	
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
