/*****************************************
 *                                       *
 * Plik zawiera funkcje obsługi sygnałów *
 * w serwerach soap dla eAwiza           *
 *                                       *
 * Data : 29-09-2009                     *
 * Autor : Tomasz Klimek                 *
 *                                       *
 *****************************************/

#include <string.h>
#include <semaphore.h>
#include <string.h>
#include <bmd/mail_sender/cache.h>
#include <bmd/mail_sender/shmem.h>
#include <bmd/mail_sender/mail_sender.h>
#include <bmd/libbmdglobals/libbmdglobals.h>


/**********************************************/
/* 	Obsługa dodania nowego szablonu       */
/**********************************************/

void addTemplate(int num, siginfo_t* info, void *other ) {

  /* ------------------------- */

      char* templateId = NULL;
      long  longRet    =    0;

  /* ------------------------- */

   asprintf(&templateId, "%d", (info->si_value).sival_int); 

   PRINT_INFO("AWIZOJMSSERVERINF ADD NEW TEMPLATE TO CACHE\n"); 
   PRINT_INFO("TEMPLATE ID : %d.\n", (info->si_value).sival_int );

   /* ********************************************************* */
   /*   Odczyt szablonu z bazy i dodanie go do pamięci cache.   */
   /* ********************************************************* */

   longRet = getDataFromDB( templateId, _GLOBAL_awizoConfig.dbase_handler, NULL );

   if (longRet != BMD_OK){
	PRINT_ERROR("Błędy w trakcie operacji zarządzania pamięcią cache. Error = %d\n", BMD_ERR_OP_FAILED);
   }

   sem_post(&(_GLOBAL_shptr->clientMutex));
   free0(templateId);

}

/**********************************************/
/* 	Obsługa czyszczenia pamięci cache     */
/**********************************************/

void clearCache(int num, siginfo_t* info, void *other ) {

 PRINT_INFO("AWIZOJMSSERVERINF CURRENT CACHE SIZE : %ld.\n", _GLOBAL_myCache.ulCount );
 PRINT_INFO("AWIZOJMSSERVERINF CLEAR CACHE\n");

 if (DestroyQueue(&_GLOBAL_myCache, DestroyCacheNode) != BMD_OK) {
	PRINT_ERROR("Błędy w trakcie czyszczenia pamięci cache. Error = %d\n", BMD_ERR_OP_FAILED);
 }

 PRINT_INFO("AWIZOJMSSERVERINF CURRENT CACHE SIZE : %ld.\n", _GLOBAL_myCache.ulCount );

 sem_post(&(_GLOBAL_shptr->clientMutex));

}

/**********************************************/
/* Obsługa dodania komunikatu do kolejki      */
/**********************************************/

void addMailToQueue(int num, siginfo_t* info, void *other ) {

 PRINT_INFO("AWIZOJMSSERVERINF ADD MESSAGE TO MSGQUEUE\n"); 
 
 /* ----------------------------- */

    message_t*  shMessage = NULL;

 /* ----------------------------- */

 shMessage = (message_t*)calloc(1, sizeof(message_t));

 if (shMessage == NULL) {
	PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
 }

 memcpy(shMessage, &(_GLOBAL_shptr->buf), sizeof(message_t));

 InsertToQueue(&_GLOBAL_myXmlQueue, shMessage);

 PRINT_INFO("AWIZOJMSSERVERINF CURRENT MAILQUEUE SIZE : %ld.\n", _GLOBAL_myXmlQueue.ulCount );

/* ********************************************************************* */
/* Sprawdzenie ilości elementów w kolejce i uruchomienie procesu wysyłki */
/* ********************************************************************* */

 sem_post(&(_GLOBAL_shptr->clientMutex));



}

/**********************************************/
/*    Obsługa czyszczenia kolejki z mailami   */
/**********************************************/

void clearMailQueue(int num, siginfo_t* info, void *other ) {

 PRINT_INFO("AWIZOJMSSERVERINF CURRENT MAILQUEUE SIZE : %ld.\n", _GLOBAL_myXmlQueue.ulCount );
 PRINT_INFO("AWIZOJMSSERVERINF CLEAR MAILQUEUE\n");

 
   if (DestroyQueue(&_GLOBAL_myXmlQueue, DestroyMsgNode) != BMD_OK) {
	PRINT_ERROR("Błędy w trakcie czyszczenia kolejki komunikatów. Error = %d\n", BMD_ERR_OP_FAILED);
   }

 PRINT_INFO("AWIZOJMSSERVERINF CURRENT MAILQUEUE SIZE : %ld.\n", _GLOBAL_myXmlQueue.ulCount );

 sem_post(&(_GLOBAL_shptr->clientMutex));

}


/*************************************************/
/*    Nadszedł czas wysyłki zawartości kolejki   */
/*************************************************/

void queueTimer( int signal ) {

 PRINT_INFO("AWIZOJMSSERVERINF TIME TO SEND AWIZOS.\n" );

}


/*****************************************************/
/*  Obsługa wysyłki zawartości kolejki po timeoucie  */
/*****************************************************/

void sendMailQueue( int signal ) {

   /* ----------------------------------------- */

      void*             dbase_handler   = NULL;
      long 	        longRet         =    0;
      char*             response        = NULL;

   /* ----------------------------------------- */

 if ((_GLOBAL_shptr->config).stopSending == 0) {

      sem_wait(&(_GLOBAL_shptr->clientMutex)); 

      longRet = bmd_db_connect2(_GLOBAL_awizoConfig.dbIP, _GLOBAL_awizoConfig.dbPort, _GLOBAL_awizoConfig.dbName, \
			   _GLOBAL_awizoConfig.dbUser, _GLOBAL_awizoConfig.dbPassword, &dbase_handler);
 
       if ( longRet < 0 ){
		PRINT_ERROR("Błąd połączenia z bazą danych. Error = %d\n", BMD_ERR_OP_FAILED);
       }

	 /* **************************************************** */
 	 /* uruchomienie procesu wysyłki e-Awiza w jednej sesji  */
 	 /* **************************************************** */

	PRINT_INFO("AWIZOJMSSERVERINF SENT %ld AWIZOS FROM MAILQUEUE.\n", _GLOBAL_myXmlQueue.ulCount );

    	longRet = prepareAndSendAwizoSet( dbase_handler, &response );

    	if (longRet != BMD_OK) {
		PRINT_ERROR("Błąd wysyłki zakolejkowanych wiadomości email. Error = %d\n", BMD_ERR_OP_FAILED);
    	}

        bmd_db_disconnect(&dbase_handler);

  	/* ************************************* */
  	/*   Wyczyszczenie kolejki wiadomości    */
  	/* ************************************* */

	PRINT_INFO("AWIZOJMSSERVERINF CURRENT MAILQUEUE SIZE : %ld.\n", _GLOBAL_myXmlQueue.ulCount );
 	PRINT_INFO("AWIZOJMSSERVERINF CLEAR MAILQUEUE\n");

 
   	if (DestroyQueue(&_GLOBAL_myXmlQueue, DestroyMsgNode) != BMD_OK) {
		PRINT_ERROR("Błędy w trakcie czyszczenia kolejki komunikatów. Error = %d\n", BMD_ERR_OP_FAILED);
   	}

 	PRINT_INFO("AWIZOJMSSERVERINF CURRENT MAILQUEUE SIZE : %ld.\n", _GLOBAL_myXmlQueue.ulCount );

 	sem_post(&(_GLOBAL_shptr->clientMutex));
}
else{
   PRINT_INFO("AWIZOJMSSERVERINF USER STOPED AWIZO SENDING PROCESS.\n" );
}


}




/*****************************************************/
/*  Obsługa wysyłki zawartości kolejki po timeoucie  */
/*****************************************************/

void changeServerConfiguration( int signal ) {

   /* ----------------------------------------- */

      long 	        longRet         =    0; 
      GenBuf_t* 	certFile 	= NULL;

   /* ----------------------------------------- */

	PRINT_INFO("AWIZOJMSSERVERINF CHANGE AWIZO CONFIGURATION.\n" );

	if (strlen((_GLOBAL_shptr->config).smtpAddr) > 0) {
		free0(_GLOBAL_awizoConfig.smtpAddr);
		asprintf( &(_GLOBAL_awizoConfig.smtpAddr), "%s", (_GLOBAL_shptr->config).smtpAddr);
	}

	if (strlen((_GLOBAL_shptr->config).smtpPort) > 0) {
		free0(_GLOBAL_awizoConfig.smtpPort);
		asprintf( &(_GLOBAL_awizoConfig.smtpPort), "%s", (_GLOBAL_shptr->config).smtpPort);
	}

	if (strlen((_GLOBAL_shptr->config).smtpUser) > 0) {
		free0(_GLOBAL_awizoConfig.user);
		asprintf( &(_GLOBAL_awizoConfig.user), "%s", (_GLOBAL_shptr->config).smtpUser);
	}

	if (strlen((_GLOBAL_shptr->config).smtpPswd) > 0) {
		free0(_GLOBAL_awizoConfig.password);
		asprintf( &(_GLOBAL_awizoConfig.password), "%s", (_GLOBAL_shptr->config).smtpPswd);
	}

	if ( strlen((_GLOBAL_shptr->config).awizoSuccess ) > 0) {
		free0(_GLOBAL_awizoConfig.awizoSuccess);
		asprintf( &(_GLOBAL_awizoConfig.awizoSuccess), "%s", (_GLOBAL_shptr->config).awizoSuccess);
	}

	if ( strlen((_GLOBAL_shptr->config).awizoFailed ) > 0) {
		free0(_GLOBAL_awizoConfig.awizoFailed);
		asprintf( &(_GLOBAL_awizoConfig.awizoFailed), "%s", (_GLOBAL_shptr->config).awizoFailed);
	}

	if ( strlen((_GLOBAL_shptr->config).awizoFailed ) > 0) {
		free0(_GLOBAL_awizoConfig.awizoFailed);
		asprintf( &(_GLOBAL_awizoConfig.awizoFailed), "%s", (_GLOBAL_shptr->config).awizoFailed);
	}

	if ( strlen((_GLOBAL_shptr->config).csvDir ) > 0) {
		free0(_GLOBAL_awizoConfig.csvDir);
		asprintf( &(_GLOBAL_awizoConfig.csvDir), "%s", (_GLOBAL_shptr->config).csvDir);
	}

	if ( strlen((_GLOBAL_shptr->config).csvSeparator ) > 0) {
		free0(_GLOBAL_awizoConfig.csvSeparator);
		asprintf( &(_GLOBAL_awizoConfig.csvSeparator), "%s", (_GLOBAL_shptr->config).csvSeparator);
	}

	if ((_GLOBAL_shptr->config).maxImageSize > -1) {
		_GLOBAL_awizoConfig.maxImageSize = (_GLOBAL_shptr->config).maxImageSize; 
	}

	if ((_GLOBAL_shptr->config).allowImageAbsent > -1) {
		_GLOBAL_awizoConfig.allowImageAbsent = (_GLOBAL_shptr->config).allowImageAbsent;
	}

	if ((_GLOBAL_shptr->config).awizoAction > -1) {
		_GLOBAL_awizoConfig.awizoAction = (_GLOBAL_shptr->config).awizoAction;
	}

	if ((_GLOBAL_shptr->config).mailqueue > -1) {
		_GLOBAL_awizoConfig.mailqueue = (_GLOBAL_shptr->config).mailqueue;
	}

	if ((_GLOBAL_shptr->config).maxcachesize > -1) {
		_GLOBAL_awizoConfig.maxcachesize = (_GLOBAL_shptr->config).maxcachesize;
	}

	if ((_GLOBAL_shptr->config).maxmailqueue > -1) {
		_GLOBAL_awizoConfig.maxmailqueue = (_GLOBAL_shptr->config).maxmailqueue;
	}

	if (strlen((_GLOBAL_shptr->config).pfxfile) > 0 && strlen((_GLOBAL_shptr->config).pfxPin) > 0) {
		bmd_ctx_destroy(&(_GLOBAL_awizoConfig.ctx));

		/* **************************************************************** */
		/*       Pobieranie ceryfikatu podpisującego z bazy danych          */
		/* **************************************************************** */

 		longRet = bmd_db_import_blob(_GLOBAL_awizoConfig.dbase_handler, \
						(_GLOBAL_shptr->config).pfxfile, &certFile);
 
		if (longRet != BMD_OK) {
			PRINT_ERROR("Błąd pobierania certyfikatu do podpisu z bazy danych. Error = %d\n", BMD_ERR_OP_FAILED);
 		}

		longRet = bmd_set_ctx_fileInMem( certFile, (_GLOBAL_shptr->config).pfxPin, \
						 strlen((_GLOBAL_shptr->config).pfxPin), \
						 &(_GLOBAL_awizoConfig.ctx) );

 		if (longRet != BMD_OK) {
			PRINT_ERROR("Błąd w trakcie ustawiania kontekstu do podpisywania wiadomości. Error = %d\n", BMD_ERR_OP_FAILED);
		}
	}

 	sem_post(&(_GLOBAL_shptr->clientMutex));

}


