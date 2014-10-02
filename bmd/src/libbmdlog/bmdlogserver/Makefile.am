AM_LDFLAGS=-release 2.3 
bin_PROGRAMS=bmdlogserver
bmdlogserver_SOURCES=bmdlogserver.c bmdlogserver_signal.c
bmdlogserver_LDADD = ../../libbmdnet/libbmdnet.la \
	../../libbmdasn1_common/libbmdasn1_common.la ../../libbmdasn1_core/libbmdasn1_core.la \
	../../libbmd/libbmd.la ../../libbmdconfig/libbmdconfig.la ../../libbmdutils/libbmdutils.la \
	../../libbmderr/libbmderr.la ../log_server_utils/libbmdlogserverutils.la ../tree_build/libbmdtreebuild.la \
	../../libbmddb/libbmddb.la ../../libbmdglobals/libbmdglobals.la
	
bmdlogserver_la_CPPFLAGS= -Werror -Wall
bmdlogserver_LDADD+=-ldl
