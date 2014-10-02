#ifndef __ROOT_SERVER__
#define __ROOT_SERVER__


#include <bmd/libbmdlog/balance/atd.h>
#include <bmd/libbmdlog/libbmdlog.h>

  /* ------------------------------------ */
  /* Informacje o serwerach logów         */
  /* druga warstwa                        */
 
     typedef struct 
      {
        /* ------------------------------- */
    
           char*                   servID;    /* identyfikator serwera II-layer */
           logserv_t          serverParam;    /* parametry serwera */
           ATD_LOAD_NODE_PTR  loadNodePtr;    /* wskaźnik do węzła zawierającego */
                                              /* obciązenie dla danego serwera */
       /*  ------------------------------- */
      } logServParam_t;

  /* ------------------------------------ */


  /* ------------------------------------------------------- */
  /* Ustawienia głównego serwera logów                       */

     typedef struct rootServParam
      {
        /* ---------------------------------------------------- */

	   char*                                            ip;    /* IP głównego serwera logów I - layer */
           char*                                          port;    /* wiadomo */
           char*                             log_default_level;    /* poziom domyślny logu, pole wycofane */
           char*                           log_accepting_level;    /* akceptowany poziom logu, pole wycofane */
           char*                                temp_directory;    /* lokalizacja katalogu tymczasowego */
           char*                                      confFile;    /* ścieżka do pliku konfiguracyjnego */
           char***                                logPolicyTab;    /* tablica określająca, które zdarzenia
                                                                      mają być rejestrowane */
           long   (*balanceFunc)( struct rootServParam* const, \
	                             logServParam_t*** const );    /* wskźnik do funkcji balancingu */
           logServParam_t**                      LogServersTab;    /* tablica dostępnych serwerów */
           ATD                                        LoadList;    /* lista obciążenia serwerów */
           ATD                                  ConnectionList;    /* lista połączeń */

        /* ---------------------------------------------------- */
      } rootServParam_t;

  /* ----------------------------------------------------------------- */

     typedef enum 
      { 
    	up_server, change_params 
      } paramCase_t;

  /* ----------------------------------------------------------------- */

     typedef enum
      {
        ignore, collect
      } logaction_t;

  /* ------------------------------------------------------------------ */

     err_t getRootServerParams( const char* const, const paramCase_t, \
                                rootServParam_t* const );
     err_t rootServerParamsInit( rootServParam_t* const );

  /* ------------------------------------------------------------------ */
  

#endif

