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
struct bmd230__manyCascadeLinksResult* result=NULL;
long i=0;
long iter=0;
char *transactionId = NULL;


struct bmd230__cascadeLinksInfo cascadeLinksInfo;

	cascadeLinksInfo.__size=2;
	cascadeLinksInfo.__ptr=(struct bmd230__singleCascadeLinkInfo*)calloc(cascadeLinksInfo.__size, sizeof(struct bmd230__singleCascadeLinkInfo));
	
	cascadeLinksInfo.__ptr[0].correspondingFilesMaxLevel=0;
	cascadeLinksInfo.__ptr[0].visible=ALL;
	cascadeLinksInfo.__ptr[0].linkInfo=(struct bmd230__singleLinkInfo*)calloc(1, sizeof(struct bmd230__singleLinkInfo));
	cascadeLinksInfo.__ptr[0].linkInfo->pointingId		= "1";
	cascadeLinksInfo.__ptr[0].linkInfo->ownerIdentityId	= "1";
	cascadeLinksInfo.__ptr[0].linkInfo->ownerCertificateId	= NULL;
	cascadeLinksInfo.__ptr[0].linkInfo->ownerGroupId	= "1";
	cascadeLinksInfo.__ptr[0].linkInfo->linkName		= NULL;
	cascadeLinksInfo.__ptr[0].linkInfo->linkDescription	= "link stworzony kaskadowo (1)";
	cascadeLinksInfo.__ptr[0].linkInfo->granting		= GRANTING_ALLOWED;
	cascadeLinksInfo.__ptr[0].linkInfo->expirationDate	= NULL;

	cascadeLinksInfo.__ptr[0].linkInfo->expirationDate=(struct bmd230__DateTime*)calloc(1, sizeof(struct bmd230__DateTime));
	cascadeLinksInfo.__ptr[0].linkInfo->expirationDate->year=2011;
	cascadeLinksInfo.__ptr[0].linkInfo->expirationDate->month=6;
	cascadeLinksInfo.__ptr[0].linkInfo->expirationDate->day=10;
	cascadeLinksInfo.__ptr[0].linkInfo->expirationDate->hour=14;
	cascadeLinksInfo.__ptr[0].linkInfo->expirationDate->minute=53;
	cascadeLinksInfo.__ptr[0].linkInfo->expirationDate->second=21;

	
	cascadeLinksInfo.__ptr[1].correspondingFilesMaxLevel=0;
	cascadeLinksInfo.__ptr[1].visible=ALL;
	cascadeLinksInfo.__ptr[1].linkInfo=(struct bmd230__singleLinkInfo*)calloc(1, sizeof(struct bmd230__singleLinkInfo));
	cascadeLinksInfo.__ptr[1].linkInfo->pointingId		= "2";
	cascadeLinksInfo.__ptr[1].linkInfo->ownerIdentityId	= "1";
	cascadeLinksInfo.__ptr[1].linkInfo->ownerCertificateId	= NULL;
	cascadeLinksInfo.__ptr[1].linkInfo->ownerGroupId	= "1";
	cascadeLinksInfo.__ptr[1].linkInfo->linkName		= NULL;
	cascadeLinksInfo.__ptr[1].linkInfo->linkDescription	= "link stworzony kaskadowo (2)";
	cascadeLinksInfo.__ptr[1].linkInfo->granting		= GRANTING_ALLOWED;
	cascadeLinksInfo.__ptr[1].linkInfo->expirationDate	= NULL;

	_GLOBAL_debug_level=4;
	transactionId=strdup("0");
	

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
	
	status = soap_call_bmd230__bmdCreateManyCascadeLinks(&soap, host, NULL, base64Cert, NULL, NULL, NULL, &cascadeLinksInfo, transactionId, &result);

	if (status == SOAP_OK)
	{
		if(result == NULL)
			{ printf("result = NULL\n"); }
		else
		{
			for(i=0; i< result->__size; i++)
			{
				printf("Kod bledu dla elementu %li = %li\n", i, result->__ptr[i].errorCode);
				if(result->__ptr[i].idsList != NULL)
				{
					for(iter=0; iter<result->__ptr[i].idsList->__size; iter++)
					{
						printf("Dla %li elementu utworzono link z id: %i\n", i, result->__ptr[i].idsList->__ptr[iter]);
					}
				}
				else
				{
					printf("Dla %li elementu nie utworzono zadnego linku\n", i);
				}
			}
		}
	}
	else
	{
		/*odpowiedz w razie bledu*/
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
