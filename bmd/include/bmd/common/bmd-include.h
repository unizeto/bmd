#ifndef _BMD_INCLUDE_INCLUDED_
#define _BMD_INCLUDE_INCLUDED_

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<ctype.h>
#include<time.h>
#ifndef WIN32
#include<sys/time.h>
#else	/* ifndef WIN32 */
#include <pcre.h>
#include<io.h>
#endif	/* ifndef WIN32 */

#include<errno.h>

#include<curl/curl.h>
#include<curl/easy.h>
#include<curl/types.h>

#ifdef _SunOS_
#include<stdarg.h>
#endif

#ifdef HAVE_STDARG_H
	#include <stdarg.h>
#endif

#ifndef _WIN32
#include<unistd.h>
#include<dlfcn.h>
#include<sys/uio.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<strings.h>
#include<sys/time.h>
#include <stdint.h>
#endif

#endif	/*
 */
