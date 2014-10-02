#include <bmd/libbmdglobals/libbmdglobals.h>


	long				_GLOBAL_debug_level_error			= 0;
	long				_GLOBAL_debug_level_deny			= 0;
	long				_GLOBAL_debug_level_ok				= 0;
	long				_GLOBAL_debug_level_try				= 0;
	long				_GLOBAL_debug_level_debug			= 0;
	FILE*				_GLOBAL_win32_log_file 				= NULL;

	server_configuration_t*			_GLOBAL_bmd_configuration;
	plugin_std_messaging_configuration_t*	_GLOBAL_std_messaging_configuration;
	plugin_std_conservate_configuration_t*	_GLOBAL_std_conservate_configuration;
	plugin_std_archiving_configuration_t*	_GLOBAL_std_archiving_configuration	= NULL;
	plugin_std_session_configuration_t*		_GLOBAL_std_session_configuration = NULL;

	struct groups_graph_dict*	_GLOBAL_dict_groups_graph			= NULL;
	dictionary_data_t*		_GLOBAL_UsersDictionary				= NULL;
	dictionary_data_t*		_GLOBAL_GroupsDictionary			= NULL;
	dictionary_data_t*		_GLOBAL_ClassesDictionary			= NULL;
	dictionary_data_t*		_GLOBAL_UserGroupsDictionary			= NULL;
	dictionary_data_t*		_GLOBAL_UserClassesDictionary			= NULL;
	dictionary_data_t*		_GLOBAL_SecurityCategoriesGraphDictionary 	= NULL;
	dictionary_data_t*		_GLOBAL_GroupsGraphDictionary			= NULL;
	dictionary_data_t*		_GLOBAL_RolesDictionary				= NULL;
	dictionary_data_t*		_GLOBAL_UserRolesDictionary			= NULL;
	dictionary_data_t*		_GLOBAL_RolesAndActionsDictionary		= NULL;
	dictionary_data_t*		_GLOBAL_ActionsDictionary			= NULL;
	dictionary_data_t*		_GLOBAL_SecurityDictionary 			= NULL;
	dictionary_data_t*		_GLOBAL_AddMetadataTypesDictionary		= NULL;
	dictionary_data_t*		_GLOBAL_RoleRightsStructureDictionary		= NULL;
	dictionary_data_t*		_GLOBAL_RoleRightsDictionary			= NULL;
	dictionary_data_t*		_GLOBAL_SecurityCategoriesDictionary		= NULL;
	dictionary_data_t*		_GLOBAL_SecurityLevelsDictionary		= NULL;
	dictionary_data_t*		_GLOBAL_MetadataValuesDictionary		= NULL;

	char 				_GLOBAL_exbuff[1000000];
	long				_GLOBAL_libbmddb_initialized			= 0;

	bmd_db_fun_table_t*		_GLOBAL_f_tbl					= NULL;
	bmd_crypt_ctx_t*		_GLOBAL_ctx;
	void*				_GLOBAL_CallBackParent;

	int 				_CONST_CHARS_PER_LINE				= 72;
	bmd_conf*			_GLOBAL_StdSessionSOE_conf			= NULL;
	bmd_conf*			_GLOBAL_AdviceSOE_conf				= NULL;
	bmd_conf*			_GLOBAL_StdDVCS_conf				= NULL;
	bmd_conf*			_GLOBAL_StdConservate_conf			= NULL;
	bmd_conf*			_GLOBAL_StdClear_conf				= NULL;
	bmd_conf*			_GLOBAL_StdAuth_conf				= NULL;
	bmd_conf*			_GLOBAL_StdMessaging_conf			= NULL;

	server_request_data_t*		_GLOBAL_server_request_data			= NULL;
	long				_GLOBAL_interrupt				= 0;
	long 				_GLOBAL_can_close_server			= 0;
	bmd_conf*			_GLOBAL_konfiguracja				= NULL;
	bmd_conf*			_GLOBAL_logSoapKonfiguracja			= NULL;
	bmd_conf*			_GLOBAL_awizoSoapKonfiguracja			= NULL;
	bmd_conf*			_GLOBAL_ikpSoapKonfiguracja			= NULL;
	
	long				_GLOBAL_ikpSoap_enable_wssecurity		= 0;
	long				_GLOBAL_ikpSoap_secure_response			= 0;
	char*				_GLOBAL_ikpSoap_wsse_key				= NULL;
	char*				_GLOBAL_ikpSoap_wsse_key_password		= NULL;
	char*				_GLOBAL_ikpSoap_wsse_certificate		= NULL;

	char 				_GLOBAL_current_dir[256];
	char*				_GLOBAL_twg_transaction_dir_name		= NULL;

	bmd_info_t*			_GLOBAL_bmd_connection				= NULL;

#ifndef WIN32
//	invoiceData_t			_GLOBAL_InvoiceData;
//	awizoData_t			_GLOBAL_awizoData;
//	ikpData_t			_GLOBAL_ikpBrokerData;

	long				_GLOBAL_debug_level				= 0;
	long int 			_GLOBAL_children_number				= 0;
	char*				_GLOBAL_response				= NULL;
	long				_GLOBAL_index2					= 0;
	log_t* 				_GLOBAL_bmdlog_HANDLER;
	log_t* 				_GLOBAL_soaplog_HANDLER;

	/*int				_GLOBAL_bmdth_debug_mode		= 0; */
	/*int				_GLOBAL_thread_model_type		= BMD_THREAD_NOTINITIALIZED; */
	pid_t 				_GLOBAL_child					= 0;

	volatile sig_atomic_t		_GLOBAL_action					= NOTHING;
	bmdlogServParam_t		_GLOBAL_bmdlogServParam_tParams;
	rootServParam_t			_GLOBAL_rootServParam_tParams;

	pid_t 				_GLOBAL_twg_parent_pid				= 0;
	pid_t 				_GLOBAL_twg_child_own_pid			= 0;
	long int			_GLOBAL_log_children_number 			= 0;
	long int			_GLOBAL_awizo_children_number 			= 0;
	long int 			_GLOBAL_ikp_children_number			= 0;
	pid_t				_GLOBAL_log_twg_parent_pid			= 0;
	pid_t				_GLOBAL_awizo_twg_parent_pid			= 0;
	pid_t				_GLOBAL_ikp_twg_parent_pid			= 0;
	pid_t				_GLOBAL_log_twg_child_own_pid			= 0;
	pid_t				_GLOBAL_awizo_twg_child_own_pid			= 0;
	pid_t				_GLOBAL_ikp_twg_child_own_pid			= 0;
//	customers_t 			_GLOBAL_customer;
//	forms_t 			_GLOBAL_form;
//	correspondence_t 		_GLOBAL_correspondence;
//	daexport_options_t 		_GLOBAL_options;
	configuration_t			_GLOBAL_awizoConfig;
	configuration_t			_GLOBAL_awizoBrokerConfig;
	queue_t				_GLOBAL_myCache;
	queue_t 			_GLOBAL_myXmlQueue;
	shRegion_t 			_GLOBAL_shared;
	shRegion_t*			_GLOBAL_shptr 					= NULL;
	volatile sig_atomic_t		_GLOBAL_awizoAction				= 0;
	sigset_t			_GLOBAL_mask_set;
	long				_GLOBAL_awizo_restart_db_connection		= 0;


	long				_GLOBAL_soap_wssecurity_enabled			= 0;
	long				_GLOBAL_soap_certificate_required		= 0;

	long					_GLOBAL_bmdSoapServersCount		= 0;
//	BmdSoapServerInfo_t*	_GLOBAL_bmdSoapServersArray		= NULL;
	char*					_GLOBAL_temporary_directory		= NULL;

	long					_GLOBAL_jvm_initialized			= 0;

	/*<asynchronous timestamping>*/
	long					_GLOBAL_markedWaitingForOthers	= 0;
	long					_GLOBAL_contextId				= 0;
	void*					_GLOBAL_SHMEM_asyncTs			= NULL;
	int						_GLOBAL_shmemId					= 0;
	key_t					_GLOBAL_semaphore_key			= 0;
	int						_GLOBAL_semaphoreId				= 0;
	long					_GLOBAL_restartDbConnection		= 0;
	/*</asynchronous timestamping>*/

	/* <lobRemover> */
	void*					_GLOBAL_SHMEM_lobRemover		= NULL;
	int					_GLOBAL_shmemId_lobRemover		= 0;
	key_t					_GLOBAL_semaphore_key_lobRemover	= 0;
	int					_GLOBAL_semaphoreId_lobRemover		= 0;
	/* </lobRemover> */

#endif /*ifndef WIN32*/


#ifdef WIN32

	HANDLE*				_GLOBAL_lock_cs;
	bmd_svc_info_t*			_GLOBAL_svc_info				= NULL;
	char*				_GLOBAL_svc_name				= NULL;
	char*				_GLOBAL_bmd_conf_file				= NULL;
	SERVICE_STATUS_HANDLE		_GLOBAL_svc_sh;
	HANDLE				_GLOBAL_svc_se;

#endif

