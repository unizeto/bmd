AM_LDFLAGS=-release 2.3 

lib_LTLIBRARIES = libbmdmime.la 
libbmdmime_la_SOURCES = libbmdmime.c p7m_mime.c p7m_mime_utils.c  p7s.c p7s_internal.c attachment_utils.c
libbmdmime_la_LIBADD = \
	../libbmdutils/libbmdutils.la
libbmdmime_la_CPPFLAGS = -Wall -Werror
INCLUDES = ${INCDIR} 

#noinst_PROGRAMS= test7s
#test7s_SOURCES= test7s.c
#test7s_LDADD= ./libbmdmime.la ../libbmdpki/libbmdpki.la ../libbmdasn1_common/libbmdasn1_common.la ../libbmdasn1_core/libbmdasn1_core.la ../libbmdcms/libbmdcms.la ../libbmdts/libbmdts.la ../libbmdcrl/libbmdcrl.la ../libbmd/libbmd.la ../libbmdmail/libbmdmail.la

