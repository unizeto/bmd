/*****************************************
 *                                       *
 * Plik zawiera deklaracje funkcji       *
 * obsługi sygnałów w serwerach logów    *
 * BMD - druga warstwa.                  *
 *                                       *
 * Data : 27-08-2007                     *
 * Autor : Tomasz Klimek                 *
 *                                       *
 *****************************************/


#ifndef __BMDLOGSERVER_SIGNAL_H__
#define __BMDLOGSERVER_SIGNAL_H__

#include <sys/types.h>
#include <sys/wait.h>

 /* --------------------------------------- */

    void sigChildDHandler( int );
    void sigChildDStopServer( int );
    void signTime( int );
    void signTimer( int );
    void newLog( int, siginfo_t*, void * );
    void buildL0( int );

 /* --------------------------------------- */

#endif


