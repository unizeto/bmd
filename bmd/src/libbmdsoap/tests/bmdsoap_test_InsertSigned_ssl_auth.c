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
struct bmd230__mtdsValues *mtds = NULL;
struct xsd__base64Binary *base64Cert = NULL;
struct bmd230__myBinaryDataType *base64Signature = NULL;
struct bmd230__fileInfo *input = NULL;
GenBuf_t *tmp_file = NULL;
GenBuf_t *podpis = NULL;
long int result=0;
int stream = 0;
char *file=NULL;
char *sig=NULL;
int i=0;
char *transactionId=NULL;
char* signatureType	= NULL;
FILE* fd1 = NULL;
FILE* fd2 = NULL;
char* hashFile	= NULL;
GenBuf_t* hashBuf	= NULL;


	_GLOBAL_debug_level=0;
	
	if(argc != 11 && argc != 13)
	{
		printf("%s\n",argv[0]);
		printf("\nniepoprawne wywołanie\n\nuzyj ponizszych parametrow\n");
		printf("-------------------------------------------------------\n");
		printf("\t-d liczba\tpoziom logowania\n");
		printf("\t-p file\t\tplik do dodania do bazy\n");
		printf("\t-sC file\tpodpis (format CADES) do dodania do bazy\n");
		printf("\t-sX file\tpodpis (format XADES) do dodania do bazy\n");
		printf("\t-sP file\tpodpis wyodrebniony z podpisanego PDF \n");
		printf("\t-sha1 file\tplik zawierajacy skrot (w zapisie hex) z podpisanej czesci PDF \n");
		printf("\t-t file\t\tid transakcji\n");
		printf("\t-S 0|1\t\tnostream|stream\n");
		printf("-------------------------------------------------------\n");
		return -1;
	}

	
	for(i=1; i<argc; i++)
	{
		if (strcmp(argv[i],"-d")==0)
		{
			if (argc>i+1) _GLOBAL_debug_level=atoi(argv[i+1]);
		}
		if (strcmp(argv[i],"-p")==0)
		{
			if (argc>i+1) asprintf(&file,"%s",argv[i+1]);
		}
		if (strcmp(argv[i],"-sC")==0)
		{
			if(signatureType != NULL)
			{
				printf("Blad wywolania: Wybierz tylko jeden rodzaj podisu\n");
				return -1;
			}
			signatureType="CADES";
			if (argc>i+1) asprintf(&sig,"%s",argv[i+1]);
		}
		if (strcmp(argv[i],"-sX")==0)
		{
			if(signatureType != NULL)
			{
				printf("Blad wywolania: Wybierz tylko jeden rodzaj podisu\n");
				return -1;
			}
			signatureType="XADES";
			if (argc>i+1) asprintf(&sig,"%s",argv[i+1]);
		}
		if(strcmp(argv[i],"-sP")==0)
		{
			if(signatureType != NULL)
			{
				printf("Blad wywolania: Wybierz tylko jeden rodzaj podisu\n");
				return -1;
			}
			signatureType="PADES";
			if (argc>i+1) asprintf(&sig,"%s",argv[i+1]);
		}
		if(strcmp(argv[i],"-sha1")==0)
		{
			if (argc>i+1) asprintf(&hashFile,"%s",argv[i+1]);
		}
		if (strcmp(argv[i],"-t")==0)
		{
			if (argc>i+1) asprintf(&transactionId,"%s",argv[i+1]);
		}
		if (strcmp(argv[i],"-S")==0)
		{
			if (argc>i+1) stream=atoi(argv[i+1]);
		}
	}
	
	if(strcmp(signatureType, "PADES") == 0)
	{
		if( hashFile == NULL)
		{
			printf("Blad wywolania: Dla podisu PAdES musisz podac opcje -sha1\n");
			return -1;
		}
	}
	else
	{
		if(hashFile != NULL)
		{
			printf("Blad wywolania: Opcja -sha1 moze byc wywolana tylko dla podpisu PAdES\n");
			return -1;
		}
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
	if(strcmp(signatureType, "PADES") != 0)
	{
		mtds->__ptr = (struct bmd230__mtdSingleValue *)malloc(sizeof(struct bmd230__mtdSingleValue));
		mtds->__ptr[0].mtdOid = "1.2.616.1.113527.4.3.3.8";
		mtds->__ptr[0].mtdValue = "root@localhost";
		mtds->__ptr[0].mtdDesc = "";
		mtds->__size = 1;
	}
	else
	{
		mtds->__size = 3;
		mtds->__ptr = (struct bmd230__mtdSingleValue *)calloc(mtds->__size, sizeof(struct bmd230__mtdSingleValue));
		mtds->__ptr[0].mtdOid = "1.2.616.1.113527.4.3.3.8";
		mtds->__ptr[0].mtdValue = "root@localhost";
		mtds->__ptr[0].mtdDesc = "";
		
		assert(bmd_load_binary_content(hashFile, &hashBuf) == BMD_OK);
		
		mtds->__ptr[1].mtdOid = OID_PKI_METADATA_PADES_HASH_HEX_VALUE;
		mtds->__ptr[1].mtdValue = strndup(hashBuf->buf, hashBuf->size);
		mtds->__ptr[1].mtdDesc = "";
		
		mtds->__ptr[2].mtdOid = OID_PKI_METADATA_PADES_HASH_ALGORITHM;
		mtds->__ptr[2].mtdValue = OID_HASH_FUNCTION_SHA1;
		mtds->__ptr[2].mtdDesc = "";
	}

	GenBuf_t *cert_pem=NULL;
	base64Cert = (struct xsd__base64Binary *)malloc(sizeof(struct xsd__base64Binary));
	base64Signature = (struct bmd230__myBinaryDataType *)malloc(sizeof(struct bmd230__myBinaryDataType));
	input=(struct bmd230__fileInfo *)malloc(sizeof(struct bmd230__fileInfo));

	bmd_load_binary_content(keyfile,&cert_pem);
	base64Cert->__ptr=(unsigned char*)cert_pem->buf;
	base64Cert->__size=cert_pem->size;

	asprintf(&(input->filename),"%s",strrchr(file,'/') ? strrchr(file,'/')+1 : file );
	input->file=(struct bmd230__myBinaryDataType *)malloc(sizeof(struct bmd230__myBinaryDataType));
	input->file->id=NULL;
	base64Signature->id = NULL;

	soap_set_namespaces(&soap, bmd230_namespaces);

   switch(stream)
   {

     case 0 : {
                bmd_load_binary_content(sig,&podpis);
                base64Signature->__ptr = (unsigned char*)podpis->buf;
                base64Signature->__size = podpis->size;
                base64Signature->type = NULL;
                base64Signature->options = NULL;

                /*przygotowanie pliku do wysyłki*/

                status = bmd_load_binary_content(file,&tmp_file);
                if (status != 0)
                 printf("coś nie tak z plikiem :)\n");

                input->file->__ptr=(unsigned char*)tmp_file->buf;
                input->file->__size=tmp_file->size;
                input->file->type=NULL;
                input->file->options=NULL;

                /*********************************************************************************/
                /************************ funkcja testowa ****************************************/
                 status = soap_call_bmd230__bmdInsertSignedFile(&soap, host, NULL, base64Cert, NULL, NULL, NULL, mtds, input, base64Signature, signatureType , transactionId, nostream, &result);
                /*********************************************************************************/
                /*********************************************************************************/

                break;
              }

     case 1 : {
                soap_set_omode(&soap, SOAP_IO_CHUNK);
                fd1 = fopen(sig,"r");
                base64Signature->__ptr = (unsigned char*)fd1;
                base64Signature->__size = 0;
                base64Signature->type = "";
                base64Signature->options = soap_dime_option(&soap,0, "podpis.sig" );

                fd2 = fopen(file,"r");
                input->file->__ptr=(unsigned char*)fd2;
                input->file->__size=0;
                input->file->type="";
                input->file->options = soap_dime_option(&soap,0, strrchr(file,'/') ? strrchr(file,'/')+1 : file);

                /*********************************************************************************/
                /************************ funkcja testowa ****************************************/
                 status = soap_call_bmd230__bmdInsertSignedFile(&soap, host, NULL, base64Cert, NULL, NULL, NULL, mtds, input, base64Signature, signatureType, transactionId, stream, &result);
                /*********************************************************************************/
                /*********************************************************************************/

                break;
              }
             }

	free(tmp_file); tmp_file=NULL;

	if (status == SOAP_OK)
	{
		/*********************************************************************************/
		/************************ funkcja odpowiedź ****************************************/
		printf("Plik zapisany w archiwum; id %li\n",result);

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
