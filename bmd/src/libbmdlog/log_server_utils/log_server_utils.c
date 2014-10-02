/****************************************
 *                                      *
 * W pliku tym zawarte będą wszystkie   *
 * funkcje pomocnicze wykorzystywane    *
 * w systemie podpisanych logów BMD     *
 *                                      *
 * Data : 27-08-2007                    *
 * Autor : Tomasz Klimek                *
 *                                      *
 ****************************************/


#include <stdio.h>
#include <pcre.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <sys/dir.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <errno.h>
#include <bmd/libbmdcms/libbmdcms.h>
#include <bmd/libbmdlog/libbmdlog.h>
#include <bmd/libbmdlog/log_server_utils/log_server_utils.h>
#include <bmd/libbmdlog/root_server/root.dic>
#include <bmd/libbmdlog/bmdlogserver/bmdlogserver.dic>
#include <bmd/libbmdlog/bmdlogserver/database.dic>
#include <bmd/libbmdlog/root_server/sec_serv.dic>
#include <bmd/libbmdutils/libbmdutils.h>
#include <bmd/libbmdlog/balance/atd.h>
#include <bmd/libbmdlog/balance/algo.h>
#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmddb/libbmddb.h>


/**
 * Funkcja komponuje zapytanie SQLowe  umieszczejące 
 * informacje o logach w bazie danych
 * 
 * @param[in] log_tLog - struktura przechowywująca informację o pojedynczym logu
 * @param[out] stringSQLCommand - skonstruowane zapytanie SQLowe
 * 
 * @return W przypadku błędu funkcja zwraca komunikat za pomocą stosu błędów
 *
*/


err_t CreateSQLCommand(const log_t * const log_tLog, char ** const stringSQLCommand)
{
 
 /* --------------------------------------- */

    long      intRet             =       0;
    char*     stringFront        =    NULL;
    char*     stringEnd          =    NULL;
    char*     stringTmp          =    NULL;
    char*     stringSeqnextval   =    NULL;
    char*     stringEscaped      =    NULL;
    void**    log_tTableElements =    NULL;
    long      i                  =       0;
    err_t     err_tRet           =       0;
    cLevel_t  cLevel_tWHERE      = nowhere;

 /* --------------------------------------- */


 
    if ( log_tLog == NULL )
     {
        BMD_SETERR( err_tRet, BMD_ERR_PARAM1 );
        BMD_SETIERR( err_tRet, AD, "Niepoprawny parametr nr. 1" );
    
        return err_tRet;
     }
    
    if ( stringSQLCommand == NULL || *stringSQLCommand != NULL )
     {
        BMD_SETERR( err_tRet, BMD_ERR_PARAM2 );
        BMD_SETIERR( err_tRet, AD, "Niepoprawny parametr nr. 2" );

        return err_tRet;
     }


    intRet = asprintf( &stringFront, "%s", \
    \
     "INSERT INTO log ( "\
                        "remote_host, remote_port, src_file, src_line, src_function," \
			"date_time_end, remote_pid, log_owner, service, operation_type," \
                        "log_level, date_time_begin, log_string, log_reason, log_solution," \
			"document_size, document_filename, log_referto, event_code," \
			"log_owner_event_visible, log_referto_event_visible, user_data1," \
			"user_data2, user_data3, user_data4, user_data5, hash_value, id" \
		     ") " );	
    
    if ( intRet == -1 )
     {
        BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
        BMD_SETIERR( err_tRet, AD, "Bląd alokacji pamięci" );
    
        return err_tRet;
     }
     

    /*  ******************************************   LEVEL 1  ******************************************* */      

    cLevel_tWHERE = clevel1;

    intRet = asprintf(&stringEnd, "%s", "VALUES (");

    if ( intRet == -1 )
     {
        BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
        BMD_SETIERR( err_tRet, AD, "Bląd alokacji pamięci" );
    
        goto GARBAGE;
     }

    /*  ******************************************   LEVEL 2  ******************************************* */      
    
    cLevel_tWHERE = clevel2;

    log_tTableElements = (void**)calloc( 1, sizeof(log_t));
    
    if ( log_tTableElements == NULL )
     {
        BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
        BMD_SETIERR( err_tRet, AD, "Bląd alokacji pamięci" );
    
        goto GARBAGE;
     }
    
    /*  ******************************************   LEVEL 3  ******************************************* */      

    cLevel_tWHERE = clevel3;

    memcpy( log_tTableElements, log_tLog, sizeof(log_t) );

    for( i = PRIM_PARAM_START-2; i <= ADD_PARAM_END+1; i++ )
     {
        stringTmp = stringEnd;
        
	if ( log_tTableElements[i] != NULL )
	 {
	 
	   /* intRet = bmd_db_escape_string(log_tLog->dbase_handler, (char*)(log_tTableElements[i]),STANDARD_ESCAPING,&stringEscaped );

	    if ( intRet < 0 )
	     {
	        BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
                BMD_SETIERR( err_tRet, AD, "Bląd escapowania stringa" );

                free0( stringTmp );
                free0( stringEscaped );
                stringEnd = NULL;
		goto GARBAGE;
	     }*/

	    intRet = asprintf( &stringEnd, "%s%s%s%s", stringEnd, "'",(char*)(log_tTableElements[i]), "',");

	    free0( stringTmp );
	    free0( stringEscaped );

	    if ( intRet == -1 )
	     {
	        BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
                BMD_SETIERR( err_tRet, AD, "Bląd alokacji pamięci" );

                stringEnd = NULL;
		goto GARBAGE;
	     }
	 }
        else
	 {
	    intRet = asprintf( &stringEnd, "%s%s", stringEnd, "NULL,");
	    free0( stringTmp );

	    if ( intRet == -1 )
	     {
	        BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
                BMD_SETIERR( err_tRet, AD, "Bląd alokacji pamięci" );
		
		stringEnd = NULL;
		goto GARBAGE;
	     }
	    
	 }
     }

    stringSeqnextval = bmd_db_get_sequence_nextval(log_tLog->dbase_handler, "log_id_seq");
    
    if ( stringSeqnextval == NULL )
     {
        BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
        BMD_SETIERR( err_tRet, AD, "Bląd alokacji pamięci" );

	goto GARBAGE;
     }

    /*  ******************************************   LEVEL 4  ******************************************* */      

    cLevel_tWHERE = clevel4;

    stringTmp = stringEnd;
    intRet = asprintf( &stringEnd, "%s%s%s", stringEnd, stringSeqnextval, ");");
    free0(stringTmp);

    if ( intRet == -1 )
     {
        BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
        BMD_SETIERR( err_tRet, AD, "Bląd alokacji pamięci" );

        stringEnd = NULL;
	goto GARBAGE;
     }


    intRet = asprintf(stringSQLCommand, "%s%s", stringFront, stringEnd);
    

    if ( intRet == -1 )
     {
        BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
        BMD_SETIERR( err_tRet, AD, "Bląd alokacji pamięci" );

	*stringSQLCommand = NULL;
     }


 GARBAGE :
 
    switch( cLevel_tWHERE )
     {
      case clevel4 : { free0( stringSeqnextval ); }
      case clevel3 : { free0( log_tTableElements ); }
      case clevel2 : { free0( stringEnd ); }
      case clevel1 : { free0( stringFront ); }
      default : {}
     }
   
  return err_tRet;
}


/**
 *
 * Funkcja oblicza wartość skrótu z łańcucha przechowywanego
 * w GenBufie
 *
 * @param[in] GenBuf_tInput - wejściowy GenBuf
 * @param[out] stringHash - obliczona wartość skrótu w postaci stringa
 * @return W przypadku błedu zwracany jest komunikat za pomocą stosu błędów
 *
*/


err_t CreateHash( GenBuf_t* const GenBuf_tInput, char** const stringHash )
{


    /* ------------------------------------------- */

       err_t             err_tRet       =       0;
       long              intRet         =       0;
       bmd_crypt_ctx_t*  bmdcctx_t      =    NULL;
       cLevel_t          cLevel_tWHERE  = nowhere;
       GenBuf_t*         GenBuf_tOutput =    NULL;

    /* ------------------------------------------- */



     if ( GenBuf_tInput == NULL )
      {
    	    BMD_SETERR( err_tRet, BMD_ERR_PARAM1 );
	    BMD_SETIERR( err_tRet, AD, "Niepoprawny pierwszy parametr");
    
	    return err_tRet;
      }

     intRet = bmd_set_ctx_hash( &bmdcctx_t, HASH_ALGORITHM ); // wybieramy funkcje skrotu
  
     if ( intRet < 0 )
      {
    	    BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
	    BMD_SETIERR( err_tRet, AD, "Błąd tworzenia kontekstu");
    
	    return err_tRet;
      }

     /*  ******************************************   LEVEL 1  ******************************************* */      

     cLevel_tWHERE = clevel1;
PRINT_INFO("STRING TO HASH:|%s|\n", (char* )GenBuf_tInput->buf);

     intRet = bmd_hash_data( GenBuf_tInput, &bmdcctx_t, &GenBuf_tOutput, NULL);
  
     if ( intRet < 0 )
      {
    	    BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
    	    BMD_SETIERR( err_tRet, AD, "Obliczenie skrotu nie powiodło się" );

	    goto GARBAGE;
      }

     /*  ******************************************   LEVEL 2  ******************************************* */      

     cLevel_tWHERE = clevel2;

     /* Poprawiono */
     /* intRet = bmd_hash_as_string( GenBuf_tOutput, &(*stringHash), 0, 1 );*/ 
     intRet = bmd_hash_as_string( GenBuf_tOutput, stringHash, 0, 1 );
     PRINT_INFO("HASH VALUE:|%s|\n", *stringHash);

     if ( intRet < 0 )
      {
    	    BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
    	    BMD_SETIERR( err_tRet, AD, "Obliczenie skrotu w postaci stringa nie powiodło się" );

	    goto GARBAGE;
      }

  /* -------------------------------------------------------------------------------------------- */
  
 GARBAGE :
 
    switch( cLevel_tWHERE )
     {
      case clevel2 : { free_gen_buf( &GenBuf_tOutput ); }
      case clevel1 : { 
                      intRet = bmd_ctx_destroy( &bmdcctx_t );
		      
		      if ( intRet < 0 )
		       {
		    	    BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
		    	    BMD_SETIERR( err_tRet, AD, "Błąd zwalniania kontekstu");
		       } 
                     }
      default : {}
     }

  /* -------------------------------------------------------------------------------------------- */
   
  return err_tRet;

}




/**
 *
 * Funkcja oblicza wartość skrótu z logu
 * 
 * @param[in, out] log_tLog - węzeł z pojedynczym logiem
 * 
 * @return W przypadku błedu zwracany jest komunikat za pomocą stosu błędów
 *
*/


err_t CreateHashfromLog( log_t * const log_tLog )
{

 
    /* ------------------------------------------- */

       err_t             err_tRet       =       0;
       GenBuf_t*         GenBuf_tInput  =    NULL;
       char*             stringHash     =    NULL;
     
    /* ------------------------------------------- */

 
 
     err_tRet = FillGenBuf( log_tLog, &GenBuf_tInput );

     if (BMD_ERR(err_tRet))
      {
         BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
         BMD_SETIERR( err_tRet, AD, "Utworzenie GenBufa nie powiodło się" );

         return err_tRet;
      }


     err_tRet = CreateHash( GenBuf_tInput, &stringHash );
  
     if ( !BMD_ERR( err_tRet ) )
      {
         log_tLog->hash_value = stringHash;
      }
  
     free_gen_buf( &GenBuf_tInput );
   
  return err_tRet;

}


/**
 *
 * Funkcja wypełnia zawartość bufora generycznego informacjami
 * pobranymi ze struktury opisującej log.
 *
 * @param[in] log_tLog - węzeł opisujący pojedynczy log
 * @param[out] GenBuf_tBuff - wynikowy bufor generyczny
 *
 * @return W przypadku błędu zwracany jest komunikat za pomocą stosu błędów
 *
*/

err_t FillGenBuf( const log_t* const log_tLog, GenBuf_t** const GenBuf_tBuff )
 {

  
   /* ----------------------------------------- */

      err_t      err_tRet            =       0;
      GenBuf_t*  GenBuf_tTmp         =    NULL;
      void**     stringLog_tElements =    NULL;
      long       i                   =       0;
      long       intRet              =       0;
      char*      stringLog           =    NULL;
      char*      stringLogTmp        =    NULL;
 
   /* ----------------------------------------- */


 
    if ( GenBuf_tBuff == NULL || *GenBuf_tBuff != NULL )
     {
        BMD_SETERR( err_tRet, BMD_ERR_PARAM1 );
        BMD_SETIERR( err_tRet, AD, "Niepoprawny parametr nr. 1" );
    
        return err_tRet;
     }
    
    if ( log_tLog == NULL )
     {
        BMD_SETERR( err_tRet, BMD_ERR_PARAM2 );
        BMD_SETIERR( err_tRet, AD, "Niepoprawny parametr nr. 2" );

        return err_tRet;
     }

    if ( asprintf( &stringLog, "%s", "") < 0 )
     {
        BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
        BMD_SETIERR( err_tRet, AD, "Błąd alokacji pamięci" );

        return err_tRet;
     }

    stringLog_tElements = (void**)calloc(1, sizeof( log_t ));
    
    if ( stringLog_tElements == NULL )
     {
        BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
        BMD_SETIERR( err_tRet, AD, "Błąd alokacji pamięci" );

        return err_tRet;
     }
    
    memcpy( stringLog_tElements, log_tLog, sizeof( log_t ));

    for( i = 1; i<= ADD_PARAM_END; i++)
     {
      if ( stringLog_tElements[i] != NULL )
       {
    	    stringLogTmp = stringLog;
    	    intRet = asprintf(&stringLog, "%s%s", stringLog, (char*)(stringLog_tElements[i]));
    	    free0( stringLogTmp );
      
    	    if ( intRet == -1 )
             {
    	        BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
    		BMD_SETIERR( err_tRet, AD, "Błąd alokacji pamięci" );
		stringLog = NULL;
            
    	        goto GARBAGE;
             }
       }
     }

    intRet = set_gen_buf2( (char*)stringLog, strlen( stringLog ), &GenBuf_tTmp );
    
    if ( intRet < 0 )
     {
        BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
        BMD_SETIERR( err_tRet, AD, "Błąd konwersji do GenBuf_t" );
     }
  
    *GenBuf_tBuff = GenBuf_tTmp;

 GARBAGE : 
  
    free0( stringLog );
    free0( stringLog_tElements );

  return err_tRet;
 }


/**
 * Funkcja wyszukuje podnapisy pasujące do podanego wzorca
 * 
 * @param[in] string - analizowany łańcuch
 * @param[in] pattern - zadany wzorzec
 * @param[out] stringTable - tablica podnapisów
 *
 * @return W przypadku błędu zwracany jest komunikat za pomocą stosu błędów
 *
*/


err_t GetSubstring( const char* const string, const char* const pattern, char*** const stringTable )
{


  /* ----------------------------------------- */

	err_t        err_tRet             =    0;
	pcre*        re                   = NULL;
	const char*  error                = NULL;
	const char*  stringPtr            = NULL;
	int          erroffset            =    0;
	long         intRet               =    0;
	long         ile                  =    0;
	long         i                    =    0;
	int          ovector[OVECTOR_NUM] =   {};

  /* ----------------------------------------- */



   if ( string == NULL )
    {
	BMD_SETERR( err_tRet, BMD_ERR_PARAM1 ); 
	BMD_SETIERR( err_tRet, AD, "Niepoprawny pierwszy parametr");
    
	return err_tRet;
    }

   if ( pattern == NULL )
    {
	BMD_SETERR( err_tRet, BMD_ERR_PARAM2 ); 
	BMD_SETIERR( err_tRet, AD, "Niepoprawny drugi parametr");
    
	return err_tRet;
    }

   if ( stringTable == NULL )
    {
	BMD_SETERR( err_tRet, BMD_ERR_PARAM2 ); 
	BMD_SETIERR( err_tRet, AD, "Niepoprawny trzeci parametr");
    
	return err_tRet;
    }

   re = pcre_compile( pattern, 0, &error, &erroffset, NULL);
   
   if ( re == NULL )
    {
	BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED ); 
	BMD_SETIERR( err_tRet, AD, "Błąd inicjowania PCRE");
    
	return err_tRet;
    }
   
   for( ile=0; (intRet=pcre_exec(re, NULL, string, strlen(string),\
        ovector[1], 0, ovector, OVECTOR_NUM))>0; ile+=intRet);
   
   
   *stringTable = ( char** )calloc( ile+1, sizeof( char* ) );
   
   if ( *stringTable == NULL )
    {
	BMD_SETERR( err_tRet, BMD_ERR_MEMORY ); 
	BMD_SETIERR( err_tRet, AD, "Błąd alokacji pamięci");
    
	return err_tRet;
    }
   
   ovector[1] = 0;
   
   for( i=0; (intRet=pcre_exec(re, NULL, string, strlen(string),\
        ovector[1], 0, ovector, OVECTOR_NUM))>0; i++)
    {
	intRet = pcre_get_substring( string, ovector, intRet, 0, &stringPtr );
	intRet = asprintf( &((*stringTable)[i]), "%s", stringPtr );
	
	if ( intRet == -1 )
	 {
	    BMD_SETERR( err_tRet, BMD_ERR_MEMORY ); 
	    BMD_SETIERR( err_tRet, AD, "Błąd alokacji pamięci");
    
	    return err_tRet;
 	 }
    }

   pcre_free(re);

 return err_tRet;

}


/**
 * Funkcja zwalnia zaalokowane elementy struktury log_t.
 * 
 * @param[in, out] log_tLog - adres struktury log_tLog.
 * @param[in] option_tOpt - określa typ zwalnianej sekcji
 * - prim - zwlanianie parametrów podstawowych
 * - add  - zwalnianie parametrów opcjonalnych
 * - all - wszystkie
 * @return W przypadku błędu zwracany jest komunikat za pomocą stosy błędów
 *
*/


err_t FreeLog_tElements( log_t * const log_tLog, const option_t option_tOpt )
 {
 
   /* --------------------------- */

      err_t   err_tRet    =    0;
      void**  log_tLogTab = NULL;

   /* --------------------------- */



   if ( log_tLog == NULL )
    {
        BMD_SETERR( err_tRet, BMD_ERR_PARAM1 );
        BMD_SETIERR( err_tRet, AD, "Podano nieprawidłowy parametr nr 1" );
     
        return err_tRet;
    } 
   
   if ( option_tOpt != prim && option_tOpt != _add && option_tOpt != all  )
    {
        BMD_SETERR( err_tRet, BMD_ERR_PARAM1 );
        BMD_SETIERR( err_tRet, AD, "Podano nieprawidłowy parametr nr 2" );
     
        return err_tRet;
    } 
 
   log_tLogTab = (void**)calloc( 1, sizeof( log_t));
   
   if ( log_tLogTab == NULL )
    {
        BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
        BMD_SETIERR( err_tRet, AD, "Błąd alokacji pamięci" );
    
        return err_tRet;
    }
  
   memcpy( log_tLogTab, log_tLog, sizeof( log_t ));
   
   if ( option_tOpt == all || option_tOpt == prim )
    {
	free1( log_tLogTab, PRIM_PARAM_START, ((PRIM_PARAM_END-3)+1));     /* Obliczenia dla zachowania czytelności */
    }
   
   if ( option_tOpt == all || option_tOpt == _add )
    {
	free1( log_tLogTab, (PRIM_PARAM_END-2), (ADD_PARAM_END+1) );	  /* Obliczenia dla zachowania czytelności */
    }     

   memcpy( log_tLog, log_tLogTab, sizeof( log_t ));
   
   return err_tRet;

 }


/**
 * Funkcja nawiązuje połączenie z głównym serwerem logów
 * 
 * @param[in] handler - uchwyt połączenia
 * @param[in] servTable - tablica dostępnych serwerów
 *
 * @return W przypadku błędu zwracany jest komunikat za pomocą stosu błędów
 *
*/

long Connect2RootServer( bmdnet_handler_ptr const handler, void** const servTable )
{

   /* --------------------- */

      long    intRet   = 0;
      long    i        = 0;
      long    j        = 0;

   /* --------------------- */
 
 
   if ( servTable == NULL )
    {
	return BMD_ERR_PARAM1; 
    }
  

   for( i=0; servTable[i] != NULL; i++ )
    {
        for( j = atoi((((logserv_t**)(servTable))[i])->attempt); j>1; j--)
	 {
            intRet = bmdnet_connect( handler, (((logserv_t**)(servTable))[i])->ip, \
 	                             atoi((((logserv_t**)(servTable))[i])->port));
 	                             
	    if ( intRet == 0 ) return i;
	    sleep( atoi((((logserv_t**)(servTable))[i])->delay) );
	 }

        intRet = bmdnet_connect( handler, (((logserv_t**)(servTable))[i])->ip, \
 	                         atoi((((logserv_t**)(servTable))[i])->port));
	    
	if ( intRet == 0 ) return i;
    }
   
  return -1;  
 
}

/**
 * Funkcja nawiązuje połączenie z podrzędnymi serwerammi logów
 * 
 * @param[in] handler - uchwyt połączenia
 * @param[in] servTable - tablica dostępnych serwerów
 *
 * @return W przypadku błędu zwracany jest komunikat za pomocą stosu błędów
 *
*/


long Connect2BMDLogServer( bmdnet_handler_ptr const handler, void** const servTable )
{

   /* --------------------- */

      long    intRet   = 0;
      long    i        = 0;
      long    j        = 0;

   /* --------------------- */
 
 
   if ( servTable == NULL )
    {
	return BMD_ERR_PARAM1; 
    }
  

   for( i=0; servTable[i] != NULL; i++ )
    {
        for( j = atoi(((((logServParam_t**)(servTable))[i])->serverParam).attempt); j>1; j--)
	 {
            intRet = bmdnet_connect( handler, ((((logServParam_t**)(servTable))[i])->serverParam).ip, \
 	                             atoi(((((logServParam_t**)(servTable))[i])->serverParam).port) );
	    
	    if ( intRet == 0 ) return i;
	    sleep( atoi(((((logServParam_t**)(servTable))[i])->serverParam).delay) );
	 }

        intRet = bmdnet_connect( handler, ((((logServParam_t**)(servTable))[i])->serverParam).ip, \
 	                             atoi(((((logServParam_t**)(servTable))[i])->serverParam).port) );
	    
	if ( intRet == 0 ) return i;
    }
   
  return -1;  
 
}

/**
 * Funkcja nawiązuje połączenie z danym serwerem logów
 * 
 * @param[in] log_tLog - uchwyt węzła logu
 *
 * @return W przypadku błędu zwracany jest komunikat za pomocą stosu błędów
 *
*/

err_t LogConnect( bmdnet_handler_ptr* const handler, \
                  long (*connectFunc)(bmdnet_handler_ptr const, void** const), \
		  void** const serversTable, long *const index )
{

   /* -------------------- */

      long   intRet   = 0;
      err_t  err_tRet = 0; 

   /* -------------------- */
 
   if ( !(*handler) )
    {
       intRet = bmdnet_create( handler );

       if ( intRet < 0 )
        {
           BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
           BMD_SETIERR( err_tRet, AD, "Błąd przy tworzeniu gniazda" );
        }
    }

   intRet = connectFunc( (*handler), serversTable );

   if ( intRet < 0 )
    {
       BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED ); 
       BMD_SETIERR( err_tRet, AD, "Połączenie z dowolnym serwerem logów nie powiodło się" );
    }
   
   *index = intRet;

  return err_tRet;  
 
}

/**
 * Funkcja odpowiedzialna za wysyłanie logu do
 * odpowiedniego serwera logów
 *
 * @param[in] log_tLog - uchwyt do struktury log_t
 * @param[in] GenBuf_tBufor - uchwyt do bufora generycznego z 
 *                            transmitowaną zawartością
 *
 * @return W przypadku błędu zwracany jest komunikat za pomocą stosu błędów
 *
*/

err_t LogSendLoop( bmdnet_handler_ptr* const handler, \
                  long (*connectFunc)(bmdnet_handler_ptr const, void** const), \
		  void** const serversTable, GenBuf_t* const GenBuf_tBufor )
{


    /* ------------------------------------- */

       err_t        err_tRet         =    0;
       long         intSendBytes     =    0;
       long         intRecvBytes     =    0;
       long         intLogAck        =    0;
       static long  intIndex         =    0;
       void**       serversTableCopy = NULL;

    /* ------------------------------------- */



    err_tRet = CopyServersTable( serversTable, &serversTableCopy );

    if ( BMD_ERR( err_tRet ) )
     {
    	return err_tRet;
     }
 
 /* --------------------------------------------- WYSYLKA LOGWOW ----------------------------------------------- */ 

    PRINT_VDEBUG("Oczekiwanie na odpowiedź serwera...\n");

    for( ; *serversTableCopy ; )
     {
        intSendBytes = (*handler) ? bmdnet_send( (*handler), &(GenBuf_tBufor->size), sizeof(long)) : -2;

        if ( intSendBytes < 0 )
        {
            if ( intSendBytes == -1 )
            {
            	goto CHANGE_SERV;
            }

		err_tRet = LogConnect( handler, connectFunc, serversTableCopy, &intIndex );
	
	    if ( BMD_ERR( err_tRet ))
	     {
	        break;
	     }
	    continue;
         }
        else if ( intSendBytes < sizeof(long) )
	 {
	    continue;
    	 }
	
        intSendBytes = (*handler) ? bmdnet_send( (*handler), GenBuf_tBufor->buf, GenBuf_tBufor->size ) : -2;


        if ( intSendBytes < 0 )
         {

            if ( intSendBytes == -1 )
             {
		goto CHANGE_SERV;
             }

    	    err_tRet = LogConnect( handler, connectFunc, serversTableCopy, &intIndex );
	
	    if ( BMD_ERR( err_tRet ))
	     {
	        break;
	     }
	    continue;
         }
        else if ( intSendBytes < GenBuf_tBufor->size )
	 {
	    continue;
    	 }

/* --------------------------------------------------------------------------------------------------------------- */

/* ----------------------------------------- OCZEKIWANIE NA POTWIERDZENIE ---------------------------------------- */

        intRecvBytes = bmdnet_recv( (*handler), &intLogAck, sizeof(long));

        if ( intRecvBytes < 0 || intRecvBytes < sizeof(long) )
         {
  
  CHANGE_SERV :

            bmdnet_close( handler );

            err_tRet = ChangeServersTable( serversTableCopy, intIndex );

            if ( BMD_ERR( err_tRet ) )
             {
               break;
             }
         }
        else 
         {
            PRINT_VDEBUG("Operacja zakończona sukcesem\n");

            goto EXIT;
         }
     }

/* --------------------------------------------------------------------------------------------------------------- */

    BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
    BMD_SETIERR( err_tRet, AD, "Zapis logu zakończony niepowodzeniem" );

  EXIT :

    free0( serversTableCopy );

  return err_tRet;

}

/**
 * Funkcja powiela tablicę dostępnych serwerów logów
 *
 * @param[in]  sourceServersTable - tablica serwerów 
 * @param[out] destServersTable - powielona tablica serwerów 
 *
 * @return W przypadku błędu zwracany jest komunikat za pomocą stosu błędów
 *
*/

err_t CopyServersTable( void** const sourceServersTable, void*** const destServersTable )
{

   /* ------------------- */

      err_t err_tRet = 0;
      long  i        = 0;
      long  intCount = 0;

   /* ------------------- */


   if ( sourceServersTable == NULL )
    {
        BMD_SETERR( err_tRet, BMD_ERR_PARAM1 );
        BMD_SETIERR( err_tRet, AD, "Niepoprawny wskaźnik do tablicy serwerów" );
     
        return err_tRet;
    } 


   if ( destServersTable == NULL || *destServersTable != NULL )
    {
        BMD_SETERR( err_tRet, BMD_ERR_PARAM2 );
        BMD_SETIERR( err_tRet, AD, "Niepoprawny wskaźnik do tablicy docelowej" );
     
        return err_tRet;
    } 
 
   for( i=0, intCount=0; sourceServersTable[i] ; intCount++, i++ );

   (*destServersTable) = (void**)calloc( intCount+1, sizeof( void* ) );

   if ( *destServersTable == NULL )
    {
        BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
        BMD_SETIERR( err_tRet, AD, "Błąd alokacji pamięci" );
    
        return err_tRet;
    }

   for( i=0; sourceServersTable[i]; i++ )
    {
       (*destServersTable)[i] = sourceServersTable[i];
    }


  return err_tRet;

}


/**
 * Funkcja usuwa z tablicy serwer o danym indeksie
 *
 * @param[in] ServersTable  - uchwyt do tablicy serwerów
 * @param[in] index         - indeks usuwanego serwera
 *
 * @return W przypadku błędu zwracany jest komunikat za pomocą stosu błędów
 *
*/

err_t ChangeServersTable( void** const ServersTable, long index )
{

   /* ------------------- */

      err_t err_tRet = 0;

   /* ------------------- */


   if ( ServersTable == NULL || *ServersTable == NULL )
    {
        BMD_SETERR( err_tRet, BMD_ERR_PARAM1 );
        BMD_SETIERR( err_tRet, AD, "Niepoprawny wskaźnik do tablicy serwerów" );
     
        return err_tRet;
    } 


   if ( index < 0 )
    {
        BMD_SETERR( err_tRet, BMD_ERR_PARAM2 );
        BMD_SETIERR( err_tRet, AD, "Niepoprawny indeks tablicy serwerów" );
     
        return err_tRet;
    } 

   for( ; ServersTable[index] != NULL ; index++ )
    {
      if ( ServersTable[index+1] != NULL )
       {
          ServersTable[index] = ServersTable[index+1];
       }
      else
       {
          ServersTable[index] = NULL;
       }
    }

  return err_tRet;
}


/**
 * Funkcja generuje unikalną nazwę pliku dla logu  w przypadku 
 * braku połączenia z serwerem logów
 *
 * @param[out] stringFileName - wygenerowana nazwa pliku
 *
 * @return W przypadku błędu zwracany jest komunikat za 
 *         pomocą stosu błędów
 *
*/


err_t GenerateFileName( const char* const temp_directory, char** const stringFileName )
{


     /* ------------------------------------ */

        char*           stringTmp    = NULL;
        long            intRet       =    0;
        err_t           err_tRet     =    0;
	DIR*            DIRDirectory = NULL;
	struct timeval                   tv;
        time_t                   time_tTime;
	extern int                    errno;
     
     /* ------------------------------------ */

    
     if ( temp_directory == NULL )
      {
         BMD_SETERR( err_tRet, BMD_ERR_PARAM1 );
         BMD_SETIERR( err_tRet, AD, "Niepoprawny pierwszy parametr" );
    
         return err_tRet;
      }

     if ( stringFileName == NULL || *stringFileName != NULL )
      {
         BMD_SETERR( err_tRet, BMD_ERR_PARAM2 );
         BMD_SETIERR( err_tRet, AD, "Niepoprawny drugi parametr" );
    
         return err_tRet;
      }
     
     time( &time_tTime );
     intRet = gettimeofday( &tv, NULL );
     
     if ( intRet < 0 )
      {
         BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
         BMD_SETIERR( err_tRet, AD, "Błąd pobierania dokładnego czasu" );
    
         return err_tRet;
      }

     DIRDirectory = opendir( temp_directory );
     
     if ( DIRDirectory == NULL )
      {
         if ( errno == ENOENT )
	  {
	     intRet = mkdir( temp_directory, 0777 );
	     
	     if ( intRet < 0 )
	      {
	           BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
                   BMD_SETIERR( err_tRet, AD, "Błąd podczas tworzenia katalogu" );
    
                   return err_tRet;
 	      }
	  }
         else
	  {
	           BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
                   BMD_SETIERR( err_tRet, AD, "Błąd otwarcia katalogu" );
    
                   return err_tRet;
	  }
      }
     else
      {
    	 closedir( DIRDirectory );
      }
     
       
     intRet = asprintf( &stringTmp, "%s%s%s", temp_directory, "/", asctime(localtime(&time_tTime)) );
     
     if ( intRet == -1 )
      {
         BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
         BMD_SETIERR( err_tRet, AD, "Błąd alokacji pamięci" );
    
         return err_tRet;
      }
     
     stringTmp[ strlen(stringTmp)-1 ] = '\0';

     intRet = asprintf( &(*stringFileName), "%s%li%li", stringTmp, (long)(tv.tv_sec), (long)(tv.tv_usec) );
     free( stringTmp );

     if ( intRet == -1 )
      {
         BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
         BMD_SETIERR( err_tRet, AD, "Błąd alokacji pamięci" );
    
         return err_tRet;
      }

  return err_tRet;

}

/**
 * Funkcja dokonuje zrzutu danego logu na dysk
 *
 * @param[in] GenBuf_tBufor - uchwyt do generycznego bufora z 
 *                            zawartością do zapisu
 *
 
*/

err_t SaveLogOnDisk( const char* const temp_directory, GenBuf_t* const GenBuf_tBufor )
{
   
   /* ----------------------------- */

      err_t  err_tRet       =    0;
      char*  stringFileName = NULL;
      long   intRet         =    0;

   /* ----------------------------- */



   if ( temp_directory == NULL )
    {
         BMD_SETERR( err_tRet, BMD_ERR_PARAM1 );
         BMD_SETIERR( err_tRet, AD, "Niepoprawny pierwszy parametr" );
    
         return err_tRet;
    }

   if ( GenBuf_tBufor == NULL )
    {
         BMD_SETERR( err_tRet, BMD_ERR_PARAM1 );
         BMD_SETIERR( err_tRet, AD, "Niepoprawny drugi parametr" );
    
         return err_tRet;
    }

   err_tRet = GenerateFileName( temp_directory, &stringFileName );

   if ( BMD_ERR( err_tRet ) )
    {
	return err_tRet;
    }

   intRet = bmd_save_buf( GenBuf_tBufor, stringFileName );
   free0( stringFileName );

   if ( intRet < 0 )
    {
         BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
         BMD_SETIERR( err_tRet, AD, "Błąd zapisu logu na dysku" );
    }

  return err_tRet;
}

/**
 * Funkcja odczytuje zawartość katalogu w którym przechowywane
 * są tymczasowo logi oczekaujące na najbliższą możliwość
 * wysyłki
 *
 * @param[in] log_tLog - uchwyt na strukturę logu
*/



err_t LoadLogFromDisk( const char* const temp_directory, bmdnet_handler_ptr* const handler, \
                       long (*connectFunc)(bmdnet_handler_ptr const, void** const), \
		       void** const serversTable )
{

    
   /* --------------------------------------- */
    
      long            intRet          =    0;
      err_t           err_tRet        =    0;
      struct direct*  directDir       = NULL;
      DIR*            DIRDirectory    = NULL;
      char*           stringFullFName = NULL;
      GenBuf_t*       GenBuf_tBufor   = NULL;
      extern int                       errno; 
      
   /* --------------------------------------- */
   
             
    
    DIRDirectory = opendir( temp_directory );
 
    if ( DIRDirectory == NULL )
     {
         BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
         BMD_SETIERR( err_tRet, AD, "Błąd otwarcia katalogu z logami" );
	 
	 return err_tRet;
     }

    while (( directDir = readdir(DIRDirectory)) != 0)  
     {
      if ( directDir->d_type != DT_DIR )
       { 
            intRet = asprintf( &stringFullFName, "%s%s%s", temp_directory, "/", directDir->d_name);
    	    
	    if ( intRet == -1 )
	     {
	      	BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
              	BMD_SETIERR( err_tRet, AD, "Błąd alokacji pamięci" );
	 
	      	return err_tRet;
	     }
	    
	    intRet = bmd_load_binary_content( stringFullFName, &GenBuf_tBufor );
	    
	    if ( intRet < 0 )
	     {
	     	BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
              	BMD_SETIERR( err_tRet, AD, "Błąd odczytu pliku logu z dysku" );
	 
	      	return err_tRet;
	     }
	    
	    err_tRet = LogSendLoop( handler, connectFunc, serversTable, GenBuf_tBufor );
	    
	    if ( !BMD_ERR ( err_tRet ) )
	     {
	     	intRet = unlink( stringFullFName );
		
		if ( intRet < 0 )
		 {
		     BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
              	     BMD_SETIERR( err_tRet, AD, "Błąd usunięcia pliku logu" );
	       	 }
	     }
	    
	    free( stringFullFName );
	    free_gen_buf( &GenBuf_tBufor );
       }
     }

    closedir( DIRDirectory );

   
  return err_tRet;
}

/**
 * Funkcja inicjuje strukturę przechowywującą parametry
 * główengo serwera logów
 *
 * @param[in] rootServParam_tParams - wskażnik do struktury
 *                               przechowywującej parametry
 * 
 * @return W przypadku błędu zwracany jest komunikat za pomocą
 *         stosu błędów
 */

err_t rootServerParamsInit( rootServParam_t* const rootServParam_tParams )
{

   /* ------------------- */
    
      err_t err_tRet = 0;
 
   /* ------------------- */



      if ( rootServParam_tParams == NULL )
       {
    	    BMD_SETERR( err_tRet, BMD_ERR_PARAM1 );
    	    BMD_SETIERR( err_tRet, AD, "Niepoprawny pierwszy parametr");
   
    	    return err_tRet;
       }	

       /* -------------------------------------------------- */
       /* Inicjowanie struktury parametrów serwera */
       /* -------------------------------------------------- */
  
          rootServParam_tParams->ip                  = NULL;
          rootServParam_tParams->port                = NULL;
          rootServParam_tParams->temp_directory      = NULL;
          rootServParam_tParams->log_default_level   = NULL;
          rootServParam_tParams->log_accepting_level = NULL;
          rootServParam_tParams->balanceFunc         = NULL;
          rootServParam_tParams->LogServersTab       = NULL;
          rootServParam_tParams->logPolicyTab        = NULL;

          INIT( &(rootServParam_tParams->LoadList) );
          INIT( &(rootServParam_tParams->ConnectionList) );
  
       /* -------------------------------------------------- */
  

   return err_tRet;

}


/**
 * Funkcja kopiuje nowe parametry głównego serwera logów
 *
 * @param[in] source - nowe ustawienia
 * @param[in] paramCase_tParam - rodzaj wykonywanej akcji (up_server, change_params)
 * @param[out] dest - stare ustawienia
 *
 * @return W przypadku błędu zwracany jest komunikat za pomocą stosu błędów
 *
 */


err_t rootServerParamsCpy( rootServParam_t* const source, const paramCase_t paramCase_tParam, \
                           rootServParam_t* const dest )
{


   /* -------------------- */

      long   i        = 0;
      long   intSize  = 0;
      err_t  err_tRet = 0;

   /* -------------------- */
 
 
 
      if ( source == NULL )
       {
            BMD_SETERR( err_tRet, BMD_ERR_PARAM1 );
            BMD_SETIERR( err_tRet, AD, "Nie podano adresu źródła" );
     
            return err_tRet;
       }
      
      if ( dest == NULL ) 
       {
            BMD_SETERR( err_tRet, BMD_ERR_PARAM2 );
            BMD_SETIERR( err_tRet, AD, "Nie podano adresu celu" );
     
            return err_tRet;
       }


      switch( paramCase_tParam )
       {

          case up_server     : {
	                         free0( dest->ip );
				 free0( dest->port );
                                 free0( dest->temp_directory );
				 
	                         dest->ip   = source->ip;
				 dest->port = source->port;
	                         dest->temp_directory = source->temp_directory;
				 dest->logPolicyTab = source->logPolicyTab;
	                       }
	  		   
	  case change_params : {
	                         free0( dest->log_default_level );
                                 free0( dest->log_accepting_level ) ;

                                 dest->log_default_level = source->log_default_level;
				 dest->log_accepting_level = source->log_accepting_level;
                                 dest->balanceFunc = source->balanceFunc;
 
                                 if ( dest->LogServersTab )
                                  {
                                    for(i=0; (dest->LogServersTab)[i] != NULL; i++)
                                     {
		                        free0( ((dest->LogServersTab)[i])->servID );
		                        free0( (((dest->LogServersTab)[i])->serverParam).ip );
		                        free0( (((dest->LogServersTab)[i])->serverParam).port );
		                        free0( (((dest->LogServersTab)[i])->serverParam).attempt );
		                        free0( (((dest->LogServersTab)[i])->serverParam).delay );
		                        free0( (dest->LogServersTab)[i] );
                                     }
                                  }	

				 free0( dest->LogServersTab ); 

                                 for( i=0, intSize=0; (source->LogServersTab)[i] != NULL; \
				      i++, intSize++);


                                 dest->LogServersTab = ( logServParam_t** )calloc( intSize+1, \
                                                       sizeof( logServParam_t* ));

                                 if ( dest->LogServersTab == NULL )
                                  {
                                      BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
                                      BMD_SETIERR( err_tRet, AD, "Błąd alokacji pamięci dla tablicy serwerów logów" );

				      return err_tRet;
				  }

                                 for( i=0; (source->LogServersTab)[i] != NULL; i++ )
				  {
				      (dest->LogServersTab)[i] = (source->LogServersTab)[i];
				  }
			       }
       
          default : {}
       
       }
 
   return err_tRet;

}

/**
 * Funkcja pobiera parametry głównego serwera logów z pliku
 * konfiguracyjnego
 *
 * @param[in]  stringFileName - ścieżka do pliku konfiguracyjnego
 * @param[in]  paramCase_tParam - rodzaj wykonywanej akcji
 * @param[out] rootServParam_tParams - wskaźnik na strukturę
 *                                     opisującą parametry
 *
 * @return W przypadku błędu zwracany jest komunikat za pomocą
 *         stosu błędów
 */


err_t getRootServerParams( const char* const stringFileName, \
                           const paramCase_t paramCase_tParam, \
                           rootServParam_t* const rootServParam_tParams )
 {

 
      /* ---------------------------------------------------- */

         long            intRet                    =       0;
         long            intSize                   =       0;
	 long            intOldServerListStatus    =       0;
         long            intLoadServerStatus       =       0;
         long            intDoubleServers          =       0;
	 long            i                         =       0;
	 long            j                         =       0;
         long            k                         =       0;
	 char*           stringKomunikat           =    NULL;
	 char*           stringBalanceStrategy     =    NULL;
         char*           stringRunningServers      =    NULL;
	 char**          stringRunningServersTable =    NULL;
         char*           dbIP                      =    NULL;
	 char*           dbPORT                    =    NULL;
	 char*           dbNAME                    =	NULL;
	 char*           dbUSER                    =    NULL;
         char*           dbPASSWD		   =	NULL;
         char*		 dbLIBRARY		   =    NULL;
	 void*           dbHANDLER		   =    NULL;
	 bmd_conf*       bmdconfHandle             =    NULL;
         err_t           err_tRet                  =       0;
         cLevel_t        cLevel_tWHERE             = nowhere;
         rootServParam_t                  rootServParam_tTMP;

      /* ---------------------------------------------------- */


      
      if ( stringFileName == NULL )
       {
            BMD_SETERR( err_tRet, BMD_ERR_PARAM1 );
            BMD_SETIERR( err_tRet, AD, "Nie podano lokalizacji pliku konfiguracyjnego" );
     
            return err_tRet;
       }
      
      if ( rootServParam_tParams == NULL ) 
       {
            BMD_SETERR( err_tRet, BMD_ERR_PARAM3 );
            BMD_SETIERR( err_tRet, AD, "Nieprawidłowy trzeci parametr" );
     
            return err_tRet;
       }
      
      err_tRet = rootServerParamsInit( &rootServParam_tTMP );

      if ( BMD_ERR( err_tRet ) )
       {
            return err_tRet;
       }

      intRet = bmdconf_init( &bmdconfHandle );
      
      if ( intRet < 0 )
       {
            BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
            BMD_SETIERR( err_tRet, AD, "Błąd inicjowania pliku konfiguracyjnego" );
     
            return err_tRet;
       } 

      intRet = bmdconf_set_mode( bmdconfHandle, BMDCONF_TOLERANT );
      
      if ( intRet < 0 )
       {
            BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
            BMD_SETIERR( err_tRet, AD, "Błąd zmiany ustawień odczytu słownika" );
     
            goto GARBAGE;
       }

      intRet = bmdconf_add_section2dict( bmdconfHandle, "ROOT_SERVER", root_dic, BMDCONF_SECTION_REQ );

      if ( intRet < 0 )
       {
            BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
            BMD_SETIERR( err_tRet, AD, "Błąd przy dodawaniu sekcji do słownika" );

            goto GARBAGE;
       }

      intRet = bmdconf_add_section2dict( bmdconfHandle, "DATA_BASE", database_dic, BMDCONF_SECTION_REQ );

      if ( intRet < 0 )
       {
            BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
            BMD_SETIERR( err_tRet, AD, "Błąd przy dodawaniu sekcji do słownika" );

            goto GARBAGE;
       }

      intRet = bmdconf_load_file( stringFileName, bmdconfHandle, &stringKomunikat );

      if ( intRet < 0 )
       {
            BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
            BMD_SETIERR( err_tRet, AD, stringKomunikat );

            goto GARBAGE;
       }

      free0( stringKomunikat );

      switch( paramCase_tParam )
       {

         case up_server : {

            intRet = bmdconf_get_value( bmdconfHandle, "ROOT_SERVER", "IP", &(rootServParam_tTMP.ip) );

            if ( intRet < 0 )
             {
        	BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
                BMD_SETIERR( err_tRet, AD, "Błąd odczytu numeru IP serwera z pliku konfiguracyjnego" );
		cLevel_tWHERE = clevel1;

                goto GARBAGE;
             }

            intRet = bmdconf_get_value( bmdconfHandle, "ROOT_SERVER", "PORT", &(rootServParam_tTMP.port) );

            if ( intRet < 0 )
             {
       	        BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
                BMD_SETIERR( err_tRet, AD, "Błąd odczytu numeru PORTU serwera z pliku konfiguracyjnego" );
		cLevel_tWHERE = clevel1;

                goto GARBAGE;
             }
             
            /*
            intRet = bmdconf_get_value( bmdconfHandle, "ROOT_SERVER", "TEMP_DIRECTORY", \
                                        &(rootServParam_tTMP.temp_directory) );

            if ( intRet < 0 )
             {
                BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
                BMD_SETIERR( err_tRet, AD, "Błąd odczytu lokalizacji katalogu tymczasowgo" );
     		cLevel_tWHERE = clevel1;

                goto GARBAGE;
             }
	     */

            intRet = bmdconf_get_value( bmdconfHandle, "DATA_BASE", "IP", &dbIP );

            if ( intRet < 0 )
             {
                BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
                BMD_SETIERR( err_tRet, AD, "Błąd odczytu ip bazy danych" );
     		cLevel_tWHERE = clevel1;

                goto GARBAGE;
             }

            intRet = bmdconf_get_value( bmdconfHandle, "DATA_BASE", "PORT", &dbPORT );

            if ( intRet < 0 )
             {
                BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
                BMD_SETIERR( err_tRet, AD, "Błąd odczytu portu bazy danych" );
     		cLevel_tWHERE = clevel1;

                goto GARBAGE;
             }

            intRet = bmdconf_get_value( bmdconfHandle, "DATA_BASE", "BASE_NAME", &dbNAME );

            if ( intRet < 0 )
             {
                BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
                BMD_SETIERR( err_tRet, AD, "Błąd odczytu nazwy bazy danych" );
     		cLevel_tWHERE = clevel1;

                goto GARBAGE;
             }

            intRet = bmdconf_get_value( bmdconfHandle, "DATA_BASE", "USER", &dbUSER );

            if ( intRet < 0 )
             {
                BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
                BMD_SETIERR( err_tRet, AD, "Błąd odczytu użytkownika bazy danych" );
     		cLevel_tWHERE = clevel1;

                goto GARBAGE;
             }

            intRet = bmdconf_get_value( bmdconfHandle, "DATA_BASE", "PASSWORD", &dbPASSWD );

            if ( intRet < 0 )
             {
                BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
                BMD_SETIERR( err_tRet, AD, "Błąd odczytu użytkownika bazy danych" );
     		cLevel_tWHERE = clevel1;

                goto GARBAGE;
             }

	    intRet = bmdconf_get_value( bmdconfHandle, "DATA_BASE", "DB_LIBRARY", &dbLIBRARY );

	    if ( intRet < 0 )
	     {                                                                                                                                                        
		asprintf(&dbLIBRARY, "libbmddb_postgres.so");
	     }                                                                                                                                                        
	    intRet = bmd_db_init(dbLIBRARY);
	    
	    if ( intRet != BMD_OK )
	     {
                BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
		BMD_SETIERR( err_tRet, AD, "Błąd inicjowania biblioteki obsługi bazy danych" );
		
		goto GARBAGE;
             }
	    //printf("bmd_db_connect2_log_server_utils\n");
	    intRet = bmd_db_connect2( dbIP, dbPORT, dbNAME, dbUSER, dbPASSWD, &dbHANDLER );

            if ( intRet < 0 )
             {
                BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
                BMD_SETIERR( err_tRet, AD, "Błąd połączenia z bazą danych" );
     		cLevel_tWHERE = clevel1;

                goto GARBAGE;
             }

            /* Wywołanie funkcji odczytującej politykę logów z bazy danych  */

            err_tRet = getEventsPolicy( dbHANDLER , &(rootServParam_tTMP.logPolicyTab) );

            if ( BMD_ERR( err_tRet ) )
             {
                cLevel_tWHERE = clevel1;

                goto GARBAGE;
             }

         }

         case change_params : {

            intRet = bmdconf_get_value( bmdconfHandle, "ROOT_SERVER", "LOG_DEFAULT_LEVEL", \
                                        &(rootServParam_tTMP.log_default_level) );

            if ( intRet < 0 )
             {
       	        BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
                BMD_SETIERR( err_tRet, AD, "Błąd odczytu domyślnego poziomu logu" );
     		cLevel_tWHERE = clevel1;

                goto GARBAGE;
             }
	
	   /*
            intRet = bmdconf_get_value( bmdconfHandle, "ROOT_SERVER", "LOG_ACCEPTING_LEVEL", \
                                        &(rootServParam_tTMP.log_accepting_level) );
   
            if ( intRet < 0 )
             {
       	        BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
                BMD_SETIERR( err_tRet, AD, "Błąd odczytu akceptowalnego poziomu logu" );
     		cLevel_tWHERE = clevel1;

                goto GARBAGE;
             }
	   */
	   
            intRet = bmdconf_get_value( bmdconfHandle, "ROOT_SERVER", "BALANCE_STRATEGY", \
                                        &stringBalanceStrategy );
   
            if ( intRet < 0 )
             {
    	        BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
                BMD_SETIERR( err_tRet, AD, "Błąd odczytu rodzaju algorytmu szeregowania" );
     		cLevel_tWHERE = clevel1;

                goto GARBAGE;
             }

            /*
              Tutaj należy na podstawie zmiennej "stringBalanceStrategy" wybrać odpowiedni
	      algorytm zarządzania obciążeniem. To będzie do poprawienia !!!!
            */

            rootServParam_tTMP.balanceFunc = FirstFree;

            intRet = bmdconf_get_value( bmdconfHandle, "ROOT_SERVER", "RUNNING_SERVERS", \
                                        &stringRunningServers );
            if ( intRet < 0 )
             {
    	        BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
                BMD_SETIERR( err_tRet, AD, "Błąd odczytu aktywnych serwerów logów" );
     		cLevel_tWHERE = clevel1;

                goto GARBAGE;
             }

            err_tRet = GetSubstring( stringRunningServers, "[a-zA-Z0-9_]+", &stringRunningServersTable );
       
            if ( BMD_ERR( err_tRet ) )
             {
	        BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
                BMD_SETIERR( err_tRet, AD, "Błąd w trakcie wyszukiwania podnapisów" );
		cLevel_tWHERE = clevel1;
	     
	        goto GARBAGE;
	     }     

            for( i=0, intSize=0; stringRunningServersTable[i] != NULL; i++, intSize++);


            rootServParam_tTMP.LogServersTab = ( logServParam_t** )calloc( intSize+1, \
                                                sizeof( logServParam_t* ));

            if ( rootServParam_tTMP.LogServersTab == NULL )
             {
                BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
                BMD_SETIERR( err_tRet, AD, "Błąd alokacji pamięci dla tablicy serwerów logów" );
		cLevel_tWHERE = clevel1;

	        goto GARBAGE;
             }

            for( i=0, j=0; stringRunningServersTable[i] != NULL; i++ )
	     {
	      
	       /* sprawdzenie czy serwera nie ma na starej liście */
	       /* jeśli nie ma to !intStatus == 0                 */


 	       for( k=0, intOldServerListStatus = 1; \
		    (rootServParam_tParams->LogServersTab != NULL) && \
		    ((rootServParam_tParams->LogServersTab)[k] != NULL) && \
	            ( intOldServerListStatus = (strcmp((rootServParam_tParams->LogServersTab[k])->servID, \
		                                        stringRunningServersTable[i] ) != 0) ? \
		  			       1 : 0 \
		    ); k++ \
		  );  
		  
               // sprawdzamy czy przypadkiem nie mamy uruchomionego serwera o takim samym identyfikatorze

	       intLoadServerStatus = ( FIND( &(rootServParam_tParams->LoadList), CmpLoadElem, \
				             stringRunningServersTable[i], forward, NULL ) > 0) ? 1 : 0;


               // chwilowo nie potrzebne ponieważ nie uruchomiono jescze mechanizmu balancera


               if ( intLoadServerStatus && intOldServerListStatus )
	        {
	    	     BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
        	     BMD_SETIERR( err_tRet, AD, "Serwer ma aktywne zadania i nie ma go na starej liście" );
	             
		     continue;
		}

               if ( !intLoadServerStatus || !intOldServerListStatus )
	        {

 	          intRet = bmdconf_add_section2dict( bmdconfHandle, stringRunningServersTable[i], \
	                                                     sec_serv_dic, BMDCONF_SECTION_REQ );

                  if ( intRet < 0 )
                   {
            	       BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
        	       BMD_SETIERR( err_tRet, AD, "Błąd przy dodawaniu sekcji do słownika" );
                   
		       continue;
		   }


  	          intRet = bmdconf_load_file( stringFileName, bmdconfHandle, &stringKomunikat );

                  if ( intRet < 0 )
                   {
        	       BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
        	       BMD_SETIERR( err_tRet, AD, stringKomunikat );

		       continue;
		   }
	     
	     	  free0( stringKomunikat ); 
		
	          ( rootServParam_tTMP.LogServersTab )[j] = ( logServParam_t* )calloc( 1, \
                                                               sizeof( logServParam_t ));
	
	          if ( (rootServParam_tTMP.LogServersTab)[j] == NULL )
                   {
                	BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
        		BMD_SETIERR( err_tRet, AD, "Błąd alokacji pamięci" );
                   	cLevel_tWHERE = clevel1;

		        goto GARBAGE;
		   }
		

		  intRet = asprintf( &((( rootServParam_tTMP.LogServersTab )[j])->servID), "%s", \
		                     stringRunningServersTable[i] );
		
		  if ( intRet < 0 )
		   {
		       BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
                       BMD_SETIERR( err_tRet, AD, "Błąd alokacji pamięci" );
                       cLevel_tWHERE = clevel1;

		       goto GARBAGE;
		   }
				
		  intRet = bmdconf_get_value( bmdconfHandle, stringRunningServersTable[i], "ATTEMPT", \
	                                      &((((rootServParam_tTMP.LogServersTab)[j])->serverParam).attempt));

                  if ( intRet < 0 )
                   {
                       BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
                       BMD_SETIERR( err_tRet, AD, "Błąd w trakcie pobierania ilości prób połączenia" );
                   
		       /* trezba zwolnić id */
		       
                       free0( ((rootServParam_tTMP.LogServersTab)[j])->servID );
		       free0( ( rootServParam_tTMP.LogServersTab )[j] );
		   
		       continue;
		   }
	 

                  intRet = bmdconf_get_value( bmdconfHandle, stringRunningServersTable[i], "DELAY", \
	                                      &((((rootServParam_tTMP.LogServersTab)[j])->serverParam).delay));

                  if ( intRet < 0 )
                   {
                       BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
                       BMD_SETIERR( err_tRet, AD, "Błąd w trakcie pobierania odstępu czasowego" \
	                                          "pomiędzy próbami połącznia" );

                       free0( (((rootServParam_tTMP.LogServersTab)[j])->serverParam).attempt );
                       free0( ((rootServParam_tTMP.LogServersTab)[j])->servID );
                       free0( ( rootServParam_tTMP.LogServersTab )[j] );
                       
		       continue;
		   }
		} 

               if ( !intLoadServerStatus )
	        {

                  intRet = bmdconf_get_value( bmdconfHandle, stringRunningServersTable[i], "IP", \
	                                      &((((rootServParam_tTMP.LogServersTab)[j])->serverParam).ip));

                  if ( intRet < 0 )
                   {
                       BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
                       BMD_SETIERR( err_tRet, AD, "Błąd w trakcie pobierania numeru IP serwera logów" );
                   
                       free0( (((rootServParam_tTMP.LogServersTab)[j])->serverParam).delay );
                       free0( (((rootServParam_tTMP.LogServersTab)[j])->serverParam).attempt );
                       free0( ((rootServParam_tTMP.LogServersTab)[j])->servID );
		       free0( ( rootServParam_tTMP.LogServersTab )[j] );
		   
		       continue;
		   }

                  intRet = bmdconf_get_value( bmdconfHandle, stringRunningServersTable[i], "PORT", \
	                                      &((((rootServParam_tTMP.LogServersTab)[j])->serverParam).port));


                  if ( intRet < 0 )
                   {
                       BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
                       BMD_SETIERR( err_tRet, AD, "Błąd w trakcie pobierania numeru PORTU serwera logów" );
		   }

		  if ( intRet == 0 )
		   {	        

                     for( k=0, intDoubleServers=1; \
		          k<j && (intDoubleServers = ( strcmp( (((rootServParam_tTMP.LogServersTab)[k])->serverParam).ip, \
			                                       (((rootServParam_tTMP.LogServersTab)[j])->serverParam).ip \
							     ) != 0 \
						     ) || \
			  			     ( strcmp( (((rootServParam_tTMP.LogServersTab)[k])->serverParam).port, \
						               (((rootServParam_tTMP.LogServersTab)[j])->serverParam).port \
							     ) != 0 \
						     ) ? \
			                             1 : 0 \
			         ); \
		          k++ ); 

		     
		     if ( rootServParam_tParams->LogServersTab && intDoubleServers )
		      {
                        for( k=0; \
		             ((rootServParam_tParams->LogServersTab)[k] != NULL) && \
		             (intDoubleServers = ( strcmp( (((rootServParam_tParams->LogServersTab)[k])->serverParam).ip, \
		                                           (((rootServParam_tTMP.LogServersTab)[j])->serverParam).ip \
							 ) != 0 \
						 ) || \
  			  		         ( strcmp( (((rootServParam_tParams->LogServersTab)[k])->serverParam).port, \
  					                   (((rootServParam_tTMP.LogServersTab)[j])->serverParam).port \
							 ) != 0 \
                                                 ) || \
                                                 ( strcmp( ((rootServParam_tParams->LogServersTab)[k])->servID, \
						           ((rootServParam_tTMP.LogServersTab)[j])->servID \
						         ) == 0 \
			                         ) ? \
						 1 : 0 \
			     ); \
 	                     k++ );

		      }

		     if (!intDoubleServers) 
		      {
		         BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
                         BMD_SETIERR( err_tRet, AD, "Wykryto próbę zdefiniowania zdublowanych serwerów logów" );
		      }
		   }
		
		  if ( intRet < 0 || (!intDoubleServers) )
		   {
		       free0( (((rootServParam_tTMP.LogServersTab)[j])->serverParam).ip );
		       free0( (((rootServParam_tTMP.LogServersTab)[j])->serverParam).delay );
                       free0( (((rootServParam_tTMP.LogServersTab)[j])->serverParam).attempt );
		       free0( ((rootServParam_tTMP.LogServersTab)[j])->servID );
                       free0( ( rootServParam_tTMP.LogServersTab )[j] );

		       continue;
		   }
		}
	      j++;	
	     }
	}
    }

      if ( j>0 )
       { 
          if ( BMD_ERR(rootServerParamsCpy( &rootServParam_tTMP, paramCase_tParam, \
                                            rootServParam_tParams )) )
           {
               BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
               BMD_SETIERR( err_tRet, AD, "Błąd w trakcie kopiowania parametrów" );
           } 
       }


  /* ----------------------------------------------------------------------------------------- */

    GARBAGE :
  
 
    switch( cLevel_tWHERE )
     {
  
       case clevel1 : { 
  		        free0( rootServParam_tTMP.ip );
                        free0( rootServParam_tTMP.port );
                        free0( rootServParam_tTMP.temp_directory );
                        free0( rootServParam_tTMP.log_default_level );
                        free0( rootServParam_tTMP.log_accepting_level ) ;

                        if ( rootServParam_tTMP.LogServersTab )
                         {
                           for(i=0; (rootServParam_tTMP.LogServersTab)[i] != NULL; i++)
                            {
		              free0( ((rootServParam_tTMP.LogServersTab)[i])->servID );
		              free0( (((rootServParam_tTMP.LogServersTab)[i])->serverParam).ip );
		              free0( (((rootServParam_tTMP.LogServersTab)[i])->serverParam).port );
		              free0( (((rootServParam_tTMP.LogServersTab)[i])->serverParam).attempt );
		              free0( (((rootServParam_tTMP.LogServersTab)[i])->serverParam).delay );
		              free0( (rootServParam_tTMP.LogServersTab)[i] );
                            }
                         }	
                           free0( rootServParam_tTMP.LogServersTab ); 
                       }
  
       case nowhere : {
    			if ( stringRunningServersTable )
                         {
                           for(i=0; stringRunningServersTable[i] != NULL; i++)
                            {
  	                      free0( stringRunningServersTable[i] );
	                    }
                         }

                        free0( stringRunningServersTable );
                        free0( stringBalanceStrategy );
                        free0( stringRunningServers);
                        free0( stringKomunikat );
			free0( dbIP );
			free0( dbPORT );
			free0( dbNAME );
			free0( dbUSER );
			free0( dbPASSWD );

		      }
  
       default : { 
    		    bmdconf_destroy( &bmdconfHandle ); 
    		    bmd_db_disconnect( &dbHANDLER );
    	         }
  
     }

  /* ----------------------------------------------------------------------------------------- */

 
  return err_tRet;

}

/**
 * Funkcja inicjuje strukturę przechowywującą parametry
 * serwera logów warstwy drugiej
 *
 * @param[in] bmdlogServParam_tParams - wskaźnik do struktury
 *                                 przechowywującej parametry
 * 
 * @return W przypadku błędu zwracany jest komunikat za pomocą
 *         stosu błędów
 */

err_t BMDLogServerParamsInit( bmdlogServParam_t* const bmdlogServParam_tParams )
{

   /* ------------------- */
    
      err_t err_tRet = 0;
 
   /* ------------------- */



      if ( bmdlogServParam_tParams == NULL )
       {
    	    BMD_SETERR( err_tRet, BMD_ERR_PARAM1 );
    	    BMD_SETIERR( err_tRet, AD, "Niepoprawny pierwszy parametr");
   
    	    return err_tRet;
       }	

       /* ------------------------------------------------------ */
       /* Inicjowanie struktury parametrów serwera */
       /* ------------------------------------------------------ */

          bmdlogServParam_tParams->ip                    = NULL;
          bmdlogServParam_tParams->temp_directory        = NULL;
          bmdlogServParam_tParams->signContext           = NULL;
          bmdlogServParam_tParams->dbhandle              = NULL;

          (bmdlogServParam_tParams->baseParam).ip        = NULL;
          (bmdlogServParam_tParams->baseParam).port      = NULL;
          (bmdlogServParam_tParams->baseParam).base_name = NULL;
          (bmdlogServParam_tParams->baseParam).user      = NULL;
          (bmdlogServParam_tParams->baseParam).password  = NULL;
  
       /* ------------------------------------------------------ */
  

   return err_tRet;

}


/**
 * Funkcja pobiera parametry serwera logów drugiej warstwy z pliku
 * konfiguracyjnego
 *
 * @param[in]  stringFileName - ścieżka do pliku konfiguracyjnego
 * @param[out] bmdlogServParam_tParams - wskaźnik na strukturę opisującą 
 *             parametry serwera logów
 *
 * @return W przypadku błędu zwracany jest komunikat za pomocą
 *         stosu błędów
 */


err_t getBMDLogServerParams( const char* const stringFileName, \
                             bmdlogServParam_t* const bmdlogServParam_tParams )
{


   /* -------------------------------------- */
   
      long       intRet           =       0;
      char*      stringKomunikat  =    NULL;
      char*      cert_file        =    NULL;
      char*      cert_passwd      =    NULL;
      bmd_conf*  bmdconfHandle    =    NULL;
      cLevel_t   cLevel_tWHERE    = nowhere;
      err_t      err_tRet         =       0;
      char*	 dbLIBRARY	  =    NULL;

   /* -------------------------------------- */
   



      if ( stringFileName == NULL )
       {
            BMD_SETERR( err_tRet, BMD_ERR_PARAM1 );
            BMD_SETIERR( err_tRet, AD, "Nie podano lokalizacji pliku konfiguracyjnego" );
     
            return err_tRet;
       }
      
      if ( bmdlogServParam_tParams == NULL ) 
       {
            BMD_SETERR( err_tRet, BMD_ERR_PARAM2 );
            BMD_SETIERR( err_tRet, AD, "Nieprawidłowy drugi parametr" );
     
            return err_tRet;
       }

      intRet = bmdconf_init( &bmdconfHandle );
      
      if ( intRet < 0 )
       {
            BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
            BMD_SETIERR( err_tRet, AD, "Błąd inicjowania pliku konfiguracyjnego" );
     
            return err_tRet;
       } 
 
      intRet = bmdconf_set_mode( bmdconfHandle, BMDCONF_TOLERANT );
      
      if ( intRet < 0 )
       {
            BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
            BMD_SETIERR( err_tRet, AD, "Błąd zmiany ustawień odczytu słownika" );
     
            goto GARBAGE;
       }

      intRet = bmdconf_add_section2dict( bmdconfHandle, "SERVER", bmdlogserver_dic, BMDCONF_SECTION_REQ );
      
      if ( intRet < 0 )
       {
            BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
            BMD_SETIERR( err_tRet, AD, "Błąd przy dodawaniu sekcji do słownika\n" );
     
            goto GARBAGE;
       }

      intRet = bmdconf_add_section2dict( bmdconfHandle, "DATA_BASE", database_dic, BMDCONF_SECTION_REQ );

      if ( intRet < 0 )
       {
    	    BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
            BMD_SETIERR( err_tRet, AD, "Błąd przy dodawaniu sekcji do słownika\n" );
     
            goto GARBAGE;
       }

      intRet = bmdconf_load_file( stringFileName, bmdconfHandle, &stringKomunikat );
      
      if ( intRet < 0 )
       {
            BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
            BMD_SETIERR( err_tRet, AD, stringKomunikat );
     
            goto GARBAGE;
       }

      intRet = bmdconf_get_value( bmdconfHandle, "SERVER", "IP", &(bmdlogServParam_tParams->ip) );
   
      if ( intRet < 0 )
       {
            BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
            BMD_SETIERR( err_tRet, AD, "Błąd odczytu numeru IP serwera z pliku konfiguracyjnego" );
            cLevel_tWHERE = clevel1;
     
            goto GARBAGE;
       }

      intRet = bmdconf_get_value_int( bmdconfHandle, "SERVER", "PORT", &(bmdlogServParam_tParams->port) );
   
      if ( intRet < 0 )
       {
            BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
            BMD_SETIERR( err_tRet, AD, "Błąd odczytu numeru PORTU serwera z pliku konfiguracyjnego" );
            cLevel_tWHERE = clevel1;
     
            goto GARBAGE;
       }

      intRet = bmdconf_get_value_int( bmdconfHandle, "SERVER", "ATTEMPT", &(bmdlogServParam_tParams->attempt) );
   
      if ( intRet < 0 )
       {
            BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
            BMD_SETIERR( err_tRet, AD, "Błąd odczytu ilości prób połączenia" );
            cLevel_tWHERE = clevel1;
     
            goto GARBAGE;
       }

      intRet = bmdconf_get_value_int( bmdconfHandle, "SERVER", "DELAY", &(bmdlogServParam_tParams->delay) );
   
      if ( intRet < 0 )
       {
            BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
            BMD_SETIERR( err_tRet, AD, "Błąd odczytu opóźnienia między połączeniami" );
            cLevel_tWHERE = clevel1;
     
            goto GARBAGE;
       }

      /*
      intRet = bmdconf_get_value_int( bmdconfHandle, "SERVER", "MAX_CLIENTS", &(bmdlogServParam_tParams->max_clients) );
   
      if ( intRet < 0 )
       {
            BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
            BMD_SETIERR( err_tRet, AD, "Błąd odczytu maksymalnej liczby klientów " );
            cLevel_tWHERE = clevel1;
     
            goto GARBAGE;
       }
       
      intRet = bmdconf_get_value( bmdconfHandle, "SERVER", "TEMP_DIRECTORY", &(bmdlogServParam_tParams->temp_directory) );
   
      if ( intRet < 0 )
       {
            BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
            BMD_SETIERR( err_tRet, AD, "Błąd odczytu nazwy katalogu tymczasowego dla logów" );
            cLevel_tWHERE = clevel1;
     
            goto GARBAGE;
       } 
      */

      intRet = bmdconf_get_value_int( bmdconfHandle, "SERVER", "LOG_AMOUNT", &(bmdlogServParam_tParams->log_amount) );
   
      if ( intRet < 0 )
       {
            BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
            BMD_SETIERR( err_tRet, AD, "Błąd odczytu wielkości paczki do podpisania" );
            cLevel_tWHERE = clevel1;
     
            goto GARBAGE;
       }

      intRet = bmdconf_get_value_int( bmdconfHandle, "SERVER", "SIGN_TIME", &(bmdlogServParam_tParams->sign_time) );
   
      if ( intRet < 0 )
       {
            BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
            BMD_SETIERR( err_tRet, AD, "Błąd odczytu opóźnienia czasowego pomiędzy generowanymi podpisami" );
            cLevel_tWHERE = clevel1;
     
            goto GARBAGE;
       }

      intRet = bmdconf_get_value( bmdconfHandle, "SERVER", "CERT_FILE", &cert_file );
   
      if ( intRet < 0 )
       {
            BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
            BMD_SETIERR( err_tRet, AD, "Błąd odczytu nazwy pliku z certyfikatem podpisującym" );
            cLevel_tWHERE = clevel1;
     
            goto GARBAGE;
       }

      intRet = bmdconf_get_value( bmdconfHandle, "SERVER", "CERT_PASSWD", &cert_passwd );
   
      if ( intRet < 0 )
       {
            BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
            BMD_SETIERR( err_tRet, AD, "Błąd odczytu hasła dla certyfikatu" );
            cLevel_tWHERE = clevel1;
     
            goto GARBAGE;
       }

      intRet = bmdconf_get_value( bmdconfHandle, "DATA_BASE", "IP", &((bmdlogServParam_tParams->baseParam).ip) );
   
      if ( intRet < 0 )
       {
            BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
            BMD_SETIERR( err_tRet, AD, "Błąd odczytu numeru IP bazy danych" );
            cLevel_tWHERE = clevel1;
     
            goto GARBAGE;
       }

      intRet = bmdconf_get_value( bmdconfHandle, "DATA_BASE", "PORT", &((bmdlogServParam_tParams->baseParam).port) );
   
      if ( intRet < 0 )
       {
            BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
            BMD_SETIERR( err_tRet, AD, "Błąd odczytu numeru portu serwera bazy danych" );
            cLevel_tWHERE = clevel1;
     
            goto GARBAGE;
       }

      intRet = bmdconf_get_value( bmdconfHandle, "DATA_BASE", "BASE_NAME", \
                         &((bmdlogServParam_tParams->baseParam).base_name) );
   
      if ( intRet < 0 )
       {
            BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
            BMD_SETIERR( err_tRet, AD, "Błąd odczytu nazwy bazy danych" );
            cLevel_tWHERE = clevel1;
     
            goto GARBAGE;
       }

      intRet = bmdconf_get_value( bmdconfHandle, "DATA_BASE", "USER", &((bmdlogServParam_tParams->baseParam).user) );
   
      if ( intRet < 0 )
       {
            BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
            BMD_SETIERR( err_tRet, AD, "Błąd odczytu nazwy użytkownika bazy danych" );
            cLevel_tWHERE = clevel1;
     
            goto GARBAGE;
       }

      intRet = bmdconf_get_value( bmdconfHandle, "DATA_BASE", "PASSWORD", &((bmdlogServParam_tParams->baseParam).password) );
   
      if ( intRet < 0 )
       {
            BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
            BMD_SETIERR( err_tRet, AD, "Błąd odczytu nazwy użytkownika bazy danych" );
            cLevel_tWHERE = clevel1;
     
            goto GARBAGE;
       }
       
      intRet = bmdconf_get_value( bmdconfHandle, "DATA_BASE", "DB_LIBRARY", &dbLIBRARY );
   
      intRet = bmd_db_init(dbLIBRARY);
      free0( dbLIBRARY );

      if ( intRet != BMD_OK )
       {
           BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
           BMD_SETIERR( err_tRet, AD, "Błąd inicjowania biblioteki obsługi bazy danych" );
	   cLevel_tWHERE = clevel1;
       
	   goto GARBAGE;                                                                                                                                        
       }
   
      intRet = bmd_set_ctx_file( cert_file, cert_passwd, strlen( cert_passwd ), &(bmdlogServParam_tParams->signContext) );

      if ( intRet < 0 )
       {
            BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
            BMD_SETIERR( err_tRet, AD, "Błąd przy ustawianiu kontekstu" );
            cLevel_tWHERE = clevel1;
       }


 GARBAGE :
  
    switch( cLevel_tWHERE )
     {
  
       case clevel1 : { 
                        free0( bmdlogServParam_tParams->ip );
                        free0( bmdlogServParam_tParams->temp_directory );
                        free0( (bmdlogServParam_tParams->baseParam).ip );
                        free0( (bmdlogServParam_tParams->baseParam).port );
                        free0( (bmdlogServParam_tParams->baseParam).base_name );
                        free0( (bmdlogServParam_tParams->baseParam).user );
                        free0( (bmdlogServParam_tParams->baseParam).password );
		      }
  
       case nowhere : {
                        free0( stringKomunikat );
                        free0( cert_file );
                        free0( cert_passwd );
		      }
  
       default      : { bmdconf_destroy( &bmdconfHandle ); }
  
     }

   return err_tRet;

}

err_t getEventsPolicy( void* dbHANDLER, char**** const logTABPOLICY )
{

     /* ------------------------------------- */

        err_t              err_tRet   =    0;
        char*              stringSQL  = NULL;
        long               rows       =    0;
        long               cols       =    0;
        long               intRet     =    0;
        void*              result     = NULL;
        db_row_strings_t*  rowstruct  = NULL;
        long		   fetched    =    0;
        long               i          =    0;
        long               j          =    0;

     /* ------------------------------------- */


	if ( dbHANDLER == NULL )
         {
           BMD_SETERR( err_tRet, BMD_ERR_PARAM1 );
           BMD_SETIERR( err_tRet, AD, "Niepoprawny parametr nr. 1" );

           return err_tRet;
         }

        if ( logTABPOLICY == NULL || *logTABPOLICY != NULL )
         {
           BMD_SETERR( err_tRet, BMD_ERR_PARAM2 );
           BMD_SETIERR( err_tRet, AD, "Niepoprawny parametr nr. 2" );

           return err_tRet;
         }


        if ( asprintf( &stringSQL, "select * from events_table_behavior order by id;" ) < 0)
         {
           BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
           BMD_SETIERR( err_tRet, AD, "Błąd alokacji pamięci" );

           return err_tRet;
         }

        if ( bmd_db_execute_sql( dbHANDLER, stringSQL, &rows, &cols, &result) < 0 )
         {
           BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
           BMD_SETIERR( err_tRet, AD, "Błąd przetwarzania zapytania SQLowego" );

           return err_tRet;
         }
        rows=23;
        (*logTABPOLICY) = (char***)calloc(rows+1, sizeof(char**));

        if ( (*logTABPOLICY) == NULL )
         {
           BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
           BMD_SETIERR( err_tRet, AD, "Błąd alokacji pamięci" );

           return err_tRet;
         }

        for( i=0; i < rows; i++ )
         {
           (*logTABPOLICY)[i] = (char**)calloc(cols-1, sizeof(char*));

           if ( (*logTABPOLICY)[i] == NULL )
            {
              BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
              BMD_SETIERR( err_tRet, AD, "Błąd alokacji pamięci" );

              return err_tRet;
            }
         }

        for( i=0 ;; i++ )
         {
           intRet = bmd_db_result_get_row( dbHANDLER, result, 0, 0, &rowstruct, FETCH_NEXT,&fetched );

           if ( intRet < 0 ) break;

           for( j=0; j < rowstruct->size-2; j++ )
            {
               (*logTABPOLICY)[i][j] = (char*)calloc(1, sizeof(char));

               if ( (*logTABPOLICY)[i][j] == NULL )
                {
                  BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
                  BMD_SETIERR( err_tRet, AD, "Błąd alokacji pamięci" );

                  return err_tRet;
                }

              *((*logTABPOLICY)[i][j]) =  (char)atoi(rowstruct->colvals[j+2]);
            }

           bmd_db_row_struct_free( &rowstruct );
         }

	bmd_db_result_free(&result);

     return err_tRet;

}


long getTime( char** const stringTime )
{

  /* ----------------------- */

     struct timeval      tv;
     struct tm*      tm_ptr;

  /* ----------------------- */

     if ( stringTime == NULL || *stringTime != NULL )
     {
        PRINT_ERROR("Niepoprawny pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
        return BMD_ERR_PARAM1;
     }

     gettimeofday(&tv, 0);

     tm_ptr = (struct tm*)localtime(&(tv.tv_sec));

     asprintf( stringTime, "%04d-%02d-%02d %02d:%02d:%02d.%06li", 1900+tm_ptr->tm_year, \
                            tm_ptr->tm_mon+1, tm_ptr->tm_mday, tm_ptr->tm_hour, tm_ptr->tm_min, \
                            tm_ptr->tm_sec,(long)tv.tv_usec );

     return BMD_OK;

}

