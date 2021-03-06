/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIXAttributeCertificate"
 * 	found in "ac1.asn1"
 */

#ifndef	_RoleSyntax_H_
#define	_RoleSyntax_H_


#include <bmd/libbmdasn1_core/asn_application.h>

/* Including external dependencies */
#include <bmd/libbmdasn1_common/GeneralName.h>
#include <bmd/libbmdasn1_core/constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct GeneralNames;

/* RoleSyntax */
typedef struct RoleSyntax {
	struct GeneralNames	*roleAuthority	/* OPTIONAL */;
	GeneralName_t	 roleName;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} RoleSyntax_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_RoleSyntax;

#ifdef __cplusplus
}
#endif

/* Referred external types */
#include <bmd/libbmdasn1_common/GeneralNames.h>

#endif	/* _RoleSyntax_H_ */
