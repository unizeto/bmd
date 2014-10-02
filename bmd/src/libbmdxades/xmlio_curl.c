/** @file
Ten plik zawiera mechanizm służący do transmisji plików z wykorzystaniem biblioteki CURL.
Mechanizm ten używany jest głównie do pobierania XSLT wykorzystujących protokół https://.
*/

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <bmd/common/bmd-sds.h>
#include <bmd/libbmdpki/libbmdpki.h>
#include <bmd/libbmdutils/libbmdutils.h>


#include <bmd/libbmdxades/xmlio.h>
#include <bmd/libbmderr/libbmderr.h>

/**
Sprawdzanie, czy podany plik może zostać pobrany z wykorzystaniem niniejszego mechanizmu.

@param filename nazwa pliku do ściągnięcia

@retval 0 ten typ pliku nie jest obsługiwany
@retval 1 ten typ pliku jest obsługiwany
*/


int bmdxml_IO_curl_Match(const char * filename)
{
	int len = strlen(filename);
	char * new_filename = malloc((len+1)*sizeof(char));
	char * pos = new_filename;
	int status = 0 ;

	PRINT_VDEBUG("%s(%s)\n",__FUNCTION__,filename);


	while (*filename != '\0') {
		*pos = tolower(*filename);
		filename ++; pos++;
	}
	*pos = '\0';

	if ( 	strncmp("https://",new_filename,8) == 0 ||
		strncmp("http://",new_filename,7) == 0 ||
		strncmp("ftp://",new_filename,6) == 0	)
	{
		status = 1;
	} else status = 0;

	free(new_filename);
	return status;
}

typedef struct __bmdxml_IO_curl_st {
	int position;
	GenBuf_t * buffer;
} bmdxml_io_curl;


/**
Otwarcie pliku z wykorzystaniem adresu podanego w argumencie.

@param filename nazwa pliku do otwarcia
@return struktura reprezentująca otwarty plik lub NULL w wypadku błędu
*/

void * bmdxml_IO_curl_Open(const char * filename)
{
	bmdxml_io_curl * curl = malloc( sizeof(bmdxml_io_curl) );

	PRINT_VDEBUG("%s(%s)\n",__FUNCTION__,filename);

	curl->buffer = get_from_url((char*)filename);

	curl->position = 0;

	if (curl->buffer == NULL) {
		free(curl);
		PRINT_DEBUG("Nie można połączyć się z adresem %s\n",filename);
		return NULL;
	}

	return curl;
}

/**
Pobranie danych z połączenia

@param context handler połączenia
@param buffer  bufor na dane
@param len     wielkość bufora

@return ilość bajtów zapisanych w buforze
*/



int bmdxml_IO_curl_Read(void * context, char * buffer, int len)
{
	bmdxml_io_curl * curl = (bmdxml_io_curl *) context;

	int nbytes = curl->buffer->size - curl->position;

	PRINT_VDEBUG("%s(%i)\n",__FUNCTION__,len);
	if (nbytes < len) {
		len = nbytes;
	}

	memcpy(buffer, curl->buffer->buf + curl->position, len);
	PRINT_VDEBUG("Read %i bytes\n",len);
	curl->position += len;
	return len;
}

/** Zamkniecie polaączenia i zwolnienie kontekstu
@param context Kontekst reprezentujący dane połączenie
@return 0 on success

*/

int bmdxml_IO_curl_Close(void * context)
{
	bmdxml_io_curl * curl = context;
	if (!(curl != NULL))
	{
		PRINT_ERROR("curl nie może być NULL!(LK)\n");
		abort();
	}

	PRINT_VDEBUG(__FUNCTION__);

	free_gen_buf(&(curl->buffer));
	free(curl);
	return 0;
}


