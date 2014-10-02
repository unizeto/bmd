#include <bmd/libbmdutils/libbmdutils.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>
#include <bmd/libbmd/libbmd.h>
#include <bmd/libbmderr/libbmderr.h>

#ifdef WIN32
#include <windows.h>
#pragma warning(disable:4100)
#else
#include <dirent.h>
#endif

#ifdef _WIN32

#include <time.h>

/**
 * Funkcja pobiera czas w zależności od strefy czasowej.

 * @param[out] tv - adres struktury timeval.
 * @param[out] tz - adres struktury timezone.

 * @return status zakończenia operacji.
 * @retval 0 - pomyślne zakończenie.
*/

long gettimeofday(struct timeval *tv, struct timezone *tz)
{
    FILETIME        ft;
    LARGE_INTEGER   li;
    __int64         t;
    static long      tzflag;

    if (tv)
    {
        GetSystemTimeAsFileTime(&ft);
        li.LowPart  = ft.dwLowDateTime;
        li.HighPart = ft.dwHighDateTime;
        t  = li.QuadPart;       /* In 100-nanosecond intervals */
        t -= EPOCHFILETIME;     /* Offset to the Epoch time */
        t /= 10;                /* In microseconds */
        tv->tv_sec  = (long)(t / 1000000);
        tv->tv_usec = (long)(t % 1000000);
    }

    if (tz)
    {
        if (!tzflag)
        {
            _tzset();
            tzflag++;
        }
        tz->tz_minuteswest = _timezone / 60;
        tz->tz_dsttime = _daylight;
    }

    return 0;
}
#endif /* ifdef _WIN32*/



/**
funkcja GetActualTime() pobiera aktualny czas (wypelnia strukture tm aktualnym czasem)
Funkcja jest cross-platformowa i dostarcza implementacje dla Linux i Windows.
Uwaga!!! Funckja bazuje na localtime(), wiec uwzglednia przesuniecie czasowe.

@param actualTime to adres struktury tm, ktora zostanie wypelniona
aktualnym czasem
*/
long GetActualTime(struct tm *actualTime)
{

#ifndef WIN32
	struct timeval timevalNow;

	if(actualTime == NULL)
		{ return -1; }

	gettimeofday(&timevalNow,NULL);
	if(localtime_r(&(timevalNow.tv_sec), actualTime) == NULL)
		{ return -2; }
	else
		{ return 0;	}

#else /*WIN32*/

	time_t time_tNow;

	if(actualTime == NULL)
		{ return -1; }

	_time64(&time_tNow);
	if( _localtime64_s(actualTime, &time_tNow) != 0)
		{ return -2; }
	else
		{ return 0;	}

#endif /*ifndef WIN32*/

}



/**
* @author WSZ
* Funkcja bmdGetSubstring() wydobywa podlancuch znakowy od znaku start do znaku end (nie wlaczajac znaku end).
*
* @param[in] string to adres przetwarzanego lancucha znakowego
* @param[in] start to index znaku, ktory rozpoczyna wydobywany podlancuch
* @param[in] end to index znaku nastepujacego po znaku, ktory bedzie ostatnim w wydobywanym podlancuchu
* @param[out] substring to adres wskaznika, do ktorego bedzie przypisany (zaalokowany) wydobywany podlancuch
* @return Funkcja zwraca BMD_OK w przypadku sukcesu, lub odpowiednia wartosc mniejsza od BMD_OK w przypadku bledu.
*/
long bmdGetSubstring(char *string, long start, long end, char** substring)
{
long i, offset=0;
char *new_string;

	if(string == NULL)
		{ BMD_FOK(BMD_ERR_PARAM1); }
	if(start >= end)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if((long)strlen(string) < end)
		{ BMD_FOK(BMD_ERR_PARAM3); }
	if(substring == NULL)
		{ BMD_FOK(BMD_ERR_PARAM4); }
	if(*substring != NULL)
		{ BMD_FOK(BMD_ERR_PARAM4); }

	
        new_string=(char*)calloc((end-start+1), sizeof(char));
	if(new_string == NULL)
		{ BMD_FOK(NO_MEMORY); }

	for(i=start;i<end;i++)
	{
		new_string[offset]=string[i];
		offset++;
	}
	new_string[offset]='\0';

	*substring=new_string;
	new_string=NULL;
	return BMD_OK;
}



/** Funkcja zamienia string symbolizujący OID na tablicę cyfr OIDu (long)
 * @param[in] string - zmienna łańcuchowa symbolizująca OID.
 * @param[out] oid_table - adres tablicy OID-ów typu (long).
 * @param[out] oid_table_size - rozmiar tablicy OID-ów.

 * @return status zakończenia operacji.
 * @retval 0 - pomyślne zakończenie.
 * @retval NO_MEMORY - błąd alokacji pamięci.
 * @retval -d - błąd dopasowania do wzorca.
 * @note brak sprawdzenia poprawności przekazywanych argumentów
 */

long str_of_OID_to_ulong(const char *string, long **oid_table, long *oid_table_size)
{
SubstringsGenBuf_t substrings;		/* genbuf zawierajacy tablice substringow i ich ilosc */
GenBuf_t analized_genbuf;		/* analizowany bufor zawierajacy dane z pliku */
char *regexp = "[0-9]+";		/* wyrazenie regularne szukajace cyfr OIDu */
long i = 0;
long err = 0;

	/* Utworz genbuf ze stringa i jego rozmiaru */
	analized_genbuf.buf = (char *) string;
	analized_genbuf.size = (int)strlen(string);

	/* Szukaj w pojedynczj linii (w stringu), wielokrotnego dopasowania do 		*/
	/* wzorca - czyli szukaj liczb	skladajacych sie na OID				*/
	err = find_substrings(&analized_genbuf, regexp, &substrings, SINGLELINE, YES);
	if (err != 0) return err;

	/* Tablica substrings->substrings_table zawiera kolejne dopasowania 		*/
	/* zas substrings->no_of_substrings ilosc dopasowanych elementow w tablicy	*/

	/* na podstawie ilosci dopasowan liczb, z ktorych sklda sie OID - alokuje pamiec          */
	/* na argumenty tablicy longow bedacyej argumentem funkcji OBJECT_IDENTIFIER_set_arcs */
	*oid_table_size = sizeof(long) * substrings.no_of_substrings;
	*oid_table = (long *) malloc (sizeof(long) * substrings.no_of_substrings );
	if (*oid_table == NULL)
	{
		return NO_MEMORY;
	}

	for (i=0; i<substrings.no_of_substrings; i++)
	{
		(*oid_table)[i] = strtol(substrings.substrings_table[i], (char **)NULL, 10);
	}

	/* teraz substringi nie sa juz nam potrzebne */
	free_substrings(&substrings);
	return err;
}

/**
 * Funkcja ustala typ OID-u.
 * 1) sprawdzenie czy OID nie odpowiada danym z RFC3281 - atr. systemowe
 * - oid_type=SYS_METADATA
 * 2) sprawdzenie czy pierwszych 7 oktetów pasuje do prefixu prefix_sys_metadata - atr. systemowe
 * - oid_type=SYS_METADATA
 * 3) sprawdzenie czy pierwszych 7 oktetów pasuje do prefixu prefix_usr_metadata - atr. uźytkownika
 * - oid_type=USER_METADATA
 * 4) jeśli żadne z tych kryteriów nie jest spełnione - atr. dodatkowe
 * - oid_type=ADDITIONAL_METADATA

 * @param oid -  adres struktury OID_attr_t.
 * @retval ADDITIONAL_METADATA.
 * @retval SYS_METADATA.
 * @retval USER_METADATA.
 * @note funkcja nie rozpoznaje OID-ów ( PKI_METADATA, ujemne OID-y klasyfikuje do
 * ADDITIONAL_METADATA.
 * @note brak sprawdzenia poprawności przekazywanych argumentów.
*/
long determine_oid_type_str(	char *oid)
{
long oid_type=ADDITIONAL_METADATA;

	/************************************************************************/
	/* okreslenie typu metadanej po jej prefixie (poczatkowej czesci oid'a) */
	/************************************************************************/
	if (strncmp(oid, OID_PREFIX_ACTION_METADATA, strlen(OID_PREFIX_ACTION_METADATA))==0)
	{
		oid_type=ACTION_METADATA;
	}
	else if (	(strncmp(oid, OID_PREFIX_SYS_METADATA, strlen(OID_PREFIX_SYS_METADATA))==0) ||
			(strcmp(oid, OID_RFC3281_GROUP)==0) ||
			(strcmp(oid, OID_RFC3281_CLEARANCE)==0) ||
			(strcmp(oid, OID_RFC3281_ROLE)==0))
	{
		oid_type=SYS_METADATA;
	}
	else if (strncmp(oid, OID_PREFIX_PKI_METADATA, strlen(OID_PREFIX_PKI_METADATA))==0)
	{
		oid_type=PKI_METADATA;
	}

	return oid_type;
}

char *determine_oid_column_name (char *oid)
{
char *tmp	= NULL;
	if (strcmp(oid, OID_SYS_METADATA_CRYPTO_OBJECTS_ID)==0)				{	return "crypto_objects.id";	}
	else if (strcmp(oid, OID_SYS_METADATA_CRYPTO_OBJECTS_LOCATION_ID)==0)		{	return "crypto_objects.location_id";	}
	else if (strcmp(oid, OID_SYS_METADATA_CRYPTO_OBJECTS_FILENAME)==0)		{	return "crypto_objects.name";	}
	else if (strcmp(oid, OID_SYS_METADATA_CRYPTO_OBJECTS_FILESIZE)==0)		{	return "crypto_objects.filesize";	}
	else if (strcmp(oid, OID_SYS_METADATA_CRYPTO_OBJECTS_FILESIZE_LESS)==0)		{	return "crypto_objects.filesize";	}
	else if (strcmp(oid, OID_SYS_METADATA_CRYPTO_OBJECTS_FILESIZE_MORE)==0)		{	return "crypto_objects.filesize";	}
	else if (strcmp(oid, OID_SYS_METADATA_CRYPTO_OBJECTS_CREATOR)==0)		{	return "crypto_objects.creator";	}
	else if (strcmp(oid, OID_SYS_METADATA_CRYPTO_OBJECTS_OWNER)==0)			{	return "crypto_objects.fk_owner";	}
	else if (strcmp(oid, OID_SYS_METADATA_CRYPTO_OBJECTS_TIMESTAMP)==0)		{	return "crypto_objects.insert_date";	}
	else if (strcmp(oid, OID_SYS_METADATA_CRYPTO_OBJECTS_TIMESTAMP_LESS)==0)	{	return "crypto_objects.insert_date";	}
	else if (strcmp(oid, OID_SYS_METADATA_CRYPTO_OBJECTS_TIMESTAMP_MORE)==0)	{	return "crypto_objects.insert_date";	}
	else if (strcmp(oid, OID_SYS_METADATA_CRYPTO_OBJECTS_GROUP)==0)			{	return "crypto_objects.fk_group";	}
	else if (strcmp(oid, OID_SYS_METADATA_CRYPTO_OBJECTS_CORESPONDING_ID)==0)	{	return "crypto_objects.corresponding_id";	}
	else if (strcmp(oid, OID_SYS_METADATA_CRYPTO_OBJECTS_VISIBLE)==0)		{	return "crypto_objects.visible";	}
	else if (strcmp(oid, OID_SYS_METADATA_CRYPTO_OBJECTS_PRESENT)==0)		{	return "crypto_objects.present";	}
	else if (strcmp(oid, OID_SYS_METADATA_CRYPTO_OBJECTS_FILE_TYPE)==0)		{	return "crypto_objects.file_type";	}
	else if (strcmp(oid, OID_SYS_METADATA_CRYPTO_OBJECTS_TRANSACTION_ID)==0)	{	return "crypto_objects.transaction_id";	}
	else if (strcmp(oid, OID_SYS_METADATA_CRYPTO_OBJECTS_POINTING_ID)==0)		{	return "crypto_objects.pointing_id";	}
	else if (strcmp(oid, OID_SYS_METADATA_CRYPTO_OBJECTS_POINTING_LOCATION_ID)==0)	{	return "crypto_objects.pointing_location_id";	}
	else if (strcmp(oid, OID_SYS_METADATA_CRYPTO_OBJECTS_FOR_GRANT)==0)		{	return "crypto_objects.for_grant";	}
	else if (strcmp(oid, OID_SYS_METADATA_CRYPTO_OBJECTS_HASH_VALUE)==0)		{	return "crypto_objects.hash_value";	}
	else if (strcmp(oid, OID_SYS_METADATA_CRYPTO_OBJECTS_EXPIRATION_DATE)==0)		{	return "crypto_objects.expiration_date";	}
	else if (strcmp(oid, OID_SYS_METADATA_CRYPTO_OBJECTS_FOR_TIMESTAMPING)==0)		{	return "crypto_objects.for_timestamping";	}
	
	else if (strcmp(oid, OID_ACTION_METADATA_ID)==0)				{	return "action_register.id";	}
	else if (strcmp(oid, OID_ACTION_METADATA_ACTION_LOCATION_ID)==0)		{	return "action_register.location_id";	}
	else if (strcmp(oid, OID_ACTION_METADATA_OBJECT_ID)==0)				{	return "action_register.fk_objects";	}
	else if (strcmp(oid, OID_ACTION_METADATA_OBJECT_TYPE)==0)			{	return "action_register.objects_type";	}
	else if (strcmp(oid, OID_ACTION_METADATA_ACTION)==0)				{	return "action_register.fk_actions";	}
	else if (strcmp(oid, OID_ACTION_METADATA_USER)==0)				{	return "action_register.fk_users";	}
	else if (strcmp(oid, OID_ACTION_METADATA_USER_CLASS)==0)			{	return "action_register.fk_users_class";	}
	else if (strcmp(oid, OID_ACTION_METADATA_STATUS)==0)				{	return "action_register.status";	}
	else if (strcmp(oid, OID_ACTION_METADATA_STATUS_MESSAGE)==0)			{	return "action_register.status_msg";	}
	else if (strcmp(oid, OID_ACTION_METADATA_DATE)==0)				{	return "action_register.action_date";	}
	else if (strcmp(oid, OID_PKI_METADATA_TIMESTAMP_ID)==0)				{	return "prf_timestamps.id";	}
	else if (strcmp(oid, OID_PKI_METADATA_TIMESTAMP_LOCATION_ID)==0)		{	return "prf_timestamps.location_id";	}
	else if (strcmp(oid, OID_PKI_METADATA_TIMESTAMP_CRYPTO_OBJECTS)==0)		{	return "prf_timestamps.fk_crypto_objects";	}
	else if (strcmp(oid, OID_PKI_METADATA_TIMESTAMP_OBJECTS)==0)			{	return "prf_timestamps.fk_objects";	}
	else if (strcmp(oid, OID_PKI_METADATA_TIMESTAMP_VERSION)==0)			{	return "prf_timestamps.version";	}
	else if (strcmp(oid, OID_PKI_METADATA_TIMESTAMP_POLICYOID)==0)			{	return "prf_timestamps.policyoid";	}
	else if (strcmp(oid, OID_PKI_METADATA_TIMESTAMP_SERIALNUMBER)==0)		{	return "prf_timestamps.serialnumber";	}
	else if (strcmp(oid, OID_PKI_METADATA_TIMESTAMP_GENTIME)==0)			{	return "prf_timestamps.gentime";	}
	else if (strcmp(oid, OID_PKI_METADATA_TIMESTAMP_TIMESTAMPCERTVALIDTO)==0)	{	return "prf_timestamps.timestampcertvalidto";	}
	else if (strcmp(oid, OID_PKI_METADATA_TIMESTAMP_TSADN)==0)			{	return "prf_timestamps.tsadn";	}
	else if (strcmp(oid, OID_PKI_METADATA_TIMESTAMP_TSASN)==0)			{	return "prf_timestamps.tsasn";	}
	else if (strcmp(oid, OID_PKI_METADATA_TIMESTAMP_STANDARD)==0)			{	return "prf_timestamps.standard";	}
	else if (strcmp(oid, OID_PKI_METADATA_TIMESTAMP_USERORSYSTEM)==0)		{	return "prf_timestamps.userorsystem";	}
	else if (strcmp(oid, OID_PKI_METADATA_TIMESTAMP_CONSERVED)==0)			{	return "prf_timestamps.conserved";	}
	else if (strcmp(oid, OID_PKI_METADATA_TIMESTAMP_TIMESTAMP)==0)			{	return "prf_timestamps.timestamp";	}
	else if (strcmp(oid, OID_PKI_METADATA_TIMESTAMP_TIMESTAMP_TYPE)==0)		{	return "prf_timestamps.timestamp_type";	}

	bmd_str_replace(&oid, "." , "_");
	asprintf(&tmp, "crypto_objects.amv_%s",oid);
	if (tmp==NULL)	{	return NULL;	}

	return tmp;
}



/**
 * Funkcja pobiera dane z adresu wskazywanego przez (url) i zapisuje
 * je do bufora (GenBuf_t).

 * @param[in] url - adres url.
 * @retval chunk - adres bufora GenBuf_t.
 * @retval NULL - błąd.

*/
GenBuf_t *get_from_url(char *url)
{
	CURL *curl_handle;
	GenBuf_t *chunk;
	long status;

	chunk=(GenBuf_t *)malloc(sizeof(GenBuf_t));
	chunk->buf=NULL;
	chunk->size=0;

	//curl_global_init(CURL_GLOBAL_ALL);
	curl_handle = curl_easy_init();
	if (curl_handle == NULL) {
		PRINT_DEBUG("CURL: Cannot initialize curl\n");
		free(chunk);
		return NULL;
	}

	if (curl_easy_setopt(curl_handle, CURLOPT_URL, url) != CURLE_OK) {
		PRINT_DEBUG("CURL: Cannot set url to %s\n",url);
		free(chunk);
		return NULL;
	};


	if (curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_to_gen_buf_callback) != CURLE_OK) {
		PRINT_DEBUG("CURL: cannot set write callback\n");
		free(chunk);
		return NULL;
	};

	if (curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)chunk) != CURLE_OK) {
		PRINT_DEBUG("CURL: cannot set write data\n");
		free(chunk);
		return NULL;
	}

	if (curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "BMD-agent/1.0") != CURLE_OK) {
		PRINT_DEBUG("CURL: cannot set user agent\n");
		free(chunk);
		return NULL;
	}

	if (curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0) != CURLE_OK) {
		PRINT_DEBUG("CURL: cannot disable SSL certificate verification\n");
		free(chunk);
		return NULL;
	}


	if (curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYHOST, 0) != CURLE_OK) {
		PRINT_DEBUG("CURL: cannot disable SSL host verification\n");
		free(chunk);
		return NULL;
	}

	if ((status = curl_easy_perform(curl_handle)) != CURLE_OK) {
		PRINT_DEBUG("CURL: cannot connect to address %s: error %li\n", url, status);
		if (chunk->buf != NULL) {
			free(chunk->buf);
		}
		free(chunk);
		return NULL;
	};

	curl_easy_cleanup(curl_handle);

	return chunk;
}


/**
 * Funkcja wywoływana przez funkcję get_from_url, kopiuje zawartość wskazywaną
 * przez url (ptr) do bufora generycznego (data).

 * @param[in] ptr - dane wskazywane przez url.
 * @param[in] size - rozmiar pojedynczej danej.
 * @param[in] nmemb - ilość danych.
 * @param[out] data - adres bufora generycznego.

 * retval s - ilość skopiowanych danych.
 * retval -d - bład.

*/
size_t  write_to_gen_buf_callback(void *ptr, long size, long nmemb, void *data)
{
	register long realsize = size * nmemb;
	GenBuf_t *mem = (GenBuf_t *)data;

	bmd_assert (size >= 0 && nmemb >= 0, "LK");
	bmd_assert (data != NULL, "LK");

	bmd_assert (mem->size >= 0, "LK");
	bmd_assert(ptr != NULL, "LK");

	mem->buf = (char *)myrealloc(mem->buf, mem->size + realsize + 1);
	if (mem->buf)
	{
		memcpy(&(mem->buf[mem->size]), ptr, realsize);
		mem->size += realsize;
		mem->buf[mem->size] = 0;
	} else {
		return ERR(ERR_sys, "Cannot (re)allocate memory!", "LK");
	}

	return realsize;
}


/**
 * Funkcja wykonuje realokację pamięci.

 * @param[in,out] ptr - adres bloku pamięci.
 * @param[in] size - rozmiar.

 * @return zwraca adres bloku pamięci lub NULL w
 * przypadku błedu.

*/

void *myrealloc(void *ptr, long size)
{
	if(ptr)
		return realloc(ptr, size);
	else
		return malloc(size);
}



/**
 * Funkcja wyszukuje podłańcuch (keyword) w łańcuchu znaków (input)
 * i zastępuje go podłańcuchem (replace_with).

 * @param[in,out] input - wejściowy łańcuch znaków.
 * @param[in] keyword - wyszukiwany podłańcuch.
 * @param[in] replace_with - zastępujący podłańcuch.
 * @retval 0 - zakończenie operacji.
 * @note funkcja nie sprawdza poprawności przekazywanych argumentów
 * @note dla niektórych prawidłowych parametrów wywołanie funkcji
 *  powoduje zawieszenie programu.
*/
long bmd_str_replace(char **input, char *keyword, char *replace_with)
{

        char *pos_keyword		= NULL;
        char *before			= NULL;
        char *after			= NULL;
        char *tmp			= NULL;

	if(input == NULL)		{       BMD_FOK(BMD_ERR_PARAM1);        }
	if(*input == NULL)		{	BMD_FOK(BMD_ERR_PARAM1);	}
	if(keyword == NULL)		{	BMD_FOK(BMD_ERR_PARAM2);	}
	if(replace_with == NULL)	{	BMD_FOK(BMD_ERR_PARAM3);	}

        for(;;)
        {
                pos_keyword=strstr(*input,keyword);
                if(pos_keyword==NULL)
		{
                        break;
		}

		if( ( pos_keyword-(*input) )>0 )
                {
                        before=(char *)malloc(pos_keyword-(*input)+ 1);
			if(before == NULL)		{	BMD_FOK(NO_MEMORY);	}
                        memset(before,0,pos_keyword-(*input)+1);
                        memmove(before,*input,pos_keyword-(*input));
                }
                else
		{
                        before=NULL;
		}

		if(strlen(pos_keyword+strlen(keyword))>0)
                {
                        after=(char *)malloc(strlen(pos_keyword+strlen(keyword))+ 1);
			if(after == NULL)		{	BMD_FOK(NO_MEMORY);	}
                        memset(after,0,strlen(pos_keyword+strlen(keyword))+1);
                        memmove(after,pos_keyword+strlen(keyword),strlen(pos_keyword+strlen(keyword)));
                }
                else
		{
                        after=NULL;
		}

                if( (before==NULL) && (after==NULL) )
		{
                        asprintf(&tmp,"%s",replace_with);
		}
                if( (before) && (after==NULL) )
		{
                        asprintf(&tmp,"%s%s",before,replace_with);
		}
                if( (before==NULL) && (after) )
		{
                        asprintf(&tmp,"%s%s",replace_with,after);
		}
                if( (before) && (after) )
		{
                        asprintf(&tmp,"%s%s%s",before,replace_with,after);
		}
		if(tmp == NULL)		{	BMD_FOK(NO_MEMORY);	}

                free(*input);*input=NULL;
		*input = tmp;
                tmp=NULL;

                free(before);before=NULL;
                free(after);after=NULL;
        }
        return 0;
}



/**
 * Funkcja pobiera listę plików z katalogu.

 * @param[in] dir_name - nazwa katalogu.
 * @param[out] files - adres tablicy plików.

 * @return status zakończenia operacji.
 * @retval 0 - zakończenie pomyślne.
 * @retval -1 - bład.
 * @note brak sparawdzenia poprawności przekazywanych argumentów.
 * @note lista plików kończy się strażnikiem bezpieczniej byłoby
 *  wprowadzić dodatkowy parametr dla zwracania tej ilości.
*/

long get_files_from_dir(char *dir_name,char ***files)
{
// brak sprawdzenia czy files wskazuje na zaalokowany obszar.
#ifndef WIN32
	DIR *dp=NULL;
	struct dirent *d=NULL;
	struct stat file_info;
	long status;
	long count=0;
	long i=0;
	long length,dir_length,absolute_length;

	char current_dir[256];

	getcwd(current_dir,256);
	// brak sprawdzenia pomyślności wykonania
	chdir(dir_name);
	dp=opendir(dir_name);
	if(dp==NULL)
	{
		return -1;
	}

	/* policz ilosc plikow w katalogu */
	while( (d=readdir(dp)) )
	{
		if(d->d_ino !=0)
		{
			status=stat(d->d_name,&file_info);
			if(S_ISDIR(file_info.st_mode))
			{
			}
			else
				count++;
		}
	}

	(*files)=(char **)malloc((count+1)*sizeof(char *));
	//brak sprawdzenia czy zaalokowano pamięć.
	rewinddir(dp);
	dir_length=strlen(dir_name);
	while( (d=readdir(dp)) )
	{
		if(d->d_ino !=0)
		{
			status=stat(d->d_name,&file_info);
			if(S_ISDIR(file_info.st_mode))
			{
			}
			else
			{
				length=strlen(d->d_name);
				absolute_length=length+dir_length+3;
				(*files)[i]=(char *)malloc(absolute_length+2);
				//brak sprawdzenia czy zaalokowano pamięć
				memset((*files)[i],0,absolute_length+1);
				memcpy((*files)[i],dir_name,dir_length);
				memcpy((*files)[i]+dir_length,"/",1);
				memcpy((*files)[i]+dir_length+1,d->d_name,length);
				i++;
			}
		}
	}
	(*files)[i]=NULL;
	closedir(dp);
	chdir(current_dir);
	// brak sprawdzenia czy zmieniono katalog
	return 0;
#else
	return -1;
#endif
}


/**
 * Funkcja pobiera listę plików z katalogu.

 * @param[in] dir - nazwa katalogu.
 * @param[out] files - tablica znalezionych plików.

 * @return status zakończenia operacji.
 * @retval BMD_OK - zakończenie pomyślne.
 * @retval BMD_ERR_OP_FAILED - bład wyszukiwania plików.
 * @retval BMD_ERR_PARAM1 - niepoprawny argument.
 * @retval  BMD_ERR_PARAM2 - niepoprawny argument.
*/

long bmd_get_files_from_dir(char *dir,char ***files)
{
#ifdef WIN32
	long i=0;
	char *utf8Filename=NULL;
	wchar_t *wideFilename=NULL;
	HANDLE handle;
	WIN32_FIND_DATA findData;



	if(dir==NULL)
		return BMD_ERR_PARAM1;
	if(files==NULL)
		return BMD_ERR_PARAM2;
	if((*files)!=NULL)
		return BMD_ERR_PARAM2;


	asprintf(&utf8Filename,"%s\\%s",dir,"*.cer");
	//brak sprawdzenia czy zaalokowano pamięć
	UTF8StringToWindowsString(utf8Filename, &wideFilename, NULL);
	handle=FindFirstFile(wideFilename, &findData);
	free(utf8Filename); utf8Filename=NULL;
	free(wideFilename); wideFilename=NULL;
	if(handle==INVALID_HANDLE_VALUE)
		return BMD_ERR_OP_FAILED;

	(*files)=(char **)realloc(*files,sizeof(char *)*(i+2));
	//brak sprawdzenia czy zaalokowano pamięć//brak sprawdzenia czy zaalokowano pamięć
	(*files)[i]=NULL;
	(*files)[i+1]=NULL;

	WindowsStringToUTF8String(findData.cFileName, &utf8Filename, NULL);
	asprintf(&((*files)[0]),"%s\\%s",dir,utf8Filename);
	free(utf8Filename); utf8Filename=NULL;
	//brak sprawdzenia czy zaalokowano pamięć

	while(FindNextFile(handle,&findData))
	{
		i++;
		(*files)=(char **)realloc(*files,sizeof(char *)*(i+2));
		(*files)[i]=NULL;(*files)[i+1]=NULL;
		WindowsStringToUTF8String(findData.cFileName, &utf8Filename, NULL);
		asprintf(&((*files)[i]),"%s\\%s",dir,utf8Filename);
		free(utf8Filename); utf8Filename=NULL;
		//brak sprawdzenia czy zaalokowano pamięć
	}
	FindClose(handle);
	return BMD_OK;
#else
	long status;
	status=get_files_from_dir(dir,files);
	if(status!=BMD_OK)
	    return BMD_ERR_OP_FAILED;

	return BMD_OK;
#endif
}



#ifdef WIN32
// wzorowano na:

/* Like vsprintf but provides a pointer to malloc'd storage, which must
   be freed by the caller.
   Copyright (C) 1994 Free Software Foundation, Inc.

This file is part of the libiberty library.
Libiberty is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

Libiberty is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with libiberty; see the file COPYING.LIB.  If
not, write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */



//static long int_vasprintf PARAMS ((char **, const char *, va_list *));

static int
int_vasprintf_port (char **result,
     const char *format,
     va_list *args)

{
  const char *p = format;
  /* Add one to make sure that it is never zero, which might cause malloc
     to return NULL.  */
  long total_width = (int)strlen (format) + 1;
  va_list ap;

  memcpy (&ap, args, sizeof (va_list));

  while (*p != '\0')
    {
      if (*p++ == '%')
	{
	  while (strchr ("-+ #0", *p))
	    ++p;
	  if (*p == '*')
	    {
	      ++p;
	      total_width += abs (va_arg (ap, int));
	    }
	  else
	    total_width += strtoul (p, (char **) &p, 10);
	  if (*p == '.')
	    {
	      ++p;
	      if (*p == '*')
		{
		  ++p;
		  total_width += abs (va_arg (ap, int));
		}
	      else
	      total_width += strtoul (p, (char **) &p, 10);
	    }
	  while (strchr ("hlL", *p))
	    ++p;
	  /* Should be big enough for any format specifier except %s and floats.  */
	  total_width += 30;
	  switch (*p)
	    {
	    case 'd':
	    case 'i':
	    case 'o':
	    case 'u':
	    case 'x':
	    case 'X':
	    case 'c':
	      (void) va_arg (ap, int);
	      break;
	    case 'f':
	    case 'e':
	    case 'E':
	    case 'g':
	    case 'G':
	      (void) va_arg (ap, double);
	      /* Since an ieee double can have an exponent of 307, we'll
		 make the buffer wide enough to cover the gross case. */
	      total_width += 307;
	      break;
	    case 's':
	      total_width += (int)strlen (va_arg (ap, char *));
	      break;
	    case 'p':
	    case 'n':
	      (void) va_arg (ap, char *);
	      break;
	    }
	  p++;
	}
    }
#ifdef TEST
  global_total_width = total_width;
#endif
  *result = malloc (total_width);
  if (*result != NULL)
    return vsprintf (*result, format, *args);
  else
    return 0;
}

int
vasprintf_port (char **result, const char *format, va_list args)
{
  return int_vasprintf_port (result, format, &args);
}

#endif

/**
sort_oids oznacza tablice oidow
sort_orders oznacza tablice z wartosciami "ASC", "DESC"
*/
long GetSortConditions(char *sort, server_request_data_t* req, char ***sort_oids,long *count_oids,char ***sort_orders,long *count_orders)
{
long iter			= 0;
long jupiter		= 0; 
long foundInForm	= 0;

char** valuesTable		= NULL;
long valuesTableCount	= 0;

char** oidsTable		= NULL;
char** orderTypesTable	= NULL;
long oidsCount			= 0;

	if(sort==NULL)
		{ BMD_FOK(BMD_ERR_PARAM1); }
	if(req == NULL)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(req->no_of_formularz == 0)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(req->no_of_formularz > 0 && req->formularz == NULL)
		{ BMD_FOK(BMD_ERR_PARAM2); }
	if(sort_oids == NULL)
		{ BMD_FOK(BMD_ERR_PARAM3); }
	if(count_oids == NULL)
		{ BMD_FOK(BMD_ERR_PARAM4); }
	if(sort_orders == NULL)
		{ BMD_FOK(BMD_ERR_PARAM5); }
	if(count_orders	== NULL)
		{ BMD_FOK(BMD_ERR_PARAM6); }


	if(bmd_strsep(sort, '|', &valuesTable, &valuesTableCount) < BMD_OK)
	{
		BMD_FOK(BMD_ERR_OP_FAILED);
	}

	// czy wartosci wystepuja parami
	if(valuesTableCount % 2 != 0)
	{
		BMD_FOK(BMD_VERIFY_REQUEST_INCORRECT_SORT_CRITERIA);
	}
	
	// pusty ciag sort
	if(valuesTableCount == 0)
	{
		*sort_oids=NULL;
		*count_oids=0;
		*sort_orders=NULL;
		*count_orders=0;
		return BMD_OK;
	}
	
	for(jupiter=0; jupiter<valuesTableCount; jupiter+=2)
	{
		// sprawdzenie OID'u metadanej
		foundInForm = 0;
		for(iter=0; iter<req->no_of_formularz; iter++)
		{
			if(req->formularz[iter] != NULL && req->formularz[iter]->oid_str != NULL)
			{
				if(strcmp(req->formularz[iter]->oid_str, valuesTable[jupiter]) == 0)
				{
					foundInForm = 1;
					break;
				}
			}
		}
		if(foundInForm == 0)
		{
			bmd_strsep_destroy(&valuesTable, valuesTableCount);
			BMD_FOK(BMD_VERIFY_REQUEST_SEARCH_IMPERMISSIBLE_SORT_METADATA);
		}
		
		// sposob uporzadkowania
		if(strcmp(valuesTable[jupiter+1], "ASC") != 0 && strcmp(valuesTable[jupiter+1], "DESC") != 0)
		{
			bmd_strsep_destroy(&valuesTable, valuesTableCount);
			BMD_FOK(BMD_VERIFY_REQUEST_SEARCH_INVALID_SORT_ORDER);
		}
	}

	oidsCount=valuesTableCount / 2;
	oidsTable=(char**)calloc(oidsCount, sizeof(char*));
	if(oidsTable == NULL)
	{
		bmd_strsep_destroy(&valuesTable, valuesTableCount);
		BMD_FOK(NO_MEMORY);
	}
	orderTypesTable=(char**)calloc(oidsCount, sizeof(char*));
	if(oidsTable == NULL)
	{
		free(oidsTable); oidsTable=NULL;
		bmd_strsep_destroy(&valuesTable, valuesTableCount);
		BMD_FOK(NO_MEMORY);
	}
	
	for(iter=0; iter<oidsCount; iter++)
	{
		oidsTable[iter]=valuesTable[(2*iter)];
		valuesTable[(2*iter)]=NULL;
		
		orderTypesTable[iter]=valuesTable[(2*iter)+1];
		valuesTable[(2*iter)+1]=NULL;
	}
	bmd_strsep_destroy(&valuesTable, valuesTableCount);
	
	*sort_oids=oidsTable;
	oidsTable=NULL;
	*count_oids=oidsCount;
	
	*sort_orders=orderTypesTable;
	orderTypesTable=NULL;
	*count_orders=oidsCount;

	return BMD_OK;
}

long DestroyTableOfStrings(char*** table, long stringsCount)
{
long iter			= 0;

	if(table == NULL)
		{ return -1; }
	if(*table == NULL)
		{ return -2; }
	if(stringsCount < 0)
		{ return -3; }
	
	for(iter=0; iter<stringsCount; iter++)
	{
		free( (*table)[iter] );
		(*table)[iter]=NULL;
	}
	free((*table));
	*table=NULL;
	
	return 0;
}

long DestroyTableOfBuffers(GenBuf_t*** table, long buffersCount)
{
long iter		= 0;

	if(table == NULL)
		{ return -1; }
	if(*table == NULL)
		{ return -2; }
	if(buffersCount < 0)
		{ return -3; }
	
	for(iter=0; iter<buffersCount; iter++)
		{ free_gen_buf( &((*table)[iter]) ); }
	free(*table);
	*table = NULL;
	
	return 0;
}

/** obsluga tablicy stringow */
long bmdStringArray_Create(bmdStringArray_t** newArray)
{
bmdStringArray_t *tempArray=NULL;

	if(newArray == NULL)
		{ return -1; }
	if(*newArray != NULL)
		{ return -2; }

	tempArray=(bmdStringArray_t*)calloc(1, sizeof(bmdStringArray_t));
	if(tempArray == NULL)
		{ return -11; }

	*newArray=tempArray;
	tempArray=NULL;
	return 0;
}

long bmdStringArray_AddItem(bmdStringArray_t* stringArray, char* string)
{
char **tempArray=NULL;
char *tempString=NULL;

	if(stringArray == NULL)
		{ return -1; }
	if(string == NULL)
		{ return -2; }


	tempString=strdup(string);
	if(tempString == NULL)
		{ return -11; }

	tempArray=(char**)realloc(stringArray->array, (stringArray->itemsCounter+1) * sizeof(char*));
	if(tempArray == NULL)
	{
		free(tempString);
		return -12;
	}

	
	stringArray->array=tempArray;
	stringArray->itemsCounter++;
	stringArray->array[stringArray->itemsCounter - 1]=tempString;

	tempArray=NULL;
	tempString=NULL;
	
	return 0;
}

/* alokuje zwracany string, indeksy od 0*/
long bmdStringArray_GetItem(bmdStringArray_t* stringArray, long itemIndex, char** string)
{
char *tempString=NULL;

	if(stringArray==NULL)
		{ return -1; }
	if(itemIndex < 0 || itemIndex >= stringArray->itemsCounter)
		{ return -2; }
	if(string == NULL)
		{ return -3; }
	if(*string != NULL)
		{ return -4; }

	tempString=strdup(stringArray->array[itemIndex]);
	if(tempString == NULL)
		{ return -11; }

	*string=tempString;
	tempString=NULL;
	return 0;
}

/* zwraca wskaznik na string (nie alokuje pamieci)*/
long bmdStringArray_GetItemPtr(bmdStringArray_t* stringArray, long itemIndex, char** stringPtr)
{
	if(stringArray==NULL)
		{ return -1; }
	if(itemIndex < 0 || itemIndex >= stringArray->itemsCounter)
		{ return -2; }
	if(stringPtr == NULL)
		{ return -3; }
	if(*stringPtr != NULL)
		{ return -4; }

	*stringPtr=stringArray->array[itemIndex];
	return 0;
}

long bmdStringArray_GetItemsCount(bmdStringArray_t* stringArray, long* itemsCount)
{
	if(stringArray==NULL)
		{ return -1; }

	*itemsCount=stringArray->itemsCounter;
	return 0;
}

long bmdStringArray_DestroyList(bmdStringArray_t** stringArray)
{
	long iter=0;

	if(stringArray == NULL)
		{ return -1; }
	if(*stringArray != NULL)
		{ return -2; }

	for(iter=0; iter<(*stringArray)->itemsCounter; iter++)
	{
		free((*stringArray)->array[iter]);
	}
	free(*stringArray);
	*stringArray=NULL;
	return 0;
}

/**
* @author WSZ
* Funkcja IsNumericIdentifier() sprawdza, czy lanuch znakowy ma postac identyfikatora numerycznego (dodatnia liczba calkowita).
* Przeznaczona jest do sprawdzania wartosci (przekazywanych w zadaniach), ktore sluza do budowania warunkow zapytan SQL (ochrona przed SQL injection).
* @param[in] identifier to sprawdzany lancuch znakowy
* @return Funkcja zwraca BMD_OK, jesli identifier jest identyfikatorem numerycznym, a wartosc ujemna w przypadku bledu.
*/
long IsNumericIdentifier(char* identifier)
{
long identifierLength			= 0;
long iter						= 0;

	if(identifier == NULL)
		{ BMD_FOK(BMD_ERR_PARAM1); }
	
	identifierLength=(long)strlen(identifier);
	if(identifier == 0)
	{
		BMD_FOK(LIBBMDUTILS_NOT_NUMERIC_IDENTIFIER);
	}
	
	for(iter=0; iter<identifierLength; iter++)
	{
		if(identifier[iter] < '0' || identifier[iter] > '9')
		{
			BMD_FOK(LIBBMDUTILS_NOT_NUMERIC_IDENTIFIER);
		}
	}
	
	return BMD_OK;
}

