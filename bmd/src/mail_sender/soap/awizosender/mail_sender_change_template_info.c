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


int ns__awizoChangeTemplateInfo(struct soap* soap, long id, struct ns__templateComplex *emailTemplate, long* result) {


/* ------------------------------- */

   char*	  SQLfront = NULL;
   char*	   SQLback = NULL;
   char*	    SQLTmp = NULL;
   long		   longRet =    0;
   void*     dbase_handler = NULL;
   void*      query_result = NULL;

/* ------------------------------- */ 

 PRINT_INFO("AWIZOSOAPSERVERINF Change Template Informations.\n" );

 if (_GLOBAL_shptr->awizoAction != queueNothing) {
	PRINT_INFO("AWIZOSOAPSERVERINF Can't change template informations, server already in use!\n" );
	return soap_receiver_fault(soap, "Can't change template info" , "Awizo serwer already in use");
 }

 if (id <= 0) {
	PRINT_ERROR("Niepoprawny identyfikator szablonu. Error = %d\n", BMD_ERR_OP_FAILED);
	return soap_receiver_fault(soap, "Illegal template id value", "template id <= 0");
 }

 asprintf(&SQLfront, "UPDATE mailing_email_template SET ");

 if (emailTemplate->created != NULL ) {
	return soap_receiver_fault(soap, "You can not change this param", "created attribute has default value");
 }

 if (emailTemplate->deleted != NULL ) {
	return soap_receiver_fault(soap, "You can not change this param", "You can change deleted attribute using different function");
 }

 if (emailTemplate->messagesenderid > 0 ) {
    if (SQLback) {
	asprintf(&SQLTmp, "%s", SQLback);
	free0(SQLback); 
	asprintf(&SQLback, "%s, messagesenderid = %ld", SQLTmp, emailTemplate->messagesenderid);
	free0(SQLTmp);
    }	
    else {
    	asprintf(&SQLback, "messagesenderid = %ld",  emailTemplate->messagesenderid);
    }
 }

 if (emailTemplate->name != NULL ) {
    if (SQLback) {
    	asprintf(&SQLTmp, "%s", SQLback);
	free0(SQLback); 
	asprintf(&SQLback, "%s, name = '%s'", SQLTmp, emailTemplate->name);
	free0(SQLTmp);
    }
    else {
	asprintf(&SQLback, "name = '%s'", emailTemplate->name);
    }	
 }

 if (emailTemplate->lang != NULL ) {
    if (SQLback) {
    	asprintf(&SQLTmp, "%s", SQLback);
	free0(SQLback); 
	asprintf(&SQLback, "%s, lang = '%s'", SQLTmp, emailTemplate->lang);
	free0(SQLTmp);
    }
    else {
	asprintf(&SQLback, "lang = '%s'", emailTemplate->lang);
    }
 }

 if (emailTemplate->version != NULL ) {
    if (SQLback) {
	asprintf(&SQLTmp, "%s", SQLback);
	free0(SQLback); 
	asprintf(&SQLback, "%s, version = '%s'", SQLTmp, emailTemplate->version);
	free0(SQLTmp);
    }
    else {
	asprintf(&SQLback, "version = '%s'", emailTemplate->version);
    }
 }

 if (emailTemplate->description != NULL ) {
    if (SQLback) {
	asprintf(&SQLTmp, "%s", SQLback);
	free0(SQLback); 
	asprintf(&SQLback, "%s, description = '%s'", SQLTmp, emailTemplate->description);
	free0(SQLTmp);
    }
    else {
	asprintf(&SQLback, "description = '%s'", emailTemplate->description);
    }
 }

 if (emailTemplate->xsltohtml != NULL ) {
    if (SQLback) {
	asprintf(&SQLTmp, "%s", SQLback);
	free0(SQLback); 
	asprintf(&SQLback, "%s, xsltohtml = '%s'", SQLTmp, emailTemplate->xsltohtml);
	free0(SQLTmp);
    }
    else {
	asprintf(&SQLback, "xsltohtml = '%s'", emailTemplate->xsltohtml);
    }
 }

 if (emailTemplate->xsltotext != NULL ) {
    if (SQLback) {
	asprintf(&SQLTmp, "%s", SQLback);
	free0(SQLback); 
	asprintf(&SQLback, "%s, xsltotext = '%s'", SQLTmp, emailTemplate->xsltotext);
	free0(SQLTmp);
    }
    else {
	asprintf(&SQLback, "xsltotext = '%s'", emailTemplate->xsltotext);
    }
 }

 if (emailTemplate->sign != NULL ) {
    if (SQLback) {
	asprintf(&SQLTmp, "%s", SQLback);
	free0(SQLback); 
	asprintf(&SQLback, "%s, sign = %ld", SQLTmp, emailTemplate->sign);
	free0(SQLTmp);
    }
    else {
	asprintf(&SQLback, "sign = %ld", emailTemplate->sign);
    }
 }

 if (SQLback == NULL) {
	PRINT_ERROR("Nie zmieniono danych szablonu. Error = %d\n", BMD_ERR_OP_FAILED);
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


