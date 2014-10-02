SUBDIRS = insert select delete get update history proofs 
bin_PROGRAMS = test_set_get_with_proofs test_set_select test_set_update


test_set_update_CPPFLAGS=	
test_set_update_LDADD=	../libbmdsql.la			\
				../../libbmddb/libbmddb.la	\
				../../libbmdts/libbmdts.la	\
				../../libbmdcms/libbmdcms.la	\
				../../libbmdX509/libbmdX509.la	\
				../../libbmdasn1_common/libbmdasn1_common.la	\
				../../libbmdasn1_core/libbmdasn1_core.la	\
				../../libbmdutils/libbmdutils.la	\
				../../libbmdpkcs11/libbmdpkcs11.la
				
test_set_update_SOURCES=../../bmd_server/main/js.c test_set_update.c

