/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Explicit88"
 * 	found in "rfc3280-PKIX1Explicit88.asn1"
 */

#ifndef	_PrivateDomainName_H_
#define	_PrivateDomainName_H_


#include <bmd/libbmdasn1_core/asn_application.h>

/* Including external dependencies */
#include <bmd/libbmdasn1_core/NumericString.h>
#include <bmd/libbmdasn1_core/PrintableString.h>
#include <bmd/libbmdasn1_core/constr_CHOICE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum PrivateDomainName_PR {
	PrivateDomainName_PR_NOTHING,	/* No components present */
	PrivateDomainName_PR_numeric,
	PrivateDomainName_PR_printable,
} PrivateDomainName_PR;

/* PrivateDomainName */
typedef struct PrivateDomainName {
	PrivateDomainName_PR present;
	union PrivateDomainName_u {
		NumericString_t	 numeric;
		PrintableString_t	 printable;
	} choice;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} PrivateDomainName_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_PrivateDomainName;

#ifdef __cplusplus
}
#endif

#endif	/* _PrivateDomainName_H_ */