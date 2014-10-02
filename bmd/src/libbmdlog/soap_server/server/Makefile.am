bin_PROGRAMS		=	logsoap_server

logsoap_server_SOURCES 	= 	logsoap_server.c \
				logsoap_server_internal_send.c \
				logsoap_server_internal_verification.c \
				logServer.c \
				logC.c

logsoap_server_LDADD	=	../../../libbmdconfig/libbmdconfig.la \
				../../../libbmdglobals/libbmdglobals.la \
				../../../libbmderr/libbmderr.la \
				../../verify_logs/libbmdverifylogs.la \
				../../libbmdlog.la \
				stdsoap2.o

stdsoap2.o: stdsoap2.c
	gcc -c -DWITH_NONAMESPACES stdsoap2.c

logsoap_server_CFLAGS = -Wall
logsoap_server_CPPFLAGS = -Wall
