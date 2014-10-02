/****************************************************
 *					            *
 * Implementacja listy do przechowywania dowolnego  *
 * rodzaju węzła. 				    *
 *						    *
 * Data : 22-08-2007                                *
 * Autor : Tomasz Klimek                            *
 *						    *
 ****************************************************/

#ifndef __LOG_LIST__
#define __LOG_LIST__

#include <stdio.h>
#include <bmd/invoice_conv/list.h>

  typedef enum { forward, backward } direction_t; /* kierunek przeglądania listy */

 /* --------------------------------------------------------------------------------------------------------- */

 /* API */
    
    long DeleteFromLogList( list_t* const, void** const, void (*)(void*) );
    long FindInLogList( list_t*, long (*_select)(const void* const, const void* const), \
                const void* const, const direction_t, void** const );
    
  /* --------------------------------------------------------------------------------------------------------- */
 
 
#endif


