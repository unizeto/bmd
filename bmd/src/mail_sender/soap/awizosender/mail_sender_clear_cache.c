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
#include <bmd/mail_sender/mail_sender.h>
#include <bmd/mail_sender/queue.h>
#include "soapH.h"


int ns__clearCache(struct soap* soap, long* result) {

 PRINT_INFO("AWIZOSOAPSERVERINF Clear current awizo cache.\n" );

 if (_GLOBAL_shptr->awizoAction != queueNothing) {
  PRINT_INFO("AWIZOSOAPSERVERINF Can't clear awizo cache, structure already in use !\n" );
  *result = _GLOBAL_shptr->awizoAction;
  return SOAP_OK;
 }

 *result = _GLOBAL_shptr->awizoAction;

 kill(0, SIGRTMIN+5);

 PRINT_INFO("AWIZOSOAPSERVERINF Request served correctly !\n" );

 return SOAP_OK;

}


