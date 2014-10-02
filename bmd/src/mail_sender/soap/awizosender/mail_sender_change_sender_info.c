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



int ns__awizoChangeSenderInfo(struct soap* soap, long id, struct ns__senderComplex* sender, long* result) {

/* ------------------------------- */

   char*	  SQLfront = NULL;
   char*	   SQLback = NULL;
   char*	    SQLTmp = NULL;
   long		   longRet =    0;
   void*     dbase_handler = NULL;
   void*      query_result = NULL;

/* ------------------------------- */ 



 PRINT_INFO("AWIZOSOAPSERVERINF Change Sender Informations.\n" );

 if (_GLOBAL_shptr->awizoAction != queueNothing) {
	PRINT_INFO("AWIZOSOAPSERVERINF Can't change Sender informations, server already in use!\n" );
	return soap_receiver_fault(soap, "Can't change sender info" , "Awizo serwer already in use");
 }

 if (id <= 0) {
	PRINT_ERROR("Niepoprawny identyfikator nadawcy. Error = %d\n", BMD_ERR_OP_FAILED);
	return soap_receiver_fault(soap, "Illegal sender id value", "sender id <= 0");
 }

 asprintf(&SQLfront, "UPDATE mailing_message_senders SET "); 

 if (sender->name != NULL ) {
	asprintf(&SQLback, "name = '%s'", sender->name);
 }

 if (sender->email != NULL ) {
	asprintf(&SQLTmp, "%s", SQLback);
	free0(SQLback); 
	asprintf(&SQLback, "%s, email = '%s'", SQLTmp, sender->email);
	free0(SQLTmp);
 }

 if (sender->created != NULL ) {
	asprintf(&SQLTmp, "%s", SQLback);
	free0(SQLback); 
	asprintf(&SQLback, "%s, created = '%s'", SQLTmp, sender->created);
	free0(SQLTmp);
 }

 if (sender->deleted != NULL ) {
	asprintf(&SQLTmp, "%s", SQLback);
	free0(SQLback); 
	asprintf(&SQLback, "%s, deleted = '%s'", SQLTmp, sender->deleted);
	free0(SQLTmp);
 }

 if (sender->smtpaddr != NULL ) {
	asprintf(&SQLTmp, "%s", SQLback);
	free0(SQLback); 
	asprintf(&SQLback, "%s, smtpaddr = '%s'", SQLTmp, sender->smtpaddr);
	free0(SQLTmp);
 }

 if (sender->smtpport != NULL ) {
	asprintf(&SQLTmp, "%s", SQLback);
	free0(SQLback); 
	asprintf(&SQLback, "%s, smtpport = '%s'", SQLTmp, sender->smtpport);
	free0(SQLTmp);
 }

 if (sender->smtpuser != NULL) {
	asprintf(&SQLTmp, "%s", SQLback);
	free0(SQLback); 
	asprintf(&SQLback, "%s, smtpuser = '%s'", SQLTmp, sender->smtpuser);
	free0(SQLTmp);
 }

 if (sender->smtppassword != NULL) {
	asprintf(&SQLTmp, "%s", SQLback);
	free0(SQLback); 
	asprintf(&SQLback, "%s, smtppassword = '%s'", SQLTmp, sender->smtppassword);
	free0(SQLTmp);
 }

 if (sender->pfxfile != NULL) {
	asprintf(&SQLTmp, "%s", SQLback);
	free0(SQLback); 
	asprintf(&SQLback, "%s, pfxfile = '%s'", SQLTmp, sender->pfxfile);
	free0(SQLTmp);
 }

 if (sender->pfxpin != NULL) {
	asprintf(&SQLTmp, "%s", SQLback);
	free0(SQLback); 
	asprintf(&SQLback, "%s, pfxpin = '%s'", SQLTmp, sender->pfxpin);
	free0(SQLTmp);
 }

 if (sender->maximagesize != NULL) {
	asprintf(&SQLTmp, "%s", SQLback);
	free0(SQLback); 
	asprintf(&SQLback, "%s, maximagesize = '%s'", SQLTmp, sender->maximagesize);
	free0(SQLTmp);
 }

 if (sender->csvdir != NULL) {
	asprintf(&SQLTmp, "%s", SQLback);
	free0(SQLback); 
	asprintf(&SQLback, "%s, csvdir = '%s'", SQLTmp, sender->csvdir);
	free0(SQLTmp);
 }

 if (sender->csvseparator != NULL) {
	asprintf(&SQLTmp, "%s", SQLback);
	free0(SQLback); 
	asprintf(&SQLback, "%s, csvseparator = '%s'", SQLTmp, sender->csvseparator);
	free0(SQLTmp);
 }

 if (sender->awizoaction > 0 ) {
	asprintf(&SQLTmp, "%s", SQLback);
	free0(SQLback); 
	asprintf(&SQLback, "%s, awizoaction = %ld", SQLTmp, sender->awizoaction);
	free0(SQLTmp);
 }

 if (sender->allowimageabsent > 0 ) {
	asprintf(&SQLTmp, "%s", SQLback);
	free0(SQLback); 
	asprintf(&SQLback, "%s, csvseparator = %ld", SQLTmp, sender->allowimageabsent);
	free0(SQLTmp);
 }

 if (sender->smsapiurl != NULL) {
	asprintf(&SQLTmp, "%s", SQLback);
	free0(SQLback); 
	asprintf(&SQLback, "%s, smsapiurl = '%s'", SQLTmp, sender->smsapiurl);
	free0(SQLTmp);
 }
 
 if (sender->smsapierror != NULL) {
	asprintf(&SQLTmp, "%s", SQLback);
	free0(SQLback); 
	asprintf(&SQLback, "%s, smsapierror = '%s'", SQLTmp, sender->smsapierror);
	free0(SQLTmp);
 }


 if (SQLback == NULL) {
	PRINT_ERROR("Nie zmieniono danych nadawcy. Error = %d\n", BMD_ERR_OP_FAILED);
	return SOAP_OK;
 }

 asprintf(&SQLTmp, "%s", SQLback);
 free0(SQLback);

 asprintf(&SQLback, "%s%s WHERE id = %ld;", SQLfront, SQLTmp, id );
 free0(SQLTmp);
 free0(SQLfront);

 longRet = bmd_db_connect2(_GLOBAL_awizoConfig.dbIP, \
				 _GLOBAL_awizoConfig.dbPort, \
				 _GLOBAL_awizoConfig.dbName, \
				 _GLOBAL_awizoConfig.dbUser, \
				 _GLOBAL_awizoConfig.dbPassword, \
				  &dbase_handler);
 
 if ( longRet < 0 ){
	PRINT_ERROR("Błąd połączenia z bazą danych. Error = %d\n", BMD_ERR_OP_FAILED);
	return soap_receiver_fault(soap, "Database connection failed", "Maybe database server does not work");
 }

 longRet = bmd_db_execute_sql( dbase_handler, SQLback,NULL,NULL, &query_result);
 bmd_db_result_free(&query_result);
 free0(SQLback);

 if (longRet != BMD_OK){
	PRINT_ERROR("Błąd zapisu danych do bazy. Error = %ld\n", longRet);
	bmd_db_disconnect(&dbase_handler);
	return soap_receiver_fault(soap, "Error saving data", "Maybe no database connection");
 }

 bmd_db_disconnect(&dbase_handler);


 return SOAP_OK;

}


