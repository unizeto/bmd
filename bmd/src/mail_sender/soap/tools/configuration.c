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
#include <bmd/mail_sender/bmd_conf_sections.h>
#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmddb/libbmddb.h>



/**
* Utworzenie węzła z ustawieniami serwera archiwum
*
* @param[in,out] bmdserver_tNode - uchwyt do struktury 
*                          przechowywującej parametry
*
* @param[in] name - nazwa serwera bmd
* @param[in] addr - adres serwera archiwum
* @param[in] port - port serwera archiwum
* @param[in] der  - certyfikat uzytkownika
* @param[in] pfx  - certyfikat uzytkownika 
* @param[in] pfx_pass - hasło dla certyfikatu uzytkownika
* @param[in] bmd_cert - certyfikat serwera
*
* @return status zakończenia
* @retval BMD_ERR_PARAM1 - niepoprawny pierwszy parametr
* @retval BMD_ERR_PARAM2 - niepoprawny drugi parametr
* @retval BMD_ERR_OP_FAILED - błędy związane z obsługą słownika
* @retval BMD_ERR_MEMORY - błąd alokacji pamięci
* @retval BMD_OK - zakończenie pomyślne
*
*/



long CreateBmdServerNode(bmdserver_t** const bmdserver_tNode, \
				char* const name, \
				char* const addr, \
				char* const port, \
				char* const der, \
				char* const pfx, \
				char* const pfx_pass, \
				char* const bmd_cert)
{


	if (bmdserver_tNode == NULL) {
		PRINT_ERROR("Niepoprawny pierwszy parametr. Error = %d\n", BMD_ERR_PARAM1);
		return BMD_ERR_PARAM1;
	}

/*
	if (name == NULL) {
		PRINT_ERROR("Nie podano nazwy serwera. Error = %d\n", BMD_ERR_PARAM2);
		return BMD_ERR_PARAM2;
	}
*/

	if (addr == NULL) {
		PRINT_ERROR("Nie podano adresu serwera bmd. Error = %d\n", BMD_ERR_PARAM3);
		return BMD_ERR_PARAM3;
	}

	if (port == NULL) {
		PRINT_ERROR("Nie podano portu serwera bmd. Error = %d\n", BMD_ERR_PARAM4);
		return BMD_ERR_PARAM4;
	}

/*
	if (der == NULL) {
		PRINT_ERROR("Nie podano certyfikatu uzytkownika. Error = %d\n", BMD_ERR_PARAM5);
		return BMD_ERR_PARAM5;
	}

	if (pfx == NULL) {
		PRINT_ERROR("Nie podano certyfikatu uzytkownika. Error = %d\n", BMD_ERR_PARAM6);
		return BMD_ERR_PARAM6;
	}

	if (pfx_pass == NULL) {
		PRINT_ERROR("Nie podano hasla do certyfikatu uzytkownika. Error = %d\n", BMD_ERR_PARAM7);
		return BMD_ERR_PARAM7;
	}

	if (bmd_cert == NULL) {
		PRINT_ERROR("Nie podano certyfikatu serwera. Error = %d\n", BMD_ERR_PARAM8);
		return BMD_ERR_PARAM8;
	}
*/
	*bmdserver_tNode = (bmdserver_t*)calloc(1, sizeof(bmdserver_t));

	if ( (*bmdserver_tNode) == NULL ){
		PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
		return BMD_ERR_MEMORY;
	}


	if (name != NULL) {
		asprintf( &((*bmdserver_tNode)->name),"%s", name);
	}

	if (addr != NULL) {
		asprintf( &((*bmdserver_tNode)->addr),"%s", addr);
	}

	if (port != NULL) {
		asprintf( &((*bmdserver_tNode)->port),"%s", port);
	}

	if (der != NULL) {
	    asprintf( &((*bmdserver_tNode)->der),"%s", der);
	}

	if (pfx != NULL) {
	    asprintf( &((*bmdserver_tNode)->pfx),"%s", pfx);
	}

	if (pfx_pass != NULL) {
	    asprintf( &((*bmdserver_tNode)->pfx_pass),"%s", pfx_pass);
	}

	if (bmd_cert != NULL) {
	    asprintf( &((*bmdserver_tNode)->bmd_cert),"%s", bmd_cert);
	}


   return BMD_OK;
}



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

long getAwizoConfiguration( 	bmd_conf* const bmdconfHandle, \
				const char* const confFile, \
				configuration_t* const config ) 
{


/* -------------------------------------------------------- */

	long			intRet 		 = 0;
	char*			stringSQL 	 = NULL;
	long			rows 		 = 0;
	long			cols 		 = 0;
	void*			result 		 = NULL;
	db_row_strings_t*	rowstruct 	 = NULL;
	long			fetched		 = 0;
/*	char*			pfxFile		 = NULL; */
/*	char*			pfxPin 		 = NULL; */
	char*			bmdServers 	 = NULL;
	char*			stringKomunikat  = NULL;
	/* GenBuf_t*		certFile 	 = NULL; */
	char**			bmdServersTable	 = NULL;
	int			i		 = 0;
	char*			stringAddr 	 = NULL; /* adres serwera BMD */
	char*		    	stringPort 	 = NULL; /* port serwera BMD */
	char*			stringDer	 = NULL; /* ścieżka do certyfikatu klucza publicznego klienta */
	char*			stringPfx 	 = NULL; /* ścieżka do certyfikatu klucza publicznego wraz z kluczem prywatnym klienta */
	char*			stringPfxpass	 = NULL; /* hasło certyfikatu klienta */
	char*			stringBmdcert	 = NULL; /* ścieżka do certyfikatu serwera */
	bmdserver_t*		bmdserver_tNode  = NULL;

/* --------------------------------------------------------- */


 /* ************************************* */
 /*   Zainicjowanie listy serwerów BMD    */
 /* ************************************* */

 InitQueue(&(config->bmdservers));

 intRet = bmdconf_get_value( bmdconfHandle, "soap", "db_host", &(config->dbIP));

 if ( intRet < 0 ) {
	PRINT_ERROR("Błąd odczytu adresu bazy danych z pliku konfiguracyjnego. Error = %d\n", BMD_ERR_OP_FAILED);
	return BMD_ERR_OP_FAILED;
 }

 intRet = bmdconf_get_value( bmdconfHandle, "soap", "db_port", &(config->dbPort));

 if ( intRet < 0 ) {
	PRINT_ERROR("Błąd odczytu portu bazy danych z pliku konfiguracyjnego. Error = %d\n", BMD_ERR_OP_FAILED);
	return BMD_ERR_OP_FAILED;
 }

 intRet = bmdconf_get_value( bmdconfHandle, "soap", "db_name", &(config->dbName));

 if ( intRet < 0 ) {
	PRINT_ERROR("Błąd odczytu nazwy bazy danych z pliku konfiguracyjnego. Error = %d\n", BMD_ERR_OP_FAILED);
	return BMD_ERR_OP_FAILED;
 }

 intRet = bmdconf_get_value( bmdconfHandle, "soap", "db_user", &(config->dbUser));

 if ( intRet < 0 ) {
	PRINT_ERROR("Błąd odczytu nazwy bazy danych z pliku konfiguracyjnego. Error = %d\n", BMD_ERR_OP_FAILED);
	return BMD_ERR_OP_FAILED;
 }

 intRet = bmdconf_get_value( bmdconfHandle, "soap", "db_passwd", &(config->dbPassword));

 if ( intRet < 0 ) {
	PRINT_ERROR("Błąd odczytu hasła bazy danych z pliku konfiguracyjnego. Error = %d\n", BMD_ERR_OP_FAILED);
	return BMD_ERR_OP_FAILED;
 }

 intRet = bmdconf_get_value( bmdconfHandle, "soap", "db_library", &(config->dbLibrary));

 if ( intRet < 0 ) {
	PRINT_ERROR("Błąd odczytu biblioteki obsługi bazy danych z pliku konfiguracyjnego. Error = %d\n", BMD_ERR_OP_FAILED);
	return BMD_ERR_OP_FAILED;
 }

 intRet = bmd_db_init(config->dbLibrary);

 if ( intRet != BMD_OK ) {
	PRINT_ERROR("Błąd inicjowania biblioteki obsługi bazy danych. Error = %d\n", BMD_ERR_OP_FAILED);
	return BMD_ERR_OP_FAILED;
 }

 intRet = bmdconf_get_value( bmdconfHandle, "soap", "bmd_servers", &bmdServers );

 if ( intRet < 0 ) {
        PRINT_ERROR("Błąd odczytu listy serwerów BMD z pliku konfiguracyjnego. Error = %d\n", BMD_ERR_OP_FAILED);
        return BMD_ERR_OP_FAILED;
 }

 intRet = getSubstring( bmdServers, "[a-zA-Z0-9_]+", &bmdServersTable );

 if ( intRet < 0 ) {
        PRINT_ERROR("Błąd w trakcie wyszukiwania podnapisów. Error = %d\n", BMD_ERR_OP_FAILED);
        return BMD_ERR_OP_FAILED;
       }


 /* ***************************************** */
 /*     Pobranie ustawień konfiguracyjnych    */
 /*            serwerów archiwum              */
 /* ***************************************** */


 for( i=0; bmdServersTable[i] != NULL; i++ ) {

	intRet = bmdconf_add_section2dict( bmdconfHandle, bmdServersTable[i], ws_DICT_BMD, BMDCONF_SECTION_REQ );

	if ( intRet < 0 ) {
		PRINT_ERROR("Błąd przy dodawaniu sekcji do słownika. Error = %d\n", BMD_ERR_OP_FAILED);
		return BMD_ERR_OP_FAILED;
	}

	intRet = bmdconf_load_file(confFile, bmdconfHandle, &stringKomunikat );

	if ( intRet < 0 ) {
		PRINT_ERROR("%s. Error = %d\n", stringKomunikat, BMD_ERR_OP_FAILED);
		free0( stringKomunikat );
		return BMD_ERR_OP_FAILED;
        }

        free0( stringKomunikat );

        intRet = bmdconf_get_value(bmdconfHandle, bmdServersTable[i], "adr", &stringAddr);

        if ( intRet < 0 ) {
		PRINT_ERROR("Błąd odczytu adresu serwera archiwum. Error = %d\n", BMD_ERR_OP_FAILED);
		return BMD_ERR_OP_FAILED;
	}

        intRet = bmdconf_get_value(bmdconfHandle, bmdServersTable[i], "port", &stringPort);

        if ( intRet < 0 ) {
		PRINT_ERROR("Błąd odczytu numeru portu archiwum. Error = %d\n", BMD_ERR_OP_FAILED);
		return BMD_ERR_OP_FAILED;
	}

        intRet = bmdconf_get_value(bmdconfHandle, bmdServersTable[i], "der", &stringDer);

        if ( intRet < 0 ) {
		PRINT_ERROR("Błąd odczytu certyfikatu uzytkownika. Error = %d\n", BMD_ERR_OP_FAILED);
		return BMD_ERR_OP_FAILED;
	}

        intRet = bmdconf_get_value(bmdconfHandle, bmdServersTable[i], "pfx", &stringPfx);

        if ( intRet < 0 ) {
		PRINT_ERROR("Błąd odczytu certyfikatu uzytkownika. Error = %d\n", BMD_ERR_OP_FAILED);
		return BMD_ERR_OP_FAILED;
	}

        intRet = bmdconf_get_value(bmdconfHandle, bmdServersTable[i], "pfx_pass", &stringPfxpass);

        if ( intRet < 0 ) {
		PRINT_ERROR("Błąd odczytu hasla certyfikatu uzytkownika. Error = %d\n", BMD_ERR_OP_FAILED);
		return BMD_ERR_OP_FAILED;
	}

        intRet = bmdconf_get_value(bmdconfHandle, bmdServersTable[i], "bmd_cert", &stringBmdcert);

        if ( intRet < 0 ) {
		PRINT_ERROR("Błąd odczytu certyfikatu serwera. Error = %d\n", BMD_ERR_OP_FAILED);
		return BMD_ERR_OP_FAILED;
	}

	/* ********************************************************* */
	/*      Utworzenie wezła do przechowywania konfiguracji      */
 	/* ********************************************************* */

	intRet = CreateBmdServerNode( &bmdserver_tNode,\
					bmdServersTable[i],\
					stringAddr,\
					stringPort,\
					stringDer,\
					stringPfx,\
					stringPfxpass,\
					stringBmdcert);

	if ( intRet < 0 ) {
		PRINT_ERROR("Błąd tworzenia wezła z konfiguracją bmd : %s. Error = %d\n", bmdServersTable[i], BMD_ERR_OP_FAILED);
		return BMD_ERR_OP_FAILED;
	}

	/* ********************************************************* */
	/*             Wstawianie elementu do kolejki                */
	/* ********************************************************* */

	intRet = InsertToQueue(&(config->bmdservers), bmdserver_tNode);

	if ( intRet < 0 ) {
		PRINT_ERROR("Błąd wstawiania wezła z konfiguracja bmd : %s do kolejki. Error = %d\n", bmdServersTable[i], BMD_ERR_OP_FAILED);
		return BMD_ERR_OP_FAILED;
	}

	free0(stringAddr);
	free0(stringPort);
	free0(stringDer);
	free0(stringPfx);
	free0(stringPfxpass);
	free0(stringBmdcert);
	free0(bmdServersTable[i]);

 }

 free0(bmdServersTable);
 free0(bmdServers);

 
/* **************************************** */
/*    Pobranie ustawień konfiguracyjnych    */ 
/*             z bazy danych.               */
/* **************************************** */

 intRet = bmd_db_connect2( config->dbIP, config->dbPort, config->dbName, config->dbUser, config->dbPassword, &(config->dbase_handler));

 if ( intRet < 0 ){
	PRINT_ERROR("Błąd połączenia z bazą danych. Error = %d\n", BMD_ERR_OP_FAILED);
	return BMD_ERR_OP_FAILED;
 }

 if ( asprintf( &stringSQL, "SELECT * FROM config WHERE id in (SELECT MAX(id) FROM config)" ) < 0) {
	PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
	bmd_db_disconnect(&(config->dbase_handler));
	return BMD_ERR_MEMORY;
 }

 if ( bmd_db_execute_sql( config->dbase_handler, stringSQL, &rows, &cols, &result) < 0 ) {
	PRINT_ERROR("Błąd zapytania do bazy danych. Error = %d\n", BMD_ERR_OP_FAILED);
	free0(stringSQL);
	bmd_db_disconnect(&(config->dbase_handler));
	return BMD_ERR_OP_FAILED;
 } 

 free0(stringSQL);

 intRet = bmd_db_result_get_row( config->dbase_handler, result, 0, 0, &rowstruct, FETCH_NEXT, &fetched );

 if ( intRet < 0 ) {
	PRINT_ERROR("Błąd pobierania danych z bazy. Error = %d\n", BMD_ERR_OP_FAILED);
	bmd_db_result_free( &result );
	bmd_db_disconnect(&(config->dbase_handler));
 	return BMD_ERR_OP_FAILED;
 }

/* *************************************************************** */
/*      Zapisywanie w strukturze ustawień konfiguracyjnych         */
/* *************************************************************** */


/* *************************************************************** */
/*      Pobranie ustawień podstawowych dla kolejki mailingu        */
/* *************************************************************** */

 (_GLOBAL_shptr->config).mailqueue = atol(rowstruct->colvals[1]); 
 (_GLOBAL_shptr->config).maxcachesize = atol(rowstruct->colvals[2]);
 (_GLOBAL_shptr->config).maxmailqueue = atol(rowstruct->colvals[3]);
 (_GLOBAL_shptr->config).sendingtime = atoi(rowstruct->colvals[4]);

 bmd_db_row_struct_free( &rowstruct );
 bmd_db_result_free( &result );
 bmd_db_disconnect(&(config->dbase_handler));


/* **************************************************************** */
/*         Inicjowanie tabeli na przechowywane maile                */
/* **************************************************************** */
/*
 config->awizoTable = (mail_send_info_t*)calloc((_GLOBAL_shptr->config).maxmailqueue, sizeof(mail_send_info_t));

 if (config->awizoTable == NULL) {
	PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
	return BMD_ERR_MEMORY;
 }
*/

/* **************************************************************** */
/*       Pobieranie ceryfikatu podpisującego z bazy danych          */
/* **************************************************************** */


 return BMD_OK;

}


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



long getAwizoBrokerConfiguration(bmd_conf* const bmdconfHandle, \
				const char* const confFile, \
				configuration_t* const config ) 
{


/* --------------------------------------------- */

   long			intRet 		 =    0;
   long			longRet		 =    0;
   char*		stringSQL 	 = NULL;
   long			rows 		 =    0;
   long			cols 		 =    0;
   void*		result 		 = NULL;
   db_row_strings_t*	rowstruct 	 = NULL;
   long			fetched		 =    0;
   bmdserver_t*		bmdserver_tNode  = NULL;

/* --------------------------------------------- */


 /* **************************************** */
 /*   Zainicjowanie listy serwerów e-Awizo   */
 /* **************************************** */

 InitQueue(&(config->bmdservers));

 intRet = bmdconf_get_value( bmdconfHandle, "soap", "db_host", &(config->dbIP));

 if ( intRet < 0 ) {
	PRINT_ERROR("Błąd odczytu adresu bazy danych z pliku konfiguracyjnego. Error = %d\n", BMD_ERR_OP_FAILED);
	return BMD_ERR_OP_FAILED;
 }

 intRet = bmdconf_get_value( bmdconfHandle, "soap", "db_port", &(config->dbPort));

 if ( intRet < 0 ) {
	PRINT_ERROR("Błąd odczytu portu bazy danych z pliku konfiguracyjnego. Error = %d\n", BMD_ERR_OP_FAILED);
	return BMD_ERR_OP_FAILED;
 }

 intRet = bmdconf_get_value( bmdconfHandle, "soap", "db_name", &(config->dbName));

 if ( intRet < 0 ) {
	PRINT_ERROR("Błąd odczytu nazwy bazy danych z pliku konfiguracyjnego. Error = %d\n", BMD_ERR_OP_FAILED);
	return BMD_ERR_OP_FAILED;
 }

 intRet = bmdconf_get_value( bmdconfHandle, "soap", "db_user", &(config->dbUser));

 if ( intRet < 0 ) {
	PRINT_ERROR("Błąd odczytu nazwy bazy danych z pliku konfiguracyjnego. Error = %d\n", BMD_ERR_OP_FAILED);
	return BMD_ERR_OP_FAILED;
 }

 intRet = bmdconf_get_value( bmdconfHandle, "soap", "db_passwd", &(config->dbPassword));

 if ( intRet < 0 ) {
	PRINT_ERROR("Błąd odczytu hasła bazy danych z pliku konfiguracyjnego. Error = %d\n", BMD_ERR_OP_FAILED);
	return BMD_ERR_OP_FAILED;
 }

 intRet = bmdconf_get_value( bmdconfHandle, "soap", "db_library", &(config->dbLibrary));

 if ( intRet < 0 ) {
	PRINT_ERROR("Błąd odczytu biblioteki obsługi bazy danych z pliku konfiguracyjnego. Error = %d\n", BMD_ERR_OP_FAILED);
	return BMD_ERR_OP_FAILED;
 }


 intRet = bmd_db_init(config->dbLibrary);

 if ( intRet != BMD_OK ) {
	PRINT_ERROR("Błąd inicjowania biblioteki obsługi bazy danych. Error = %d\n", BMD_ERR_OP_FAILED);
	return BMD_ERR_OP_FAILED;
 }

/* **************************************** */
/*    Pobranie ustawień konfiguracyjnych    */ 
/*             z bazy danych.               */
/* **************************************** */

 if ( asprintf( &stringSQL, "SELECT host,port FROM awizo_servers;" ) < 0) {
	PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
	return BMD_ERR_MEMORY;
 }

 intRet = bmd_db_connect2( config->dbIP, config->dbPort, config->dbName, config->dbUser, config->dbPassword, &(config->dbase_handler));

 if ( intRet < 0 ){
	PRINT_ERROR("Błąd połączenia z bazą danych. Error = %d\n", BMD_ERR_OP_FAILED);
	free0(stringSQL);
	return BMD_ERR_OP_FAILED;
 }


 if ( bmd_db_execute_sql( config->dbase_handler, stringSQL, &rows, &cols, &result) < 0 ) {
	PRINT_ERROR("Błąd zapytania do bazy danych. Error = %d\n", BMD_ERR_OP_FAILED);
	free0(stringSQL);
	bmd_db_disconnect(&(config->dbase_handler));
	return BMD_ERR_OP_FAILED;
 } 

 free0(stringSQL);


 do {

	intRet = bmd_db_result_get_row( config->dbase_handler, result, 0, 0, &rowstruct, FETCH_NEXT, &fetched );


	if (intRet == 0) {

	/* ********************************************************* */
	/*    Utworzenie węzła przechowywującego serwery e-Awiza     */
	/* ********************************************************* */

	longRet = CreateBmdServerNode( &bmdserver_tNode,\
				NULL, \
				rowstruct->colvals[0], \
				rowstruct->colvals[1], \
				NULL, \
				NULL, \
				NULL, \
				NULL);

	if ( longRet < 0 ) {
		PRINT_ERROR("Błąd tworzenia wezła z lokalizacją serwera e-Awizo. Error = %d\n", BMD_ERR_OP_FAILED);
 		bmd_db_row_struct_free( &rowstruct );
		bmd_db_result_free( &result );
 		bmd_db_disconnect(&(config->dbase_handler));
		return BMD_ERR_OP_FAILED;
	}

	/* *********************************************** */
	/*   Dodanie węzła z serwerem e-Awiza do kolejki   */
	/* *********************************************** */

	longRet = InsertToQueue(&(config->bmdservers), bmdserver_tNode);

	if ( longRet < 0 ) {
		PRINT_ERROR("Błąd wstawiania wezła z lokalizacją serwera e-Awizo do kolejki. Error = %d\n", BMD_ERR_OP_FAILED);
 		bmd_db_row_struct_free( &rowstruct );
		bmd_db_result_free( &result );
 		bmd_db_disconnect(&(config->dbase_handler));
		return BMD_ERR_OP_FAILED;
	}

	bmd_db_row_struct_free( &rowstruct );

	}

 }while(intRet == 0);

 bmd_db_row_struct_free( &rowstruct );
 bmd_db_result_free( &result );
 bmd_db_disconnect(&(config->dbase_handler));

 return BMD_OK;

}

