/* ***********************************************
 *  						 *
 * Węzeł przechowywujący informacje o przydziale *
 * danego klienta do konkretnego serwera,	 *
 * realizowane przy pomocy PIDu		         *
 *				 		 *
 * Data : 23-08-2007                             *
 * Autor : Tomasz Klimek                         *
 *                                               *
 *************************************************
*/

#ifndef __CONNECTION_NODE__
#define __CONNECTION_NODE__

#include <sys/types.h>
#include <unistd.h>


    #define EQUAL_CONN_ELEMENT 1
    #define NOT_EQUAL_CONN_ELEMENT 0

 typedef struct
  {
   
     /* ------------------------ */
   
        pid_t               PID;  /* PID procesu */
        char*            servID;  /* identyfikator serwera */
     
     /* ------------------------ */

  } connNode_t;


 /* ----------------------------------------------------------------- */
 
 /* API */
 
    long CreateConnNode( connNode_t** const, const pid_t, const char* const );
    long DestroyConnNode( void* const );
    long CmpConnElem( const void* const, const void* const ); 

 /* ------------------------------------------------------------------ */

  
#endif

