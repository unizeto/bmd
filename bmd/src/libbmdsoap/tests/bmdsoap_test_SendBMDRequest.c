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
struct bmd230__mtdsInfo *userMtds = NULL;
struct bmd230__searchResults *searchResults = NULL;
struct xsd__base64Binary *base64Cert = NULL;
char *serverResponse = NULL;
int *result = NULL;
int id=0;
int i=0;
int j=0;

	_GLOBAL_debug_level=0;
	if (argc==5)
	{
		for (i=1; i<argc; i++)
		{
			if (strcmp(argv[i],"-d")==0)
			{
				if (argc>i+1) _GLOBAL_debug_level=atoi(argv[i+1]);
			}
			if (strcmp(argv[i],"-p")==0)
			{
				if (argc>i+1) id=atoi(argv[i+1]);
			}
		}
	}
	else
	{
		printf("%s\n",argv[0]);
		printf("\nniepoprawne wywołanie\n\nuzyj ponizszych parametrow\n");
		printf("-------------------------------------------------------\n");
		printf("\t-d liczba\tpoziom logowania\n");
		printf("\t-p liczba\tid usuwanego pliku\n");
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
	struct bmd230__fileInfo *fileResponse = NULL;
	base64Cert = (struct xsd__base64Binary *)malloc(sizeof(struct xsd__base64Binary));
	status = bmd_load_binary_content(keyfile,&cert_pem);
	if (status != BMD_OK)
	{
		PRINT_DEBUG("SOAPCLIENTERR Error while reading certificate file\n");
		return 0;
	}
	base64Cert->__ptr=cert_pem->buf;
	base64Cert->__size=cert_pem->size;


	/*****************************************************************************************************************/
	/*****************************************************************************************************************/
	/*****************************************************************************************************************/
	/*****************************************************************************************************************/


	struct bmd230__bmdListDatagram *request		= NULL;
	struct bmd230__bmdListDatagram *response	= NULL;

	/******************************************/
	/*	alokacja pamieci na datagramset	*/
	/******************************************/
	request=(struct bmd230__bmdListDatagram *)malloc(sizeof (struct bmd230__bmdListDatagram ));

	/************************************/
	/*	alokacja pamieci na datagram	*/
	/************************************/
	request->__size=5;
	request->__ptr=(struct bmd230__bmdSingleDatagram *)malloc(sizeof (struct bmd230__bmdSingleDatagram )*request->__size);
	for (i=0; i<request->__size; i++)
	{
		request->__ptr[i].randId = 666;
		request->__ptr[i].filesRemaining = i;
		request->__ptr[i].datagramStatus = i*i;
		request->__ptr[i].datagramType = i*i +8;
		request->__ptr[i].actionMetaData=(struct bmd230__bmdListMetadata *)malloc(sizeof(struct bmd230__bmdListMetadata));
	/*	request->__ptr[i].sysMetaData=(struct bmd230__bmdListMetadata *)malloc(sizeof(struct bmd230__bmdListMetadata));
		request->__ptr[i].pkiMetaData=(struct bmd230__bmdListMetadata *)malloc(sizeof(struct bmd230__bmdListMetadata));
		request->__ptr[i].additionalMetaData=(struct bmd230__bmdListMetadata *)malloc(sizeof(struct bmd230__bmdListMetadata));
	*/
		request->__ptr[i].actionMetaData->__size=3;
	/*	request->__ptr[i].sysMetaData->__size=3;
		request->__ptr[i].pkiMetaData->__size=3;
		request->__ptr[i].additionalMetaData->__size=3;

	*/	request->__ptr[i].actionMetaData->__ptr=(struct bmd230__bmdSingleMetadata *)malloc(sizeof(struct bmd230__bmdSingleMetadata) * request->__ptr[i].actionMetaData->__size);
	/*	request->__ptr[i].sysMetaData->__ptr=(struct bmd230__bmdSingleMetadata *)malloc(sizeof(struct bmd230__bmdSingleMetadata) * request->__ptr[i].sysMetaData->__size);
		request->__ptr[i].pkiMetaData->__ptr=(struct bmd230__bmdSingleMetadata *)malloc(sizeof(struct bmd230__bmdSingleMetadata) * request->__ptr[i].pkiMetaData->__size);
		request->__ptr[i].additionalMetaData->__ptr=(struct bmd230__bmdSingleMetadata *)malloc(sizeof(struct bmd230__bmdSingleMetadata) * request->__ptr[i].additionalMetaData->__size);

	*/

		request->__ptr[i].protocolData=(struct xsd__base64Binary *)malloc(sizeof(struct xsd__base64Binary));
		request->__ptr[i].protocolData->__size=0;
		request->__ptr[i].protocolData->__ptr=(unsigned char *)malloc(sizeof(unsigned char));

		request->__ptr[i].protocolDataFilename=(struct xsd__base64Binary *)malloc(sizeof(struct xsd__base64Binary));
		request->__ptr[i].protocolDataFilename->__size=0;
		request->__ptr[i].protocolDataFilename->__ptr=(unsigned char *)malloc(sizeof(unsigned char));

		request->__ptr[i].protocolDataFileId=(struct xsd__base64Binary *)malloc(sizeof(struct xsd__base64Binary));
		request->__ptr[i].protocolDataFileId->__size=0;
		request->__ptr[i].protocolDataFileId->__ptr=(unsigned char *)malloc(sizeof(unsigned char));

		request->__ptr[i].protocolDataOwner=(struct xsd__base64Binary *)malloc(sizeof(struct xsd__base64Binary));
		request->__ptr[i].protocolDataOwner->__size=0;
		request->__ptr[i].protocolDataOwner->__ptr=(unsigned char *)malloc(sizeof(unsigned char));

		for (j=0; j<request->__ptr[i].actionMetaData->__size; j++)
		{
			request->__ptr[i].actionMetaData->__ptr[j].myId = i;
			request->__ptr[i].actionMetaData->__ptr[j].ownerId = j;

			request->__ptr[i].actionMetaData->__ptr[j].oid=(xsd__string)malloc(sizeof(xsd__string) * strlen("1.2.3.21.32423.12.32.3"));
			memset(request->__ptr[i].actionMetaData->__ptr[j].oid, 0, strlen("1.2.3.21.32423.12.32.3"));
			memcpy(request->__ptr[i].actionMetaData->__ptr[j].oid, "1.2.3.21.32423.12.32.3", strlen("1.2.3.21.32423.12.32.3"));

			request->__ptr[i].actionMetaData->__ptr[j].value=(struct xsd__base64Binary *)malloc(sizeof(struct xsd__base64Binary));
			request->__ptr[i].actionMetaData->__ptr[j].value->__ptr=(unsigned char *)malloc(sizeof(unsigned char));
			request->__ptr[i].actionMetaData->__ptr[j].value->__size=0;
		}

	/*	for (j=0; j<request->__ptr[i].sysMetaData->__size; j++)
		{
			request->__ptr[i].sysMetaData->__ptr[j].myId = i;
			request->__ptr[i].sysMetaData->__ptr[j].ownerId = j;

			request->__ptr[i].sysMetaData->__ptr[j].value=(struct xsd__base64Binary *)malloc(sizeof(struct xsd__base64Binary));
			request->__ptr[i].sysMetaData->__ptr[j].value->__ptr=(unsigned char *)malloc(sizeof(unsigned char));
			request->__ptr[i].sysMetaData->__ptr[j].value->__size=0;
		}

		for (j=0; j<request->__ptr[i].pkiMetaData->__size; j++)
		{
			request->__ptr[i].pkiMetaData->__ptr[j].myId = i;
			request->__ptr[i].pkiMetaData->__ptr[j].ownerId = j;

			request->__ptr[i].pkiMetaData->__ptr[j].value=(struct xsd__base64Binary *)malloc(sizeof(struct xsd__base64Binary));
			request->__ptr[i].pkiMetaData->__ptr[j].value->__ptr=(unsigned char *)malloc(sizeof(unsigned char));
			request->__ptr[i].pkiMetaData->__ptr[j].value->__size=0;
		}

		for (j=0; j<request->__ptr[i].additionalMetaData->__size; j++)
		{
			request->__ptr[i].additionalMetaData->__ptr[j].myId = i;
			request->__ptr[i].additionalMetaData->__ptr[j].ownerId = j;

			request->__ptr[i].additionalMetaData->__ptr[j].value=(struct xsd__base64Binary *)malloc(sizeof(struct xsd__base64Binary));
			request->__ptr[i].additionalMetaData->__ptr[j].value->__ptr=(unsigned char *)malloc(sizeof(unsigned char));
			request->__ptr[i].additionalMetaData->__ptr[j].value->__size=0;
		}*/
	}

	/************************************/
	/*	alokacja pamieci na metadane	*/
	/************************************/

	soap_set_namespaces(&soap, bmd230_namespaces);

	/*********************************************************************************/
	/************************ funkcja testowa ****************************************/
	status=soap_call_bmd230__bmdSendBMDRequest(&soap, host, NULL, request, &response);
	/*********************************************************************************/
	/*********************************************************************************/

	if (status == SOAP_OK)
	{
		/*********************************************************************************/
		/************************ funkcja odpowiedź ****************************************/
		printf("Plik usunięty z archiwum; result %i\n",result);
		/*********************************************************************************/
		/*********************************************************************************/
	}
	else
	{
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
