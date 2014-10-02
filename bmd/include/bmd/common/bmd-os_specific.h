#ifndef _BMD_OS_SPECIFIC_INCLUDED_
#define _BMD_OS_SPECIFIC_INCLUDED_

/** @bug : u_int32_t czy moze byc unsigned int */
#ifdef WIN32
	#define dlopen( _libraryname, _loadmode ) LoadLibraryW( _libraryname )
	#define dlsym( _hmodule, _funsymname ) GetProcAddress( (HMODULE)_hmodule, _funsymname )
	#define dlclose( _hmodule ) FreeLibrary( (HMODULE)_hmodule )
	#define gmtime_r( _clock, _result ) \
					( *(_result) = *gmtime( (_clock) ), \
					(_result) )
	typedef enum { NO, YES } boolean_t;
	typedef __int32 u_int32_t;
	typedef __int64 u_int64_t;
	#define mode_t int
	#define O_SYNC 0
	#define STRDUPL _strdup
	#define ITOA _itoa
	#define RAND(x){rand_s(&x);}
	#define GETPID _getpid()
	#define STRCMPI _stricmp
	#define STRCASECMP _stricmp
	#define WIN_ETC_PATH "C:/WINDOWS/system32/drivers%s"			// (ROL) sciezka do folderu /etc
	#define WINDOWIZE_CONF_PATHS( x) windowize_conf_paths( x);
#else
	#if defined(__linux__) || defined(__FreeBSD__) || defined(__bsdi__) || defined(__OpenBSD__) || defined(__NetBSD__)
		#ifndef _GNU_SOURCE
			#define _GNU_SOURCE
		#endif
	#endif
	#define __int64 u_int64_t
	#define __int16 u_int16_t
	#define __int32 u_int32_t
	#define STRDUPL strdup
	#define ITOA itoa
	#define RAND(x){x = rand();}
	#define GETPID getpid()
	#define STRCMPI StrCmpI
	#define STRCASECMP strcasecmp
	#define min(a,b)            (((a) < (b)) ? (a) : (b))
	#define WIN_ETC_PATH "%s"										// (ROL) sciezka do folderu /etc nie zostaje modyfikowana pod linuxem
	#define WINDOWIZE_CONF_PATHS( x)	{}
	#define __cdecl
	#define O_BINARY 0x00
	#define _O_BINARY 0
	#ifndef _XOPEN_SOURCE
		#define _XOPEN_SOURCE
	#endif
	#ifndef _SunOS_			/* na razie wiem ze pod Solarisem typ boolean jest ustawiony, ale nie ma
						       identyfikatorow NO/YES a tu dla wygody musza byc zadeklarowane					 */
		#ifndef _HAVE_BOOLEAN_T
			#ifdef SWIG
				typedef enum boolean { BMDNO, BMDYES } boolean_t;
			#else
				typedef enum boolean { NO, YES } boolean_t;
			#endif
			#ifndef NO
				#define NO 0
			#endif
			#ifndef YES
				#define YES 1
			#endif
		#endif
	#else
		#define NO 0
		#define YES 1
	#endif
#endif

#endif /* _BMD_OS_SPECIFIC_INCLUDED_ */

