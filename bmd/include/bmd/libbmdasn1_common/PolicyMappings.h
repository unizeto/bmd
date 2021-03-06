/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Implicit88"
 * 	found in "rfc3280-PKIX1Implicit88.asn1"
 */

#ifndef	_PolicyMappings_H_
#define	_PolicyMappings_H_


#include <bmd/libbmdasn1_core/asn_application.h>

/* Including external dependencies */
#include <bmd/libbmdasn1_core/asn_SEQUENCE_OF.h>
#include <bmd/libbmdasn1_common/CertPolicyId.h>
#include <bmd/libbmdasn1_core/constr_SEQUENCE.h>
#include <bmd/libbmdasn1_core/constr_SEQUENCE_OF.h>

#ifdef __cplusplus
extern "C" {
#endif

struct PolicyMappings__Member {
	CertPolicyId_t	 issuerDomainPolicy;
	CertPolicyId_t	 subjectDomainPolicy;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
};

/* PolicyMappings */
typedef struct PolicyMappings {
	A_SEQUENCE_OF(struct PolicyMappings__Member) list;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} PolicyMappings_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_PolicyMappings;

#ifdef __cplusplus
}
#endif

#endif	/* _PolicyMappings_H_ */
