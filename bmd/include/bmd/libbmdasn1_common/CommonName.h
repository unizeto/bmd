/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Explicit88"
 * 	found in "rfc3280-PKIX1Explicit88.asn1"
 */

#ifndef	_CommonName_H_
#define	_CommonName_H_


#include <bmd/libbmdasn1_core/asn_application.h>

/* Including external dependencies */
#include <bmd/libbmdasn1_core/PrintableString.h>

#ifdef __cplusplus
extern "C" {
#endif

/* CommonName */
typedef PrintableString_t	 CommonName_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_CommonName;
asn_struct_free_f CommonName_free;
asn_struct_print_f CommonName_print;
asn_constr_check_f CommonName_constraint;
ber_type_decoder_f CommonName_decode_ber;
der_type_encoder_f CommonName_encode_der;
xer_type_decoder_f CommonName_decode_xer;
xer_type_encoder_f CommonName_encode_xer;

#ifdef __cplusplus
}
#endif

#endif	/* _CommonName_H_ */
