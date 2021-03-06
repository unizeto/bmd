/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIXAttributeCertificate"
 * 	found in "ac1.asn1"
 */

#ifndef	_Target_H_
#define	_Target_H_


#include <bmd/libbmdasn1_core/asn_application.h>

/* Including external dependencies */
#include <bmd/libbmdasn1_common/GeneralName.h>
#include <bmd/libbmdasn1_common/TargetCert.h>
#include <bmd/libbmdasn1_core/constr_CHOICE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum Target_PR {
	Target_PR_NOTHING,	/* No components present */
	Target_PR_targetName,
	Target_PR_targetGroup,
	Target_PR_targetCert,
} Target_PR;

/* Target */
typedef struct Target {
	Target_PR present;
	union Target_u {
		GeneralName_t	 targetName;
		GeneralName_t	 targetGroup;
		TargetCert_t	 targetCert;
	} choice;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} Target_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_Target;

#ifdef __cplusplus
}
#endif

#endif	/* _Target_H_ */
