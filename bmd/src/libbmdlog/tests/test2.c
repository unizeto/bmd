#include <bmd/libbmdlog/libbmdlog.h>
#include <bmd/libbmderr/libbmderr.h>

extern long _GLOBAL_debug_level;


int main(int argc, char* argv[])
{
 int         ret =    0;
 log_t* log_tLog = NULL;
 char* stringTmp = NULL;
 err_t  err_tRet =    0;
 int           i =    0;
 int      intPid =    0;
 int           j =    0;
 int           N =    0;


 N = atoi(argv[1]);


 _GLOBAL_debug_level = 4;


 for( i=1; i<=N; i++ ) {

    err_tRet = LogInit( "client.conf", &log_tLog);

    if ( BMD_ERR( err_tRet )) {
	BMD_BTERR( err_tRet );
	BMD_FREEERR( err_tRet );
    }

    err_tRet = LogCreate(log_tLog);

    if ( BMD_ERR( err_tRet )) {
	BMD_BTERR( err_tRet );
    	BMD_FREEERR( err_tRet );
    }

    asprintf(&stringTmp, "11:4");

    err_tRet = LogSetParam( log_level, stringTmp, log_tLog );

    if ( BMD_ERR( err_tRet )) {
	BMD_BTERR( err_tRet );
	BMD_FREEERR( err_tRet );
    }

    free( stringTmp);

    err_tRet = LogSetParam( log_owner, "Willow", log_tLog );

    if ( BMD_ERR( err_tRet )) {
	BMD_BTERR( err_tRet );
	BMD_FREEERR( err_tRet );
    }

    err_tRet = LogSetParam( service, "SOP", log_tLog );

    if ( BMD_ERR( err_tRet )) {
	BMD_BTERR( err_tRet );
	BMD_FREEERR( err_tRet );
    }

    err_tRet = LogSetParam( operation_type, "zażółć gęślą jaźń", log_tLog );

    if ( BMD_ERR( err_tRet )) {
	BMD_BTERR( err_tRet );
	BMD_FREEERR( err_tRet );
    }

    err_tRet = LogSetParam( log_referto, "Sysytem obsługi eFaktur", log_tLog );

    if ( BMD_ERR( err_tRet )) {
	BMD_BTERR( err_tRet );
	BMD_FREEERR( err_tRet );
    }

    err_tRet = LogSetParam( event_code, "345", log_tLog );

    if ( BMD_ERR( err_tRet )) {
	BMD_BTERR( err_tRet );
	BMD_FREEERR( err_tRet );
    }


    err_tRet = LogSetParam( log_referto_event_visible, "1", log_tLog );

    if ( BMD_ERR( err_tRet )) {
	BMD_BTERR( err_tRet );
	BMD_FREEERR( err_tRet );
    }

    err_tRet = LogSetParam( date_time_begin, "2010-02-12 09:28:54.413454", log_tLog );

    if ( BMD_ERR( err_tRet )) {
	BMD_BTERR( err_tRet );
	BMD_FREEERR( err_tRet );
    }

    err_tRet = LogCommit(log_tLog);

    if ( BMD_ERR( err_tRet )) {
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

 printf("\n LOG nr : \n %d \n", i);

 //sleep(1);

}


 return 0;
}
