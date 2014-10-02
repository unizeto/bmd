/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Explicit88"
 * 	found in "rfc3280-PKIX1Explicit88.asn1"
 */

#ifndef	_AlgorithmIdentifier_H_
#define	_AlgorithmIdentifier_H_


#include <bmd/libbmdasn1_core/asn_application.h>

/* Including external dependencies */
#include <bmd/libbmdasn1_core/OBJECT_IDENTIFIER.h>
#include <bmd/libbmdasn1_core/ANY.h>
#include <bmd/libbmdasn1_core/constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* AlgorithmIdentifier */
typedef struct AlgorithmIdentifier {
	OBJECT_IDENTIFIER_t	 algorithm;
	ANY_t	*parameters	/* OPTIONAL */;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} AlgorithmIdentifier_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_AlgorithmIdentifier;

#ifdef __cplusplus
}
#endif

#endif	/* _AlgorithmIdentifier_H_ */
