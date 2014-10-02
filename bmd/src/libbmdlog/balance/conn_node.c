/************************************
 * Implementacja funkcji do obsługi *
 * połączeń dla serwera logów       *
 *				    *
 ************************************/


#include <bmd/libbmdlog/balance/conn_node.h>
#include <bmd/libbmdutils/libbmdutils.h>



/**
 * Funkcja tworzy nowy węzeł dla każdego klienta 
 * przyporządkowując go do danego serwera.
 * 
 * @param[in, out] connNode_tNode - tworzony węzeł
 * @param[in] _PID - PID klienta
 * @param[in] _servID - identyfikator serwera
 *
 * @return status zakończenia operacji
 * @retval BMD_ERR_PARAM1 - niepoprawny pierwszy parametr
 * @retval BMD_ERR_PARAM2 - niepoprawny drugi parametr
 * @retval BMD_ERR_PARAM3 - niepoprawny trzeci parametr
 * @retval BMD_ERR_MEMORY - błąd alokacji pamięci
 * @retval BMD_OK - zakończenie pomyślne
 *
*/

long CreateConnNode( connNode_t** const connNode_tNode, const pid_t _PID, \
                    const char* const _servID )
 {
  
    /* --------------- */
   
       long intRet = 0;

    /* --------------- */
 
 
    if ( connNode_tNode == NULL )
     {
	 return BMD_ERR_PARAM1;
     }
    
    if ( _PID <= 0 )
     {
	 return BMD_ERR_PARAM2;
     }
  
    if ( _servID == NULL )
     {
	 return BMD_ERR_PARAM3;
     }
     
    *connNode_tNode = (connNode_t*)calloc(1, sizeof( connNode_t ));
  
    if ( (*connNode_tNode) == NULL )
     {
 	 return BMD_ERR_MEMORY;
     }
  
    intRet = asprintf( &((*connNode_tNode)->servID), "%s", _servID );

     if ( intRet < 0 )
      {
         return BMD_ERR_MEMORY;
      }

    (*connNode_tNode)->PID = _PID;
  
   return BMD_OK;
  
 }

/**
 * Funkcja usuwa podany węzeł.
 * 
 * @param[in] connNode_tNode - aktualny węzeł
 *
 * @return status zakończenia operacji
 * @retval BMD_ERR_PARAM1 - niepoprawny pierwszy parametr
 * @retval BMD_OK - zakończenie pomyślne
 *
*/

long DestroyConnNode( void* const connNode_tNode )
 {
  
   if ( connNode_tNode == NULL )
    {
     return BMD_ERR_PARAM1;
    }
 
   free( ((connNode_t*)connNode_tNode)->servID );
   free( (connNode_t*)connNode_tNode );
  
  return BMD_OK;
 }


/**
 * Funkcja porównuje bieżący węzeł z zadanym elementem.
 * 
 * @param[in] connNode_tNode - bieżący węzeł
 * @param[in] elem - porównywany element
 * 
 * @return status zakończenia operacji
 * @retval BMD_ERR_PARAM1 - niepoprawny pierwszy parametr
 * @retval BMD_ERR_PARAM2 - niepoprawny drugi parametr
 * @retval EQUAL_CONN_ELEMENT - dopasowano element
 * @retval NOT_EQUAL_CONN_ELEMENT - nie dopasowano elementu
 *
*/

long CmpConnElem(const void* const connNode_tNode, const void* const elem)
 {

   if ( connNode_tNode == NULL )
    {
     return BMD_ERR_PARAM1;
    }

   if ( elem == NULL )
    {
     return BMD_ERR_PARAM2;
    }

   if ( ((connNode_t*)connNode_tNode)->PID == *((pid_t*)elem) )
    {
     return EQUAL_CONN_ELEMENT; /* dopasowano element */
    }

  return NOT_EQUAL_CONN_ELEMENT; /* nie dopasowano elementu */

 }


