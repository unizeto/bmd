/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "OCSP"
 * 	found in "ocsp.asn1"
 */

#ifndef	_CertID_H_
#define	_CertID_H_


#include <bmd/libbmdasn1_core/asn_application.h>

/* Including external dependencies */
#include <bmd/libbmdasn1_common/AlgorithmIdentifier.h>
#include <bmd/libbmdasn1_core/OCTET_STRING.h>
#include <bmd/libbmdasn1_common/CertificateSerialNumber.h>
#include <bmd/libbmdasn1_core/constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* CertID */
typedef struct CertID {
	AlgorithmIdentifier_t	 hashAlgorithm;
	OCTET_STRING_t	 issuerNameHash;
	OCTET_STRING_t	 issuerKeyHash;
	CertificateSerialNumber_t	 serialNumber;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} CertID_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_CertID;

#ifdef __cplusplus
}
#endif

#endif	/* _CertID_H_ */
