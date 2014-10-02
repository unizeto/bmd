AM_LDFLAGS=-release 2.3 

lib_LTLIBRARIES=libbmdconfig.la

libbmdconfig_la_SOURCES=bmdconfig.c dict.c do_config.c hash_pcre.c lista.c mem_list.c str.c parse_mem.c for_db.c update_file.c
libbmdconfig_la_LIBADD=../libbmderr/libbmderr.la -lpcre
AM_CFLAGS=-Wall -ansi -Werror


#noinst_PROGRAMS=test_config test_confdb conf2db db2conf

#test_config_SOURCES=test_config.c
#test_config_LDADD=./libbmdconfig.la

#test_confdb_SOURCES=test_confdb.c
#test_confdb_LDADD=./libbmdconfig.la

#conf2db_SOURCES=conf2db.c
#conf2db_LDADD=./libbmdconfig.la

#db2conf_SOURCES=db2conf.c
#db2conf_LDADD=./libbmdconfig.la

