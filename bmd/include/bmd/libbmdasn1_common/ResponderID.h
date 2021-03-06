/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "OCSP"
 * 	found in "ocsp.asn1"
 */

#ifndef	_ResponderID_H_
#define	_ResponderID_H_


#include <bmd/libbmdasn1_core/asn_application.h>

/* Including external dependencies */
#include <bmd/libbmdasn1_common/Name.h>
#include <bmd/libbmdasn1_common/KeyHash.h>
#include <bmd/libbmdasn1_core/constr_CHOICE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum ResponderID_PR {
	ResponderID_PR_NOTHING,	/* No components present */
	ResponderID_PR_byName,
	ResponderID_PR_byKey,
} ResponderID_PR;

/* ResponderID */
typedef struct ResponderID {
	ResponderID_PR present;
	union ResponderID_u {
		Name_t	 byName;
		KeyHash_t	 byKey;
	} choice;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} ResponderID_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_ResponderID;

#ifdef __cplusplus
}
#endif

#endif	/* _ResponderID_H_ */
