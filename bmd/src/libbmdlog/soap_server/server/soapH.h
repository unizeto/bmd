/* soapH.h
   Generated by gSOAP 2.7.9f from logsoap.h
   Copyright(C) 2000-2006, Robert van Engelen, Genivia Inc. All Rights Reserved.
   This part of the software is released under one of the following licenses:
   GPL, the gSOAP public license, or Genivia's license for commercial use.
*/

#ifndef soapH_H
#define soapH_H
#include "soapStub.h"
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
#define SOAP_TYPE_byte (2)
#endif
SOAP_FMAC3 void SOAP_FMAC4 soap_default_byte(struct soap*, char *);
SOAP_FMAC3 int SOAP_FMAC4 soap_put_byte(struct soap*, const char *, const char*, const char*);
SOAP_FMAC3 int SOAP_FMAC4 soap_out_byte(struct soap*, const char*, int, const char *, const char*);
SOAP_FMAC3 char * SOAP_FMAC4 soap_get_byte(struct soap*, char *, const char*, const char*);
SOAP_FMAC3 char * SOAP_FMAC4 soap_in_byte(struct soap*, const char*, char *, const char*);

#ifndef SOAP_TYPE_int
#define SOAP_TYPE_int (1)
#endif
SOAP_FMAC3 void SOAP_FMAC4 soap_default_int(struct soap*, int *);
SOAP_FMAC3 int SOAP_FMAC4 soap_put_int(struct soap*, const int *, const char*, const char*);
SOAP_FMAC3 int SOAP_FMAC4 soap_out_int(struct soap*, const char*, int, const int *, const char*);
SOAP_FMAC3 int * SOAP_FMAC4 soap_get_int(struct soap*, int *, const char*, const char*);
SOAP_FMAC3 int * SOAP_FMAC4 soap_in_int(struct soap*, const char*, int *, const char*);

#ifndef SOAP_TYPE_long
#define SOAP_TYPE_long (9)
#endif
SOAP_FMAC3 void SOAP_FMAC4 soap_default_long(struct soap*, long *);
SOAP_FMAC3 int SOAP_FMAC4 soap_put_long(struct soap*, const long *, const char*, const char*);
SOAP_FMAC3 int SOAP_FMAC4 soap_out_long(struct soap*, const char*, int, const long *, const char*);
SOAP_FMAC3 long * SOAP_FMAC4 soap_get_long(struct soap*, long *, const char*, const char*);
SOAP_FMAC3 long * SOAP_FMAC4 soap_in_long(struct soap*, const char*, long *, const char*);

#ifndef WITH_NOGLOBAL

#ifndef SOAP_TYPE_SOAP_ENV__Fault
#define SOAP_TYPE_SOAP_ENV__Fault (27)
#endif
SOAP_FMAC3 void SOAP_FMAC4 soap_default_SOAP_ENV__Fault(struct soap*, struct SOAP_ENV__Fault *);
SOAP_FMAC3 void SOAP_FMAC4 soap_serialize_SOAP_ENV__Fault(struct soap*, const struct SOAP_ENV__Fault *);
SOAP_FMAC3 int SOAP_FMAC4 soap_put_SOAP_ENV__Fault(struct soap*, const struct SOAP_ENV__Fault *, const char*, const char*);
SOAP_FMAC3 int SOAP_FMAC4 soap_out_SOAP_ENV__Fault(struct soap*, const char*, int, const struct SOAP_ENV__Fault *, const char*);
SOAP_FMAC3 struct SOAP_ENV__Fault * SOAP_FMAC4 soap_get_SOAP_ENV__Fault(struct soap*, struct SOAP_ENV__Fault *, const char*, const char*);
SOAP_FMAC3 struct SOAP_ENV__Fault * SOAP_FMAC4 soap_in_SOAP_ENV__Fault(struct soap*, const char*, struct SOAP_ENV__Fault *, const char*);

#endif

#ifndef WITH_NOGLOBAL

#ifndef SOAP_TYPE_SOAP_ENV__Reason
#define SOAP_TYPE_SOAP_ENV__Reason (26)
#endif
SOAP_FMAC3 void SOAP_FMAC4 soap_default_SOAP_ENV__Reason(struct soap*, struct SOAP_ENV__Reason *);
SOAP_FMAC3 void SOAP_FMAC4 soap_serialize_SOAP_ENV__Reason(struct soap*, const struct SOAP_ENV__Reason *);
SOAP_FMAC3 int SOAP_FMAC4 soap_put_SOAP_ENV__Reason(struct soap*, const struct SOAP_ENV__Reason *, const char*, const char*);
SOAP_FMAC3 int SOAP_FMAC4 soap_out_SOAP_ENV__Reason(struct soap*, const char*, int, const struct SOAP_ENV__Reason *, const char*);
SOAP_FMAC3 struct SOAP_ENV__Reason * SOAP_FMAC4 soap_get_SOAP_ENV__Reason(struct soap*, struct SOAP_ENV__Reason *, const char*, const char*);
SOAP_FMAC3 struct SOAP_ENV__Reason * SOAP_FMAC4 soap_in_SOAP_ENV__Reason(struct soap*, const char*, struct SOAP_ENV__Reason *, const char*);

#endif

#ifndef WITH_NOGLOBAL

#ifndef SOAP_TYPE_SOAP_ENV__Detail
#define SOAP_TYPE_SOAP_ENV__Detail (25)
#endif
SOAP_FMAC3 void SOAP_FMAC4 soap_default_SOAP_ENV__Detail(struct soap*, struct SOAP_ENV__Detail *);
SOAP_FMAC3 void SOAP_FMAC4 soap_serialize_SOAP_ENV__Detail(struct soap*, const struct SOAP_ENV__Detail *);
SOAP_FMAC3 int SOAP_FMAC4 soap_put_SOAP_ENV__Detail(struct soap*, const struct SOAP_ENV__Detail *, const char*, const char*);
SOAP_FMAC3 int SOAP_FMAC4 soap_out_SOAP_ENV__Detail(struct soap*, const char*, int, const struct SOAP_ENV__Detail *, const char*);
SOAP_FMAC3 struct SOAP_ENV__Detail * SOAP_FMAC4 soap_get_SOAP_ENV__Detail(struct soap*, struct SOAP_ENV__Detail *, const char*, const char*);
SOAP_FMAC3 struct SOAP_ENV__Detail * SOAP_FMAC4 soap_in_SOAP_ENV__Detail(struct soap*, const char*, struct SOAP_ENV__Detail *, const char*);

#endif

#ifndef WITH_NOGLOBAL

#ifndef SOAP_TYPE_SOAP_ENV__Code
#define SOAP_TYPE_SOAP_ENV__Code (23)
#endif
SOAP_FMAC3 void SOAP_FMAC4 soap_default_SOAP_ENV__Code(struct soap*, struct SOAP_ENV__Code *);
SOAP_FMAC3 void SOAP_FMAC4 soap_serialize_SOAP_ENV__Code(struct soap*, const struct SOAP_ENV__Code *);
SOAP_FMAC3 int SOAP_FMAC4 soap_put_SOAP_ENV__Code(struct soap*, const struct SOAP_ENV__Code *, const char*, const char*);
SOAP_FMAC3 int SOAP_FMAC4 soap_out_SOAP_ENV__Code(struct soap*, const char*, int, const struct SOAP_ENV__Code *, const char*);
SOAP_FMAC3 struct SOAP_ENV__Code * SOAP_FMAC4 soap_get_SOAP_ENV__Code(struct soap*, struct SOAP_ENV__Code *, const char*, const char*);
SOAP_FMAC3 struct SOAP_ENV__Code * SOAP_FMAC4 soap_in_SOAP_ENV__Code(struct soap*, const char*, struct SOAP_ENV__Code *, const char*);

#endif

#ifndef WITH_NOGLOBAL

#ifndef SOAP_TYPE_SOAP_ENV__Header
#define SOAP_TYPE_SOAP_ENV__Header (22)
#endif
SOAP_FMAC3 void SOAP_FMAC4 soap_default_SOAP_ENV__Header(struct soap*, struct SOAP_ENV__Header *);
SOAP_FMAC3 void SOAP_FMAC4 soap_serialize_SOAP_ENV__Header(struct soap*, const struct SOAP_ENV__Header *);
SOAP_FMAC3 int SOAP_FMAC4 soap_put_SOAP_ENV__Header(struct soap*, const struct SOAP_ENV__Header *, const char*, const char*);
SOAP_FMAC3 int SOAP_FMAC4 soap_out_SOAP_ENV__Header(struct soap*, const char*, int, const struct SOAP_ENV__Header *, const char*);
SOAP_FMAC3 struct SOAP_ENV__Header * SOAP_FMAC4 soap_get_SOAP_ENV__Header(struct soap*, struct SOAP_ENV__Header *, const char*, const char*);
SOAP_FMAC3 struct SOAP_ENV__Header * SOAP_FMAC4 soap_in_SOAP_ENV__Header(struct soap*, const char*, struct SOAP_ENV__Header *, const char*);

#endif

#ifndef SOAP_TYPE_ns__verifyTree
#define SOAP_TYPE_ns__verifyTree (19)
#endif
SOAP_FMAC3 void SOAP_FMAC4 soap_default_ns__verifyTree(struct soap*, struct ns__verifyTree *);
SOAP_FMAC3 void SOAP_FMAC4 soap_serialize_ns__verifyTree(struct soap*, const struct ns__verifyTree *);
SOAP_FMAC3 int SOAP_FMAC4 soap_put_ns__verifyTree(struct soap*, const struct ns__verifyTree *, const char*, const char*);
SOAP_FMAC3 int SOAP_FMAC4 soap_out_ns__verifyTree(struct soap*, const char*, int, const struct ns__verifyTree *, const char*);
SOAP_FMAC3 struct ns__verifyTree * SOAP_FMAC4 soap_get_ns__verifyTree(struct soap*, struct ns__verifyTree *, const char*, const char*);
SOAP_FMAC3 struct ns__verifyTree * SOAP_FMAC4 soap_in_ns__verifyTree(struct soap*, const char*, struct ns__verifyTree *, const char*);

#ifndef SOAP_TYPE_ns__verifyTreeResponse
#define SOAP_TYPE_ns__verifyTreeResponse (18)
#endif
SOAP_FMAC3 void SOAP_FMAC4 soap_default_ns__verifyTreeResponse(struct soap*, struct ns__verifyTreeResponse *);
SOAP_FMAC3 void SOAP_FMAC4 soap_serialize_ns__verifyTreeResponse(struct soap*, const struct ns__verifyTreeResponse *);
SOAP_FMAC3 int SOAP_FMAC4 soap_put_ns__verifyTreeResponse(struct soap*, const struct ns__verifyTreeResponse *, const char*, const char*);
SOAP_FMAC3 int SOAP_FMAC4 soap_out_ns__verifyTreeResponse(struct soap*, const char*, int, const struct ns__verifyTreeResponse *, const char*);
SOAP_FMAC3 struct ns__verifyTreeResponse * SOAP_FMAC4 soap_get_ns__verifyTreeResponse(struct soap*, struct ns__verifyTreeResponse *, const char*, const char*);
SOAP_FMAC3 struct ns__verifyTreeResponse * SOAP_FMAC4 soap_in_ns__verifyTreeResponse(struct soap*, const char*, struct ns__verifyTreeResponse *, const char*);

#ifndef SOAP_TYPE_ns__verifyLog
#define SOAP_TYPE_ns__verifyLog (16)
#endif
SOAP_FMAC3 void SOAP_FMAC4 soap_default_ns__verifyLog(struct soap*, struct ns__verifyLog *);
SOAP_FMAC3 void SOAP_FMAC4 soap_serialize_ns__verifyLog(struct soap*, const struct ns__verifyLog *);
SOAP_FMAC3 int SOAP_FMAC4 soap_put_ns__verifyLog(struct soap*, const struct ns__verifyLog *, const char*, const char*);
SOAP_FMAC3 int SOAP_FMAC4 soap_out_ns__verifyLog(struct soap*, const char*, int, const struct ns__verifyLog *, const char*);
SOAP_FMAC3 struct ns__verifyLog * SOAP_FMAC4 soap_get_ns__verifyLog(struct soap*, struct ns__verifyLog *, const char*, const char*);
SOAP_FMAC3 struct ns__verifyLog * SOAP_FMAC4 soap_in_ns__verifyLog(struct soap*, const char*, struct ns__verifyLog *, const char*);

#ifndef SOAP_TYPE_ns__verifyLogResponse
#define SOAP_TYPE_ns__verifyLogResponse (15)
#endif
SOAP_FMAC3 void SOAP_FMAC4 soap_default_ns__verifyLogResponse(struct soap*, struct ns__verifyLogResponse *);
SOAP_FMAC3 void SOAP_FMAC4 soap_serialize_ns__verifyLogResponse(struct soap*, const struct ns__verifyLogResponse *);
SOAP_FMAC3 int SOAP_FMAC4 soap_put_ns__verifyLogResponse(struct soap*, const struct ns__verifyLogResponse *, const char*, const char*);
SOAP_FMAC3 int SOAP_FMAC4 soap_out_ns__verifyLogResponse(struct soap*, const char*, int, const struct ns__verifyLogResponse *, const char*);
SOAP_FMAC3 struct ns__verifyLogResponse * SOAP_FMAC4 soap_get_ns__verifyLogResponse(struct soap*, struct ns__verifyLogResponse *, const char*, const char*);
SOAP_FMAC3 struct ns__verifyLogResponse * SOAP_FMAC4 soap_in_ns__verifyLogResponse(struct soap*, const char*, struct ns__verifyLogResponse *, const char*);

#ifndef SOAP_TYPE_ns__logServerSend
#define SOAP_TYPE_ns__logServerSend (13)
#endif
SOAP_FMAC3 void SOAP_FMAC4 soap_default_ns__logServerSend(struct soap*, struct ns__logServerSend *);
SOAP_FMAC3 void SOAP_FMAC4 soap_serialize_ns__logServerSend(struct soap*, const struct ns__logServerSend *);
SOAP_FMAC3 int SOAP_FMAC4 soap_put_ns__logServerSend(struct soap*, const struct ns__logServerSend *, const char*, const char*);
SOAP_FMAC3 int SOAP_FMAC4 soap_out_ns__logServerSend(struct soap*, const char*, int, const struct ns__logServerSend *, const char*);
SOAP_FMAC3 struct ns__logServerSend * SOAP_FMAC4 soap_get_ns__logServerSend(struct soap*, struct ns__logServerSend *, const char*, const char*);
SOAP_FMAC3 struct ns__logServerSend * SOAP_FMAC4 soap_in_ns__logServerSend(struct soap*, const char*, struct ns__logServerSend *, const char*);

#ifndef SOAP_TYPE_ns__logServerSendResponse
#define SOAP_TYPE_ns__logServerSendResponse (12)
#endif
SOAP_FMAC3 void SOAP_FMAC4 soap_default_ns__logServerSendResponse(struct soap*, struct ns__logServerSendResponse *);
SOAP_FMAC3 void SOAP_FMAC4 soap_serialize_ns__logServerSendResponse(struct soap*, const struct ns__logServerSendResponse *);
SOAP_FMAC3 int SOAP_FMAC4 soap_put_ns__logServerSendResponse(struct soap*, const struct ns__logServerSendResponse *, const char*, const char*);
SOAP_FMAC3 int SOAP_FMAC4 soap_out_ns__logServerSendResponse(struct soap*, const char*, int, const struct ns__logServerSendResponse *, const char*);
SOAP_FMAC3 struct ns__logServerSendResponse * SOAP_FMAC4 soap_get_ns__logServerSendResponse(struct soap*, struct ns__logServerSendResponse *, const char*, const char*);
SOAP_FMAC3 struct ns__logServerSendResponse * SOAP_FMAC4 soap_in_ns__logServerSendResponse(struct soap*, const char*, struct ns__logServerSendResponse *, const char*);

#ifndef SOAP_TYPE_ns__logDatagram
#define SOAP_TYPE_ns__logDatagram (7)
#endif
SOAP_FMAC3 void SOAP_FMAC4 soap_default_ns__logDatagram(struct soap*, struct ns__logDatagram *);
SOAP_FMAC3 void SOAP_FMAC4 soap_serialize_ns__logDatagram(struct soap*, const struct ns__logDatagram *);
SOAP_FMAC3 int SOAP_FMAC4 soap_put_ns__logDatagram(struct soap*, const struct ns__logDatagram *, const char*, const char*);
SOAP_FMAC3 int SOAP_FMAC4 soap_out_ns__logDatagram(struct soap*, const char*, int, const struct ns__logDatagram *, const char*);
SOAP_FMAC3 struct ns__logDatagram * SOAP_FMAC4 soap_get_ns__logDatagram(struct soap*, struct ns__logDatagram *, const char*, const char*);
SOAP_FMAC3 struct ns__logDatagram * SOAP_FMAC4 soap_in_ns__logDatagram(struct soap*, const char*, struct ns__logDatagram *, const char*);

#ifndef WITH_NOGLOBAL

#ifndef SOAP_TYPE_PointerToSOAP_ENV__Reason
#define SOAP_TYPE_PointerToSOAP_ENV__Reason (29)
#endif
SOAP_FMAC3 void SOAP_FMAC4 soap_serialize_PointerToSOAP_ENV__Reason(struct soap*, struct SOAP_ENV__Reason *const*);
SOAP_FMAC3 int SOAP_FMAC4 soap_put_PointerToSOAP_ENV__Reason(struct soap*, struct SOAP_ENV__Reason *const*, const char*, const char*);
SOAP_FMAC3 int SOAP_FMAC4 soap_out_PointerToSOAP_ENV__Reason(struct soap*, const char *, int, struct SOAP_ENV__Reason *const*, const char *);
SOAP_FMAC3 struct SOAP_ENV__Reason ** SOAP_FMAC4 soap_get_PointerToSOAP_ENV__Reason(struct soap*, struct SOAP_ENV__Reason **, const char*, const char*);
SOAP_FMAC3 struct SOAP_ENV__Reason ** SOAP_FMAC4 soap_in_PointerToSOAP_ENV__Reason(struct soap*, const char*, struct SOAP_ENV__Reason **, const char*);

#endif

#ifndef WITH_NOGLOBAL

#ifndef SOAP_TYPE_PointerToSOAP_ENV__Detail
#define SOAP_TYPE_PointerToSOAP_ENV__Detail (28)
#endif
SOAP_FMAC3 void SOAP_FMAC4 soap_serialize_PointerToSOAP_ENV__Detail(struct soap*, struct SOAP_ENV__Detail *const*);
SOAP_FMAC3 int SOAP_FMAC4 soap_put_PointerToSOAP_ENV__Detail(struct soap*, struct SOAP_ENV__Detail *const*, const char*, const char*);
SOAP_FMAC3 int SOAP_FMAC4 soap_out_PointerToSOAP_ENV__Detail(struct soap*, const char *, int, struct SOAP_ENV__Detail *const*, const char *);
SOAP_FMAC3 struct SOAP_ENV__Detail ** SOAP_FMAC4 soap_get_PointerToSOAP_ENV__Detail(struct soap*, struct SOAP_ENV__Detail **, const char*, const char*);
SOAP_FMAC3 struct SOAP_ENV__Detail ** SOAP_FMAC4 soap_in_PointerToSOAP_ENV__Detail(struct soap*, const char*, struct SOAP_ENV__Detail **, const char*);

#endif

#ifndef WITH_NOGLOBAL

#ifndef SOAP_TYPE_PointerToSOAP_ENV__Code
#define SOAP_TYPE_PointerToSOAP_ENV__Code (24)
#endif
SOAP_FMAC3 void SOAP_FMAC4 soap_serialize_PointerToSOAP_ENV__Code(struct soap*, struct SOAP_ENV__Code *const*);
SOAP_FMAC3 int SOAP_FMAC4 soap_put_PointerToSOAP_ENV__Code(struct soap*, struct SOAP_ENV__Code *const*, const char*, const char*);
SOAP_FMAC3 int SOAP_FMAC4 soap_out_PointerToSOAP_ENV__Code(struct soap*, const char *, int, struct SOAP_ENV__Code *const*, const char *);
SOAP_FMAC3 struct SOAP_ENV__Code ** SOAP_FMAC4 soap_get_PointerToSOAP_ENV__Code(struct soap*, struct SOAP_ENV__Code **, const char*, const char*);
SOAP_FMAC3 struct SOAP_ENV__Code ** SOAP_FMAC4 soap_in_PointerToSOAP_ENV__Code(struct soap*, const char*, struct SOAP_ENV__Code **, const char*);

#endif

#ifndef SOAP_TYPE_PointerTolong
#define SOAP_TYPE_PointerTolong (10)
#endif
SOAP_FMAC3 void SOAP_FMAC4 soap_serialize_PointerTolong(struct soap*, long *const*);
SOAP_FMAC3 int SOAP_FMAC4 soap_put_PointerTolong(struct soap*, long *const*, const char*, const char*);
SOAP_FMAC3 int SOAP_FMAC4 soap_out_PointerTolong(struct soap*, const char *, int, long *const*, const char *);
SOAP_FMAC3 long ** SOAP_FMAC4 soap_get_PointerTolong(struct soap*, long **, const char*, const char*);
SOAP_FMAC3 long ** SOAP_FMAC4 soap_in_PointerTolong(struct soap*, const char*, long **, const char*);

#ifndef SOAP_TYPE_PointerTons__logDatagram
#define SOAP_TYPE_PointerTons__logDatagram (8)
#endif
SOAP_FMAC3 void SOAP_FMAC4 soap_serialize_PointerTons__logDatagram(struct soap*, struct ns__logDatagram *const*);
SOAP_FMAC3 int SOAP_FMAC4 soap_put_PointerTons__logDatagram(struct soap*, struct ns__logDatagram *const*, const char*, const char*);
SOAP_FMAC3 int SOAP_FMAC4 soap_out_PointerTons__logDatagram(struct soap*, const char *, int, struct ns__logDatagram *const*, const char *);
SOAP_FMAC3 struct ns__logDatagram ** SOAP_FMAC4 soap_get_PointerTons__logDatagram(struct soap*, struct ns__logDatagram **, const char*, const char*);
SOAP_FMAC3 struct ns__logDatagram ** SOAP_FMAC4 soap_in_PointerTons__logDatagram(struct soap*, const char*, struct ns__logDatagram **, const char*);

#ifndef SOAP_TYPE_xsd__string
#define SOAP_TYPE_xsd__string (6)
#endif
SOAP_FMAC3 void SOAP_FMAC4 soap_default_xsd__string(struct soap*, char **);
SOAP_FMAC3 void SOAP_FMAC4 soap_serialize_xsd__string(struct soap*, char *const*);
SOAP_FMAC3 int SOAP_FMAC4 soap_put_xsd__string(struct soap*, char *const*, const char*, const char*);
SOAP_FMAC3 int SOAP_FMAC4 soap_out_xsd__string(struct soap*, const char*, int, char*const*, const char*);
SOAP_FMAC3 char ** SOAP_FMAC4 soap_get_xsd__string(struct soap*, char **, const char*, const char*);
SOAP_FMAC3 char * * SOAP_FMAC4 soap_in_xsd__string(struct soap*, const char*, char **, const char*);

#ifndef SOAP_TYPE__QName
#define SOAP_TYPE__QName (5)
#endif
SOAP_FMAC3 void SOAP_FMAC4 soap_default__QName(struct soap*, char **);
SOAP_FMAC3 void SOAP_FMAC4 soap_serialize__QName(struct soap*, char *const*);
SOAP_FMAC3 int SOAP_FMAC4 soap_put__QName(struct soap*, char *const*, const char*, const char*);
SOAP_FMAC3 int SOAP_FMAC4 soap_out__QName(struct soap*, const char*, int, char*const*, const char*);
SOAP_FMAC3 char ** SOAP_FMAC4 soap_get__QName(struct soap*, char **, const char*, const char*);
SOAP_FMAC3 char * * SOAP_FMAC4 soap_in__QName(struct soap*, const char*, char **, const char*);

#ifndef SOAP_TYPE_string
#define SOAP_TYPE_string (3)
#endif
SOAP_FMAC3 void SOAP_FMAC4 soap_default_string(struct soap*, char **);
SOAP_FMAC3 void SOAP_FMAC4 soap_serialize_string(struct soap*, char *const*);
SOAP_FMAC3 int SOAP_FMAC4 soap_put_string(struct soap*, char *const*, const char*, const char*);
SOAP_FMAC3 int SOAP_FMAC4 soap_out_string(struct soap*, const char*, int, char*const*, const char*);
SOAP_FMAC3 char ** SOAP_FMAC4 soap_get_string(struct soap*, char **, const char*, const char*);
SOAP_FMAC3 char * * SOAP_FMAC4 soap_in_string(struct soap*, const char*, char **, const char*);

#ifdef __cplusplus
}
#endif

#endif

/* End of soapH.h */