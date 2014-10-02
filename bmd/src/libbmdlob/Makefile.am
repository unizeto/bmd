AM_LDFLAGS=-release 2.3 

lib_LTLIBRARIES = libbmdlob.la
libbmdlob_la_SOURCES=libbmdlob.c lob_utils.c lob_send.c lob_recv.c lob_create.c
libbmdlob_la_LIBADD = \
	../libbmdasn1_core/libbmdasn1_core.la \
	../libbmdnet/libbmdnet.la \
	../libbmderr/libbmderr.la \
	../libbmdpr/libbmdpr.la \
	../libbmdglobals/libbmdglobals.la
INCLUDES = ${INCDIR} 
libbmdlob_la_CPPFLAGS = -I../../include/bmd/libbmdpr/bmd_resp/asn1clib	\
			-I../../include/bmd/libbmdpr/bmd_datagram/asn1clib -Wall -Werror


