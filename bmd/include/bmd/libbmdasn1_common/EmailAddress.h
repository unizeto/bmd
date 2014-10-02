/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Explicit88"
 * 	found in "rfc3280-PKIX1Explicit88.asn1"
 */

#ifndef	_EmailAddress_H_
#define	_EmailAddress_H_


#include <bmd/libbmdasn1_core/asn_application.h>

/* Including external dependencies */
#include <bmd/libbmdasn1_core/IA5String.h>

#ifdef __cplusplus
extern "C" {
#endif

/* EmailAddress */
typedef IA5String_t	 EmailAddress_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_EmailAddress;
asn_struct_free_f EmailAddress_free;
asn_struct_print_f EmailAddress_print;
asn_constr_check_f EmailAddress_constraint;
ber_type_decoder_f EmailAddress_decode_ber;
der_type_encoder_f EmailAddress_encode_der;
xer_type_decoder_f EmailAddress_decode_xer;
xer_type_encoder_f EmailAddress_encode_xer;

#ifdef __cplusplus
}
#endif

#endif	/* _EmailAddress_H_ */