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
/* 	Obsługa czyszczenia pamięci cache     */
/**********************************************/

void clearCache(int num, siginfo_t* info, void *other ) {

	/**************************************************/
	/* Wystąpienie zdarzenia związanego z usunięciem  */ 
	/*     szablonów z pamięci cache 	          */
	/**************************************************/

 PRINT_INFO("AWIZOSOAPSERVERINF clear cache.\n" );

 sem_wait(&(_GLOBAL_shptr->awizoActionMutex));

 if (_GLOBAL_shptr->awizoAction == queueNothing) {
	_GLOBAL_shptr->awizoAction = cacheClear;
 }

 sem_post(&(_GLOBAL_shptr->awizoActionMutex));


}

/**********************************************/
/* Obsługa dodania komunikatu do kolejki      */
/**********************************************/

void addMailToQueue(int id) { 

 /***********************************************/
 /* Wystąpienie zdarzenia związanego z dodaniem */ 
 /*     komunikatu do kolejki komunikatów       */
 /***********************************************/


 sigfillset(&_GLOBAL_mask_set);
 sigprocmask(SIG_SETMASK, &_GLOBAL_mask_set, NULL);
/*
 sem_wait(&(_GLOBAL_shptr->awizoActionMutex));

 _GLOBAL_shptr->awizoAction = queueNextMail;

 sem_post(&(_GLOBAL_shptr->awizoActionMutex));
*/

}



/**********************************************/
/*    Obsługa czyszczenia kolejki z mailami   */
/**********************************************/

void clearMailQueue(int num, siginfo_t* info, void *other ) {

 PRINT_INFO("AWIZOSOAPSERVERINF clear mail queue.\n" );

/*
 sem_wait(&(_GLOBAL_shptr->awizoActionMutex));

	if (_GLOBAL_shptr->awizoAction == queueNothing){
		_GLOBAL_shptr->awizoAction = queueClear;		
	}

 sem_post(&(_GLOBAL_shptr->awizoActionMutex));
*/

}


/*************************************************/
/*    Nadszedł czas wysyłki zawartości kolejki   */
/*************************************************/

void queueTimer( int signal ) {

 PRINT_INFO("AWIZOSOAPSERVERINF time to send awizos.\n" );

}


/*****************************************************/
/*  Obsługa wysyłki zawartości kolejki po timeoucie  */
/*****************************************************/

void sendMailQueue( int signal ) {

 /*****************************************/
 /* Wystąpienie zdarzenia związanego z    */ 
 /*   wysyłką bieżącej zawartości kolejki */
 /*****************************************/

 sem_wait(&(_GLOBAL_shptr->awizoActionMutex));

 if (_GLOBAL_shptr->awizoAction == queueNothing) {
	_GLOBAL_shptr->awizoAction = queueTime;
 }

 sem_post(&(_GLOBAL_shptr->awizoActionMutex));

}


void changeServerConfiguration( int signal ) {

   /* ----------------------------------------- */

/*
      long 	        longRet         =    0; 
      GenBuf_t* 	certFile 	= NULL;
*/

   /* ----------------------------------------- */

	PRINT_INFO("AWIZOSOAPSERVERINF change awizo configuration.\n" );

/*

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

*/

		/* **************************************************************** */
		/*       Pobieranie ceryfikatu podpisującego z bazy danych          */
		/* **************************************************************** */

/*

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
*/
}


