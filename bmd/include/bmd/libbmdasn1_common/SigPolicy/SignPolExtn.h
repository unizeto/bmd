/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "ETS-ElectronicSignaturePolicies-88syntax"
 * 	found in "rfc3125.asn1"
 */

#ifndef	_SignPolExtn_H_
#define	_SignPolExtn_H_


#include <bmd/libbmdasn1_core/asn_application.h>

/* Including external dependencies */
#include <bmd/libbmdasn1_core/OBJECT_IDENTIFIER.h>
#include <bmd/libbmdasn1_core/OCTET_STRING.h>
#include <bmd/libbmdasn1_core/constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* SignPolExtn */
typedef struct SignPolExtn {
	OBJECT_IDENTIFIER_t	 extnID;
	OCTET_STRING_t	 extnValue;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} SignPolExtn_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_SignPolExtn;

#ifdef __cplusplus
}
#endif

#endif	/* _SignPolExtn_H_ */
