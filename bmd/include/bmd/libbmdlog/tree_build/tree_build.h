#ifndef __TREE_BUILD_H__
#define __TREE_BUILD_H__

#include <bmd/libbmdlog/balance/atd.h>
#include <bmd/libbmdcms/libbmdcms.h>
#include <bmd/libbmderr/err_stack.h>

 
/* ---------------------------------------------------- */

   typedef enum { BUILD_ALL, BUILD_OPM } buildAction_t;

/* ---------------------------------------------------- */
/* API */

/* ------------------------------------------------------------ */

   err_t ReadLogFromDatabase( void* const, ATD* const );
   err_t EvaluateTreeL0( void* const, ATD* const, long, \
                         const buildAction_t );
   err_t EvaluateOtherTreeLevels( void* const, ATD* const, \
                                  bmd_crypt_ctx_t* const, \
			          long );
		       
		       
/* ------------------------------------------------------------- */
   
   
#endif

