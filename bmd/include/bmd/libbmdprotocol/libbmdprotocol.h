#ifndef _LIBBMDPROTOCOL_INCLUDED_
#define _LIBBMDPROTOCOL_INCLUDED_
#include <bmd/common/bmd-sds.h>
#include <bmd/libbmdnet/libbmdnet.h>

#ifdef WIN32
	#ifdef LIBBMDPROTOCOL_EXPORTS
		#define LIBBMDPROTOCOL_SCOPE __declspec(dllexport)
	#else
		#ifdef _USRDLL
			#define LIBBMDPROTOCOL_SCOPE __declspec(dllimport)
		#else
			#define LIBBMDPROTOCOL_SCOPE
		#endif
	#endif
#else
	#define LIBBMDPROTOCOL_SCOPE
#endif // ifdef WIN32

#if defined(__cplusplus)
extern "C" {
#endif
	/* pr_internal.c */
    	LIBBMDPROTOCOL_SCOPE long set_prefix_from_int(uint32_t value, long twf_size, GenBuf_t ** output);
    	LIBBMDPROTOCOL_SCOPE long get_prefix_from_gb(GenBuf_t * input,uint32_t * id);
    	LIBBMDPROTOCOL_SCOPE long get_asn1_data_block_size(char *buffer,long length);
    	LIBBMDPROTOCOL_SCOPE long obsolote_asn1_get_from_socket(bmdnet_handler_ptr handler,char **buf);
    	LIBBMDPROTOCOL_SCOPE long strip_prefix(GenBuf_t ** input_buf,uint32_t * id,GenBuf_t ** stripped_buf);

	/* pr_internal_del.c */
	LIBBMDPROTOCOL_SCOPE long bmd2_datagram_del_symkey(bmdDatagram_t *dtg);

	/* pr_internal_get.c */
	LIBBMDPROTOCOL_SCOPE long bmd2_datagram_get_k(bmdDatagram_t *dtg,GenBuf_t **k);
	LIBBMDPROTOCOL_SCOPE long bmd2_datagram_get_i(bmdDatagram_t *dtg,GenBuf_t **i);
	LIBBMDPROTOCOL_SCOPE long bmd2_datagram_get_n(bmdDatagram_t *dtg,GenBuf_t **n);
	LIBBMDPROTOCOL_SCOPE long bmd2_datagram_get_ki_mod_n(bmdDatagram_t *dtg,GenBuf_t **ki_mod_n);
	LIBBMDPROTOCOL_SCOPE long bmd2_datagram_get_form(bmdDatagram_t *dtg,GenBuf_t **form);
	LIBBMDPROTOCOL_SCOPE long bmd2_datagram_get_oids(bmdDatagram_t *dtg,GenBuf_t **oids);
	LIBBMDPROTOCOL_SCOPE long bmd2_datagram_get_symkey(bmdDatagram_t *dtg,GenBuf_t **sym_key);
	LIBBMDPROTOCOL_SCOPE long bmd2_datagram_get_symkey_hash(bmdDatagram_t *dtg,GenBuf_t **sym_key_hash);
	LIBBMDPROTOCOL_SCOPE long bmd2_datagram_get_actions( bmdDatagram_t *dtg, long **actions, long *no_of_actions);
	LIBBMDPROTOCOL_SCOPE long bmd2_datagram_get_server_label(	bmdDatagram_t *dtg, char **server_label);

	LIBBMDPROTOCOL_SCOPE long bmd2_datagram_get_role_id_list(bmdDatagram_t *dtg,GenBufList_t **roleIdList);
	LIBBMDPROTOCOL_SCOPE long bmd2_datagram_get_role_name_list(bmdDatagram_t *dtg,GenBufList_t **roleNameList);
	LIBBMDPROTOCOL_SCOPE long bmd2_datagram_get_default_role_index(bmdDatagram_t *dtg, GenBufList_t *roleIdList, long *index);
	LIBBMDPROTOCOL_SCOPE long bmd2_datagram_get_current_role_index(bmdDatagram_t *dtg, GenBufList_t *roleIdList, long *index);
	LIBBMDPROTOCOL_SCOPE long bmd2_datagram_get_group_id_list(bmdDatagram_t *dtg, GenBufList_t **groupIdList);
	LIBBMDPROTOCOL_SCOPE long bmd2_datagram_get_group_name_list(bmdDatagram_t *dtg, GenBufList_t **groupNameList);
	LIBBMDPROTOCOL_SCOPE long bmd2_datagram_get_class_id_list(bmdDatagram_t *dtg, GenBufList_t **classIdList);
	LIBBMDPROTOCOL_SCOPE long bmd2_datagram_get_class_name_list(bmdDatagram_t *dtg, GenBufList_t **classNameList);
	LIBBMDPROTOCOL_SCOPE long bmd2_datagram_get_default_group_index(bmdDatagram_t *dtg, GenBufList_t *groupIdList, long *index);
	LIBBMDPROTOCOL_SCOPE long bmd2_datagram_get_current_group_index(bmdDatagram_t *dtg, GenBufList_t *groupIdList, long *index);
	LIBBMDPROTOCOL_SCOPE long bmd2_datagram_get_current_class_index(bmdDatagram_t *dtg, GenBufList_t *classIdList, long *index);

	/* pr_internal_set.c */
	LIBBMDPROTOCOL_SCOPE long bmd2_datagram_set_k(GenBuf_t *k,bmdDatagram_t **dtg);
	LIBBMDPROTOCOL_SCOPE long bmd2_datagram_set_i(GenBuf_t *i,bmdDatagram_t **dtg);
	LIBBMDPROTOCOL_SCOPE long bmd2_datagram_set_n(GenBuf_t *n,bmdDatagram_t **dtg);
	LIBBMDPROTOCOL_SCOPE long bmd2_datagram_set_ki_mod_n(GenBuf_t *ki_mod_n,bmdDatagram_t **dtg);
	LIBBMDPROTOCOL_SCOPE long bmd2_datagram_set_form(GenBuf_t *form,bmdDatagram_t **dtg);
	LIBBMDPROTOCOL_SCOPE long bmd2_datagram_set_oids(GenBuf_t *oids,bmdDatagram_t **dtg);
	LIBBMDPROTOCOL_SCOPE long bmd2_datagram_set_symkey(GenBuf_t *sym_key,bmdDatagram_t **dtg);
	LIBBMDPROTOCOL_SCOPE long bmd2_datagram_set_symkey_hash(GenBuf_t *sym_key_hash,bmdDatagram_t **dtg);
	LIBBMDPROTOCOL_SCOPE long bmd2_datagram_set_actions(long *actions,long no_of_actions,bmdDatagram_t **dtg);
	LIBBMDPROTOCOL_SCOPE long bmd2_datagram_set_current_role_and_group(char *roleName, char *groupId, bmdDatagram_t **dtg);
	LIBBMDPROTOCOL_SCOPE long bmd2_datagram_set_server_label(	char *server_label, bmdDatagram_t **dtg);
	LIBBMDPROTOCOL_SCOPE long bmd2_datagram_get_cert_login_as(bmdDatagram_t *dtg,GenBuf_t **cert_login_as);

	LIBBMDPROTOCOL_SCOPE long bmd2_datagram_set_role_list(GenBufList_t *role_list,bmdDatagram_t **dtg);
	LIBBMDPROTOCOL_SCOPE long bmd2_datagram_set_group_list(GenBufList_t *group_list,bmdDatagram_t **dtg);
	LIBBMDPROTOCOL_SCOPE long bmd2_datagram_set_class_list(	GenBuf_t *userSerial, GenBuf_t *userCert_dn, bmdDatagram_t **dtg);
	LIBBMDPROTOCOL_SCOPE long bmd2_datagram_set_security_list(GenBufList_t *security_list,bmdDatagram_t **dtg);

	LIBBMDPROTOCOL_SCOPE long bmd2_datagram_update_symkey(GenBuf_t *sym_key,bmdDatagram_t **dtg);

	LIBBMDPROTOCOL_SCOPE long bmd2_datagram_add_metadata_char(const char *oid,char *value,bmdDatagram_t **datagram);

	/* pr_login_req.c */
	LIBBMDPROTOCOL_SCOPE long bmd2_create_login_request(bmd_crypt_ctx_t *sig_ctx,bmd_crypt_ctx_t *enc_ctx,GenBuf_t *cert_login_as,
                                                           GenBuf_t **k,GenBuf_t **request);
	LIBBMDPROTOCOL_SCOPE long bmd2_decode_login_request(	GenBuf_t **der_input,
								long twf_offset,
								bmd_crypt_ctx_t *dec_ctx,
								GenBuf_t **k,
								GenBuf_t **sig_cert,
								GenBuf_t **cert_login_as,
								long twf_der_input_free);

	/* pr_login_resp.c */
	LIBBMDPROTOCOL_SCOPE long bmd2_create_login_response(	bmd_crypt_ctx_t *sig_ctx,
								bmd_crypt_ctx_t *enc_ctx,
								GenBuf_t *k,
								GenBuf_t *i,
								GenBuf_t *n,
								GenBuf_t *der_form,
								GenBuf_t *bmdKeyIV,
								uint32_t request_id,
								GenBufList_t *role_list,
								GenBufList_t *group_list,
								GenBufList_t *security_list,
								GenBuf_t *user_chosen_role,
								GenBuf_t *user_chosen_group,
								GenBuf_t *serial,
								GenBuf_t *cert_dn,
								GenBuf_t *hcsb,
								long *actions,
								long no_of_actions,
								char *server_label,
								GenBuf_t **output);

	LIBBMDPROTOCOL_SCOPE	long bmd2_decode_login_response(	GenBuf_t **net_input,
									bmd_crypt_ctx_t *dec_ctx,
									GenBuf_t *client_bmd_cert,
									GenBuf_t **bmd_cert,
									GenBuf_t *k0,
									GenBuf_t **k,
									GenBuf_t **n,
									GenBuf_t **i,
									GenBuf_t **der_form,
									bmd_crypt_ctx_t **sym_ctx,
									uint32_t *request_id,
									GenBufList_t **roleIdList,
									GenBufList_t **roleNameList,
									long *defaultRoleIndex,
									GenBufList_t **groupIdList,
									GenBufList_t **groupNameList,
									long *defaultGroupIndex,
									GenBufList_t **classIdList,
									GenBufList_t **classNameList,
									long *defaultClassIndex,
									GenBuf_t **hcsb,
									long **actions,
									long *no_of_actions,
									char **server_label,
									long *servver);		// wyciagamy protocol version - potrzebne do OnlineUpdate


	/* wersja druga funkcji do requestow sesyjnych */
	LIBBMDPROTOCOL_SCOPE long bmd2_create_session_request(/*GenBuf_t **der_dtg_input,*/
									GenBuf_t **twf_input_buffers,
									long twf_no_of_buffers,

									bmd_crypt_ctx_t *sym_ctx,
									uint32_t request_id,
									GenBuf_t ***twf_output_buffers,
									long *twf_no_of_output_buffers);

	LIBBMDPROTOCOL_SCOPE long bmd2_decode_session_request(	GenBuf_t **input,
								GenBuf_t **cert_login_as,
								long twf_offset,
								bmd_crypt_ctx_t *dec_ctx,
								GenBuf_t *cache_ki_mod_n,
								GenBuf_t *i,
								GenBuf_t *n,
								GenBuf_t **new_ki_mod_n,
								bmdDatagramSet_t **dtg,
								long *twf_bmdDatagramsTransmissionCount,
								long *twf_bmdDatagramsAllCount,
								long twf_input_free,
								long *twf_memory_guard,
								long deserialisation_max_memory);


	long obsolote___bmd_wrap_with_octet_string(GenBuf_t **input, GenBuf_t *twf_prefix, GenBuf_t **output);
	long obsolote___bmd_unwrap_from_octet_string(GenBuf_t *input,GenBuf_t **output);
	long bmd_get_from_socket(bmdnet_handler_ptr handler, GenBuf_t **output);

#if defined(__cplusplus)
}
#endif
#endif
