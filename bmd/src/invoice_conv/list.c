/****************************************************
 *                                                  *
 * Implementacja listy do przechowywania dowolnego  *
 * rodzaju węzła.                                   *
 *                                                  *
 * Data : 22-08-2007                                *
 * Autor : Tomasz Klimek                            *
 *                                                  *
 ****************************************************/

#include <bmd/invoice_conv/list.h>
#include <bmd/libbmdutils/libbmdutils.h>
#include <bmd/libbmderr/libbmderr.h>
/**
* Funkcja inicjuje strukturę listy.
*
* @param[in, out] list_tList - uchwyt do listy
*
* @return status zakończenia
* @retval BMD_ERR_PARAM1 - niepoprawny pierwszy parametr
* @retval BMD_OK - zakończenie pomyślne
*/


long InitList( list_t* const list_tList )
{

    if ( list_tList == NULL )
     {
       PRINT_ERROR("Niepoprawny pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
       return BMD_ERR_PARAM1;
     }

    list_tList->head = NULL;
    list_tList->tail = NULL;
    list_tList->ulCount = 0;

  return BMD_OK;

}


/**
* Funkcja wstawia nowy element do listy.
*
* @param[in,out] list_tList - uchwyt do listy
* @param[in] _node - wstawiany węzeł
*
* @return status zakończenia 
* @retval BMD_ERR_PARAM1 - niepoprawny pierwszy parametr
* @retval BMD_ERR_MEMORY - błąd alokacji pamięci
* @retval BMD_OK - zakończenie pomyślne
*/


long InsertToList( list_t* const list_tList, void* const _node )
 {

   /* ---------------------------------- */

      listNode_t* listNode_tNode = NULL;

   /* ---------------------------------- */

    if ( list_tList == NULL )
     {
       PRINT_ERROR("Niepoprawny pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
       return BMD_ERR_PARAM1;
     }

    listNode_tNode = (listNode_t*)calloc(1, sizeof(listNode_t));

    if ( listNode_tNode == NULL )
     {
       PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
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
    list_tList->ulCount++;

   return BMD_OK;

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
* @retval EMPTY_LIST (-5) - lista jest pusta
* @retval FOUND_IN_LIST (1) - odnaleziono poszukiwany węzeł
* @retval NOT_FOUND_IN_LIST (0) - nie odnaleziono poszukiwanego węzła
*
*/


long FindInList( list_t* list_tList, long (*select)(const void* const, const void* const),\
                const void* const elem, void** const out)
 {

   /* ----------------------------------- */

      long          intRet         =    0;
      listNode_t*  listNode_tNode = NULL;

   /* ----------------------------------- */


   if ( list_tList == NULL )
    {
      PRINT_ERROR("Niepoprawny pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
      return BMD_ERR_PARAM1;
    }

   if ( select == NULL )
    {
      PRINT_ERROR("Niepoprawny drugi parametr. Error = %d\n", BMD_ERR_PARAM2);
      return BMD_ERR_PARAM2;
    }

   if ( elem == NULL )
    {
      PRINT_ERROR("Niepoprawny trzeci parametr. Error = %d\n", BMD_ERR_PARAM3);
      return BMD_ERR_PARAM3;
    }

   if ( list_tList->head == NULL || list_tList->tail == NULL )
    {
      PRINT_INFO("Lista jest pusta\n");
      return EMPTY_LIST;
    }

   listNode_tNode = ( out && *out) ? *out : list_tList->head;

   while( 1 )
    {
      intRet = select( listNode_tNode->node, elem );

      if ( intRet == FOUND_IN_LIST )
       {
         PRINT_INFO("Odnaleziono poszukiwany element\n");

         if ( out ) (*out) = listNode_tNode;
         break;
       }

      listNode_tNode = listNode_tNode->prev;

      if ( listNode_tNode == NULL )
       {
         PRINT_INFO("Nie odnaleziono poszukiwanego elementu\n");
         break;
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
* @retval EMPTY_LIST - lista jest pusta
* @retval BMD_OK  - zakończenie pomyślne
*
*/

long DeleteFromList( list_t* const list_tList, listNode_t* const elem, void (*del)(void*) )
 {

    if ( list_tList == NULL )
     {
        PRINT_ERROR("Niepoprawny pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
        return BMD_ERR_PARAM1;
     }
 
    if ( elem == NULL )
     {
        PRINT_ERROR("Niepoprawny drugi parametr. Error = %d\n", BMD_ERR_PARAM2);
        return BMD_ERR_PARAM2;
     } 

    if ( list_tList->head == NULL || list_tList->tail == NULL )
     {
        PRINT_INFO("Lista jest pusta\n");
        return EMPTY_LIST;
     }

    if ( del ) del( elem->node );

    if ( elem->next != NULL )
     elem->next->prev = elem->prev;

    if ( elem->prev != NULL )
     elem->prev->next = elem->next;

    if ( elem == list_tList->tail )
     list_tList->tail = elem->next;

    if ( elem == list_tList->head )
     list_tList->head = elem->prev; 

    list_tList->ulCount--;

    free(elem);

  return BMD_OK;

 }

