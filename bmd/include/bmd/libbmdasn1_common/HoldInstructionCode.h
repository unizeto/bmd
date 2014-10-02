/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "PKIX1Implicit88"
 * 	found in "rfc3280-PKIX1Implicit88.asn1"
 */

#ifndef	_HoldInstructionCode_H_
#define	_HoldInstructionCode_H_


#include <bmd/libbmdasn1_core/asn_application.h>

/* Including external dependencies */
#include <bmd/libbmdasn1_core/OBJECT_IDENTIFIER.h>

#ifdef __cplusplus
extern "C" {
#endif

/* HoldInstructionCode */
typedef OBJECT_IDENTIFIER_t	 HoldInstructionCode_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_HoldInstructionCode;
asn_struct_free_f HoldInstructionCode_free;
asn_struct_print_f HoldInstructionCode_print;
asn_constr_check_f HoldInstructionCode_constraint;
ber_type_decoder_f HoldInstructionCode_decode_ber;
der_type_encoder_f HoldInstructionCode_encode_der;
xer_type_decoder_f HoldInstructionCode_decode_xer;
xer_type_encoder_f HoldInstructionCode_encode_xer;

#ifdef __cplusplus
}
#endif

#endif	/* _HoldInstructionCode_H_ */
