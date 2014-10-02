/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIXAttributeCertificate"
 * 	found in "ac1.asn1"
 */

#ifndef	_SecurityCategory_H_
#define	_SecurityCategory_H_


#include <bmd/libbmdasn1_core/asn_application.h>

/* Including external dependencies */
#include <bmd/libbmdasn1_core/OBJECT_IDENTIFIER.h>
#include <bmd/libbmdasn1_core/ANY.h>
#include <bmd/libbmdasn1_core/constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* SecurityCategory */
typedef struct SecurityCategory {
	OBJECT_IDENTIFIER_t	 type;
	ANY_t	 value;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} SecurityCategory_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_SecurityCategory;

#ifdef __cplusplus
}
#endif

#endif	/* _SecurityCategory_H_ */
