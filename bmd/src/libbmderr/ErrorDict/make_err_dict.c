#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmddb/libbmddb.h>
#include <bmd/libbmdcms/libbmdcms.h>
#include <stdio.h>

struct error
{
	long MeassageId_Unix;
	long MeassageId;
	char *Severity;
	char *Facility;
	char *SymbolicName;
	char *Language;
	char *msg;
};

struct error errors[] =
{{BMD_VERIFY_REQUEST_FILE_PARSE_ERROR, 					1, "Error", "System", "BMD_VERIFY_REQUEST_FILE_PARSE_ERROR", "English", "Error in parsing kontrolki file"},
{BMD_VERIFY_REQUEST_GET_CLEARANCE_ERROR , 				2, "Error", "System", "BMD_VERIFY_REQUEST_GET_CLEARANCE_ERROR", "English", "Error in getting clients clearance from attribute certificate"},
{BMD_VERIFY_REQUEST_INVALID_FILENAME , 					3, "Error", "System", "BMD_VERIFY_REQUEST_INVALID_FILENAME", "English", "Filename given in protocol is incorrect"},
{BMD_VERIFY_REQUEST_INVALID_ADD_METADATA , 				4, "Error", "System", "BMD_VERIFY_REQUEST_INVALID_ADD_METADATA", "English", "Invalid additional metadata value"},
{BMD_VERIFY_REQUEST_INVALID_USR_METADATA , 				5, "Error", "System", "BMD_VERIFY_REQUEST_INVALID_USR_METADATA", "English", "Invalid user metadata value"},
{BMD_VERIFY_REQUEST_INVALID_SYS_METADATA , 				6, "Error", "System", "BMD_VERIFY_REQUEST_INVALID_SYS_METADATA", "English", "Invalid system metadata value"},
{BMD_VERIFY_REQUEST_INVALID_ROLE , 						7, "Error", "System", "BMD_VERIFY_REQUEST_INVALID_ROLE", "English", "Invalid role given in protocol"},
{BMD_VERIFY_REQUEST_INVALID_ADD_METADATA_OID , 				8, "Error", "System", "BMD_VERIFY_REQUEST_INVALID_ADD_METADATA_OID", "English", "Invalid oid of additional metadata"},
{BMD_VERIFY_REQUEST_INVALID_USR_METADATA_OID ,				9, "Error", "System", "BMD_VERIFY_REQUEST_INVALID_USR_METADATA_OID", "English", "Invalid oid of user metadata"},
{BMD_VERIFY_REQUEST_INVALID_SYS_METADATA_OID , 				10, "Error", "System", "BMD_VERIFY_REQUEST_INVALID_SYS_METADATA_OID", "English", "Invalid oid of system metadata"},
{BMD_VERIFY_REQUEST_CHECK_CLEARANCE_ERROR , 				11, "Error", "System", "BMD_VERIFY_REQUEST_CHECK_CLEARANCE_ERROR", "English", "Error in checking clients clearence"},
{BMD_VERIFY_REQUEST_INVALID_CLEARANCE_FORMAT , 				12, "Error", "System", "BMD_VERIFY_REQUEST_INVALID_CLEARANCE_FORMAT", "English", "Invalid clients clearence format"},
{BMD_VERIFY_REQUEST_GET_GROUPS_ERROR , 					13, "Error", "System", "BMD_VERIFY_REQUEST_GET_GROUPS_ERROR", "English", "Error in getting group info from attribute certificate"},
{BMD_VERIFY_REQUEST_INVALID_SEC_CLEARANCE , 				14, "Error", "System", "BMD_VERIFY_REQUEST_INVALID_SEC_CLEARANCE", "English", "Invalid clearance level to perform action with given resource"},
{BMD_VERIFY_LOGIN_REQUEST_NO_GROUPS_IN_AC , 				15, "Error", "System", "BMD_VERIFY_LOGIN_REQUEST_NO_GROUPS_IN_AC", "English", "No group specified in clients attribute certificate"},
{BMD_VERIFY_LOGIN_REQUEST_NO_GROUPS_IN_USER_FORM , 			16, "Error", "System", "BMD_VERIFY_LOGIN_REQUEST_NO_GROUPS_IN_USER_FORM", "English", "No group specified in user form"},
{BMD_VERIFY_LOGIN_REQUEST_INVALID_GROUP_SOURCE_IN_USER_FORM , 	17, "Error", "System", "BMD_VERIFY_LOGIN_REQUEST_INVALID_GROUP_SOURCE_IN_USER_FORM", "English", "Invalid group source in user form (proper BMD))"},
{BMD_VERIFY_REQUEST_METADATA_NOT_UPDATEABLE ,			 	18, "Error", "System", "BMD_VERIFY_REQUEST_METADATA_NOT_UPDATEABLE", "English", "Metadata not updateable according to user form or role rights"},
{BMD_VERIFY_REQUEST_METADATA_NOT_UPDATEABLE_ROLE_RIGHTS, 		19, "Error", "System", "BMD_VERIFY_REQUEST_METADATA_NOT_UPDATEABLE_ROLE_RIGHTS", "English", "User role does not allow metadata update (role_rights table)"},
{BMD_VERIFY_REQUEST_INVALID_ADD_METADATA_REGEXP, 			19, "Error", "System", "BMD_VERIFY_REQUEST_INVALID_ADD_METADATA_REGEXP", "English", "Metadata value incorrect according to regular expresion in user form"},
{BMD_VERIFY_REQUEST_METADATA_NOT_HISTORYABLE ,			 	18, "Error", "System", "BMD_VERIFY_REQUEST_METADATA_NOT_HISTORYABLE", "English", "Metadata not historyable according to user form or role rights"},
{BMD_VERIFY_REQUEST_SEARCH_TOO_MANY_OPEN_BRACKETS ,			18, "Error", "System", "BMD_VERIFY_REQUEST_SEARCH_TOO_MANY_OPEN_BRACKETS", "English", "Search request query syntax error - too many openning brackets in search request"},
{BMD_VERIFY_REQUEST_SEARCH_TOO_MANY_CLOSE_BRACKETS ,			18, "Error", "System", "BMD_VERIFY_REQUEST_SEARCH_TOO_MANY_CLOSE_BRACKETS", "English", "Search request query syntax error - too many closing brackets in search request"},
{BMD_VERIFY_REQUEST_SEARCH_SYNTAX_LOGICAL_INVALID_POS ,		18, "Error", "System", "BMD_VERIFY_REQUEST_SEARCH_SYNTAX_LOGICAL_INVALID_POS", "English", "Search request query syntax error - logical operator in invalid position"},
{BMD_VERIFY_REQUEST_SERVER_CONF_UPDATE ,					18, "Error", "System", "BMD_VERIFY_REQUEST_SERVER_CONF_UPDATE", "English", "Server configuration being updated. Please retry later"},
{BMD_VERIFY_REQUEST_SEARCH_IMPERMISSIBLE_SORT_METADATA ,	18, "Error", "System", "BMD_VERIFY_REQUEST_SEARCH_IMPERMISSIBLE_SORT_METADATA", "English", "Impermissible metadata oid was detected in sort criteria"},
{BMD_VERIFY_REQUEST_SEARCH_INVALID_SORT_ORDER ,				18, "Error", "System", "BMD_VERIFY_REQUEST_SEARCH_INVALID_SORT_ORDER", "English", "Invalid sort order value was detected in sort criteria"},
{BMD_VERIFY_REQUEST_INCORRECT_SORT_CRITERIA ,				18, "Error", "System", "BMD_VERIFY_REQUEST_INCORRECT_SORT_CRITERIA", "English", "Sort criteria are incorrect"},
{BMD_VERIFY_REQUEST_NO_ADD_METADATA_SPECIFIED ,				18, "Error", "System", "BMD_VERIFY_REQUEST_NO_ADD_METADATA_SPECIFIED", "English", "No additional metadata is specified"},
{BMD_VERIFY_REQUEST_AMBIGUOUS_CRITERIA ,					18, "Error", "System", "BMD_VERIFY_REQUEST_AMBIGUOUS_CRITERIA", "English", "Imposible to find unique document according to given criteria"},




/******************************************************************/
/*	bledy weryfikacji certyfikatu logujacego za pomoca pluginu	*/
/******************************************************************/
{BMD_VERIFY_LOGIN_CERTIFICATE_CERT_STORE_DIR_ERROR , 			18, "Error", "System", "BMD_VERIFY_LOGIN_CERTIFICATE_CERT_STORE_DIR_ERROR", "English", "Error in reading from servers cert_store directory"},
{BMD_VERIFY_LOGIN_CERTIFICATE_NO_CERTS_IN_CERT_STORE , 		19, "Error", "System", "BMD_VERIFY_LOGIN_CERTIFICATE_NO_CERTS_IN_CERT_STORE", "English", "No certs found in servers cert_store directory"},
{BMD_VERIFY_LOGIN_CERTIFICATE_DECODE_ERROR , 				20, "Error", "System", "BMD_VERIFY_LOGIN_CERTIFICATE_DECODE_ERROR", "English", "Error in decoding login certificate"},
{BMD_VERIFY_LOGIN_CERTIFICATE_INVALID_FORMAT , 				21, "Error", "System", "BMD_VERIFY_LOGIN_CERTIFICATE_INVALID_FORMAT", "English", "Login certificate invalid format"},
{BMD_VERIFY_LOGIN_CERTIFICATE_LOAD_CERT_ERROR , 			22, "Error", "System", "BMD_VERIFY_LOGIN_CERTIFICATE_LOAD_CERT_ERROR", "English", "Error in loading certificate path certs file"},
{BMD_VERIFY_LOGIN_CERTIFICATE_WITH_CERT_ERROR , 			23, "Error", "System", "BMD_VERIFY_LOGIN_CERTIFICATE_WITH_CERT_ERROR", "English", "Error in verifying login cert with certificate path cert"},
{BMD_VERIFY_LOGIN_CERTIFICATE_CERT_PATH , 				24, "Error", "System", "BMD_VERIFY_LOGIN_CERTIFICATE_CERT_PATH", "English", "Error in verifying login certs cert path"},

{BMD_VERIFY_LOGIN_CERTIFICATE_CRL_EXT_ABSENT , 				25, "Error", "System", "BMD_VERIFY_LOGIN_CERTIFICATE_CRL_EXT_ABSENT", "English", "Extension absent in servers crl list"},
{BMD_VERIFY_LOGIN_CERTIFICATE_LOAD_CRL_ERROR , 				26, "Error", "System", "BMD_VERIFY_LOGIN_CERTIFICATE_LOAD_CRL_ERROR", "English", "Error in loading servers crl list file"},
{BMD_VERIFY_LOGIN_CERTIFICATE_NO_CRL_IN_FILE , 				27, "Error", "System", "BMD_VERIFY_LOGIN_CERTIFICATE_NO_CRL_IN_FILE", "English", "No crl cert in servers crl list file"},
{BMD_VERIFY_LOGIN_CERTIFICATE_CRL_DECODE_ERROR , 			28, "Error", "System", "BMD_VERIFY_LOGIN_CERTIFICATE_CRL_DECODE_ERROR", "English", "Error in decoding servers crl list certificate"},
{BMD_VERIFY_LOGIN_CERTIFICATE_ON_CRL , 					29, "Error", "System", "BMD_VERIFY_LOGIN_CERTIFICATE_ON_CRL", "English",  "Login certificate invalid - present on crl list"},
{BMD_VERIFY_LOGIN_CERTIFICATE_OUT_OF_DATE , 				30, "Error", "System", "BMD_VERIFY_LOGIN_CERTIFICATE_OUT_OF_DATE", "English", "Login certificate invalid - out of date"},

{BMD_VERIFY_LOGIN_CERTIFICATE_LOAD_ACL_ERROR , 				31, "Error", "System", "BMD_VERIFY_LOGIN_CERTIFICATE_LOAD_ACL_ERROR", "English", "Error in loading servers acl file"},
{BMD_VERIFY_LOGIN_CERTIFICATE_ACL_INVALID_FORMAT ,			32, "Error", "System", "BMD_VERIFY_LOGIN_CERTIFICATE_ACL_INVALID_FORMAT", "English", "Invalid format of servers acl file"},
{BMD_VERIFY_LOGIN_CERTIFICATE_FAILURE , 					33, "Error", "System", "BMD_VERIFY_LOGIN_CERTIFICATE_FAILURE", "English", "Login certificates serial number not found in serial_acl file"},
{BMD_VERIFY_LOGIN_CERTIFICATE_NOT_FOND_IN_ACL_FILE ,			33, "Error", "System", "BMD_VERIFY_LOGIN_CERTIFICATE_NOT_FOND_IN_ACL_FILE", "English", "Login certificates serial number not found in serial_acl file"},
{BMD_VERIFY_LOGIN_CERTIFICATE_NOT_FOND_IN_ACL_DICT ,			33, "Error", "System", "BMD_VERIFY_LOGIN_CERTIFICATE_NOT_FOND_IN_ACL_DICT", "English", "Login certificates serial number not found in serials dictionary"},
{BMD_VERIFY_LOGIN_CERTIFICATE_NOT_ACCEPTED_IN_ACL_DICT ,		33, "Error", "System", "BMD_VERIFY_LOGIN_CERTIFICATE_NOT_ACCEPTED_IN_ACL_DICT", "English", "Login certificate is not accepted during to serials dictionary"},

/************************************************/
/*	bledy weryfikacji poprawnosci datagramu	*/
/************************************************/
{NO_USR_METADATA_PRESENT , 							39, "Error", "System", "NO_USR_METADATA_PRESENT", "English", "No user metadata present"},
{NO_SYS_METADATA_PRESENT , 							40, "Error", "System", "NO_SYS_METADATA_PRESENT", "English", "No system metadata present"},
{NO_PKI_METADATA_PRESENT , 							41, "Error", "System", "NO_PKI_METADATA_PRESENT", "English", "No pki metadata present"},
{INVALID_ADDITIONAL_METADATA_OID_FORMAT , 				42, "Error", "System", "INVALID_ADDITIONAL_METADATA_OID_FORMAT", "English", "Invalid additional metadata oid format"},
{INVALID_SYSTEM_METADATA_OID_FORMAT , 					43, "Error", "System", "INVALID_SYSTEM_METADATA_OID_FORMAT", "English", "Invalid system metadata oid format"},
{INVALID_USER_METADATA_OID_FORMAT , 					44, "Error", "System", "INVALID_USER_METADATA_OID_FORMAT", "English", "Invalid user metadata oid format"},
{INVALID_PKI_METADATA_OID_FORMAT , 						45, "Error", "System", "INVALID_PKI_METADATA_OID_FORMAT", "English", "Invalid pki metadata oid format"},
{BMD_ERR_OID_NOT_FOUND , 							45, "Error", "System", "BMD_ERR_OID_NOT_FOUND", "English", "Metadata with specified oid not found in datagram"},
{UNKNOWN_METADATA_TYPE,								46, "Error", "System", "UNKNOWN_METADATA_TYPE", "English", "Unknown metadata type"},
{ACTION_METADATA_USER_CLASS_NOT_SPECIFIED,					46, "Error", "System", "ACTION_METADATA_USER_CLASS_NOT_SPECIFIED", "English", "One and only one class id (action metadata) must be specified"},
{ACTION_METADATA_ACTION_NOT_SPECIFIED,						46, "Error", "System", "ACTION_METADATA_ACTION_NOT_SPECIFIED", "English", "At least one action id (action metadata) must be specified"},
{INVALID_PKI_METADATA_HASH_ALGORITHM_OID_FORMAT,	46, "Error", "System", "INVALID_PKI_METADATA_HASH_ALGORITHM_OID_FORMAT", "English", "Incorrect format of hash algorithm oid"},
{INVALID_PKI_METADATA_HEX_HASH_VALUE_FORMAT,		46, "Error", "System", "INVALID_PKI_METADATA_HEX_HASH_VALUE_FORMAT", "English", "Incorrect format of hexadecimal hash value"},
{HEX_TO_BINARY_CONVERTION_ERROR,					46, "Error", "System", "HEX_TO_BINARY_CONVERTION_ERROR", "English", "Unable to convert hexadecimal to binary. Is hexadecimal format correct?"},
{BMD_ERR_OPEN_FILE,									46, "Error", "System", "BMD_ERR_OPEN_FILE", "English", "Unable to open file"},




{BMD_LOGIN_NO_CLIENT_HAS_NO_GROUP , 					46, "Error", "System", "BMD_LOGIN_NO_CLIENT_HAS_NO_GROUP", "English", "Clients has no goup specified"},

{BMD_CLIENT_ROLE_NOT_FOUND , 							47, "Error", "System", "BMD_CLIENT_ROLE_NOT_FOUND", "English", "Clients role not found in dictionary"},
{BMD_CLIENT_ROLE_NOT_PERMIT_LOGIN_FOR_SOMEBODY , 			48, "Error", "System", "BMD_CLIENT_ROLE_NOT_PERMIT_LOGIN_FOR_SOMEBODY", "English", "Clients role does not permit login for somebody"},
{BMD_CLIENT_ROLE_NOT_PERMIT_ACTION , 					49, "Error", "System", "BMD_CLIENT_ROLE_NOT_PERMIT_ACTION", "English", "Clients role does not permit action"},
{BMD_CLIENT_NOT_FOUND_IN_ATTRIBUTES , 					50, "Error", "System", "BMD_CLIENT_NOT_FOUND_IN_ATTRIBUTES", "English", "Clients certificate not found in attributes.db"},
{BMD_CLIENT_CLEARANCE_NOT_FOUND_IN_ATTRIBUTES , 			51, "Error", "System", "BMD_CLIENT_CLEARANCE_NOT_FOUND_IN_ATTRIBUTES", "English", "Clients clearance not found in attributes.db"},

{BMD_ROLE_RIGHTS_INCOMPLETE_RECORD , 					51, "Error", "System", "BMD_ROLE_RIGHTS_INCOMPLETE_RECORD", "English", "Record according to metadata missing in role_rights table"},
{BMD_ROLE_RIGHTS_INVALID_RECORD_LENGTH , 					51, "Error", "System", "BMD_ROLE_RIGHTS_INVALID_RECORD_LENGTH", "English", "Invalid format of record according to metadata in role_rights"},

/************************/
/*	bledy bazy danych	*/
/************************/
{BMD_DB_INVALID_CONNECTION_HANDLE , 					47, "Error", "System", "BMD_DB_INVALID_CONNECTION_HANDLE", "English", "Database connection handle invalid"},
{BMD_DB_SERVER_ADDRESS_NOT_SPECIFIED , 					47, "Error", "System", "BMD_DB_SERVER_ADDRESS_NOT_SPECIFIED", "English", "Database server address not specified"},
{BMD_DB_SERVER_PORT_NOT_SPECIFIED , 					48, "Error", "System", "BMD_DB_SERVER_PORT_NOT_SPECIFIED", "English", "Database server port not specified"},
{BMD_DB_NAME_NOT_SPECIFIED , 							49, "Error", "System", "BMD_DB_NAME_NOT_SPECIFIED", "English", "Database name not specified"},
{BMD_DB_USER_NAME_NOT_SPECIFIED , 						50, "Error", "System", "BMD_DB_USER_NAME_NOT_SPECIFIED", "English", "Database user name not specified"},
{BMD_DB_USER_PASSWORD_NOT_SPECIFIED , 					51, "Error", "System", "BMD_DB_USER_PASSWORD_NOT_SPECIFIED", "English", "Database user password not specified"},
{BMD_DB_COMMAND_NOT_SPECIFIED , 						51, "Error", "System", "BMD_DB_COMMAND_NOT_SPECIFIED", "English", "Database command not specified"},
{BMD_DB_EXECUTION_ERROR , 								51, "Error", "System", "BMD_DB_EXECUTION_ERROR", "English", "Unable to execute database command"},
{BMD_DB_RECONNECT_ERROR , 								51, "Error", "System", "BMD_DB_RECONNECT_ERROR", "English", "Unable to reconnect to database"},



/* usuwanie pliku */
{BMD_SQL_DELETE_DB_ERROR , 							52, "Error", "System", "BMD_SQL_DELETE_DB_ERROR", "English", "Database error during delete action"},
{BMD_SQL_DELETE_FILE_NOT_FOUND ,						53, "Error", "System", "BMD_SQL_DELETE_FILE_NOT_FOUND", "English", "File to delete not found"},
{BMD_SQL_DELETE_FILE_LOB_NOT_FOUND ,					54, "Error", "System", "BMD_SQL_DELETE_FILE_LOB_NOT_FOUND", "English", "Files to delete lob not found"},
{BMD_SQL_DELETE_SYMKEY_LOB_NOT_FOUND ,					55, "Error", "System", "BMD_SQL_DELETE_SYMKEY_LOB_NOT_FOUND", "English", "Files to delete symkeys lob not found"},
{BMD_SQL_DELETE_SIGNATURE_LOB_NOT_FOUND , 				56, "Error", "System", "BMD_SQL_DELETE_SIGNATURE_LOB_NOT_FOUND", "English", "Files to delete signaturess lob not found"},
{BMD_SQL_DELETE_TIMESTAMP_LOB_NOT_FOUND , 				57, "Error", "System", "BMD_SQL_DELETE_TIMESTAMP_LOB_NOT_FOUND", "English", "Files to delete timestamps lob not found"},

/* pobieranie pliku */
{BMD_SQL_GET_ONLY_FILE_DB_ERROR , 						58, "Error", "System", "BMD_SQL_GET_ONLY_FILE_DB_ERROR", "English", "Database error during get only file action"},
{BMD_SQL_GET_ONLY_FILE_NOT_FOUND_ERROR , 					59, "Error", "System", "BMD_SQL_GET_ONLY_FILE_NOT_FOUND_ERROR", "English", "File to get not found"},
{BMD_SQL_GET_ONLY_FILE_LOB_NOT_FOUND_ERROR , 				60, "Error", "System", "BMD_SQL_GET_ONLY_FILE_LOB_NOT_FOUND_ERROR", "English", "Files to get lob not found"},
{BMD_SQL_GET_ONLY_FILE_SYMKEY_LOB_NOT_FOUND_ERROR , 			61, "Error", "System", "BMD_SQL_GET_ONLY_FILE_SYMKEY_LOB_NOT_FOUND_ERROR", "English", "Files to get symkeys lob not found"},
{BMD_SQL_GET_ONLY_FILE_NO_EINVOICE_FETCHED_MTD , 			61, "Error", "System", "BMD_SQL_GET_ONLY_FILE_NO_EINVOICE_FETCHED_MTD", "English", "Unable to note document fetch - invalid database structure"},

{BMD_SQL_GET_CGI_DB_ERROR , 							62, "Error", "System", "BMD_SQL_GET_CGI_DB_ERROR", "English", "Database error during get file action in CGI mode"},
{BMD_SQL_GET_CGI_NOT_FOUND_ERROR , 						63, "Error", "System", "BMD_SQL_GET_CGI_NOT_FOUND_ERROR", "English", "File to get not found in CGI mode"},
{BMD_SQL_GET_CGI_LOB_NOT_FOUND_ERROR , 					64, "Error", "System", "BMD_SQL_GET_CGI_LOB_NOT_FOUND_ERROR", "English", "Files to get lob not found in CGI mode"},
{BMD_SQL_GET_CGI_SYMKEY_LOB_NOT_FOUND_ERROR , 				65, "Error", "System", "BMD_SQL_GET_CGI_SYMKEY_LOB_NOT_FOUND_ERROR", "English", "Files to get symkeys lob not found in CGI mode"},

/* wyszukiwanie pliku */
{BMD_SQL_SEARCH_DB_ERROR , 							66, "Error", "System", "BMD_SQL_SEARCH_DB_ERROR", "English", "Database error during search action"},
{BMD_SQL_SEARCH_PG_FATAL_ERROR_DB_ERROR , 				67, "Error", "System", "BMD_SQL_SEARCH_PG_FATAL_ERROR_DB_ERROR", "English", "Fatal database error during search action"},
{BMD_SQL_SEARCH_INVALID_QUERY_TYPE ,					68, "Error", "System", "BMD_SQL_SEARCH_INVALID_QUERY_TYPE", "English", "Invalid query type in datagram"},

/* pobieranie metadanych pliku */
{BMD_SQL_GET_METADATA_DB_ERROR , 						69, "Error", "System", "BMD_SQL_GET_METADATA_DB_ERROR", "English", "Database error during get metadata action"},
{BMD_SQL_GET_METADATA_NOT_FOUND_ERROR , 					70, "Error", "System", "BMD_SQL_GET_METADATA_NOT_FOUND_ERROR", "English", "File to get metadata not found"},
{BMD_SQL_GET_METADATA_LOB_NOT_FOUND_ERROR , 				71, "Error", "System", "BMD_SQL_GET_METADATA_LOB_NOT_FOUND_ERROR", "English", "Files to get metadata symkeys lob not found"},

/* pobieranie metadanych pliku CGI*/
{BMD_SQL_GET_METADATA_CGI_DB_ERROR , 					72, "Error", "System", "BMD_SQL_GET_METADATA_CGI_DB_ERROR", "English", "Database error during get metadata action in CGI mode"},
{BMD_SQL_GET_METADATA_CGI_NOT_FOUND_ERROR , 				73, "Error", "System", "BMD_SQL_GET_METADATA_CGI_NOT_FOUND_ERROR", "English", "File to get metadata not found in CGI mode"},
{BMD_SQL_GET_METADATA_CGI_LOB_NOT_FOUND_ERROR , 			74, "Error", "System", "BMD_SQL_GET_METADATA_CGI_LOB_NOT_FOUND_ERROR", "English", "Files to get metadata symkeys lob not found in CGI mode"},

/* weryfikacja podpisu*/
{BMD_SQL_VERIFY_SIGNATURE_DB_ERROR , 					75, "Error", "System", "BMD_SQL_VERIFY_SIGNATURE_DB_ERROR", "English", "Database error during signature verification"},
{BMD_SQL_VERIFY_SIGNATURE_FILE_NOT_FOUND_ERROR , 			76, "Error", "System", "BMD_SQL_VERIFY_SIGNATURE_FILE_NOT_FOUND_ERROR", "English", "File to verify signature not found"},
{BMD_SQL_VERIFY_SIGNATURE_FILE_LOB_NOT_FOUND_ERROR , 			77, "Error", "System", "BMD_SQL_VERIFY_SIGNATURE_FILE_LOB_NOT_FOUND_ERROR", "English", "Files to verify signature lob not found"},
{BMD_SQL_VERIFY_SIGNATURE_SYMKEY_LOB_NOT_FOUND_ERROR , 		78, "Error", "System", "BMD_SQL_VERIFY_SIGNATURE_SYMKEY_LOB_NOT_FOUND_ERROR", "English", "Files to verify signature symkeys lob not found"},
{BMD_SQL_VERIFY_SIGNATURE_NOT_FOUND_ERROR , 				79, "Error", "System", "BMD_SQL_VERIFY_SIGNATURE_NOT_FOUND_ERROR", "English", "Signature to verify not found (file not signed)"},
{BMD_SQL_VERIFY_SIGNATURE_LOB_NOT_FOUND_ERROR , 			80, "Error", "System", "BMD_SQL_VERIFY_SIGNATURE_LOB_NOT_FOUND_ERROR", "English", "Signatures lob to verify not found"},
/* update metadanych */
{BMD_SQL_UPDATE_DB_ERROR ,						265, "Error", "System", "BMD_SQL_UPDATE_DB_ERROR", "English", "Database error during metadata update"},
{BMD_SQL_UPDATE_FILE_NOT_FOUND_ERROR ,					266, "Error", "System", "BMD_SQL_UPDATE_FILE_NOT_FOUND_ERROR", "English", "File to update metadata not found"},
{BMD_SQL_UPDATE_FILE_LOB_NOT_FOUND_ERROR ,				267, "Error", "System", "BMD_SQL_UPDATE_FILE_LOB_NOT_FOUND_ERROR", "English", "Files lob to update metadata not found"},
{BMD_SQL_UPDATE_SYMKEY_LOB_NOT_FOUND_ERROR ,				268, "Error", "System", "BMD_SQL_UPDATE_SYMKEY_LOB_NOT_FOUND_ERROR", "English", "File signatures lob to update metadata not found"},

/* ZAMKNIECIE TRANSAKCJI */
{BMD_SQL_CLOSE_TRANSACTION_DB_ERROR ,					268, "Error", "System", "BMD_SQL_CLOSE_TRANSACTION_DB_ERROR", "English", "Database error during tclosing transaction"},
{BMD_SQL_CLOSE_TRANSACTION_FILE_NOT_FOUND_ERROR ,			268, "Error", "System", "BMD_SQL_CLOSE_TRANSACTION_FILE_NOT_FOUND_ERROR", "English", "File to close transaction not found"},
{BMD_SQL_CLOSE_TRANSACTION_ID_NOT_FOUND_IN_DTG_ERROR ,			268, "Error", "System", "BMD_SQL_CLOSE_TRANSACTION_ID_NOT_FOUND_IN_DTG_ERROR", "English", "Transaction identifier not found in request datagram"},


/* OPERACJE NA KATALOGACH */
{BMD_SQL_GET_DIR_LIST_NOT_FOUND_ERROR ,					268, "Error", "System", "BMD_SQL_GET_DIR_LIST_NOT_FOUND_ERROR", "English", "Directory to find contents of not found"},
{BMD_SQL_DELETE_DIR_NOT_FOUND_ERROR ,					268, "Error", "System", "BMD_SQL_DELETE_DIR_NOT_FOUND_ERROR", "English", "Directory to delete not found"},
{BMD_SQL_GET_DIR_NOT_FOUND_ERROR ,					268, "Error", "System", "BMD_SQL_GET_DIR_NOT_FOUND_ERROR", "English", "Directory to get content of not found"},
{BMD_SQL_DELETE_DIR_NOT_EMPTY_ERROR ,					268, "Error", "System", "BMD_SQL_DELETE_DIR_NOT_EMPTY_ERROR", "English", "Directory to delete is not empty"},

/* OPERACJE HIStORYCZNOSCI */
{BMD_SQL_HISTORY_NOT_FOUND_ERROR ,					268, "Error", "System", "BMD_SQL_HISTORY_NOT_FOUND_ERROR", "English", "No history found for specified file"},



/******************************************/
/*	bledy operacji na plikach w BMD	*/
/******************************************/
{BMD_OPR_GET_INVALID_ID_FORMAT , 					81, "Error", "System", "BMD_OPR_GET_INVALID_ID_FORMAT", "English", "Invalid format of id given in protocol"},
{BMD_OPR_GET_HASH_NOT_FOUND , 						81, "Error", "System", "BMD_OPR_GET_HASH_NOT_FOUND", "English", "Files hash not found in database"},


/******************************************/
/*	bledy zwracane przez plugin DVCS	*/
/******************************************/
{PLUGIN_DVCS_CERTIFICATE_INVALID, 						82, "Error", "System", "PLUGIN_DVCS_CERTIFICATE_INVALID", "English", "DVCS - invalid certificate"},
{PLUGIN_DVCS_CERTIFICATE_VERIFIED, 						83, "Error", "System", "PLUGIN_DVCS_CERTIFICATE_VERIFIED", "English", "DVCS - certificate verified positively"},
{PLUGIN_DVCS_CERTIFICATE_VERIFIED_CONDITIONALLY, 			84, "Error", "System", "PLUGIN_DVCS_CERTIFICATE_VERIFIED_CONDITIONALLY", "English", "DVCS - certificate verified conditionally"},
{PLUGIN_DVCS_INVALID_LOGIN_CERTIFICATE ,					85, "Error", "System", "PLUGIN_DVCS_INVALID_LOGIN_CERTIFICATE", "English", "DVCS - invalid verification of login certificate"},
{PLUGIN_DVCS_INVALID_CERTIFICATE , 						86, "Error", "System", "PLUGIN_DVCS_INVALID_CERTIFICATE", "English", "DVCS - invalid verification of certificate"},

{PLUGIN_DVCS_SIGNATURE_INVALID ,						87, "Error", "System", "PLUGIN_DVCS_SIGNATURE_INVALID", "English", "DVCS - invalid verification of signature"},
{PLUGIN_DVCS_SIGNATURE_VERIFIED,						88, "Error", "System", "PLUGIN_DVCS_SIGNATURE_VERIFIED", "English", "DVCS - signature verified positively"},
{PLUGIN_DVCS_SIGNATURE_VERIFIED_CONDITIONALLY, 				89, "Error", "System", "PLUGIN_DVCS_SIGNATURE_VERIFIED_CONDITIONALLY", "English", "DVCS - signature verified conditionally"},
{PLUGIN_DVCS_DATA_ABSENT, 							91, "Error", "System", "PLUGIN_DVCS_DATA_ABSENT", "English", "DVCS - signature/data is absent"},
{PLUGIN_DVCS_SERVICE_FAILURE, 						92, "Error", "System", "PLUGIN_DVCS_SERVICE_FAILURE", "English", "DVCS - DVCS service failure"},

{PLUGIN_DVCS_FILE_NOT_FOUND , 					93, "Error", "System", "PLUGIN_DVCS_FILE_NOT_FOUND", "English", "DVCS - file to verify signature not found"},

{PLUGIN_DVCS_INSERTION_SIGNATURE_VERIFIED, 			94, "Informational", "System", "PLUGIN_DVCS_INSERTION_SIGNATURE_VERIFIED", "English", "DVCS - document insertion - signature verified positively"},
{PLUGIN_DVCS_INSERTION_SIGNATURE_VERIFIED_CONDITIONALLY, 	95, "Informational", "System", "PLUGIN_DVCS_INSERTION_SIGNATURE_VERIFIED_CONDITIONALLY", "English", "DVCS - document insertion - signature verified conditionally"},
{PLUGIN_DVCS_INSERTION_ACCEPTED_INVALID_SIGNATURE, 		96, "Informational", "System", "PLUGIN_DVCS_INSERTION_ACCEPTED_INVALID_SIGNATURE", "English", "DVCS - document insertion - accepted despite negative verification of signature"},
{PLUGIN_DVCS_INSERTION_ACCEPTED_SERVICE_FAILURE, 		97, "Informational", "System", "PLUGIN_DVCS_INSERTION_ACCEPTED_SERVICE_FAILURE", "English", "DVCS - document insertion - accepted although DVCS verification was impossible"},
{PLUGIN_DVCS_INSERTION_ACCEPTED_NO_SIGNATURE, 			98, "Informational", "System", "PLUGIN_DVCS_INSERTION_ACCEPTED_NO_SIGNATURE", "English", "DVCS - document insertion - accepted without signature"},
{PLUGIN_DVCS_INSERTION_REJECTED_INVALID_SIGNATURE, 		99, "Error", "System", "PLUGIN_DVCS_INSERTION_REJECTED_INVALID_SIGNATURE", "English", "DVCS - document insertion - rejected because of negative verification of signature"},
{PLUGIN_DVCS_INSERTION_REJECTED_SERVICE_FAILURE, 		100, "Error", "System", "PLUGIN_DVCS_INSERTION_REJECTED_SERVICE_FAILURE", "English", "DVCS - document insertion - rejected because DVCS verification was impossible"},
{PLUGIN_DVCS_INSERTION_REJECTED_NO_SIGNATURE, 			100, "Error", "System", "PLUGIN_DVCS_INSERTION_REJECTED_NO_SIGNATURE", "English", "DVCS - document insertion - rejected because of absence of signature"},
{PLUGIN_DVCS_INSERTION_INTERNAL_ERROR, 				100, "Error", "System", "PLUGIN_DVCS_INSERTION_INTERNAL_ERROR", "English", "DVCS - document insertion - plugin internal error"},

/******************************/
/*	bledy wysylania e-maili	*/
/********************** *******/
{PLUGIN_SESSION_MAIL_SEND_ERROR , 					101, "Error", "System", "PLUGIN_SESSION_MAIL_SEND_ERROR", "English", "Error in mail sending session"},
{PLUGIN_SESSION_SMTP_ADDRESS_CONF_ERROR , 				102, "Error", "System", "PLUGIN_SESSION_SMTP_ADDRESS_CONF_ERROR", "English", "Error in smtp address configuration option"},
{PLUGIN_SESSION_MAIL_TEMPLATE_CONF_ERROR , 				103, "Error", "System", "PLUGIN_SESSION_MAIL_TEMPLATE_CONF_ERROR", "English", "Error in mail template configuration option"},
{PLUGIN_SESSION_MAIL_IMAGES_ERROR , 					104, "Error", "System", "PLUGIN_SESSION_MAIL_IMAGES_ERROR", "English", "Error in mail images configuration option"},
{PLUGIN_SESSION_EINVOICE_MAIL_TITLE_ERROR ,				105, "Error", "System", "PLUGIN_SESSION_EINVOICE_MAIL_TITLE_ERROR", "English", "Error in einvoice mail title configuration option"},
{PLUGIN_SESSION_SMIME_FILE_CONF_ERROR , 				106, "Error", "System", "PLUGIN_SESSION_SMIME_FILE_CONF_ERROR", "English", "Error in smime file configuration option"},
{PLUGIN_SESSION_SMIME_PASS_CONF_ERROR , 				107, "Error", "System", "PLUGIN_SESSION_SMIME_PASS_CONF_ERROR", "English", "Error in smime files password configuration option"},
{PLUGIN_SESSION_CONF_STRUCT_ERROR , 					108, "Error", "System", "PLUGIN_SESSION_CONF_STRUCT_ERROR", "English", "Error in session plugin configuration structure"},
{PLUGIN_SESSION_RECIPIENT_NOT_SPECIFIED , 				108, "Error", "System", "PLUGIN_SESSION_RECIPIENT_NOT_SPECIFIED", "English", "E-mail recipient not specified"},
{PLUGIN_SESSION_MAX_ATTACH_SIZE_ERROR , 				108, "Error", "System", "PLUGIN_SESSION_MAX_ATTACH_SIZE_ERROR", "English", "Error in getting max attachment size from StdSession configuration"},
{PLUGIN_SESSION_MAIL_SEND_CONF_ERROR , 					108, "Error", "System", "PLUGIN_SESSION_MAIL_SEND_CONF_ERROR", "English", "Error in mail_send configuration option"},
{PLUGIN_SESSION_MAIL_FORMAT_CONF_ERROR , 				108, "Error", "System", "PLUGIN_SESSION_MAIL_FORMAT_CONF_ERROR", "English", "Error in mail_format configuration option"},
{PLUGIN_SESSION_MAIL_FORMAT_UNKNOWN , 					108, "Error", "System", "PLUGIN_SESSION_MAIL_FORMAT_UNKNOWN", "English", "Unknown mail format specified in plugins configuration file"},
{PLUGIN_SESSION_NOT_RUNNING , 						108, "Error", "System", "PLUGIN_SESSION_NOT_RUNNING", "English", "Mail session plugin not running on server side."},

{PLUGIN_SESSION_UNSPECIFIED_METADATA_CONF_ERROR , 				102, "Error", "System", "PLUGIN_SESSION_UNSPECIFIED_METADATA_CONF_ERROR", "English", "Option value_representing_unspecified_metadata must be specified"},
{PLUGIN_SESSION_PARAMETRIC_METADATA_CONF_ERROR , 				102, "Error", "System", "PLUGIN_SESSION_PARAMETRIC_METADATA_CONF_ERROR", "English", "Option add_attachment_parametric_metadata must be specified"},
{PLUGIN_SESSION_PARAM1_UNSPECIFIED_VALUE_CONF_ERROR , 				102, "Error", "System", "PLUGIN_SESSION_PARAM1_UNSPECIFIED_VALUE_CONF_ERROR", "English", "Option add_attachment_param1_unspecified_value must be specified"},
{PLUGIN_SESSION_PARAM2_UNSPECIFIED_VALUE_CONF_ERROR , 				102, "Error", "System", "PLUGIN_SESSION_PARAM2_UNSPECIFIED_VALUE_CONF_ERROR", "English", "Option add_attachment_param2_unspecified_value must be specified"},
{PLUGIN_SESSION_IF_SPECIFIED_METADATA_CONF_ERROR , 				102, "Error", "System", "PLUGIN_SESSION_IF_SPECIFIED_METADATA_CONF_ERROR", "English", "At least one of parameter options must be specified"},
{PLUGIN_SESSION_UNRECOGNIZED_FIXED_METADATA_OID , 				102, "Error", "System", "PLUGIN_SESSION_UNRECOGNIZED_FIXED_METADATA_OID", "English", "Unrecognized metadata oid for add_attachment_fixed_metadata setting"},
{PLUGIN_SESSION_UNRECOGNIZED_PARAMETRIC_METADATA_OID , 				102, "Error", "System", "PLUGIN_SESSION_UNRECOGNIZED_PARAMETRIC_METADATA_OID", "English", "Unrecognized metadata oid for add_attachment_parametric_metadata setting"},
{PLUGIN_SESSION_UNRECOGNIZED_PARAM1_METADATA_OID , 				102, "Error", "System", "PLUGIN_SESSION_UNRECOGNIZED_PARAM1_METADATA_OID", "English", "Unrecognized metadata oid for add_attachment_param1_if_specified_metadata setting"},
{PLUGIN_SESSION_UNRECOGNIZED_PARAM2_METADATA_OID , 				102, "Error", "System", "PLUGIN_SESSION_UNRECOGNIZED_PARAM2_METADATA_OID", "English", "Unrecognized metadata oid for add_attachment_param2_if_specified_metadata setting"},
{PLUGIN_SESSION_ADVICE_NOT_SENT_ACCORDING_TO_METADATA , 				102, "Error", "System", "PLUGIN_SESSION_ADVICE_NOT_SENT_ACCORDING_TO_METADATA", "English", "Advice not sent according to metadata values"},


{PLUGIN_SESSION_SESSION_CREATE_ERROR , 					108, "Error", "System", "PLUGIN_SESSION_SESSION_CREATE_ERROR", "English", "Error in creating smtp session"},
{PLUGIN_SESSION_ADD_MSG_TO_SESSION_ERROR , 				108, "Error", "System", "PLUGIN_SESSION_ADD_MSG_TO_SESSION_ERROR", "English", "Error in adding e-mail message to smtp session"},
{PLUGIN_SESSION_SET_MSG_CALLBACK_ERROR , 				108, "Error", "System", "PLUGIN_SESSION_SET_MSG_CALLBACK_ERROR", "English", "Error in setting callback function to smtp message"},
{PLUGIN_SESSION_ADD_RECIPIENT_TO_MSG_ERROR , 				108, "Error", "System", "PLUGIN_SESSION_ADD_RECIPIENT_TO_MSG_ERROR", "English", "Error in adding recipient to smtp message"},

{PLUGIN_SESSION_MAIL_SERVER_ADDRESS_NOT_SPECIFIED , 			108, "Error", "System", "PLUGIN_SESSION_MAIL_SERVER_ADDRESS_NOT_SPECIFIED", "English", "Smtp mail servers address not specified"},
{PLUGIN_SESSION_MAIL_SERVER_PORT_NOT_SPECIFIED , 			108, "Error", "System", "PLUGIN_SESSION_MAIL_SERVER_PORT_NOT_SPECIFIED", "English", "Smtp mail servers port not specified"},

{PLUGIN_SESSION_OID_FORMAT_CREATION_MODE , 				108, "Error", "System", "PLUGIN_SESSION_OID_FORMAT_CREATION_MODE", "English", "Bad format of creation mode oid"},
{PLUGIN_SESSION_NO_METADATA_CREATION_MODE , 				108, "Error", "System", "PLUGIN_SESSION_NO_METADATA_CREATION_MODE", "English", "Creation mode metadata not found"},
{PLUGIN_SESSION_FORMAT_OID_TO_TAG_ENTRY , 				108, "Error", "System", "PLUGIN_SESSION_FORMAT_OID_TO_TAG_ENTRY", "English", "Bad format of map_oid_to_tag configuration entry"},
{PLUGIN_SESSION_OID_FORMAT_OID_TO_TAG_ENTRY , 				108, "Error", "System", "PLUGIN_SESSION_OID_FORMAT_OID_TO_TAG_ENTRY", "English", "Bad oid format in map_oid_to_tag configuration entry"},
{PLUGIN_SESSION_NO_REQUESTED_ADD_METADATA , 				108, "Error", "System", "PLUGIN_SESSION_NO_REQUESTED_ADD_METADATA", "English", "Requested additional metadata not found (according to map_oid_to_tag entry)"},
{PLUGIN_SESSION_CREATION_MODE_VALUE , 					108, "Error", "System", "PLUGIN_SESSION_CREATION_MODE_VALUE", "English", "Bad creation mode value"},
{PLUGIN_SESSION_OID_FORMAT_SENDING_MODE , 				108, "Error", "System", "PLUGIN_SESSION_OID_FORMAT_SENDING_MODE", "English", "Bad format of sending mode oid"},
{PLUGIN_SESSION_OID_FORMAT_TEMPLATE_VERSION , 				108, "Error", "System", "PLUGIN_SESSION_OID_FORMAT_TEMPLATE_VERSION", "English", "Bad format of template version oid"},
{PLUGIN_SESSION_OID_FORMAT_LANGUAGE , 					108, "Error", "System", "PLUGIN_SESSION_OID_FORMAT_LANGUAGE", "English", "Bad format of language oid"},
{PLUGIN_SESSION_OID_FORMAT_RECIPIENT_EMAIL , 				108, "Error", "System", "PLUGIN_SESSION_OID_FORMAT_RECIPIENT_EMAIL", "English", "Bad format of recipient's e-mail oid"},
{PLUGIN_SESSION_OID_FORMAT_RECIPIENT_PHONE , 				108, "Error", "System", "PLUGIN_SESSION_OID_FORMAT_RECIPIENT_PHONE", "English", "Bad format of recipient's phone number oid"},
{PLUGIN_SESSION_NO_METADATA_SENDING_MODE , 				108, "Error", "System", "PLUGIN_SESSION_NO_METADATA_SENDING_MODE", "English", "Sending mode metadata not found"},
{PLUGIN_SESSION_NO_METADATA_TEMPLATE_VERSION , 				108, "Error", "System", "PLUGIN_SESSION_NO_METADATA_TEMPLATE_VERSION", "English", "Template version metadata not found"},
{PLUGIN_SESSION_NO_METADATA_LANGUAGE , 					108, "Error", "System", "PLUGIN_SESSION_NO_METADATA_LANGUAGE", "English", "Language metadata not found"},
{PLUGIN_SESSION_NO_METADATA_RECIPIENT_EMAIL , 				108, "Error", "System", "PLUGIN_SESSION_NO_METADATA_RECIPIENT_EMAIL", "English", "Recipient's e-mail metadata not found"},
{PLUGIN_SESSION_NO_METADATA_RECIPIENT_PHONE , 				108, "Error", "System", "PLUGIN_SESSION_NO_METADATA_RECIPIENT_PHONE", "English", "Recipient's phone number metadata not found"},
{PLUGIN_SESSION_SENDING_MODE_VALUE , 					108, "Error", "System", "PLUGIN_SESSION_SENDING_MODE_VALUE", "English", "Bad sending mode value"},
{PLUGIN_SESSION_OID_FORMAT_ACCESSIBLE , 				108, "Error", "System", "PLUGIN_SESSION_OID_FORMAT_ACCESSIBLE", "English", "Bad format of accessible oid"},
{PLUGIN_SESSION_NO_METADATA_ACCESSIBLE , 				108, "Error", "System", "PLUGIN_SESSION_NO_METADATA_ACCESSIBLE", "English", "Accessible metadata not found"},
{PLUGIN_SESSION_DOCUMENT_NOT_ACCESSIBLE , 				108, "Error", "System", "PLUGIN_SESSION_DOCUMENT_NOT_ACCESSIBLE", "English", "Document not accessible for recipient"},
{PLUGIN_SESSION_ADVICE_SENDING_ERROR , 					108, "Error", "System", "PLUGIN_SESSION_ADVICE_SENDING_ERROR", "English", "Unable to send e-advice"},
{PLUGIN_SESSION_ADVICE_SENDING_DISABLED , 				108, "Error", "System", "PLUGIN_SESSION_ADVICE_SENDING_DISABLED", "English", "Sending e-advices disabled"},


{PLUGIN_CONSERVATE_CONF_INVALID_TIME_FORMAT , 				109, "Error", "System", "PLUGIN_CONSERVATE_CONF_INVALID_TIME_FORMAT", "English", "Invalid exact time format in conservate plugin configuration file"},
{PLUGIN_CONSERVATE_CONF_EXACT_TIME_MISSING , 				110, "Error", "System", "PLUGIN_CONSERVATE_CONF_EXACT_TIME_MISSING", "English", "Exact time in conservate plugin configuration file not specified"},
{PLUGIN_CONSERVATE_TS_SERVER_ADDRESS_UNDEFINED , 			110, "Error", "System", "PLUGIN_CONSERVATE_TS_SERVER_ADDRESS_UNDEFINED", "English", "Timestamp server address undefined"},
{PLUGIN_CONSERVATE_TS_SERVER_PORT_UNDEFINED , 				110, "Error", "System", "PLUGIN_CONSERVATE_TS_SERVER_PORT_UNDEFINED", "English", "Timestamp server port undefined"},
{PLUGIN_CONSERVATE_TS_SERVER_TIMEOUT_UNDEFINED , 			110, "Error", "System", "PLUGIN_CONSERVATE_TS_SERVER_TIMEOUT_UNDEFINED", "English", "Timestamp server timeout undefined"},

{PLUGIN_ARCHIVING_INIT_PLUGIN_ERROR ,					110, "Error", "System", "PLUGIN_ARCHIVING_INIT_PLUGIN_ERROR", "English", "Unable to initialize archiving plugin"},
{PLUGIN_ARCHIVING_IMPROPER_IN_USE_OID_FORMAT ,				110, "Error", "System", "PLUGIN_ARCHIVING_IMPROPER_IN_USE_OID_FORMAT", "English", "Improper value of in_use_oid configuration setting (in archiving plugin configuration file)"},



/******************************/
/*	bledy soap serwera	*/
/******************************/
{BMDSOAP_SERVER_EMPTY_LOGIN_CERTIFICATE , 				111, "Error", "System", "BMDSOAP_SERVER_EMPTY_LOGIN_CERTIFICATE", "English", "Soap login certificate not specified in client request"},
{BMDSOAP_SERVER_BMD_INIT_ERROR , 					112, "Error", "System", "BMDSOAP_SERVER_BMD_INIT_ERROR", "English", "Soap bmd initialization error"},
{BMDSOAP_SERVER_BMD_INFO_CREATE_ERROR , 				113, "Error", "System", "BMDSOAP_SERVER_BMD_INFO_CREATE_ERROR", "English", "Soap bmd initialization error"},
{BMDSOAP_SERVER_CONF_READ_ERROR , 					114, "Error", "System", "BMDSOAP_SERVER_CONF_READ_ERROR", "English", "Error in reading soap server configuration file"},
{BMDSOAP_SERVER_LOGIN_SET_ERROR , 					115, "Error", "System", "BMDSOAP_SERVER_LOGIN_SET_ERROR", "English", "Error in setting soap servers login info"},
{BMDSOAP_SERVER_PFX_SET_ERROR , 					116, "Error", "System", "BMDSOAP_SERVER_PFX_SET_ERROR", "English", "Error in setting soap servers login certificate"},
{BMDSOAP_SERVER_BMD_CERT_SET_ERROR ,					117, "Error", "System", "BMDSOAP_SERVER_BMD_CERT_SET_ERROR", "English", "Error in setting bmd servers public key in soap server"},
{BMDSOAP_SERVER_PFX_AS_SET_ERROR , 					118, "Error", "System", "BMDSOAP_SERVER_PFX_AS_SET_ERROR", "English", "Error in setting soap server clients login certificate"},
{BMDSOAP_SERVER_BMD_LOGIN_ERROR , 					119, "Error", "System", "BMDSOAP_SERVER_BMD_LOGIN_ERROR", "English", "Error in login soap server to bmd server"},
{BMDSOAP_SERVER_ROLE_SET_ERROR , 					120, "Error", "System", "BMDSOAP_SERVER_ROLE_SET_ERROR", "English", "Error in setting soap server login role"},
{BMDSOAP_SERVER_EMPTY1_PARAM_ERROR , 					121, "Error", "System", "BMDSOAP_SERVER_EMPTY1_PARAM_ERROR", "English", "First parameter not specified"},
{BMDSOAP_SERVER_EMPTY2_PARAM_ERROR , 					122, "Error", "System", "BMDSOAP_SERVER_EMPTY2_PARAM_ERROR", "English", "Second parameter not specified"},
{BMDSOAP_SERVER_EMPTY3_PARAM_ERROR ,					123, "Error", "System", "BMDSOAP_SERVER_EMPTY3_PARAM_ERROR", "English", "Third parameter not specified"},
{BMDSOAP_SERVER_EMPTY4_PARAM_ERROR , 					124, "Error", "System", "BMDSOAP_SERVER_EMPTY4_PARAM_ERROR", "English", "Fourth parameter not specified"},
{BMDSOAP_SERVER_EMPTY5_PARAM_ERROR , 					125, "Error", "System", "BMDSOAP_SERVER_EMPTY5_PARAM_ERROR", "English", "Fifth parameter not specified"},
{BMDSOAP_SERVER_EMPTY6_PARAM_ERROR , 					126, "Error", "System", "BMDSOAP_SERVER_EMPTY6_PARAM_ERROR", "English", "Sixth parameter not specified"},
{BMDSOAP_SERVER_EMPTY7_PARAM_ERROR , 					127, "Error", "System", "BMDSOAP_SERVER_EMPTY7_PARAM_ERROR", "English", "Seventh parameter not specified"},
{BMDSOAP_SERVER_EMPTY8_PARAM_ERROR , 					128, "Error", "System", "BMDSOAP_SERVER_EMPTY8_PARAM_ERROR", "English", "Eighth parameter not specified"},
{BMDSOAP_SERVER_EMPTY9_PARAM_ERROR , 					129, "Error", "System", "BMDSOAP_SERVER_EMPTY9_PARAM_ERROR", "English", "Ninth parameter not specified"},
{BMDSOAP_SERVER_SOCKET_BIND_ERROR , 					130, "Error", "System", "BMDSOAP_SERVER_SOCKET_BIND_ERROR", "English", "Error in binding soap server socket"},
{BMDSOAP_SERVER_GET_DTG_FROM_DTGSET , 					131, "Error", "System", "BMDSOAP_SERVER_GET_DTG_FROM_DTGSET", "English", "Error in getting dtg from stgset in soap server"},
{BMDSOAP_SERVER_TEMP_DIR_UNDEF , 					132, "Error", "System", "BMDSOAP_SERVER_TEMP_DIR_UNDEF", "English", "Temporary directory undefined in configuration file"},
{BMDSOAP_SERVER_FINAL_FILENAME_UNDEF , 					133, "Error", "System", "BMDSOAP_SERVER_FINAL_FILENAME_UNDEF", "English", "Final filename undefined in client request"},
{BMDSOAP_SERVER_TEMP_FILENAME_UNDEF , 					134, "Error", "System", "BMDSOAP_SERVER_TEMP_FILENAME_UNDEF", "English", "Temporary filename undefined in client request"},
{BMDSOAP_SERVER_INPUT_FILE_UNDEF , 					135, "Error", "System", "BMDSOAP_SERVER_INPUT_FILE_UNDEF", "English", "Input file undefined in client request"},
{BMDSOAP_SERVER_INVALID_LIMIT_VALUE , 					136, "Error", "System", "BMDSOAP_SERVER_INVALID_LIMIT_VALUE", "English", "Invalid limit or offset value in search request"},
{BMDSOAP_SERVER_INVALID_FILE_ID , 					137, "Error", "System", "BMDSOAP_SERVER_INVALID_FILE_ID", "English", "Invalid files id value in client request"},
{BMDSOAP_SERVER_INVALID_FILE_PROPERTY , 				138, "Error", "System", "BMDSOAP_SERVER_INVALID_FILE_PROPERTY", "English", "Invalid files property value in client request"},
{BMDSOAP_SERVER_INPUT_SIG_UNDEF , 					139, "Error", "System", "BMDSOAP_SERVER_INPUT_SIG_UNDEF", "English", "Input signature undefined in client request"},
{BMDSOAP_SERVER_GCWD_ERROR , 						140, "Error", "System", "BMDSOAP_SERVER_GCWD_ERROR", "English", "Error in getting current working directory"},
{BMDSOAP_SERVER_TMP_DIR_CREATE_ERROR , 					141, "Error", "System", "BMDSOAP_SERVER_TMP_DIR_CREATE_ERROR", "English", "Error in creating temporary directory"},
{BMDSOAP_SERVER_CD_TMP_DIR , 						142, "Error", "System", "BMDSOAP_SERVER_CD_TMP_DIR", "English", "Error in changing cwd to temporary directory"},
{BMDSOAP_SERVER_XML_FILE_NOT_FOUND , 					143, "Error", "System", "BMDSOAP_SERVER_XML_FILE_NOT_FOUND", "English", "Error in getting xml file from archive"},
{BMDSOAP_SERVER_NO_PDF_FILE , 						144, "Error", "System", "BMDSOAP_SERVER_NO_PDF_FILE", "English", "Error in generating or getting pdf file from archive"},
{BMDSOAP_SERVER_TMP_DIR_OPEN_ERROR , 					145, "Error", "System", "BMDSOAP_SERVER_TMP_DIR_OPEN_ERROR", "English", "Error in openning temporary directory"},
{BMDSOAP_SERVER_GET_CHUNK_NOT_FOUND , 					146, "Error", "System", "BMDSOAP_SERVER_GET_CHUNK_NOT_FOUND", "English", "Requested chunk not found"},
{BMDSOAP_SERVER_SEARCH_OPERATOR_INVALID ,				147, "Error", "System", "BMDSOAP_SERVER_SEARCH_OPERATOR_INVALID", "English", "Invalid search operator in client request (LIKE, ILIKE, EXACT, EQUAL)"},
{BMDSOAP_SERVER_SEARCH_LOGICAL_INVALID , 				148, "Error", "System", "BMDSOAP_SERVER_SEARCH_LOGICAL_INVALID", "English", "Invalid search logical operator in client request (OR, AND)"},
{BMDSOAP_SERVER_SEARCH_SORT_ORDER_INVALID , 				149, "Error", "System", "BMDSOAP_SERVER_SEARCH_SORT_ORDER_INVALID", "English", "Invalid sort order in client request (ASC, DESC)"},
{BMDSOAP_SERVER_SEARCH_SORT_OID_INVALID , 				150, "Error", "System", "BMDSOAP_SERVER_SEARCH_SORT_OID_INVALID", "English", "Invalid sort oid format in client request"},
{BMDSOAP_SERVER_INVALID_TRANSACTION_ID , 				151, "Error", "System", "BMDSOAP_SERVER_INVALID_TRANSACTION_ID", "English", "Invalid transaction identifier in client request"},
{BMDSOAP_SERVER_TRANSACTION_TIMEOUT , 					152, "Error", "System", "BMDSOAP_SERVER_TRANSACTION_TIMEOUT", "English", "Transaction timeout (no file found with given transaction id)"},
{BMDSOAP_SERVER_UNDEFINED_FILE_HASH , 					153, "Error", "System", "BMDSOAP_SERVER_UNDEFINED_FILE_HASH", "English", "Undefined file hash in client request"},
{BMDSOAP_SERVER_INVALID_FILE_HASH , 					154, "Error", "System", "BMDSOAP_SERVER_INVALID_FILE_HASH", "English", "Invalid file hash in client request"},
{BMDSOAP_SERVER_INVALID_UPDATE_REASON,					269, "Error", "System", "BMDSOAP_SERVER_INVALID_UPDATE_REASON", "English", "Invalid metadata update reason"},
{BMDSOAP_SERVER_INVALID_TRANSMISSION_TYPE,				269, "Error", "System", "BMDSOAP_SERVER_INVALID_TRANSMISSION_TYPE", "English", "Invalid transmition type"},
{BMDSOAP_SERVER_SEARCH_RESULT_FORMAT_INVALID,				269, "Error", "System", "BMDSOAP_SERVER_NO_SELECTED_PKI_METADATA", "English", "Invalid search result format in client request (CSV, MTD)"},
{BMDSOAP_SERVER_NO_SELECTED_PKI_METADATA,				269, "Error", "System", "BMDSOAP_SERVER_GETPKI_METADATA_ERR", "English", "Pointed file has no selected pki data"},
{BMDSOAP_SERVER_GETPKI_METADATA_ERR,					269, "Error", "System", "BMDSOAP_SERVER_SEARCH_RESULT_FORMAT_INVALID", "English", "Error while getting pki metadata"},
{BMDSOAP_SERVER_BAD_SIGNATURE_TYPE_PARAM,				269, "Error", "System", "BMDSOAP_SERVER_BAD_SIGNATURE_TYPE_PARAM", "English", "Bad signature type parameter (only 'CADES' and 'XADES' values are allowed)"},
{BMDSOAP_SERVER_SIGNATURE_TYPE_NOT_SPECIFIED,				269, "Error", "System", "BMDSOAP_SERVER_SIGNATURE_TYPE_NOT_SPECIFIED", "English", "Signature type parameter is not specified"},
{BMDSOAP_SERVER_NO_WSSECURITY_CERTIFICATE,				269, "Error", "System", "BMDSOAP_SERVER_NO_WSSECURITY_CERTIFICATE", "English", "Unable to get certificate from WS Security header"},
{BMDSOAP_SERVER_NO_DEFAULT_LOGIN_CERTIFICATE,				269, "Error", "System", "BMDSOAP_SERVER_NO_DEFAULT_LOGIN_CERTIFICATE", "English", "No login certificate in request. Unable to load default login certificate"},
{BMDSOAP_SERVER_INVALID_ARCHIVE_FILE_ID,				269, "Error", "System", "BMDSOAP_SERVER_INVALID_ARCHIVE_FILE_ID", "English", "Invalid archive file id in client request"},
{BMDSOAP_SERVER_EMPTY_FILE_NAME,				269, "Error", "System", "BMDSOAP_SERVER_EMPTY_FILE_NAME", "English", "Empty file name"},

{BMDSOAP_SERVER_LOCK_FILE_ERROR,					269, "Error", "System", "BMDSOAP_SERVER_LOCK_FILE_ERROR", "English", "Cannot create file lock"},
{BMDSOAP_SERVER_LOCK_FILE_EXISTS_ERROR,					269, "Error", "System", "BMDSOAP_SERVER_LOCK_FILE_EXISTS_ERROR", "English", "Child process did not unlink file lock"},
{BMDSOAP_SERVER_STREAM_TIMEOUT,						269, "Error", "System", "BMDSOAP_SERVER_STREAM_TIMEOUT", "English", "Stream operation timeout"},
{BMDSOAP_SERVER_METADATA_VALUE_TOO_LONG,				269, "Error", "System", "BMDSOAP_SERVER_METADATA_VALUE_TOO_LONG", "English", "Received metadata value too long"},
{BMDSOAP_SERVER_XSLT_PATTERN_DICTIONARY_OPEN_ERROR,			269, "Error", "System", "BMDSOAP_SERVER_XSLT_PATTERN_DICTIONARY_OPEN_ERROR", "English", "Error in openning specified xslt dictionary file"},
{BMDSOAP_SERVER_ADVANCED_SEARCH_INVALID_FIRST_LOG_OPER,			269, "Error", "System", "BMDSOAP_SERVER_ADVANCED_SEARCH_INVALID_FIRST_LOG_OPER", "English", "First logical operator in search query cannot be specified"},
{BMDSOAP_SERVER_ADVANCED_SEARCH_INVALID_LOG_OPER,			269, "Error", "System", "BMDSOAP_SERVER_ADVANCED_SEARCH_INVALID_LOG_OPER", "English", "One of logical operators is invalid in search query"},
{BMDSOAP_SERVER_ADVANCED_SEARCH_INVALID_LOG_OPER_MISSING,		269, "Error", "System", "BMDSOAP_SERVER_ADVANCED_SEARCH_INVALID_LOG_OPER_MISSING", "English", "One of logical operators is missing in search query"},
{BMDSOAP_SERVER_ADVANCED_SEARCH_INVALID_ADJUSTMENT_OPER,		269, "Error", "System", "BMDSOAP_SERVER_ADVANCED_SEARCH_INVALID_ADJUSTMENT_OPER", "English", "Invalid search adjustment operator in client request (LIKE, ILIKE)"},
{BMDSOAP_SERVER_ADVANCED_SEARCH_INVALID_ADJUSTMENT_ARG,			269, "Error", "System", "BMDSOAP_SERVER_ADVANCED_SEARCH_INVALID_ADJUSTMENT_ARG", "English", "Invalid search adjustmet argument (NONE, BEGIN, END, BOTH)"},
{BMDSOAP_SERVER_ADVANCED_SEARCH_INVALID_STATEMENT,			269, "Error", "System", "BMDSOAP_SERVER_ADVANCED_SEARCH_INVALID_STATEMENT", "English", "Invalid search statement in client request (POSITIVE, NEGATIVE)"},

{BMDSOAP_SERVER_USER_USER_INFO_UNDEFINED,				269, "Error", "System", "BMDSOAP_SERVER_USER_USER_INFO_UNDEFINED", "English", "User information not specified"},
{BMDSOAP_SERVER_USER_SERIAL_UNDEFINED,					269, "Error", "System", "BMDSOAP_SERVER_USER_SERIAL_UNDEFINED", "English", "Users certificate serial number not specified"},
{BMDSOAP_SERVER_USER_ISSUER_UNDEFINED,					269, "Error", "System", "BMDSOAP_SERVER_USER_ISSUER_UNDEFINED", "English", "Users certificate issuer not specified"},
{BMDSOAP_SERVER_USER_NAME_UNDEFINED,					269, "Error", "System", "BMDSOAP_SERVER_USER_NAME_UNDEFINED", "English", "User name not specified"},
{BMDSOAP_SERVER_USER_IDENTITY_UNDEFINED,				269, "Error", "System", "BMDSOAP_SERVER_USER_IDENTITY_UNDEFINED", "English", "User identity not specified"},
{BMDSOAP_SERVER_USER_DEFAULT_GROUP_UNDEFINED,				269, "Error", "System", "BMDSOAP_SERVER_USER_DEFAULT_GROUP_UNDEFINED", "English", "Users default group not specified"},
{BMDSOAP_SERVER_USER_DEFAULT_ROLE_UNDEFINED,				269, "Error", "System", "BMDSOAP_SERVER_USER_DEFAULT_ROLE_UNDEFINED", "English", "Users default role not specified"},
{BMDSOAP_SERVER_USER_ID_UNDEFINED,					269, "Error", "System", "BMDSOAP_SERVER_USER_ID_UNDEFINED", "English", "Users id not specified"},
{BMDSOAP_SERVER_USER_DEFAULT_CLASS_UNDEFINED,				269, "Error", "System", "BMDSOAP_SERVER_USER_DEFAULT_CLASS_UNDEFINED", "English", "Users default class not specified"},


{BMDSOAP_SERVER_ROLE_INFO_NOT_DEFINED,					269, "Error", "System", "BMDSOAP_SERVER_ROLE_INFO_NOT_DEFINED", "English", "Role information not specified"},
{BMDSOAP_SERVER_ROLE_ACTIONS_NOT_DEFINED,				269, "Error", "System", "BMDSOAP_SERVER_ROLE_ACTIONS_NOT_DEFINED", "English", "Roles action list not specified"},
{BMDSOAP_SERVER_ROLE_ROLE_RIGHTS_NOT_DEFINED,				269, "Error", "System", "BMDSOAP_SERVER_ROLE_ROLE_RIGHTS_NOT_DEFINED", "English", "Roles rigth not specified"},
{BMDSOAP_SERVER_ROLE_NAME_NOT_DEFINED,					269, "Error", "System", "BMDSOAP_SERVER_ROLE_NAME_NOT_DEFINED", "English", "Roles name not specified"},
{BMDSOAP_SERVER_ROLE_ID_NOT_DEFINED,					269, "Error", "System", "BMDSOAP_SERVER_ROLE_ID_NOT_DEFINED", "English", "Roles id not specified"},


{BMDSOAP_SERVER_GROUP_INFO_NOT_DEFINED,					269, "Error", "System", "BMDSOAP_SERVER_REGISTER_GROUP_INFO_NOT_DEFINED", "English", "Group information not specified"},
{BMDSOAP_SERVER_GROUP_NAME_NOT_DEFINED,					269, "Error", "System", "BMDSOAP_SERVER_REGISTER_GROUP_NAME_NOT_DEFINED", "English", "Groups name not specified"},
{BMDSOAP_SERVER_GROUP_ID_NOT_DEFINED,					269, "Error", "System", "BMDSOAP_SERVER_REGISTER_GROUP_ID_NOT_DEFINED", "English", "Groups id not specified"},

{BMDSOAP_SERVER_SEC_CAT_INFO_NOT_DEFINED,				269, "Error", "System", "BMDSOAP_SERVER_SEC_CAT_INFO_NOT_DEFINED", "English", "Security category information not specified"},
{BMDSOAP_SERVER_SEC_CAT_NAME_NOT_DEFINED,				269, "Error", "System", "BMDSOAP_SERVER_SEC_CAT_NAME_NOT_DEFINED", "English", "Security category name not specified"},

{BMDSOAP_OWNER_NOT_SPECIFIED,				269, "Error", "System", "BMDSOAP_OWNER_NOT_SPECIFIED", "English", "Owner is not specified"},
{BMDSOAP_OWNER_GROUP_NO_SPECIFIED,			269, "Error", "System", "BMDSOAP_OWNER_GROUP_NO_SPECIFIED", "English", "Owner group is not specified"},
{BMDSOAP_LINK_NAME_NOT_SPECIFIED,				269, "Error", "System", "BMDSOAP_LINK_NAME_NOT_SPECIFIED", "English", "Link name is not specified"},
{BMDSOAP_POINTING_ID_NOT_SPECIFIED,				269, "Error", "System", "BMDSOAP_POINTING_ID_NOT_SPECIFIED", "English", "Pointing id is not specified"},
{BMDSOAP_FILE_ID_NOT_SPECIFIED,					269, "Error", "System", "BMDSOAP_FILE_ID_NOT_SPECIFIED", "English", "File id is not specified"},
{BMDSOAP_NO_CRITERIA_SPECIFIED,					269, "Error", "System", "BMDSOAP_NO_CRITERIA_SPECIFIED", "English", "No criteria specified to indicate document."},

{BMDSOAP_INCORRECT_VISIBILITY_VALUE,					269, "Error", "System", "BMDSOAP_INCORRECT_VISIBILITY_VALUE", "English", "Incorrect visibility value."},

{BMDSOAP_SERVER_LOBS_NOT_FINISHED ,					155, "Error", "System", "BMDSOAP_SERVER_LOBS_NOT_FINISHED", "English", "Lob transfer not finished properly"},
{BMDSOAP_SERVER_TOTAL_FILESIZE_INVALID , 				156, "Error", "System", "BMDSOAP_SERVER_TOTAL_FILESIZE_INVALID", "English", "Invalid total filesize value"},
{BMDSOAP_SERVER_PACK_NUMBER_INVALID , 					157, "Error", "System", "BMDSOAP_SERVER_PACK_NUMBER_INVALID", "English", "Invalid package number (count)"},
{BMDSOAP_SERVER_LOBS_NOT_FINISHED_TRANSACTION , 			157, "Error", "System", "BMDSOAP_SERVER_LOBS_NOT_FINISHED_TRANSACTION", "English", "File in transaction not transfered completely"},
{BMDSOAP_SERVER_ERR_CREATING_TRANSACTION_MARKER , 			157, "Error", "System", "BMDSOAP_SERVER_ERR_CREATING_TRANSACTION_MARKER", "English", "Error in creating transaction marker in temporary directory"},
{BMDSOAP_SERVER_NO_TRANSACTION_STARTED ,				157, "Error", "System", "BMDSOAP_SERVER_NO_TRANSACTION_STARTED", "English", "Transaction not started"},
{BMDSOAP_SERVER_TRANSACTION_ALREADY_STARTED,				157, "Error", "System", "BMDSOAP_SERVER_TRANSACTION_ALREADY_STARTED", "English", "Transaction already started"},
{BMDSOAP_SERVER_CANNOT_CREATE_TRANSACTION_MARKER,			157, "Error", "System", "BMDSOAP_SERVER_CANNOT_CREATE_TRANSACTION_MARKER", "English", "Cannot create transaction marker"},
{BMDSOAP_SERVER_CHDIR_ERROR, 					 	157, "Error", "System", "BMDSOAP_SERVER_CHDIR_ERROR", "English", "Cannot change working directory"},
{BMDSOAP_SERVER_ERR_CREATING_TRANSMITTED_MARKER,			157, "Error", "System", "BMDSOAP_SERVER_ERR_CREATING_TRANSMITTED_MARKER", "English", "Cannot mark transmitted file"},
{BMDSOAP_SERVER_OPENDIR_ERROR,						157, "Error", "System", "BMDSOAP_SERVER_OPENDIR_ERROR", "English", "Cannot open directory"},
{BMDSOAP_CLOSING_EMPTY_TRANSACTION,					157, "Error", "System", "BMDSOAP_CLOSING_EMPTY_TRANSACTION", "English", "Closing transaction with no file transmitted"},
{BMDSOAP_SERVER_INTERNAL_ERROR,						157, "Error", "System", "BMDSOAP_SERVER_INTERNAL_ERROR", "English", "Error in internal operations"},
{BMDSOAP_SERVER_IDLE_TRANSACTION_TIMEOUT,				157, "Error", "System", "BMDSOAP_SERVER_IDLE_TRANSACTION_TIMEOUT", "English", "Transaction marker removed because of timeout"},
{BMDSOAP_SERVER_PID_FILE_ERROR,						157, "Error", "System", "BMDSOAP_SERVER_PID_FILE_ERROR", "English", "Error with pid file handling"},
{BMDSOAP_SERVER_TRANSMISSION_BROKEN,					157, "Error", "System", "BMDSOAP_SERVER_TRANSMISSION_BROKEN", "English", "Transmission broken or wrong proc catalog set"},
{BMDSOAP_SERVER_ERR_REMOVING_TRANSMISSION_MARKER,			157, "Error", "System", "BMDSOAP_SERVER_ERR_REMOVING_TRANSMISSION_MARKER", "English", "Error in removing transmission marker"},
{BMDSOAP_SERVER_UNDEFINED_METADATA_OID_FORM,		157, "Error", "System", "BMDSOAP_SERVER_UNDEFINED_METADATA_OID_FORM", "English", "Undefined OID in form of metadata expected by user"},


/******************************************/
/*	bledy biblioteki kompresujacej	*/
/******************************************/
{ZLIB_COMP_SRC_FILE_OPEN_ERROR , 						158, "Error", "System", "ZLIB_COMP_SRC_FILE_OPEN_ERROR", "English", "Error in opening compress source file"},
{ZLIB_COMP_DST_FILE_OPEN_ERROR , 						159, "Error", "System", "ZLIB_COMP_DST_FILE_OPEN_ERROR", "English", "Error in opening compress destination file"},
{ZLIB_DECOMP_SRC_FILE_OPEN_ERROR , 						160, "Error", "System", "ZLIB_DECOMP_SRC_FILE_OPEN_ERROR", "English", "Error in opening decompress source file"},
{ZLIB_DECOMP_DST_FILE_OPEN_ERROR , 						161, "Error", "System", "ZLIB_DECOMP_DST_FILE_OPEN_ERROR", "English", "Error in opening decompress destination file"},
{ZLIB_DECOMP_NO_FILES_FOR_COMPRESSION , 					162, "Error", "System", "ZLIB_DECOMP_NO_FILES_FOR_COMPRESSION", "English", "List of files to compress is empty"},
{ZLIB_DECOMP_NO_OUTPUT_FILE ,							163, "Error", "System", "ZLIB_DECOMP_NO_OUTPUT_FILE", "English", "Compress output file not defined"},

/******************************************/
/*	bledy biblioteki conwersji z xmla	*/
/******************************************/
{BMDSOAP_SERVER_PARSE_XSL_ERROR , 						164, "Error", "System", "BMDSOAP_SERVER_PARSE_XSL_ERROR", "English", "Error in parsing xsl file during conversion to html"},
{BMDSOAP_SERVER_PARSE_XML_ERROR , 						165, "Error", "System", "BMDSOAP_SERVER_PARSE_XML_ERROR", "English", "Error in parsing xml file during conversion to html"},
{BMDSOAP_SERVER_APPLY_STYLE_ERROR , 						166, "Error", "System", "BMDSOAP_SERVER_APPLY_STYLE_ERROR", "English", "Error in applying stylesheet on xsl document"},
{BMDSOAP_SERVER_SAVE_HTML_ERROR , 						167, "Error", "System", "BMDSOAP_SERVER_SAVE_HTML_ERROR", "English", "Error in saving result html file"},
{BMDSOAP_SERVER_XML_PATTERN_UNDEF , 						168, "Error", "System", "BMDSOAP_SERVER_XML_PATTERN_UNDEF", "English", "Undefined xsl pattern in soap server configuration"},
{BMDSOAP_SERVER_XML_PATTERN_CANT_OPEN , 					169, "Error", "System", "BMDSOAP_SERVER_XML_PATTERN_CANT_OPEN", "English", "Cannot open xml pattern file defined in soap server configuration"},

/******************************************/
/* bledy znacznika czasu                  */
/******************************************/
{BMD_TIMESTAMP_RESOURCE_NOT_AVAILABLE , 					169, "Error", "System", "BMD_TIMESTAMP_RESOURCE_NOT_AVAILABLE", "English", "Timestamp resource not available"},
{BMD_TIMESTAMP_OPERATION_FAILED , 						169, "Error", "System", "BMD_TIMESTAMP_OPERATION_FAILED", "English", "Timestamping operation failed"},

/************************************/
/*	bledy podsystemu sieciowego	*/
/************************************/
{BMD_ERR_NET_CONNECT , 								170, "Error", "System", "BMD_ERR_NET_CONNECT", "English", "Error in connecting to archive remote host"},
{BMD_ERR_NET_RESOLV , 								171, "Error", "System", "BMD_ERR_NET_RESOLV", "English", "Error in connecting to archive remote host (unresolved domain address)"},
{BMD_ERR_NET_READ , 								172, "Error", "System", "BMD_ERR_NET_READ", "English", "Error in reading from socket"},
{BMD_ERR_NET_WRITE , 								173, "Error", "System", "BMD_ERR_NET_WRITE", "English", "Error in writing to socket"},
{CONNECTION_REJECTED , 								174, "Error", "System", "CONNECTION_REJECTED", "English", "Connection rejected by remote host"},
{SESS_DEV_ERR , 								175, "Error", "System", "SESS_DEV_ERR", "English", "Net connection subsystem error"},
{SESS_ATTR_ERR , 								176, "Error", "System", "SESS_ATTR_ERR", "English", "Net connection subsystem error"},
{SEND_PUSH_ERR , 								177, "Error", "System", "SEND_PUSH_ERR", "English", "Net connection subsystem error"},
{SEND_FLUSH_ERR , 								178, "Error", "System", "SEND_FLUSH_ERR", "English", "Net connection subsystem error"},
{RECV_POP_ERR , 								179, "Error", "System", "RECV_POP_ERR", "English", "Net connection subsystem error"},

{BMD_ERR_UNIMPLEMENTED , 							180, "Error", "System", "BMD_ERR_UNIMPLEMENTED", "English", "Functionality not implemented"},
{BMD_ERR_NOTAVAIL , 								181, "Error", "System", "BMD_ERR_NOTAVAIL", "English", "Functionality not available"},
{BMD_ERR_OP_FAILED , 								182, "Error", "System", "BMD_ERR_OP_FAILED", "English", "Operation failed"},
{BMD_ERR_CREDENTIALS , 								183, "Error", "System", "BMD_ERR_CREDENTIALS", "English", "Inproper credentials"},
{BMD_ERR_RAND , 								184, "Error", "System", "BMD_ERR_RAND", "English", "Randomize subsystem error"},
{BMD_ERR_COMPLETE , 								185, "Error", "System", "BMD_ERR_COMPLETE", "English", "Operation already completed"},
{BMD_ERR_NOTINITED , 								186, "Error", "System", "BMD_ERR_NOTINITED", "English", "Variables not initialized"},
{BMD_ERR_PROTOCOL , 								187, "Error", "System", "BMD_ERR_PROTOCOL", "English", "Protocol error"},
{BMD_ERR_OVERFLOW , 								188, "Error", "System", "BMD_ERR_OVERFLOW", "English", "Overflow error"},
{BMD_ERR_NOT_ALLOWED , 								190, "Error", "System", "BMD_ERR_NOT_ALLOWED", "English", "Operation not allowed"},
{BMD_ERR_TIMEOUT ,								191, "Error", "System", "BMD_ERR_TIMEOUT", "English", "Operation timeout"},
{BMD_ERR_USER_OP_FAILED , 							192, "Error", "System", "BMD_ERR_USER_OP_FAILED", "English", "Operation failed on clients side"},
{BMD_ERR_LOGIN_AGAIN , 								193, "Error", "System", "BMD_ERR_LOGIN_AGAIN", "English", "Disconected from server, please login again"},
{BMD_ERR_RESOURCE_NOTAVAIL , 							194, "Error", "System", "BMD_ERR_RESOURCE_NOTAVAIL", "English", "Resource unavailable"},
{BMD_CANCEL , 									195, "Error", "System", "BMD_CANCEL", "English", "Operation cancelled"},
{PLUGIN_NOT_LOADED , 								196, "Error", "System", "PLUGIN_NOT_LOADED", "English", "Plugin not loaded"},
{VERSION_NUMBER_ERR , 								197, "Error", "System", "VERSION_NUMBER_ERR", "English", "Protocol version incorrect"},
{LOB_CANCELED_BY_CLIENT ,							198, "Error", "System", "LOB_CANCELED_BY_CLIENT", "English", "Lob transfer cancelled by client"},
{FILE_SIZE_ERR , 								199, "Error", "System", "FILE_SIZE_ERR", "English", "File size is too big"},
{SET_GEN_BUF_FAILED , 								200, "Error", "System", "SET_GEN_BUF_FAILED", "English", "Error in setting gen buf"},
{BMD_DB_CONNECTION_ERROR , 							201, "Error", "System", "BMD_DB_CONNECTION_ERROR", "English", "Error in connecting to database"},
{ERR_NO_USER_WITH_GIVEN_ID , 							202, "Error", "System", "ERR_NO_USER_WITH_GIVEN_ID", "English", "No user with given id found in database"},
{BMD_ERR_INTERRUPTED , 								189, "Error", "System", "BMD_ERR_INTERRUPTED", "English", "Operation interrupted by user"},
{BMD_ERR_PREPARE_ANSWER , 							189, "Error", "System", "BMD_ERR_PREPARE_ANSWER", "English", "Unable to prepare answer"},
{BMD_ERR_OBSOLETE , 								190, "Error", "System", "BMD_ERR_OBSOLETE", "English", "Functionality is obsolete"},


{BMD_ERR_WRONG_SERIALISATION_TYPE,						202, "Error", "System", "BMD_ERR_WRONG_SERIALISATION_TYPE", "English", "Wrong serialisation type"},
{BMD_ERR_PTHREAD_CREATE,							202, "Error", "System", "BMD_ERR_PTHREAD_CREATE", "English", "Thread creation error"},
{BMD_ERR_PTHREAD_JOIN,								202, "Error", "System", "BMD_ERR_PTHREAD_JOIN", "English", "Thread join error"},


/************************************************************/
/*	bledy systemowe (pamiec, pliki, bledy odczytu itd)	*/
/************************************************************/
{BMD_ERR_MEMORY , 								203, "Error", "System", "BMD_ERR_MEMORY", "English", "Memory allocation error"},
{BMD_ERR_DISK , 								204, "Error", "System", "BMD_ERR_DISK", "English", "Disk operation error"},
{BMD_ERR_FORMAT , 								205, "Error", "System", "BMD_ERR_FORMAT", "English", "Invalid data format"},
{BMD_ERR_NODATA , 								206, "Error", "System", "BMD_ERR_NODATA", "English", "No data found with specified conditions"},
{BMD_ERR_TOO_BIG , 								207, "Error", "System", "BMD_ERR_TOO_BIG", "English", "Data too big"},
{NO_MEMORY , 									208, "Error", "System", "NO_MEMORY", "English", "Insufficient memory"},
{NO_SERIAL_FILE_ERR , 								209, "Error", "System", "NO_SERIAL_FILE_ERR", "English", "Serial file error"},
{MAX_MEMORY_EXCEEDED, 								209, "Error", "System", "MAX_MEMORY_EXCEEDED", "English", "Memory limit exceeded"},

/************************/
/*	bledy argumentow	*/
/************************/
{BMD_ERR_PARAM1 , 								210, "Error", "System", "BMD_ERR_PARAM1", "English", "Invalid first parameter value"},
{BMD_ERR_PARAM2 , 								211, "Error", "System", "BMD_ERR_PARAM2", "English", "Invalid second parameter value"},
{BMD_ERR_PARAM3 , 								212, "Error", "System", "BMD_ERR_PARAM3", "English", "Invalid third parameter value"},
{BMD_ERR_PARAM4 , 								213, "Error", "System", "BMD_ERR_PARAM4", "English", "Invalid fourth parameter value"},
{BMD_ERR_PARAM5 , 								214, "Error", "System", "BMD_ERR_PARAM5", "English", "Invalid fifth parameter value"},
{BMD_ERR_PARAM6 , 								215, "Error", "System", "BMD_ERR_PARAM6", "English", "Invalid sixth parameter value"},
{BMD_ERR_PARAM7 , 								216, "Error", "System", "BMD_ERR_PARAM7", "English", "Invalid seventh parameter value"},
{BMD_ERR_PARAM8 , 								217, "Error", "System", "BMD_ERR_PARAM8", "English", "Invalid eight parameter value"},
{BMD_ERR_PARAM9 , 								218, "Error", "System", "BMD_ERR_PARAM9", "English", "Invalid ninth parameter value"},
{BMD_ERR_PARAM10 , 								219, "Error", "System", "BMD_ERR_PARAM10", "English", "Invalid tenth parameter value"},
{BMD_ERR_PARAM11 , 								219, "Error", "System", "BMD_ERR_PARAM11", "English", "Invalid eleventh parameter value"},
{BMD_ERR_PARAM12 , 								219, "Error", "System", "BMD_ERR_PARAM12", "English", "Invalid twelfth tenth parameter value"},
{BMD_ERR_PARAM13 , 								219, "Error", "System", "BMD_ERR_PARAM13", "English", "Invalid threetenth parameter value"},
{BMD_ERR_PARAM14 , 								219, "Error", "System", "BMD_ERR_PARAM14", "English", "Invalid fourteenth parameter value"},
{BMD_ERR_PARAM15 , 								219, "Error", "System", "BMD_ERR_PARAM15", "English", "Invalid fifteenth parameter value"},
{BMD_ERR_PARAM16 , 								219, "Error", "System", "BMD_ERR_PARAM16", "English", "Invalid sixteenth parameter value"},
{BMD_ERR_PARAM17 , 								219, "Error", "System", "BMD_ERR_PARAM17", "English", "Invalid seventeenth parameter value"},
{BMD_ERR_PARAM18 , 								219, "Error", "System", "BMD_ERR_PARAM18", "English", "Invalid eighteenth parameter value"},
{NO_ARGUMENTS , 								220, "Error", "System", "NO_ARGUMENTS", "English", "Too few arguments error"},

/******************************************/
/*	bledy podsystemu kryptograficznego	*/
/******************************************/
{BMD_ERR_PKI_VERIFY , 								221, "Error", "System", "BMD_ERR_PKI_VERIFY", "English", "Error in PKI verification"},
{BMD_ERR_PKI_SIGN , 								222, "Error", "System", "BMD_ERR_PKI_SIGN", "English", "Unable to sign data"},
{BMD_ERR_PKI_ENCRYPT ,								223, "Error", "System", "BMD_ERR_PKI_ENCRYPT", "English", "Error in encypting data"},
{BMD_ERR_PKI_DECRYPT , 								224, "Error", "System", "BMD_ERR_PKI_DECRYPT", "English", "Error in decrypting data"},
{BMD_ERR_HASH_VERIFY , 								225, "Error", "System", "BMD_ERR_HASH_VERIFY", "English", "Error in hash verification"},
{BMD_ERR_KEY_CORRESP , 								226, "Error", "System", "BMD_ERR_KEY_CORRESP", "English", "Public and private key mismatch"},
{BMD_ERR_SIGN_VERIFY , 								227, "Error", "System", "BMD_ERR_SIGN_VERIFY", "English", "Error in signature verification"},
{BMD_WRONG_SERVER_CERT , 							228, "Error", "System", "BMD_WRONG_SERVER_CERT", "English", "Incorect servers certificate"},
{BMD_ERR_PKI_NO_CRL , 								229, "Error", "System", "BMD_ERR_PKI_NO_CRL", "English", "Error in getting CRL list"},
{PKI_OBJ_ERR_CODE_BIO , 							230, "Error", "System", "PKI_OBJ_ERR_CODE_BIO", "English", "Error in creating internat BIO for gen buf"},

{PKI_OBJ_ERR_CODE_P12_NO_CERT , 						231, "Error", "System", "PKI_OBJ_ERR_CODE_P12_NO_CERT", "English", "No certificates found in given file"},
{PKI_OBJ_ERR_CODE_P12_NO_EE_CERT , 						232, "Error", "System", "PKI_OBJ_ERR_CODE_P12_NO_EE_CERT", "English", "No terminal unit certificates found in given file"},
{PKI_OBJ_ERR_CODE_P12_PASS , 							233, "Error", "System", "PKI_OBJ_ERR_CODE_P12_PASS", "English", "Ivalid password for given certificate"},
{PKI_OBJ_ERR_CODE_FORMAT_UNKNOWN , 						234, "Error", "System", "PKI_OBJ_ERR_CODE_FORMAT_UNKNOWN", "English", "Unknown file format"},
{PKI_OBJ_ERR_CODE_FORMAT_NOT_CERT , 						235, "Error", "System", "PKI_OBJ_ERR_CODE_FORMAT_NOT_CERT", "English", "Given file is not a certificate"},
{CERT_CHAIN_BAD , 								236, "Error", "System", "CERT_CHAIN_BAD", "English", "Invalid certification path chain"},
{ERR_CERT_CHAIN_INT , 								237, "Error", "System", "ERR_CERT_CHAIN_INT", "English", "Error in initializing certification path"},
{ERR_CERT_CHAIN_DIR , 								238, "Error", "System", "ERR_CERT_CHAIN_DIR", "English", "Invalid certification path directory"},
{ERR_CERT_CHAIN_GET ,								239, "Error", "System", "ERR_CERT_CHAIN_GET", "English", "Error in getting certification path"},
{ERR_CERT_ISSUER_GET , 								240, "Error", "System", "ERR_CERT_ISSUER_GET", "English", "Error in getting certificates issuer"},

{PKCS15_PRIVKEY_ERR , 								241, "Error", "System", "PKCS15_PRIVKEY_ERR", "English", "Private key error"},

{ERR_READ_PRIVKEY , 								242, "Error", "System", "ERR_READ_PRIVKEY", "English", "Error in reading private key"},
{ERR_READ_PUBKEY , 								243, "Error", "System", "ERR_READ_PUBKEY", "English", "Error in reading public key"},
{ERR_READ_X509 , 								244, "Error", "System", "ERR_READ_X509", "English", "Error in reading X509"},

{CERTIFICATE_INVALID , 								245, "Error", "System", "CERTIFICATE_INVALID", "English", "Invalid certificate"},
{BN_ERROR , 									246, "Error", "System", "BN_ERROR", "English", "OpenSSL error"},
{ERR_PKCS12_NO_ENDENTITY_CERT,					247, "Error", "System", "ERR_PKCS12_NO_ENDENTITY_CERT", "English", "Unable to find end entity certificate in PKCS#12 container"},

/* nowe bledy */
{LIBBMDASN1_COMMON_UTILS_UNRECOGNIZED_ATTRIBUTE_OID , 			264, "Error", "System", "LIBBMDASN1_COMMON_UTILS_UNRECOGNIZED_ATTRIBUTE_OID", "English", "Unrecognized atrribute object identifier"},
{LIBBMDASN1_COMMON_UTILS_NAME_HAS_NO_RDNSEQUENCE_FIELD , 		264, "Error", "System", "LIBBMDASN1_COMMON_UTILS_NAME_HAS_NO_RDNSEQUENCE_FIELD", "English", "Asn1 name has no rdn sequence field"},
{LIBBMDASN1_COMMON_UTILS_CLONE_RDNSEQUENCE , 				264, "Error", "System", "LIBBMDASN1_COMMON_UTILS_CLONE_RDNSEQUENCE", "English", "Error in cloning rdn sequence"},
{LIBBMDASN1_COMMON_UTILS_CLONE_ATTRVALUE , 				264, "Error", "System", "LIBBMDASN1_COMMON_UTILS_CLONE_ATTRVALUE", "English", "Error in cloning AttributeValue"},
{LIBBMDASN1_COMMON_UTILS_CLONE_ATTRVALUE_SURNAME , 			264, "Error", "System", "LIBBMDASN1_COMMON_UTILS_CLONE_ATTRVALUE_SURNAME", "English", "Error in cloning surname attribute value"},
{LIBBMDASN1_COMMON_UTILS_CLONE_ATTRVALUE_GIVENNAME , 			264, "Error", "System", "LIBBMDASN1_COMMON_UTILS_CLONE_ATTRVALUE_GIVENNAME", "English", "Error in cloning given name attribute value"},
{LIBBMDASN1_COMMON_UTILS_CLONE_ATTRVALUE_COMMONNAME ,		 	264, "Error", "System", "LIBBMDASN1_COMMON_UTILS_CLONE_ATTRVALUE_COMMONNAME", "English", "Error in cloning common name attribute value"},
{LIBBMDASN1_COMMON_UTILS_CLONE_ATTRVALUE_ORGANIZATION , 		264, "Error", "System", "LIBBMDASN1_COMMON_UTILS_CLONE_ATTRVALUE_ORGANIZATION", "English", "Error in cloning organization attribute value"},
{LIBBMDASN1_COMMON_UTILS_CLONE_ATTRVALUE_COUNTRY , 			264, "Error", "System", "LIBBMDASN1_COMMON_UTILS_CLONE_ATTRVALUE_COUNTRY", "English", "Error in cloning country attribute value"},
{LIBBMDASN1_COMMON_UTILS_CLONE_ATTRVALUE_LOCALITY , 			264, "Error", "System", "LIBBMDASN1_COMMON_UTILS_CLONE_ATTRVALUE_LOCALITY", "English", "Error in cloning locality attribute value"},
{LIBBMDASN1_COMMON_UTILS_CLONE_ATTRVALUE_ORGANIZATIONALUNIT , 		264, "Error", "System", "LIBBMDASN1_COMMON_UTILS_CLONE_ATTRVALUE_ORGANIZATIONALUNIT", "English", "Error in cloning organizational unit attribute value"},
{LIBBMDASN1_COMMON_UTILS_UNRECOGNIZED_TIME_TYPE , 			264, "Error", "System", "LIBBMDASN1_COMMON_UTILS_UNRECOGNIZED_TIME_TYPE", "English", "Unrecognized time type in attribute"},
{LIBBMDASN1_COMMON_UTILS_GET_VALUE_FROM_ATTRIBUTE , 			264, "Error", "System", "LIBBMDASN1_COMMON_UTILS_GET_VALUE_FROM_ATTRIBUTE", "English", "Error in getting Value from AttributeTypeAndValue"},
{LIBBMDASN1_COMMON_UTILS_GET_ATTVALUE_FROM_RDNSEQUENCE , 		264, "Error", "System", "LIBBMDASN1_COMMON_UTILS_GET_ATTVALUE_FROM_RDNSEQUENCE", "English", "Error in geeting attribute value from rdn sequence"},
{LIBBMDASN1_COMMON_UTILS_CONV_ATTRIBUTE_VALUE_TO_STR , 			264, "Error", "System", "LIBBMDASN1_COMMON_UTILS_CONV_ATTRIBUTE_VALUE_TO_STR", "English", "Error in converting rdn sequence to string"},
{LIBBMDASN1_COMMON_UTILS_CONV_EMAIL_TO_STRING_FROM_ATTRIBUTE , 		264, "Error", "System", "LIBBMDASN1_COMMON_UTILS_CONV_EMAIL_TO_STRING_FROM_ATTRIBUTE", "English", "Error in converting EmailAddress to string from attribute"},
{LIBBMDASN1_COMMON_UTILS_CONV_RELATIVE_DISTINGUISHED_NAME_TO_STR ,	264, "Error", "System", "LIBBMDASN1_COMMON_UTILS_CONV_RELATIVE_DISTINGUISHED_NAME_TO_STR", "English", "Error in converting RelativeDistinguishedName to string"},
{LIBBMDASN1_COMMON_UTILS_CONV_UTC_TIME_TO_STR , 			264, "Error", "System", "LIBBMDASN1_COMMON_UTILS_CONV_UTC_TIME_TO_STR", "English", "Error in converting UTC time to string"},
{LIBBMDASN1_COMMON_UTILS_CONV_GENERALIZED_TIME_TO_STR , 		264, "Error", "System", "LIBBMDASN1_COMMON_UTILS_CONV_GENERALIZED_TIME_TO_STR", "English", "Error in converting generalized time to string"},
{LIBBMDASN1_COMMON_UTILS_CONV_RDN_SEQUENCE_TO_STR , 			264, "Error", "System", "LIBBMDASN1_COMMON_UTILS_CONV_RDN_SEQUENCE_TO_STR", "English", "Error in converting rdn sequence to string"},
{LIBBMDASN1_COMMON_UTILS_CONV_TELETEX_STR_TO_STR_ERR , 			264, "Error", "System", "LIBBMDASN1_COMMON_UTILS_CONV_TELETEX_STR_TO_STR_ERR", "English", "Error in converting teletextstring to string"},
{LIBBMDASN1_COMMON_UTILS_CONV_PRINTABLE_STR_TO_STR_ERR , 		264, "Error", "System", "LIBBMDASN1_COMMON_UTILS_CONV_PRINTABLE_STR_TO_STR_ERR", "English", "Error in converting printablestring to string"},
{LIBBMDASN1_COMMON_UTILS_CONV_UNIVERSAL_STR_TO_STR_ERR , 		264, "Error", "System", "LIBBMDASN1_COMMON_UTILS_CONV_UNIVERSAL_STR_TO_STR_ERR", "English", "Error in converting universalstring to string"},
{LIBBMDASN1_COMMON_UTILS_CONV_UTF8_STR_TO_STR_ERR , 			264, "Error", "System", "LIBBMDASN1_COMMON_UTILS_CONV_UTF8_STR_TO_STR_ERR", "English", "Error in converting universalstring to string"},
{LIBBMDASN1_COMMON_UTILS_CONV_BMD_STR_TO_STR_ERR , 			264, "Error", "System", "LIBBMDASN1_COMMON_UTILS_CONV_BMD_STR_TO_STR_ERR", "English", "Error in converting bmdstring to string"},
{LIBBMDASN1_COMMON_UTILS_CONV_ATTRIBUTE_VALUE_TO_DIRECTORY_STR , 	264, "Error", "System", "LIBBMDASN1_COMMON_UTILS_CONV_ATTRIBUTE_VALUE_TO_DIRECTORY_STR", "English", "Error in converting AttributeValue to DirectoryString"},
{LIBBMDASN1_COMMON_UTILS_CONV_DIRECTORY_STR_TO_STR , 			264, "Error", "System", "LIBBMDASN1_COMMON_UTILS_CONV_DIRECTORY_STR_TO_STR", "English", "Error in converting DirectoryString to string"},
{LIBBMDASN1_COMMON_UTILS_CONV_ANOTHER_NAME_TO_STR , 			264, "Error", "System", "LIBBMDASN1_COMMON_UTILS_CONV_ANOTHER_NAME_TO_STR", "English", "Error in converting another name to string"},
{LIBBMDASN1_COMMON_UTILS_CONV_IA5_STR_TO_STR , 				264, "Error", "System", "LIBBMDASN1_COMMON_UTILS_CONV_IA5_STR_TO_STR", "English", "Error in converting IA5String to string"},
{LIBBMDASN1_COMMON_UTILS_CONV_OR_ADDRESS_TO_STR , 			264, "Error", "System", "LIBBMDASN1_COMMON_UTILS_CONV_OR_ADDRESS_TO_STR", "English", "Error in converting ORAddress to string"},
{LIBBMDASN1_COMMON_UTILS_CONV_NAME_TO_STR , 				264, "Error", "System", "LIBBMDASN1_COMMON_UTILS_CONV_NAME_TO_STR", "English", "Error in converting Name to string"},
{LIBBMDASN1_COMMON_UTILS_CONV_EDI_PARTY_NAME_TO_STR , 			264, "Error", "System", "LIBBMDASN1_COMMON_UTILS_CONV_EDI_PARTY_NAME_TO_STR", "English", "Error in converting EDIPartyName to string"},
{LIBBMDASN1_COMMON_UTILS_CONV_UNIFORM_RESOURCE_IDENTIFIER_TO_STR ,	264, "Error", "System", "LIBBMDASN1_COMMON_UTILS_CONV_UNIFORM_RESOURCE_IDENTIFIER_TO_STR", "English", "Error in converting uniform resource identifier to string"},
{LIBBMDASN1_COMMON_UTILS_CONV_IP_ADDRESS_TO_STR , 			264, "Error", "System", "LIBBMDASN1_COMMON_UTILS_CONV_IP_ADDRESS_TO_STR", "English", "Error in converting ip adress to string"},
{LIBBMDASN1_COMMON_UTILS_CONV_REGISTERED_ID_TO_STR , 			264, "Error", "System", "LIBBMDASN1_COMMON_UTILS_CONV_REGISTERED_ID_TO_STR", "English", "Error in converting registered id to string"},
{LIBBMDASN1_COMMON_UTILS_CONV_DISTRIBUTION_POINT_TO_STR , 		264, "Error", "System", "LIBBMDASN1_COMMON_UTILS_CONV_DISTRIBUTION_POINT_TO_STR", "English", "Error in converting DistributionPoint to string"},
{LIBBMDASN1_COMMON_UTILS_CONV_DIST_PNT_FULL_NAME_STR , 			264, "Error", "System", "LIBBMDASN1_COMMON_UTILS_CONV_DIST_PNT_FULL_NAME_STR", "English", "Error in converting distinguished point full name to string"},
{LIBBMDASN1_COMMON_UTILS_CONV_DIST_PNT_NAME_REL_TO_CRL_ISSUER_STR,	264, "Error", "System", "LIBBMDASN1_COMMON_UTILS_CONV_DIST_PNT_NAME_REL_TO_CRL_ISSUER_STR", "English", "Error in converting distinguished point name relative to CRL issuer to string"},
{LIBBMDASN1_COMMON_UTILS_CONV_INTEGER_TO_LONG , 			264, "Error", "System", "LIBBMDASN1_COMMON_UTILS_CONV_INTEGER_TO_LONG", "English", "Error in converting asn1 integer to long"},
{LIBBMDASN1_COMMON_UTILS_CONV_LONG_TO_INTEGER ,				264, "Error", "System", "LIBBMDASN1_COMMON_UTILS_CONV_LONG_TO_INTEGER", "English", "Error in converting asn1 integer to long"},
{LIBBMDASN1_COMMON_UTILS_CONV_STR_TO_OID , 				264, "Error", "System", "LIBBMDASN1_COMMON_UTILS_CONV_STR_TO_OID", "English", "Error in converting string to oid"},
{LIBBMDASN1_COMMON_UTILS_CONV_OCTET_STR_TO_STR , 			264, "Error", "System", "LIBBMDASN1_COMMON_UTILS_CONV_OCTET_STR_TO_STR", "English", "Error in converting octet string to string"},
{LIBBMDASN1_COMMON_UTILS_CONV_STR_TO_OCTET_STR , 			264, "Error", "System", "LIBBMDASN1_COMMON_UTILS_CONV_STR_TO_OCTET_STR", "English", "Error in converting string to octet string"},
{LIBBMDASN1_COMMON_UTILS_CONV_GEN_BUF_TO_OCTET_STRING , 		264, "Error", "System", "LIBBMDASN1_COMMON_UTILS_CONV_GEN_BUF_TO_OCTET_STRING", "English", "Error in converting octet string to string"},
{LIBBMDASN1_COMMON_UTILS_CONV_STR_TO_ANY , 				264, "Error", "System", "LIBBMDASN1_COMMON_UTILS_CONV_STR_TO_ANY", "English", "Error in converting string to any buffer"},
{LIBBMDASN1_COMMON_UTILS_DECODE_UTF8_STRING_ERR , 			264, "Error", "System", "LIBBMDASN1_COMMON_UTILS_DECODE_UTF8_STRING_ERR", "English", "Error in ber decoding utf8 string"},
{LIBBMDASN1_COMMON_UTILS_DECODE_INTEGER_ERR , 				264, "Error", "System", "LIBBMDASN1_COMMON_UTILS_DECODE_INTEGER_ERR", "English", "Error in ber decoding integer"},
{LIBBMDASN1_COMMON_UTILS_DECODE_DIRECTORY_STRING_DATA_ERR , 		264, "Error", "System", "LIBBMDASN1_COMMON_UTILS_DECODE_DIRECTORY_STRING_DATA_ERR", "English", "Error in decoding directory string data"},
{LIBBMDASN1_COMMON_UTILS_DER_ENCODING_ERR , 				264, "Error", "System", "LIBBMDASN1_COMMON_UTILS_DER_ENCODING_ERR", "English", "Error in der ecoding"},
{LIBBMDASN1_COMMON_UTILS_DECODE_POSTAL_ADDRESS_DATA_ERR , 		264, "Error", "System", "LIBBMDASN1_COMMON_UTILS_DECODE_POSTAL_ADDRESS_DATA_ERR", "English", "Error in decoding postal address data"},
{LIBBMDASN1_COMMON_UTILS_DECODE_IA5_DATA_ERR , 				264, "Error", "System", "LIBBMDASN1_COMMON_UTILS_DECODE_IA5_DATA_ERR", "English", "Error in decoding IA5 string data"},
{LIBBMDASN1_COMMON_UTILS_DECODE_CRL_DISTPOINTS_DATA_ERR , 		264, "Error", "System", "LIBBMDASN1_COMMON_UTILS_DECODE_CRL_DISTPOINTS_DATA_ERR", "English", "Error in decoding crl distinguished points string data"},
{LIBBMDASN1_COMMON_UTILS_GET_VALUE_FROM_ATT_TYPE_AND_VALUE , 		264, "Error", "System", "LIBBMDASN1_COMMON_UTILS_GET_VALUE_FROM_ATT_TYPE_AND_VALUE", "English", "Error in getting value from attribute type and value"},
{LIBBMDASN1_COMMON_UTILS_ATTRIBUTE_TYPE_NOT_PRESENT , 			264, "Error", "System", "LIBBMDASN1_COMMON_UTILS_ATTRIBUTE_TYPE_NOT_PRESENT", "English", "Attribute type not present in rdn sequence"},
{LIBBMDPR_DATAGRAMSET_TO_SMALL ,           				264, "Error", "System", "LIBBMDPR_DATAGRAMSET_TO_SMALL", "English", 			"Datagramset to small for deserialisation"},

{LIBBMDCONFIG_LIBBMDCONFIG_GET_CONFIG_VALUE_ERR , 			264, "Error", "System", "LIBBMDCONFIG_LIBBMDCONFIG_GET_CONFIG_VALUE_ERR", "English", 	"Error in getting configuration value"},
{LIBBMDCONFIG_LIBBMDCONFIG_INVALID_CONF_FILE_PATH , 			264, "Error", "System", "LIBBMDCONFIG_LIBBMDCONFIG_INVALID_CONF_FILE_PATH", "English", 	"Invalid configuration file path"},
{LIBBMDCONFIG_LIBBMDCONFIG_EMPTY_CONF_STRUCTURE , 			264, "Error", "System", "LIBBMDCONFIG_LIBBMDCONFIG_EMPTY_CONF_STRUCTURE", "English", 	"Empty configuration structure option"},
{LIBBMDCONFIG_LIBBMDCONFIG_OPEN_CONF_FILE_ERR , 			264, "Error", "System", "LIBBMDCONFIG_LIBBMDCONFIG_OPEN_CONF_FILE_ERR", "English", 	"Error in openning configuration file"},
{LIBBMDCONFIG_LIBBMDCONFIG_CLOSE_CONF_FILE_ERR , 			264, "Error", "System", "LIBBMDCONFIG_LIBBMDCONFIG_CLOSE_CONF_FILE_ERR", "English", 	"Error in openning configuration file"},
{LIBBMDCONFIG_LIBBMDCONFIG_PARSE_CONF_FILE_ERR , 			264, "Error", "System", "LIBBMDCONFIG_LIBBMDCONFIG_PARSE_CONF_FILE_ERR", "English", 	"Error in parsing configration file"},
{LIBBMDCONFIG_LIBBMDCONFIG_VALIDATE_CONF_FILE_ERR , 			264, "Error", "System", "LIBBMDCONFIG_LIBBMDCONFIG_VALIDATE_CONF_FILE_ERR", "English", 	"Error in validating configration file"},

{LIBBMDCONFIG_UPDATE_FILE_OPEN_SRC_FILE_ERR , 				264, "Error", "System", "LIBBMDCONFIG_UPDATE_FILE_OPEN_SRC_FILE_ERR", "English", 	"Error in openning src file for config copy"},
{LIBBMDCONFIG_UPDATE_FILE_WRITE_DEST_FILE_ERR , 			264, "Error", "System", "LIBBMDCONFIG_UPDATE_FILE_WRITE_DEST_FILE_ERR", "English", 	"Error in writing to dest file in config copy"},

{LIBBMD_LICENCE_REQUIRED_AUTH_PLUGIN_ERR , 				264, "Error", "System", "LIBBMD_LICENCE_REQUIRED_AUTH_PLUGIN_ERR", "English", 		"Server licence requires authorization plugin"},
{LIBBMD_LICENCE_REQUIRED_CONSERVATE_PLUGIN_ERR , 			264, "Error", "System", "LIBBMD_LICENCE_REQUIRED_CONSERVATE_PLUGIN_ERR", "English", 	"Server licence requires conservate plugin"},
{LIBBMD_LICENCE_REQUIRED_DVCS_PLUGIN_ERR , 				264, "Error", "System", "LIBBMD_LICENCE_REQUIRED_DVCS_PLUGIN_ERR", "English", 		"Server licence requires dvcs plugin"},
{LIBBMD_LICENCE_REQUIRED_SESSION_PLUGIN_ERR , 				264, "Error", "System", "LIBBMD_LICENCE_REQUIRED_SESSION_PLUGIN_ERR", "English", 	"Server licence requires session plugin"},
{LIBBMD_LICENCE_REQUIRED_CLEAR_PLUGIN_ERR , 				264, "Error", "System", "LIBBMD_LICENCE_REQUIRED_CLEAR_PLUGIN_ERR", "English", 		"Server licence requires clear plugin"},
{LIBBMD_LICENCE_REQUIRED_MESSAGING_PLUGIN_ERR , 			264, "Error", "System", "LIBBMD_LICENCE_REQUIRED_MESSAGING_PLUGIN_ERR", "English", 	"Server licence requires messaging plugin"},
{LIBBMD_LICENCE_REQUIRED_ARCHIVING_PLUGIN_ERR , 			264, "Error", "System", "LIBBMD_LICENCE_REQUIRED_ARCHIVING_PLUGIN_ERR", "English", 	"Server licence requires archiving plugin"},


{LIBBMDUTILS_INCORRECT_YEAR_VALUE , 			264, "Error", "System", "LIBBMDUTILS_INCORRECT_YEAR_VALUE", "English", "Incorrect year value."},
{LIBBMDUTILS_INCORRECT_MONTH_VALUE , 			264, "Error", "System", "LIBBMDUTILS_INCORRECT_MONTH_VALUE", "English", "Incorrect month value."},
{LIBBMDUTILS_INCORRECT_DAY_VALUE , 			264, "Error", "System", "LIBBMDUTILS_INCORRECT_DAY_VALUE", "English", "Incorrect day value."},
{LIBBMDUTILS_INCORRECT_HOUR_VALUE , 			264, "Error", "System", "LIBBMDUTILS_INCORRECT_HOUR_VALUE", "English", "Incorrect hour value."},
{LIBBMDUTILS_INCORRECT_MINUTE_VALUE , 			264, "Error", "System", "LIBBMDUTILS_INCORRECT_MINUTE_VALUE", "English", "Incorrect minute value."},
{LIBBMDUTILS_INCORRECT_SECOND_VALUE , 			264, "Error", "System", "LIBBMDUTILS_INCORRECT_SECOND_VALUE", "English", "Incorrect second (time) value."},
{LIBBMDUTILS_INCORRECT_DATE_TIME_FORMAT ,		264, "Error", "System", "LIBBMDUTILS_INCORRECT_DATE_TIME_FORMAT", "English", "Incorrect date-time format (required 'yyyy-mm-dd hh:mm:ss' format)."},
{LIBBMDUTILS_DATE_TIME_NOT_AFTER_NOW ,			264, "Error", "System", "LIBBMDUTILS_DATE_TIME_NOT_AFTER_NOW", "English", "Incorrect date (required date in the future)."},
{LIBBMDUTILS_NOT_NUMERIC_IDENTIFIER ,			264, "Error", "System", "LIBBMDUTILS_NOT_NUMERIC_IDENTIFIER", "English", "Incorrect identifier format. Pass numeric identifier."},


{LIBBMDSQL_GET_SYMKEY_GET_ERR , 					264, "Error", "System", "LIBBMDSQL_GET_SYMKEY_GET_ERR", "English", 			"Error in getting symmetric key from datagram during servicing get request"},
{LIBBMDSQL_GET_SYMKEY_DECODE_ERR , 					264, "Error", "System", "LIBBMDSQL_GET_SYMKEY_DECODE_ERR", "English", 			"Error in decoding symmetric key during servicing get request"},
{LIBBMDSQL_GET_SET_SYMKEY_CTX_ERR , 					264, "Error", "System", "LIBBMDSQL_GET_SET_SYMKEY_CTX_ERR", "English", 			"Error in setting symmetric key context during servicing get request"},
{LIBBMDSQL_GET_CREATE_DER_SYMKEY_CTX_ERR , 				264, "Error", "System", "LIBBMDSQL_GET_CREATE_DER_SYMKEY_CTX_ERR", "English", 		"Error in creating der from symmetric key during servicing get request"},
{LIBBMDSQL_GET_UPDATE_DER_SYMKEY_CTX_ERR , 				264, "Error", "System", "LIBBMDSQL_GET_UPDATE_DER_SYMKEY_CTX_ERR", "English", 		"Error in updating der from symmetric key during servicing get request"},
{LIBBMDSQL_GET_DELETE_DER_SYMKEY_CTX_ERR , 				264, "Error", "System", "LIBBMDSQL_GET_DELETE_DER_SYMKEY_CTX_ERR", "English",		"Error in deleting der from symmetric key during servicing get request"},
{LIBBMDSQL_GET_FILL_RESPONSE_DTG_WITH_DTGS_ERR , 			264, "Error", "System", "LIBBMDSQL_GET_FILL_RESPONSE_DTG_WITH_DTGS_ERR", "English", 	"Error in filling response datagramset during servicing get request"},
{LIBBMDSQL_GET_SYMMETRIC_DECRYPT_ERR , 					264, "Error", "System", "LIBBMDSQL_GET_SYMMETRIC_DECRYPT_ERR", "English", 		"Error in data symmetric decryption during servicing get request"},
{LIBBMDSQL_GET_DOCUMENT_ABSENT_IN_DB_ERR , 				264, "Error", "System", "LIBBMDSQL_GET_DOCUMENT_ABSENT_IN_DB_ERR", "English", 		"File requested to get not found in archive (possibly deleted)"},

{LIBBMDSQL_PROOFS_PARSE_SIGNATURE_STANDARD_ERR,				264, "Error", "System", "LIBBMDSQL_PROOFS_PARSE_SIGNATURE_STANDARD_ERR", "English", 	"Error in parsing timestamps standard"},
{LIBBMDSQL_PROOFS_PARSE_CERTIFICATE_VERSION_ERR,			264, "Error", "System", "LIBBMDSQL_PROOFS_PARSE_CERTIFICATE_VERSION_ERR", "English", 	"Error in parsing certificates version"},
{LIBBMDSQL_PROOFS_PARSE_CERTIFICATE_SERIAL_ERR,				264, "Error", "System", "LIBBMDSQL_PROOFS_PARSE_CERTIFICATE_SERIAL_ERR", "English", 	"Error in parsing certificates serial number"},
{LIBBMDSQL_PROOFS_PARSE_CERTIFICATE_SUBJECT_DN_ERR,			264, "Error", "System", "LIBBMDSQL_PROOFS_PARSE_CERTIFICATE_SUBJECT_DN_ERR", "English", "Error in parsing certificate subject dn number"},
{LIBBMDSQL_PROOFS_PARSE_CERTIFICATE_SIG_ALG_ERR,			264, "Error", "System", "LIBBMDSQL_PROOFS_PARSE_CERTIFICATE_SIG_ALG_ERR", "English", 	"Error in parsing certificate signatures algorithm"},
{LIBBMDSQL_PROOFS_PARSE_CERTIFICATE_ISSUER_ERR,				264, "Error", "System", "LIBBMDSQL_PROOFS_PARSE_CERTIFICATE_ISSUER_ERR", "English", 	"Error in parsing certificates issuer"},
{LIBBMDSQL_PROOFS_PARSE_CERTIFICATE_ISSUER_SN_ERR,			264, "Error", "System", "LIBBMDSQL_PROOFS_PARSE_CERTIFICATE_ISSUER_SN_ERR", "English", 	"Error in parsing certificate issuers serial number"},
{LIBBMDSQL_PROOFS_PARSE_CERTIFICATE_VALIDITY_NB_ERR,			264, "Error", "System", "LIBBMDSQL_PROOFS_PARSE_CERTIFICATE_VALIDITY_NB_ERR", "English", "Error in parsing certificate (not before) validity"},
{LIBBMDSQL_PROOFS_PARSE_CERTIFICATE_VALIDITY_NA_ERR,			264, "Error", "System", "LIBBMDSQL_PROOFS_PARSE_CERTIFICATE_VALIDITY_NA_ERR", "English", "Error in parsing certificate (not after) validity"},
{LIBBMDSQL_PROOFS_INSERT_SIGNATURE_TO_DB_ERR,				264, "Error", "System", "LIBBMDSQL_PROOFS_INSERT_SIGNATURE_TO_DB_ERR", "English", 	"Error in inserting signature to database"},
{LIBBMDSQL_PROOFS_INSERT_SIGNATURE_LOB_TO_DB_ERR,			264, "Error", "System", "LIBBMDSQL_GET_SYMMETRIC_DECRYPT_ERR", "English", 		"Error in inserting signatures lob to database"},
{LIBBMDSQL_PROOFS_INSERT_SIGNATURE_TABLE_TO_DB_ERR,			264, "Error", "System", "LIBBMDSQL_PROOFS_INSERT_SIGNATURE_LOB_TO_DB_ERR", "English", 	"Error in inserting signatures info to prf_signature table in database"},
{LIBBMDSQL_PROOFS_INSERT_CERTIFICATE_TO_DB_ERR,				264, "Error", "System", "LIBBMDSQL_PROOFS_INSERT_CERTIFICATE_TO_DB_ERR", "English", 	"Error in inserting certificate to database"},
{LIBBMDSQL_PROOFS_INSERT_CERTIFICATE_LOB_TO_DB_ERR,			264, "Error", "System", "LIBBMDSQL_PROOFS_INSERT_CERTIFICATE_LOB_TO_DB_ERR", "English", "Error in inserting certificates lob to database"},
{LIBBMDSQL_PROOFS_INSERT_CERTIFICATE_TABLE_TO_DB_ERR,			264, "Error", "System", "LIBBMDSQL_PROOFS_INSERT_CERTIFICATE_TABLE_TO_DB_ERR", "English", "Error in inserting certificates info to prf_signature table in database"},
{LIBBMDSQL_PROOFS_INSERT_CERTIFICATE_ALREADY_EXISTS_ERR,		264, "Error", "System", "LIBBMDSQL_PROOFS_INSERT_CERTIFICATE_ALREADY_EXISTS_ERR", "English", "Certificate already exists in database"},

{LIBBMDSQL_PRF_SIGNATURE_PARSE_SIGNATURE_STANDARD_ERR,			264, "Error", "System", "LIBBMDSQL_PRF_SIGNATURE_PARSE_SIGNATURE_STANDARD_ERR", "English", "Error in parsing signatures standard"},

{LIBBMDSQL_SELECT_MAIN_CREATE_DATAGRAM_ERR,				264, "Error", "System", "LIBBMDSQL_SELECT_MAIN_CREATE_DATAGRAM_ERR", "English", "Error in creating datagram during select request"},
{LIBBMDSQL_SELECT_MAIN_FILL_WITH_CRYPTO_OBJECTS_DATA_ERR,		264, "Error", "System", "LIBBMDSQL_SELECT_MAIN_FILL_WITH_CRYPTO_OBJECTS_DATA_ERR", "English", "Error in filling response with crypto objects data"},
{LIBBMDSQL_SELECT_MAIN_FILL_WITH_METADATA_DATA_ERR,			264, "Error", "System", "LIBBMDSQL_SELECT_MAIN_FILL_WITH_METADATA_DATA_ERR", "English", "Error in filling response with metadata data"},

{LIBBMDSQL_INSERT_GET_ALL_METADATA_FROM_ID_ERR,				264, "Error", "System", "LIBBMDSQL_INSERT_GET_ALL_METADATA_FROM_ID_ERR", "English", "Error in getting all metadata according to docuuments id"},
{LIBBMDSQL_INSERT_REQUIRED_LOB_NOT_PRESENT_IN_REQUEST_ERR,		264, "Error", "System", "LIBBMDSQL_INSERT_REQUIRED_LOB_NOT_PRESENT_IN_REQUEST_ERR", "English", "Requiered lob data not present in request datagram"},
{LIBBMDSQL_INSERT_INSERT_FILE_LOB_TO_DB_ERR,				264, "Error", "System", "LIBBMDSQL_INSERT_INSERT_FILE_LOB_TO_DB_ERR", "English", "Error in inserting documents lob to database"},
{LIBBMDSQL_INSERT_GET_SYMKEY_FROM_DATAGRAM_ERR,				264, "Error", "System", "LIBBMDSQL_INSERT_GET_SYMKEY_FROM_DATAGRAM_ERR", "English", "Error in getting symmetric key from datagram"},
{LIBBMDSQL_INSERT_DECODE_SYMKEY_ERR,					264, "Error", "System", "LIBBMDSQL_INSERT_DECODE_SYMKEY_ERR", "English", "Error in decoding symmetric key"},
{LIBBMDSQL_INSERT_DECRYPT_PROTOCOL_DATA_ERR,				264, "Error", "System", "LIBBMDSQL_INSERT_DECRYPT_PROTOCOL_DATA_ERR", "English", "Error in decrypting protocol data"},
{LIBBMDSQL_INSERT_SET_HASH_CONTEXT_ERR,					264, "Error", "System", "LIBBMDSQL_INSERT_SET_HASH_CONTEXT_ERR", "English", "Error in setting hash context"},
{LIBBMDSQL_INSERT_SET_HASH_ERR,						264, "Error", "System", "LIBBMDSQL_INSERT_SET_HASH_ERR", "English", "Error in hashing data"},
{LIBBMDSQL_INSERT_CREATE_LOB_TRANSPORT_INFO_ERR,			264, "Error", "System", "LIBBMDSQL_INSERT_CREATE_LOB_TRANSPORT_INFO_ERR", "English", "Error in creating large object transport information"},
{LIBBMDSQL_INSERT_RECEIVELOB_CHUNK_ERR,					264, "Error", "System", "LIBBMDSQL_INSERT_RECEIVELOB_CHUNK_ERR", "English", "Error in receive large object chunk"},
{LIBBMDSQL_INSERT_START_LOB_TRANSPORT_TO_DB_ERR,			264, "Error", "System", "LIBBMDSQL_INSERT_START_LOB_TRANSPORT_TO_DB_ERR", "English", "Error in beginning large object transport into database"},
{LIBBMDSQL_INSERT_END_LOB_TRANSPORT_TO_DB_ERR,				264, "Error", "System", "LIBBMDSQL_INSERT_END_LOB_TRANSPORT_TO_DB_ERR", "English", "Error in ending large object transport into database"},
{LIBBMDSQL_INSERT_ITERATE_LOB_TRANSPORT_TO_DB_ERR,			264, "Error", "System", "LIBBMDSQL_INSERT_ITERATE_LOB_TRANSPORT_TO_DB_ERR", "English", "Error in iterate large object transport into database"},
{LIBBMDSQL_INSERT_ADD_HASH_TO_DATAGRAM_ERR,				264, "Error", "System", "LIBBMDSQL_INSERT_ADD_HASH_TO_DATAGRAM_ERR", "English", "Error in adding hash to datagram"},
{LIBBMDSQL_INSERT_NO_POINTING_ID_IN_LINK_CREATION,			264, "Error", "System", "LIBBMDSQL_INSERT_NO_POINTING_ID_IN_LINK_CREATION", "English", "Pointing id missing in symbolic link creation request"},
{LIBBMDSQL_INSERT_NO_POINTING_LOCATION_ID_IN_LINK_CREATION,		264, "Error", "System", "LIBBMDSQL_INSERT_NO_POINTING_LOCATION_ID_IN_LINK_CREATION", "English", "Pointing location id missing in symbolic link creation request"},
{LIBBMDSQL_INSERT_FILE_TO_LINK_TO_NOT_FOUND,				264, "Error", "System", "LIBBMDSQL_INSERT_FILE_TO_LINK_TO_NOT_FOUND", "English", "File to create symbolic link to not found in database"},
{LIBBMDSQL_INSERT_DIRECTORY_ALREADY_EXIST,				264, "Error", "System", "LIBBMDSQL_INSERT_DIRECTORY_ALREADY_EXIST", "English", "Directory with given name already exists"},
{LIBBMDSQL_INSERT_FILE_TO_LINK_IS_LINK,					264, "Error", "System", "LIBBMDSQL_INSERT_FILE_TO_LINK_IS_LINK", "English", "Unable to create link to link"},
{LIBBMDSQL_INSERT_UNALLOWABLE_DIR_FILE_TYPE,				264, "Error", "System", "LIBBMDSQL_INSERT_UNALLOWABLE_DIR_FILE_TYPE", "English", "Unallowable file type value - 'dir' value is reserved."},
{LIBBMDSQL_INSERT_UNALLOWABLE_LINK_FILE_TYPE,				264, "Error", "System", "LIBBMDSQL_INSERT_UNALLOWABLE_LINK_FILE_TYPE", "English", "Unallowable file type value - 'link' value is reserved."},
{LIBBMDSQL_INSERT_INCORRECT_FOR_GRANT_VALUE,				264, "Error", "System", "LIBBMDSQL_INSERT_INCORRECT_FOR_GRANT_VALUE", "English", "Incorrect value of system metadata (for_grant)."},
{LIBBMDSQL_NO_CASCADE_LINK_CREATED,					264, "Error", "System", "LIBBMDSQL_NO_CASCADE_LINK_CREATED", "English", "No cascade link was created."},
{LIBBMDSQL_NO_LINK_CREATED,							264, "Error", "System", "LIBBMDSQL_NO_LINK_CREATED", "English", "No link was created."},
{LIBBMDSQL_INCORRECT_VISIBILITY_VALUE,					264, "Error", "System", "LIBBMDSQL_INCORRECT_VISIBILITY_VALUE", "English", "Incorrect visibility value"},
{LIBBMDSQL_NO_METADATA_FOR_UPDATE,					264, "Error", "System", "LIBBMDSQL_NO_METADATA_FOR_UPDATE", "English", "No metadata for update."},
{LIBBMDSQL_NOT_REGULAR_FILE_ID,					264, "Error", "System", "LIBBMDSQL_NOT_REGULAR_FILE_ID", "English", "Required regular file id (not link)."},

{LIBBMDSQL_PADES_HASH_VALUE_NOT_SPECIFIED,					264, "Error", "System", "LIBBMDSQL_PADES_HASH_VALUE_NOT_SPECIFIED", "English", "Hash value metadata (mandatory for PAdES) is not specified."},
{LIBBMDSQL_PADES_HASH_ALGORITHM_NOT_SPECIFIED,					264, "Error", "System", "LIBBMDSQL_PADES_HASH_ALGORITHM_NOT_SPECIFIED", "English", "Hash algorithm metadata (mandatory for PAdES) is not specified."},
{LIBBMDSQL_PADES_TOO_MANY_HASH_VALUE_METADATA,					264, "Error", "System", "LIBBMDSQL_PADES_TOO_MANY_HASH_VALUE_METADATA", "English", "Only one occurrency of hash value metadata for PAdES is allowed."},
{LIBBMDSQL_PADES_TOO_MANY_HASH_ALGORITHM_METADATA,					264, "Error", "System", "LIBBMDSQL_PADES_TOO_MANY_HASH_ALGORITHM_METADATA", "English", "Only one occurrency of hash algorithm metadata for PAdES is allowed."},
{LIBBMDSQL_PADES_INCORRECT_HASH_SIZE,					264, "Error", "System", "LIBBMDSQL_PADES_INCORRECT_HASH_SIZE", "English", "Incorrect hash value size."},
{LIBBMDSQL_PADES_INCORRECT_HEX_HASH_SIZE,					264, "Error", "System", "LIBBMDSQL_PADES_INCORRECT_HEX_HASH_SIZE", "English", "Incorrect hex hash value size."},
{LIBBMDSQL_PADES_NEW_CRYPTO_OBJECT_NOT_SPECIFIED,					264, "Error", "System", "LIBBMDSQL_PADES_NEW_CRYPTO_OBJECT_NOT_SPECIFIED", "English", "PAdES for replacement is not specified (when adding signature)."},
{LIBBMDSQL_PADES_TOO_MANY_NEW_CRYPTO_OBJECT,					264, "Error", "System", "LIBBMDSQL_PADES_TOO_MANY_NEW_CRYPTO_OBJECT", "English", "Only one PAdES for replacement is allowed (when adding signature)."},
{LIBBMDSQL_ONLY_ONE_SIGNATURE_ALLOWED,						264, "Error", "System", "LIBBMDSQL_ONLY_ONE_SIGNATURE_ALLOWED", "English", "Document has been signed already. Only one signature is allowed per file."},

{BMD_ERR_FILE_OWNER_NOT_SPECIFIED,					264, "Error", "System", "BMD_ERR_FILE_OWNER_NOT_SPECIFIED", "English", "File owner is not specified."},
{BMD_ERR_FILE_ID_NOT_SPECIFIED,						264, "Error", "System", "BMD_ERR_FILE_ID_NOT_SPECIFIED", "English", "File id is not specified."},
{BMD_ERR_CURRENT_VERSION_FILE_ID_NOT_SPECIFIED,			264, "Error", "System", "BMD_ERR_CURRENT_VERSION_FILE_ID_NOT_SPECIFIED", "English", "Current version file id is not specified."},


{LIBBMDSQL_DICT_CONV_METADATA_TO_GROUP_ID_ERR,				264, "Error", "System", "LIBBMDSQL_DICT_CONV_METADATA_TO_GROUP_ID_ERR", "English", "Error in converting metadata to users group id"},
{LIBBMDSQL_DICT_CONV_METADATA_TO_ROLE_ERR,				264, "Error", "System", "LIBBMDSQL_DICT_CONV_METADATA_TO_ROLE_ERR", "English", "Error in converting metadata to users role"},
{LIBBMDSQL_DICT_CONV_METADATA_TO_SEC_CATEGORY_ERR,			264, "Error", "System", "LIBBMDSQL_DICT_CONV_METADATA_TO_SEC_CATEGORY_ERR", "English", "Error in converting metadata to users security category"},
{LIBBMDSQL_DICT_CONV_METADATA_TO_SEC_LEVEL_ERR,				264, "Error", "System", "LIBBMDSQL_DICT_CONV_METADATA_TO_SEC_LEVEL_ERR", "English", "Error in converting metadata to users security category"},
{LIBBMDSQL_DICT_CONV_DICT_TYPE_TO_ID_ERR,				264, "Error", "System", "LIBBMDSQL_DICT_CONV_DICT_TYPE_TO_ID_ERR", "English", "Error in converting dictonary type to id"},
{LIBBMDSQL_DICT_CREATE_DICT_FOR_DOC_TYPES_ERR,				264, "Error", "System", "LIBBMDSQL_DICT_CREATE_DICT_FOR_DOC_TYPES_ERR", "English", "Error in creating dictonary for document types"},
{LIBBMDSQL_DICT_CREATE_DICT_FOR_SOFTWARE_TYPES_ERR,			264, "Error", "System", "LIBBMDSQL_DICT_CREATE_DICT_FOR_SOFTWARE_TYPES_ERR", "English", "Error in creating dictonary for software types"},
{LIBBMDSQL_DICT_CREATE_DICT_FOR_METADATA_TYPES_ERR,			264, "Error", "System", "LIBBMDSQL_DICT_CREATE_DICT_FOR_METADATA_TYPES_ERR", "English", "Error in creating dictonary for metadata types"},
{LIBBMDSQL_DICT_CREATE_DICT_FOR_METADATA_TYPE_NAMES_ERR,		264, "Error", "System", "LIBBMDSQL_DICT_CREATE_DICT_FOR_METADATA_TYPE_NAMES_ERR", "English", "Error in creating dictonary for metadata type names"},
{LIBBMDSQL_DICT_CREATE_DICT_FOR_PKI_TYPES_ERR,				264, "Error", "System", "LIBBMDSQL_DICT_CREATE_DICT_FOR_PKI_TYPES_ERR", "English", "Error in creating dictonary for pki types"},
{LIBBMDSQL_DICT_CREATE_DICT_FOR_GROUPS_ERR,				264, "Error", "System", "LIBBMDSQL_DICT_CREATE_DICT_FOR_GROUPS_ERR", "English", "Error in creating dictonary for groups types"},
{LIBBMDSQL_DICT_CREATE_DICT_FOR_ROLES_ERR,				264, "Error", "System", "LIBBMDSQL_DICT_CREATE_DICT_FOR_ROLES_ERR", "English", "Error in creating dictonary for roles types"},
{LIBBMDSQL_DICT_CREATE_DICT_FOR_SECURITY_LEVELS_ERR,			264, "Error", "System", "LIBBMDSQL_DICT_CREATE_DICT_FOR_SECURITY_LEVELS_ERR", "English", "Error in creating dictonary for security levels"},
{LIBBMDSQL_DICT_CREATE_DICT_FOR_SECURITY_CATEGORIES_ERR,		264, "Error", "System", "LIBBMDSQL_DICT_CREATE_DICT_FOR_SECURITY_CATEGORIES_ERR", "English", "Error in creating dictonary for securty categories"},
{LIBBMDSQL_DICT_CREATE_DICT_FOR_RELATION_TYPES_ERR,			264, "Error", "System", "LIBBMDSQL_DICT_CREATE_DICT_FOR_RELATION_TYPES_ERR", "English", "Error in creating dictonary for relation types"},
{LIBBMDSQL_DICT_CREATE_DICT_FOR_GROUPS2_ERR,				264, "Error", "System", "LIBBMDSQL_DICT_CREATE_DICT_FOR_GROUPS2_ERR", "English", "Error in creating dictonary for groups2"},
{LIBBMDSQL_DICT_CREATE_DICT_FOR_SECURITY_CATEGORIES2_ERR,		264, "Error", "System", "LIBBMDSQL_DICT_CREATE_DICT_FOR_SECURITY_CATEGORIES2_ERR", "English", "Error in creating dictonary for securty categories2"},
{LIBBMDSQL_DICT_DUMP_DICT_FOR_DOC_TYPES_ERR,				264, "Error", "System", "LIBBMDSQL_DICT_DUMP_DICT_FOR_DOC_TYPES_ERR", "English", "Error in dumping dictonary for document types"},
{LIBBMDSQL_DICT_DUMP_DICT_FOR_SOFTWARE_TYPES_ERR,			264, "Error", "System", "LIBBMDSQL_DICT_DUMP_DICT_FOR_SOFTWARE_TYPES_ERR", "English", "Error in dumping dictonary for software types"},
{LIBBMDSQL_DICT_DUMP_DICT_FOR_METADATA_TYPES_ERR,			264, "Error", "System", "LIBBMDSQL_DICT_DUMP_DICT_FOR_METADATA_TYPES_ERR", "English", "Error in dumping dictonary for metadata types"},
{LIBBMDSQL_DICT_DUMP_DICT_FOR_METADATA_TYPE_NAMES_ERR,			264, "Error", "System", "LIBBMDSQL_DICT_DUMP_DICT_FOR_METADATA_TYPE_NAMES_ERR", "English", "Error in dumping dictonary for metadata type names"},
{LIBBMDSQL_DICT_DUMP_DICT_FOR_PKI_TYPES_ERR,				264, "Error", "System", "LIBBMDSQL_DICT_DUMP_DICT_FOR_PKI_TYPES_ERR", "English", "Error in dumping dictonary for pki types"},
{LIBBMDSQL_DICT_DUMP_DICT_FOR_GROUPS_ERR,				264, "Error", "System", "LIBBMDSQL_DICT_DUMP_DICT_FOR_GROUPS_ERR", "English", "Error in dumping dictonary for groups types"},
{LIBBMDSQL_DICT_DUMP_DICT_FOR_ROLES_ERR,				264, "Error", "System", "LIBBMDSQL_DICT_DUMP_DICT_FOR_ROLES_ERR", "English", "Error in dumping dictonary for roles types"},
{LIBBMDSQL_DICT_DUMP_DICT_FOR_SECURITY_LEVELS_ERR,			264, "Error", "System", "LIBBMDSQL_DICT_DUMP_DICT_FOR_SECURITY_LEVELS_ERR", "English", "Error in dumping dictonary for security levels"},
{LIBBMDSQL_DICT_DUMP_DICT_FOR_SECURITY_CATEGORIES_ERR,			264, "Error", "System", "LIBBMDSQL_DICT_DUMP_DICT_FOR_SECURITY_CATEGORIES_ERR", "English", "Error in dumping dictonary for securty categories"},
{LIBBMDSQL_DICT_DUMP_DICT_FOR_RELATION_TYPES_ERR,			264, "Error", "System", "LIBBMDSQL_GET_SYMMETRIC_DECRYPT_ERR", "English", " Error in dumping dictonary for relation types"},
{LIBBMDSQL_DICT_DUMP_DICT_FOR_GROUPS2_ERR,				264, "Error", "System", "LIBBMDSQL_DICT_DUMP_DICT_FOR_RELATION_TYPES_ERR", "English", "Error in dumping dictonary for groups2"},
{LIBBMDSQL_DICT_DUMP_DICT_FOR_SECURITY_CATEGORIES2_ERR,			264, "Error", "System", "LIBBMDSQL_DICT_DUMP_DICT_FOR_SECURITY_CATEGORIES2_ERR", "English", "Error in dumping dictonary for securty categories2"},
{LIBBMDSQL_DICT_DUMP_DICT_FOR_ROLE_STRUCT_ERR,				264, "Error", "System", "LIBBMDSQL_DICT_DUMP_DICT_FOR_ROLE_STRUCT_ERR", "English", "Error in dumping dictonary for role struct"},
{LIBBMDSQL_DICT_DUMP_DICT_FOR_GROUPS_GRAPH_ERR,				264, "Error", "System", "LIBBMDSQL_DICT_DUMP_DICT_FOR_GROUPS_GRAPH_ERR", "English", "Error in dumping dictonary for groups graph"},
{LIBBMDSQL_DICT_DESTROY_DICT_FOR_DOC_TYPES_ERR,				264, "Error", "System", "LIBBMDSQL_DICT_DESTROY_DICT_FOR_DOC_TYPES_ERR", "English", "Error in destroying dictonary for document types"},
{LIBBMDSQL_DICT_DESTROY_DICT_FOR_SOFTWARE_TYPES_ERR,			264, "Error", "System", "LIBBMDSQL_DICT_DESTROY_DICT_FOR_SOFTWARE_TYPES_ERR", "English", "Error in destroying dictonary for software types"},
{LIBBMDSQL_DICT_DESTROY_DICT_FOR_METADATA_TYPES_ERR,			264, "Error", "System", "LIBBMDSQL_DICT_DESTROY_DICT_FOR_METADATA_TYPES_ERR", "English", "Error in destroying dictonary for metadata types"},
{LIBBMDSQL_DICT_DESTROY_DICT_FOR_METADATA_TYPE_NAMES_ERR,		264, "Error", "System", "LIBBMDSQL_DICT_DESTROY_DICT_FOR_METADATA_TYPE_NAMES_ERR", "English", "Error in destroying dictonary for metadata type names"},
{LIBBMDSQL_DICT_DESTROY_DICT_FOR_PKI_TYPES_ERR,				264, "Error", "System", "LIBBMDSQL_DICT_DESTROY_DICT_FOR_PKI_TYPES_ERR", "English", "Error in destroying dictonary for pki types"},
{LIBBMDSQL_DICT_DESTROY_DICT_FOR_GROUPS_ERR,				264, "Error", "System", "LIBBMDSQL_DICT_DESTROY_DICT_FOR_GROUPS_ERR", "English", "Error in destroying dictonary for groups types"},
{LIBBMDSQL_DICT_DESTROY_DICT_FOR_ROLES_ERR,				264, "Error", "System", "LIBBMDSQL_DICT_DESTROY_DICT_FOR_ROLES_ERR", "English", "Error in destroying dictonary for roles types"},
{LIBBMDSQL_DICT_DESTROY_DICT_FOR_SECURITY_LEVELS_ERR,			264, "Error", "System", "LIBBMDSQL_DICT_DESTROY_DICT_FOR_SECURITY_LEVELS_ERR", "English", "Error in destroying dictonary for security levels"},
{LIBBMDSQL_DICT_DESTROY_DICT_FOR_SECURITY_CATEGORIES_ERR,		264, "Error", "System", "LIBBMDSQL_DICT_DESTROY_DICT_FOR_SECURITY_CATEGORIES_ERR", "English", "Error in destroying dictonary for securty categories"},
{LIBBMDSQL_DICT_DESTROY_DICT_FOR_RELATION_TYPES_ERR,			264, "Error", "System", "LIBBMDSQL_DICT_DESTROY_DICT_FOR_RELATION_TYPES_ERR", "English", "Error in destroying dictonary for relation types"},
{LIBBMDSQL_DICT_DESTROY_DICT_FOR_GROUPS2_ERR,				264, "Error", "System", "LIBBMDSQL_DICT_DESTROY_DICT_FOR_GROUPS2_ERR", "English", "Error in destroying dictonary for groups2"},
{LIBBMDSQL_DICT_DESTROY_DICT_FOR_SECURITY_CATEGORIES2_ERR,		264, "Error", "System", "LIBBMDSQL_DICT_DESTROY_DICT_FOR_SECURITY_CATEGORIES2_ERR", "English", "Error in destroying dictonary for securty categories2"},
{LIBBMDSQL_DICT_DESTROY_DICT_FOR_ROLE_STRUCT_ERR,			264, "Error", "System", "LIBBMDSQL_DICT_DESTROY_DICT_FOR_ROLE_STRUCT_ERR", "English", "Error in destroying dictonary for role struct"},
{LIBBMDSQL_DICT_NO_ELEMENTS_IN_DICTIONARY,				264, "Error", "System", "LIBBMDSQL_DICT_NO_ELEMENTS_IN_DICTIONARY", "English", "Metadata oid not found in internal dictionary"},
{LIBBMDSQL_DICT_UNKNOWN_SYSMETADATA,					264, "Error", "System", "LIBBMDSQL_DICT_UNKNOWN_SYSMETADATA", "English", "Unknown system metadata oid in request"},
{LIBBMDSQL_DICT_VALUE_NOT_FOUND,					264, "Error", "System", "LIBBMDSQL_DICT_VALUE_NOT_FOUND", "English", "Value not found in dictionary"},
{LIBBMDSQL_DICT_GROUP_VALUE_NOT_FOUND,					264, "Error", "System", "LIBBMDSQL_DICT_GROUP_VALUE_NOT_FOUND", "English", "Specified group not found in dictionary"},
{LIBBMDSQL_DICT_ROLE_VALUE_NOT_FOUND,					264, "Error", "System", "LIBBMDSQL_DICT_ROLE_VALUE_NOT_FOUND", "English", "Specified role not found in dictionary"},
{LIBBMDSQL_DICT_USER_VALUE_NOT_FOUND,					264, "Error", "System", "LIBBMDSQL_DICT_USER_VALUE_NOT_FOUND", "English", "Specified user not found in dictionary"},
{LIBBMDSQL_DICT_ACTION_VALUE_NOT_FOUND,					264, "Error", "System", "LIBBMDSQL_DICT_ACTION_VALUE_NOT_FOUND", "English", "Specified action not found in dictionary"},
{LIBBMDSQL_DICT_SEC_LEVEL_VALUE_NOT_FOUND,				264, "Error", "System", "LIBBMDSQL_DICT_SEC_LEVEL_VALUE_NOT_FOUND", "English", "Specified security level not found in dictionary"},
{LIBBMDSQL_DICT_SEC_CATEGORY_VALUE_NOT_FOUND,				264, "Error", "System", "LIBBMDSQL_DICT_SEC_CATEGORY_VALUE_NOT_FOUND", "English", "Specified security category not found in dictionary"},
{LIBBMDSQL_DICT_CLASS_VALUE_NOT_FOUND,					264, "Error", "System", "LIBBMDSQL_DICT_CLASS_VALUE_NOT_FOUND", "English", "Specified class not found in dictionary"},




{LIBBMDSQL_USER_ISSUER_UNDEFINED,					264, "Error", "System", "LIBBMDSQL_USER_ISSUER_UNDEFINED", "English", "User certificates issuer not specified"},
{LIBBMDSQL_USER_SN_UNDEFINED,						264, "Error", "System", "LIBBMDSQL_USER_SN_UNDEFINED", "English", "User certificates serial number not specified"},
{LIBBMDSQL_USER_NAME_UNDEFINED,						264, "Error", "System", "LIBBMDSQL_USER_NAME_UNDEFINED", "English", "Users name  not specified"},
{LIBBMDSQL_USER_IDENTITY_UNDEFINED,					264, "Error", "System", "LIBBMDSQL_USER_IDENTITY_UNDEFINED", "English", "User certificates identification not specified"},
{LIBBMDSQL_USER_ROLE_UNDEFINED,						264, "Error", "System", "LIBBMDSQL_USER_ROLE_UNDEFINED", "English", "User certificates role not specified"},
{LIBBMDSQL_USER_GROUP_UNDEFINED,					264, "Error", "System", "LIBBMDSQL_USER_GROUP_UNDEFINED", "English", "User certificates group not specified"},
{LIBBMDSQL_USER_CLASS_UNDEFINED,					264, "Error", "System", "LIBBMDSQL_USER_CLASS_UNDEFINED", "English", "User certificates class not specified"},
{LIBBMDSQL_USER_DEFAULT_GROUP_UNDEFINED,				264, "Error", "System", "LIBBMDSQL_USER_DEFAULT_GROUP_UNDEFINED", "English", "Users default group not specified"},
{LIBBMDSQL_USER_DEFAULT_CLASS_UNDEFINED,				264, "Error", "System", "LIBBMDSQL_USER_DEFAULT_CLASS_UNDEFINED", "English", "Users default class not specified"},
{LIBBMDSQL_USER_DEFAULT_ROLE_UNDEFINED,					264, "Error", "System", "LIBBMDSQL_USER_DEFAULT_ROLE_UNDEFINED", "English", "Users default role not specified"},
{LIBBMDSQL_USER_ISSUER_TOO_MANY,					264, "Error", "System", "LIBBMDSQL_USER_ISSUER_TOO_MANY", "English", "Too many certificates issuers specified in user registration request"},
{LIBBMDSQL_USER_SN_TOO_MANY,						264, "Error", "System", "LIBBMDSQL_USER_SN_TOO_MANY", "English", "Too many certificates serial numbers specified in user registration request"},
{LIBBMDSQL_USER_NAME_TOO_MANY,						264, "Error", "System", "LIBBMDSQL_USER_NAME_TOO_MANY", "English", "Too may users names specified in user registration request"},
{LIBBMDSQL_USER_IDENTITY_TOO_MANY,					264, "Error", "System", "LIBBMDSQL_USER_IDENTITY_TOO_MANY", "English", "Too many users identities specified in user registration request"},
{LIBBMDSQL_USER_CERT_ALREADY_REGISTERED,				264, "Error", "System", "LIBBMDSQL_USER_CERT_ALREADY_REGISTERED", "English", "Certificate already registered in system"},
{LIBBMDSQL_USER_ROLE_ERROR,						264, "Error", "System", "LIBBMDSQL_USER_ROLE_ERROR", "English", "Error adding user to specified role"},
{LIBBMDSQL_USER_GROUP_ERROR,						264, "Error", "System", "LIBBMDSQL_USER_GROUP_ERROR", "English", "Error adding user to specified group"},
{LIBBMDSQL_USER_CLASS_ERROR,						264, "Error", "System", "LIBBMDSQL_USER_CLASS_ERROR", "English", "Error adding user to specified class"},
{LIBBMDSQL_USER_DEFAULT_GROUP_TOO_MANY,					264, "Error", "System", "LIBBMDSQL_USER_DEFAULT_GROUP_TOO_MANY", "English", "Too may users default groups specified in user registration request"},
{LIBBMDSQL_USER_DEFAULT_ROLE_TOO_MANY,					264, "Error", "System", "LIBBMDSQL_USER_DEFAULT_ROLE_TOO_MANY", "English", "Too may users default roles specified in user registration request"},
{LIBBMDSQL_USER_DEFAULT_CLASS_TOO_MANY,					264, "Error", "System", "LIBBMDSQL_USER_DEFAULT_CLASS_TOO_MANY", "English", "Too may users default classes specified in user registration request"},
{LIBBMDSQL_USER_SECURITY_ERROR,						264, "Error", "System", "LIBBMDSQL_USER_SECURITY_ERROR", "English", "Error adding user to specified security category og security level"},
{LIBBMDSQL_USER_NOT_REGISTERED_IN_SYSTEM,				264, "Error", "System", "LIBBMDSQL_USER_NOT_REGISTERED_IN_SYSTEM", "English", "Specified user not registered in system"},
{LIBBMDSQL_USER_LIST_NO_USERS,						264, "Error", "System", "LIBBMDSQL_USER_LIST_NO_USERS", "English", "No registered users"},
{LIBBMDSQL_USER_FILE_OWNER,						264, "Error", "System", "LIBBMDSQL_USER_FILE_OWNER", "English", "User being deleted is file owner"},
{LIBBMDSQL_USER_DEFAULT_ROLE_NOT_ROLE,					264, "Error", "System", "LIBBMDSQL_USER_DEFAULT_ROLE_NOT_ROLE", "English", "Specified default role not found on roles list"},
{LIBBMDSQL_USER_DEFAULT_GROUP_NOT_GROUP,				264, "Error", "System", "LIBBMDSQL_USER_DEFAULT_GROUP_NOT_GROUP", "English", "Specified default group not found on groups list"},
{LIBBMDSQL_USER_DEFAULT_CLASS_NOT_CLASS,				264, "Error", "System", "LIBBMDSQL_USER_DEFAULT_CLASS_NOT_CLASS", "English", "Specified default class not found on class list"},
{LIBBMDSQL_USER_NEW_GROUP_NAME_TOO_MANY,				264, "Error", "System", "LIBBMDSQL_USER_NEW_GROUP_NAME_TOO_MANY", "English", "Too mane names of new group for user being registered"},
{LIBBMDSQL_USER_NO_CLASS_DEFINED,					264, "Error", "System", "LIBBMDSQL_USER_NO_CLASS_DEFINED", "English", "User has no defined classes"},
{LIBBMDSQL_USER_NO_DEFAULT_CLASS_DEFINED,				264, "Error", "System", "LIBBMDSQL_USER_NO_DEFAULT_CLASS_DEFINED", "English", "User has no defined default class"},
{LIBBMDSQL_USER_TOO_MANY_REGISTERED_USERS,				264, "Error", "System", "LIBBMDSQL_USER_TOO_MANY_REGISTERED_USERS", "English", "Number of registered users exceeds maximum value"},
{LIBBMDSQL_USER_IDENTITY_NOT_FOUND,					264, "Error", "System", "LIBBMDSQL_USER_IDENTITY_NOT_FOUND", "English", "Specified identity not registered in the system"},
{DISABLE_OWN_IDENTITY_FORBIDDEN,					264, "Error", "System", "DISABLE_OWN_IDENTITY_FORBIDDEN", "English", "Disabling own identity is forbidden."},
{DISABLE_CURRENT_CERTIFICATE_FORBIDDEN,					264, "Error", "System", "DISABLE_CURRENT_CERTIFICATE_FORBIDDEN", "English", "Disabling current user's certificate is forbidden."},
{LIBBMDSQL_USER_TOO_MANY_REGISTERED_IDENTITIES,				264, "Error", "System", "LIBBMDSQL_USER_TOO_MANY_REGISTERED_IDENTITIES", "English", "Number of registered identities exceeds maximum value"},
{LIBBMDSQL_USER_LICENSE_FORBID_REGISTER_MORE_IDENTITIES,		264, "Error", "System", "LIBBMDSQL_USER_LICENSE_FORBID_REGISTER_MORE_IDENTITIES", "English", "License forbids to register more identities according to specified limit"},

{LIBBMDSQL_ROLE_NOT_REGISTERED_IN_SYSTEM,				264, "Error", "System", "LIBBMDSQL_ROLE_NOT_REGISTERED_IN_SYSTEM", "English", "Specified role not registered in system"},
{LIBBMDSQL_ROLE_USER_ACCESS_DENIED,					264, "Error", "System", "LIBBMDSQL_ROLE_USER_ACCESS_DENIED", "English", "User has no access to specified role"},
{LIBBMDSQL_ROLE_LIST_NO_ROLES,						264, "Error", "System", "LIBBMDSQL_ROLE_LIST_NO_ROLES", "English", "No registered roles"},
{LIBBMDSQL_ROLE_NAME_TOO_MANY,						264, "Error", "System", "LIBBMDSQL_ROLE_NAME_TOO_MANY", "English", "Too many role names specified in request"},
{LIBBMDSQL_ROLE_NAME_UNDEFINED,						264, "Error", "System", "LIBBMDSQL_ROLE_NAME_UNDEFINED", "English", "Role name and group name not specified in request"},
{LIBBMDSQL_ROLE_ROLE_ALREADY_REGISTERED,				264, "Error", "System", "LIBBMDSQL_ROLE_ROLE_ALREADY_REGISTERED", "English", "Specified role name already registered"},
{LIBBMDSQL_ROLE_ID_TOO_MANY,						264, "Error", "System", "LIBBMDSQL_ROLE_ID_TOO_MANY", "English", "Too many role ids specified in request"},
{LIBBMDSQL_ROLE_ID_UNDEFINED,						264, "Error", "System", "LIBBMDSQL_ROLE_ID_UNDEFINED", "English", "Role id not specified in request"},
{LIBBMDSQL_ROLE_NOT_OWNED,						264, "Error", "System", "LIBBMDSQL_ROLE_NOT_OWNED", "English", "Role not accessible for user"},
{LIBBMDSQL_ROLE_TOO_MANY_REGISTERED_ROLES,				264, "Error", "System", "LIBBMDSQL_USER_NO_DEFAULT_CLASS_DEFINED", "English", "Number of registered roles exceeds maximum value"},

{LIBBMDSQL_GROUP_NOT_REGISTERED_IN_SYSTEM,				264, "Error", "System", "LIBBMDSQL_GROUP_NOT_REGISTERED_IN_SYSTEM", "English", "Specified group not registered in system"},
{LIBBMDSQL_GROUP_DEST_GROUPS_FOUND,					264, "Error", "System", "LIBBMDSQL_GROUP_DEST_GROUPS_FOUND", "English", "Specified group has subgroups"},
{LIBBMDSQL_GROUP_ID_TOO_MANY,						264, "Error", "System", "LIBBMDSQL_GROUP_ID_TOO_MANY", "English", "Too many group ids specified in request"},
{LIBBMDSQL_GROUP_ID_UNDEFINED,						264, "Error", "System", "LIBBMDSQL_GROUP_ID_UNDEFINED", "English", "Group id undefined in request"},
{LIBBMDSQL_GROUP_NOT_OWNED,						264, "Error", "System", "LIBBMDSQL_GROUP_NOT_OWNED", "English", "Group not accessible for user"},
{LIBBMDSQL_GROUP_NOT_FOUND,						264, "Error", "System", "LIBBMDSQL_GROUP_NOT_FOUND", "English", "Group not not found in archive"},
{LIBBMDSQL_GROUP_USERS_WITH_GROUP_EXIST,				264, "Error", "System", "LIBBMDSQL_GROUP_USERS_WITH_GROUP_EXIST", "English", "Users with specified group found"},
{LIBBMDSQL_GROUP_PARENT_GROUP_NOT_REGISTERED,				264, "Error", "System", "LIBBMDSQL_GROUP_PARENT_GROUP_NOT_REGISTERED", "English", "Specified parent group does not exsist"},
{LIBBMDSQL_GROUP_CHILD_GROUP_NOT_REGISTERED,				264, "Error", "System", "LIBBMDSQL_GROUP_CHILD_GROUP_NOT_REGISTERED", "English", "Specified child group does not exsist"},
{LIBBMDSQL_GROUP_CHANGE_NAME_TOO_MANY,					264, "Error", "System", "LIBBMDSQL_GROUP_CHANGE_NAME_TOO_MANY", "English", "Too many group names specified in current role/group set request"},
{LIBBMDSQL_GROUP_LIST_NO_GROUP,						264, "Error", "System", "LIBBMDSQL_GROUP_LIST_NO_GROUP", "English", "No registered group"},
{LIBBMDSQL_GROUP_TOO_MANY_REGISTERED_GROUPS,				264, "Error", "System", "LIBBMDSQL_USER_NO_DEFAULT_CLASS_DEFINED", "English", "Number of registered groups exceeds maximum value"},

{LIBBMDSQL_CLASS_NOT_REGISTERED_IN_SYSTEM,				264, "Error", "System", "LIBBMDSQL_CLASS_NOT_REGISTERED_IN_SYSTEM", "English", "Specified class not registered in system"},
{LIBBMDSQL_CLASS_NAME_TOO_MANY,						264, "Error", "System", "LIBBMDSQL_CLASS_NAME_TOO_MANY", "English", "No registered group"},
{LIBBMDSQL_CLASS_NAME_UNDEFINED,					264, "Error", "System", "LIBBMDSQL_CLASS_NAME_UNDEFINED", "English", "No registered group"},
{LIBBMDSQL_CLASS_DEST_GROUPS_FOUND,					264, "Error", "System", "LIBBMDSQL_CLASS_DEST_GROUPS_FOUND", "English", "No registered group"},
{LIBBMDSQL_CLASS_USERS_WITH_CLASS_EXIST,				264, "Error", "System", "LIBBMDSQL_CLASS_USERS_WITH_CLASS_EXIST", "English", "Specified class not found"},
{LIBBMDSQL_CLASS_CLEAR_CLASS_GRAPH_TABLE_ERR,				264, "Error", "System", "LIBBMDSQL_CLASS_CLEAR_CLASS_GRAPH_TABLE_ERR", "English", "No registered group"},
{LIBBMDSQL_CLASS_CLEAR_USERS_AND_CLASSES_TABLE_ERR,			264, "Error", "System", "LIBBMDSQL_CLASS_CLEAR_USERS_AND_GROUPS_TABLE_ERR", "English", "No registered group"},
{LIBBMDSQL_CLASS_CLEAR_GROUPS_TABLE_ERR,				264, "Error", "System", "LIBBMDSQL_CLASS_CLEAR_GROUPS_TABLE_ERR", "English", "No registered group"},
{LIBBMDSQL_CLASS_NOT_FOUND,						264, "Error", "System", "LIBBMDSQL_CLASS_NOT_FOUND", "English", "No registered group"},
{LIBBMDSQL_CLASS_CHANGE_NAME_TOO_MANY,					264, "Error", "System", "LIBBMDSQL_GROUP_LIST_NO_GROUP", "English", "Class error"},
{LIBBMDSQL_CLASS_LIST_NO_CLASS,						264, "Error", "System", "LIBBMDSQL_GROUP_LIST_NO_GROUP", "English", "Class error"},
{LIBBMDSQL_CLASS_CLASS_ALREADY_REGISTERED,				264, "Error", "System", "LIBBMDSQL_GROUP_LIST_NO_GROUP", "English", "Class error"},
{LIBBMDSQL_CLASS_PARENT_CLASS_ERR,					264, "Error", "System", "LIBBMDSQL_GROUP_LIST_NO_GROUP", "English", "Class error"},
{LIBBMDSQL_CLASS_CHILD_CLASS_ERR,					264, "Error", "System", "LIBBMDSQL_GROUP_LIST_NO_GROUP", "English", "Class error"},
{LIBBMDSQL_CLASS_PARENT_CLASS_NOT_REGISTERED,				264, "Error", "System", "LIBBMDSQL_GROUP_LIST_NO_GROUP", "English", "Class error"},
{LIBBMDSQL_CLASS_CHILD_CLASS_NOT_REGISTERED,				264, "Error", "System", "LIBBMDSQL_GROUP_LIST_NO_GROUP", "English", "Class error"},
{LIBBMDSQL_CLASS_ID_TOO_MANY,						264, "Error", "System", "LIBBMDSQL_GROUP_LIST_NO_GROUP", "English", "Class error"},
{LIBBMDSQL_CLASS_ID_UNDEFINED,						264, "Error", "System", "LIBBMDSQL_GROUP_LIST_NO_GROUP", "English", "Class error"},
{LIBBMDSQL_CLASS_NOT_OWNED,						264, "Error", "System", "LIBBMDSQL_GROUP_LIST_NO_GROUP", "English", "Class not accessible for user"},
{LIBBMDSQL_CLASS_DEFAULT_NOT_SPECIFIED,					264, "Error", "System", "LIBBMDSQL_GROUP_LIST_NO_GROUP", "English", "Class error"},
{LIBBMDSQL_CLASS_TOO_MANY_REGISTERED_CLASSES,				264, "Error", "System", "LIBBMDSQL_USER_NO_DEFAULT_CLASS_DEFINED", "English", "Number of registered classes exceeds maximum value"},

{LIBBMDSQL_SEC_CATEGORY_NOT_REGISTERED_IN_SYSTEM,			264, "Error", "System", "LIBBMDSQL_SEC_CATEGORY_NOT_REGISTERED_IN_SYSTEM", "English", "Specified security category not registered in system"},
{LIBBMDSQL_SEC_NAME_NOT_DEFINED,					264, "Error", "System", "LIBBMDSQL_SEC_NAME_NOT_DEFINED", "English", "Neither security category name nor security level name specified"},
{LIBBMDSQL_SEC_CAT_ALREADY_REGISTERED,					264, "Error", "System", "LIBBMDSQL_SEC_CAT_ALREADY_REGISTERED", "English", "Security category already registered"},
{LIBBMDSQL_SEC_CAT_PARENT_CAT_NOT_REGISTERED,				264, "Error", "System", "LIBBMDSQL_SEC_CAT_PARENT_CAT_NOT_REGISTERED", "English", "Security category parent category not registered"},
{LIBBMDSQL_SEC_CAT_CHILD_CAT_NOT_REGISTERED,				264, "Error", "System", "LIBBMDSQL_SEC_CAT_CHILD_CAT_NOT_REGISTERED", "English", "Security category child category not registered"},
{LIBBMDSQL_SEC_CAT_PARENT_SEC_CAT_ERR,					264, "Error", "System", "LIBBMDSQL_SEC_CAT_PARENT_SEC_CAT_ERR", "English", "Error adding parent category"},
{LIBBMDSQL_SEC_CAT_CHILD_SEC_CAT_ERR,					264, "Error", "System", "LIBBMDSQL_SEC_CAT_CHILD_SEC_CAT_ERR", "English", "Error adding child category"},
{LIBBMDSQL_SEC_CAT_NAME_TOO_MANY,					264, "Error", "System", "LIBBMDSQL_SEC_CAT_NAME_TOO_MANY", "English", "Too many category names specified"},

{LIBBMDSQL_SEC_LEVEL_NOT_REGISTERED_IN_SYSTEM,				264, "Error", "System", "LIBBMDSQL_SEC_LEVEL_NOT_REGISTERED_IN_SYSTEM", "English", "Specified security level not registered in system"},
{LIBBMDSQL_SEC_LEVEL_PRIORITY_NOT_REGISTERED_IN_SYSTEM,			264, "Error", "System", "LIBBMDSQL_SEC_LEVEL_PRIORITY_NOT_REGISTERED_IN_SYSTEM", "English", "Specified security level priority not registered in system"},
{LIBBMDSQL_SEC_LEVEL_PRIORITY_ALREADY_REGISTERED,			264, "Error", "System", "LIBBMDSQL_SEC_LEVEL_PRIOROTY_ALREADY_REGISTERED", "English", "Security level priority already registered in system"},
{LIBBMDSQL_SEC_LEVEL_ALREADY_REGISTERED,				264, "Error", "System", "LIBBMDSQL_SEC_LEVEL_ALREADY_REGISTERED", "English", "Security level already registered in system"},
{LIBBMDSQL_SEC_LEVEL_NAME_TOO_MANY,					264, "Error", "System", "LIBBMDSQL_SEC_LEVEL_NAME_TOO_MANY", "English", "Too many security level names specified"},
{LIBBMDSQL_SEC_LEVEL_PRIORITY_TOO_MANY,					264, "Error", "System", "LIBBMDSQL_SEC_LEVEL_PRIORITY_TOO_MANY", "English", "Too many security level priorities specified"},


{LIBBMDSQL_LINK_CREATE_NO_OWNER_CERT_ISSUER,				264, "Error", "System", "LIBBMDSQL_LINK_CREATE_NO_OWNER_CERT_ISSUER", "English", "No link owner certificates issuer specified in link create request"},
{LIBBMDSQL_LINK_CREATE_NO_OWNER_CERT_SN,				264, "Error", "System", "LIBBMDSQL_LINK_CREATE_NO_OWNER_CERT_SN", "English", "No link owner certificates serial number specified in link create request"},
{LIBBMDSQL_LINK_CREATE_NO_OWNER_ID,					264, "Error", "System", "LIBBMDSQL_LINK_CREATE_NO_OWNER_ID", "English", "No link owners id specified in link create request"},
{LIBBMDSQL_LINK_CREATE_TOO_MANY_OWNER_CERT_ISSUER,			264, "Error", "System", "LIBBMDSQL_LINK_CREATE_TOO_MANY_OWNER_CERT_ISSUER", "English", "Too many link owner certificates issuers specified in link create request"},
{LIBBMDSQL_LINK_CREATE_TOO_MANY_OWNER_CERT_SN,				264, "Error", "System", "LIBBMDSQL_LINK_CREATE_TOO_MANY_OWNER_CERT_SN", "English", "Too many link owner certificates serial numbers specified in link create request"},
{LIBBMDSQL_LINK_CREATE_TOO_MANY_OWNER_CERT_ID,				264, "Error", "System", "LIBBMDSQL_LINK_CREATE_TOO_MANY_OWNER_CERT_ID", "English", "Too many link owners ids specified in link create request"},
{LIBBMDSQL_LINK_CREATE_FORBIDDEN,					264, "Error", "System", "LIBBMDSQL_LINK_CREATE_FORBIDDEN", "English", "Link creation for indicated file is forbidden."},


{LIBBMDSQL_ADD_SIGNATURE_NO_SIGNATURE_IN_DTG,				264, "Error", "System", "LIBBMDSQL_ADD_SIGNATURE_NO_SIGNATURE_IN_DTG", "English", "No signature to add to file found in request"},
{LIBBMDSQL_ADD_SIGNATURE_NO_FILE_WITH_ID,				264, "Error", "System", "LIBBMDSQL_ADD_SIGNATURE_NO_FILE_WITH_ID", "English", "No file with found with specified conditions found to add signature to"},
{LIBBMDSQL_ADD_SIGNATURE_NO_FILE_ID_SPECIFIED,				264, "Error", "System", "LIBBMDSQL_ADD_SIGNATURE_NO_FILE_ID_SPECIFIED", "English", "No file id specified in add signature request"},


{LIBBMDPKI_CMS_INTERNAL_PRIVATE_KEY_DECRYPT_ERR, 			264, "Error", "System", "LIBBMDPKI_CMS_INTERNAL_PRIVATE_KEY_DECRYPT_ERR", "English", "Error in decrypting private key from raw file servicing get request"},

{LIBBMDPKI_CTX_API_PUBLIC_KEY_FROM_CERT_FILE_ERR, 			264, "Error", "System", "LIBBMDPKI_CTX_API_PUBLIC_KEY_FROM_CERT_FILE_ERR", "English", "Error in getting servers public key from certificate file"},

{LIBBMDTS_ERR_TS_DECODE_TSR_DATA, 					264, "Error", "System", "LIBBMDTS_ERR_TS_DECODE_TSR_DATA", "English", "Error in decoding timestamp data"},
{LIBBMDTS_ERR_TS_SIGNATURE_NOT_PRESENT_IN_CMS, 				264, "Error", "System", "LIBBMDTS_ERR_TS_SIGNATURE_NOT_PRESENT_IN_CMS", "English", "Signature is nat present in CMS structure"},
{LIBBMDTS_ERR_TS_TIMESTAMP_NOT_PRESENT_IN_SIGNATURE, 			264, "Error", "System", "LIBBMDTS_ERR_TS_TIMESTAMP_NOT_PRESENT_IN_SIGNATURE", "English", "Timestamp is not present in signature structure "},
{LIBBMDTS_ERR_TS_DECODE_TSTINFO, 					264, "Error", "System", "LIBBMDTS_ERR_TS_DECODE_TSTINFO", "English", "Error in decoding timestamp info structure"},
{LIBBMDTS_ERR_TS_GETTING_GEN_TIME, 					264, "Error", "System", "LIBBMDTS_ERR_TS_GETTING_GEN_TIME", "English", "Error in getting generation time info"},
{LIBBMDTS_ERR_TS_CONVERT_INTEGER_T_TO_LONG, 				264, "Error", "System", "LIBBMDTS_ERR_TS_CONVERT_INTEGER_T_TO_LONG", "English", "Integer data too long to convert timestamp"},
{LIBBMDTS_ERR_TS_CONVERT_OBJETCT_IDENTIFIER_T_TO_STRING, 		264, "Error", "System", "LIBBMDTS_ERR_TS_CONVERT_OBJETCT_IDENTIFIER_T_TO_STRING", "English", "Error in converting object identifier to string (timestamp structure)"},
{LIBBMDTS_ERR_TS_CONVERT_INTEGER_T_TO_STRING, 				264, "Error", "System", "LIBBMDTS_ERR_TS_CONVERT_INTEGER_T_TO_STRING", "English", "Error in converting integer to string (timestamp structure)"},
{LIBBMDTS_ERR_TS_NO_CERTIFICATES_PRESENT_IN_SIGNED_DATA, 		264, "Error", "System", "LIBBMDTS_ERR_TS_NO_CERTIFICATES_PRESENT_IN_SIGNED_DATA", "English", "No certificates present in signature data"},
{LIBBMDTS_ERR_TS_NO_x509_CERTIFICATES_PRESENT_IN_SIGNED_DATA,		264, "Error", "System", "LIBBMDTS_ERR_TS_NO_x509_CERTIFICATES_PRESENT_IN_SIGNED_DATA", "English", "No x509 certificates present in signature data"},
{LIBBMDTS_ERR_TS_CLONING_CERTIFICATE, 					264, "Error", "System", "LIBBMDTS_ERR_TS_CLONING_CERTIFICATE", "English", "Error in cloning certificate"},
{LIBBMDTS_ERR_TS_UNRECOGNIZED_ATTR_TYPE, 				264, "Error", "System", "LIBBMDTS_ERR_TS_UNRECOGNIZED_ATTR_TYPE", "English", "Unrecognized attribute type in timestamp structure"},

{LIBBMDAA2_ERR_USER_LOGIN_NOT_REGISTERED, 				264, "Error", "System", "LIBBMDAA2_ERR_USER_LOGIN_NOT_REGISTERED", "English", "User trying to log in is not registered in users data"},
{LIBBMDAA2_ERR_USER_LOGIN_NOT_ACCEPTED, 				264, "Error", "System", "LIBBMDAA2_ERR_USER_LOGIN_NOT_ACCEPTED", "English", "User trying to log in has no acceptance acording to users data"},
{LIBBMDAA2_ERR_USER_GROUP_NOT_SPECIFIED, 				264, "Error", "System", "LIBBMDAA2_ERR_USER_GROUP_NOT_SPECIFIED", "English", "User trying to log in has no group specified"},
{LIBBMDAA2_ERR_USER_DEFAULT_GROUP_NOT_SPECIFIED, 			264, "Error", "System", "LIBBMDAA2_ERR_USER_DEFAULT_GROUP_NOT_SPECIFIED", "English", "User trying to log in has no default group specified"},
{LIBBMDAA2_ERR_USER_ROLE_NOT_SPECIFIED, 				264, "Error", "System", "LIBBMDAA2_ERR_USER_ROLE_NOT_SPECIFIED", "English", "User trying to log in has no role specified"},
{LIBBMDAA2_ERR_USER_DEFAULT_ROLE_NOT_SPECIFIED, 			264, "Error", "System", "LIBBMDAA2_ERR_USER_DEFAULT_ROLE_NOT_SPECIFIED", "English", "User trying to log in has no default role specified"},

{BMDSERVER_ERR_USER_ROLE_UNKNOWN, 					264, "Error", "System", "BMDSERVER_ERR_USER_ROLE_UNKNOWN", "English", "Users role trying to log in is unknown"},
{ERR_DECODE_CMS_DATA, 							264, "Error", "System", "ERR_DECODE_CMS_DATA", "English", "Error decoding CMS data (signature)"},

{CONSOLE_CLIENT_UPLOAD_NO_FILES_TO_UPLOAD, 				264, "Error", "System", "CONSOLE_CLIENT_UPLOAD_NO_FILES_TO_UPLOAD", "English", "No files found to upload"},
{CONSOLE_CLIENT_UPLOAD_OPEN_UPLOAD_DIR_ERR, 				264, "Error", "System", "CONSOLE_CLIENT_UPLOAD_OPEN_UPLOAD_DIR_ERR", "English", "Error openning upload directory"},
{CONSOLE_CLIENT_DOWNLOAD_OPEN_LIST_FILE_ERR, 				264, "Error", "System", "CONSOLE_CLIENT_UPLOAD_OPEN_UPLOAD_DIR_ERR", "English", "Error openning id list file"},
{CONSOLE_CLIENT_UPLOAD_ADDING_FILES_ERR, 				264, "Error", "System", "CONSOLE_CLIENT_UPLOAD_ADDING_FILES_ERR", "English", "Error adding files to upload request"},

{0, 0, NULL, NULL, NULL, NULL, NULL}

};

int main(int argc, char *argv[])
{
long i			= 0;
long j			= 0;
long status		= 0;
long win=0;
FILE	*fd		= NULL;

char *beginning	= {"\
#include <bmd/libbmderr/libbmderr.h>\n\
#include <bmd/libbmddb/libbmddb.h>\n\
#include <string.h>\n\
#include <stdint.h>\n\
#include <stdio.h>\n\
#include <errno.h>\n\
#include <sys/types.h>\n\
#include <signal.h>\n\
\n\
char *GetErrorStr(long err_code)\n\
{\n\
	switch (err_code)\n\
	{"};

char *beginning2 	= {"\
\n\
long GetErrorWinNum(long err_code)\n\
{\n\
	switch (err_code)\n\
	{"};

char *ending2 	= {"\
	\tdefault :	return -1; break;\n\
	}\n\
}\n\n"};

char *ending	= {"\
	\tdefault :	return \"An error occured\"; break;\n\
	}\n\
}\n\n\
char *GetErrorMsg(	long error_code)\n\
{\n\
	\treturn GetErrorStr(error_code);\n\
}\n"};


char *beginning_win	= {"; /* --------------------------------------------------------\n\
; HEADER SECTION\n\
;*/\n\
SeverityNames=(Success=0x0:STATUS_SEVERITY_SUCCESS\n\
               Informational=0x1:STATUS_SEVERITY_INFORMATIONAL\n\
               Warning=0x2:STATUS_SEVERITY_WARNING\n\
               Error=0x3:STATUS_SEVERITY_ERROR\n\
              )\n\
;\n\
;\n\
FacilityNames=(System=0x0:FACILITY_SYSTEM\n\
               Runtime=0x2:FACILITY_RUNTIME\n\
               Stubs=0x3:FACILITY_STUBS\n\
               Io=0x4:FACILITY_IO_ERROR_CODE\n\
              )\n\
;\n\
;/* ------------------------------------------------------------------\n\
; MESSAGE DEFINITION SECTION\n\
;*/\n\
\n\
\n\
MessageIdTypedef=WORD\n\
\n\
MessageId=0x1\n\
SymbolicName=CAT_1\n\
Language=English\n\
Konfiguracja\n\
.\n\
\n\
MessageId=0x2\n\
SymbolicName=CAT_2\n\
Language=English\n\
Kryptografia\n\
.\n\
\n\
MessageId=0x3\n\
SymbolicName=CAT_3\n\
Language=English\n\
Operacje plikowe\n\
.\n\
\n\
MessageId=0x4\n\
SymbolicName=CAT_4\n\
Language=English\n\
Baza danych\n\
.\n\
\n\
MessageId=0x5\n\
SymbolicName=CAT_5\n\
Language=English\n\
DVCS\n\
.\n\
\n\
MessageId=0x6\n\
SymbolicName=CAT_6\n\
Language=English\n\
Systemowe\n\
.\n\
\n\
MessageId=0x7\n\
SymbolicName=CAT_7\n\
Language=English\n\
Obsluga sieci\n\
.\n\
\n\
MessageIdTypedef=DWORD\n\
\n\
"};
char *ending_win	= {"\n"};


	printf("/-----------------------------------------------------------------------------------------/\n");
	printf("|    ***********    *********     *********     *********     *********      *********    |\n");
	printf("|    ***********   ***********   ***********   ***********   ***********    ***********   |\n");
	printf("|    **            **       **   **       **   **       **   **       **    **       **   |\n");
	printf("|    **            **       **   **       **   **       **   **       **    **            |\n");
	printf("|    **            **       **   **       **   **       **   **       **    **            |\n");
	printf("|    ***********   ***********   ***********   **       **   ***********    **********    |\n");
	printf("|    ***********   **********    **********    **       **   **********      **********   |\n");
	printf("|    **            **     **     **     **     **       **   **     **               **   |\n");
	printf("|    **            **      **    **      **    **       **   **      **              **   |\n");
	printf("|    **            **       **   **       **   **       **   **       **    **       **   |\n");
	printf("|    ***********   **       **   **       **   ***********   **       **    ***********   |\n");
	printf("|    ***********   **       **   **       **    *********    **       **     *********    |\n");
	printf("/-----------------------------------------------------------------------------------------/\n\n\n");

	/******************************************************/
	/*	budownie pliku ze slownikiem bledow dla linuxa	*/
	/******************************************************/
#ifndef WIN32
	printf("Opening destination file...");
	fd=fopen(argv[1], "w+");
	if (fd!=NULL)
	{
		printf("\tOK\n");
		fprintf(fd, "%s\n", beginning);

		while (1)
		{
			while (1)
			{
				if (i!=j)
				{
					if (errors[i].MeassageId_Unix==errors[j].MeassageId_Unix)
					{
						printf("/*      Error - 2 error codes are the same  (%i)   */\n",errors[i].MeassageId_Unix);
						return -1;
					}
				}
				if (errors[j].MeassageId==0) break;
				j++;
			}
			if (errors[i].MeassageId==0) break;
			fprintf(fd, "\t\tcase %i : return \"%s\"; break;\n", errors[i].MeassageId_Unix, errors[i].msg);
			i++;
		}
		fprintf(fd, "%s\n", ending);

		fprintf(fd, "%s\n", beginning2);
		i=0;
		while (1)
		{
			if (errors[i].MeassageId==0) break;
			fprintf(fd, "\t\tcase %i : return %i; break;\n", errors[i].MeassageId_Unix, errors[i].MeassageId);
			i++;
		}
		fprintf(fd, "%s\n", ending2);

		printf("Closing destination file...");
		status=fclose(fd);
		if (status<0)
		{
			printf("\tFAILED\n");
			return -1;
		}
		printf("\tOK\n");

	}
	else
	{
		printf("\tFAILED\n");
		return -1;
	}
#else
	/************************************************************/
	/*	budownie pliku ze slownikiem bledow dla windowsa	*/
	/************************************************************/
	printf("Opening destination file...");
	fd=fopen(argv[1], "w+");
	if (fd!=NULL)
	{
		printf("\tOK\n");
		fprintf(fd, "%s\n", beginning_win);

		while (1)
		{
			if (errors[i].MeassageId==0) break;
			fprintf(fd,"MessageId=0x%X\n",errors[i].MeassageId);
			fprintf(fd,"Severity=%s\n",errors[i].Severity);
			fprintf(fd,"Facility=%s\n",errors[i].Facility);
			fprintf(fd,"SymbolicName=%s\n",errors[i].SymbolicName);
			fprintf(fd,"Language=%s\n",errors[i].Language);
			fprintf(fd,"%s\n",errors[i].msg);
			fprintf(fd, ".\n\n");
			i++;
		}
		fprintf(fd, "%s\n", ending_win);
		printf("Closing destination file...");
		status=fclose(fd);
		if (status<0)
		{
			printf("\tFAILED\n");
			return -1;
		}
		printf("\tOK\n");

	}
	else
	{
		printf("\tFAILED\n");
		return -1;
	}
#endif
	return 0;
}
