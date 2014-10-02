AM_LDFLAGS=-release 2.3 

#AM_CFLAGS=${CPP_PEDANTIC} -pedantic -ansi
AM_CFLAGS=-Wall
lib_LTLIBRARIES = libbmdbase64.la 
INCLUDES = ${INCDIR} 

libbmdbase64_la_SOURCES = cencode.c cdecode.c libbmdbase64.c
libbmdbase64_la_LIBADD = ../libbmdglobals/libbmdglobals.la
libbmdbase64_la_CPPFLAGS = -Wall -Werror

