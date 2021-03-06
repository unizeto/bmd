/* soapServer.c
   Generated by gSOAP 2.7.9f from logsoap.h
   Copyright(C) 2000-2006, Robert van Engelen, Genivia Inc. All Rights Reserved.
   This part of the software is released under one of the following licenses:
   GPL, the gSOAP public license, or Genivia's license for commercial use.
*/
#include "soapH.h"
#ifdef __cplusplus
extern "C" {
#endif

SOAP_SOURCE_STAMP("@(#) soapServer.c ver 2.7.9f 2010-02-17 11:37:47 GMT")


SOAP_FMAC5 int SOAP_FMAC6 soap_serve(struct soap *soap)
{
#ifndef WITH_FASTCGI
	unsigned int k = soap->max_keep_alive;
#endif

	do
	{
#ifdef WITH_FASTCGI
		if (FCGI_Accept() < 0)
		{
			soap->error = SOAP_EOF;
			return soap_send_fault(soap);
		}
#endif

		soap_begin(soap);

#ifndef WITH_FASTCGI
		if (soap->max_keep_alive > 0 && !--k)
			soap->keep_alive = 0;
#endif

		if (soap_begin_recv(soap))
		{	if (soap->error < SOAP_STOP)
			{
#ifdef WITH_FASTCGI
				soap_send_fault(soap);
#else 
				return soap_send_fault(soap);
#endif
			}
			soap_closesock(soap);

			continue;
		}

		if (soap_envelope_begin_in(soap)
		 || soap_recv_header(soap)
		 || soap_body_begin_in(soap)
		 || soap_serve_request(soap)
		 || (soap->fserveloop && soap->fserveloop(soap)))
		{
#ifdef WITH_FASTCGI
			soap_send_fault(soap);
#else
			return soap_send_fault(soap);
#endif
		}

#ifdef WITH_FASTCGI
	} while (1);
#else
	} while (soap->keep_alive);
#endif
	return SOAP_OK;
}

#ifndef WITH_NOSERVEREQUEST
SOAP_FMAC5 int SOAP_FMAC6 soap_serve_request(struct soap *soap)
{
	soap_peek_element(soap);
	if (!soap_match_tag(soap, soap->tag, "ns:logServerSend"))
		return soap_serve_ns__logServerSend(soap);
	if (!soap_match_tag(soap, soap->tag, "ns:verifyLog"))
		return soap_serve_ns__verifyLog(soap);
	if (!soap_match_tag(soap, soap->tag, "ns:verifyTree"))
		return soap_serve_ns__verifyTree(soap);
	return soap->error = SOAP_NO_METHOD;
}
#endif

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_ns__logServerSend(struct soap *soap)
{	struct ns__logServerSend soap_tmp_ns__logServerSend;
	struct ns__logServerSendResponse soap_tmp_ns__logServerSendResponse;
	long soap_tmp_long;
	soap_default_ns__logServerSendResponse(soap, &soap_tmp_ns__logServerSendResponse);
	soap_default_long(soap, &soap_tmp_long);
	soap_tmp_ns__logServerSendResponse.result = &soap_tmp_long;
	soap_default_ns__logServerSend(soap, &soap_tmp_ns__logServerSend);
	soap->encodingStyle = NULL;
	if (!soap_get_ns__logServerSend(soap, &soap_tmp_ns__logServerSend, "ns:logServerSend", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = ns__logServerSend(soap, soap_tmp_ns__logServerSend.logDatagram, &soap_tmp_long);
	if (soap->error)
		return soap->error;
	soap_serializeheader(soap);
	soap_serialize_ns__logServerSendResponse(soap, &soap_tmp_ns__logServerSendResponse);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put_ns__logServerSendResponse(soap, &soap_tmp_ns__logServerSendResponse, "ns:logServerSendResponse", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	};
	if (soap_end_count(soap)
	 || soap_response(soap, SOAP_OK)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put_ns__logServerSendResponse(soap, &soap_tmp_ns__logServerSendResponse, "ns:logServerSendResponse", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap->error;
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_ns__verifyLog(struct soap *soap)
{	struct ns__verifyLog soap_tmp_ns__verifyLog;
	struct ns__verifyLogResponse soap_tmp_ns__verifyLogResponse;
	long soap_tmp_long;
	soap_default_ns__verifyLogResponse(soap, &soap_tmp_ns__verifyLogResponse);
	soap_default_long(soap, &soap_tmp_long);
	soap_tmp_ns__verifyLogResponse.result = &soap_tmp_long;
	soap_default_ns__verifyLog(soap, &soap_tmp_ns__verifyLog);
	soap->encodingStyle = NULL;
	if (!soap_get_ns__verifyLog(soap, &soap_tmp_ns__verifyLog, "ns:verifyLog", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = ns__verifyLog(soap, soap_tmp_ns__verifyLog.logNode, &soap_tmp_long);
	if (soap->error)
		return soap->error;
	soap_serializeheader(soap);
	soap_serialize_ns__verifyLogResponse(soap, &soap_tmp_ns__verifyLogResponse);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put_ns__verifyLogResponse(soap, &soap_tmp_ns__verifyLogResponse, "ns:verifyLogResponse", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	};
	if (soap_end_count(soap)
	 || soap_response(soap, SOAP_OK)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put_ns__verifyLogResponse(soap, &soap_tmp_ns__verifyLogResponse, "ns:verifyLogResponse", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap->error;
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_ns__verifyTree(struct soap *soap)
{	struct ns__verifyTree soap_tmp_ns__verifyTree;
	struct ns__verifyTreeResponse soap_tmp_ns__verifyTreeResponse;
	long soap_tmp_long;
	soap_default_ns__verifyTreeResponse(soap, &soap_tmp_ns__verifyTreeResponse);
	soap_default_long(soap, &soap_tmp_long);
	soap_tmp_ns__verifyTreeResponse.result = &soap_tmp_long;
	soap_default_ns__verifyTree(soap, &soap_tmp_ns__verifyTree);
	soap->encodingStyle = NULL;
	if (!soap_get_ns__verifyTree(soap, &soap_tmp_ns__verifyTree, "ns:verifyTree", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = ns__verifyTree(soap, soap_tmp_ns__verifyTree.signNode, &soap_tmp_long);
	if (soap->error)
		return soap->error;
	soap_serializeheader(soap);
	soap_serialize_ns__verifyTreeResponse(soap, &soap_tmp_ns__verifyTreeResponse);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put_ns__verifyTreeResponse(soap, &soap_tmp_ns__verifyTreeResponse, "ns:verifyTreeResponse", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	};
	if (soap_end_count(soap)
	 || soap_response(soap, SOAP_OK)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put_ns__verifyTreeResponse(soap, &soap_tmp_ns__verifyTreeResponse, "ns:verifyTreeResponse", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap->error;
	return soap_closesock(soap);
}

#ifdef __cplusplus
}
#endif

/* End of soapServer.c */
