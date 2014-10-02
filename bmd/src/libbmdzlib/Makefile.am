AM_LDFLAGS=-release 2.3 
lib_LTLIBRARIES=libbmdzlib.la

libbmdzlib_la_SOURCES	=	zlib_utils.c \
					compress.c \
					zip.c \
					ioapi.c \
					../libbmderr/libbmderr.c
libbmdzlib_la_LIBADD	=	../libbmdglobals/libbmdglobals.la
libbmdzlib_la_CPPFLAGS	=	-Werror -Wall
