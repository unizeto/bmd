AM_LDFLAGS=-release 2.3 
lib_LTLIBRARIES = libbmddb_postgres.la 

libbmddb_postgres_la_SOURCES = pq_libbmddb.c db_pq_blob.c db_pq_connect.c db_pq_internal.c
libbmddb_postgres_la_SOURCES += db_pq_result.c db_pq_sql.c db_pq_transaction.c db_pq_utils.c
libbmddb_postgres_la_LIBADD = ${POSTGRES_BMDDB_LIBS}
INCLUDES = ${INCDIR} ${PGSQL_CFLAGS}
libbmddb_postgres_la_CPPFLAGS = -Werror -Wall

