/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "ETS-ElectronicSignaturePolicies-88syntax"
 * 	found in "rfc3125.asn1"
 */

#ifndef	_SignPolExtensions_H_
#define	_SignPolExtensions_H_


#include <bmd/libbmdasn1_core/asn_application.h>

/* Including external dependencies */
#include <bmd/libbmdasn1_core/asn_SEQUENCE_OF.h>
#include <bmd/libbmdasn1_core/constr_SEQUENCE_OF.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct SignPolExtn;

/* SignPolExtensions */
typedef struct SignPolExtensions {
	A_SEQUENCE_OF(struct SignPolExtn) list;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} SignPolExtensions_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_SignPolExtensions;

#ifdef __cplusplus
}
#endif

/* Referred external types */
#include <bmd/libbmdasn1_common/SigPolicy/SignPolExtn.h>

#endif	/* _SignPolExtensions_H_ */