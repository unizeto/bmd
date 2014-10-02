#ifndef _BMD_ERRORS_INCLUDED_
#define _BMD_ERRORS_INCLUDED_

/* kod poprawny */
#define BMD_OK			0

/****************************************************************************************/
/*	bledy nalezy w przyszlosci pogrupowac i kazdej z grup nadac konkretny numer,	*/
/*	a numer bledu z danej grupy jakos powiazac z jej numerem			*/
/****************************************************************************************/

/************************************************/
/*	bledy logowania lub odczytania zadania	*/
/************************************************/
#define BMD_VERIFY_REQUEST_FILE_PARSE_ERROR					-2001000	/* blad parsowania pliku kontrolki.cofn */
#define BMD_VERIFY_REQUEST_GET_CLEARANCE_ERROR					-2001001	/* oid grupy nie wystepuje na formularzu */
#define BMD_VERIFY_REQUEST_INVALID_FILENAME					-2001002	/* nazwa pliku przekazana w datagramie jest niedozwolona */
#define BMD_VERIFY_REQUEST_INVALID_ADD_METADATA					-2001003	/* niepoprawna wartość metadanej dodatkowej */
#define BMD_VERIFY_REQUEST_INVALID_USR_METADATA					-2001004	/* niepoprawna wartość metadanej uzytkownika */
#define BMD_VERIFY_REQUEST_INVALID_SYS_METADATA					-2001005	/* niepoprawna wartość metadanej systemowej */
#define BMD_VERIFY_REQUEST_INVALID_ROLE						-2001006	/* niepoprawna weryfikacja roli klienta */
#define BMD_VERIFY_REQUEST_INVALID_ADD_METADATA_OID				-2001007	/* niepoprawna wartość oida metadanej dodatkowej */
#define BMD_VERIFY_REQUEST_INVALID_USR_METADATA_OID				-2001008	/* niepoprawna wartość oida metadanej uzytkownika */
#define BMD_VERIFY_REQUEST_INVALID_SYS_METADATA_OID				-2001009	/* niepoprawna wartość oida metadanej systemowej */
#define BMD_VERIFY_REQUEST_CHECK_CLEARANCE_ERROR				-2001010	/* sprawdzenie clearenca zwrocilo blad */
#define BMD_VERIFY_REQUEST_INVALID_CLEARANCE_FORMAT				-2001011	/* pobranie clearance'ow z certyfikatu atrybutow nie powiodlo sie */
#define BMD_VERIFY_REQUEST_GET_GROUPS_ERROR					-2001012	/* pobranie grup z certyfikatu atrybutow nie powiodlo sie */
#define BMD_VERIFY_REQUEST_INVALID_SEC_CLEARANCE				-2001013
#define BMD_VERIFY_REQUEST_METADATA_NOT_UPDATEABLE				-2001014
#define BMD_VERIFY_REQUEST_METADATA_NOT_UPDATEABLE_ROLE_RIGHTS		-2001015
#define BMD_VERIFY_LOGIN_REQUEST_NO_GROUPS_IN_AC				-2001016
#define BMD_VERIFY_LOGIN_REQUEST_NO_GROUPS_IN_USER_FORM			-2001017
#define BMD_VERIFY_LOGIN_REQUEST_INVALID_GROUP_SOURCE_IN_USER_FORM	-2001018
#define BMD_VERIFY_REQUEST_INVALID_ADD_METADATA_REGEXP			-2001019
#define BMD_VERIFY_REQUEST_METADATA_NOT_HISTORYABLE				-2001020
#define BMD_VERIFY_REQUEST_SEARCH_TOO_MANY_OPEN_BRACKETS			-2001021
#define BMD_VERIFY_REQUEST_SEARCH_TOO_MANY_CLOSE_BRACKETS			-2001022
#define BMD_VERIFY_REQUEST_SEARCH_SYNTAX_LOGICAL_INVALID_POS		-2001023
#define BMD_VERIFY_REQUEST_SERVER_CONF_UPDATE					-2001024
#define BMD_VERIFY_REQUEST_SEARCH_IMPERMISSIBLE_SORT_METADATA		-2001025
#define BMD_VERIFY_REQUEST_SEARCH_INVALID_SORT_ORDER				-2001026
#define BMD_VERIFY_REQUEST_INCORRECT_SORT_CRITERIA					-2001027
#define BMD_VERIFY_REQUEST_NO_ADD_METADATA_SPECIFIED				-2001028
#define BMD_VERIFY_REQUEST_AMBIGUOUS_CRITERIA						-2001029



#define BMD_VERIFY_LOGIN_CERTIFICATE_CERT_STORE_DIR_ERROR			-2001050
#define BMD_VERIFY_LOGIN_CERTIFICATE_NO_CERTS_IN_CERT_STORE			-2001051
#define BMD_VERIFY_LOGIN_CERTIFICATE_DECODE_ERROR				-2001052
#define BMD_VERIFY_LOGIN_CERTIFICATE_INVALID_FORMAT				-2001053
#define BMD_VERIFY_LOGIN_CERTIFICATE_LOAD_CERT_ERROR				-2001054
#define BMD_VERIFY_LOGIN_CERTIFICATE_WITH_CERT_ERROR				-2001055
#define BMD_VERIFY_LOGIN_CERTIFICATE_CERT_PATH					-2001056

#define BMD_VERIFY_LOGIN_CERTIFICATE_CRL_EXT_ABSENT				-2001057
#define BMD_VERIFY_LOGIN_CERTIFICATE_LOAD_CRL_ERROR				-2001058
#define BMD_VERIFY_LOGIN_CERTIFICATE_NO_CRL_IN_FILE				-2001059
#define BMD_VERIFY_LOGIN_CERTIFICATE_CRL_DECODE_ERROR				-2001060
#define BMD_VERIFY_LOGIN_CERTIFICATE_ON_CRL					-2001061
#define BMD_VERIFY_LOGIN_CERTIFICATE_OUT_OF_DATE				-2001062

#define BMD_VERIFY_LOGIN_CERTIFICATE_LOAD_ACL_ERROR				-2001063
#define BMD_VERIFY_LOGIN_CERTIFICATE_ACL_INVALID_FORMAT			-2001064
#define BMD_VERIFY_LOGIN_CERTIFICATE_FAILURE					-2001065
#define BMD_VERIFY_LOGIN_CERTIFICATE_NOT_FOND_IN_ACL_FILE			-2001066
#define BMD_VERIFY_LOGIN_CERTIFICATE_NOT_FOND_IN_ACL_DICT			-2001067
#define BMD_VERIFY_LOGIN_CERTIFICATE_NOT_ACCEPTED_IN_ACL_DICT		-2001068
#define BMD_NO_FORM_DATA								-2001069
#define BMD_NO_SEND_FORM_DATA								-2001070
#define BMD_NO_SEARCH_FORM_DATA							-2001071
#define BMD_NO_GRID_FORM_DATA								-2001072
#define BMD_NO_UNVISIBLE_FORM_DATA							-2001073
#define BMD_NO_UPDATE_FORM_DATA							-2001074
#define BMD_NO_UPLODAD_FORM_DATA							-2001075
#define BMD_NO_HISTORY_FORM_DATA							-2001076
#define BMD_WRONG_FORM_TYPE								-2001077
#define BMD_CERTIFICATE_DECODE_ERROR					-2001076
#define BMD_CERTIFICATE_TIME_BEFORE_NOT_PRESENT			-2001077
#define BMD_CERTIFICATE_TIME_AFTER_NOT_PRESENT			-2001078
#define BMD_CERTIFICATE_UT_TO_TIME_DECODE_PROBLEM		-2001079
#define BMD_CERTIFICATE_GT_TO_TIME_DECODE_PROBLEM		-2001080

#define NO_USR_METADATA_PRESENT 							-2002006
#define NO_SYS_METADATA_PRESENT 							-2002007
#define NO_PKI_METADATA_PRESENT 							-2002008
#define INVALID_ADDITIONAL_METADATA_OID_FORMAT       				-2002009
#define INVALID_SYSTEM_METADATA_OID_FORMAT       				-2002010
#define INVALID_USER_METADATA_OID_FORMAT       					-2002011
#define INVALID_PKI_METADATA_OID_FORMAT       					-2002012
#define BMD_ERR_OID_NOT_FOUND								-2002013
#define UNKNOWN_METADATA_TYPE							-2002014
#define ACTION_METADATA_USER_CLASS_NOT_SPECIFIED				-2002015
#define ACTION_METADATA_ACTION_NOT_SPECIFIED					-2002016
#define INVALID_PKI_METADATA_HASH_ALGORITHM_OID_FORMAT       	-2002017
#define INVALID_PKI_METADATA_HEX_HASH_VALUE_FORMAT				-2002018
#define HEX_TO_BINARY_CONVERTION_ERROR							-2002019

#define BMD_LOGIN_NO_CLIENT_HAS_NO_GROUP					-2003000	/* użytkownik nie nalezy do żadnej grupy */

#define BMD_CLIENT_ROLE_NOT_FOUND						-2004000
#define BMD_CLIENT_ROLE_NOT_PERMIT_LOGIN_FOR_SOMEBODY				-2004001
#define BMD_CLIENT_ROLE_NOT_PERMIT_ACTION					-2004002
#define BMD_CLIENT_NOT_FOUND_IN_ATTRIBUTES					-2004003
#define BMD_CLIENT_CLEARANCE_NOT_FOUND_IN_ATTRIBUTES				-2004004


#define BMD_ROLE_RIGHTS_INCOMPLETE_RECORD					-2006000
#define BMD_ROLE_RIGHTS_INVALID_RECORD_LENGTH					-2006001

/************************/
/*	bledy bazy danych	*/
/************************/
#define BMD_DB_INVALID_CONNECTION_HANDLE					-3000000
#define BMD_DB_SERVER_ADDRESS_NOT_SPECIFIED					-3000001
#define BMD_DB_SERVER_PORT_NOT_SPECIFIED					-3000002
#define BMD_DB_NAME_NOT_SPECIFIED						-3000003
#define BMD_DB_USER_NAME_NOT_SPECIFIED						-3000004
#define BMD_DB_USER_PASSWORD_NOT_SPECIFIED					-3000005
#define BMD_DB_COMMAND_NOT_SPECIFIED						-3000006
/* #define BMD_DB_SERVER_ADDRESS_NOT_SPECIFIED					-3000006
#define BMD_DB_SERVER_ADDRESS_NOT_SPECIFIED					-3000007
#define BMD_DB_SERVER_ADDRESS_NOT_SPECIFIED					-3000008*/
#define BMD_DB_EXECUTION_ERROR								-3000011
#define BMD_DB_RECONNECT_ERROR								-3000012

/* usuwanie pliku */
#define BMD_SQL_DELETE_DB_ERROR							-3001000
#define BMD_SQL_DELETE_FILE_NOT_FOUND						-3001001	/* usuwanego pliku nie ma w bazie (w crypto_objects) */
#define BMD_SQL_DELETE_FILE_LOB_NOT_FOUND					-3001002	/* loba usuwanego pliku nie ma w bazie danych */
#define BMD_SQL_DELETE_SYMKEY_LOB_NOT_FOUND					-3001003	/* loba klucza symetrycznego usuwanego pliku nie ma w bazie danych */
#define BMD_SQL_DELETE_SIGNATURE_LOB_NOT_FOUND					-3001004	/* loba podpisu usuwanego pliku nie ma w bazie danych */
#define BMD_SQL_DELETE_TIMESTAMP_LOB_NOT_FOUND					-3001005	/* loba znacznika czasu usuwanego pliku nie ma w bazie danych */

/* pobieranie pliku */
#define BMD_SQL_GET_ONLY_FILE_DB_ERROR						-3002000	/* blad bazy danych podczas pobierania tylko pliku z bazy danych */
#define BMD_SQL_GET_ONLY_FILE_NOT_FOUND_ERROR					-3002001	/* brak wpisu o rastrze pliku w tabeli crypto_objects */
#define BMD_SQL_GET_ONLY_FILE_LOB_NOT_FOUND_ERROR				-3002002	/* loba danego pliku nie znalezeiono w bazie danych */
#define BMD_SQL_GET_ONLY_FILE_SYMKEY_LOB_NOT_FOUND_ERROR			-3002003	/* loba klucza symetrycznego danego pliku nie znalezeiono w bazie danych */
#define BMD_SQL_GET_ONLY_FILE_NO_EINVOICE_FETCHED_MTD				-3002004	/* blad bazy danych podczas pobierania tylko pliku z bazy danych */

/* wyszukiwanie pliku */
#define BMD_SQL_SEARCH_DB_ERROR							-3003000	/* blad bazy danych podczas wyszukiwania pliku z bazy danych */
#define BMD_SQL_SEARCH_PG_FATAL_ERROR_DB_ERROR					-3003001	/* fatalny blad bazy danych podczas wyszukiwania pliku z bazy danych */
#define BMD_SQL_SEARCH_INVALID_QUERY_TYPE					-3003002	/* fatalny blad bazy danych podczas wyszukiwania pliku z bazy danych */

/* pobieranie metadanych pliku */
#define BMD_SQL_GET_METADATA_DB_ERROR						-3004000	/* blad bazy danych podczas pobierania metadanych pliku z bazy danych */
#define BMD_SQL_GET_METADATA_NOT_FOUND_ERROR					-3004001	/* brak metadanych do pliku */
#define BMD_SQL_GET_METADATA_LOB_NOT_FOUND_ERROR				-3004002	/* brak loba symkey'a do pliku */

/* pobieranie metadanych pliku */
#define BMD_SQL_GET_METADATA_CGI_DB_ERROR					-3005000	/* blad bazy danych podczas pobierania metadanych pliku z bazy danych CGI*/
#define BMD_SQL_GET_METADATA_CGI_NOT_FOUND_ERROR				-3005001	/* brak metadanych do pliku CGI*/
#define BMD_SQL_GET_METADATA_CGI_LOB_NOT_FOUND_ERROR				-3005002	/* brak loba symkey'a do pliku CGI*/

/* pobieranie pliku CGI*/
#define BMD_SQL_GET_CGI_DB_ERROR						-3006000	/* blad bazy danych podczas pobierania cgi pliku z bazy danych */
#define BMD_SQL_GET_CGI_NOT_FOUND_ERROR						-3006001	/* brak wpisu o rastrze pliku w tabeli crypto_objects */
#define BMD_SQL_GET_CGI_LOB_NOT_FOUND_ERROR					-3006002	/* loba danego pliku nie znalezeiono w bazie danych */
#define BMD_SQL_GET_CGI_SYMKEY_LOB_NOT_FOUND_ERROR				-3006003	/* loba klucza symetrycznego danego pliku nie znalezeiono w bazie danych */

/* weryfikacja podpisu*/
#define BMD_SQL_VERIFY_SIGNATURE_DB_ERROR					-3007000	/* blad bazy danych podczas weryfikacji podpisu */
#define BMD_SQL_VERIFY_SIGNATURE_FILE_NOT_FOUND_ERROR				-3007001	/* brak wpisu o rastrze pliku w tabeli crypto_objects */
#define BMD_SQL_VERIFY_SIGNATURE_FILE_LOB_NOT_FOUND_ERROR			-3007002	/* loba danego pliku nie znalezeiono w bazie danych */
#define BMD_SQL_VERIFY_SIGNATURE_SYMKEY_LOB_NOT_FOUND_ERROR			-3007003	/* loba klucza symetrycznego danego pliku nie znalezeiono w bazie danych */
#define BMD_SQL_VERIFY_SIGNATURE_NOT_FOUND_ERROR				-3007004	/* loba klucza symetrycznego danego pliku nie znalezeiono w bazie danych */
#define BMD_SQL_VERIFY_SIGNATURE_LOB_NOT_FOUND_ERROR				-3007005	/* loba klucza symetrycznego danego pliku nie znalezeiono w bazie danych */

/* update metadanych */
#define BMD_SQL_UPDATE_DB_ERROR							-3008000	/* blad bazy danych podczas weryfikacji podpisu */
#define BMD_SQL_UPDATE_FILE_NOT_FOUND_ERROR					-3008001	/* brak wpisu o rastrze pliku w tabeli crypto_objects */
#define BMD_SQL_UPDATE_FILE_LOB_NOT_FOUND_ERROR					-3008002	/* loba danego pliku nie znalezeiono w bazie danych */
#define BMD_SQL_UPDATE_SYMKEY_LOB_NOT_FOUND_ERROR				-3008003	/* loba klucza symetrycznego danego pliku nie znalezeiono w bazie danych */

/* ZAMKNIECIE TRANSAKCJI */
#define BMD_SQL_CLOSE_TRANSACTION_DB_ERROR					-3009000	/* blad bazy danych podczas weryfikacji podpisu */
#define BMD_SQL_CLOSE_TRANSACTION_FILE_NOT_FOUND_ERROR				-3009001	/* brak wpisu o rastrze pliku w tabeli crypto_objects */
#define BMD_SQL_CLOSE_TRANSACTION_ID_NOT_FOUND_IN_DTG_ERROR			-3009002	/* brak wpisu o rastrze pliku w tabeli crypto_objects */

/* opearcja na katalogach */
#define BMD_SQL_GET_DIR_LIST_NOT_FOUND_ERROR					-3010000
#define BMD_SQL_DELETE_DIR_NOT_FOUND_ERROR					-3010001
#define BMD_SQL_GET_DIR_NOT_FOUND_ERROR						-3010002
#define BMD_SQL_DELETE_DIR_NOT_EMPTY_ERROR					-3010003

/* opearcja historycznosci */
#define BMD_SQL_HISTORY_NOT_FOUND_ERROR						-3011000

/******************************************/
/*	bledy operacji na plikach w BMD	*/
/******************************************/
#define BMD_OPR_GET_INVALID_ID_FORMAT						-4001000	/* niepoprawny format id pliku */
#define BMD_OPR_GET_HASH_NOT_FOUND						-4001001	/* niepoprawny format id pliku */


/******************************************/
/*	bledy zwracane przez plugin DVCS	*/
/******************************************/
/* wykorzystywane podczas sprawdzania certyfikatu logujacego*/
#define PLUGIN_DVCS_CERTIFICATE_INVALID						-5001000	/* (kod ujemny) DVCS zweryfikowal certyfikat NEGATYWNIE*/
#define PLUGIN_DVCS_CERTIFICATE_VERIFIED						 5001001	/* (kod dodatni) DVCS zweryfikowal certyfikat POZYTYWNIE (KWALIFIKOWANY)*/
#define PLUGIN_DVCS_CERTIFICATE_VERIFIED_CONDITIONALLY			 5001002	/* (kod dodatni) DVCS zweryfikowal certyfikat POZYTYWNIE WARUNKOWO (ZWYKLY) */
#define PLUGIN_DVCS_INVALID_LOGIN_CERTIFICATE					-5001003	/* dvcs niepoprawnie zweryfikowal certyfikat logujacy */
#define PLUGIN_DVCS_INVALID_CERTIFICATE						-5001004	/* dvcs niepoprawnie zweryfikowal certyfikat */

/* wykorzystywane podczas sprawdzania podpisu pod okumentem w bazie danych*/
#define PLUGIN_DVCS_SIGNATURE_INVALID						-5002000	/* (kod ujemny) DVCS zweryfikowal podpis NEGATYWNIE */
#define PLUGIN_DVCS_SIGNATURE_VERIFIED						 5002001	/* (kod dodatni) DVCS zweryfikowal podpis POZYTYWNIE (KWALIFIKOWANY) */
#define PLUGIN_DVCS_SIGNATURE_VERIFIED_CONDITIONALLY				 5002002	/* (kod dodatni) DVCS zweryfikowal podpis POZYTYWNIE WARUNKOWO (ZWYKLY) */
#define PLUGIN_DVCS_DATA_ABSENT							-5002004	/* (kod ujemny) np. w przesylanym datagramie nie bylo podpisu pod plikiem */
#define PLUGIN_DVCS_SERVICE_FAILURE							-5002005	/* (kod ujemny) nie można uzyskać odpowiedzi od serwera DVCS, badz wystapil blad podczas przetwarzania zadania DVCS */
#define PLUGIN_DVCS_FILE_NOT_FOUND							-5002006	/* plik do sprawdzenia podpisu nie znaleziony (lub brak do niego uprawnień) */

/*<rezultat weryfikacji DVCS przy wstawianiu dokumentu do archiwum>*/
#define PLUGIN_DVCS_INSERTION_SIGNATURE_VERIFIED				5003010 /*(kod dodatni) podpis zweryfikowany pozytywnie*/
#define PLUGIN_DVCS_INSERTION_SIGNATURE_VERIFIED_CONDITIONALLY			5003011 /*(kod dodatni) podpis zweryfikowany warunkowo*/
#define PLUGIN_DVCS_INSERTION_ACCEPTED_INVALID_SIGNATURE			5003012 /*(kod dodatni) dokument przyjety, mimo ze podpis zweryfikowany negatywnie*/
#define PLUGIN_DVCS_INSERTION_ACCEPTED_SERVICE_FAILURE			5003013 /*(kod dodatni) dokument przyjety, mimo ze usluga DVC nieodpowiada badz zle obsluguje zadanie*/
#define PLUGIN_DVCS_INSERTION_ACCEPTED_NO_SIGNATURE				5003014 /*(kod dodatni) dokument przyjety, mimo ze nie jest podpisany*/
#define PLUGIN_DVCS_INSERTION_REJECTED_INVALID_SIGNATURE			-5003015 /*(kod ujemny) dokument odrzucony z powodu zweryfikowany negatywnie podpisu*/
#define PLUGIN_DVCS_INSERTION_REJECTED_SERVICE_FAILURE				-5003016 /*(kod ujemny) dokument odrzucony poniewaz usluga DVC nieodpowiada badz zle obsluguje zadanie*/
#define PLUGIN_DVCS_INSERTION_REJECTED_NO_SIGNATURE				-5003017 /*(kod ujemny) dokument odrzucony ze wzgledu na brak podpisu*/
#define PLUGIN_DVCS_INSERTION_INTERNAL_ERROR					-5003018 /*(kod ujemny) jesli programista cos popsul w pluginieDVCS (wewnetrzny blad pluginu DVCS)*/


/******************************/
/*	bledy wysylania e-maili	*/
/********************** *******/
#define PLUGIN_SESSION_MAIL_SEND_ERROR						-6001000	/* blad wysylki awizo w postaci e-maila */
#define PLUGIN_SESSION_SMTP_ADDRESS_CONF_ERROR					-6001001	/* brak adresu serwera SMTP w pliku konfiguracyjnym pluginu StdSession */
#define PLUGIN_SESSION_MAIL_TEMPLATE_CONF_ERROR					-6001002	/* brak pliku z szablonem e-maila w pliku konfiguracyjnym pluginu StdSession */
#define PLUGIN_SESSION_MAIL_IMAGES_ERROR						-6001003	/* brak katalogu z grafikami dolaczanymi do maila w pliku konfiguracyjnym pluginu StdSession */
#define PLUGIN_SESSION_EINVOICE_MAIL_TITLE_ERROR				-6001004	/* brak tytulu maila w pliku konfiguracyjnym pluginu StdSession */
#define PLUGIN_SESSION_SMIME_FILE_CONF_ERROR					-6001005	/* brak pliku z certyfikatem do podpisu maila w pliku konfiguracyjnym pluginu StdSession */
#define PLUGIN_SESSION_SMIME_PASS_CONF_ERROR					-6001006	/* brak hasla do pliku z certyfikatem do podpisu maila w pliku konfiguracyjnym pluginu StdSession */
#define PLUGIN_SESSION_CONF_STRUCT_ERROR						-6001007	/* niepoprawna struktura konfiguracyjna pluginu StdSession */
#define PLUGIN_SESSION_RECIPIENT_NOT_SPECIFIED					-6001008	/* niepoprawna struktura konfiguracyjna pluginu StdSession */
#define PLUGIN_SESSION_MAX_ATTACH_SIZE_ERROR					-6001009
#define PLUGIN_SESSION_MAIL_SEND_CONF_ERROR					-6001010
#define PLUGIN_SESSION_MAIL_FORMAT_CONF_ERROR					-6001011
#define PLUGIN_SESSION_MAIL_FORMAT_UNKNOWN					-6001012
#define PLUGIN_SESSION_NOT_RUNNING							-6001013

#define PLUGIN_SESSION_UNSPECIFIED_METADATA_CONF_ERROR			-6001015
#define PLUGIN_SESSION_PARAMETRIC_METADATA_CONF_ERROR			-6001016
#define PLUGIN_SESSION_PARAM1_UNSPECIFIED_VALUE_CONF_ERROR		-6001017
#define PLUGIN_SESSION_PARAM2_UNSPECIFIED_VALUE_CONF_ERROR		-6001018
#define PLUGIN_SESSION_IF_SPECIFIED_METADATA_CONF_ERROR			-6001019
#define PLUGIN_SESSION_UNRECOGNIZED_FIXED_METADATA_OID			-6001020
#define PLUGIN_SESSION_UNRECOGNIZED_PARAMETRIC_METADATA_OID		-6001021
#define PLUGIN_SESSION_UNRECOGNIZED_PARAM1_METADATA_OID			-6001022
#define PLUGIN_SESSION_UNRECOGNIZED_PARAM2_METADATA_OID			-6001023
#define PLUGIN_SESSION_ADVICE_NOT_SENT_ACCORDING_TO_METADATA	-6001024

#define PLUGIN_SESSION_SESSION_CREATE_ERROR					-6001100
#define PLUGIN_SESSION_ADD_MSG_TO_SESSION_ERROR					-6001101
#define PLUGIN_SESSION_SET_MSG_CALLBACK_ERROR					-6001102
#define PLUGIN_SESSION_ADD_RECIPIENT_TO_MSG_ERROR				-6001103

#define PLUGIN_SESSION_MAIL_SERVER_ADDRESS_NOT_SPECIFIED			-6001200
#define PLUGIN_SESSION_MAIL_SERVER_PORT_NOT_SPECIFIED				-6001201

#define PLUGIN_SESSION_NO_MAIL_TO_SEND						6001000

/***************************/
/* bledy pluginu AdviceSOE */
/***************************/
#define PLUGIN_SESSION_OID_FORMAT_CREATION_MODE				-6001301
#define PLUGIN_SESSION_NO_METADATA_CREATION_MODE			-6001302
#define PLUGIN_SESSION_FORMAT_OID_TO_TAG_ENTRY				-6001303
#define PLUGIN_SESSION_OID_FORMAT_OID_TO_TAG_ENTRY			-6001304
#define PLUGIN_SESSION_NO_REQUESTED_ADD_METADATA			-6001305
#define PLUGIN_SESSION_CREATION_MODE_VALUE				-6001306
#define PLUGIN_SESSION_OID_FORMAT_SENDING_MODE				-6001307
#define PLUGIN_SESSION_OID_FORMAT_TEMPLATE_VERSION			-6001308
#define PLUGIN_SESSION_OID_FORMAT_LANGUAGE				-6001309
#define PLUGIN_SESSION_OID_FORMAT_RECIPIENT_EMAIL			-6001310
#define PLUGIN_SESSION_OID_FORMAT_RECIPIENT_PHONE			-6001311
#define PLUGIN_SESSION_NO_METADATA_SENDING_MODE				-6001312
#define PLUGIN_SESSION_NO_METADATA_TEMPLATE_VERSION			-6001313
#define PLUGIN_SESSION_NO_METADATA_LANGUAGE				-6001314
#define PLUGIN_SESSION_NO_METADATA_RECIPIENT_EMAIL			-6001315
#define PLUGIN_SESSION_NO_METADATA_RECIPIENT_PHONE			-6001316
#define PLUGIN_SESSION_SENDING_MODE_VALUE				-6001317
#define PLUGIN_SESSION_OID_FORMAT_ACCESSIBLE				-6001318
#define PLUGIN_SESSION_NO_METADATA_ACCESSIBLE				-6001319
#define PLUGIN_SESSION_DOCUMENT_NOT_ACCESSIBLE				-6001320
#define PLUGIN_SESSION_ADVICE_SENDING_ERROR				-6001321
#define PLUGIN_SESSION_ADVICE_SENDING_DISABLED				-6001322

/******************************************/
/*	plugin konserwacji warto dowodowej	*/
/******************************************/
#define PLUGIN_CONSERVATE_CONF_INVALID_TIME_FORMAT				-6002000
#define PLUGIN_CONSERVATE_CONF_EXACT_TIME_MISSING				-6002001
#define PLUGIN_CONSERVATE_TS_SERVER_ADDRESS_UNDEFINED				-6002002
#define PLUGIN_CONSERVATE_TS_SERVER_PORT_UNDEFINED				-6002003
#define PLUGIN_CONSERVATE_TS_SERVER_TIMEOUT_UNDEFINED				-6002004

/******************************************/
/*	plugin archiwizacji dokumentow	*/
/******************************************/
#define PLUGIN_ARCHIVING_INIT_PLUGIN_ERROR					-6003000
#define PLUGIN_ARCHIVING_IMPROPER_IN_USE_OID_FORMAT				-6003001

/******************************/
/*	bledy soap serwera	*/
/******************************/
#define BMDSOAP_SERVER_EMPTY_LOGIN_CERTIFICATE					-7001000	/* certyfikat logujacy klienta soap jest pusty */
#define BMDSOAP_SERVER_BMD_INIT_ERROR						-7001001
#define BMDSOAP_SERVER_BMD_INFO_CREATE_ERROR					-7001002
#define BMDSOAP_SERVER_CONF_READ_ERROR						-7001003
#define BMDSOAP_SERVER_LOGIN_SET_ERROR						-7001004
#define BMDSOAP_SERVER_PFX_SET_ERROR						-7001005
#define BMDSOAP_SERVER_BMD_CERT_SET_ERROR						-7001006
#define BMDSOAP_SERVER_PFX_AS_SET_ERROR						-7001007
#define BMDSOAP_SERVER_BMD_LOGIN_ERROR						-7001008
#define BMDSOAP_SERVER_ROLE_SET_ERROR						-7001009
#define BMDSOAP_SERVER_EMPTY1_PARAM_ERROR						-7001011
#define BMDSOAP_SERVER_EMPTY2_PARAM_ERROR						-7001012
#define BMDSOAP_SERVER_EMPTY3_PARAM_ERROR						-7001013
#define BMDSOAP_SERVER_EMPTY4_PARAM_ERROR						-7001014
#define BMDSOAP_SERVER_EMPTY5_PARAM_ERROR						-7001015
#define BMDSOAP_SERVER_EMPTY6_PARAM_ERROR						-7001016
#define BMDSOAP_SERVER_EMPTY7_PARAM_ERROR						-7001017
#define BMDSOAP_SERVER_EMPTY8_PARAM_ERROR						-7001018
#define BMDSOAP_SERVER_EMPTY9_PARAM_ERROR						-7001019
#define BMDSOAP_SERVER_SOCKET_BIND_ERROR						-7001020
#define BMDSOAP_SERVER_GET_DTG_FROM_DTGSET					-7001021
#define BMDSOAP_SERVER_GET_FILE_FROM_DTG						-7001022
#define BMDSOAP_SERVER_LOB_REQUEST_ERROR						-7001023
#define BMDSOAP_SERVER_ADD_FILE_LOB_REQUEST_ERROR				-7001024
#define BMDSOAP_SERVER_TEMP_DIR_UNDEF						-7001025
#define BMDSOAP_SERVER_FINAL_FILENAME_UNDEF					-7001026
#define BMDSOAP_SERVER_TEMP_FILENAME_UNDEF					-7001027
#define BMDSOAP_SERVER_INPUT_FILE_UNDEF						-7001028
#define BMDSOAP_SERVER_INVALID_LIMIT_VALUE					-7001029
#define BMDSOAP_SERVER_INVALID_FILE_ID						-7001030
#define BMDSOAP_SERVER_INVALID_FILE_PROPERTY					-7001031
#define BMDSOAP_SERVER_INPUT_SIG_UNDEF						-7001032
#define BMDSOAP_SERVER_GCWD_ERROR							-7001033
#define BMDSOAP_SERVER_TMP_DIR_CREATE_ERROR					-7001034
#define BMDSOAP_SERVER_CD_TMP_DIR							-7001035
#define BMDSOAP_SERVER_XML_FILE_NOT_FOUND						-7001036
#define BMDSOAP_SERVER_NO_PDF_FILE							-7001037
#define BMDSOAP_SERVER_TMP_DIR_OPEN_ERROR						-7001038
#define BMDSOAP_SERVER_GET_CHUNK_NOT_FOUND					-7001039
#define BMDSOAP_SERVER_SEARCH_OPERATOR_INVALID					-7001040
#define BMDSOAP_SERVER_SEARCH_LOGICAL_INVALID					-7001041
#define BMDSOAP_SERVER_SEARCH_SORT_ORDER_INVALID				-7001043
#define BMDSOAP_SERVER_SEARCH_SORT_OID_INVALID					-7001044
#define BMDSOAP_SERVER_INVALID_TRANSACTION_ID					-7001045
#define BMDSOAP_SERVER_TRANSACTION_TIMEOUT					-7001046
#define BMDSOAP_SERVER_UNDEFINED_FILE_HASH					-7001047
#define BMDSOAP_SERVER_INVALID_FILE_HASH						-7001048
#define BMDSOAP_SERVER_INVALID_UPDATE_REASON					-7001049
#define BMDSOAP_SERVER_INVALID_TRANSMISSION_TYPE				-7001050
#define BMDSOAP_SERVER_SEARCH_RESULT_FORMAT_INVALID				-7001051
#define BMDSOAP_SERVER_NO_SELECTED_PKI_METADATA					-7001052
#define BMDSOAP_SERVER_GETPKI_METADATA_ERR					-7001053
#define BMDSOAP_SERVER_LOCK_FILE_ERROR						-7001054
#define BMDSOAP_SERVER_LOCK_FILE_EXISTS_ERROR					-7001055
#define BMDSOAP_SERVER_STREAM_TIMEOUT						-7001056
#define BMDSOAP_SERVER_METADATA_VALUE_TOO_LONG					-7001057
#define BMDSOAP_SERVER_XSLT_PATTERN_DICTIONARY_OPEN_ERROR			-7001058
#define BMDSOAP_SERVER_BAD_SIGNATURE_TYPE_PARAM					-7001059
#define BMDSOAP_SERVER_SIGNATURE_TYPE_NOT_SPECIFIED				-7001060
#define BMDSOAP_SERVER_NO_WSSECURITY_CERTIFICATE				-7001061
#define BMDSOAP_SERVER_NO_DEFAULT_LOGIN_CERTIFICATE				-7001062
#define BMDSOAP_SERVER_INVALID_ARCHIVE_FILE_ID					-7001063
#define BMDSOAP_SERVER_EMPTY_FILE_NAME							-7001064


#define BMDSOAP_SERVER_LOBS_NOT_FINISHED					-7001100
#define BMDSOAP_SERVER_TOTAL_FILESIZE_INVALID					-7001101
#define BMDSOAP_SERVER_PACK_NUMBER_INVALID					-7001102
#define BMDSOAP_SERVER_INTERNAL_OPERATION_ERROR					-7001103
#define BMDSOAP_SERVER_CSV_ESCAPING_ERROR					-7001104
#define BMDSOAP_SERVER_CANNOT_OBTAIN_METADATA_TYPE				-7001105
#define BMDSOAP_SERVER_METADATA_TO_CSV_CONVERSION_ERROR				-7001106
#define BMDSOAP_SERVER_LOBS_NOT_FINISHED_TRANSACTION				-7001107
#define BMDSOAP_SERVER_ERR_CREATING_TRANSACTION_MARKER				-7001108

#define BMDSOAP_SERVER_ADVANCED_SEARCH_INVALID_FIRST_LOG_OPER		-7001200
#define BMDSOAP_SERVER_ADVANCED_SEARCH_INVALID_LOG_OPER			-7001201
#define BMDSOAP_SERVER_ADVANCED_SEARCH_INVALID_LOG_OPER_MISSING		-7001202
#define BMDSOAP_SERVER_ADVANCED_SEARCH_INVALID_ADJUSTMENT_OPER		-7001203
#define BMDSOAP_SERVER_ADVANCED_SEARCH_INVALID_ADJUSTMENT_ARG		-7001204
#define BMDSOAP_SERVER_ADVANCED_SEARCH_INVALID_STATEMENT			-7001205
#define BMDSOAP_SERVER_NO_TRANSACTION_STARTED					-7001206
#define BMDSOAP_SERVER_TRANSACTION_ALREADY_STARTED				-7001207
#define BMDSOAP_SERVER_CANNOT_CREATE_TRANSACTION_MARKER			-7001208
#define BMDSOAP_SERVER_CHDIR_ERROR							-7001209
#define BMDSOAP_SERVER_ERR_CREATING_TRANSMISSION_MARKER			-7001210
#define BMDSOAP_SERVER_ERR_REMOVING_TRANSMISSION_MARKER			-7001211
#define BMDSOAP_SERVER_ERR_CREATING_TRANSMITTED_MARKER			-7001212
#define BMDSOAP_SERVER_OPENDIR_ERROR						-7001213
#define BMDSOAP_CLOSING_EMPTY_TRANSACTION						-7001214
#define BMDSOAP_SERVER_INTERNAL_ERROR						-7001215
#define BMDSOAP_SERVER_IDLE_TRANSACTION_TIMEOUT					-7001216
#define BMDSOAP_SERVER_PID_FILE_ERROR						-7001221
#define BMDSOAP_SERVER_TRANSMISSION_BROKEN					-7001222
#define BMDSOAP_SERVER_UNDEFINED_METADATA_OID_FORM				-7001223

#define BMDSOAP_SERVER_USER_USER_INFO_UNDEFINED					-7001300
#define BMDSOAP_SERVER_USER_SERIAL_UNDEFINED					-7001301
#define BMDSOAP_SERVER_USER_ISSUER_UNDEFINED					-7001302
#define BMDSOAP_SERVER_USER_NAME_UNDEFINED					-7001303
#define BMDSOAP_SERVER_USER_IDENTITY_UNDEFINED					-7001304
#define BMDSOAP_SERVER_USER_DEFAULT_GROUP_UNDEFINED				-7001305
#define BMDSOAP_SERVER_USER_DEFAULT_ROLE_UNDEFINED				-7001306
#define BMDSOAP_SERVER_USER_ID_UNDEFINED					-7001307
#define BMDSOAP_SERVER_USER_DEFAULT_CLASS_UNDEFINED				-7001308

#define BMDSOAP_SERVER_ROLE_INFO_NOT_DEFINED					-7001401
#define BMDSOAP_SERVER_ROLE_ACTIONS_NOT_DEFINED					-7001402
#define BMDSOAP_SERVER_ROLE_ROLE_RIGHTS_NOT_DEFINED				-7001403
#define BMDSOAP_SERVER_ROLE_NAME_NOT_DEFINED					-7001404
#define BMDSOAP_SERVER_ROLE_ID_NOT_DEFINED					-7001405

#define BMDSOAP_SERVER_GROUP_INFO_NOT_DEFINED					-7001501
#define BMDSOAP_SERVER_GROUP_NAME_NOT_DEFINED					-7001502
#define BMDSOAP_SERVER_GROUP_ID_NOT_DEFINED					-7001503

#define BMDSOAP_SERVER_CLASS_INFO_NOT_DEFINED					-7001601
#define BMDSOAP_SERVER_CLASS_NAME_NOT_DEFINED					-7001602
#define BMDSOAP_SERVER_CLASS_ID_NOT_DEFINED					-7001603


#define BMDSOAP_SERVER_SEC_CAT_INFO_NOT_DEFINED					-7001701
#define BMDSOAP_SERVER_SEC_CAT_NAME_NOT_DEFINED					-7001702


#define BMDSOAP_OWNER_NOT_SPECIFIED						-7001800
#define BMDSOAP_OWNER_GROUP_NO_SPECIFIED					-7001801
#define BMDSOAP_LINK_NAME_NOT_SPECIFIED						-7001802
#define BMDSOAP_POINTING_ID_NOT_SPECIFIED					-7001803
#define BMDSOAP_FILE_ID_NOT_SPECIFIED						-7001804
#define BMDSOAP_NO_CRITERIA_SPECIFIED						-7001805

#define BMDSOAP_INCORRECT_VISIBILITY_VALUE					-7001900


/******************************************/
/*	bledy biblioteki kompresujacej	*/
/******************************************/
#define ZLIB_COMP_SRC_FILE_OPEN_ERROR						-7002000
#define ZLIB_COMP_DST_FILE_OPEN_ERROR						-7002001
#define ZLIB_DECOMP_SRC_FILE_OPEN_ERROR						-7002002
#define ZLIB_DECOMP_DST_FILE_OPEN_ERROR						-7002003
#define ZLIB_DECOMP_NO_FILES_FOR_COMPRESSION					-7002004
#define ZLIB_DECOMP_NO_OUTPUT_FILE							-7002005

/******************************************/
/*	bledy biblioteki conwersji z xml'a	*/
/******************************************/
#define BMDSOAP_SERVER_PARSE_XSL_ERROR						-7003000
#define BMDSOAP_SERVER_PARSE_XML_ERROR						-7003001
#define BMDSOAP_SERVER_APPLY_STYLE_ERROR						-7003002
#define BMDSOAP_SERVER_SAVE_HTML_ERROR						-7003003
#define BMDSOAP_SERVER_XML_PATTERN_UNDEF						-7003004
#define BMDSOAP_SERVER_XML_PATTERN_CANT_OPEN					-7003005

/******************************************/
/* bledy biblioteki libbmdxpath           */
/******************************************/
#define BMDXPATH_CTX_ERROR								-7004000
#define BMDXPATH_NAMESPACE_REGISTER_ERROR						-7004001
#define BMDXPATH_NAMESPACE_EXTRACT_ERROR						-7004002
#define BMDXPATH_XPATH_EVALUATE							-7004003
#define BMDXPATH_XPATH_NO_RESULT							-7004004
#define BMDXPATH_XPATH_GET_NODES							-7004005

/******************************************/
/* bledy znacznika czasu                  */
/******************************************/
#define BMD_TIMESTAMP_RESOURCE_NOT_AVAILABLE					-7005000
#define BMD_TIMESTAMP_OPERATION_FAILED						-7005001
#define BMD_TIMESTAMP_NO_TSA_FIELD						-7005002


/******************************************/
/* bledy Delivery Authority itp           */
/******************************************/
/** Niepoprawne opcje wywołania programu.
 * Upewnij się, że wywołujesz program zgodnie z instrukcją.*/
#define BMD_DA_ERROR_PARSE_OPTIONS	-7006000

/** Nie można pobrać konfiguracji programu z podanego pliku konfiguracyjnego.
 * Sprawdź, czy podany plik istnieje i ma dobry format.*/
#define BMD_DA_ERROR_GET_CONFIGURATION	-7006001

/** Wystąpił błąd przy łączeniu się z bazą danych.
 * Najprawdopodobniej podano nieprawidłowe parametry bazy danych,
 * takie jak adres hosta, port, lub hasło użytkownika bazy danych.
 * Możliwe jest także, że wystąpiły problemy sieciowe. */
#define BMD_DA_ERROR_CONNECT_TO_DATABASE -7006002

/** Nie można wczytać pliku konfiguracyjnego.
 * Możliwe, że podano nieprawidłową ścieżkę do pliku, albo użytkownik
 * z którego uruchamiana jest aplikacja nie ma prawa do odczytu tego pliku.
 * */
#define BMD_DA_ERROR_LOAD_CONFIGURATION_FILE	-7006003

/** Nie można pobrać jednej z opcji konfiguracyjnych.*/
#define BMD_DA_ERROR_GET_CONFIGURATION_OPTION	-7006004

/** Nie można pobrać określonej przez użytkownika specjalnej wartości.
 * Wartość specjalna, to taka, która jest przechowywana w bazie danych,
 * a nie jest dostępna z poziomu formularza XML, np. czas wystawienia UPO.
 * Możliwe, że podany klucz wartości specjalnej nie został rozpoznany.*/
#define BMD_DA_ERROR_CANNOT_GET_SPECIAL_VALUE	-7006005

/** Błąd w operacji na bazie danych.
 * Należy przeanalizować logi aplikacji i serwera BMD.*/
#define BMD_DA_ERROR_DATABASE_OPERATION_ERROR	-7006006

/** Nieprawidłowe wypełnienie struktury daexport_options_t, uniemożliwiające
 * dalsze działanie aplikacji.
 * Należy sprawdzić w logach aplikacji, jakiego pola dotyczy błąd.*/
#define BMD_DA_ERROR_STRUCT_OPTIONS	-7006007

/** Wystąpił problem z pobiearaniem wyników zapytania.
 * Możliwe jest także, że wynik zapytania ma inną formę, niż spodziewana,
 * albo znaleziono nieprawidłową liczbę rekordów.*/
#define BMD_DA_ERROR_DATABASE_RESULT_ERROR	-7006008

/** Wystąpił błąd kodowania/dekodowania ciągu znaków na ciąg, który
 * można zapisać w bazie danych (funkcje (un)escapeByteaString()).*/
#define BMD_DA_ERROR_DATABASE_BYTEA_ERROR	-7006009

/** Wystąpił błąd podczas przygotowywania danych do logowania do BAE.
 * Najprawdopodobniej podano nieprawidłowe dane serwera lub niepoprawne
 * pliki PKI.*/
#define BMD_DA_ERROR_PREPARE_TO_LOGIN_TO_BAE	-7006010

/** Wystąpił błąd podczas próby połączenia z serwerem BAE.
 * Mogły wystąpić problemy sieciowe. Należy sprawdzić logi aplikacji
 * oraz logi serwera BMD. Możliwe, że certyfikat aplikacji nie został
 * dodany do konfiguracji serwera (attributes.db, serials_acl.conf).*/
#define BMD_DA_ERROR_LOGIN_TO_BAE	-7006011

/** Nie udało się ustawić roli aplikacji w serwerze BAE.
 * Należy przeanalizować logi BAE, możliwe, że dokonano nieprawidłowego
 * wpisu w pliku attributes.db.*/
#define BMD_DA_ERROR_SET_ROLE	-7006011

/** Nie udało się wylogować z archiwum. Należy sprawdzić logi serwera BAE.*/
#define BMD_DA_ERROR_LOGOUT_FROM_BAE	-7006012

/** Nie udało się utworzyć datagramu.
 * Należy przeanalizować logi aplikacji.*/
#define BMD_DA_ERROR_DATAGRAM_CREATE	-7006013

/** Nie udało się ustawić w datagramie treści dokumentu.
 * Możliwe że dane są niepoprawne. Możliwe, że nie dało rady utworzyć bufora generycznego.
 * Możliwe, że nazwa pliku jest niepoprawna. Proszę przeanalizować logi aplikacji.
 * */
#define BMD_DA_ERROR_DATAGRAM_SET_CONTENT	-7006014

/** Nie udało się ustawić metadanych datagramu.
 *
 * */
#define BMD_DA_ERROR_DATAGRAM_SET_METADATA	-7006015

/** Struktura z informacjami o korespondencji nie została poprawnie wypełniona.
 * Możliwe, że wymagane pola nie zostały wypełnione, albo wykorzystanie struktury
 * spowodowałoby wycieki pamięci, poprzez ponowne wypełnianie zaalokowanych pól.*/
#define BMD_DA_ERROR_STRUCT_CORRESPONDENCE	-7006016

/** Struktura z informacjami o formularzu nie została poprawnie wypełniona.*/
#define BMD_DA_ERROR_STRUCT_FORMS	-7006017

/** Błąd podczas parsowania dokumentu XML.*/
#define BMD_DA_ERROR_XML_PARSE_DOCUMENT	-7006018

/** Błąd inicjalizacji kontekstu XPath dla dokumentu XML.*/
#define BMD_DA_ERROR_XML_XPATH_CTX	-7006019

/** Bład rejestracji przestrzeni nazw w kontekście XPath.*/
#define BMD_DA_ERROR_XML_XPATH_NAMESPACES	-7006020

/** Struktura z informacjami o kliencie (urzędzie) nie została poprawnie wypełniona.*/
#define BMD_DA_ERROR_STRUCT_CUSTOMERS	-7006021

/** Błąd pobierania wartości lub specjalnej wartości dla zadanego metatagu.
 * Możliwe, że podano nieprawidłowy tag. Albo w bazie danych / pamięci programu
 * nie jest przechowywana żądana wartość.*/
#define BMD_DA_ERROR_METADATA_GET_VALUE	-7006022

/** Błąd pobierania adresu xpath z bazy danych dla zadanego tagu.*/
#define BMD_DA_ERROR_METADATA_GET_XPATH	-7006023

/** Błąd dodawania datagramu do datagramsetu.*/
#define BMD_DA_ERROR_DATAGRAMSET	-7006024

/** Błąd wysyłki datgramu do archiwum.*/
#define BMD_DA_ERROR_DATAGRAM_SEND	-7006025

/** Nie udało się pobrać opcji połączenia z BAE.*/
#define BMD_DA_ERROR_GET_BAE_OPTIONS	-7006026

/** Błąd pobierania informacji o kliencie (urzędzie).*/
#define BMD_DA_ERROR_GET_CUSTOMER	-7006027

/** Błąd występuje, gdy próba pobrania dokumentu z bazy danych zakończy się niepowodzeniem,
 * przykładowo zapytanie o dokument ma nieprawidłową formę.*/
#define BMD_DA_ERROR_GET_CORRESPONDENCE	-7006028

/** Bład występuje, gdy próba pobrania informacji o formularzu dla danej domeny
 * zakończy się niepowodzeniem.
 * Możliwe, że podano nieprawdiłowy oid formularza.*/
#define BMD_DA_ERROR_GET_FORM	-7006029

/** Błąd przygotowania datagramu z dokumentem do wyeksportowania.*/
#define BMD_DA_ERROR_DATAGRAM_PREPARE	-7006030

/** Błąd eksportu datagramu do Bezpiecznego Archiwum Elektronicznego.*/
#define BMD_DA_ERROR_DATAGRAM_EXPORT	-7006031

/** Błąd pobierania informacji o domenach, w których
 * znajdują się dokumenty do wyeksportowania.*/
#define BMD_DA_ERROR_GET_DOMAINS	-7006032


/************************************/
/*	bledy podsystemu sieciowego	*/
/************************************/
#define BMD_ERR_NET_CONNECT								-8001000	/* blad - nie mozna polaczyc sie przez siec */
#define BMD_ERR_NET_RESOLV								-8001001	/* blad - nie mozna polaczyc sie przez siec - nie mozna rozwiazac nazwy domenowej*/
#define BMD_ERR_NET_READ								-8001002	/* blad - nie mozna czytac z gniazda */
#define BMD_ERR_NET_WRITE								-8001003	/* blad - nie mozna pisac do gniazda */
#define CONNECTION_REJECTED								-8001004
#define SESS_DEV_ERR									-8001005
#define SESS_ATTR_ERR									-8001006
#define SEND_PUSH_ERR									-8001007
#define SEND_FLUSH_ERR									-8001008
#define RECV_POP_ERR									-8001009


/************************/
/*	bledy powszechne	*/
/************************/
#define BMD_ERR_UNIMPLEMENTED								-9001000	/* funkcjonalnosci niezaimplementowana */
#define BMD_ERR_NOTAVAIL								-9001001	/* zadana funkcjonalnosc nie jest dostepna */
#define BMD_ERR_OP_FAILED								-9001002	/* operacja nie powiodla sie */
#define BMD_ERR_CREDENTIALS								-9001003	/* bledne dane uwierzytelniajace */
#define BMD_ERR_RAND									-9001004	/* blad podsystemu liczb losowych */
#define BMD_ERR_COMPLETE								-9001005 	/* blad - nie mozna kontynuowac operacji bo juz ja wykonano */
#define BMD_ERR_NOTINITED								-9001006 	/* blad - niezaincjalizowane dane */
#define BMD_ERR_PROTOCOL								-9001007 	/* blad - cos nie tak z protokolem - np. nie zgadzajacy sie ki_mod_n czy cos w tym stylu */
#define BMD_ERR_OVERFLOW								-9001008 	/* blad - za duzo jakis obiektow - np. w cache */
#define BMD_ERR_NOT_ALLOWED								-9001010 	/* blad - brak uprawnien do wykoania operacji */
#define BMD_ERR_TIMEOUT									-9001011 	/* blad - uplynal czas na wykonanie operacji */
#define BMD_ERR_USER_OP_FAILED								-9001012 	/* nie mozna poprawnie wykonac operacji po stronie klienckiej */
#define BMD_ERR_LOGIN_AGAIN								-9001013 	/* nalezy sie ponownie zalogowac - np. z uwagi na restart serwera */
#define BMD_ERR_RESOURCE_NOTAVAIL  							-9001014 	/* zasob niedostepny - np. baza danych lub znacznik czasu */
#define BMD_CANCEL 									-9001015 	/* anulowano operacje*/
#define PLUGIN_NOT_LOADED			 					-9001016
#define VERSION_NUMBER_ERR                     						-9001017 	/* Niezgodna wersja protokołu */
#define LOB_CANCELED_BY_CLIENT                   					-9001018 	/* Klient perwał wysyłkę plików */
#define FILE_SIZE_ERR                        				  	  	-9001019 	/* Wysyłany plik jest zbyt duży */
#define SET_GEN_BUF_FAILED 								-9001020
#define BMD_DB_CONNECTION_ERROR								-9001021
#define SQL_CLEARANCE_AMBIGUITY_ERR							-9001022
#define ERR_NO_USER_WITH_GIVEN_ID							-9001023
#define BMD_ERR_INVALID_PTR								-9001024 /* niepoprawny wskaznik */
#define BMD_ERR_WRONG_SERIALISATION_TYPE						-9001025
#define BMD_ERR_PTHREAD_CREATE								-9001026
#define BMD_ERR_PTHREAD_JOIN								-9001027

#define BMD_ERR_INTERRUPTED						 		-9001028    /* blad - przerwane przez uzytkownika operacja */

#define BMD_ERR_PREPARE_ANSWER							-9001029
#define BMD_ERR_OBSOLETE								-9001030

/************************************************************/
/*	bledy systemowe (pamiec, pliki, bledy odczytu itd)	*/
/************************************************************/
#define BMD_ERR_MEMORY									-9002000 	/* nie powiodla sie alokacja pamiecie */
#define BMD_ERR_DISK									-9002001 	/* operacja odczytu, pobrania wlasciwosci, zapisu na dysk nie powiodlo sie */
#define BMD_ERR_FORMAT									-9002002 	/* niepoprawny format danych */
#define BMD_ERR_NODATA									-9002003 	/* blad - nie ma danych */
#define BMD_ERR_TOO_BIG									-9002004 	/* obiekt za duzy */
#define NO_MEMORY 									-9002005
#define NO_SERIAL_FILE_ERR 								-9002006 	/* kod bledu zwiazany z brakiem pliku z serialem */
#define MAX_MEMORY_EXCEEDED								-9002007

/************************/
/*	bledy argumentow	*/
/************************/
#define BMD_ERR_PARAM1									-9003000 	/* niepoprawna wartosc 1. parametru */
#define BMD_ERR_PARAM2									-9003001 	/* niepoprawna wartosc 2. parametru */
#define BMD_ERR_PARAM3									-9003002 	/* niepoprawna wartosc 3. parametru */
#define BMD_ERR_PARAM4									-9003003 	/* niepoprawna wartosc 4. parametru */
#define BMD_ERR_PARAM5									-9003004 	/* niepoprawna wartosc 5. parametru */
#define BMD_ERR_PARAM6									-9003005 	/* niepoprawna wartosc 6. parametru */
#define BMD_ERR_PARAM7									-9003006 	/* niepoprawna wartosc 7. parametru */
#define BMD_ERR_PARAM8									-9003007 	/* niepoprawna wartosc 8. parametru */
#define BMD_ERR_PARAM9									-9003008 	/* niepoprawna wartosc 9. parametru */
#define BMD_ERR_PARAM10									-9003009 	/* niepoprawna wartosc 10. parametru */
#define BMD_ERR_PARAM11									-9003010
#define BMD_ERR_PARAM12                                         	     		-9003011
#define BMD_ERR_PARAM13									-9003012
#define BMD_ERR_PARAM14									-9003013
#define BMD_ERR_PARAM15									-9003014
#define BMD_ERR_PARAM16									-9003015
#define NO_ARGUMENTS 									-9003016 	/* brak argumentow */
#define BMD_ERR_PARAM17									-9003017
#define BMD_ERR_PARAM18									-9003018

/******************************************/
/*	bledy podsystemu kryptograficznego	*/
/******************************************/
#define BMD_ERR_PKI_VERIFY								-9004000 	/* blad - niepoprawna weryfikacja PKI */
#define BMD_ERR_PKI_SIGN								-9004001 	/* nie mozna wykonac podpisu */
#define BMD_ERR_PKI_ENCRYPT								-9004002 	/* nie mozna zaszyfrowac danych  */
#define BMD_ERR_PKI_DECRYPT								-9004003 	/* nie mozna zdeszyfrowac danych */
#define BMD_ERR_HASH_VERIFY								-9004004 	/* blad - niepoprawna weryfikacja skrótu*/
#define BMD_ERR_KEY_CORRESP								-9004005 	/* blad - klucz publiczny nie koresponduje z prywatnym */
#define BMD_ERR_SIGN_VERIFY								-9004006 	/* blad - podpis nie zostal poprawnie zweryfikowany */
#define BMD_WRONG_SERVER_CERT 								-9004007 	/* zwracane przez bmd_login, gdy wykorzystywany niewlasciwy certyfikat serwera*/
#define BMD_ERR_PKI_NO_CRL								-9004008 	/* zwracane przez bmd_verify_cert_with_crl() w cert_api.c, gdy nie podamy CRL i nie zostanie pobrana przez sieć*/
#define PKI_OBJ_ERR_CODE_BIO     							-9004009 	/* Nie mozna stworzyc wewnetrznego BIO do reprezentacji gen_buf */
#define PKI_OBJ_ERR_CODE_GENBUF  							-9004010 	/* Z generycznego bufora nie mozna bylo zapisac do BIO */
#define PKI_OBJ_ERR_CODE_P12_NO_CERT 							-9004011 	/* Plik p12/pfx nie zawieral zadnych certyfikatow ( ????? ) */
#define PKI_OBJ_ERR_CODE_P12_NO_EE_CERT 						-9004012 	/* Plik p12/pfx nie zawieral zadnych certyfikatow jednostki koncowej */
#define PKI_OBJ_ERR_CODE_P12_PASS 							-9004013 	/* Bledne haslo do pliku p12/pfx */
#define PKI_OBJ_ERR_CODE_FORMAT_UNKNOWN 						-9004014 	/* nieznany format pliku */
#define PKI_OBJ_ERR_CODE_FORMAT_NOT_CERT 						-9004015 	/* podany plik nie jest certyfikatem */
#define CERT_CHAIN_BAD									-9004016 	/* niepoprawna sciezka weryfikacji */
#define ERR_CERT_CHAIN_INT								-9004017 	/* blad inicjalizacji sciezki weryfikacji */
#define ERR_CERT_CHAIN_DIR								-9004018 	/* blad ustawienia katalogu sciezki weryfikacji */
#define ERR_CERT_CHAIN_GET								-9004019 	/* blad pobrania sciezki weryfikacji */
#define ERR_CERT_ISSUER_GET								-9004020 	/* blad pobrania wystawcy z certyfikatu */
#define PKCS15_ERR									-9004021 	/* blad PKCS15 */
#define PKCS15_PRIVKEY_ERR								-9004022 	/* blad klucza prywatnego */
#define PKCS15_CERT_ERR									-9004023 	/* blad klucza certyfikatu */
#define ERR_READ_PRIVKEY								-9004024
#define ERR_READ_PUBKEY									-9004025
#define ERR_READ_X509									-9004026
#define CERTIFICATE_INVALID			 					-9004027
#define BN_ERROR 									-9004028 	/* kody bledow OpenSSL added by bozon */

#define BMD_PKCS11_NO_CARDS								-9004029
#define BMD_PKCS11_NO_CERTS								-9004030
#define BMD_PKCS11_PIN_LOCKED								-9004031
#define ERR_PKCS12_NO_ENDENTITY_CERT					-9004032


#define CERT_CHAIN_OK									1 /* poprawna sciezka weryfikacji */

/******************************/
/*	bledy konfiguracji	*/
/******************************/
#define ERR_NO_CONFIG_FILE 								-9005000
#define KLIENT_CONF_FILE_ERROR_MEMORY							-9005001
#define KLIENT_CONF_FILE_ERROR_FILE							-9005002
#define KLIENT_CONF_REGEXP_ERROR_COMPILE						-9005003
#define KLIENT_CONF_REGEXP_ERROR_MATCH							-9005004
#define KLIENT_CONF_REGEXP_ERROR_TOO_MANY_MATCH						-9005005
#define KLIENT_CONF_FILE_IMPROPER							-9005006
#define KLIENT_CONF_FILE_IMPROPER_LINE							-9005007
#define KLIENT_CONF_FILE_IMPROPER_VALUES_FILE   					-9005008
#define KLIENT_CONF_IMPROPER_KRYTERIUM 							-9005009
#define KLIENT_CONF_IMPROPER_REGEXP 							-9005010
#define KLIENT_CONF_IMPROPER_KRYTERIUM_VALUE 						-9005011
#define KLIENT_CONF_IMPROPER_OPTION 							-9005012
#define KLIENT_CONF_IMPROPER_OPTION_VALUE 						-9005013
#define PCRE_GET_STRING_ERROR								-9005014 	/* bledy dotyczace parsowania pliku konfiguracyjnego kontrolki.conf */

/************/
/*	bledy	*/
/************/
#define PCRE_COMPILATION_ERR 								-9006000
#define FUNC_ERR 									-9006001
#define PARSE_BAD									-9006002
#define BAD_P12_PASSWORD								-9006003

#define UNCOMPARABLE_ARGUMENTS 								-9007000
#define WRITE_TO_FILE_FAIL								-9007001
#define NO_OUT_BUFFER									-9007002
#define TIMEERR										-9007003
#define WRONG_CONTENT									-9007004
#define ERR_READ_IN_FILE								-9007005
#define BIO_NEW_ERR									-9007006
#define SIGN_ERROR									-9007007
#define ERR_GET_EMAIL									-9007008
#define ERR_GET_METADATA 								-9007009
#define NO_METADATA_FOUND								-9007010 /*Nie mozna znalezc metadanej za pomoca funkcji CheckAddMetadataExistenceInDb*/
#define GET_ALL_METADATA_FROM_ID_FAILURE						-9007011
#define BMD_ERR_OPEN_FILE								-9007012

#define CRL_DP_NOT_FOUND								-9008000
#define OCSP_DP_NOT_FOUND								-9008001


/* nowe kody bledow */
#define LIBBMDASN1_COMMON_UTILS_UNRECOGNIZED_ATTRIBUTE_OID				-10000101	/* Unrecognized atrribute object identifier */
#define LIBBMDASN1_COMMON_UTILS_NAME_HAS_NO_RDNSEQUENCE_FIELD				-10000102	/* Asn1 name has no rdn sequence field */
#define LIBBMDASN1_COMMON_UTILS_CLONE_RDNSEQUENCE					-10000103	/* Error in cloning rdn sequence */
#define LIBBMDASN1_COMMON_UTILS_CLONE_ATTRVALUE						-10000104	/* Error in cloning AttributeValue*/
#define LIBBMDASN1_COMMON_UTILS_CLONE_ATTRVALUE_SURNAME					-10000105	/* Error in cloning surname attribute value*/
#define LIBBMDASN1_COMMON_UTILS_CLONE_ATTRVALUE_GIVENNAME				-10000106	/* Error in cloning given name attribute value*/
#define LIBBMDASN1_COMMON_UTILS_CLONE_ATTRVALUE_COMMONNAME				-10000107	/* Error in cloning common name attribute value*/
#define LIBBMDASN1_COMMON_UTILS_CLONE_ATTRVALUE_ORGANIZATION				-10000108	/* Error in cloning organization attribute value*/
#define LIBBMDASN1_COMMON_UTILS_CLONE_ATTRVALUE_COUNTRY					-10000109	/* Error in cloning country attribute value*/
#define LIBBMDASN1_COMMON_UTILS_CLONE_ATTRVALUE_LOCALITY				-10000110	/* Error in cloning locality attribute value*/
#define LIBBMDASN1_COMMON_UTILS_CLONE_ATTRVALUE_ORGANIZATIONALUNIT			-10000111	/* Error in cloning organizational unit attribute value*/
#define LIBBMDASN1_COMMON_UTILS_UNRECOGNIZED_TIME_TYPE					-10000112	/* Unrecognized time type in attribute */
#define LIBBMDASN1_COMMON_UTILS_GET_VALUE_FROM_ATTRIBUTE				-10000113	/* Error in getting Value from AttributeTypeAndValue */
#define LIBBMDASN1_COMMON_UTILS_GET_ATTVALUE_FROM_RDNSEQUENCE				-10000114	/* Error in geeting attribute value from rdn sequence */
#define LIBBMDASN1_COMMON_UTILS_CONV_ATTRIBUTE_VALUE_TO_STR				-10000115	/* Error in converting rdn sequence to string */
#define LIBBMDASN1_COMMON_UTILS_CONV_EMAIL_TO_STRING_FROM_ATTRIBUTE			-10000116	/* Error in converting EmailAddress to string from attribute */
#define LIBBMDASN1_COMMON_UTILS_CONV_RELATIVE_DISTINGUISHED_NAME_TO_STR			-10000117	/* Error in converting RelativeDistinguishedName to string */
#define LIBBMDASN1_COMMON_UTILS_CONV_UTC_TIME_TO_STR					-10000118	/* Error in converting UTC time to string */
#define LIBBMDASN1_COMMON_UTILS_CONV_GENERALIZED_TIME_TO_STR				-10000119	/* Error in converting generalized time to string */
#define LIBBMDASN1_COMMON_UTILS_CONV_RDN_SEQUENCE_TO_STR				-10000120	/* Error in converting rdn sequence to string */
#define LIBBMDASN1_COMMON_UTILS_CONV_TELETEX_STR_TO_STR_ERR				-10000121	/* Error in converting teletextstring to string */
#define LIBBMDASN1_COMMON_UTILS_CONV_PRINTABLE_STR_TO_STR_ERR				-10000122	/* Error in converting printablestring to string */
#define LIBBMDASN1_COMMON_UTILS_CONV_UNIVERSAL_STR_TO_STR_ERR				-10000123	/* Error in converting universalstring to string */
#define LIBBMDASN1_COMMON_UTILS_CONV_UTF8_STR_TO_STR_ERR				-10000124	/* Error in converting universalstring to string */
#define LIBBMDASN1_COMMON_UTILS_CONV_BMD_STR_TO_STR_ERR					-10000125	/* Error in converting bmdstring to string */
#define LIBBMDASN1_COMMON_UTILS_CONV_ATTRIBUTE_VALUE_TO_DIRECTORY_STR			-10000126	/* Error in converting AttributeValue to DirectoryString */
#define LIBBMDASN1_COMMON_UTILS_CONV_DIRECTORY_STR_TO_STR				-10000127	/* Error in converting DirectoryString to string */
#define LIBBMDASN1_COMMON_UTILS_CONV_ANOTHER_NAME_TO_STR				-10000128	/* Error in converting another name to string */
#define LIBBMDASN1_COMMON_UTILS_CONV_IA5_STR_TO_STR					-10000129	/* Error in converting IA5String to string */
#define LIBBMDASN1_COMMON_UTILS_CONV_OR_ADDRESS_TO_STR					-10000130	/* Error in converting ORAddress to string */
#define LIBBMDASN1_COMMON_UTILS_CONV_NAME_TO_STR					-10000131	/* Error in converting Name to string */
#define LIBBMDASN1_COMMON_UTILS_CONV_EDI_PARTY_NAME_TO_STR				-10000132	/* Error in converting EDIPartyName to string */
#define LIBBMDASN1_COMMON_UTILS_CONV_UNIFORM_RESOURCE_IDENTIFIER_TO_STR			-10000133	/* Error in converting uniform resource identifier to string */
#define LIBBMDASN1_COMMON_UTILS_CONV_IP_ADDRESS_TO_STR					-10000134	/* Error in converting ip adress to string */
#define LIBBMDASN1_COMMON_UTILS_CONV_REGISTERED_ID_TO_STR				-10000135	/* Error in converting registered id to string */
#define LIBBMDASN1_COMMON_UTILS_CONV_DISTRIBUTION_POINT_TO_STR				-10000136	/* Error in converting DistributionPoint to string */
#define LIBBMDASN1_COMMON_UTILS_CONV_DIST_PNT_FULL_NAME_STR				-10000137	/* Error in converting distinguished point full name to string */
#define LIBBMDASN1_COMMON_UTILS_CONV_DIST_PNT_NAME_REL_TO_CRL_ISSUER_STR		-10000138	/* Error in converting distinguished point name relative to CRL issuer to string */
#define LIBBMDASN1_COMMON_UTILS_CONV_INTEGER_TO_LONG					-10000139	/* Error in converting asn1 integer to long */
#define LIBBMDASN1_COMMON_UTILS_CONV_LONG_TO_INTEGER					-10000140	/* Error in converting asn1 integer to long */
#define LIBBMDASN1_COMMON_UTILS_CONV_STR_TO_OID						-10000141	/* Error in converting string to oid */
#define LIBBMDASN1_COMMON_UTILS_CONV_OCTET_STR_TO_STR					-10000142	/* Error in converting octet string to string */
#define LIBBMDASN1_COMMON_UTILS_CONV_STR_TO_OCTET_STR					-10000143	/* Error in converting string to octet string */
#define LIBBMDASN1_COMMON_UTILS_CONV_GEN_BUF_TO_OCTET_STRING				-10000144	/* Error in converting octet string to string */
#define LIBBMDASN1_COMMON_UTILS_CONV_STR_TO_ANY						-10000145	/* Error in converting string to any buffer */
#define LIBBMDASN1_COMMON_UTILS_DECODE_UTF8_STRING_ERR					-10000146	/* Error in ber decoding utf8 string */
#define LIBBMDASN1_COMMON_UTILS_DECODE_INTEGER_ERR					-10000147	/* Error in ber decoding integer */
#define LIBBMDASN1_COMMON_UTILS_DECODE_DIRECTORY_STRING_DATA_ERR			-10000148	/* Error in decoding directory string data */
#define LIBBMDASN1_COMMON_UTILS_DER_ENCODING_ERR					-10000149	/* Error in der ecoding */
#define LIBBMDASN1_COMMON_UTILS_DECODE_POSTAL_ADDRESS_DATA_ERR				-10000150	/* Error in decoding postal address data */
#define LIBBMDASN1_COMMON_UTILS_DECODE_IA5_DATA_ERR					-10000151	/* Error in decoding IA5 string data */
#define LIBBMDASN1_COMMON_UTILS_DECODE_CRL_DISTPOINTS_DATA_ERR				-10000152	/* Error in decoding crl distinguished points string data */
#define LIBBMDASN1_COMMON_UTILS_GET_VALUE_FROM_ATT_TYPE_AND_VALUE			-10000153	/* Error in getting value from attribute type and value */
#define LIBBMDASN1_COMMON_UTILS_ATTRIBUTE_TYPE_NOT_PRESENT				-10000154	/* Attribute type not present in rdn sequence */
#define LIBBMDASN1_COMMON_UTILS_CLONE_ATTRVALUE_SERIALNUMBER				-10000155  /* Error in cloning serial number attribute value*/
#define LIBBMDASN1_COMMON_UTILS_CLONE_ATTRVALUE_PSEUDONYM				-10000156  /* Error in cloning pseudonym attribute value*/
#define LIBBMDASN1_COMMON_UTILS_CLONE_ATTRVALUE_STATE					-10000157  /* Error in cloning state attribute value*/
#define LIBBMDASN1_COMMON_UTILS_CLONE_ATTRVALUE_POSTALADDRESS				-10000158  /* Error in cloning postal adress attribute value*/

#define LIBBMDPR_DATAGRAMSET_TO_SMALL							-10010000 /*Datagramset za maly zeby go serializowac*/


#define LIBBMDCONFIG_LIBBMDCONFIG_GET_CONFIG_VALUE_ERR					-20000101	/*  Error in getting configuration value */
#define LIBBMDCONFIG_LIBBMDCONFIG_INVALID_CONF_FILE_PATH				-20000102	/*  Invalid configuration file path */
#define LIBBMDCONFIG_LIBBMDCONFIG_EMPTY_CONF_STRUCTURE					-20000103	/*  Empty configuration structure option */
#define LIBBMDCONFIG_LIBBMDCONFIG_OPEN_CONF_FILE_ERR					-20000104	/*  Error in openning configuration file */
#define LIBBMDCONFIG_LIBBMDCONFIG_CLOSE_CONF_FILE_ERR					-20000105	/*  Error in openning configuration file */
#define LIBBMDCONFIG_LIBBMDCONFIG_PARSE_CONF_FILE_ERR					-20000106	/*  Error in parsing configration file */
#define LIBBMDCONFIG_LIBBMDCONFIG_VALIDATE_CONF_FILE_ERR				-20000107	/*  Error in validating configration file */

#define LIBBMDCONFIG_UPDATE_FILE_OPEN_SRC_FILE_ERR					-20000201	/*  Error in openning src file for config copy */
#define LIBBMDCONFIG_UPDATE_FILE_WRITE_DEST_FILE_ERR					-20000202	/*  Error in writing to dest file in config copy */

#define LIBBMD_LICENCE_REQUIRED_AUTH_PLUGIN_ERR						-20000301	/*  Server licence requires authorization plugin */
#define LIBBMD_LICENCE_REQUIRED_CONSERVATE_PLUGIN_ERR					-20000302	/*  Server licence requires conservate plugin */
#define LIBBMD_LICENCE_REQUIRED_DVCS_PLUGIN_ERR						-20000303	/*  Server licence requires dvcs plugin */
#define LIBBMD_LICENCE_REQUIRED_SESSION_PLUGIN_ERR					-20000304	/*  Server licence requires session plugin */
#define LIBBMD_LICENCE_REQUIRED_CLEAR_PLUGIN_ERR					-20000305	/*  Server licence requires clear plugin */
#define LIBBMD_LICENCE_REQUIRED_MESSAGING_PLUGIN_ERR					-20000306	/*  Server licence requires messaging plugin */
#define LIBBMD_LICENCE_REQUIRED_ARCHIVING_PLUGIN_ERR					-20000307	/*  Server licence requires archiving plugin */

#define LIBBMDUTILS_INCORRECT_YEAR_VALUE						-20000401
#define LIBBMDUTILS_INCORRECT_MONTH_VALUE						-20000402
#define LIBBMDUTILS_INCORRECT_DAY_VALUE							-20000403
#define LIBBMDUTILS_INCORRECT_HOUR_VALUE						-20000404
#define LIBBMDUTILS_INCORRECT_MINUTE_VALUE						-20000405
#define LIBBMDUTILS_INCORRECT_SECOND_VALUE						-20000406
#define LIBBMDUTILS_INCORRECT_DATE_TIME_FORMAT						-20000407
#define LIBBMDUTILS_DATE_TIME_NOT_AFTER_NOW						-20000408
#define LIBBMDUTILS_NOT_NUMERIC_IDENTIFIER						-20000410


#define LIBBMDSQL_GET_SYMKEY_GET_ERR							-30010001	/* Error in getting symmetric key from datagram during servicing get request */
#define LIBBMDSQL_GET_SYMKEY_DECODE_ERR							-30010002	/* Error in decoding symmetric key during servicing get request */
#define LIBBMDSQL_GET_SET_SYMKEY_CTX_ERR						-30010003	/* Error in setting symmetric key context during servicing get request */
#define LIBBMDSQL_GET_CREATE_DER_SYMKEY_CTX_ERR						-30010004	/* Error in creating der from symmetric key during servicing get request */
#define LIBBMDSQL_GET_UPDATE_DER_SYMKEY_CTX_ERR						-30010005	/* Error in updating der from symmetric key during servicing get request */
#define LIBBMDSQL_GET_DELETE_DER_SYMKEY_CTX_ERR						-30010006	/* Error in deleting der from symmetric key during servicing get request */
#define LIBBMDSQL_GET_FILL_RESPONSE_DTG_WITH_DTGS_ERR					-30010007	/* Error in filling response datagramset during servicing get request */
#define LIBBMDSQL_GET_SYMMETRIC_DECRYPT_ERR						-30010008	/* Error in data symmetric decryption during servicing get request */
#define LIBBMDSQL_GET_DOCUMENT_ABSENT_IN_DB_ERR						-30010009	/* File requested to get not found in archive (possibly deleted) */

#define LIBBMDSQL_PROOFS_PARSE_SIGNATURE_STANDARD_ERR					-30020001	/* Error in parsing timestamp's standard */
#define LIBBMDSQL_PROOFS_PARSE_CERTIFICATE_VERSION_ERR					-30020002	/* Error in parsing certificate's version */
#define LIBBMDSQL_PROOFS_PARSE_CERTIFICATE_SERIAL_ERR					-30020003	/* Error in parsing certificate's serial number */
#define LIBBMDSQL_PROOFS_PARSE_CERTIFICATE_SUBJECT_DN_ERR				-30020004	/* Error in parsing certificate subject dn number */
#define LIBBMDSQL_PROOFS_PARSE_CERTIFICATE_SIG_ALG_ERR					-30020005	/* Error in parsing certificate signature's algorithm */
#define LIBBMDSQL_PROOFS_PARSE_CERTIFICATE_ISSUER_ERR					-30020006	/* Error in parsing certificate's issuer */
#define LIBBMDSQL_PROOFS_PARSE_CERTIFICATE_ISSUER_SN_ERR				-30020007	/* Error in parsing certificate issuer's serial number */
#define LIBBMDSQL_PROOFS_PARSE_CERTIFICATE_VALIDITY_NB_ERR				-30020008	/* Error in parsing certificate (not before) validity */
#define LIBBMDSQL_PROOFS_PARSE_CERTIFICATE_VALIDITY_NA_ERR				-30020009	/* Error in parsing certificate (not after) validity */
#define LIBBMDSQL_PROOFS_INSERT_SIGNATURE_TO_DB_ERR					-30020010	/* Error in inserting signature to database */
#define LIBBMDSQL_PROOFS_INSERT_SIGNATURE_LOB_TO_DB_ERR					-30020011	/* Error in inserting signature's lob to database */
#define LIBBMDSQL_PROOFS_INSERT_SIGNATURE_TABLE_TO_DB_ERR				-30020012	/* Error in inserting signature's info to prf_signature table in database */
#define LIBBMDSQL_PROOFS_INSERT_CERTIFICATE_TO_DB_ERR					-30020013	/* Error in inserting certificate to database */
#define LIBBMDSQL_PROOFS_INSERT_CERTIFICATE_LOB_TO_DB_ERR				-30020014	/* Error in inserting certificate's lob to database */
#define LIBBMDSQL_PROOFS_INSERT_CERTIFICATE_TABLE_TO_DB_ERR				-30020015	/* Error in inserting certificate's info to prf_signature table in database */
#define LIBBMDSQL_PROOFS_INSERT_CERTIFICATE_ALREADY_EXISTS_ERR				-30020016	/* Certificate already exists in database */

#define LIBBMDSQL_PRF_SIGNATURE_PARSE_SIGNATURE_STANDARD_ERR				-30030001	/* Error in parsing signature's standard */

#define LIBBMDSQL_SELECT_MAIN_CREATE_DATAGRAM_ERR					-30040001	/* Error in creating datagram during select request */
#define LIBBMDSQL_SELECT_MAIN_FILL_WITH_CRYPTO_OBJECTS_DATA_ERR				-30040002	/* Error in filling response with crypto objects data */
#define LIBBMDSQL_SELECT_MAIN_FILL_WITH_METADATA_DATA_ERR				-30040003	/* Error in filling response with metadata data */

#define LIBBMDSQL_INSERT_GET_ALL_METADATA_FROM_ID_ERR					-30050001	/* Error in getting all metadata according to docuument's id */
#define LIBBMDSQL_INSERT_REQUIRED_LOB_NOT_PRESENT_IN_REQUEST_ERR			-30050002	/* Requiered lob data not present in request datagram */
#define LIBBMDSQL_INSERT_INSERT_FILE_LOB_TO_DB_ERR					-30050003	/* Error in inserting document's lob to database */
#define LIBBMDSQL_INSERT_GET_SYMKEY_FROM_DATAGRAM_ERR					-30050004	/* Error in getting symmetric key from datagram */
#define LIBBMDSQL_INSERT_DECODE_SYMKEY_ERR						-30050005	/* Error in decoding symmetric key */
#define LIBBMDSQL_INSERT_DECRYPT_PROTOCOL_DATA_ERR					-30050006	/* Error in decrypting protocol data */
#define LIBBMDSQL_INSERT_SET_HASH_CONTEXT_ERR						-30050007	/* Error in setting hash context */
#define LIBBMDSQL_INSERT_SET_HASH_ERR							-30050008	/* Error in hashing data */
#define LIBBMDSQL_INSERT_CREATE_LOB_TRANSPORT_INFO_ERR					-30050009	/* Error in creating large object transport information */
#define LIBBMDSQL_INSERT_RECEIVELOB_CHUNK_ERR						-30050010	/* Error in receive large object chunk */
#define LIBBMDSQL_INSERT_START_LOB_TRANSPORT_TO_DB_ERR					-30050011	/* Error in beginning large object transport into database */
#define LIBBMDSQL_INSERT_END_LOB_TRANSPORT_TO_DB_ERR					-30050012	/* Error in ending large object transport into database */
#define LIBBMDSQL_INSERT_ITERATE_LOB_TRANSPORT_TO_DB_ERR				-30050013	/* Error in iterate large object transport into database */
#define LIBBMDSQL_INSERT_ADD_HASH_TO_DATAGRAM_ERR					-30050014	/* Error in adding hash to datagram */
#define LIBBMDSQL_INSERT_NO_POINTING_ID_IN_LINK_CREATION				-30050015	/* Error in adding hash to datagram */
#define LIBBMDSQL_INSERT_NO_POINTING_LOCATION_ID_IN_LINK_CREATION			-30050016	/* Error in adding hash to datagram */
#define LIBBMDSQL_INSERT_FILE_TO_LINK_TO_NOT_FOUND					-30050017	/* Error in adding hash to datagram */
#define LIBBMDSQL_INSERT_DIRECTORY_ALREADY_EXIST					-30050018	/* Error in adding hash to datagram */
#define LIBBMDSQL_INSERT_FILE_TO_LINK_IS_LINK						-30050019	/* Error in adding hash to datagram */
#define LIBBMDSQL_INSERT_UNALLOWABLE_DIR_FILE_TYPE					-30050020	/* */
#define LIBBMDSQL_INSERT_UNALLOWABLE_LINK_FILE_TYPE					-30050021	/* */
#define LIBBMDSQL_INSERT_INCORRECT_FOR_GRANT_VALUE					-30050022	/* */
#define LIBBMDSQL_NO_CASCADE_LINK_CREATED						-30050023	/* */
#define LIBBMDSQL_INCORRECT_VISIBILITY_VALUE					-30050024
#define LIBBMDSQL_NO_METADATA_FOR_UPDATE						-30050025
#define LIBBMDSQL_NOT_REGULAR_FILE_ID							-30050026
#define LIBBMDSQL_NO_LINK_CREATED								-30050027	/* w przypadku, gdy np. stwierdzono tworzenie dublikatu */
#define	LIBBMDSQL_PADES_HASH_VALUE_NOT_SPECIFIED				-30050028
#define LIBBMDSQL_PADES_HASH_ALGORITHM_NOT_SPECIFIED			-30050029
#define LIBBMDSQL_PADES_TOO_MANY_HASH_VALUE_METADATA			-30050030
#define LIBBMDSQL_PADES_TOO_MANY_HASH_ALGORITHM_METADATA		-30050031
#define LIBBMDSQL_PADES_INCORRECT_HASH_SIZE						-30050032
#define LIBBMDSQL_PADES_INCORRECT_HEX_HASH_SIZE					-30050033
#define LIBBMDSQL_PADES_NEW_CRYPTO_OBJECT_NOT_SPECIFIED			-30050034
#define LIBBMDSQL_PADES_TOO_MANY_NEW_CRYPTO_OBJECT			-30050035
#define LIBBMDSQL_ONLY_ONE_SIGNATURE_ALLOWED				-30050036


#define BMD_ERR_FILE_OWNER_NOT_SPECIFIED						-30050101
#define BMD_ERR_FILE_ID_NOT_SPECIFIED							-30050102
#define BMD_ERR_CURRENT_VERSION_FILE_ID_NOT_SPECIFIED			-30050103


#define LIBBMDSQL_DICT_CONV_METADATA_TO_GROUP_ID_ERR					-30060001	/* Error in converting metadata to user's group id */
#define LIBBMDSQL_DICT_CONV_METADATA_TO_ROLE_ERR					-30060002	/* Error in converting metadata to user's role */
#define LIBBMDSQL_DICT_CONV_METADATA_TO_SEC_CATEGORY_ERR				-30060003	/* Error in converting metadata to user's security category */
#define LIBBMDSQL_DICT_CONV_METADATA_TO_SEC_LEVEL_ERR					-30060004	/* Error in converting metadata to user's security category */
#define LIBBMDSQL_DICT_CONV_DICT_TYPE_TO_ID_ERR						-30060005	/* Error in converting dictonary type to id */
#define LIBBMDSQL_DICT_CREATE_DICT_FOR_DOC_TYPES_ERR					-30060006	/* Error in creating dictonary for document types */
#define LIBBMDSQL_DICT_CREATE_DICT_FOR_SOFTWARE_TYPES_ERR				-30060007	/* Error in creating dictonary for software types */
#define LIBBMDSQL_DICT_CREATE_DICT_FOR_METADATA_TYPES_ERR				-30060008	/* Error in creating dictonary for metadata types */
#define LIBBMDSQL_DICT_CREATE_DICT_FOR_METADATA_TYPE_NAMES_ERR				-30060009	/* Error in creating dictonary for metadata type names*/
#define LIBBMDSQL_DICT_CREATE_DICT_FOR_PKI_TYPES_ERR					-30060010	/* Error in creating dictonary for pki types */
#define LIBBMDSQL_DICT_CREATE_DICT_FOR_GROUPS_ERR					-30060011	/* Error in creating dictonary for groups types */
#define LIBBMDSQL_DICT_CREATE_DICT_FOR_ROLES_ERR					-30060012	/* Error in creating dictonary for roles types */
#define LIBBMDSQL_DICT_CREATE_DICT_FOR_SECURITY_LEVELS_ERR				-30060013	/* Error in creating dictonary for security levels */
#define LIBBMDSQL_DICT_CREATE_DICT_FOR_SECURITY_CATEGORIES_ERR				-30060014	/* Error in creating dictonary for securty categories */
#define LIBBMDSQL_DICT_CREATE_DICT_FOR_RELATION_TYPES_ERR				-30060015	/* Error in creating dictonary for relation types */
#define LIBBMDSQL_DICT_CREATE_DICT_FOR_GROUPS2_ERR					-30060016	/* Error in creating dictonary for groups2 */
#define LIBBMDSQL_DICT_CREATE_DICT_FOR_SECURITY_CATEGORIES2_ERR				-30060017	/* Error in creating dictonary for securty categories2 */
#define LIBBMDSQL_DICT_DUMP_DICT_FOR_DOC_TYPES_ERR					-30060018	/* Error in dumping dictonary for document types */
#define LIBBMDSQL_DICT_DUMP_DICT_FOR_SOFTWARE_TYPES_ERR					-30060019	/* Error in dumping dictonary for software types */
#define LIBBMDSQL_DICT_DUMP_DICT_FOR_METADATA_TYPES_ERR					-30060020	/* Error in dumping dictonary for metadata types */
#define LIBBMDSQL_DICT_DUMP_DICT_FOR_METADATA_TYPE_NAMES_ERR				-30060021	/* Error in dumping dictonary for metadata type names*/
#define LIBBMDSQL_DICT_DUMP_DICT_FOR_PKI_TYPES_ERR					-30060022	/* Error in dumping dictonary for pki types */
#define LIBBMDSQL_DICT_DUMP_DICT_FOR_GROUPS_ERR						-30060023	/* Error in dumping dictonary for groups types */
#define LIBBMDSQL_DICT_DUMP_DICT_FOR_ROLES_ERR						-30060024	/* Error in dumping dictonary for roles types */
#define LIBBMDSQL_DICT_DUMP_DICT_FOR_SECURITY_LEVELS_ERR				-30060025	/* Error in dumping dictonary for security levels */
#define LIBBMDSQL_DICT_DUMP_DICT_FOR_SECURITY_CATEGORIES_ERR				-30060026	/* Error in dumping dictonary for securty categories */
#define LIBBMDSQL_DICT_DUMP_DICT_FOR_RELATION_TYPES_ERR					-30060027	/* Error in dumping dictonary for relation types */
#define LIBBMDSQL_DICT_DUMP_DICT_FOR_GROUPS2_ERR					-30060028	/* Error in dumping dictonary for groups2 */
#define LIBBMDSQL_DICT_DUMP_DICT_FOR_SECURITY_CATEGORIES2_ERR				-30060029	/* Error in dumping dictonary for securty categories2 */
#define LIBBMDSQL_DICT_DUMP_DICT_FOR_ROLE_STRUCT_ERR					-30060030	/* Error in dumping dictonary for role struct */
#define LIBBMDSQL_DICT_DUMP_DICT_FOR_GROUPS_GRAPH_ERR					-30060031	/* Error in dumping dictonary for groups graph */
#define LIBBMDSQL_DICT_DESTROY_DICT_FOR_DOC_TYPES_ERR					-30060032	/* Error in destroying dictonary for document types */
#define LIBBMDSQL_DICT_DESTROY_DICT_FOR_SOFTWARE_TYPES_ERR				-30060033	/* Error in destroying dictonary for software types */
#define LIBBMDSQL_DICT_DESTROY_DICT_FOR_METADATA_TYPES_ERR				-30060034	/* Error in destroying dictonary for metadata types */
#define LIBBMDSQL_DICT_DESTROY_DICT_FOR_METADATA_TYPE_NAMES_ERR				-30060035	/* Error in destroying dictonary for metadata type names*/
#define LIBBMDSQL_DICT_DESTROY_DICT_FOR_PKI_TYPES_ERR					-30060036	/* Error in destroying dictonary for pki types */
#define LIBBMDSQL_DICT_DESTROY_DICT_FOR_GROUPS_ERR					-30060037	/* Error in destroying dictonary for groups types */
#define LIBBMDSQL_DICT_DESTROY_DICT_FOR_ROLES_ERR					-30060038	/* Error in destroying dictonary for roles types */
#define LIBBMDSQL_DICT_DESTROY_DICT_FOR_SECURITY_LEVELS_ERR				-30060039	/* Error in destroying dictonary for security levels */
#define LIBBMDSQL_DICT_DESTROY_DICT_FOR_SECURITY_CATEGORIES_ERR				-30060040	/* Error in destroying dictonary for securty categories */
#define LIBBMDSQL_DICT_DESTROY_DICT_FOR_RELATION_TYPES_ERR				-30060041	/* Error in destroying dictonary for relation types */
#define LIBBMDSQL_DICT_DESTROY_DICT_FOR_GROUPS2_ERR					-30060042	/* Error in destroying dictonary for groups2 */
#define LIBBMDSQL_DICT_DESTROY_DICT_FOR_SECURITY_CATEGORIES2_ERR			-30060043	/* Error in destroying dictonary for securty categories2 */
#define LIBBMDSQL_DICT_DESTROY_DICT_FOR_ROLE_STRUCT_ERR					-30060044	/* Error in destroying dictonary for role struct */
#define LIBBMDSQL_DICT_NO_ELEMENTS_IN_DICTIONARY					-30060045
#define LIBBMDSQL_DICT_UNKNOWN_SYSMETADATA						-30060046
#define LIBBMDSQL_DICT_VALUE_NOT_FOUND							-30060047
#define LIBBMDSQL_DICT_GROUP_VALUE_NOT_FOUND						-30060048
#define LIBBMDSQL_DICT_ROLE_VALUE_NOT_FOUND						-30060049
#define LIBBMDSQL_DICT_USER_VALUE_NOT_FOUND						-30060050
#define LIBBMDSQL_DICT_ACTION_VALUE_NOT_FOUND						-30060051
#define LIBBMDSQL_DICT_SEC_LEVEL_VALUE_NOT_FOUND					-30060052
#define LIBBMDSQL_DICT_SEC_CATEGORY_VALUE_NOT_FOUND					-30060053
#define LIBBMDSQL_DICT_CLASS_VALUE_NOT_FOUND						-30060054

#define LIBBMDSQL_USER_ID_UNDEFINED							-30070000
#define LIBBMDSQL_USER_ISSUER_UNDEFINED							-30070001
#define LIBBMDSQL_USER_SN_UNDEFINED							-30070002
#define LIBBMDSQL_USER_NAME_UNDEFINED							-30070003
#define LIBBMDSQL_USER_IDENTITY_UNDEFINED						-30070004
#define LIBBMDSQL_USER_ROLE_UNDEFINED							-30070005
#define LIBBMDSQL_USER_GROUP_UNDEFINED							-30070006
#define LIBBMDSQL_USER_CLASS_UNDEFINED							-30070007
#define LIBBMDSQL_USER_DEFAULT_ROLE_UNDEFINED						-30070008
#define LIBBMDSQL_USER_DEFAULT_GROUP_UNDEFINED						-30070009
#define LIBBMDSQL_USER_DEFAULT_CLASS_UNDEFINED						-30070010
#define LIBBMDSQL_USER_ISSUER_TOO_MANY							-30070011
#define LIBBMDSQL_USER_SN_TOO_MANY							-30070012
#define LIBBMDSQL_USER_NAME_TOO_MANY							-30070013
#define LIBBMDSQL_USER_IDENTITY_TOO_MANY						-30070014
#define LIBBMDSQL_USER_CERT_ALREADY_REGISTERED						-30070015
#define LIBBMDSQL_USER_ROLE_ERROR							-30070016
#define LIBBMDSQL_USER_GROUP_ERROR							-30070017
#define LIBBMDSQL_USER_CLASS_ERROR							-30070018
#define LIBBMDSQL_USER_SECURITY_ERROR							-30070019
#define LIBBMDSQL_USER_DEFAULT_GROUP_TOO_MANY						-30070020
#define LIBBMDSQL_USER_DEFAULT_ROLE_TOO_MANY						-30070021
#define LIBBMDSQL_USER_DEFAULT_CLASS_TOO_MANY						-30070022
#define LIBBMDSQL_USER_NOT_REGISTERED_IN_SYSTEM						-30070023
#define LIBBMDSQL_USER_USER_NOT_FOUND							-30070024
#define LIBBMDSQL_USER_LIST_NO_USERS							-30070025
#define LIBBMDSQL_USER_GROUP_TOO_MANY							-30070026
#define LIBBMDSQL_USER_DEFAULT_ROLE_INVALID						-30070027
#define LIBBMDSQL_USER_DEFAULT_GROUP_INVALID						-30070028
#define LIBBMDSQL_USER_ACCEPTED_UNDEFINED						-30070029
#define LIBBMDSQL_USER_FILE_OWNER							-30070030
#define LIBBMDSQL_USER_DEFAULT_ROLE_NOT_ROLE						-30070031
#define LIBBMDSQL_USER_DEFAULT_GROUP_NOT_GROUP						-30070032
#define LIBBMDSQL_USER_DEFAULT_CLASS_NOT_CLASS						-30070033
#define LIBBMDSQL_USER_NEW_GROUP_NAME_TOO_MANY						-30070034
#define LIBBMDSQL_USER_NO_CLASS_DEFINED							-30070035
#define LIBBMDSQL_USER_NO_DEFAULT_CLASS_DEFINED						-30070036
#define LIBBMDSQL_USER_TOO_MANY_REGISTERED_USERS					-30070037
#define LIBBMDSQL_USER_DEFAULT_CLASS_INVALID						-30070038
#define LIBBMDSQL_USER_IDENTITY_NOT_FOUND						-30070039
#define DISABLE_OWN_IDENTITY_FORBIDDEN							-30070040
#define DISABLE_CURRENT_CERTIFICATE_FORBIDDEN						-30070041
#define LIBBMDSQL_USER_TOO_MANY_REGISTERED_IDENTITIES					-30070042
#define LIBBMDSQL_USER_LICENSE_FORBID_REGISTER_MORE_IDENTITIES				-30070043

#define LIBBMDSQL_ROLE_USER_ACCESS_DENIED						-30070100
#define LIBBMDSQL_ROLE_NOT_REGISTERED_IN_SYSTEM						-30070101
#define LIBBMDSQL_ROLE_LIST_NO_ROLES							-30070102
#define LIBBMDSQL_ROLE_NAME_TOO_MANY							-30070103
#define LIBBMDSQL_ROLE_NAME_UNDEFINED							-30070104
#define LIBBMDSQL_ROLE_ROLE_ALREADY_REGISTERED						-30070105
#define LIBBMDSQL_ROLE_ACTION_ERROR							-30070106
#define LIBBMDSQL_ROLE_ROLE_RIGHTS_ERROR						-30070107
#define LIBBMDSQL_ROLE_ROLE_ALREADY_UPDATEED						-30070108
#define LIBBMDSQL_ROLE_CLEAR_RR_TABLE_ERR						-30070109
#define LIBBMDSQL_ROLE_CLEAR_ACTIONS_TABLE_ERR						-30070110
#define LIBBMDSQL_ROLE_CLEAR_ROLES_TABLE_ERR						-30070111
#define LIBBMDSQL_ROLE_USERS_WITH_ROLE_EXIST						-30070112
#define LIBBMDSQL_ROLE_ID_TOO_MANY							-30070113
#define LIBBMDSQL_ROLE_ID_UNDEFINED							-30070114
#define LIBBMDSQL_ROLE_NOT_OWNED							-30070115
#define LIBBMDSQL_ROLE_TOO_MANY_REGISTERED_ROLES					-30070116

#define LIBBMDSQL_GROUP_NOT_REGISTERED_IN_SYSTEM					-30070200
#define LIBBMDSQL_GROUP_NAME_TOO_MANY							-30070201
#define LIBBMDSQL_GROUP_NAME_UNDEFINED							-30070202
#define LIBBMDSQL_GROUP_DEST_GROUPS_FOUND						-30070203
#define LIBBMDSQL_GROUP_USERS_WITH_GROUP_EXIST						-30070204
#define LIBBMDSQL_GROUP_CLEAR_GROUP_GRAPH_TABLE_ERR					-30070205
#define LIBBMDSQL_GROUP_CLEAR_USERS_AND_GROUPS_TABLE_ERR				-30070206
#define LIBBMDSQL_GROUP_CLEAR_GROUPS_TABLE_ERR						-30070207
#define LIBBMDSQL_GROUP_NOT_FOUND							-30070208
#define LIBBMDSQL_GROUP_CHANGE_NAME_TOO_MANY						-30070209
#define LIBBMDSQL_GROUP_LIST_NO_GROUP							-30070210
#define LIBBMDSQL_GROUP_GROUP_ALREADY_REGISTERED					-30070211
#define LIBBMDSQL_GROUP_PARENT_GROUP_ERR						-30070212
#define LIBBMDSQL_GROUP_CHILD_GROUP_ERR							-30070213
#define LIBBMDSQL_GROUP_PARENT_GROUP_NOT_REGISTERED					-30070214
#define LIBBMDSQL_GROUP_CHILD_GROUP_NOT_REGISTERED					-30070215
#define LIBBMDSQL_GROUP_ID_TOO_MANY							-30070216
#define LIBBMDSQL_GROUP_ID_UNDEFINED							-30070217
#define LIBBMDSQL_GROUP_NOT_OWNED							-30070218
#define LIBBMDSQL_GROUP_OWNED								-30070219
#define LIBBMDSQL_GROUP_TOO_MANY_REGISTERED_GROUPS					-30070220

#define LIBBMDSQL_CLASS_NOT_REGISTERED_IN_SYSTEM					-30070300
#define LIBBMDSQL_CLASS_NAME_TOO_MANY							-30070301
#define LIBBMDSQL_CLASS_NAME_UNDEFINED							-30070302
#define LIBBMDSQL_CLASS_DEST_GROUPS_FOUND						-30070303
#define LIBBMDSQL_CLASS_USERS_WITH_CLASS_EXIST						-30070304
#define LIBBMDSQL_CLASS_CLEAR_CLASS_GRAPH_TABLE_ERR					-30070305
#define LIBBMDSQL_CLASS_CLEAR_USERS_AND_CLASSES_TABLE_ERR				-30070306
#define LIBBMDSQL_CLASS_CLEAR_GROUPS_TABLE_ERR						-30070307
#define LIBBMDSQL_CLASS_NOT_FOUND							-30070308
#define LIBBMDSQL_CLASS_CHANGE_NAME_TOO_MANY						-30070309
#define LIBBMDSQL_CLASS_LIST_NO_CLASS							-30070310
#define LIBBMDSQL_CLASS_CLASS_ALREADY_REGISTERED					-30070311
#define LIBBMDSQL_CLASS_PARENT_CLASS_ERR						-30070312
#define LIBBMDSQL_CLASS_CHILD_CLASS_ERR							-30070313
#define LIBBMDSQL_CLASS_PARENT_CLASS_NOT_REGISTERED					-30070314
#define LIBBMDSQL_CLASS_CHILD_CLASS_NOT_REGISTERED					-30070315
#define LIBBMDSQL_CLASS_ID_TOO_MANY							-30070316
#define LIBBMDSQL_CLASS_ID_UNDEFINED							-30070317
#define LIBBMDSQL_CLASS_NOT_OWNED							-30070318
#define LIBBMDSQL_CLASS_DEFAULT_NOT_SPECIFIED						-30070319
#define LIBBMDSQL_CLASS_CLEAR_CLASSES_TABLE_ERR						-30070320
#define LIBBMDSQL_CLASS_TOO_MANY_REGISTERED_CLASSES					-30070321

#define LIBBMDSQL_SEC_LEVEL_NOT_REGISTERED_IN_SYSTEM					-30070401
#define LIBBMDSQL_SEC_LEVEL_PRIORITY_NOT_REGISTERED_IN_SYSTEM				-30070402
#define LIBBMDSQL_SEC_CATEGORY_NOT_REGISTERED_IN_SYSTEM					-30070403
#define LIBBMDSQL_SEC_NAME_NOT_DEFINED							-30070404
#define LIBBMDSQL_SEC_CAT_ALREADY_REGISTERED						-30070405
#define LIBBMDSQL_SEC_CAT_PARENT_CAT_NOT_REGISTERED					-30070406
#define LIBBMDSQL_SEC_CAT_CHILD_CAT_NOT_REGISTERED					-30070407
#define LIBBMDSQL_SEC_CAT_PARENT_SEC_CAT_ERR						-30070408
#define LIBBMDSQL_SEC_CAT_CHILD_SEC_CAT_ERR						-30070409
#define LIBBMDSQL_SEC_CAT_NAME_TOO_MANY							-30070410

#define LIBBMDSQL_SEC_LEVEL_ALREADY_REGISTERED						-30070141
#define LIBBMDSQL_SEC_LEVEL_PRIORITY_ALREADY_REGISTERED					-30070142
#define LIBBMDSQL_SEC_LEVEL_NAME_TOO_MANY						-30070143
#define LIBBMDSQL_SEC_LEVEL_PRIORITY_TOO_MANY						-30070144



#define LIBBMDSQL_LINK_CREATE_NO_OWNER_CERT_ISSUER					-30080001
#define LIBBMDSQL_LINK_CREATE_NO_OWNER_CERT_SN						-30080002
#define LIBBMDSQL_LINK_CREATE_NO_OWNER_ID						-30080003
#define LIBBMDSQL_LINK_CREATE_TOO_MANY_OWNER_CERT_ISSUER				-30080004
#define LIBBMDSQL_LINK_CREATE_TOO_MANY_OWNER_CERT_SN					-30080005
#define LIBBMDSQL_LINK_CREATE_TOO_MANY_OWNER_CERT_ID					-30080006
#define LIBBMDSQL_LINK_CREATE_FORBIDDEN							-30080007
#define LIBBMDSQL_ADD_SIGNATURE_NO_SIGNATURE_IN_DTG					-30090001
#define LIBBMDSQL_ADD_SIGNATURE_NO_FILE_WITH_ID						-30090002
#define LIBBMDSQL_ADD_SIGNATURE_NO_FILE_ID_SPECIFIED					-30090003


#define LIBBMDPKI_CMS_INTERNAL_PRIVATE_KEY_DECRYPT_ERR					-40000101	/* Error in decryption private key from raw file servicing get request */

#define LIBBMDPKI_CTX_API_PUBLIC_KEY_FROM_CERT_FILE_ERR					-40020001

#define LIBBMDTS_ERR_TS_DECODE_TSR_DATA							-50010001
#define LIBBMDTS_ERR_TS_SIGNATURE_NOT_PRESENT_IN_CMS					-50010002
#define LIBBMDTS_ERR_TS_TIMESTAMP_NOT_PRESENT_IN_SIGNATURE				-50010003
#define LIBBMDTS_ERR_TS_DECODE_TSTINFO							-50010004
#define LIBBMDTS_ERR_TS_GETTING_GEN_TIME						-50010005
#define LIBBMDTS_ERR_TS_CONVERT_INTEGER_T_TO_LONG					-50010006
#define LIBBMDTS_ERR_TS_CONVERT_OBJETCT_IDENTIFIER_T_TO_STRING				-50010007
#define LIBBMDTS_ERR_TS_CONVERT_INTEGER_T_TO_STRING					-50010008
#define LIBBMDTS_ERR_TS_NO_CERTIFICATES_PRESENT_IN_SIGNED_DATA				-50010009
#define LIBBMDTS_ERR_TS_NO_x509_CERTIFICATES_PRESENT_IN_SIGNED_DATA			-50010010
#define LIBBMDTS_ERR_TS_CLONING_CERTIFICATE						-50010011
#define LIBBMDTS_ERR_TS_UNRECOGNIZED_ATTR_TYPE						-50010012

#define LIBBMDAA2_ERR_USER_LOGIN_NOT_REGISTERED						-60010001 /* User trying to log in is not registered in users data */
#define LIBBMDAA2_ERR_USER_LOGIN_NOT_ACCEPTED						-60010002 /* User trying to log in has no acceptance acording to users data */
#define LIBBMDAA2_ERR_USER_GROUP_NOT_SPECIFIED						-60010003 /* User trying to log in has no group specified */
#define LIBBMDAA2_ERR_USER_DEFAULT_GROUP_NOT_SPECIFIED					-60010004
#define LIBBMDAA2_ERR_USER_ROLE_NOT_SPECIFIED						-60010005 /* User trying to log in has no role specified */
#define LIBBMDAA2_ERR_USER_DEFAULT_ROLE_NOT_SPECIFIED					-60010006

#define BMDSERVER_ERR_USER_ROLE_UNKNOWN							-70010001 /* User trying to log in has no role specified */

#define CONSOLE_CLIENT_UPLOAD_NO_FILES_TO_UPLOAD					-80010001 /* No files found to upload */
#define CONSOLE_CLIENT_UPLOAD_OPEN_UPLOAD_DIR_ERR					-80010002 /* Error openning upload directory */
#define CONSOLE_CLIENT_DOWNLOAD_OPEN_LIST_FILE_ERR					-80010003 /* Error openning id list file */
#define CONSOLE_CLIENT_UPLOAD_ADDING_FILES_ERR						-80010004 /* Error openning id list file */

#endif
