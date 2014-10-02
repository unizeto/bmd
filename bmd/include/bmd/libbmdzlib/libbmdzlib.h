#ifndef _LIBBMDZLIB_INCLUDED_
#define _LIBBMDZLIB_INCLUDED_

#include <bmd/libbmdzlib/zip.h>
#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdglobals/libbmdglobals.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "zlib.h"
#include <fcntl.h>
#include <time.h>


#define SET_BINARY_MODE(file) setmode(fileno(file), O_BINARY)
#define CHUNK 16384
#define WRITEBUFFERSIZE 8192

/*
#include <bmd/bmd_server/bmd_server.h>
#include <dirent.h>
#include <bmd/libbmderr/libbmderr.h>
*/



/************************************************************/
/*	Kompresowanie pliku zrodlowego do pliku docelowego	*/
/************************************************************/
long zipCompressFile( char **in, long in_count, char *out);
long zipPackFile( char **in, long in_count, char *out);
long filetime( char *filename, tm_zip *tmzip, uLong *dt);


#endif
