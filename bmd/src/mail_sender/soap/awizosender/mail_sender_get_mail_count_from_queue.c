#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/dir.h>
#include <fcntl.h>
#include <bmd/mail_sender/attachment.h>
#include <bmd/mail_sender/cache.h>
#include <bmd/libbmdglobals/libbmdglobals.h>
#include <bmd/libbmderr/libbmderr.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>
#include <bmd/mail_sender/queue.h>
#include "soapH.h"


/**********************************************/
/*  Żądanie pobrania ilości maili z kolejki   */
/**********************************************/

int ns__countMailFromQueue(struct soap* soap, long* result) {

 
 PRINT_INFO("AWIZOSOAPSERVERINF Get count of mailqueue.\n" );
 PRINT_INFO("AWIZOSOAPSERVERINF count of mailqueue : %ld.\n", _GLOBAL_myXmlQueue.ulCount );

 *result = _GLOBAL_myXmlQueue.ulCount;

 return SOAP_OK;

}


