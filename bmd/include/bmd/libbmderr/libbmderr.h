#ifndef _LIBBMDERR_INCLUDED_
#define _LIBBMDERR_INCLUDED_

#include <assert.h>
#include <bmd/common/bmd-common.h>
#include <bmd/libbmdglobals/libbmdglobals.h>
#include <time.h>
#ifdef WIN32
#include <sys/timeb.h>
#endif

#ifdef WIN32
	#ifdef LIBBMDERR_EXPORTS
		#define LIBBMDERR_SCOPE __declspec(dllexport)
	#else
		#ifdef _USRDLL
			#define LIBBMDERR_SCOPE __declspec(dllimport)
		#else
			#define LIBBMDERR_SCOPE
		#endif
	#endif
#else
	#define LIBBMDERR_SCOPE
#endif	/* ifdef WIN32 */

#ifdef WIN32
	#define LIBBMDERR_IMPORT __declspec(dllimport)
#else
	#define LIBBMDERR_IMPORT extern
#endif /* ifdef WIN32 */

#ifdef WIN32
//	struct server_configuration; //predeclaration (by WSZ)
//	LIBBMDERR_SCOPE struct server_configuration *_GLOBAL_bmd_configuration;
#endif

#if defined(__cplusplus)
extern "C" {
#endif

/*extern long _GLOBAL_debug_level;*/

/* Klasy bledow */
#define ERR_sys 		1000   /**< bledy systemowe, np. pamieci, pliki itd. */
#define ERR_arg    		2000   /**< bledny argument wywolania funkcji */
#define ERR_net    		4000   /**< blad podsystemu sieciowego */
#define ERR_parser 		5000   /**< blad konwersji XML, ASN1 itd */
#define ERR_other       31000  /**< Pozostale bledy  */

#define free0(ptr) { free(ptr); ptr = NULL; }
#define af "Masta Blasta Af1n"

/** Makro konwertujące strig zawierający opis błędu na jego kod.
	Kod błędu to ujemna liczba zbudowana według schematu:
	klasa + (str2int(opis) % 1000)
	@param string opis błędu
	@param who kto odpowiada za błąd?
	@author LK
*/
#ifndef WIN32
#define ERR(type,string,who) -(type + bmd_str2int(string))
#else
#define ERR(type,string,who) -(type + (int)bmd_str2int(string))+(0*printf("ERR %s\n",string));
#endif

char *GetErrorStr(long err_code);
char *GetErrorMsg(long error_code);
long GetErrorWinNum(long error_code);
uint32_t bmd_str2int (char * str);

/** definicja funkcji asprintf - normalnie obecnej w *BSD oraz w Linux **/
#if !defined(__linux__) && !defined( __FreeBSD__ ) && !defined(__bsdi__ ) && !defined( __OpenBSD__ ) && !defined( __NetBSD__ )
int asprintf(char **ptr, const char *fmt, ...);
int portable_vsnprintf(char *str, size_t str_m, const char *fmt,va_list ap);
#endif

#ifdef WIN32
int bmd_err_set_win32_global_log_file(const char *log_file);
long bmd_err_win32_log_enabled(void);
FILE* bmd_err_win32_get_log_file(void);
LIBBMDERR_SCOPE char* dlerror(void);
#endif

/**
Asercja. Makro zwróci błąd, jeśli dana asercja nie jest spełniona.
Jeśli dodatkowo nie zdefiniowaliśmy NDEBUG, zostanie wywołane abort().

@param COND warunek do sprawdzenia
@param who inicjały osoby odpowiedzialnej za tą asercję

@author LK
*/
#define bmd_assert(COND,who) { \
    if (! (COND)) { \
	PRINT_DEBUG("Assertion failed: function %s() in %s:%i (%s)\n", \
			__FUNCTION__, __FILE__,__LINE__, who);\
	assert(COND); \
	return ERR(ERR_other, "Assertion failed", who); \
    } \
}
/** To samo co bmd_assert ale nic nie zwraca */

#define bmd_assert_void(COND,who) { \
    if (! (COND)) { \
	PRINT_DEBUG("Assertion failed: function %s() in %s:%i (%s)\n", \
			__FUNCTION__, __FILE__,__LINE__, who);\
	assert(COND); \
    } \
}

/** the same as bmd_assert() but returns NULL */

#define bmd_assert_ptr(COND,who) { \
    if (! (COND)) { \
	PRINT_DEBUG("Assertion failed: function %s() in %s:%i (%s)\n", \
			__FUNCTION__, __FILE__, __LINE__, who);\
	assert(COND); \
	return 0; \
    } \
}

/*prawidlowy format debug potrzebny Pawlowi*/
/*********  makro definiujace wypisywanie na ekran komunikatow debug ******/
#ifndef WIN32
void ___debug_printtime(void);
#define PRINTLOG(fmt,args...) \
{\
	___debug_printtime();\
	printf("   [%s:%i:%s][%i]\t",__FILE__,__LINE__,__FUNCTION__,getpid()); \
	printf(fmt,##args);\
	fflush(stdout); \
}
#define PRINT_DEBUG(fmt,args...) if (_GLOBAL_debug_level >= 7) {printf("[L07] "); PRINTLOG(fmt,##args);}
#define PRINT_VDEBUG(fmt,args...) if (_GLOBAL_debug_level >= 11) {printf("[L11] "); PRINTLOG(fmt,##args);}

/*************************************************************************************************************/
/*
	PRINT_PANIC 	- poziom 0 - obsłużone segfault'y - błędy, pzr yktórych napweno pojawi się segfault
	PRINT_FATAL 	- poziom 1 - błędy powodujące wyłączenie serwera
	PRINT_LOG 	- poziom 2 - informaje o połaczeniu, typie ządania - właściwym obsłużeniu żądania
	PRINT_ERROR 	- poziom 3 - błędy
	PRINT_WARNING	- poziom 4 - ostrzeżenia - nieprawidłowe wartości parametrów
	PRINT_INFO	- poziom 5 - informacje o każdej z uruchamianych funkcji (wstawiać na początku każdej funkcji)
	PRINT_NOTICE	- poziom 6 - poszczególne etapy obsługi żadania
	PRINT_DEBUG1	- poziom 7 - rownoznaczny z PRINT_DEBUG
	PRINT_DEBUG2	- poziom 8 -
	PRINT_DEBUG3	- poziom 9 -
	PRINT_DEBUG4	- poziom 10 -log w dowolnym miejscu kodu (np. co drugą linię kodu) w miarę możliwości usuwane
	PRINT_DEBUG5	- poziom 11 - rownoznaczny z PRINT_VDEBUG
*/
/******************************** formaty debug obslugujace rózne poziomy debuga *****************************/

	#define PRINT_PANIC(fmt,args...) if (_GLOBAL_debug_level >= 0) {printf("[L00] "); PRINTLOG(fmt,##args);}
	#define PRINT_FATAL(fmt,args...) if (_GLOBAL_debug_level >= 1) {printf("[L01] "); PRINTLOG(fmt,##args);}
	#define PRINT_LOG(fmt,args...) if (_GLOBAL_debug_level >= 2) {printf("[L02] "); PRINTLOG(fmt,##args);}
	#define PRINT_ERROR(fmt,args...) if ((_GLOBAL_debug_level >= 3) || (_GLOBAL_debug_level == -4) || (_GLOBAL_debug_level == -5)) {printf("[L03] "); PRINTLOG(fmt,##args);}
	#define PRINT_WARNING(fmt,args...) if (_GLOBAL_debug_level >= 4) {printf("[L04] "); PRINTLOG(fmt,##args);}
	#define PRINT_INFO(fmt,args...) if (_GLOBAL_debug_level >= 5) {printf("[L05] "); PRINTLOG(fmt,##args);}
	#define PRINT_NOTICE(fmt,args...) if (_GLOBAL_debug_level >= 6) {printf("[L06] "); PRINTLOG(fmt,##args);}
	#define PRINT_DEBUG1(fmt,args...) if (_GLOBAL_debug_level >= 7) {printf("[L07] "); PRINTLOG(fmt,##args);}
	#define PRINT_DEBUG2(fmt,args...) if (_GLOBAL_debug_level >= 8) {printf("[L08] "); PRINTLOG(fmt,##args);}
	#define PRINT_DEBUG3(fmt,args...) if (_GLOBAL_debug_level >= 9) {printf("[L09] "); PRINTLOG(fmt,##args);}
	#define PRINT_DEBUG4(fmt,args...) if (_GLOBAL_debug_level >= 10) {printf("[L10] "); PRINTLOG(fmt,##args);}
	#define PRINT_DEBUG5(fmt,args...) if (_GLOBAL_debug_level >= 11) {printf("[L11] "); PRINTLOG(fmt,##args);}
	#define PRINT_TEST(fmt,args...) if (_GLOBAL_debug_level <= -4) {printf("[L-4] "); PRINTLOG(fmt,##args);}
	#define PRINT_GK(fmt,args...) {}
#else
	#define PRINT_ERROR(fmt,...) if( bmd_err_win32_log_enabled() == 1 ) \
									 {\
										time_t t; \
										char sec_ctime[256];\
										struct _timeb tb;\
										memset(sec_ctime,0,256); \
										_ftime(&tb); \
										t=time(NULL); \
										ctime_s(sec_ctime,256,&t); \
										sec_ctime[strlen(sec_ctime)-1]='\0'; \
										fprintf(bmd_err_win32_get_log_file(),"[%li.%li %s %li][%s:%i:%s] : ", \
													   (long)tb.time,(long)tb.millitm,\
													   sec_ctime, \
													   (long)GetCurrentThreadId(), \
													   __FILE__,__LINE__,__FUNCTION__); \
										fprintf(bmd_err_win32_get_log_file(),fmt,__VA_ARGS__);\
										fflush(bmd_err_win32_get_log_file());\
									}
	#define PRINT_NULL(fmt,...)	  {}
	#define PRINT_ERROR1  PRINT_NULL
	#define PRINT_ERROR2  PRINT_NULL
	#define PRINT_ERROR3  PRINT_NULL
	#define PRINT_ERROR4  PRINT_NULL
	#define PRINT_ERROR5  PRINT_NULL
	#define PRINT_DEBUG   PRINT_NULL
	#define PRINT_VDEBUG  PRINT_NULL
	#define PRINT_PANIC   PRINT_NULL
	#define PRINT_TEST    PRINT_NULL
	#define PRINT_FATAL   PRINT_NULL
	#define PRINT_LOG     PRINT_NULL
	#define PRINT_NOTICE  PRINT_NULL
	#define PRINT_WARNING PRINT_NULL
	#define PRINT_INFO    PRINT_NULL
	#define PRINT_GK(fmt,...)	 { PRINT_ERROR(fmt, __VA_ARGS__) }
#endif

/* makro zastepujace pod windows funkcje strndup funkcja strdup
UWAGA
	nie nalezy kozystac z tego makro jesli SIZE jest inny niz sizeof(SRC)
*/
#ifndef WIN32
	#define STRNDUP(SRC,SIZE) strndup(SRC,SIZE)
#else
	#define STRNDUP(SRC,SIZE) strdup(SRC)
#endif

#if defined(__cplusplus)
}
#endif
#endif

