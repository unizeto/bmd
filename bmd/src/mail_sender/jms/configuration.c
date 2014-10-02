/*****************************************
 *  Ustawienia konfiguracyjne            *
 *                                       *
 * Data : 26-08-2009                     *
 * Autor : Tomasz Klimek                 *
 *                                       *
 *****************************************/

#include <stdio.h>
#include <stdlib.h>
#include <bmd/libbmdsql/common.h>
#include <bmd/libbmdpki/libbmdpki.h>
#include <bmd/libbmdutils/libbmdutils.h>
#include <bmd/libbmdconfig/libbmdconfig.h>
#include <bmd/mail_sender/configuration.h>
#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmddb/libbmddb.h>



/**
* Funkcja pobiera parametry z pliku konfiguracyjnego
*
* @param[in] confFile - ścieżka do pliku konfiguracyjnego
* @param[in,out] config - uchwyt do struktury 
                               przechowywującej parametry
*
* @return status zakończenia
* @retval BMD_ERR_PARAM1 - niepoprawny pierwszy parametr
* @retval BMD_ERR_PARAM2 - niepoprawny drugi parametr
* @retval BMD_ERR_OP_FAILED - błędy związane z obsługą słownika
* @retval BMD_ERR_MEMORY - błąd alokacji pamięci
* @retval BMD_OK - zakończenie pomyślne
*
*/

long getAwizoConfiguration( bmd_conf* const bmdconfHandle, configuration_t* const config ) {


/* ------------------------------------------------- */

	long  intRet = 0;
	char*  stringSQL = NULL;
	long  rows = 0;
	long  cols = 0;
	void* result = NULL;
	db_row_strings_t*  rowstruct = NULL;
	long  fetched = 0;
	char* pfxFile = NULL;
	char* pfxPin = NULL;
	GenBuf_t* certFile = NULL;

/* -------------------------------------------------- */


 intRet = bmdconf_get_value( bmdconfHandle, "jms", "db_host", &(config->dbIP));

 if ( intRet < 0 ) {
	PRINT_ERROR("Błąd odczytu adresu bazy danych z pliku konfiguracyjnego. Error = %d\n", BMD_ERR_OP_FAILED);
	return BMD_ERR_OP_FAILED;
 }

 intRet = bmdconf_get_value( bmdconfHandle, "jms", "db_port", &(config->dbPort));

 if ( intRet < 0 ) {
	PRINT_ERROR("Błąd odczytu portu bazy danych z pliku konfiguracyjnego. Error = %d\n", BMD_ERR_OP_FAILED);
	return BMD_ERR_OP_FAILED;
 }

 intRet = bmdconf_get_value( bmdconfHandle, "jms", "db_name", &(config->dbName));

 if ( intRet < 0 ) {
	PRINT_ERROR("Błąd odczytu nazwy bazy danych z pliku konfiguracyjnego. Error = %d\n", BMD_ERR_OP_FAILED);
	return BMD_ERR_OP_FAILED;
 }

 intRet = bmdconf_get_value( bmdconfHandle, "jms", "db_user", &(config->dbUser));

 if ( intRet < 0 ) {
	PRINT_ERROR("Błąd odczytu nazwy bazy danych z pliku konfiguracyjnego. Error = %d\n", BMD_ERR_OP_FAILED);
	return BMD_ERR_OP_FAILED;
 }

 intRet = bmdconf_get_value( bmdconfHandle, "jms", "db_passwd", &(config->dbPassword));

 if ( intRet < 0 ) {
	PRINT_ERROR("Błąd odczytu hasła bazy danych z pliku konfiguracyjnego. Error = %d\n", BMD_ERR_OP_FAILED);
	return BMD_ERR_OP_FAILED;
 }

 intRet = bmdconf_get_value( bmdconfHandle, "jms", "db_library", &(config->dbLibrary));

 if ( intRet < 0 ) {
	PRINT_ERROR("Błąd odczytu biblioteki obsługi bazy danych z pliku konfiguracyjnego. Error = %d\n", BMD_ERR_OP_FAILED);
	return BMD_ERR_OP_FAILED;
 }

 intRet = bmd_db_init(config->dbLibrary);

 if ( intRet != BMD_OK ) {
	PRINT_ERROR("Błąd inicjowania biblioteki obsługi bazy danych. Error = %d\n", BMD_ERR_OP_FAILED);
	return BMD_ERR_OP_FAILED;
 }

 intRet = bmd_db_connect2( config->dbIP, config->dbPort, config->dbName, config->dbUser, config->dbPassword, &(config->dbase_handler));

 if ( intRet < 0 ){
	PRINT_ERROR("Błąd połączenia z bazą danych. Error = %d\n", BMD_ERR_OP_FAILED);
	return BMD_ERR_OP_FAILED;
 }

/* Rozważyć możliwość pobierania różnych rekordów dotyczących konfiguracji */

 if ( asprintf( &stringSQL, "SELECT * FROM config WHERE id = 1;" ) < 0) {
	PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
	return BMD_ERR_MEMORY;
 }

 if ( bmd_db_execute_sql( config->dbase_handler, stringSQL, &rows, &cols, &result) < 0 ) {
	PRINT_ERROR("Błąd zapytania do bazy danych. Error = %d\n", BMD_ERR_OP_FAILED);
	return BMD_ERR_OP_FAILED;
 } 

 intRet = bmd_db_result_get_row( config->dbase_handler, result, 0, 0, &rowstruct, FETCH_NEXT, &fetched );

 if ( intRet < 0 ) {
	PRINT_ERROR("Błąd pobierania danych z bazy. Error = %d\n", BMD_ERR_OP_FAILED);
	return BMD_ERR_OP_FAILED;
 }

/* *************************************************************** */
/*      Zapisywanie w strukturze ustawień konfiguracyjnych         */
/* *************************************************************** */

 asprintf( &(config->smtpAddr), "%s", rowstruct->colvals[1]);
 asprintf( &(config->smtpPort), "%s", rowstruct->colvals[2]);
 asprintf( &(config->user), "%s", rowstruct->colvals[3]);
 asprintf( &(config->password), "%s", rowstruct->colvals[4]);
 asprintf( &pfxFile, "%s", rowstruct->colvals[5]);
 asprintf( &pfxPin, "%s", rowstruct->colvals[6]);
 asprintf( &(config->awizoSuccess), "%s", rowstruct->colvals[7]);
 asprintf( &(config->awizoFailed), "%s", rowstruct->colvals[8]);
 
 //asprintf( &(config->allowImageAbsent), "%s", rowstruct->colvals[9]);

 asprintf( &(config->csvDir), "%s", rowstruct->colvals[11]);
 asprintf( &(config->csvSeparator), "%s", rowstruct->colvals[12]);

 config->allowImageAbsent = atol(rowstruct->colvals[9]);
 config->maxImageSize = atol(rowstruct->colvals[10]);
 config->awizoAction = atol(rowstruct->colvals[13]);
 config->mailqueue = atol(rowstruct->colvals[14]);
 config->maxcachesize = atol(rowstruct->colvals[15]);
 config->maxmailqueue = atol(rowstruct->colvals[16]);
 //config->sendingtime = atol(rowstruct->colvals[17]);
 (_GLOBAL_shptr->config).sendingtime = atoi(rowstruct->colvals[17]);


 bmd_db_row_struct_free( &rowstruct );
 bmd_db_result_free( &result );

/* **************************************************************** */
/*       Pobieranie ceryfikatu podpisującego z bazy danych          */
/* **************************************************************** */

 intRet = bmd_db_import_blob( config->dbase_handler, pfxFile, &certFile);
 free0(pfxFile);

 if (intRet != BMD_OK) {
	PRINT_ERROR("Błąd pobierania certyfikatu do podpisu z bazy danych. Error = %d\n", BMD_ERR_OP_FAILED);
	return BMD_ERR_OP_FAILED;
 }

 intRet = bmd_set_ctx_fileInMem( certFile, pfxPin ,strlen(pfxPin), &(config->ctx));
 free0(pfxPin);

 if (intRet != BMD_OK) {
	PRINT_ERROR("Błąd w trakcie ustawiania kontekstu do podpisywania wiadomości. Error = %d\n", BMD_ERR_OP_FAILED);
	return BMD_ERR_OP_FAILED;
 }

/* **************************************************************** */
/*         Inicjowanie tabeli na przechowywane maile                */
/* **************************************************************** */


 config->awizoTable = (mail_send_info_t*)calloc( config->maxmailqueue, sizeof(mail_send_info_t));

 if (config->awizoTable == NULL) {
	PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
	return BMD_ERR_MEMORY;
 }


 return BMD_OK;

}

