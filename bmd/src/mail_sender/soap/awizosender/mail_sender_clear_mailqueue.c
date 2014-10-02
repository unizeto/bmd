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
/*    Żądanie czyszczenia kolejki z mailami   */
/**********************************************/

int ns__clearMailQueue(struct soap* soap, long* result) {

 PRINT_INFO("AWIZOSOAPSERVERINF Clear current awizo set from mailqueue.\n" );

 if (_GLOBAL_shptr->awizoAction != queueNothing) {
  PRINT_INFO("AWIZOSOAPSERVERINF Can't clear awizo queue, structure already in use !\n" );
  *result = _GLOBAL_shptr->awizoAction;
  return SOAP_OK;
 }

 *result = _GLOBAL_shptr->awizoAction;

 kill( 0, SIGRTMIN+7 );

 PRINT_INFO("AWIZOSOAPSERVERINF Request served correctly !\n" );
 
 return SOAP_OK;

}


