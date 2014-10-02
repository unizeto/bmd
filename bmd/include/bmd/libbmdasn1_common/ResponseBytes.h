/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "OCSP"
 * 	found in "ocsp.asn1"
 */

#ifndef	_ResponseBytes_H_
#define	_ResponseBytes_H_


#include <bmd/libbmdasn1_core/asn_application.h>

/* Including external dependencies */
#include <bmd/libbmdasn1_core/OBJECT_IDENTIFIER.h>
#include <bmd/libbmdasn1_core/OCTET_STRING.h>
#include <bmd/libbmdasn1_core/constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ResponseBytes */
typedef struct ResponseBytes {
	OBJECT_IDENTIFIER_t	 responseType;
	OCTET_STRING_t	 response;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} ResponseBytes_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_ResponseBytes;

#ifdef __cplusplus
}
#endif

#endif	/* _ResponseBytes_H_ */
