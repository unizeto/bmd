bin_PROGRAMS = awizosoap_broker

awizosoap_broker_SOURCES  =     ../tools/queue.c \
				../tools/utils.c \
				../tools/configuration.c \
				../tools/attachment.c \
				../tools/shmem.c \
				../tools/cache.c \
				../tools/awizo_signal.c \
				awizo_soap_broker.c \
				mail_sender_clear_cache.c \
				mail_sender_internal_send.c \
				mail_sender_internal_send_setmsgqueue.c \
				mail_sender_clear_mailqueue.c \
				mail_sender_get_mail_count_from_queue.c \
				mail_sender_change_config.c \
				mail_sender_stopsending.c \
				mail_sender_send_sms.c \
				mail_sender_get_senders.c \
				mail_sender_insert_template.c \
				mail_sender_download_template.c \
				mail_sender_change_template_info.c \
				mail_sender_delete_template.c \
				mail_sender_insert_attachment.c \
				mail_sender_delete_attachment.c \
				mail_sender_download_attachment.c \
				mail_sender_change_attachment_info.c \
				mail_sender_insert_sender.c \
				mail_sender_delete_sender.c \
				mail_sender_get_sender_info.c \
				mail_sender_addAdviceDispatchSetting.c \
				mail_sender_changeAdviceDispatchSetting.c \
				mail_sender_removeAdviceDispatchSetting.c \
				../tools/soapServer.c \
				../tools/soapC.c \
				../tools/soapClient.c \
				../tools/stdsoap2.c \
				mail_sender_change_sender_info.c


awizosoap_broker_LDADD    =     ../../../libbmdconfig/libbmdconfig.la \
				../../../libbmdglobals/libbmdglobals.la \
				../../../libbmdmime/libbmdmime.la \
				../../../libbmdmail/libbmdmail.la \
				../../../libbmd/libbmd.la \
				../../../libbmderr/libbmderr.la \
				../../../libbmddb/libbmddb.la \
				../../../libbmdsql/libbmdsql.la \
				../../../libbmdasn1_common/libbmdasn1_common.la \
				../../../libbmdutils/libbmdutils.la \
				../../../libbmdcurl/libbmdcurl.la

AM_LDFLAGS = -lrt
awizosoap_broker_CFLAGS = -I../tools/
