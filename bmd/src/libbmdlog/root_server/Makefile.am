AM_LDFLAGS=-release 2.3 
bin_PROGRAMS=root_server
root_server_SOURCES=root_server.c rootserver_signal.c
root_server_LDADD = ../../libbmdnet/libbmdnet.la \
	../../libbmdasn1_common/libbmdasn1_common.la ../../libbmdasn1_core/libbmdasn1_core.la \
	../../libbmd/libbmd.la ../../libbmdconfig/libbmdconfig.la ../../libbmdutils/libbmdutils.la \
	../../libbmderr/libbmderr.la ../balance/libbmdbalance.la \
	../log_server_utils/libbmdlogserverutils.la \
	../../libbmddb/libbmddb.la ../../libbmdglobals/libbmdglobals.la
root_server_CPPFLAGS= -Werror -Wall
root_server_LDADD+=-ldl
