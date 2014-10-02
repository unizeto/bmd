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
struct bmd230__searchResults *searchResults = NULL;
struct xsd__base64Binary *base64Cert = NULL;
char *serverResponse = NULL;
struct bmd230__mtdsValues *mtds = NULL;
struct bmd230__fileInfo *input = NULL;
GenBuf_t *tmp_file = NULL;
long int result=0;
char *file=NULL;
int i=0;
char *transactionId = NULL;
char *description=NULL;
int stream = 0;
FILE *fd = NULL;

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
			if (strcmp(argv[i],"-r")==0)
			{
				if (argc>i+1) asprintf(&description,"%s",argv[i+1]);
			}
		}
	}
	else
	{
		printf("%s\n",argv[0]);
		printf("\nniepoprawne wywołanie\n\nuzyj ponizszych parametrow\n");
		printf("-------------------------------------------------------\n");
		printf("\t-d liczba\tpoziom logowania\n");
		printf("\t-p nazwa\t\tnazwa katalogu\n");
		printf("\t-r opis\t\topis katalogu\n");
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

	/*przygotowanie danych niezbędnych do uruchomienia funkcji web-owej*/
	mtds = (struct bmd230__mtdsValues *)malloc(sizeof(struct bmd230__mtdsValues));
	mtds->__size = 2;
	mtds->__ptr = (struct bmd230__mtdsSingleValue *)malloc(sizeof(struct bmd230__mtdSingleValue)*mtds->__size);
	mtds->__ptr[0].mtdOid = "1.2.616.1.113527.4.3.3.8";
	mtds->__ptr[0].mtdValue = "lesioo";
	mtds->__ptr[0].mtdDesc = "";
	mtds->__ptr[1].mtdOid = OID_SYS_METADATA_CRYPTO_OBJECTS_CORESPONDING_ID;
	mtds->__ptr[1].mtdValue = "13";
	mtds->__ptr[1].mtdDesc = "";

// OID_SYS_METADATA_SEC_CAT
// OID_SYS_METADATA_BMD_SEC_LEVELS_STR

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

	/*przygotowanie pliku do wysyłki*/

        switch ( stream )
         {
            case 0 : {
// 	               status = bmd_load_binary_content(file,&tmp_file);
//
//                        if (status != 0)
//                         {
//                          printf("coś nie tak z plikiem :)\n");
//                          exit(-1);
//                         }
//
//                        input->file->__ptr=tmp_file->buf;
//                        input->file->__size=tmp_file->size;
//                        input->file->type=NULL;
//                        input->file->options=NULL;

                       /*********************************************************************************/
	               /************************ funkcja testowa ****************************************/
// 				status = soap_call_bmd230__bmdInsertFile(&soap, host, NULL, base64Cert, NULL, NULL, mtds, input, transactionId, nostream, &result);
				status = soap_call_bmd230__bmdCreateDirectory(&soap, host, NULL, base64Cert, NULL, NULL, NULL, mtds, description, file, transactionId, &result);
                       /*********************************************************************************/
                       /*********************************************************************************/

                       break;
                     }

            case 1 : {
                       soap_set_omode(&soap, SOAP_IO_CHUNK);
                       soap.fdimereadopen = dime_read_open;
                       soap.fdimereadclose = dime_read_close;
                       soap.fdimeread = dime_read;

                       fd = fopen(file, "r");

                       input->file->__ptr=(unsigned char*)fd;
                       input->file->__size=0;
                       input->file->type="";
                       input->file->options=soap_dime_option(&soap,0, strrchr(file,'/') ? strrchr(file,'/')+1 : file );

                       /*********************************************************************************/
	               /************************ funkcja testowa ****************************************/
//                        status = soap_call_bmd230__bmdInsertFile(&soap, host, NULL, base64Cert, NULL, NULL, mtds, input, transactionId, stream, &result);
                       /*********************************************************************************/
                       /*********************************************************************************/

                       break;
                     }

          default :  {
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
		printf("Katalog utworzony w archiwum; id %i\n",result);

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
