/**
 * Implementacja funkcji do obsługi
 * obciążenia dla serwera logów
*/

#include <bmd/libbmdlog/balance/load_node.h>
#include <bmd/libbmdutils/libbmdutils.h>

/**
 * Funkcja tworzy nowy węzeł informacji o obciążeniu dla
 * danego serwera.
 *
 * @param[in] _servID - identyfikator serwera.
 * @param[in, out] loadNode_tNode - tworzony węzeł.
 *
 * @return status zakończenia operacji
 * @retval BMD_ERR_PARAM1 - niepoprawny pierwszy parametr
 * @retval BMD_ERR_PARAM2 - niepoprawny drugi parametr
 * @retval BMD_ERR_MEMORY - błąd alokacji pamięci
 * @retval BMD_OK - pomyślne zakończenie
 *
*/


long CreateLoadNode( loadNode_t** const loadNode_tNode, const char* const _servID )
 {

    /* --------------- */
      
       long intRet = 0;

    /* --------------- */
  

    if ( loadNode_tNode == NULL )
     {
 	 return BMD_ERR_PARAM1;  /* nieprawidłowy pierwszy parametr */
     }
    
    if ( _servID == NULL )
     {
  	 return BMD_ERR_PARAM2;  /* nieprawidłowy drugi parametr */
     }

    *loadNode_tNode = (loadNode_t*)calloc(1, sizeof( loadNode_t ));
  
    if ( (*loadNode_tNode) == NULL )
     {
         return BMD_ERR_MEMORY;
     }
  
    intRet = asprintf( &((*loadNode_tNode)->servID), "%s",  _servID );
    
    if ( intRet < 0 )
     {
         return BMD_ERR_MEMORY;
     }

    (*loadNode_tNode)->connectionCount = 1;
  
  return BMD_OK;
  
 }

/**
 * Funkcja usuwa węzeł informacji o obciążeniu dla
 * danego serwera.
 *
 * @param[in] loadNode_tNode - usuwany węzeł.
 *
 * @return status zakończenia operacji
 * @retval BMD_ERR_PARAM1 - niepoprawny pierwszy parametr
 * @retval BMD_OK - pomyślne zakończenie
 *
*/
 

long DestroyLoadNode( void* const loadNode_tNode )
 {
   
   if ( loadNode_tNode == NULL )
    {
	return BMD_ERR_PARAM1; /* nieprawidłowy pierwszy parametr */
    }
   

   free( ((loadNode_t*)loadNode_tNode)->servID );
   free( (loadNode_t*) loadNode_tNode );
  
  return BMD_OK;
 }
 

/**
 * Funkcja porównuje bieżący węzeł z zadanym elementem.
 *
 * @param[in] elem - porównywany element
 * @param[in] loadNode_tNode - aktualny węzeł.
 *
 * @return status zakończenia operacji
 * @retval BMD_ERR_PARAM1 - niepoprawny pierwszy parametr
 * @retval BMD_ERR_PARAM2 - niepoprawny drugi parametr
 * @retval EQUAL_LOAD_ELEMENT - dopasowano element
 * @retval NOT_EQUAL_LOAD_ELEMENT - nie dopasowano elementu
 *
*/

 
long CmpLoadElem(const void* const loadNode_tNode, const void* const elem)
 {

   if ( loadNode_tNode == NULL )
    {
     return BMD_ERR_PARAM1; /* nieprawidłowy pierwszy parametr */
    }

   if ( elem == NULL )
    {
     return BMD_ERR_PARAM2; /* nieprawidłowy drugi parametr */
    }

   if ( strcmp( ((loadNode_t*)loadNode_tNode)->servID, (char*)elem ) == 0 )
    {
     return EQUAL_LOAD_ELEMENT; /* dopasowano element */
    }

  return NOT_EQUAL_LOAD_ELEMENT; /* nie dopasowano elementu */
 }


