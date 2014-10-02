#ifndef __BMD_LOG_SERVER__
#define __BMD_LOG_SERVER__

#include <signal.h>
#include <sys/wait.h>
#include <bmd/libbmdpki/libbmdpki.h>
#include <bmd/libbmderr/err_stack.h>

 /* ---------------------------------------------------------------------------- */

    #define HASH_ALGORITHM 	BMD_HASH_ALGO_SHA1
    #define BUILD_L0 			1
    #define BUILD_LN 			2
    #define STOP_SERVER 		3
    #define NOTHING 			0

 /* ---------------------------------------------------------------------------- */

    typedef struct
     {
       /* ------------------------ */

          char*		ip;
          char*		port;
   	  char*		base_name;
	  char*		user;
	  char*		password;

       /* ------------------------ */

     }dataBaseParam_t;

 /* ---------------------------------------------------------------------------- */

    typedef struct
    {

      /* --------------------------------------- */

         char*			ip;
         long			port;
         long			attempt;
         long			delay;
         long			max_clients;
         char*			temp_directory;
         long			log_amount;
         long			sign_time;
         dataBaseParam_t	baseParam;
         bmd_crypt_ctx_t*	signContext;
         void*			dbhandle;

      /* --------------------------------------- */ 
 
    } bmdlogServParam_t;


 /* ----------------------------------------------------------------------------- */

    err_t BMDLogServerParamsInit( bmdlogServParam_t* const );
    err_t getBMDLogServerParams( const char* const, bmdlogServParam_t* const );

 /* ----------------------------------------------------------------------------- */

#endif
