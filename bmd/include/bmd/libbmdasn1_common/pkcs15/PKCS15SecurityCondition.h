/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "PKCS-15"
 * 	found in "pkcs15_ms.asn1"
 */

#ifndef	_PKCS15SecurityCondition_H_
#define	_PKCS15SecurityCondition_H_


#include <bmd/libbmdasn1_core/asn_application.h>

/* Including external dependencies */
#include <bmd/libbmdasn1_common/pkcs15/PKCS15Identifier.h>
#include <bmd/libbmdasn1_core/asn_SEQUENCE_OF.h>
#include <bmd/libbmdasn1_core/constr_SEQUENCE_OF.h>
#include <bmd/libbmdasn1_core/constr_CHOICE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum PKCS15SecurityCondition_PR {
	PKCS15SecurityCondition_PR_NOTHING,	/* No components present */
	PKCS15SecurityCondition_PR_authId,
	PKCS15SecurityCondition_PR_not,
	PKCS15SecurityCondition_PR_and,
	PKCS15SecurityCondition_PR_or,
} PKCS15SecurityCondition_PR;

/* Forward declarations */
struct PKCS15SecurityCondition;

/* PKCS15SecurityCondition */
typedef struct PKCS15SecurityCondition {
	PKCS15SecurityCondition_PR present;
	union PKCS15SecurityCondition_u {
		PKCS15Identifier_t	 authId;
		struct PKCS15SecurityCondition	*not;
		struct PKCS15SecurityCondition__and {
			A_SEQUENCE_OF(struct PKCS15SecurityCondition) list;
			
			/* Context for parsing across buffer boundaries */
			asn_struct_ctx_t _asn_ctx;
		} and;
		struct PKCS15SecurityCondition__or {
			A_SEQUENCE_OF(struct PKCS15SecurityCondition) list;
			
			/* Context for parsing across buffer boundaries */
			asn_struct_ctx_t _asn_ctx;
		} or;
	} choice;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} PKCS15SecurityCondition_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_PKCS15SecurityCondition;

#ifdef __cplusplus
}
#endif

/* Referred external types */
#include <bmd/libbmdasn1_common/pkcs15/PKCS15SecurityCondition.h>

#endif	/* _PKCS15SecurityCondition_H_ */
