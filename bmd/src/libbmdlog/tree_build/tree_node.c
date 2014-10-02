/************************************
 * Implementacja funkcji do budowy  *
 * zrownoważonego drzewa logów.     *
 *				    *
 ************************************/


#include <bmd/libbmdlog/tree_build/tree_node.h>
#include <bmd/libbmdutils/libbmdutils.h>

/**
 * Funkcja tworzy nowy węzeł drzewa 
 * dla każdego hasha z tabeli hash.
 * 
 * @param[in, out] treeNode_tNode - tworzony węzeł
 * @param[in] hashId - identyfiaktor hasha
 * @param[in] hashValue - hash
 *
 * @return status zakończenia operacji
 * @retval BMD_ERR_PARAM1 - niepoprawny pierwszy parametr
 * @retval BMD_ERR_PARAM2 - niepoprawny drugi parametr
 * @retval BMD_ERR_PARAM3 - niepoprawny trzeci parametr
 * @retval BMD_ERR_MEMORY - błąd alokacji pamięci
 * @retval BMD_OK - zakończenie pomyślne
 *
*/

int CreateTreeNode( treeNode_t** const treeNode_tNode, const char* const hashId, \
                    const char* const hashValue, const nodeType_t type )
 {
  
    /* --------------- */
   
       int intRet = 0;

    /* --------------- */
 
 
    if ( treeNode_tNode == NULL )
     {
	 return BMD_ERR_PARAM1;
     }
    
    if ( hashId == NULL )
     {
	 return BMD_ERR_PARAM2;
     }
  
    if ( hashValue == NULL )
     {
	 return BMD_ERR_PARAM3;
     }
     
    *treeNode_tNode = (treeNode_t*)calloc(1, sizeof( treeNode_t ));
  
    if ( (*treeNode_tNode) == NULL )
     {
 	 return BMD_ERR_MEMORY;
     }
  
    intRet = asprintf( &((*treeNode_tNode)->hash_value), "%s", hashValue );

     if ( intRet < 0 )
      {
         return BMD_ERR_MEMORY;
      }

    intRet = asprintf( &((*treeNode_tNode)->id), "%s", hashId );

     if ( intRet < 0 )
      {
         return BMD_ERR_MEMORY;
      }

    (*treeNode_tNode)->nodeStatus = type;

   return BMD_OK;
  
 }

/**
 * Funkcja usuwa podany węzeł.
 * 
 * @param[in] treeNode_tNode - aktualny węzeł
 *
 * @return status zakończenia operacji
 * @retval BMD_ERR_PARAM1 - niepoprawny pierwszy parametr
 * @retval BMD_OK - zakończenie pomyślne
 *
*/

void DestroyTreeNode( void* const treeNode_tNode )
 {
  
   free( ((treeNode_t*)treeNode_tNode)->hash_value );
   free( ((treeNode_t*)treeNode_tNode)->id );
   free( (treeNode_t*)treeNode_tNode );
  
 }


