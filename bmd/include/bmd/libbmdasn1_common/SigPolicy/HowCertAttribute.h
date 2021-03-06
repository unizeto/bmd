/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "ETS-ElectronicSignaturePolicies-88syntax"
 * 	found in "rfc3125.asn1"
 */

#ifndef	_HowCertAttribute_H_
#define	_HowCertAttribute_H_


#include <bmd/libbmdasn1_core/asn_application.h>

/* Including external dependencies */
#include <bmd/libbmdasn1_core/ENUMERATED.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum HowCertAttribute {
	HowCertAttribute_claimedAttribute	= 0,
	HowCertAttribute_certifiedAttribtes	= 1,
	HowCertAttribute_either	= 2
} HowCertAttribute_e;

/* HowCertAttribute */
typedef ENUMERATED_t	 HowCertAttribute_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_HowCertAttribute;
asn_struct_free_f HowCertAttribute_free;
asn_struct_print_f HowCertAttribute_print;
asn_constr_check_f HowCertAttribute_constraint;
ber_type_decoder_f HowCertAttribute_decode_ber;
der_type_encoder_f HowCertAttribute_encode_der;
xer_type_decoder_f HowCertAttribute_decode_xer;
xer_type_encoder_f HowCertAttribute_encode_xer;

#ifdef __cplusplus
}
#endif

#endif	/* _HowCertAttribute_H_ */
