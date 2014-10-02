/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "CryptographicMessageSyntax"
 * 	found in "rfc3852-CryptographicMessageSyntax2004.asn1"
 */

#ifndef	_ExtendedCertificateInfo_H_
#define	_ExtendedCertificateInfo_H_


#include <bmd/libbmdasn1_core/asn_application.h>

/* Including external dependencies */
#include <bmd/libbmdasn1_common/CMSVersion.h>
#include <bmd/libbmdasn1_common/Certificate.h>
#include <bmd/libbmdasn1_common/UnauthAttributes.h>
#include <bmd/libbmdasn1_core/constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ExtendedCertificateInfo */
typedef struct ExtendedCertificateInfo {
	CMSVersion_t	 version;
	Certificate_t	 certificate;
	UnauthAttributes_t	 attributes;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} ExtendedCertificateInfo_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_ExtendedCertificateInfo;

#ifdef __cplusplus
}
#endif

#endif	/* _ExtendedCertificateInfo_H_ */
