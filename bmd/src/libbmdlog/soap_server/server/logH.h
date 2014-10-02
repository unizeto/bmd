/* logH.h
   Generated by gSOAP 2.7.17 from logsoap.h
   Copyright(C) 2000-2010, Robert van Engelen, Genivia Inc. All Rights Reserved.
   This part of the software is released under one of the following licenses:
   GPL, the gSOAP public license, or Genivia's license for commercial use.
*/

#ifndef logH_H
#define logH_H
#include "logStub.h"
#ifdef __cplusplus
extern "C" {
#endif
#ifndef WITH_NOIDREF
SOAP_FMAC3 void SOAP_FMAC4 soap_markelement(struct soap*, const void*, int);
SOAP_FMAC3 int SOAP_FMAC4 soap_putelement(struct soap*, const void*, const char*, int, int);
SOAP_FMAC3 void *SOAP_FMAC4 soap_getelement(struct soap*, int*);
SOAP_FMAC3 int SOAP_FMAC4 soap_putindependent(struct soap*);
SOAP_FMAC3 int SOAP_FMAC4 soap_getindependent(struct soap*);
#endif
SOAP_FMAC3 int SOAP_FMAC4 soap_ignore_element(struct soap*);

#ifndef SOAP_TYPE_byte
#define SOAP_TYPE_byte (3)
#endif
SOAP_FMAC3 void SOAP_FMAC4 soap_default_byte(struct soap*, char *);
SOAP_FMAC3 int SOAP_FMAC4 soap_out_byte(struct soap*, const char*, int, const char *, const char*);
SOAP_FMAC3 char * SOAP_FMAC4 soap_in_byte(struct soap*, const char*, char *, const char*);

#define soap_write_byte(soap, data) ( soap_begin_send(soap) || soap_put_byte(soap, data, NULL, NULL) || soap_end_send(soap) )


SOAP_FMAC3 int SOAP_FMAC4 soap_put_byte(struct soap*, const char *, const char*, const char*);

#define soap_read_byte(soap, data) ( soap_begin_recv(soap) || !soap_get_byte(soap, data, NULL, NULL) || soap_end_recv(soap) )

SOAP_FMAC3 char * SOAP_FMAC4 soap_get_byte(struct soap*, char *, const char*, const char*);

#ifndef SOAP_TYPE_int
#define SOAP_TYPE_int (1)
#endif
SOAP_FMAC3 void SOAP_FMAC4 soap_default_int(struct soap*, int *);
SOAP_FMAC3 int SOAP_FMAC4 soap_out_int(struct soap*, const char*, int, const int *, const char*);
SOAP_FMAC3 int * SOAP_FMAC4 soap_in_int(struct soap*, const char*, int *, const char*);

#define soap_write_int(soap, data) ( soap_begin_send(soap) || soap_put_int(soap, data, NULL, NULL) || soap_end_send(soap) )


SOAP_FMAC3 int SOAP_FMAC4 soap_put_int(struct soap*, const int *, const char*, const char*);

#define soap_read_int(soap, data) ( soap_begin_recv(soap) || !soap_get_int(soap, data, NULL, NULL) || soap_end_recv(soap) )

SOAP_FMAC3 int * SOAP_FMAC4 soap_get_int(struct soap*, int *, const char*, const char*);

#ifndef SOAP_TYPE_long
#define SOAP_TYPE_long (10)
#endif
SOAP_FMAC3 void SOAP_FMAC4 soap_default_long(struct soap*, long *);
SOAP_FMAC3 int SOAP_FMAC4 soap_out_long(struct soap*, const char*, int, const long *, const char*);
SOAP_FMAC3 long * SOAP_FMAC4 soap_in_long(struct soap*, const char*, long *, const char*);

#define soap_write_long(soap, data) ( soap_begin_send(soap) || soap_put_long(soap, data, NULL, NULL) || soap_end_send(soap) )


SOAP_FMAC3 int SOAP_FMAC4 soap_put_long(struct soap*, const long *, const char*, const char*);

#define soap_read_long(soap, data) ( soap_begin_recv(soap) || !soap_get_long(soap, data, NULL, NULL) || soap_end_recv(soap) )

SOAP_FMAC3 long * SOAP_FMAC4 soap_get_long(struct soap*, long *, const char*, const char*);

#ifndef WITH_NOGLOBAL

#ifndef SOAP_TYPE_SOAP_ENV__Fault
#define SOAP_TYPE_SOAP_ENV__Fault (28)
#endif
SOAP_FMAC3 void SOAP_FMAC4 soap_default_SOAP_ENV__Fault(struct soap*, struct SOAP_ENV__Fault *);
SOAP_FMAC3 void SOAP_FMAC4 soap_serialize_SOAP_ENV__Fault(struct soap*, const struct SOAP_ENV__Fault *);
SOAP_FMAC3 int SOAP_FMAC4 soap_out_SOAP_ENV__Fault(struct soap*, const char*, int, const struct SOAP_ENV__Fault *, const char*);
SOAP_FMAC3 struct SOAP_ENV__Fault * SOAP_FMAC4 soap_in_SOAP_ENV__Fault(struct soap*, const char*, struct SOAP_ENV__Fault *, const char*);

#define soap_write_SOAP_ENV__Fault(soap, data) ( soap_begin_send(soap) || (soap_serialize_SOAP_ENV__Fault(soap, data), 0) || soap_put_SOAP_ENV__Fault(soap, data, NULL, NULL) || soap_end_send(soap) )


SOAP_FMAC3 int SOAP_FMAC4 soap_put_SOAP_ENV__Fault(struct soap*, const struct SOAP_ENV__Fault *, const char*, const char*);

#define soap_read_SOAP_ENV__Fault(soap, data) ( soap_begin_recv(soap) || !soap_get_SOAP_ENV__Fault(soap, data, NULL, NULL) || soap_end_recv(soap) )

SOAP_FMAC3 struct SOAP_ENV__Fault * SOAP_FMAC4 soap_get_SOAP_ENV__Fault(struct soap*, struct SOAP_ENV__Fault *, const char*, const char*);

#endif

#ifndef WITH_NOGLOBAL

#ifndef SOAP_TYPE_SOAP_ENV__Reason
#define SOAP_TYPE_SOAP_ENV__Reason (27)
#endif
SOAP_FMAC3 void SOAP_FMAC4 soap_default_SOAP_ENV__Reason(struct soap*, struct SOAP_ENV__Reason *);
SOAP_FMAC3 void SOAP_FMAC4 soap_serialize_SOAP_ENV__Reason(struct soap*, const struct SOAP_ENV__Reason *);
SOAP_FMAC3 int SOAP_FMAC4 soap_out_SOAP_ENV__Reason(struct soap*, const char*, int, const struct SOAP_ENV__Reason *, const char*);
SOAP_FMAC3 struct SOAP_ENV__Reason * SOAP_FMAC4 soap_in_SOAP_ENV__Reason(struct soap*, const char*, struct SOAP_ENV__Reason *, const char*);

#define soap_write_SOAP_ENV__Reason(soap, data) ( soap_begin_send(soap) || (soap_serialize_SOAP_ENV__Reason(soap, data), 0) || soap_put_SOAP_ENV__Reason(soap, data, NULL, NULL) || soap_end_send(soap) )


SOAP_FMAC3 int SOAP_FMAC4 soap_put_SOAP_ENV__Reason(struct soap*, const struct SOAP_ENV__Reason *, const char*, const char*);

#define soap_read_SOAP_ENV__Reason(soap, data) ( soap_begin_recv(soap) || !soap_get_SOAP_ENV__Reason(soap, data, NULL, NULL) || soap_end_recv(soap) )

SOAP_FMAC3 struct SOAP_ENV__Reason * SOAP_FMAC4 soap_get_SOAP_ENV__Reason(struct soap*, struct SOAP_ENV__Reason *, const char*, const char*);

#endif

#ifndef WITH_NOGLOBAL

#ifndef SOAP_TYPE_SOAP_ENV__Detail
#define SOAP_TYPE_SOAP_ENV__Detail (24)
#endif
SOAP_FMAC3 void SOAP_FMAC4 soap_default_SOAP_ENV__Detail(struct soap*, struct SOAP_ENV__Detail *);
SOAP_FMAC3 void SOAP_FMAC4 soap_serialize_SOAP_ENV__Detail(struct soap*, const struct SOAP_ENV__Detail *);
SOAP_FMAC3 int SOAP_FMAC4 soap_out_SOAP_ENV__Detail(struct soap*, const char*, int, const struct SOAP_ENV__Detail *, const char*);
SOAP_FMAC3 struct SOAP_ENV__Detail * SOAP_FMAC4 soap_in_SOAP_ENV__Detail(struct soap*, const char*, struct SOAP_ENV__Detail *, const char*);

#define soap_write_SOAP_ENV__Detail(soap, data) ( soap_begin_send(soap) || (soap_serialize_SOAP_ENV__Detail(soap, data), 0) || soap_put_SOAP_ENV__Detail(soap, data, NULL, NULL) || soap_end_send(soap) )


SOAP_FMAC3 int SOAP_FMAC4 soap_put_SOAP_ENV__Detail(struct soap*, const struct SOAP_ENV__Detail *, const char*, const char*);

#define soap_read_SOAP_ENV__Detail(soap, data) ( soap_begin_recv(soap) || !soap_get_SOAP_ENV__Detail(soap, data, NULL, NULL) || soap_end_recv(soap) )

SOAP_FMAC3 struct SOAP_ENV__Detail * SOAP_FMAC4 soap_get_SOAP_ENV__Detail(struct soap*, struct SOAP_ENV__Detail *, const char*, const char*);

#endif

#ifndef WITH_NOGLOBAL

#ifndef SOAP_TYPE_SOAP_ENV__Code
#define SOAP_TYPE_SOAP_ENV__Code (22)
#endif
SOAP_FMAC3 void SOAP_FMAC4 soap_default_SOAP_ENV__Code(struct soap*, struct SOAP_ENV__Code *);
SOAP_FMAC3 void SOAP_FMAC4 soap_serialize_SOAP_ENV__Code(struct soap*, const struct SOAP_ENV__Code *);
SOAP_FMAC3 int SOAP_FMAC4 soap_out_SOAP_ENV__Code(struct soap*, const char*, int, const struct SOAP_ENV__Code *, const char*);
SOAP_FMAC3 struct SOAP_ENV__Code * SOAP_FMAC4 soap_in_SOAP_ENV__Code(struct soap*, const char*, struct SOAP_ENV__Code *, const char*);

#define soap_write_SOAP_ENV__Code(soap, data) ( soap_begin_send(soap) || (soap_serialize_SOAP_ENV__Code(soap, data), 0) || soap_put_SOAP_ENV__Code(soap, data, NULL, NULL) || soap_end_send(soap) )


SOAP_FMAC3 int SOAP_FMAC4 soap_put_SOAP_ENV__Code(struct soap*, const struct SOAP_ENV__Code *, const char*, const char*);

#define soap_read_SOAP_ENV__Code(soap, data) ( soap_begin_recv(soap) || !soap_get_SOAP_ENV__Code(soap, data, NULL, NULL) || soap_end_recv(soap) )

SOAP_FMAC3 struct SOAP_ENV__Code * SOAP_FMAC4 soap_get_SOAP_ENV__Code(struct soap*, struct SOAP_ENV__Code *, const char*, const char*);

#endif

#ifndef WITH_NOGLOBAL

#ifndef SOAP_TYPE_SOAP_ENV__Header
#define SOAP_TYPE_SOAP_ENV__Header (21)
#endif
SOAP_FMAC3 void SOAP_FMAC4 soap_default_SOAP_ENV__Header(struct soap*, struct SOAP_ENV__Header *);
SOAP_FMAC3 void SOAP_FMAC4 soap_serialize_SOAP_ENV__Header(struct soap*, const struct SOAP_ENV__Header *);
SOAP_FMAC3 int SOAP_FMAC4 soap_out_SOAP_ENV__Header(struct soap*, const char*, int, const struct SOAP_ENV__Header *, const char*);
SOAP_FMAC3 struct SOAP_ENV__Header * SOAP_FMAC4 soap_in_SOAP_ENV__Header(struct soap*, const char*, struct SOAP_ENV__Header *, const char*);

#define soap_write_SOAP_ENV__Header(soap, data) ( soap_begin_send(soap) || (soap_serialize_SOAP_ENV__Header(soap, data), 0) || soap_put_SOAP_ENV__Header(soap, data, NULL, NULL) || soap_end_send(soap) )


SOAP_FMAC3 int SOAP_FMAC4 soap_put_SOAP_ENV__Header(struct soap*, const struct SOAP_ENV__Header *, const char*, const char*);

#define soap_read_SOAP_ENV__Header(soap, data) ( soap_begin_recv(soap) || !soap_get_SOAP_ENV__Header(soap, data, NULL, NULL) || soap_end_recv(soap) )

SOAP_FMAC3 struct SOAP_ENV__Header * SOAP_FMAC4 soap_get_SOAP_ENV__Header(struct soap*, struct SOAP_ENV__Header *, const char*, const char*);

#endif

#ifndef SOAP_TYPE_log__verifyTree
#define SOAP_TYPE_log__verifyTree (20)
#endif
SOAP_FMAC3 void SOAP_FMAC4 soap_default_log__verifyTree(struct soap*, struct log__verifyTree *);
SOAP_FMAC3 void SOAP_FMAC4 soap_serialize_log__verifyTree(struct soap*, const struct log__verifyTree *);
SOAP_FMAC3 int SOAP_FMAC4 soap_out_log__verifyTree(struct soap*, const char*, int, const struct log__verifyTree *, const char*);
SOAP_FMAC3 struct log__verifyTree * SOAP_FMAC4 soap_in_log__verifyTree(struct soap*, const char*, struct log__verifyTree *, const char*);

#define soap_write_log__verifyTree(soap, data) ( soap_begin_send(soap) || (soap_serialize_log__verifyTree(soap, data), 0) || soap_put_log__verifyTree(soap, data, NULL, NULL) || soap_end_send(soap) )


SOAP_FMAC3 int SOAP_FMAC4 soap_put_log__verifyTree(struct soap*, const struct log__verifyTree *, const char*, const char*);

#define soap_read_log__verifyTree(soap, data) ( soap_begin_recv(soap) || !soap_get_log__verifyTree(soap, data, NULL, NULL) || soap_end_recv(soap) )

SOAP_FMAC3 struct log__verifyTree * SOAP_FMAC4 soap_get_log__verifyTree(struct soap*, struct log__verifyTree *, const char*, const char*);

#ifndef SOAP_TYPE_log__verifyTreeResponse
#define SOAP_TYPE_log__verifyTreeResponse (19)
#endif
SOAP_FMAC3 void SOAP_FMAC4 soap_default_log__verifyTreeResponse(struct soap*, struct log__verifyTreeResponse *);
SOAP_FMAC3 void SOAP_FMAC4 soap_serialize_log__verifyTreeResponse(struct soap*, const struct log__verifyTreeResponse *);
SOAP_FMAC3 int SOAP_FMAC4 soap_out_log__verifyTreeResponse(struct soap*, const char*, int, const struct log__verifyTreeResponse *, const char*);
SOAP_FMAC3 struct log__verifyTreeResponse * SOAP_FMAC4 soap_in_log__verifyTreeResponse(struct soap*, const char*, struct log__verifyTreeResponse *, const char*);

#define soap_write_log__verifyTreeResponse(soap, data) ( soap_begin_send(soap) || (soap_serialize_log__verifyTreeResponse(soap, data), 0) || soap_put_log__verifyTreeResponse(soap, data, NULL, NULL) || soap_end_send(soap) )


SOAP_FMAC3 int SOAP_FMAC4 soap_put_log__verifyTreeResponse(struct soap*, const struct log__verifyTreeResponse *, const char*, const char*);

#define soap_read_log__verifyTreeResponse(soap, data) ( soap_begin_recv(soap) || !soap_get_log__verifyTreeResponse(soap, data, NULL, NULL) || soap_end_recv(soap) )

SOAP_FMAC3 struct log__verifyTreeResponse * SOAP_FMAC4 soap_get_log__verifyTreeResponse(struct soap*, struct log__verifyTreeResponse *, const char*, const char*);

#ifndef SOAP_TYPE_log__verifyLog
#define SOAP_TYPE_log__verifyLog (17)
#endif
SOAP_FMAC3 void SOAP_FMAC4 soap_default_log__verifyLog(struct soap*, struct log__verifyLog *);
SOAP_FMAC3 void SOAP_FMAC4 soap_serialize_log__verifyLog(struct soap*, const struct log__verifyLog *);
SOAP_FMAC3 int SOAP_FMAC4 soap_out_log__verifyLog(struct soap*, const char*, int, const struct log__verifyLog *, const char*);
SOAP_FMAC3 struct log__verifyLog * SOAP_FMAC4 soap_in_log__verifyLog(struct soap*, const char*, struct log__verifyLog *, const char*);

#define soap_write_log__verifyLog(soap, data) ( soap_begin_send(soap) || (soap_serialize_log__verifyLog(soap, data), 0) || soap_put_log__verifyLog(soap, data, NULL, NULL) || soap_end_send(soap) )


SOAP_FMAC3 int SOAP_FMAC4 soap_put_log__verifyLog(struct soap*, const struct log__verifyLog *, const char*, const char*);

#define soap_read_log__verifyLog(soap, data) ( soap_begin_recv(soap) || !soap_get_log__verifyLog(soap, data, NULL, NULL) || soap_end_recv(soap) )

SOAP_FMAC3 struct log__verifyLog * SOAP_FMAC4 soap_get_log__verifyLog(struct soap*, struct log__verifyLog *, const char*, const char*);

#ifndef SOAP_TYPE_log__verifyLogResponse
#define SOAP_TYPE_log__verifyLogResponse (16)
#endif
SOAP_FMAC3 void SOAP_FMAC4 soap_default_log__verifyLogResponse(struct soap*, struct log__verifyLogResponse *);
SOAP_FMAC3 void SOAP_FMAC4 soap_serialize_log__verifyLogResponse(struct soap*, const struct log__verifyLogResponse *);
SOAP_FMAC3 int SOAP_FMAC4 soap_out_log__verifyLogResponse(struct soap*, const char*, int, const struct log__verifyLogResponse *, const char*);
SOAP_FMAC3 struct log__verifyLogResponse * SOAP_FMAC4 soap_in_log__verifyLogResponse(struct soap*, const char*, struct log__verifyLogResponse *, const char*);

#define soap_write_log__verifyLogResponse(soap, data) ( soap_begin_send(soap) || (soap_serialize_log__verifyLogResponse(soap, data), 0) || soap_put_log__verifyLogResponse(soap, data, NULL, NULL) || soap_end_send(soap) )


SOAP_FMAC3 int SOAP_FMAC4 soap_put_log__verifyLogResponse(struct soap*, const struct log__verifyLogResponse *, const char*, const char*);

#define soap_read_log__verifyLogResponse(soap, data) ( soap_begin_recv(soap) || !soap_get_log__verifyLogResponse(soap, data, NULL, NULL) || soap_end_recv(soap) )

SOAP_FMAC3 struct log__verifyLogResponse * SOAP_FMAC4 soap_get_log__verifyLogResponse(struct soap*, struct log__verifyLogResponse *, const char*, const char*);

#ifndef SOAP_TYPE_log__logServerSend
#define SOAP_TYPE_log__logServerSend (14)
#endif
SOAP_FMAC3 void SOAP_FMAC4 soap_default_log__logServerSend(struct soap*, struct log__logServerSend *);
SOAP_FMAC3 void SOAP_FMAC4 soap_serialize_log__logServerSend(struct soap*, const struct log__logServerSend *);
SOAP_FMAC3 int SOAP_FMAC4 soap_out_log__logServerSend(struct soap*, const char*, int, const struct log__logServerSend *, const char*);
SOAP_FMAC3 struct log__logServerSend * SOAP_FMAC4 soap_in_log__logServerSend(struct soap*, const char*, struct log__logServerSend *, const char*);

#define soap_write_log__logServerSend(soap, data) ( soap_begin_send(soap) || (soap_serialize_log__logServerSend(soap, data), 0) || soap_put_log__logServerSend(soap, data, NULL, NULL) || soap_end_send(soap) )


SOAP_FMAC3 int SOAP_FMAC4 soap_put_log__logServerSend(struct soap*, const struct log__logServerSend *, const char*, const char*);

#define soap_read_log__logServerSend(soap, data) ( soap_begin_recv(soap) || !soap_get_log__logServerSend(soap, data, NULL, NULL) || soap_end_recv(soap) )

SOAP_FMAC3 struct log__logServerSend * SOAP_FMAC4 soap_get_log__logServerSend(struct soap*, struct log__logServerSend *, const char*, const char*);

#ifndef SOAP_TYPE_log__logServerSendResponse
#define SOAP_TYPE_log__logServerSendResponse (13)
#endif
SOAP_FMAC3 void SOAP_FMAC4 soap_default_log__logServerSendResponse(struct soap*, struct log__logServerSendResponse *);
SOAP_FMAC3 void SOAP_FMAC4 soap_serialize_log__logServerSendResponse(struct soap*, const struct log__logServerSendResponse *);
SOAP_FMAC3 int SOAP_FMAC4 soap_out_log__logServerSendResponse(struct soap*, const char*, int, const struct log__logServerSendResponse *, const char*);
SOAP_FMAC3 struct log__logServerSendResponse * SOAP_FMAC4 soap_in_log__logServerSendResponse(struct soap*, const char*, struct log__logServerSendResponse *, const char*);

#define soap_write_log__logServerSendResponse(soap, data) ( soap_begin_send(soap) || (soap_serialize_log__logServerSendResponse(soap, data), 0) || soap_put_log__logServerSendResponse(soap, data, NULL, NULL) || soap_end_send(soap) )


SOAP_FMAC3 int SOAP_FMAC4 soap_put_log__logServerSendResponse(struct soap*, const struct log__logServerSendResponse *, const char*, const char*);

#define soap_read_log__logServerSendResponse(soap, data) ( soap_begin_recv(soap) || !soap_get_log__logServerSendResponse(soap, data, NULL, NULL) || soap_end_recv(soap) )

SOAP_FMAC3 struct log__logServerSendResponse * SOAP_FMAC4 soap_get_log__logServerSendResponse(struct soap*, struct log__logServerSendResponse *, const char*, const char*);

#ifndef SOAP_TYPE_log__logDatagram
#define SOAP_TYPE_log__logDatagram (8)
#endif
SOAP_FMAC3 void SOAP_FMAC4 soap_default_log__logDatagram(struct soap*, struct log__logDatagram *);
SOAP_FMAC3 void SOAP_FMAC4 soap_serialize_log__logDatagram(struct soap*, const struct log__logDatagram *);
SOAP_FMAC3 int SOAP_FMAC4 soap_out_log__logDatagram(struct soap*, const char*, int, const struct log__logDatagram *, const char*);
SOAP_FMAC3 struct log__logDatagram * SOAP_FMAC4 soap_in_log__logDatagram(struct soap*, const char*, struct log__logDatagram *, const char*);

#define soap_write_log__logDatagram(soap, data) ( soap_begin_send(soap) || (soap_serialize_log__logDatagram(soap, data), 0) || soap_put_log__logDatagram(soap, data, NULL, NULL) || soap_end_send(soap) )


SOAP_FMAC3 int SOAP_FMAC4 soap_put_log__logDatagram(struct soap*, const struct log__logDatagram *, const char*, const char*);

#define soap_read_log__logDatagram(soap, data) ( soap_begin_recv(soap) || !soap_get_log__logDatagram(soap, data, NULL, NULL) || soap_end_recv(soap) )

SOAP_FMAC3 struct log__logDatagram * SOAP_FMAC4 soap_get_log__logDatagram(struct soap*, struct log__logDatagram *, const char*, const char*);

#ifndef WITH_NOGLOBAL

#ifndef SOAP_TYPE_PointerToSOAP_ENV__Reason
#define SOAP_TYPE_PointerToSOAP_ENV__Reason (30)
#endif
SOAP_FMAC3 void SOAP_FMAC4 soap_serialize_PointerToSOAP_ENV__Reason(struct soap*, struct SOAP_ENV__Reason *const*);
SOAP_FMAC3 int SOAP_FMAC4 soap_out_PointerToSOAP_ENV__Reason(struct soap*, const char *, int, struct SOAP_ENV__Reason *const*, const char *);
SOAP_FMAC3 struct SOAP_ENV__Reason ** SOAP_FMAC4 soap_in_PointerToSOAP_ENV__Reason(struct soap*, const char*, struct SOAP_ENV__Reason **, const char*);

#define soap_write_PointerToSOAP_ENV__Reason(soap, data) ( soap_begin_send(soap) || (soap_serialize_PointerToSOAP_ENV__Reason(soap, data), 0) || soap_put_PointerToSOAP_ENV__Reason(soap, data, NULL, NULL) || soap_end_send(soap) )

SOAP_FMAC3 int SOAP_FMAC4 soap_put_PointerToSOAP_ENV__Reason(struct soap*, struct SOAP_ENV__Reason *const*, const char*, const char*);

#define soap_read_PointerToSOAP_ENV__Reason(soap, data) ( soap_begin_recv(soap) || !soap_get_PointerToSOAP_ENV__Reason(soap, data, NULL, NULL) || soap_end_recv(soap) )

SOAP_FMAC3 struct SOAP_ENV__Reason ** SOAP_FMAC4 soap_get_PointerToSOAP_ENV__Reason(struct soap*, struct SOAP_ENV__Reason **, const char*, const char*);

#endif

#ifndef WITH_NOGLOBAL

#ifndef SOAP_TYPE_PointerToSOAP_ENV__Detail
#define SOAP_TYPE_PointerToSOAP_ENV__Detail (29)
#endif
SOAP_FMAC3 void SOAP_FMAC4 soap_serialize_PointerToSOAP_ENV__Detail(struct soap*, struct SOAP_ENV__Detail *const*);
SOAP_FMAC3 int SOAP_FMAC4 soap_out_PointerToSOAP_ENV__Detail(struct soap*, const char *, int, struct SOAP_ENV__Detail *const*, const char *);
SOAP_FMAC3 struct SOAP_ENV__Detail ** SOAP_FMAC4 soap_in_PointerToSOAP_ENV__Detail(struct soap*, const char*, struct SOAP_ENV__Detail **, const char*);

#define soap_write_PointerToSOAP_ENV__Detail(soap, data) ( soap_begin_send(soap) || (soap_serialize_PointerToSOAP_ENV__Detail(soap, data), 0) || soap_put_PointerToSOAP_ENV__Detail(soap, data, NULL, NULL) || soap_end_send(soap) )

SOAP_FMAC3 int SOAP_FMAC4 soap_put_PointerToSOAP_ENV__Detail(struct soap*, struct SOAP_ENV__Detail *const*, const char*, const char*);

#define soap_read_PointerToSOAP_ENV__Detail(soap, data) ( soap_begin_recv(soap) || !soap_get_PointerToSOAP_ENV__Detail(soap, data, NULL, NULL) || soap_end_recv(soap) )

SOAP_FMAC3 struct SOAP_ENV__Detail ** SOAP_FMAC4 soap_get_PointerToSOAP_ENV__Detail(struct soap*, struct SOAP_ENV__Detail **, const char*, const char*);

#endif

#ifndef WITH_NOGLOBAL

#ifndef SOAP_TYPE_PointerToSOAP_ENV__Code
#define SOAP_TYPE_PointerToSOAP_ENV__Code (23)
#endif
SOAP_FMAC3 void SOAP_FMAC4 soap_serialize_PointerToSOAP_ENV__Code(struct soap*, struct SOAP_ENV__Code *const*);
SOAP_FMAC3 int SOAP_FMAC4 soap_out_PointerToSOAP_ENV__Code(struct soap*, const char *, int, struct SOAP_ENV__Code *const*, const char *);
SOAP_FMAC3 struct SOAP_ENV__Code ** SOAP_FMAC4 soap_in_PointerToSOAP_ENV__Code(struct soap*, const char*, struct SOAP_ENV__Code **, const char*);

#define soap_write_PointerToSOAP_ENV__Code(soap, data) ( soap_begin_send(soap) || (soap_serialize_PointerToSOAP_ENV__Code(soap, data), 0) || soap_put_PointerToSOAP_ENV__Code(soap, data, NULL, NULL) || soap_end_send(soap) )

SOAP_FMAC3 int SOAP_FMAC4 soap_put_PointerToSOAP_ENV__Code(struct soap*, struct SOAP_ENV__Code *const*, const char*, const char*);

#define soap_read_PointerToSOAP_ENV__Code(soap, data) ( soap_begin_recv(soap) || !soap_get_PointerToSOAP_ENV__Code(soap, data, NULL, NULL) || soap_end_recv(soap) )

SOAP_FMAC3 struct SOAP_ENV__Code ** SOAP_FMAC4 soap_get_PointerToSOAP_ENV__Code(struct soap*, struct SOAP_ENV__Code **, const char*, const char*);

#endif

#ifndef SOAP_TYPE_PointerTolong
#define SOAP_TYPE_PointerTolong (11)
#endif
SOAP_FMAC3 void SOAP_FMAC4 soap_serialize_PointerTolong(struct soap*, long *const*);
SOAP_FMAC3 int SOAP_FMAC4 soap_out_PointerTolong(struct soap*, const char *, int, long *const*, const char *);
SOAP_FMAC3 long ** SOAP_FMAC4 soap_in_PointerTolong(struct soap*, const char*, long **, const char*);

#define soap_write_PointerTolong(soap, data) ( soap_begin_send(soap) || (soap_serialize_PointerTolong(soap, data), 0) || soap_put_PointerTolong(soap, data, NULL, NULL) || soap_end_send(soap) )

SOAP_FMAC3 int SOAP_FMAC4 soap_put_PointerTolong(struct soap*, long *const*, const char*, const char*);

#define soap_read_PointerTolong(soap, data) ( soap_begin_recv(soap) || !soap_get_PointerTolong(soap, data, NULL, NULL) || soap_end_recv(soap) )

SOAP_FMAC3 long ** SOAP_FMAC4 soap_get_PointerTolong(struct soap*, long **, const char*, const char*);

#ifndef SOAP_TYPE_PointerTolog__logDatagram
#define SOAP_TYPE_PointerTolog__logDatagram (9)
#endif
SOAP_FMAC3 void SOAP_FMAC4 soap_serialize_PointerTolog__logDatagram(struct soap*, struct log__logDatagram *const*);
SOAP_FMAC3 int SOAP_FMAC4 soap_out_PointerTolog__logDatagram(struct soap*, const char *, int, struct log__logDatagram *const*, const char *);
SOAP_FMAC3 struct log__logDatagram ** SOAP_FMAC4 soap_in_PointerTolog__logDatagram(struct soap*, const char*, struct log__logDatagram **, const char*);

#define soap_write_PointerTolog__logDatagram(soap, data) ( soap_begin_send(soap) || (soap_serialize_PointerTolog__logDatagram(soap, data), 0) || soap_put_PointerTolog__logDatagram(soap, data, NULL, NULL) || soap_end_send(soap) )

SOAP_FMAC3 int SOAP_FMAC4 soap_put_PointerTolog__logDatagram(struct soap*, struct log__logDatagram *const*, const char*, const char*);

#define soap_read_PointerTolog__logDatagram(soap, data) ( soap_begin_recv(soap) || !soap_get_PointerTolog__logDatagram(soap, data, NULL, NULL) || soap_end_recv(soap) )

SOAP_FMAC3 struct log__logDatagram ** SOAP_FMAC4 soap_get_PointerTolog__logDatagram(struct soap*, struct log__logDatagram **, const char*, const char*);

#ifndef SOAP_TYPE_xsd__string
#define SOAP_TYPE_xsd__string (7)
#endif

#define soap_default_xsd__string(soap, a) soap_default_string(soap, a)


#define soap_serialize_xsd__string(soap, a) soap_serialize_string(soap, a)

SOAP_FMAC3 int SOAP_FMAC4 soap_out_xsd__string(struct soap*, const char*, int, char*const*, const char*);
SOAP_FMAC3 char * * SOAP_FMAC4 soap_in_xsd__string(struct soap*, const char*, char **, const char*);

#define soap_write_xsd__string(soap, data) ( soap_begin_send(soap) || (soap_serialize_xsd__string(soap, data), 0) || soap_put_xsd__string(soap, data, NULL, NULL) || soap_end_send(soap) )

SOAP_FMAC3 int SOAP_FMAC4 soap_put_xsd__string(struct soap*, char *const*, const char*, const char*);

#define soap_read_xsd__string(soap, data) ( soap_begin_recv(soap) || !soap_get_xsd__string(soap, data, NULL, NULL) || soap_end_recv(soap) )

SOAP_FMAC3 char ** SOAP_FMAC4 soap_get_xsd__string(struct soap*, char **, const char*, const char*);

#ifndef SOAP_TYPE__QName
#define SOAP_TYPE__QName (5)
#endif

#define soap_default__QName(soap, a) soap_default_string(soap, a)


#define soap_serialize__QName(soap, a) soap_serialize_string(soap, a)

SOAP_FMAC3 int SOAP_FMAC4 soap_out__QName(struct soap*, const char*, int, char*const*, const char*);
SOAP_FMAC3 char * * SOAP_FMAC4 soap_in__QName(struct soap*, const char*, char **, const char*);

#define soap_write__QName(soap, data) ( soap_begin_send(soap) || (soap_serialize__QName(soap, data), 0) || soap_put__QName(soap, data, NULL, NULL) || soap_end_send(soap) )

SOAP_FMAC3 int SOAP_FMAC4 soap_put__QName(struct soap*, char *const*, const char*, const char*);

#define soap_read__QName(soap, data) ( soap_begin_recv(soap) || !soap_get__QName(soap, data, NULL, NULL) || soap_end_recv(soap) )

SOAP_FMAC3 char ** SOAP_FMAC4 soap_get__QName(struct soap*, char **, const char*, const char*);

#ifndef SOAP_TYPE_string
#define SOAP_TYPE_string (4)
#endif
SOAP_FMAC3 void SOAP_FMAC4 soap_default_string(struct soap*, char **);
SOAP_FMAC3 void SOAP_FMAC4 soap_serialize_string(struct soap*, char *const*);
SOAP_FMAC3 int SOAP_FMAC4 soap_out_string(struct soap*, const char*, int, char*const*, const char*);
SOAP_FMAC3 char * * SOAP_FMAC4 soap_in_string(struct soap*, const char*, char **, const char*);

#define soap_write_string(soap, data) ( soap_begin_send(soap) || (soap_serialize_string(soap, data), 0) || soap_put_string(soap, data, NULL, NULL) || soap_end_send(soap) )

SOAP_FMAC3 int SOAP_FMAC4 soap_put_string(struct soap*, char *const*, const char*, const char*);

#define soap_read_string(soap, data) ( soap_begin_recv(soap) || !soap_get_string(soap, data, NULL, NULL) || soap_end_recv(soap) )

SOAP_FMAC3 char ** SOAP_FMAC4 soap_get_string(struct soap*, char **, const char*, const char*);

#ifdef __cplusplus
}
#endif

#endif

/* End of logH.h */
