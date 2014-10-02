bin_PROGRAMS = logsoap_test_send
logsoap_test_send_SOURCES = logsoap_test_send.c ../server/logClient.c ../server/logC.c CommonTest.c
logsoap_test_send_LDADD =  ../../../libbmdconfig/libbmdconfig.la stdsoap2.o
logsoap_test_send_CFLAGS = -Wall -I../server/

stdsoap2.o : ../server/stdsoap2.c ../server/stdsoap2.h
	cc -c -DWITH_NONAMESPACES ../server/stdsoap2.c