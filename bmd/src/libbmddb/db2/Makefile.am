AM_LDFLAGS=-release 2.3 
lib_LTLIBRARIES = libbmddb_db2.la 

libbmddb_db2_la_SOURCES = db2_libbmddb.c db_db2_blob.c db_db2_connect.c db_db2_internal.c
libbmddb_db2_la_SOURCES += db_db2_result.c db_db2_sql.c db_db2_transaction.c db_db2_utils.c
libbmddb_db2_la_LIBADD = ${DB2_BMDDB_LIBS}
INCLUDES = ${INCDIR} ${DB2_CFLAGS}
libbmddb_db2_la_CPPFLAGS = -Werror -Wall

AM_CFLAGS = ${DB2_INCLUDE}
