AM_LDFLAGS=-release 2.3 
lib_LTLIBRARIES=libbmdlog.la
libbmdlog_la_SOURCES=libbmdlog.c
libbmdlog_la_LIBADD = ../libbmdnet/libbmdnet.la ../libbmdasn1_common/libbmdasn1_common.la \
	../libbmdasn1_core/libbmdasn1_core.la ../libbmdpki/libbmdpki.la \ 
	../libbmd/libbmd.la ../libbmdconfig/libbmdconfig.la \
	 ../libbmdutils/libbmdutils.la \
	../libbmddb/libbmddb.la ../libbmdglobals/libbmdglobals.la \
	 ./log_server_utils/libbmdlogserverutils.la

libbmdlog_la_CPPFLAGS= -Werror -Wall
#SUBDIRS = balance libbmdlognet log_server_utils tree_build bmdlogserver root_server . textbmdlog
SUBDIRS = balance log_server_utils tree_build bmdlogserver root_server . textbmdlog verify_logs soap_server
