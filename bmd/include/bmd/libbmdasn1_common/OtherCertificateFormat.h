/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "CryptographicMessageSyntax"
 * 	found in "rfc3852-CryptographicMessageSyntax2004.asn1"
 */

#ifndef	_OtherCertificateFormat_H_
#define	_OtherCertificateFormat_H_


#include <bmd/libbmdasn1_core/asn_application.h>

/* Including external dependencies */
#include <bmd/libbmdasn1_core/OBJECT_IDENTIFIER.h>
#include <bmd/libbmdasn1_core/ANY.h>
#include <bmd/libbmdasn1_core/constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* OtherCertificateFormat */
typedef struct OtherCertificateFormat {
	OBJECT_IDENTIFIER_t	 otherCertFormat;
	ANY_t	 otherCert;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} OtherCertificateFormat_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_OtherCertificateFormat;

#ifdef __cplusplus
}
#endif

#endif	/* _OtherCertificateFormat_H_ */
