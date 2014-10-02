/***************************************************************
 *                        libbmdlog.c                          *
 * Biblioteka dla konstruowania drzewa z podziałem sekretu     *
 * drzewo Merkla                                               *
 *                                                             *
 * Sr 18 Lipiec 2007 15:45                                     *
 *                                                             *
 * Autor : Unizeto Technologies SA.                            *
 *                                                             *
 ***************************************************************/

#include <time.h>
#include <string.h>
#include <bmd/libbmdlog/libbmdlog.h>
#include <bmd/libbmdutils/libbmdutils.h>
#include <bmd/libbmdlog/log_server_utils/log_server_utils.h>
#include <bmd/libbmdlog/balance/atd.h>
#include <bmd/libbmdlog/tree_build/tree_build.h>
#include <bmd/libbmdlog/tree_build/tree_node.h>
#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmddb/libbmddb.h>
#include <bmd/libbmdglobals/libbmdglobals.h>

void printLista(ATD* const lista);
void printNode(ATD* const lista, treeNode_t* tn);
void printfNodeStatus(treeNode_t* tn);



// w tej funkcji występują tylko odczyty z bazy danych
err_t ReadLogFromDatabase( void* const dbhandle, ATD* const lista )
{

  /* -------------------------------------------- */

     char*              stringSQL      =    NULL;
     long               rows           =       0;
     long               cols           =       0;
     long               intRet         =       0;
     err_t              err_tRet       =       0;
     void*              result         =    NULL;
     char*              stringMax      =    NULL;
     long long max                     =       0;
     db_row_strings_t*  rowstruct      =    NULL;
     treeNode_t*        treeNode_tNode =    NULL;
     long  fetched        =       0;
     cLevel_t           cLevel_tWHERE  = nowhere;

  /* -------------------------------------------- */

  if ( dbhandle == NULL )
   {
      BMD_SETERR( err_tRet, BMD_ERR_PARAM1 );
      BMD_SETIERR( err_tRet, AD, "Nieprawidłowy uchwyt do bazy danych" );

      return err_tRet;
   }

  if ( lista == NULL )
   {
      BMD_SETERR( err_tRet, BMD_ERR_PARAM2 );
      BMD_SETIERR( err_tRet, AD, "Nieprawidłowy uchwyt do listy" );

      return err_tRet;
   }

  if ( asprintf( &stringSQL, "SELECT max(log_src) FROM assoc;" ) < 0)
   {
      BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
      BMD_SETIERR( err_tRet, AD, "Błąd alokacji pamięci" );

      return err_tRet;
   }

   cLevel_tWHERE = clevel1;

   /* **************************************   LEVEL 1   ***************************************** */

  if ( bmd_db_execute_sql( dbhandle, stringSQL, &rows, &cols, &result ) < 0 )
   {
      BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
      BMD_SETIERR( err_tRet, AD, "Błąd odczytu najstarszego indeksu logu z tabeli assoc" );

      goto GARBAGE;
   }

   cLevel_tWHERE = clevel2;

   /* **************************************   LEVEL 2   ***************************************** */


  if ( bmd_db_result_get_value( dbhandle, result, 0, 0, &stringMax, FETCH_NEXT, &fetched ) < 0 )
   {
      BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
      BMD_SETIERR( err_tRet, AD, "Błąd pobrania najstarszego indeksu logu z tabeli assoc\n" );

      goto GARBAGE;
   }

   cLevel_tWHERE = clevel3;

   /* **************************************   LEVEL 3   ***************************************** */

  max = atoll( stringMax );

  fetched = 0;
  free0( stringSQL );
  bmd_db_result_free( &result );

  if ( asprintf( &stringSQL, "SELECT id, hash_value FROM log WHERE id > %lld ORDER BY id;", max ) < 0)
   {
      BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
      BMD_SETIERR( err_tRet, AD, "Błąd alokacji pamięci" );

      goto GARBAGE;
   }

  if ( bmd_db_execute_sql( dbhandle, stringSQL, &rows, &cols, &result) < 0 )
   {
      BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
      BMD_SETIERR( err_tRet, AD, "Błąd zapytania do bazy danych" );

      goto GARBAGE;
   }

  cLevel_tWHERE = clevel4;

   /* **************************************   LEVEL 4   ***************************************** */

  do{
      intRet = bmd_db_result_get_row( dbhandle, result, 0, 0, &rowstruct, FETCH_NEXT, \
                                          &fetched );

      if ( intRet == 0)
       {
         if ( CreateTreeNode( &treeNode_tNode, rowstruct->colvals[0], \
	                        rowstruct->colvals[1], logNode ) < 0 )
          {
              BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
              BMD_SETIERR( err_tRet, AD, "Błąd przy tworzeniu węzła drzewa" );

              goto GARBAGE;
          }

         if ( INSERT( lista, treeNode_tNode  ) < 0 )
          {
              BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
              BMD_SETIERR( err_tRet, AD, "Błąd umieszczania elementu na liście" );
              DestroyTreeNode( treeNode_tNode );

              goto GARBAGE;
          }
         //printNode(lista, treeNode_tNode);
         bmd_db_row_struct_free( &rowstruct );
       }
    } while( intRet == 0);





 GARBAGE :

  switch( cLevel_tWHERE )
  {
    case clevel4 : { bmd_db_row_struct_free( &rowstruct ); }
    case clevel3 : { free0( stringMax ); }
    case clevel2 : { bmd_db_result_free( &result ); }
    case clevel1 : { free0( stringSQL );}
    case nowhere : {}
    default : {}
  }

  return err_tRet;

}




err_t EvaluateTreeL0( void* const dbhandle, ATD* const lista, \
                      long uiPacket, const buildAction_t action )
{

  /* -------------------------------------------- */

     char*              stringSQL      =    NULL;
     char*              stringHash     =    NULL;
     char*              stringHashTmp  =    NULL;
     char*              seq_next_val   =    NULL;
     char*              last_id        =    NULL;
     long                rows           =       0;
     long                cols           =       0;
     long                N              =       0;
     long                i              =       0;
     long                j              =       0;
     long                k              =       0;
     void*              result         =    NULL;
     GenBuf_t*          GenBuf_tBuffer =    NULL;
     err_t              err_tRet       =       0;
     long  fetched        =       0;
     ATD_PTR            Iterator       =    NULL;
     cLevel_t           cLevel_tWHERE  = nowhere;

  /* -------------------------------------------- */

  if ( dbhandle == NULL )
   {
      BMD_SETERR( err_tRet, BMD_ERR_PARAM1 );
      BMD_SETIERR( err_tRet, AD, "Niepoprawny uchwyt do bazy danych" );

      return err_tRet;
   }

  if ( lista == NULL )
   {
      BMD_SETERR( err_tRet, BMD_ERR_PARAM2 );
      BMD_SETIERR( err_tRet, AD, "Nieprawidłowy uchwyt do listy" );

      return err_tRet;
   }

  if ( uiPacket <= 0 )
   {
      BMD_SETERR( err_tRet, BMD_ERR_PARAM3 );
      BMD_SETIERR( err_tRet, AD, "Nieprawidłowa wielkość paczki" );

      return err_tRet;
   }

  if ( (action != BUILD_ALL) && (action != BUILD_OPM) )
   {
      BMD_SETERR( err_tRet, BMD_ERR_PARAM4 );
      BMD_SETIERR( err_tRet, AD, "Nieprawidłowa akcja" );

      return err_tRet;
   }


  for( k=0; k<2 && lista->ulCount > 0; k++ )
   {
     if ( action == BUILD_ALL )
      {
        N = ( lista->ulCount < uiPacket ) ? \
              uiPacket = lista->ulCount, lista->ulCount : \
	      lista->ulCount - (lista->ulCount % uiPacket);
      }
     else
      {
        N = lista->ulCount - (lista->ulCount % uiPacket);
      }

     for( i=0; i<N; i+=uiPacket )
      {
        if ( asprintf( &stringHash, "%s", "" ) < 0 )
         {
           BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
           BMD_SETIERR( err_tRet, AD, "Błąd alokacji pamięci" );

           return err_tRet;
         }

        cLevel_tWHERE = clevel1;

  /* **************************************   LEVEL 1   ***************************************** */

        for( j=0, Iterator=lista->head; j < uiPacket; j++ )
         {
            stringHashTmp = stringHash;

            if ( asprintf( &stringHash, "%s%s", stringHashTmp, \
                 ((treeNode_t*)(Iterator->node))->hash_value) < 0 )
             {
                BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
                BMD_SETIERR( err_tRet, AD, "Błąd alokacji pamięci" );

                goto GARBAGE;
             }
            free0( stringHashTmp );
            Iterator=Iterator->prev;
         }

        cLevel_tWHERE = clevel2;

  /* **************************************   LEVEL 2   ***************************************** */

        seq_next_val = bmd_db_get_sequence_nextval( dbhandle, "hash_id_seq");

        if ( seq_next_val == NULL )
         {
            BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
            BMD_SETIERR( err_tRet, AD, "Błąd odczytu kolejnego indeksu" );

            goto GARBAGE;
         }

        cLevel_tWHERE = clevel3;

   /* **************************************   LEVEL 3   ***************************************** */

        if ( set_gen_buf2( (char*)stringHash, strlen( stringHash ), &GenBuf_tBuffer ) < 0 )
         {
            BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
            BMD_SETIERR( err_tRet, AD, "Błąd inicjowania GenBufa" );

            goto GARBAGE;
         }

        cLevel_tWHERE = clevel4;

   /* **************************************   LEVEL 4   ***************************************** */

        free0( stringHash );
        err_tRet = CreateHash( GenBuf_tBuffer, &stringHash );

        if ( BMD_ERR( err_tRet ) )
         {
            BMD_BTERR( err_tRet );
            BMD_FREEERR( err_tRet );

            BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
            BMD_SETIERR( err_tRet, AD, "Błąd obliczania skrótu" );

            goto GARBAGE;
         }

        free_gen_buf( &GenBuf_tBuffer );

        if ( asprintf( &stringSQL, "INSERT INTO hash(hash_value,id) VALUES('%s',%s);", \
                                    stringHash, seq_next_val ) < 0 )
         {
            BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
            BMD_SETIERR( err_tRet, AD, "Błąd alokacji pamięci" );

            goto GARBAGE;
         }

        free0( seq_next_val );

        cLevel_tWHERE = clevel5;

   /* **************************************   LEVEL 5   ***************************************** */

        free0( stringHash );
        if ( bmd_db_start_transaction( dbhandle,BMD_TRANSACTION_LEVEL_NONE) < 0 )
         {

            BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
            BMD_SETIERR( err_tRet, AD, "Błąd inicjowania transakcji" );

            goto GARBAGE;
         }

        if ( bmd_db_execute_sql( dbhandle, stringSQL, &rows, &cols, &result ) < 0 )
         {
            BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
            BMD_SETIERR( err_tRet, AD, "Błąd zapisu nowego hasha do bazy" );

            goto GARBAGE;
         }

        cLevel_tWHERE = clevel6;

   /* **************************************   LEVEL 6   ***************************************** */

        bmd_db_result_free( &result );
        free0( stringSQL );

        if ( asprintf( &stringSQL, "%s", "VALUES prevval FOR hash_id_seq;" ) < 0 )
        //if ( asprintf( &stringSQL, "%s", "SELECT last_value FROM hash_id_seq;" ) < 0 )
         {
            BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
            BMD_SETIERR( err_tRet, AD, "Błąd alokacji pamięci" );

            goto GARBAGE;
         }

        if ( bmd_db_execute_sql( dbhandle, stringSQL, &rows, &cols, &result ) < 0 )
         {
            BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
            BMD_SETIERR( err_tRet, AD, "Błąd odczytu ostatniego indeksu" );

            goto GARBAGE;
         }

        free0( stringSQL );
        last_id = NULL;
        if ( bmd_db_result_get_value( dbhandle, result, 0, 0, &last_id, FETCH_NEXT, &fetched ) < 0 )
         {
            BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
            BMD_SETIERR( err_tRet, AD, "Błąd pobrania ostatniego indeksu" );

            goto GARBAGE;
         }

        cLevel_tWHERE = clevel7;

   /* **************************************   LEVEL 7   ***************************************** */

        bmd_db_result_free( &result );
        fetched = 0;

        for( ;j>0; j-- )
         {
           if ( asprintf( &stringSQL, "INSERT INTO assoc(id, log_src) VALUES('%s', '%s');", \
	                  last_id, ((treeNode_t*)((lista->head)->node))->id ) < 0 )
            {
               BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
               BMD_SETIERR( err_tRet, AD, "Błąd alokacji pamięci" );

               goto GARBAGE;
            }

           if ( bmd_db_execute_sql( dbhandle, stringSQL, &rows, &cols, &result ) < 0 )
            {
               BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
               BMD_SETIERR( err_tRet, AD, "Błąd zapisu do tabeli assoc" );

               goto GARBAGE;
            }

           DELETE( lista, (void**)(&(lista->head)), DestroyTreeNode );

           free0( stringSQL );
           bmd_db_result_free( &result );
         }

        free0( last_id );

        if ( bmd_db_end_transaction( dbhandle ) < 0 )
         {
            BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
            BMD_SETIERR( err_tRet, AD, "Błąd zakończenia transakcji" );

            goto GARBAGE;
         }
      }
   }


 /* ------------------------------------------------- */

 GARBAGE :

  switch( cLevel_tWHERE )
  {
    case clevel7 : { free0( last_id ); }
    case clevel6 : { bmd_db_result_free( &result ); }
    case clevel5 : { free0( stringSQL ); }
    case clevel4 : { free_gen_buf( &GenBuf_tBuffer ); }
    case clevel3 : { free0( seq_next_val ); }
    case clevel2 : { free0( stringHash ); }
    case clevel1 : { free0( stringHashTmp ); }
    case nowhere  : {}
    default  : {}
  }

  /* ------------------------------------------------- */

 return err_tRet;

}


err_t EvaluateOtherTreeLevels( void* const dbhandle, ATD* const lista, \
                               bmd_crypt_ctx_t* const signContext, \
                               long uiPacket )

{

   /* -------------------------------------------------- */

      char*                   stringSQL       =    NULL;
      char*                   stringHashTmp   =    NULL;
      char*                   stringHash      =    NULL;
      char*                   seq_next_val    =    NULL;
      char*                   last_id         =    NULL;
      char*                   stringSignHash  =    NULL;
      char*                   stringMax       =    NULL;
      long                     intRet          =       0;
      long                     intStrCmp       =       0;
      long                     rows            =       0;
      long                     cols            =       0;
      long                     i               =       0;
      long                     j               =       0;
      long                     N               =       0;
      char                 *longLobID       =       0;
      void*                   result          =    NULL;
      db_row_strings_t*       rowstruct       =    NULL;
      long       fetched         =       0;
      long long      max             =       0;
      GenBuf_t*               GenBuf_tBuffer  =    NULL;
      GenBuf_t*               GenBuf_tSigData =    NULL;
      Attribute_t*            attrTime        =    NULL;
      SignedAttributes_t*     signAttrs       =    NULL;
      void*                   treeNode_tNode  =    NULL;
      err_t                   err_tRet        =       0;
      ATD_PTR                 Iterator        =    NULL;
      bmd_signature_params_t* sigParams       =    NULL;
      cLevel_t                cLevel_tWHERE   = nowhere;

  /* --------------------------------------------------- */


if ( dbhandle == NULL )
   {
      BMD_SETERR( err_tRet, BMD_ERR_PARAM1 );
      BMD_SETIERR( err_tRet, AD, "Nieprawidłowy uchwyt do bazy danych" );

      return err_tRet;
   }

  if ( lista == NULL )
   {
      BMD_SETERR( err_tRet, BMD_ERR_PARAM2 );
      BMD_SETIERR( err_tRet, AD, "Nieprawidłowy uchwyt do listy" );

      return err_tRet;
   }

  if ( signContext == NULL )
   {
      BMD_SETERR( err_tRet, BMD_ERR_PARAM3 );
      BMD_SETIERR( err_tRet, AD, "Nieprawidłowy uchwyt do kontekstu" );

      return err_tRet;
   }

  if ( uiPacket <= 0 )
   {
      BMD_SETERR( err_tRet, BMD_ERR_PARAM4 );
      BMD_SETIERR( err_tRet, AD, "Nieprawidłowa wielkość paczki" );

      return err_tRet;
   }

  if ( asprintf( &stringSQL, "SELECT max(hash_src) FROM assoc;" ) < 0)
   {
      BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
      BMD_SETIERR( err_tRet, AD, "Błąd alokacji pamięci" );

      return err_tRet;
   }

   cLevel_tWHERE = clevel1;

  /* **************************************   LEVEL 1   ***************************************** */

  if ( bmd_db_execute_sql( dbhandle, stringSQL, &rows, &cols, &result ) < 0 )
   {
      BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
      BMD_SETIERR( err_tRet, AD, "Błąd odczytu najstarszego indeksu hasha z tabeli assoc" );

      goto GARBAGE;
   }

  cLevel_tWHERE = clevel2;

  /* **************************************   LEVEL 2   ***************************************** */

  if ( bmd_db_result_get_value( dbhandle, result, 0, 0, &stringMax, FETCH_NEXT, &fetched ) < 0 )
   {
      BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
      BMD_SETIERR( err_tRet, AD, "Błąd pobrania najstarszego indeksu hasha z tabeli assoc" );

      goto GARBAGE;
   }

  cLevel_tWHERE = clevel3;

  /* **************************************   LEVEL 3   ***************************************** */

  max = atoll( stringMax );

  fetched = 0;
  free0( stringSQL );
  bmd_db_result_free( &result );

  if ( asprintf( &stringSQL, "SELECT id, hash_value, sig_value FROM hash WHERE id > %lld ORDER BY id;", \
       max ) < 0 )
   {
      BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
      BMD_SETIERR( err_tRet, AD, "Błąd alokacji pamięci" );

      goto GARBAGE;
   }

  if ( bmd_db_execute_sql( dbhandle, stringSQL,  &rows, &cols, &result ) < 0 )
   {
      BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
      BMD_SETIERR( err_tRet, AD, "Błąd zapytania do bazy danych" );

      goto GARBAGE;
   }

  cLevel_tWHERE = clevel4;

  /* **************************************   LEVEL 4   ***************************************** */

   do{
       intRet = bmd_db_result_get_row( dbhandle, result, 0, 0, &rowstruct, FETCH_NEXT, \
                                           &fetched );

       if ( intRet == 0)
        {
          intStrCmp = strcmp( rowstruct->colvals[2], "" );
          if ( CreateTreeNode( (treeNode_t**)(&treeNode_tNode), rowstruct->colvals[0], \
                                rowstruct->colvals[1], ( intStrCmp != 0 ) ? oldestHashNode : hashNode ) < 0 )
           {
              BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
              BMD_SETIERR( err_tRet, AD, "Błąd przy tworzeniu węzła drzewa" );
              //printf("");
              goto GARBAGE;
           }

          if ( INSERT( lista, treeNode_tNode ) < 0 )
           {
              BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
              BMD_SETIERR( err_tRet, AD, "Błąd umieszczania elementu na liście" );
              DestroyTreeNode( treeNode_tNode );

              goto GARBAGE;
           }
          //printNode(lista, treeNode_tNode);
          bmd_db_row_struct_free( &rowstruct );
        }

     }while( intRet == 0);
   //printLista(lista);
   bmd_db_result_free( &result );
   fetched=0;

   if ( bmd_db_start_transaction( dbhandle,BMD_TRANSACTION_LEVEL_NONE) < 0 )
    {
        BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
        BMD_SETIERR( err_tRet, AD, "Błąd inicjowania transakcji" );

        goto GARBAGE;
    }

   for( ; lista->ulCount > 1 || ( (lista->ulCount == 1) && \
         (((treeNode_t*)((lista->head)->node))->nodeStatus != oldestHashNode)) ; )
   {

     N = (lista->ulCount < uiPacket || uiPacket == 1) ? \
          uiPacket = lista->ulCount, lista->ulCount : \
          lista->ulCount - (lista->ulCount % uiPacket);

     for( i=0; i<N; i+=uiPacket )
      {
        if ( asprintf( &stringHash, "%s", "" ) < 0 )
         {

            BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
            BMD_SETIERR( err_tRet, AD, "Błąd alokacji pamięci" );

            goto GARBAGE;
         }

        cLevel_tWHERE = clevel5;

        /* **************************************   LEVEL 5   ***************************************** */

        int mi = 0;
        for( j=0, Iterator=lista->head; j < uiPacket; )
         {
        	mi ++;
        	if(mi >=20)
        	{
                BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
                BMD_SETIERR( err_tRet, AD, "Błąd umieszczania elementu na liście" );
                DestroyTreeNode( treeNode_tNode );

                goto GARBAGE;
        	}

            if ( Iterator != lista->tail && \
              (((treeNode_t*)(Iterator->node))->nodeStatus == oldestHashNode ) )
             {
                treeNode_tNode = (void*)Iterator;
                Iterator = Iterator->prev;
                DELETE( lista, &treeNode_tNode, NULL );

                if ( INSERT( lista, treeNode_tNode ) < 0 )
                 {
                    BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
                    BMD_SETIERR( err_tRet, AD, "Błąd umieszczania elementu na liście" );
                    DestroyTreeNode( treeNode_tNode );

                    goto GARBAGE;
                 }
             }
            else
             {
                stringHashTmp = stringHash;
                if ( asprintf( &stringHash, "%s%s", stringHashTmp, \
                   ((treeNode_t*)(Iterator->node))->hash_value ) < 0 )
                 {
                     BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
                     BMD_SETIERR( err_tRet, AD, "Błąd alokacji pamięci" );

                     goto GARBAGE;
                 }

                free0( stringHashTmp );
                Iterator=Iterator->prev;
                j++;
             }
         }

        cLevel_tWHERE = clevel6;

        /* **************************************   LEVEL 6   ***************************************** */

        seq_next_val = bmd_db_get_sequence_nextval( dbhandle, "hash_id_seq");

        if ( seq_next_val == NULL )
         {
             BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
             BMD_SETIERR( err_tRet, AD, "Błąd alokacji pamięci" );

             goto GARBAGE;
         }

        cLevel_tWHERE = clevel7;

        /* **************************************   LEVEL 7   ***************************************** */

        free_gen_buf( &GenBuf_tBuffer );

        if ( set_gen_buf2( (char*)stringHash, strlen( stringHash ), &GenBuf_tBuffer ) < 0 )
         {
             BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
             BMD_SETIERR( err_tRet, AD, "Błąd alokacji GenBuffa" );

             goto GARBAGE;
         }

        cLevel_tWHERE = clevel8;

        /* **************************************   LEVEL 8   ***************************************** */

        free0( stringHash );
        err_tRet = CreateHash( GenBuf_tBuffer, &stringHash );

        if ( BMD_ERR( err_tRet ) )
         {
             BMD_BTERR( err_tRet );
             BMD_FREEERR( err_tRet );
             BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
             BMD_SETIERR( err_tRet, AD, "Błąd obliczania skrótu" );
             goto GARBAGE;
         }

        free_gen_buf( &GenBuf_tBuffer );

        if ( set_gen_buf2( (char*)stringHash, strlen( stringHash ), &GenBuf_tBuffer ) < 0 )
         {
             BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
             BMD_SETIERR( err_tRet, AD, "Błąd alokacji GenBuffa" );
             goto GARBAGE;
         }

        free0( stringSQL );

        if ( Iterator == NULL )
         {
            if ( SigAttr_Create_SigningTime(&attrTime) < 0 )
             {
                 BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
                 BMD_SETIERR( err_tRet, AD, "Błąd przy tworzeniu atrybutu" );
                 goto GARBAGE;
             }

            cLevel_tWHERE = clevel9;

            /* **************************************   LEVEL 9   ***************************************** */

            if ( SignedAtributes_init( &signAttrs ) < 0 )
             {
                 BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
                 BMD_SETIERR( err_tRet, AD, "Błąd przy inicjowaniu atrybutu" );
                 goto GARBAGE;
             }

            cLevel_tWHERE = clevel10;

            /* **************************************   LEVEL 10   ***************************************** */

            if ( SignedAtributes_add( attrTime, signAttrs ) < 0 )
             {
                 BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
                 BMD_SETIERR( err_tRet, AD, "Błąd przy dodawaniu atrybutu" );

                 goto GARBAGE;
             }

            if ( SigningParams_Create( signAttrs, NULL, &sigParams ) < 0 )
             {
                 BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
                 BMD_SETIERR( err_tRet, AD, "Błąd przy tworzeniu parametrów" );
                 goto GARBAGE;
             }

            cLevel_tWHERE = clevel11;

            /* **************************************   LEVEL 11   ***************************************** */

            sigParams->signature_flags = BMD_CMS_SIG_INTERNAL;

            if ( bmd_sign_data( GenBuf_tBuffer, signContext, &GenBuf_tSigData, \
                                (void*)sigParams ) < 0 )
             {
                 BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
                 BMD_SETIERR( err_tRet, AD, "Błąd przy składaniu podpisu" );
                 goto GARBAGE;
             }
            if ( bmd_db_export_blob( dbhandle, GenBuf_tSigData, &longLobID) < 0 )
             {
                 BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
                 BMD_SETIERR( err_tRet, AD, "Błąd przy eksportowaniu loba do BD" );

                 goto GARBAGE;
            }

            if ( asprintf( &stringSQL, "INSERT INTO hash(id,hash_value,sig_value)" \
                                       "VALUES(%s,'%s','%s');", \
                                        seq_next_val, stringHash, longLobID ) < 0 )
             {
                 BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
                 BMD_SETIERR( err_tRet, AD, "Błąd alokacji pamięci" );
                 goto GARBAGE;
             }

            SigningParams_Destroy( &sigParams );
            free_gen_buf( &GenBuf_tSigData );
            free0( stringSignHash );
            signAttrs = NULL;
            attrTime  = NULL;
         }
        else
         {
            if ( asprintf( &stringSQL, "INSERT INTO hash(hash_value,id) VALUES('%s',%s);", \
                                        stringHash, seq_next_val ) < 0 )
             {
                 BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
                 BMD_SETIERR( err_tRet, AD, "Błąd alokacji pamięci" );

                 goto GARBAGE;
             }
         }
        if ( bmd_db_execute_sql( dbhandle, stringSQL, &rows, &cols, &result ) < 0 )
         {
             BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
             BMD_SETIERR( err_tRet, AD, "Błąd zapisu hasha do BD" );
             goto GARBAGE;
         }

        bmd_db_result_free( &result );
        free0( stringSQL );
        free0( seq_next_val );

        if ( asprintf( &stringSQL, "%s", "VALUES prevval FOR hash_id_seq;" ) < 0 )
         {
             BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
             BMD_SETIERR( err_tRet, AD, "Błąd alokacji pamięci" );

             goto GARBAGE;
         }
        if ( bmd_db_execute_sql( dbhandle, stringSQL,  &rows, &cols, &result ) < 0 )
         {
             BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
             BMD_SETIERR( err_tRet, AD, "Błąd zapytania o last hash_id do BD" );
             goto GARBAGE;
         }

        free0( stringSQL );
        free0(last_id);
        if ( bmd_db_result_get_value( dbhandle, result, 0, 0, &last_id, \
	                          FETCH_NEXT, &fetched ) < 0 )
         {
             BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
             BMD_SETIERR( err_tRet, AD, "Błąd pobierania last hash_id z BD" );

             goto GARBAGE;
         }
        if ( CreateTreeNode( (treeNode_t**)(&treeNode_tNode), last_id, stringHash, \
                             (Iterator == NULL) ? oldestHashNode : hashNode ) < 0 )
         {
             BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
             BMD_SETIERR( err_tRet, AD, "Błąd przy tworzeniu węzła drzewa" );

             goto GARBAGE;
         }

        if ( INSERT( lista, treeNode_tNode ) < 0 )
         {
             BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
             BMD_SETIERR( err_tRet, AD, "Błąd przy umieszczaniu węzła na liście" );
             DestroyTreeNode( treeNode_tNode );

             goto GARBAGE;
         }

        free0( stringHash );
        bmd_db_result_free( &result );
        fetched = 0;

        for( ;j>0; j-- )
         {
            if ( ((treeNode_t*)((lista->head)->node))->nodeStatus == logNode )
             {
                if ( asprintf( &stringSQL, "INSERT INTO assoc(id, log_src) VALUES('%s','%s');", \
                                            last_id, ((treeNode_t*)((lista->head)->node))->id ) < 0 )
                 {
                    BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
                    BMD_SETIERR( err_tRet, AD, "Błąd alokacji pamięci" );

                    goto GARBAGE;
                 }
             }
            else
             {
                if ( asprintf( &stringSQL, "INSERT INTO assoc(id, hash_src) VALUES('%s','%s');", \
                                            last_id, ((treeNode_t*)((lista->head)->node))->id ) < 0 )
                 {
                    BMD_SETERR( err_tRet, BMD_ERR_MEMORY );
                    BMD_SETIERR( err_tRet, AD, "Błąd alokacji pamięci" );

                    goto GARBAGE;
                 }
             }
            if ( bmd_db_execute_sql( dbhandle, stringSQL, &rows, &cols, &result ) < 0 )
             {
                BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
                BMD_SETIERR( err_tRet, AD, "Błąd zapisu struktury poddrzewa do BD" );

                goto GARBAGE;
             }

            DELETE( lista, (void**)(&(lista->head)), DestroyTreeNode );
            free0( stringSQL );
            bmd_db_result_free( &result );
         }
        free0(last_id);
      }
   }
  if ( bmd_db_end_transaction( dbhandle ) < 0 )
   {
      BMD_SETERR( err_tRet, BMD_ERR_OP_FAILED );
      BMD_SETIERR( err_tRet, AD, "Błąd zakończenia transakcji" );
   }


  /* ------------------------------------------------------------- */

 GARBAGE :

  switch( cLevel_tWHERE )
  {
    case clevel11 : {
                      SigningParams_Destroy( &sigParams );
                      signAttrs = NULL;
                      attrTime  = NULL;
                    }
    case clevel10 : { SignedAttributes_destroy( &signAttrs ); }
    case clevel9  : { Attribute_destroy( &attrTime ); }
    case clevel8  : { free_gen_buf( &GenBuf_tBuffer ); }
    case clevel7  : { free0( seq_next_val ); }
    case clevel6  : { free0( stringHash ); }
    case clevel5  : { free0( stringHashTmp ); }
    case clevel4  : { bmd_db_row_struct_free( &rowstruct ); }
    case clevel3  : { free0( stringMax ); }
    case clevel2  : { bmd_db_result_free( &result ); }
    case clevel1  : { free0( stringSQL );}
    default       : {}
  }

  /* -------------------------------------------------------------- */

  return err_tRet;

}

void printLista(ATD* const lista)
{
	  ATD_PTR Iterator = lista->head;
	  printf("-------------------Lista head-------------------\n");
	  if(Iterator == NULL)
	  {
		  printf("NULL\n");
		  return;
	  }
	  printf("%s\n", ((treeNode_t*)(Iterator->node))->id);
	  printfNodeStatus((treeNode_t*)(Iterator->node));
	  printf("%s\n", ((treeNode_t*)(Iterator->node))->hash_value);

	  while(Iterator!=lista->tail)
	  {
		  printf("%s\n", ((treeNode_t*)(Iterator->node))->id);
		  printfNodeStatus((treeNode_t*)(Iterator->node));
		  printf("%s\n", ((treeNode_t*)(Iterator->node))->hash_value);
		  Iterator = Iterator->prev;
	  }
	  printf("-------------------Lista tail-------------------\n");
}
void printNode(ATD* const lista, treeNode_t* tn)
{
	if(lista == NULL)
		return;

	if(tn == NULL)
			return;
	  printf("Node id: %s\n", ((treeNode_t*)tn)->id);
	  printfNodeStatus(tn);
	  printf("Node hash_value%s\n", ((treeNode_t*)tn)->hash_value);

	  if(tn == (treeNode_t*)lista->head)
		  printf("Is lista->head\n");
  	  if(tn == (treeNode_t*)lista->tail)
  		printf("Is lista->tail\n");

}

void printfNodeStatus(treeNode_t* tn)
{
	if (tn == NULL)
		return;
	printf("Node status: ");
	switch( tn->nodeStatus )
	{
	case logNode:
		printf("logNode");
		break;
	case hashNode:
		printf("hashNode");
		break;
	case oldestHashNode:
		printf("oldestHashNode");
		break;
	default:
		break;
	}
	printf("\n");
}
