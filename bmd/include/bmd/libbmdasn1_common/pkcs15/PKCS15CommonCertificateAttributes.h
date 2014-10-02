/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "PKCS-15"
 * 	found in "pkcs15_ms.asn1"
 */

#ifndef	_PKCS15CommonCertificateAttributes_H_
#define	_PKCS15CommonCertificateAttributes_H_


#include <bmd/libbmdasn1_core/asn_application.h>

/* Including external dependencies */
#include <bmd/libbmdasn1_common/pkcs15/PKCS15Identifier.h>
#include <bmd/libbmdasn1_core/BOOLEAN.h>
#include <bmd/libbmdasn1_core/GeneralizedTime.h>
#include <bmd/libbmdasn1_core/asn_SEQUENCE_OF.h>
#include <bmd/libbmdasn1_core/constr_SEQUENCE_OF.h>
#include <bmd/libbmdasn1_core/constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct PKCS15CredentialIdentifier;
struct OOBCertHash;
struct PKCS15Usage;

/* PKCS15CommonCertificateAttributes */
typedef struct PKCS15CommonCertificateAttributes {
	PKCS15Identifier_t	 iD;
	BOOLEAN_t	*authority	/* DEFAULT FALSE */;
	struct PKCS15CredentialIdentifier	*identifier	/* OPTIONAL */;
	struct OOBCertHash	*certHash	/* OPTIONAL */;
	struct PKCS15Usage	*trustedUsage	/* OPTIONAL */;
	struct PKCS15CommonCertificateAttributes__identifiers {
		A_SEQUENCE_OF(struct PKCS15CredentialIdentifier) list;
		
		/* Context for parsing across buffer boundaries */
		asn_struct_ctx_t _asn_ctx;
	} *identifiers;
	BOOLEAN_t	*implicitTrust	/* DEFAULT FALSE */;
	GeneralizedTime_t	*startDate	/* OPTIONAL */;
	GeneralizedTime_t	*endDate	/* OPTIONAL */;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} PKCS15CommonCertificateAttributes_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_PKCS15CommonCertificateAttributes;

#ifdef __cplusplus
}
#endif

/* Referred external types */
#include <bmd/libbmdasn1_common/pkcs15/PKCS15CredentialIdentifier.h>
#include <bmd/libbmdasn1_common/pkcs15/OOBCertHash.h>
#include <bmd/libbmdasn1_common/pkcs15/PKCS15Usage.h>

#endif	/* _PKCS15CommonCertificateAttributes_H_ */