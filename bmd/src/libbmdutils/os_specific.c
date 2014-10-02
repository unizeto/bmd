#include <bmd/libbmdutils/libbmdutils.h> 
#include <bmd/libbmderr/libbmderr.h> 


#ifndef _WIN32
	#include <sys/stat.h>
	#include <stdlib.h>
	#include <unistd.h>
#else
#pragma warning(disable:4100)
#endif 


#ifndef HAVE_SETENV
#define HAVE_SETENV      

/**
 * Funkcja tworzy nowa zmienna srodowiskowa lub modyfikuje zawartosc 
 * juz zdefiniowanej zmiennej.

 * @param[in] _label - nazwa zmiennej srodowiskowej.
 * @param[in] _value - wartosc zmiennej srodowiskowej.

 * @note funkcja posiada niewykorzystywany parametr _modify.

 * @return status zakonczenia operacji.
 * @retval 0 - pomyslne zakonczenie.
*/

long setenv(const char *_label, const char *_value, long _modify)
        { 
		// brak sprawdzenia poprawności argumentów
                char *_pBuf = (char *)malloc(strlen((const char *)_label)+strlen((const char *)_value)+2); 
                memset(_pBuf,0,strlen((const char *)_value)+1); 
                sprintf(_pBuf, "%s=%s", (const char *)_label, (const char *)_value); 
                _putenv(_pBuf); 
                free(_pBuf);

		return 0; 
        }

/**
 * Funkcja zwalnia wartosc przechowywana w zmiennej srodowiskowej.

 * @param[in,out] _label - nazwa zmiennej srodowiskowej.

 * @return status zakonczenia operacji.
 * @retval 0 - pomyslne zakonczenie.
*/

long unsetenv(const char *  _label ) 
        { 
		//brak sprawdzenia poprawności argumentów
                char *_pBuf = (char *)malloc(strlen((const char *)_label)+2); 
                sprintf(_pBuf, "%s=", (const char *)_label); 
                _putenv(_pBuf); 
                free(_pBuf);

		return 0; 
        }
#endif


#ifndef _WIN32

/**
 * Funkcja sprawdza, czy plik lub katalog istnieje. 
 
 * @param[in] file - nazwa pliku.
 
 * @return status zakonczenia operacji.
 * @retval -1 - plik lub katalog nie istnieje.
 * @retval 0 - plik lub katalog istnieje.
 */

long file_not_exists(char * file)
{
	struct stat buf;

	if (!(file != NULL))
	{
		PRINT_ERROR("Podano NULL zamiast nazwy pliku (LK)\n");
		abort();
	}
	
	return stat (file, &buf);
}

#else 

long file_not_exists(char * file) 
{
	return 0;
}

#endif


#ifdef WIN32

int strcasecmp(const char *s1, const char *s2)
{
	return _stricmp(s1, s2);
}


int strncasecmp(const char *s1, const char *s2, size_t n)
{
	return _strnicmp(s1, s2, n);
}

#endif /*ifdef WIN32*/
