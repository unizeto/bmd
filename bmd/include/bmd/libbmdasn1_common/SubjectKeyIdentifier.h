/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "CryptographicMessageSyntax"
 * 	found in "rfc3852-CryptographicMessageSyntax2004.asn1"
 */

#ifndef	_SubjectKeyIdentifier_H_
#define	_SubjectKeyIdentifier_H_


#include <bmd/libbmdasn1_core/asn_application.h>

/* Including external dependencies */
#include <bmd/libbmdasn1_core/OCTET_STRING.h>

#ifdef __cplusplus
extern "C" {
#endif

/* SubjectKeyIdentifier */
typedef OCTET_STRING_t	 SubjectKeyIdentifier_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_SubjectKeyIdentifier;
asn_struct_free_f SubjectKeyIdentifier_free;
asn_struct_print_f SubjectKeyIdentifier_print;
asn_constr_check_f SubjectKeyIdentifier_constraint;
ber_type_decoder_f SubjectKeyIdentifier_decode_ber;
der_type_encoder_f SubjectKeyIdentifier_encode_der;
xer_type_decoder_f SubjectKeyIdentifier_decode_xer;
xer_type_encoder_f SubjectKeyIdentifier_encode_xer;

#ifdef __cplusplus
}
#endif

#endif	/* _SubjectKeyIdentifier_H_ */