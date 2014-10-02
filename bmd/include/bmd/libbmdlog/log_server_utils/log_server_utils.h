#ifndef __LOG_SERVER_UTILS__
#define __LOG_SERVER_UTILS__

#include <bmd/libbmdlog/libbmdlog.h>
/*#include <bmd/libbmddb/postgres/pq_libbmddb.h>*/
#include <bmd/libbmdlog/root_server/root_server.h>
#include <bmd/libbmdlog/bmdlogserver/bmdlogserver.h>

/*doane ze wzglêu na db2 i weryfikator*/
long  successfullyFetched =    0;

//#define OVECTOR_NUM 100

  
 /* ---------------------------------------------------------------------------- */
 /* API */

    err_t CreateSQLCommand( const log_t* const, char** const );
    err_t CreateHash( GenBuf_t* const, char** const );
    err_t CreateHashfromLog( log_t* const );
    err_t FillGenBuf( const log_t* const, GenBuf_t** const );
    err_t GetSubstring( const char* const, const char* const, char*** const );
    err_t FreeLog_tElements( log_t* const, const option_t );
    long Connect2RootServer( bmdnet_handler_ptr const, void** const );
    long Connect2BMDLogServer( bmdnet_handler_ptr const, void** const );
    err_t GenerateFileName( const char* const, char** const );
    err_t SaveLogOnDisk( const char* const, GenBuf_t* const );
    err_t CopyServersTable( void** const , void*** const );
    err_t ChangeServersTable( void** const, long );
    err_t getEventsPolicy( void*, char**** const );
    long getTime( char** const );
    err_t rootServerParamsInit( rootServParam_t* const );
    err_t rootServerParamsCpy( rootServParam_t* const, const paramCase_t, \
                               rootServParam_t* const );
    err_t getRootServerParams( const char* const, const paramCase_t, \
                               rootServParam_t* const );
    err_t BMDLogServerParamsInit( bmdlogServParam_t* const );
    err_t getBMDLogServerParams( const char* const, bmdlogServParam_t* const );
    err_t LoadLogFromDisk( const char* const, bmdnet_handler_ptr* const, \
                           long (*)( bmdnet_handler_ptr const, void** const ), \
		           void** const );    
    err_t LogConnect( bmdnet_handler_ptr* const, \
                      long (*)( bmdnet_handler_ptr const, void** const ), \
		      void** const, long *const );
    err_t LogSendLoop( bmdnet_handler_ptr* const, \
                       long (*)( bmdnet_handler_ptr const, void** const ), \
		       void** const, GenBuf_t* const );

 /* ---------------------------------------------------------------------------- */

 

#endif
