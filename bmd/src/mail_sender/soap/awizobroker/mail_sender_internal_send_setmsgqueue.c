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

    PRINT_ERROR("Komunikat nie obsługiwany przez serwer brokera e-Awiza. Error = %d\n", BMD_ERR_OP_FAILED);                                                  
    return soap_receiver_fault(soap, "Komunikat nie obsługiwany przez serwer brokera e-Awiza", NULL); 

}


