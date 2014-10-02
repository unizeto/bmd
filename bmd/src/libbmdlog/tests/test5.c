#include <bmd/libbmddb/postgres/pq_libbmddb.h>
#include <bmd/libbmdlog/libbmdlog.h>
#include <bmd/libbmdutils/libbmdutils.h>
#include <bmd/libbmdlog/log_server_utils/log_server_utils.h>
#include <bmd/libbmdlog/balance/atd.h>
#include <bmd/libbmdlog/balance/tree_node.h>
#include <time.h>
#include <string.h>

#define PACKET_SIZE 6

extern long _GLOBAL_debug_level;


int main()
{

  /* ----------------------------------------- */

     int                intRet         =    0;
     int                N              =    0;
     void*              dbhandle       = NULL;
     void*              result         = NULL;
     char*              stringSQL      = NULL;
     char*              stringHash     = NULL;
     char*              stringHashTmp  = NULL;
     int                            rows,cols;
     int                i              =    0;
     int                j              =    0;
     char*              seq_next_val   = NULL;
     char*              last_id        = NULL;
     unsigned long int  fetched     =    0;
     db_row_strings_t*  rowstruct      = NULL;
     GenBuf_t*          GenBuf_tBuffer = NULL;
     treeNode_t*        treeNode_tNode = NULL;
     err_t              err_tRet       =    0;
     ATD                                lista;
     ATD_PTR            Iterator       = NULL;

  /* ----------------------------------------- */


  _GLOBAL_debug_level = 2;


  intRet = INIT( &lista );

  if ( intRet < 0 ) exit(1);

  intRet = bmd_db_connect2("127.0.0.1", "5432", "logi", "tklimek", "", &dbhandle);

  if ( intRet < 0) exit(2);

  intRet = asprintf( &stringSQL, "%s", "SELECT * FROM log WHERE NOT EXISTS" \
                      "(SELECT * FROM assoc WHERE log.id = assoc.log_src) order by log.id;" );

  if ( intRet < 0 ) exit(3);

  // rozne zapytania

  intRet = bmd_db_execute_sql( dbhandle, stringSQL, WITHOUT_CURSOR, WITHOUT_ROWS, \
                            WITHOUT_COLS, &rows, &cols, &result);

  if ( intRet < 0 ) exit(4);

  do{
      intRet = bmd_db_result_get_row( dbhandle, result, 0, 0, &rowstruct, FETCH_NEXT, \
                                          WITHOUT_COLNAMES, &fetched );
      if ( intRet == 0)
       {
           if ( CreateTreeNode( &treeNode_tNode, rowstruct->colvals[0], rowstruct->colvals[19], logNode ) < 0 ) exit(5);
           if ( INSERT( &lista, treeNode_tNode  ) < 0 ) exit(6);

           free0( rowstruct );
       }

    }while( intRet == 0);




  free0( result );
  fetched = 0;

  /* PIERWSZA CZESC ALGORYTMU - POSZUKIWANIE WIELOKROTNOSCI PACZKI  */

  N = lista.ulCount - (lista.ulCount % PACKET_SIZE);

  for( i=0; i<N; i+= PACKET_SIZE )
  {

     intRet = asprintf( &stringHash, "%s", "" );

     if ( intRet < 0 ) exit(7);

     for( j=0, Iterator=lista.head; j < PACKET_SIZE; j++, Iterator=Iterator->prev )
      {
         stringHashTmp = stringHash;
         if ( asprintf( &stringHash, "%s%s", stringHashTmp, \
	      ((treeNode_t*)(Iterator->node))->hash_value) < 0) exit(8);

         free0( stringHashTmp );
      }

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

     free_gen_buf( &GenBuf_tBuffer );
     free( stringSQL );
     intRet = asprintf( &stringSQL, "INSERT INTO hash(hash_value,id) VALUES('%s',%s);", \
                                     stringHash, seq_next_val );

     free0( stringHash );
     intRet = bmd_db_execute_sql( dbhandle, stringSQL, WITHOUT_CURSOR, WITHOUT_ROWS, \
                               WITHOUT_COLS, &rows, &cols, result );

     free0( result );
     free0( stringSQL );

     intRet = asprintf( &stringSQL, "%s", "SELECT last_value FROM hash_id_seq;" );
     intRet = bmd_db_execute_sql( dbhandle, stringSQL, WITHOUT_CURSOR, WITHOUT_ROWS, \
	                          WITHOUT_COLS, &rows, &cols, &result );

     free0( stringSQL );

     intRet = bmd_db_result_get_value( dbhandle, result, 0, 0, &last_id, FETCH_NEXT, &fetched );
     free0( result );
     fetched = 0;

     for( ;j>0; j-- )
         {
           intRet = asprintf( &stringSQL, "INSERT INTO assoc(id, log_src) VALUES('%s', '%s');", \
	                      last_id, ((treeNode_t*)((lista.head)->node))->id );

	   intRet = bmd_db_execute_sql( dbhandle, stringSQL, WITHOUT_CURSOR, WITHOUT_ROWS, \
	                             WITHOUT_COLS, &rows, &cols, &result );

           DELETE( &lista, lista.head, DestroyTreeNode );

           free0( stringSQL );
           free0( result );
	 }
      free0( last_id );
   }

  /* DRUGA CZESC ALGORYTMU - POSZUKIWANIE POZOSTAŁYCH LOGÓW */

  if ( lista.ulCount > 1)
   {

   intRet = asprintf( &stringHash, "%s", "" );

   if ( intRet < 0 ) exit(7);

   for( Iterator=lista.head; Iterator != NULL ; Iterator=Iterator->prev )
    {
       stringHashTmp = stringHash;
       if ( asprintf( &stringHash, "%s%s", stringHashTmp, \
	      ((treeNode_t*)(Iterator->node))->hash_value) < 0) exit(8);

       free0( stringHashTmp );
    }

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

     free_gen_buf( &GenBuf_tBuffer );
     free( stringSQL );
     intRet = asprintf( &stringSQL, "INSERT INTO hash(hash_value,id) VALUES('%s',%s);", \
                                     stringHash, seq_next_val );

     free0( stringHash );
     intRet = bmd_db_execute_sql( dbhandle, stringSQL, WITHOUT_CURSOR, WITHOUT_ROWS, \
                               WITHOUT_COLS, &rows, &cols, &result );

     free0( result );
     free0( stringSQL );

     intRet = asprintf( &stringSQL, "%s", "SELECT last_value FROM hash_id_seq;" );
     intRet = bmd_db_execute_sql( dbhandle, stringSQL, WITHOUT_CURSOR, WITHOUT_ROWS, \
	                          WITHOUT_COLS, &rows, &cols, &result );

     free0( stringSQL );

     intRet = bmd_db_result_get_value( dbhandle, result, 0, 0, &last_id, FETCH_NEXT, &fetched );
     free0( result );
     fetched = 0;

     for( ;lista.head; )
         {
           intRet = asprintf( &stringSQL, "INSERT INTO assoc(id, log_src) VALUES('%s', '%s');", \
	                      last_id, ((treeNode_t*)((lista.head)->node))->id );

	   intRet = bmd_db_execute_sql( dbhandle, stringSQL, WITHOUT_CURSOR, WITHOUT_ROWS, \
	                             WITHOUT_COLS, &rows, &cols, &result );

           DELETE( &lista, lista.head, DestroyTreeNode );

           free0( stringSQL );
           free0( result );
	 }

     free0( last_id );
   }

 intRet = bmd_db_disconnect(&dbhandle);

 return 0;
}


