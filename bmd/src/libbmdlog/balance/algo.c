/************************************
 * Implementacje funkcji            *
 * szeregowania                     *
 *				    *
 ************************************/


#include <bmd/libbmdlog/balance/algo.h>
#include <bmd/libbmdlog/root_server/root_server.h>


/**
 * Funkcja szeregująca
*/

long FirstFree( struct rootServParam* const rootServParams, \
               logServParam_t*** const logServTable )
 {
  
 
    if ( rootServParams == NULL )
     {
	 return BMD_ERR_PARAM1;
     }
    
    if ( logServTable == NULL )
     {
	 return BMD_ERR_PARAM2;
     }
  
    
    (*logServTable) = rootServParams->LogServersTab;

  
   return 0;
  
 }


