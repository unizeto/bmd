/*
 * Generated by asn1c-0.9.20 (http://lionet.info/asn1c)
 * From ASN.1 module "SigPolicyID"
 * 	found in "ws.asn1"
 */

#ifndef	_SigPolicyQualifierId_H_
#define	_SigPolicyQualifierId_H_


#include <bmd/libbmdasn1_core/asn_application.h>

/* Including external dependencies */
#include <bmd/libbmdasn1_core/OBJECT_IDENTIFIER.h>

#ifdef __cplusplus
extern "C" {
#endif

/* SigPolicyQualifierId */
typedef OBJECT_IDENTIFIER_t	 SigPolicyQualifierId_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_SigPolicyQualifierId;
asn_struct_free_f SigPolicyQualifierId_free;
asn_struct_print_f SigPolicyQualifierId_print;
asn_constr_check_f SigPolicyQualifierId_constraint;
ber_type_decoder_f SigPolicyQualifierId_decode_ber;
der_type_encoder_f SigPolicyQualifierId_encode_der;
xer_type_decoder_f SigPolicyQualifierId_decode_xer;
xer_type_encoder_f SigPolicyQualifierId_encode_xer;

#ifdef __cplusplus
}
#endif

#endif	/* _SigPolicyQualifierId_H_ */