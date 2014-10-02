/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Explicit88"
 * 	found in "PKIX1Explicit88.asn1"
 */

#ifndef	_RSAPublicKey_H_
#define	_RSAPublicKey_H_


#include <bmd/libbmdasn1_core/asn_application.h>

/* Including external dependencies */
#include <bmd/libbmdasn1_core/INTEGER.h>
#include <bmd/libbmdasn1_core/constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* RSAPublicKey */
typedef struct RSAPublicKey {
	INTEGER_t	 modulus;
	INTEGER_t	 publicExponent;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} RSAPublicKey_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_RSAPublicKey;

#ifdef __cplusplus
}
#endif

#endif	/* _RSAPublicKey_H_ */