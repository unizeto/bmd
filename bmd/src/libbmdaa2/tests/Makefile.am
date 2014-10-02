noinst_PROGRAMS = gen_config gen_ac gen_name read_ac

all_CPPFLAGS =	-ggdb3 -O0
all_LDADD = 		../../libbmdasn1_core/libbmdasn1_core.la \
			../../libbmdasn1_common/libbmdasn1_common.la	\
			../../libbmdconfig/libbmdconfig.la      \
			../../libbmdutils/libbmdutils.la \
			../../libbmdX509/libbmdX509.la \
			../../libbmdcms/libbmdcms.la \
			../../libbmddb/libbmddb.la \
			../libbmdaa2.la ../../libbmdpki/libbmdpki.la ../../libbmdpkcs11/libbmdpkcs11.la \
			../../libbmdts/libbmdts.la ../../libbmdcrl/libbmdcrl.la


#			../configuration/libbmdaa2configuration.la     

gen_ac_CPPFLAGS = $(all_CPPFLAGS)
gen_ac_LDADD =	$(all_LDADD)
gen_name_CPPFLAGS = $(all_CPPFLAGS)
gen_name_LDADD = $(all_LDADD)
gen_config_CPPFLAGS = $(all_CPPFLAGS)
gen_config_LDADD = $(all_LDADD)
read_ac_CPPFLAGS = $(all_CPPFLAGS)
read_ac_LDADD = $(all_LDADD)


#../configuration/libbmdaa_unix_configuration.la	\
#			../db/libbmdaa_unix_db.la	\
#			../wrappers/libbmdaa_unix_wrappers.la	\
#			../../libbmderr/libbmderr.la	\
#			../../libbmdconfig/libbmdconfig.la	\
#			../../libbmdutils/libbmdutils.la	\
#			../../libbmdasn1_core/libbmdasn1_core.la \
#			../../libbmdasn1_common/libbmdasn1_common.la
#			

