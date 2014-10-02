/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "CryptographicMessageSyntax"
 * 	found in "rfc3852-CryptographicMessageSyntax2004.asn1"
 */

#ifndef	_MessageAuthenticationCode_H_
#define	_MessageAuthenticationCode_H_


#include <bmd/libbmdasn1_core/asn_application.h>

/* Including external dependencies */
#include <bmd/libbmdasn1_core/OCTET_STRING.h>

#ifdef __cplusplus
extern "C" {
#endif

/* MessageAuthenticationCode */
typedef OCTET_STRING_t	 MessageAuthenticationCode_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_MessageAuthenticationCode;
asn_struct_free_f MessageAuthenticationCode_free;
asn_struct_print_f MessageAuthenticationCode_print;
asn_constr_check_f MessageAuthenticationCode_constraint;
ber_type_decoder_f MessageAuthenticationCode_decode_ber;
der_type_encoder_f MessageAuthenticationCode_encode_der;
xer_type_decoder_f MessageAuthenticationCode_decode_xer;
xer_type_encoder_f MessageAuthenticationCode_encode_xer;

#ifdef __cplusplus
}
#endif

#endif	/* _MessageAuthenticationCode_H_ */
