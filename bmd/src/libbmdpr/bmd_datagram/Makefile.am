SUBDIRS = asn1clib
noinst_LTLIBRARIES = libbmdpr_datagram.la
libbmdpr_datagram_la_SOURCES = bmd_datagram.c bmd_datagram_set.c bmd_datagram_internal.c bmd_datagram_utils.c
libbmdpr_datagram_la_SOURCES += bmd_datagram_set_internal.c bmd_datagram_set_utils.c
libbmdpr_datagram_la_LIBADD = asn1clib/libbmdpr_datagram_asn1clib.la
libbmdpr_datagram_la_CPPFLAGS =     -I../../../include/bmd/libbmdpr/bmd_datagram/asn1clib   \
                                    -I../../../include/bmd/libbmdpr/bmd_resp/asn1clib -Wall -Werror
