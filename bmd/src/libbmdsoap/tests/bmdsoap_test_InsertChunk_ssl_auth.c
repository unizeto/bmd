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
struct bmd230__mtdsValues *mtds = NULL;
struct bmd230__searchResults *searchResults = NULL;
struct xsd__base64Binary *base64Cert = NULL;
char *serverResponse = NULL;
struct bmd230__fileInfo *input = NULL;
struct bmd230__fileInfo *output = NULL;
GenBuf_t *tmp_file = NULL;
long int result=0;
char *file=NULL;
int i=0;
char *filename			= NULL;
char * transactionId		= NULL;

	_GLOBAL_debug_level=0;
	if (argc==7)
	{
		for (i=1; i<argc; i++)
		{
			if (strcmp(argv[i],"-d")==0)
			{
				if (argc>i+1) _GLOBAL_debug_level=atoi(argv[i+1]);
			}
			if (strcmp(argv[i],"-p")==0)
			{
				if (argc>i+1) asprintf(&file,"%s",argv[i+1]);
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
		printf("\t-p file\t\tplik do dodania do bazy\n");
		printf("\t-t file\t\tid transakcji\n");
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
	mtds = (struct bmd230__mtdsValues *)malloc(sizeof(struct bmd230__mtdsValues));
	mtds->__ptr = (struct bmd230__mtdsSingleValue *)malloc(sizeof(struct bmd230__mtdSingleValue));
	mtds->__ptr[0].mtdOid = "1.2.616.1.113527.4.3.3.18";
	mtds->__ptr[0].mtdValue = "plesniak@unet2.pl";
	mtds->__ptr[0].mtdDesc = "";
	mtds->__size = 1;

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

	/*przygotowanie pliku do wysyłki*/
	input=(struct bmd230__fileInfo *)malloc(sizeof(struct bmd230__fileInfo));
	status = bmd_load_binary_content(file,&tmp_file);
	if (status != 0)
		printf("coś nie tak z plikiem :)\n");
	input->file=(struct xsd__base64Binary *)malloc(sizeof(struct xsd__base64Binary));
	input->file->__ptr=tmp_file->buf;
	input->file->__size=tmp_file->size;
	asprintf(&(input->filename),"%s",file);
	free(tmp_file); tmp_file=NULL;

	soap_set_namespaces(&soap, bmd230_namespaces);

	printf("start stream insert...");
	int rund = 10;
	char *tmpdir	= NULL;
	status = soap_call_bmd230__bmdInsertFileChunkStart(	&soap, host, NULL, base64Cert, NULL, NULL,NULL, mtds, file, input->file->__size*rund, NULL, &tmpdir);
	if ((status!=0) || (tmpdir==NULL))
	{
		soap_print_fault(&soap, stderr);
		soap_end(&soap);
		soap_done(&soap);
		free(serverURL);
		return -1;
	}

	printf("\t\t OK (%s)\n",tmpdir);
	char *count_c = NULL;
	int count_i	= 0;
	long int filesize=0;
	while (1)
	{
		if ((count_i+1)==rund)
		{
			asprintf(&count_c, "final");
			count_i++;
		}
		else
		{
			asprintf(&count_c, "%i", ++count_i);
		}
		printf("inserting part %s of %i...",count_c,rund);
		filesize+=input->file->__size;

		status=soap_call_bmd230__bmdInsertFileChunkIter(	&soap, host, NULL, base64Cert, NULL, NULL, NULL, input, tmpdir, count_c, &result);
		if (status!=0)
		{
			soap_print_fault(&soap, stderr);
			soap_end(&soap);
			soap_done(&soap);
			free(serverURL);
			return -1;
		}
		printf("\t\t OK (%s)\n",tmpdir);
		if (count_i==rund)
			break;
	}


	soap_end(&soap);
	soap_done(&soap);
	free(serverURL);

	return 0;
}
