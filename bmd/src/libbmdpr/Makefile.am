AM_LDFLAGS=-release 2.3 

SUBDIRS = bmd_resp bmd_datagram prlib-common
lib_LTLIBRARIES=libbmdpr.la
libbmdpr_la_SOURCES=libbmdpr.c
libbmdpr_la_LIBADD=	bmd_resp/libbmdpr_resp.la	\
				bmd_datagram/libbmdpr_datagram.la	\
				prlib-common/libbmdpr_common.la	\
				../libbmdutils/libbmdutils.la \
				../libbmdpki/libbmdpki.la
libbmdpr_la_CPPFLAGS = -Wall -Werror

