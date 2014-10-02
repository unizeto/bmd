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




int ns__awizoDownloadAttachment(struct soap* soap, long id, struct ns__attachmentComplex **emailAttachment )
{

/* ------------------------------------------ */

   void*	      dbase_handler   = NULL;
   char*	      stringSQL	      = NULL;
   long		      longRet         =    0;
   void*	      query_result    = NULL;
   long  	      rows            =    0;
   long  	      cols            =    0;
   void* 	      result          = NULL;
   db_row_strings_t*  rowstruct       = NULL;
   long               fetched         =    0;

/* ------------------------------------------ */



PRINT_INFO("AWIZOSOAPSERVERINF Download Awizo Attachment.\n" );

if (_GLOBAL_shptr->awizoAction != queueNothing) {
	PRINT_INFO("AWIZOSOAPSERVERINF Can't download attachment from database, server already in use!\n" );
	return soap_receiver_fault(soap, "Can't download attachment from database" , "Awizo serwer already in use");
}

(*emailAttachment)=(struct ns__attachmentComplex*)soap_malloc(soap, sizeof(struct ns__attachmentComplex));

if((*emailAttachment) == NULL) {
	PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
	return soap_receiver_fault(soap, "Memory allocation failed", NULL);
}

memset(*emailAttachment,0,sizeof(struct ns__attachmentComplex));

if ( asprintf( &stringSQL, "SELECT created, deleted, templateid, filename, mimetype, disposition, content FROM mailing_email_attachments WHERE id = %ld;", id) < 0) {
	PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
	return soap_receiver_fault(soap, "Memory allocation failed", NULL);
}

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

if ( bmd_db_execute_sql( dbase_handler, stringSQL, &rows, &cols, &result) < 0 ) {
	PRINT_ERROR("Błąd zapytania do bazy danych. Error = %d\n", BMD_ERR_OP_FAILED);
	bmd_db_disconnect(&dbase_handler);
	free0(stringSQL);
	return soap_receiver_fault(soap, "SQL request failed", NULL);
}

free0(stringSQL);

longRet = bmd_db_result_get_row(dbase_handler, result, 0, 0, &rowstruct, FETCH_NEXT, &fetched );

if (longRet == 0) {
	asprintf(&((*emailAttachment)->created), "%s", rowstruct->colvals[0]); 
	asprintf(&((*emailAttachment)->deleted), "%s", rowstruct->colvals[1]);
	(*emailAttachment)->templateid = atol(rowstruct->colvals[2]);
	asprintf(&((*emailAttachment)->filename), "%s", rowstruct->colvals[3]);
	asprintf(&((*emailAttachment)->mimetype), "%s", rowstruct->colvals[4]);
	asprintf(&((*emailAttachment)->disposition), "%s", rowstruct->colvals[5]);
	asprintf(&((*emailAttachment)->text), "%s", rowstruct->colvals[6]);
}

bmd_db_row_struct_free( &rowstruct );
bmd_db_result_free( &result );
fetched = 0;

bmd_db_disconnect(&dbase_handler);
 
return SOAP_OK;

}




