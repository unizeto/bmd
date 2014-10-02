#include <bmd/libbmddb/postgres/pq_libbmddb.h>
#include <bmd/libbmdlog/libbmdlog.h>
#include <bmd/libbmdutils/libbmdutils.h>
#include <bmd/libbmdlog/log_server_utils/log_server_utils.h>
#include <bmd/libbmdlog/balance/atd.h>
#include <time.h>
#include <string.h>

#define PACKET_SIZE 2


extern long _GLOBAL_debug_level;

void delString( void* );

int main()
{

  /* ----------------------------------------- */

     int                intRet         =    0;
     int                intDbRet       =    0;
     void*              dbhandle       = NULL;
     void*              logResult      = NULL;
     void*              hashResult     = NULL;
     void*              res            = NULL;
     char*              stringSQL      = NULL;
     char*              stringHash     = NULL;
     char*              stringHashTmp  = NULL;
     int                            rows,cols;
     int                i              =    0;
     char*              seq_next_val   = NULL;
     char*              last_id        = NULL;
     char*              log_id         = NULL;
     unsigned long int  logFetched     =    0;
     unsigned long int  hashFetched    =    0;
     db_row_strings_t*  rowstruct      = NULL;
     GenBuf_t*          GenBuf_tBuffer = NULL;
     err_t              err_tRet       =    0;
     ATD                                lista;
     ATD_PTR                  Iterator = NULL;

  /* ----------------------------------------- */


  _GLOBAL_debug_level = 2;


  intRet = INIT( &lista );

  if ( intRet < 0 ) exit(1);

  intRet = bmd_db_connect2("127.0.0.1", "5432", "logi", "tklimek", "", &dbhandle);

  if ( intRet < 0) exit(1);

  intRet = asprintf( &stringSQL, "%s", "SELECT * FROM log WHERE NOT EXISTS" \
                      "(SELECT * FROM assoc WHERE log.id = assoc.log_src) order by log.id;" );

  if ( intRet < 0 ) exit(1);

  intRet = bmd_db_execute_sql( dbhandle, stringSQL, WITHOUT_CURSOR, WITHOUT_ROWS, \
                            WITHOUT_COLS, &rows, &cols, &logResult);

  if ( intRet < 0 ) exit(1);


 do{
     intRet = asprintf( &stringHash, "%s", "" );
     if ( intRet < 0 ) exit(1);

     for( i=0; (i<PACKET_SIZE)&&(intDbRet == 0); )
      {
        intDbRet = bmd_db_result_get_row( dbhandle, logResult, 0, 0, &rowstruct, FETCH_NEXT, \
                                              WITHOUT_COLNAMES, &logFetched );

        if ( intDbRet == 0)
         {
           stringHashTmp = stringHash;
           if ( asprintf( &stringHash, "%s%s", stringHash, rowstruct->colvals[19] ) < 0) exit(1);
           if ( asprintf( &log_id, "%s", rowstruct->colvals[0] ) < 0 ) exit(1);
           if ( INSERT( &lista, log_id ) < 0 ) exit(1);

           free0( stringHashTmp );
           free0( rowstruct );
	   i++;
         }
      }

     if ( i == PACKET_SIZE )
      {
        seq_next_val = bmd_db_get_sequence_nextval( dbhandle, "hash_id_seq");
        if ( seq_next_val == NULL )
	 {
           fprintf( stderr, "Blad sekwencji\n");
	   exit(1);
	 }

        intRet = set_gen_buf2( stringHash, strlen( stringHash ), &GenBuf_tBuffer );
        if ( intRet < 0 )
	 {
           fprintf(stderr, "Blad GenBufa\n");
           exit(1);
	 }

        free0( stringHash );
        err_tRet = CreateHash( GenBuf_tBuffer, &stringHash );

	if ( BMD_ERR( err_tRet ) )
	 {
           BMD_BTERR( err_tRet );
           BMD_FREEERR( err_tRet );
	   exit(1);
	 }

        free( stringSQL );
        intRet = asprintf( &stringSQL, "INSERT INTO hash(hash_value,id) VALUES('%s',%s);", \
                                        stringHash, seq_next_val );

        free0( stringHash );
        intRet = bmd_db_execute_sql( dbhandle, stringSQL, WITHOUT_CURSOR, WITHOUT_ROWS, \
	                          WITHOUT_COLS, &rows, &cols, &hashResult );

        free0( hashResult );
        free0( stringSQL );

        intRet = asprintf( &stringSQL, "%s", "SELECT last_value FROM hash_id_seq;" );
        intRet = bmd_db_execute_sql( dbhandle, stringSQL, WITHOUT_CURSOR, WITHOUT_ROWS, \
	                          WITHOUT_COLS, &rows, &cols, &hashResult );

        free0( stringSQL );

        intRet = bmd_db_result_get_value( dbhandle, hashResult, 0, 0, &last_id, FETCH_NEXT, &hashFetched );
        free0( hashResult );
        hashFetched = 0;

        for( ;lista.head != NULL ; )
         {
           intRet = asprintf( &stringSQL, "INSERT INTO assoc(id, log_src) VALUES('%s', '%s');", \
	                      last_id, (char*)((lista.head)->node));

	   intRet = bmd_db_execute_sql( dbhandle, stringSQL, WITHOUT_CURSOR, WITHOUT_ROWS, \
	                             WITHOUT_COLS, &rows, &cols, &res );

           DELETE( &lista, lista.head, delString );

           free0( stringSQL );
           free0( res );
	 }
        free0( last_id );
      }

   }while( intDbRet == 0);

 intRet = bmd_db_disconnect(&dbhandle);

 return 0;
}


void delString(void* string)
{
 free( string );
}

