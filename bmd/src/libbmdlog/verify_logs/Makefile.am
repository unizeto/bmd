AM_LDFLAGS=-release 2.3 
lib_LTLIBRARIES=libbmdverifylogs.la
libbmdverifylogs_la_SOURCES=verify_logs_func.c
libbmdverifylogs_la_LIBADD = ../../libbmderr/libbmderr.la ../../libbmddb/libbmddb.la ../../libbmdsql/libbmdsql.la \
../../libbmd/libbmd.la ../../libbmdconfig/libbmdconfig.la \
			    ../balance/libbmdbalance.la	../log_server_utils/libbmdlogserverutils.la ../../libbmdglobals/libbmdglobals.la
libbmdverifylogs_la_CPPFLAGS= -Werror -Wall

bin_PROGRAMS=verify_logs audit_tree

verify_logs_SOURCES=verify_logs_main.c
verify_logs_LDADD=  ../../libbmderr/libbmderr.la ../../libbmddb/libbmddb.la ../../libbmdsql/libbmdsql.la \
		    ../../libbmd/libbmd.la ../../libbmdconfig/libbmdconfig.la libbmdverifylogs.la
verify_logs_CPPFLAGS= -Werror -Wall

audit_tree_SOURCES=audit_tree_main.c
audit_tree_LDADD= ../../libbmderr/libbmderr.la ../../libbmddb/libbmddb.la ../../libbmdsql/libbmdsql.la \
		    ../../libbmd/libbmd.la ../../libbmdconfig/libbmdconfig.la libbmdverifylogs.la
audit_tree_CPPFLAGS= -Werror -Wall
