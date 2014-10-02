bin_PROGRAMS		=	bmdsoap_server

bmdsoap_server_SOURCES 	= 	bmdsoap_server.c \
					bmdsoap_server_get_utils.c \
					bmdsoap_server_insert_utils.c \
					bmdsoap_server_utils.c \
					bmdsoap_server_internal.c \
					bmdsoap_server_internal_get.c \
					bmdsoap_server_internal_insert.c \
					bmdsoap_server_internal_search.c \
					bmdsoap_server_internal_update.c \
					bmdsoap_server_internal_directory.c \
					bmdsoap_server_internal_user.c \
					bmdsoap_server_internal_role.c \
					bmdsoap_server_internal_group.c \
					bmdsoap_server_internal_class.c \
					bmdsoap_server_internal_security.c \
					bmdsoap_server_internal_link.c \
					bmdsoap_server_internal_timestamp.c\
					bmdsoap_server_internal_advice.c\
					bmdsoap_server_internal_action.c\
					gsoap_compiled/bmd230Server.c \
					gsoap_compiled/bmd230C.c \
					gsoap_src/wsseapi.c \
					gsoap_src/smdevp.c \
					gsoap_src/threads.c \
					gsoap_src/dom.c \
					bmd_wsse.c

bmdsoap_server_LDADD	=	../../libbmd/libbmd.la \
					../../libbmdconfig/libbmdconfig.la \
					../../libbmdasn1_common/libbmdasn1_common.la \
					../../libbmdsql/libbmdsql.la \
					../../libbmdzlib/libbmdzlib.la \
					../../libbmdxslt/libbmdxslt.la \
					../../libbmddb/libbmddb.la \
					../../libbmdutils/libbmdutils.la \
					../../libbmdarchpack/libbmdarchpack.la \
					../../libbmdglobals/libbmdglobals.la \
					stdsoap2.o

stdsoap2.o: gsoap_src/stdsoap2.c gsoap_src/stdsoap2.h
	cc -c -DWITH_NONAMESPACES gsoap_src/stdsoap2.c -DWITH_OPENSSL -DWITH_DOM -lcrypt -lssl

bmdsoap_server_CFLAGS = -DWITH_OPENSSL -DWITH_DOM -lcrypt -lssl -Wall  -Igsoap_src/ -Igsoap_compiled/
