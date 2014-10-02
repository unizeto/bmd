#ifndef _BMD_KS_CONST_INCLUDED_
#define _BMD_KS_CONST_INCLUDED_

#ifdef WIN32
#include <windows.h>
#include <tchar.h>
#include <shlwapi.h>
#include <strsafe.h>
#define dlopen( _libraryname, _loadmode ) LoadLibraryA( _libraryname )
#define dlsym( _hmodule, _funsymname ) GetProcAddress( (HMODULE)_hmodule, _funsymname )
#define dlclose( _hmodule ) FreeLibrary( (HMODULE)_hmodule )
#endif
#include <pcre.h>
#include <stdio.h>
#include <stdarg.h>

#ifndef WIN32
	#ifndef SOCKET_DEFINED
		#define SOCKET_DEFINED
		typedef long SOCKET;
	#endif
	typedef void * HMODULE;
	typedef long DWORD;
	typedef long WORD;
	typedef long BOOL;
	typedef void * LPVOID;
	typedef void * HANDLE;
	#define vsprintf_s vsnprintf
	#define closesocket close
	#define WINAPI
	#include <wchar.h>
	#include <pthread.h>
	#include <string.h>
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <unistd.h>
	#include <fcntl.h>
	#include <sys/types.h>
        #include <sys/socket.h>
	#include <arpa/inet.h>
	#include <netinet/in.h> 
	#include <dlfcn.h>
#endif

#define SVCNAME                "bmd_keyservice" /* nazwa uslugi */
#define DISP_SVCNAME           "BMD - serwis kluczy" /* nazwa wyswietlana */

#define DEF_IP                 "0.0.0.0"        /* domyslny adres IP */
#define DEF_PORT               12000            /* domyslny port */

#define SVC_ERROR 1
#define SLOT_UNDEFINED  -1
#define SVC_MAX_CRYPTO_DATA_LENGTH 1024
#define SVC_CKA_ID_LENGTH          20
#define SVC_MAX_BUF_LEN            8192

#ifdef WIN32
#define SVC_LOG_OK			EVENTLOG_SUCCESS
#define SVC_LOG_INFO		EVENTLOG_INFORMATION_TYPE
#define SVC_LOG_WARNING		EVENTLOG_WARNING_TYPE
#define SVC_LOG_ERROR		EVENTLOG_ERROR_TYPE
#else
#define SVC_LOG_OK		0
#define SVC_LOG_INFO		1
#define SVC_LOG_WARNING		2
#define SVC_LOG_ERROR		3
#endif

#define SVC_LOG_LEVEL_NONE		0
#define SVC_LOG_LEVEL_NORMAL	1
#define SVC_LOG_LEVEL_DEBUG		2

#endif
