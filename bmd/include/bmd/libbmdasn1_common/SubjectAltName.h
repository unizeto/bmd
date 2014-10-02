/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Implicit88"
 * 	found in "rfc3280-PKIX1Implicit88.asn1"
 */

#ifndef	_SubjectAltName_H_
#define	_SubjectAltName_H_


#include <bmd/libbmdasn1_core/asn_application.h>

/* Including external dependencies */
#include <bmd/libbmdasn1_common/GeneralNames.h>

#ifdef __cplusplus
extern "C" {
#endif

/* SubjectAltName */
typedef GeneralNames_t	 SubjectAltName_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_SubjectAltName;
asn_struct_free_f SubjectAltName_free;
asn_struct_print_f SubjectAltName_print;
asn_constr_check_f SubjectAltName_constraint;
ber_type_decoder_f SubjectAltName_decode_ber;
der_type_encoder_f SubjectAltName_encode_der;
xer_type_decoder_f SubjectAltName_decode_xer;
xer_type_encoder_f SubjectAltName_encode_xer;

#ifdef __cplusplus
}
#endif

#endif	/* _SubjectAltName_H_ */
