/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Implicit88"
 * 	found in "rfc3280-PKIX1Implicit88.asn1"
 */

#ifndef	_CRLDistributionPoints_H_
#define	_CRLDistributionPoints_H_


#include <bmd/libbmdasn1_core/asn_application.h>

/* Including external dependencies */
#include <bmd/libbmdasn1_core/asn_SEQUENCE_OF.h>
#include <bmd/libbmdasn1_core/constr_SEQUENCE_OF.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct DistributionPoint;

/* CRLDistributionPoints */
typedef struct CRLDistributionPoints {
	A_SEQUENCE_OF(struct DistributionPoint) list;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} CRLDistributionPoints_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_CRLDistributionPoints;

#ifdef __cplusplus
}
#endif

/* Referred external types */
#include <bmd/libbmdasn1_common/DistributionPoint.h>

#endif	/* _CRLDistributionPoints_H_ */
