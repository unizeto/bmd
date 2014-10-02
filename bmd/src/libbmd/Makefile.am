AM_LDFLAGS=-release 2.3 

lib_LTLIBRARIES = libbmd.la 
libbmd_la_SOURCES = libbmd.c libbmd_genbuf.c libbmd_login.c libbmd_main.c libbmd_utils.c libbmd_datagram.c libbmd_internal.c
libbmd_la_SOURCES += libbmd_bmdinfo.c libbmd_send.c libbmd_userform.c libbmd_send_internal.c libbmd_send_lob.c
libbmd_la_SOURCES += libbmd_lob_request.c libbmd_bmdattr.c libbmd_datagramset.c
libbmd_la_LIBADD = ../libbmdpr/libbmdpr.la \
	../libbmdutils/libbmdutils.la \
	../libbmdasn1_core/libbmdasn1_core.la \
	../libbmdasn1_common/libbmdasn1_common.la \
	../libbmdnet/libbmdnet.la \
	../libbmdts/libbmdts.la ../bmd_ks/libbks.la \
	../libbmdbase64/libbmdbase64.la \
	../libbmdpkcs11/libbmdpkcs11.la \
	../libbmdpki/libbmdpki.la ../libbmdbase64/libbmdbase64.la \
	../libbmdpkcs12/libbmdpkcs12.la ../libbmdlob/libbmdlob.la \
	../libbmdglobals/libbmdglobals.la \
	../libbmdprotocol/libbmdprotocol.la \
	../libbmdlob/libbmdlob.la \
	../libbmdxpath/libbmdxpath.la
libbmd_la_CPPFLAGS = -Wall -Werror
INCLUDES = ${INCDIR} 

