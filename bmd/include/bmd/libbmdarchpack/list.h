/****************************************************
 *                                                  *
 * Implementacja listy do przechowywania dowolnego  *
 * rodzaju węzła.                                   *
 *                                                  *
 * Data : 22-08-2007                                *
 * Autor : Tomasz Klimek                            *
 *                                                  *
 ****************************************************/

#ifndef __LIST__
#define __LIST__

#include <stdio.h>


    #define FOUND_IN_LIST 1
    #define NOT_FOUND_IN_LIST 0
    #define EMPTY_LIST -5


  typedef struct listNode
   {
     /* ----------------------- */

        void*             node;  /* przechowywany węzeł */
        struct listNode*  prev;  /* poprzedni węzeł */
        struct listNode*  next;  /* następny węzeł */

     /* ----------------------- */

   } listNode_t;

  typedef struct
   {
     /* ------------------- */

        listNode_t*   head;  /* początek listy */
        listNode_t*   tail;  /* koniec listy */
        long       ulCount;  /* liczba elementów */

     /* ------------------- */

   } list_t;

 /* --------------------------------------------------------------------------------------------------------- */

 /* API */

    int InitList( list_t* const );
    int InsertToList( list_t* const, void* const );
    int FindInList( list_t*, int (*)(const void* const, const void* const),const void* const, void** const );
    int DeleteFromList( list_t* const, listNode_t* const, void (*)(void*) );

 /* --------------------------------------------------------------------------------------------------------- */


#endif

