SUBDIRS = asn1clib prlib
noinst_LTLIBRARIES = libbmdpr_resp.la
libbmdpr_resp_la_SOURCES = bmd_resp.c
libbmdpr_resp_la_LIBADD = asn1clib/libbmdpr_resp_asn1clib.la prlib/libbmdpr_resp_prlib.la
libbmdpr_resp_la_CPPFLAGS=	-I../../../include/bmd/libbmdpr/bmd_resp/asn1clib	\
				-I../../../include/bmd/libbmdpr/bmd_datagram/asn1clib -Wall -Werror
