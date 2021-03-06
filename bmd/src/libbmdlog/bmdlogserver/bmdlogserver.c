/* *******************************************************
 *                   bmdlogserver.c                      *
 *       Implementacja serwera liścia			 *
 *               ( pierwsza warstwa )                    *
 *                                                       *
 *    Wtorek 18-09-2007 10:15                            *
 *                                                       *
 * Autor : Unizeto Technologies SA                       *
 *							 *
 *********************************************************/


#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <bmd/libbmdnet/libbmdnet.h>
#include <bmd/libbmdlog/libbmdlog.h>
#include <bmd/libbmdutils/libbmdutils.h>
#include <bmd/libbmdlog/log_server_utils/log_server_utils.h>
#include <bmd/libbmdlog/bmdlogserver/bmdlogserver.h>
#include <bmd/libbmdlog/bmdlogserver/bmdlogserver.dic>
#include <bmd/libbmdlog/bmdlogserver/database.dic>
#include <bmd/libbmdlog/bmdlogserver/bmdlogserver_signal.h>
#include <bmd/libbmdlog/balance/atd.h>
#include <bmd/libbmdlog/tree_build/tree_node.h>
#include <bmd/libbmdlog/tree_build/tree_build.h>
#include <bmd/libbmdcms/libbmdcms.h>
#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmddb/libbmddb.h>
#include <bmd/libbmdglobals/libbmdglobals.h>



void* signTimerFunc()
{

   /* --------------------------------------- */

      long          	longElapsedTime =  0;
      struct sigaction    structSigsignTimer;

   /* --------------------------------------- */


      structSigsignTimer.sa_handler = signTimer;
      structSigsignTimer.sa_flags = 0;

      sigaction( SIGALRM, &structSigsignTimer, NULL);

      signal( SIGINT, SIG_DFL);

      for(;;)
       {
          longElapsedTime = alarm( _GLOBAL_bmdlogServParam_tParams.sign_time );
          pause();
       }

    return NULL;
    
}



void* server( bmdnet_handler_ptr handler )
{

 /* --------------------------------------------------- */

    union sigval                                 value;
    asn_dec_rval_t                              decErr;
    GenBuf_t*          GenBuf_tBufor       =      NULL;
    BMDLogElements_t*  LogElem             =         0;
    log_t*             log_tLog            =      NULL;
    OCTET_STRING_t**   OCTET_STRING_tTable =      NULL;
    char*              stringSQLCommand    =      NULL;
    char**             log_tLogTable       =      NULL;
    void*              result              =      NULL;
    long               intBytesRead        =         0;
    long               intLogAck           = LOG_SAVED;
    long               intNumBytes         =         0;
    long               i                   =         0;
    long               intRet              =         0;
    long               intRows             =         0;
    long               intCols             =         0;
    cLevel_t           clevel_tWHERE       =   nowhere;
    err_t              err_tRet            =         0;

 /* --------------------------------------------------- */

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

   log_tLog = calloc( 1, sizeof( log_t ));

   if ( log_tLog == NULL )
    {
        PRINT_ERROR("Błąd alokacji pamieci\n");
        
        goto GARBAGE;
    }


   clevel_tWHERE = clevel3;

   /* *********************************************      LEVEL 3     ****************************************** */


   OCTET_STRING_tTable = (OCTET_STRING_t **)calloc( 1, ASN_ELEMENTS_COUNT * sizeof( OCTET_STRING_t* ));

   if ( OCTET_STRING_tTable == NULL )
    {
        PRINT_ERROR("Błąd alokacji pamieci\n");
        
        goto GARBAGE;
    }


   clevel_tWHERE = clevel4;

   /* *********************************************      LEVEL 4     ****************************************** */


   log_tLogTable = (char**)calloc(1, sizeof( log_t ));

   if ( log_tLogTable == NULL )
    {
        PRINT_ERROR("Błąd alokacji pamieci\n");
        
        goto GARBAGE;
    }

   /* Trzeba przewidziec fakt rozłączenia z bazą danych i możliwość ponownego połączenia  */

   intRet = bmd_db_connect2( (_GLOBAL_bmdlogServParam_tParams.baseParam).ip, (_GLOBAL_bmdlogServParam_tParams.baseParam).port, \
                                (_GLOBAL_bmdlogServParam_tParams.baseParam).base_name, (_GLOBAL_bmdlogServParam_tParams.baseParam).user, \
                                (_GLOBAL_bmdlogServParam_tParams.baseParam).password, &(log_tLog->dbase_handler));

   clevel_tWHERE = clevel5;

   /* *********************************************      LEVEL 5     ****************************************** */

   if ( intRet < 0 )
    {
        PRINT_ERROR("Nieudane połączenie z bazą danych\n");
        
        goto GARBAGE;
    }

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
            GenBuf_tBufor->buf = realloc( GenBuf_tBufor->buf, intNumBytes );
	
            if ( GenBuf_tBufor->buf == NULL )
             {
                PRINT_ERROR("Błąd alokacji pamieci\n");
                goto GARBAGE;
             }

            GenBuf_tBufor->size = intNumBytes;
         }
   
        clevel_tWHERE = clevel6;

        /* *********************************************      LEVEL 6     ****************************************** */

        intBytesRead = bmdnet_recv(handler, GenBuf_tBufor->buf, intNumBytes);
   
        if ( intBytesRead < intNumBytes )
         {
            PRINT_ERROR("Błąd transmisji danych\n");
            goto GARBAGE;

         }
      
        decErr = ber_decode( 0, &asn_DEF_BMDLogElements, (void**)&LogElem, GenBuf_tBufor->buf, intNumBytes );
	
        if ( decErr.code != RC_OK )
         {
            PRINT_ERROR("Błąd dekodowaia danych\n");
            goto GARBAGE;
         }

        
        memcpy( OCTET_STRING_tTable, LogElem, ASN_ELEMENTS_COUNT * sizeof( OCTET_STRING_t* ));

        clevel_tWHERE = clevel7;


	      /* *********************************************      LEVEL 7     ****************************************** */

        for( i = 0; i< ASN_ELEMENTS_COUNT; i++ )
         {
          if ( OCTET_STRING_tTable[i] != NULL )
             {
                intRet = OCTET_STRING2string( OCTET_STRING_tTable[i], &(log_tLogTable[i+1]));

                if ( intRet == -1 )
                 {
                    PRINT_ERROR("Blad alokacji pamieci\n");
                    
                    goto GARBAGE;
                 }
             }
         }
       
        memcpy( log_tLog, log_tLogTable, (ASN_ELEMENTS_COUNT+1) * sizeof( char* ));

        err_tRet = CreateHashfromLog( log_tLog );
	        if ( BMD_ERR( err_tRet ))
         {
            BMD_BTERR( err_tRet );
            BMD_FREEERR( err_tRet );
            
            goto GARBAGE;
         }
	
	      clevel_tWHERE = clevel8;
	
	      /* *********************************************      LEVEL 8     ****************************************** */

        err_tRet = CreateSQLCommand( log_tLog, &stringSQLCommand );

        if ( BMD_ERR( err_tRet ))
         {
            BMD_BTERR( err_tRet );
            BMD_FREEERR( err_tRet );
            
            goto GARBAGE;
         }

        intRet = bmd_db_execute_sql( log_tLog->dbase_handler, stringSQLCommand,&intRows, &intCols, &result);
      
        if ( intRet < 0 )
         {
            PRINT_ERROR("Błąd połączenia z bazą danych\n");
            
            goto GARBAGE;
         }

        intRet = sigqueue( getppid(), SIGRTMIN+4 , value); // kontrola błędów ???

        bmdnet_send( handler, &intLogAck, sizeof(long));

        /* -------------------------------------------------------------------------- */
        /* Moj malutki czysciciel                                                     */

        bmd_db_result_free(&result);
        free0( stringSQLCommand );
        free0( log_tLog->hash_value );
        free1( log_tLogTable, PRIM_PARAM_START-2, ASN_ELEMENTS_COUNT+1 );
        asn_DEF_BMDLogElements.free_struct( &asn_DEF_BMDLogElements, LogElem, 0);
        LogElem = 0;
        clevel_tWHERE = clevel6;

        /* -------------------------------------------------------------------------- */
 
    }

    intRet = bmd_db_disconnect(&(log_tLog->dbase_handler));

    if ( intRet < 0 )
     {
        PRINT_ERROR("Błąd zamkniecia bazy\n");
     }

 /* CZYSCICIEL */

 /* ---------------------------------------------------------------------------------------------  */

 GARBAGE :

   switch( clevel_tWHERE )
    {
        case clevel8 : { free0( log_tLog->hash_value ); }
        case clevel7 : {
                         free1( log_tLogTable, PRIM_PARAM_START-2, ASN_ELEMENTS_COUNT+1 );
                         asn_DEF_BMDLogElements.free_struct( &asn_DEF_BMDLogElements, LogElem, 0);
                         LogElem = 0;
                       }
        case clevel6 : { free_gen_buf( &GenBuf_tBufor ); }
	      case clevel5 : { free0( log_tLogTable ); }
	      case clevel4 : { free0( OCTET_STRING_tTable ); }
	      case clevel3 : { free0( log_tLog ); }
	      case clevel2 : { free0( GenBuf_tBufor );}
	      case clevel1 : { bmdnet_close( &handler ); }
	      default : {}
    }

 /* ---------------------------------------------------------------------------------------------  */

    
 return NULL; 
}


void* serverMain( bmdnet_handler_ptr socket )
{

   /* ---------------------------- */

      long              intRet = 0;
      struct sigaction  structSig;

   /* ---------------------------- */


  //_GLOBAL_debug_level = 30;

   signal( SIGCHLD, sigChildDHandler );  // wypadałoby zamienić na sigaction
   signal( SIGINT, SIG_DFL);

   structSig.sa_sigaction = newLog;
   structSig.sa_flags = SA_SIGINFO | SA_RESTART;

   intRet = sigaction( SIGRTMIN+4, &structSig, NULL ); //możliwe sprawdzenie błędu ?

   intRet = bmdnet_create(&socket);

   if ( intRet < 0 )
    {
        PRINT_ERROR("Błąd przy tworzeniu gniazda\n");
        exit(BMD_ERR_OP_FAILED);
    }

   bmdnet_logserver(socket, _GLOBAL_bmdlogServParam_tParams.ip, _GLOBAL_bmdlogServParam_tParams.port, 10, NULL, server);
   bmdnet_close(&socket);

  return NULL;

}


void monitor(void)
 {
  
   /* -------------------------------- */

      long        intRet          =  1;
      err_t      err_tRet        =  0;
      ATD                    hashList;

   /* -------------------------------- */


   INIT( &hashList );

   while(1)
    {
       pause();
       
       if (_GLOBAL_action == STOP_SERVER)
        {
         return;
        }      
        
       if ( ( intRet < 0 ) || ( intRet == 1 ) )
        {

           intRet = bmd_db_connect2( (_GLOBAL_bmdlogServParam_tParams.baseParam).ip, \
                                        (_GLOBAL_bmdlogServParam_tParams.baseParam).port, \
                                        (_GLOBAL_bmdlogServParam_tParams.baseParam).base_name, \
                                        (_GLOBAL_bmdlogServParam_tParams.baseParam).user, \
                                        (_GLOBAL_bmdlogServParam_tParams.baseParam).password, \
                                         &(_GLOBAL_bmdlogServParam_tParams.dbhandle));

           if ( intRet < 0 ) 
            {
               PRINT_ERROR("Brak połączenia z bazą danych\n");
               _GLOBAL_action = NOTHING; 
               continue;
            } 
           //bmd_db_disconnect( &(_GLOBAL_bmdlogServParam_tParams.dbhandle) );
        }

       for( ; hashList.head ; )
    	   DELETE( &hashList, (void**)(&(hashList.head)), DestroyTreeNode );

       err_tRet = ReadLogFromDatabase( _GLOBAL_bmdlogServParam_tParams.dbhandle, &hashList );

       if ( BMD_ERR( err_tRet ) )
        {
           bmd_db_disconnect( &(_GLOBAL_bmdlogServParam_tParams.dbhandle) );
           BMD_BTERR( err_tRet );
           BMD_FREEERR( err_tRet );
           intRet = 1;

           continue;
        }

       switch( _GLOBAL_action )
        {
          case BUILD_L0  : {
                             (_GLOBAL_action != STOP_SERVER) ? _GLOBAL_action = NOTHING : 0;
                              
                             err_tRet = EvaluateTreeL0( _GLOBAL_bmdlogServParam_tParams.dbhandle , &hashList, \
                                                        _GLOBAL_bmdlogServParam_tParams.log_amount, BUILD_OPM );

                             if ( BMD_ERR( err_tRet ) )
                              {
                                 bmd_db_disconnect( &(_GLOBAL_bmdlogServParam_tParams.dbhandle) );
                                 BMD_BTERR( err_tRet );
                                 BMD_FREEERR( err_tRet );
                                 intRet = 1;
                              }

                             break;
                           }

          case BUILD_LN  : {

                             (_GLOBAL_action != STOP_SERVER) ? _GLOBAL_action = NOTHING : 0; 
          
                             err_tRet = EvaluateTreeL0( _GLOBAL_bmdlogServParam_tParams.dbhandle , &hashList, \
                                                        _GLOBAL_bmdlogServParam_tParams.log_amount, BUILD_ALL );

                             if ( BMD_ERR( err_tRet ) )
                              {
                                 bmd_db_disconnect( &(_GLOBAL_bmdlogServParam_tParams.dbhandle) );
                                 BMD_BTERR( err_tRet );
                                 BMD_FREEERR( err_tRet );
                                 intRet = 1;
                                 break;
                              }

                             err_tRet = EvaluateOtherTreeLevels( _GLOBAL_bmdlogServParam_tParams.dbhandle, &hashList, \
                                                                 _GLOBAL_bmdlogServParam_tParams.signContext, \
                                                                 _GLOBAL_bmdlogServParam_tParams.log_amount );

                             if ( BMD_ERR( err_tRet ) )
                              {
                                 bmd_db_disconnect( &(_GLOBAL_bmdlogServParam_tParams.dbhandle) );
                                 BMD_BTERR( err_tRet );
                                 BMD_FREEERR( err_tRet );
                                 intRet = 1;
                              }
                              
                           }

          default        : { 
                             if (_GLOBAL_action == STOP_SERVER) return;
                           }                           
        }
    }

 }

/* MAIN */


int main( int argc, char* argv[] )
 {

   /* -------------------------------------------- */

      long                intRet           =    0;
      long                i                =    0;
      err_t               err_tRet         =    0; 
      bmdnet_handler_ptr  socket           = NULL;
      struct sigaction          structSigsignTime;
      struct sigaction           structSigbuildL0;
      struct sigaction        structSigStopServer;

   /* -------------------------------------------- */



      if ( argc < 3 )
       {
          printf("\n****** LOG SERVER for BMD - Unizeto Technologies SA *******\n");
          printf("*                                                          *\n");
          printf("*        -c [configuration file path]                      *\n");
          printf("*                                                          *\n");
          printf("************************************************************\n\n");
          
          exit(BMD_ERR_OP_FAILED);
       }

      err_tRet = BMDLogServerParamsInit( &_GLOBAL_bmdlogServParam_tParams );

      if ( BMD_ERR( err_tRet ) )
       {
         BMD_BTERR( err_tRet );
         BMD_FREEERR( err_tRet );
      
         exit(BMD_ERR_OP_FAILED);
       }

      _GLOBAL_debug_level = 0;

       bmd_init();

       for (i=1; i<argc; i++)
         {
           if (strcmp(argv[i],"-d") == 0)
            {
              if (argc>i+1) _GLOBAL_debug_level = atoi(argv[i+1]);
		PRINT_INFO("_GLOBAL_debug_level = %d\n", _GLOBAL_debug_level);
            }
            
           if (strcmp(argv[i],"-c") == 0)
            {
              if (argc>i+1)
               {
                  err_tRet = getBMDLogServerParams( argv[i+1], &_GLOBAL_bmdlogServParam_tParams );

                  if ( BMD_ERR( err_tRet ) )
                   {
                      BMD_BTERR( err_tRet );
                      BMD_FREEERR( err_tRet );
      
                      exit(BMD_ERR_OP_FAILED);
                   }
               }
            }
         }

       switch (fork())
	{
	    case  0 : { serverMain(socket); exit(BMD_OK); }
	    case -1 : { 
			PRINT_ERROR("Błąd uruchamiania procesu serwera\n");
			exit(BMD_ERR_OP_FAILED);
		      } 	
	}

       switch (fork())
	{
	    case  0 : { signTimerFunc(); exit(BMD_OK); }
	    case -1 : { 
			PRINT_ERROR("Błąd uruchamiania procesu timera\n");
			exit(BMD_ERR_OP_FAILED);
		      } 	
	}

       structSigsignTime.sa_handler = signTime;
       structSigsignTime.sa_flags = 0;

       structSigbuildL0.sa_handler = buildL0;
       structSigbuildL0.sa_flags = 0;

       structSigStopServer.sa_handler = sigChildDStopServer;
       structSigStopServer.sa_flags = 0;

       // można dodać obsługę błedu
       intRet = sigaction( SIGALRM, &structSigsignTime, NULL);

       // można dodać obsługę błedu
       intRet = sigaction( SIGUSR1, &structSigbuildL0, NULL);

       // można dodać obsługę błedu
       intRet = sigaction( SIGCHLD, &structSigStopServer, NULL); 
      
       /* funkcja Monitora  */
       /* ----------------- */

          monitor();

       /* ----------------- */

  return 0;
  
}//main


