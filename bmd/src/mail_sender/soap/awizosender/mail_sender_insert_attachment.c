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



int ns__awizoInsertAttachment(struct soap* soap, struct ns__attachmentComplex *emailAttachment, long* result)
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


PRINT_INFO("AWIZOSOAPSERVERINF Insert Attachment.\n" );

if (_GLOBAL_shptr->awizoAction != queueNothing) {
	PRINT_INFO("AWIZOSOAPSERVERINF Can't insert attachment, server already in use!\n" );
	return soap_receiver_fault(soap, "Can't insert attachment" , "Awizo serwer already in use");
}

if (emailAttachment->templateid <= 0) {
	PRINT_ERROR("Niepoprawny identyfikator szablonu. Error = %d\n", BMD_ERR_OP_FAILED);
	return soap_receiver_fault(soap, "Illegal template id value", "template id <= 0");
}

asprintf(&SQLFront, "INSERT INTO mailing_email_attachments(templateid");
asprintf(&SQLBack, "VALUES(%ld", emailAttachment->templateid); 


if (emailAttachment->filename != NULL ) {
	asprintf(&SQLBackTmp, "%s", SQLBack);
	asprintf(&SQLFrontTmp, "%s", SQLFront);
	free0(SQLBack);
	free0(SQLFront);
	asprintf(&SQLFront, "%s, filename", SQLFrontTmp);
	asprintf(&SQLBack, "%s, '%s'", SQLBackTmp, emailAttachment->filename);
	free0(SQLFrontTmp);
	free0(SQLBackTmp);
}

if (emailAttachment->mimetype != NULL ) {
	asprintf(&SQLBackTmp, "%s", SQLBack);
	asprintf(&SQLFrontTmp, "%s", SQLFront);
	free0(SQLBack);
	free0(SQLFront);
	asprintf(&SQLFront, "%s, mimetype", SQLFrontTmp);
	asprintf(&SQLBack, "%s, '%s'", SQLBackTmp, emailAttachment->mimetype);
	free0(SQLFrontTmp);
	free0(SQLBackTmp);
}

if (emailAttachment->disposition != NULL ) {
	asprintf(&SQLBackTmp, "%s", SQLBack);
	asprintf(&SQLFrontTmp, "%s", SQLFront);
	free0(SQLBack);
	free0(SQLFront);
	asprintf(&SQLFront, "%s, disposition", SQLFrontTmp);
	asprintf(&SQLBack, "%s, '%s'", SQLBackTmp, emailAttachment->disposition);
	free0(SQLFrontTmp);
	free0(SQLBackTmp);
}

if (emailAttachment->text != NULL ) {
	asprintf(&SQLBackTmp, "%s", SQLBack);
	asprintf(&SQLFrontTmp, "%s", SQLFront);
	free0(SQLBack);
	free0(SQLFront);
	asprintf(&SQLFront, "%s, text", SQLFrontTmp);
	asprintf(&SQLBack, "%s, '%s'", SQLBackTmp, emailAttachment->text);
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
bmd_db_result_free(&query_result);
free0(stringSQL);

if (longRet != BMD_OK){
	PRINT_ERROR("Błąd zapisu danych do bazy. Error = %ld\n", longRet);
	bmd_db_disconnect(&dbase_handler);
	return soap_receiver_fault(soap, "Error saving data", "Maybe no database connection");
}

bmd_db_disconnect(&dbase_handler);

return SOAP_OK;

}
