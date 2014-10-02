AM_LDFLAGS=-release 2.3 

lib_LTLIBRARIES = libbmdprotocol.la
libbmdprotocol_la_SOURCES=pr_internal.c pr_login_req.c pr_login_resp.c pr_session.c pr_internal_get.c pr_internal_set.c pr_internal_del.c

libbmdprotocol_la_LIBADD=../libbmdpr/libbmdpr.la ../libbmdconfig/libbmdconfig.la ../libbmdglobals/libbmdglobals.la
libbmdprotocol_la_CPPFLAGS=	-I../../include/bmd/libbmdpr/bmd_resp/asn1clib	\
				-I../../include/bmd/libbmdpr/bmd_datagram/asn1clib -Werror -Wall



