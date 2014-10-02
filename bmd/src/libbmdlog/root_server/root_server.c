/* ******************************************************* 
 *                   root_server.c                       *
 *       Implementacja głównego serwera logów            *
 *               ( pierwsza warstwa )                    * 
 *                                                       *
 *    Wtorek 18-09-2007 10:15                            *
 *                                                       *
 * Autor : Unizeto Technologies SA                       *
 *                                                       *
 *********************************************************/


#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <bmd/libbmdnet/libbmdnet.h>
#include <bmd/libbmdutils/libbmdutils.h>
#include <bmd/libbmdlog/libbmdlog.h>
#include <bmd/libbmdlog/log_server_utils/log_server_utils.h>
#include <bmd/libbmdlog/root_server/root_server.h>
#include <bmd/libbmdlog/root_server/rootserver_signal.h>
#include <bmd/libbmdglobals/libbmdglobals.h>
#include <bmd/libbmderr/libbmderr.h>


void* server( bmdnet_handler_ptr handler )
{

 /* --------------------------------------------------- */

    GenBuf_t*          GenBuf_tBufor       =      NULL;
    BMDLogElements_t*  LogElem             =         0;
    long               intBytesRead        =         0;
    long               intNumBytes         =         0;
    long               intEVENTS           =         0;
    int	               intMESSAGE          =         0;
    char**             charLogLevel        =      NULL;
    char*              stringLogLevel      =      NULL;
    char**             charLogDefaultLevel =      NULL;
    long               intRet              =         0;
    long               intLogAck           = LOG_SAVED;
    cLevel_t           clevel_tWHERE       =   nowhere;
    logaction_t        logaction_tAction   =    ignore;
    logServParam_t**   balanceOutTable     =      NULL;
    err_t              err_tRet            =         0;
    bmdnet_handler_ptr socketBMDLogServer  =      NULL;
    struct sigaction                   sigactionStruct;
    asn_dec_rval_t                              decErr;
    asn_enc_rval_t                              encErr;

 /* --------------------------------------------------- */

   sigactionStruct.sa_handler = SIG_IGN;

   intRet = sigaction( SIGPIPE, &sigactionStruct, NULL );

   if ( intRet < 0 )
    {
      PRINT_ERROR("Błąd funkcji sigaction\n");
      goto GARBAGE;
    }

   clevel_tWHERE = clevel1;  

   /* *********************************************      LEVEL 1     ****************************************** */

   GenBuf_tBufor = calloc( 1, sizeof( GenBuf_t ));

   if ( GenBuf_tBufor == NULL )
    {
      PRINT_ERROR("Błąd alokacji pamięci\n");
      goto GARBAGE;
    }

   GenBuf_tBufor->buf  = NULL;
   GenBuf_tBufor->size =    0;


   clevel_tWHERE = clevel2;

   /* *********************************************      LEVEL 2     ****************************************** */


   while( 1 )
    {

      intBytesRead = bmdnet_recv(handler, &intNumBytes, sizeof(long));

      if ( intBytesRead <= 0 )
       {
          break; // koniec transmisji
       }

      if ( intBytesRead < sizeof( long ) )
       {
          PRINT_ERROR("Błąd odczytu ilości transmitowanych danych\n");

          goto GARBAGE;
       }

      if ( GenBuf_tBufor->size < intNumBytes )
       {
          GenBuf_tBufor->buf = realloc((GenBuf_tBufor->buf), intNumBytes );

          if ( GenBuf_tBufor->buf == NULL )
           {
              PRINT_ERROR("Błąd alokacji pamieci\n");

              goto GARBAGE;
           }

	  GenBuf_tBufor->size = intNumBytes;
       }

      intBytesRead = bmdnet_recv(handler, GenBuf_tBufor->buf, intNumBytes);

      if ( intBytesRead < intNumBytes )
       {
          PRINT_ERROR("Błąd transmisji danych\n");
          goto GARBAGE;
       }
      decErr = ber_decode( 0, &asn_DEF_BMDLogElements, (void**)&LogElem, GenBuf_tBufor->buf, intNumBytes );
	
      if ( decErr.code != RC_OK )
       {
          PRINT_ERROR("Błąd dekodowania danych\n");
          goto GARBAGE;
       }

      clevel_tWHERE = clevel3;

   /* *********************************************      LEVEL 3     ****************************************** */

      err_tRet = GetSubstring( _GLOBAL_rootServParam_tParams.log_default_level, "[0-9]+", &charLogDefaultLevel );

      if ( BMD_ERR( err_tRet ) )
       {
          BMD_BTERR( err_tRet );
          BMD_FREEERR( err_tRet );

          goto GARBAGE;
       }

      clevel_tWHERE = clevel4;

   /* *********************************************      LEVEL 4     ****************************************** */

      if ( LogElem->logLevel != NULL )
       {
          intRet = OCTET_STRING2string( LogElem->logLevel, &stringLogLevel );

          if ( intRet == -1 )
           {
              PRINT_ERROR("Błąd konwersji 8String2String\n");

              goto GARBAGE;
           }	

          clevel_tWHERE = clevel5;

        /* *********************************************      LEVEL 5     ****************************************** */

          err_tRet = GetSubstring( stringLogLevel, "[0-9]+", &charLogLevel );

          if ( BMD_ERR( err_tRet ) )
           {
              PRINT_ERROR("Błąd odczytu poziomu logowania\n");

              goto GARBAGE;
           }

          clevel_tWHERE = clevel6;

          /* *********************************************      LEVEL 6     ****************************************** */

          intEVENTS  = atoi(charLogLevel[0]);
          intMESSAGE = atoi(charLogLevel[1]);

          if ( _GLOBAL_rootServParam_tParams.logPolicyTab[intEVENTS][intMESSAGE] && \
               *(_GLOBAL_rootServParam_tParams.logPolicyTab[intEVENTS][intMESSAGE]) == 1 )
           {
              logaction_tAction = collect;
           }
       }
      else
       {
          intEVENTS  = atoi(charLogDefaultLevel[0]);
          intMESSAGE = atoi(charLogDefaultLevel[1]);

          if ( _GLOBAL_rootServParam_tParams.logPolicyTab[intEVENTS][intMESSAGE] && \
               *(_GLOBAL_rootServParam_tParams.logPolicyTab[intEVENTS][intMESSAGE]) == 1 )
           {
              LogElem->logLevel = OCTET_STRING_new_fromBuf( &asn_DEF_OCTET_STRING, \
                                  _GLOBAL_rootServParam_tParams.log_default_level, \
                                  strlen( _GLOBAL_rootServParam_tParams.log_default_level ) );

              if ( LogElem->logLevel == NULL )
               {
                  PRINT_ERROR("Blad alokacji pamieci\n");

                  goto GARBAGE; 
               }

              logaction_tAction = collect;
           }
       }
	
    /* 1) Należy zapakować nową strukturę logu */

      if ( logaction_tAction == collect )
       {
printf("Czy remote hist == null?\n");
	 if ( LogElem->remoteHost == NULL )
	  {

		printf("remote host: %s\n", handler->stringClientHost);
		LogElem->remoteHost = OCTET_STRING_new_fromBuf( &asn_DEF_OCTET_STRING, \
					handler->stringClientHost, strlen(handler->stringClientHost));
	  }

         if ( LogElem->remoteHost == NULL )
          {
              PRINT_ERROR("Błąd alokacji pamieci\n");

              goto GARBAGE;
          }

	 if ( LogElem->remotePort == NULL )
	  {
		LogElem->remotePort = OCTET_STRING_new_fromBuf( &asn_DEF_OCTET_STRING, \
					handler->stringClientPort, strlen(handler->stringClientPort));
	  }

         if ( LogElem->remotePort == NULL )
          {
              PRINT_ERROR("Błąd alokacji pamieci\n");

              goto GARBAGE;
          }

         encErr = der_encode( &asn_DEF_BMDLogElements, LogElem, 0, NULL );

         if ( encErr.encoded == -1 )
          {
              PRINT_ERROR("Błąd kodera\n");

              goto GARBAGE;
          }

         if ( GenBuf_tBufor->size < encErr.encoded )
          {
              GenBuf_tBufor->buf = realloc( GenBuf_tBufor->buf, encErr.encoded );
	
              if ( GenBuf_tBufor->buf == NULL )
               {
                    PRINT_ERROR("Błąd alokacji pamieci\n");
                    goto GARBAGE; 
               } 
          }

         GenBuf_tBufor->size = 0;

         encErr = der_encode( &asn_DEF_BMDLogElements, LogElem, CU_buff_composer_reentrant, \
                              (void*)GenBuf_tBufor );

         if ( encErr.encoded == -1 )
          {
              PRINT_ERROR("Błąd kodera\n");
              goto GARBAGE;
          }

        /*  2) Wybrać serwer logów zgodny z algorytmem balancingu  */

         intRet = _GLOBAL_rootServParam_tParams.balanceFunc( &_GLOBAL_rootServParam_tParams, &balanceOutTable );

         if ( intRet < 0 )
          {
              PRINT_ERROR("Błąd balancera\n");
              goto GARBAGE;
          }

            err_tRet = LogSendLoop( &socketBMDLogServer, Connect2BMDLogServer, \
                                   (void**)balanceOutTable, GenBuf_tBufor );

         if ( BMD_ERR( err_tRet ) )
          {
              BMD_BTERR( err_tRet );
              BMD_FREEERR( err_tRet );

              goto GARBAGE;
          }
       }	
      else
       {
          PRINT_VDEBUG("Log nie jest rejestrowany\n");
       }

      bmdnet_send( handler, &intLogAck, sizeof(long));  //kontrola błędów ???
 
     /* -------------------------------------------------------------------------- */
     /* Moj malutki czysciciel                                                     */

        free0( charLogLevel[0] );
        free0( charLogLevel[1] );
        free0( charLogLevel );

        free0( stringLogLevel );

        free0( charLogDefaultLevel[0] );
        free0( charLogDefaultLevel[1] );
        free0( charLogDefaultLevel );

        logaction_tAction = ignore;
        asn_DEF_BMDLogElements.free_struct( &asn_DEF_BMDLogElements, LogElem, 0);
        LogElem = 0;
        clevel_tWHERE = clevel2;

     /* -------------------------------------------------------------------------- */
 
    }

 /* CZYSCICIEL */

 GARBAGE :

   switch( clevel_tWHERE )
    {
        case clevel6 : {
                         free0( charLogLevel[0] );
                         free0( charLogLevel[1] );
                         free0( charLogLevel );
                       }
        case clevel5 : { free0( stringLogLevel ); }
        case clevel4 : { 
                         free0( charLogDefaultLevel[0] );
                         free0( charLogDefaultLevel[1] );
                         free0( charLogDefaultLevel );
                       }
        case clevel3 : { 
                         asn_DEF_BMDLogElements.free_struct( &asn_DEF_BMDLogElements, LogElem, 0);
                         LogElem = 0;
                       }
        case clevel2 : { free_gen_buf( &GenBuf_tBufor ); }
        case clevel1 : { bmdnet_close( &handler ); }
        case nowhere : {}
        default : {}
    }
 return NULL; 
}

/* --------------------------------------------- MAIN --------------------------------------------- */


int main( int argc, char* argv[] )
 {


     /* -------------------------------------------- */

        err_t               err_tRet         =    0;
        long                intRet           =    0;
        long                i                =    0;
        bmdnet_handler_ptr  socket           = NULL;
        struct sigaction      structSigChangeParams;

     /* -------------------------------------------- */




        if ( argc < 3 )
         {
           printf("\n****** LOG BROKER SERVER for BMD - Unizeto Technologies SA *******\n");
           printf("*                                                                *\n");
           printf("*        -c [configuration file path]                            *\n");
           printf("*                                                                *\n");
           printf("******************************************************************\n\n");

           exit(-1);
         }

        err_tRet = rootServerParamsInit( &_GLOBAL_rootServParam_tParams );

        if ( BMD_ERR( err_tRet ) )
         {
           BMD_BTERR( err_tRet );
           BMD_FREEERR( err_tRet );

           exit(-1);
         }

        _GLOBAL_debug_level = 0;

        for (i=1; i<argc; i++)
         {
           if (strcmp(argv[i],"-d") == 0)
            {
              if (argc>i+1) _GLOBAL_debug_level = atoi(argv[i+1]);
            }

           if (strcmp(argv[i],"-c") == 0)
            {
              if (argc>i+1)
               {
                  if ( asprintf( &(_GLOBAL_rootServParam_tParams.confFile), "%s", argv[i+1]) < 0 )
                   {
                      PRINT_ERROR( "Błąd alokacji pamięci\n" );

                      exit(-1);
                   }
               }
            }
         }

        err_tRet = getRootServerParams( _GLOBAL_rootServParam_tParams.confFile, up_server, \
                                        &_GLOBAL_rootServParam_tParams );

        if ( BMD_ERR( err_tRet ) )
         {
    	     BMD_BTERR( err_tRet );
	     BMD_FREEERR( err_tRet );

           exit(-1);
         }

        bmdnet_init(); 		        /* błąd obsłużony abortem */

        signal( SIGCHLD, sigChildDHandler );  //wypadałoby zamienić na sigaction

        structSigChangeParams.sa_handler = sigChangeParams;
        structSigChangeParams.sa_flags = SA_RESTART;


        intRet = sigaction( SIGUSR1, &structSigChangeParams, NULL);

        intRet = bmdnet_create(&socket);

        if ( intRet < 0 )
         {
            PRINT_ERROR("Błąd przy tworzeniu gniazda\n");

            exit(-1);
         }

       bmdnet_logserver(socket, _GLOBAL_rootServParam_tParams.ip, atoi( _GLOBAL_rootServParam_tParams.port ), 10, NULL, server);
       bmdnet_close(&socket);

       bmdnet_destroy();


   return 0;

}

/* ------------------------------------------------------------------------------------------------ */

