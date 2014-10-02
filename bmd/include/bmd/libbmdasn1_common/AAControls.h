/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIXAttributeCertificate"
 * 	found in "ac1.asn1"
 */

#ifndef	_AAControls_H_
#define	_AAControls_H_


#include <bmd/libbmdasn1_core/asn_application.h>

/* Including external dependencies */
#include <bmd/libbmdasn1_core/INTEGER.h>
#include <bmd/libbmdasn1_core/BOOLEAN.h>
#include <bmd/libbmdasn1_core/constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct AttrSpec;

/* AAControls */
typedef struct AAControls {
	INTEGER_t	*pathLenConstraint	/* OPTIONAL */;
	struct AttrSpec	*permittedAttrs	/* OPTIONAL */;
	struct AttrSpec	*excludedAttrs	/* OPTIONAL */;
	BOOLEAN_t	*permitUnSpecified	/* DEFAULT TRUE */;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} AAControls_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_AAControls;

#ifdef __cplusplus
}
#endif

/* Referred external types */
#include <bmd/libbmdasn1_common/AttrSpec.h>

#endif	/* _AAControls_H_ */
