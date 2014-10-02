/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "PKCS-15"
 * 	found in "pkcs15_ms.asn1"
 */

#ifndef	_PKCS15urlWithDigest_H_
#define	_PKCS15urlWithDigest_H_


#include <bmd/libbmdasn1_core/asn_application.h>

/* Including external dependencies */
#include <bmd/libbmdasn1_core/IA5String.h>
#include <bmd/libbmdasn1_common/pkcs15/PKCS15DigestInfoWithDefault.h>
#include <bmd/libbmdasn1_core/constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* PKCS15urlWithDigest */
typedef struct PKCS15urlWithDigest {
	IA5String_t	 url;
	PKCS15DigestInfoWithDefault_t	 digest;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} PKCS15urlWithDigest_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_PKCS15urlWithDigest;

#ifdef __cplusplus
}
#endif

#endif	/* _PKCS15urlWithDigest_H_ */
