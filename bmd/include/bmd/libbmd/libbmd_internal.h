#ifndef _LIBBMD_INTERNAL_INCLUDED_
#define _LIBBMD_INTERNAL_INCLUDED_

#include <bmd/libbmd/libbmd.h>
#include <bmd/libbmdnet/libbmdnet.h>

#if defined(__cplusplus)
extern "C" {
#endif
/* funkcje nieeksportowalne */
long get_filename(char *input,char **output);
long add_unvisible_kontrolki(bmd_info_t *bmd_info,bmdDatagram_t **datagram);
long strip_metadata_in_datagram(bmdDatagram_t **datagram);
long check_metadata_in_datagram(bmd_info_t *bmd_info,bmdDatagram_t *datagram);
long get_owner_from_cert(GenBuf_t *cert,GenBuf_t **owner);

long __bmd_prepare_request(bmd_info_t *bmd_info,bmdDatagram_t *request_datagram,bmd_crypt_ctx_t **lob_crypt_ctx);
long __bmd_prepare_request_buffer(	bmdDatagramSet_t *request_datagram_set,
					bmd_info_t *bmd_info,
					long twf_start_index,
					long twf_transmission_count,

					GenBuf_t ***output,
					long *twf_no_of_buffers,

					GenBuf_t *tmp_ki_mod_n_client,
					long twf_free_datagrams_flag);

long __bmd_request_send(	bmdnet_handler_ptr *session_handle,
				GenBuf_t **request_to_send, long twf_no_of_buffers,
				bmd_info_t *bmd_info, long twf_connect);
long __bmd_response_recv(bmdnet_handler_ptr *session_handle,bmd_info_t *bmd_info,GenBuf_t **received_data);
long __bmd_decode_response_buffer(	GenBuf_t **input,
					bmd_info_t *bmd_info,
					GenBuf_t *tmp_ki_mod_n_client,
					bmdDatagramSet_t **response_datagram_set,
					GenBuf_t **new_ki_mod_n,
					long *twf_bmdDatagramsTransmissionCount,
					long *twf_bmdDatagramsAllCount,
					long *twf_memory_guard,
					long deserialisation_max_memory);

long __bmd_prepare_response(bmd_info_t *bmd_info,
							bmdDatagramSet_t **response_datagram_set,
							lob_request_info_t *lob_info,
							bmdnet_handler_ptr session_handle);

long __bmd_prepare_response_to_buf(	bmd_info_t *bmd_info,
					bmdDatagramSet_t **response_datagram_set,
					lob_request_info_t *lob_info,
					bmdnet_handler_ptr session_handle,
					GenBuf_t **lobBuffer,
					long maxLobSize);


long __bmd_prepare_response_in_parts(	bmd_info_t *bmd_info,
							bmdDatagramSet_t **response_datagram_set,
							lob_request_info_t *lob_info,
							bmdnet_handler_ptr session_handle,
							long lob_session_hash,
							long twf_timeout);

long __bmd_recv_lob(bmdnet_handler_ptr session_handle,lob_request_info_t *lob_info,bmdDatagram_t *dtg,bmd_crypt_ctx_t *dec_ctx);
long __bmd_verify_lob_input_data(bmdDatagramSet_t *request_datagram_set,lob_request_info_t *info);

/*to jest w libbmd_send_lob.c */
long __bmd_send_lobs_cancel(lob_request_info_t *info,bmdnet_handler_ptr session_handle);

bmdnet_handler_ptr __bmd_create_handler(bmd_info_t *bmd_info);
#if defined(__cplusplus)
}
#endif
#endif
