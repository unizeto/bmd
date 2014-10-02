/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "ETS-ElectronicSignaturePolicies-88syntax"
 * 	found in "rfc3125.asn1"
 */

#ifndef	_CommonRules_H_
#define	_CommonRules_H_


#include <bmd/libbmdasn1_core/asn_application.h>

/* Including external dependencies */
#include <bmd/libbmdasn1_core/constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct SignerAndVerifierRules;
struct SigningCertTrustCondition;
struct TimestampTrustCondition;
struct AttributeTrustCondition;
struct AlgorithmConstraintSet;
struct SignPolExtensions;

/* CommonRules */
typedef struct CommonRules {
	struct SignerAndVerifierRules	*signerAndVeriferRules	/* OPTIONAL */;
	struct SigningCertTrustCondition	*signingCertTrustCondition	/* OPTIONAL */;
	struct TimestampTrustCondition	*timeStampTrustCondition	/* OPTIONAL */;
	struct AttributeTrustCondition	*attributeTrustCondition	/* OPTIONAL */;
	struct AlgorithmConstraintSet	*algorithmConstraintSet	/* OPTIONAL */;
	struct SignPolExtensions	*signPolExtensions	/* OPTIONAL */;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} CommonRules_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_CommonRules;

#ifdef __cplusplus
}
#endif

/* Referred external types */
#include <bmd/libbmdasn1_common/SigPolicy/SignerAndVerifierRules.h>
#include <bmd/libbmdasn1_common/SigPolicy/SigningCertTrustCondition.h>
#include <bmd/libbmdasn1_common/SigPolicy/TimestampTrustCondition.h>
#include <bmd/libbmdasn1_common/SigPolicy/AttributeTrustCondition.h>
#include <bmd/libbmdasn1_common/SigPolicy/AlgorithmConstraintSet.h>
#include <bmd/libbmdasn1_common/SigPolicy/SignPolExtensions.h>

#endif	/* _CommonRules_H_ */
