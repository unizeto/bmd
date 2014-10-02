#include <bmd/libbmdlog/libbmdlog.h>
#include <bmd/libbmdasn1_common/CommonUtils.h>
#include <bmd/libbmdglobals/libbmdglobals.h>
#include "logH.h"


int log__logServerSend(	struct soap* soap, struct log__logDatagram *logDatagram, long* result)
{


	err_t  err_tRet = 0;

	err_tRet = LogCreate(_GLOBAL_soaplog_HANDLER);

	if ( BMD_ERR( err_tRet ))
	{
		BMD_BTERR( err_tRet );
		BMD_FREEERR( err_tRet );

		return soap_receiver_fault(soap, "Log create failed", "Maybe no log server connection");
	}

	if ( logDatagram->remoteHost )
	{
		err_tRet = LogSetParam( remote_host, logDatagram->remoteHost, _GLOBAL_soaplog_HANDLER );

		if ( BMD_ERR( err_tRet ))
		{
			BMD_BTERR( err_tRet );
			BMD_FREEERR( err_tRet );
		
			return soap_receiver_fault(soap, "Remote host save failed", "Not enough memory");			
		}
	}

	if ( logDatagram->remotePort )
	{
		err_tRet = LogSetParam( remote_port, logDatagram->remotePort, _GLOBAL_soaplog_HANDLER );

		if ( BMD_ERR( err_tRet ))
		{
			BMD_BTERR( err_tRet );
			BMD_FREEERR( err_tRet );
		
			return soap_receiver_fault(soap, "Remote port save failed", "Not enough memory");
		}
	}

	if ( logDatagram->srcFile )
	{
		err_tRet = LogSetParam( src_file, logDatagram->srcFile, _GLOBAL_soaplog_HANDLER );

		if ( BMD_ERR( err_tRet ))
		{
			BMD_BTERR( err_tRet );
			BMD_FREEERR( err_tRet );
		
			return soap_receiver_fault(soap, "Source file save failed", "Not enough memory");
		}
	}

	if ( logDatagram->srcLine )
	{
		err_tRet = LogSetParam( src_line, logDatagram->srcLine, _GLOBAL_soaplog_HANDLER );

		if ( BMD_ERR( err_tRet ))
		{
			BMD_BTERR( err_tRet );
			BMD_FREEERR( err_tRet );
		
			return soap_receiver_fault(soap, "Source line save failed", "Not enough memory");
		}
	}

	if ( logDatagram->srcFunction )
	{
		err_tRet = LogSetParam( src_function, logDatagram->srcFunction, _GLOBAL_soaplog_HANDLER );

		if ( BMD_ERR( err_tRet ))
		{
			BMD_BTERR( err_tRet );
			BMD_FREEERR( err_tRet );
		
			return soap_receiver_fault(soap, "Source function save failed", "Not enough memory");
		}
	}



	if ( logDatagram->logOwner )
	{
		err_tRet = LogSetParam( log_owner, logDatagram->logOwner, _GLOBAL_soaplog_HANDLER );

		if ( BMD_ERR( err_tRet ))
		{
			BMD_BTERR( err_tRet );
			BMD_FREEERR( err_tRet );

			return soap_receiver_fault(soap, "Log owner save failed", "Not enough memory");
		}
	}

	if ( logDatagram->service )
	{
		err_tRet = LogSetParam( service, logDatagram->service, _GLOBAL_soaplog_HANDLER );

		if ( BMD_ERR( err_tRet ))
		{
			BMD_BTERR( err_tRet );
			BMD_FREEERR( err_tRet );
		
			return soap_receiver_fault(soap, "Service save failed", "Not enough memory");
		}
	}

	if ( logDatagram->operationType )
	{
		err_tRet = LogSetParam( operation_type, logDatagram->operationType, _GLOBAL_soaplog_HANDLER );

		if ( BMD_ERR( err_tRet ))
		{
			BMD_BTERR( err_tRet );
			BMD_FREEERR( err_tRet );
		
			return soap_receiver_fault(soap, "Operation type save failed", "Not enough memory");
		}
	}	

	if ( logDatagram->logLevel )
	{
		err_tRet = LogSetParam( log_level, logDatagram->logLevel, _GLOBAL_soaplog_HANDLER );

		if ( BMD_ERR( err_tRet ))
		{
			BMD_BTERR( err_tRet );
			BMD_FREEERR( err_tRet );
		
			return soap_receiver_fault(soap, "Log level save failed", "Not enough memory");
		}
	}

	if ( logDatagram->dateTimeBegin )
	{
		err_tRet = LogSetParam( date_time_begin, logDatagram->dateTimeBegin, _GLOBAL_soaplog_HANDLER );

		if ( BMD_ERR( err_tRet ))
		{
			BMD_BTERR( err_tRet );
			BMD_FREEERR( err_tRet );
		
			return soap_receiver_fault(soap, "Date time begin save failed", "Not enough memory");
		}
	}

	if ( logDatagram->logString )
	{
		err_tRet = LogSetParam( log_string, logDatagram->logString, _GLOBAL_soaplog_HANDLER );

		if ( BMD_ERR( err_tRet ))
		{
			BMD_BTERR( err_tRet );
			BMD_FREEERR( err_tRet );
		
			return soap_receiver_fault(soap, "Log string save failed", "Not enough memory");
		}
	}

	if ( logDatagram->logReason )
	{
		err_tRet = LogSetParam( log_reason, logDatagram->logReason, _GLOBAL_soaplog_HANDLER );

		if ( BMD_ERR( err_tRet ))
		{
			BMD_BTERR( err_tRet );
			BMD_FREEERR( err_tRet );
		
			return soap_receiver_fault(soap, "Log reason save failed", "Not enough memory");
		}
	}

	if ( logDatagram->logSolution )
	{
		err_tRet = LogSetParam( log_solution, logDatagram->logSolution, _GLOBAL_soaplog_HANDLER );

		if ( BMD_ERR( err_tRet ))
		{
			BMD_BTERR( err_tRet );
			BMD_FREEERR( err_tRet );
		
			return soap_receiver_fault(soap, "Log solution save failed", "Not enough memory");
		}
	}

	if ( logDatagram->documentSize )
	{
		err_tRet = LogSetParam( document_size, logDatagram->documentSize, _GLOBAL_soaplog_HANDLER );

		if ( BMD_ERR( err_tRet ))
		{
			BMD_BTERR( err_tRet );
			BMD_FREEERR( err_tRet );
		
			return soap_receiver_fault(soap, "Document size save failed", "Not enough memory");
		}
	}

	if ( logDatagram->documentFilename )
	{
		err_tRet = LogSetParam( document_filename, logDatagram->documentFilename, _GLOBAL_soaplog_HANDLER );

		if ( BMD_ERR( err_tRet ))
		{
			BMD_BTERR( err_tRet );
			BMD_FREEERR( err_tRet );
		
			return soap_receiver_fault(soap, "Document file name save failed", "Not enough memory");
		}
	}

	if ( logDatagram->logReferto )
	{
		err_tRet = LogSetParam( log_referto, logDatagram->logReferto, _GLOBAL_soaplog_HANDLER );

		if ( BMD_ERR( err_tRet ))
		{
			BMD_BTERR( err_tRet );
			BMD_FREEERR( err_tRet );
		
			return soap_receiver_fault(soap, "Log refer to save failed", "Not enough memory");
		}
	}

	if ( logDatagram->eventCode )
	{
		err_tRet = LogSetParam( event_code, logDatagram->eventCode, _GLOBAL_soaplog_HANDLER );

		if ( BMD_ERR( err_tRet ))
		{
			BMD_BTERR( err_tRet );
			BMD_FREEERR( err_tRet );
		
			return soap_receiver_fault(soap, "Event code save failed", "Not enough memory");
		}
	}

	if ( logDatagram->logOwnerEventVisible )
	{
		err_tRet = LogSetParam( log_owner_event_visible, logDatagram->logOwnerEventVisible, _GLOBAL_soaplog_HANDLER );

		if ( BMD_ERR( err_tRet ))
		{
			BMD_BTERR( err_tRet );
			BMD_FREEERR( err_tRet );
		
			return soap_receiver_fault(soap, "Log owner event visible save failed ", "Not enough memory");
		}
	}

	if ( logDatagram->logRefertoEventVisible )
	{
		err_tRet = LogSetParam( log_referto_event_visible, logDatagram->logRefertoEventVisible, _GLOBAL_soaplog_HANDLER );

		if ( BMD_ERR( err_tRet ))
		{
			BMD_BTERR( err_tRet );
			BMD_FREEERR( err_tRet );
		
			return soap_receiver_fault(soap, "Log refer to event visible save failed ", "Not enough memory");
		}
	}

        err_tRet = LogCommit(_GLOBAL_soaplog_HANDLER);

	if ( BMD_ERR( err_tRet ))
	{
		BMD_BTERR( err_tRet );
		BMD_FREEERR( err_tRet );

		return soap_receiver_fault(soap, "Log save to database failed", "Maybe no log server connection");
	}


    return SOAP_OK;
}


