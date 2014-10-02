/*****************************************
 *                                       *
 * Plik zawiera deklaracje funkcji       *
 * obsługi sygnałów w serwerach logów    *
 * BMD - pierwsza warstwa.               *
 *                                       *
 * Data : 27-08-2007                     *
 * Autor : Tomasz Klimek                 *
 *                                       *
 *****************************************/


#ifndef __ROOTSERVER_SIGNAL_H__
#define __ROOTSERVER_SIGNAL_H__

#include <sys/types.h>
#include <sys/wait.h>

 void sigChildDHandler( int signal );
 void sigChangeParams( int signal );

#endif

