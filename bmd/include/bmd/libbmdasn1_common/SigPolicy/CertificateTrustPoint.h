/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "ETS-ElectronicSignaturePolicies-88syntax"
 * 	found in "rfc3125.asn1"
 */

#ifndef	_CertificateTrustPoint_H_
#define	_CertificateTrustPoint_H_


#include <bmd/libbmdasn1_core/asn_application.h>

/* Including external dependencies */
#include <bmd/libbmdasn1_common/Certificate.h>
#include <bmd/libbmdasn1_common/SigPolicy/PathLenConstraint.h>
#include <bmd/libbmdasn1_core/constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct AcceptablePolicySet;
struct NameConstraints;
struct PolicyConstraints;

/* CertificateTrustPoint */
typedef struct CertificateTrustPoint {
	Certificate_t	 trustpoint;
	PathLenConstraint_t	*pathLenConstraint	/* OPTIONAL */;
	struct AcceptablePolicySet	*acceptablePolicySet	/* OPTIONAL */;
	struct NameConstraints	*nameConstraints	/* OPTIONAL */;
	struct PolicyConstraints	*policyConstraints	/* OPTIONAL */;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} CertificateTrustPoint_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_CertificateTrustPoint;

#ifdef __cplusplus
}
#endif

/* Referred external types */
#include <bmd/libbmdasn1_common/SigPolicy/AcceptablePolicySet.h>
#include <bmd/libbmdasn1_common/NameConstraints.h>
#include <bmd/libbmdasn1_common/PolicyConstraints.h>

#endif	/* _CertificateTrustPoint_H_ */