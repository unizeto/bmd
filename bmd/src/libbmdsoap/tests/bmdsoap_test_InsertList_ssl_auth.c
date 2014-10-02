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
struct bmd230__fileComplexList *input = NULL;
GenBuf_t *tmp_file = NULL;
struct bmd230__idList *result	= NULL;
char *file=NULL;
int i=0;
int ile_plikow=5;
int stream = 0;
char *transactionId	=NULL;
FILE **fd = NULL;

	_GLOBAL_debug_level=0;
	if (argc==9)
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
			if (strcmp(argv[i],"-s")==0)
			{
				if (argc>i+1) stream=atoi(argv[i+1]);
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
		printf("\t-s 0|1\t\tnostream|stream\n");
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

	/*przygotowanie danych niezbędnych do uruchomienia funkcji web-owej*/
	mtds = (struct bmd230__mtdsValues *)malloc(sizeof(struct bmd230__mtdsValues));
	mtds->__size = 3;
	mtds->__ptr = (struct bmd230__mtdsSingleValue *)malloc(sizeof(struct bmd230__mtdSingleValue)*mtds->__size);
	mtds->__ptr[0].mtdOid = "1.2.616.1.113527.4.3.3.24";
	mtds->__ptr[0].mtdValue = "0";
	mtds->__ptr[0].mtdDesc = "";
	mtds->__ptr[1].mtdOid = "1.2.616.1.113527.4.3.3.26";
	mtds->__ptr[1].mtdValue = "18038";
	mtds->__ptr[1].mtdDesc = "";
	mtds->__ptr[2].mtdOid = "1.2.616.1.113527.4.3.3.25";
	mtds->__ptr[2].mtdValue = "pdf";
	mtds->__ptr[2].mtdDesc = "";


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

	/*przygotowanie pliku do wysyłki*/
	/*input->file=(struct xsd__base64Binary *)malloc(sizeof(struct xsd__base64Binary));
	input->file->__ptr=tmp_file->buf;
	input->file->__size=tmp_file->size;
	asprintf(&(input->filename),"%s",file);*/


	/************************/
	/*	dane wejsciowe	*/
	/************************/
	input=(struct bmd230__fileComplexList *)malloc(sizeof(struct bmd230__fileComplexList)+2);
	input->__size=ile_plikow;
	input->__ptr=(struct bmd230__fileComplex *)malloc(sizeof(struct bmd230__fileComplex)*ile_plikow);
	fd = (FILE**)malloc(ile_plikow*sizeof(FILE*));


	soap_set_namespaces(&soap, bmd230_namespaces);

   switch ( stream )
    {
      case 0 :  {
                  status = bmd_load_binary_content(file,&tmp_file);

                  if (status != 0)
                   printf("coś nie tak z plikiem :)\n");

                  break;
                }
      case 1 :  {
                  soap_set_omode(&soap, SOAP_IO_CHUNK);
                  soap.fdimereadopen = dime_read_open;
                  soap.fdimereadclose = dime_read_close;
                  soap.fdimeread = dime_read;

                  break;
                }
      default : {
                  PRINT_ERROR("SOAPSERVERERR %s. Error=%i\n",GetErrorMsg(BMDSOAP_SERVER_INVALID_TRANSMISSION_TYPE),BMDSOAP_SERVER_INVALID_TRANSMISSION_TYPE);
                  soap_end(&soap);
                  soap_done(&soap);
                  free(serverURL);

                  return -1;
                }
    }


	for (i=0; i<input->__size; i++)
	{
		input->__ptr[i].file=(struct bmd230__fileInfo *)malloc(sizeof(struct bmd230__fileInfo));
		input->__ptr[i].file->file=(struct bmd230__myBinaryDataType *)malloc(sizeof(struct bmd230__myBinaryDataType));
		asprintf(&(input->__ptr[i].file->filename),"%s", strrchr(file,'/') ? strrchr(file,'/')+1 : file );
		input->__ptr[i].file->file->id=NULL;

      switch ( stream )
       {
         case 0 :  {
		      input->__ptr[i].file->file->__ptr=tmp_file->buf;
		      input->__ptr[i].file->file->__size=tmp_file->size;
		      input->__ptr[i].file->file->type=NULL;
		      input->__ptr[i].file->file->options=NULL;

                      break;
                   }

         case 1 :  {
                      fd[i] = fopen(input->__ptr[i].file->filename, "r");

                      input->__ptr[i].file->file->__ptr=(unsigned char*)fd[i];
                      input->__ptr[i].file->file->__size=0;
                      input->__ptr[i].file->file->type="";
                      input->__ptr[i].file->file->options=soap_dime_option(&soap,0,input->__ptr[i].file->filename);

                     break;
                   }

         default : {
                     PRINT_ERROR("SOAPSERVERERR %s. Error=%i\n",GetErrorMsg(BMDSOAP_SERVER_INVALID_TRANSMISSION_TYPE),BMDSOAP_SERVER_INVALID_TRANSMISSION_TYPE);
                     soap_end(&soap);
                     soap_done(&soap);
                     free(serverURL);

                     return -1;
                   }
       }

		input->__ptr[i].mtds=(struct bmd230__mtdsValues *)malloc(sizeof(struct bmd230__mtdsValues));
		input->__ptr[i].mtds->__ptr=(struct bmd230__mtdSingleValue *)malloc(sizeof(struct bmd230__mtdSingleValue)*10);
		input->__ptr[i].mtds->__ptr[0].mtdOid = "1.2.616.1.113527.4.3.3.8";
		input->__ptr[i].mtds->__ptr[0].mtdValue = "plesniak@unet.pl";
		input->__ptr[i].mtds->__ptr[0].mtdDesc = "";
		input->__ptr[i].mtds->__size = 1;
	}
	free(tmp_file); tmp_file=NULL;

	result=(struct bmd230__idList *)malloc(sizeof(struct bmd230__idList));
	result->__ptr=(xsd__int *)malloc(sizeof(xsd__int)*ile_plikow);
	result->__size=ile_plikow;

	/*********************************************************************************/
	/*********************************************************************************/

   switch (stream)
    {

     case 0 : {
	             /*********************************************************************************/
	             /************************ funkcja testowa ****************************************/
	              status = soap_call_bmd230__bmdInsertFileList(&soap, host, NULL, base64Cert, NULL, NULL, NULL, input, transactionId, nostream, &result);
	             /*********************************************************************************/
	             /*********************************************************************************/

                 break;
              }

     case 1 : {
	              status = soap_call_bmd230__bmdInsertFileList(&soap, host, NULL, base64Cert, NULL, NULL, NULL, input, transactionId, stream, &result);

                 break;
              }

     default : {
                 PRINT_ERROR("SOAPSERVERERR %s. Error=%i\n",GetErrorMsg(BMDSOAP_SERVER_INVALID_TRANSMISSION_TYPE),BMDSOAP_SERVER_INVALID_TRANSMISSION_TYPE);
                 soap_end(&soap);
                 soap_done(&soap);
                 free(serverURL);

                 return -1;
               }
    }


	if (status == SOAP_OK)
	{
		/*********************************************************************************/
		/************************ funkcja odpowiedź ****************************************/
		for (i=0; i<input->__size; i++)
		{
			printf("Plik  %i  zapisany w archiwum; id %i\n",i ,result->__ptr[i]);
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
