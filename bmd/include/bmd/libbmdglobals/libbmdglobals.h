#ifndef __LIBBMDGLOBALS_H__
#define __LIBBMDGLOBALS_H__


#ifdef WIN32
	#ifdef LIBBMDGLOBALS_EXPORTS
		#define LIBBMDGLOBALS_SCOPE __declspec(dllexport)
	#else
		#define LIBBMDGLOBALS_SCOPE __declspec(dllimport)
	#endif
#else
	#define LIBBMDGLOBALS_SCOPE extern
#endif /* ifdef WIN32 */


#ifndef WIN32

#include <bmd/libbmdlog/libbmdlog.h>
#include <bmd/libbmdlog/bmdlogserver/bmdlogserver.h>
#include <bmd/libbmdlog/root_server/root_server.h>
#include <bmd/mail_sender/configuration.h>
#include <bmd/mail_sender/queue.h>
#include <bmd/mail_sender/shmem.h>
#include <sys/ipc.h>

#endif /*ifndef WIN32*/

#include <bmd/common/bmd-sds.h>
#include <bmd/libbmdsql/data_dict.h>
#include <bmd/libbmddb/bmd_db_functions.h>
#include <bmd/common/global_types.h>

#ifdef WIN32
#include <bmd/bmd_server/bmd_server.h>
#include <stdio.h>
#endif /*ifdef WIN32*/



#define OVECTOR_NUM 200
	LIBBMDGLOBALS_SCOPE	long				_GLOBAL_debug_level;
	LIBBMDGLOBALS_SCOPE long				_GLOBAL_debug_level_error;
	LIBBMDGLOBALS_SCOPE long				_GLOBAL_debug_level_deny;
	LIBBMDGLOBALS_SCOPE long				_GLOBAL_debug_level_ok;
	LIBBMDGLOBALS_SCOPE long				_GLOBAL_debug_level_try;
	LIBBMDGLOBALS_SCOPE long				_GLOBAL_debug_level_debug;
	LIBBMDGLOBALS_SCOPE FILE*				_GLOBAL_win32_log_file;

	LIBBMDGLOBALS_SCOPE server_configuration_t*			_GLOBAL_bmd_configuration;
	LIBBMDGLOBALS_SCOPE plugin_std_messaging_configuration_t*	_GLOBAL_std_messaging_configuration;
	LIBBMDGLOBALS_SCOPE plugin_std_conservate_configuration_t*	_GLOBAL_std_conservate_configuration;
	LIBBMDGLOBALS_SCOPE plugin_std_archiving_configuration_t*	_GLOBAL_std_archiving_configuration;
	LIBBMDGLOBALS_SCOPE plugin_std_session_configuration_t*		_GLOBAL_std_session_configuration;

	LIBBMDGLOBALS_SCOPE struct groups_graph_dict*	_GLOBAL_dict_groups_graph;
	LIBBMDGLOBALS_SCOPE dictionary_data_t*		_GLOBAL_UsersDictionary;
	LIBBMDGLOBALS_SCOPE dictionary_data_t*		_GLOBAL_GroupsDictionary;
	LIBBMDGLOBALS_SCOPE dictionary_data_t*		_GLOBAL_ClassesDictionary;
	LIBBMDGLOBALS_SCOPE dictionary_data_t*		_GLOBAL_GroupsGraphDictionary;
	LIBBMDGLOBALS_SCOPE dictionary_data_t*		_GLOBAL_RolesDictionary;
	LIBBMDGLOBALS_SCOPE dictionary_data_t*		_GLOBAL_UserRolesDictionary;
	LIBBMDGLOBALS_SCOPE dictionary_data_t*		_GLOBAL_RolesAndActionsDictionary;
	LIBBMDGLOBALS_SCOPE dictionary_data_t*		_GLOBAL_ActionsDictionary;
	LIBBMDGLOBALS_SCOPE dictionary_data_t*		_GLOBAL_SecurityDictionary;
	LIBBMDGLOBALS_SCOPE dictionary_data_t*		_GLOBAL_AddMetadataTypesDictionary;
	LIBBMDGLOBALS_SCOPE dictionary_data_t*		_GLOBAL_RoleRightsStructureDictionary;
	LIBBMDGLOBALS_SCOPE dictionary_data_t*		_GLOBAL_RoleRightsDictionary;
	LIBBMDGLOBALS_SCOPE dictionary_data_t*		_GLOBAL_SecurityCategoriesDictionary;
	LIBBMDGLOBALS_SCOPE dictionary_data_t*		_GLOBAL_SecurityLevelsDictionary;
	LIBBMDGLOBALS_SCOPE dictionary_data_t*		_GLOBAL_UserGroupsDictionary;
	LIBBMDGLOBALS_SCOPE dictionary_data_t*		_GLOBAL_UserClassesDictionary;
	LIBBMDGLOBALS_SCOPE dictionary_data_t*		_GLOBAL_SecurityCategoriesGraphDictionary;
	LIBBMDGLOBALS_SCOPE dictionary_data_t*		_GLOBAL_MetadataValuesDictionary;

	LIBBMDGLOBALS_SCOPE char				_GLOBAL_exbuff[1000000];
	LIBBMDGLOBALS_SCOPE long				_GLOBAL_libbmddb_initialized;

	LIBBMDGLOBALS_SCOPE bmd_db_fun_table_t*		_GLOBAL_f_tbl;

	LIBBMDGLOBALS_SCOPE bmd_crypt_ctx_t*			_GLOBAL_ctx;
	LIBBMDGLOBALS_SCOPE void*				_GLOBAL_CallBackParent;

	LIBBMDGLOBALS_SCOPE int 			_CONST_CHARS_PER_LINE;

	LIBBMDGLOBALS_SCOPE bmd_conf*			_GLOBAL_StdSessionSOE_conf;
	LIBBMDGLOBALS_SCOPE bmd_conf*			_GLOBAL_AdviceSOE_conf;
	LIBBMDGLOBALS_SCOPE bmd_conf*			_GLOBAL_StdClear_conf;
	LIBBMDGLOBALS_SCOPE bmd_conf*			_GLOBAL_StdConservate_conf;
	LIBBMDGLOBALS_SCOPE bmd_conf*			_GLOBAL_StdDVCS_conf;
	LIBBMDGLOBALS_SCOPE bmd_conf*			_GLOBAL_StdAuth_conf;
	LIBBMDGLOBALS_SCOPE bmd_conf*			_GLOBAL_StdMessaging_conf;


	LIBBMDGLOBALS_SCOPE server_request_data_t*		_GLOBAL_server_request_data;
	LIBBMDGLOBALS_SCOPE long				_GLOBAL_interrupt;
	LIBBMDGLOBALS_SCOPE long 				_GLOBAL_can_close_server;
	LIBBMDGLOBALS_SCOPE bmd_conf*			_GLOBAL_konfiguracja;
	LIBBMDGLOBALS_SCOPE bmd_conf*			_GLOBAL_logSoapKonfiguracja;
	LIBBMDGLOBALS_SCOPE bmd_conf*			_GLOBAL_awizoSoapKonfiguracja;
	LIBBMDGLOBALS_SCOPE bmd_conf*			_GLOBAL_ikpSoapKonfiguracja;

	LIBBMDGLOBALS_SCOPE long				_GLOBAL_ikpSoap_enable_wssecurity;
	LIBBMDGLOBALS_SCOPE long				_GLOBAL_ikpSoap_secure_response;
	LIBBMDGLOBALS_SCOPE char*				_GLOBAL_ikpSoap_wsse_key;
	LIBBMDGLOBALS_SCOPE char*				_GLOBAL_ikpSoap_wsse_key_password;
	LIBBMDGLOBALS_SCOPE char*				_GLOBAL_ikpSoap_wsse_certificate;



	LIBBMDGLOBALS_SCOPE char 				_GLOBAL_current_dir[256];
	LIBBMDGLOBALS_SCOPE char*				_GLOBAL_twg_transaction_dir_name;

	LIBBMDGLOBALS_SCOPE bmd_info_t*			_GLOBAL_bmd_connection;

#ifndef WIN32


	LIBBMDGLOBALS_SCOPE long				_GLOBAL_debug_level;
    LIBBMDGLOBALS_SCOPE char*					_GLOBAL_response;
	LIBBMDGLOBALS_SCOPE long					_GLOBAL_index2;
	LIBBMDGLOBALS_SCOPE log_t*				_GLOBAL_bmdlog_HANDLER;
	LIBBMDGLOBALS_SCOPE log_t*				_GLOBAL_soaplog_HANDLER;

	LIBBMDGLOBALS_SCOPE int				_GLOBAL_bmdth_debug_mode;
	LIBBMDGLOBALS_SCOPE int 				_GLOBAL_thread_model_type;
	LIBBMDGLOBALS_SCOPE long int 			_GLOBAL_children_number;
	LIBBMDGLOBALS_SCOPE long int 			_GLOBAL_log_children_number;
	LIBBMDGLOBALS_SCOPE long int 			_GLOBAL_awizo_children_number;
	LIBBMDGLOBALS_SCOPE long int 			_GLOBAL_ikp_children_number;



	LIBBMDGLOBALS_SCOPE pid_t 				_GLOBAL_child;

	LIBBMDGLOBALS_SCOPE volatile sig_atomic_t		_GLOBAL_action;
	LIBBMDGLOBALS_SCOPE bmdlogServParam_t		_GLOBAL_bmdlogServParam_tParams;
	LIBBMDGLOBALS_SCOPE rootServParam_t   		_GLOBAL_rootServParam_tParams;

	LIBBMDGLOBALS_SCOPE pid_t 				_GLOBAL_twg_parent_pid;
	LIBBMDGLOBALS_SCOPE pid_t 				_GLOBAL_log_twg_parent_pid;
	LIBBMDGLOBALS_SCOPE pid_t 				_GLOBAL_awizo_twg_parent_pid;
	LIBBMDGLOBALS_SCOPE pid_t 				_GLOBAL_ikp_twg_parent_pid;


	LIBBMDGLOBALS_SCOPE pid_t				_GLOBAL_twg_child_own_pid;

	LIBBMDGLOBALS_SCOPE pid_t				_GLOBAL_log_twg_child_own_pid;
	LIBBMDGLOBALS_SCOPE pid_t				_GLOBAL_awizo_twg_child_own_pid;
	LIBBMDGLOBALS_SCOPE pid_t				_GLOBAL_ikp_twg_child_own_pid;
	
	
	
	LIBBMDGLOBALS_SCOPE configuration_t 			_GLOBAL_awizoConfig;
	LIBBMDGLOBALS_SCOPE configuration_t 			_GLOBAL_awizoBrokerConfig;
	LIBBMDGLOBALS_SCOPE queue_t 				_GLOBAL_myCache;
	LIBBMDGLOBALS_SCOPE queue_t 				_GLOBAL_myXmlQueue;
	LIBBMDGLOBALS_SCOPE shRegion_t 				_GLOBAL_shared;
	LIBBMDGLOBALS_SCOPE shRegion_t*				_GLOBAL_shptr;
	LIBBMDGLOBALS_SCOPE volatile sig_atomic_t		_GLOBAL_awizoAction;
	LIBBMDGLOBALS_SCOPE sigset_t 				_GLOBAL_mask_set;
	LIBBMDGLOBALS_SCOPE long					_GLOBAL_awizo_restart_db_connection;

	LIBBMDGLOBALS_SCOPE long				_GLOBAL_soap_wssecurity_enabled;
	LIBBMDGLOBALS_SCOPE long				_GLOBAL_soap_certificate_required;

	LIBBMDGLOBALS_SCOPE long					_GLOBAL_bmdSoapServersCount;
	
	LIBBMDGLOBALS_SCOPE char*					_GLOBAL_temporary_directory;

	LIBBMDGLOBALS_SCOPE long					_GLOBAL_jvm_initialized;

	/*<asynchronous timestamping>*/
	LIBBMDGLOBALS_SCOPE	long					_GLOBAL_markedWaitingForOthers;
	LIBBMDGLOBALS_SCOPE	long					_GLOBAL_contextId;
	LIBBMDGLOBALS_SCOPE	void*					_GLOBAL_SHMEM_asyncTs;
	LIBBMDGLOBALS_SCOPE int						_GLOBAL_shmemId;
	LIBBMDGLOBALS_SCOPE key_t					_GLOBAL_semaphore_key;
	LIBBMDGLOBALS_SCOPE int						_GLOBAL_semaphoreId;
	LIBBMDGLOBALS_SCOPE long					_GLOBAL_restartDbConnection;
	/*</asynchronous timestamping>*/

	/* <lobRemover> */
	LIBBMDGLOBALS_SCOPE void*				_GLOBAL_SHMEM_lobRemover;
	LIBBMDGLOBALS_SCOPE int					_GLOBAL_shmemId_lobRemover;
	LIBBMDGLOBALS_SCOPE key_t				_GLOBAL_semaphore_key_lobRemover;
	LIBBMDGLOBALS_SCOPE int					_GLOBAL_semaphoreId_lobRemover;
	/* </lobRemover> */

#endif /*ifndef WIN32*/


#ifdef WIN32

	LIBBMDGLOBALS_SCOPE HANDLE*				_GLOBAL_lock_cs;
	LIBBMDGLOBALS_SCOPE bmd_svc_info_t* 			_GLOBAL_svc_info;
	LIBBMDGLOBALS_SCOPE char*				_GLOBAL_svc_name;
	LIBBMDGLOBALS_SCOPE char*				_GLOBAL_bmd_conf_file;
	LIBBMDGLOBALS_SCOPE SERVICE_STATUS_HANDLE		_GLOBAL_svc_sh;
	LIBBMDGLOBALS_SCOPE HANDLE				_GLOBAL_svc_se;

#endif /*ifdef WIN32*/

	LIBBMDGLOBALS_SCOPE dictionary_data_t *getGlobalGroupsDictionary(void);
	LIBBMDGLOBALS_SCOPE dictionary_data_t *getGlobalUserGroupsDictionary(void);

	LIBBMDGLOBALS_SCOPE long mask_user_action_with_roles_and_actions(		long **actions, long *no_of_ations, GenBuf_t *user_chosen_role);

	LIBBMDGLOBALS_SCOPE long initSingleDataDictionary(		dictionary_data_t **dictionary, char *SQlQuery);

	LIBBMDGLOBALS_SCOPE long freeAllDataDictionary();
	LIBBMDGLOBALS_SCOPE long freeSingleDataDictionary(		dictionary_data_t **dictionary);
	LIBBMDGLOBALS_SCOPE long printSingleDataDictionary(		dictionary_data_t *dictionary);

	LIBBMDGLOBALS_SCOPE long getColumnWithCondition(	dictionary_data_t *dictionary,
							long conditionColumnNumber,
							char *conditionValue,
							long ansColumnNumber,
							long *anssCount,
							char ***anss);

	LIBBMDGLOBALS_SCOPE long getElementWithCondition(	dictionary_data_t *dictionary,
							long conditionColumnNumber,
							char *conditionValue,
							long ansColumnNumber,
							char **ans);

	LIBBMDGLOBALS_SCOPE long getRowWithCondition(	dictionary_data_t *dictionary,
							long conditionColumnNumber,
							char *conditionValue,
							long *ansColIndexs,
							long ansColNumber,
							long *anssCount,
							char ***anss);

	LIBBMDGLOBALS_SCOPE long getRowIndexWithCondition(	dictionary_data_t *dictionary,
								long conditionColumnNumber,
								char *conditionValue,
								long *ansIndex);

	LIBBMDGLOBALS_SCOPE long getCountWithCondition(	dictionary_data_t *dictionary,
							long conditionColumnNumber,
							char *conditionValue,
							long *ansCount);

	LIBBMDGLOBALS_SCOPE long DatabaseDictionaryTypeToID(	char *OID,
								char *table_name,
								char **id);

	LIBBMDGLOBALS_SCOPE long DatabaseDictionaryTypeToSQLCastString(	char *OID,
									char *table_name,
									char **sql_cast_string);

	LIBBMDGLOBALS_SCOPE long DatabaseDictionaryTypeToName(	char *OID,
								char **metadataName);



#endif /*ifndef __LIBBMDGLOBALS_H__*/
