AM_LDFLAGS=-release 2.3 

lib_LTLIBRARIES = libbmdpki.la 
libbmdpki_la_SOURCES = libbmdpki.c sym_api.c sym_internal.c sig_api.c cms_api.c hash_api.c cms_internal.c bmdpki_sym.c
libbmdpki_la_SOURCES += ts_api.c ts_internal.c ctx_api.c ctx_internal.c sig_internal.c sig_params.c cert_api.c
libbmdpki_la_SOURCES += cert_path_api.c
libbmdpki_la_LIBADD = ../libbmdpkcs12/libbmdpkcs12.la
libbmdpki_la_LIBADD += ../libbmderr/libbmderr.la ../bmd_ks/libbks.la \
			../libbmdbase64/libbmdbase64.la \
	 	../libbmdX509/libbmdX509.la ../libbmdcms/libbmdcms.la \
		../libbmdpkcs11/libbmdpkcs11.la
libbmdpki_la_CPPFLAGS = -Wall -Werror
INCLUDES = ${INCDIR} 

#noinst_PROGRAMS= testTST #pkitest
#testTST_SOURCES= testTST.c
#testTST_LDADD= ../libbmdpki/libbmdpki.la ../libbmdts/libbmdts.la ../libbmd/libbmd.la ../libbmdpkcs11/libbmdpkcs11.la ../libbmdcrl/libbmdcrl.la
#pkitest_SOURCES= pkitest.c
#pkitest_LDADD= ../libbmdpki/libbmdpki.la ../libbmdts/libbmdts.la ../libbmd/libbmd.la ../libbmdpkcs11/libbmdpkcs11.la ../libbmdcrl/libbmdcrl.la

