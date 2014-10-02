#include <bmd/libbmdcurl/libbmdcurl.h>
#include <bmd/libbmd/libbmd.h>
#include <bmd/libbmdpki/libbmdpki.h>


extern long _GLOBAL_debug_level;

int funkcja(long int got, void * p)
{
	return 0;
}

int main(int argc, char **argv)
{

int status=0;
FILE *fd=NULL;
bmd_info_t *bi=NULL;

	//_GLOBAL_debug_level=20;
/********************************************************************************************/
/********************************************************************************************/
/********************************************************************************************/
/********************************************************************************************/

	bmd_init();
	bmd_info_create(&bi);
	bmd_info_login_set("127.0.0.1",6666,NULL, -1, NULL, NULL, BMD_PROXY_TYPE_NONE, NULL,&bi);

	/************************************************/
	/*	pobranie danych crypto z pliku		*/
	/************************************************/
	status=bmd_info_set_credentials_pfx("klient.p12","12345678",8,&bi);
	/************************************************/
	/*	pobranie danych crypto z karty		*/
	/************************************************/
	/*status=bmd_set_ctx_pkcs11(&(bi->crypto_info.log_ctx),"aetpkss1.dll",BMD_CTX_TYPE_ASYM,BMD_PKCS11_AUTO_LOAD_CERT);*/
	if (status!=0)
	{
		printf("\n\n\nERROR\n\n");
		return -1;
	}

	status=HttpsDownloadFile(	"https://test.lklimek.unizeto.pl/sslreq/refman.pdf",
					"CA/ca.crt",
					bi->crypto_info.log_ctx,
					&funkcja,
					"pliczek.pdf",
					NULL);	// brak parenta nadrzednego
	if (status<BMD_OK)
	{
		printf("LIBBMDCURLERR Error\n");
		return 0;
	}

	if (fd!=NULL) fclose(fd);
 	return 0;
}
