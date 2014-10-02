#include <bmd/libbmdsql/common.h>
#include <bmd/libbmdpki/libbmdpki.h>
#include <bmd/libbmdlog/libbmdlog.h>
#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdlog/balance/atd.h>
#include <bmd/libbmdlog/log_server_utils/log_server_utils.h>
#include <bmd/libbmddb/libbmddb.h>

/**
 * Funkcja usuwa element przechowywany na stosie
 *
 * @param[in]  elem   - element przechowywany
 *                      na stosie
 *
 */

void destroyStackElem( void* elem )
{
   free(elem);
}

/**
 * Funkcja opróżnia stos
 *
 * @param[in,out]  stack  - uchwyt stosu
 *
 */

void clearStack( ATD* stack )
{
   for( ; stack->tail;  )
   {
     DELETE( stack, (void**)(&(stack->tail)), destroyStackElem );
   }
}


/**
 * Funkcja ustawia status weryfikacji dla węzła skrótu
 *
 * @param[in] connectionHandler - uchwyt do bazy danych
 * @param[in] status            - status do zapisania w bazie
 * @param[in] id                - identyfikator węzła skrótu
 *
 */

void setHashStatus( void* const connectionHandler, long status, const long cascadeErr,\
                    const long id, char const* const timeVerify )
{
    /* ---------------------- */

       char* SQLquery = NULL;
       char* value    = NULL;

    /* ---------------------- */

       if ((cascadeErr == BMD_LOG_HASH_EQUAL) && (status != BMD_LOG_ERR_ID_ABSENT))
       {
           status = BMD_LOG_HASH_EQUAL;
       }

       asprintf( &SQLquery, "UPDATE hash SET verification_status = %li, verification_date_time='%s' where id = %li", \
                 status, timeVerify, id);

       ExecuteSQLQueryWithAnswerKnownDBConnection( connectionHandler, SQLquery, &value);

       free0(value);
       free0(SQLquery);
}

/**
 * Funkcja ustawia status weryfikacji dla węzła skrótu
 *
 * @param[in] connectionHandler - uchwyt do bazy danych
 * @param[in] status            - status do zapisania w bazie
 * @param[in] id                - identyfikator węzła logu
 *
 */

void setLogStatus( void* const connectionHandler, long status, const long cascadeErr,\
                   const long id, char const* const timeVerify )
{
   /* ----------------------- */

      char*  SQLquery = NULL;
      char*     value = NULL;

   /* ----------------------- */

      asprintf( &SQLquery, "UPDATE log SET verification_status=%li, verification_date_time='%s' where id = %li", \
		status, timeVerify, id );

      ExecuteSQLQueryWithAnswerKnownDBConnection( connectionHandler, SQLquery, &value);

      free0(value);
      free0(SQLquery);
}


/**
 * Funkcja ustawia status weryfikacji dla danego węzła
 *
 * @param[in,out]  stack  - uchwyt stosu
 *
 */
long setVerificationStatus( void* const connectionHandler, \
                           void (*setStatus)(void* const, long, const long, const long, char const* const ), \
                           const long status, const long cascadeErr, const long id, char const* const timeVerify )
{

    if( connectionHandler == NULL )
    {
       PRINT_ERROR("Niepoprawny pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
       return BMD_ERR_PARAM1;
    }

    if( setStatus == NULL )
    {
       PRINT_ERROR("Niepoprawny drugi parametr. Error = %d\n", BMD_ERR_PARAM2);
       return BMD_ERR_PARAM2;
    }

    if( id <= 0 )
    {
       PRINT_ERROR("Niepoprawny czwarty parametr. Error = %d\n", BMD_ERR_PARAM4);
       return BMD_ERR_PARAM4;
    }

    if( timeVerify == NULL )
    {
       PRINT_ERROR("Niepoprawny piąty parametr. Error = %d\n", BMD_ERR_PARAM5);
       return BMD_ERR_PARAM5;
    }

    setStatus( connectionHandler, status, cascadeErr, id, timeVerify );

    return BMD_OK;
}


/**
 * Funkcja oblicza skrót z napisu źródłowego
 *
 * @param[in]  src        - napis źródłowy
 * @param[out] stringhash - skrót obliczany z napisu
 *                          źródłowego
 *
 * @return status zakończenia
 * @retval BMD_ERR_PARAM1 - niepoprawny pierwszy parametr
 * @retval BMD_ERR_PARAM2 - niepoprawny drugi parametr
 * @retval BMD_ERR_OP_FAILED - błędy przy operacjach na Genbufie
 * @retval BMD_OK - zakończenie pomyślne
 *
 */


long makeHash( const char* const src, char** const stringHash )
{

    /* ------------------------------------- */

       GenBuf_t*          tmpConcat  = NULL;
       bmd_crypt_ctx_t*   hashCTX    = NULL;
       long               retVal     =    0;
       GenBuf_t*          binaryHash = NULL;

    /* ------------------------------------- */


    if( src == NULL )
    {
       PRINT_ERROR("Niepoprawny pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
       return BMD_ERR_PARAM1;
    }

    if( stringHash == NULL || *stringHash != NULL )
    {
       PRINT_ERROR("Niepoprawny drugi parametr. Error = %d\n", BMD_ERR_PARAM2);
       return BMD_ERR_PARAM2;
    }

    set_gen_buf2( (char*)src, strlen(src), &tmpConcat );

    retVal = bmd_set_ctx_hash( &hashCTX, BMD_HASH_ALGO_SHA1 );

    if( retVal != BMD_OK )
    {
       PRINT_ERROR("Błąd przy ustawianiu kontekstu na funkcję skrótu SHA1. Error = %d\n", BMD_ERR_OP_FAILED);
       free_gen_buf(&tmpConcat);

       return BMD_ERR_OP_FAILED;
    }

    retVal = bmd_hash_data( tmpConcat, &hashCTX, &binaryHash, NULL );
    bmd_ctx_destroy( &hashCTX );
    free_gen_buf( &tmpConcat );

    if( retVal != BMD_OK )
    {
       PRINT_ERROR("Błąd w trakcie obliczania skrótu. Error = %d\n", BMD_ERR_OP_FAILED);
       return BMD_ERR_OP_FAILED;
    }

    retVal = bmd_hash_as_string( binaryHash, stringHash, 0/*separator*/, 1);
    free_gen_buf( &binaryHash );

    if( retVal != BMD_OK )
    {
       PRINT_ERROR("Błąd w trakcie konwersji wartości skrótu do stringa. Error = %d\n", BMD_ERR_OP_FAILED);
       return BMD_ERR_OP_FAILED;
    }

    return BMD_OK;

}


/**
 * Funkcja pobiera identyfikatory poszczególnych poddrzew
 * drzewa o zadanym id
 *
 * @param[in]  connectionHandler - uchwyt do bazy danych
 * @param[in]  depth    - sprawdzamy tylko bieżące drzewo lub
                          pozostałe poddrzewa
 * @param[in] rootNode - identyfikator drzewa (korzeń główny)
 * @param[in] offset   - kolejni najbliżsi potomkowie korzenia
 *                       (tylko węzły z podpisem )
 * @param[out] childNode - identyfikator poddrzewa
 *
 * @return status zakończenia
 * @retval BMD_ERR_PARAM1 - niepoprawny pierwszy parametr
 * @retval BMD_ERR_PARAM2 - niepoprawny drugi parametr
 * @retval BMD_ERR_PARAM3 - niepoprawny trzeci parametr
 * @retval BMD_ERR_PARAM4 - niepoprawny czwarty parametr
 * @retval BMD_ERR_PARAM5 - niepoprawny piąty parametr
 * @retval BMD_LOG_ERR_DB_EXECUTE - błąd w dostępie do bazy danych
 * @retval BMD_LOG_ERR_ID_ABSENT - brak logu o danym identyfikatorze
 * @retval BMD_OK - zakończenie pomyślne
 */

long getSignNodeId( void* const connectionHandler, depth_t depth, const long long rootNode, const long long offset, long* const childNode )
{


    /* ----------------------- */

       char*  SQLquery = NULL;
       char*  value    = NULL;
       long   retVal   =    0;

    /* ----------------------- */



    if( connectionHandler == NULL )
    {
       PRINT_ERROR("Niepoprawny pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
       return BMD_ERR_PARAM1;
    }

    if( depth != branch && depth != subtree )
    {
       PRINT_ERROR("Niepoprawny drugi parametr. Error = %d\n", BMD_ERR_PARAM2);
       return BMD_ERR_PARAM2;
    }

    if( offset < 0 )
    {
       PRINT_ERROR("Niepoprawny trzeci parametr. Error = %d\n", BMD_ERR_PARAM3);
       return BMD_ERR_PARAM3;
    }

    if( rootNode < 1 )
    {
       PRINT_ERROR("Niepoprawny czwarty parametr. Error = %d\n", BMD_ERR_PARAM4);
       return BMD_ERR_PARAM4;
    }

    if( childNode == NULL )
    {
        PRINT_ERROR("Niepoprawny piąty parametr. Error = %d\n", BMD_ERR_PARAM5);
        return BMD_ERR_PARAM5;
    }

    switch( depth )
    {
       case branch : {
                       asprintf( &SQLquery, "SELECT id FROM hash WHERE sig_value IS NOT NULL AND id=%lld OFFSET %lld LIMIT 1;", rootNode, offset );
                       break;
                     }
       case subtree : {
                        asprintf( &SQLquery, "SELECT id FROM hash WHERE sig_value IS NOT NULL AND id<=%lld ORDER BY ID ASC OFFSET %lld LIMIT 1;", rootNode, offset );
                      }
    }

    retVal = ExecuteSQLQueryWithAnswerKnownDBConnection( connectionHandler, SQLquery, &value);
    free0( SQLquery );

    if( retVal == BMD_ERR_NODATA )
    {
       return BMD_LOG_ERR_ID_ABSENT;
    }

    if( retVal != 0 )
    {
       /*PRINT_ERROR("Błąd w trakcie zapytania do bazy danych : %s. Error = %d\n", PQdb((PGconn*)connectionHandler), BMD_LOG_ERR_DB_EXECUTE);*/
       return BMD_LOG_ERR_DB_EXECUTE;
    }

    *childNode = atoll((char*)value);

    return BMD_OK;
}


/**
 * Funkcja dokonuje połączenia skrótów z poszczególnych
 * węzłów logów
 *
 * @param[in]  connectionHandler - uchwyt do bazy danych
 * @param[in]  queryResult       - wynik dotyczący bezpośrednich
 *                                 skrótów z logów w bazie
 *
 * @param[in]  rowsCount - ilość pobranych wierszy wyniku
 * @param[out] stringHash - połączone skróty z poszczególnych logów
 *
 * @return status zakończenia
 * @retval BMD_ERR_PARAM1 - niepoprawny pierwszy parametr
 * @retval BMD_ERR_PARAM2 - niepoprawny drugi parametr
 * @retval BMD_ERR_PARAM3 - niepoprawny trzeci parametr
 * @retval BMD_ERR_PARAM4 - niepoprawny czwarty parametr
 * @retval BMD_LOG_ERR_DB_EXECUTE - błąd w dostępie do bazy danych
 * @retval BMD_LOG_ERR_OP_FAILED - błąd w trakcie łączenia skrótów
 * @retval BMD_OK - zakończenie pomyślne
 */


long concatLogNodes( void* const connectionHandler, void* const queryResult, long const rowsCount, char** stringHash )
{


    /* ----------------------------------------- */

       long          i                   =    0;
       long          retVal              =    0;
       char*         value               = NULL;
       char*         lastHashValue       = NULL;
       successfullyFetched =    0;

    /* ----------------------------------------- */




    if( connectionHandler == NULL )
    {
      PRINT_ERROR("Niepoprawny pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
      return BMD_ERR_PARAM1;
    }

    if( queryResult == NULL )
    {
      PRINT_ERROR("Niepoprawny drugi parametr. Error = %d\n", BMD_ERR_PARAM2);
      return BMD_ERR_PARAM2;
    }

    if( rowsCount < 1 )
    {
      PRINT_ERROR("Niepoprawny trzeci parametr. Error = %d\n", BMD_ERR_PARAM3);
      return BMD_ERR_PARAM3;
    }

    if( stringHash == NULL || *stringHash != NULL )
    {
        PRINT_ERROR("Niepoprawny czwarty parametr. Error = %d\n", BMD_ERR_PARAM4);
        return BMD_ERR_PARAM4;
    }

	PRINT_INFO(": bmd_db_result_get_value\n");
    retVal = bmd_db_result_get_value( connectionHandler, queryResult, 0 /*row*/, 1 /*column*/, &value, FETCH_ABSOLUTE, &successfullyFetched );
    successfullyFetched = 0;

    if( retVal != 0 )
    {
      PRINT_ERROR("Błąd odczytu wartości skrótu dla logu. Error = %d\n", BMD_LOG_ERR_DB_EXECUTE );
      return BMD_LOG_ERR_DB_EXECUTE;
    }

    *stringHash = value;
    value = NULL;

    for( i=1; i < rowsCount; i++ )
    {
      retVal = bmd_db_result_get_value( connectionHandler, queryResult, i /*row*/, 1 /*column*/, &value, FETCH_ABSOLUTE, &successfullyFetched);
      successfullyFetched = 0;

      if( retVal != 0 )
      {
         PRINT_ERROR("Błąd odczytu wartości skrótu dla logu. Error = %d\n", BMD_LOG_ERR_DB_EXECUTE );
         free0( (*stringHash) );

         return BMD_LOG_ERR_DB_EXECUTE;
      }

      retVal = bmd_strmerge( (*stringHash), value, &lastHashValue );
      free0( (*stringHash) );
      free0(value);

      if( retVal != 0 )
      {
        PRINT_ERROR("Błąd konkatenacji wartości skrótów węzłów logów. Error = %d\n", BMD_ERR_OP_FAILED);
        return BMD_ERR_OP_FAILED;
      }

      *stringHash = lastHashValue;
      lastHashValue = NULL;
    }

    return BMD_OK;

}



/**
* Funkcja oblicza skrót z konkatenacji pól wchodzących
* w skład logu
*
* @param[in]  connectionHandler - uchwyt do bazy danych
* @param[in]  logId  - identyfikator logu poddawany
*                      weryfikacji
* @param[out] hash - skrót obliczany z konkatenacji
*                    pól wchodzących w skład logu
*
* @return status zakończenia
* @retval BMD_ERR_PARAM1 - niepoprawny pierwszy parametr
* @retval BMD_ERR_PARAM2 - niepoprawny drugi parametr
* @retval BMD_ERR_PARAM3 - niepoprawny trzeci parametr
* @retval BMD_ERR_OP_FAILED - błędy przy operacjach na Genbufie
*                             lub przy obliczaniu skrótu SHA1
* @retval BMD_LOG_ERR_DB_EXECUTE - błąd w dostępie do bazy danych
* @retval BMD_LOG_ERR_ID_ABSENT - brak logu o danym identyfikatorze
* @retval BMD_OK - zakończenie pomyślne
*
*/

long countLogHash( void* const connectionHandler, long const logId, GenBuf_t** const hash )

{

    /*  ---------------------------------------------  */

        char*             SQLquery            = NULL;
        bmd_crypt_ctx_t*  hashCTX             = NULL;
        void*             queryResult         = NULL;
        char*             value               = NULL;
        char*             concatenation       = NULL;
        char*             tmpStr              = NULL;
        GenBuf_t*         tmpConcat           = NULL;
        GenBuf_t*         tmpHash             = NULL;
        long              colsCount           =    0;
        long              iter                =    0;
        long              retVal              =    0;
        successfullyFetched              =    0;
        long                               rowsCount=1;
	char                             dateBuf[27];

    /*  ---------------------------------------------  */



    if( connectionHandler == NULL )
    {
      PRINT_ERROR("Niepoprawny pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);

      return BMD_ERR_PARAM1;
    }

    if( logId < 1 )
    {
      PRINT_ERROR("Niepoprawny drugi parametr. Error = %d\n", BMD_ERR_PARAM2);

      return BMD_ERR_PARAM2;
    }

    if( hash == NULL )
    {
      PRINT_ERROR("Niepoprawny trzeci parametr. Error = %d\n", BMD_ERR_PARAM3);

      return BMD_ERR_PARAM3;
    }

    if( *hash != NULL )
    {
      PRINT_ERROR("Niepoprawny trzeci parametr. Error = %d\n", BMD_ERR_PARAM3);

      return BMD_ERR_PARAM3;
    }

    asprintf( &SQLquery,	"SELECT remote_host,\
					remote_port,\
					src_file,\
					src_line,\
					src_function,\
					date_time_end,\
					remote_pid,\
					log_owner,\
					service,\
					operation_type,\
					log_level,\
					date_time_begin,\
					log_string,\
					log_reason,\
					log_solution,\
					document_size,\
					document_filename,\
					log_referto,\
					event_code,\
					log_owner_event_visible,\
					log_referto_event_visible,\
					user_data1,\
					user_data2,\
					user_data3,\
					user_data4,\
					user_data5 \
				FROM log WHERE id=%li;", logId);

    retVal = bmd_db_execute_sql( connectionHandler, SQLquery,  &rowsCount, &colsCount, &queryResult );

    free(SQLquery); SQLquery=NULL;

    if( retVal != 0 )
    {
      /*PRINT_ERROR("Błąd w trakcie zapytania do bazy danych : %s. Error = %d\n", PQdb((PGconn*)connectionHandler), BMD_LOG_ERR_DB_EXECUTE);*/

      return BMD_LOG_ERR_DB_EXECUTE;
    }

    if( rowsCount < 1 )
    {
      PRINT_ERROR("Brak logu o podanym id = %ld. Error = %d\n", logId, BMD_LOG_ERR_ID_ABSENT);
      bmd_db_result_free(&queryResult);

      return BMD_LOG_ERR_ID_ABSENT;
    }

    retVal = bmd_db_result_get_value( connectionHandler, queryResult, 0 /*row*/, 0 /*column*/, &value, FETCH_ABSOLUTE, &successfullyFetched);
    
	if(retVal != BMD_ERR_NODATA && retVal != BMD_OK)
		PRINT_ERROR("ERROR while getting result from table\n");
	if(retVal == BMD_ERR_NODATA)
		PRINT_INFO("ERROR BMD_ERR_NODATA\n");
	    
	successfullyFetched = 1;
	
    if(retVal != 0)
    {
      PRINT_ERROR("Błąd odczytu jednego z pól logu o id = %ld. Error = %d\n", logId, BMD_LOG_ERR_DB_EXECUTE);
      bmd_db_result_free(&queryResult);
      return BMD_LOG_ERR_DB_EXECUTE;
    }

    concatenation=value;
    dateBuf[26] = '\0';

    value=NULL;

    for( iter = 1; iter < colsCount; iter++ )
    {
      retVal = bmd_db_result_get_value( connectionHandler, queryResult, 0 /*row*/, iter /*column*/, &value, FETCH_ABSOLUTE, &successfullyFetched );
	  if(retVal != BMD_ERR_NODATA && retVal != BMD_OK)
	  {
		PRINT_ERROR("ERROR while getting result from table\n");
		bmd_db_result_free(&queryResult);
	        return BMD_LOG_ERR_DB_EXECUTE;
	  }
	  if(retVal == BMD_ERR_NODATA)
			PRINT_INFO("ERROR BMD_ERR_NODATA\n");
    successfullyFetched = 1;

      /* *************************************************************/
      /* Jeśli odczytujemy kolumne date_time_begin lub date_time_end */
      /* *************************************************************/

    if (iter == 5 || iter == 11){
	
	if (strlen(value) > 0){


		memset(dateBuf, '0', sizeof(dateBuf)-1);
	
		memcpy(dateBuf, value, strlen(value));
	
		if (strlen(value) == 19){
			
			dateBuf[19] = '.';
		}
		if(strlen(value) > 16)
                if (iter == 11){
			
			dateBuf[10] = '-';
			dateBuf[13] = '.';
			dateBuf[16] = '.';
		}


		retVal = bmd_strmerge( concatenation, dateBuf, &tmpStr );
	}
        else{
		retVal = bmd_strmerge( concatenation, value, &tmpStr );
        }
      }
      else {
		retVal = bmd_strmerge( concatenation, value, &tmpStr );
      }
      free(value); value=NULL;
      if( retVal != 0 )
      {
        continue;
      }

      free(concatenation);
      concatenation=tmpStr;
      tmpStr=NULL;
    }

    bmd_db_result_free(&queryResult);


    retVal = set_gen_buf2( (char*)concatenation, strlen(concatenation), &tmpConcat );
    free(concatenation); concatenation=NULL;

    if( retVal != 0 )
    {
      PRINT_ERROR("Błąd przy wypełnianiu genbuffa. Error = %d\n", BMD_ERR_OP_FAILED);

      return BMD_ERR_OP_FAILED;
    }

    retVal = bmd_set_ctx_hash( &hashCTX, BMD_HASH_ALGO_SHA1 );

    if( retVal != BMD_OK )
    {
      PRINT_ERROR("Błąd przy ustawianiu kontekstu na funkcję skrótu SHA1. Error = %d\n", BMD_ERR_OP_FAILED);
      free_gen_buf(&tmpConcat);

      return BMD_ERR_OP_FAILED;
    }

    retVal = bmd_hash_data( tmpConcat, &hashCTX, &tmpHash, NULL );
    free_gen_buf( &tmpConcat );
    bmd_ctx_destroy( &hashCTX );

    if( retVal != BMD_OK )
    {
      PRINT_ERROR("Błąd w trakcie obliczania skrótu. Error = %d\n", BMD_ERR_OP_FAILED);

      return BMD_ERR_OP_FAILED;
    }

	char* stringHash=NULL;

     retVal = bmd_hash_as_string( tmpHash, &stringHash, 0, 1 );
     //PRINT_INFO("HASH VALUE AS STRING:|%s|\n", stringHash);

     if ( retVal  < 0 )
      {
    	    PRINT_ERROR("Błąd w trakcie obliczania skrótu. Error = %d\n", BMD_ERR_OP_FAILED);

	    return BMD_ERR_OP_FAILED;
      }


    *hash = tmpHash;
    tmpHash = NULL;

    return BMD_OK;

}


/**
* Funkcja dokonuje porównania obliczonej wartości skrótu dla
* zadanego logu, z wartością przechowywaną w bazie
*
* @param[in]  connectionHandler - uchwyt do bazy danych
* @param[in]  logId  - identyfikator logu poddawany
*                      weryfikacji
*
* @return status zakończenia
* @retval BMD_ERR_PARAM1 - niepoprawny pierwszy parametr
* @retval BMD_ERR_PARAM2 - niepoprawny drugi parametr
* @retval BMD_ERR_PARAM3 - niepoprawny trzeci parametr
* @retval BMD_ERR_OP_FAILED - błędy przy konwersji skrótu w postaci
*                             binarnej do stringa lub różnica w długości
*                             porównywanych skrótów
* @retval BMD_LOG_ERR_DB_EXECUTE - błąd w dostępie do bazy danych
* @retval BMD_LOG_HASH_EQUAL - porównywane skróty są takie same
* @retval BMD_LOG_HASH_DIFFRENT - porównywane skróty różnią się
*
*/


long checkLogHash( void* const connectionHandler, long const logId, GenBuf_t* const binaryHash )

{

    /* ---------------------- */

       long   retVal   =    0;
       char* SQLquery = NULL;
       char* hashStr  = NULL;
       char* dbHash   = NULL;

    /* ---------------------- */



    if( connectionHandler == NULL )
    {
       PRINT_ERROR("Niepoprawny pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);

       return BMD_ERR_PARAM1;
    }

    if( logId < 1 )
    {
       PRINT_ERROR("Niepoprawny drugi parametr. Error = %d\n", BMD_ERR_PARAM2);

       return BMD_ERR_PARAM2;
    }

    if( binaryHash == NULL )
    {
       PRINT_ERROR("Niepoprawny trzeci parametr. Error = %d\n", BMD_ERR_PARAM3);

       return BMD_ERR_PARAM3;
    }

    if( binaryHash->buf == NULL || binaryHash->size <= 0 )
    {
       PRINT_ERROR("Niepoprawny trzeci parametr. Error = %d\n", BMD_ERR_PARAM3);

       return BMD_ERR_PARAM3;
    }

    asprintf( &SQLquery, "SELECT hash_value FROM log WHERE id=%li;", logId );
    retVal = ExecuteSQLQueryWithAnswerKnownDBConnection( connectionHandler, SQLquery, &dbHash);

    free(SQLquery); SQLquery=NULL;

    if( retVal != BMD_OK )
    {
      //PRINT_ERROR("Błąd w trakcie zapytania do bazy danych : %s. Error = %d\n", PQdb((PGconn*)connectionHandler), BMD_LOG_ERR_DB_EXECUTE );

      return BMD_LOG_ERR_DB_EXECUTE;
    }

    retVal = bmd_hash_as_string( binaryHash, &hashStr, 0 /*separator*/, 1 );
//PRINT_INFO("HASH AS STRING:|%s|\n", hashStr);

    if( retVal != BMD_OK )
    {
      PRINT_ERROR("Błąd w trakcie konwersji wartości skrótu do stringa. Error = %d\n", BMD_ERR_OP_FAILED);
      free(dbHash); dbHash=NULL;

      return BMD_ERR_OP_FAILED;
    }

    if( strlen(dbHash) != strlen(hashStr) )
    {
      PRINT_ERROR("Różnica w długości porównywanych skrótów. Error = %d\n", BMD_ERR_OP_FAILED);
      free(dbHash); dbHash=NULL;
      free(hashStr); hashStr=NULL;

      return BMD_ERR_OP_FAILED;
    }

    retVal = memcmp(dbHash, hashStr, strlen(dbHash));
    free(dbHash); dbHash=NULL;
    free(hashStr); hashStr=NULL;

    return ( retVal == 0 ? BMD_LOG_HASH_EQUAL : BMD_LOG_HASH_DIFFRENT );

}


/**
*
* @param[in]  connectionHandler - uchwyt do bazy danych
* @param[in]  logId  - identyfikator logu poddawany
*                      weryfikacji
*
* @return status zakończenia
* @retval BMD_ERR_PARAM1 - niepoprawny pierwszy parametr
* @retval BMD_ERR_PARAM2 - niepoprawny drugi parametr
* @retval BMD_ERR_OP_FAILED - błąd przy obliczaniu wartości skrótu pól danego logu
* @retval BMD_LOG_HASH_EQUAL - pomyślna weryfikacja logu
* @retval BMD_LOG_HASH_DIFFRENT - niepomyślna weryfikacja logu
*
*/


long getHashFromHashNode( void* const connectionHandler, void* const queryResult, long const row, char** const hashValue )

{

    /* ---------------------------------------------- */

       long                retVal             =    0;
       char*              lobId               = NULL;
       successfullyFetched 	              =    0;
       GenBuf_t*          signature           = NULL;
       GenBuf_t*          cert                = NULL;
       GenBuf_t*          content             = NULL;

    /* ---------------------------------------------- */



    if( connectionHandler == NULL )
    {
       PRINT_ERROR("Niepoprawny pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);

       return BMD_ERR_PARAM1;
    }

    /*mo�e by� NULL, nieuzywany w db2*/
    //if( queryResult == NULL )
    //{
    //   PRINT_ERROR("Niepoprawny drugi parametr. Error = %d\n", BMD_ERR_PARAM2);
    //   return BMD_ERR_PARAM2;
    //}

    if( row < 0 )
    {
       PRINT_ERROR("Niepoprawny trzeci parametr. Error = %d\n", BMD_ERR_PARAM3);

       return BMD_ERR_PARAM3;
    }

    if( hashValue == NULL )
    {
       PRINT_ERROR("Niepoprawny czwarty parametr. Error = %d\n", BMD_ERR_PARAM4);

       return BMD_ERR_PARAM4;
    }

    retVal = bmd_db_result_get_value( connectionHandler, queryResult, row, 2, &lobId, FETCH_ABSOLUTE, &successfullyFetched );
    successfullyFetched = 1;

    PRINT_INFO("lobId: %s", lobId);
    if( retVal != 0)
    {
       PRINT_ERROR( "Błąd odczytu pola z numerem loba podpisu. Error = %d\n", BMD_LOG_ERR_DB_EXECUTE );

       return BMD_LOG_ERR_DB_EXECUTE;
    }

    bmd_db_result_free(connectionHandler);

    if( strcmp( lobId, "" ) != 0 )
    {
	PRINT_INFO("Pobranie loba z podpisem węzła przechowywujacego skrót o id %s\n", lobId);
       /* Pobranie loba z podpisem węzła przechowywujacego skrót */
       retVal = bmd_db_import_blob( connectionHandler, lobId, &signature);
       free( lobId ); lobId = NULL;

       if(signature == NULL){
	  PRINT_INFO("Signature is NULL\n");}
       else
	  PRINT_INFO("Signature size %ld\n", signature->size);
       
       PRINT_INFO("Checkpoint\n");
       if ( retVal != BMD_OK )
       {
         PRINT_ERROR( "Błąd odczytu loba z podpisem. Error = %d\n", BMD_LOG_ERR_LO_GET );

         return BMD_LOG_ERR_LO_GET;
       }

       retVal = bmd_get_cert_and_content_from_signature( signature, &cert, &content );
       free_gen_buf( &signature );
       free_gen_buf( &cert );

       if ( retVal != 0 )
       {
         PRINT_ERROR( "Błąd pobierania podpisanego skrótu. Error = %d\n", BMD_LOG_ERR_LO_GET );

         return BMD_ERR_OP_FAILED;
       }

       *hashValue = (char*)calloc( content->size+1, sizeof(char) );

       if( *hashValue == NULL )
       {
         PRINT_ERROR( "Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY );
         free_gen_buf( &content );

         return BMD_ERR_MEMORY;
       }

       memcpy( *hashValue, content->buf, content->size );
       free_gen_buf( &content );

       return BMD_LOG_STRONG_HASH;

    }

    free( lobId ); lobId = NULL;
    retVal = bmd_db_result_get_value( connectionHandler, queryResult, row, 1, hashValue, FETCH_ABSOLUTE, &successfullyFetched );
    successfullyFetched = 0;

    if( retVal != 0 )
    {
       PRINT_ERROR( "Błąd odczytu pola z wartością skrótu. Error = %d\n", BMD_LOG_ERR_DB_EXECUTE );

       return BMD_LOG_ERR_DB_EXECUTE;
    }


    return BMD_LOG_WEAK_HASH;

}

/**
 * Funkcja dokonuje połączenia skrótów z pośrednich
 * węzłów skrótów
 *
 * @param[in]  connectionHandler - uchwyt do bazy danych
 * @param[in]  queryResult       - wynik dotyczący pośrednich
 *                                 skrótów z logów w bazie
 *
 * @param[in]  rowsCount - ilość pobranych wierszy wyniku
 * @param[in]  parentHashId - identyfikator węzła będącego bezpośrednim
 *                            przodkiem węzłów, których skróty łączymy
 * @param[out] stringHash - połączone skróty z pośrednich węzłów skrótu
 *
 * @return status zakończenia
 * @retval BMD_ERR_PARAM1 - niepoprawny pierwszy parametr
 * @retval BMD_ERR_PARAM2 - niepoprawny drugi parametr
 * @retval BMD_ERR_PARAM3 - niepoprawny trzeci parametr
 * @retval BMD_ERR_PARAM4 - niepoprawny czwarty parametr
 * @retval BMD_ERR_PARAM5 - niepoprawny piąty parametr
 * @retval BMD_ERR_PARAM6 - niepoprawny szósty parametr
 * @retval BMD_LOG_ERR_DB_EXECUTE - błąd w dostępie do bazy danych
 * @retval BMD_LOG_ERR_OP_FAILED - błąd w trakcie łączenia skrótów / umieszczenia
 *                                 elementu na stosie
 * @retval BMD_OK - zakończenie pomyślne
 */


long concatHashNodes( void* const connectionHandler, void* const queryResult, long const rowsCount, const long long parentHashId, char** stringHash, ATD* const stack )
{

    /* ----------------------------------------- */

       long          i                   =    0;
       long          retVal              =    0;
       char*         value               = NULL;
       char*         hashId              = NULL;
       char*         concatenation       = NULL;
       char*         lastHashValue       = NULL;
       char*         tmpHash             = NULL;
       successfullyFetched =    0;

    /* ----------------------------------------- */


    if( connectionHandler == NULL )
    {
       PRINT_ERROR("Niepoprawny pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
       return BMD_ERR_PARAM1;
    }

    if( queryResult == NULL )
    {
       PRINT_ERROR("Niepoprawny drugi parametr. Error = %d\n", BMD_ERR_PARAM2);
       return BMD_ERR_PARAM2;
    }

    if( rowsCount < 1 )
    {
       PRINT_ERROR("Niepoprawny trzeci parametr. Error = %d\n", BMD_ERR_PARAM3);
       return BMD_ERR_PARAM3;
    }

    if( parentHashId < 1 )
    {
       PRINT_ERROR("Niepoprawny czwarty parametr. Error = %d\n", BMD_ERR_PARAM4);
       return BMD_ERR_PARAM4;
    }

    if( stringHash == NULL || *stringHash != NULL )
    {
       PRINT_ERROR("Niepoprawny piąty parametr. Error = %d\n", BMD_ERR_PARAM5);
       return BMD_ERR_PARAM5;
    }

    if( stack == NULL )
    {
       PRINT_ERROR("Niepoprawny szósty parametr. Error = %d\n", BMD_ERR_PARAM6);
       return BMD_ERR_PARAM6;
    }


    for( i=0; i < rowsCount; i++ )
    {
       retVal = getHashFromHashNode( connectionHandler, queryResult, i, &value );   //poprawić odczyty z odpowiednich kolumn

       if( retVal < 0 )
       {
         PRINT_ERROR("Błąd odczytu wartości skrótu potomka węzła o id=%lld. Error = %d\n", parentHashId, BMD_LOG_ERR_DB_EXECUTE);
         free0(concatenation);
         free0(lastHashValue);

         return BMD_LOG_ERR_DB_EXECUTE;
       }

       if( retVal == BMD_LOG_STRONG_HASH )
       {
          lastHashValue = value;
          value = NULL;
       }
       else
       {
          retVal = bmd_db_result_get_value( connectionHandler, queryResult, i, 0, &hashId, FETCH_ABSOLUTE, &successfullyFetched );
          successfullyFetched = 0;

          if( retVal != 0 )
          {
             PRINT_ERROR("Błąd odczytu numeru id węzła skrótu. Error = %d\n", BMD_LOG_ERR_DB_EXECUTE );
             free0(concatenation);
             free0(lastHashValue);

             return BMD_LOG_ERR_DB_EXECUTE;
          }

          if ( INSERT( stack, hashId ) < 0 )
          {
             PRINT_ERROR("Błąd umieszczenia elementu na stosie. Error = %d\n", BMD_ERR_OP_FAILED);
             free0(concatenation);
             free0(lastHashValue);
             free0(hashId);

             return BMD_ERR_OP_FAILED;
          }

          hashId = NULL;

          if( concatenation == NULL )
          {
             concatenation = value;
             value = NULL;
          }
          else
          {
             retVal = bmd_strmerge( concatenation, value, &tmpHash );
             free0( concatenation );
             free0( value );

             if( retVal < 0 )
             {
                PRINT_ERROR("Błąd konkatenacji wartości skrótów. Error = %d\n", BMD_ERR_OP_FAILED);
                free0(lastHashValue);

                return BMD_ERR_OP_FAILED;
             }

             concatenation = tmpHash;
             tmpHash = NULL;
          }
        }
    }

    if ( lastHashValue != NULL )
    {
      retVal = bmd_strmerge( concatenation, lastHashValue, &tmpHash );
      free0( concatenation );
      free0( lastHashValue );

      if( retVal < 0 )
      {
         PRINT_ERROR("Błąd konkatenacji wartości skrótów. Error = %d\n", BMD_ERR_OP_FAILED);
         return BMD_ERR_OP_FAILED;
      }

      concatenation = tmpHash;
    }

    *stringHash = concatenation;

    return BMD_OK;

}


/**
 * Funkcja dokonuje weryfikacji każdego logu będącego bezpośrednim
 * potomkiem danego węzła skrótu
 *
 * @param[in]  connectionHandler - uchwyt do bazy danych
 * @param[in]  queryResult       - wynik dotyczący pośrednich
 *                                 skrótów z logów w bazie
 *
 * @param[in]  rowsCount - ilość pobranych wierszy wyniku
 *
 * @return status zakończenia
 * @retval BMD_ERR_PARAM1 - niepoprawny pierwszy parametr
 * @retval BMD_ERR_PARAM2 - niepoprawny drugi parametr
 * @retval BMD_ERR_PARAM3 - niepoprawny trzeci parametr
 * @retval BMD_LOG_ERR_DB_EXECUTE - błąd w dostępie do bazy danych
 * @retval BMD_OK - zakończenie pomyślne
 */


long verifyLogNodes( void* const connectionHandler, void* const queryResult, long const rowsCount, \
                    const long cascadeErr, char const * const timeVerify )
{

    /* ----------------------------------------- */

       long          i                   =    0;
       long          retVal              =    0;
       long          logId               =    0;
       char*         value               = NULL;
       successfullyFetched =    0;

    /* ----------------------------------------- */



    if( connectionHandler == NULL )
    {
        PRINT_ERROR("Niepoprawny pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
        return BMD_ERR_PARAM1;
    }

    if( queryResult == NULL )
    {
        PRINT_ERROR("Niepoprawny drugi parametr. Error = %d\n", BMD_ERR_PARAM2);
        return BMD_ERR_PARAM2;
    }

    if( rowsCount < 1 )
    {
        PRINT_ERROR("Niepoprawny trzeci parametr. Error = %d\n", BMD_ERR_PARAM3);
        return BMD_ERR_PARAM3;
    }

    if( timeVerify == NULL )
    {
        PRINT_ERROR("Niepoprawny piąty parametr. Error = %d\n", BMD_ERR_PARAM5);
        return BMD_ERR_PARAM5;
    }

    for( i=0; i < rowsCount; i++ )
    {
       retVal = bmd_db_result_get_value( connectionHandler, queryResult, i /*row*/, 0 /*column*/, &value, FETCH_ABSOLUTE, &successfullyFetched);
       successfullyFetched = 0;

       if( retVal != 0 )
       {
          PRINT_ERROR( "Błąd odczytu pola z identyfikatorem loga. Error = %d\n", BMD_LOG_ERR_DB_EXECUTE );
          return BMD_LOG_ERR_DB_EXECUTE;
       }

       logId = atol(value);
       free0(value);

       retVal = ( cascadeErr != BMD_LOG_HASH_EQUAL ) ? cascadeErr : bmdLogVerifyLogNode( connectionHandler, logId );
       setVerificationStatus( connectionHandler, setLogStatus, retVal, cascadeErr, logId, timeVerify );

       if ( retVal == BMD_LOG_HASH_DIFFRENT )
       {
          printf("MODYFIKACJA WĘZŁA LOGU O ID=%ld\n", logId);
       }
       else if ( retVal != BMD_LOG_HASH_EQUAL )
       {
          printf("BŁĄD WERYFIKACJI WĘZŁA LOGU O ID=%ld\n", logId);
       }
    }

    return BMD_OK;

}



/**
 * Funkcja pobiera bezpośrednich potomków danego węzła i
 * dokonuje jego weryfikacji
 *
 * @param[in]  connectionHandler - uchwyt do bazy danych
 * @param[in]  parentHashId - identyfikator weryfikowanego
 *                            węzła
 * @param[out]  stack       - stos do przechowywania
 *                            potomnych węzłów skrótów
 *
 * @return status zakończenia
 * @retval BMD_ERR_PARAM1 - niepoprawny pierwszy parametr
 * @retval BMD_ERR_PARAM2 - niepoprawny drugi parametr
 * @retval BMD_ERR_PARAM3 - niepoprawny trzeci parametr
 * @retval BMD_LOG_ERR_DB_EXECUTE - błąd w dostępie do bazy danych
 * @retval BMD_OK - zakończenie pomyślne
 */



long getChildIdAndVerifyHashNode( void* const connectionHandler, const long long parentHashId, ATD* const stack, const long  cascadeErr, char const * const timeVerify )
{

    /* ----------------------------------------- */

       char*         SQLquery            = NULL;
       char*         value               = NULL;
       char*         stringHash          = NULL;
       char*         concatedHash        = NULL;
       long          retVal              =    0;
       long          errCode             =    0;
       long           rowsCount           =    0;
       long           colsCount           =    0;
       long          hashSource          =    0;
       void*         queryResult         = NULL;

    /* ----------------------------------------- */



    if( connectionHandler == NULL )
    {
       PRINT_ERROR("Niepoprawny pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
       errCode = BMD_ERR_PARAM1;

       goto EXIT;
    }

    if ( parentHashId < 1 )
    {
       PRINT_ERROR("Niepoprawny drugi parametr. Error = %d\n", BMD_ERR_PARAM2);
       errCode = BMD_ERR_PARAM2;

       goto EXIT;
    }

    if ( stack == NULL )
    {
       PRINT_ERROR("Niepoprawny trzeci parametr. Error = %d\n", BMD_ERR_PARAM3);
       errCode = BMD_ERR_PARAM3;

       goto EXIT;
    }

    if ( timeVerify == NULL )
    {
        PRINT_ERROR("Niepoprawny czwarty parametr. Error = %d\n", BMD_ERR_PARAM4);
        errCode = BMD_ERR_PARAM4;

        goto EXIT;
    }

    asprintf( &SQLquery, "SELECT id, hash_value, sig_value FROM hash WHERE id IN (SELECT hash_src FROM assoc WHERE id=%lld) ORDER BY id ASC;", parentHashId );

    retVal = bmd_db_execute_sql(connectionHandler, SQLquery, &rowsCount, &colsCount, &queryResult);
    free0( SQLquery );

    if( retVal != 0 )
    {
/*       PRINT_ERROR("Błąd w trakcie zapytania do bazy danych : %s. Error = %d\n", PQdb((PGconn*)connectionHandler), BMD_LOG_ERR_DB_EXECUTE);*/
       errCode = BMD_LOG_ERR_DB_EXECUTE;

       goto EXIT;
    }

    if( rowsCount < 1 )
    {
      bmd_db_result_free(&queryResult);

      asprintf( &SQLquery, "SELECT id, hash_value FROM log WHERE id IN (SELECT log_src FROM assoc WHERE id=%lld ) ORDER BY id ASC;", parentHashId );

      retVal = bmd_db_execute_sql(connectionHandler, SQLquery, &rowsCount, &colsCount, &queryResult);
      free0( SQLquery );

      if( retVal != 0 )
      {
/*         PRINT_ERROR("Błąd w trakcie zapytania do bazy danych : %s. Error = %d\n", PQdb((PGconn*)connectionHandler), BMD_LOG_ERR_DB_EXECUTE);*/
         errCode = BMD_LOG_ERR_DB_EXECUTE;

         goto EXIT;
      }

      if( rowsCount < 1 )
      {
         printf("MODYFIKACJA W PODDRZEWIE O ID=%lld\n", parentHashId);

         PRINT_ERROR("Brak potomków dla węzła skrótu o id = %lld. Error = %d\n", parentHashId, BMD_LOG_ERR_ID_ABSENT);
         errCode = BMD_LOG_ERR_ID_ABSENT;

         goto EXIT;
      }

      if( cascadeErr != BMD_LOG_HASH_EQUAL )
      {
         printf("MODYFIKACJA W PODDRZEWIE O ID=%lld\n", parentHashId);
         verifyLogNodes( connectionHandler, queryResult, rowsCount, cascadeErr, timeVerify );
         errCode = cascadeErr;

         goto EXIT;
      }

      // wywołanie funkcji weryfikującej węzeł skrótu na poziomie L0 i LOG
      retVal = concatLogNodes( connectionHandler, queryResult, rowsCount, &concatedHash );

      if ( retVal != BMD_OK )
      {
         PRINT_ERROR("Nie mogę wykonać pełnej weryfikacji dla drzewa o id = %lld. Error = %d\n", parentHashId, BMD_LOG_ERR_ID_ABSENT);
         bmd_db_result_free(&queryResult);
         errCode = BMD_ERR_OP_FAILED;

         goto EXIT;
      }

      retVal = makeHash( concatedHash, &stringHash );

      if ( retVal != BMD_OK )
      {
         PRINT_ERROR("Nie mogę wykonać pełnej weryfikacji dla drzewa o id = %lld. Error = %d\n", parentHashId, BMD_LOG_ERR_ID_ABSENT);
         bmd_db_result_free(&queryResult);

         errCode = BMD_ERR_OP_FAILED;

         goto EXIT;
      }

      free0( concatedHash );

      asprintf( &SQLquery, "SELECT hash_value FROM hash WHERE id=%lld;", parentHashId );

      retVal = ExecuteSQLQueryWithAnswerKnownDBConnection(connectionHandler, SQLquery, &value);
      free0( SQLquery );

      if( retVal != 0 )
      {
         /*PRINT_ERROR("Błąd w trakcie zapytania do bazy danych : %s. Error = %d\n", PQdb((PGconn*)connectionHandler), BMD_LOG_ERR_DB_EXECUTE );*/
         bmd_db_result_free(&queryResult);
         free0( stringHash );
         errCode = BMD_LOG_ERR_DB_EXECUTE;

         goto EXIT;
      }

      if( strlen( stringHash ) != strlen( value ) )
      {
         PRINT_ERROR("Różnica w długości porównywanych skrótów. Error = %d\n", BMD_ERR_OP_FAILED);
         bmd_db_result_free(&queryResult);
         free0( stringHash );
         free0( value );
         errCode = BMD_ERR_OP_FAILED;

         goto EXIT;
      }

      retVal = memcmp( stringHash /* obliczony skrót z logów */, value /* skrót pobrany z bazy */ , strlen(value) );
      free0( stringHash );
      free0( value );

      if( retVal != 0 )
      {
         printf("MODYFIKACJA W PODDRZEWIE O ID=%lld\n", parentHashId);
         verifyLogNodes( connectionHandler, queryResult, rowsCount, BMD_LOG_HASH_DIFFRENT, timeVerify );
         errCode = BMD_LOG_HASH_DIFFRENT;

         goto EXIT;
      }

      verifyLogNodes( connectionHandler, queryResult, rowsCount, cascadeErr, timeVerify );

      bmd_db_result_free(&queryResult);

      errCode = BMD_LOG_HASH_EQUAL;
    }
    else
    {
      // wywołanie funkcji weryfikującej węzeł skrótu na poziomie LN
      retVal = concatHashNodes( connectionHandler, queryResult, rowsCount, parentHashId, &concatedHash, stack );
      bmd_db_result_free( &queryResult );

      if ( retVal != BMD_OK )
      {
         PRINT_ERROR("Nie mogę wykonać pełnej weryfikacji dla drzewa o id = %lld. Error = %d\n", parentHashId, BMD_LOG_ERR_ID_ABSENT);
         errCode = BMD_ERR_OP_FAILED;

         goto EXIT;
      }

     if ((cascadeErr != BMD_LOG_HASH_EQUAL) && (cascadeErr != BMD_LOG_SIGN_NODE))
     {
       printf("MODYFIKACJA W PODDRZEWIE O ID=%lld\n", parentHashId);
       free0( concatedHash );
       errCode = cascadeErr;

       goto EXIT;
     }

     // weryfikacja hasha
     retVal = makeHash( concatedHash, &stringHash );
     free0( concatedHash );

     if ( retVal != BMD_OK )
     {
       PRINT_ERROR("Nie mogę wykonać pełnej weryfikacji dla drzewa o id = %lld. Error = %d\n", parentHashId, BMD_LOG_ERR_ID_ABSENT);
       errCode = BMD_ERR_OP_FAILED;

       goto EXIT;
     }

     asprintf( &SQLquery, "SELECT id,hash_value,sig_value FROM hash WHERE id=%lld;", parentHashId );

     retVal = bmd_db_execute_sql( connectionHandler, SQLquery, &rowsCount, &colsCount, &queryResult);
     free0( SQLquery );

     if( retVal != 0 )
     {
       /*PRINT_ERROR("Błąd w trakcie zapytania do bazy danych : %s. Error = %d\n", PQdb((PGconn*)connectionHandler), BMD_LOG_ERR_DB_EXECUTE);*/
       free0( stringHash );
       errCode = BMD_LOG_ERR_DB_EXECUTE;

       goto EXIT;
     }

     if( rowsCount < 1 )
     {
        PRINT_ERROR("Brak węzła z wartością skrótu o id = %lld. Error = %d\n", parentHashId, BMD_LOG_ERR_ID_ABSENT);
        bmd_db_result_free(&queryResult);
        free0( stringHash );
        errCode = BMD_LOG_ERR_ID_ABSENT;

        goto EXIT;
     }

     hashSource = getHashFromHashNode( connectionHandler, queryResult, 0, &value );
     bmd_db_result_free(&queryResult);

     if( hashSource < 0 )
     {
        PRINT_ERROR("Błąd odczytu wartości skrótu węzła o id=%lld. Error = %d\n", parentHashId, BMD_LOG_ERR_DB_EXECUTE);
        free0( stringHash );
        errCode = BMD_LOG_ERR_DB_EXECUTE;

        goto EXIT;
     }

     if( strlen( stringHash ) != strlen( value ) )
     {
        PRINT_ERROR("Różnica w długości porównywanych skrótów. Error = %d\n", BMD_ERR_OP_FAILED);
        free0( stringHash );
        free0( value );
        errCode = BMD_ERR_OP_FAILED;

        goto EXIT;
     }

     retVal = memcmp( stringHash /* obliczony skrót z logów */, value /* skrót pobrany z bazy */ , strlen(value) );

     free0( stringHash );
     free0( value );

     if( retVal != 0 )
     {
        printf("MODYFIKACJA W PODDRZEWIE O ID=%lld\n", parentHashId);
        errCode = BMD_LOG_HASH_DIFFRENT;

        goto EXIT;
     }

     errCode = BMD_LOG_HASH_EQUAL;
    }

EXIT :

   setVerificationStatus( connectionHandler, setHashStatus, errCode, cascadeErr ,parentHashId, timeVerify );

   return errCode;

}

/**
* Funkcja dokonuje weryfikacji zadanego logu w kontekscie
* obliczenia skrótu z jego pól i porównania go z wartością
* skrótu przechowywaną w bazie
*
* @param[in]  connectionHandler - uchwyt do bazy danych
* @param[in]  logId  - identyfikator logu poddawany
*                      weryfikacji
*
* @return status zakończenia
* @retval BMD_ERR_PARAM1 - niepoprawny pierwszy parametr
* @retval BMD_ERR_PARAM2 - niepoprawny drugi parametr
* @retval BMD_ERR_OP_FAILED - błąd przy obliczaniu wartości skrótu pól danego logu
* @retval BMD_LOG_HASH_EQUAL - pomyślna weryfikacja logu
* @retval BMD_LOG_HASH_DIFFRENT - niepomyślna weryfikacja logu
*
*/


long bmdLogVerifyLogNode( void* const connectionHandler, long const logId )
{

    /* ----------------------------- */

       long        retVal     =    0;
       GenBuf_t*  binaryHash = NULL;

    /* ----------------------------- */


    if( connectionHandler == NULL )
    {
       PRINT_ERROR("Niepoprawny pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);

       return BMD_ERR_PARAM1;
    }

    if( logId < 1 )
    {
       PRINT_ERROR("Niepoprawny drugi parametr. Error = %d\n", BMD_ERR_PARAM2);

       return BMD_ERR_PARAM2;
    }

    retVal = countLogHash( connectionHandler, logId, &binaryHash );


    if( retVal != BMD_OK )
    {
       PRINT_ERROR("Błąd przy obliczaniu wartości skrótu pól logu o id = %ld. Error = %d\n", logId, BMD_ERR_OP_FAILED );

       return BMD_ERR_OP_FAILED;
    }

    retVal = checkLogHash( connectionHandler, logId, binaryHash );
    
    free_gen_buf(&binaryHash);


    return retVal;

}



/**
* Funkcja dokonuje weryfikacji węzła skrótu będącego bezpośrednim
* przodkiem rozpatrywanego logu (poziom 0).
*
* @param[in]  connectionHandler - uchwyt do bazy danych
* @param[in]  logId  - identyfikator logu poddawany
*                      weryfikacji
* @param[out] hashNodeId - identyfikator przodka (węzeł skrótu)
*                          loga o identyfikatorze logId
*
* @return status zakończenia
* @retval BMD_ERR_PARAM1 - niepoprawny pierwszy parametr
* @retval BMD_ERR_PARAM2 - niepoprawny drugi parametr
* @retval BMD_ERR_PARAM3 - niepoprawny trzeci parametr
* @retval BMD_LOG_ERR_ID_ABSENT - brak logu/skrótu o danym identyfikatorze
* @retval BMD_LOG_ERR_DB_EXECUTE - błąd w dostępie do bazy danych
* @retval BMD_ERR_OP_FAILED - błąd konkatenacji wartości skrótów węzłów logów /
*                             błąd przy ustawianiu kontekstu na funkcję skrótu SHA1 /
*                             błąd w trakcie obliczania skrótu /
*                             błąd w trakcie konwersji wartości skrótu do stringa /
*                             różnica w długości porównywanych skrótów.
* @retval BMD_LOG_HASH_EQUAL - pomyślna weryfikacja węzła skrótu
* @retval BMD_LOG_HASH_DIFFRENT - niepomyślna weryfikacja węzła skrótu
*
*/


long bmdLogVerifyHashNodeLevel0( void* const connectionHandler, long const logId, long* const hashNodeId )
{

    /* --------------------------------------------- */

       char*             SQLquery            = NULL;
       char*             hashId              = NULL;
       long               retVal              =    0;
       long 		  retVal_1	      =    0;
       long               rowsCount           =    0;
       long               colsCount           =    0;
       void*             queryResult         = NULL;
       long               i                   =    0;
       char*             value               = NULL;
       char*             concatenation       = NULL;
       char*             tmpHashValue        = NULL;
       GenBuf_t*         tmpConcat           = NULL;
       GenBuf_t*         binaryHash          = NULL;
       bmd_crypt_ctx_t*  hashCTX             = NULL;
       successfullyFetched =    0;

    /* --------------------------------------------- */



    if( connectionHandler == NULL )
    {
      PRINT_ERROR("Niepoprawny pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);

      return BMD_ERR_PARAM1;
    }

    if( logId < 1 )
    {
      PRINT_ERROR("Niepoprawny drugi parametr. Error = %d\n", BMD_ERR_PARAM2);

      return BMD_ERR_PARAM2;
    }

    if( hashNodeId == NULL )
    {
      PRINT_ERROR("Niepoprawny trzeci parametr. Error = %d\n", BMD_ERR_PARAM3);

      return BMD_ERR_PARAM3;
    }

    /* pobieram id hasha, dla ktorego musze sprawdzic poprawność logu */
    asprintf( &SQLquery, "SELECT id FROM assoc where log_src=%li;", logId);

    retVal = ExecuteSQLQueryWithAnswerKnownDBConnection( connectionHandler, SQLquery, &hashId);
    free(SQLquery); SQLquery=NULL;

    if( retVal != 0 )
    {
      if( retVal == BMD_ERR_NODATA ) //brak logu o podanym logId
      {
        PRINT_ERROR("Log o id = %ld nie posiada przodka. Error = %d\n", logId, BMD_LOG_ERR_ID_ABSENT);

        return BMD_LOG_ERR_ID_ABSENT;
      }
      else
      {
/*        PRINT_ERROR("Błąd w trakcie zapytania do bazy danych : %s. Error = %d\n", PQdb((PGconn*)connectionHandler), BMD_LOG_ERR_DB_EXECUTE);*/

        return BMD_LOG_ERR_DB_EXECUTE;
      }
    }

    if( hashId == NULL )
    {
      PRINT_ERROR("Brak węzła skrótu dla logu o id = %ld. Error = %d\n", logId, BMD_LOG_ERR_ID_ABSENT);

      return BMD_LOG_ERR_ID_ABSENT;
    }

    asprintf( &SQLquery, "SELECT hash_value FROM log WHERE id IN ( SELECT log_src FROM assoc WHERE id=%s ) ORDER BY id ASC;", hashId );
    *hashNodeId = atol( hashId );
    free(hashId); hashId=NULL;

    retVal = bmd_db_execute_sql(connectionHandler, SQLquery, &rowsCount, &colsCount, &queryResult);
    free(SQLquery); SQLquery=NULL;

    if( retVal != 0 )
    {
      /*PRINT_ERROR("Błąd w trakcie zapytania do bazy danych : %s. Error = %d\n", PQdb((PGconn*)connectionHandler), BMD_LOG_ERR_DB_EXECUTE);*/
	PRINT_ERROR("Mine error \n");
      return BMD_LOG_ERR_DB_EXECUTE;
    }

    

    retVal_1 = bmd_db_result_get_value( connectionHandler, queryResult, 0 /*row*/, 0 /*column*/, &value, FETCH_ABSOLUTE, &successfullyFetched );
    
    if(retVal_1 != 0)
    {
       PRINT_ERROR("Brak potomków dla wezła skrótu o id = %s lub b��d po��czenia z BD. Error = %d\n", hashId, BMD_LOG_ERR_ID_ABSENT);
       free(hashId); hashId=NULL;
       return BMD_LOG_ERR_ID_ABSENT;
    }

    successfullyFetched = 1;

    concatenation = value;
    value = NULL;
    PRINT_INFO("Into loop\n");
    i=0;
    while( 1 )
    {
      PRINT_INFO("Checkpoint %ld\n", i);
      i++;
      retVal_1 = bmd_db_result_get_value( connectionHandler, queryResult, 0 /*row*/, 0 /*column*/, &value, FETCH_ABSOLUTE, &successfullyFetched);

      if( retVal_1 != 0 && retVal_1 != BMD_ERR_NODATA)
      {	
          PRINT_ERROR("Błąd konkatenacji wartości skrótów węzłów logów. Error = %d\n", BMD_ERR_OP_FAILED);
          free(tmpHashValue); tmpHashValue = NULL;
          bmd_db_result_free( &queryResult );

          return BMD_ERR_OP_FAILED;
      }
      else if(retVal_1 == BMD_ERR_NODATA)
	  break;

      retVal = bmd_strmerge( concatenation, value, &tmpHashValue );
      free(concatenation); concatenation=NULL;
      free(value); value=NULL;
      concatenation = tmpHashValue;
      tmpHashValue = NULL;
    }

    set_gen_buf2( (char*)concatenation, strlen(concatenation), &tmpConcat );
    free(concatenation); concatenation=NULL;
    bmd_db_result_free( &queryResult );

    retVal = bmd_set_ctx_hash( &hashCTX, BMD_HASH_ALGO_SHA1 );

    if( retVal != BMD_OK )
    {
      PRINT_ERROR("Błąd przy ustawianiu kontekstu na funkcję skrótu SHA1. Error = %d\n", BMD_ERR_OP_FAILED);
      free_gen_buf(&tmpConcat);

      return BMD_ERR_OP_FAILED;
    }

    retVal = bmd_hash_data( tmpConcat, &hashCTX, &binaryHash, NULL );
    bmd_ctx_destroy( &hashCTX );
    free_gen_buf( &tmpConcat );

    if( retVal != BMD_OK )
    {
      PRINT_ERROR("Błąd w trakcie obliczania skrótu. Error = %d\n", BMD_ERR_OP_FAILED);

      return BMD_ERR_OP_FAILED;
    }

    retVal = bmd_hash_as_string( binaryHash, &tmpHashValue, 0/*separator*/, 1);

    if( retVal != BMD_OK )
    {
      PRINT_ERROR("Błąd w trakcie konwersji wartości skrótu do stringa. Error = %d\n", BMD_ERR_OP_FAILED);
      free_gen_buf(&binaryHash); binaryHash=NULL;

      return BMD_ERR_OP_FAILED;
    }

    free_gen_buf( &binaryHash ); binaryHash=NULL;

    asprintf( &SQLquery, "SELECT hash_value FROM hash WHERE id=%li;", *hashNodeId );
    retVal = ExecuteSQLQueryWithAnswerKnownDBConnection(connectionHandler, SQLquery, &value);
    free( SQLquery ); SQLquery = NULL;

    if( retVal != 0 )
    {
/*  \    PRINT_ERROR("Błąd w trakcie zapytania do bazy danych : %s. Error = %d\n", PQdb((PGconn*)connectionHandler), BMD_LOG_ERR_DB_EXECUTE );*/
      free( tmpHashValue ); tmpHashValue = NULL;

      return BMD_LOG_ERR_DB_EXECUTE;
    }

    if( strlen( tmpHashValue ) != strlen( value ) )
    {
      PRINT_ERROR("Różnica w długości porównywanych skrótów. Error = %d\n", BMD_ERR_OP_FAILED);
      free( tmpHashValue ); tmpHashValue = NULL;
      free( value ); value = NULL;

      return BMD_ERR_OP_FAILED;
    }

    retVal = memcmp( tmpHashValue /* obliczony skrót z logów */, value /* skrót pobrany z bazy */ , strlen(value) );

    free( tmpHashValue ); tmpHashValue = NULL;
    free( value ); value = NULL;

    return ( retVal == 0 ? BMD_LOG_HASH_EQUAL : BMD_LOG_HASH_DIFFRENT );

}

/**
* Funkcja dokonuje weryfikacji węzła skrótu na pozostałych
* poziomach w górę drzewa
*
* @param[in]  connectionHandler - uchwyt do bazy danych
* @param[in]  hashId  - identyfikator węzła skrótu poddawany
*                      weryfikacji
* @param[out] parentHashId - identyfikator przodka (węzeł skrótu)
*                          hasha o identyfikatorze hashId
*
* @return status zakończenia
* @retval BMD_ERR_PARAM1 - niepoprawny pierwszy parametr
* @retval BMD_ERR_PARAM2 - niepoprawny drugi parametr
* @retval BMD_ERR_PARAM3 - niepoprawny trzeci parametr
* @retval BMD_LOG_ERR_ID_ABSENT - brak logu/skrótu o danym identyfikatorze
* @retval BMD_LOG_ERR_DB_EXECUTE - błąd w dostępie do bazy danych
* @retval BMD_ERR_OP_FAILED - błąd konkatenacji wartości skrótów węzłów logów /
*                             błąd przy ustawianiu kontekstu na funkcję skrótu SHA1 /
*                             błąd w trakcie obliczania skrótu /
*                             błąd w trakcie konwersji wartości skrótu do stringa /
*                             różnica w długości porównywanych skrótów.
* @retval BMD_LOG_HASH_EQUAL - pomyślna weryfikacja węzła skrótu
* @retval BMD_LOG_HASH_DIFFRENT - niepomyślna weryfikacja węzła skrótu
*
*/


long bmdLogVerifyHashNodeHigherLevel( void* const connectionHandler, long hashId, long* const parentHashId )
{

    /* --------------------------------------- */

       char*             SQLquery      = NULL;
       long               retVal        =    0;
       long	 	 retVal_1	=    0;
       long               hashSource    =    0;
       char*             tmpHashId     = NULL;
       long               rowsCount     =    0;
       long               colsCount     =    0;
       void*             queryResult   = NULL;
       long               i             =    0;
       char*             concatenation = NULL;
       char*             value         = NULL;
       char*             lastHashValue = NULL;
       char*             stringHash    = NULL;
       GenBuf_t*         tmpConcat     = NULL;
       bmd_crypt_ctx_t*  hashCTX       = NULL;
       GenBuf_t*         binaryHash    = NULL;

    /* --------------------------------------- */





    if( connectionHandler == NULL )
    {
      PRINT_ERROR("Niepoprawny pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);

      return BMD_ERR_PARAM1;
    }

    if( hashId < 1 )
    {
      PRINT_ERROR("Niepoprawny drugi parametr. Error = %d\n", BMD_ERR_PARAM2);

      return BMD_ERR_PARAM2;
    }

    if( parentHashId == NULL )
    {
      PRINT_ERROR("Niepoprawny trzeci parametr. Error = %d\n", BMD_ERR_PARAM3);

      return BMD_ERR_PARAM3;
    }


    while(1)
    {

      /* pobieram hash parenta */
      asprintf( &SQLquery, "SELECT id FROM assoc where hash_src=%li;", hashId );

      retVal = ExecuteSQLQueryWithAnswerKnownDBConnection( connectionHandler, SQLquery, &tmpHashId);
      free( SQLquery ); SQLquery = NULL;

      if( retVal == BMD_ERR_NODATA ) //brak skrótu o podanym logId
      {
        PRINT_ERROR("Węzeł skrótu o id = %ld nie posiada przodka. Error = %d\n", hashId, BMD_LOG_ERR_ID_ABSENT);

        return BMD_LOG_ERR_ID_ABSENT;
      }

      if( retVal != 0 )
      {
        /*PRINT_ERROR("Błąd w trakcie zapytania do bazy danych : %s. Error = %d\n", PQdb((PGconn*)connectionHandler), BMD_LOG_ERR_DB_EXECUTE);*/

        return BMD_LOG_ERR_DB_EXECUTE;
      }

      *parentHashId = atol( tmpHashId );
      free( tmpHashId ); tmpHashId=NULL;

      asprintf( &SQLquery, "SELECT id,hash_value,sig_value FROM hash WHERE id IN ( SELECT hash_src FROM assoc WHERE id=%ld ) ORDER BY id ASC;", *parentHashId );

      retVal_1 = bmd_db_execute_sql( connectionHandler, SQLquery, &rowsCount, &colsCount, &queryResult);

      free( SQLquery ); SQLquery = NULL;

      PRINT_INFO("retVal_1: %ld\n", retVal_1);
      if( retVal_1 != 0 && retVal_1 != BMD_ERR_NODATA)
      {
        return BMD_LOG_ERR_DB_EXECUTE;
      }

      else if( retVal_1 == BMD_ERR_NODATA)
      {
        PRINT_ERROR("Brak potomków dla wezła skrótu o id = %ld. Error = %d\n", *parentHashId, BMD_LOG_ERR_ID_ABSENT);
        return BMD_LOG_ERR_ID_ABSENT;
      }

      for( i=0; ; i++ )
      {
        retVal = getHashFromHashNode( connectionHandler, queryResult, i, &value );
      PRINT_INFO("Checkpoint %ld\n", i);
        if( retVal < 0 )
        {
          PRINT_ERROR("Błąd odczytu wartości skrótu potomka węzła o id=%ld. Error = %d\n", *parentHashId, BMD_LOG_ERR_DB_EXECUTE);
          free(concatenation); concatenation = NULL;
          free(lastHashValue); lastHashValue = NULL;
          bmd_db_result_free(&queryResult);

          return BMD_LOG_ERR_DB_EXECUTE;
        }

        if( retVal == BMD_LOG_STRONG_HASH )
        {
          lastHashValue = value;
          value = NULL;
        }
        else if( concatenation == NULL )
        {
          concatenation = value;
          value = NULL;
        }
        else
        {
          retVal = bmd_strmerge( concatenation, value, &stringHash );
          free( concatenation ); concatenation = NULL;
          free( value ); value = NULL;

          if( retVal < 0 )
          {
            PRINT_ERROR("Błąd konkatenacji wartości skrótów. Error = %d\n", BMD_ERR_OP_FAILED);
            free(lastHashValue); lastHashValue = NULL;
            bmd_db_result_free(&queryResult);

            return BMD_ERR_OP_FAILED;
          }

          concatenation = stringHash;
          stringHash = NULL;
        }

      }

      if ( lastHashValue != NULL )
      {
          retVal = bmd_strmerge( concatenation, lastHashValue, &stringHash );
          free( concatenation ); concatenation = NULL;
          free( lastHashValue ); lastHashValue = NULL;

          if( retVal < 0 )
          {
            PRINT_ERROR("Błąd konkatenacji wartości skrótów. Error = %d\n", BMD_ERR_OP_FAILED);
            bmd_db_result_free(&queryResult);

            return BMD_ERR_OP_FAILED;
          }

          concatenation = stringHash;
          stringHash = NULL;
      }

      bmd_db_result_free(&queryResult);

      set_gen_buf2( (char*)concatenation, strlen(concatenation), &tmpConcat );
      free( concatenation ); concatenation = NULL;

      retVal = bmd_set_ctx_hash( &hashCTX, BMD_HASH_ALGO_SHA1 );

      if( retVal != BMD_OK )
      {
        PRINT_ERROR("Błąd przy ustawianiu kontekstu na funkcję skrótu SHA1. Error = %d\n", BMD_ERR_OP_FAILED);
        free_gen_buf(&tmpConcat);

        return BMD_ERR_OP_FAILED;
      }

      retVal = bmd_hash_data( tmpConcat, &hashCTX, &binaryHash, NULL );
      bmd_ctx_destroy( &hashCTX );
      free_gen_buf( &tmpConcat );

      if( retVal != BMD_OK )
      {
        PRINT_ERROR("Błąd w trakcie obliczania skrótu. Error = %d\n", BMD_ERR_OP_FAILED);

        return BMD_ERR_OP_FAILED;
      }

      retVal = bmd_hash_as_string( binaryHash, &stringHash, 0/*separator*/, 1);
      free_gen_buf( &binaryHash );

      if( retVal != BMD_OK )
      {
        PRINT_ERROR("Błąd w trakcie konwersji wartości skrótu do stringa. Error = %d\n", BMD_ERR_OP_FAILED);

        return BMD_ERR_OP_FAILED;
      }

      asprintf( &SQLquery, "SELECT id,hash_value,sig_value FROM hash WHERE id=%ld;", *parentHashId );

      retVal = bmd_db_execute_sql( connectionHandler, SQLquery, &rowsCount, &colsCount, &queryResult);

      free( SQLquery ); SQLquery = NULL;

      if( retVal != 0 && retVal != BMD_ERR_NODATA)
      {
        /*PRINT_ERROR("Błąd w trakcie zapytania do bazy danych : %s. Error = %d\n", PQdb((PGconn*)connectionHandler), BMD_LOG_ERR_DB_EXECUTE);*/
        free( stringHash ); stringHash = NULL;
        return BMD_LOG_ERR_DB_EXECUTE;
      }

      if( retVal == BMD_ERR_NODATA )
      {
        PRINT_ERROR("Brak węzła z wartością skrótu o id = %ld. Error = %d\n", *parentHashId, BMD_LOG_ERR_ID_ABSENT);
        bmd_db_result_free(&queryResult);
        free( stringHash ); stringHash = NULL;

        return BMD_LOG_ERR_ID_ABSENT;
      }

      hashSource = getHashFromHashNode( connectionHandler, queryResult, 0, &value );
      bmd_db_result_free(&queryResult);

      if( hashSource < 0 )
      {
        PRINT_ERROR("Błąd odczytu wartości skrótu węzła o id=%ld. Error = %d\n", *parentHashId, BMD_LOG_ERR_DB_EXECUTE);
        free( stringHash ); stringHash = NULL;

        return BMD_LOG_ERR_DB_EXECUTE;
      }

      if( strlen( stringHash ) != strlen( value ) )
      {
        PRINT_ERROR("Różnica w długości porównywanych skrótów. Error = %d\n", BMD_ERR_OP_FAILED);
        free( stringHash ); stringHash = NULL;
        free( value ); value = NULL;

        return BMD_ERR_OP_FAILED;
      }

      retVal = memcmp( stringHash /* obliczony skrót z logów */, value /* skrót pobrany z bazy */ , strlen(value) );

      free( stringHash ); stringHash = NULL;
      free( value ); value = NULL;

      if( retVal != 0 )
      {
        return BMD_LOG_HASH_DIFFRENT;
      }

      if( hashSource == BMD_LOG_STRONG_HASH )
      {
        return BMD_LOG_HASH_EQUAL;
      }

      hashId = *parentHashId;

    }

/* END */

}


/**
 * Funkcja dokonuje pełnej lub częściwej weryfikacji
 * struktury drzewa Merkla
 *
 *
 * @param[in]  connectionHandler - uchwyt do bazy danych
 * @param[in]  sigNode  - identyfikator węzła z podpisem
 * @param[in]  depth    - branch --> weryfikacja tylko
 *                                   danej krawędzi
 *                        subtree --> weryfikacja całego
 *                                    poddrzewa
 * @return status zakończenia
 * @retval BMD_ERR_PARAM1 - niepoprawny pierwszy parametr
 * @retval BMD_ERR_PARAM2 - niepoprawny drugi parametr
 * @retval BMD_ERR_PARAM3 - niepoprawny trzeci parametr
 * @retval BMD_LOG_ERR_ID_ABSENT - brak węzła z podpisem o danym identyfikatorze
 * @retval BMD_LOG_ERR_DB_EXECUTE - błąd w dostępie do bazy danych
 *
 */


long bmdLogCheckSubtree( void* const connectionHandler,long long sigNode, depth_t depth )
{

    /* ---------------------------------------------- */

       char*              SQLquery            = NULL;
       char*              tmpHashId           = NULL;
       long          hashNode            =    0;
       long               retVal              =    0;
       ATD                                     stack;
       long i                   =    0;
       long treeDepth           =    0;
       char*              stringTime          = NULL;

    /* ---------------------------------------------- */


    if( connectionHandler == NULL )
    {
      PRINT_ERROR("Niepoprawny pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
      return BMD_ERR_PARAM1;
    }

    if( sigNode < 1 )
    {
      PRINT_ERROR("Niepoprawny drugi parametr. Error = %d\n", BMD_ERR_PARAM2);
      return BMD_ERR_PARAM2;
    }

    if( depth != branch && depth != subtree )
    {
      PRINT_ERROR("Niepoprawny trzeci parametr. Error = %d\n", BMD_ERR_PARAM3);
      return BMD_ERR_PARAM3;
    }

    //odszukanie najbliższego węzła z podpisem

    asprintf( &SQLquery, "SELECT sig_value FROM hash where sig_value IS NOT null AND id=%lld;", sigNode );

    retVal = ExecuteSQLQueryWithAnswerKnownDBConnection( connectionHandler, SQLquery, &tmpHashId);
    free0( SQLquery );

    if( retVal == BMD_ERR_NODATA ) //brak skrótu o podanym sigNode
    {
      PRINT_ERROR("Brak węzła z podpisem o podanym id=%lld. Error = %d\n", sigNode, BMD_LOG_ERR_ID_ABSENT);
      return BMD_LOG_ERR_ID_ABSENT;
    }

    free0(tmpHashId);
    INIT( &stack );

    getTime(&stringTime);

    /*********************************************************************** */

    /* AUDYT BEZPIECZEŃSTWA STRUKTURY DRZEWA MERKLA                          */

    /*********************************************************************** */

    for( i=0;;i++ )
    {
       // Pobieranie węzłów z podpisem
      retVal = getSignNodeId( connectionHandler, depth, sigNode, i, &hashNode );

      if ( retVal != BMD_OK ) break;

      retVal = BMD_LOG_HASH_EQUAL;
      treeDepth = stack.ulCount;

      retVal = getChildIdAndVerifyHashNode( connectionHandler, hashNode, &stack,\
                                            BMD_LOG_SIGN_NODE, stringTime );

      while( stack.tail )
      {
        hashNode = atoll((char*)((stack.tail)->node));
        DELETE( &stack, (void**)(&(stack.tail)), destroyStackElem );


        if ( retVal == BMD_LOG_HASH_EQUAL )
        {
          treeDepth = stack.ulCount;
        }
        else if ( stack.ulCount < treeDepth )
        {
          retVal = BMD_LOG_HASH_EQUAL;
          treeDepth = stack.ulCount;
        }

        retVal = getChildIdAndVerifyHashNode( connectionHandler, hashNode, &stack,\
                                              retVal, stringTime );
      }
    }

    free0(stringTime);

    return ( retVal == BMD_LOG_ERR_ID_ABSENT ) ? BMD_OK : retVal;

}

