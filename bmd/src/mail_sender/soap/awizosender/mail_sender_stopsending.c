#include <semaphore.h>
#include <bmd/mail_sender/queue.h>
#include <bmd/mail_sender/shmem.h>
#include <bmd/mail_sender/cache.h>
#include <bmd/mail_sender/attachment.h>
#include <bmd/mail_sender/mail_sender.h>
#include <bmd/libbmdglobals/libbmdglobals.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>
#include "soapH.h"


int ns__stopSendingProcess(struct soap* soap,long awizo_switch, long* result) {


 PRINT_INFO("AWIZOSOAPSERVERINF start/stop sending awizos.\n" );

 if (awizo_switch < 0 || awizo_switch > 1) {
	return soap_receiver_fault(soap, "eAwizo wrong param", "only 0-continue sending or 1-stop sending allowed");
 }

 sem_wait(&(_GLOBAL_shptr->awizoActionMutex));

 (_GLOBAL_shptr->config).stopSending = awizo_switch;

 sem_post(&(_GLOBAL_shptr->awizoActionMutex));

 return SOAP_OK;

}


