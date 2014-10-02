/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "PKCS-15"
 * 	found in "pkcs15_ms.asn1"
 */

#ifndef	_PKCS15CertificateType_H_
#define	_PKCS15CertificateType_H_


#include <bmd/libbmdasn1_core/asn_application.h>

/* Including external dependencies */
#include <bmd/libbmdasn1_common/pkcs15/PKCS15CertificateObject.h>
#include <bmd/libbmdasn1_core/constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* PKCS15CertificateType */
typedef struct PKCS15CertificateType {
	PKCS15CertificateObject_t	 x509Certificate;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} PKCS15CertificateType_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_PKCS15CertificateType;

#ifdef __cplusplus
}
#endif

#endif	/* _PKCS15CertificateType_H_ */