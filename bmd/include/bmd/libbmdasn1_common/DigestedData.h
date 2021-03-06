/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "CryptographicMessageSyntax"
 * 	found in "rfc3852-CryptographicMessageSyntax2004.asn1"
 */

#ifndef	_DigestedData_H_
#define	_DigestedData_H_


#include <bmd/libbmdasn1_core/asn_application.h>

/* Including external dependencies */
#include <bmd/libbmdasn1_common/CMSVersion.h>
#include <bmd/libbmdasn1_common/DigestAlgorithmIdentifier.h>
#include <bmd/libbmdasn1_common/EncapsulatedContentInfo.h>
#include <bmd/libbmdasn1_common/Digest.h>
#include <bmd/libbmdasn1_core/constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* DigestedData */
typedef struct DigestedData {
	CMSVersion_t	 version;
	DigestAlgorithmIdentifier_t	 digestAlgorithm;
	EncapsulatedContentInfo_t	 encapContentInfo;
	Digest_t	 digest;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} DigestedData_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_DigestedData;

#ifdef __cplusplus
}
#endif

#endif	/* _DigestedData_H_ */
