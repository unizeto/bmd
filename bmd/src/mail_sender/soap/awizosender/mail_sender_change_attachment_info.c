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



int ns__awizoChangeAttachmentInfo(struct soap* soap, long id, struct ns__attachmentComplex *emailAttachment, long* result) {

/* ------------------------------- */

   char*	  SQLfront = NULL;
   char*	   SQLback = NULL;
   char*	    SQLTmp = NULL;
   long		   longRet =    0;
   void*     dbase_handler = NULL;
   void*      query_result = NULL;

/* ------------------------------- */ 

PRINT_INFO("AWIZOSOAPSERVERINF Change Attachment Informations.\n" );

 if (_GLOBAL_shptr->awizoAction != queueNothing) {
	PRINT_INFO("AWIZOSOAPSERVERINF Can't change attachment informations, server already in use!\n" );
	return soap_receiver_fault(soap, "Can't change attachment info" , "Awizo serwer already in use");
 }

 if (id <= 0) {
	PRINT_ERROR("Niepoprawny identyfikator załącznika. Error = %d\n", BMD_ERR_OP_FAILED);
	return soap_receiver_fault(soap, "Illegal attachment id value", "attachment id <= 0");
 }

 asprintf(&SQLfront, "UPDATE mailing_email_attachments SET "); 

 if (emailAttachment->created != NULL ) {
	return soap_receiver_fault(soap, "You can not change this param", "created attribute has default value"); 
 }

 if (emailAttachment->deleted != NULL ) {
	return soap_receiver_fault(soap, "You can not change this param", "You can change deleted attribute using different function");
 }

 if (emailAttachment->templateid > 0 ) {
	if (SQLback) {
		asprintf(&SQLTmp, "%s", SQLback);
		free0(SQLback); 
		asprintf(&SQLback, "%s, templateid = %ld", SQLTmp, emailAttachment->templateid);
		free0(SQLTmp);
	}
	else{
		asprintf(&SQLback, "templateid = %ld", emailAttachment->templateid);
	}
 }

 if (emailAttachment->filename != NULL ) {
	if (SQLback) {
		asprintf(&SQLTmp, "%s", SQLback);
		free0(SQLback); 
		asprintf(&SQLback, "%s, filename = '%s'", SQLTmp, emailAttachment->filename);
		free0(SQLTmp);
	}
	else {
		asprintf(&SQLback, "filename = '%s'", emailAttachment->filename);
	}
 }

 if (emailAttachment->mimetype != NULL ) {
	if(SQLback) {
		asprintf(&SQLTmp, "%s", SQLback);
		free0(SQLback); 
		asprintf(&SQLback, "%s, mimetype = '%s'", SQLTmp, emailAttachment->mimetype);
		free0(SQLTmp);
	}
	else {
		asprintf(&SQLback, "mimetype = '%s'", emailAttachment->mimetype);
	}
 }

 if (emailAttachment->disposition != NULL ) {
	if (SQLback) {
		asprintf(&SQLTmp, "%s", SQLback);
		free0(SQLback); 
		asprintf(&SQLback, "%s, disposition = '%s'", SQLTmp, emailAttachment->disposition);
		free0(SQLTmp);
	}
	else {
		asprintf(&SQLback, "disposition = '%s'", emailAttachment->disposition);
	}
 }

 if (emailAttachment->text != NULL ) {
	if (SQLback) {
		asprintf(&SQLTmp, "%s", SQLback);
		free0(SQLback); 
		asprintf(&SQLback, "%s, text = '%s'", SQLTmp, emailAttachment->text);
		free0(SQLTmp);
	}
	else {
		asprintf(&SQLback, "text = '%s'", emailAttachment->text);
	}
 }

 if (SQLback == NULL) {
	PRINT_ERROR("Nie zmieniono danych załącznika. Error = %d\n", BMD_ERR_OP_FAILED);
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


