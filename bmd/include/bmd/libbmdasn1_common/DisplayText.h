/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Implicit88"
 * 	found in "rfc3280-PKIX1Implicit88.asn1"
 */

#ifndef	_DisplayText_H_
#define	_DisplayText_H_


#include <bmd/libbmdasn1_core/asn_application.h>

/* Including external dependencies */
#include <bmd/libbmdasn1_core/IA5String.h>
#include <bmd/libbmdasn1_core/VisibleString.h>
#include <bmd/libbmdasn1_core/BMPString.h>
#include <bmd/libbmdasn1_core/UTF8String.h>
#include <bmd/libbmdasn1_core/constr_CHOICE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum DisplayText_PR {
	DisplayText_PR_NOTHING,	/* No components present */
	DisplayText_PR_ia5String,
	DisplayText_PR_visibleString,
	DisplayText_PR_bmpString,
	DisplayText_PR_utf8String,
} DisplayText_PR;

/* DisplayText */
typedef struct DisplayText {
	DisplayText_PR present;
	union DisplayText_u {
		IA5String_t	 ia5String;
		VisibleString_t	 visibleString;
		BMPString_t	 bmpString;
		UTF8String_t	 utf8String;
	} choice;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} DisplayText_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_DisplayText;

#ifdef __cplusplus
}
#endif

#endif	/* _DisplayText_H_ */
