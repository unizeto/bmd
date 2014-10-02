/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Explicit88"
 * 	found in "rfc3280-PKIX1Explicit88.asn1"
 */

#ifndef	_SubjectPublicKeyInfo_H_
#define	_SubjectPublicKeyInfo_H_


#include <bmd/libbmdasn1_core/asn_application.h>

/* Including external dependencies */
#include <bmd/libbmdasn1_common/AlgorithmIdentifier.h>
#include <bmd/libbmdasn1_core/BIT_STRING.h>
#include <bmd/libbmdasn1_core/constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* SubjectPublicKeyInfo */
typedef struct SubjectPublicKeyInfo {
	AlgorithmIdentifier_t	 algorithm;
	BIT_STRING_t	 subjectPublicKey;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} SubjectPublicKeyInfo_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_SubjectPublicKeyInfo;

#ifdef __cplusplus
}
#endif

#endif	/* _SubjectPublicKeyInfo_H_ */
