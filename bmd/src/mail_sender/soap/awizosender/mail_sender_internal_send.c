#include <semaphore.h>
#include <bmd/mail_sender/queue.h>
#include <bmd/mail_sender/shmem.h>
#include <bmd/mail_sender/cache.h>
#include <bmd/mail_sender/attachment.h>
#include <bmd/mail_sender/mail_sender.h>
#include <bmd/libbmdglobals/libbmdglobals.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>
#include "soapH.h"

/* ****************************************** */
/*  Rozłączamy po każdym błędzie i na końcu   */
/* ****************************************** */
void DisconnectDB()
{
	long status = bmd_db_disconnect(&(_GLOBAL_awizoConfig.dbase_handler));
	if ( status < 0 )
		PRINT_ERROR("Błąd rozłączenia z bazą danych. Error = %d\n", BMD_ERR_OP_FAILED);
	return;
}



int ns__awizoSend(struct soap* soap, struct ns__awizoMessage *message, char** MsgID)
{

/* -------------------------------------------- */

   long			longRet         =    0;
   long 		smsSendStatus   =    0;
   long 		emailSendStatus =    0;
   //char*		templateId      = NULL;
   void*		template        = NULL;
   char*		response        = NULL;
   mail_send_info_t*	mail_info       = NULL;
   void*                dbase_handler   = NULL;
   int                  sval            =    0;
   union sigval		value;
   GenBuf_t		plik;
   xmlDocPtr		hMessage;
   awizo_mode_t		mode = modeEMAIL;	/* domyslnie wysylka emaili, nie smsow */

/* --------------------------------------------- */

  PRINT_INFO("\n");

  sem_getvalue(&(_GLOBAL_shptr->clientMutex), &sval);

  if (sval <= 0){
	return soap_receiver_fault(soap, "Awizo server already in use!", "Try another awizo servers");
  }
  
  sigfillset(&_GLOBAL_mask_set);
  sigprocmask(SIG_SETMASK, &_GLOBAL_mask_set, NULL);

  sem_wait(&(_GLOBAL_shptr->clientMutex));
  sem_wait(&(_GLOBAL_shptr->awizoActionMutex));

   _GLOBAL_shptr->awizoAction = queueNextMail;

  sem_post(&(_GLOBAL_shptr->awizoActionMutex));


  plik.buf = (char*)malloc(message->value->__size+2);
  memset(plik.buf,0,message->value->__size+1);
  memcpy(plik.buf,message->value->__ptr,message->value->__size);
  plik.size = message->value->__size;
 
/* ****************************************** */
/*  Parsowanie otrzymanego komunikatu xml     */
/* ****************************************** */

  longRet = MessageParse( &plik, &hMessage );

  if ( longRet != BMD_OK ) {
	PRINT_ERROR("Błąd parsowania otrzymanego komunikatu. Error = %d\n", BMD_ERR_OP_FAILED);
	free0(plik.buf);
	sigemptyset(&_GLOBAL_mask_set);
 	sigprocmask(SIG_SETMASK, &_GLOBAL_mask_set, NULL);
	sem_post(&(_GLOBAL_shptr->clientMutex));
	return soap_receiver_fault(soap, "Message parse error", "Syntax error in xml message");
  }

 /* ******************************************** */
 /*      Pobranie adresu email i numeru tel      */
 /*                  odbiorcy                    */
 /* ******************************************** */ 

 /* ******************************************************** */
 /* Jeżeli kolejkowanie komunikatów jest wyłączone to nie    */
 /* synchronizujemy dostępu do pamięci dzielonej.            */
 /* Awizo jest wysyłane od razu.                             */
 /* ******************************************************** */

 /* ************************************************* */
 /*  Jeżeli parsowanie komunikatu XML zakończy się    */
 /*     pomyślnie, to nadawany jest mu unikalny       */
 /*    identyfikator wiadomości (MSGID)               */
 /* ************************************************* */

 prepareMessageId(hMessage, MsgID);

 if ((_GLOBAL_shptr->config).mailqueue == 1) {

 /* ******************************************************************** */
 /*   Synchronizacja dosŧepu do zapisu i odczytu komunikatów xmlowych.   */
 /* ******************************************************************** */

 /* ***************************************** */
 /*   Zapis komunikatu do pamięci dzielonej   */
 /* ***************************************** */
  
   memcpy((_GLOBAL_shptr->buf).message, plik.buf, plik.size );
   memcpy((_GLOBAL_shptr->buf).messageID, *MsgID, strlen(*MsgID)+1 );
   (_GLOBAL_shptr->buf).msgSize = plik.size;

 /* ***************************************** */
 /*  Zapis komunikatu do kolejki komunikatów  */
 /* ***************************************** */


   free0(plik.buf);
   kill(0, SIGUSR1);
 }
 else {

   free0(plik.buf);

 /* **************************************** */
 /*  Od teraz tu wykonujemy połączenie z db  */
 /* **************************************** */

   _GLOBAL_awizoConfig.dbase_handler=NULL;
   long status = bmd_db_connect2(_GLOBAL_awizoConfig.dbIP, \
				 _GLOBAL_awizoConfig.dbPort, \
				 _GLOBAL_awizoConfig.dbName, \
				 _GLOBAL_awizoConfig.dbUser, \
				 _GLOBAL_awizoConfig.dbPassword, \
				  &(_GLOBAL_awizoConfig.dbase_handler));
 
	if ( status < 0 ){
		PRINT_ERROR("Błąd połączenia z bazą danych. Error = %d\n", status);
		return 0;
	}

 
 /* ***************************************** */
 /*   sprawdzenie trybu pracy wysylki awiz    */
 /* ***************************************** */

   mode = getAwizoMode(hMessage);

   if (mode == modeNONE){
	PRINT_ERROR("Nie określono trybu pracy eAwiza. Error = %d\n", BMD_ERR_OP_FAILED);
	PRINT_INFO("MsgID : %s\n", *MsgID);

	saveRaportToDb(hMessage,_GLOBAL_awizoConfig.dbase_handler, *MsgID, -1, -1, "Awizo mode not selected", modeNONE);
	DisconnectDB();
	sigemptyset(&_GLOBAL_mask_set);
 	sigprocmask(SIG_SETMASK, &_GLOBAL_mask_set, NULL);
	xmlFreeDoc(hMessage);
	sem_post(&(_GLOBAL_shptr->clientMutex));
	return soap_receiver_fault(soap, "Awizo mode not selected", *MsgID);
   }

 


 /* *********************************** */
 /*  Tylko w przypadku wysylki smsow    */
 /* *********************************** */

   if (mode == modeSMS || mode == modeSMSEMAIL) {
	saveRaportToDb(hMessage,_GLOBAL_awizoConfig.dbase_handler, *MsgID, 0, 1, "Prepared successfully", modeSMS);

	smsSendStatus = sendSMS(template, hMessage, _GLOBAL_awizoConfig.dbase_handler, *MsgID);

	if ( smsSendStatus != BMD_OK ) {
		PRINT_ERROR("Błąd wysyłki wiadomości SMS. Error = %d\n", BMD_ERR_OP_FAILED);
		PRINT_INFO("MsgID : %s\n", *MsgID);
		/*
		sem_post(&(_GLOBAL_shptr->clientMutex));
		free0(templateId);
		*/
		updateRaportInDb(_GLOBAL_awizoConfig.dbase_handler, *MsgID, modeSMS, "SMS send error", '0');
		//xmlFreeDoc(hMessage);
		//return soap_receiver_fault(soap, "SMS send error", *MsgID);
  	}
	else {

		/* ********************************************* */
 		/*  Zapis statusu po prawidłowej wysyłce SMSów   */
 		/* ********************************************* */

    		updateRaportInDb(_GLOBAL_awizoConfig.dbase_handler, *MsgID, modeSMS, "SMS was successfully send", '1');
	}
   }

 /* ********************************** */
 /*  Tylko w przypadku wysylki maili   */
 /* ********************************** */

   if (mode == modeEMAIL || mode == modeSMSEMAIL) {

 /* ********************************** */
 /*   Przygotowanie tresci smime'a     */
 /* ********************************** */

	longRet = prepareAwizo(template, hMessage, _GLOBAL_awizoConfig.dbase_handler, *MsgID, &mail_info );
	if (longRet != BMD_OK) {
		PRINT_ERROR("Błąd komponowania wiadomości email. Error = %d\n", BMD_ERR_OP_FAILED);
		PRINT_INFO("MsgID : %s\n", *MsgID);
		sigemptyset(&_GLOBAL_mask_set);
 		sigprocmask(SIG_SETMASK, &_GLOBAL_mask_set, NULL);
		saveRaportToDb(hMessage,_GLOBAL_awizoConfig.dbase_handler, *MsgID, 0, longRet, "eAwizo prepared failed", modeEMAIL);
		DisconnectDB();
		xmlFreeDoc(hMessage);
		//free0(templateId);

		if (smsSendStatus != BMD_OK){
			return soap_receiver_fault(soap, "SMS send error and eAwizo prepared failed", *MsgID);
		}
		
		sem_post(&(_GLOBAL_shptr->clientMutex));

		return soap_receiver_fault(soap, "eAwizo prepared failed", *MsgID);
	}


	/* ******************************************* */
	/*  Zapis statusu o prawidłowym skomponowaniu  */
	/*   eAwiza do tabeli raportów bazy danych     */
	/* ******************************************* */

	saveRaportToDb(hMessage,_GLOBAL_awizoConfig.dbase_handler, *MsgID, 1, 1, "Prepared successfully", modeEMAIL);

 	_GLOBAL_awizoConfig.awizoTable = (mail_send_info_t*)calloc(1, sizeof(mail_send_info_t));

	if (_GLOBAL_awizoConfig.awizoTable == NULL) {
		PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
		PRINT_INFO("MsgID : %s\n", *MsgID);
		sigemptyset(&_GLOBAL_mask_set);
 		sigprocmask(SIG_SETMASK, &_GLOBAL_mask_set, NULL);
		saveRaportToDb(hMessage,_GLOBAL_awizoConfig.dbase_handler, *MsgID, 0, BMD_ERR_MEMORY, "eAwizo table memory error", modeEMAIL);
		DisconnectDB();
		xmlFreeDoc(hMessage);
		//free0(templateId);
		sem_post(&(_GLOBAL_shptr->clientMutex));

		if (smsSendStatus != BMD_OK){
			return soap_receiver_fault(soap, "SMS send error and eAwizo table memory error", *MsgID);
		}

		return soap_receiver_fault(soap, "eAwizo table memory error", *MsgID);
  	}

	/*wysylka*/
	memcpy(&(_GLOBAL_awizoConfig.awizoTable[0]), mail_info, sizeof(mail_send_info_t));

	emailSendStatus = bmd_send_envoice_avizo(_GLOBAL_awizoConfig.awizoTable, 1, &response);

	if (emailSendStatus != BMD_OK) {
		PRINT_ERROR("Błąd wysyłania wiadomości email. Error = %d\n", BMD_ERR_OP_FAILED);
		PRINT_INFO("MsgID : %s\n", *MsgID);
		sigemptyset(&_GLOBAL_mask_set);
 		sigprocmask(SIG_SETMASK, &_GLOBAL_mask_set, NULL);
		updateRaportInDb(_GLOBAL_awizoConfig.dbase_handler, *MsgID, modeEMAIL, "eAwizo send failed", response[0]);
		DisconnectDB();
		free0(response);
		freeMailSendInfo(_GLOBAL_awizoConfig.awizoTable, 1);
		xmlFreeDoc(hMessage);
		sem_post(&(_GLOBAL_shptr->clientMutex));

		if (smsSendStatus != BMD_OK){
			return soap_receiver_fault(soap, "SMS and eAwizo send failed", *MsgID);
		}

		return soap_receiver_fault(soap, "eAwizo send failed", *MsgID);
	}	
	//PRINT_INFO("------\n");
	updateRaportInDb(_GLOBAL_awizoConfig.dbase_handler, *MsgID, modeEMAIL, "eAwizo send successfully", response[0]);
	DisconnectDB();
	free0(response);
	freeMailSendInfo(_GLOBAL_awizoConfig.awizoTable, 1);
	free0(_GLOBAL_awizoConfig.awizoTable);

	if (smsSendStatus != BMD_OK){
		sem_post(&(_GLOBAL_shptr->clientMutex));
		return soap_receiver_fault(soap, "SMS send failed and eAwizo send successfully", *MsgID);
	}
	
   }

 sigemptyset(&_GLOBAL_mask_set);
 sigprocmask(SIG_SETMASK, &_GLOBAL_mask_set, NULL);
 sem_post(&(_GLOBAL_shptr->clientMutex));

 }

 sem_wait(&(_GLOBAL_shptr->awizoActionMutex));

  _GLOBAL_shptr->awizoAction = queueNothing;

 sem_post(&(_GLOBAL_shptr->awizoActionMutex));


 xmlFreeDoc(hMessage);
 //free0(templateId);
 return SOAP_OK;

}


