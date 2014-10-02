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
int i=0;
char *cacert=NULL;
char *capath=NULL;
int status = 0;
struct xsd__base64Binary *base64Cert = NULL;
char *hash=NULL;
long int id=0;
//GenBuf_t plik;
GenBuf_t *tmp;
char* folder			= NULL;
char* tempFolder		= NULL;
char* tempNestedFolder	= NULL;


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
			if (strcmp(argv[i],"-folder")==0)
			{
				if (argc>i+1)
				{
					asprintf(&folder,"%s",argv[i+1]);
				}
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
		printf("\t-folder\t\tnazwa folder, w ktorym umieszczona zostanie wartosc dowodowa\n");
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
	struct bmd230__PkiFileInfo *fileResponse = NULL;
	base64Cert = (struct xsd__base64Binary *)malloc(sizeof(struct xsd__base64Binary));
	status = bmd_load_binary_content(keyfile,&cert_pem);
	if (status != BMD_OK)
	{
		PRINT_DEBUG("SOAPCLIENTERR Error while reading certificate file\n");
		return 0;
	}
	base64Cert->__ptr=(unsigned char*)cert_pem->buf;
	base64Cert->__size=cert_pem->size;

	soap.send_timeout = 300; // 60 seconds
	soap.recv_timeout = 300; // 60 seconds

	soap_set_namespaces(&soap, bmd230_namespaces);

	/*********************************************************************************/
	/************************ funkcja testowa ****************************************/
	status=soap_call_bmd230__bmdGetFilePKIById(&soap, host, NULL, base64Cert, NULL, NULL, NULL, id, &fileResponse);
	/*********************************************************************************/
	/*********************************************************************************/
	int j=0;
	int k=0;
	/******************************/
	/*tmp=(GenBuf_t *)malloc(sizeof(GenBuf_t)+2);
	tmp->buf=(unsigned char *)malloc(  +2);
	memset(tmp->buf, 0, +1);
	memcpy(tmp->buf,  , );
	tmp->size=
	bmd_save_buf(tmp,"plik.pdf");
	free_gen_buf(&tmp);*/
	/*******************************/
	char *tmp_s=NULL;
	if (status == SOAP_OK)
	{
		mkdir(folder, 0777);
		/*********************************************************************************/
		/************************ funkcja odpowiedź ****************************************/
		printf("%s\n",fileResponse->filename);
		printf("timestamps %i\n",fileResponse->timestamp->__size);
		asprintf(&tempFolder, "%s/Timestamp", folder);
		mkdir(tempFolder, 0777);
		for (i=0; i<fileResponse->timestamp->__size; i++)
		{
			printf("\t\t%i\t%i\t\n",i+1,fileResponse->timestamp->__ptr[i].value->__size);

			tmp=(GenBuf_t *)malloc(sizeof(GenBuf_t)+2);
			tmp->buf=(char *)malloc(fileResponse->timestamp->__ptr[i].value->__size+2);
			memset(tmp->buf, 0, fileResponse->timestamp->__ptr[i].value->__size+1);
			memcpy(tmp->buf,  fileResponse->timestamp->__ptr[i].value->__ptr, fileResponse->timestamp->__ptr[i].value->__size);
			tmp->size=fileResponse->timestamp->__ptr[i].value->__size;
			asprintf(&tmp_s,"%s/timestamp_%i.tsr", tempFolder, i);
 			bmd_save_buf(tmp,tmp_s);
			free(tmp_s); tmp_s=NULL;
			free_gen_buf(&tmp);

		}
		free(tempFolder); tempFolder=NULL;
		
		printf("signatures %i\n",fileResponse->signature->__size);
		asprintf(&tempFolder, "%s/Signature", folder);
		mkdir(tempFolder, 0777);
		for (i=0; i<fileResponse->signature->__size; i++)
		{
			// w przypadku PAdES podpisu nie bedzie w odpowiedzi (bo jest w pliku PDF)
			if(fileResponse->signature->__ptr[i].value != NULL)
			{
				printf("\t\t%i\t%i\t\n",i+1,fileResponse->signature->__ptr[i].value->__size);
				tmp=(GenBuf_t *)malloc(sizeof(GenBuf_t)+2);
				tmp->buf=(char *)malloc(fileResponse->signature->__ptr[i].value->__size+2);
				memset(tmp->buf, 0, fileResponse->signature->__ptr[i].value->__size+1);
				memcpy(tmp->buf,  fileResponse->signature->__ptr[i].value->__ptr, fileResponse->signature->__ptr[i].value->__size);
				tmp->size=fileResponse->signature->__ptr[i].value->__size;
				asprintf(&tmp_s,"%s/signature_%i.sig", tempFolder, i);
				bmd_save_buf(tmp,tmp_s);
				free(tmp_s); tmp_s=NULL;
				free_gen_buf(&tmp);
			}
			else
				{ printf("signature not saved (because PDF contains signature)\n"); }

			printf("\t\ttimestamps %i\n",fileResponse->signature->__ptr[i].timestamp->__size);
			asprintf(&tempNestedFolder, "%s/Signature/Timestamp", folder);
			mkdir(tempNestedFolder, 0777);
			for (j=0; j<fileResponse->signature->__ptr[i].timestamp->__size; j++)
			{
				printf("\t\t\t%i\t%i\t\n",j+1,fileResponse->signature->__ptr[i].timestamp->__ptr[j].value->__size);

				tmp=(GenBuf_t *)malloc(sizeof(GenBuf_t)+2);
				tmp->buf=(char *)malloc(fileResponse->signature->__ptr[i].timestamp->__ptr[j].value->__size+2);
				memset(tmp->buf, 0, fileResponse->signature->__ptr[i].timestamp->__ptr[j].value->__size+1);
				memcpy(tmp->buf,  fileResponse->signature->__ptr[i].timestamp->__ptr[j].value->__ptr, fileResponse->signature->__ptr[i].timestamp->__ptr[j].value->__size);
				tmp->size=fileResponse->signature->__ptr[i].timestamp->__ptr[j].value->__size;
				asprintf(&tmp_s,"%s/timestamp_sig_%i.sig.tsr", tempNestedFolder, j);
 				bmd_save_buf(tmp,tmp_s);
				free(tmp_s); tmp_s=NULL;
				free_gen_buf(&tmp);
			}
			free(tempNestedFolder); tempNestedFolder=NULL;

			printf("\t\tdvcs %i\n",fileResponse->signature->__ptr[i].dvcs->__size);
			for (j=0; j<fileResponse->signature->__ptr[i].dvcs->__size; j++)
			{
				asprintf(&tempNestedFolder, "%s/Signature/Dvcs%i", folder, j);
				mkdir(tempNestedFolder, 0777);
				printf("\t\t\t%i\t%i\t\n",j+1,fileResponse->signature->__ptr[i].dvcs->__ptr[j].value->__size);

				tmp=(GenBuf_t *)malloc(sizeof(GenBuf_t)+2);
				tmp->buf=(char *)malloc(fileResponse->signature->__ptr[i].dvcs->__ptr[j].value->__size+2);
				memset(tmp->buf, 0, fileResponse->signature->__ptr[i].dvcs->__ptr[j].value->__size+1);
				memcpy(tmp->buf,  fileResponse->signature->__ptr[i].dvcs->__ptr[j].value->__ptr, fileResponse->signature->__ptr[i].dvcs->__ptr[j].value->__size);
				tmp->size=fileResponse->signature->__ptr[i].dvcs->__ptr[j].value->__size;
				asprintf(&tmp_s,"%s/dvcs_sig.dvcs", tempNestedFolder);
 				bmd_save_buf(tmp,tmp_s);
				free(tmp_s); tmp_s=NULL;
				free_gen_buf(&tmp);
				

				printf("\t\t\ttimestamp %i\n",fileResponse->signature->__ptr[i].dvcs->__ptr[j].timestamp->__size);
				asprintf(&tempNestedFolder, "%s/Timestamp", tempNestedFolder);
				mkdir(tempNestedFolder, 0777);
				for (k=0; k<fileResponse->signature->__ptr[i].dvcs->__ptr[j].timestamp->__size; k++)
				{
					printf("\t\t\t\t%i\t%i\t\n",j+1,fileResponse->signature->__ptr[i].dvcs->__ptr[j].timestamp->__ptr[k].value->__size);

					tmp=(GenBuf_t *)malloc(sizeof(GenBuf_t)+2);
					tmp->buf=(char *)malloc(fileResponse->signature->__ptr[i].dvcs->__ptr[j].timestamp->__ptr[k].value->__size+2);
					memset(tmp->buf, 0, fileResponse->signature->__ptr[i].dvcs->__ptr[j].timestamp->__ptr[k].value->__size+1);
					memcpy(tmp->buf,  fileResponse->signature->__ptr[i].dvcs->__ptr[j].timestamp->__ptr[k].value->__ptr, fileResponse->signature->__ptr[i].dvcs->__ptr[j].timestamp->__ptr[k].value->__size);
					tmp->size=fileResponse->signature->__ptr[i].dvcs->__ptr[j].timestamp->__ptr[k].value->__size;
					asprintf(&tmp_s,"%s/timestamp_dvcs_sig_%i.sig.tsr", tempNestedFolder, k);
 					bmd_save_buf(tmp,tmp_s);
					free(tmp_s); tmp_s=NULL;
					free_gen_buf(&tmp);
				}
				free(tempNestedFolder); tempNestedFolder=NULL;
			}

		}
		free(tempFolder); tempFolder=NULL;
		
		/*printf("Znaleziony plik: %s\nWielkość pliku: %i\n", fileResponse->filename,fileResponse->file->__size);
		plik.buf=(char*)malloc(fileResponse->file->__size+2);
		memset(plik.buf,0,fileResponse->file->__size+1);
		memcpy(plik.buf,fileResponse->file->__ptr,fileResponse->file->__size);
		plik.size=fileResponse->file->__size;
		asprintf(&tempFolder, "%s/%s", folder, fileResponse->filename);
		bmd_save_buf(&plik,tempFolder);
		free(tempFolder); tempFolder=NULL;
		*/
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
