AM_LDFLAGS=-release 2.3 

#SUBDIRS=. tests

lib_LTLIBRARIES=libbmdnet.la

AM_CPPFLAGS= -Wall -DNO_DEBUG_UNIX_


libbmdnet_la_SOURCES=libbmdnet.c libbmdnet_dllmain.c libbmdnet_init.c libbmdnet_create.c libbmdnet_connect.c libbmdnet_send.c libbmdnet_recv.c libbmdnet_server.c
libbmdnet_la_LIBADD=../libbmdglobals/libbmdglobals.la

# testlibbmdnet_SOURCES= libbmdnet.a ../libbmdthreads/libbmdthreads.a testlibbmdnet.c 

