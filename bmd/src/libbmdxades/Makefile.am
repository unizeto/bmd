AM_LDFLAGS=-release 2.3 
# bin_PROGRAMS=testxades1
#testxades_LDADD=./libbmdxades.la

#testxades1_LDADD=./libbmdxades.la

lib_LTLIBRARIES=libbmdxades.la
libbmdxades_la_SOURCES=xades.c validate.c xmlio_curl.c
libbmdxades_la_LIBADD= \
	../libbmderr/libbmderr.la ../libbmdts/libbmdts.la ${XMLSEC_LIBS} ../libbmdxml/libbmdxml.la -lxmlsec1-openssl -lxmlsec1
libbmdxades_la_CPPFLAGS= -Wall -Werror -DXMLSEC_CRYPTO_OPENSSL
AM_CPPFLAGS=${CPP_PEDANTIC} ${XMLSEC_CFLAGS}
AM_CFLAGS=${XMLSEC_CFLAGS}
