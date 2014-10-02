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



int ns__awizoInsertSender(struct soap* soap, struct ns__senderComplex* sender, long* result)
{

/* ---------------------------- */

   char*  SQLFront      = NULL;
   char*  SQLBack       = NULL;
   char*  SQLFrontTmp   = NULL;
   char*  SQLBackTmp    = NULL;
   long   longRet       =    0;
   void*  dbase_handler = NULL;
   char*  stringSQL	= NULL;
   void*  query_result  = NULL;


/* ------------------------------------ */


PRINT_INFO("AWIZOSOAPSERVERINF Insert Sender.\n" );

if (_GLOBAL_shptr->awizoAction != queueNothing) {
	PRINT_INFO("AWIZOSOAPSERVERINF Can't insert sender, server already in use!\n" );
	return soap_receiver_fault(soap, "Can't insert sender" , "Awizo serwer already in use");
}

if (sender->name == NULL) {
	PRINT_ERROR("Nie zdefiniowano nazwy nadawcy. Error = %d\n", BMD_ERR_OP_FAILED);
	return soap_receiver_fault(soap, "Illegal sender name", "sender name = NULL");
}

asprintf(&SQLFront, "INSERT INTO mailing_message_senders(name");
asprintf(&SQLBack, "VALUES('%s'", sender->name); 


if (sender->email != NULL ) {
	asprintf(&SQLBackTmp, "%s", SQLBack);
	asprintf(&SQLFrontTmp, "%s", SQLFront);
	free0(SQLBack);
	free0(SQLFront);
	asprintf(&SQLFront, "%s, email", SQLFrontTmp);
	asprintf(&SQLBack, "%s, '%s'", SQLBackTmp, sender->email);
	free0(SQLFrontTmp);
	free0(SQLBackTmp);
}

if (sender->smtpaddr != NULL ) {
	asprintf(&SQLBackTmp, "%s", SQLBack);
	asprintf(&SQLFrontTmp, "%s", SQLFront);
	free0(SQLBack);
	free0(SQLFront);
	asprintf(&SQLFront, "%s, smtpaddr", SQLFrontTmp);
	asprintf(&SQLBack, "%s, '%s'", SQLBackTmp, sender->smtpaddr);
	free0(SQLFrontTmp);
	free0(SQLBackTmp);
}

if (sender->smtpport != NULL ) {
	asprintf(&SQLBackTmp, "%s", SQLBack);
	asprintf(&SQLFrontTmp, "%s", SQLFront);
	free0(SQLBack);
	free0(SQLFront);
	asprintf(&SQLFront, "%s, smtpport", SQLFrontTmp);
	asprintf(&SQLBack, "%s, '%s'", SQLBackTmp, sender->smtpport);
	free0(SQLFrontTmp);
	free0(SQLBackTmp);
}

if (sender->smtpuser != NULL ) {
	asprintf(&SQLBackTmp, "%s", SQLBack);
	asprintf(&SQLFrontTmp, "%s", SQLFront);
	free0(SQLBack);
	free0(SQLFront);
	asprintf(&SQLFront, "%s, smtpuser", SQLFrontTmp);
	asprintf(&SQLBack, "%s, '%s'", SQLBackTmp, sender->smtpuser);
	free0(SQLFrontTmp);
	free0(SQLBackTmp);
}

if (sender->smtppassword != NULL ) {
	asprintf(&SQLBackTmp, "%s", SQLBack);
	asprintf(&SQLFrontTmp, "%s", SQLFront);
	free0(SQLBack);
	free0(SQLFront);
	asprintf(&SQLFront, "%s, smtppassword", SQLFrontTmp);
	asprintf(&SQLBack, "%s, '%s'", SQLBackTmp, sender->smtppassword);
	free0(SQLFrontTmp);
	free0(SQLBackTmp);
}

if (sender->pfxfile != NULL ) {
	asprintf(&SQLBackTmp, "%s", SQLBack);
	asprintf(&SQLFrontTmp, "%s", SQLFront);
	free0(SQLBack);
	free0(SQLFront);
	asprintf(&SQLFront, "%s, pfxfile", SQLFrontTmp);
	asprintf(&SQLBack, "%s, '%s'", SQLBackTmp, sender->pfxfile);
	free0(SQLFrontTmp);
	free0(SQLBackTmp);
}

if (sender->pfxpin != NULL ) {
	asprintf(&SQLBackTmp, "%s", SQLBack);
	asprintf(&SQLFrontTmp, "%s", SQLFront);
	free0(SQLBack);
	free0(SQLFront);
	asprintf(&SQLFront, "%s, pfxpin", SQLFrontTmp);
	asprintf(&SQLBack, "%s, '%s'", SQLBackTmp, sender->pfxpin);
	free0(SQLFrontTmp);
	free0(SQLBackTmp);
}

if (sender->maximagesize != NULL ) {
	asprintf(&SQLBackTmp, "%s", SQLBack);
	asprintf(&SQLFrontTmp, "%s", SQLFront);
	free0(SQLBack);
	free0(SQLFront);
	asprintf(&SQLFront, "%s, maximagesize", SQLFrontTmp);
	asprintf(&SQLBack, "%s, '%s'", SQLBackTmp, sender->maximagesize);
	free0(SQLFrontTmp);
	free0(SQLBackTmp);
}

if (sender->csvdir != NULL ) {
	asprintf(&SQLBackTmp, "%s", SQLBack);
	asprintf(&SQLFrontTmp, "%s", SQLFront);
	free0(SQLBack);
	free0(SQLFront);
	asprintf(&SQLFront, "%s, csvdir", SQLFrontTmp);
	asprintf(&SQLBack, "%s, '%s'", SQLBackTmp, sender->csvdir);
	free0(SQLFrontTmp);
	free0(SQLBackTmp);
}

if (sender->csvseparator != NULL ) {
	asprintf(&SQLBackTmp, "%s", SQLBack);
	asprintf(&SQLFrontTmp, "%s", SQLFront);
	free0(SQLBack);
	free0(SQLFront);
	asprintf(&SQLFront, "%s, csvseparator", SQLFrontTmp);
	asprintf(&SQLBack, "%s, '%s'", SQLBackTmp, sender->csvseparator);
	free0(SQLFrontTmp);
	free0(SQLBackTmp);
}

if (sender->awizoaction >= 0 ) {
	asprintf(&SQLBackTmp, "%s", SQLBack);
	asprintf(&SQLFrontTmp, "%s", SQLFront);
	free0(SQLBack);
	free0(SQLFront);
	asprintf(&SQLFront, "%s, awizoaction", SQLFrontTmp);
	asprintf(&SQLBack, "%s, %ld", SQLBackTmp, sender->awizoaction);
	free0(SQLFrontTmp);
	free0(SQLBackTmp);
}

if (sender->allowimageabsent >= 0 ) {
	asprintf(&SQLBackTmp, "%s", SQLBack);
	asprintf(&SQLFrontTmp, "%s", SQLFront);
	free0(SQLBack);
	free0(SQLFront);
	asprintf(&SQLFront, "%s, allowimageabsent", SQLFrontTmp);
	asprintf(&SQLBack, "%s, %ld", SQLBackTmp, sender->allowimageabsent);
	free0(SQLFrontTmp);
	free0(SQLBackTmp);
}

if (sender->smsapiurl != NULL ) {
	asprintf(&SQLBackTmp, "%s", SQLBack);
	asprintf(&SQLFrontTmp, "%s", SQLFront);
	free0(SQLBack);
	free0(SQLFront);
	asprintf(&SQLFront, "%s, smsapiurl", SQLFrontTmp);
	asprintf(&SQLBack, "%s, '%s'", SQLBackTmp, sender->smsapiurl);
	free0(SQLFrontTmp);
	free0(SQLBackTmp);
}

if (sender->smsapierror != NULL ) {
	asprintf(&SQLBackTmp, "%s", SQLBack);
	asprintf(&SQLFrontTmp, "%s", SQLFront);
	free0(SQLBack);
	free0(SQLFront);
	asprintf(&SQLFront, "%s, smsapierror", SQLFrontTmp);
	asprintf(&SQLBack, "%s, '%s'", SQLBackTmp, sender->smsapierror);
	free0(SQLFrontTmp);
	free0(SQLBackTmp);
}

asprintf(&SQLBackTmp, "%s", SQLBack);
asprintf(&SQLFrontTmp, "%s", SQLFront);
free0(SQLBack);
free0(SQLFront);
asprintf(&SQLFront, "%s)", SQLFrontTmp);
asprintf(&SQLBack, "%s);", SQLBackTmp);
free0(SQLFrontTmp);
free0(SQLBackTmp);

asprintf(&stringSQL, "%s %s", SQLFront, SQLBack);
free0(SQLFront);
free0(SQLBack);


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

longRet = bmd_db_execute_sql( dbase_handler, stringSQL,NULL,NULL, &query_result);

fprintf(stderr, "\n %p \n", query_result);

bmd_db_result_free(&query_result);
free0(stringSQL);

if (longRet != BMD_OK){
	PRINT_ERROR("Błąd zapisu danych do bazy. Error = %ld\n", longRet);
	bmd_db_disconnect(&dbase_handler);
	return soap_receiver_fault(soap, "Error saving data", "Maybe no database connection");
}

bmd_db_disconnect(&dbase_handler);

PRINT_INFO("Zapis wystawcy zakończony sukcesem");

return SOAP_OK;

}
