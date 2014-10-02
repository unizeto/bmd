/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Explicit88"
 * 	found in "rfc3280-PKIX1Explicit88.asn1"
 */

#ifndef	_OrganizationalUnitNames_H_
#define	_OrganizationalUnitNames_H_


#include <bmd/libbmdasn1_core/asn_application.h>

/* Including external dependencies */
#include <bmd/libbmdasn1_common/OrganizationalUnitName.h>
#include <bmd/libbmdasn1_core/asn_SEQUENCE_OF.h>
#include <bmd/libbmdasn1_core/constr_SEQUENCE_OF.h>

#ifdef __cplusplus
extern "C" {
#endif

/* OrganizationalUnitNames */
typedef struct OrganizationalUnitNames {
	A_SEQUENCE_OF(OrganizationalUnitName_t) list;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} OrganizationalUnitNames_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_OrganizationalUnitNames;

#ifdef __cplusplus
}
#endif

#endif	/* _OrganizationalUnitNames_H_ */
