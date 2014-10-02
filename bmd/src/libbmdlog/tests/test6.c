#include <bmd/libbmddb/postgres/pq_libbmddb.h>
#include <bmd/libbmdlog/libbmdlog.h>
#include <bmd/libbmdutils/libbmdutils.h>
#include <bmd/libbmdlog/log_server_utils/log_server_utils.h>
#include <bmd/libbmdlog/balance/atd.h>
#include <bmd/libbmdlog/bmdlogserver/tree.h>
#include <bmd/libbmdcms/libbmdcms.h>
#include <time.h>
#include <string.h>

#define PACKET_SIZE 2

extern long _GLOBAL_debug_level;

void delString( void* );

int main()
{

  /* ------------------------------------------------ */

     int                      intRet          =     0;
     void*                    dbhandle        =  NULL;
     void*                    result          =  NULL;
     char*                    stringSQL       =  NULL;
     char*                    stringHash      =  NULL;
     char*                    stringSignHash  =  NULL;
     char*                    stringHashTmp   =  NULL;
     int                                    rows,cols;
     int                      i               =     0;
     int                      j               =     0;
     int                      N               =     0;
     char*                    seq_next_val    =  NULL;
     char*                    last_id         =  NULL;
     char*                    log_id          =  NULL;
     unsigned long int        fetched         =     0;
     long int                 longLobID       =     0;
     db_row_strings_t*        rowstruct       =  NULL;
     GenBuf_t*                GenBuf_tBuffer  =  NULL;
     GenBuf_t*                GenBuf_tSigData =  NULL;
     Attribute_t*             attrTime        =  NULL;
     err_t                    err_tRet        =     0;
     treeNode_t*              treeNode_tNode  =  NULL;
     bmd_signature_params_t*  sigParams       =  NULL;
     bmd_crypt_ctx_t*         signContext     =  NULL;
     SignedAttributes_t*      signAttrs       =  NULL;
     nodeType_t                              nodeType;
     ATD_PTR                  Iterator        =  NULL;
     ATD                                     hashList;

  /* ------------------------------------------------- */


  _GLOBAL_debug_level = 2;

  intRet = INIT( &hashList );

  bmd_init();

  bmd_set_ctx_file( "tklimek.pfx", "tklimek:6", 9, &signContext );

  if ( intRet < 0 ) exit(1);

  intRet = bmd_db_connect2("127.0.0.1", "5432", "logi", "tklimek", "", &dbhandle);

  if ( intRet < 0) exit(1);

  intRet = asprintf( &stringSQL, "%s", "SELECT * FROM hash WHERE NOT EXISTS" \
          "(SELECT * FROM assoc WHERE hash.id = assoc.hash_src) order by hash.id;" );

  if ( intRet < 0 ) exit(1);

  // rozne zapytania



  intRet = bmd_db_execute_sql( dbhandle, stringSQL, WITHOUT_CURSOR, WITHOUT_ROWS, \
                            WITHOUT_COLS, &rows, &cols, &result );

  if ( intRet < 0 ) exit(1);

  do{
     intRet = bmd_db_result_get_row( dbhandle, result, 0, 0, &rowstruct, FETCH_NEXT, \
                                         WITHOUT_COLNAMES, &fetched );
     if ( intRet == 0)
      {
        nodeType = ( fetched == 1 ) ? oldestHashNode : hashNode;

        if ( CreateTreeNode( &treeNode_tNode, rowstruct->colvals[0], \
	                     rowstruct->colvals[1], nodeType ) < 0) exit(1);
        if ( INSERT( &hashList, treeNode_tNode ) < 0 ) exit(1);

        free0( rowstruct );
      }

   }while( intRet == 0);

  // rozne dane odczytywane z bazy


 free0( result );
 fetched=0;

 N = hashList.ulCount - ( hashList.ulCount % PACKET_SIZE );

 for( i=0; i<N; i+=PACKET_SIZE )
  {
    intRet = asprintf( &stringHash, "%s", "" );

    if ( intRet < 0 ) exit(1);

    if ( (((treeNode_t*)((hashList.head)->node))->nodeStatus == oldestHashNode ) \
         && hashList.ulCount > PACKET_SIZE )
     {
        PullOutFromList( &hashList, hashList.head, (void**)(&treeNode_tNode) );
        INSERT( &hashList, treeNode_tNode );
     }

     // dodatkowy warunek na najstarszy hash


    for( j=0, Iterator=hashList.head; j < PACKET_SIZE; j++, Iterator=Iterator->prev )
     {
       stringHashTmp = stringHash;
       if ( asprintf( &stringHash, "%s%s", stringHashTmp, \
          ((treeNode_t*)(Iterator->node))->hash_value )< 0) exit(1);

       free( stringHashTmp );
     }

     // taka sama petla

   /* -------------------------------------------------------------------- */

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

   /* -------------------------------------------------------------------- */
   // taki sam fragment kodu


    free_gen_buf( &GenBuf_tBuffer );

    intRet = set_gen_buf2( stringHash, strlen( stringHash ), &GenBuf_tBuffer );

    if ( intRet < 0 )
     {
        fprintf(stderr, "Blad GenBufa\n");
        exit(1);
     }

    free0( stringSQL );

    if ( Iterator == NULL )
     {
        intRet = SigAttr_Create_SigningTime(&attrTime);
        if (intRet < 0) exit(1);

	    intRet = SignedAtributes_init( &signAttrs );
	    if (intRet < 0) exit(2);

	    intRet = SignedAtributes_add( attrTime, signAttrs );
	    if (intRet < 0) exit(2);

	    intRet = SigningParams_Create( signAttrs, NULL, &sigParams );
	    if (intRet < 0) exit(3);

	    sigParams->signature_flags = BMD_CMS_SIG_INTERNAL;

	    intRet = bmd_sign_data( GenBuf_tBuffer, signContext, &GenBuf_tSigData, (void*)sigParams );
	    if (intRet < 0 ) exit(4);

            intRet = exportFileIntoLO( dbhandle, GenBuf_tSigData, &longLobID, WITH_TRANSACTION );
	    if (intRet < 0 ) exit(5);

            intRet = asprintf( &stringSQL, "INSERT INTO hash(id,hash_value,sig_value) VALUES(%s,'%s','%ld');", \
                                            seq_next_val, stringHash, longLobID );

	    SigningParams_Destroy( &sigParams );
            free_gen_buf( &GenBuf_tSigData );
	    free0( stringSignHash );
         }
        else
         {
           intRet = asprintf( &stringSQL, "INSERT INTO hash(hash_value,id) VALUES('%s',%s);", \
                                            stringHash, seq_next_val );
         }

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

 bmd_ctx_destroy( &signContext );
 bmd_end();

 intRet = bmd_db_disconnect(&dbhandle);

 return 0;
}


