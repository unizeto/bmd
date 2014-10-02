#include <bmd/libbmddb/postgres/pq_libbmddb.h>
#include <bmd/libbmdlog/libbmdlog.h>
#include <bmd/libbmdutils/libbmdutils.h>
#include <bmd/libbmdlog/log_server_utils/log_server_utils.h>
#include <bmd/libbmdlog/balance/atd.h>
#include <bmd/libbmdlog/bmdlogserver/tree.h>
#include <time.h>
#include <string.h>

#define PACKET_SIZE 3

extern long _GLOBAL_debug_level;

void delString( void* );

int main()
{

  /* ----------------------------------------- */

     int                intRet         =    0;
     int                intDbRet       =    0;
     void*              dbhandle       = NULL;
     void*              hashResult     = NULL;
     char*              stringSQL      = NULL;
     char*              stringHash     = NULL;
     char*              stringHashTmp  = NULL;
     int                            rows,cols;
     int                i              =    0;
     char*              seq_next_val   = NULL;
     char*              last_id        = NULL;
     char*              log_id         = NULL;
     unsigned long int  hashLastValue  =    0;
     unsigned long int  hashFetched    =    0;
     db_row_strings_t*  rowstruct      = NULL;
     GenBuf_t*          GenBuf_tBuffer = NULL;
     err_t              err_tRet       =    0;
     treeNode_t*        treeNode_tNode = NULL;
     ATD                             hashList;
     ATD_PTR            Iterator       = NULL;
  /* ----------------------------------------- */


  _GLOBAL_debug_level = 2;

  intRet = INIT( &hashList );

  if ( intRet < 0 ) exit(1);

  intRet = bmd_db_connect2("127.0.0.1", "5432", "logi", "tklimek", "", &dbhandle);

  if ( intRet < 0) exit(1);

  intRet = asprintf( &stringSQL, "%s", "SELECT * FROM hash WHERE NOT EXISTS" \
                      "(SELECT * FROM assoc WHERE hash.id = assoc.hash_src) order by hash.id;" );

  if ( intRet < 0 ) exit(1);

  intRet = bmd_db_execute_sql( dbhandle, stringSQL, WITHOUT_CURSOR, WITHOUT_ROWS, \
                            WITHOUT_COLS, &rows, &cols, &hashResult);

  if ( intRet < 0 ) exit(1);


 do{
     intDbRet = bmd_db_result_get_row( dbhandle, hashResult, 0, 0, &rowstruct, FETCH_NEXT, \
                                           WITHOUT_COLNAMES, &hashFetched );
     if ( intDbRet == 0)
      {
       if ( CreateTreeNode( &treeNode_tNode, rowstruct->colvals[0], rowstruct->colvals[1], False ) < 0) exit(1);
       if ( INSERT( &hashList, treeNode_tNode ) < 0 ) exit(1);

       free0( rowstruct );
      }

   }while( intDbRet == 0);

 free0( hashResult);
 hashFetched=0;

 for(; hashList.head; )
  {
    intRet = asprintf( &stringHash, "%s", "" );

    if ( intRet < 0 ) exit(1);

    for( i=0, Iterator=hashList.head; (i<PACKET_SIZE)&&Iterator; i++, Iterator=Iterator->prev )
     {
       stringHashTmp = stringHash;
       if ( asprintf( &stringHash, "%s%s", stringHashTmp, \
          ((treeNode_t*)(Iterator->node))->hash_value )< 0) exit(1);

       free( stringHashTmp );
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

        if ( Iterator == NULL )
	 fprintf(stderr, "\nOSTATNI HASH\n");

        free0( stringSQL );
        intRet = asprintf( &stringSQL, "INSERT INTO hash(hash_value,id) VALUES('%s',%s);", \
                                        stringHash, seq_next_val );

        intRet = bmd_db_execute_sql( dbhandle, stringSQL, WITHOUT_CURSOR, WITHOUT_ROWS, \
	                          WITHOUT_COLS, &rows, &cols, &hashResult );

        free0( hashResult );
        free0( stringSQL );

        intRet = asprintf( &stringSQL, "%s", "SELECT last_value FROM hash_id_seq;" );
        intRet = bmd_db_execute_sql( dbhandle, stringSQL, WITHOUT_CURSOR, WITHOUT_ROWS, \
	                          WITHOUT_COLS, &rows, &cols, &hashResult );

        free0( stringSQL );

        intRet = bmd_db_result_get_value( dbhandle, hashResult, 0, 0, &last_id, FETCH_NEXT, &hashFetched );

        if ( CreateTreeNode( &treeNode_tNode, last_id, stringHash, False ) < 0) exit(1);
        if ( INSERT( &hashList, treeNode_tNode ) < 0 ) exit(1);

        free0( stringHash );
        free0( hashResult );
        hashFetched = 0;

        for( ;i>0; i-- )
         {
           intRet = asprintf( &stringSQL, "INSERT INTO assoc(id, hash_src) VALUES('%s','%s');", \
	                      last_id, ((treeNode_t*)((hashList.head)->node))->id );

	   intRet = bmd_db_execute_sql( dbhandle, stringSQL, WITHOUT_CURSOR, WITHOUT_ROWS, \
	                             WITHOUT_COLS, &rows, &cols, &hashResult );

           fprintf(stderr, "%s\n", stringSQL );

           DELETE( &hashList, hashList.head, DestroyTreeNode );

           free0( stringSQL );
           free0( hashResult );
	 }
       free0(last_id);
     }
    else break;
  }

 intRet = bmd_db_disconnect(&dbhandle);

 return 0;
}


