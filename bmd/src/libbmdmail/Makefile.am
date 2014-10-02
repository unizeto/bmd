AM_LDFLAGS=-release 2.3 

lib_LTLIBRARIES = libbmdmail.la 
libbmdmail_la_SOURCES = libbmdmail.c mail_send.c mail_utils.c bmdmail.c
libbmdmail_la_LIBADD = \
	../libbmdutils/libbmdutils.la \
	../libbmdglobals/libbmdglobals.la
libbmdmail_la_CPPFLAGS = -Wall -Werror
INCLUDES = ${INCDIR} 

