#include <semaphore.h>
#include <string.h>
#include <bmd/mail_sender/queue.h>
#include <bmd/mail_sender/shmem.h>
#include <bmd/mail_sender/cache.h>
#include <bmd/mail_sender/attachment.h>
#include <bmd/mail_sender/mail_sender.h>
#include <bmd/libbmdglobals/libbmdglobals.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>
#include "soapH.h"


/* Poprawić zmianę konfiguracji */

int ns__changeConfiguration(struct soap* soap, struct ns__awizoConfig *conf, long* result) {

 sem_wait(&(_GLOBAL_shptr->clientMutex));

 PRINT_INFO("AWIZOSOAPSERVERINF CHANGE AWIZO CONFIGURATION.\n" );

 if (conf->maxcachesize) {
	(_GLOBAL_shptr->config).mailqueue = atoi(conf->mailqueue);
 }
 else {
	(_GLOBAL_shptr->config).mailqueue = -1;
 }

 if (conf->maxmailqueue) {
	(_GLOBAL_shptr->config).maxmailqueue = atoi(conf->maxmailqueue);
 }
 else {
	(_GLOBAL_shptr->config).maxmailqueue = -1;
 }

 if (conf->sendingtime) {
	(_GLOBAL_shptr->config).sendingtime = atoi(conf->sendingtime);
 }
 else {
	(_GLOBAL_shptr->config).sendingtime = -1;
 }

 PRINT_INFO("AWIZOSOAPSERVERINF CONFIGURATION INFO.\n" );
 PRINT_INFO("AWIZOSOAPSERVERINF MAIL QUEUE : %s.\n", conf->mailqueue);
 PRINT_INFO("AWIZOSOAPSERVERINF MAX CACHE SIZE : %s.\n", conf->maxcachesize);
 PRINT_INFO("AWIZOSOAPSERVERINF MAX MAIL QUEUE : %s.\n", conf->maxmailqueue);
 PRINT_INFO("AWIZOSOAPSERVERINF AWIZO SENDING TIME : %s.\n", conf->sendingtime);

 kill( getppid(), SIGHUP );


 return SOAP_OK;

}


