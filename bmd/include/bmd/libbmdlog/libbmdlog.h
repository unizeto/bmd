/*************************************************
 *                                               *
 * Interfejs dla systemu generowania podpisanych *
 * logów w BMD                                   *
 *                                               *
 * Data : 23-08-2007                             *
 * Autor : Unizeto Technologies SA               *
 *                                               *
 *************************************************/

#ifndef __LIBBMD_LOG__
#define __LIBBMD_LOG__

#include <time.h>
#include <string.h>
#include <bmd/libbmdnet/libbmdnet.h>
#include <bmd/libbmdlog/err_code.h>
#include <bmd/libbmdconfig/libbmdconfig.h>
#include <bmd/libbmdasn1_common/BMDLogElements.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>
#include <bmd/libbmderr/err_stack.h>

 /* potwierdzenie zachowaia logu */
 #define LOG_SAVED 1

 /* początek parametrów podstawowych */
 #define PRIM_PARAM_START 3

 /* koniec parametrów podstawowych */
 #define PRIM_PARAM_END 10

 /* koniec parametrów opcjonalnych */
 #define ADD_PARAM_END 26 

 /* ilość transmitowanych elementów  */
 /* wpisy opcjonalne + wpisy podstawowe + zdalny host i port  */
 
 #define ASN_ELEMENTS_COUNT ADD_PARAM_END


#define free1( object, start, end ) \
	{ \
		long i; \
		for((i)=(start); (i)<(end); (i)++) \
			{ \
				free( (object)[i] ); \
				(object)[i] = NULL; \
			} \
	}

#define free2( object ) \
	{ \
		long i; \
		for((i)=0; object[i]!=NULL ; (i)++) \
			{ \
				free( (object)[i] ); \
				(object)[i] = NULL; \
			} \
	}


 /* ++++++++++ STRUKTURA ZAWIERAJĄCA INFO O SERWERZE LOGÓW ++++++++++++++ */


 typedef struct
  {

    /* ------------------ */

       char*          ip;     /* IP serwera logów  */
       char*        port;     /* PORT tego servera */
       char*     attempt;     /* liczba prób połączenia */
       char*       delay;     /* opóźnienie pomiędzy próbami połączenia */
 
    /* ------------------ */

  } logserv_t;


 /* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */



 /* +++++++++++++++++++ DEFINICJA STRUKTURY LOGU ++++++++++++++++++++++++  */


 typedef struct
  {
  
       long*                          id;
    /* ---------------------------------- */
    /* Wpisy permanentne nie
       transmitowane                      */

       char*                 remote_host;   /*  1 */
       char*                 remote_port;   /*  2 */
  
    /* ++++++++++++++++++++++++++++++++++ */
    /* Wpisy transmitowane                */
    /* ++++++++++++++++++++++++++++++++++ */
    /* ---------------------------------- */
    /* Wpisy podstawowe */
  
       char*                    src_file;   /*  3 */
       char*                    src_line;   /*  4 */
       char*                src_function;   /*  5 */
       char*               date_time_end;   /*  6 */
       char*                  remote_pid;   /*  7 */
       char*                   log_owner;   /*  8 */
       char*                     service;   /*  9 */
       char*              operation_type;   /* 10 */

    /* ---------------------------------- */
    /* Wpisy opcjonalne */

       char*                   log_level;   /* 11 */
       char*             date_time_begin;   /* 12 */
       char*                  log_string;   /* 13 */
       char*                  log_reason;   /* 14 */
       char*                log_solution;   /* 15 */
       char*               document_size;   /* 16 */
       char*           document_filename;   /* 17 */
       char*		     log_referto;   /* 18 */
       char*		      event_code;   /* 19 */
       char*	 log_owner_event_visible;   /* 20 */
       char*   log_referto_event_visible;   /* 21 */
       char*                  user_data1;   /* 22 */
       char*                  user_data2;   /* 23 */
       char*                  user_data3;   /* 24 */
       char*                  user_data4;   /* 25 */
       char*                  user_data5;   /* 26 */

    /* ---------------------------------- */
    /* ++++++++++++++++++++++++++++++++++ */
  
       char*                  hash_value;
       void*               dbase_handler;
       logserv_t**          serversTable;
       char***              logPolicyTab;
       bmdnet_handler_ptr serwer_handler;

    /* ---------------------------------- */
  
  } log_t;


 /* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

    typedef enum 
     { 
       _id, date_time_begin, date_time_end, service, \
       log_owner, log_level, operation_type, log_string, \
       log_reason, log_solution, src_file, src_line, \
       src_function, remote_host, remote_port, remote_pid, \
       document_filename, document_size, hash_value, \
       log_referto, event_code, log_owner_event_visible, \
       log_referto_event_visible, user_data1, user_data2, \
       user_data3, user_data4, user_data5
     } param_t;

 /* --------------------------------------------------------- */

    typedef enum { branch, subtree } depth_t;

 /* --------------------------------------------------------- */

    typedef enum 
     { 
       nowhere, clevel1, clevel2, clevel3, clevel4, \
       clevel5, clevel6, clevel7, clevel8, clevel9, \
       clevel10, clevel11, clevel12, clevel13
     } cLevel_t; 

 /* --------------------------------------------------------- */

    typedef enum { prim, _add, all } option_t;
 
 /* --------------------------------------------------------- */

/* Funkcje pomocnicze dla weryfikatora logów */

long setVerificationStatus(void* const, void (*setStatus)(void* const, long, const long, const long, char const * const ),\
			    long, const long, const long, char const * const );
void setLogStatus( void* const, long, const long, const long , char const * const );

/* ----------------------------------------------------------------------------------------------------- */


/* ----------------------------------------------------------------------------------------------------- */
/* API dla klienta serwera logów */

 /* Zainicjowanie mechanizmu logowania - połączenie z serwerem */
 err_t LogInit ( const char* const, log_t ** const );

 /* Finalizacja procesu logowania - zamknięcie połączenia */
 err_t LogEnd ( log_t ** const );

 /* Pobranie parametrów generowanych automatycznie */
 err_t LogCreate ( log_t * const );

 /* Wysłanie logu do serwera */
 err_t LogCommit ( log_t * const );

 /* Ustawianie dodatkowych parametrów */
 err_t LogSetParam ( const param_t, const char * const, log_t * const );

/* ----------------------------------------------------------------------------------------------------- */
/* API dla weryfikatora drzewa Merkla */

 /* Weryfikacja zawartości logu */
 long bmdLogVerifyLogNode( void* const, long const );

 /* Weryfikacja węzła skrótu na najniższym poziomie drzewa Merkla */
 long bmdLogVerifyHashNodeLevel0( void* const, long const, long* const );

 /* Weryfikacja węzłów skrótów na pozostałych poziomach drzewa Merkla */
 long bmdLogVerifyHashNodeHigherLevel( void* const, long, long* const );

 /* Audyt struktury logicznej drzewa Merkla + (pełna/częściowa) weryfikacja zadanego poddrzewa */
 long bmdLogCheckSubtree( void* const, long long, depth_t );

 
/* ----------------------------------------------------------------------------------------------------- */


 
#endif
