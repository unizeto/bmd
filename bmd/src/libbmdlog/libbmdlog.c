/***************************************************************
 *                        libbmdlog.c                          *
 * Biblioteka dla generowania logów w BMD                      *
 * Wykorzystywana po stronie klienta                           *
 *                                                             *
 * Sr 18 Lipiec 2007 15:45                                     *
 *                                                             *
 * Autor : Unizeto Technologies SA.                            *
 *                                                             *
 ***************************************************************/

#include <signal.h>
#include <sys/time.h>
#include <bmd/libbmdlog/libbmdlog.h>
#include <bmd/libbmdlog/client.dic>
#include <bmd/libbmdlog/log_servers.dic>
#include <bmd/libbmdlog/messages.res>
#include <bmd/libbmdlog/bmdlogserver/database.dic>
#include <bmd/libbmdlog/log_server_utils/log_server_utils.h>
#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmddb/libbmddb.h>

/**
 * Funkcja służy do zainicjowania struktury logów :
 * połączenia z serwerem logów, którego adres i numer
 * portu pobierany jest z pliku konfiguracyjnego
 * przekazywanego jako parametr funkcji
 * 
 * @param[in, out] log_tLog - adres struktury log_t
 * @return status zakończenia operacji
*/

/* ----------------------------------------------------------- */


err_t LogInit( const char* const confFile, log_t** const log_tLog )
 {
 
 
  /* --------------------------------------------------- */
 
     long		intRet                =       0;
     long		intSize               =       0;
     long		i                     =       0;
     long		intIndex              =       0;
     char*		stringKomunikat       =    NULL;
     char*		stringLogServers      =    NULL;
     char**		stringLogServersTable =    NULL;
     char*		dbIP                  =    NULL;
     char*		dbPORT                =    NULL;
     char*		dbNAME                =    NULL;
     char*		dbUSER                =    NULL;
     char*		dbPASSWD              =    NULL;
     char*		dbLIBRARY             =    NULL;
     void*		dbHANDLER             =	   NULL;
     bmd_conf*		bmd_confHandle        =    NULL;
     err_t		err_tRet              =       0;
     cLevel_t		cLevel_tWHERE         = nowhere;
     struct sigaction			sigactionStruct;

  /* --------------------------------------------------- */

 
   if ( confFile == NULL )
    {
        BMD_SETERR( err_tRet, BMD_ERR_PARAM1 );
        BMD_SETIERR( err_tRet, AD, LIBBMDLOG_INFO1 );

        return err_tRet;
    }

   if ( log_tLog == NULL || *log_tLog != NULL )
    {
        BMD_SETERR( err_tRet, BMD_ERR_PARAM2 );
        BMD_SETIERR( err_tRet, AD, LIBBMDLOG_INFO2 );

        return err_tRet;
    } 

   *log_tLog = (log_t*)calloc(1, sizeof(log_t));

   if ( *log_tLog == NULL )
    {
        BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
        BMD_SETIERR( err_tRet, AD, LIBBMDLOG_ERR1 );

        return err_tRet;
    }

   cLevel_tWHERE = clevel1;  

   /*  *************************************************  LEVEL 1  ******************************************** */

   sigactionStruct.sa_handler = SIG_IGN;

   intRet = sigaction( SIGPIPE, &sigactionStruct, NULL );

   if ( intRet < 0 )
    {
        BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
        BMD_SETIERR( err_tRet, AD, LIBBMDLOG_ERR2 );

        goto GARBAGE;
    }

   intRet = bmdconf_init( &bmd_confHandle );

   if ( intRet < 0 )
    {
        BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
        BMD_SETIERR( err_tRet, AD, LIBBMDLOG_ERR3 );

        goto GARBAGE;
    }

   cLevel_tWHERE = clevel2; 

   /*  *************************************************  LEVEL 2  ******************************************** */

   intRet = bmdconf_set_mode( bmd_confHandle, BMDCONF_TOLERANT );

   if ( intRet < 0 )
    {
        BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
        BMD_SETIERR( err_tRet, AD, LIBBMDLOG_ERR4 );

      goto GARBAGE;
    }

   intRet = bmdconf_add_section2dict( bmd_confHandle, "CLIENT", client_dic, BMDCONF_SECTION_REQ );

   if ( intRet < 0 )
    {
        BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
        BMD_SETIERR( err_tRet, AD, LIBBMDLOG_ERR5 );

        goto GARBAGE;
    }

   intRet = bmdconf_add_section2dict( bmd_confHandle, "DATA_BASE", database_dic, BMDCONF_SECTION_REQ );

   if ( intRet < 0 )
    {
        BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
        BMD_SETIERR( err_tRet, AD, LIBBMDLOG_ERR5 );

        goto GARBAGE;
    }

   intRet = bmdconf_load_file( confFile, bmd_confHandle, &stringKomunikat );

   if ( intRet < 0 )
    {
        BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
        BMD_SETIERR( err_tRet, AD, stringKomunikat );

        goto GARBAGE;
    }

   free0( stringKomunikat );

   intRet = bmdconf_get_value( bmd_confHandle, "CLIENT", "LOG_SERVERS", &stringLogServers );

   if ( intRet < 0 )
    {
        BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
        BMD_SETIERR( err_tRet, AD, LIBBMDLOG_ERR6 );

        goto GARBAGE;
    }

   cLevel_tWHERE = clevel3;

   /*  *************************************************  LEVEL 3  ******************************************** */

   intRet = bmdconf_get_value( bmd_confHandle, "DATA_BASE", "IP", &dbIP );

   if ( intRet < 0 )
    {
        BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
        BMD_SETIERR( err_tRet, AD, LIBBMDLOG_ERR18 );

        goto GARBAGE;
		}

   intRet = bmdconf_get_value( bmd_confHandle, "DATA_BASE", "PORT", &dbPORT );

   if ( intRet < 0 )
    {
        BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
        BMD_SETIERR( err_tRet, AD, LIBBMDLOG_ERR19 );

        goto GARBAGE;
    }

   intRet = bmdconf_get_value( bmd_confHandle, "DATA_BASE", "BASE_NAME", &dbNAME );

   if ( intRet < 0 )
    {
        BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
        BMD_SETIERR( err_tRet, AD, LIBBMDLOG_ERR20 );

        goto GARBAGE;
    }

   intRet = bmdconf_get_value( bmd_confHandle, "DATA_BASE", "USER", &dbUSER );

   if ( intRet < 0 )
    {
        BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
        BMD_SETIERR( err_tRet, AD, LIBBMDLOG_ERR21 );

        goto GARBAGE;
    }

   intRet = bmdconf_get_value( bmd_confHandle, "DATA_BASE", "PASSWORD", &dbPASSWD );

   if ( intRet < 0 )
    {
        BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
        BMD_SETIERR( err_tRet, AD, LIBBMDLOG_ERR22 );

        goto GARBAGE;
    }

   intRet = bmdconf_get_value( bmd_confHandle, "DATA_BASE", "DB_LIBRARY", &dbLIBRARY );

   if ( intRet < 0 )
    {
	asprintf(&dbLIBRARY, "libbmddb_postgres.so");
    }

   intRet = bmd_db_init(dbLIBRARY);

   if ( intRet != BMD_OK )
    {
	BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
	BMD_SETIERR( err_tRet, AD, LIBBMDLOG_ERR24 );
	
	goto GARBAGE;                                
    }

   intRet = bmd_db_connect2( dbIP, dbPORT, dbNAME, dbUSER, dbPASSWD, &dbHANDLER );

   if ( intRet < 0 )
    {
        BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
        BMD_SETIERR( err_tRet, AD, LIBBMDLOG_ERR23 );

        goto GARBAGE;
    }

    /* Wywołanie funkcji odczytującej politykę logów z bazy danych  */

    err_tRet = getEventsPolicy( dbHANDLER , &((*log_tLog)->logPolicyTab) );

    bmd_db_disconnect( &dbHANDLER );

    if ( BMD_ERR( err_tRet ) )
     {
        goto GARBAGE;
     }

   err_tRet = GetSubstring( stringLogServers, "[a-zA-Z0-9_]+", &stringLogServersTable );

   if ( BMD_ERR( err_tRet ) )
    {
        BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
        BMD_SETIERR( err_tRet, AD, LIBBMDLOG_ERR8 );

        goto GARBAGE;
    }

   free0( stringLogServers );

   cLevel_tWHERE = clevel4;

   /*  *************************************************  LEVEL 4  ******************************************** */

   for( i=0, intSize=0; stringLogServersTable[i] != NULL; i++, intSize++);

   (*log_tLog)->serversTable = ( logserv_t**)calloc( intSize+1, sizeof( logserv_t* ));

   if ( ((*log_tLog)->serversTable) == NULL )
    {
        BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
        BMD_SETIERR( err_tRet, AD, LIBBMDLOG_ERR1 );

        goto GARBAGE;
    }

   cLevel_tWHERE = clevel5;

   /*  *************************************************  LEVEL 5  ******************************************** */

   for( i=0; stringLogServersTable[i] != NULL; i++ )
    {
      intRet = bmdconf_add_section2dict( bmd_confHandle, stringLogServersTable[i], \
                                         log_servers_dic, BMDCONF_SECTION_REQ );

      if ( intRet < 0 )
       {
          BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
          BMD_SETIERR( err_tRet, AD, LIBBMDLOG_ERR5 );

          goto GARBAGE;
       }

      intRet = bmdconf_load_file( confFile, bmd_confHandle, &stringKomunikat );

      if ( intRet < 0 )
       {
          BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
          BMD_SETIERR( err_tRet, AD, stringKomunikat );

          goto GARBAGE;
       }

      free0( stringKomunikat );

      ((*log_tLog)->serversTable)[i] = ( logserv_t*)calloc( 1, sizeof( logserv_t ));

      if ( ((*log_tLog)->serversTable)[i] == NULL )
       {
          BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
          BMD_SETIERR( err_tRet, AD, LIBBMDLOG_ERR1 );

          goto GARBAGE;
       }

      intRet = bmdconf_get_value( bmd_confHandle, stringLogServersTable[i], "IP", \
                                  &((((*log_tLog)->serversTable)[i])->ip) );

      if ( intRet < 0 )
       {
          BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
          BMD_SETIERR( err_tRet, AD, LIBBMDLOG_ERR10 );

          goto GARBAGE;
       }


      intRet = bmdconf_get_value( bmd_confHandle, stringLogServersTable[i], "PORT", \
                                  &((((*log_tLog)->serversTable)[i])->port) );

      if ( intRet < 0 )
       {
          BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
          BMD_SETIERR( err_tRet, AD, LIBBMDLOG_ERR11 );

          goto GARBAGE;
       }

      intRet = bmdconf_get_value( bmd_confHandle, stringLogServersTable[i], "ATTEMPT", \
                                  &((((*log_tLog)->serversTable)[i])->attempt) );

      if ( intRet < 0 )
       {
          BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
          BMD_SETIERR( err_tRet, AD, LIBBMDLOG_ERR12 );

          goto GARBAGE;
       }

      intRet = bmdconf_get_value( bmd_confHandle, stringLogServersTable[i], "DELAY", \
	                            &((((*log_tLog)->serversTable)[i])->delay) );

      if ( intRet < 0 )
       {
          BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
          BMD_SETIERR( err_tRet, AD, LIBBMDLOG_ERR13 );

          goto GARBAGE;
       }

    }

   /*  Gdy wykorzystujemy serwer logów dla BMD to bmdnet_init trzeba wykomentować */

   /*
   intRet = bmdnet_init();

   if ( intRet < 0 )
    {
        BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
        BMD_SETIERR( err_tRet, AD, LIBBMDLOG_ERR14 );

	goto GARBAGE;
    }

    */


   cLevel_tWHERE = clevel6;

   /*  *************************************************  LEVEL 6  ******************************************** */


   err_tRet = LogConnect( &((*log_tLog)->serwer_handler), Connect2RootServer, \
                          (void**)((*log_tLog)->serversTable), &intIndex );

   if ( BMD_ERR( err_tRet ) )
    {
        BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
        BMD_SETIERR( err_tRet, AD, LIBBMDLOG_INFO4 );

	      goto GARBAGE;
    }

   cLevel_tWHERE = nowhere;

   /*  *************************************************  LEVEL 0  ******************************************** */

   for( i=0; stringLogServersTable[i] != NULL; i++ )
    {
      free0( stringLogServersTable[i] );
    }
   free0( stringLogServersTable ); 

   intRet = bmdconf_destroy( &bmd_confHandle );



 /* ----------------------------------------------------------------------------- */

 /* CZYSCICIEL */

 GARBAGE :

  switch( cLevel_tWHERE )
   {
    case clevel6  : { /* bmdnet_destroy();*/ }
    case clevel5  : {
                      for( i=0; ((*log_tLog)->serversTable)[i] != NULL; i++ )
                       {
                          free0( (((*log_tLog)->serversTable)[i])->ip );
                          free0( (((*log_tLog)->serversTable)[i])->port );
                          free0( (((*log_tLog)->serversTable)[i])->attempt );
                          free0( (((*log_tLog)->serversTable)[i])->delay );
                          free0( ((*log_tLog)->serversTable)[i] );
                       }
                      free0( (*log_tLog)->serversTable );
                    }
    case clevel4  : {
                      for( i=0; stringLogServersTable[i] != NULL; i++ )
                       {
                          free0( stringLogServersTable[i] );
                       }
                      free0( stringLogServersTable );
                    }
    case clevel3  : {
			free0( stringLogServers );
			free0( dbIP );
			free0( dbPORT );
			free0( dbNAME );
			free0( dbUSER );
			free0( dbPASSWD );
		    }
    case clevel2  : { bmdconf_destroy( &bmd_confHandle ); }
    case clevel1  : { free0( (*log_tLog) ); }
    case nowhere  : {}
    default       : {}
   }

  return err_tRet;

 /* --------------------------------------------------------------------------- */

}

/* ----------------------------------------------------------- */

/**
 * Funkcja tworzy wpisy podstawowe dla struktury logów.
 * 
 *@param[in, out] log_tLog - adres struktury log_t
 *@return status zakończenia operacji 
*/

/* ----------------------------------------------------------- */


err_t LogCreate( log_t* const log_tLog )
 {

  /* --------------------------------------- */

     char*          stringTime    =    NULL; 
     long            intRet       =       0;
     err_t          err_tRet      =       0;

  /* --------------------------------------- */


   if ( log_tLog == NULL )
    {
      BMD_SETERR( err_tRet, BMD_ERR_PARAM1 );
      BMD_SETIERR( err_tRet, AD, LIBBMDLOG_INFO1 );

      return err_tRet;
    }

   err_tRet = FreeLog_tElements( log_tLog, all );

   if ( BMD_ERR(err_tRet) )
    {
      return err_tRet;
    }

   intRet = getTime( &stringTime );

   if ( intRet != BMD_OK )
    {
       BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
       BMD_SETIERR( err_tRet, AD, LIBBMDLOG_ERR1 );

       return err_tRet;
    }

   log_tLog->date_time_end = stringTime;

   intRet = asprintf(&(log_tLog->remote_pid), "%d", getpid());

   if ( intRet == -1 )
    {
      BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
      BMD_SETIERR( err_tRet, AD, LIBBMDLOG_ERR1 );

      return err_tRet;
    }

  #ifdef LOG_DEBUG

   intRet = asprintf(&(log_tLog->src_file),"%s",__FILE__);
  
   if ( intRet == -1 )
    {
      BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
      BMD_SETIERR( err_tRet, AD, LIBBMDLOG_ERR1 );

      return err_tRet;
    }
      
   intRet = asprintf(&(log_tLog->src_function),"%s",__FUNCTION__);
  
   if ( intRet == -1 )
    {
      BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
      BMD_SETIERR( err_tRet, AD, LIBBMDLOG_ERR1 );

      return err_tRet;
    }
  
   intRet = asprintf(&(log_tLog->src_line), "%d", __LINE__);

   if ( intRet == -1 )
    {
      BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
      BMD_SETIERR( err_tRet, AD, LIBBMDLOG_ERR1 );

      return err_tRet;
    }
  
  #endif

  return err_tRet;

 }

/* ----------------------------------------------------------- */

/**
 * Funkcja tworzy wpisy opcjonalne dla struktury logów.
 * Realizacja tej funkcjonalności za pomoacą tej funkcji
 * nie jest optymalna, ponieważ każde definiowanie nowego
 * parametru opcjonalnego wymaga wywołania tej funkcji.
 * Planowane jest zaimplementowanie funkcji która umożliwi
 * definiowanie kilku parametrów opcjonalnych naraz.
 *
 *@param[in] param_tType - określa który parametr dodać.
 *@param[in] stringValue - wartość dla tego parametru.
 *@param[in, out] log_tLog - adres struktury log_tLog.			   
 *@return status zakończenia operacji
*/

/* ----------------------------------------------------------- */


err_t LogSetParam( const param_t param_tType, const char * const stringValue, log_t * const log_tLog )
 {


  /* --------------------- */

     long    intRet   = 0;
     err_t   err_tRet = 0;

  /* --------------------- */



   if ( stringValue == NULL )
    {
        BMD_SETERR( err_tRet, BMD_ERR_PARAM2 );
        BMD_SETIERR( err_tRet, AD, LIBBMDLOG_INFO2 );

        return err_tRet;
    }

   if ( log_tLog == NULL )
    {
        BMD_SETERR( err_tRet, BMD_ERR_PARAM3 );
        BMD_SETIERR( err_tRet, AD, LIBBMDLOG_INFO3 );

        return err_tRet;
    }


   switch( param_tType )
    {

       case service : 			{
						if ( log_tLog->service )
						{
							BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
							BMD_SETIERR( err_tRet, AD, LIBBMDLOG_INFO5 );

							break;
						}

						intRet = asprintf(&(log_tLog->service),"%s", stringValue);

						if ( intRet == -1 )
						{
							BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
							BMD_SETIERR( err_tRet, AD, LIBBMDLOG_ERR1 );
						}

						break;
					}

      case log_level : 			{
						if ( log_tLog->log_level )
						{
							BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
							BMD_SETIERR( err_tRet, AD, LIBBMDLOG_INFO6 );

							break;
						}

						intRet = asprintf(&(log_tLog->log_level),"%s", stringValue);

						if ( intRet == -1 )
						{
							BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
							BMD_SETIERR( err_tRet, AD, LIBBMDLOG_ERR1 );
						}

						break;
					}

      case log_owner : 			{
						if ( log_tLog->log_owner )
						{
							BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
							BMD_SETIERR( err_tRet, AD, LIBBMDLOG_INFO7 );

							break;
						}

						intRet = asprintf(&(log_tLog->log_owner),"%s", stringValue);

						if ( intRet == -1 )
						{
							BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
							BMD_SETIERR( err_tRet, AD, LIBBMDLOG_ERR1 );
						}

						break;
					}

      case date_time_begin : 		{
						if ( log_tLog->date_time_begin )
						{
							BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
							BMD_SETIERR( err_tRet, AD, LIBBMDLOG_INFO8 );

							break;
						}

						intRet = asprintf(&(log_tLog->date_time_begin), "%s", stringValue);
				  
						if ( intRet == -1 )
						{
							BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
							BMD_SETIERR( err_tRet, AD, LIBBMDLOG_ERR1 );
						}

						break;
					}

      case log_string : 		{
						if ( log_tLog->log_string )
						{
							BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
							BMD_SETIERR( err_tRet, AD, LIBBMDLOG_INFO9 );

							break;
						}

						intRet = asprintf(&(log_tLog->log_string), "%s", stringValue);

						if ( intRet == -1 )
						{
							BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
							BMD_SETIERR( err_tRet, AD, LIBBMDLOG_ERR1 );
						}

						break;
					}

      case log_reason : 		{
						if ( log_tLog->log_reason )
						{
							BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
							BMD_SETIERR( err_tRet, AD, LIBBMDLOG_INFO10 );

							break;
						}

						intRet = asprintf(&(log_tLog->log_reason), "%s", stringValue);

						if ( intRet == -1 )
						{
							BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
							BMD_SETIERR( err_tRet, AD, LIBBMDLOG_ERR1 );
						}

						break;
					}

      case log_solution :		{
						if ( log_tLog->log_solution )
						{
							BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
							BMD_SETIERR( err_tRet, AD, LIBBMDLOG_INFO11 );

							break;
						}

						intRet = asprintf(&(log_tLog->log_solution), "%s", stringValue);

						if ( intRet == -1 )
						{
							BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
							BMD_SETIERR( err_tRet, AD, LIBBMDLOG_ERR1 );
						}

						break;
					}

      case document_filename : 		{

						if ( log_tLog->document_filename )
						{
							BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
							BMD_SETIERR( err_tRet, AD, LIBBMDLOG_INFO12 );

							break;
						}

						intRet = asprintf(&(log_tLog->document_filename), "%s", stringValue);

						if ( intRet == -1 )
						{
							BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
							BMD_SETIERR( err_tRet, AD, LIBBMDLOG_ERR1 );
						}

						break;
					}

      case document_size : 		{
						if ( log_tLog->document_size )
						{
							BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
							BMD_SETIERR( err_tRet, AD, LIBBMDLOG_INFO13 );

							break;
						}

						intRet = asprintf(&(log_tLog->document_size), "%s", stringValue);

						if ( intRet == -1 )
						{
							BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
							BMD_SETIERR( err_tRet, AD, LIBBMDLOG_ERR1 );
						}

						break;
					}

      case operation_type :		{
						if ( log_tLog->operation_type )
						{
							BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
							BMD_SETIERR( err_tRet, AD, LIBBMDLOG_INFO14 );

							break;
						}

						intRet = asprintf(&(log_tLog->operation_type), "%s", stringValue);

						if ( intRet == -1 )
						{
							BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
							BMD_SETIERR( err_tRet, AD, LIBBMDLOG_ERR1 );
						}

						break;
					}

      case log_referto : 		{
						if ( log_tLog->log_referto )
						{
							BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
							BMD_SETIERR( err_tRet, AD, LIBBMDLOG_INFO16 );

							break;
						}

						intRet = asprintf(&(log_tLog->log_referto), "%s", stringValue);

						if ( intRet == -1 )
						{
							BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
							BMD_SETIERR( err_tRet, AD, LIBBMDLOG_ERR1 );
						}

						break;
					}

      case event_code : 		{
						if ( log_tLog->event_code )
						{
							BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
							BMD_SETIERR( err_tRet, AD, LIBBMDLOG_INFO17 );

							break;
						}

						intRet = asprintf(&(log_tLog->event_code), "%s", stringValue);

						if ( intRet == -1 )
						{
							BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
							BMD_SETIERR( err_tRet, AD, LIBBMDLOG_ERR1 );
						}

						break;
					}

     case log_owner_event_visible :	{
						if ( log_tLog->log_owner_event_visible )
						{
							BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
							BMD_SETIERR( err_tRet, AD, LIBBMDLOG_INFO18 );

							break;
						}

						intRet = asprintf(&(log_tLog->log_owner_event_visible), "%s", stringValue);

						if ( intRet == -1 )
						{
							BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
							BMD_SETIERR( err_tRet, AD, LIBBMDLOG_ERR1 );
						}

						break;
                            	    	}

     case log_referto_event_visible :	{
						if ( log_tLog->log_referto_event_visible )
						{
							BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
							BMD_SETIERR( err_tRet, AD, LIBBMDLOG_INFO21 );

							break;
						}

						intRet = asprintf(&(log_tLog->log_referto_event_visible), "%s", stringValue);

						if ( intRet == -1 )
						{
							BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
							BMD_SETIERR( err_tRet, AD, LIBBMDLOG_ERR1 );
						}

						break;
					}

      case remote_host : 		{
						if ( log_tLog->remote_host )
						{
							BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
							BMD_SETIERR( err_tRet, AD, LIBBMDLOG_INFO19 );

							break;
						}

						intRet = asprintf(&(log_tLog->remote_host),"%s", stringValue);

						if ( intRet == -1 )
						{
							BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
							BMD_SETIERR( err_tRet, AD, LIBBMDLOG_ERR1 );
						}

						break;
					}

      case remote_port : 		{
						if ( log_tLog->remote_port )
						{
							BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
							BMD_SETIERR( err_tRet, AD, LIBBMDLOG_INFO20 );

							break;
						}

						intRet = asprintf(&(log_tLog->remote_port),"%s", stringValue);

						if ( intRet == -1 )
						{
							BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
							BMD_SETIERR( err_tRet, AD, LIBBMDLOG_ERR1 );
						}

						break;
					}

      case user_data1 : 		{
						if ( log_tLog->user_data1 )
						{
							BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
							BMD_SETIERR( err_tRet, AD, LIBBMDLOG_INFO22 );

							break;
						}

						intRet = asprintf(&(log_tLog->user_data1),"%s", stringValue);

						if ( intRet == -1 )
						{
							BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
							BMD_SETIERR( err_tRet, AD, LIBBMDLOG_ERR1 );
						}

						break;
					}

      case user_data2 : 		{
						if ( log_tLog->user_data2 )
						{
							BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
							BMD_SETIERR( err_tRet, AD, LIBBMDLOG_INFO23 );

							break;
						}

						intRet = asprintf(&(log_tLog->user_data2),"%s", stringValue);

						if ( intRet == -1 )
						{
							BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
							BMD_SETIERR( err_tRet, AD, LIBBMDLOG_ERR1 );
						}

						break;
					}

      case user_data3 : 		{
						if ( log_tLog->user_data3 )
						{
							BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
							BMD_SETIERR( err_tRet, AD, LIBBMDLOG_INFO24 );

							break;
						}

						intRet = asprintf(&(log_tLog->user_data3),"%s", stringValue);

						if ( intRet == -1 )
						{
							BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
							BMD_SETIERR( err_tRet, AD, LIBBMDLOG_ERR1 );
						}

						break;
					}

      case user_data4 : 		{
						if ( log_tLog->user_data4 )
						{
							BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
							BMD_SETIERR( err_tRet, AD, LIBBMDLOG_INFO25 );

							break;
						}

						intRet = asprintf(&(log_tLog->user_data4),"%s", stringValue);

						if ( intRet == -1 )
						{
							BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
							BMD_SETIERR( err_tRet, AD, LIBBMDLOG_ERR1 );
						}

						break;
					}

      case user_data5 : 		{
						if ( log_tLog->user_data5 )
						{
							BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
							BMD_SETIERR( err_tRet, AD, LIBBMDLOG_INFO26 );

							break;
						}

						intRet = asprintf(&(log_tLog->user_data5),"%s", stringValue);

						if ( intRet == -1 )
						{
							BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
							BMD_SETIERR( err_tRet, AD, LIBBMDLOG_ERR1 );
						}

						break;
					}


      default : 			{
						BMD_SETERR( err_tRet, BMD_ERR_PARAM1 );
						BMD_SETIERR( err_tRet, AD, LIBBMDLOG_INFO1 );
					}
    }

   return err_tRet;
  
 }


/* ----------------------------------------------------------- */

/**
 * Funkcja wysyła zakodowaną strukturę log_t do serwera
 * logów
 *
 *@param[in, out] log_tLog - adres struktury log_tLog.
 *@return status zakończenia operacji
*/

/* ----------------------------------------------------------- */

err_t LogCommit ( log_t * const log_tLog )
 {

  /* --------------------------------------------------- */
 
     void**             log_tTab              =    NULL;
     long               i                     =       0;
     long               j                     =       0;
     long               intEVENTS             =       0;
     int	        intMESSAGE            =       0;
     GenBuf_t*          GenBuf_tBufor         =    NULL;
     BMDLogElements_t*  BMDLogElements_tData  =       0;
     OCTET_STRING_t**   BMDLogElements_tTable =    NULL;
     err_t              err_tRet              =       0;
     cLevel_t           cLevel_tWHERE         = nowhere;
     char**             charLogLevel          =    NULL;
     asn_enc_rval_t                         asn_enc_ERR;

  /* --------------------------------------------------- */



   if ( log_tLog == NULL )
    {
      BMD_SETERR( err_tRet, BMD_ERR_PARAM1 );
      BMD_SETIERR( err_tRet, AD, LIBBMDLOG_INFO1 );

      return err_tRet;
    }

   err_tRet = GetSubstring( log_tLog->log_level, "[0-9]+", &charLogLevel );

   if ( BMD_ERR( err_tRet ) )
    {
      BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
      BMD_SETIERR( err_tRet, AD, LIBBMDLOG_ERR1 );

      return err_tRet;
    }

   intEVENTS  = atoi(charLogLevel[0]);
   intMESSAGE = atoi(charLogLevel[1]);

   free0( charLogLevel[0] );
   free0( charLogLevel[1] );
   free0( charLogLevel );

   if ( log_tLog->logPolicyTab[intEVENTS][intMESSAGE] && \
        *(log_tLog->logPolicyTab[intEVENTS][intMESSAGE]) == 0 )
	 {
		return err_tRet;
	 }

   BMDLogElements_tData = calloc( 1, sizeof(BMDLogElements_t));

   if ( BMDLogElements_tData == NULL )
    {
      BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
      BMD_SETIERR( err_tRet, AD, LIBBMDLOG_ERR1 );

      return err_tRet;
    }

   cLevel_tWHERE = clevel1;
   
   /*  *************************************************  LEVEL 1  ******************************************** */

   GenBuf_tBufor = calloc( 1, sizeof(GenBuf_t));
    
   if ( GenBuf_tBufor == NULL )
    {
      BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
      BMD_SETIERR( err_tRet, AD, LIBBMDLOG_ERR1 );

      goto GARBAGE;
    }

   cLevel_tWHERE = clevel2;
   
   /*  *************************************************  LEVEL 2  ******************************************** */
    
   BMDLogElements_tTable = (OCTET_STRING_t**)calloc( ASN_ELEMENTS_COUNT, sizeof(OCTET_STRING_t*));

   if ( BMDLogElements_tTable == NULL )
    {
      BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
      BMD_SETIERR( err_tRet, AD, LIBBMDLOG_ERR1 );

      goto GARBAGE;
    }
    
   cLevel_tWHERE = clevel3;

   /*  *************************************************  LEVEL 3  ******************************************** */

   log_tTab = (void**)malloc(sizeof(log_t));

   if ( log_tTab == NULL )
    {
      BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
      BMD_SETIERR( err_tRet, AD, LIBBMDLOG_ERR1 );

      goto GARBAGE;
    }

   memcpy(log_tTab, log_tLog, sizeof(log_t));

   //printf("PRIM_PARAM_START: %d\n", PRIM_PARAM_START);

   //printf("(char*)(log_tTab[1]): %s\n", (char*)(log_tTab[1]));
   //printf("(char*)(log_tTab[2]): %s\n", (char*)(log_tTab[2]));
   

   #ifdef LOG_DEBUG
    for ( i=PRIM_PARAM_START, j=2; i<=PRIM_PARAM_END; i++, j++ )
   //printf("LogDebug 1\n");
   #else
    for ( i=PRIM_PARAM_START+3, j=5; i<=PRIM_PARAM_END; i++, j++ )
   //printf("LogDebug 0\n");
   #endif
     {
        if ( log_tTab[i] == NULL )
         {
           BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
           BMD_SETIERR( err_tRet, AD, LIBBMDLOG_ERR15 );
	  
           goto GARBAGE;
         }

        BMDLogElements_tTable[j] = OCTET_STRING_new_fromBuf( &asn_DEF_OCTET_STRING, (char*)(log_tTab[i]), \
                                                              strlen((char*)(log_tTab[i])));
      
        if ( BMDLogElements_tTable[j] == NULL )
         {
            BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
            BMD_SETIERR( err_tRet, AD, LIBBMDLOG_ERR1 );
            
            cLevel_tWHERE = clevel4;

            /*  *************************************************  LEVEL 4  ******************************************** */

            goto GARBAGE;
         } 
     } 

    for ( ; i<=ADD_PARAM_END; i++, j++ )
     {
        if ( log_tTab[i] != NULL )
         {
           BMDLogElements_tTable[j] = OCTET_STRING_new_fromBuf( &asn_DEF_OCTET_STRING, (char*)(log_tTab[i]), \
	                                                     strlen((char*)(log_tTab[i])));
                                                       
           if ( BMDLogElements_tTable[j] == NULL )
            {
              BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
              BMD_SETIERR( err_tRet, AD, LIBBMDLOG_ERR1 );
              
              cLevel_tWHERE = clevel4;
	    
              /*  *************************************************  LEVEL 4  ******************************************** */

              goto GARBAGE;
            }
         }
     } 

    memcpy( BMDLogElements_tData, BMDLogElements_tTable, ASN_ELEMENTS_COUNT * sizeof(OCTET_STRING_t*) );
    asn_enc_ERR = der_encode( &asn_DEF_BMDLogElements, BMDLogElements_tData, 0, NULL );

    cLevel_tWHERE = clevel4;

    /*  *************************************************  LEVEL 4  ******************************************** */

    if ( asn_enc_ERR.encoded == -1 )
     {
       BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
       BMD_SETIERR( err_tRet, AD, LIBBMDLOG_ERR16 );

       goto GARBAGE;
     }

    GenBuf_tBufor->buf = (char *)calloc( asn_enc_ERR.encoded,  sizeof( char ));

    if ( GenBuf_tBufor->buf == NULL )
     {
       BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
       BMD_SETIERR( err_tRet, AD, LIBBMDLOG_ERR1 );

       goto GARBAGE;
     }

    cLevel_tWHERE = clevel5;

    /*  *************************************************  LEVEL 5  ******************************************** */

    asn_enc_ERR = der_encode( &asn_DEF_BMDLogElements, BMDLogElements_tData, CU_buff_composer_reentrant, \
                              (void*)GenBuf_tBufor );

    if ( asn_enc_ERR.encoded == -1 )
     {
       BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
       BMD_SETIERR( err_tRet, AD, LIBBMDLOG_ERR16 );

       goto GARBAGE;
     }

   /* +++++++++++++++++++++++++++++++++++++++++++++   WYSYŁKA LOGÓW   ++++++++++++++++++++++++++++++++++++++++++ */

    err_tRet = LogSendLoop( &(log_tLog->serwer_handler), Connect2RootServer, \
                          (void**)(log_tLog->serversTable), GenBuf_tBufor );

    if ( BMD_ERR( err_tRet ) )
     {
       goto GARBAGE;
     }

   /* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */  

    err_tRet = FreeLog_tElements( log_tLog, all );

 /* ------------------------------------------------------------------------------------------------------- */

 /* CZYSCICIEL */

 GARBAGE :

  switch( cLevel_tWHERE )
   {
    case clevel5 : { free_gen_buf( &GenBuf_tBufor ); }
    case clevel4 : { free0( log_tTab ); }
    case clevel3 : { free0( BMDLogElements_tTable ); }
    case clevel2 : { free_gen_buf( &GenBuf_tBufor ); }
    case clevel1 : { 
                     asn_DEF_BMDLogElements.free_struct( &asn_DEF_BMDLogElements, BMDLogElements_tData, 1 );
                     break; 
                   }
    default : {}
   }

  return err_tRet;

 /* ------------------------------------------------------------------------------------------------------- */

 }


/* ----------------------------------------------------------- */

/**
 * Funkcja kończy komunikację z serwerem logów
 * i zwalnia przydzielone zasoby.
 * 
 *@param[in, out] log_tLog - adres struktury log_tLog.			   
 *@return status zakończenia operacji
*/

/* ----------------------------------------------------------- */


err_t LogEnd ( log_t ** const log_tLog )
 {
  
   /* ---------------------------------- */
    
      err_t             err_tRet    = 0;
      long               i           = 0;
      struct sigaction  sigactionStruct;
      long               intRet      = 0;
  
   /* ---------------------------------- */

    
   if ( log_tLog == NULL || *log_tLog == NULL )
    {
      BMD_SETERR( err_tRet, BMD_ERR_PARAM1 );
      BMD_SETIERR( err_tRet, AD, LIBBMDLOG_INFO1 );
     
      return err_tRet;
    } 

   sigactionStruct.sa_handler = SIG_DFL;

   intRet = sigaction( SIGPIPE, &sigactionStruct, NULL );

   if ( intRet < 0 )
    {
      BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
      BMD_SETIERR( err_tRet, AD, LIBBMDLOG_ERR2 );
    }

   if (BMD_ERR( FreeLog_tElements( (*log_tLog), all ) ))
    {
      BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
      BMD_SETIERR( err_tRet, AD, LIBBMDLOG_ERR17 );
    }
 
   if ((*log_tLog)->serwer_handler)
    {
      bmdnet_close(&((*log_tLog)->serwer_handler));
    }

//   bmd_db_end();		/* Dla serwe a BMD tę linię trzeba zakomentować  */
   bmdnet_destroy();

   for( i=0; ((*log_tLog)->serversTable)[i] != NULL; i++ )
    {
      free0( (((*log_tLog)->serversTable)[i])->ip );
      free0( (((*log_tLog)->serversTable)[i])->port );
      free0( (((*log_tLog)->serversTable)[i])->attempt );
      free0( (((*log_tLog)->serversTable)[i])->delay );
      free0( ((*log_tLog)->serversTable)[i] );
    }

   free0( (*log_tLog)->serversTable );
   free0( *log_tLog );

   return err_tRet;
   
 }


