#include <semaphore.h>
#include <bmd/mail_sender/queue.h>
#include <bmd/mail_sender/shmem.h>
#include <bmd/mail_sender/cache.h>
#include <bmd/mail_sender/attachment.h>
#include <bmd/mail_sender/mail_sender.h>
#include <bmd/libbmdglobals/libbmdglobals.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>
#include "soapH.h"


/* ************************************************** */
/*   Rozważyć sprawdzanie statusu wysłanych  eAwiz    */
/* ************************************************** */

int ns__awizoSetFromMsgQueueSend(struct soap* soap, long* result) {


 PRINT_INFO("AWIZOSOAPSERVERINF sent current awizo set from mailqueue.\n" );

 if (_GLOBAL_shptr->awizoAction != queueNothing) {
  PRINT_INFO("AWIZOSOAPSERVERINF Awizo sending in progress !\n" );
  *result = _GLOBAL_shptr->awizoAction;
  return SOAP_OK;
 }

 *result = _GLOBAL_shptr->awizoAction;

 kill( 0, SIGUSR2 ); 

 PRINT_INFO("AWIZOSOAPSERVERINF Request served correctly !\n" );

 return SOAP_OK;

}


