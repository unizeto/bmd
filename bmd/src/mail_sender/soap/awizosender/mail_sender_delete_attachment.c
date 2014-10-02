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



int ns__awizoDeleteAttachment(struct soap* soap, long id, long* result)
{

 /* ------------------------------------ */

     void*	dbase_handler   = NULL;
     char*	stringSQL	= NULL;
     char*	stringTime	= NULL;	
     long	longRet         =    0;
     void*	query_result    = NULL;

 /* ------------------------------------- */



 PRINT_INFO("AWIZOSOAPSERVERINF Delete Awizo Attachment.\n" );

 if (_GLOBAL_shptr->awizoAction != queueNothing) {
	PRINT_INFO("AWIZOSOAPSERVERINF Can't delete attachment, server already in use!\n" );
	return soap_receiver_fault(soap, "Can't delete attachment" , "Awizo serwer already in use");
 }

 if (id <= 0) {
	PRINT_ERROR("Niepoprawny identyfikator załącznika. Error = %d\n", BMD_ERR_OP_FAILED);
	return soap_receiver_fault(soap, "Illegal attachment id value", "template id value <= 0");
 }

 getTime(&stringTime);
 asprintf( &stringSQL, "UPDATE mailing_email_attachments SET deleted = '%s' WHERE id = %ld", stringTime, id);

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
 free0(stringTime);

 if (longRet != BMD_OK){
	PRINT_ERROR("Błąd zapisu danych do bazy. Error = %ld\n", longRet);
	bmd_db_disconnect(&dbase_handler);
	return soap_receiver_fault(soap, "Error saving data", "Maybe no database connection");
 }

 bmd_db_disconnect(&dbase_handler);
 

return SOAP_OK;


}
