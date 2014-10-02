AM_LDFLAGS=-release 2.3 
lib_LTLIBRARIES=libbmdsql.la

libbmdsql_la_SOURCES= \
			common.c \
			delete.c \
			data_dict.c \
			get.c \
			insert.c \
			select_main.c select_usr.c select_sys.c select_add.c select_utils.c select_unreceived.c \
			update.c \
			history.c \
			raport.c \
			cases.c	\
			prf_timestamp.c \
			prf_signature.c \
			prf_certificate.c \
			prf_crl.c \
			prf_common.c \
			user.c\
			role.c\
			group.c\
			class.c\
			timestamp.c\
			security.c\
			link.c\
			version.c\
			action.c


libbmdsql_la_LIBADD=${BMDDB_LDADD} ../libbmdpr/libbmdpr.la \
			../libbmdts/libbmdts.la	\
			../libbmdcrl/libbmdcrl.la \
			../libbmddb/libbmddb.la \
			../libbmdglobals/libbmdglobals.la \
			../libbmdxades/libbmdxades.la \
			../libbmdutils/libbmdutils.la

libbmdsql_la_CFLAGS = -Wall -Werror

libbmdsql_la_CPPFLAGS=	-I../../include/bmd/libbmdasn1_core	\
			-I../../include/bmd/libbmdasn1_common 	\
			-I../../include/bmd/libbmdpr/bmd_resp/asn1clib	\
			-I../../include/bmd/libbmdpr/bmd_datagram/asn1clib \
			-I /usr/include/xmlsec1/
