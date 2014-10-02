/* **************************************************
 * 						    *
 * Węzeł przechowywujący informacje o obciążeniu    *
 * danego serwera tzn. ilość aktywnych klientów     *
 *						    *
 * Data : 22-08-2007                                *
 * Autor : Tomasz Klimek                            *
 *                                                  *
 * **************************************************
*/

#ifndef __LOAD_NODE__
#define __LOAD_NODE__

#include <stdio.h>


    #define EQUAL_LOAD_ELEMENT 1
    #define NOT_EQUAL_LOAD_ELEMENT 0


 typedef struct
  {
   
     /* ------------------------ */
   
        char*            servID;   /* identyfikator serwera */
        long     connectionCount;   /* liczba klientów */
     
     /* ------------------------ */

  } loadNode_t;


 /* -------------------------------------------------------- */
 
 /* API */ 

    long CreateLoadNode( loadNode_t** const, const char* const );
    long DestroyLoadNode( void* const );
    long CmpLoadElem( const void* const, const void* const);

 /* -------------------------------------------------------- */
  
#endif

