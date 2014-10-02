AM_LDFLAGS=-release 2.3 
lib_LTLIBRARIES=libbmdlogserverutils.la
libbmdlogserverutils_la_SOURCES=log_server_utils.c
libbmdlogserverutils_la_LIBADD = ../../libbmdnet/libbmdnet.la \
	../../libbmdasn1_common/libbmdasn1_common.la ../../libbmdasn1_core/libbmdasn1_core.la \
	../../libbmd/libbmd.la ../../libbmdconfig/libbmdconfig.la ../../libbmdutils/libbmdutils.la \
	../../libbmderr/libbmderr.la ../balance/libbmdbalance.la
libbmdlogserverutils_la_CPPFLAGS= -Werror -Wall
