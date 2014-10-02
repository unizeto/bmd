/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "PKCS-15"
 * 	found in "pkcs15_ms.asn1"
 */

#ifndef	_PKCS15Usage_H_
#define	_PKCS15Usage_H_


#include <bmd/libbmdasn1_core/asn_application.h>

/* Including external dependencies */
#include <bmd/libbmdasn1_common/KeyUsage.h>
#include <bmd/libbmdasn1_core/OBJECT_IDENTIFIER.h>
#include <bmd/libbmdasn1_core/asn_SEQUENCE_OF.h>
#include <bmd/libbmdasn1_core/constr_SEQUENCE_OF.h>
#include <bmd/libbmdasn1_core/constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* PKCS15Usage */
typedef struct PKCS15Usage {
	KeyUsage_t	*keyUsage	/* OPTIONAL */;
	struct PKCS15Usage__extKeyUsage {
		A_SEQUENCE_OF(OBJECT_IDENTIFIER_t) list;
		
		/* Context for parsing across buffer boundaries */
		asn_struct_ctx_t _asn_ctx;
	} *extKeyUsage;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} PKCS15Usage_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_PKCS15Usage;

#ifdef __cplusplus
}
#endif

#endif	/* _PKCS15Usage_H_ */