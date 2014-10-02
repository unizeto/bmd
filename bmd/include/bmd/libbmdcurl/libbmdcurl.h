#ifndef _HAVE_LIBBMDCURL_LIBRARY
#define _HAVE_LIBBMDCURL_LIBRARY

/************************** by PKL ******************************/
/*	Biblioteka zawierająca funkcje umożlwiajace pobranie 	*/
/*	pliku z serwera w trybie HTTPS wykorzystywana do	*/
/*	aktualizacji oprogramowania aplikacji klienta BMD	*/
/****************************************************************/


#ifdef WIN32
	#ifdef LIBBMDCURL_EXPORTS
		#define LIBBMDCURL_SCOPE __declspec(dllexport)
	#else
		#ifdef _USRDLL
			#define LIBBMDCURL_SCOPE __declspec(dllimport)
		#else
			#define LIBBMDCURL_SCOPE
		#endif
	#endif
#else
	#define LIBBMDCURL_SCOPE
#endif // ifdef WIN32

#define CURL_FOK(x)\
{\
	long bmd_fok_status = 0;\
	bmd_fok_status=x;\
	if(bmd_fok_status!=CURLE_OK)\
	{\
		PRINT_ERROR("%s. Error: %li\n",GetErrorMsg(bmd_fok_status),bmd_fok_status);\
		return bmd_fok_status;\
	}\
}


#include <stdio.h>
#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>
#include <bmd/libbmd/libbmd.h>

#define CURL_ERR_PARAM1		-1
#define CURL_ERR_PARAM2		-2
#define CURL_ERR_PARAM3		-3
#define CURL_ERR_PARAM4		-4
#define CURL_ERR_PARAM5		-5
#define CURL_ERR_PARAM6		-6
#define CURL_ERR_PARAM7		-7
#define CURL_ERR_PARAM8		-8
#define CURL_ERR_PARAM9		-9

#define CURL_ERR_FILE_OPEN	-20
#define CURL_ERR_FILE_WRITE	-21
#define CURL_ERR_FILE_READ	-22

#define CURL_ERR_INIT_FAILED	-30
#define CURL_ERR_SET_OPT	-31
#define CURL_ERR_SET_HOST	-32
#define CURL_ERR_SET_CERT	-33
#define CURL_ERR_SET_VER	-34
#define CURL_ERR_SET_OUTPUT	-35

#define CURL_ERR_OPERATE	-40
#define CURL_ERR_CLEAN		-41
#define CURL_ERR_CERT_URL	-51															// certyfikat niezgodny z adresem (najczesciej jesli próba pobrania z adresu zapisanego inaczej niz w certyfikacie)

#if defined(__cplusplus)
extern "C" {
#endif

/* ****************************************** */
/*    Struktura przechowywująca zawartość     */
/*              pobranego pliku               */
/*             --|Klemensio|--                */ 
/* ****************************************** */

struct MemoryStruct {
  char *memory;
  size_t size;
};

LIBBMDCURL_SCOPE long SimpleCurlPost(	GenBuf_t *input,
					char *host);

LIBBMDCURL_SCOPE long SimpleCurlPostAndGetAnswer( char *host, GenBuf_t *input, char **answer );

LIBBMDCURL_SCOPE long SimpleCurlGet(char* host, GenBuf_t** input);

LIBBMDCURL_SCOPE long HttpsDownloadFile(	char *address,
						char *cainfo,
						bmd_crypt_ctx_t *log_ctx,
						long (*callback)(long, void *),
						char *file,
						void * parent);



/****************************************************************************************/
/*											*/
/*			FUNKCJE WEWNĘTRZNE BIBLIOTEKI					*/
/*											*/
/****************************************************************************************/
CURL *BMD_curl_initialize();
long BMD_set_curl_host(CURL *curl, char *host, long port);
long BMD_set_curl_certificate_options(	CURL *curl,		/*wskaźnik do curl'a*/
					char *cert_type,	/*roszerzeneie certyfikatu*/
					char *cert,		/*plik certyfikatu*/
					char *cert_pass,	/*haslo certyfikatu*/
					char *key_type,		/*roszerzeneie klucza*/
					char *key,		/*plik klucza*/
					char *key_pass);	/*haslo klucza*/
long BMD_set_curl_output_options(	CURL * curl,		/*wskaźnik do curl'a*/
					FILE *fd);		/*deskryptor pliku*/
long BMD_set_curl_host_verification_options(	CURL *curl,	/*wskaźnik do curl'a*/
						long verify,
						char *capath,	/*ścieżka do katalogu z certyfikatami do weryfikacji*/
						char *cainfo);	/*plik z certyfikatami do weryfikacji*/
long BMD_curl_operate(	CURL *curl);	/*wskaźnik do curl'a*/
long BMD_curl_clean(	CURL *curl);	/*wskaźnik do curl'a*/
char **BMD_curl_get_engines(CURL *curl);
size_t getting_data(void *ptr, size_t size, size_t nmemb, void *stream);
CURLcode sslctxfun(CURL *curl, void *sslctx, void *parm);
long GetCapath(char *cainfo, char **capath);

/* ********************************** */
/*  Funkcje pomocnicze nie należące   */
/*               do API               */
/* ********************************** */

void *myMemoryRealloc(void*, size_t);
size_t WriteMemoryCallback(void*, size_t, size_t, void*);

#if defined(__cplusplus)
}
#endif


#endif

