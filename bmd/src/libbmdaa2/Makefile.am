AM_LDFLAGS=-release 2.3 


SUBDIRS = db configuration wrappers ac_server ac_client pki
lib_LTLIBRARIES = libbmdaa2.la
libbmdaa2_la_LIBADD = wrappers/libbmdaa2_wrappers.la	\
			db/libbmdaa2_db.la	\
			configuration/libbmdaa2_configuration.la	\
			ac_server/libbmdaa2_ac_server.la	\
			ac_client/libbmdaa2_ac_client.la	\
			pki/libbmdaa2_pki.la
	
libbmdaa2_la_CPPFLAGS=-I../../include/bmd -Werror -Wall

