AM_LDFLAGS=-release 2.3 
bin_PROGRAMS=jmslogfetcher

jmslogfetcher_SOURCES=jmslogfetcher.c
jmslogfetcher_LDADD = ../../libbmdnet/libbmdnet.la ../../libbmdasn1_common/libbmdasn1_common.la \
	../../libbmdasn1_core/libbmdasn1_core.la ../../libbmd/libbmd.la ../../libbmdconfig/libbmdconfig.la \
	../../libbmdutils/libbmdutils.la ../../libbmderr/libbmderr.la ../../libbmdglobals/libbmdglobals.la ../libbmdlog.la
jmslogfetcher_CPPFLAGS=-I /opt/SunMessageQueue/mq/include
jmslogfetcher_LDFLAGS=-L/opt/SunMessageQueue/mq/lib -lmqcrt
