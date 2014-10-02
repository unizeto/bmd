AM_LDFLAGS=-release 2.3 
lib_LTLIBRARIES = libbmdutils.la
libbmdutils_la_SOURCES=libbmdutils.c asn1_utils.c mem_utils.c \
	kontrolki_utils.c aa_conf_utils.c \
	os_specific.c \
	datagram2XML.c \
	UnicodeSupport.c \
	bmd_ipc.c

libbmdutils_la_LIBADD = \
	../libbmdasn1_core/libbmdasn1_core.la \
	../libbmdnet/libbmdnet.la \
	../libbmderr/libbmderr.la ../libbmdxpath/libbmdxpath.la
INCLUDES = ${INCDIR} 

libbmdutils_la_CPPFLAGS = -Werror -Wall
