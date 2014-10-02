#include <bmd/libbmdlog/libbmdlog.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>
#include <bmd/libbmdglobals/libbmdglobals.h>
#include <bmd/libbmddb/libbmddb.h>
#include <bmd/libbmdlog/log_server_utils/log_server_utils.h>
#include <bmd/libbmderr/libbmderr.h>
#include "logH.h"


int log__verifyLog(struct soap* soap, long logNode, long* result)
{

/* 	---------------------------------------	 */

	long 	status			=    0;
	char*	dbHost			= NULL;
	char*	dbPort			= NULL;
	char*	dbName			= NULL;
	char*	dbLibrary		= NULL;
	char*	dbUser			= NULL;
	char*	dbPasswd		= NULL;
	void*	connectionHandler	= NULL;
	long	hashNodeId		=    0;
	long	parenthashNodeId 	=    0;
	char*	stringTime		= NULL;

/*	---------------------------------------	*/

	status = bmdconf_get_value(_GLOBAL_logSoapKonfiguracja,"soap","db_host",&dbHost);
	if (status<BMD_OK)
	{
		PRINT_FATAL("SOAPLOGSERVERFATAL %s\n",GetErrorMsg(BMDSOAP_SERVER_CONF_READ_ERROR));
		return soap_receiver_fault(soap, GetErrorMsg(BMDSOAP_SERVER_CONF_READ_ERROR), \
			"Maybe disk read error ");
	}

	status = bmdconf_get_value(_GLOBAL_logSoapKonfiguracja,"soap","db_port",&dbPort);
	if (status<BMD_OK)
	{
		PRINT_FATAL("SOAPLOGSERVERFATAL %s\n",GetErrorMsg(BMDSOAP_SERVER_CONF_READ_ERROR));
		return soap_receiver_fault(soap, GetErrorMsg(BMDSOAP_SERVER_CONF_READ_ERROR), \
			"Maybe disk read error ");
	}

	status = bmdconf_get_value(_GLOBAL_logSoapKonfiguracja,"soap","db_name",&dbName);
	if (status<BMD_OK)
	{
		PRINT_FATAL("SOAPLOGSERVERFATAL %s\n",GetErrorMsg(BMDSOAP_SERVER_CONF_READ_ERROR));
		return soap_receiver_fault(soap, GetErrorMsg(BMDSOAP_SERVER_CONF_READ_ERROR), \
			"Maybe disk read error ");
	}

	status = bmdconf_get_value(_GLOBAL_logSoapKonfiguracja,"soap","db_library",&dbLibrary);
	if (status<BMD_OK)
	{
		PRINT_FATAL("SOAPLOGSERVERFATAL %s\n",GetErrorMsg(BMDSOAP_SERVER_CONF_READ_ERROR));
		return soap_receiver_fault(soap, GetErrorMsg(BMDSOAP_SERVER_CONF_READ_ERROR), \
			"Maybe disk read error ");
	}

	status = bmdconf_get_value(_GLOBAL_logSoapKonfiguracja,"soap","db_user",&dbUser);
	if (status<BMD_OK)
	{
		PRINT_FATAL("SOAPLOGSERVERFATAL %s\n",GetErrorMsg(BMDSOAP_SERVER_CONF_READ_ERROR));
		return soap_receiver_fault(soap, GetErrorMsg(BMDSOAP_SERVER_CONF_READ_ERROR), \
			"Maybe disk read error ");
	}

	status = bmdconf_get_value(_GLOBAL_logSoapKonfiguracja,"soap","db_passwd",&dbPasswd);
	if (status<BMD_OK)
	{
		PRINT_FATAL("SOAPLOGSERVERFATAL %s\n",GetErrorMsg(BMDSOAP_SERVER_CONF_READ_ERROR));
		return soap_receiver_fault(soap, GetErrorMsg(BMDSOAP_SERVER_CONF_READ_ERROR), \
			"Maybe disk read error ");
	}

	status = bmd_db_init(dbLibrary);
	if (status != BMD_OK)
	{
		PRINT_FATAL("SOAPLOGSERVERFATAL Database library init failed %d\n", BMD_ERR_OP_FAILED);
		return soap_receiver_fault(soap, "Database library init failed", \
				"Maybe database server not running");
	}

	status = bmd_db_connect2(dbHost, dbPort, dbName, dbUser, dbPasswd, &connectionHandler);

	if (status != BMD_OK)
	{
		PRINT_FATAL("SOAPLOGSERVERFATAL Database connection error %d\n", BMD_ERR_OP_FAILED);
		return soap_receiver_fault(soap, "Database connection failed", \
				"Maybe database server not running");
	}

	status = bmdLogVerifyLogNode( connectionHandler, logNode );

	switch( status )
	{
		case BMD_LOG_HASH_EQUAL : { break; }

		case BMD_LOG_HASH_DIFFRENT : { 
			*result = BMD_LOG_MODIFICATION; 
			goto EXIT;
		}

		default : {
			*result = BMD_LOG_VERIFICATION_FAILED;
			goto EXIT;
		}
	}

	status = bmdLogVerifyHashNodeLevel0( connectionHandler, logNode, &hashNodeId );

	switch( status )
	{
		case BMD_LOG_HASH_EQUAL	: { break; }

		case BMD_LOG_HASH_DIFFRENT : {
			*result = BMD_LOG_HASH_L0_MODIFICATION;
			goto EXIT;
		}

		default : {
			*result = BMD_LOG_VERIFICATION_FAILED;
			goto EXIT;
		}
	}

	status = bmdLogVerifyHashNodeHigherLevel( connectionHandler, hashNodeId, &parenthashNodeId );

	switch( status )
	{
		case BMD_LOG_HASH_EQUAL : { break; }

		case BMD_LOG_HASH_DIFFRENT  : {
			*result = BMD_LOG_OTHER_HASH_LEVEL_MODIFICATION;
			goto EXIT;
		}

		default : {
			*result = BMD_LOG_VERIFICATION_FAILED;
			goto EXIT;
		}
	}

 	*result = BMD_LOG_VERIFICATION_SUCCESS;

EXIT :


 /* ---------------------------------------------------------------------------------------- */
 /*   Zapis do bazy statusu weryfikacji logu                                                 */
 /* ---------------------------------------------------------------------------------------- */

   getTime(&stringTime);

   setVerificationStatus(connectionHandler, setLogStatus, *result, 0, logNode, stringTime );

 /* --------------------------------------------------------------------------------------- */

 free(stringTime);

 bmd_db_disconnect(&connectionHandler);

 bmd_db_end();


 return SOAP_OK;
}


int log__verifyTree(struct soap* soap, long signNode, long* result)
{

/* 	---------------------------------------	 */

	long 	status			=    0;
	char*	dbHost			= NULL;
	char*	dbPort			= NULL;
	char*	dbName			= NULL;
	char*	dbLibrary		= NULL;
	char*	dbUser			= NULL;
	char*	dbPasswd		= NULL;
	void*	connectionHandler	= NULL;

/*	---------------------------------------	*/


	status = bmdconf_get_value(_GLOBAL_logSoapKonfiguracja,"soap","db_host",&dbHost);
	if (status<BMD_OK)
	{
		PRINT_FATAL("SOAPLOGSERVERFATAL %s\n",GetErrorMsg(BMDSOAP_SERVER_CONF_READ_ERROR));
		return soap_receiver_fault(soap, GetErrorMsg(BMDSOAP_SERVER_CONF_READ_ERROR), \
			"Maybe disk read error ");
	}

	status = bmdconf_get_value(_GLOBAL_logSoapKonfiguracja,"soap","db_port",&dbPort);
	if (status<BMD_OK)
	{
		PRINT_FATAL("SOAPLOGSERVERFATAL %s\n",GetErrorMsg(BMDSOAP_SERVER_CONF_READ_ERROR));
		return soap_receiver_fault(soap, GetErrorMsg(BMDSOAP_SERVER_CONF_READ_ERROR), \
			"Maybe disk read error ");
	}

	status = bmdconf_get_value(_GLOBAL_logSoapKonfiguracja,"soap","db_name",&dbName);
	if (status<BMD_OK)
	{
		PRINT_FATAL("SOAPLOGSERVERFATAL %s\n",GetErrorMsg(BMDSOAP_SERVER_CONF_READ_ERROR));
		return soap_receiver_fault(soap, GetErrorMsg(BMDSOAP_SERVER_CONF_READ_ERROR), \
			"Maybe disk read error ");
	}

	status = bmdconf_get_value(_GLOBAL_logSoapKonfiguracja,"soap","db_library",&dbLibrary);
	if (status<BMD_OK)
	{
		PRINT_FATAL("SOAPLOGSERVERFATAL %s\n",GetErrorMsg(BMDSOAP_SERVER_CONF_READ_ERROR));
		return soap_receiver_fault(soap, GetErrorMsg(BMDSOAP_SERVER_CONF_READ_ERROR), \
			"Maybe disk read error ");
	}

	status = bmdconf_get_value(_GLOBAL_logSoapKonfiguracja,"soap","db_user",&dbUser);
	if (status<BMD_OK)
	{
		PRINT_FATAL("SOAPLOGSERVERFATAL %s\n",GetErrorMsg(BMDSOAP_SERVER_CONF_READ_ERROR));
		return soap_receiver_fault(soap, GetErrorMsg(BMDSOAP_SERVER_CONF_READ_ERROR), \
			"Maybe disk read error ");
	}

	status = bmdconf_get_value(_GLOBAL_logSoapKonfiguracja,"soap","db_passwd",&dbPasswd);
	if (status<BMD_OK)
	{
		PRINT_FATAL("SOAPLOGSERVERFATAL %s\n",GetErrorMsg(BMDSOAP_SERVER_CONF_READ_ERROR));
		return soap_receiver_fault(soap, GetErrorMsg(BMDSOAP_SERVER_CONF_READ_ERROR), \
			"Maybe disk read error ");
	}

	status = bmd_db_init(dbLibrary);
	if (status != BMD_OK)
	{
		PRINT_FATAL("SOAPLOGSERVERFATAL Database library init failed %d\n", BMD_ERR_OP_FAILED);
		return soap_receiver_fault(soap, "Database library init failed", \
				"Maybe database server not running");
	}

	status = bmd_db_connect2(dbHost, dbPort, dbName, dbUser, dbPasswd, &connectionHandler);

	if (status != BMD_OK)
	{
		PRINT_FATAL("SOAPLOGSERVERFATAL Database connection error %d\n", BMD_ERR_OP_FAILED);
		return soap_receiver_fault(soap, "Database connection failed", \
				"Maybe database server not running");
	}

	
	status = bmdLogCheckSubtree( connectionHandler, signNode, (depth_t)subtree );

	bmd_db_disconnect(&connectionHandler);

	bmd_db_end();


    return SOAP_OK;
}

