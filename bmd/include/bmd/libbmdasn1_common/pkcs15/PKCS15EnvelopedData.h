/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "PKCS-15"
 * 	found in "pkcs15_ms.asn1"
 */

#ifndef	_PKCS15EnvelopedData_H_
#define	_PKCS15EnvelopedData_H_


#include <bmd/libbmdasn1_core/asn_application.h>

/* Including external dependencies */
#include <bmd/libbmdasn1_core/INTEGER.h>
#include <bmd/libbmdasn1_common/RecipientInfos.h>
#include <bmd/libbmdasn1_common/pkcs15/PKCS15EncryptedContentInfo.h>
#include <bmd/libbmdasn1_core/asn_SET_OF.h>
#include <bmd/libbmdasn1_core/constr_SET_OF.h>
#include <bmd/libbmdasn1_core/constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum PKCS15EnvelopedData__version {
	PKCS15EnvelopedData__version_v0	= 0,
	PKCS15EnvelopedData__version_v1	= 1,
	PKCS15EnvelopedData__version_v2	= 2,
	PKCS15EnvelopedData__version_v3	= 3,
	PKCS15EnvelopedData__version_v4	= 4
} PKCS15EnvelopedData__version_e;

/* Forward declarations */
struct OriginatorInfo;
struct Attribute;

/* PKCS15EnvelopedData */
typedef struct PKCS15EnvelopedData {
	INTEGER_t	 version;
	struct OriginatorInfo	*originatorInfo	/* OPTIONAL */;
	RecipientInfos_t	 recipientInfos;
	PKCS15EncryptedContentInfo_t	 encryptedContentInfo;
	struct PKCS15EnvelopedData__unprotectedAttrs {
		A_SET_OF(struct Attribute) list;
		
		/* Context for parsing across buffer boundaries */
		asn_struct_ctx_t _asn_ctx;
	} *unprotectedAttrs;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} PKCS15EnvelopedData_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_PKCS15EnvelopedData;

#ifdef __cplusplus
}
#endif

/* Referred external types */
#include <bmd/libbmdasn1_common/OriginatorInfo.h>
#include <bmd/libbmdasn1_common/Attribute.h>

#endif	/* _PKCS15EnvelopedData_H_ */
