/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "CryptographicMessageSyntax"
 * 	found in "rfc3852-CryptographicMessageSyntax2004.asn1"
 */

#ifndef	_CertificateChoices_H_
#define	_CertificateChoices_H_


#include <bmd/libbmdasn1_core/asn_application.h>

/* Including external dependencies */
#include <bmd/libbmdasn1_common/Certificate.h>
#include <bmd/libbmdasn1_core/constr_CHOICE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum CertificateChoices_PR {
	CertificateChoices_PR_NOTHING,	/* No components present */
	CertificateChoices_PR_certificate,
} CertificateChoices_PR;

/* CertificateChoices */
typedef struct CertificateChoices {
	CertificateChoices_PR present;
	union CertificateChoices_u {
		Certificate_t	 certificate;
	} choice;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} CertificateChoices_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_CertificateChoices;

#ifdef __cplusplus
}
#endif

#endif	/* _CertificateChoices_H_ */