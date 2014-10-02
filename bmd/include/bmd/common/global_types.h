#ifndef _LIBBMD_GLOBAL_TYPES_
#define _LIBBMD_GLOBAL_TYPES_

#ifdef WIN32
#pragma warning( disable : 4114 )
#endif

#include <bmd/common/bmd-sds.h>
#include <bmd/common/bmd-crypto_ds.h>
#include <bmd/libbmdconfig/libbmdconfig.h>
#include <bmd/libbmdaa2/configuration/AA_config.h>
#include <bmd/libbmdasn1_common/AttributeCertificate.h>

typedef enum EnumDbFetchMode { FETCH_NEXT, FETCH_ABSOLUTE } EnumDbFetchMode_t;
typedef enum EnumDbOrderingMode { ORDER_ASC, ORDER_DESC } EnumDbOrderingMode_t;
typedef enum EnumDbWhichPiece { LOB_FIRST_PIECE, LOB_NEXT_PIECE, LOB_LAST_PIECE, LOB_ONE_PIECE } EnumDbWhichPiece_t;
typedef enum KontrolkiType { SEARCH_KONTROLKI,SEND_KONTROLKI,UPDATE_KONTROLKI,UNVISIBLE_KONTROLKI, GRID_KONTROLKI, HISTORY_KONTROLKI} KontrolkiType_t;
typedef enum LobsInDatagram {LOB_NOT_IN_DATAGRAM, LOB_IN_DATAGRAM} LobsInDatagram_t;
typedef enum TimestampType {MAIN_TS, ADD_TS, SIGN_TS, DVCS_TS} TimestampType_t;
typedef enum TimestampSource {PLAIN_DATA, HASH_DATA} TimestampSource_t;

typedef struct JS_return {
	long a;
	char **oid;		    /* !!! lista identyfikatorow LOB'ow z bazy  */
	long cur_index;
	LobsInDatagram_t LobsInDatagram;
	bmd_crypt_ctx_t *user_cert_ctx;	    /* kontekst z certyfikatem uzytkownika */
	long symkeyForUserAlreadySet;	/* To dotyczy tylko pobierania plikow z archiwum
						0 oznacza, ze klucz symetryczny dla uzytkownika (potrzebny do deszyfrowania LOBow) nie zostal jeszcze ustawiony w odpowiedzi
						1 oznacza, ze klucz symetryczny dla uzytkownika zostal juz ustawiony w pierwszym POPRAWNYM dagramie w datagramsecie odpowiedzi
						*/
	bmd_crypt_ctx_t *server_ctx;	    /* kontekst ktorym zdeszyfruje sie klucz symetryczny szyfrujacy LOB'a */
	bmd_crypt_ctx_t **lob_ctx_server;   /* lista kontekstow deszyfrujacych LOB : db --> serwer */
	bmd_crypt_ctx_t *lob_ctx_user;	    /* pojedynczy kontekst szyfrujacy dane do uzytkownika - msilewicz */
} JS_return_t;

typedef struct user_forms_data {
	char *roleName;
	BMD_attr_t **formularz;
	long no_of_formularz;
} user_forms_data_t;

/* struktura opisujaca dane potrzebne do obsluzenia requesta */
typedef struct server_request_data {
	void *db_conn_info; 			/**< uniwersalny wskaznik do polaczen z baza danych */

	GenBuf_t *receive_buffer;		/**< bufor odbiorczy */
	GenBuf_t *request_buffer;  		/**< bufor requesta */
	GenBuf_t *response_buffer; 		/**< bufor dla responsa */
	GenBuf_t *DER_session_request;

	uint32_t request_id; 			/**< identyfikator requesta */
	uint32_t ki_mod_n;   			/**< cos co zapobiega replayom */

	GenBuf_t *k0;				/* wartosc k0 odebrana przy pierwszym logowaniu sie klienta - ma znaczenie tylko przez requescie logowania sie */
	GenBuf_t *i;				/* wartosc "i" wygenerowana przez serwer */
	GenBuf_t *n;				/* wartosc "n" wygenerowana przez serwer */

	GenBuf_t *cur_ki_mod_n;			/* aktualna wartosc k^i mod n przechowywana w cache */
	GenBuf_t *client_ki_mod_n;		/* wartosc k^i mod n przyslana przez klienta */

	bmd_crypt_ctx_t *symmetricContext;
	bmd_crypt_ctx_t *userCertificate;
	bmdDatagramSet_t *request_datagram; 	/**< datagram requesta */

	bmdDatagram_t *login_request_datagram;
	bmdDatagram_t *login_response_datagram;	 /**< datagram response */

	bmdDatagramSet_t *js_datagram;
	JS_return_t js_ret;				/**< wynik dziala JS_main - LOB_IN_DATAGRAM lub LOB_NOT_IN_DATAGRAM */

	AttributeCertificate_t *ac_cert;		/**< zdekodowany certyfikat atrybutow */
	certificate_attributes_t *ua;			/**< atrybuty uzytkownika */

	GenBuf_t *X509_buf;
	GenBuf_t *ac; 					/**< DER certyfikatu atrybutow */
	GenBuf_t *cert_login_as;			/**< DER certyfikatu w imieniu ktorego nastepuje logowanie */
	GenBuf_t *server_symmetric_buf;		/**< DER klucza sesyjnego do umieszczenia w cache */
	GenBuf_t *client_symmetric_buf;		/**< DER tego samego klucza sesyjnego do wyslania do uzytkownika */
	GenBuf_t *hash_client_symmetric_buf;	/**< HASH z tego co powyzej */

	BMD_attr_t **formularz;			/**aktualny formularz **/
	long no_of_formularz;
	user_forms_data_t **user_forms;		/**formularze dostepne dla wszystkich roli uzytkownika **/

	bmd_crypt_ctx_t *smimeContext;

	/* zmienne konfiguracyjne */
	char *adres_znacznika_czasu;
	long port_znacznika_czasu;

	char *tsp_resp;
	long tsp_resp_length;

	char *cert_store;

/* 	GenBuf_t *user_role;
*/

	long no_of_actions;
	long *actions;

	BMD_attr_t **user_form;
} server_request_data_t;


/* wskazniki na funkcje z pluginow dotyczacych requestow sesyjnych */
typedef long (*session_plugin_init_func)(const char *);
typedef long (*session_plugin_pre_func)(bmdDatagramSet_t *);
#ifndef WIN32
typedef long (*session_plugin_post_func)(bmdDatagramSet_t *,char *, long, long, long, server_request_data_t *);
#else /*WIN32*/
typedef long (*session_plugin_post_func)(bmdDatagramSet_t *,char *, long, long, long, server_request_data_t *, HANDLE*);
#endif /*ifndef WIN32*/

typedef long (*session_plugin_finalize)(void);

typedef struct session_plugin_info {
	session_plugin_init_func	bmd_plugin_init;
    session_plugin_post_func	bmd_plugin_action_post;	    /* funkcja wykonywana po polaczeniu z BDPA */
	char *plugin_library;
    char *plugin_library_conf;
    void *plugin_library_handle;
} session_plugin_info_t;


/* wskazniki na funkcje z pluginow dotyczacycj weryfikacji certyfikatow logujacych sie */
typedef long (*auth_plugin_init_func)(const char *);
typedef long (*auth_plugin_verify_func)(GenBuf_t *);
typedef long (*auth_plugin_finalize_func)(void);

typedef long (*conservate_plugin_init_func)(const char *plugin_conservate_conf_file);
typedef long (*conservate_plugin_do_func)(server_request_data_t *);
typedef long (*conservate_plugin_timestamp_pades_func)(GenBuf_t*, GenBuf_t**, GenBuf_t**);
typedef long (*conservate_plugin_do_single_func)(void *, char *ts_server, long ts_port, struct GenBufList *list, const char* const timestamp_metadata_oid_str);
typedef	long (*timestamp_plugin_do_single_func) ( GenBuf_t *, TimestampType_t, TimestampSource_t, GenBuf_t **);
typedef long (*conservate_plugin_finalize_func)(void);
typedef long (*bmd_run_asynchronous_timestamping_func)(void);
typedef long (*dvcs_plugin_init_func)(const char *plugin_dvcs_conf_file);
typedef long (*dvcs_plugin_do_func)(void *, int, bmdDatagramSet_t *, bmdDatagramSet_t **,server_request_data_t *);
typedef long (*dvcs_plugin_validate_certificate)(GenBuf_t *);
typedef long (*dvcs_plugin_validate_external_sig)(bmdDatagram_t *, GenBuf_t *);
typedef long (*dvcs_plugin_certify_DVCScert_ft)(bmdDatagram_t *, bmdDatagramSet_t **); /* ft = function type (gdyby ktos byl ciekaw)
*/
typedef long (*dvcs_plugin_certify_timestamp_ft)(bmdDatagram_t *, bmdDatagramSet_t **);
typedef long (*dvcs_plugin_certify_document_ft)(bmdDatagram_t *, bmdDatagramSet_t **);
typedef long (*dvcs_plugin_verify_cryptObject_signature_ft)(bmdDatagram_t *, bmdDatagramSet_t **);

typedef long (*clear_plugin_init_func)(const char *plugin_clear_conf_file);
typedef long (*clear_plugin_do_func)(void *const twf_GLOBAL_bmd_configuration_ptr);
typedef long (*clear_plugin_expired_links_remover_func)(void);
typedef long (*clear_plugin_start_lobs_remover_func)(void* hDB, long* deletedCounter);
typedef long (*clear_plugin_run_lobs_remover_func)(void);
typedef long (*clear_plugin_update_last_insertion_time_func)(void);
typedef long (*clear_plugin_finalize_func)(void);

typedef long (*messaging_plugin_send_notification_func)(bmdDatagramSet_t *);
typedef long (*messaging_plugin_register_action_func)(void *, bmdDatagram_t *, bmdDatagram_t *, server_request_data_t*);
typedef long (*messaging_plugin_register_error_func)(void *, long, bmdDatagramSet_t *, server_request_data_t*);
typedef long (*messaging_plugin_delete_action_register_func)(void*, char *, char *);
typedef long (*messaging_plugin_init_func)(const char *);

typedef long (*archiving_plugin_start_func)(void);

typedef struct server_crypto_info {
	bmd_crypt_ctx_t *server_ctx;
	bmd_crypt_ctx_t *server_sym_ctx;
	bmd_crypt_ctx_t *smime_ctx;
} server_crypto_info_t;

typedef struct auth_plugin_info {
	auth_plugin_init_func bmd_auth_plugin_initialize;	    /* inicjalizuje plugin */
	auth_plugin_verify_func bmd_auth_plugin_verify;	    /* weryfikuje certyfikat */
	auth_plugin_finalize_func bmd_auth_plugin_finalize;	    /* finalizuje plugin */
	char *auth_plugin_library;
	char *auth_plugin_library_conf;
	void *auth_plugin_library_handle;
} auth_plugin_info_t;

typedef struct conservate_plugin_info
{
	conservate_plugin_init_func 		bmd_conservate_plugin_initialize;	    	/* inicjalizuje plugin */
	conservate_plugin_do_func 		bmd_conservate_plugin_do;
	conservate_plugin_timestamp_pades_func	bmd_plugin_timestamp_pades;
	conservate_plugin_do_single_func	bmd_conservate_plugin_do_single;/*praca plugina*/
	timestamp_plugin_do_single_func		bmd_do_timestamp_plugin;
	conservate_plugin_finalize_func		bmd_conservate_plugin_finalize;
	bmd_run_asynchronous_timestamping_func	bmd_run_asynchronous_timestamping;
	char *conservate_plugin_library;
	char *conservate_plugin_library_conf;
	void *conservate_plugin_library_handle;
} conservate_plugin_info_t;

typedef struct dvcs_plugin_info
{
	dvcs_plugin_init_func 			bmd_dvcs_plugin_initialize;	    		/* inicjalizuje plugin */
	dvcs_plugin_do_func 			bmd_dvcs_plugin_do;
	dvcs_plugin_validate_certificate	bmd_dvcs_plugin_validate_certificate;		/* walidacja certyfikatu podanego w parametrze */
	dvcs_plugin_validate_external_sig	bmd_dvcs_plugin_validate_external_signature;	/* walidacja podpisu zewnetrznego podanego w parametrze */
	dvcs_plugin_certify_DVCScert_ft		bmd_dvcs_plugin_certify_DVCScert;		/* poswiadczanie DVCS istnienia poswiadczenia DVCS */
	dvcs_plugin_certify_timestamp_ft	bmd_dvcs_plugin_certify_timestamp;		/* poswiadczanie DVCS istnienia timestampa */
	dvcs_plugin_certify_document_ft		bmd_dvcs_plugin_certify_document;		/* poswiadczanie DVCS istnienia dokumentu (cryptoObjectu) */
	dvcs_plugin_verify_cryptObject_signature_ft bmd_dvcs_plugin_verify_cryptObject_signature; /* weryfikacji podpisu zlozonoe w archiwum jako dokument (cryptoObject) */

	char *dvcs_plugin_library;
	char *dvcs_plugin_library_conf;
	void *dvcs_plugin_library_handle;
} dvcs_plugin_info_t;

typedef struct clear_plugin_info
{
	clear_plugin_init_func 		bmd_clear_plugin_initialize;	    	/* inicjalizuje plugin */
	clear_plugin_finalize_func	bmd_clear_plugin_finalize;
	clear_plugin_do_func 		bmd_clear_plugin_do;
	clear_plugin_expired_links_remover_func		bmd_run_expired_links_remover;
	clear_plugin_start_lobs_remover_func		bmd_start_lobs_remover;
	clear_plugin_run_lobs_remover_func		bmd_run_lobs_remover;
	clear_plugin_update_last_insertion_time_func	bmd_update_last_insertion_time;

	char *clear_plugin_library;
	char *clear_plugin_library_conf;
	void *clear_plugin_library_handle;

} clear_plugin_info_t;

typedef struct messaging_plugin_info
{
	messaging_plugin_send_notification_func 	bmd_messaging_plugin_send_notification;
	messaging_plugin_register_action_func		bmd_messaging_plugin_register_action;
	messaging_plugin_register_error_func		bmd_messaging_plugin_register_error;
	messaging_plugin_delete_action_register_func	bmd_messaging_plugin_delete_action_register;
	messaging_plugin_init_func 			bmd_messaging_plugin_initialize;

	char *messaging_plugin_library;
	char *messaging_plugin_library_conf;
	void *messaging_plugin_library_handle;

} messaging_plugin_info_t;

typedef struct archiving_plugin_info
{
	archiving_plugin_start_func bmd_archiving_plugin_start;

	char *archiving_plugin_library;
	char *archiving_plugin_library_conf;
	void *archiving_plugin_library_handle;

} archiving_plugin_info_t;


typedef struct logs_configuration {
	long quiet_FATAL_enable; /* 0 oznacza disabled , 1  enabled, inne wartosci nie sa dopuszczalne
*/
	long verbose_FATAL_enable;
	long quiet_DENY_enable;
	long verbose_DENY_enable;
	long quiet_DONE_enable;
	long verbose_DONE_enable;
	long quiet_TRY_enable;
	long verbose_TRY_enable;
	long quiet_DEBUG_enable;
	long verbose_DEBUG_enable;
	long log_server_SEND_enable;  /* oznacza czy zamierzamy zarejestrowa�zdarzenie w serwerze log�. TK */
	char *log_server_conf;
} logs_configuration_t;

typedef struct server_configuration {

	char *bmd_conf_file;
	char *bmd_enc_file;

	char *address;
	long port;
	char *server_label;
	char *bmd_privkey_password;
	char *bmd_pfx_file;
	long sending_thread;
	long max_datagram_in_set_transmission;
	long deserialisation_max_memory;
	char *location_id;
	long enable_location_id_in_search;

	long enable_group_check;
	long enable_clearance_check;

	long enable_delete_history;
	char *default_delete_update_reason;
	char *user_cert_time_registration;
	long enable_regexp_in_search_request;
	long allow_only_one_signature_per_file;
	long enable_report_counting;

	char *db_library;
	char *db_conninfo;
	char *db_username;
	char *db_password;
	char *db_address;
	long db_port;
	char *db_name;

	char *ks_adr;
	long ks_port;

	char *pkcs11_bmd_driver;

	char *aa_conf_file;

	char *bmd_xml_form;
	char *bmd_xsd_form;

	char *incoming_path;
	char *outgoing_path;
	char *error_path;


	struct SharedMemory *shm;                            /* konfiguracja Shared Memory */
	ConfigData_t *ConfigData;                            /* konfiguracja urzedzu atrybutow */

	session_plugin_info_t session_plugin_conf;           /* konfiguracja pluginu sesyjnego */
	auth_plugin_info_t auth_plugin_conf;                 /* konfiguracja pluginu do autoryzacji */
	conservate_plugin_info_t conservate_plugin_conf;     /* konfiguracja pluginu do konserwacji */
	clear_plugin_info_t clear_plugin_conf;               /* konfiguracja pluginu do czyszczenia bazy danych */
	dvcs_plugin_info_t dvcs_plugin_conf;                 /* konfiguracja pluginu dvcs */
	messaging_plugin_info_t messaging_plugin_conf;               /* konfiguracja pluginu do czyszczenia bazy danych */
	archiving_plugin_info_t archiving_plugin_conf;


	char *shared_mem_name;
	char *semaphore_name;

	/****************************************/
	/* konfiguracja kryptograficzna serwera */
	/****************************************/
	server_crypto_info_t *crypto_conf;

	/************************************************/
	/* konfiguracja sposobu tworzenia logow serwera */
	/************************************************/
	logs_configuration_t logs_configuration;

	/**********************************************************/
	/* wskazanie, czy configuracja jest aktualnie zablokowana */
	/**********************************************************/
	long server_configuration_block;

	long max_registered_identities;
	long max_registered_users;
	long max_registered_groups;
	long max_registered_roles;
	long max_registered_classes;
	long max_connections;

} server_configuration_t;

typedef struct plugin_std_messaging_configuration
{
	long register_action_in_db;
	long send_action_notification;
	long save_action_notification;
	long base64_action_notification;
	long rollback_request_due_to_registration_failure;

	char *register_action_db_library;
    	char *register_action_db_conninfo;
	char *register_action_db_username;
	char *register_action_db_password;
	char *register_action_db_address;
	long register_action_db_port;
	char *register_action_db_name;

	char *send_action_notification_host;

	char *incoming_datagram_temporary_file;
	char *outgoing_datagram_temporary_file;

	long enable_data_filter;
	bmdStringArray_t *allowed_metadata_array;
} plugin_std_messaging_configuration_t;


typedef struct plugin_std_conservate_configuration
{
	long enable_conservation;

	long enable_main_ts;
	char *ts_main_address;
	long ts_main_port;
	long ts_main_timeout;
	
	long insert_timestamp_into_pdf;
	char* jre_home;
	char* java_wrapper_libs_path;
	long jvm_memory_size;

	long enable_additional_ts;
	char *ts_additional_address;
	long ts_additional_port;
	long ts_additional_timeout;

	long enable_signature_ts;
	char *ts_signature_address;
	char *ts_signature_pades_address;
	long ts_signature_port;
	long ts_signature_timeout;

	long enable_dvcs_ts;
	char *ts_dvcs_address;
	long ts_dvcs_port;
	long ts_dvcs_timeout;
	
	long asynchronous_timestamping;
	long concurrent_processes_count;
	long round_size_per_process;
	long round_sleep_per_process;

} plugin_std_conservate_configuration_t;


typedef struct plugin_std_archiving_configuration
{
	long enable_archiving;
	long archiving_period;
	char* in_use_oid;
	long interval_days;
	long interval_hours;
	long round_size;
} plugin_std_archiving_configuration_t;

typedef struct plugin_std_session_configuration
{
	long enable_first_get_notice;
	char *notice_metadata_oid;
	char *file_get_string;
	char *notice_for_user_class_id;
} plugin_std_session_configuration_t;

typedef struct bmd_srvr_crypto {
	char *pfx_file;
	char *prv_ky_pss;
	char *ks_addr;
	long ksport;

} bmd_svc_crypto_t;

typedef struct bmd_clearance {
	long sec_level_prio;	/* priorytet poziomu bezpiecze�twa */
	char *sec_level_oid;		/* poziom bezpieczenstwa - OID */
	char *sec_level_name;		/* nazwa poziomu bezpieczenstwa */
	long sec_category_id;	/* id poziomu z bazy danych */
	char *sec_category;		/* kategoria bezpieczenstwa - wartosc */
	char *sec_category_oid;		/* OID kategorii bezpieczenstwa */
} bmd_clearance_t;

typedef struct bmd_db_conn_info {
	char *db_conn_info;
} bmd_db_conn_info_t;

typedef struct bmd_thread_return {
	long tws_status;
} bmd_thread_return_t;

#endif /* _LIBBMD_GLOBAL_TYPES_ */
