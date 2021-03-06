/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "PKCS-15"
 * 	found in "pkcs15_ms.asn1"
 */

#ifndef	_PKCS15EncryptedContentInfo_H_
#define	_PKCS15EncryptedContentInfo_H_


#include <bmd/libbmdasn1_core/asn_application.h>

/* Including external dependencies */
#include <bmd/libbmdasn1_core/OBJECT_IDENTIFIER.h>
#include <bmd/libbmdasn1_common/AlgorithmIdentifier.h>
#include <bmd/libbmdasn1_core/OCTET_STRING.h>
#include <bmd/libbmdasn1_core/constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* PKCS15EncryptedContentInfo */
typedef struct PKCS15EncryptedContentInfo {
	OBJECT_IDENTIFIER_t	 contentType;
	AlgorithmIdentifier_t	 contentEncryptionAlgorithm;
	OCTET_STRING_t	*encryptedContent	/* OPTIONAL */;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} PKCS15EncryptedContentInfo_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_PKCS15EncryptedContentInfo;

#ifdef __cplusplus
}
#endif

#endif	/* _PKCS15EncryptedContentInfo_H_ */
