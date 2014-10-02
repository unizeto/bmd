/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Explicit88"
 * 	found in "rfc3280-PKIX1Explicit88.asn1"
 */

#ifndef	_X520Pseudonym_H_
#define	_X520Pseudonym_H_


#include <bmd/libbmdasn1_core/asn_application.h>

/* Including external dependencies */
#include <bmd/libbmdasn1_core/TeletexString.h>
#include <bmd/libbmdasn1_core/PrintableString.h>
#include <bmd/libbmdasn1_core/UniversalString.h>
#include <bmd/libbmdasn1_core/UTF8String.h>
#include <bmd/libbmdasn1_core/BMPString.h>
#include <bmd/libbmdasn1_core/constr_CHOICE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum X520Pseudonym_PR {
	X520Pseudonym_PR_NOTHING,	/* No components present */
	X520Pseudonym_PR_teletexString,
	X520Pseudonym_PR_printableString,
	X520Pseudonym_PR_universalString,
	X520Pseudonym_PR_utf8String,
	X520Pseudonym_PR_bmpString,
} X520Pseudonym_PR;

/* X520Pseudonym */
typedef struct X520Pseudonym {
	X520Pseudonym_PR present;
	union X520Pseudonym_u {
		TeletexString_t	 teletexString;
		PrintableString_t	 printableString;
		UniversalString_t	 universalString;
		UTF8String_t	 utf8String;
		BMPString_t	 bmpString;
	} choice;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} X520Pseudonym_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_X520Pseudonym;

#ifdef __cplusplus
}
#endif

#endif	/* _X520Pseudonym_H_ */
