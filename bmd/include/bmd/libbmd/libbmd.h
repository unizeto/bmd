#ifndef _LIBBMD_API_DEFINED_
#define _LIBBMD_API_DEFINED_

#include <wchar.h>
#include <sys/types.h>
#include <bmd/common/bmd-errors.h>
#include <bmd/common/bmd-sds.h>
#include <bmd/common/bmd-crypto_ds.h>

#ifdef WIN32
	#ifdef LIBBMD_EXPORTS
		#define LIBBMD_SCOPE __declspec(dllexport)
	#else
		#define LIBBMD_SCOPE __declspec(dllimport)
	#endif
#else
	#define LIBBMD_SCOPE
#endif

#if defined(__cplusplus)
extern "C" {
#endif

/* Typy datagramow - czyli typy zadan do BMD */
#define BMD_DATAGRAM_TYPE_GET_FORM			1
#define BMD_DATAGRAM_TYPE_INSERT			2
#define BMD_DATAGRAM_TYPE_INSERT_LO			3
#define BMD_DATAGRAM_TYPE_GET				4
#define BMD_DATAGRAM_TYPE_GET_LO			5
#define BMD_DATAGRAM_TYPE_GET_PKI			6
#define BMD_DATAGRAM_TYPE_GET_PKI_LO			7
#define BMD_DATAGRAM_TYPE_DELETE			8
#define BMD_DATAGRAM_TYPE_PERSISTENT			9
#define BMD_DATAGRAM_GET_CGI				10
#define BMD_DATAGRAM_GET_CGI_PKI			11
#define BMD_DATAGRAM_GET_CGI_LO				12
#define BMD_DATAGRAM_GET_CGI_PKI_LO			13
#define BMD_DATAGRAM_GET_METADATA			14
#define BMD_DATAGRAM_GET_METADATA_WITH_TIME		15
#define BMD_DATAGRAM_GET_ONLY_FILE			16
#define BMD_DATAGRAM_TYPE_NOOP				17
#define BMD_DATAGRAM_TYPE_SEND_AVIZO			18
#define BMD_DATAGRAM_TYPE_VALIDATE_CERTIFICATE		19
#define BMD_DATAGRAM_TYPE_VALIDATE_SIGNATURE		20
#define BMD_DATAGRAM_TYPE_UPDATE			21
#define BMD_DATAGRAM_TYPE_CLOSE_SESSION			22
#define BMD_DATAGRAM_TYPE_LOGIN_AS_OTHER		23
#define BMD_DATAGRAM_TYPE_LOGIN_FOREVER			24
#define BMD_DATAGRAM_TYPE_CERTIFY_DVCS			25
#define BMD_DATAGRAM_TYPE_CERTIFY_TIMESTAMP		26
#define BMD_DATAGRAM_TYPE_CERTIFY_DOCUMENT		27
#define BMD_DATAGRAM_TYPE_VERIFY_CRYPTO_SIGNATURE	28
#define BMD_DATAGRAM_TYPE_END_TRANSACTION		29
#define BMD_DATAGRAM_TYPE_COUNT_DOCS			30
#define BMD_DATAGRAM_GET_METADATA_ADD			31
#define BMD_DATAGRAM_DIR_CREATE				32
#define BMD_DATAGRAM_DIR_LIST				33
#define BMD_DATAGRAM_DIR_LIST_WITH_OR			34
#define BMD_DATAGRAM_DIR_DELETE				35
#define BMD_DATAGRAM_DIR_GET_CONTENT			36
#define BMD_DATAGRAM_DIR_GET_DETAIL			37
#define BMD_DATAGRAM_TYPE_SEARCH			38
#define BMD_DATAGRAM_TYPE_REGISTER_USER			39
#define BMD_DATAGRAM_TYPE_UPDATE_USER			40
#define BMD_DATAGRAM_TYPE_GET_USER_LIST			41
#define BMD_DATAGRAM_TYPE_DISABLE_USER			42
#define BMD_DATAGRAM_TYPE_DELETE_USER			43
#define BMD_DATAGRAM_TYPE_REGISTER_ROLE			44
#define BMD_DATAGRAM_TYPE_UPDATE_ROLE			45
#define BMD_DATAGRAM_TYPE_GET_ROLE_LIST			46
#define BMD_DATAGRAM_TYPE_DELETE_ROLE			47
#define BMD_DATAGRAM_TYPE_CHOOSE_ROLE			48
#define BMD_DATAGRAM_LINK_CREATE			49
#define BMD_DATAGRAM_ADD_SIGNATURE			50
#define BMD_DATAGRAM_TYPE_REGISTER_GROUP		51
#define BMD_DATAGRAM_TYPE_UPDATE_GROUP			52
#define BMD_DATAGRAM_TYPE_GET_GROUP_LIST		53
#define BMD_DATAGRAM_TYPE_DELETE_GROUP			54
#define BMD_DATAGRAM_TYPE_REGISTER_SECURITY		55
#define BMD_DATAGRAM_TYPE_UPDATE_SECURITY		56
#define BMD_DATAGRAM_TYPE_GET_SECURITY_LIST		57
#define BMD_DATAGRAM_TYPE_DELETE_SECURITY		58
#define BMD_DATAGRAM_TYPE_REGISTER_CLASS		59
#define BMD_DATAGRAM_TYPE_UPDATE_CLASS			60
#define BMD_DATAGRAM_TYPE_GET_CLASS_LIST		61
#define BMD_DATAGRAM_TYPE_DELETE_CLASS			62
#define BMD_DATAGRAM_TYPE_CONSERVATE_TIMESTAMPS		63
#define BMD_DATAGRAM_TYPE_UPDATE_METADATA_DEFAULT_VALUE	64
#define BMD_DATAGRAM_TYPE_SEARCH_UNRECEIVED		65
#define BMD_DATAGRAM_TYPE_GET_ACCESS_OBJECTS		66
#define BMD_DATAGRAM_TYPE_ADD_CERTIFICATE_TO_IDENTITY	67
#define BMD_DATAGRAM_TYPE_DISABLE_IDENTITY		68
#define BMD_DATAGRAM_TYPE_CREATE_CASCADE_LINKS		69
#define BMD_DATAGRAM_TYPE_UPDATE_FILE_VERSION_LO	70
#define BMD_DATAGRAM_TYPE_DELETE_CASCADE_LINKS		71
#define BMD_DATAGRAM_TYPE_UPDATE_SYS_METADATA		72
#define BMD_DATAGRAM_TYPE_GET_ARCHIVE_FILE_VERSION_LO	73
#define BMD_DATAGRAM_TYPE_GET_VERSIONS_HISTORY		74
#define BMD_DATAGRAM_TYPE_DELETE_REPLACED_LOBS		75
#define BMD_DATAGRAM_TYPE_GET_ACTIONS_HISTORY		76
#define BMD_DATAGRAM_TYPE_SEND_ADVICE_WITH_PARAMETERS	77



/* AKCJE W KLIENCIE - w tym miejscu ponieważ tak jak typy datagramów umieszczane są w tabeli roles_and_actions */
#define BMD_CLIENT_ACTION_ENABLED						1000
#define BMD_CLIENT_ACTION_LIMIT_MDIC_TO_USER			1010					/*Max Doc Insert Count*/
#define BMD_CLIENT_ACTION_DOWNLOAD_ZIP					1011
#define BMD_CLIENT_ACTION_INSERT_DOCS_OVER_LIMIT		1012
#define BMD_CLIENT_ACTION_WARN_DOCS_OVER_LIMIT			1013

#define BMD_CLIENT_ACTION_PRINT_VIEW					1020
#define BMD_CLIENT_ACTION_PRINT_ALL						1021
#define BMD_CLIENT_ACTION_MTD_UPDATE_ALL				1022
#define BMD_CLIENT_ACTION_GET_MTD_CSV					1023
#define BMD_CLIENT_ACTION_MANUAL_INPUT_MTD				1024
#define BMD_CLIENT_ACTION_EDIT_SIGN_PRESENCE			1025
#define BMD_CLIENT_ACTION_SEND_WITH_SIGNATURE			1026
#define BMD_CLIENT_ACTION_APP_ONLINE_UPDATE				1027
#define BMD_CLIENT_ACTION_HIDE_CLEARANCES_IN_SC			1028
#define BMD_CLIENT_ACTION_SEARCH_IN_USER_DOC_CHKD		1029
#define BMD_CLIENT_ACTION_EDIT_SEARCH_IN_USER_DOC		1030
#define BMD_CLIENT_ACTION_AUTO_COPY_DEF_CONFS			1031		/* nie uzywane (potrzebne jeszcze przed zalogowaniem - musi zostac w licencji */
#define BMD_CLIENT_ACTION_ROLE_CHANGE_ALLOWED			1032
#define BMD_CLIENT_ACTION_ROLE_CHANGE_FAST				1033
#define BMD_CLIENT_ACTION_ALLOW_CMD						1034
#define BMD_CLIENT_ACTION_MASS_MTD_UPDATE_BUTTON		1035
#define BMD_CLIENT_ACTION_ENABLE_FILE_INS_VALIDATION	1036
#define BMD_CLIENT_ACTION_CASE_SENS_CHKD				1037
#define BMD_CLIENT_ACTION_EDIT_CASE_SENS				1038
#define BMD_CLIENT_ACTION_EXACT_MATCH_CHKD				1039
#define BMD_CLIENT_ACTION_EDIT_EXACT_MATCH				1040
#define BMD_CLIENT_ACTION_SEARCH_PANEL_VISIBLE			1041
#define BMD_CLIENT_ACTION_SEND_PANEL_VISIBLE			1042
#define BMD_CLIENT_ACTION_GET_ALL_FILES					1043
#define BMD_CLIENT_ACTION_GET_ADD_PDF					1044
#define BMD_CLIENT_ACTION_GET_ADD_XML					1045
#define BMD_CLIENT_ACTION_GET_MTD_XML					1046
#define BMD_CLIENT_ACTION_GET_FILE						1047
#define BMD_CLIENT_ACTION_VIEW_FILE						1048
#define BMD_CLIENT_ACTION_ALLOW_LOGIN_FROM_PFX			1049
#define BMD_CLIENT_ACTION_CONF_SEARCH					1050
#define BMD_CLIENT_ACTION_CONF_MTD						1051
#define BMD_CLIENT_ACTION_CONF_SEND_GET					1052
#define BMD_CLIENT_ACTION_CONF_SIGN						1053
#define BMD_CLIENT_ACTION_CONF_PRINT					1054
#define BMD_CLIENT_ACTION_CONF_MAIN						1055
#define BMD_CLIENT_ACTION_CONF_APP_ONLINE_UPDATE		1056
#define BMD_CLIENT_ACTION_CONF_SERVER_ADDRESS			1057
#define BMD_CLIENT_ACTION_CONF_PROXY_SETTINGS			1058
#define BMD_CLIENT_ACTION_CONF_CONN_CHECK				1059
#define BMD_CLIENT_ACTION_CONF_LOGIN_SOURCE				1060
#define BMD_CLIENT_ACTION_CONF_GUI_PLUGIN				1061
#define BMD_CLIENT_ACTION_CONF_ZIP_CONTENT_ENABLED		1062
#define BMD_CLIENT_ACTION_CONF_ZIP_CONTENT_VISIBLE		1063
#define BMD_CLIENT_ACTION_CONF_ZIP_DWN_OPT_ENABLED		1064
#define BMD_CLIENT_ACTION_CONF_ZIP_DWN_OPT_VISIBLE		1065
#define BMD_CLIENT_ACTION_CONF_ZIP_DWN_PAR_ENABLED		1066
#define BMD_CLIENT_ACTION_CONF_ZIP_DWN_PAR_VISIBLE		1067
#define BMD_CLIENT_ACTION_CLEAR_ACT_TB_AFTER_LOGOUT		1068
#define BMD_CLIENT_ACTION_CONF_MAIN_ALLOW_AFTER_LOGOUT	1069
#define BMD_CLIENT_ACTION_CONF_SEND_GET_FOLDERS_TYPES	1070
#define BMD_CLIENT_ACTION_ADD_SIGNATURE					1071
#define BMD_CLIENT_ACTION_ADD_FILES_EDIT_PANEL_VISIBLE	1072
#define BMD_CLIENT_ACTION_COUNT_DOCS_ON_SEARCH			1073
#define BMD_CLIENT_ACTION_SEND_ADVICE_TO_ALL_FILES		1074
#define BMD_CLIENT_ACTION_DELETE_ALL_FILES				1075
#define BMD_CLIENT_ACTION_CREATE_LINK					1076
#define BMD_CLIENT_ACTION_ADD_SIGN_OWNER_NAME			1077
#define BMD_CLIENT_ACTION_SIGN_ALL_FILES				1078



#define BMD_DATAGRAM_TYPE_GET_FORM_RESPONSE				BMD_DATAGRAM_TYPE_GET_FORM+100
#define BMD_DATAGRAM_TYPE_INSERT_RESPONSE				BMD_DATAGRAM_TYPE_INSERT+100
#define BMD_DATAGRAM_TYPE_INSERT_LO_RESPONSE				BMD_DATAGRAM_TYPE_INSERT_LO+100
#define BMD_DATAGRAM_TYPE_GET_RESPONSE					BMD_DATAGRAM_TYPE_GET+100
#define BMD_DATAGRAM_TYPE_GET_LO_RESPONSE				BMD_DATAGRAM_TYPE_GET_LO+100
#define BMD_DATAGRAM_TYPE_GET_PKI_RESPONSE				BMD_DATAGRAM_TYPE_GET_PKI+100
#define BMD_DATAGRAM_TYPE_GET_PKI_LO_RESPONSE				BMD_DATAGRAM_TYPE_GET_PKI_LO+100
#define BMD_DATAGRAM_TYPE_DELETE_RESPONSE				BMD_DATAGRAM_TYPE_DELETE+100
#define BMD_DATAGRAM_TYPE_PERSISTENT_RESPONSE				BMD_DATAGRAM_TYPE_PERSISTENT+100
#define BMD_DATAGRAM_GET_CGI_RESPONSE					BMD_DATAGRAM_GET_CGI+100
#define BMD_DATAGRAM_GET_CGI_PKI_RESPONSE				BMD_DATAGRAM_GET_CGI_PKI+100
#define BMD_DATAGRAM_GET_CGI_LO_RESPONSE				BMD_DATAGRAM_GET_CGI_LO+100
#define BMD_DATAGRAM_GET_CGI_PKI_LO_RESPONSE				BMD_DATAGRAM_GET_CGI_PKI_LO+100
#define BMD_DATAGRAM_GET_METADATA_RESPONSE				BMD_DATAGRAM_GET_METADATA+100
#define BMD_DATAGRAM_GET_METADATA_WITH_TIME_RESPONSE			BMD_DATAGRAM_GET_METADATA_WITH_TIME+100
#define BMD_DATAGRAM_GET_ONLY_FILE_RESPONSE				BMD_DATAGRAM_GET_ONLY_FILE+100
#define BMD_DATAGRAM_TYPE_NOOP_RESPONSE                 		BMD_DATAGRAM_TYPE_NOOP+100
#define BMD_DATAGRAM_TYPE_SEND_AVIZO_RESPONSE            		BMD_DATAGRAM_TYPE_SEND_AVIZO+100
#define BMD_DATAGRAM_TYPE_VALIDATE_CERTIFICATE_RESPONSE  		BMD_DATAGRAM_TYPE_VALIDATE_CERTIFICATE+100
#define BMD_DATAGRAM_TYPE_VALIDATE_SIGNATURE_RESPONSE			BMD_DATAGRAM_TYPE_VALIDATE_SIGNATURE+100
#define BMD_DATAGRAM_TYPE_UPDATE_RESPONSE				BMD_DATAGRAM_TYPE_UPDATE+100
#define BMD_DATAGRAM_TYPE_CLOSE_SESSION_RESPONSE			BMD_DATAGRAM_TYPE_CLOSE_SESSION+100
#define BMD_DATAGRAM_TYPE_LOGIN_AS_OTHER_RESPONSE			BMD_DATAGRAM_TYPE_LOGIN_AS_OTHER+100
#define BMD_DATAGRAM_TYPE_LOGIN_FOREVER_RESPONSE			BMD_DATAGRAM_TYPE_LOGIN_FOREVER+100
#define BMD_DATAGRAM_TYPE_CERTIFY_DVCS_RESPONSE				BMD_DATAGRAM_TYPE_CERTIFY_DVCS+100
#define BMD_DATAGRAM_TYPE_CERTIFY_TIMESTAMP_RESPONSE			BMD_DATAGRAM_TYPE_CERTIFY_TIMESTAMP+100
#define BMD_DATAGRAM_TYPE_CERTIFY_DOCUMENT_RESPONSE			BMD_DATAGRAM_TYPE_CERTIFY_DOCUMENT+100
#define BMD_DATAGRAM_TYPE_VERIFY_CRYPTO_SIGNATURE_RESPONSE		BMD_DATAGRAM_TYPE_VERIFY_CRYPTO_SIGNATURE+100
#define BMD_DATAGRAM_TYPE_END_TRANSACTION_RESPONSE			BMD_DATAGRAM_TYPE_END_TRANSACTION+100
#define BMD_DATAGRAM_TYPE_COUNT_DOCS_RESPONSE				BMD_DATAGRAM_TYPE_COUNT_DOCS+100
#define BMD_DATAGRAM_GET_METADATA_ADD_RESPONSE				BMD_DATAGRAM_GET_METADATA_ADD+100
#define BMD_DATAGRAM_DIR_CREATE_RESPONSE				BMD_DATAGRAM_DIR_CREATE+100
#define BMD_DATAGRAM_DIR_LIST_RESPONSE					BMD_DATAGRAM_DIR_LIST+100
#define BMD_DATAGRAM_DIR_LIST_WITH_OR_RESPONSE				BMD_DATAGRAM_DIR_LIST_WITH_OR+100
#define BMD_DATAGRAM_DIR_DELETE_RESPONSE				BMD_DATAGRAM_DIR_DELETE+100
#define BMD_DATAGRAM_DIR_GET_CONTENT_RESPONSE				BMD_DATAGRAM_DIR_GET_CONTENT+100
#define BMD_DATAGRAM_DIR_GET_DETAIL_RESPONSE				BMD_DATAGRAM_DIR_GET_DETAIL+100
#define BMD_DATAGRAM_TYPE_SEARCH_RESPONSE				BMD_DATAGRAM_TYPE_SEARCH+100
#define BMD_DATAGRAM_TYPE_SEARCH_UNRECEIVED_RESPONSE			BMD_DATAGRAM_TYPE_SEARCH_UNRECEIVED+100
#define BMD_DATAGRAM_TYPE_REGISTER_USER_RESPONSE			BMD_DATAGRAM_TYPE_REGISTER_USER+100
#define BMD_DATAGRAM_TYPE_UPDATE_USER_RESPONSE				BMD_DATAGRAM_TYPE_UPDATE_USER+100
#define BMD_DATAGRAM_TYPE_GET_USER_LIST_RESPONSE			BMD_DATAGRAM_TYPE_GET_USER_LIST+100
#define BMD_DATAGRAM_TYPE_REGISTER_ROLE_RESPONSE			BMD_DATAGRAM_TYPE_REGISTER_ROLE+100
#define BMD_DATAGRAM_TYPE_UPDATE_ROLE_RESPONSE				BMD_DATAGRAM_TYPE_UPDATE_ROLE+100
#define BMD_DATAGRAM_TYPE_GET_ROLE_LIST_RESPONSE			BMD_DATAGRAM_TYPE_GET_ROLE_LIST+100
#define BMD_DATAGRAM_TYPE_DELETE_ROLE_RESPONSE				BMD_DATAGRAM_TYPE_DELETE_ROLE+100
#define BMD_DATAGRAM_TYPE_CHOOSE_ROLE_RESPONSE				BMD_DATAGRAM_TYPE_CHOOSE_ROLE+100
#define BMD_DATAGRAM_LINK_CREATE_RESPONSE				BMD_DATAGRAM_LINK_CREATE+100
#define BMD_DATAGRAM_ADD_SIGNATURE_RESPONSE				BMD_DATAGRAM_ADD_SIGNATURE+100
#define BMD_DATAGRAM_TYPE_REGISTER_GROUP_RESPONSE			BMD_DATAGRAM_TYPE_REGISTER_GROUP+100
#define BMD_DATAGRAM_TYPE_UPDATE_GROUP_RESPONSE				BMD_DATAGRAM_TYPE_UPDATE_GROUP+100
#define BMD_DATAGRAM_TYPE_GET_GROUP_LIST_RESPONSE			BMD_DATAGRAM_TYPE_GET_GROUP_LIST+100
#define BMD_DATAGRAM_TYPE_DELETE_GROUP_RESPONSE				BMD_DATAGRAM_TYPE_DELETE_GROUP+100
#define BMD_DATAGRAM_TYPE_REGISTER_SECURITY_RESPONSE			BMD_DATAGRAM_TYPE_REGISTER_SECURITY+100
#define BMD_DATAGRAM_TYPE_UPDATE_SECURITY_RESPONSE			BMD_DATAGRAM_TYPE_UPDATE_SECURITY+100
#define BMD_DATAGRAM_TYPE_GET_SECURITY_LIST_RESPONSE			BMD_DATAGRAM_TYPE_GET_SECURITY_LIST+100
#define BMD_DATAGRAM_TYPE_DELETE_SECURITY_RESPONSE			BMD_DATAGRAM_TYPE_DELETE_SECURITY+100
#define BMD_DATAGRAM_TYPE_REGISTER_CLASS_RESPONSE			BMD_DATAGRAM_TYPE_REGISTER_CLASS+100
#define BMD_DATAGRAM_TYPE_UPDATE_CLASS_RESPONSE				BMD_DATAGRAM_TYPE_UPDATE_CLASS+100
#define BMD_DATAGRAM_TYPE_GET_CLASS_LIST_RESPONSE			BMD_DATAGRAM_TYPE_GET_CLASS_LIST+100
#define BMD_DATAGRAM_TYPE_DELETE_CLASS_RESPONSE				BMD_DATAGRAM_TYPE_DELETE_CLASS+100
#define BMD_DATAGRAM_TYPE_CONSERVATE_TIMESTAMPS_RESPONSE		BMD_DATAGRAM_TYPE_CONSERVATE_TIMESTAMPS+100
#define BMD_DATAGRAM_TYPE_UPDATE_METADATA_DEFAULT_VALUE_RESPONSE	BMD_DATAGRAM_TYPE_UPDATE_METADATA_DEFAULT_VALUE+100
#define BMD_DATAGRAM_TYPE_GET_ACCESS_OBJECTS_RESPONSE			BMD_DATAGRAM_TYPE_GET_ACCESS_OBJECTS+100
#define BMD_DATAGRAM_TYPE_ADD_CERTIFICATE_TO_IDENTITY_RESPONSE		BMD_DATAGRAM_TYPE_ADD_CERTIFICATE_TO_IDENTITY+100
#define BMD_DATAGRAM_TYPE_DISABLE_IDENTITY_RESPONSE			BMD_DATAGRAM_TYPE_DISABLE_IDENTITY+100
#define BMD_DATAGRAM_TYPE_CREATE_CASCADE_LINKS_RESPONSE			BMD_DATAGRAM_TYPE_CREATE_CASCADE_LINKS+100
#define BMD_DATAGRAM_TYPE_UPDATE_FILE_VERSION_LO_RESPONSE		BMD_DATAGRAM_TYPE_UPDATE_FILE_VERSION_LO+100
#define BMD_DATAGRAM_TYPE_DELETE_CASCADE_LINKS_RESPONSE			BMD_DATAGRAM_TYPE_DELETE_CASCADE_LINKS+100
#define BMD_DATAGRAM_TYPE_UPDATE_SYS_METADATA_RESPONSE			BMD_DATAGRAM_TYPE_UPDATE_SYS_METADATA+100
#define BMD_DATAGRAM_TYPE_GET_ARCHIVE_FILE_VERSION_LO_RESPONSE		BMD_DATAGRAM_TYPE_GET_ARCHIVE_FILE_VERSION_LO+100
#define BMD_DATAGRAM_TYPE_GET_VERSIONS_HISTORY_RESPONSE			BMD_DATAGRAM_TYPE_GET_VERSIONS_HISTORY+100
#define BMD_DATAGRAM_TYPE_DELETE_REPLACED_LOBS_RESPONSE			BMD_DATAGRAM_TYPE_DELETE_REPLACED_LOBS+100
#define BMD_DATAGRAM_TYPE_GET_ACTIONS_HISTORY_RESPONSE			BMD_DATAGRAM_TYPE_GET_ACTIONS_HISTORY+100
#define BMD_DATAGRAM_TYPE_SEND_ADVICE_WITH_PARAMETERS_RESPONSE		BMD_DATAGRAM_TYPE_SEND_ADVICE_WITH_PARAMETERS+100

/* Typy metadanych */
typedef enum EnumMetaData { UNDEFINED_METADATA, ACTION_METADATA, SYS_METADATA, PKI_METADATA, ADDITIONAL_METADATA } EnumMetaData_t;

typedef enum HexSeparation { WITHOUT_SEPARATOR=0, WITH_SEPARATOR } HexSeparation_e;

/* definicje kodow bledow */
#define BMD_OK 0

/* połączenia persistent */
#define BMD_NOT_PERSISTENT	0
#define BMD_PERSISTENT		1

#define BMD_SEND_LOB		1
#define BMD_RECV_LOB		2
#define BMD_RECV_LOB_IN_PARTS	3

#define BMD_ONLY_GROUP_DOC		BMD_REQ_DB_SELECT_ONLY_GROUP
#define BMD_ONLY_OWNER_DOC		BMD_REQ_DB_SELECT_ONLY_OWNER
#define BMD_ONLY_CREATOR_DOC		BMD_REQ_DB_SELECT_ONLY_CREATOR
#define BMD_OWNER_OR_CREATOR_DOC	BMD_REQ_DB_SELECT_CREATOR_OR_OWNER

#define BMD_QUERY_ILIKE         BMD_REQ_DB_SELECT_ILIKE_QUERY		/* zapytanie ignorujace wielkosc liter i dopasowujace podlancuchy */
#define BMD_QUERY_LIKE          BMD_REQ_DB_SELECT_LIKE_QUERY		/* zapytanie nie ignorujace wielkosc liter i dopasowujace podlancuchy */
#define BMD_QUERY_EXACT_ILIKE   BMD_REQ_DB_SELECT_EXACT_ILIKE_QUERY	/* zapytanie ignorujace wielkosc liter ale dopasowujace dokladnie */
#define BMD_QUERY_EQUAL         BMD_REQ_DB_SELECT_EQUAL_QUERY		/* zapytanie dopasowujace doslawnie, przez operator '=' */

#define BMD_LINK_CREATION_IGNORE_DEPENDENT_GROUPS 30050000

#ifndef _REFRESH_CALLBACK
	#define _REFRESH_CALLBACK
	typedef long (*bmdnet_progress_callback_t) (long total_bytes,
						   long done_bytes);
#endif

LIBBMD_SCOPE long bmd_init(void);
LIBBMD_SCOPE long bmd_end(void);
LIBBMD_SCOPE long bmd_info_create(bmd_info_t **bi);
LIBBMD_SCOPE long bmd_info_login_set(const char *bmd_address,const long bmd_port,const char *proxy_url,
					const long proxy_port,const char *proxy_username,const char *proxy_password,const long proxy_type,
					const char *proxy_auth_type,bmd_info_t **bi);

LIBBMD_SCOPE long bmd_info_set_role_callback(role_selection_callback function,bmd_info_t **bi);

LIBBMD_SCOPE long bmd_info_set_selected_role(bmd_info_t **bi);

LIBBMD_SCOPE long bmd_info_set_cert_login_as(GenBuf_t *cert_login_as,bmd_info_t **bi);

LIBBMD_SCOPE long bmd_login(bmd_info_t ** bmd_info);

LIBBMD_SCOPE long bmd_login2(bmd_info_t ** bmd_info, long *servver);

LIBBMD_SCOPE long bmd_logout(	bmd_info_t **bmd_info,
				long deserialisation_max_memory);

LIBBMD_SCOPE long bmd_send_request(	bmd_info_t *bmd_info,
					bmdDatagramSet_t *request_datagram_set,
					long twf_max_datagram_in_set_transmission,
					bmdDatagramSet_t **response_datagram_set,
					long twf_free_datagrams_flag,
					long deserialisation_max_memory);

LIBBMD_SCOPE long bmd_send_request_2(	bmd_info_t *bmd_info,
					bmdDatagramSet_t *request_datagram_set,
					long twf_max_datagram_in_set_transmission,
					bmdDatagramSet_t **response_datagram_set,
					bmdnet_progress_callback_t refreshcallback,
					long twf_free_datagrams_flag,
					long deserialisation_max_memory);

/*LIBBMD_SCOPE long bmd_send_nop(bmd_info_t * bmd_info); // funkcja juz nie uzywana - patrz bmd_check_protocol_version()*/

LIBBMD_SCOPE long bmd_check_protocol_version(	bmd_info_t * bmd_info,
						long * klntver,
						long * servver,
						long deserialisation_max_memory);



LIBBMD_SCOPE long bmd_send_request_lo_in_parts(	bmd_info_t *bmd_info,
						bmdDatagramSet_t *request_datagram_set,
						long twf_max_datagram_in_set_transmission,
						bmdDatagramSet_t **response_datagram_set,
						lob_request_info_t *info,
						bmdnet_progress_callback_t refreshcallback,
						long total_file_size,
						long lob_session_hash,
						long stream_timeout,
						long deserialisation_max_memory);

LIBBMD_SCOPE long bmd_send_request_lo(	bmd_info_t *bmd_info,
					bmdDatagramSet_t *request_datagram_set,
					long twf_max_datagram_in_set_transmission,
					bmdDatagramSet_t **response_datagram_set,
					lob_request_info_t *info,
					long deserialisation_max_memory);

LIBBMD_SCOPE long bmd_send_request_lo2(	bmd_info_t *bmd_info,
					bmdDatagramSet_t *request_datagram_set,
					long twf_max_datagram_in_set_transmission,
					bmdDatagramSet_t **response_datagram_set,
					lob_request_info_t *info,
					bmdnet_progress_callback_t refreshcallback,
					long deserialisation_max_memory);

LIBBMD_SCOPE long bmd_send_request_lo2_to_buf(	bmd_info_t *bmd_info,
						bmdDatagramSet_t *request_datagram_set,
						long twf_max_datagram_in_set_transmission,
						bmdDatagramSet_t **response_datagram_set,
						lob_request_info_t *info,
						bmdnet_progress_callback_t refreshcallback,
						GenBuf_t **lobBuffer,
						long maxLobSize,
						long deserialisation_max_memory);

LIBBMD_SCOPE long bmd_datagram_create(const long type,bmdDatagram_t ** datagram);

LIBBMD_SCOPE long bmd_datagram_add_data(const GenBuf_t * data,const char *data_name,bmdDatagram_t ** datagram);

LIBBMD_SCOPE long bmd_datagram_add_file(const char *file,bmdDatagram_t ** datagram);
LIBBMD_SCOPE long bmd_datagram_set_filename(const char *filename,bmdDatagram_t **dtg);

LIBBMD_SCOPE long bmd_datagram_add_metadata(EnumMetaData_t type,const char *oid,GenBuf_t * value,
bmdDatagram_t **datagram, long twf_copy_ptr);

LIBBMD_SCOPE long bmd_datagram_add_metadata_2(const char *oid, GenBuf_t * value,bmdDatagram_t **datagram, long twf_copy_ptr);

LIBBMD_SCOPE long bmd_datagram_add_metadata_char(	const char *oid,
							char *value,
							bmdDatagram_t **datagram);

LIBBMD_SCOPE long bmd_datagram_add_metadata_char_type(	const char *oid,
							long type,
							char *value,
							bmdDatagram_t **datagram);

LIBBMD_SCOPE long bmd_datagram_add_metadata_2_with_params(	const char *oid,
										GenBuf_t * value,
										bmdDatagram_t **datagram,
										EnumMetaData_t metadataType,
										long myId,
										long ownerType,
										long ownerId,
										long twf_copy_ptr);

LIBBMD_SCOPE long bmd_datagram_add_limits(long from, long offset,bmdDatagram_t ** datagram);
LIBBMD_SCOPE long bmd_add_hash_to_dtg(char *hash,bmdDatagram_t **dtg);
LIBBMD_SCOPE long bmd_add_hash_to_dtg_as_data(char *hash,bmdDatagram_t **dtg);
LIBBMD_SCOPE long bmd_genbuf_set(const char *content,const long length, GenBuf_t ** buf);
LIBBMD_SCOPE long bmd_genbuf_free(GenBuf_t ** buf);
LIBBMD_SCOPE long bmd_genbuf_hex_dump(GenBuf_t *input,char **output,char sep,long uppercase);
LIBBMD_SCOPE long bmd_hex_genbuf_bin_dump(char *input,GenBuf_t **output);
LIBBMD_SCOPE long bmd_convert_hex2base64(char* hexString, HexSeparation_e hexSeparation, GenBuf_t** base64Buffer);
LIBBMD_SCOPE long bmd_genbuf_set_from_file(char *filename,GenBuf_t **output);

LIBBMD_SCOPE long bmd_genbuf_get_buf(GenBuf_t *gb,char **buf);
LIBBMD_SCOPE long bmd_genbuf_get_size(GenBuf_t *gb,long *size);

LIBBMD_SCOPE long genbuf_to_char(GenBuf_t *gbuf,char **output);

LIBBMD_SCOPE long bmd_datagram_free(bmdDatagram_t ** datagram);

LIBBMD_SCOPE long bmd_datagram_get_id(const bmdDatagram_t * datagram,long *id);

LIBBMD_SCOPE long bmd_datagram_set_id(const long *id,bmdDatagram_t ** datagram);

LIBBMD_SCOPE long bmd_datagram_save_file(const char *filename,const bmdDatagram_t *datagram);

LIBBMD_SCOPE long bmd_datagram_get_metadata(EnumMetaData_t type, const char *oid,bmdDatagram_t *datagram, long pos, GenBuf_t **metadata_value);
LIBBMD_SCOPE long bmd_datagram_get_metadata_ownerType(EnumMetaData_t type, const char *oid, bmdDatagram_t *datagram, long pos, long *ownerType);

LIBBMD_SCOPE long bmd_datagram_get_metadata_2(const char *oid,bmdDatagram_t *datagram,GenBuf_t **metadata_value);

LIBBMD_SCOPE long bmd_datagram_add_to_set(bmdDatagram_t * datagram,bmdDatagramSet_t **datagrams_set);

LIBBMD_SCOPE long bmd_datagram_determine_pki_metadata_types(bmdDatagram_t *datagram);

LIBBMD_SCOPE long bmd_datagram_set_status(long status,long sql_operator,bmdDatagram_t **datagram);

LIBBMD_SCOPE long bmd_lob_request_info_create(lob_request_info_t **lob_info);

LIBBMD_SCOPE long bmd_lob_request_info_destroy(lob_request_info_t **lob_info);
LIBBMD_SCOPE long bmd_lob_request_info_add_filename(const char *filename,lob_request_info_t **lob_info,long send_or_recv,long size);
LIBBMD_SCOPE long bmd_lob_request_info_add_buffer(lob_request_info_t *lob_info, GenBuf_t* buffer);

#if 0
/* PLefunkcje puste - nie wykorzystywane */
LIBBMD_SCOPE long bmd_lob_request_info_set_progress_flag(long send_or_recv);
LIBBMD_SCOPE long bmd_lob_request_info_set_progress_data(void *send_progress_data,void *recv_progress_data);
#endif
LIBBMD_SCOPE long bmd_info_set_credentials_pfx(char *pfx_filename,char *password,long pass_length,bmd_info_t **bi);
LIBBMD_SCOPE long bmd_info_set_bmd_cert(char *cert_filename,bmd_info_t **bi);

LIBBMD_SCOPE long bmd_info_get_form(long form_type,bmd_info_t *bi,BMD_attr_t ***form,long *form_count);
LIBBMD_SCOPE long bmd_info_load_forms(bmd_info_t *bmdInfo, bmdDatagram_t *dtg);
LIBBMD_SCOPE long bmd_info_destroy_forms(bmd_info_t *bmdInfo);
LIBBMD_SCOPE long bmd_info_destroy_actions(bmd_info_t *bmdInfo);
LIBBMD_SCOPE long bmd_info_destroy(bmd_info_t **bi); /*by RO*/

LIBBMD_SCOPE long bmd_attr_get_oid(long nr,BMD_attr_t **form,char **oid);
LIBBMD_SCOPE long bmd_attr_get_description(long nr,BMD_attr_t **form,char **description);
LIBBMD_SCOPE long bmd_datagramset_free(bmdDatagramSet_t **dtg_set);
LIBBMD_SCOPE long bmd_datagramset_get_datagram(bmdDatagramSet_t *dtg_set,long nr,bmdDatagram_t **dtg);
LIBBMD_SCOPE long bmd_datagram_get_filename(bmdDatagram_t *dtg,char **filename);

LIBBMD_SCOPE long bmd_datagram_get_file(bmdDatagram_t *dtg,GenBuf_t **file);
#if defined(__cplusplus)
}
#endif
#endif
