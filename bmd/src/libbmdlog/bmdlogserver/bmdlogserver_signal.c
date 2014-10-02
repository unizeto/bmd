/*****************************************
 *                                       *
 * Plik zawiera funkcje obsługi sygnałów *
 * w serwerach logów BMD - druga warstwa *
 *                                       *
 * Data : 27-08-2007                     *
 * Autor : Tomasz Klimek                 *
 *                                       *
 *****************************************/


#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <bmd/libbmdlog/bmdlogserver/bmdlogserver.h>
#include <bmd/libbmdglobals/libbmdglobals.h>


void sigChildDHandler( int signal )
 {

    /* --------------- */

       int intPid = 0;  

    /* --------------- */

   while (( intPid = waitpid( -1, NULL, WNOHANG )) > 0 );

 }


void sigChildDStopServer( int signal )
 {
   _GLOBAL_action = STOP_SERVER;
 }


void newLog(int num, siginfo_t* info, void *other )
 {
 
    /* ---------------------------------------- */
    
       static volatile sig_atomic_t packet = 0;

    /* ---------------------------------------- */

    packet++;

    if ( packet >= _GLOBAL_bmdlogServParam_tParams.log_amount )
     {
       packet = 0;
       kill( getppid(), SIGUSR1 ); // możliwe sprawdzenie błędu ?
     }
 }



void signTime( int signal )
 {
  ( _GLOBAL_action != STOP_SERVER ) ? _GLOBAL_action = BUILD_LN : 0;
 }



void buildL0( int signal )
 {
   ( _GLOBAL_action != BUILD_LN && _GLOBAL_action != STOP_SERVER ) ? _GLOBAL_action = BUILD_L0 : 0;
 }



void signTimer( int signal )
 {
    kill( getppid(), SIGALRM ); // możliwe sprawdzenie błędu ?
 }


