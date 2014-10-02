AM_LDFLAGS=-release 2.3 
lib_LTLIBRARIES=libbmdcurl.la

libbmdcurl_la_SOURCES=curl.c
			
libbmdcurl_la_LIBADD=${BMDDB_LDADD} ../libbmdpr/libbmdpr.la \
			../libbmdts/libbmdts.la	\
			../libbmdcrl/libbmdcrl.la \
			../libbmddb/libbmddb.la \
			../libbmdglobals/libbmdglobals.la
			
libbmdcurl_la_CPPFLAGS=	-I../../include/bmd/libbmdasn1_core	\
			-I../../include/bmd/libbmdasn1_common 	\
			-I../../include/bmd/libbmdpr/bmd_resp/asn1clib	\
			-I../../include/bmd/libbmdpr/bmd_datagram/asn1clib -Werror -Wall


