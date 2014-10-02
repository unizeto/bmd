noinst_PROGRAMS = cert_stats

cert_stats_CPPFLAGS=	-I../../../include/bmd/libbmdasn1_core		\
			-I../../../include/bmd/libbmdasn1_common	\
			-ggdb3 -O0
			
cert_stats_LDADD =../libbmdX509.la \
		../../libbmdcms/libbmdcms.la \
		../../libbmdasn1_core/libbmdasn1_core.la \
		../../libbmdasn1_common/libbmdasn1_common.la \
		../../libbmdutils/libbmdutils.la	\
		../../libbmdpki/libbmdpki.la	\
		../../libbmdcrl/libbmdcrl.la	\
		../../libbmdpkcs11/libbmdpkcs11.la	\
		../../libbmdts/libbmdts.la

