/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "CryptographicMessageSyntax"
 * 	found in "rfc3852-CryptographicMessageSyntax2004.asn1"
 */

#ifndef	_UserKeyingMaterial_H_
#define	_UserKeyingMaterial_H_


#include <bmd/libbmdasn1_core/asn_application.h>

/* Including external dependencies */
#include <bmd/libbmdasn1_core/OCTET_STRING.h>

#ifdef __cplusplus
extern "C" {
#endif

/* UserKeyingMaterial */
typedef OCTET_STRING_t	 UserKeyingMaterial_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_UserKeyingMaterial;
asn_struct_free_f UserKeyingMaterial_free;
asn_struct_print_f UserKeyingMaterial_print;
asn_constr_check_f UserKeyingMaterial_constraint;
ber_type_decoder_f UserKeyingMaterial_decode_ber;
der_type_encoder_f UserKeyingMaterial_encode_der;
xer_type_decoder_f UserKeyingMaterial_decode_xer;
xer_type_encoder_f UserKeyingMaterial_encode_xer;

#ifdef __cplusplus
}
#endif

#endif	/* _UserKeyingMaterial_H_ */
