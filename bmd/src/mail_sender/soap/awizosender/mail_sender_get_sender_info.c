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




int ns__awizoGetSenderInfo(struct soap* soap, long id, struct ns__senderComplex** sender )
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



PRINT_INFO("AWIZOSOAPSERVERINF Get Information of Awizo Sender.\n");

if (_GLOBAL_shptr->awizoAction != queueNothing) {
	PRINT_INFO("AWIZOSOAPSERVERINF Can't get information about awizo sender, server already in use!\n" );
	return soap_receiver_fault(soap, "Can't get information of awizo sender from database" , "Awizo serwer already in use");
}

(*sender)=(struct ns__senderComplex*)soap_malloc(soap, sizeof(struct ns__senderComplex));

if((*sender) == NULL) {
	PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
	return soap_receiver_fault(soap, "Memory allocation failed", NULL);
}

memset(*sender,0,sizeof(struct ns__senderComplex));


if ( asprintf( &stringSQL, "SELECT name, email, created, deleted, smtpaddr, smtpport, smtpuser, smtppassword, pfxfile, pfxpin, maximagesize, csvdir, csvseparator, awizoaction, allowimageabsent, smsapiurl, smsapierror FROM mailing_message_senders WHERE id = %ld;", id) < 0) {
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
	asprintf(&((*sender)->name), "%s", rowstruct->colvals[0]); 
	asprintf(&((*sender)->email), "%s", rowstruct->colvals[1]);
	asprintf(&((*sender)->created), "%s", rowstruct->colvals[2]);
	asprintf(&((*sender)->deleted), "%s", rowstruct->colvals[3]);
	asprintf(&((*sender)->smtpaddr), "%s", rowstruct->colvals[4]);
	asprintf(&((*sender)->smtpport), "%s", rowstruct->colvals[5]);
	asprintf(&((*sender)->smtpuser), "%s", rowstruct->colvals[6]);
	asprintf(&((*sender)->smtppassword), "%s", rowstruct->colvals[7]);
	asprintf(&((*sender)->pfxfile), "%s", rowstruct->colvals[8]);
	asprintf(&((*sender)->pfxpin), "%s", rowstruct->colvals[9]);
	asprintf(&((*sender)->maximagesize), "%s", rowstruct->colvals[10]);
	asprintf(&((*sender)->csvdir), "%s", rowstruct->colvals[11]);
	asprintf(&((*sender)->csvseparator), "%s", rowstruct->colvals[12]);
	(*sender)->awizoaction = atol(rowstruct->colvals[13]);
	(*sender)->allowimageabsent = atol(rowstruct->colvals[14]);
	asprintf(&((*sender)->smsapiurl), "%s", rowstruct->colvals[15]);
	asprintf(&((*sender)->smsapierror), "%s", rowstruct->colvals[16]);
}

bmd_db_row_struct_free( &rowstruct );
bmd_db_result_free( &result );
fetched = 0;

bmd_db_disconnect(&dbase_handler);
 
return SOAP_OK;


}
