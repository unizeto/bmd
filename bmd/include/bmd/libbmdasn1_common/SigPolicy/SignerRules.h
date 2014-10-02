/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "ETS-ElectronicSignaturePolicies-88syntax"
 * 	found in "rfc3125.asn1"
 */

#ifndef	_SignerRules_H_
#define	_SignerRules_H_


#include <bmd/libbmdasn1_core/asn_application.h>

/* Including external dependencies */
#include <bmd/libbmdasn1_core/BOOLEAN.h>
#include <bmd/libbmdasn1_common/SigPolicy/CMSAttrs.h>
#include <bmd/libbmdasn1_common/SigPolicy/CertRefReq.h>
#include <bmd/libbmdasn1_common/SigPolicy/CertInfoReq.h>
#include <bmd/libbmdasn1_core/constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct SignPolExtensions;

/* SignerRules */
typedef struct SignerRules {
	BOOLEAN_t	*externalSignedData	/* OPTIONAL */;
	CMSAttrs_t	 mandatedSignedAttr;
	CMSAttrs_t	 mandatedUnsignedAttr;
	CertRefReq_t	*mandatedCertificateRef	/* DEFAULT 1 */;
	CertInfoReq_t	*mandatedCertificateInfo	/* DEFAULT 0 */;
	struct SignPolExtensions	*signPolExtensions	/* OPTIONAL */;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} SignerRules_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_SignerRules;

#ifdef __cplusplus
}
#endif

/* Referred external types */
#include <bmd/libbmdasn1_common/SigPolicy/SignPolExtensions.h>

#endif	/* _SignerRules_H_ */