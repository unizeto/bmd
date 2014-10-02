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
int status						= 0;
long int ssl_enabled				= 0;
long int authenticate				= 0;
char *keyfile					= NULL;
char *keyfile_passwd				= NULL;
char *keyfile_ssl					= NULL;
char *keyfile_passwd_ssl			= NULL;
char *cacert					= NULL;
char *capath					= NULL;
char *host 						= NULL;
char *hash						= NULL;
bmd_conf *konfiguracja				= NULL;

int fd						= 0;
int i							= 0;
long int id						= 0;
char *tmpdir					= NULL;
char *count						= NULL;
char *final_filename				= NULL;
GenBuf_t *cert_pem				= NULL;
struct xsd__base64Binary *base64Cert	= NULL;
struct bmd230__fileInfo *fileResponse		= NULL;
struct soap soap;

	/******************************************************/
	/*	wczytanie parametrow uruchomieniowych programu	*/
	/******************************************************/
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
				if (argc>i+1) asprintf(&hash,"%s",argv[i+1]);
				printf("%s\n",hash);
				id = atol(hash);
				printf("%li\n",id);
			}
		}
	}
	else
	{
		printf("%s\n",argv[0]);
		printf("\nniepoprawne wywołanie\n\nuzyj ponizszych parametrow\n");
		printf("-------------------------------------------------------\n");
		printf("\t-d liczba\tpoziom logowania\n");
		printf("\t-p liczba\tid pobieranego pliku\n");
		printf("-------------------------------------------------------\n");
		return -1;
	}

	/******************************************/
	/*	wczytanie funkcji konfiguracyjnych	*/
	/******************************************/
	load_soap_configuration(&konfiguracja);
	configuration(konfiguracja,&host,&keyfile,&keyfile_passwd,&keyfile_ssl,&keyfile_passwd_ssl,&cacert,&capath,&ssl_enabled);

	/******************************************************/
	/*	funkcja ustanowienia połaczenia z serwerem	*/
	/******************************************************/
	BMD_FOK(connection(&soap,ssl_enabled,authenticate,keyfile_ssl,keyfile_passwd_ssl,cacert,capath));

	/************************************************************************/
	/*	przygotowanie danych niezbędnych do uruchomienia funkcji web-owej	*/
	/************************************************************************/
	BMD_FOK(bmd_load_binary_content(keyfile,&cert_pem));
	base64Cert = (struct xsd__base64Binary *)malloc(sizeof(struct xsd__base64Binary));
	base64Cert->__ptr=cert_pem->buf;
	base64Cert->__size=cert_pem->size;

	soap_set_namespaces(&soap, bmd230_namespaces);

	/************************************/
	/*	rozpoczecie pobierania chunka	*/
	/************************************/
	printf("starting chunk downloading...\t\t");
	status=soap_call_bmd230__bmdGetFileChunkStart(&soap, host, NULL,base64Cert, NULL, NULL,NULL, id,&tmpdir);
	if (status == SOAP_OK)
	{
		printf("OK (tmpdir=%s)\n",tmpdir);
	}
	else
	{
		printf("FAILED\n");
		soap_print_fault(&soap, stderr);
		soap_end(&soap);
		soap_done(&soap);
		return -1;
	}

	/******************************************/
	/*	kolejne iteracje pobierania chunkow	*/
	/******************************************/
	i=1;
	while(1)
	{
		asprintf(&count,"%i",i++);
		printf("downloading chunk number %s...\t\t",count);
		status=soap_call_bmd230__bmdGetFileChunkIter(&soap, host, NULL,base64Cert, NULL, NULL,NULL, tmpdir,count,&fileResponse);
		if (status == SOAP_OK)
		{
			if ((fd=open("lesioo-wynik.tet",O_RDWR|O_CREAT|O_APPEND,S_IRWXU|S_IRWXG|S_IRWXO))!=-1)
			{
				if (write(fd, fileResponse->file->__ptr, fileResponse->file->__size)==-1)
				{
					perror("error: ");
				}
				close(fd);
			}
			else
			{
				perror("error: ");
			}

			printf("OK (downloaded chunk %s)\n",fileResponse->filename);
			if (strcmp(fileResponse->filename,"")!=0)
			{
				break;
			}

		}
		else
		{
			printf("FAILED\n");
			soap_print_fault(&soap, stderr);
			soap_end(&soap);
			soap_done(&soap);
			return -1;
		}
	}

	soap_end(&soap);
	soap_done(&soap);

	return 0;
}
