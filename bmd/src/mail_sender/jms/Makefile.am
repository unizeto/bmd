bin_PROGRAMS=awizo_jms

awizo_jms_SOURCES = awizo_jms.c \
		    awizo_signal.c \
		    queue.c \
		    cache.c \
		    configuration.c \
		    utils.c \
		    attachment.c \
		    shmem.c

awizo_jms_LDADD = ../../libbmdnet/libbmdnet.la \
		../../libbmdasn1_common/libbmdasn1_common.la \
		../../libbmdasn1_core/libbmdasn1_core.la \
		../../libbmd/libbmd.la \
		../../libbmdconfig/libbmdconfig.la \
		../../libbmdutils/libbmdutils.la \
		../../libbmderr/libbmderr.la \
		../../libbmdglobals/libbmdglobals.la \
		../../libbmdmime/libbmdmime.la \
		../../libbmdmail/libbmdmail.la \
		../../libbmd/libbmd.la \
		../../libbmdsql/libbmdsql.la

awizo_jms_CPPFLAGS=-I/opt/MessageQueue/mq/include
awizo_jms_LDFLAGS=-L/opt/MessageQueue/mq/lib -lmqcrt