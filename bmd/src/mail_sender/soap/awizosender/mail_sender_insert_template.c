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


int ns__awizoInsertTemplate(struct soap* soap, struct ns__templateComplex* emailTemplate, long* result)
{

/* ------------------------------------ */

   void*	dbase_handler   = NULL;
   char*	stringSQL	= NULL;	
   long		messagesenderid =    0;
   long		longRet         =    0;
   char*        name		= NULL;
   char*	lang		= NULL;
   char*	version		= NULL;
   char*	description	= NULL;
   char*	xsltohtml	= NULL;
   char*	xsltotext	= NULL;
   long		sign		=    0;
   void*	query_result    = NULL;

/* --------------------------------------------- */



 PRINT_INFO("AWIZOSOAPSERVERINF Insert Awizo Template.\n" );

 if (_GLOBAL_shptr->awizoAction != queueNothing) {
	PRINT_INFO("AWIZOSOAPSERVERINF Can't insert new template, server already in use!\n" );
	return soap_receiver_fault(soap, "Can't insert new template" , "Awizo serwer already in use");
 }

 if (emailTemplate->messagesenderid <= 0) {
	PRINT_ERROR("Niepoprawny identyfikator nadawcy. Error = %d\n", BMD_ERR_OP_FAILED);
	return soap_receiver_fault(soap, "Illegal messagesenderid value", "messagesenderid <= 0");
 }
 else {
	messagesenderid = emailTemplate->messagesenderid;
 }

 if (strlen(emailTemplate->name)<=0) {
	PRINT_ERROR("Niepoprawna nazwa szablonu. Error = %d\n", BMD_ERR_OP_FAILED);
	return soap_receiver_fault(soap, "Illegal name value", "The length of name is zero");
 }
 else {
	asprintf(&name, emailTemplate->name);
 }

 if (emailTemplate->lang == NULL) {
	asprintf(&lang, "");
 }
 else {
	asprintf(&lang, emailTemplate->lang);
 }

 if (emailTemplate->version == NULL) {
	asprintf(&version, "");
 }
 else {
	asprintf(&version, emailTemplate->version);
 }

 if (emailTemplate->description == NULL) {
	asprintf(&description, "");
 }
 else {
	asprintf(&description, emailTemplate->description);
 }

 if (emailTemplate->xsltohtml == NULL) {
	asprintf(&xsltohtml, "");
 }
 else {
	asprintf(&xsltohtml, emailTemplate->xsltohtml);
 }

 if (emailTemplate->xsltotext == NULL) {
	asprintf(&xsltotext, "");
 }
 else{
	asprintf(&xsltotext, emailTemplate->xsltotext );
 }

 if (emailTemplate->sign <= 0) {
	sign = 0;
 }
 else{
	sign = emailTemplate->sign;
 }

 if (asprintf( &stringSQL, "INSERT INTO mailing_email_template(messagesenderid, name, lang, version, description, xsltohtml, xsltotext, sign ) VALUES(%ld, '%s', '%s', '%s', '%s', '%s', '%s', %ld);", messagesenderid, name, lang, version, description, xsltohtml, xsltotext, sign ) < 0) {
	PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
	free0(name);
	free0(lang);
	free0(version);
	free0(description);
	free0(xsltohtml);
	free0(xsltotext);
	return soap_receiver_fault(soap, "Memory allocation failed", NULL);
 }

 free0(name);
 free0(lang);
 free0(version);
 free0(description);
 free0(xsltohtml);
 free0(xsltotext);

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
	PRINT_ERROR("Błąd zapisu danych raportu do bazy. Error = %ld\n", longRet);
	bmd_db_disconnect(&dbase_handler);
	return soap_receiver_fault(soap, "Error saving data", "Maybe no database connection");
 }

 bmd_db_disconnect(&dbase_handler);

 PRINT_INFO("Zapis szablonu zakończony sukcesem\n");
 
 return SOAP_OK;

}
