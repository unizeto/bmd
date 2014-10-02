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




/**********************************************/
/*  Żądanie pobrania list nadawców            */
/**********************************************/


int ns__awizoGetSendersList(struct soap* soap, struct ns__senderComplexList **senderList) {


/* ------------------------------------- */

    char* stringSQL              = NULL;
    char* allSenders             = NULL;
    void* dbase_handler          = NULL;
    db_row_strings_t*  rowstruct = NULL;
    int   intRet                 =    0;
    long  fetched                =    0;
    long  rows                   =    0;
    long  cols                   =    0;
    void* result                 = NULL;
    int   i                      =    0;

/* ------------------------------------- */


  PRINT_INFO("AWIZOSOAPSERVERINF Get awizo senders list.\n" );

  if (_GLOBAL_shptr->awizoAction != queueNothing) {
	PRINT_INFO("AWIZOSOAPSERVERINF Can't get awizo senders, serwer already in use !\n" );
	return soap_receiver_fault(soap, "Awizo serwer already in use", NULL);
 }

  (*senderList)=(struct ns__senderComplexList*)soap_malloc(soap,sizeof(struct ns__senderComplexList));

  if((*senderList) == NULL) {
	PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
	return soap_receiver_fault(soap, "Memory allocation failed", NULL);
  }

  memset(*senderList,0,sizeof(struct ns__senderComplexList));



  intRet = bmd_db_connect2(_GLOBAL_awizoConfig.dbIP, \
				 _GLOBAL_awizoConfig.dbPort, \
				 _GLOBAL_awizoConfig.dbName, \
				 _GLOBAL_awizoConfig.dbUser, \
				 _GLOBAL_awizoConfig.dbPassword, \
				  &dbase_handler);
 
  if ( intRet < 0 ){
	return soap_receiver_fault(soap, "Database connection failed", "Maybe database server does not work");
  }


  if (asprintf( &stringSQL, "SELECT count(*) FROM mailing_message_senders;") < 0) {
	PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
	return soap_receiver_fault(soap, "Memory allocation failed", NULL);
  }

  intRet = ExecuteSQLQueryWithAnswerKnownDBConnection(dbase_handler, stringSQL, &allSenders);
  free0(stringSQL);

  if (intRet != BMD_OK) {
	PRINT_ERROR("Nieprawidłowa liczba nadawców. Error = %d\n", BMD_ERR_OP_FAILED);
	return soap_receiver_fault(soap, "Select operation failed", NULL);
  }

  if ( asprintf( &stringSQL, "SELECT id,name,email,created,deleted,smtpaddr,smtpport,smtpuser,smtppassword,pfxfile,pfxpin,maximagesize,csvdir,csvseparator,awizoaction,allowimageabsent,smsapiurl,smsapierror FROM mailing_message_senders;") < 0) {
	PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
	return soap_receiver_fault(soap, "Memory allocation failed", NULL);
  }

  if ( bmd_db_execute_sql( dbase_handler, stringSQL, &rows, &cols, &result) < 0 ) {
	PRINT_ERROR("Błąd zapytania do bazy danych. Error = %d\n", BMD_ERR_OP_FAILED);
	free0(stringSQL);
	return soap_receiver_fault(soap, "SQL request failed", NULL);
  } 

 free0(stringSQL);


 /* **************************************** */
 /* Alokacja pamięci dla struktur odpowiedzi */
 /* **************************************** */

  (*senderList)->__size=atoi(allSenders);


  (*senderList)->__ptr=(struct ns__senderComplex*)soap_malloc(soap, sizeof(struct ns__senderComplex)*(*senderList)->__size);

  if((*senderList)->__ptr == NULL ) {
    	PRINT_ERROR("Błąd alokacji pamięci. Error = %d\n", BMD_ERR_MEMORY);
	return soap_receiver_fault(soap, "Memory allocation failed", NULL);
  }


  i=0;

  do {
	intRet = bmd_db_result_get_row(dbase_handler, result, 0, 0, &rowstruct, FETCH_NEXT, &fetched );
	
	if (intRet == 0) {
		(*senderList)->__ptr[i].id = atoi(rowstruct->colvals[0]);
 		asprintf(&((*senderList)->__ptr[i].name), "%s", rowstruct->colvals[1]);
		asprintf(&((*senderList)->__ptr[i].email),"%s",rowstruct->colvals[2]);
		asprintf(&((*senderList)->__ptr[i].created),"%s",rowstruct->colvals[3]);
		asprintf(&((*senderList)->__ptr[i].deleted),"%s",rowstruct->colvals[4]);
		asprintf(&((*senderList)->__ptr[i].smtpaddr),"%s",rowstruct->colvals[5]);
		asprintf(&((*senderList)->__ptr[i].smtpport),"%s",rowstruct->colvals[6]);
		asprintf(&((*senderList)->__ptr[i].smtpuser),"%s",rowstruct->colvals[7]);
		asprintf(&((*senderList)->__ptr[i].smtppassword),"%s",rowstruct->colvals[8]);
		asprintf(&((*senderList)->__ptr[i].pfxfile),"%s",rowstruct->colvals[9]);
		asprintf(&((*senderList)->__ptr[i].pfxpin),"%s",rowstruct->colvals[10]);
		asprintf(&((*senderList)->__ptr[i].maximagesize),"%s",rowstruct->colvals[11]);
		asprintf(&((*senderList)->__ptr[i].csvdir),"%s",rowstruct->colvals[12]);
		asprintf(&((*senderList)->__ptr[i].csvseparator),"%s",rowstruct->colvals[13]);
		(*senderList)->__ptr[i].awizoaction = atoi(rowstruct->colvals[14]);
		(*senderList)->__ptr[i].allowimageabsent = atoi(rowstruct->colvals[15]);
		asprintf(&((*senderList)->__ptr[i].smsapiurl),"%s",rowstruct->colvals[16]);
		asprintf(&((*senderList)->__ptr[i].smsapierror),"%s",rowstruct->colvals[17]);
		
		i+=1;

		bmd_db_row_struct_free( &rowstruct );
	}

 }while(intRet == 0);

 bmd_db_result_free( &result );
 fetched = 0;
 free0(allSenders);

 bmd_db_disconnect(&dbase_handler);


 return SOAP_OK;

}


