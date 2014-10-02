#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/dir.h>
#include <fcntl.h>
#include <bmd/mail_sender/attachment.h>
#include <bmd/libbmddb/libbmddb.h>
#include <bmd/mail_sender/cache.h>
#include <bmd/libbmdglobals/libbmdglobals.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>
#include <bmd/mail_sender/queue.h>
#include <bmd/mail_sender/mail_sender.h>
#include "soapH.h"
#include <bmd/libbmderr/libbmderr.h>



/**********************************************/
/*  Żądanie pobrania list nadawców            */
/**********************************************/


int ns__changeAdviceDispatchSetting(struct soap* soap, long dispatchSettingId, struct ns__awizoMessage* message, struct ns__dateTimeComplexList *dateTime, long* result) {


/* ------------------------------------- */


    char* stringSQL              = NULL;
    void* dbase_handler          = NULL;
    db_row_strings_t*  rowstruct = NULL;
    int   intRet                 =    0;
    long  fetched                =    0;
    long  rows                   =    0;
    long  cols                   =    0;
    void* dbResult               = NULL;
    int   i                      =    0;
    char* idNextValue		 = NULL;


/* ------------------------------------- */





  PRINT_INFO("AWIZOSOAPSERVERINF Change advice dispatch settings.\n");

  if (_GLOBAL_shptr->awizoAction != queueNothing) {
	PRINT_INFO("AWIZOSOAPSERVERINF Can't add advice dispatch settings, serwer already in use !\n" );
	return soap_receiver_fault(soap, "Awizo serwer already in use", NULL);
  }






  intRet = bmd_db_connect2(_GLOBAL_awizoConfig.dbIP, \
				 _GLOBAL_awizoConfig.dbPort, \
				 _GLOBAL_awizoConfig.dbName, \
				 _GLOBAL_awizoConfig.dbUser, \
				 _GLOBAL_awizoConfig.dbPassword, \
				  &dbase_handler);
 
  if ( intRet < 0 ){
	return soap_receiver_fault(soap, "Database connection failed", "Maybe database server does not work");
  }


  if (message != NULL) {
	asprintf(&stringSQL, "UPDATE buffer SET message = '%s' WHERE id=%ld;", message->value->__ptr, dispatchSettingId);

	intRet = ExecuteSQLQueryWithAnswerKnownDBConnection(dbase_handler, stringSQL, &idNextValue);                                                               
	free0(stringSQL);
/*	
	if (intRet != BMD_OK) {
    	    PRINT_ERROR("Update message operation failed. Error = %d\n", BMD_ERR_OP_FAILED);
	    return soap_receiver_fault(soap, "Update message operation failed", NULL);
	}
*/
  }


  if (dateTime != NULL) {
	asprintf(&stringSQL, "UPDATE buffer SET date_time = '%s' WHERE id=%ld;", dateTime->__ptr[0].dateTime, dispatchSettingId);

	intRet = ExecuteSQLQueryWithAnswerKnownDBConnection(dbase_handler, stringSQL, &idNextValue);
	free0(stringSQL);
/*	
	if (intRet != BMD_OK) {                                                                                                                              
	    PRINT_ERROR("Update datetime operation failed. Error = %d\n", BMD_ERR_OP_FAILED);
	    return soap_receiver_fault(soap, "Update datetime  operation failed", NULL); 
        }
*/
  }


 bmd_db_disconnect(&dbase_handler);

 *result = dispatchSettingId;

 return SOAP_OK;

}

