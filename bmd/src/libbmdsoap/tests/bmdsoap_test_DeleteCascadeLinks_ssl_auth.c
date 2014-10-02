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



struct bmd230__deleteCascadeLinksList deleteCascadeLinksList;


	deleteCascadeLinksList.__size=1;
	deleteCascadeLinksList.__ptr=(struct bmd230__deleteCascadeLinksElement*)calloc(deleteCascadeLinksList.__size, sizeof(struct bmd230__deleteCascadeLinksElement));
	

	deleteCascadeLinksList.__ptr[0].ownerIdentityId="1";
	deleteCascadeLinksList.__ptr[0].ownerCertificateId=NULL;
	deleteCascadeLinksList.__ptr[0].correspondingFilesMaxLevel=0;
	deleteCascadeLinksList.__ptr[0].visible=ALL;
	deleteCascadeLinksList.__ptr[0].fileId="57";

//	deleteCascadeLinksList.__ptr[1].ownerIdentityId="1";
//	deleteCascadeLinksList.__ptr[1].ownerCertificateId=NULL;
// 	deleteCascadeLinksList.__ptr[1].correspondingFilesMaxLevel=0;
// 	deleteCascadeLinksList.__ptr[1].visible=ALL;
// 	deleteCascadeLinksList.__ptr[1].fileId="58";


	_GLOBAL_debug_level=4;
	
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
	
	status = soap_call_bmd230__bmdDeleteCascadeLinks(&soap, host, NULL, base64Cert, NULL, NULL, NULL, &deleteCascadeLinksList, &result);

	if (status == SOAP_OK)
	{
		if(result == NULL)
			{ printf("result = NULL\n"); }
		else
		{
			for(i=0; i< result->__size; i++)
			{
				if(result->__ptr[i].idsList != NULL)
				{
					if(result->__ptr[i].idsList->__size == 0)
					{
						printf("Dla elementu %li nie usunieto zadnego linka\n", i);
					}
					for(iter=0; iter<result->__ptr[i].idsList->__size; iter++)
					{
						printf("Dla %li elementu usunieto link z id: %i\n", i, result->__ptr[i].idsList->__ptr[iter]);
					}
				}
				else
				{
					printf("Dla %li elementu otrzymano kod bledu: %li\n", i, result->__ptr[i].errorCode);
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
