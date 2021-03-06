/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "PKCS-15"
 * 	found in "pkcs15_ms.asn1"
 */

#ifndef	_PKCS15URL_H_
#define	_PKCS15URL_H_


#include <bmd/libbmdasn1_core/asn_application.h>

/* Including external dependencies */
#include <bmd/libbmdasn1_core/PrintableString.h>
#include <bmd/libbmdasn1_common/pkcs15/PKCS15urlWithDigest.h>
#include <bmd/libbmdasn1_core/constr_CHOICE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum PKCS15URL_PR {
	PKCS15URL_PR_NOTHING,	/* No components present */
	PKCS15URL_PR_url,
	PKCS15URL_PR_urlWithDigest,
} PKCS15URL_PR;

/* PKCS15URL */
typedef struct PKCS15URL {
	PKCS15URL_PR present;
	union PKCS15URL_u {
		PrintableString_t	 url;
		PKCS15urlWithDigest_t	 urlWithDigest;
	} choice;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} PKCS15URL_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_PKCS15URL;

#ifdef __cplusplus
}
#endif

#endif	/* _PKCS15URL_H_ */
