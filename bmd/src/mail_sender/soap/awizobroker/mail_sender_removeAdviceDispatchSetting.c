#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/dir.h>
#include <fcntl.h>
#include <bmd/mail_sender/attachment.h>
#include <bmd/mail_sender/cache.h>
#include <bmd/libbmdglobals/libbmdglobals.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>
#include <bmd/mail_sender/queue.h>
#include <bmd/mail_sender/mail_sender.h>
#include "soapH.h"




/**********************************************/
/*  Żądanie pobrania list nadawców            */
/**********************************************/


int ns__removeAdviceDispatchSetting(struct soap* soap, long dispatchSettingId, long* result) {


/* ------------------------------ */

   long	        longRet   =    0;
   long		attempt	  =    0;
   queueNode_t* iterator  = NULL;
   char*	host	  = NULL;
   struct soap       soap2Eawizo;

/* ------------------------------ */


 attempt = MAX_ATTEMPT;

 soap_init2(&soap2Eawizo,SOAP_C_UTFSTRING,SOAP_C_UTFSTRING);

 iterator = (_GLOBAL_awizoConfig.bmdservers).head;

 for(;;){
  asprintf(&host, "http://%s:%s",((bmdserver_t*)(iterator->node))->addr,((bmdserver_t*)(iterator->node))->port);

  longRet = soap_call_ns__removeAdviceDispatchSetting(&soap2Eawizo, host, NULL, dispatchSettingId, result);
  free0(host);

  if (longRet == SOAP_OK) {
	PRINT_INFO("Zapisano rekord w bazie danych\n");
	return SOAP_OK;
  }
  else {
	if (iterator->prev){
		iterator=iterator->prev;
		continue;
	}
	else if (--attempt <= 0){
		PRINT_ERROR("Nie udało się nawiązać połączenia z żadnym z serwerów e-Awiz. Error = %d\n", BMD_ERR_OP_FAILED);
		//soap_end(&soap2Eawizo);
		//soap_done(&soap2Eawizo);
		//return soap_receiver_fault(soap, "Nie udało się nawiązać połączenia z żadnym z serwerów e-Awiz.", NULL);
		attempt = MAX_ATTEMPT;
	}
	iterator = (_GLOBAL_awizoConfig.bmdservers).head;
	sleep(1);
  }
 }
	soap_end(&soap2Eawizo);
	soap_done(&soap2Eawizo);

	return SOAP_OK;






}


