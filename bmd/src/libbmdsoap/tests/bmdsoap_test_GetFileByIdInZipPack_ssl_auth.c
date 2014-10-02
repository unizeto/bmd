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


void *dime_write_open(struct soap*, const char*, const char*, const char*);
void dime_write_close(struct soap*, void*);
int dime_write(struct soap*, void*, const char*, size_t);

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
struct xsd__base64Binary *base64Cert = NULL;

int i=0;
char *hash=NULL;
long int id=0;
GenBuf_t plik;
int stream = 0;

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
				if (argc>i+1) asprintf(&hash,"%s",argv[i+1]);
				printf("%s\n",hash);
				id = atol(hash);
				printf("%li\n",id);
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
		printf("\t-p liczba\tid pobieranego pliku\n");
		printf("\t-s 0|1\t\tnostream|stream\n");
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

	base64Cert->__ptr=(unsigned char*)cert_pem->buf;
	base64Cert->__size=cert_pem->size;


	soap_set_namespaces(&soap, bmd230_namespaces);
	

   switch(stream)
   {
     case 0 :  {
                 /*********************************************************************************/
                 /************************ funkcja testowa ****************************************/
                  status=soap_call_bmd230__bmdGetFileByIdInZipPack(&soap, host, NULL, base64Cert, NULL, NULL, NULL, id, nostream, &fileResponse);
                /*********************************************************************************/
                /*********************************************************************************/

                if (status == SOAP_OK)
                {
                  /*********************************************************************************/
                  /************************ funkcja odpowiedź ****************************************/
                   printf("Znaleziony plik: %s\nWielkość pliku: %i\n", fileResponse->filename,fileResponse->file->__size);
                   plik.buf=(char*)malloc(fileResponse->file->__size+2);
                   memset(plik.buf,0,fileResponse->file->__size+1);
                   memcpy(plik.buf,fileResponse->file->__ptr,fileResponse->file->__size);
                   plik.size=fileResponse->file->__size;
                   //printf("%s\n",fileResponse->file->__ptr);
                   bmd_save_buf(&plik,fileResponse->filename);
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

                 break;
               }

     case 1 :  {
                 soap.fdimewriteopen = dime_write_open;
                 soap.fdimewriteclose = dime_write_close;
                 soap.fdimewrite = dime_write;

                 status=soap_call_bmd230__bmdGetFileByIdInZipPack(&soap, host, NULL, base64Cert, NULL, NULL, NULL,id, stream, &fileResponse);

                 if (status == SOAP_OK)
                  {
                   /*********************************************************************************/
                   /************************ funkcja odpowiedź ****************************************/
                    printf("Znaleziony plik: %s\n", fileResponse->filename);

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

                 break;
               }
   }

	soap_end(&soap);
	soap_done(&soap);
	free(serverURL);

	return 0;
}

void *dime_write_open(struct soap *soap, const char *id, const char *type, const char *options)
{
  /* ----------------------- */

      FILE* handle   = NULL;

  /* ----------------------- */


    handle = fopen(options+4, "wb");

    if (!handle)
     {
        soap->error = SOAP_EOF;
        soap->errnum = errno; // get reason
     }

  return (void*)handle;

}


void dime_write_close(struct soap *soap, void *handle)
{
   fclose((FILE*)handle);
}


int dime_write(struct soap *soap, void *handle, const char *buf, size_t len)
{

  /* ---------------- */

     size_t nwritten;

  /* ---------------- */

    while (len)
     {
       nwritten = fwrite(buf, 1, len, (FILE*)handle);

       if (!nwritten)
        {
          soap->errnum = errno; // get reason

          return SOAP_EOF;
        }

       len -= nwritten;
       buf += nwritten;
     }

   return SOAP_OK;
}



