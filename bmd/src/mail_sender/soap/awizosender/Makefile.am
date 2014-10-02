bin_PROGRAMS = awizosoap_sender

awizosoap_sender_SOURCES  =     ../tools/queue.c ../tools/cache.c \
				../tools/utils.c \
				../tools/configuration.c \
				awizosoap_server.c \
				../tools/awizo_signal.c \
				../tools/attachment.c \
				mail_sender_internal_send.c \
				mail_sender_clear_cache.c \
				mail_sender_internal_send_setmsgqueue.c \
				mail_sender_clear_mailqueue.c \
				mail_sender_get_mail_count_from_queue.c \
				mail_sender_change_config.c \
				mail_sender_stopsending.c \
				mail_sender_send_sms.c \
				mail_sender_get_senders.c \
				mail_sender_insert_template.c \
				mail_sender_download_template.c \
				mail_sender_insert_attachment.c \
				mail_sender_delete_template.c \
				mail_sender_change_template_info.c \
				mail_sender_delete_attachment.c \
				mail_sender_download_attachment.c \
				mail_sender_change_attachment_info.c \
				mail_sender_insert_sender.c \
				mail_sender_delete_sender.c \
				mail_sender_get_sender_info.c \
				mail_sender_change_sender_info.c \
				mail_sender_addAdviceDispatchSetting.c \
				mail_sender_changeAdviceDispatchSetting.c \
				mail_sender_removeAdviceDispatchSetting.c \
				../tools/shmem.c \
				../tools/soapServer.c \
				../tools/soapC.c \
				../tools/stdsoap2.c

awizosoap_sender_LDADD    =     ../../../libbmdconfig/libbmdconfig.la \
				../../../libbmdglobals/libbmdglobals.la \
				../../../libbmderr/libbmderr.la \
				../../../libbmdcurl/libbmdcurl.la \
				../../../libbmdlog/libbmdlog.la \
				../../../bmd_ks/libbks.la \
				../../../libbmdmime/libbmdmime.la \
				../../../libbmdmail/libbmdmail.la \
				../../../libbmd/libbmd.la \
				../../../libbmddb/libbmddb.la \
				../../../libbmdsql/libbmdsql.la \
			../../../libbmdasn1_common/libbmdasn1_common.la \
				../../../libbmdutils/libbmdutils.la
AM_LDFLAGS = -lrt
awizosoap_sender_CFLAGS = -I../tools/
