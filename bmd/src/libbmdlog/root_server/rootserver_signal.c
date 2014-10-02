/*****************************************
 *                                       *
 * Plik zawiera funkcje obsługi sygnałów *
 * w serwerach logów BMD - pierwsza      *
 * warstwa                               *
 *                                       *
 * Data : 27-08-2007                     *
 * Autor : Tomasz Klimek                 *
 *                                       *
 *****************************************/

#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <bmd/libbmdutils/libbmdutils.h>
#include <bmd/libbmdlog/root_server/root_server.h>
#include <bmd/libbmdglobals/libbmdglobals.h>



void sigChildDHandler( int signal )
 {

   /* --------------- */

      int intPid = 0;

   /* --------------- */

   while (( intPid = waitpid( -1, NULL, WNOHANG )) > 0 );
   
 }

void sigChangeParams( int signal )
 {

   /* ------------------- */

      err_t err_tRet = 0;

   /* ------------------- */


   err_tRet = getRootServerParams( _GLOBAL_rootServParam_tParams.confFile, change_params, \
                                   &_GLOBAL_rootServParam_tParams );

   if ( BMD_ERR( err_tRet ) )
    {
        BMD_BTERR( err_tRet );
        BMD_FREEERR( err_tRet );
    }

 }
