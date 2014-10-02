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
#include <bmd/libbmderr/libbmderr.h>
#include <bmd/mail_sender/queue.h>
#include "soapH.h"


/**********************************************/
/*  Żądanie pobrania ilości maili z kolejki   */
/**********************************************/

int ns__countMailFromQueue(struct soap* soap, long* result) {

    PRINT_ERROR("Komunikat nie obsługiwany przez serwer brokera e-Awiza. Error = %d\n", BMD_ERR_OP_FAILED);                                                  
    return soap_receiver_fault(soap, "Komunikat nie obsługiwany przez serwer brokera e-Awiza", NULL); 

}

