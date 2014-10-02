bin_PROGRAMS=rencryption

rencryption_SOURCES = rencryption.c
rencryption_LDADD=	
				../../libbmdutils/libbmdutils.la \
				../../libbmdpr/libbmdpr.la \
				../../libbmdaa2/libbmdaa2.la \
				../../libbmdasn1_common/libbmdasn1_common.la \
				../../libbmdasn1_core/libbmdasn1_core.la \
				../../libbmdbase64/libbmdbase64.la \
				../../libbmdpki/libbmdpki.la \
				../../libbmdpkcs12/libbmdpkcs12.la \
				../../libbmdnet/libbmdnet.la \
				../../libbmdprotocol/libbmdprotocol.la \
				../../libbmddb/libbmddb.la \
				../../libbmdts/libbmdts.la \
				../../libbmdcms/libbmdcms.la \
				../../libbmdX509/libbmdX509.la \
				../../libbmdsql/libbmdsql.la \
				../../libbmd/libbmd.la \
				../../libbmdconfig/libbmdconfig.la \
				../../libbmdpkcs11/libbmdpkcs11.la \
				../../libbmdmail/libbmdmail.la \
				../../libbmdmime/libbmdmime.la \
				../../libbmdlog/libbmdlog.la \
				../../libbmdglobals/libbmdglobals.la

rencryption_LDADD+=${BMDDB_LDADD} -ldl


rencryption_CPPFLAGS=  	-I../../../../include/bmd/libbmdasn1_core     \
                	-I../../../../include/bmd/libbmdasn1_common   \
                	-I../../../../include/bmd/libbmdpr/bmd_datagram/asn1clib      \
                	-I../../../../include/bmd/libbmdpr/bmd_resp/asn1clib -Wall -Werror
