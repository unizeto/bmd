#include <bmd/libbmdlog/libbmdlog.h>
#include <bmd/libbmdglobals/libbmdglobals.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>



//extern long _GLOBAL_debug_level;



int main()
{
 int         ret =    0;
 log_t* log_tLog = NULL;
 char* stringTmp = NULL;
 err_t  err_tRet =    0;
 int           i =    0;
 int      intPid =    0;
 int           j =    0;

 _GLOBAL_debug_level = 20;



  err_tRet = LogInit( "client.conf", &log_tLog);


  if ( BMD_ERR( err_tRet ))
   {
      BMD_BTERR( err_tRet );
      BMD_FREEERR( err_tRet );

   }


  err_tRet = LogCreate(log_tLog);

  if ( BMD_ERR( err_tRet ))
   {
      BMD_BTERR( err_tRet );
      BMD_FREEERR( err_tRet );
   }


//  asprintf(&stringTmp, "%d", info);

  err_tRet = LogSetParam( log_level, "11:4", log_tLog );

  if ( BMD_ERR( err_tRet ))
   {
      BMD_BTERR( err_tRet );
      BMD_FREEERR( err_tRet );
   }

   free( stringTmp);

   err_tRet = LogSetParam( log_owner, "Wiktacy", log_tLog );


   if ( BMD_ERR( err_tRet ))
    {
        BMD_BTERR( err_tRet );
        BMD_FREEERR( err_tRet );
    }

    err_tRet = LogSetParam( service, "MagFakt", log_tLog );

    if ( BMD_ERR( err_tRet ))
     {
        BMD_BTERR( err_tRet );
        BMD_FREEERR( err_tRet );
     }

    err_tRet = LogSetParam( operation_type, "odczyt danych z BMD", log_tLog );

    if ( BMD_ERR( err_tRet ))
     {
        BMD_BTERR( err_tRet );
        BMD_FREEERR( err_tRet );
     }

    err_tRet = LogSetParam( log_referto, "log dotyczacy Mieczyslawa", log_tLog );

    if ( BMD_ERR( err_tRet ))
     {
        BMD_BTERR( err_tRet );
        BMD_FREEERR( err_tRet );
     }

    err_tRet = LogSetParam( date_time_begin, "2011-01-29 07:21:33.001000", log_tLog );

    if ( BMD_ERR( err_tRet ))
     {
        BMD_BTERR( err_tRet );
        BMD_FREEERR( err_tRet );
     }

    err_tRet = LogSetParam( event_code, "345", log_tLog );

    if ( BMD_ERR( err_tRet ))
     {
        BMD_BTERR( err_tRet );
        BMD_FREEERR( err_tRet );
     }


    err_tRet = LogSetParam(user_data1, "jakieś dane1", log_tLog );

    if ( BMD_ERR( err_tRet ))
     {
        BMD_BTERR( err_tRet );
        BMD_FREEERR( err_tRet );
     }

    err_tRet = LogSetParam(user_data2, "jakieś dane2", log_tLog );

    if ( BMD_ERR( err_tRet ))
     {
        BMD_BTERR( err_tRet );
        BMD_FREEERR( err_tRet );
     }

    err_tRet = LogSetParam(user_data3, "jakieś dane3", log_tLog );

    if ( BMD_ERR( err_tRet ))
     {
        BMD_BTERR( err_tRet );
        BMD_FREEERR( err_tRet );
     }

    err_tRet = LogSetParam(user_data4, "jakieś dane4", log_tLog );

    if ( BMD_ERR( err_tRet ))
     {
        BMD_BTERR( err_tRet );
        BMD_FREEERR( err_tRet );
     }

    err_tRet = LogSetParam(user_data5, "jakieś dane5", log_tLog );

    if ( BMD_ERR( err_tRet ))
     {
        BMD_BTERR( err_tRet );
        BMD_FREEERR( err_tRet );
     }

    err_tRet = LogCommit(log_tLog);

    if ( BMD_ERR( err_tRet ))
     {
        BMD_BTERR( err_tRet );
        BMD_FREEERR( err_tRet );
        printf("Tutaj wlazlem\n");
     }

    err_tRet = LogEnd(&log_tLog);

    if ( BMD_ERR( err_tRet ))
     {
        BMD_BTERR( err_tRet );
        BMD_FREEERR( err_tRet );
     }

 printf("\n Wynik : \n %d \n", i);

 return 0;
}
