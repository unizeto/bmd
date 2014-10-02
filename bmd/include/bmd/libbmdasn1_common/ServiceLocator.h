/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "OCSP"
 * 	found in "ocsp.asn1"
 */

#ifndef	_ServiceLocator_H_
#define	_ServiceLocator_H_


#include <bmd/libbmdasn1_core/asn_application.h>

/* Including external dependencies */
#include <bmd/libbmdasn1_common/Name.h>
#include <bmd/libbmdasn1_common/AuthorityInfoAccessSyntax.h>
#include <bmd/libbmdasn1_core/constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ServiceLocator */
typedef struct ServiceLocator {
	Name_t	 issuer;
	AuthorityInfoAccessSyntax_t	 locator;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} ServiceLocator_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_ServiceLocator;

#ifdef __cplusplus
}
#endif

#endif	/* _ServiceLocator_H_ */