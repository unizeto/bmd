/*******************************************
 *  Implementacja listy do przechowywania  *
 *  dowolnego rodzaju węzła.               *
 *					   *
 *  Autor : Tomasz Klimek		   *
 *  Data : 22-08-2007			   *
 *					   *
 *******************************************/

#include <bmd/libbmdlog/balance/list.h>
#include <bmd/libbmdutils/libbmdutils.h>

/**
* Funkcja inicjuje strukturę listy.
*
* @param[in, out] list_tList - uchwyt do listy
*
* @return status zakończenia
* @retval BMD_ERR_PARAM1 - niepoprawny pierwszy parametr
* @retval O - zakończenie pomyślne
*/


long InitList( list_t* const list_tList )
{

    if ( list_tList == NULL )
     {
        return BMD_ERR_PARAM1;
     }  
 
    list_tList->head = NULL;
    list_tList->tail = NULL;
    list_tList->ulCount = 0;
    
 return 0;
}


/**
* Funkcja wstawia nowy element do listy.
*
* @param[in, out] list_tList - uchwyt do listy
* @param[in] _node - wstawiany węzeł
*
* @return status zakończenia 
* @retval BMD_ERR_PARAM1 - niepoprawny pierwszy parametr
* @retval BMD_ERR_MEMORY - błąd alokacji pamięci
* @retval O - zakończenie pomyślne
*/


long InsertToList( list_t* const list_tList, void* const _node )
 {
 
   /* ---------------------------------- */
   
      listNode_t* listNode_tNode = NULL;

   /* ---------------------------------- */


    if ( list_tList == NULL )
     {
        return BMD_ERR_PARAM1;
     }  

    listNode_tNode = (listNode_t*)calloc(1, sizeof(listNode_t));
   
    if ( listNode_tNode == NULL )
     {
        return BMD_ERR_MEMORY;
     }
    
    listNode_tNode->prev = NULL;
    listNode_tNode->node = _node;
  
    if ( list_tList->tail != NULL )
     {
        list_tList->tail->prev = listNode_tNode;
        listNode_tNode->next = list_tList->tail;
     }
    else
     {
        listNode_tNode->next = NULL;
	list_tList->head = listNode_tNode;
     }

    list_tList->tail = listNode_tNode;
    (list_tList->ulCount)++;

   return 0;

 }
 
/**
* Funkcja wyszukuje zadany węzeł na liście.
*
* @param[in] list_tList - uchwyt do listy
* @param[in] select - wskaźnik do funkcji porównującej
* @param[in] elem - poszukiwany element
* @param[out] out - wskaźnik na odnaleziony węzeł
*
* @return status zakończenia operacji
* @retval BMD_ERR_PARAM1 - niepoprawny pierwszy parametr
* @retval BMD_ERR_PARAM2 - niepoprawny drugi parametr
* @retval BMD_ERR_PARAM3 - niepoprawny trzeci parametr
* @retval BMD_ERR_PARAM4 - niepoprawny czwarty parametr
* @retval FOUND_IN_LIST (1)  - odnaleziono wyszukiwany węzeł
* @retval 0 - zakończenie pomyślne
*/


long FindInLogList( list_t* list_tList, long (*select)(const void* const, const void* const), \
            const void* const elem, const direction_t searchDirection, void** const out)
 {
  
   /* ----------------------------------- */
   
      long          intRet         =    0;
      listNode_t*  listNode_tNode = NULL;     

   /* ----------------------------------- */


   if ( list_tList == NULL )
    {
	return BMD_ERR_PARAM1;
    }
   
   if ( select == NULL )
    {
	return BMD_ERR_PARAM2;
    }

   if ( elem == NULL )
    {
	return BMD_ERR_PARAM3;
    }

   if ( list_tList->head == NULL || list_tList->tail == NULL )
    {
        return EMPTY_LIST;
    }

   if ( searchDirection == forward )
    {
      listNode_tNode = list_tList->head;

      while( 1 )
       {
  	 intRet = select( listNode_tNode->node, elem );

	 if ( intRet == FOUND_IN_LIST ) 
          {
	     if ( out )
              {
	         (*out) = listNode_tNode;
	      } 
	     break;
          }

         listNode_tNode = listNode_tNode->prev;

         if ( listNode_tNode == NULL )
          {
             break;
          }
       }
    }
   else if ( searchDirection == backward )
    {
      listNode_tNode = list_tList->tail;

      while( 1 )
       {
  	 intRet = select( listNode_tNode->node, elem );

	 if ( intRet == FOUND_IN_LIST ) 
          {
	     if ( out )
	      {
                 (*out) = listNode_tNode;
	      }	 
	     break;
          }

         listNode_tNode = listNode_tNode->next;

         if ( listNode_tNode == NULL )
          {
             break;
          }
       }
    }

  return intRet;

 }
 
/**
* Funkcja usuwa zadany węzeł z listy.
*
* @param[in] list_tList - uchwyt do listy
* @param[in] elem - usuwany węzeł
* @param[out] del - wskaźnik do funkcji usuwającej zawartość węzła
*
* @return status zakończenia operacji
* @retval BMD_ERR_PARAM1 - niepoprawny pierwszy parametr
* @retval BMD_ERR_PARAM2 - niepoprawny drugi parametr
* @retval BMD_ERR_PARAM3 - niepoprawny trzeci parametr
* @retval EMPTY_LIST - lista jest pusta
* @retval O  - zakończenie pomyślne
*/

long DeleteFromLogList( list_t* const list_tList, void** const elem, void (*del)(void*) )
 {

    void* delNode = NULL;



    if ( list_tList == NULL )
     {
        return BMD_ERR_PARAM1;
     }
 
    if ( elem == NULL || *elem == NULL )
     {
        return BMD_ERR_PARAM2;
     } 

    if ( list_tList->head == NULL || list_tList->tail == NULL )
     {
        return EMPTY_LIST;
     }

    if ( del != NULL ) 
     {
        del( ((listNode_t*)(*elem))->node );
     }

    delNode = (*elem);

    if ( ((listNode_t*)(*elem))->next != NULL )
     { 
         ((listNode_t*)(*elem))->next->prev = ((listNode_t*)(*elem))->prev;
     }

    if ( ((listNode_t*)(*elem))->prev != NULL )
     {
        ((listNode_t*)(*elem))->prev->next = ((listNode_t*)(*elem))->next;
     } 

    if ( (listNode_t*)delNode == list_tList->tail )
     {
        list_tList->tail = ((listNode_t*)delNode)->next;
     }
 
    if ( (listNode_t*)delNode == list_tList->head )
     {
        list_tList->head = ((listNode_t*)delNode)->prev; 
     }
 
    if ( del == NULL )
     {
//         delNode = (*elem);
         (*elem) = ((listNode_t*)(*elem))->node;
     }

    (list_tList->ulCount)--;
    free( delNode );

  return 0;

 }
 

