AM_LDFLAGS=-release 2.3 


lib_LTLIBRARIES=libbmddvcs.la

libbmddvcs_la_SOURCES=dvcs.c dvcs_api.c dvcs_externalsig.c

libbmddvcs_la_LIBADD=../libbmdasn1_core/libbmdasn1_core.la ../libbmdasn1_common/libbmdasn1_common.la ../libbmdcms/libbmdcms.la ../libbmdpki/libbmdpki.la ../libbmdutils/libbmdutils.la ../libbmdts/libbmdts.la
libbmddvcs_la_LIBADD += ../libbmderr/libbmderr.la  ../libbmdglobals/libbmdglobals.la
libbmddvcs_la_LDFLAGS= -Wl,--no-undefined

libbmddvcs_la_CPPFLAGS= -Wall -Werror

AM_CPPFLAGS=${CPP_PEDANTIC}
#AM_LDFLAGS=-luniDVCS -L/opt/proCertumAPI/lib 

