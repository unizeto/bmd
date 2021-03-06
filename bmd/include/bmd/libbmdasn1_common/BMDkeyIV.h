/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "BMDKeyIV"
 * 	found in "sym.asn1"
 */

#ifndef	_BMDkeyIV_H_
#define	_BMDkeyIV_H_


#include <bmd/libbmdasn1_core/asn_application.h>

/* Including external dependencies */
#include <bmd/libbmdasn1_common/AlgorithmIdentifier.h>
#include <bmd/libbmdasn1_core/OCTET_STRING.h>
#include <bmd/libbmdasn1_core/constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* BMDkeyIV */
typedef struct BMDkeyIV {
	AlgorithmIdentifier_t	 keyEncAlgoOID;
	OCTET_STRING_t	 encryptedKey;
	AlgorithmIdentifier_t	 symAlgoOID;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} BMDkeyIV_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_BMDkeyIV;

#ifdef __cplusplus
}
#endif

#endif	/* _BMDkeyIV_H_ */
