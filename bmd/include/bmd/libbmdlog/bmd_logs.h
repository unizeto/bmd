#ifndef _HAVE_BMD_SERVER_LOGS_H
#define _HAVE_BMD_SERVER_LOGS_H

#ifndef WIN32
#include <bmd/libbmdlog/libbmdlog.h>
#include <bmd/libbmdlog/bmd_logs_policy.h>

/*Q_FATAL i V_FATAL wyswietlaja komunikaty nawet, gdy nie mozna utworzyc skrukture przechowujaca konfiguracje serwera */

#define Q_FATAL(fmt, args...) \
			if(_GLOBAL_bmd_configuration == NULL) \
			{\
				___debug_printtime(); \
				printf(" \tFATAL: "); \
			 	printf(fmt,##args); \
				fflush(stdout); \
			}\
			else if(_GLOBAL_bmd_configuration->logs_configuration.quiet_FATAL_enable != 0) \
			{\
				___debug_printtime(); \
				printf(" \tFATAL: "); \
			 	printf(fmt,##args); \
				fflush(stdout); \
			}

#define V_FATAL(fmt, args...) \
			if(_GLOBAL_bmd_configuration == NULL) \
			{\
				___debug_printtime(); \
				printf(" \tFATAL: "); \
			 	printf(fmt,##args); \
				fflush(stdout); \
			}\
			else if(_GLOBAL_bmd_configuration->logs_configuration.verbose_FATAL_enable != 0) \
			{\
				___debug_printtime(); \
				printf(" \tFATAL: "); \
			 	printf(fmt,##args); \
				fflush(stdout); \
			}


#define Q_DENY(fmt, args...) \
			if(_GLOBAL_bmd_configuration != NULL) \
			{ \
				if(_GLOBAL_bmd_configuration->logs_configuration.quiet_DENY_enable != 0) \
				{\
					___debug_printtime(); \
					printf(" \tDENIED: "); \
			 		printf(fmt,##args); \
					fflush(stdout); \
				}\
			}

#define V_DENY(fmt, args...) \
			if(_GLOBAL_bmd_configuration != NULL) \
			{ \
				if(_GLOBAL_bmd_configuration->logs_configuration.verbose_DENY_enable != 0) \
				{\
					___debug_printtime(); \
					printf(" \tDENIED: "); \
					printf(fmt,##args); \
					fflush(stdout); \
				}\
			}

#define Q_DONE(fmt, args...) \
			if(_GLOBAL_bmd_configuration != NULL) \
			{ \
				if(_GLOBAL_bmd_configuration->logs_configuration.quiet_DONE_enable != 0) \
				{\
					___debug_printtime(); \
					printf(" \tDONE: "); \
			 		printf(fmt,##args); \
					fflush(stdout); \
				}\
			}

#define V_DONE(fmt, args...) \
			if(_GLOBAL_bmd_configuration != NULL) \
			{ \
				if(_GLOBAL_bmd_configuration->logs_configuration.verbose_DONE_enable != 0) \
				{\
					___debug_printtime(); \
					printf(" \tDONE: "); \
					printf(fmt,##args); \
					fflush(stdout); \
				}\
			}


#define Q_TRY(fmt, args...) \
			if(_GLOBAL_bmd_configuration != NULL) \
			{ \
				if(_GLOBAL_bmd_configuration->logs_configuration.quiet_TRY_enable != 0) \
				{\
					___debug_printtime(); \
					printf(" \tATTEMPT: "); \
					printf(fmt,##args); \
					fflush(stdout); \
				}\
			}

#define V_TRY(fmt, args...) \
			if(_GLOBAL_bmd_configuration != NULL) \
			{ \
				if(_GLOBAL_bmd_configuration->logs_configuration.verbose_TRY_enable != 0) \
				{\
					___debug_printtime(); \
					printf(" \tATTEMPT: "); \
					printf(fmt,##args); \
					fflush(stdout); \
				}\
			}

#define Q_DEBUG(fmt, args...) \
			if(_GLOBAL_bmd_configuration != NULL) \
			{\
				if(_GLOBAL_bmd_configuration->logs_configuration.quiet_DEBUG_enable != 0) \
				{\
					___debug_printtime(); \
					printf(" \tDEBUG: "); \
					printf(fmt,##args); \
					fflush(stdout); \
				}\
			}

#define V_DEBUG(fmt, args...) \
			if(_GLOBAL_bmd_configuration != NULL) \
			{ \
				if(_GLOBAL_bmd_configuration->logs_configuration.verbose_DEBUG_enable != 0) \
				{\
					___debug_printtime(); \
					printf(" \tDEGUG: "); \
					printf(fmt,##args); \
					fflush(stdout); \
				}\
			}
#endif
#define CHECK_V_DONE ((_GLOBAL_bmd_configuration) && (_GLOBAL_bmd_configuration->logs_configuration.verbose_DONE_enable ))
#ifndef WIN32
/* ------------------------------------------------------------------------------------------------------------ */

/* MAKRA DOTYCZACE SERWERA LOGOW */
/* DATA : 04-03-2008 */
/* WINOWAJCA : TK */

/* ------------------------------------------------------------------------------------------------------------ */

#define LOGINIT() \
		if(_GLOBAL_bmd_configuration != NULL) \
		{ \
			if(_GLOBAL_bmd_configuration->logs_configuration.log_server_SEND_enable == 1) \
			{\
				err_t err_tRet = LogInit ( _GLOBAL_bmd_configuration->logs_configuration.log_server_conf, \
				                          &_GLOBAL_bmdlog_HANDLER ); \
					if( BMD_ERR( err_tRet ) ) \
						{\
							BMD_BTERR( err_tRet ); \
							BMD_FREEERR( err_tRet ); \
						}\
			}\
		}

#define LOGEND() \
		if(_GLOBAL_bmd_configuration != NULL) \
		{ \
			if(_GLOBAL_bmd_configuration->logs_configuration.log_server_SEND_enable == 1) \
			{\
				err_t err_tRet = LogEnd ( &_GLOBAL_bmdlog_HANDLER ); \
					if( BMD_ERR( err_tRet ) ) \
						{\
							BMD_BTERR( err_tRet ); \
							BMD_FREEERR( err_tRet ); \
						}\
			}\
		}

#define LOGCREATE( logOWNER ) \
		if(_GLOBAL_bmd_configuration != NULL) \
		{ \
			if(_GLOBAL_bmd_configuration->logs_configuration.log_server_SEND_enable == 1) \
			{\
				err_t err_tRet = LogCreate( _GLOBAL_bmdlog_HANDLER ); \
				if( BMD_ERR( err_tRet ) ) \
				{\
					BMD_BTERR( err_tRet ); \
					BMD_FREEERR( err_tRet ); \
				}\
				else \
				{\
					err_tRet = LogSetParam( log_owner, logOWNER, _GLOBAL_bmdlog_HANDLER ); \
					if( BMD_ERR( err_tRet ) ) \
					{\
						BMD_BTERR( err_tRet ); \
						BMD_FREEERR( err_tRet ); \
					}\
					else \
					{\
						err_tRet = LogSetParam( service, "BAE", _GLOBAL_bmdlog_HANDLER ); \
						if( BMD_ERR( err_tRet ) ) \
						{\
							BMD_BTERR( err_tRet ); \
							BMD_FREEERR( err_tRet ); \
						}\
					}\
				}\
			}\
		}

#define LOG_TYPE( logTYPE ) \
		if(_GLOBAL_bmd_configuration != NULL) \
		{ \
			if(_GLOBAL_bmd_configuration->logs_configuration.log_server_SEND_enable == 1) \
			{\
				 err_t err_tRet = LogSetParam( operation_type, logTYPE, _GLOBAL_bmdlog_HANDLER ); \
				 if( BMD_ERR( err_tRet ) ) \
					{\
						BMD_BTERR( err_tRet ); \
						BMD_FREEERR( err_tRet ); \
					}\
			}\
		}

#define LOG_STRING( logSTRING ) \
		if(_GLOBAL_bmd_configuration != NULL) \
		{ \
			if(_GLOBAL_bmd_configuration->logs_configuration.log_server_SEND_enable == 1) \
			{\
				 err_t err_tRet = LogSetParam( log_string, logSTRING, _GLOBAL_bmdlog_HANDLER ); \
				 if( BMD_ERR( err_tRet ) ) \
					{\
						BMD_BTERR( err_tRet ); \
						BMD_FREEERR( err_tRet ); \
					}\
			}\
		}

#define LOG_REASON( logREASON ) \
		if(_GLOBAL_bmd_configuration != NULL) \
		{ \
			if(_GLOBAL_bmd_configuration->logs_configuration.log_server_SEND_enable == 1) \
			{\
				 err_t err_tRet = LogSetParam( log_reason, logREASON, _GLOBAL_bmdlog_HANDLER ); \
				 if( BMD_ERR( err_tRet ) ) \
					{\
						BMD_BTERR( err_tRet ); \
						BMD_FREEERR( err_tRet ); \
					}\
			}\
		}

#define LOG_SOLUTION( logSOLUTION ) \
		if(_GLOBAL_bmd_configuration != NULL) \
		{ \
			if(_GLOBAL_bmd_configuration->logs_configuration.log_server_SEND_enable == 1) \
			{\
				 err_t err_tRet = LogSetParam( log_solution, logSOLUTION, _GLOBAL_bmdlog_HANDLER ); \
				 if( BMD_ERR( err_tRet ) ) \
					{\
						BMD_BTERR( err_tRet ); \
						BMD_FREEERR( err_tRet ); \
					}\
			}\
		}

#define LOG_TIME_END( logTIME_END ) \
		if(_GLOBAL_bmd_configuration != NULL) \
		{ \
			if(_GLOBAL_bmd_configuration->logs_configuration.log_server_SEND_enable == 1) \
			{\
				char* stringTmp = NULL; \
				logTIME_END[strlen(logTIME_END)-1] = '\0'; \
				stringTmp = logTIME_END; \
				asprintf(&logTIME_END, "%s %s", logTIME_END, tzname[1]); \
				err_t err_tRet = LogSetParam( date_time_end, logTIME_END, _GLOBAL_bmdlog_HANDLER ); \
				if( BMD_ERR( err_tRet ) ) \
					{\
						BMD_BTERR( err_tRet ); \
						BMD_FREEERR( err_tRet ); \
					}\
				free0(stringTmp); \
				free0(logTIME_END); \
			}\
		}

#define LOG_DOC_NAME( logDOC_NAME ) \
		if(_GLOBAL_bmd_configuration != NULL) \
		{ \
			if(_GLOBAL_bmd_configuration->logs_configuration.log_server_SEND_enable == 1) \
			{\
				 err_t err_tRet = LogSetParam( document_filename, logDOC_NAME, _GLOBAL_bmdlog_HANDLER ); \
				 if( BMD_ERR( err_tRet ) ) \
					{\
						BMD_BTERR( err_tRet ); \
						BMD_FREEERR( err_tRet ); \
					}\
			}\
		}

#define LOGCOMMIT() \
		if(_GLOBAL_bmd_configuration != NULL) \
		{ \
			if(_GLOBAL_bmd_configuration->logs_configuration.log_server_SEND_enable == 1) \
			{\
				err_t err_tRet = LogCommit( _GLOBAL_bmdlog_HANDLER ); \
					if( BMD_ERR( err_tRet ) ) \
						{\
							BMD_BTERR( err_tRet ); \
							BMD_FREEERR( err_tRet ); \
						}\
			}\
		}

#define LOG_LEVEL( eventCLASS, msgCLASS ) \
		if(_GLOBAL_bmd_configuration != NULL) \
		{ \
			if(_GLOBAL_bmd_configuration->logs_configuration.log_server_SEND_enable == 1) \
			{\
				char* stringLEVEL = NULL; \
				asprintf(&stringLEVEL, "%d:%d", eventCLASS, msgCLASS); \
				err_t err_tRet = LogSetParam( log_level, stringLEVEL, _GLOBAL_bmdlog_HANDLER ); \
				if( BMD_ERR( err_tRet ) ) \
					{\
						BMD_BTERR( err_tRet ); \
						BMD_FREEERR( err_tRet ); \
					}\
			}\
		}

/* ------------------------------------------------------------------------------------------------------------ */


#else

#define Q_FATAL(fmt, ...) {}
#define V_FATAL(fmt, ...) {}
#define Q_DONE(fmt, ...) {}
#define V_DONE(fmt, ...) {}
#define Q_TRY(fmt, ...) {}
#define V_TRY(fmt, ...) {}
#define Q_DENY(fmt, ...) {}
#define V_DENY(fmt, ...) {}
#define Q_DEBUG(fmt, ...) {}
#define V_DEBUG(fmt, ...) {}

#define LOGINIT(fmt, ...) {}
#define LOGEND(fmt, ...) {}
#define LOGCREATE(fmt, ...) {}
#define LOG_TYPE(fmt, ...) {}
#define LOG_STRING(fmt, ...) {}
#define LOG_REASON(fmt, ...) {}
#define LOG_SOLUTION(fmt, ...) {}
#define LOG_TIME_END(fmt, ...) {}
#define LOGCOMMIT() {}
#define LOG_LEVEL(fmt, ...) {}
#define LOG_DOC_NAME(fmt, ... ) {}
/*#define LOGCREATE(fmt, ...) {} */
/* #define LOGCREATE(fmt, ...) {} */
/* #define LOGCREATE(fmt, ...) {} */




#endif /* #ifndef WIN32 */



#endif /* ifndef _HAVE_BMD_SERVER_LOGS_H */
