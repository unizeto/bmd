/************************** by PKL ******************************/
/*	Biblioteka zawierajaca funkcje umozlwiajace pobranie 	*/
/*	pliku z serwera w trybie HTTPS wykorzystywana do	*/
/*	aktualizacji oprogramowania aplikacji klienta BMD.	*/
/*	Wykorzystane zostaly mechanizmu z biblioteki libcurl.	*/
/****************************************************************/
#include <stdio.h>
#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>
#include <bmd/libbmd/libbmd.h>
#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdcurl/libbmdcurl.h>
#include <bmd/libbmdutils/libbmdutils.h>
#include <bmd/libbmdpkcs12/libbmdpkcs12.h>
#include <bmd/libbmdpki/libbmdpki.h>
#include <openssl/ssl.h>
#include <bmd/libbmdglobals/libbmdglobals.h>

long (*func)(long got, void * parent);

#ifdef WIN32
#pragma warning(disable:4100)
#endif

/**
 * Proste wysylanie danych zawartych w buuforze generycznym na okreslony adres za posrednictwem mechanizmow CURL
 *
 * @param[in]	input		wejsciowy bufor generyczny zawierajacy dane do wyslania
 * @param[in]	host		adres docelowy, na jaki maja zostac wyslane dane
 *
 * @retval BMD_OK		poprawne wywolanie funkcji
 * @retval CURL_ERR_PARAM1	niepoprawny 1. parametr wywolania funkcji
 * @retval CURL_ERR_PARAM2	niepoprawny 2. parametr wywolania funkcji
 * @retval inne			bledy zwracane przez funkcje wywolane wewnatrz opisywanej funkcji
 */
long SimpleCurlPost(	GenBuf_t *input,
			char *host)
{
CURL *curl;
CURLcode res;
struct curl_slist *headers	= NULL;


	/************************/
	/* walidacja parametrow */
	/************************/
	if (input==NULL)	{	BMD_FOK(CURL_ERR_PARAM1);	}
	if (input->buf==NULL)	{	BMD_FOK(CURL_ERR_PARAM1);	}
	if (input->size<=0)	{	BMD_FOK(CURL_ERR_PARAM1);	}
	if (host==NULL)		{	BMD_FOK(CURL_ERR_PARAM2);	}

	curl = curl_easy_init();
	if (curl==NULL)		{	BMD_FOK(CURL_ERR_INIT_FAILED);	}

	headers = curl_slist_append(headers, "Content-Type: text/xml; charset=utf-8");
	if (headers==NULL)	{	BMD_FOK(CURL_ERR_INIT_FAILED);	}

	CURL_FOK(curl_easy_setopt(curl, CURLOPT_URL, host));
	CURL_FOK(curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers));
	CURL_FOK(curl_easy_setopt(curl, CURLOPT_POSTFIELDS, input->buf));
	CURL_FOK(curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10));
	CURL_FOK(curl_easy_setopt(curl, CURLOPT_VERBOSE,0));
	CURL_FOK(curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, input->size));


	res = curl_easy_perform(curl);

	curl_easy_cleanup(curl);

	return BMD_OK;
}


/**
 * Proste wysylanie danych zawartych w buuforze generycznym na okreslony adres za 
 * posrednictwem mechanizmow CURL i oczekiwanie na odpowiedź
 *
 * @param[in]	input		wejsciowy bufor generyczny zawierajacy dane do wyslania
 * @param[in]	host		adres docelowy, na jaki maja zostac wyslane dane
 *
 * @retval BMD_OK		poprawne wywolanie funkcji
 * @retval CURL_ERR_PARAM1	niepoprawny 1. parametr wywolania funkcji
 * @retval CURL_ERR_PARAM2	niepoprawny 2. parametr wywolania funkcji
 * @retval inne			bledy zwracane przez funkcje wywolane wewnatrz opisywanej funkcji
 */
long SimpleCurlPostAndGetAnswer( char *host, GenBuf_t *input, char **answer )
{
CURL *curl;
CURLcode res;
struct curl_slist *headers	= NULL;
struct MemoryStruct		 chunk;


  /************************/
  /* walidacja parametrow */
  /************************/

  if (input==NULL)	{	BMD_FOK(CURL_ERR_PARAM1);	}
  if (input->buf==NULL)	{	BMD_FOK(CURL_ERR_PARAM1);	}
  if (input->size<=0)	{	BMD_FOK(CURL_ERR_PARAM1);	}
  if (host==NULL)	{	BMD_FOK(CURL_ERR_PARAM2);	}
  if (answer==NULL || *answer!=NULL) { 	BMD_FOK(CURL_ERR_PARAM3);  }


  chunk.memory = NULL;
  chunk.size = 0; 

  curl = curl_easy_init();
  if (curl==NULL)		{	BMD_FOK(CURL_ERR_INIT_FAILED);	}

  headers = curl_slist_append(headers, "Content-Type: text/xml; charset=utf-8");
  if (headers==NULL)	{	BMD_FOK(CURL_ERR_INIT_FAILED);	}

  CURL_FOK(curl_easy_setopt(curl, CURLOPT_URL, host));
  CURL_FOK(curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers));
  CURL_FOK(curl_easy_setopt(curl, CURLOPT_POSTFIELDS, input->buf));;
  CURL_FOK(curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10));
  CURL_FOK(curl_easy_setopt(curl, CURLOPT_VERBOSE,0));
  CURL_FOK(curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, input->size));

  /* send all data to this function  */
  CURL_FOK(curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback));
 
  /* we pass our 'chunk' struct to the callback function */ 
  CURL_FOK(curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk));
 
  /* some servers don't like requests that are made without a user-agent
     field, so we provide one */

  CURL_FOK(curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0"));
  
  res = curl_easy_perform(curl);
	
  asprintf(answer, "%s", chunk.memory);

  free0(chunk.memory);

  curl_easy_cleanup(curl);

  return BMD_OK;
}










































long SimpleCurlGet(char* host, GenBuf_t** output) {

 /* --------------------------------- */

    CURL*                curl_handle;
    struct MemoryStruct        chunk;
    long        longRet          = 0;

 /* --------------------------------- */


 /************************/
 /* walidacja parametrow */
 /************************/

 if (host==NULL)	{	BMD_FOK(CURL_ERR_PARAM1);	}
 if (output==NULL || *output!=NULL)	{	BMD_FOK(CURL_ERR_PARAM2);	}

 chunk.memory = NULL;
 chunk.size = 0; 

 curl_global_init(CURL_GLOBAL_ALL);

 /* init the curl session */ 
 curl_handle = curl_easy_init();
 
 /* specify URL to get */ 
 curl_easy_setopt(curl_handle, CURLOPT_URL, host);
 
 /* send all data to this function  */ 
 curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
 
 /* we pass our 'chunk' struct to the callback function */ 
 curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
 
 /* some servers don't like requests that are made without a user-agent
 field, so we provide one */ 
 curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
 
 /* get it! */ 
 curl_easy_perform(curl_handle);
 
 /* cleanup curl stuff */ 
 curl_easy_cleanup(curl_handle);
 
 longRet = set_gen_buf2(chunk.memory, (long)chunk.size, output);

 if (longRet!=BMD_OK)	{	BMD_FOK(BMD_ERR_OP_FAILED);	}

 if(chunk.memory) { 
	free(chunk.memory);
 }

 curl_global_cleanup();

 return BMD_OK;
}


/**
*Pobranie pliku z podanej lokalizacji po SSL
*
*@param[in] *address wskaznik bufora zawierajacego adres hosta (np. https://192.168.43.23)
*@param[in] *cainfo sciezka do pliku certyfikatu CA
*@param[in] *log_ctx contekst uzyskany z pliku lub karty
*@param[in] (*f)(long) callback w czasie pobierania danych z serwera
*@param[in] *file adres funkcji callback uruchamianej podczas odbierania kazdej z paczek (prototyp: funkcja(long got, void *parent);   got-ilość oebranych danych )
*@param[in] *parent wskaznik na obiekt klasy, w ktorej zawarta jest metoda odświezania (np. BMDUpdater)
*
*@retval BMD_OK - zakonczenie pomyślne.
*@retval CURL_ERR_PARAM1 niepoprawny adres hosta
*@retval CURL_ERR_PARAM2 niepoprawny certyfikat
*@retval CURL_ERR_PARAM3 niepoprawny klucz
*@retval CURL_ERR_FILE_OPEN blad otwarcia pliku
*@retval CURL_ERR_FILE_WRITE blad zapisu pliku
*@retval CURL_ERR_FILE_READ blad odczytu pliku
*@retval CURL_ERR_INIT_FAILED blad inicjalizacji curl'a
*@retval CURL_ERR_SET_OPT blad ustawienia opcji struktury curl
*@retval CURL_ERR_SET_HOST blad ustawienia hosta
*@retval CURL_ERR_SET_CERT blad ustawienia klucza lub certyfikatu klienta
*@retval CURL_ERR_SET_VER blad ustawienia opcji weryfikacji serwera
*@retval CURL_ERR_SET_OUTPUT blad ustawienia pliku wyjściowego
*@retval CURL_ERR_OPERATE blad dzialania sesji
*@retval CURL_ERR_CLEAN blad zwalniania zasobow
*/
long HttpsDownloadFile(	char *address,
			char *cainfo,
			bmd_crypt_ctx_t *log_ctx,
			long (*callback)(long , void * ),
			char *file,
			void * parent)
{
CURL *curl=NULL;
long status=0;
FILE *fd=NULL;
SSL_CTX *ctx_data=NULL;
char *capath=NULL;
long source=0;

	PRINT_INFO("LIBBMDCURLINF Updating BMD client aplication\n");
	_GLOBAL_ctx=log_ctx;
	func=callback;
	_GLOBAL_CallBackParent = parent;

	/********************************/
	/*	walidacja parametrow	*/
	/********************************/
	if(address==NULL)
	{
		PRINT_ERROR("LIBBMDCURLERR Invalid first parameter error. Error=%i\n",CURL_ERR_PARAM1);
		return CURL_ERR_PARAM1;
	}

	if(cainfo==NULL)
	{
		PRINT_ERROR("LIBBMDCURLERR Invalid second parameter error. Error=%i\n",CURL_ERR_PARAM2);
		return CURL_ERR_PARAM2;
	}
	if (log_ctx==NULL)
	{
		PRINT_ERROR("LIBBMDCURLERR Invalid third parameter error. Error=%i\n",CURL_ERR_PARAM3);
		return CURL_ERR_PARAM3;
	}
	if ((log_ctx->pkcs11!=NULL) && (log_ctx->pkcs11->all_certs[0]!=NULL))
	{
		source++;
	}
	if ((log_ctx->file!=NULL) && (log_ctx->file->cert!=NULL) && (log_ctx->file->cert->buf!=NULL) && (log_ctx->file->cert->size!=0))
	{
		source++;
	}
	if (source==0)
	{
		PRINT_ERROR("LIBBMDCURLERR Invalid third parameter error. Error=%i\n",CURL_ERR_PARAM3);
		return CURL_ERR_PARAM3;
	}
	if (file==NULL)
	{
		PRINT_ERROR("LIBBMDCURLERR Invalid fifth parameter error. Error=%i\n",CURL_ERR_PARAM5);
		return CURL_ERR_PARAM5;
	}

	/************************************************/
	/*	wyciagniecie katalogu ze ściezki	*/
	/************************************************/
	GetCapath(cainfo, &capath);

	/****************************************/
	/*	inicjalizacja curl'a		*/
	/****************************************/
	curl=BMD_curl_initialize();
	if (curl==NULL)
	{
		PRINT_ERROR("LIBBMDCURLERR Error in initializing url library. Error=%i\n",CURL_ERR_INIT_FAILED);
		return CURL_ERR_INIT_FAILED;
	}

	/****************************************/
	/*	ustawienie trybu debuga curla	*/
	/****************************************/
	status=curl_easy_setopt(curl,CURLOPT_VERBOSE,1);
	if(status!=CURLE_OK)
	{
		PRINT_ERROR("LIBBMDCURLERR Error in setting curl option. Error=%i\n",CURL_ERR_SET_OPT);
		return CURL_ERR_SET_OPT;
	}

	/****************************************/
	/*	ustawienie hosta docelowego	*/
	/****************************************/
	status=BMD_set_curl_host(curl,address,443);
	if(status!=BMD_OK)
	{
		PRINT_ERROR("LIBBMDCURLERR Error in setting curl's host option. Error=%i\n",CURL_ERR_SET_HOST);
		return CURL_ERR_SET_HOST;
	}
	/****************************************/
	/*	ustawienie opcji weryfikacji	*/
	/****************************************/
	status=BMD_set_curl_host_verification_options(curl,1,capath,cainfo);
	if(status!=BMD_OK)
	{
		PRINT_ERROR("LIBBMDCURLERR Error in setting curl's verification option. Error=%i\n",CURL_ERR_SET_VER);
		return CURL_ERR_SET_VER;
	}

	/****************************************/
	/*	pobranie zmiennych dot. enginow	*/
	/****************************************/
	/*engines=BMD_curl_get_engines(curl);

	for (i=0;engines[i];i++)
	{
		printf("%s\n",engines[i]);
	}*/

	/****************************************/
	/* 	otwarcie pliku do zapisu wyniku	*/
	/****************************************/
	fd=fopen(file,"wb+");
	if (fd==NULL)
	{
		PRINT_ERROR("LIBBMDCURLERR Error in opennig file %s. Error=%i\n",file,CURL_ERR_FILE_OPEN);
		return CURL_ERR_FILE_OPEN;
	}

	/****************************************/
	/*	ustawienie opcji wynikowych	*/
	/****************************************/
	status=BMD_set_curl_output_options(curl,fd);
	if (status!=BMD_OK)
	{
		PRINT_ERROR("LIBBMDCURLERR Error in setting curl's output option. Error=%i  %li\n",CURL_ERR_SET_OUTPUT,status);
		fclose(fd);
		return CURL_ERR_SET_OUTPUT;
	}

	/****************************************/
	/*	callback do ctx			*/
	/****************************************/
	status=curl_easy_setopt(curl,CURLOPT_SSL_CTX_DATA,ctx_data);
	if(status!=BMD_OK)
	{
		PRINT_ERROR("LIBBMDCURLERR Error in setting curl option. Error=%i\n",CURL_ERR_SET_OPT);
		fclose(fd);
		return CURL_ERR_SET_OPT;
	}

	status=curl_easy_setopt(curl,CURLOPT_SSL_CTX_FUNCTION,sslctxfun);
	if(status!=BMD_OK)
	{
		PRINT_ERROR("LIBBMDCURLERR Error in setting curl option. Error=%i\n",CURL_ERR_SET_OPT);
		fclose(fd);
		return CURL_ERR_SET_OPT;
	}

	/****************************************/
	/*	start sesji			*/
	/****************************************/
	status=BMD_curl_operate(curl);
	if (status!=BMD_OK)
	{
		if (status==CURL_ERR_CERT_URL)
		{
			PRINT_ERROR("LIBBMDCURLERR Error in setting curl's operation. Error=%i\n",CURL_ERR_CERT_URL);
			fclose(fd);
			return CURL_ERR_CERT_URL;													// certyfikat niezgodny z adresem (najczesciej jesli proba pobrania z adresu zapisanego inaczej niz w certyfikacie)
		}
		//PRINT_ERROR("LIBBMDCURLERR Error in setting curl's operation. Error=%i\n",CURL_ERR_OPERATE);
		fclose(fd);
		return status;//RO
	}

	/****************************************/
	/*	porzadki			*/
	/****************************************/
	status=BMD_curl_clean(curl);
	if (status<BMD_OK)
	{
		PRINT_ERROR("LIBBMDCURLERR Error in cleaning up after curl. Error=%i\n",CURL_ERR_CLEAN);
		fclose(fd);
		return CURL_ERR_CLEAN;
	}

	fclose(fd);
	return BMD_OK;
}


/****************************************************************************************/
/*											*/
/*			FUNKCJE WEWNeTRZNE BIBLIOTEKI					*/
/*											*/
/****************************************************************************************/
/**
*Wyluskanie katalogu z podanej ściezki
*
*@param[in] *cainfo wskaznik bufora zawierajacego ściezke wraz z plikiem
*@param[out] *capath ściezka do pliku (tylko katalog)
*@retval BMD_OK - zakonczenie pomyślne.
*/
long GetCapath(	char *cainfo,
		char **capath)
{
long i=0;
long pos=10000;

	PRINT_INFO("LIBBMDCURL Getting directory of giver filename\n");
	for (i=0; i<(int)strlen(cainfo); i++)
	{
		if ((cainfo[i]=='/') || (cainfo[i]=='\\'))
		{
			pos=i;
		}
	}
	if (pos==10000)
	{
		pos=0;
	}

	*capath=(char *)malloc(pos+2);
	memset(*capath,0,pos+1);
	if (pos!=0)
	{
		for (i=0; i<pos+1; i++)
		{
			(*capath)[i]=cainfo[i];
		}
		(*capath)[pos+1]='\0';
	}
	else
	{
		(*capath)[pos]='\0';
	}
	return 0;
}


/**
*Inicjalizacja dzialania biblioteki libCurl
*
*@retval *curl wskaznik do zainicjowanej struktury CURL
*@retval NULL w przypadku blednego wykonania funkcji
*/
CURL *BMD_curl_initialize()
{
CURL *curl;

	PRINT_INFO("LIBBMDCURLINF Initializing curl library\n");
//	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl=curl_easy_init();
	if (curl==NULL)
	{
		PRINT_ERROR("LIBBMDCURLERR Error in curl initialization\n");
	}
	return curl;
}


/**
*Przechwycenie akcji odebrania danych do zapisu
*
*@param[in] *ptr wskaznik bufora zawierajacego odczytane dane
*@param[in] size wielkość odczytywanych elementow
*@param[in] nmemb ilość odczytywanych elementow
*@retval size_t ilość odebranych zapisanych danych
*/
size_t getting_data(	void *ptr,
			size_t size,
			size_t nmemb,
			void *stream)
{
size_t read=0;
long status = 0;
	long tmp_size;
	read=fwrite(ptr,size,nmemb,stream);
	tmp_size=(long)(size*nmemb);
	status = func(tmp_size, _GLOBAL_CallBackParent);											// callback postepu
	if (status < 0)
	{	return (size_t)-1; /* @todo - to bez sensu */
	}
//	Sleep(100);
	return read;
}


/**
*Funkcja callback wywolywana podczas pobierania danych z serwera
*
*@param[in] *curl wskaznik do struktury curl
*@param[in] *sslctx wskaznik do struktury kontekstu SSL'a
*@param[in] *parm
*@retval BMD_OK - zakonczenie pomyślne.
*/
CURLcode sslctxfun(	CURL *curl,
			void *sslctx,
			void *parm)
{
long status=0;

	PRINT_INFO("LIBBMDCURLINF ssl function\n");
	switch(_GLOBAL_ctx->ctx_source)
	{
		case BMD_CTX_SOURCE_FILE:
			PRINT_INFO("LIBBMDCURLINF Using crypto file\n");
			/********************************************************/
			/*	podpieie certyfikatu do kontekstu ssl		*/
			/********************************************************/
			status=SSL_CTX_use_certificate_ASN1(sslctx, _GLOBAL_ctx->file->cert->size, \
			(unsigned char*)(_GLOBAL_ctx->file->cert->buf));
			if (status!=1)
			{
				PRINT_ERROR("LIBBMDCURLERR Error in setting user certificate\n");
			}

			/********************************************************/
			/*	podpieie klucza prywatnego do kontekstu ssl	*/
			/********************************************************/
			status=SSL_CTX_use_RSAPrivateKey(sslctx, _GLOBAL_ctx->file->privateKey);
			if (status!=1)
			{
				PRINT_ERROR("LIBBMDCURLERR Error in setting user private key\n");
			}

			break;
		case BMD_CTX_SOURCE_PKCS11:
			PRINT_INFO("LIBBMDCURLINF Using crypto device\n");
			/********************************************************/
			/*	podpieie certyfikatu do kontekstu ssl		*/
			/********************************************************/
			status=SSL_CTX_use_certificate_ASN1(sslctx, _GLOBAL_ctx->pkcs11->cert->size, \
			(unsigned char*)(_GLOBAL_ctx->pkcs11->cert->buf));
			if (status!=1)
			{
				PRINT_ERROR("LIBBMDCURLERR Error in setting user certificate\n");
			}

			/************************************************************************/
			/*	tutaj mam problem z podpieciem klucza prywatnego z karty	*/
			/*	sposobem moze być uzycie enginow openSSL, ale nie wiem w	*/
			/*	w jaki sposob sie do tego dobrać				*/
			/************************************************************************/

			/*status=SSL_CTX_use_RSAPrivateKey(sslctx, ctx->pkcs11->privateKey);
			if (status!=1)
			{
				PRINT_ERROR("LIBBMDCURLERR Error in setting user private key\n");
			}*/
			return (-1)*BMD_CTX_SOURCE_PKCS11;
			break;
	}
	return CURLE_OK;
}



long BMD_set_curl_host(	CURL *curl,
			char *host,
			long port)
{
long status=0;

	PRINT_INFO("LIBBMDCURLINF Setting curl's host options\n");
	status=curl_easy_setopt(curl, CURLOPT_PORT, port);
	if (status!=CURLE_OK)
	{
		PRINT_ERROR("LIBBMDCURLERR Error in setting curl option. Error=%i\n",CURL_ERR_SET_OPT);
		return CURL_ERR_SET_OPT;
	}

	status=curl_easy_setopt(curl, CURLOPT_URL, host);
	if (status!=CURLE_OK)
	{
		PRINT_ERROR("LIBBMDCURLERR Error in setting curl option. Error=%i\n",CURL_ERR_SET_OPT);
		return CURL_ERR_SET_OPT;
	}


	return BMD_OK;
}

long BMD_set_curl_certificate_options(	CURL *curl,		/*wskaznik do curl'a*/
					char *cert_type,	/*roszerzeneie certyfikatu*/
					char *cert,		/*plik certyfikatu*/
					char *cert_pass,	/*haslo certyfikatu*/
					char *key_type,		/*roszerzeneie klucza*/
					char *key,		/*plik klucza*/
					char *key_pass)		/*haslo klucza*/
{
long status=0;

	PRINT_INFO("LIBBMDCURLINF Setting curl's certificate options\n");
	status=curl_easy_setopt(curl,CURLOPT_SSLCERTTYPE,cert_type);
	if (status!=CURLE_OK)
	{
		PRINT_ERROR("LIBBMDCURLERR Error in setting curl option. Error=%i\n",CURL_ERR_SET_OPT);
		return CURL_ERR_SET_OPT;
	}

	status=curl_easy_setopt(curl,CURLOPT_SSLCERT,cert);
	if (status!=CURLE_OK)
	{
		PRINT_ERROR("LIBBMDCURLERR Error in setting curl option. Error=%i\n",CURL_ERR_SET_OPT);
		return CURL_ERR_SET_OPT;
	}

	status=curl_easy_setopt(curl,CURLOPT_SSLCERTPASSWD,cert_pass);
	if (status!=CURLE_OK)
	{
		PRINT_ERROR("LIBBMDCURLERR Error in setting curl option. Error=%i\n",CURL_ERR_SET_OPT);
		return CURL_ERR_SET_OPT;
	}

	status=curl_easy_setopt(curl,CURLOPT_SSLKEYTYPE,key_type);
	if (status!=CURLE_OK)
	{
		PRINT_ERROR("LIBBMDCURLERR Error in setting curl option. Error=%i\n",CURL_ERR_SET_OPT);
		return CURL_ERR_SET_OPT;
	}

	status=curl_easy_setopt(curl,CURLOPT_SSLKEY,key);
	if (status!=CURLE_OK)
	{
		PRINT_ERROR("LIBBMDCURLERR Error in setting curl option. Error=%i\n",CURL_ERR_SET_OPT);
		return CURL_ERR_SET_OPT;
	}

	status=curl_easy_setopt(curl,CURLOPT_SSLKEYPASSWD,key_pass);
	if (status!=CURLE_OK)
	if (status!=CURLE_OK)
	{
		PRINT_ERROR("LIBBMDCURLERR Error in setting curl option. Error=%i\n",CURL_ERR_SET_OPT);
		return CURL_ERR_SET_OPT;
	}

	return BMD_OK;
}

long BMD_set_curl_output_options(	CURL * curl,		/*wskaznik do curl'a*/
					FILE *fd)
{
long status=0;

	PRINT_INFO("LIBBMDCURLINF Setting curl's output options\n");

	status=curl_easy_setopt(curl, CURLOPT_WRITEDATA, fd);
	if (status!=CURLE_OK)
	{
		PRINT_ERROR("LIBBMDCURLERR Error in setting curl option. Error=%i\n",CURL_ERR_SET_OPT);
		return CURL_ERR_SET_OPT;
	}

	status=curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, getting_data);
	if (status!=CURLE_OK)
	{
		PRINT_ERROR("LIBBMDCURLERR Error in setting curl option. Error=%i\n",CURL_ERR_SET_OPT);
		return CURL_ERR_SET_OPT;
	}
	return BMD_OK;
}

long BMD_set_curl_host_verification_options(	CURL *curl,	/*wskaznik do curl'a*/
						long verify,	/*czy weryfikować host*/
						char *capath,	/*ściezka do katalogu z certyfikatami do weryfikacji*/
						char *cainfo	/*plik z certyfikatami do weryfikacji*/
						)
{
long status=0;

	PRINT_INFO("LIBBMDCURLINF Setting curl's host verification options\n");

	status=curl_easy_setopt(curl,CURLOPT_SSL_VERIFYPEER,verify);
	if (status!=CURLE_OK)
	{
		PRINT_ERROR("LIBBMDCURLERR Error in setting curl option. Error=%i\n",CURL_ERR_SET_OPT);
		return CURL_ERR_SET_OPT;
	}

	if (verify==0)
	{
		return BMD_OK;
	}

	status=curl_easy_setopt(curl,CURLOPT_SSL_VERIFYHOST,2);
	if (status!=CURLE_OK)
	{
		PRINT_ERROR("LIBBMDCURLERR Error in setting curl option. Error=%i\n",CURL_ERR_SET_OPT);
		return CURL_ERR_SET_OPT;
	}

	if (capath!=NULL)
	{
		status=curl_easy_setopt(curl,CURLOPT_CAPATH,capath);
		if (status!=CURLE_OK)
		{
			PRINT_ERROR("LIBBMDCURLERR Error in setting curl option. Error=%i\n",CURL_ERR_SET_OPT);
			return CURL_ERR_SET_OPT;
		}
	}

	if (cainfo!=NULL)
	{
		status=curl_easy_setopt(curl,CURLOPT_CAINFO,cainfo);
		if (status!=CURLE_OK)
		{
			PRINT_ERROR("LIBBMDCURLERR Error in setting curl option. Error=%i\n",CURL_ERR_SET_OPT);
			return CURL_ERR_SET_OPT;
		}
	}

	return BMD_OK;
}

long BMD_curl_operate(	CURL *curl)	/*wskaznik do curl'a*/
{
long status=0;

char *error_buffer=(char*)calloc(CURL_ERROR_SIZE+1000, 1); //WSZ
curl_easy_setopt(curl,CURLOPT_ERRORBUFFER,error_buffer); //WSZ


	PRINT_INFO("LIBBMDCURLINF Starting curl's session\n");
	status=curl_easy_perform(curl);
	if (status!=CURLE_OK)
	{
		free(error_buffer); error_buffer=NULL;//WSZ
		//PRINT_ERROR("LIBBMDCURLERR Error in performing session curl. Error=%li\n",status);
		return status;
	}
	free(error_buffer); error_buffer=NULL;//WSZ
	return BMD_OK;
}

long BMD_curl_clean(CURL *curl)	/*wskaznik do curl'a*/
{
	curl_easy_cleanup(curl);
	/************************************************************************/
	/*	ponizsza funkcja wylaczala inne funkcjonalności grubego klenta 	*/
	/************************************************************************/
	//curl_global_cleanup();
	return BMD_OK;
}

char **BMD_curl_get_engines(CURL *curl)
{
long status=0;
struct curl_slist *engine_list=NULL;
struct curl_slist *ptr=NULL;
char **engines=NULL;
long count=0;

	PRINT_INFO("LIBBMDCURLINF Getting available engines\n");
	status=curl_easy_getinfo(curl,CURLINFO_SSL_ENGINES,&engine_list);
	ptr=engine_list;
	while(ptr!=NULL)
	{
		count++;
		ptr=ptr->next;
	}

	if (count)
	{
		engines=(char **)malloc(count+1);
	}

	count=0;
	ptr=engine_list;
	while(ptr!=NULL)
	{
		engines[count]=(char *)malloc(strlen(ptr->data)+2);
		memset(engines[count],0,strlen(ptr->data)+1);
		memcpy(engines[count],ptr->data,strlen(ptr->data));
		count++;
		ptr=ptr->next;
	}
	engines[count+1]=NULL;
	return engines;
}

void *myMemoryRealloc(void *ptr, size_t size) {
  if(ptr)
    return realloc(ptr, size);
  else
    return malloc(size);
}

size_t WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data) {
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)data;
 
  mem->memory = myMemoryRealloc(mem->memory, mem->size + realsize + 1);
  if (mem->memory) {
    memcpy(&(mem->memory[mem->size]), ptr, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;
  }
  return realsize;
}



